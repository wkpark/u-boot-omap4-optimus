#include <config.h>
#ifdef CONFIG_TWL6030

#include <twl6030.h>
#include <charging_ic.h>
#include <cosmo_muic.h>

#include <lge_nvdata_emmc.h>
#include "max17043.h"
#if defined (CONFIG_COSMO_REV_11)||defined(CONFIG_LGE_CX2)||defined(CONFIG_LGE_P2)
#include "gpio_i2c.h"
#endif

int lge_pcb_ver;

static inline int twl6030_i2c_write_u8(u8 chip_no, u8 val, u8 reg)
{
	return i2c_write(chip_no, reg, 1, &val, 1);
}
#ifdef CONFIG_COSMO_SU760
#define FUEL_GAUGE_VOLT_ERROR_RANGE		60
#else
#define FUEL_GAUGE_VOLT_ERROR_RANGE		43       
#endif

int reference_graph(int __x, battery_graph_prop* ref_battery_graph, int arraysize, int* error_range)
{
	int i = 1;
	int __y = 0;
	int slope, const_term;
	int delta_y, delta_x;

	D(" battery graph array size = %d", arraysize );
	D(" battery graph array size = %d", arraysize );

	while( __x < ref_battery_graph[i].x \
			&& i < (arraysize - 1) )
	{
		i++;
	}

	delta_x = ref_battery_graph[i-1].x - ref_battery_graph[i].x;
	delta_y = (ref_battery_graph[i-1].y - ref_battery_graph[i].y);

	slope = delta_y  / delta_x;

	const_term = (ref_battery_graph[i].y) - (ref_battery_graph[i].x * slope);

	__y = (__x* slope + const_term);

	*error_range = slope * (FUEL_GAUGE_VOLT_ERROR_RANGE) / (*error_range);

	if(*error_range < 0)
	{
		*error_range *= -1;
	}

	if(*error_range < 5)
	{
		*error_range = 5;
	}

	D(" ####### array_size = %d ##########", arraysize);
	D(" ##### SLOPE = %d, CONST_TERM = %d ##########", slope, const_term);
	D(" ##### CALCULATED __y = %d ##########", __y);

	if(ref_battery_graph[i-1].y > ref_battery_graph[i].y)
	{
		if(__y > ref_battery_graph[i-1].y)
		{
			__y = ref_battery_graph[i-1].y;
			D(" ##### fixing __y = %d ##########", __y);
		}
		else if(__y < ref_battery_graph[i].y)
		{
			__y = ref_battery_graph[i].y;
			D(" ##### fixing __y = %d ##########", __y);
		}
	}
	else
	{
		if(__y < ref_battery_graph[i-1].y)
		{
			__y = ref_battery_graph[i-1].y;
			D(" ##### fixing __y = %d ##########", __y);
		}
		else if(__y > ref_battery_graph[i].y)
		{
			__y = ref_battery_graph[i].y;
			D(" ##### fixing __y = %d ##########", __y);
		}
	}

	return __y;
}

static inline int twl6030_i2c_read_u8(u8 chip_no, u8 *val, u8 reg)
{
	return i2c_read(chip_no, reg, 1, val, 1);
}

static inline int clear_n_set(u8 chip_no, u8 clear, u8 set, u8 reg)
{
	int ret;
	u8 val = 0;

	ret = twl6030_i2c_read_u8(chip_no, &val, reg);
	if (ret) {		
		return ret;
	}

	val &= ~(clear);

	val |= set;

	ret = twl6030_i2c_write_u8(chip_no, val, reg);
	if (ret) {
		return ret;
	}
	return 0;
}

static int twl6030_adc_sw1_trigger(void)
{
	u8 val;
	int ret;

	ret = twl6030_i2c_read_u8(TWL6030_CHIP_ADC,  &val,	CTRL_P1);
	if (ret)
		return ret;

	val |= SP1;

	ret = twl6030_i2c_write_u8(TWL6030_CHIP_ADC, val,	CTRL_P1);
	
	if (ret)
		return ret;

	val =  BUSY;
	while (!((val & EOCP1) && (!(val & BUSY)))) {
		ret = twl6030_i2c_read_u8(TWL6030_CHIP_ADC,  &val,	CTRL_P1);
		if (ret)
			return ret;
		mdelay(10);
	}

	return 0;
}

