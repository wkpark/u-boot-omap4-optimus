#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/io.h>
#include <asm/arch/bits.h>
#include <asm/arch/mux.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/sys_info.h>
#include <asm/arch/clocks.h>
#include <asm/arch/mem.h>
#include <i2c.h>
#include <twl6030.h>
#include <asm/mach-types.h>
#include <environment.h>

#include <cosmo_muic.h>
#include <charging_ic.h>

extern uchar(*boot_env_get_char_spec) (int index);
extern int (*boot_env_init) (void);
extern int (*boot_saveenv) (void);
extern void (*boot_env_relocate_spec) (void);

extern uchar mmc_env_get_char_spec(int index);
extern int mmc_env_init(void);
extern int mmc_saveenv(void);
extern void mmc_env_relocate_spec(void);
extern char *mmc_env_name_spec;

char *env_name_spec;

env_t *env_ptr;

int board_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	boot_env_get_char_spec = mmc_env_get_char_spec;
	boot_env_init = mmc_env_init;
	boot_saveenv = mmc_saveenv;
	boot_env_relocate_spec = mmc_env_relocate_spec;
	env_ptr = (env_t *) (CFG_FLASH_BASE + CFG_ENV_OFFSET);
	env_name_spec = mmc_env_name_spec;

	gpmc_init(); 
	
	gd->bd->bi_arch_number = MACH_TYPE_LGE_COSMOPOLITAN;
	gd->bd->bi_boot_params = (0x80000000 + 0x100); 

	return 0;
}

/*****************************************
 * Routine: board_late_init
 * Description: Late hardware init.
 * *****************************************/
int board_late_init(void)
{
        int status;

        /* mmc */
        if( (status = omap4_mmc_init()) != 0) {
                return status;
        }
	return 0;
}

int get_boot_type(void);
void v7_flush_dcache_all(int , int);
void setup_auxcr(int , int);

static	int		_downloadkey;

#define PADCONF_KEY_C0		(OMAP44XX_CTRL_BASE + CONTROL_PADCONF_UNIPRO_TX0)
#define PADCONF_KEY_C1		(OMAP44XX_CTRL_BASE + CONTROL_PADCONF_UNIPRO_TY0)
#define PADCONF_KEY_R0		(OMAP44XX_CTRL_BASE + CONTROL_PADCONF_UNIPRO_RX0)
#define PADCONF_KEY_R1		(OMAP44XX_CTRL_BASE + CONTROL_PADCONF_UNIPRO_RY0)

#define GPIO_KEY_C0			171
#define GPIO_KEY_C1			172
#define GPIO_KEY_R0			175
#define GPIO_KEY_R1			176

#define KEY_VOLUMEUP		0x0001
#define KEY_VOLUMEDOWN		0x0002
#define KEY_3DHOT			0x0004

int	do_keyscan(int force_rescan)
{
	static int		scanned	=	0;
	static int		scancode;
	short	c0, c1, r0, r1;

	if (scanned && !force_rescan)
		return	scancode;

	scancode	=	0;

	c0	=	__raw_readw(PADCONF_KEY_C0);	
	c1	=	__raw_readw(PADCONF_KEY_C1);	
	r0	=	__raw_readw(PADCONF_KEY_R0);	
	r1	=	__raw_readw(PADCONF_KEY_R1);	

	__raw_writew(M3,				PADCONF_KEY_C0);	
	__raw_writew(M3,				PADCONF_KEY_C1);	
	__raw_writew(PTU | IEN | M3,	PADCONF_KEY_R0);	
	__raw_writew(PTU | IEN | M3,	PADCONF_KEY_R1);	

	omap_set_gpio_direction(GPIO_KEY_C0, 0);	
	omap_set_gpio_direction(GPIO_KEY_C1, 0);
	omap_set_gpio_direction(GPIO_KEY_R0, 1);	
	omap_set_gpio_direction(GPIO_KEY_R1, 1);

	omap_set_gpio_dataout(GPIO_KEY_C0, 1);
	omap_set_gpio_dataout(GPIO_KEY_C1, 0);
	if (!omap_get_gpio_datain(GPIO_KEY_R1))
		scancode	|=	KEY_3DHOT;

	omap_set_gpio_dataout(GPIO_KEY_C0, 0);
	omap_set_gpio_dataout(GPIO_KEY_C1, 1);
	if (!omap_get_gpio_datain(GPIO_KEY_R0))
		scancode	|=	KEY_VOLUMEUP;
	if (!omap_get_gpio_datain(GPIO_KEY_R1))
		scancode	|=	KEY_VOLUMEDOWN;

	__raw_writew(c0, PADCONF_KEY_C0);	
	__raw_writew(c1, PADCONF_KEY_C1);	
	__raw_writew(r0, PADCONF_KEY_R0);	
	__raw_writew(r1, PADCONF_KEY_R1);	

	scanned	=	1;
	if (force_rescan == 2)
		scanned	= 0;

	return	scancode;
}

