#include <common.h>
#include <command.h>
#include <malloc.h>
#include <devices.h>
#include <version.h>
#include <net.h>
#include <asm/arch/i2c.h>
#include <asm/arch/gpio.h>

#include <asm-arm/io.h>
#if defined (CONFIG_LGE_CX2)
#include "../board/cx2/cx2_dss.h"
#elif defined (CONFIG_LGE_P2)
#include "../board/p2/p2_dss.h"
#else
#include "../board/cosmopolitan/cosmo_dss.h"
#endif

#ifdef CONFIG_DRIVER_SMC91111
#include "../drivers/smc91111.h"
#endif
#ifdef CONFIG_DRIVER_LAN91C96
#include "../drivers/lan91c96.h"
#endif
#ifdef CONFIG_FBCON
#include "../drivers/fbcon.h"
#endif

#include <twl6030.h>

#ifdef CONFIG_LGE_NVDATA
#include <lge_nvdata_emmc.h>
#endif

#define CABLE_DETECT_910K

static char web_down_flag1;
static char web_down_flag2;
extern int web_download_mode;

DECLARE_GLOBAL_DATA_PTR;

#if (CONFIG_COMMANDS & CFG_CMD_NAND)
#ifdef ENV_IS_VARIABLE
extern u8 is_nand;
#endif
void nand_init (void);
#endif

#if (CONFIG_COMMANDS & CFG_CMD_FLASH)
#ifdef ENV_IS_VARIABLE
extern u8 is_flash;
#endif
#endif

#if (CONFIG_COMMANDS & CFG_CMD_ONENAND)
#ifdef ENV_IS_VARIABLE
extern u8 is_onenand;
#endif
void onenand_init(void);
#endif

ulong monitor_flash_len;

#ifdef CONFIG_HAS_DATAFLASH
extern int  AT91F_DataflashInit(void);
extern void dataflash_print_info(void);
#endif

#ifdef CONFIG_MMC
extern void board_mmc_init(void);
#endif

#ifndef CONFIG_IDENT_STRING
#define CONFIG_IDENT_STRING ""
#endif

const char version_string[] =
	U_BOOT_VERSION" (" __DATE__ " - " __TIME__ ")"CONFIG_IDENT_STRING;

#ifdef CONFIG_DRIVER_CS8900
extern void cs8900_get_enetaddr (uchar * addr);
#endif

#ifdef CONFIG_DRIVER_RTL8019
extern void rtl8019_get_enetaddr (uchar * addr);
#endif

extern void ifx_start_low();
extern void ifx_boot_start();

static ulong mem_malloc_start = 0;
static ulong mem_malloc_end = 0;
static ulong mem_malloc_brk = 0;

static
void mem_malloc_init (ulong dest_addr)
{
	mem_malloc_start = dest_addr;
	mem_malloc_end = dest_addr + CFG_MALLOC_LEN;
	mem_malloc_brk = mem_malloc_start;

	memset ((void *) mem_malloc_start, 0,
			mem_malloc_end - mem_malloc_start);
}

void *sbrk (ptrdiff_t increment)
{
	ulong old = mem_malloc_brk;
	ulong new = old + increment;

	if ((new < mem_malloc_start) || (new > mem_malloc_end)) {
		return (NULL);
	}
	mem_malloc_brk = new;

	return ((void *) old);
}

static int init_baudrate (void)
{
	char tmp[64];	
	int i = getenv_r ("baudrate", tmp, sizeof (tmp));
	gd->bd->bi_baudrate = gd->baudrate = (i > 0)
			? (int) simple_strtoul (tmp, NULL, 10)
			: CONFIG_BAUDRATE;

	return (0);
}

static int display_banner (void)
{
	printf ("\n\n%s\n\n", version_string);
	debug ("U-Boot code: %08lX -> %08lX  BSS: -> %08lX\n",
	       _armboot_start, _bss_start, _bss_end);
#ifdef CONFIG_MODEM_SUPPORT
	debug ("Modem Support enabled\n");
#endif
#ifdef CONFIG_USE_IRQ
	debug ("IRQ Stack: %08lx\n", IRQ_STACK_START);
	debug ("FIQ Stack: %08lx\n", FIQ_STACK_START);
#endif

	return (0);
}

static int display_dram_config (void)
{
	int i;

#ifdef DEBUG
	puts ("RAM Configuration:\n");

	for(i=0; i<CONFIG_NR_DRAM_BANKS; i++) {
		printf ("Bank #%d: %08lx ", i, gd->bd->bi_dram[i].start);
		print_size (gd->bd->bi_dram[i].size, "\n");
	}
#else
	ulong size = 0;

	for (i=0; i<CONFIG_NR_DRAM_BANKS; i++) {
		size += gd->bd->bi_dram[i].size;
	}
	puts("DRAM:  ");
	print_size(size, "\n");
#endif

	return (0);
}

#ifndef CFG_NO_FLASH
static void display_flash_config (ulong size)
{
	puts ("Flash: ");
	print_size (size, "\n");
}
#endif 

typedef int (init_fnc_t) (void);

int print_cpuinfo (void); 

init_fnc_t *init_sequence[] = {
	cpu_init,		
	board_init,		
	interrupt_init,		
	env_init,		
	init_baudrate,		
	serial_init,		
	console_init_f,		
	display_banner,		
#if defined(CONFIG_DISPLAY_CPUINFO)
	print_cpuinfo,		
#endif
#if defined(CONFIG_DISPLAY_BOARDINFO)
	checkboard,		
#endif
	dram_init,		
	display_dram_config,
	NULL,
};

#define	GPIO_FLASH_EN		190
#define GPIO_MOVIE_MODE_EN	28
#define GPIO_LCD_RESET_N	30
#if defined (CONFIG_LGE_CX2) || defined (CONFIG_LGE_P2)
#define GPIO_LCD_MAKER_ID 158
#endif

int             DownloadValid = 0;  

#define GPIO_LCD_CP_EN		98

#define GPIO_LCD_EN		27

#ifdef CONFIG_FBCON
static struct fbcon_config fb_cfg = {
	.base = 0x87000000,
	.width = 480,
	.height = 800,
	.stride = 480,
	.bpp = 32,
	.format = FB_FORMAT_RGB8888,
	.update_start = NULL,
	.update_done = NULL,
};
#endif

