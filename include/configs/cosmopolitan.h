#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_ARMCORTEXA9	1    
#define CONFIG_OMAP		1    
#define CONFIG_OMAP44XX		1    
#define CONFIG_OMAP4430		1    
#define CONFIG_4430SDP		1    
#define CONFIG_FASTBOOT		1    

#define BOARD_LATE_INIT		1

#define CONFIG_COSMO            1

#define CONFIG_COSMO_BOOT_IMG	1
#define CONFIG_ARCH_OMAP4	1
#define CONFIG_PANEL_COSMO	1 	

#define CONFIG_OFF_PADCONF	1    

#include <asm/arch/cpu.h>        

#define V_OSCK                   38400000  

#define V_SCLK                   V_OSCK

#define PRCM_CLK_CFG2_332MHZ     1    
#define PRCM_PCLK_OPP2           1    

#undef CONFIG_USE_IRQ                 
#define CONFIG_MISC_INIT_R

#define CONFIG_CMDLINE_TAG       1    
#define CONFIG_SETUP_MEMORY_TAGS 1
#define CONFIG_INITRD_TAG        1
#define CONFIG_REVISION_TAG      1

#define CONFIG_BOOT_DEVICE_EMMC	 1 

#define CONFIG_COSMOPOLITAN      1

#define CONFIG_FBCON             1
#define CONFIG_LGE_WEB_DOWNLOAD  1
//#define CONFIG_LGE_FOTA_FEATURE  1
#define CONFIG_LGE_NVDATA        1
 
#if defined(CONFIG_BOOT_DEVICE_EMMC)
#define CFG_ENV_SIZE             SZ_128K    

#else
#define CFG_ENV_SIZE             SZ_128K    
#endif

#define CFG_NO_FLASH

#define CFG_MALLOC_LEN           (CFG_ENV_SIZE + SZ_128K)
#define CFG_GBL_DATA_SIZE        128  

#define CONFIG_RT9524_CHARGER	1

#define V_NS16550_CLK            (48000000)  

#if 1	

#define CFG_NS16550_COM1         OMAP44XX_UART1
#endif	

#define CFG_NS16550
#define CFG_NS16550_SERIAL
#define CFG_NS16550_REG_SIZE     (-4)
#define CFG_NS16550_CLK          V_NS16550_CLK
#define CFG_NS16550_COM3         OMAP44XX_UART3
#define CFG_NS16550_COM4         OMAP44XX_UART4

#define AP_CP_UART 1     

#define CONFIG_SERIAL1           1    

#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX        4

#define CONFIG_BAUDRATE          115200
#define CFG_BAUDRATE_TABLE       {4800, 9600, 19200, 38400, 57600, 115200}

#define CONFIG_MMC              1
#define CONFIG_DOS_PARTITION    1

#define NET_CMDS                 (CFG_CMD_DHCP|CFG_CMD_NFS|CFG_CMD_NET)

#if defined(CONFIG_BOOT_DEVICE_EMMC)
#define CONFIG_MMC              1
#define CFG_MMC_BASE            0xF0000000

#define CONFIG_COMMANDS          ((CFG_CMD_I2C | CONFIG_CMD_DFL |  \
	CFG_CMD_MMC) & \
	~(CFG_CMD_FLASH | CFG_CMD_AUTOSCRIPT | CFG_CMD_NET | CFG_CMD_IMLS | CFG_CMD_ECHO | CFG_CMD_NAND | CFG_CMD_ONENAND))

#else

#define CONFIG_COMMANDS          ((CFG_CMD_I2C | CONFIG_CMD_DFL | NET_CMDS | \
	CFG_CMD_JFFS2 | CFG_CMD_MMC | CFG_CMD_FAT) &			\
	(~CFG_CMD_AUTOSCRIPT | CFG_CMD_NAND | CFG_CMD_ONENAND)) 
#endif

#define CFG_I2C_SPEED            100
#define CFG_I2C_SLAVE            1
#define CFG_I2C_BUS              0
#define CFG_I2C_BUS_SELECT       1
#define CONFIG_DRIVER_OMAP44XX_I2C 1
#define CONFIG_TWL6030		 1

#define CONFIG_BOOTP_MASK        CONFIG_BOOTP_DEFAULT

#include <cmd_confdefs.h>

#if (CONFIG_COMMANDS & CFG_CMD_NET)
#define CONFIG_MICREL_ETH_KS8851 1
#define CONFIG_CMD_PING 1
#define CONFIG_NET_MULTI 1
#define KS8851_SNL 1

#endif