int twl6030_get_adc_value(int channel){
	int ret, temp;
	u8 val;

	twl6030_adc_sw1_trigger();

	ret = twl6030_i2c_read_u8(TWL6030_CHIP_ADC, &val, channel*2 + GPCH0 + 1);
	
	if (ret)
		return 0;	
	temp = ((int) val) << 8;

	ret = twl6030_i2c_read_u8(TWL6030_CHIP_ADC, &val, channel*2 + GPCH0);
	
	if (ret)
		return 0;	

	temp = temp + (int)val;
	temp = temp & 0x03ff;

	return  temp;
}

static int twl6030_volt_setup(){
	u8 data;

	twl6030_i2c_read_u8(TWL6030_CHIP_PM, &data, MISC1);
	data |= VBAT_MEAS;

	twl6030_i2c_write_u8(TWL6030_CHIP_PM, data, MISC1);

	return 0;
}

static int twl6030_adc_setup(){
	int ret = 0;

	ret = clear_n_set(TWL6030_CHIP_ADC, 0, GPADCR, TOGGLE1);
	if (ret)
		return ret;	

	ret = clear_n_set(TWL6030_CHIP_ADC, 0, GPADCS, TOGGLE1);
	if (ret)
		return ret;	

	ret = twl6030_i2c_write_u8(TWL6030_CHIP_ADC, GPADC_CTRL_SCALER_EN | GPADC_CTRL_SCALER_DIV4, GPADC_CTRL);

	return 0;
}

int twl6030_set_pcb_version(void){
	int i, j;
	int ret = 0;
	int adc_vol;

	ret = twl6030_adc_setup();
	if (ret)
		return ret;

	adc_vol = twl6030_get_adc_value(3) * 1200 / 10;

#if defined (CONFIG_LGE_CX2) 
	printf("[CX2 Revision Check] adc_vol=%d\n",adc_vol);
#endif

	if(adc_vol >= 30000 && adc_vol < 40800 ){
		lge_pcb_ver = 4;
		
	}
	else{
		adc_vol = adc_vol / 10000;
		lge_pcb_ver = 0;
#ifdef CONFIG_COSMO_SU760

							if (adc_vol < 10) lge_pcb_ver=4;	
							if (adc_vol >=10)	lge_pcb_ver=6;	
							if (adc_vol >=13)	lge_pcb_ver=7;	
#else

		if (adc_vol >= 4) lge_pcb_ver++;
		if (adc_vol >= 5) lge_pcb_ver++;
		if (adc_vol >= 7) lge_pcb_ver++;
		if (adc_vol >= 8) lge_pcb_ver++;
		if (adc_vol >= 9) lge_pcb_ver++;
		if (adc_vol >= 11) lge_pcb_ver++;
		if (adc_vol >= 12) lge_pcb_ver++;
#endif
	}

#if defined (CONFIG_LGE_CX2) 
		printf("[CX2 Revision Check] lge_pcb_ver=%d\n",lge_pcb_ver);
#endif

	switch (lge_pcb_ver) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			D("PCB Version: %c(%d mV)",
					'A' + lge_pcb_ver - 1, adc_vol * 100);
			break;
		case 6:
		case 7:
			D("PCB Version: 1.%c(%d mV)",
					'0' + lge_pcb_ver - 5, adc_vol * 100);
			break;
		default:
			D("PCB Version: WRONG (%d mV)", adc_vol * 100);	
			return 1;
	}
	return lge_pcb_ver;
}

#if !defined(CONFIG_RT9524_CHARGER) && defined(CONFIG_TWL6030)

void twl6030_charging_register_init(void)
{

	twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CHARGERUSB_VICHRG_1500,
							CHARGERUSB_VICHRG);
	twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CHARGERUSB_CIN_LIMIT_NONE,
							CHARGERUSB_CINLIMIT);
	twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, MBAT_TEMP,
							CONTROLLER_INT_MASK);
	twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, MASK_MCHARGERUSB_THMREG,
							CHARGERUSB_INT_MASK);
	twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CHARGERUSB_VOREG_4P0,
							CHARGERUSB_VOREG);
	twl6030_i2c_write_u8(TWL6030_CHIP_CHARGER, CHARGERUSB_CTRL2_VITERM_100,
							CHARGERUSB_CTRL2);

}
#endif

u8 read_start_reason(void)
{
	u8 data; 

	twl6030_i2c_read_u8(TWL6030_CHIP_PM, &data, PHOENIX_START_CONDITION); 
	return data;
}

void write_start_reason(u8 data)
{
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, data, PHOENIX_START_CONDITION); 
}

