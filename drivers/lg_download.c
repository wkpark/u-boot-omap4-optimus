#include <common.h>
#if defined(CONFIG_BOOT_DEVICE_EMMC)
//#if (CONFIG_COMMANDS & CFG_CMD_ONENAND)
#include <asm/types.h>
#include <linux/mtd/onenand.h>

#include "lg_download.h"
#include "lg_usb.h"

#include <ns16550.h>
#include <asm/arch/bits.h>
#include <asm/io.h>
#include <asm/arch/gpio.h>
#include <mmc.h>

#include <lge_nvdata_emmc.h>


#include <twl6030.h>

#include <asm/arch-omap4/cpu.h>
#include "../cpu/omap4/mmc_protocol.h"

#define mdelay(msec) {int i = msec; while (--i >= 0) udelay(1000);}
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define LCRVAL LCR_8N1					/* 8 data, 1 stop, no parity */
#define MCRVAL (MCR_DTR | MCR_RTS)			/* RTS/DTR */
#define FCRVAL (FCR_FIFO_EN | FCR_RXSR | FCR_TXSR)	/* Clear & enable FIFOs */

#define IFX_RESET 26
#define IFX_PWRON 27
#define USIF1_SW 182

#define SIZEOFREQFLASHID		0xAC

char trap_flag;


#define MAX_AT_BUFFER_SIZE		80
unsigned char 	test_Tbuf[3]={0,};
unsigned char	test_Rbuf[0x17]={0,};
unsigned char 	test_Tbuf2[80]={0,};
unsigned char	test_Rbuf2[80]={0,};

unsigned char hdlc_tx_buf [MAX_PACKET_SIZE]={0,}; 	/* tx buffer */

volatile unsigned char TxBuf[MAX_IFX_PACKET_SIZE + 4]={0,};
volatile unsigned char HeaderBuf[IFX_PACKET_HEADER_SIZE]={0,};

unsigned int  g_onenand_address;	

unsigned int g_onenand_block_offset;
unsigned int onenand_data_offset;				/* offset of current onenand_data */
unsigned int g_onenand_address_size;
volatile unsigned int  MmcSecStartAddr;	

unsigned int MmcSecLength;
unsigned int CurrentFileIndex = 0;

unsigned char					IsIFXInit = 0; // Factory Info. XML_LOG

extern unsigned int usbtty_bulk_fifo_index;
extern u8 *usbtty_bulk_fifo;

extern int poll_usbtty(void);
extern void usbtty_shutdown(void);
extern int usbtty_init(void);

extern int mmc_init(int slot);
extern int mmc_write(int mmc_cont, unsigned char *src, unsigned long dst, int size);

extern void ifx_start_low();
extern void ifx_uart_sw_ctrl(int);
extern void ifx_dl_reset();


static int 					hdlc_send_packet (unsigned char *pBuf, unsigned int wLen);

static int 					nonhdlc_send_packet (unsigned char *pCmdBuf, unsigned int wLen, unsigned char *pDataBuf, unsigned int dLen);
static int 					send_buffer (unsigned char *buf, int len);
static unsigned int 		parsingCmd(byte *pBuf);




int 						 Lg_uartRxCharWait(unsigned char ch, unsigned int escape_cnt);
int 						 Lg_uartRxLen(unsigned char* data, unsigned int len, unsigned int escape_cnt);
unsigned char				 Lg_Checksum_Short(unsigned char* data, unsigned short len);
unsigned char 				 Lg_Checksum_Int(unsigned char* data, unsigned int len);
unsigned short 				 Lg_Checksum_ShortB(unsigned char* data, unsigned short len);
int 						 IFXReqSetProtConf(unsigned char* pBuf);
int 						 IFXCnfBaudChange(void);
int							 IFXReqCfiInfo_1(void);
int 						 IFXReqCfiInfo_2(void);
int 						 IFXReqFlashEraseStart(char bEraseAll);

int 						 IFXReqFlashEraseCheck(void);
int 						 IFXReqEnd(void);
int 						 IFXReqFlashSetAddress(unsigned int StartAddr);


/*
typedef enum package_type_s
{
  Errors = 0,  // These errors cause retransmission from the PC

  Req_Start_flash_write = 0x01,
  Req_Write,
  Req_Complete_flash_write,
  Req_Erase,
  Req_Read,
  Req_Initialize_partition,

  Req_go = 0x20,
  Req_no_op,
  Req_Reset,
  Req_Powerdown,

  Req_Software_ver = 0x40,
 
  Rsp_Ack = 0x50,
  Rsp_Nak,

  Req_IFX_erase = 0x60,
  Req_IFX_write,
  Req_IFX_Start_write,
  Req_IFX_End_write,
  Req_Security_info,  
  
  packagedummy = 0xFF
} package_type_t;

*/


unsigned char*		pTmpBuf = NULL;
extern int		DownloadValid;

// WEB DOWNLOAD [START]
	static char web_download_flag2=0x00; 
	int web_download_mode=0; 
// WEB DOWNLOAD [END]