#if defined (CONFIG_LGE_CX2) || defined (CONFIG_LGE_P2)
void lcd_init(void)
{
	unsigned char	data_for_lcd;

	omap_set_gpio_direction(GPIO_LCD_EN, 0);		
	omap_set_gpio_dataout(GPIO_LCD_EN, 1);

	omap_set_gpio_direction(GPIO_LCD_CP_EN, 0); 	
	omap_set_gpio_dataout(GPIO_LCD_CP_EN, 1);

	select_bus(1, OMAP_I2C_STANDARD);

	data_for_lcd	=	0x05;

	i2c_write(0x38, 0x10, 1, &data_for_lcd, 1); 	
	
	data_for_lcd	=		0x42;
	i2c_write(0x38, 0x30, 1, &data_for_lcd, 1); 	
	
	data_for_lcd	=		0x7F;
	i2c_write(0x38, 0xA0, 1, &data_for_lcd, 1); 	

	select_bus(0, OMAP_I2C_STANDARD);			

	omap_set_gpio_direction(GPIO_LCD_RESET_N, 0);	
	omap_set_gpio_dataout(GPIO_LCD_RESET_N, 1); 
	omap_set_gpio_dataout(GPIO_LCD_RESET_N, 0);
	omap_set_gpio_dataout(GPIO_LCD_RESET_N, 1);

}

#else
void	lcd_init()
{
	unsigned char	data_for_lcd;

	omap_set_gpio_direction(GPIO_LCD_EN, 0);		
	omap_set_gpio_dataout(GPIO_LCD_EN, 1);

	omap_set_gpio_direction(GPIO_LCD_CP_EN, 0);		
	omap_set_gpio_dataout(GPIO_LCD_CP_EN, 1);

	i2c_init(OMAP_I2C_STANDARD, 0x36);
	select_bus(I2C2, OMAP_I2C_STANDARD);

#define	LM3528_UNISON	1

#ifdef	LM3528_UNISON
	data_for_lcd	=	0xc5;	
#else
	data_for_lcd	=	0xc3;	
#endif
	i2c_write(0x36, 0x10, 1, &data_for_lcd, 1);	

	omap_set_gpio_dataout(GPIO_LCD_EN, 0);	
  mdelay(40);

	data_for_lcd	=	0x00;	
	i2c_write(0x36, 0x80, 1, &data_for_lcd, 1);	

	omap_set_gpio_dataout(GPIO_LCD_EN, 1);

	if(web_download_mode==1)
		data_for_lcd	=	0x44; 
	else
	data_for_lcd	=	0x7A;

	i2c_write(0x36, 0xA0, 1, &data_for_lcd, 1);	

#ifndef	LM3528_UNISON	
	data_for_lcd	=	0x7f;
	i2c_write(0x36, 0xB0, 1, &data_for_lcd, 1);	
#endif

	select_bus(0, 400);			

	omap_set_gpio_direction(GPIO_LCD_RESET_N, 0);	
	omap_set_gpio_dataout(GPIO_LCD_RESET_N, 0);	
	omap_set_gpio_dataout(GPIO_LCD_RESET_N, 1);

}
#endif