u8 read_last_turnoff_status(void)
{
	u8 data; 

	twl6030_i2c_read_u8(TWL6030_CHIP_PM, &data, PHOENIX_LAST_TURNOFF_STS); 
	return data;
}

u8 read_current_hw_condition(void)
{
	u8 data; 

	twl6030_i2c_read_u8(TWL6030_CHIP_PM, &data, STS_HW_CONDITIONS); 
	return data;
}

void write_dev_on_off(u8 data)
{
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, data, PHOENIX_DEV_ON); 
	return;
}

#define MODEM_PWRON_GPIO	164

#if 0
static int twl6030battery_temp_setup(void)
{
#define TWL4030_MODULE_MADC     0x07
#define TWL_MODULE_MADC     TWL4030_MODULE_MADC
#define TWL6030_GPADC_CTRL      0x00    
#define GPADC_CTRL_SCALER_EN    (1 << 2)    
#define GPADC_CTRL_SCALER_DIV4  (1 << 3)

	int ret;
	u8 rd_reg;

	i2c_init(OMAP_I2C_STANDARD, TWL6030_CHIP_ADC);

	mdelay(1);		

	ret = twl6030_i2c_read_u8(TWL_MODULE_MADC, &rd_reg, TWL6030_GPADC_CTRL);
#if defined(TWL6030_PMIC_ENABLE_CHARGING)
	rd_reg |= GPADC_CTRL_TEMP1_EN | GPADC_CTRL_TEMP2_EN |
		GPADC_CTRL_TEMP1_EN_MONITOR | GPADC_CTRL_TEMP2_EN_MONITOR |
		GPADC_CTRL_SCALER_DIV4;
#endif
	mdelay(1);		
	
	rd_reg |= GPADC_CTRL_SCALER_EN | GPADC_CTRL_SCALER_DIV4;
	ret |= twl6030_i2c_write_u8(TWL_MODULE_MADC, rd_reg, TWL6030_GPADC_CTRL);
	mdelay(1);		

	return ret;
}
#endif

static int twl6030battery_voltage_setup(void)
{
#define TWL6030_MODULE_ID0  0x0D

#define REG_MISC1       0xE4
#define VAC_MEAS        0x04
#define VBAT_MEAS       0x02
#define BB_MEAS         0x01

#define TWL4030_MODULE_USB      0x00
#define TWL_MODULE_USB      TWL4030_MODULE_USB

#define REG_USB_VBUS_CTRL_SET   0x04
#define VBUS_MEAS       0x01
#define REG_USB_ID_CTRL_SET 0x06
#define ID_MEAS         0x01

	int ret;
	u8 rd_reg;

	i2c_init(OMAP_I2C_STANDARD, TWL6030_CHIP_PM);

	mdelay(1);		

	ret = twl6030_i2c_read_u8(TWL6030_CHIP_PM, &rd_reg, REG_MISC1);
	rd_reg = rd_reg | VAC_MEAS | VBAT_MEAS | BB_MEAS;
	ret |= twl6030_i2c_write_u8(TWL6030_CHIP_PM, rd_reg, REG_MISC1);
	mdelay(1);		

	ret = twl6030_adc_setup();
	if (ret)
		return ret;

	mdelay(1);		
#if 0
	
	i2c_init(OMAP_I2C_STANDARD, TWL6030_CHIP_USB);
	mdelay(1);		

	ret |= twl6030_i2c_read_u8(TWL_MODULE_USB, &rd_reg, REG_USB_VBUS_CTRL_SET);
	rd_reg = rd_reg | VBUS_MEAS;
	ret |= twl6030_i2c_write_u8(TWL_MODULE_USB, rd_reg, REG_USB_VBUS_CTRL_SET);
	mdelay(1);		

	ret |= twl6030_i2c_read_u8(TWL_MODULE_USB, &rd_reg, REG_USB_ID_CTRL_SET);
	rd_reg = rd_reg | ID_MEAS;
	ret |= twl6030_i2c_write_u8(TWL_MODULE_USB, rd_reg, REG_USB_ID_CTRL_SET);
	mdelay(1);		
#endif
	return ret;
}

int get_adc_volt(int channel)
{
	int vbat;
	int adc_code;

	adc_code = twl6030_get_adc_value(channel);
	vbat = ((adc_code * channel) * 100) / 128;

	return vbat;
}
 
#define TEMP_TIMES 10000