void Main_Event_Process(void)
{
	byte				pCmd_buf[100]; //= (byte*)0x80500000;
	struct prot_cmd_t*	pRxPack;
	struct prot_rsp_t	TxPack;
	unsigned int		nLen = 0;

	memset((void*)pCmd_buf, 0x00, sizeof(pCmd_buf));	
	nLen = parsingCmd(pCmd_buf);
	if ((nLen+2 < sizeof(struct prot_init_partition_t)) && (pCmd_buf[0]!=Req_Web_Dload_cmd_t))
	{
		//TxPack.type = Rsp_Nak;
		//hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));
		usbtty_bulk_fifo_index = 0;
		return;
	}

	pRxPack = (struct prot_cmd_t*)malloc(sizeof(struct prot_cmd_t));
	memcpy((void*)pRxPack, (void*)pCmd_buf, sizeof(struct prot_cmd_t));

	
	switch (pRxPack->type)
	{

	        case Req_Web_Dload_cmd_t:
		{
			usbtty_bulk_fifo_index = 0; 	

			char IMEI[15];
			
			struct diag_webdload_req_type* web_req;
			struct diag_webdload_rsp_type web_rsp;

                   web_req = (struct diag_webdload_req_type*)pCmd_buf;
			memset((void*)&web_rsp, 0x00, sizeof(struct diag_webdload_rsp_type));

			web_rsp.cmd_code = web_req->cmd_code;
			web_rsp.sub_cmd = web_req->sub_cmd;
                    web_rsp.success = 1;

            		switch(web_req->sub_cmd)
            		{
                		case WEBDLOAD_CONNECT_MODE:
                		{
                    		web_rsp.rsp_data.connection_mode = 2; /* Download Mode */
                    		break;
                		}
                		case WEBDLOAD_READ_IMEI:
                		{
					lge_static_nvdata_emmc_read(LGE_NVDATA_STATIC_IMEI_OFFSET,IMEI,15);
					memcpy((void*)web_rsp.rsp_data.imei, (void*)IMEI, (IMEI_ARRAY_LEN-1));
				
                    		break;
                		}
                		default:
                		{
                    		web_rsp.success = 0;
                    		break;
                		}
            		}

            		hdlc_send_packet((unsigned char*)&web_rsp, sizeof(struct diag_webdload_rsp_type));

			break;
		}	
	case Req_factory_info_t_2:
		{
			unsigned char * pFactoryBuf = (unsigned char *) malloc(MAX_AT_BUFFER_SIZE);
			/*
			===>  PID, Model name is saved like below in emmc (index 16)
			------------------------------------------------------------------------------------
			| 		PID(32 byte) |   4 byte (model name size) |3 or 4 or 5 (we can't know but... don't care)  |
			------------------------------------------------------------------------------------
			*/
			int TestNum = 0;
			int loopIdx = 0, Num = 0, Idx = 0;
			unsigned char	ReadCh, ReadCh2;
			byte bFind = 0;
			
			struct prot_rsp_PID_t_2 rsp;

			memset((void*)&rsp, 0x00, sizeof(struct prot_rsp_PID_t_2));
			usbtty_bulk_fifo_index = 0; 
			if (IsIFXInit == 0)
			{
				//extern  void hub_ifx_uart_sw_ctrl(int sel);
				// 1. UART switch change to infineon
				 ifx_start_low();;

				IsIFXInit = 1;
			}   

			mdelay(10000);

			  ifx_uart_sw_ctrl(0);
			//strcpy((char*)test_Tbuf2, "AT%IMEI");
			//test_Tbuf2[strlen("AT%IMEI")] = 0x0D;
			strcpy((char*)test_Tbuf2, "AT%INFO");
			test_Tbuf2[strlen("AT%INFO")] = 0x0D;

			// Read INFO
			putsB ((unsigned char*)test_Tbuf2, 8, 100000);
			TestNum = 0;
			while (1)
			{
				bFind = 0;
				memset((void*)test_Rbuf2, 0x00, sizeof(test_Rbuf2));
				mdelay(200);

				Num = 0;
				ReadCh2 = 0x00;
				memset((void*)test_Rbuf2, 0x00, sizeof(test_Rbuf2));
				while(1)
				{
					if (getcB (100000, (char*)&ReadCh) == -1)
						break;			
					
					printf("######## AT Test: %x\n",ReadCh); 
					test_Rbuf2[Num++] = ReadCh;
				}
				printf("============= PASS ================\n");
				
				for (loopIdx = 0; loopIdx < Num; loopIdx++)
				{
					if (test_Rbuf2[loopIdx]==0x41 && test_Rbuf2[loopIdx+1]==0x54 && test_Rbuf2[loopIdx+2] == 0x25 && test_Rbuf2[loopIdx+3] == 0x49)					
					{						
						printf("AT Test1:%x, %x, %x, %x",test_Rbuf2[8], test_Rbuf2[9], test_Rbuf2[10], test_Rbuf2[11]); 
						printf("AT Test2:%x, %x, %x, %x",test_Rbuf2[12], test_Rbuf2[13], test_Rbuf2[14], test_Rbuf2[15]);	
						printf("AT Test3:%x, %x, %x, %x\n",test_Rbuf2[16], test_Rbuf2[17], test_Rbuf2[18], test_Rbuf2[19]);	
					
						bFind = 1;
						break;
					}
				}

				printf("Fine %x\n", bFind);	

				if (bFind == 0)
				{
					mdelay(10);
					putsB ((unsigned char*)test_Tbuf2, 8, 100000);				
					TestNum++; 

					if (TestNum > 30)
					{
						//FastbootStatus("fail");
						/*
						memset((void*)&rsp, 0x00, sizeof(struct prot_rsp_PID_t));
						rsp.type = Rsp_Ack;
						hdlc_send_packet ((unsigned char*)&rsp, sizeof(struct prot_rsp_PID_t)); 				
						*/
						break;
					}	
								
					continue;
				}

				break;
			}
				
			if (TestNum > 10)
			{
				int ModelNameSize = 0;

				printf("###### Can't find PID\n");
//				read_PID_from_emmc(pFactoryBuf);

				int char_num = 0 ;

				while(char_num < 32 )
				{
					lge_static_nvdata_emmc_read(LGE_NVDATA_STATIC_PID_DETECT_OFFSET+(char_num*2),pFactoryBuf+char_num,1);
					char_num++;
				}

				printf(" \n \n \n \n mmc read  PID = %s \n\n\n\n",pFactoryBuf );
				
				memcpy(rsp.phPID, pFactoryBuf, 32);
				ModelNameSize = *(int*)(pFactoryBuf + 32);
				memcpy(rsp.phSWV, pFactoryBuf + 32 + 4, ModelNameSize);
				printf("###### ModelName size:%d\n", ModelNameSize);
				
				rsp.type = Rsp_Ack;
				hdlc_send_packet ((unsigned char*)&rsp, sizeof(struct prot_rsp_PID_t_2)); 					

				free(pFactoryBuf);
				break;
			}
			else
			{
				int char_num = 0 ;
				printf("###### find PID\n");
				//memcpy(rsp.phIMEI, (byte*)(test_Rbuf2 + sizeof("AT%IMEI") + 1), 17); // sizeof("AT%IMEI") => 9bytes
				memcpy(rsp.phPID, (byte*)(test_Rbuf2 + loopIdx + strlen("AT%INFO") + 4), 32); // sizeof("AT%IMEI") => 9bytes
				memcpy(pFactoryBuf, (byte*)(test_Rbuf2 + loopIdx + strlen("AT%INFO") + 4), 32);
				printf(" \n \n \n \n PID = %s \n\n\n\n",pFactoryBuf );


				while(char_num < 32 )
				{
					lge_static_nvdata_emmc_write(LGE_NVDATA_STATIC_PID_DETECT_OFFSET+(char_num*2),rsp.phPID+char_num,1);
					char_num++;
				}

			}


			strcpy((char*)test_Tbuf2, "AT%SWV");
			test_Tbuf2[strlen("AT%SWV")] = 0x0D;

			// Read SWV
			putsB ((unsigned char*)test_Tbuf2, 7, 100000);
			TestNum = 0;
			while (1)
			{

				bFind = 0;
				memset((void*)test_Rbuf2, 0x00, sizeof(test_Rbuf2));
				mdelay(200);

				Num = 0;
				ReadCh2 = 0x00;
				memset((void*)test_Rbuf2, 0x00, sizeof(test_Rbuf2));
				while(1)
				{
					if (getcB (100000, (char*)&ReadCh) == -1)
						break;			
					
					printf("######## AT Test: %x\n",ReadCh); 
					test_Rbuf2[Num++] = ReadCh;
				}
				printf("============= PASS ================\n");
				
				for (loopIdx = 0; loopIdx < Num; loopIdx++)
				{
					if (test_Rbuf2[loopIdx]==0x41 && test_Rbuf2[loopIdx+1]==0x54 && test_Rbuf2[loopIdx+2] == 0x25 && test_Rbuf2[loopIdx+3] == 0x53)					
					{						
						printf("AT Test1:%x, %x, %x, %x",test_Rbuf2[8], test_Rbuf2[9], test_Rbuf2[10], test_Rbuf2[11]); 
						printf("AT Test2:%x, %x, %x, %x",test_Rbuf2[12], test_Rbuf2[13], test_Rbuf2[14], test_Rbuf2[15]);	
					
						bFind = 1;
						break;
					}
				}

				printf("Fine %x\n", bFind);	

				if (bFind == 0)
				{
					mdelay(10);
					putsB ((unsigned char*)test_Tbuf2, 8, 100000);				
					TestNum++; 

					if (TestNum > 10)
					{
						break;
					}					
					continue;
				}

				break;
			}

			if (TestNum > 10)
			{
				int ModelNameSize = 0;

				printf("###### Can't find SWV\n");
				memcpy(rsp.phPID, pFactoryBuf, 32);
				ModelNameSize = *(int*)(pFactoryBuf + 32);
				memcpy(rsp.phSWV, pFactoryBuf + 32 + 4, ModelNameSize);
				printf("###### ModelName size:%d\n", ModelNameSize);
				
				rsp.type = Rsp_Ack;
				hdlc_send_packet ((unsigned char*)&rsp, sizeof(struct prot_rsp_PID_t_2)); 					
				
				free(pFactoryBuf);	
				break;
			}

			TestNum = loopIdx;
			while(1)
			{
				if (test_Rbuf2[TestNum] == 0x2d)
				{
					break;
				}
				printf("@@@@@@@@@ TestNum:%d: %x\n", TestNum, test_Rbuf2[TestNum]);
				TestNum++;

				if (TestNum > 30)
				{
					printf("We can't find\n");
					break;
				}
			}
			
			memcpy(rsp.phSWV, (byte*)(test_Rbuf2 + loopIdx + strlen("AT%SWV") + 4), TestNum - loopIdx - strlen("AT%SWV") - 4); // sizeof("AT%IMEI") => 9bytes

			printf(" \n \n \n \n PID = %s \n\n\n\n\n ", rsp.phSWV);
			
			rsp.phSWV[TestNum - loopIdx + - strlen("AT%SWV") - 4] = NULL;
			*(int*)((char*)pFactoryBuf + 32) = (int)(TestNum - loopIdx - strlen("AT%SWV") - 4);
			memcpy((void*)((char*)pFactoryBuf + 32 + 4), (void*)rsp.phSWV, TestNum - loopIdx - strlen("AT%SWV") - 4 + 1);

			rsp.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&rsp, sizeof(struct prot_rsp_PID_t_2)); 				
			free(pFactoryBuf);
			break;
		}
		case Req_factory_info_t:
		{
			int TestNum = 0;
			int loopIdx = 0, Num = 0;
			unsigned char	ReadCh, ReadCh2;

			printf("====Req_factory_info_t======\n"); //jb.chae
			
			struct prot_rsp_PID_t rsp;

			usbtty_bulk_fifo_index = 0; 
			if (IsIFXInit == 0)
			{
                          ifx_start_low();
				IsIFXInit = 1;
			}
			udelay(10000);
	
                           ifx_uart_sw_ctrl(0);

			mdelay(5000);  
			strcpy((char*)test_Tbuf2, "AT%IMEI");
			test_Tbuf2[strlen("AT%IMEI")] = 0x0D;
			
			printf("============== woonrae woonrae =============\n");

			putsB ((unsigned char*)test_Tbuf2, 8, 1000000);
			TestNum = 0;

			while (1)
			{
				memset((void*)test_Rbuf2, 0x00, sizeof(test_Rbuf2));
				mdelay(200);

				Num = 0;
				ReadCh2 = 0x00;
				memset((void*)test_Rbuf2, 0x00, sizeof(test_Rbuf2));
				for (loopIdx = 0; loopIdx < 60; loopIdx++)  //40->60
				{
					if (getcB (1000000, (char*)&ReadCh) == -1)
						break;

					printf("%x \n", ReadCh);
					if (ReadCh == 0x4B && ReadCh2 == 0x4F)  // 0x4B = K, 0x4F = O
					{
						test_Rbuf2[Num++] = ReadCh;
						getcB (1000000, (char*)&ReadCh);
						getcB (1000000, (char*)&ReadCh);	
						break;
					}
					else
						ReadCh2 = ReadCh;
			
					test_Rbuf2[Num++] = ReadCh;
				}
				printf("============= PASS ================\n");
				if (test_Rbuf2[8]==0x0d && test_Rbuf2[9]==0x0a && test_Rbuf2[10] == 0x02) //AT%I	
				{
					
					printf("AT Test1:%x, %x, %x, %x\n",test_Rbuf2[8], test_Rbuf2[9], test_Rbuf2[10], test_Rbuf2[11]); 
					printf("AT Test2:%x, %x, %x, %x\n",test_Rbuf2[12], test_Rbuf2[13], test_Rbuf2[14], test_Rbuf2[15]);	
					printf("AT Test3:%x, %x, %x, %x\n",test_Rbuf2[16], test_Rbuf2[17], test_Rbuf2[18], test_Rbuf2[19]);	
					
					break;
				}			
  
				mdelay(10);
				putsB ((unsigned char*)test_Tbuf2, 8, 100000);
				
				TestNum++; 
			
			if (TestNum > 10)
				break;
			
			}


			if (TestNum  > 10 )
			{
					//FastbootStatus("fail");
					memset((void*)&rsp, 0x00, sizeof(struct prot_rsp_PID_t));
					rsp.type = Rsp_Ack;
					hdlc_send_packet ((unsigned char*)&rsp, sizeof(struct prot_rsp_PID_t)); 				
					break;
			}


			memcpy(rsp.phIMEI, (byte*)(test_Rbuf2 + sizeof("AT%IMEI") +3), 17);  


			memset(test_Tbuf2, 0x00, sizeof(test_Tbuf2)); 
			memset(test_Rbuf2, 0x00, sizeof(test_Rbuf2)); 
			memcpy(rsp.phPID, (byte*)(test_Rbuf2), 64); 

			rsp.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&rsp, sizeof(struct prot_rsp_PID_t)); 				

			break;
		}

		case Req_Notify_Start_DL:
		{
			usbtty_bulk_fifo_index = 0;	
			DownloadValid = 1;
			
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
			break;
		}
		
		case Req_Start_flash_write:
		{
			//extern void hub_ifx_dl_poweroff();
			//hub_ifx_dl_poweroff();
			usbtty_bulk_fifo_index = 0;		

			mmc_init(1);
				
			if(web_download_mode == 1)
			{
				web_download_flag2 = 0x00;
				lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_WED_DOWNLOAD_OFFSET2,&web_download_flag2,1);

				if(web_download_flag2 != LGE_NVDATA_RESET_CAUSE_WEB_DOWNLOAD_RESET2) 
				{
				web_download_flag2 = LGE_NVDATA_RESET_CAUSE_WEB_DOWNLOAD_RESET2;
				lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_WED_DOWNLOAD_OFFSET2,&web_download_flag2,1);	
			}
			}
				
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));			
			
			break;
		}

		case Req_Initialize_partition:
		{
			struct prot_init_partition_t RxInitPart;
			memcpy((void*)&RxInitPart, (void*)pCmd_buf, sizeof(struct prot_init_partition_t));
#ifdef CONFIG_COSMO_SU760
#else
			extern int download_start;
			extern int cable_910K_detect;
			extern void download_logo(int);
	
				
			if(cable_910K_detect ==1)
			{
				download_start =  1; 
			}
#endif
			MmcSecStartAddr = RxInitPart.addr;
			MmcSecLength = RxInitPart.len;
			
			// g_onenand_address = RxInitPart.addr;
			// g_onenand_address_size = RxInitPart.len;


			g_onenand_block_offset = 0;
			onenand_data_offset = 0;				
			usbtty_bulk_fifo_index = 0;
			
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));
			break;
		}

		case Req_Erase:
		{
			struct prot_erase_t Rxerase;
			memcpy((void*)&Rxerase, (void*)pCmd_buf, sizeof(struct prot_erase_t));

			MmcSecStartAddr = Rxerase.addr;
			// g_onenand_address = Rxerase.addr;
			//g_onenand_address_size = Rxerase.len;
			g_onenand_block_offset = 0;
			onenand_data_offset = 0;				
			usbtty_bulk_fifo_index = 0;


			// mmc_erase(1, (unsigned int)g_onenand_address, (int)Rxerase.len);
			if (1 != mmc_erase(1, (unsigned int)MmcSecStartAddr, (int)Rxerase.len))
			{
	
			TxPack.type = Rsp_Nak;
				hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));	
				break;
			}
			
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));
			
			break;
		}		


		case Req_Write:
		{
			struct prot_write_t	Rxwrite;
			size_t retlen, oobretlen;
			unsigned int    badblockIdx = 0;
			unsigned int	AccumSize = 0;			
#if defined (CONFIG_LGE_CX2) || defined (CONFIG_LGE_P2)
                        memcpy((void*)&Rxwrite, (void*)pCmd_buf, sizeof(struct prot_write_t));
#else
			memcpy((void*)&Rxwrite, (void*)pCmd_buf, sizeof(struct prot_init_partition_t));
#endif
			//CurrentFileIndex += 2;
			//RX_BULKFIFO_SIZE = 4KByte

			while (Rxwrite.len > usbtty_bulk_fifo_index - CurrentFileIndex)
			{
				int poll_status;
				
				poll_status = poll_usbtty();

				if (TTYUSB_ERROR == poll_status) {
					break;
				} else if (TTYUSB_DISCONNECT == poll_status) {
					/* beak, cleanup and re-init */
					printf("USBtty disconnect detected\n");
					usbtty_shutdown();
					printf("USB term shutdown\n");
					usbtty_init();
					printf("USBtty Reinitialised\n");
				}			
			}

			mmc_write(1, &usbtty_bulk_fifo[CurrentFileIndex], (int)MmcSecStartAddr, Rxwrite.len);
#if 0
			if (mmc_write(1, &usbtty_bulk_fifo[CurrentFileIndex], (int)g_onenand_address, Rxwrite.len) == 1)
			{
				TxPack.type = Rsp_Nak;
				TxPack.cause     = error_erase_fail;
				
				hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
				break;			
			}
#endif			
			MmcSecStartAddr += Rxwrite.len/512;
			usbtty_bulk_fifo_index = 0;

			TxPack.type = Req_Write;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));
			
			break;
		}

		case Req_Write_Async:
		{
			struct prot_write_async_t	Rxwrite;
			unsigned long	RealPacketSize = 0;
			int		bDLFirst	= 0;

			memcpy((void*)&Rxwrite, (void*)pCmd_buf, sizeof(struct prot_write_async_t));

			if (bDLFirst == 0)
			{
				bDLFirst = 1;
				TxPack.type = Rsp_Ack;
				hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
			}			

			while (1)
			{
				if (Rxwrite.TotalLen <= Rxwrite.len)
				{
					RealPacketSize = Rxwrite.TotalLen;
				}
				else
				{
					RealPacketSize = Rxwrite.len;
				}

				while (RealPacketSize > usbtty_bulk_fifo_index - CurrentFileIndex)
				{
					int poll_status;
					
					poll_status = poll_usbtty();

					if (TTYUSB_ERROR == poll_status) {
						break;
					} else if (TTYUSB_DISCONNECT == poll_status) {
						printf("USBtty disconnect detected\n");
						usbtty_shutdown();
						printf("USB term shutdown\n");
						usbtty_init();
						printf("USBtty Reinitialised\n");
					}			
				}


				if (mmc_write(1, &usbtty_bulk_fifo[CurrentFileIndex], (int)MmcSecStartAddr, RealPacketSize) != 1)
				{
					printf("Write error\n");
					while(1);
				}
				MmcSecStartAddr += RealPacketSize/512;
				usbtty_bulk_fifo_index = 0;

				Rxwrite.TotalLen -= RealPacketSize;
			
				if (Rxwrite.TotalLen == 0)
				{
					break;
				}
				CurrentFileIndex = 0; 
			}
			
			break;
		}
		case Req_Write_Rom_copy:
		{
			break;
		}		

		case Req_Complete_flash_write:
		{
			break;
		}

		case Req_Read:
		{

			struct prot_read_info_t Rxwrite;
			unsigned char *pBuf = hdlc_tx_buf;

			memcpy((void*)&Rxwrite, (void*)pCmd_buf, sizeof(struct prot_read_info_t));
			
			//printf("sector addr:%d, length: %d\n", MmcSecStartAddr, Rxwrite.Length);
			mmc_read(1, MmcSecStartAddr, pBuf, Rxwrite.Length);
			MmcSecStartAddr += Rxwrite.Length/512;
			
			usbtty_bulk_fifo_index = 0;	 

			TxPack.type = Rsp_Ack;
			nonhdlc_send_packet((unsigned char*)&TxPack, sizeof(struct prot_rsp_t), pBuf, Rxwrite.Length);

			break;
		}

		case Req_Ready_Read: 
		{ 
			struct prot_read_ready_info_t	Rxwrite;
			memcpy((void*)&Rxwrite, (void*)pCmd_buf, sizeof(struct prot_read_ready_info_t));	

			//printf("####1 Start Sector addrss %d\n", Rxwrite.StartAddr);
			MmcSecStartAddr = Rxwrite.StartAddr;
			//printf("####2 Start Sector addrss %d\n", MmcSecStartAddr);
			MmcSecLength = Rxwrite.Length;
			//printf("####3 Start Sector addrss %d\n", MmcSecStartAddr);
			
			usbtty_bulk_fifo_index = 0;		
			
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));	
			//printf("####4 Start Sector addrss %d\n", MmcSecStartAddr);
			break;			

		}
		case Req_go: 
		{
			break;
		}

		case Req_no_op:
		{

			usbtty_bulk_fifo_index = 0;		
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));
				
			break;
		}

		case Req_Reset:
		{
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));
			char webdownload_finished_flag = 0x00;
			lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_WED_DOWNLOAD_OFFSET2,&webdownload_finished_flag,1);


			extern int cable_910K_detect;
			extern void muic_for_download(int);
			char status[1];

			if((cable_910K_detect==1) ||(webdownload_finished_flag != 0x00) || (web_download_mode != 0))
			{
				if(webdownload_finished_flag!=0x00 ||web_download_mode != 0 )
				{
					web_download_flag2 = 0x00; 
					lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_WED_DOWNLOAD_OFFSET2,&web_download_flag2,1);
					muic_for_download(0);
					muic_for_download(1); //changing muic form AP USB to CP USB-910K for preventing omap flash pop-up box.
					udelay(1000000); 

				}