extern int dsi2_init(void);
extern void dispc_enable_lcd_out(bool);
void init_panel(int force)
{
	static int initialized = 0;

	if (!force && initialized)
		return;

#if defined (CONFIG_LGE_CX2)
	unsigned char maker_id;
#endif
	*((volatile unsigned int*) 0x4a100600) = 0x14700000;
	*((volatile unsigned int*) 0x4A100604) = 0x88888888;

	omap_set_gpio_direction(GPIO_FLASH_EN, 0);
	omap_set_gpio_dataout(GPIO_FLASH_EN, 0);

#define GPIO_AUD_PWRON		127
	omap_set_gpio_direction(GPIO_AUD_PWRON, 0);
	omap_set_gpio_dataout(GPIO_AUD_PWRON, 0);

	lcd_init();

#if defined(CONFIG_COSMO_BOOT_IMG)
#if defined (CONFIG_LGE_CX2)
	maker_id = omap_get_gpio_datain(GPIO_LCD_MAKER_ID);
	printf("SJ	:	Maker_ID is %u\n",maker_id);
	maker_id = 1;
#endif

#define dispc_read_reg(x) 	__raw_readl(0x58001000 + x.idx)
#define dispc_write_reg(x,val)	__raw_writel(val, 0x58001000 + x.idx);

	sr32(CM_CLKSEL_CORE,0,32,0x118);

	sr32(CM_DSS_CLKSTCTRL,0,32,0x2);
	sr32(CM_DSS_DSS_CLKCTRL,0,32,0x00000f02);
	sr32(CM_DSS_L3INIT_CLKCTRL,0,32,0x00000002);

	*(volatile int*) 0x4A307100 = 0x00030007;
	mdelay(2);

	u32 l;
	l = dispc_read_reg(DISPC_SYSCONFIG);
	l = FLD_MOD(l, 2, 13, 12);
	l = FLD_MOD(l, 2, 4, 3);
	l = FLD_MOD(l, 1, 2, 2);
	l = FLD_MOD(l, 1, 0, 0);
	dispc_write_reg(DISPC_SYSCONFIG, l);

	l = dispc_read_reg(DISPC_CONFIG);
	l = FLD_MOD(l, OMAP_DSS_LOAD_FRAME_ONLY, 2, 1);
	dispc_write_reg(DISPC_CONFIG, l);

	dsi2_init();

#if defined(CONFIG_LGE_CX2) || defined(CONFIG_LGE_P2)
u8 hitachi_lcd_command_for_mipi[][30] = {
#if defined(CONFIG_LGE_CX2)
	{ 0x05, 0x2A, 0x00, 0x00, 0x01, 0xDF, },
	{ 0x05, 0x2B, 0x00, 0x00, 0x03, 0x1F, },
	{ 0x02, 0x36, 0x00, },
	{ 0x02, 0x3A, 0x77, },
	{ 0x03, 0xB2, 0x00, 0xC7, },
	{ 0x02, 0xB3, 0x02, },
	{ 0x08, 0xB5, 0x01, 0x00, 0x1F, 0x00, 0x20, 0x1F, 0x00, },
	{ 0x09, 0xB6, 0x10, 0x01, 0x81, 0x01, 0x01, 0x00, 0x00, 0x00, },
	{ 0x08, 0xB7, 0x46, 0x00, 0x08, 0x00, 0x0C, 0x03, 0x00, },
	{ 0x06, 0xB8, 0x38, 0xF8, 0x2C, 0x66, 0xCC, },
	{ 0x04, 0xC0, 0x00, 0x05, 0x10, },
	{ 0x03, 0xC2, 0x07, 0x00, },
	{ 0x06, 0xC3, 0x20, 0x02, 0x00, 0x08, 0x0A, },
	{ 0x07, 0xC4, 0x01, 0x02, 0x00, 0x00, 0x11, 0x8A, },
	{ 0x06, 0xC5, 0x12, 0x34, 0x11, 0x14, 0x13, },
	{ 0x03, 0xC6, 0x22, 0x00, },
	{ 0x06, 0xC7, 0x04, 0x98, 0x18, 0x00, 0x00, },
	{ 0x03, 0xC8, 0x43, 0x60, },
	{ 0x04, 0xC9, 0x00, 0x30, 0x10, },
	{ 0x05, 0xCA, 0x82, 0xA1, 0x21, 0x11, },
	{ 0x04, 0xCB, 0x0B, 0x15, 0x34, },
	{ 0x0A, 0xD0, 0x03, 0x16, 0x77, 0x06, 0x00, 0x00, 0x31, 0x11, 0x03, },
	{ 0x0A, 0xD1, 0x03, 0x16, 0x77, 0x06, 0x00, 0x00, 0x31, 0x11, 0x03, },
	{ 0x0A, 0xD2, 0x03, 0x16, 0x77, 0x06, 0x00, 0x00, 0x31, 0x11, 0x03, },
	{ 0x0A, 0xD3, 0x03, 0x16, 0x77, 0x06, 0x00, 0x00, 0x31, 0x11, 0x03, },
	{ 0x0A, 0xD4, 0x03, 0x16, 0x77, 0x06, 0x00, 0x00, 0x31, 0x11, 0x03, },
	{ 0x0A, 0xD5, 0x03, 0x16, 0x77, 0x06, 0x00, 0x00, 0x31, 0x11, 0x03, },
	{ 0x06, 0xE0, 0x43, 0x40, 0x80, 0x00, 0x00, },
	{ 0x04, 0xE1, 0x77, 0x38, 0x22, },
	{ 0x05, 0xE3, 0x01, 0x00, 0x02, 0x00, },
	{ 0x02, 0xE7, 0x00, },
#elif defined(CONFIG_LGE_P2)
	/* from drivers/video/omap2/displays/panel-tx11d101vm-0eaa.c */
//	{ 0x05, 0x2A, 0x00, 0x00, 0x01, 0xDF, },
	{ 0x05, 0x2B, 0x00, 0x00, 0x03, 0x1F, },
//	{ 0x02, 0x36, 0x00, },
//	{ 0x02, 0x3A, 0x77, },
	{ 0x03, 0xB2, 0x00, 0xC7, },
//	{ 0x02, 0xB3, 0x02, },
	{ 0x08, 0xB5, 0x01, 0x00, 0x22, 0x00, 0x20, 0x22, 0x00, },
	{ 0x09, 0xB6, 0x10, 0x01, 0x81, 0x01, 0x01, 0x00, 0x00, 0x00, },
	{ 0x08, 0xB7, 0x46, 0x00, 0x08, 0x00, 0x0C, 0x03, 0x00, },
	{ 0x06, 0xB8, 0x3F, 0xFF, 0x28, 0x66, 0xCC, },
	{ 0x04, 0xC0, 0x01, 0x05, 0x10, },
	{ 0x03, 0xC2, 0x07, 0x00, },
	{ 0x06, 0xC3, 0x20, 0x02, 0x00, 0x08, 0x0A, },
	{ 0x07, 0xC4, 0x01, 0x02, 0x00, 0x00, 0x11, 0x8A, },
	{ 0x06, 0xC5, 0x03, 0x33, 0x03, 0x00, 0x00, },
	{ 0x03, 0xC6, 0x22, 0x00, },
	{ 0x06, 0xC7, 0x04, 0x96, 0x16, 0x00, 0x00, },
	{ 0x03, 0xC8, 0x43, 0x60, },
//	{ 0x05, 0xCA, 0x82, 0xA1, 0x21, 0x11, },
	{ 0x03, 0x44, 0x03, 0x1F, }, /* TE generated right after scaning of 799 line */
	{ 0x0A, 0xD0, 0x11, 0x25, 0x76, 0x11, 0x00, 0x00, 0x30, 0x01, 0x02, },
	{ 0x0A, 0xD1, 0x11, 0x25, 0x76, 0x11, 0x00, 0x00, 0x30, 0x01, 0x02, },
	{ 0x0A, 0xD2, 0x11, 0x25, 0x76, 0x11, 0x00, 0x00, 0x30, 0x01, 0x02, },
	{ 0x0A, 0xD3, 0x11, 0x25, 0x76, 0x11, 0x00, 0x00, 0x30, 0x01, 0x02, },
	{ 0x0A, 0xD4, 0x11, 0x25, 0x76, 0x11, 0x00, 0x00, 0x30, 0x01, 0x02, },
	{ 0x0A, 0xD5, 0x11, 0x25, 0x76, 0x11, 0x00, 0x00, 0x30, 0x01, 0x02, },
	{ 0x06, 0xE0, 0x47, 0x40, 0x80, 0x00, 0x00, },
//	{ 0x04, 0xE1, 0x77, 0x38, 0x22, },
//	{ 0x05, 0xE3, 0x01, 0x00, 0x02, 0x00, },
	{ 0x02, 0xE7, 0x00, },
#endif
	{ END_OF_COMMAND, },
};

#if defined(CONFIG_LGE_CX2)
u8 lgd_lcd_command_for_mipi[][30] = {
	{ 0x01, 0x20, },
	{ 0x02, 0x35, 0x00, },
	{ 0x02, 0x3A, 0x77, },
	{ 0x03, 0xB2, 0x00, 0xC7, },
	{ 0x02, 0xB3, 0x02, },
	{ 0x08, 0xB5, 0x20, 0x10, 0x10, 0x00, 0x20, 0x00, 0x00, },
	{ 0x09, 0xB6, 0x00, 0x22, 0x02, 0x40, 0x20, 0x20, 0x05, 0x30, },
	{ 0x08, 0xB7, 0x52, 0x00, 0x10, 0x00, 0x0C, 0x00, 0x00, },
	{ 0x04, 0xC0, 0x00, 0x0B, 0x0F, },
	{ 0x03, 0xC2, 0x07, 0x00, },
	{ 0x06, 0xC3, 0x20, 0x67, 0x00, 0x08, 0x08, },

	{ 0x07, 0xC4, 0x01, 0x17, 0x00, 0x00, 0x33, 0x0A, },

	{ 0x06, 0xC5, 0x12, 0x32, 0x14, 0x14, 0x12, },
	{ 0x03, 0xC6, 0x22, 0x00, },

        { 0x06, 0xC7, 0x04, 0x1A, 0x1A, 0x6D, 0x00, },

	{ 0x03, 0xC8, 0x44, 0x63, },
	{ 0x06, 0xE0, 0X43, 0x40, 0x80, 0x00, 0x00, },

#if 1
        { 0x0A, 0xD0, 0x00, 0x01, 0x77, 0x06, 0x01, 0x12, 0x50, 0x11, 0x03, },
        { 0x0A, 0xD1, 0x00, 0x01, 0x77, 0x06, 0x01, 0x00, 0x50, 0x11, 0x03, },
        { 0x0A, 0xD2, 0x00, 0x01, 0x77, 0x06, 0x01, 0x12, 0x50, 0x11, 0x03, },
        { 0x0A, 0xD3, 0x00, 0x01, 0x77, 0x06, 0x01, 0x00, 0x50, 0x11, 0x03, },
        { 0x0A, 0xD4, 0x00, 0x01, 0x77, 0x06, 0x01, 0x12, 0x50, 0x11, 0x03, },
        { 0x0A, 0xD5, 0x00, 0x01, 0x77, 0x06, 0x01, 0x00, 0x50, 0x11, 0x03, },
#else
	{ 0x0A, 0xD0, 0x20, 0x06, 0x76, 0x06, 0x02, 0x18, 0x40, 0x11, 0x04, },
	{ 0x0A, 0xD1, 0x20, 0x06, 0x76, 0x06, 0x01, 0x00, 0x40, 0x11, 0x04, },
	{ 0x0A, 0xD2, 0x20, 0x06, 0x76, 0x06, 0x02, 0x18, 0x40, 0x11, 0x04, },
	{ 0x0A, 0xD3, 0x20, 0x06, 0x76, 0x06, 0x01, 0x00, 0x40, 0x11, 0x04, },
	{ 0x0A, 0xD4, 0x20, 0x06, 0x76, 0x06, 0x02, 0x18, 0x40, 0x11, 0x04, },
	{ 0x0A, 0xD5, 0x20, 0x06, 0x76, 0x06, 0x01, 0x00, 0x40, 0x11, 0x04, },
#endif

	{ END_OF_COMMAND, },
};
#endif /* CONFIG_LGE_CX2 */

#else
	u8 lcd_command_for_mipi[][30] = {
		{ 0x01, 0x20, },
		{ 0x02, 0x36,0x00, },
		{ 0x02, 0x3A,0x70, },
		{ 0x04, 0xB1, 0x06, 0x43, 0x0A, },
		{ 0x03, 0xB2, 0x00, 0xC8, },
#ifdef CONFIG_P2_P940
		{ 0x02, 0xB3, 0x02, },
		{ 0x02, 0xB4, 0x00, },
		{ 0x06, 0xB5, 0x40, 0x18, 0x02, 0x04, 0x10 , },
#else
		{ 0x02, 0xB3, 0x00, },
		{ 0x02, 0xB4, 0x04, },
		{ 0x06, 0xB5, 0x40, 0x18, 0x02, 0x00, 0x01, },
#endif
		{ 0x07, 0xB6, 0x0B, 0x0F, 0x02, 0x40, 0x10, 0xE8, },
		{ 0x06, 0xC3, 0x07, 0x0A, 0x0A, 0x0A, 0x02, },
		{ 0x07, 0xC4, 0x12, 0x24, 0x18, 0x18, 0x04, 0x49, },
#ifdef CONFIG_P2_P940
		{ 0x02,0xC5, 0x5B, },
#else
		{ 0x02,0xC5, 0x6B, },
#endif
		{ 0x04,0xC6, 0x41, 0x63, 0x03, },
#ifdef CONFIG_P2_P940
		{ 0x0A, 0xD0, 0x20, 0x07, 0x72, 0x02, 0x00, 0x00, 0x10, 0x00, 0x02, },
		{ 0x0A, 0xD1, 0x20, 0x07, 0x72, 0x02, 0x00, 0x00, 0x10, 0x00, 0x02, },
		{ 0x0A, 0xD2, 0x20, 0x07, 0x72, 0x02, 0x00, 0x00, 0x10, 0x00, 0x02, },
		{ 0x0A, 0xD3, 0x01, 0x07, 0x72, 0x02, 0x00, 0x00, 0x10, 0x00, 0x02, },
		{ 0x0A, 0xD4, 0x01, 0x07, 0x72, 0x02, 0x00, 0x00, 0x10, 0x00, 0x02, },
		{ 0x0A, 0xD5, 0x01, 0x07, 0x72, 0x02, 0x00, 0x00, 0x10, 0x00, 0x02, },
#else
		{ 0x0A, 0xD0, 0x33, 0x22, 0x77, 0x02, 0x00, 0x00, 0x30, 0x01, 0x01, },
		{ 0x0A, 0xD1, 0x33, 0x22, 0x77, 0x02, 0x00, 0x00, 0x30, 0x01, 0x01, },
		{ 0x0A, 0xD2, 0x33, 0x22, 0x77, 0x02, 0x00, 0x00, 0x30, 0x01, 0x01, },
		{ 0x0A, 0xD3, 0x33, 0x22, 0x77, 0x02, 0x00, 0x00, 0x30, 0x01, 0x01, },
		{ 0x0A, 0xD4, 0x33, 0x22, 0x77, 0x02, 0x00, 0x00, 0x30, 0x01, 0x01, },
		{ 0x0A, 0xD5, 0x33, 0x22, 0x77, 0x02, 0x00, 0x00, 0x30, 0x01, 0x01, },
#endif
		{ END_OF_COMMAND, },
	};
#endif

#if defined (CONFIG_LGE_P2)
	for (l = 0; hitachi_lcd_command_for_mipi[l][0] != END_OF_COMMAND; l++) {
		dsi_vc_dcs_write(1, TCH, &hitachi_lcd_command_for_mipi[l][1], hitachi_lcd_command_for_mipi[l][0]);
	}
	printf("HITACHI LCD is eanbled\n");
#elif defined (CONFIG_LGE_CX2)
	if (maker_id == 1) {
		for (l = 0; lgd_lcd_command_for_mipi[l][0] != END_OF_COMMAND; l++) {
			dsi_vc_dcs_write(1, TCH, &lgd_lcd_command_for_mipi[l][1], lgd_lcd_command_for_mipi[l][0]);
		}
		printf("LGD LCD is eanbled\n");
	} else {
		for (l = 0; hitachi_lcd_command_for_mipi[l][0] != END_OF_COMMAND; l++) {
			dsi_vc_dcs_write(1, TCH, &hitachi_lcd_command_for_mipi[l][1], hitachi_lcd_command_for_mipi[l][0]);
		}
		printf("HITACHI LCD is eanbled\n");
	}
#else
	for (l = 0; lcd_command_for_mipi[l][0] != END_OF_COMMAND; l++) {
		dsi_vc_dcs_write(1, TCH, &lcd_command_for_mipi[l][1], lcd_command_for_mipi[l][0]);
	}
#endif
	u8 buf;
	buf = DCS_SLEEP_OUT;
	dsi_vc_dcs_write(1,TCH, &buf, 1);

#ifdef CONFIG_FBCON
	fbcon_setup(&fb_cfg);
#endif
	initialized = 1;
}