#define NAND_MAX_CHIPS           1

#if defined(CONFIG_4430ZEBU) && !defined(CONFIG_4430ES2)

#define CONFIG_BOOTDELAY         0
#define CONFIG_EXTRA_ENV_SETTINGS \
	"bootcmd=go 0x80008000\0"
#else
#define CONFIG_BOOTDELAY         0

#if 0
#define CONFIG_EXTRA_ENV_SETTINGS \
	"bootcmd=mmcinit 1;mmc 1 read 0x800 0x81000000 0x500000; bootm 81000000 loadaddr=81000000\0"
#else	
#define CONFIG_EXTRA_ENV_SETTINGS \
	"autostart=yes\0" \
	"usbtty=cdc_acm\0" \
	"stdout=serial,usbtty\0" \
	"stdin=serial,usbtty\0" \
	"stderr=serial,usbtty\0" \
	"verify=n\0" \
	""
#endif	

#endif 

#if defined(CONFIG_BOOT_DEVICE_EMMC)
#define EMMC_KERNEL_BASE_SEC		0x00000400
#define EMMC_INITRD_BASE_SEC		0x00002400
#define EMMC_RECOVERY_BASE_SEC		0x00004C00

#ifdef CONFIG_LGE_FOTA_FEATURE
#define CONFIG_BOOTCOMMAND	"mmcinit 1; mmc 1 loadimage; bootm 81000000\0"
#else
#define CONFIG_BOOTCOMMAND	"mmcinit 1; booti mmc1 boot\0"
#endif

#endif 

#define CONFIG_OMAP4_ANDROID_CMD_LINE 1

#define CONFIG_CONSOLEARGS " console=ttyO3,115200n8"

#if 0
#define CONFIG_BOOTARGS "mem=64M console=ttyS0,115200n8 noinitrd \
	root=/dev/nfs rw nfsroot=128.247.77.158:/home/a0384864/wtbu/rootfs \
	ip=dhcp"
#else

#if 0
#define CONFIG_BOOTARGS "root=/dev/ram0 rw mem=463M@0x80000000 \
init=/init vram=10M,0x87000000 omapfb.vram=\"0:3000K@0x87000000\""
#endif
#if 0 /* GB */
#define CONFIG_BOOTARGS "mem=463M@0x80000000 \
init=/init vram=10M,0x87000000 omapfb.vram=\"0:3000K@0x87000000\""
#else
#define CONFIG_BOOTARGS "mem=452M@0x80000000 \
init=/init vram=10M,0x87000000 omapfb.vram=\"0:4M@0x87000000\""
#endif
#endif

#define CONFIG_NETMASK           255.255.254.0
#define CONFIG_IPADDR            128.247.77.90
#define CONFIG_SERVERIP          128.247.77.158
#define CONFIG_BOOTFILE          "uImage"
#define CONFIG_AUTO_COMPLETE     1

#define V_PROMPT                 "LGE_P920 # "

#define CFG_LONGHELP             
#define CFG_PROMPT               V_PROMPT
#define CFG_CBSIZE               256  

#define CFG_PBSIZE               (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)
#define CFG_MAXARGS              16          
#define CFG_BARGSIZE             CFG_CBSIZE  

#define CFG_MEMTEST_START        (OMAP44XX_SDRC_CS0)  
#define CFG_MEMTEST_END          (OMAP44XX_SDRC_CS0+SZ_31M)

#undef	CFG_CLKS_IN_HZ           

#define CFG_LOAD_ADDR            (OMAP44XX_SDRC_CS0) 

#define V_PVT                    7

#define CFG_TIMERBASE            OMAP44XX_GPT2
#define CFG_PVT                  V_PVT  
#define CFG_HZ                   ((V_SCLK)/(2 << CFG_PVT))

#define CONFIG_STACKSIZE	SZ_128K 
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	SZ_4K   
#define CONFIG_STACKSIZE_FIQ	SZ_4K   
#endif

#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1		0x80000000
#define PHYS_SDRAM_1_SIZE	SZ_512M

#define SDRC_R_B_C		1

#define PISMO1_NOR_SIZE_SDPV2	GPMC_SIZE_128M
#define PISMO1_NOR_SIZE		GPMC_SIZE_64M

#define PISMO1_NAND_SIZE	GPMC_SIZE_128M
#define PISMO1_ONEN_SIZE	GPMC_SIZE_128M
#define DBG_MPDB_SIZE		GPMC_SIZE_16M
#define PISMO2_SIZE		0