#ifdef CONFIG_COSMO_SU760
				write_muic_mode_of_dload(0x01);

				trap_flag = LGE_NVDATA_RESET_CAUSE_VAL_USER_RESET;
				lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_RESET_CAUSE_OFFSET,&trap_flag,1);
				status[0] = 'S';
				lge_static_nvdata_emmc_write(LGE_NVDATA_STATIC_DOWNLOAD_FINISH,&status ,1);
				printf("\n download success !!! \n");
#else
#endif
			        udelay(1000000);				
	

			        write_dev_on_off(0x47);
			}
			
			udelay(1000000);
			write_dev_on_off( MOD_DEVOFF | CON_DEVOFF | APP_DEVOFF );

			disable_interrupts ();
			reset_cpu (0);			

			printf ("--- system reboot! ---\r\n");			
			break;			
		}
		 case Req_CP_USB_Switch:
		 {
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));

			printf ("--- CP USB swiching start ! ---\r\n");		

			extern void muic_for_download(int);
			extern void ifx_dl_reset();

			usbtty_bulk_fifo_index = 0;				
			muic_for_download(0);			
			ifx_dl_reset();
			
			printf("start udelay \n");
		
			udelay(25000000);

			extern int disable_interrupts (void);

			printf("end udelay \n");

			int muic_mode= muic_init(0);
			write_dev_on_off(0x47);
			
			break;
		}
		
		case Req_Pre_Reset:
		{
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));
			udelay(1000000); 

			usbtty_shutdown();
			printf("USB term shutdown\n");
			usbtty_init();
			printf("USBtty Reinitialised\n");			

			extern int disable_interrupts (void);

			udelay(1000000);

			write_dev_on_off(0x47);
			
			extern int disable_interrupts (void);


			disable_interrupts ();
			reset_cpu (0);			

			usbtty_bulk_fifo_index = 0;		
			printf ("--- system pre reboot! ---\r\n");				
			break;
		}

		case Req_Powerdown:
		{
			break;
		}

		case Req_Software_ver:
		{
			break;
		}

		case Req_IFX_erase:
		{
			usbtty_bulk_fifo_index = 0;			

			if (IFXReqFlashEraseStart(FALSE) == LG_DOWNLOAD_FAIL)
				while(1);
				
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));
			
			break;
		}

		case Req_IFX_Start_write:
		{
#ifdef CONFIG_COSMO_SU760
			usbtty_bulk_fifo_index = 0;
#else
			extern int download_start;
			extern int cable_910K_detect;
			
			usbtty_bulk_fifo_index = 0;

			extern void download_logo(int);
			
			if(cable_910K_detect ==1)
			{
				download_start =  1; 
				download_logo(1);
			}	
#endif

// WEB DOWNLOAD [START]
			if(web_download_mode == 1)
			{
				web_download_flag2 = 0x00;
				lge_dynamic_nvdata_emmc_read(LGE_NVDATA_DYNAMIC_WED_DOWNLOAD_OFFSET2,&web_download_flag2,1);

				if(web_download_flag2 != LGE_NVDATA_RESET_CAUSE_WEB_DOWNLOAD_RESET2)
				{
			web_download_flag2 = LGE_NVDATA_RESET_CAUSE_WEB_DOWNLOAD_RESET2;
			lge_dynamic_nvdata_emmc_write(LGE_NVDATA_DYNAMIC_WED_DOWNLOAD_OFFSET2,&web_download_flag2,1);			
			}
			}
// WEB DOWNLOAD [END]


			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));			
			break;
		}

		case Req_IFX_End_write:
		{
			break;
		}

		case Req_IFX_Security_info:
		{
			break;
		}
		
		case Req_IFX_EBL_write:
		{
			unsigned int len;

			struct prot_ebl_write_t	Rxwrite;
			size_t retlen;
			unsigned int		EBL_size;
			unsigned char		ReadCh;

			memcpy((void*)&Rxwrite, (void*)pCmd_buf, sizeof(struct prot_ebl_write_t));

			while (Rxwrite.len > usbtty_bulk_fifo_index - CurrentFileIndex)
			{
				int poll_status;
				
				poll_status = poll_usbtty();

				if (TTYUSB_ERROR == poll_status) {
					break;
				} else if (TTYUSB_DISCONNECT == poll_status) {
					/* beak, cleanup and re-init */
					printf("USBtty disconnect detected\n");
					usbtty_shutdown();
					printf("USB term shutdown\n");
					usbtty_init();
					printf("USBtty Reinitialised\n");
				}	
			}	

			EBL_size = (unsigned int)(Rxwrite.len);
			if (putsB((const char*)&EBL_size, sizeof(unsigned int), 100000) == LG_DOWNLOAD_FAIL)
				while(1);

			if (getcB (3000000, (char*)&ReadCh) == LG_DOWNLOAD_FAIL)
				while(1);

			if (ReadCh != 0xCC)
				while(1);

			if (getcB (3000000, (char*)&ReadCh) == LG_DOWNLOAD_FAIL)
				while(1);

			if (ReadCh != 0xCC)
				while(1);

			if (putsB((char*)&usbtty_bulk_fifo[CurrentFileIndex], EBL_size, 100000) == LG_DOWNLOAD_FAIL)
				while(1);

			if (putcB((char)Lg_Checksum_Int(&usbtty_bulk_fifo[CurrentFileIndex], EBL_size), 100000) == LG_DOWNLOAD_FAIL)
				while(1);

			// Wait for Completed
			if (getcB (3000000, (char*)&ReadCh) == LG_DOWNLOAD_FAIL)
				while(1);

			if (ReadCh != 0x51)			//if (ReadCh != 0x50)		//KIMBYUNGCHUL  XG618 : 0x50,  XG626 : 0x51
				while(1);
					
			// Check CRC
			if (getcB (30000000, (char*)&ReadCh) == LG_DOWNLOAD_FAIL)
				while(1);

			if (ReadCh != 0xA5)
				while(1);

			// I'm alive
			pTmpBuf = (unsigned char*)malloc(76);
			if (Lg_uartRxLen(pTmpBuf, 76, 3000000) == LG_DOWNLOAD_FAIL)
				while(1);	
			#if defined(CONFIG_COSMO_REV_B) || defined(CONFIG_COSMO_REV_C) 
				if (pTmpBuf[0] != 0xBB || (pTmpBuf[4] != 0x01 && pTmpBuf[4] != 0x14) )
					while(1);
			#else
				if (pTmpBuf[0] != 0xBB)	
					while(1);	
			#endif
			
			
				
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
			usbtty_bulk_fifo_index = 0;	

			break;
		}

		case Req_IFX_Set_Prot_conf:
		{
			
			usbtty_bulk_fifo_index = 0;	
			if (pTmpBuf == NULL)
				while(1);
				
			if (IFXReqSetProtConf(pTmpBuf) == LG_DOWNLOAD_FAIL)
				while(1);
			
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
			
			free(pTmpBuf);			
			break;
		}

		case Req_IFX_Baudraet_change:
		{

			usbtty_bulk_fifo_index = 0;				
			if (IFXCnfBaudChange() == LG_DOWNLOAD_FAIL)
				while(1);	

   			
			extern void _serial_reinit(const int port, int baud_divisor);
			_serial_reinit(1, 0x01);
			udelay(2000000); 
			
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));	
			
			break;
		}

	    case Req_IFX_PSI_RAM_write:
		{
			unsigned int len;

			struct prot_psi_ram_write_t	Rxwrite;
			size_t retlen;
			unsigned short		PsiRam_size;
			unsigned char		ReadCh;

			memcpy((void*)&Rxwrite, (void*)pCmd_buf, sizeof(struct prot_psi_ram_write_t));

			while (Rxwrite.len > usbtty_bulk_fifo_index - CurrentFileIndex)
			{
				int poll_status;
				
				poll_status = poll_usbtty();

				if (TTYUSB_ERROR == poll_status) {
					break;
				} else if (TTYUSB_DISCONNECT == poll_status) {
			/* beak, cleanup and re-init */
					printf("USBtty disconnect detected\n");
					usbtty_shutdown();
					printf("USB term shutdown\n");
					usbtty_init();
					printf("USBtty Reinitialised\n");
				}	
			}
						
			//Lg_NS16550_init (com_port, 0x1A);	
#if 0 // TODO defined(CONFIG_COSMO)

			extern  void hub_ifx_uart_sw_ctrl(int sel);
			// 1. UART switch change to infineon
			hub_ifx_uart_sw_ctrl(0);

			extern void hub_ifx_dl_reset();
			// 2. Infineon reset
			hub_ifx_dl_reset();
#endif
	
			extern void ifx_start_low();
			extern void ifx_uart_sw_ctrl(int);
	extern void ifx_dl_reset();
	
			printf ("delay for ifx download : 1\n");
			udelay(1000000);
			ifx_start_low();

#if defined(CONFIG_COSMO_REV_B) || defined(CONFIG_COSMO_REV_C)
                          udelay(10000);
#endif


			ifx_uart_sw_ctrl(0);
		

			
			len = 2;
			strcpy((char*)test_Tbuf, "AT");

			if (putsB ((char*)test_Tbuf, 2, 100000) == LG_DOWNLOAD_FAIL)
				while(1);
			if (Lg_uartRxCharWait(0xF0, 100000) == LG_DOWNLOAD_FAIL)
			while(1);

			if (Lg_uartRxLen(test_Rbuf, 0x17, 100000) == LG_DOWNLOAD_FAIL)
				while(1);

			if (test_Rbuf[0x0] != 0x16 || test_Rbuf[0x15] != 0xff || test_Rbuf[0x16] != 0xff)
				while(1);

			if (putcB (0x30, 100000) == LG_DOWNLOAD_FAIL)
				while(1);

			PsiRam_size = (unsigned short)(Rxwrite.len);
			if (putsB((const char*)&PsiRam_size, sizeof(unsigned short), 100000) == LG_DOWNLOAD_FAIL)
				while(1);
#if ! defined(CONFIG_COSMO_REV_B) && !defined(CONFIG_COSMO_REV_C)
			if (putcB (0xFF, 100000) == LG_DOWNLOAD_FAIL)
				while(1);
#endif
			if (putsB((char*)&usbtty_bulk_fifo[CurrentFileIndex], PsiRam_size, 100000) == LG_DOWNLOAD_FAIL)
				while(1);
			//goto TONAK;

			if (putcB(Lg_Checksum_Short(&usbtty_bulk_fifo[CurrentFileIndex], PsiRam_size), 100000) == LG_DOWNLOAD_FAIL)
				while(1);

			if (getcB (15000000, (char*)&ReadCh) == LG_DOWNLOAD_FAIL)
				while(1);

			if (ReadCh != 0x01)
				while(1);
			//goto TONAK;

			if (getcB (15000000, (char*)&ReadCh) == LG_DOWNLOAD_FAIL)
				while(1);

			if (ReadCh != 0x01)
				while(1);
			//goto TONAK;

			// Start psi_ram
			if (getcB (15000000, (char*)&ReadCh) == LG_DOWNLOAD_FAIL)
				while(1);

			if (ReadCh != 0x00)
				while(1);

			//goto TONAK;

			if (getcB (15000000, (char*)&ReadCh) == LG_DOWNLOAD_FAIL)
				while(1);

			if (ReadCh != 0xAA)
				while(1);
			//goto TONAK;
			
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
			usbtty_bulk_fifo_index = 0;	
			break;

TONAK:
			TxPack.type = Rsp_Nak;
			TxPack.cause     = error_ifx_start_comm;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
			usbtty_bulk_fifo_index = 0;		
			break;
	    }

		case Req_IFX_Flash_ID:
		{
			unsigned int len;

			struct prot_ifx_flash_id_t	Rxwrite;
			size_t retlen;
			unsigned short		PsiRam_size;
			unsigned char		ReadCh;
			unsigned int		payload_size = 0;
			unsigned short  	CheckSumVal;	

			memcpy((void*)&Rxwrite, (void*)pCmd_buf, sizeof(struct prot_ifx_flash_id_t));

			while (Rxwrite.len > usbtty_bulk_fifo_index - CurrentFileIndex)
			{
				int poll_status;
				
				poll_status = poll_usbtty();

				if (TTYUSB_ERROR == poll_status) {
					break;
				} else if (TTYUSB_DISCONNECT == poll_status) {
					printf("USBtty disconnect detected\n");
					usbtty_shutdown();
					printf("USB term shutdown\n");
					usbtty_init();
					printf("USBtty Reinitialised\n");
				}	
			}	

			memcpy((void*)TxBuf, (void*)&usbtty_bulk_fifo[CurrentFileIndex], SIZEOFREQFLASHID);
			memset(HeaderBuf, 0x00, IFX_PACKET_HEADER_SIZE);
			HeaderBuf[2] = 0x01; // Type
			HeaderBuf[3] = 0x08; // Type
			HeaderBuf[4] = 0xAC; // Size


			CheckSumVal = Lg_Checksum_ShortB(TxBuf, SIZEOFREQFLASHID);
			CheckSumVal += (0x801 + HeaderBuf[4]);;
			TxBuf[SIZEOFREQFLASHID] = (unsigned char)(CheckSumVal);
			TxBuf[SIZEOFREQFLASHID + 1] = (unsigned char)(CheckSumVal >> 8);
			TxBuf[SIZEOFREQFLASHID + 2] = 0x03;
			TxBuf[SIZEOFREQFLASHID + 3] = 0x00;
			
			HeaderBuf[0] = 0x02;
			HeaderBuf[1] = 0x00;

			if (putsB((char*)HeaderBuf, IFX_PACKET_HEADER_SIZE, 100000) == LG_DOWNLOAD_FAIL)
				while(1);
			//return LG_DOWNLOAD_FAIL;

			if (putsB((char*)TxBuf, SIZEOFREQFLASHID + 4, 100000) == LG_DOWNLOAD_FAIL)
				while(1);
			if (Lg_uartRxLen(HeaderBuf, IFX_PACKET_HEADER_SIZE, 6000000) == LG_DOWNLOAD_FAIL)
				while(1);
			//return LG_DOWNLOAD_FAIL;

	
			if (HeaderBuf[2] != 0x01 || HeaderBuf[3] != 0x08)
				while(1);

			payload_size = HeaderBuf[4] + (unsigned int)(HeaderBuf[5] << 8);
			payload_size += 4; 
			
			if (Lg_uartRxLen((char*)TxBuf, payload_size, 3000000) == LG_DOWNLOAD_FAIL)
				while(1);
			//return LG_DOWNLOAD_FAIL;;

			if (TxBuf[0] != 0x00 || TxBuf[1] != 0x00)
				while(1);

			TxPack.type = Req_IFX_Flash_ID;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
			usbtty_bulk_fifo_index = 0;	
	
			break;
		}

		case Req_IFX_Cfi_Info_1:
		{
			usbtty_bulk_fifo_index = 0;
			
			if (IFXReqCfiInfo_1() == LG_DOWNLOAD_FAIL)
				while(1);

			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));			
			break;
		}		

		case Req_IFX_Cfi_Info_2:
		{
			usbtty_bulk_fifo_index = 0;			

			if (IFXReqCfiInfo_2() == LG_DOWNLOAD_FAIL)
				while(1);
				
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
			break;
		}

		case Req_IFX_Start:
		{
			unsigned int len;

			struct prot_ifx_req_start_t	Rxwrite;
			size_t retlen;
			unsigned short		file_size;
			unsigned int		payload_size = 0;
			unsigned short  	CheckSumVal;	

			memcpy((void*)&Rxwrite, (void*)pCmd_buf, sizeof(struct prot_ifx_req_start_t));

			while (Rxwrite.len > usbtty_bulk_fifo_index - CurrentFileIndex)
			{
				int poll_status;
				
				poll_status = poll_usbtty();

				if (TTYUSB_ERROR == poll_status) {
					break;
				} else if (TTYUSB_DISCONNECT == poll_status) {
					printf("USBtty disconnect detected\n");
					usbtty_shutdown();
					printf("USB term shutdown\n");
					usbtty_init();
					printf("USBtty Reinitialised\n");
				}	
			}	

			file_size = Rxwrite.len;
			//memcpy((void*)TxBuf, (void*)&usbtty_bulk_fifo[CurrentFileIndex], file_size);
			memset(HeaderBuf, 0x00, IFX_PACKET_HEADER_SIZE);
			HeaderBuf[2] = 0x04; // Type
			HeaderBuf[3] = 0x02; // Type
			HeaderBuf[4] = 0x00; // size
			HeaderBuf[5] = 0x08; // size

			//CheckSumVal = Lg_Checksum_ShortB(TxBuf, file_size);
			CheckSumVal = Lg_Checksum_ShortB(&usbtty_bulk_fifo[CurrentFileIndex], file_size);
			CheckSumVal += (0x204 + 0x800);
			//TxBuf[file_size] = (unsigned char)(CheckSumVal);
			//TxBuf[file_size + 1] = (unsigned char)(CheckSumVal >> 8);
			//TxBuf[file_size + 2] = 0x03;
			//TxBuf[file_size + 3] = 0x00;
			usbtty_bulk_fifo[CurrentFileIndex + file_size] = (unsigned char)(CheckSumVal);
			usbtty_bulk_fifo[CurrentFileIndex + file_size + 1] =  (unsigned char)(CheckSumVal >> 8);;
			usbtty_bulk_fifo[CurrentFileIndex + file_size + 2] = 0x03;
			usbtty_bulk_fifo[CurrentFileIndex + file_size + 3] = 0x00;
			
			HeaderBuf[0] = 0x02;
			HeaderBuf[1] = 0x00;

			if (putsB((char*)HeaderBuf, IFX_PACKET_HEADER_SIZE, 100000) == LG_DOWNLOAD_FAIL)
				while(1);
			//return LG_DOWNLOAD_FAIL;

			//if (putsB((char*)TxBuf, file_size + 4, 100000) == LG_DOWNLOAD_FAIL)
			if (putsB((char*)&usbtty_bulk_fifo[CurrentFileIndex], file_size +4, 1000000) == LG_DOWNLOAD_FAIL)
				while(1);
		    if (Lg_uartRxLen(HeaderBuf, IFX_PACKET_HEADER_SIZE, 8000000) == LG_DOWNLOAD_FAIL)
			    while(1);
			//return LG_DOWNLOAD_FAIL;
	
			if (HeaderBuf[2] != 0x04 || HeaderBuf[3] != 0x02)
				while(1);

			payload_size = HeaderBuf[4] + (unsigned int)(HeaderBuf[5] << 8);
			payload_size += 4; 
			
			if (Lg_uartRxLen((char*)TxBuf, payload_size, 3000000) == LG_DOWNLOAD_FAIL)
				while(1);
			//return LG_DOWNLOAD_FAIL;;

			if (!(TxBuf[0] == 0x00 || TxBuf[0] == 0x01))
				while(1);

			if (TxBuf[1] != 0x00)				
				while(1);

			TxPack.type = Req_IFX_Start;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
			usbtty_bulk_fifo_index = 0;				
			break;
		}

		case Req_IFX_EraseAll:
		{

			usbtty_bulk_fifo_index = 0;			

			if (IFXReqFlashEraseStart(TRUE) == LG_DOWNLOAD_FAIL)
				while(1);
				
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
			
			break;
		}

		case Req_IFX_EraseCheck:
		{
			usbtty_bulk_fifo_index = 0;			

			if (IFXReqFlashEraseCheck() == LG_DOWNLOAD_FAIL)
				while(1);
				
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
						
			break;
		}

		case Req_IFX_End:
		{
			usbtty_bulk_fifo_index = 0;			

			if (IFXReqEnd() == LG_DOWNLOAD_FAIL)
				while(1);
				
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
			break;
		}

		case Req_IFX_Flash_Set_Address:
		{
			struct prot_ifx_flash_set_address_t	Rxwrite;

			memcpy((void*)&Rxwrite, (void*)pCmd_buf, sizeof(struct prot_ifx_flash_set_address_t));			

			if (IFXReqFlashSetAddress(Rxwrite.addr) == LG_DOWNLOAD_FAIL)
				while(1);
				
			TxPack.type = Rsp_Ack;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));	
			usbtty_bulk_fifo_index = 0;	
			
			break;
		}

		case Req_IFX_write:
		{
			unsigned int len;

			struct prot_ifx_write_t	Rxwrite;
			size_t retlen;
			unsigned short		file_size;
			unsigned int		payload_size = 0;
			unsigned short  	CheckSumVal;	

			memcpy((void*)&Rxwrite, (void*)pCmd_buf, sizeof(struct prot_ifx_write_t));

			while (Rxwrite.len > usbtty_bulk_fifo_index - CurrentFileIndex)
			{
				int poll_status;
				
				poll_status = poll_usbtty();

				if (TTYUSB_ERROR == poll_status) {
					break;
				} else if (TTYUSB_DISCONNECT == poll_status) {
					printf("USBtty disconnect detected\n");
					usbtty_shutdown();
					printf("USB term shutdown\n");
					usbtty_init();
					printf("USBtty Reinitialised\n");
				}	
			}	 

			file_size = Rxwrite.reallen;
			CheckSumVal = Rxwrite.datacrc;

			memset(HeaderBuf, 0x00, IFX_PACKET_HEADER_SIZE);
			HeaderBuf[2] = 0x04; // Type
			HeaderBuf[3] = 0x08; // Type
			
			HeaderBuf[4] = (unsigned char)(file_size); // size
			HeaderBuf[5] = (unsigned char)(file_size>>8); // size			

			usbtty_bulk_fifo[CurrentFileIndex + file_size] = (unsigned char)(CheckSumVal);
			usbtty_bulk_fifo[CurrentFileIndex + file_size + 1] = (unsigned char)(CheckSumVal >> 8);
			usbtty_bulk_fifo[CurrentFileIndex + file_size + 2] = 0x03;
			usbtty_bulk_fifo[CurrentFileIndex + file_size + 3] = 0x00;			

			HeaderBuf[0] = 0x02;
			HeaderBuf[1] = 0x00;

			if (putsB((char*)HeaderBuf, IFX_PACKET_HEADER_SIZE, 100000) == LG_DOWNLOAD_FAIL)
				while(1);
			//return LG_DOWNLOAD_FAIL;

			if (putsB((char*)&usbtty_bulk_fifo[CurrentFileIndex], file_size + 4, 1000000) == LG_DOWNLOAD_FAIL)
				while(1);
			//return LG_DOWNLOAD_FAIL;
				
			if (Lg_uartRxLen(HeaderBuf, IFX_PACKET_HEADER_SIZE, 3000000) == LG_DOWNLOAD_FAIL)
				while(1);
			//return LG_DOWNLOAD_FAIL;	

			if (HeaderBuf[2] != 0x04 || HeaderBuf[3] != 0x08)
				while(1);

			payload_size = HeaderBuf[04] + (unsigned int)(HeaderBuf[05] << 8);
			payload_size += 4;	

			if (Lg_uartRxLen((char*)TxBuf, payload_size, 3000000) == LG_DOWNLOAD_FAIL)
				while(1);
			//return LG_DOWNLOAD_FAIL;;
			
			// I can't manage error case.
			//if (TxBuf[0] != 0x00 || TxBuf[1] != 0x00)
			//	while(1);
			usbtty_bulk_fifo_index = 0;	
			
			TxPack.type = Req_IFX_write;
			hdlc_send_packet ((unsigned char*)&TxPack, sizeof(struct prot_rsp_t));				
			usbtty_bulk_fifo_index = 0;				
			break;			
		}
				 
		default:
			{
					break;
			}
	}
	free(pRxPack);
}