battery_graph_prop battery_temp_graph[] =
{
	{1774, -50 * TEMP_TIMES},
	{1748, -40 * TEMP_TIMES},
	{1701, -30 * TEMP_TIMES},
	{1622, -20 * TEMP_TIMES},
	{1504, -10 * TEMP_TIMES},
	{1341, 0 * TEMP_TIMES},
	{1142, 10 * TEMP_TIMES},
	{928, 20 * TEMP_TIMES},
	{723, 30 * TEMP_TIMES},
	{545, 40 * TEMP_TIMES},
	{401, 50 * TEMP_TIMES},
	{292, 60 * TEMP_TIMES},
	{212, 70 * TEMP_TIMES},
	{154, 80 * TEMP_TIMES},
	{113, 90 * TEMP_TIMES},
	{83, 100 * TEMP_TIMES},
};

int get_bat_temp()
{
	int adc_value;
	int error_range;
	int temp_C;

	int i = 0;

	for( i = 0; i < 3 ; i++)
	{
		adc_value = twl6030_get_adc_value(2);
		if(adc_value > 401 && adc_value < 1622)
		{
			break;
		}
		mdelay(5);
	}

	D("read adc_value = %d", adc_value);
	adc_value = ((adc_value << 1) * 100) / 112;

	error_range = TEMP_TIMES;
	temp_C = reference_graph((s64)adc_value, battery_temp_graph, ARRAY_SIZE(battery_temp_graph), &error_range) / (TEMP_TIMES / 10);
	D("Temperature adc_value = %d, calculated_temp_C = %d", adc_value, temp_C);

	D("BAT CALCULATED TEMP adc_value = %d", temp_C);
#if defined (CONFIG_COSMO_REV_11) || defined (CONFIG_COSMO_REV_10)|| defined(CONFIG_LGE_CX2) 
	D("REV_10 || 11||CX2 Board");
#endif

	return (temp_C);
}

int is_temp_good(int tbat)
{
	int temp_adc_value;

	if(tbat > 750 || tbat < -200)
	{
		D("BAT TEMP is bad stop charging...= %d", tbat);
		return 0;
	}
	return 1;
}

void check_battery_temp_for_safe_charging_and_quick_start(int charging_mode, int charger_type)
{
#define UNLIMITED_TEMP_VAL 0xA4

	int tbat, vbat = 0, soc;
	static u8 hw_cond = 0;
	u8 temp_control = 0;

	lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_CHARGING_TEMP_OFFSET , &temp_control, 1);
	D("LGE_NVDATA_DYNAMIC_CHARGING_TEMP_OFFSET temp_control = %x", temp_control);

	twl6030_adc_setup();
	twl6030_volt_setup();

	tbat = get_bat_temp();
	D(" = %d, STS_PLUG_DET %d, %d", hw_cond , STS_PLUG_DET, (hw_cond & STS_PLUG_DET));

	if( !is_temp_good(tbat) && (charging_mode != CHARGING_FACTORY) && (charging_mode != CHARGING_DEACTIVE) && (temp_control != LGE_NVDATA_DYNAMIC_CHARGING_TEMP_OFFSET) )
	{
		charging_mode = chr_ic_deactivation();
	}
	hw_cond = read_current_hw_condition();

#if defined (CONFIG_COSMO_REV_10) || defined (CONFIG_COSMO_REV_11)|| defined(CONFIG_LGE_CX2) || defined(CONFIG_LGE_P2)

	max17043_init();
	max17043_set_rcomp_with_temp(tbat);
	
	max17043_deinit();

	if(tbat > 0 )
	{
		vbat = get_voltage_once();
		mdelay(1);
		get_capacity_once();
		mdelay(1);

		if( charger_type != MUIC_FACTORY_MODE )
		{
			uboot_prcm_low();

			while(vbat < 3300 && (hw_cond & STS_PLUG_DET))
			{
				hw_cond = read_current_hw_condition();
				vbat = get_voltage_once();
				D("PMIC vbat = %d is too low", vbat);
				mdelay(150);		
			}
			D("(hw_cond & STS_PLUG_DET) = %d, STS_PLUG_DET %d, %d", hw_cond , STS_PLUG_DET, (hw_cond & STS_PLUG_DET));

			mdelay(40);		

			charging_mode = chr_ic_deactivation();

			max17043_init();
#ifdef CONFIG_COSMO_SU760
			mdelay(1700);		
#else
			mdelay(1000);		
#endif

			if(process_quick_start())
			{
				mdelay(300);		
			}
#if 0
			get_capacity_once();
			get_voltage_once();
#endif
			max17043_deinit();

			change_charging_mode(charger_type);

			uboot_prcm_high();
		}
		else
		{
			process_quick_start();
		}
	}