void start_armboot (void)
{
	init_fnc_t **init_fnc_ptr;
	char *s;

	char down[1];
	char success[1];
#ifndef CFG_NO_FLASH
	ulong size = 0;
#endif
#if defined(CONFIG_VFD) || defined(CONFIG_LCD)
#error1
	unsigned long addr;
#endif

	gd = (gd_t*)(_armboot_start - CFG_MALLOC_LEN - sizeof(gd_t));
	
	__asm__ __volatile__("": : :"memory");

	memset ((void*)gd, 0, sizeof (gd_t));
	gd->bd = (bd_t*)((char*)gd - sizeof(bd_t));
	memset (gd->bd, 0, sizeof (bd_t));

	monitor_flash_len = _bss_start - _armboot_start;

	for (init_fnc_ptr = init_sequence; *init_fnc_ptr; ++init_fnc_ptr) {
		if ((*init_fnc_ptr)() != 0) {
			hang ();
		}
	}

#if 0
#ifndef CFG_NO_FLASH
	
#ifdef ENV_IS_VARIABLE
	if (is_flash) 
#endif
		size = flash_init ();
	display_flash_config (size);
#endif 

#ifdef CONFIG_VFD
#error2
#	ifndef PAGE_SIZE
#	  define PAGE_SIZE 4096
#	endif

	addr = (_bss_end + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
	size = vfd_setmem (addr);
	gd->fb_base = addr;
#endif 

#ifdef CONFIG_LCD
#error3
#	ifndef PAGE_SIZE
#	  define PAGE_SIZE 4096
#	endif

	addr = (_bss_end + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
	size = lcd_setmem (addr);
	gd->fb_base = addr;
#endif 

#if (CONFIG_COMMANDS & CFG_CMD_NAND)
#ifdef ENV_IS_VARIABLE
	if (is_nand) 
#endif
	{
		puts ("NAND:");
		nand_init();		
	}
#endif

#if (CONFIG_COMMANDS & CFG_CMD_ONENAND)
#ifdef ENV_IS_VARIABLE
	if (is_onenand)
#endif
		onenand_init();
#endif

#ifdef CONFIG_HAS_DATAFLASH
	AT91F_DataflashInit();
	dataflash_print_info();
#endif

#else
	
	mem_malloc_init (_armboot_start - CFG_MALLOC_LEN);
#endif

#if (CONFIG_COMMANDS & CFG_CMD_MMC)
	
	board_mmc_init();
#endif

	env_relocate ();

#if 0
#ifdef CONFIG_VFD
#error4
	
	drv_vfd_init();
#endif 

	gd->bd->bi_ip_addr = getenv_IPaddr ("ipaddr");

	{
		int i;
		ulong reg;
		char *s, *e;
		char tmp[64];

		i = getenv_r ("ethaddr", tmp, sizeof (tmp));
		s = (i > 0) ? tmp : NULL;

		for (reg = 0; reg < 6; ++reg) {
			gd->bd->bi_enetaddr[reg] = s ? simple_strtoul (s, &e, 16) : 0;
			if (s)
				s = (*e) ? e + 1 : e;
		}

#ifdef CONFIG_HAS_ETH1
		i = getenv_r ("eth1addr", tmp, sizeof (tmp));
		s = (i > 0) ? tmp : NULL;

		for (reg = 0; reg < 6; ++reg) {
			gd->bd->bi_enet1addr[reg] = s ? simple_strtoul (s, &e, 16) : 0;
			if (s)
				s = (*e) ? e + 1 : e;
		}
#endif
	}
#endif

	devices_init ();	

#ifdef CONFIG_CMC_PU2
	load_sernum_ethaddr ();
#endif 

	jumptable_init ();

	console_init_r ();	


	int count = 0;
	int charging_mode;
	char bufTmp[1];
	static u8 hw_cond = 0;
	extern u8 read_current_hw_condition(void);

	mmc_init(1);
	i2c_init(OMAP_I2C_STANDARD, TWL6030_CHIP_PM);
	select_bus(I2C1, OMAP_I2C_STANDARD);

	int isHardReset = 0;
#ifdef CONFIG_LGE_NVDATA
	do{
		hw_cond = read_current_hw_condition();
		if(!(hw_cond & STS_PWRON) && isVolumeDownKeyPressed()){
			mdelay(600);
			if(++count >= 10){
				bufTmp[0] = 'H';
				lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_HARD_RESET_OFFSET,&bufTmp,1);

				isHardReset = 1;
				printf("###### HARD RESET\n");
				break;
			}
		}
		else
			break;
	}while(1);
#endif
	
	int isFactoryReset = 0;
#ifdef CONFIG_LGE_NVDATA
	lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_FACTORY_RESET_STATUS_OFFSET,&bufTmp,1);
	if(bufTmp[0] == 'R'){
#ifdef CONFIG_COSMO_SU760
		printf("###### FACTORY RESET\n");
		lge_static_nvdata_emmc_read(LGE_NVDATA_STATIC_DOWNLOAD_FINISH,&success,1);
		printf("\n\n success flag == %c \n\n",success[0]);
		
		if(success[0] == 'S')
		{
			down[0]='F';
			success[0]='F';
			lge_static_nvdata_emmc_write(LGE_NVDATA_STATIC_DOWNLOAD_FINISH,&success,1);
			
			lge_static_nvdata_emmc_write(LGE_NVDATA_STATIC_SECOND,&down,1);
			lge_static_nvdata_emmc_read(LGE_NVDATA_STATIC_SECOND,&down,1);
			printf("\n\n first flag == %c \n\n",down[0]);
		}
#endif		
		isFactoryReset = 1;
	}

	lge_static_nvdata_emmc_read(LGE_NVDATA_STATIC_FRSTSTATUS_OFFSET,&bufTmp,1);
	if(bufTmp[0] == '1' || bufTmp[0] == '3' || bufTmp[0] == '4' ){
		lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_FACTORY_RESET_STATUS_OFFSET,&bufTmp,1);
		if(bufTmp[0] != '0'){
			printf("###### FACTORY RESET: LGE_NVDATA_STATIC_FRSTSTATUS_OFFSET: %c which need to do factory reset\n", bufTmp[0]);
			isFactoryReset = 1;
		}
		
		lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_FRSTSTATUS3_OFFSET,&bufTmp,1);
		if(bufTmp[0] = '1'){
			printf("###### FACTORY RESET: <frststatus = 3>, so jump to recovery mode\n");
			isFactoryReset = 1;
		}
	}