static unsigned int parsingCmd(byte *pBuf)
{
	  enum {
		HDLC_HUNT_FOR_FLAG, 	/* Waiting for a flag to start a packet 	  */
		HDLC_GOT_FLAG,			/* Have a flag, expecting the packet to start */
		HDLC_GATHER,			/* In the middle of a packet				  */
		HDLC_PACKET_RCVD		/* Now have received a complete packet		  */
	  } state;
	  /* State variable for decoding async HDLC */
	  

	  int	chr;
		/* Current character being received */
	
	  unsigned int	len = 0;
		/* Length of packet collected so far */
	
	  unsigned short  crc = 0;
		/* Cyclic Redundancy Check, computed as we go. */

	  int	index = 0;
	
	  /*lint -esym(644,len,crc) */
	  /* Lint can't tell that the state machine guarantees that
		 we initialize len and crc before use */
	
	/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	
	 /* Look at characters and try to find a valid async-HDLC packet of
	  ** length at least MIN_PACKET_LEN with a valid CRC.
	  ** Keep looking until we find one. */
	  CurrentFileIndex = 0;
		
	  for (state = HDLC_HUNT_FOR_FLAG; state != HDLC_PACKET_RCVD; /* nil */) {
	  	if (CurrentFileIndex >= usbtty_bulk_fifo_index)
			break;
		
		chr = usbtty_bulk_fifo[CurrentFileIndex++];
	
		/* If it's an error ... */
		if (chr == SIO_RX_ERR) {
				/* Start over. */
		  state = HDLC_HUNT_FOR_FLAG;
		  continue;
		}
	
		/* communication timed out */
		if (chr == SIO_TIMEOUT) {
		  /* to do... */
		}
	
			/* Process according to which state */
		switch(state) {
		  /*lint -esym(788,HDLC_PACKET_RCVD)  No need to deal with HDLC_PACKET_RCVD
		  since we're in a loop that guarantees we're not in that state. */
		  case HDLC_HUNT_FOR_FLAG:		   /* We're looking for a flag ... */
			/*	 and we got one ... 		*/
			if (chr == ASYNC_HDLC_FLAG) {
			  state = HDLC_GOT_FLAG;	   /*	so go on to the next step. */
			}
			break;
	
				/* Had a flag, now expect a packet */
		  case HDLC_GOT_FLAG:			   
					/* Oops, another flag.	No change. */
			if (chr == ASYNC_HDLC_FLAG) {
			  break;
			}
			else {
				/* Ah, we can really begin a packet */
			  len = 0;					   /* The packet starts out empty	   */
			  crc = CRC_16_L_SEED;		   /* and the CRC in its initial state */
			  state = HDLC_GATHER;		   /* and we begin to gather a packet  */
			  /* Fall through */		   /*	(starting with this byte)	   */
			}
	
		  case HDLC_GATHER: 				  /* We're gathering a packet */
					/* We've reached the end		 */
			if (chr == ASYNC_HDLC_FLAG) {
						/* Reject any too-short packets  */
			  if (len < MIN_PACKET_LEN) {
				/* Send NAK 	  */
				state = HDLC_HUNT_FOR_FLAG; 				 /* Start over */
			  }
						/* Reject any with bad CRC */
			  else if (crc != CRC_16_L_OK_NEG) {
				/* Send NAK 	  */
#ifdef DLOAD_DEBUG             
				printf ("RCV CRC ERROR!\r\n");
#endif            
				state = HDLC_HUNT_FOR_FLAG; 				 /* Start over */
			  }
						/* Yay, it's a good packet! */
			  else {
				state = HDLC_PACKET_RCVD;					 /* Done for now   */
			  }
			  break;		   /* However it turned out, this packet is over.  */
			}
	
			/* It wasn't a flag, so we're still inside the packet. */
					/* If it was an ESC 	  */
			if (chr == ASYNC_HDLC_ESC) {
				if (CurrentFileIndex >= usbtty_bulk_fifo_index)
				break;
				
			  chr = usbtty_bulk_fifo[CurrentFileIndex++];   /* Get next character (wait for it) */
	
						/* If there was an error, */
			  if (chr == SIO_RX_ERR) {
				state = HDLC_HUNT_FOR_FLAG; 		 /* Start over */
				break;
			  }
	
			  chr ^= ASYNC_HDLC_ESC_MASK;			 /* Otherwise, de-mask it  */
			  /* No break; process the de-masked byte normally */
			}
	
					/* Make sure there's room */
			if (len >= MAX_PACKET_LEN) {
			  /* Oops, buffer too full	*/
#ifdef DLOAD_DEBUG            
			  printf ("RCV buffer too full!\r\n");
#endif          
			  state = HDLC_HUNT_FOR_FLAG;			 /* Start over */
			}
			else {
						/* Add byte to buffer */
			  pBuf[len++] = (unsigned char) chr;
						/* Update the CRC */
			  crc = CRC_16_L_STEP(crc, (unsigned short) chr);	   
			}
			break;
	
		  default:		 /* Shouldn't happen with an enum, but for safety ...  */
			state = HDLC_HUNT_FOR_FLAG; 				 /* Start over		   */
			break;
		}
	
	  }

          if(len < 2)
               return 0;

	  return len-2; // except for crc data

}