#endif

	return;
}

int change_charging_mode(int charger_type)
{
	int charging_mode;

	D("charger is detected..charger_type = %d", charger_type );

	if(charger_type == MUIC_FACTORY_MODE)
	{
		charging_mode = chr_ic_active_for_factory_mode();
	}
	else if(charger_type == MUIC_USB)
	{
		charging_mode = chr_ic_active_for_usb();
	}
	else if(charger_type == MUIC_CHRGER)
	{
		charging_mode = chr_ic_active_for_ta();					
	}
	else if(charger_type == MUIC_OPEN )
	{
		charging_mode = chr_ic_deactivation();
	}
	else
	{
		charging_mode = chr_ic_active_for_factory_mode();			
	}

	D("charging_mod = %d", charging_mode);

	D(".........  get_dp3t_mode= %d", get_dp3t_mode());

	return (charging_mode);
}

int charger_fsm(const int charger_type)
{
	static int charging_mode; 
	static u8 hw_cond = 0;
	const  u8 start_reason = read_start_reason();
	static u8 bat_online = 1;
	char cmd_line[512] = {0,};
	int off_mode_st = 0;

	static int current_charger_type;
	static u8 smpl_en = 0;		
	
	int vbbat = 0;

	current_charger_type = charger_type;
	charging_mode = change_charging_mode(current_charger_type);
	
	D("twl6030battery_voltage_setup");
	twl6030battery_voltage_setup();
	
	vbbat = get_adc_volt(8);
	D("Backup Battery Voltage = %d", vbbat);
	
	while(1)
	{
		
		hw_cond = read_current_hw_condition();

		printf("START_CONDITION: %x  HW_CONDITION: %x", start_reason, hw_cond);
		
		if(!bat_online)   
		{
			D("Please insert battery....");
			goto FSM_LOOP;
		}

		char reset_flag;
		lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_RESET_CAUSE_OFFSET,&reset_flag,1);

		printf("START_BY RESET code = %x",reset_flag);
		if(reset_flag == LGE_NVDATA_RESET_CAUSE_VAL_USER_RESET)		
		{
			
#if 0

			buf[NVDATA_RESET_FLAG_POS] = 0x00;
			
			mmc_write(1,buf,NVDATA_PARTITION_SECTOR_POS,NVDATA_PARTITION_SECTOR_SIZE);
#endif
			goto GOTO_KERNEL;
		}
		else if(reset_flag == LGE_NVDATA_RESET_CAUSE_VAL_AP_CRASH ||
				reset_flag == LGE_NVDATA_RESET_CAUSE_VAL_CP_CRASH )
		{
			goto GOTO_KERNEL;
		}
		else
		{		
			
			lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_SMPL_EN_OFFSET, &smpl_en, 1);	
			if (smpl_en) { 
				u8 vbat_low;

				select_bus(I2C1, OMAP_I2C_STANDARD);
				twl6030_i2c_read_u8(TWL6030_CHIP_PM, &vbat_low, PH_CFG_VBATLOW);
				D("PH_CFG_VBATLOW: %x", vbat_low);

				twl6030_set_battery_bounce_setting();

				#if 0
				
				lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_SMPL_EN_OFFSET, &smpl_en, 1);
				#endif
			
				if ((vbat_low & BB_SEL) && (start_reason & RESTART_BB) && !(hw_cond & STS_PLUG_DET)) {
					
					if (vbbat > 2200) {
						u32 smpl_count;

						lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_SMPL_COUNT_OFFSET, &smpl_count, 4);
						smpl_count++;
						lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_SMPL_COUNT_OFFSET, &smpl_count, 4);
						
						D("GOING TO KERNEL:: SMPL booting...");
						goto GOTO_KERNEL;
					}
				}
			}

			if( ( 2 == *(volatile unsigned int*)(SCRM_APEWARMRSTST)) || reset_flag != 0)
			{
				D("GOING TO KERNEL:: charging_mode = %d, reboot cause: %d", charging_mode, *(volatile unsigned int*)(SCRM_APEWARMRSTST));

				goto GOTO_KERNEL;
			}		
			if( (hw_cond & STS_PLUG_DET) || \
				 !(hw_cond & STS_PWRON) )

			{
				D("GOING TO KERNEL:");
#if defined (CONFIG_COSMO_SU760)
					start_vib();
#elif defined(CONFIG_COSMO_BOOT_VIBRATOR)
				if(!(hw_cond & STS_PWRON))
				{
					start_vib();
				}
#endif
				goto GOTO_KERNEL;
			}

		}