#endif /* CONFIG_LGE_NVDATA */

	extern int	recoverykey(void); 
	int i;
	if(recoverykey() || isFactoryReset || isHardReset) {
		printf("\n### ==============fota mode ==============###\n"); 
#if defined (CONFIG_LGE_P2)
		setenv("bootcmd", "mmcinit 1; mmc 1 read 0x3c00 0x81000000 0xA00000; booti 0x81000000");
#elif defined (CONFIG_LGE_CX2)
#if defined (CONFIG_CX2_SU870)
		setenv("bootcmd", "mmcinit 1; mmc 1 read 0x5800 0x81000000 0xA00000; bootm 81000000");
#else
		setenv("bootcmd", "mmcinit 1; mmc 1 read 0x8000 0x81000000 0xA00000; bootm 81000000");
#endif /* CONFIG_CX2_SU870 */
#else /* CONFIG_LGE_CX2 */
		setenv("bootcmd", "mmcinit 1; mmc 1 read 0x5800 0x81000000 0xA00000; bootm 81000000");
#endif
#if defined (CONFIG_DDR_1GB)
		setenv("bootargs", "mem=463M@0x80000000 mem=512M@0xA0000000 vmalloc=180m console=ttyO3,115200n8 init=/init vram=10M omapfb.vram=\"0:4M\"");
#else
		setenv("bootargs", "root=/dev/ram0 rw mem=463M@0x80000000 console=ttyO3,115200n8 init=/init vram=10M omapfb.vram=\"0:4M\"");
#endif
		printf("\n### ==============fota mode ==============###\n");
	}