static int send_buffer (unsigned char *buf, int len)
{
	usbtty_tx(buf, len);
	return 0;
}


static int hdlc_send_packet (unsigned char *pBuf, unsigned int wLen)
{
  unsigned int i;
  unsigned short crc = CRC_16_L_SEED;
  unsigned int buffer_pos = 0;
  static unsigned char *buffer = NULL;
	int pack_len = 4096;

  buffer = hdlc_tx_buf;

  if( wLen == 0 )
    return 0;

  for(i = 0; i < wLen; i++) {
    HDLC_ADD_ESC_BUFFER (pBuf[i], pack_len);
    crc = CRC_16_L_STEP (crc, (unsigned short) pBuf[i]);
  }

  crc ^= CRC_16_L_SEED;
  HDLC_ADD_ESC_BUFFER (((unsigned char) crc), pack_len);
  HDLC_ADD_ESC_BUFFER (((unsigned char) ((crc >> 8) & 0xFF)), pack_len);
  HDLC_ADD_BUFFER (ASYNC_HDLC_FLAG, pack_len);

  if (buffer_pos) {
    send_buffer (buffer, buffer_pos);
  }
 
  return 1;
}


 static int nonhdlc_send_packet (unsigned char *pCmdBuf, unsigned int wLen, unsigned char *pDataBuf, unsigned int dLen)
 {
   unsigned int i;
   unsigned short crc = CRC_16_L_SEED;
   unsigned int buffer_pos = 0;
   static unsigned char *buffer = NULL;
   int pack_len = 4096;
   unsigned char CmdBuf[30];
 
   buffer = CmdBuf;//hdlc_tx_buf;
 
   if( wLen == 0 )
 	 return 0;
 
   for(i = 0; i < wLen; i++) {
 	 HDLC_ADD_ESC_BUFFER (pCmdBuf[i], pack_len);
 	 crc = CRC_16_L_STEP (crc, (unsigned short) pCmdBuf[i]);
   }
 
   crc ^= CRC_16_L_SEED;
   HDLC_ADD_ESC_BUFFER (((unsigned char) crc), pack_len);
   HDLC_ADD_ESC_BUFFER (((unsigned char) ((crc >> 8) & 0xFF)), pack_len);
   HDLC_ADD_BUFFER (ASYNC_HDLC_FLAG, pack_len);

   //if (buffer_pos) {
 //	 send_buffer (buffer, buffer_pos);
  // }

   if (pDataBuf) {
   	 send_buffer (pDataBuf, dLen);
   }
  
   return 1;
 }


