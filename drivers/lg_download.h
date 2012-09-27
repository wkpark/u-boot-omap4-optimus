#ifndef _LG_download_h_
#define _LG_download_h_

#include "comdef.h"


#define MAX_PACKET_SIZE       256 * 1024

#define MAX_PACKET_LEN        (MAX_PACKET_SIZE - 1)
#define MIN_PACKET_LEN        3        /* 2 bytes CRC + 1 byte command code */

#define SIO_RX_ERR            (-1)
#define SIO_NO_CHAR           (-2)
#define SIO_TIMEOUT           (-3)


//#define RX_BULKFIFO_SIZE (0x40000) // 40000h=256K, 10000=64K 1000=4K
#define ONENAND_BLOCK_SIZE (0x40000) // Reffered from configs

#define LGE_PACKET_SIZE		   (0x10000)

/*----------*/


#define LG_DOWNLOAD_SUCCESS            0

#define LG_DOWNLOAD_FAIL              -1


#define MAX_IFX_PACKET_SIZE				2048
#define IFX_PACKET_HEADER_SIZE			6


/*-------------------------------------------------------------------------*/

/* Async HDLC achieves data transparency at the byte level by using
   two special values. The first is a flag value which begins and ends
   every packet: */
#define ASYNC_HDLC_FLAG       0x7e

/* The flag value might appear in the data.  If it does, it is sent as
   a two-byte sequence consisting of a special escape value followed by
   the flag value XORed with 0x20.  This gives a special meaning to the
   escape character, so if it appears in the data it is itself escaped
   in the same way. */
#define ASYNC_HDLC_ESC        0x7d
#define ASYNC_HDLC_ESC_MASK   0x20

/*-------------------------------------------------------------------------*/

#define HDLC_ADD_BUFFER(e,len)                              \
    do {                                                \
        buffer[buffer_pos++] = e;                       \
        if (buffer_pos == len)           \
        {                                               \
            send_buffer(buffer, buffer_pos);            \
            buffer_pos = 0;                             \
        }                                               \
    } while(0)


#define HDLC_ADD_ESC_BUFFER(d,len)                            \
    do {                                                  \
        if (d == ASYNC_HDLC_ESC  || d == ASYNC_HDLC_FLAG) \
        {                                                 \
            HDLC_ADD_BUFFER(ASYNC_HDLC_ESC,len);              \
            HDLC_ADD_BUFFER(d ^ ASYNC_HDLC_ESC_MASK,len);     \
        }                                                 \
        else                                              \
        {                                                 \
            HDLC_ADD_BUFFER(d,len);                           \
        }                                                 \
    } while(0)


/* Seed value for CRC register.  The all ones seed is part of CCITT-16, as
** well as allows detection of an entire data stream of zeroes.
*/
#define CRC_16_L_SEED           0xFFFF
#define CRC_16_L_OK             0x0F47
#define CRC_16_L_OK_NEG         0xF0B8

/* The CRC table size is based on how many bits at a time we are going
** to process through the table.  Given that we are processing the data
** 8 bits at a time, this gives us 2^8 (256) entries.
*/
#define CRC_TAB_SIZE    256             /* 2^CRC_TAB_BITS      */

/* CRC table for 16 bit CRC, with generator polynomial 0x8408,
** calculated 8 bits at a time, LSB first.  This table is used
** from a macro in sio.c.
*/

static const unsigned short crc_16_l_table[ CRC_TAB_SIZE ] = {
  0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
  0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
  0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
  0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
  0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
  0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
  0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
  0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
  0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
  0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
  0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
  0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
  0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
  0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
  0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
  0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
  0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
  0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
  0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
  0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
  0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
  0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
  0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
  0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
  0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
  0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
  0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
  0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
  0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
  0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
  0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
  0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};


#define CRC_16_L_STEP(xx_crc,xx_c) \
  (((xx_crc) >> 8) ^ crc_16_l_table[((xx_crc) ^ (xx_c)) & 0x00ff])



typedef enum package_type_s
{
  Errors = 0,  // These errors cause retransmission from the PC
  
  Req_Start_flash_write = 0x01,
  Req_Write,
  Req_Complete_flash_write,
  Req_Erase,
  Req_Read,
  Req_Initialize_partition,
  Req_Write_Rom_copy,
  Req_Ready_Read,     
  Req_Write_Async,
  
  Req_go = 0x20,
  Req_no_op,
  Req_Reset,
  Req_Powerdown,
  Req_Pre_Reset,
  Req_CP_USB_Switch,
  
  Req_Software_ver = 0x40,
  Req_Notify_Start_DL,
  
  Rsp_Ack = 0x50,
  Rsp_Nak,
  
  Req_IFX_erase = 0x60,
  Req_IFX_write,
  Req_IFX_Start_write,
  Req_IFX_End_write,
  Req_IFX_Security_info,
  Req_IFX_PSI_RAM_write,
  Req_IFX_EBL_write,
  Req_IFX_Set_Prot_conf,
  Req_IFX_Baudraet_change,
  Req_IFX_Flash_ID,
  Req_IFX_Cfi_Info_1,
  Req_IFX_Cfi_Info_2,
  Req_IFX_Start,
  Req_IFX_EraseAll,
  Req_IFX_EraseCheck,
  Req_IFX_End,
  Req_IFX_Flash_Set_Address,
  
  Req_IFX_write2,
  Req_factory_info_t,
  Req_factory_info_t_2,


  Req_Web_Dload_cmd_t = 0xEF,

  packagedummy = 0xFF
} package_type_t;