POWER_OFF:
		charging_mode = chr_ic_deactivation();
		D("GOING TO power_off :: charging_mode = %d, reboot cause: %d", charging_mode, *(volatile unsigned int*)(SCRM_APEWARMRSTST));
		D("power off.. charging_mode = %d", charging_mode);
		
		write_dev_on_off( MOD_DEVOFF | CON_DEVOFF | APP_DEVOFF );

FSM_LOOP:
		D("looping.. charging_mode = %d", charging_mode);
		udelay(10000);		
		continue;

GOTO_KERNEL:
		if( ( reset_flag && ( !start_reason || ( 2 == *(volatile unsigned int*)(SCRM_APEWARMRSTST)) ) ) \
			|| !(hw_cond & STS_PWRON) || (start_reason & STRT_ON_PWRON) || !(hw_cond & STS_PLUG_DET) )
		{
			off_mode_st = 0x20;
		}

		sprintf(cmd_line, "%s chg=%d", getenv("bootargs"), charger_type | off_mode_st);
		setenv("bootargs", cmd_line);

		D("going to kernel.. charging_mode = %d", charging_mode);
		write_dev_on_off( MOD_DEVON | CON_DEVON | APP_DEVON );
		return charging_mode;
	}
}

#if !defined(CONFIG_RT9524_CHARGER) && defined(CONFIG_TWL6030)
void twl6030_start_usb_charging(void)
{
	return;
}
#endif

int twl6030_init_battery_charging(int charger_type)
{

	return charger_fsm((const int)charger_type);
}

void twl6030_usb_device_settings()
{
	u8 data = 0;

	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x21, VUSB_CFG_STATE);

	twl6030_i2c_read_u8(TWL6030_CHIP_PM, &data, MISC2);
	data |= 0x10;

	twl6030_i2c_write_u8(TWL6030_CHIP_PM, data, MISC2);
	return;
}

void twl6030_set_vt_cam_ldo()
{
	i2c_init(OMAP_I2C_STANDARD, TWL6030_CHIP_PM);
	select_bus(I2C1, OMAP_I2C_STANDARD);

	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x01, VAUX2_CFG_GRP);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x15, VAUX2_CFG_TRANS);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x21, VAUX2_CFG_STATE);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x13, VAUX2_CFG_VOLTAGE); 

	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x01, VAUX3_CFG_GRP);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x15, VAUX3_CFG_TRANS);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x21, VAUX3_CFG_STATE);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x09, VAUX3_CFG_VOLTAGE); 

	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x01, REGEN1_CFG_GRP);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x15, REGEN1_CFG_TRANS);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x21, REGEN1_CFG_STATE); 

	select_bus(I2C1, OMAP_I2C_STANDARD); 
	return;
}

void	start_vib()
{

#define VIB_PWM 143
#define VIB_EN 25

	i2c_init(OMAP_I2C_STANDARD, TWL6030_CHIP_PM);
	select_bus(I2C1, OMAP_I2C_STANDARD);

	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x01, VUSIM_CFG_GRP);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x01, VUSIM_CFG_TRANS);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x21, VUSIM_CFG_STATE);
	twl6030_i2c_write_u8(TWL6030_CHIP_PM, 0x15, VUSIM_CFG_VOLTAGE); 

#if 1
	do_vib();
#else
	*((volatile unsigned int*) 0x4A100144)	|=	0x00000011;		

	*((volatile unsigned int*) 0x4A310134)	&=	~(1 << 25);		

	*((volatile unsigned int*) 0x4805B134)	&=	~(1 << 15);		

	*((volatile unsigned int*) 0x4A31013C)	|=	(1 << 25);		

	{
		int i;
		for(i = 0; i < 20 ; i++)
		{
			
			*((volatile unsigned int*) 0x4805B13C)	|=	(1 << 15);		
			udelay(2222);
			
			*((volatile unsigned int*) 0x4805B13C)	&=	~(1 << 15);		
			udelay(2222);
		}
	}

	*((volatile unsigned int*) 0x4A31013C)	&=	~(1 << 25);		

	*((volatile unsigned int*) 0x4A100144)	&=	~(0x00000010);		
#endif
	return;
}

void twl6030_set_battery_bounce_setting(void)
{

	u16 delay = 0xC;

	select_bus(I2C1, OMAP_I2C_STANDARD);

	twl6030_i2c_write_u8(TWL6030_CHIP_PM, BB_SEL | delay, PH_CFG_VBATLOW);
	return;
}

#endif
