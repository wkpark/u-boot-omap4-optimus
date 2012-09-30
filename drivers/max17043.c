#include <common.h>
#include <asm/arch/bits.h>
#include <asm/io.h>
#include <asm/arch/gpio.h>
#include <i2c.h>
#include <asm-arm/arch-omap4/i2c.h>
#include <twl6030.h>

#include "max17043.h"
#include <charging_ic.h>
#include <i2c.h>
#if defined (CONFIG_COSMO_REV_11)||defined(CONFIG_LGE_CX2) || defined(CONFIG_LGE_P2)
#include "gpio_i2c.h"
#endif
#define MAX17043_I2C_ADDR	0x36

#define MAX17043_VCELL_REG	0x02
#define MAX17043_SOC_REG	0x04
#define MAX17043_MODE_REG	0x06
#define MAX17043_VER_REG	0x08
#define MAX17043_CONFIG_REG	0x0C
#define MAX17043_CMD_REG	0xFE

#define MAX17043_RCOMP		0xB8
#define MAX17043_TOLERANCE	20

struct max17043_data {
	int vol;	
	int level;	
	int gradient;	
	int intercept;	
};
#define DEFAULT_RCOMP					0xB000

static int max17043_vcell = 0;
static int max17043_soc = 0;
static int max17043_version = 0;
static int max17043_config = DEFAULT_RCOMP;

#define SOC_TIMES 1000

#if 0

battery_graph_prop battery_soc_graph[] =
{
	{4100, 100 * SOC_TIMES },
	{3893, 75 * SOC_TIMES },
	{3783, 57 * SOC_TIMES },
	{3721, 36 * SOC_TIMES },
	{3686, 20 * SOC_TIMES },
	{3575, 4 * SOC_TIMES },
	{3487, 2 * SOC_TIMES },
	{3300, 0 * SOC_TIMES },
};

battery_graph_prop battery_soc_graph[] =
{
	{4100, 100 * SOC_TIMES },
	{3970, 85 * SOC_TIMES },
	{3798, 60 * SOC_TIMES },
	{3713, 36 * SOC_TIMES },
	{3675, 19 * SOC_TIMES },
	{3575, 4 * SOC_TIMES },
	{3500, 2 * SOC_TIMES },
	{3300, 0 * SOC_TIMES },
};
#else

battery_graph_prop battery_soc_graph[] =
{
	{4120, 100 * SOC_TIMES },
	{3938, 78 * SOC_TIMES },
	{3801, 57 * SOC_TIMES },
	{3728, 36 * SOC_TIMES },
	{3678, 18 * SOC_TIMES },
	{3598, 5 * SOC_TIMES },
	{3498, 2 * SOC_TIMES },
	{3200, 0 * SOC_TIMES },
};
#endif

int validate_gauge_value(int voltage, int capacity)
{
	int calculated_soc, error_range = SOC_TIMES;

	calculated_soc = reference_graph(voltage, battery_soc_graph, ARRAY_SIZE(battery_soc_graph), &error_range) / SOC_TIMES;

	printf(" ##### SOC = %d,  CALCULATED SOC = %d, error_range = %d, voltage = %d  ########\n", capacity, calculated_soc, error_range, voltage );

	if( (capacity < calculated_soc + error_range) && (capacity > calculated_soc - error_range) )
	{
		D(" ##### SOC & CALCULATED SOC is met  ##########");
		
		return 1;
	}

	D(" ##### SOC & CALCULATED SOC is NOT met 1!!!  ##########");
	
	return 0;

}

static int max17043_read_reg(int reg, uchar* data)
{
	int ret;
	int try = 1;

	while(try--)
	{
		udelay(100);
#if defined(CONFIG_COSMO_REV_11)||defined(CONFIG_LGE_CX2) || defined(CONFIG_LGE_P2)
		D("I2C1_read");
		ret = I2C1_read(reg, data, 2, 2);
#else
		ret = i2c_read(MAX17043_I2C_ADDR, (uint)reg, 1, data, 2);
#endif
		if(!ret)
		{
			break;
		}
		D("i2c_read error!!, ret = %d", ret);
	}
	
	return ret;
}

