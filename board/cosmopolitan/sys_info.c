#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/io.h>
#include <asm/arch/bits.h>
#include <asm/arch/mem.h>	
#include <asm/arch/sys_proto.h>
#include <asm/arch/sys_info.h>
#include <i2c.h>

inline u16 check_fpga_rev(void)
{
	return __raw_readw(FPGA_REV_REGISTER);
}

int check_uieeprom_avail(void)
{
	volatile unsigned short *ui_brd_name =
	    (volatile unsigned short *)EEPROM_UI_BRD + 8;
	int count = 1000;

	while ((*ui_brd_name == 0x00) && count) {
		udelay(200);
		count--;
	}
	
	return count;
}

u32 get_gpmc0_type(void)
{
	
#ifdef CONFIG_NOR_BOOT
	return 0x1;
#else
	return 0x0;
#endif
}

u32 get_cpu_type(void)
{
    
    return CPU_4430;
}

unsigned int cortex_a9_rev(void)
{

	unsigned int i;

	asm ("mrc p15, 0, %0, c0, c0, 0" : "=r" (i));

	return i;
}

u32 get_cpu_rev(void)
{
	u32 omap_rev_reg = 0;
	u32 idcode = 0;

	idcode = cortex_a9_rev();
	if (((idcode >> 4) & 0xfff) == 0xc09) {
		idcode &= 0xf;
		switch (idcode) {
		case 1:
			return CPU_4430_ES1;
		case 2:
			omap_rev_reg = (__raw_readl(CONTROL_ID_CODE)  >> 28);
			if (omap_rev_reg == 0x3)
				return CPU_4430_ES21;
			else if (omap_rev_reg >= 0x4)
				return CPU_4430_ES22;
			else
				return CPU_4430_ES20;
		}
	}
	return CPU_4430_ES22;

}

u32 is_mem_sdr(void)
{
	volatile u32 *burst = (volatile u32 *)(SDRC_MR_0 + 0x0);
	if (*burst == 0x00000031)
		return 1;
	return 0;
}

u32 get_mem_type(void)
{
	
	return DDR_DISCRETE;
}

u32 get_sdr_cs_size(u32 offset)
{
	u32 size;

	size = __raw_readl(SDRC_MCFG_0 + offset) >> 8;
	size &= 0x3FF;		
	size *= SZ_2M;		
	return size;
}

u32 get_board_type(void)
{
#ifdef CONFIG_4430SDP
	return SDP_4430_VIRTIO;
#else
	if (get_cpu_rev() == CPU_4430_ES1)
		return SDP_4430_V1;
	else
		return 0;
#endif
}

inline u32 get_sysboot_value(void)
{
	return 0x0000003F & __raw_readl(CONTROL_STATUS);
}

u32 get_gpmc0_base(void)
{
	u32 b;

	b = __raw_readl(GPMC_CONFIG_CS0 + GPMC_CONFIG7);
	b &= 0x1F;		
	b = b << 24;		
	return b;
}

u32 get_gpmc0_width(void)
{
	return WIDTH_16BIT;
}

u32 get_board_rev(void)
{

	 
	extern int lge_pcb_ver;
	return (lge_pcb_ver);

#if !(defined(CONFIG_4430SDP) || defined(CONFIG_COSMO))
	if (!check_uieeprom_avail()) {

		return 0x01;
	}
	
	ui_brd_name += 0x08;
	count = sizeof(enhanced_ui_brd_name) - 2;
	while ((enhanced_ui_brd_name[count] == ui_brd_name[count]) && count)
		count--;

	if (!count) {
		
		return 0x11;
	}
#endif
	
	return 0x10;
}

void display_board_info(u32 btype)
{
	char *bootmode[] = {
		"NAND",
		"NOR",
		"ONND",
		"P2a",
		"NOR",
		"NOR",
		"P2a",
		"P2b",
	};
	u32 brev = get_board_rev();
	char cpu_4430s[] = "4430";
	char db_ver[] = "0.0";	 
	char mem_sdr[] = "mSDR"; 
	char mem_ddr[] = "mDDR";
	char t_tst[] = "TST";	 
	char t_emu[] = "EMU";
	char t_hs[] = "HS";
	char t_gp[] = "GP";
	char unk[] = "?";
#ifdef CONFIG_LED_INFO
	char led_string[CONFIG_LED_LEN] = { 0 };
#endif

#if defined(L3_165MHZ)
	char p_l3[] = "165";
#elif defined(L3_110MHZ)
	char p_l3[] = "110";
#elif defined(L3_133MHZ)
	char p_l3[] = "133";
#elif defined(L3_100MHZ)
	char p_l3[] = "100"
#endif

#if defined(PRCM_PCLK_OPP1)
	char p_cpu[] = "1";
#elif defined(PRCM_PCLK_OPP2)
	char p_cpu[] = "2";
#elif defined(PRCM_PCLK_OPP3)
	char p_cpu[] = "3";
#elif defined(PRCM_PCLK_OPP4)
	char p_cpu[] = "4"
#endif
	char *cpu_s, *db_s, *mem_s, *sec_s;
	u32 cpu, rev, sec;

	rev = get_cpu_rev();
	cpu = get_cpu_type();
	sec = get_device_type();

#ifndef CONFIG_4430SDP
	if (is_mem_sdr())
		mem_s = mem_sdr;
	else
		mem_s = mem_ddr;
#else
	mem_s = mem_ddr;
#endif

	cpu_s = cpu_4430s;

	db_s = db_ver;
	db_s[0] += (brev >> 4) & 0xF;
	db_s[2] += brev & 0xF;

	switch (sec) {
	case TST_DEVICE:
		sec_s = t_tst;
		break;
	case EMU_DEVICE:
		sec_s = t_emu;
		break;
	case HS_DEVICE:
		sec_s = t_hs;
		break;
	case GP_DEVICE:
		sec_s = t_gp;
		break;
	default:
		sec_s = unk;
	}

	printf("OMAP%s-%s rev %d, CPU-OPP%s L3-%sMHz\n", cpu_s, sec_s, rev,
		p_cpu, p_l3);
	printf("TI 4430SDP %s Version + %s (Boot %s)\n", db_s,
		mem_s, bootmode[get_gpmc0_type()]);
#ifdef CONFIG_LED_INFO

	sprintf(led_string, "%5s%3s%3s %4s", cpu_s, sec_s, p_l3,
		bootmode[get_gpmc0_type()]);
	
	for (sec = 0; sec < CONFIG_LED_LEN; sec += 2) {
		
		u16 val = led_string[sec] << 8;
		val |= led_string[sec + 1];
		__raw_writew(val, LED_REGISTER + sec);
	}
#endif

}

u32 get_base(void)
{
	u32 val;
	__asm__ __volatile__("mov %0, pc \n" : "=r"(val) : : "memory");
	val &= 0xF0000000;
	val >>= 28;
	return val;
}

u32 running_in_flash(void)
{
	if (get_base() < 4)
		return 1;	
	return 0;		
}

u32 running_in_sram(void)
{
	if (get_base() == 4)
		return 1;	
	return 0;		
}

u32 running_in_sdram(void)
{
	if (get_base() > 4)
		return 1;	
	return 0;		
}

u32 get_boot_type(void)
{
	u32 v;

    v = get_sysboot_value() & (BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
	return v;
}

u32 get_device_type(void)
{
	int mode;

	mode = __raw_readl(SECURE_ID_CODE) & (DEVICE_MASK);
	return mode >>= 8;
}