#endif

#if 0
	/* for recovery */
	setenv("bootcmd", "mmcinit 1; mmc 1 read 0x3c00 0x81000000 0xA00000; bootm 0x81000000");
	setenv("bootargs",
		"mem=452M@0x80000000 mem=512M@0xA0000000 vmalloc=256M console=ttyO3,115200n8 "
		"init=/init vram=10M,0x87000000 omapfb.vram=\"0:4M@0x87000000\"");
#endif

	extern int muic_init(int);
	extern int dload_mode;
	extern int cable_910K_detect;
#ifdef CONFIG_COSMO_SU760
	extern int cable_56K_detect;
	extern int cable_open_usb_detect;
#endif

	int muic_mode= muic_init(isFactoryReset);
	chr_ic_init(muic_mode);
	charging_mode = twl6030_init_battery_charging(muic_mode);

	printf("\nmuic mode = %d \n",muic_mode);
	printf("dload_mode = %d \n",dload_mode);
	printf("cable_910K = %d \n",cable_910K_detect);

#ifdef CONFIG_COSMO_SU760	
	printf("cable_56K = %d \n",cable_56K_detect);	
	printf("cable_open_usb = %d \n",cable_open_usb_detect);	
#endif

	extern	int		downloadkey();

#ifdef CONFIG_COSMO_SU760
	extern int go_usbdownload (void);
#else
	extern int go_usbdownload (int);
#endif

	extern void dload_serial_init(void);

#if defined(CONFIG_LGE_WEB_DOWNLOAD) && defined(CONFIG_LGE_NVDATA)
	/* support LGE_WEB_DOWNLOAD */
	web_down_flag1 = 0x00;
	web_down_flag2 = 0x00;
	lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_WED_DOWNLOAD_OFFSET1,&web_down_flag1,1);
	lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_WED_DOWNLOAD_OFFSET2,&web_down_flag2,1);

	if( (web_down_flag1 == LGE_NVDATA_RESET_CAUSE_WEB_DOWNLOAD_RESET1) ||
		(web_down_flag2 == LGE_NVDATA_RESET_CAUSE_WEB_DOWNLOAD_RESET2) )
	{
	        web_download_mode = 1;               

		if(web_down_flag1!=0x00)
		{
			web_down_flag1 = 0x00;
			lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_WED_DOWNLOAD_OFFSET1,&web_down_flag1,1);
		}
	} else {
		web_download_mode = 0;
	}