int Lg_uartRxCharWait(unsigned char ch, unsigned int escape_cnt)
{
	unsigned int		len;
	char				ReadCh;

	memset((void*)test_Tbuf, 0x00, 3);
	strcpy((char*)test_Tbuf, "AT");
	len = 2;

	while (1)
	{
        if (getcB(100000, &ReadCh) == LG_DOWNLOAD_FAIL)
        {
        	if (escape_cnt-- == 0)
				return LG_DOWNLOAD_FAIL;
        }
		else
		{

			if (ReadCh == ch)
				return LG_DOWNLOAD_SUCCESS;
			else
			{
				if (escape_cnt-- == 0)
					return LG_DOWNLOAD_FAIL;
			}
		}

#if defined(CONFIG_COSMO_REV_B) || defined(CONFIG_COSMO_REV_C)
		udelay(50000);	
#else
		udelay(10000);	
#endif
		if (putsB ((char*)test_Tbuf, 2, 100000) == LG_DOWNLOAD_FAIL)
			return LG_DOWNLOAD_FAIL;		
	}

	return LG_DOWNLOAD_FAIL;
}


int Lg_uartRxLen(unsigned char* data, unsigned int len, unsigned int escape_cnt)
{
    int i;

	for (i = 0; i < len; i++)
	{
		if (getcB(escape_cnt, (char*)&data[i]) == LG_DOWNLOAD_FAIL)
		{
			return LG_DOWNLOAD_FAIL;
		}		
	}
	
	return LG_DOWNLOAD_SUCCESS;
}