int	downloadkey(void)
{
#ifdef CONFIG_COSMO_SU760
	int	key	= do_keyscan(1);
#else
	int	key	= do_keyscan(0);
#endif
	return	(key == KEY_VOLUMEUP);
}

int	recoverykey(void)
{
	int	key	= do_keyscan(0);

	return	(key == (KEY_VOLUMEDOWN | KEY_3DHOT));
}
int isVolumeDownKeyPressed(void){
	int key = do_keyscan(1);
	return (key == KEY_VOLUMEDOWN);
}

int trap_exit_key(void)
{

	int scancode;
	short	c0, c1, r0, r1;
	
	scancode	=	0;

	c0	=	__raw_readw(PADCONF_KEY_C0);	
	c1	=	__raw_readw(PADCONF_KEY_C1);	
	r0	=	__raw_readw(PADCONF_KEY_R0);	
	r1	=	__raw_readw(PADCONF_KEY_R1);	

	__raw_writew(M3,				PADCONF_KEY_C0);	
	__raw_writew(M3,				PADCONF_KEY_C1);	
	__raw_writew(PTU | IEN | M3,	PADCONF_KEY_R0);	
	__raw_writew(PTU | IEN | M3,	PADCONF_KEY_R1);	

	omap_set_gpio_direction(GPIO_KEY_C0, 0);	
	omap_set_gpio_direction(GPIO_KEY_C1, 0);
	omap_set_gpio_direction(GPIO_KEY_R0, 1);	
	omap_set_gpio_direction(GPIO_KEY_R1, 1);

	omap_set_gpio_dataout(GPIO_KEY_C0, 1);
	omap_set_gpio_dataout(GPIO_KEY_C1, 0);
	if (!omap_get_gpio_datain(GPIO_KEY_R1))
		scancode	|=	KEY_3DHOT;

	omap_set_gpio_dataout(GPIO_KEY_C0, 0);
	omap_set_gpio_dataout(GPIO_KEY_C1, 1);
	if (!omap_get_gpio_datain(GPIO_KEY_R0))
		scancode	|=	KEY_VOLUMEUP;
	if (!omap_get_gpio_datain(GPIO_KEY_R1))
		scancode	|=	KEY_VOLUMEDOWN;

	__raw_writew(c0, PADCONF_KEY_C0);	
	__raw_writew(c1, PADCONF_KEY_C1);	
	__raw_writew(r0, PADCONF_KEY_R0);	
	__raw_writew(r1, PADCONF_KEY_R1);	

	return	(scancode == KEY_3DHOT);
	int key = do_keyscan(1);
	return	(key == KEY_3DHOT);
}

#define		OMAP44XX_WKUP_CTRL_BASE		0x4A31E000
#if 1
#define M0_SAFE M0
#define M1_SAFE M1
#define M2_SAFE M2
#define M4_SAFE M4
#define M7_SAFE M7
#define M3_SAFE M3
#define M5_SAFE M5
#define M6_SAFE M6
#else
#define M0_SAFE M7
#define M1_SAFE M7
#define M2_SAFE M7
#define M4_SAFE M7
#define M7_SAFE M7
#define M3_SAFE M7
#define M5_SAFE M7
#define M6_SAFE M7
#endif
#define		MV(OFFSET, VALUE)\
			__raw_writew((VALUE), OMAP44XX_CTRL_BASE + (OFFSET));
#define		MV1(OFFSET, VALUE)\
			__raw_writew((VALUE), OMAP44XX_WKUP_CTRL_BASE + (OFFSET));

#define		CP(x)	(CONTROL_PADCONF_##x)
#define		WK(x)	(CONTROL_WKUP_##x)

#if defined(CONFIG_COSMO_REV_C) && defined(CONFIG_COSMO_SU760) && defined(CONFIG_COSMO_MIPI_HSI)
#include "mux_su760_rev_a_mipi_hsi.c"
#elif defined (CONFIG_COSMO_REV_D) && defined(CONFIG_COSMO_SU760) && defined(CONFIG_COSMO_MIPI_HSI)
#include "mux_su760_rev_b_mipi_hsi.c"
#elif defined (CONFIG_COSMO_REV_11) && defined(CONFIG_COSMO_P920) && defined(CONFIG_COSMO_MIPI_HSI)
#include "mux_p920.h"
#elif defined (CONFIG_COSMO_REV_11) && defined(CONFIG_COSMO_SU760) && defined(CONFIG_COSMO_MIPI_HSI)
//#include "mux_su760_rev_d_mipi_hsi.c"
#include "mux_su760.h"
#elif defined(CONFIG_COSMO_REV_C) && defined(CONFIG_COSMO_SU760)