#endif

	if( muic_mode != 1 )
	{
#ifdef CONFIG_COSMO_SU760
		lge_static_nvdata_emmc_read(LGE_NVDATA_STATIC_SECOND,&down,1);
		printf("\n\n pre if flag == %c \n\n\n\n\n\n \n\n",down[0]);

		if(down[0] =='F'  && cable_910K_detect == 1 ){
			down[0] = 'S';
			lge_static_nvdata_emmc_write(LGE_NVDATA_STATIC_SECOND,&down,1);
		}
		else if(down[0] == 'S'&& cable_910K_detect == 1){
			down[0] = 'T';
			lge_static_nvdata_emmc_write(LGE_NVDATA_STATIC_SECOND,&down,1);
		}
#ifndef CONFIG_COSMO_SU760
		else if(down[0] == 'T'&& cable_910K_detect == 1){
			down[0] = 'N';
			lge_static_nvdata_emmc_write(LGE_NVDATA_STATIC_SECOND,&down,1);
		}
#endif
		else
#endif

#ifdef CONFIG_LGE_WEB_DOWNLOAD
#ifdef CONFIG_COSMO_SU760
		if ((downloadkey() && (cable_open_usb_detect || cable_56K_detect)) || ( dload_mode != 1 && cable_910K_detect == 1) || (web_download_mode == 1) ) {
			download_logo(1);
			success[0]='F';
			lge_static_nvdata_emmc_write(LGE_NVDATA_STATIC_DOWNLOAD_FINISH,&success,1);
#else
		if (downloadkey() || ( dload_mode!=1&&cable_910K_detect==1) || (web_download_mode==1) ) {
#endif
			download_logo(0);

			if((web_download_mode==1)&&(!downloadkey())) {
				if(muic_mode == 4)
				{
#ifndef CONFIG_COSMO_SU760
					download_logo(0);
#endif
					start_vib();
				}
				else
				{
#ifndef CONFIG_COSMO_SU760
					download_logo(0);
				
					download_logo(2);
#endif
					muic_for_download(0);
					muic_for_download(1);
					udelay(5000000);
					write_dev_on_off(0x47);
					udelay(1000000);

					disable_interrupts ();

					reset_cpu (0);
				}
			}
#ifndef CONFIG_COSMO_SU760
			if ((web_download_mode==1)&&(downloadkey()) ) {
#ifdef CONFIG_LGE_NVDATA
				web_down_flag2 = 0x00;
				lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_WED_DOWNLOAD_OFFSET2,&web_down_flag2,1);
#endif

				udelay(1000000);
				write_dev_on_off(0x47);
				udelay(1000000);

				disable_interrupts ();

				reset_cpu (0);
			}
#endif

			int usb = 0;
			DownloadValid = 1;
			dload_serial_init();

#ifdef CONFIG_COSMO_SU760
			usb = go_usbdownload();
#else
			if (cable_910K_detect == 1)
				usb = go_usbdownload(1);
			else
				usb = go_usbdownload(0);
#endif		
			printf("%d \n ",usb);

			if(usb !=1)
				for (;;);
		}
#endif
	}

	check_battery_temp_for_safe_charging_and_quick_start(charging_mode, muic_mode);

	force_i2c3_stop_condition();
	twl6030_set_vt_cam_ldo();
	vt_cam_ready();

#if defined(CONFIG_MISC_INIT_R)

	misc_init_r (muic_mode);
	
#endif

	enable_interrupts ();

#ifdef CONFIG_DRIVER_CS8900
	cs8900_get_enetaddr (gd->bd->bi_enetaddr);
#endif

	if ((s = getenv ("loadaddr")) != NULL) {
		load_addr = simple_strtoul (s, NULL, 16);
	}
#if (CONFIG_COMMANDS & CFG_CMD_NET)
	if ((s = getenv ("bootfile")) != NULL) {
		copy_filename (BootFile, s, sizeof (BootFile));
	}
#endif

#ifdef BOARD_LATE_INIT
	board_late_init ();
#endif
#if (CONFIG_COMMANDS & CFG_CMD_NET)
#if defined(CONFIG_NET_MULTI)
	puts ("Net:   ");
#endif
	eth_initialize(gd->bd);
#endif

	init_panel(0);

#ifdef CONFIG_LGE_TRAP
	char trap_flag;
	char post_trap_flag = 0;

	lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_RESET_CAUSE_OFFSET,&trap_flag,1);
	if(trap_flag != 0x00 && trap_flag != LGE_NVDATA_RESET_CAUSE_VAL_USER_RESET)
	{
		if(trap_flag == LGE_NVDATA_RESET_CAUSE_VAL_AP_CRASH)
		{
			post_trap_flag = trap_flag;
			cosmo_panel_ap_trap_draw();
		}
		else if(trap_flag == LGE_NVDATA_RESET_CAUSE_VAL_CP_CRASH)
		{	
			post_trap_flag = trap_flag;
			cosmo_panel_cp_trap_draw();
		}		
		else
		{
			printf("What happened!!! the data is not expected value = %d\n",trap_flag);
			cosmo_panel_logo_draw();
		}		
		lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_RESET_CAUSE_BACKUP_OFFSET,&trap_flag,1);

		trap_flag = 0x00;

#if 0 

		char dump_flag;
		lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_CRASH_DUMP_OFFSET,&dump_flag,1);		

		if(dump_flag == 0x00)
			dump_flag = LGE_NDATA_CRASH_DUMP_ENABLE_VALUE;		

		lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_CRASH_DUMP_OFFSET,&dump_flag,1);				

		lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_AP_CRASH_DUMP_OFFSET,&dump_flag,1);		

		if(dump_flag == 0x00)
			dump_flag = LGE_NDATA_CRASH_DUMP_ENABLE_VALUE;		

		lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_AP_CRASH_DUMP_OFFSET,&dump_flag,1);	