static int max17043_write_reg(int reg, uchar* data)
{
	int ret;
	int try = 1;

	D("data[0] = %x , data[1] = %x ", data[0], data[1]);
	while(try--)
	{
		udelay(100);
		
#if defined(CONFIG_COSMO_REV_11)||defined(CONFIG_LGE_CX2) || defined(CONFIG_LGE_P2)
		D("I2C1_write");
		ret = I2C1_write(reg, data, 2, 2);
#else
		ret = i2c_write_bytes(MAX17043_I2C_ADDR, (uint)reg, data, 2);
#endif
		if(!ret)
		{
			break;
		}
		D("i2c_write error!!, ret = %d", ret);
	}
	return ret;
}

static int max17043_read_vcell(void)
{
	int ret;
	uchar vcell[2];

	ret = max17043_read_reg(MAX17043_VCELL_REG, vcell);

	max17043_vcell = (((u16)vcell[0] << 8)  |  (u16)vcell[1]) >> 4;
	
	D("max17043_vcell vcell[0] = %d, vcell[1] = %d", vcell[0], vcell[1]);

	return ret;
}
static int max17043_read_soc(void)
{
	int ret;
	uchar soc[2];

	ret = max17043_read_reg(MAX17043_SOC_REG, soc);

	max17043_soc = ((u16)soc[0] << 8) |  (u16)soc[1];
	D("max17043_soc= %d", max17043_soc);

	return ret;
}
static int max17043_read_version(void)
{
	int ret;
	uchar version[2];

	ret = max17043_read_reg(MAX17043_VER_REG, version);

	max17043_version = ((u16)version[0] << 8) | (u16)version[1];

	return ret;
}

static int max17043_read_config(void)
{
	int ret;
	uchar config[2];

	ret = max17043_read_reg(MAX17043_CONFIG_REG, config);

	max17043_config = ((u16)config[0] << 8) | (u16)config[1];
	D("%x, %x, %x", max17043_config, config[0], config[1]);

	return ret;
}
static int max17043_write_config(void)
{
	int ret;
	uchar config[2];
	config[0] = (uchar)(((u16)(max17043_config & 0xFF00)) >> 8);
	D("%x, %x, %x", max17043_config, max17043_config & 0xFF00, config[0]);
	config[1] = (uchar)(max17043_config & 0x00FF);

	ret = max17043_write_reg(MAX17043_CONFIG_REG, config);

	return ret;
}

int max17043_set_rcomp_with_temp(int temp)
{
	int rcomp;

	rcomp = DEFAULT_RCOMP >> 8;

	temp /= 10;

	if(temp < 20)
	{
		rcomp += 5*(20-temp);
	}
	else if(temp>20)
	{
		rcomp -= (22*(temp-20))/10;
	}

	if(rcomp < 0x00)
		rcomp = 0x00;
	else if(rcomp > 0xff)
		rcomp = 0xff;

	max17043_config = ((rcomp << 8) & 0xFF00) | (max17043_config & 0xFF);

	D(" rcomp max17043_config  = %x with temp = %d", max17043_config, temp);

	max17043_write_config();

	return 0;
}

int read17043_get_rcomp(void)
{
	int ret;
	ret = max17043_read_config();

	if(ret)
	{
		printf("read17043_get_rcomp ERROR!!!");
		D("read17043_get_rcomp ERROR!!!");
	}
	D(" config register = %x",max17043_config);

}

int max17043_quickstart(void)
{
	int ret;
	
	uchar mode[2];

	mode[0] = 0x40;
	mode[1] = 0x00;

	ret = max17043_write_reg(MAX17043_MODE_REG, mode);
	printf("[Battery] MAX17043 Quickstart!\n");
	return ret;
}
static int max17043_reset(void)
{
	int ret;
	uchar cmd[2] = {0x54, 0x00};

	ret = max17043_write_reg(MAX17043_CMD_REG, cmd);

	return ret;
}