#include "mux_su760_rev_a.c"
#elif defined (CONFIG_COSMO_REV_D) && defined(CONFIG_COSMO_SU760)
#include "mux_su760_rev_b.c"
#elif defined (CONFIG_COSMO_REV_11) && defined(CONFIG_COSMO_SU760)
#include "mux_su760_rev_d.c"

#elif defined(CONFIG_COSMO_REV_C) && defined(CONFIG_COSMO_MIPI_HSI)
#include "mux_rev_c_mipi_hsi.c"
#elif defined(CONFIG_COSMO_REV_D) && defined(CONFIG_COSMO_MIPI_HSI)
#include "mux_rev_d_mipi_hsi.c"
#elif defined(CONFIG_COSMO_REV_E) && defined(CONFIG_COSMO_MIPI_HSI)
#include "mux_rev_d_mipi_hsi.c"
#elif defined(CONFIG_COSMO_REV_10) && defined(CONFIG_COSMO_MIPI_HSI)
#include "mux_rev_1.0_mipi_hsi.c"
#elif defined(CONFIG_COSMO_REV_11) && defined(CONFIG_COSMO_MIPI_HSI)
#include "mux_rev_1.1_mipi_hsi.c"

#elif defined (CONFIG_COSMO_REV_B)
#include "mux_rev_b.c"
#elif defined (CONFIG_COSMO_REV_C)
#include "mux_rev_c.c"
#elif defined (CONFIG_COSMO_REV_D)
#include "mux_rev_d.c"
#elif defined (CONFIG_COSMO_REV_E)
#include "mux_rev_d.c"
#elif defined (CONFIG_COSMO_REV_10)
#include "mux_rev_1.0.c"
#elif defined (CONFIG_COSMO_REV_11)
#include "mux_rev_1.1.c"

#else
#include "mux_rev_d.c"
#endif

void set_muxconf_regs(void)
{
	MUX_DEFAULT_OMAP4();
	return;
}

void force_i2c3_stop_condition()
{
	MV(CP(I2C3_SCL),		PTU | M3)	
	MV(CP(I2C3_SDA),		PTU | M3)	
	omap_set_gpio_direction(130, 0);		
	omap_set_gpio_dataout(130, 0);			
	omap_set_gpio_direction(131, 0);		
	omap_set_gpio_dataout(131, 0);			
	udelay(500);
	omap_set_gpio_dataout(130, 1);			
	udelay(500);
	omap_set_gpio_dataout(131, 1);			
	udelay(500);
	MV(CP(I2C3_SCL),		PTU | IEN | EN | M0)
	MV(CP(I2C3_SDA),		PTU | IEN | EN | M0)
	omap_set_gpio_direction(130, 1);		
	omap_set_gpio_direction(131, 1);		
}

void vt_cam_ready()
{
	
	MV(CP(DPM_EMU11),		PTD | M3)			\
	omap_set_gpio_direction(22, 0);		
	omap_set_gpio_dataout(22, 0);		
	
	MV(CP(DPM_EMU12),		PTD | M3)			\
	omap_set_gpio_direction(23, 0);		
	omap_set_gpio_dataout(23, 0);		
	udelay(500);
	omap_set_gpio_dataout(23, 1);		
}
void vt_cam_standby()
{
	MV(CP(DPM_EMU11),		PTD | OFF_EN | OFF_PD | OFF_OUT_PTD | M3)			\
	omap_set_gpio_direction(22, 0);		
	omap_set_gpio_dataout(22, 1);		
}

void	do_vib(void)
{

#define VIB_PWM 143
#define VIB_EN 25

	int i = 0;

	volatile unsigned long int busy_delay = 0;

	MV(CP(UART3_RX_IRRX),	3);

	omap_set_gpio_direction(VIB_PWM, 0);		
	omap_set_gpio_direction(VIB_EN, 0); 	

	omap_set_gpio_dataout(VIB_EN, 1);

	for(i = 0; i < 30 ; i++)
	{
		omap_set_gpio_dataout(VIB_PWM, 1);
		while(busy_delay < 4500)
		{
			busy_delay = busy_delay + 1;
		}
		busy_delay = 0;

		omap_set_gpio_dataout(VIB_PWM, 0);

		while(busy_delay < 4500)
		{
			busy_delay = busy_delay + 1;
		}
		busy_delay = 0;
	}
	omap_set_gpio_dataout(VIB_EN, 0);

	MV(CP(UART3_RX_IRRX),	1);
}