unsigned char Lg_Checksum_Int(unsigned char* data, unsigned int len)
{
	unsigned char checksum = 0;

	while(len--)
		checksum ^= (data[len]);

	return checksum;
}



unsigned char Lg_Checksum_Short(unsigned char* data, unsigned short len)
{
	unsigned char checksum = 0;

	while(len--)
		checksum ^= (data[len]);

	return checksum;
}


unsigned short Lg_Checksum_ShortB(unsigned char* data, unsigned short len)
{
	unsigned short checksum = 0;

	while(len--)
		checksum += (data[len]);

	return checksum;
}




#define SIZEOFREQSETPROTCONF	72
int IFXReqSetProtConf(unsigned char* pBuf)
{
	unsigned int	payload_size = 0;
	unsigned short  CheckSumVal;

	memcpy((void*)TxBuf, (void*)pBuf, SIZEOFREQSETPROTCONF);
	memset(HeaderBuf, 0x00, IFX_PACKET_HEADER_SIZE);
	HeaderBuf[2] = 0x86; // Type
	HeaderBuf[4] = 0x48; // Size

	TxBuf[54] = 0x00;
	TxBuf[55] = 0x00;

	CheckSumVal = Lg_Checksum_ShortB(TxBuf, SIZEOFREQSETPROTCONF);
	CheckSumVal += (HeaderBuf[2] + HeaderBuf[4]);
	TxBuf[SIZEOFREQSETPROTCONF] = (unsigned char)(CheckSumVal);
	TxBuf[SIZEOFREQSETPROTCONF + 1] = (unsigned char)(CheckSumVal >> 8);
	TxBuf[SIZEOFREQSETPROTCONF + 2] = 0x03;
	TxBuf[SIZEOFREQSETPROTCONF + 3] = 0x00;

	HeaderBuf[0] = 0x02;
	HeaderBuf[1] = 0x00;

	if (putsB((char*)HeaderBuf, IFX_PACKET_HEADER_SIZE, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (putsB((char*)TxBuf, SIZEOFREQSETPROTCONF + 4, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;
		
	if (Lg_uartRxLen(HeaderBuf, IFX_PACKET_HEADER_SIZE, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (HeaderBuf[2] != 0x86)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	payload_size = HeaderBuf[04] + (unsigned int)(HeaderBuf[05] << 8);
	payload_size += 4;

	if (Lg_uartRxLen((char*)TxBuf, payload_size, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;;
		
	if (TxBuf[0] != 0x01 || TxBuf[1] != 0x00)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	return LG_DOWNLOAD_SUCCESS;	
}

#define SIZEOFCNFBAUDCHANGE		4
int IFXCnfBaudChange(void)
{
	unsigned char BaudRate[4] = {0xC0, 0xC6, 0x2D, 0x00};
	//unsigned char BaudRate[4] = {0x00, 0xC2, 0x01, 0x00};		//115200
	unsigned int	payload_size = 0;
	unsigned short  CheckSumVal;

	memcpy(TxBuf, BaudRate, sizeof(BaudRate));
	memset(HeaderBuf, 0x00, IFX_PACKET_HEADER_SIZE);
	HeaderBuf[2] = 0x82; // Type
	HeaderBuf[4] = 0x04; // Size

	CheckSumVal = Lg_Checksum_ShortB(TxBuf, sizeof(BaudRate));
	CheckSumVal += (HeaderBuf[2] + HeaderBuf[4]);
	TxBuf[SIZEOFCNFBAUDCHANGE] = (unsigned char)(CheckSumVal);
	TxBuf[SIZEOFCNFBAUDCHANGE + 1] = (unsigned char)(CheckSumVal >> 8);
	TxBuf[SIZEOFCNFBAUDCHANGE + 2] = 0x03;
	TxBuf[SIZEOFCNFBAUDCHANGE + 3] = 0x00;

	HeaderBuf[0] = 0x02;
	HeaderBuf[1] = 0x00;

	if (putsB((char*)HeaderBuf, IFX_PACKET_HEADER_SIZE, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (putsB((char*)TxBuf, SIZEOFCNFBAUDCHANGE + 4, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;
		
	if (Lg_uartRxLen(HeaderBuf, IFX_PACKET_HEADER_SIZE, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;
	

	if (HeaderBuf[2] != 0x82)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	payload_size = HeaderBuf[04] + (unsigned int)(HeaderBuf[05] << 8);
	payload_size += 4;

	if (Lg_uartRxLen((char*)TxBuf, payload_size, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;;

	if (TxBuf[0] != 0xC0 || TxBuf[1] != 0xC6 || TxBuf[2] != 0x2D)
	//if (TxBuf[0] != 0x00 || TxBuf[1] != 0xC2 || TxBuf[2] != 0x01)  //115200
		while(1);

	return LG_DOWNLOAD_SUCCESS;	
}


#define SIZEOFREQCFIINFO_1		2
int IFXReqCfiInfo_1(void)
{
	unsigned char ReqCfiInfo_1[2] = {0x00, 0x00};
	unsigned int	payload_size = 0;
	unsigned short  CheckSumVal;
	unsigned char	tmp1, tmp2;


	memcpy(TxBuf, ReqCfiInfo_1, sizeof(ReqCfiInfo_1));
	memset(HeaderBuf, 0x00, IFX_PACKET_HEADER_SIZE);
	HeaderBuf[2] = 0x84; // Type
	HeaderBuf[4] = 0x02; // Size

	printf("Lg_Checksum_ShortB start\n");

	CheckSumVal = Lg_Checksum_ShortB(TxBuf, sizeof(ReqCfiInfo_1));
	CheckSumVal += (HeaderBuf[2] + HeaderBuf[4]);
	TxBuf[SIZEOFREQCFIINFO_1] = (unsigned char)(CheckSumVal);
	TxBuf[SIZEOFREQCFIINFO_1 + 1] = (unsigned char)(CheckSumVal >> 8);
	TxBuf[SIZEOFREQCFIINFO_1 + 2] = 0x03;
	TxBuf[SIZEOFREQCFIINFO_1 + 3] = 0x00;


	HeaderBuf[0] = 0x02;
	HeaderBuf[1] = 0x00;


	if (putsB((char*)HeaderBuf, IFX_PACKET_HEADER_SIZE, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (putsB((char*)TxBuf, SIZEOFREQCFIINFO_1 + 4, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;
		
	if (Lg_uartRxLen(HeaderBuf, IFX_PACKET_HEADER_SIZE, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;



	if (HeaderBuf[2] != 0x84)
		while(1);

	payload_size = HeaderBuf[4] + (unsigned int)(HeaderBuf[5] << 8);
	payload_size += 4;
	tmp1 = HeaderBuf[4];
	tmp2 = HeaderBuf[5];


	if (Lg_uartRxLen((char*)TxBuf, payload_size, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;;



#if 1	//FW KIMBYUNGCHUL 20101116 [START]			XG618 : TxBuf[5], 		 XG626 : TxBuf[6]
	if (TxBuf[4] == 0x00 || TxBuf[6] == 0x00)
		while(1);
#else
	if (TxBuf[4] == 0x00 || TxBuf[5] == 0x00)
		while(1);


#endif	//FW KIMBYUNGCHUL 20101116 [END]

	return LG_DOWNLOAD_SUCCESS;
}


#define SIZEOFREQCFIINFO_2		256
int IFXReqCfiInfo_2(void)
{
	unsigned char ReqCfiInfo_2[256] = {0x00, 0x00, 0x00, 0x00, 0x98, 0xB1, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	unsigned int	payload_size = 0;
	unsigned short  CheckSumVal;


	memcpy(TxBuf, ReqCfiInfo_2, sizeof(ReqCfiInfo_2));
	memset(HeaderBuf, 0x00, IFX_PACKET_HEADER_SIZE);
	HeaderBuf[2] = 0x85; // Type
	HeaderBuf[5] = 0x01; // Size


	CheckSumVal = Lg_Checksum_ShortB(TxBuf, sizeof(ReqCfiInfo_2));
	CheckSumVal += (HeaderBuf[2] + 0x100);
	TxBuf[SIZEOFREQCFIINFO_2] = (unsigned char)(CheckSumVal);
	TxBuf[SIZEOFREQCFIINFO_2 + 1] = (unsigned char)(CheckSumVal >> 8);
	TxBuf[SIZEOFREQCFIINFO_2 + 2] = 0x03;
	TxBuf[SIZEOFREQCFIINFO_2 + 3] = 0x00;

	HeaderBuf[0] = 0x02;
	HeaderBuf[1] = 0x00;

	if (putsB((char*)HeaderBuf, IFX_PACKET_HEADER_SIZE, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (putsB((char*)TxBuf, SIZEOFREQCFIINFO_2 + 4, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;
		
	if (Lg_uartRxLen(HeaderBuf, IFX_PACKET_HEADER_SIZE, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (HeaderBuf[2] != 0x85)
		while(1);

	payload_size = HeaderBuf[04] + (unsigned int)(HeaderBuf[05] << 8);
	payload_size += 4;

	if (Lg_uartRxLen((char*)TxBuf, payload_size, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;;

	if (TxBuf[0] != 0xFF || TxBuf[1] != 0xFF)
		while(1);

	return LG_DOWNLOAD_SUCCESS;
}


#define SIZEOFREQFLASHERASEEEP		8
int IFXReqFlashEraseStart(char bEraseAll)
{

	unsigned char   ReqFlashEraseStart[8] = {0x00, 0x00, 0x08, 0x60, 0xFE, 0xFF, 0x17, 0x60};
	unsigned int	payload_size = 0;
	unsigned short  CheckSumVal;


	if (bEraseAll == TRUE)
	{
		ReqFlashEraseStart[2] = 0x00;
		ReqFlashEraseStart[3] = 0x00;
		ReqFlashEraseStart[4] = 0xFE;
		ReqFlashEraseStart[5] = 0xFF;
		ReqFlashEraseStart[6] = 0xFF;
		ReqFlashEraseStart[7] = 0xFF;
	
}

	memcpy(TxBuf, ReqFlashEraseStart, SIZEOFREQFLASHERASEEEP);
	memset(HeaderBuf, 0x00, IFX_PACKET_HEADER_SIZE);
	HeaderBuf[2] = 0x05; // Type
	HeaderBuf[3] = 0x08; // Type
	HeaderBuf[4] = 0x08; // Size

	CheckSumVal = Lg_Checksum_ShortB(TxBuf, SIZEOFREQFLASHERASEEEP);
	CheckSumVal += (0x805 + HeaderBuf[4]);
	TxBuf[SIZEOFREQFLASHERASEEEP] = (unsigned char)(CheckSumVal);
	TxBuf[SIZEOFREQFLASHERASEEEP + 1] = (unsigned char)(CheckSumVal >> 8);
	TxBuf[SIZEOFREQFLASHERASEEEP + 2] = 0x03;
	TxBuf[SIZEOFREQFLASHERASEEEP + 3] = 0x00;

	HeaderBuf[0] = 0x02;
	HeaderBuf[1] = 0x00;


	if (putsB((char*)HeaderBuf, IFX_PACKET_HEADER_SIZE, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (putsB((char*)TxBuf, SIZEOFREQFLASHERASEEEP + 4, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;
	udelay(100000);
		
	if (Lg_uartRxLen(HeaderBuf, IFX_PACKET_HEADER_SIZE, 60000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;


	if (HeaderBuf[2] != 0x05 || HeaderBuf[3] != 0x08)
		while(1);

	payload_size = HeaderBuf[4] + (unsigned int)(HeaderBuf[5] << 8);
	payload_size += 4;

	if (Lg_uartRxLen((char*)TxBuf, payload_size, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;;

	if (TxBuf[0] != 0x00 || TxBuf[1] != 0x00)
		while(1);

	return LG_DOWNLOAD_SUCCESS;
}


#define SIZEOFREQFLASHERASECHECK	2
int IFXReqFlashEraseCheck(void)
{
	unsigned char ReqFlashEraseCheck[2] = {0x00, 0x00};
	unsigned int	payload_size = 0;
	unsigned short  CheckSumVal;


	memcpy(TxBuf, ReqFlashEraseCheck, SIZEOFREQFLASHERASECHECK);
	memset(HeaderBuf, 0x00, IFX_PACKET_HEADER_SIZE);
	HeaderBuf[2] = 0x06; // Type
	HeaderBuf[3] = 0x08; // Type
	HeaderBuf[4] = 0x02; // Size

	CheckSumVal = Lg_Checksum_ShortB(TxBuf, sizeof(ReqFlashEraseCheck));
	CheckSumVal += (0x806 + HeaderBuf[4]);
	TxBuf[SIZEOFREQFLASHERASECHECK] = (unsigned char)(CheckSumVal);
	TxBuf[SIZEOFREQFLASHERASECHECK + 1] = (unsigned char)(CheckSumVal >> 8);
	TxBuf[SIZEOFREQFLASHERASECHECK + 2] = 0x03;
	TxBuf[SIZEOFREQFLASHERASECHECK + 3] = 0x00;

	HeaderBuf[0] = 0x02;
	HeaderBuf[1] = 0x00;


	if (putsB((char*)HeaderBuf, IFX_PACKET_HEADER_SIZE, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (putsB((char*)TxBuf, SIZEOFREQFLASHERASECHECK + 4, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;
		
	if (Lg_uartRxLen(HeaderBuf, IFX_PACKET_HEADER_SIZE, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (HeaderBuf[2] != 0x06 || HeaderBuf[3] != 0x08)
		while(1);

	payload_size = HeaderBuf[4] + (unsigned int)(HeaderBuf[5] << 8);
	payload_size += 4;

	if (Lg_uartRxLen((char*)TxBuf, payload_size, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (TxBuf[0] != 0x01)
		while(1);

	return LG_DOWNLOAD_SUCCESS;
}


#define SIZEOFREQEND		2
int IFXReqEnd(void)
{
	unsigned int	payload_size = 0;
	unsigned short  CheckSumVal;

	unsigned char	ArrReqEnd[2] = {0x00, 0x00};


	memcpy(TxBuf, ArrReqEnd, SIZEOFREQEND);
	memset(HeaderBuf, 0x00, IFX_PACKET_HEADER_SIZE);
	HeaderBuf[2] = 0x05; // Type
	HeaderBuf[3] = 0x02; // Type
	HeaderBuf[4] = 0x02; // Size

	CheckSumVal = Lg_Checksum_ShortB(TxBuf, SIZEOFREQEND);
	CheckSumVal += (0x205 + 0x02);
	TxBuf[SIZEOFREQEND] = (unsigned char)(CheckSumVal);
	TxBuf[SIZEOFREQEND + 1] = (unsigned char)(CheckSumVal >> 8);
	TxBuf[SIZEOFREQEND + 2] = 0x03;
	TxBuf[SIZEOFREQEND + 3] = 0x00;

	HeaderBuf[0] = 0x02;
	HeaderBuf[1] = 0x00;

	if (putsB((char*)HeaderBuf, IFX_PACKET_HEADER_SIZE, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (putsB((char*)TxBuf, SIZEOFREQEND + 4, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;
		
	if (Lg_uartRxLen(HeaderBuf, IFX_PACKET_HEADER_SIZE, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (HeaderBuf[2] != 0x05 || HeaderBuf[3] != 0x02)
		while(1);

	payload_size = HeaderBuf[04] + (unsigned int)(HeaderBuf[05] << 8);
	payload_size += 4;

	if (Lg_uartRxLen((char*)TxBuf, payload_size, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (TxBuf[0] != 0x01 || TxBuf[1] != 0x00)
		while(1);

	return LG_DOWNLOAD_SUCCESS;
}



#define SIZEOFADDRESS		4
unsigned int addressTmp;
int IFXReqFlashSetAddress(unsigned int StartAddr)
{
	unsigned int	payload_size = 0;
	unsigned short  CheckSumVal;

	addressTmp = StartAddr;
	memcpy(TxBuf, (void*)&StartAddr, SIZEOFADDRESS);
	memset(HeaderBuf, 0x00, IFX_PACKET_HEADER_SIZE);
	HeaderBuf[2] = 0x02; // Type
	HeaderBuf[3] = 0x08; // Type
	HeaderBuf[4] = 0x04; // Size

	CheckSumVal = Lg_Checksum_ShortB(TxBuf, SIZEOFADDRESS);
	CheckSumVal += (0x802 + 0x4);
	TxBuf[SIZEOFADDRESS] = (unsigned char)(CheckSumVal);
	TxBuf[SIZEOFADDRESS + 1] = (unsigned char)(CheckSumVal >> 8);
	TxBuf[SIZEOFADDRESS + 2] = 0x03;
	TxBuf[SIZEOFADDRESS + 3] = 0x00;


	HeaderBuf[0] = 0x02;
	HeaderBuf[1] = 0x00;

	if (putsB((char*)HeaderBuf, IFX_PACKET_HEADER_SIZE, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (putsB((char*)TxBuf, SIZEOFADDRESS + 4, 100000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;
		
	if (Lg_uartRxLen(HeaderBuf, IFX_PACKET_HEADER_SIZE, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (HeaderBuf[2] != 0x02 || HeaderBuf[3] != 0x08)
		while(1);

	payload_size = HeaderBuf[04] + (unsigned int)(HeaderBuf[05] << 8);
	payload_size += 4;

	if (Lg_uartRxLen((char*)TxBuf, payload_size, 3000000) == LG_DOWNLOAD_FAIL)
		while(1);
	//return LG_DOWNLOAD_FAIL;

	if (TxBuf[0] != 0x00 || TxBuf[1] != 0x00)
		while(1);

	return LG_DOWNLOAD_SUCCESS;
}
//#endif
#endif

