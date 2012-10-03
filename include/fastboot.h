#ifndef FASTBOOT_H
#define FASTBOOT_H

#include <common.h>
#include <command.h>


/* Use this as the scratchpad memory to communicate
 * to bootloader from kernel for warm-reset cases
 */
#define PUBLIC_SAR_RAM_1_FREE  (0x4a326000 + 0xA0C)
  

#define FASTBOOT_INTERFACE_CLASS     0xff
#define FASTBOOT_INTERFACE_SUB_CLASS 0x42
#define FASTBOOT_INTERFACE_PROTOCOL  0x03

#define FASTBOOT_VERSION "0.5"

#ifndef CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE
#define CFG_FASTBOOT_MKBOOTIMAGE_PAGE_SIZE 2048
#endif

typedef enum { NAND, EMMC } storage_medium_type;

struct cmd_fastboot_interface
{

	int (*rx_handler)(const unsigned char *buffer,
			  unsigned int buffer_size);

	void (*reset_handler)(void);
  

	char *product_name;

	char *serial_no;

	char *proc_rev;

	char *proc_type;

	storage_medium_type storage_medium;

	unsigned int nand_block_size;

	unsigned int nand_oob_size; 

	unsigned char *transfer_buffer;

 
	unsigned int transfer_buffer_size;

};

typedef struct fastboot_ptentry fastboot_ptentry;

struct fastboot_ptentry
{
	
	char name[16];
	
	unsigned int start;
	
	unsigned int length;

	unsigned int flags;
};

#define FASTBOOT_PTENTRY_FLAGS_REPEAT(n)              (n & 0x0f)
#define FASTBOOT_PTENTRY_FLAGS_REPEAT_MASK            0x0000000F

#define FASTBOOT_PTENTRY_FLAGS_WRITE_NEXT_GOOD_BLOCK  0x00000010

#define FASTBOOT_PTENTRY_FLAGS_WRITE_CONTIGUOUS_BLOCK 0x00000020

#define FASTBOOT_PTENTRY_FLAGS_WRITE_HW_ECC           0x00000040

#define FASTBOOT_PTENTRY_FLAGS_WRITE_SW_ECC           0x00000080

#define FASTBOOT_PTENTRY_FLAGS_WRITE_I                0x00000100

#define FASTBOOT_PTENTRY_FLAGS_WRITE_YAFFS            0x00000200

#define FASTBOOT_PTENTRY_FLAGS_WRITE_ENV              0x00000400

#define FASTBOOT_OK			0
#define FASTBOOT_ERROR			-1
#define FASTBOOT_DISCONNECT		1
#define FASTBOOT_INACTIVE		2

#define FASTBOOT_BOOT_MAGIC "ANDROID!"
#define FASTBOOT_BOOT_MAGIC_SIZE 8
#define FASTBOOT_BOOT_NAME_SIZE 16
#define FASTBOOT_BOOT_ARGS_SIZE 512

struct fastboot_boot_img_hdr {
	unsigned char magic[FASTBOOT_BOOT_MAGIC_SIZE];

	unsigned kernel_size;  
	unsigned kernel_addr;  

	unsigned ramdisk_size; 
	unsigned ramdisk_addr; 

	unsigned second_size;  
	unsigned second_addr;  

	unsigned tags_addr;    
	unsigned page_size;    
	unsigned unused[2];    

	unsigned char name[FASTBOOT_BOOT_NAME_SIZE]; 

	unsigned char cmdline[FASTBOOT_BOOT_ARGS_SIZE];

	unsigned id[8]; 
};

#if (CONFIG_FASTBOOT)

extern int fastboot_preboot(void);

extern int fastboot_init(struct cmd_fastboot_interface *interface);

extern void fastboot_shutdown(void);

extern int fastboot_poll(void);

extern int fastboot_is_highspeed(void);

extern int fastboot_fifo_size(void);

extern int fastboot_tx_status(const char *buffer, unsigned int buffer_size);

extern int fastboot_tx(unsigned char *buffer, unsigned int buffer_size);

extern int fastboot_getvar(const char *rx_buffer, char *tx_buffer);

extern int fastboot_oem(const char *command);

extern void fastboot_flash_add_ptn(fastboot_ptentry *ptn);
extern void fastboot_flash_reset_ptn(void);
extern fastboot_ptentry *fastboot_flash_find_ptn(const char *name);
extern fastboot_ptentry *fastboot_flash_get_ptn(unsigned n);
extern unsigned int fastboot_flash_get_ptn_count(void);
extern void fastboot_flash_dump_ptn(void);

extern int fastboot_flash_init(void);
extern int fastboot_flash_erase(fastboot_ptentry *ptn);
extern int fastboot_flash_read_ext(fastboot_ptentry *ptn, 
				   unsigned extra_per_page, unsigned offset, 
				   void *data, unsigned bytes);
#define fastboot_flash_read(ptn, offset, data, bytes) \
  flash_read_ext(ptn, 0, offset, data, bytes)
extern int fastboot_flash_write(fastboot_ptentry *ptn, unsigned extra_per_page,
				const void *data, unsigned bytes);

#else

#define fastboot_preboot() 0
#define fastboot_init(a) 1
#define fastboot_shutdown() 
#define fastboot_poll() 1
#define fastboot_is_highspeed() 0
#define fastboot_fifo_size() 0
#define fastboot_tx_status(a, b) 1
#define fastboot_getvar(a,b) 1
#define fastboot_tx(a, b) 1   

#define fastboot_flash_add_ptn(a) 
#define fastboot_flash_find_ptn(a) NULL
#define fastboot_flash_get_ptn(a) NULL
#define fastboot_flash_get_ptn_count() 0
#define fastboot_flash_dump_ptn() 
#define fastboot_flash_init() 
#define fastboot_flash_erase(a) 1
#define fastboot_flash_read_ext(a, b, c, d, e) 0
#define fastboot_flash_read(a, b, c, d, e) 0
#define fastboot_flash_write(a, b, c, d) 0

#endif 
#endif