#endif
		
		lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_RESET_CAUSE_OFFSET,&trap_flag,1);
	}
	else
	{

#if 0 

		char dump_flag;
		lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_CRASH_DUMP_OFFSET,&dump_flag,1);		

		if( dump_flag == LGE_NDATA_CRASH_DUMP_INITIAL_VALUE)
			dump_flag = LGE_NDATA_CRASH_DUMP_ENABLE_VALUE;		

		lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_CRASH_DUMP_OFFSET,&dump_flag,1);		

		lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_AP_CRASH_DUMP_OFFSET,&dump_flag,1);		

		if(dump_flag == LGE_NDATA_CRASH_DUMP_INITIAL_VALUE)
			dump_flag = LGE_NDATA_CRASH_DUMP_ENABLE_VALUE;		

		lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_AP_CRASH_DUMP_OFFSET,&dump_flag,1);	
#endif
		cosmo_panel_logo_draw();
	}
#else
	cosmo_panel_logo_draw();
#endif

	configure_dispc();

	u8 buf = DCS_DISPLAY_ON;
	dsi_vc_dcs_write(1,TCH, &buf, 1);
	printf("DISPLAY ON\n");

	dsi_enable_video_mode();	
	dispc_go(OMAP_DSS_CHANNEL_LCD2);

	dispc_enable_lcd_out(1);

#ifdef CONFIG_LGE_TRAP
	extern int trap_exit_key(void);
	if(post_trap_flag != 0x00)
	{

		if(post_trap_flag == LGE_NVDATA_RESET_CAUSE_VAL_AP_CRASH)
		{
			int trap_key_pressed = 0;
			
			do
			{
				trap_key_pressed = trap_exit_key();			
				cosmo_panel_ap_trap_draw();
				mdelay(100);

			}
			while(trap_key_pressed == 0);
		}
		else if(post_trap_flag == LGE_NVDATA_RESET_CAUSE_VAL_CP_CRASH)
		{
			int trap_key_pressed = 0;
			
			do
			{
				trap_key_pressed = trap_exit_key(); 		
				cosmo_panel_cp_trap_draw();
				mdelay(100);

			}
			while(trap_key_pressed == 0);
		}
	}
#endif /* CONFIG_LGE_TRAP */

	vt_cam_standby();

#ifdef CONFIG_COSMO_SU760
	ifx_boot_start();
#else
	ifx_start_low();
#endif
	ifx_uart_sw_ctrl(1);

	for (;;) {
		main_loop ();
	}

}

void write_number_of_dload(unsigned char x )
{
	unsigned char * sect = (unsigned char *) malloc(512);

	mmc_read(1, 0x202, sect, 512);
	sect[274] = x;	
	mmc_write(1, sect, 0x202, 512);
	printf("number of dload = 1\n");
	free(sect);
}

int read_number_of_dload(void)
{
	int ret = 0;
	unsigned char * sect = (unsigned char *) malloc(512);
	mmc_read(1, 0x202, sect, 512);

	if(sect[274] == 'R')
		ret = 1;
	else
		ret = 0;
	free(sect);

	return ret;
}

void hang (void)
{
	puts ("### ERROR ### Please RESET the board ###\n");
	for (;;);
}

void download_logo(int download_mode)
{
	enable_interrupts ();

	init_panel(0);

#ifdef CONFIG_COSMO_SU760
	cosmo_panel_emergency_logo_draw();
#else
	if(download_mode==0)
		cosmo_panel_emergency_logo_draw();
	else
		cosmo_panel_download_logo_draw(download_mode);
#endif
	configure_dispc();

	u8 buf = DCS_DISPLAY_ON;
	dsi_vc_dcs_write(1,TCH, &buf, 1);
	printf("DISPLAY ON\n");

	dsi_enable_video_mode();	
	dispc_go(OMAP_DSS_CHANNEL_LCD2);

	dispc_enable_lcd_out(1);

}

#ifdef CONFIG_MODEM_SUPPORT
static inline void mdm_readline(char *buf, int bufsiz);

extern void  dbg(const char *fmt, ...);
int mdm_init (void)
{
	char env_str[16];
	char *init_str;
	int i;
	extern char console_buffer[];
	extern void enable_putc(void);
	extern int hwflow_onoff(int);

	enable_putc(); 

#ifdef CONFIG_HWFLOW
	init_str = getenv("mdm_flow_control");
	if (init_str && (strcmp(init_str, "rts/cts") == 0))
		hwflow_onoff (1);
	else
		hwflow_onoff(-1);
#endif

	for (i = 1;;i++) {
		sprintf(env_str, "mdm_init%d", i);
		if ((init_str = getenv(env_str)) != NULL) {
			serial_puts(init_str);
			serial_puts("\n");
			for(;;) {
				mdm_readline(console_buffer, CFG_CBSIZE);
				dbg("ini%d: [%s]", i, console_buffer);

				if ((strcmp(console_buffer, "OK") == 0) ||
						(strcmp(console_buffer, "ERROR") == 0)) {
					dbg("ini%d: cmd done", i);
					break;
				} else 
					if (strncmp(console_buffer, "CONNECT", 7) == 0) {
						dbg("ini%d: connect", i);
						return 0;
					}
			}
		} else
			break; 

		udelay(100000);
	}

	udelay(100000);

	for(;i > 1;) { 

		mdm_readline(console_buffer, CFG_CBSIZE);
		dbg("ini_f: [%s]", console_buffer);
		if (strncmp(console_buffer, "CONNECT", 7) == 0) {
			dbg("ini_f: connected");
			return 0;
		}
	}

	return 0;
}

static inline void mdm_readline(char *buf, int bufsiz)
{
	char c;
	char *p;
	int n;

	n = 0;
	p = buf;
	for(;;) {
		c = serial_getc();

		switch(c) {
			case '\r':
				break;
			case '\n':
				*p = '\0';
				return;

			default:
				if(n++ > bufsiz) {
					*p = '\0';
					return; 
				}
				*p = c;
				p++;
				break;
		}
	}
}
#endif