int get_capacity()
{
	int soc;

	soc = max17043_soc >> 8;

	if(max17043_soc & 0x80)
	{
		soc++;
	}
	D("get_capacity = %d", soc);

	return soc;
}

int get_capacity_once()
{
	int soc;

#if defined(CONFIG_COSMO_REV_10) || defined(CONFIG_COSMO_REV_11)||defined(CONFIG_LGE_CX2) || defined(CONFIG_LGE_P2)
	max17043_init();
#endif

	max17043_read_soc();

	soc = max17043_soc >> 8;

	if(max17043_soc & 0x80)
	{
		soc++;
	}
	D("get_capacity = %d", soc);

#if defined(CONFIG_COSMO_REV_10) || defined(CONFIG_COSMO_REV_11)||defined(CONFIG_LGE_CX2) || defined(CONFIG_LGE_P2)
	max17043_deinit();
#endif

	return soc;
}

int get_voltage()
{
	int volt;

	volt = (max17043_vcell * 5) >> 2 ;
	D("get_voltage = %d", volt);

	return volt;
}

int get_voltage_once()
{
	int volt;

#if defined(CONFIG_COSMO_REV_10) || defined(CONFIG_COSMO_REV_11)||defined(CONFIG_LGE_CX2) || defined(CONFIG_LGE_P2)
	max17043_init();
#endif

	max17043_read_vcell();

	volt = (max17043_vcell * 5) >> 2 ;
	D("get_voltage = %d", volt);
#if defined(CONFIG_COSMO_REV_10) || defined(CONFIG_COSMO_REV_11)||defined(CONFIG_LGE_CX2) || defined(CONFIG_LGE_P2)
	max17043_deinit();
#endif

	return volt;
}

int get_voltage_avg()
{
#define MAX_COUNT 16
	int volt = 0;
	int count = MAX_COUNT;

#if defined(CONFIG_COSMO_REV_10) || defined(CONFIG_COSMO_REV_11)||defined(CONFIG_LGE_CX2) || defined(CONFIG_LGE_P2)
	max17043_init();
#endif

	while(count--)
	{
		max17043_read_vcell();
		volt += (max17043_vcell * 5) >> 2 ;
		mdelay(5);
	}
	volt /= MAX_COUNT;
	max17043_vcell = volt;
	D("get_voltage = %d", volt);
#if defined(CONFIG_COSMO_REV_10) || defined(CONFIG_COSMO_REV_11)||defined(CONFIG_LGE_CX2) || defined(CONFIG_LGE_P2)
	max17043_deinit();
#endif

	return volt;
}

int process_quick_start(void)
{
	int volt, soc;
	int ret;

	D("process_quick_start~~~~~~~~~~~~~");

	if(max17043_read_soc())
	{
		goto QUICK_START;
	}

	if(max17043_read_vcell())
	{
		goto QUICK_START;
	}

	soc = get_capacity();
	volt = get_voltage();

	D("soc = %d, volt = %d ", soc, volt);

	if(validate_gauge_value(volt, soc))
	{
		return 0;
	}

	if( volt > 4010 && soc > 95 )
	{
		return 0;
	}

QUICK_START:
	D("QUICK_START!!!!!!!!!!!!!!!!!!!!");
	D("soc = %d, volt = %d before QUICK_START", soc, volt);

	max17043_quickstart();

	return 1;
}

int max17043_init(void)
{

	select_bus(I2C3, OMAP_I2C_STANDARD); 
	i2c_init(OMAP_I2C_STANDARD, MAX17043_I2C_ADDR);

	udelay(1000);		

	return 0;
}

int max17043_deinit(void)
{
	select_bus(I2C1, OMAP_I2C_STANDARD); 
	udelay(1000);		

	return 0;
}