#define CFG_MAX_FLASH_SECT	(520)	
#define CFG_MAX_FLASH_BANKS      2		
#define CFG_MONITOR_LEN		SZ_256K 	

#define PHYS_FLASH_SIZE_SDPV2	SZ_128M
#define PHYS_FLASH_SIZE		SZ_32M

#ifdef OMAP4_NAND_NOR_ONENAND
#define CFG_FLASH_BASE		boot_flash_base
#define PHYS_FLASH_SECT_SIZE	boot_flash_sec

#define CFG_FLASH_BANKS_LIST	{0, 0}

#define CFG_MONITOR_BASE	CFG_FLASH_BASE 
#define CFG_ONENAND_BASE	ONENAND_MAP

#if defined(CONFIG_BOOT_DEVICE_EMMC)
#define CFG_ENV_IS_NOWHERE	1
#else
#define CFG_ENV_IS_IN_NAND	1
#define CFG_ENV_IS_IN_ONENAND	1
#define CFG_ENV_IS_IN_FLASH	1
#define ONENAND_ENV_OFFSET	0xc0000 
#define SMNAND_ENV_OFFSET	0xc0000 
#endif

#define CFG_ENV_SECT_SIZE	boot_flash_sec
#define CFG_ENV_OFFSET		boot_flash_off
#define CFG_ENV_ADDR		boot_flash_env_addr
#endif

#define CFG_MONITOR_BASE	CFG_FLASH_BASE 
#define CFG_ONENAND_BASE	ONENAND_MAP

#define CFG_ENV_IS_IN_EMMC	1
#define CFG_FLASH_BASE		0x0
#define CFG_ENV_SECT_SIZE	SZ_128K
#define CFG_ENV_OFFSET		0x700
#define CFG_ENV_ADDR		0x700

#define ENV_IS_VARIABLE		1

#define CFG_FASTBOOT_TRANSFER_BUFFER (PHYS_SDRAM_1 + SZ_16M)
#define CFG_FASTBOOT_TRANSFER_BUFFER_SIZE (SZ_512M - SZ_16M)

#ifndef CONFIG_4430ZEBU

#if 0
#define CFG_FLASH_CFI		1    
#define CFG_FLASH_CFI_DRIVER	1    
#define CFG_FLASH_USE_BUFFER_WRITE 1    
#define CFG_FLASH_PROTECTION	1    
#define CFG_FLASH_QUIET_TEST	1    
#define CFG_FLASH_CFI_WIDTH	0x02

#define CFG_FLASH_ERASE_TOUT	(100*CFG_HZ) 
#define CFG_FLASH_WRITE_TOUT	(100*CFG_HZ) 

#define CFG_MAX_MTD_BANKS	(CFG_MAX_FLASH_BANKS+CFG_MAX_NAND_DEVICE)
#define CFG_JFFS2_MEM_NAND
#define CFG_JFFS2_FIRST_BANK	CFG_MAX_FLASH_BANKS 
#define CFG_JFFS2_NUM_BANKS	1
#endif

#define ENV_IS_VARIABLE		1
#endif

#ifndef __ASSEMBLY__
extern unsigned int nand_cs_base;
extern unsigned int boot_flash_base;
extern volatile unsigned int boot_flash_env_addr;
extern unsigned int boot_flash_off;
extern unsigned int boot_flash_sec;
extern unsigned int boot_flash_type;
#endif

#define WRITE_NAND_COMMAND(d, adr) \
	__raw_writew(d, (nand_cs_base + GPMC_NAND_CMD))
#define WRITE_NAND_ADDRESS(d, adr) \
	__raw_writew(d, (nand_cs_base + GPMC_NAND_ADR))
#define WRITE_NAND(d, adr) __raw_writew(d, (nand_cs_base + GPMC_NAND_DAT))
#define READ_NAND(adr) __raw_readw((nand_cs_base + GPMC_NAND_DAT))

#define NAND_WP_OFF()  do {*(volatile u32 *)(GPMC_CONFIG) \
	|= 0x00000010; } while (0)
#define NAND_WP_ON()  do {*(volatile u32 *)(GPMC_CONFIG) \
	&= ~0x00000010; } while (0)
#ifdef CFG_NAND_LEGACY
#define NAND_CTL_CLRALE(nandptr)
#define NAND_CTL_SETALE(nandptr)
#define NAND_CTL_CLRCLE(nandptr)
#define NAND_CTL_SETCLE(nandptr)
#endif
#define NAND_DISABLE_CE(nand)
#define NAND_ENABLE_CE(nand)
#define NAND_WAIT_READY(nand)	udelay(10)

#endif