typedef enum binary_type_s
{
  binary_type_start = 0,

  binary_type_ifx_psi = 0x20,
  binary_type_ifx_slb,
  binary_type_ifx_code,
  binary_type_ifx_aeneas,  
  binary_type_ifx_ua,  

  binary_type_x_load = 0x40,
  binary_type_u_boot,
  binary_type_userdata,
  binary_type_cache,
  binary_type_uimage,
  binary_type_uimage_recovery,
  binary_type_system,

  binary_type_none,  // These errors cause retransmission from the PC

  binary_type_end = 0xFF
} binary_type_t;

typedef enum error_type_s
{
  error_type_start = 0,  // These errors cause retransmission from the PC

  error_type_mismatch_write_len = 0x01,
  error_type_timeout,
  error_bad_block_occur,
  error_ifx_start_comm,
  error_type_many_bad_block,
  error_erase_fail,
 
  error_type_end = 0xffffffff
} error_type_t;

struct prot_read_ready_info_t{
	byte			type;
	byte			reserved1;
	unsigned long	StartAddr;
	unsigned long	Length;
	unsigned int	reserved2;
}__attribute__ ((packed));


struct prot_read_info_t{
	byte			type;
	byte			reserved1;
	unsigned long	Length;
	unsigned long	reserved2;
	unsigned int	reserved3;
}__attribute__ ((packed));

struct prot_factory_info_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	reserved4;
}__attribute__ ((packed));

struct prot_rsp_PID_t{
	byte		    type;
	unsigned char	phIMEI[17];
	unsigned char	phPID[64];
	unsigned int	reserved;
	unsigned short  crc;
}__attribute__ ((packed));
struct prot_rsp_PID_t_2{
	byte		    type;
	unsigned char	phSWV[15];
	unsigned char	phPID[32];
	unsigned int	reserved;
	unsigned short  crc;
}__attribute__ ((packed));


struct prot_cmd_t{
	byte		type;
}__attribute__ ((packed));

struct prot_init_partition_t{
	byte 			type;
	byte			binary_type;	
	unsigned long	addr;
	unsigned long   len;
	unsigned int	crc;
}__attribute__ ((packed));


struct prot_erase_t{
	byte 			type;
	byte			reserved1;	
	unsigned short  dummy;
	unsigned long	addr;
	unsigned long   len;
	unsigned int	crc;
}__attribute__ ((packed));


struct prot_write_t{
	byte 			type;
	byte			binary_type;	
	unsigned short  dummy;
	unsigned long	addr;
	unsigned long   len;
	unsigned int	crc;
}__attribute__ ((packed));


struct prot_write_async_t{
	byte 			type;
	byte			binary_type;	
	unsigned short  dummy;
	unsigned long	TotalLen;
	unsigned long   len;
	unsigned int	crc;
}__attribute__ ((packed));
struct prot_write_rom_copy_t{
	byte 			type;
	byte			binary_type;	
	unsigned long	addr;
	unsigned long   len;
	unsigned int	crc;
}__attribute__ ((packed));



struct prot_psi_ram_write_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   len;
	unsigned int	crc;
} __attribute__ ((packed));

struct prot_ebl_write_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   len;
	unsigned int	crc;
} __attribute__ ((packed));


struct prot_ifx_start_write_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	crc;
} __attribute__ ((packed));

struct prot_ifx_flash_id_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   len;
	unsigned int	crc;
} __attribute__ ((packed));



struct prot_rsp_t{
	byte		    type;
	unsigned int	cause;
	unsigned int	reserved;
	unsigned short  crc;
}__attribute__ ((packed));


struct prot_ifx_set_prot_conf_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	crc;
}__attribute__ ((packed));


struct prot_ifx_baudrate_change_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	crc;
}__attribute__ ((packed));


struct prot_ifx_cfi_info_1_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	crc;
}__attribute__ ((packed));


struct prot_ifx_cfi_info_2_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	crc;
}__attribute__ ((packed));

struct prot_ifx_req_start_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   len;
	unsigned int	crc;
} __attribute__ ((packed));


struct prot_ifx_erase_all_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	crc;
} __attribute__ ((packed));


struct prot_ifx_erase_check_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	crc;
} __attribute__ ((packed));


struct prot_ifx_req_end_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	crc;
} __attribute__ ((packed));


struct prot_ifx_flash_set_address_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	addr;
	unsigned long   reserved2;
	unsigned int	crc;
}__attribute__ ((packed));



struct prot_ifx_write_t{
	byte 			type;
	byte			reserved1;	
	unsigned short	datacrc;
	unsigned short	reallen;
	unsigned long   len;
	unsigned int	crc;
} __attribute__ ((packed));


struct prot_ifx_erase_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	crc;
} __attribute__ ((packed));


struct prot_reset_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	crc;
} __attribute__ ((packed));

struct prot_pre_reset_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	crc;
} __attribute__ ((packed));

struct prot_cp_usb_switch_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	crc;
} __attribute__ ((packed));
struct prot_notify_start_dl_t{
	byte 			type;
	byte			reserved1;	
	unsigned long	reserved2;
	unsigned long   reserved3;
	unsigned int	crc;
} __attribute__ ((packed));



typedef enum
{
  WEBDLOAD_CONNECT_MODE      = 0x00, /*  0 */
  WEBDLOAD_READ_STEP_END     = 0x01, /*  1 */
  WEBDLOAD_WRITE_STEP_END    = 0x02, /*  2 */
  WEBDLOAD_READ_CNT          = 0x03, /*  3 */
  WEBDLOAD_WRITE_CNT         = 0x04, /*  4 */
  WEBDLOAD_READ_BIN_VER      = 0x05, /*  5 */
  WEBDLOAD_WRITE_BIN_VER     = 0x06, /*  6 */
  WEBDLOAD_CLEAR_SIB         = 0x07, /*  7 */
  WEBDLOAD_READ_BATT_LVL     = 0x08, /*  8 */
  WEBDLOAD_VAL_READ_STEP_END = 0x09, /*  9 */
  WEBDLOAD_VAL_READ_CNT      = 0x0A, /* 10 */
  WEBDLOAD_DISP_IMAGE        = 0x0B, /* 11 */
  WEBDLOAD_SET_RSP_NEED      = 0x0C, /* 12 */
  WEBDLOAD_GET_USB_STRM_INFO = 0x0D, /* 13 */
  WEBDLOAD_SET_AUTH_MARK     = 0x0E, /* 14 */
  WEBDLOAD_GET_AUTH_MARK     = 0x0F, /* 15 */
  WEBDLOAD_READ_SUB_REV      = 0x10, /* 16 */
  WEBDLOAD_WRITE_SUB_REV     = 0x11, /* 17 */
  WEBDLOAD_READ_LANGUAGE     = 0x12, /* 18 */
  WEBDLOAD_WRITE_LANGUAGE    = 0x13, /* 19 */
  WEBDLOAD_DEL_C_DRV         = 0x14, /* 20 */
  WEBDLOAD_DEL_IMG_DRV       = 0x15, /* 21 */
  WEBDLOAD_GET_USB_STRM_CMD  = 0x16, /* 22 */

  WEBDLOAD_READ_BL_VER       = 0x17, /* 23 */
  WEBDLOAD_READ_THEME        = 0x18, /* 24 */
  WEBDLOAD_WRITE_THEME       = 0x19, /* 25 */

  /*--------------------*/
  WEBDLOAD_READ_RSVD_VAL1    = 0x40, /* 64 */
  WEBDLOAD_WRITE_RSVD_VAL1   = 0x41, /* 65 */
  WEBDLOAD_READ_RSVD_VAL2    = 0x42, /* 66 */
  WEBDLOAD_WRITE_RSVD_VAL2   = 0x43, /* 67 */
  WEBDLOAD_READ_RSVD_ARRAY1  = 0x50, /* 80 */
  WEBDLOAD_WRITE_RSVD_ARRAY1 = 0x51, /* 81 */
  WEBDLOAD_READ_RSVD_ARRAY2  = 0x52, /* 82 */
  WEBDLOAD_WRITE_RSVD_ARRAY2 = 0x53, /* 83 */
  /*--------------------*/

  WEBDLOAD_READ_IMEI         = 0x60, /* 96 */

  /*--------------------*/
  WEBDLOAD_SUB_CMD_END       = 0x7F
} webdload_sub_cmd_type;


#define RSVD_ARRAY_LEN	8
#define IMEI_ARRAY_LEN	16

typedef union
{
  word reserved_val1;
  word reserved_val2;
  char reserved_array1[RSVD_ARRAY_LEN];
  char reserved_array2[RSVD_ARRAY_LEN];
} webdload_req_type;

typedef union
{
  byte connection_mode;
  char imei[IMEI_ARRAY_LEN];

  word reserved_val1;
  word reserved_val2;
  char reserved_array1[RSVD_ARRAY_LEN];
  char reserved_array2[RSVD_ARRAY_LEN];
} webdload_rsp_type;

struct diag_webdload_req_type {
  byte cmd_code;			// Command code
  byte sub_cmd;				// Use webdload_sub_type
  webdload_req_type req_data;
}__attribute__ ((packed));

struct diag_webdload_rsp_type {
  byte cmd_code;			// Command code
  byte sub_cmd;				// Use webdload_sub_type
  boolean success;
  webdload_rsp_type rsp_data;
}__attribute__ ((packed));


extern void Main_Event_Process(void);




#endif




