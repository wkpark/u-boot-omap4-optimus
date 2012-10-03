#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/arch/bits.h>
#include <asm/arch/mem.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/sys_info.h>

#include <environment.h>
#include <command.h>
#include <usbdcore.h>
#include "usbomap.h"

#include <twl4030.h>
#include <twl6030.h>
 
#include <common.h>
#include <asm/types.h>
#include <usbdcore.h>

#include "lg_usb.h"

#include <asm/arch/bits.h>

#if defined(CONFIG_BOOT_DEVICE_EMMC)

#ifdef CONFIG_LGE_USB
#include <linux/mtd/onenand.h>
#include <onenand_uboot.h>
#endif

#include "lg_download.h"

static volatile	u8  *pwr        = (volatile u8  *) OMAP34XX_USB_POWER;
static volatile u16 *csr0       = (volatile u16 *) OMAP34XX_USB_CSR0;
static volatile u8  *index      = (volatile u8  *) OMAP34XX_USB_INDEX;
static volatile u8  *txfifosz   = (volatile u8  *) OMAP34XX_USB_TXFIFOSZ;
static volatile u8  *rxfifosz   = (volatile u8  *) OMAP34XX_USB_RXFIFOSZ;
static volatile u16 *txfifoadd  = (volatile u16 *) OMAP34XX_USB_TXFIFOADD;
static volatile u16 *rxfifoadd  = (volatile u16 *) OMAP34XX_USB_RXFIFOADD;

#define BULK_ENDPOINT_DIAG_IN 	3
#define BULK_ENDPOINT_DIAG_OUT	2
static volatile u16 *peri_rxcsr = (volatile u16 *) OMAP34XX_USB_RXCSR(BULK_ENDPOINT_DIAG_OUT);
static volatile u16 *rxmaxp     = (volatile u16 *) OMAP34XX_USB_RXMAXP(BULK_ENDPOINT_DIAG_OUT);
static volatile u16 *rxcount    = (volatile u16 *) OMAP34XX_USB_RXCOUNT(BULK_ENDPOINT_DIAG_OUT);
static volatile u8 *bulk_fifo_rx= (volatile u8  *) OMAP34XX_USB_FIFO(BULK_ENDPOINT_DIAG_OUT);

static volatile u16 *peri_txcsr = (volatile u16 *) OMAP34XX_USB_TXCSR(BULK_ENDPOINT_DIAG_IN);
static volatile u16 *txmaxp     = (volatile u16 *) OMAP34XX_USB_TXMAXP(BULK_ENDPOINT_DIAG_IN);
static volatile u8  *bulk_fifo  = (volatile u8  *) OMAP34XX_USB_FIFO(BULK_ENDPOINT_DIAG_IN);
#define DMA_CHANNEL 1

#define DATA_RECV  		   32768
#define DATA_SEND_SIZE   	1024

static volatile u16 *peri_dma_cntl	= (volatile u16 *) OMAP34XX_USB_DMA_CNTL_CH(DMA_CHANNEL);
static volatile u32 *peri_dma_addr	= (volatile u32 *) OMAP34XX_USB_DMA_ADDR_CH(DMA_CHANNEL);
static volatile u32 *peri_dma_count	= (volatile u32 *) OMAP34XX_USB_DMA_COUNT_CH(DMA_CHANNEL);

static volatile u32 *otg_sysconfig	= (volatile u32 *) OMAP34XX_OTG_SYSCONFIG;
static volatile u32 *otg_interfsel	= (volatile u32 *) OMAP34XX_OTG_INTERFSEL;
static volatile u32 *otg_forcestdby	= (volatile u32 *) OMAP34XX_OTG_FORCESTDBY;

static volatile u32 *otghs_control = (volatile u32  *) 0x4A00233C;

static unsigned int set_address = 0;
static u8 faddr = 0xff;
static unsigned int high_speed = 0;
static unsigned int deferred_rx = 0;
static struct usb_device_request req;

static char *device_strings[DEVICE_STRING_MAX_INDEX + 1];

static u8 usbtty_req[sizeof(struct usb_device_request) + 4];
static u8 usbtty_fifo[512];
static u8 rx_buffer[DATA_RECV] __attribute__ ((aligned(0x4)));
#ifndef CONFIG_LGE_USB
static u8 *usbtty_bulk_fifo;
char *g_buffer;
unsigned int l_size = 0;
#else
u8 *usbtty_bulk_fifo = (u8 *)(0x90000000);
unsigned int usbtty_bulk_fifo_index = 0;
loff_t g_nand_offs;
int    g_nand_len;
extern struct onenand_chip onenand_chip;

#endif

extern int		DownloadValid;

static int usb_connected = 0;

#ifdef CONFIG_COSMO_SU760
int go_usbdownload (void)
#else
int go_usbdownload (int x)
#endif
{
	int poll_status;
	ulong tmo, tmp;

	muic_AP_USB();

#ifndef CONFIG_LGE_USB
	g_buffer = (char *)RAM_ADDR;
	usbtty_bulk_fifo = (u8 *)RAM_ADDR;

	memset(usbtty_bulk_fifo, 0, (1024 * 1024 * 10));
	l_size = 0;
#else
	g_nand_offs = (loff_t)NAND_OFFSET;
	g_nand_len  = (int)NAND_LENGTH;
#endif
	usbtty_init();
	printf("USB Initialised \n");

#if ! defined (CONFIG_LGE_CX2) 
	tmo = 5000000 / 1000;		
	tmo *= CFG_HZ;			
	tmo /= 1000;			

	extern ulong get_timer (ulong base);
	extern ulong get_timer_masked (void);
	extern void reset_timer_masked (void);
	
#ifndef CONFIG_COSMO_SU760
	 extern int  download_start;
#endif	
	tmp = get_timer (0);		
	if ( (tmo + tmp + 1) < tmp )
		reset_timer_masked ();	
	else
		tmo += tmp; 			

	int time=0;
	int flag=0;

#endif
	
	while(1){
#if ! defined(CONFIG_LGE_CX2) && ! defined(CONFIG_LGE_P2)
		if (get_timer_masked () >= tmo && DownloadValid == 0)
		{
			usbtty_shutdown();
			return;
		}
#endif
					
		poll_status = poll_usbtty();
	
		if (TTYUSB_ERROR == poll_status || TTYUSB_DISCONNECT == poll_status) {
		
			usbtty_shutdown();
			printf("USB term shutdown\n");
			usbtty_init();
			printf("USBtty Reinitialised\n");
		}

		extern int web_download_mode;

		if(web_download_mode!=1)
		{
#if ! defined(CONFIG_LGE_CX2) && ! defined(CONFIG_LGE_P2)

			time++;
			if(poll_status == 0)
				flag =1;

			if((poll_status != 0)&&(time>1000000)&&(flag ==0))
			{
				printf("pool_status = %d\n",poll_status);
				return 1;
			}
#ifndef CONFIG_COSMO_SU760
			if((poll_status != 0)&&(x==1)&&(download_start != 1) &&(time>250000) )
				return 1;
#endif
#endif
		}
#ifdef CONFIG_LGE_USB
		Main_Event_Process();
#endif
	}
}

int usbtty_fifo_size(void)
{
	return high_speed ? RX_ENDPOINT_MAXIMUM_PACKET_SIZE_2_0 :
			 RX_ENDPOINT_MAXIMUM_PACKET_SIZE_1_1;
}

__inline static int calculate_chksum(u8 *buff)
{
	u32 tmp = 0;
	u32 index;
	u8  tmpv;
	u8 *tempbuff = rx_buffer;

	for(index = 0; index < (DATA_RECV - 4); index++){
		tmpv = *buff++;
		*tempbuff++ = tmpv;
	         tmp += tmpv;

	}
	tmp %= 31;

	if(*(u32 *)buff == tmp){
	
		return 0;
	}
	else{
		printf("BAD CRC!!!!\t");
		printf("buffer - 0x%x\t calculate val 0x%x\n", *(u32 *)buff,tmp );
	}
	return -1;
} 

__inline static int usbtty_rx_handler (unsigned char *buffer, unsigned int buffer_size)
{
#if 0
	if(calculate_chksum(buffer) < 0)
		return -1; 
	
        usbtty_tx((g_buffer), DATA_SEND_SIZE);
#endif
	int i;
	
	for(i = 0; i < buffer_size; i++)	printf("%c ",buffer[i]);

        usbtty_tx(buffer, buffer_size);

	return 0;
}

static void usbtty_bulk_endpoint_reset (void)
{
	u8 old_index; 

	old_index = *index;

	*index = BULK_ENDPOINT_DIAG_IN;

	*txfifoadd = MUSB_EP0_FIFOSIZE >> 3;

	*txfifosz = high_speed ? TX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS_2_0 :
			TX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS_1_1;

	*index = BULK_ENDPOINT_DIAG_OUT;

#if 0
	if (high_speed) {
		*rxfifosz =  
#if 1
			RX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS_2_12; 
#else
			RX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS_2_0 | MUSB_RXFIFOSZ_DPB; 
#endif
		*rxfifoadd = (MUSB_EP0_FIFOSIZE + TX_ENDPOINT_MAXIMUM_PACKET_SIZE_2_0) >> 3;
	} else {
		*rxfifosz = RX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS_1_1 |
			MUSB_RXFIFOSZ_DPB;
		*rxfifoadd = (MUSB_EP0_FIFOSIZE + TX_ENDPOINT_MAXIMUM_PACKET_SIZE_1_1) >> 3;
	}
#else
  if (high_speed) {
    *rxfifosz = RX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS_2_12; 
    *rxfifoadd = (MUSB_EP0_FIFOSIZE + TX_ENDPOINT_MAXIMUM_PACKET_SIZE_2_0) >> 3;
  } else {
    *rxfifosz = RX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS_2_12 - 1; 
    *rxfifoadd = (MUSB_EP0_FIFOSIZE + TX_ENDPOINT_MAXIMUM_PACKET_SIZE_1_1) >> 3;
  }
#endif
	
	*index = old_index;

#if 1 
	
  if (high_speed) {
  	*rxmaxp = ((((8 - 1) & 0x0F) << 11) | 0x200);
  } else {
    *rxmaxp = ((((32 - 1) & 0x1F) << 11) | 0x040);
  }
#else
	*rxmaxp =  (0x400);
#endif

	while (*peri_rxcsr & MUSB_RXCSR_RXPKTRDY)
	{
		*peri_rxcsr |= MUSB_RXCSR_FLUSHFIFO;
		udelay(1);
	}
	
	*peri_rxcsr &= ~(MUSB_RXCSR_DMAENAB | MUSB_RXCSR_P_ISO);
	
	*peri_rxcsr |= MUSB_RXCSR_CLRDATATOG;

	*txmaxp = usbtty_fifo_size();

	while (*peri_txcsr & MUSB_TXCSR_FIFONOTEMPTY)
	{
		*peri_txcsr |= MUSB_TXCSR_FLUSHFIFO;
		udelay(1);
	}

	*peri_txcsr &= ~(MUSB_TXCSR_DMAENAB | MUSB_TXCSR_P_ISO | MUSB_TXCSR_P_UNDERRUN);
	
	*peri_txcsr |= (MUSB_TXCSR_CLRDATATOG | MUSB_TXCSR_MODE);
}

static void usbtty_reset (void)
{

#if 0

	*otg_sysconfig = (OMAP34XX_OTG_SYSCONFIG_NO_STANDBY_MODE |
			  OMAP34XX_OTG_SYSCONFIG_NO_IDLE_MODE);

	*otg_interfsel = OMAP34XX_OTG_INTERFSEL_OMAP;

	*otg_forcestdby &= ~OMAP34XX_OTG_FORCESTDBY_STANDBY;
#endif

	*pwr &= ~MUSB_POWER_SOFTCONN;
	udelay(2 * 500000); 

	faddr = 0xff;

#ifdef CONFIG_USB_1_1_DEVICE
	*pwr &= ~MUSB_POWER_HSENAB;
	*pwr |= MUSB_POWER_SOFTCONN;
#else
	*pwr |= (MUSB_POWER_SOFTCONN | MUSB_POWER_HSENAB);
#endif
	
	usbtty_bulk_endpoint_reset ();

}

static u8 read_fifo_8(void)
{
	u8 val;
	val = inb (OMAP34XX_USB_FIFO_0);
	return val;
}

static u8 read_bulk_fifo_8(void)
{
	u8 val = *bulk_fifo_rx;
	return val;
}

__inline int waitfor_completion(void)
{
	int err = 0;

	while (1) {
        	if (MUSB_DMA_CNTL_ERR & *peri_dma_cntl) {
			err = 1;
                        break;
               	}
		if (0 == *peri_dma_count)
			break;
       	}
	*peri_rxcsr &= ~(MUSB_RXCSR_DMAENAB |
		 MUSB_RXCSR_DMAMODE);
	*peri_rxcsr &= ~MUSB_RXCSR_RXPKTRDY;	
	return err;	
				
}

__inline static int read_bulk_fifo_dma(u8 *buf, u32 size)
{

	*peri_rxcsr &= ~MUSB_RXCSR_AUTOCLEAR;
	*peri_rxcsr |= MUSB_RXCSR_DMAENAB;
	*peri_rxcsr |= MUSB_RXCSR_DMAMODE;
	*peri_rxcsr |= MUSB_RXCSR_DMAENAB;

	*peri_dma_addr = (u32) buf;
	
	*peri_dma_count = size;

	*peri_dma_cntl =
		MUSB_DMA_CNTL_BUSRT_MODE_3 |
		MUSB_DMA_CNTL_END_POINT(BULK_ENDPOINT_DIAG_OUT) |
		MUSB_DMA_CNTL_MODE_1 |
		MUSB_DMA_CNTL_WRITE |
		MUSB_DMA_CNTL_ENABLE;

	return waitfor_completion();	
		
}

static void write_fifo_8(u8 val)
{
	outb (val, OMAP34XX_USB_FIFO_0);
}

static inline void write_bulk_fifo_8(u8 val)
{
	*bulk_fifo = val;
}

static void read_request(void)
{
	int i;

	for (i = 0; i < 8; i++)
		usbtty_req[i] = read_fifo_8 ();
	memcpy (&req, &usbtty_req[0], 8);

	*csr0 |= MUSB_CSR0_P_SVDRXPKTRDY;

	while  (*csr0 & MUSB_CSR0_RXPKTRDY)
		udelay(1);
}

static int do_usb_req_set_interface(void)
{
	
	if ((0 == req.wIndex) &&
	    (0 == req.wValue))	{
		usbtty_bulk_endpoint_reset ();
		ACK_REQ();
	}
	else {
		NAK_REQ();
	}

	return TTYUSB_OK;
}

static int do_usb_req_set_address(void)
{
	int ret = TTYUSB_OK;

	if (0xff == faddr)
	{
		faddr = (u8) (req.wValue & 0x7f);
		set_address = 1;

		if (*pwr & MUSB_POWER_HSMODE)
		  high_speed = 1;
		else
		  high_speed = 0;
		printf("Connection is %s speed\n",
			 high_speed?"HIGH":"FULL");

		ACK_REQ();
	}
	else
	{
		NAK_REQ();
	}

	return ret;
}

static int do_usb_req_set_configuration(void)
{
	int ret = TTYUSB_OK;

	if (0xff == faddr) {
		NAK_REQ();
	} else {
		if (0 == req.wValue) {
			
			faddr = 0xff;
			ACK_REQ();
		} else if (CONFIGURATION_NORMAL == req.wValue) {

			usbtty_bulk_endpoint_reset();

			ACK_REQ();
		} else {
			
			NAK_REQ();
		}
	}

	return ret;
}

static int do_usb_req_set_feature(void)
{
	int ret = TTYUSB_OK;

	NAK_REQ();

	return ret;
}

volatile int j;
static int do_usb_req_get_descriptor(void)
{
	int ret = TTYUSB_OK;

	if (0 == req.wLength)
	{
		ACK_REQ();
	}
	else
	{
		unsigned int byteLoop;

		if (USB_DT_DEVICE == (req.wValue >> 8))
		{
			struct usb_device_descriptor d;

			d.bLength = MIN(req.wLength, sizeof (d));
			d.bDescriptorType    = USB_DT_DEVICE;
#ifdef CONFIG_USB_1_1_DEVICE
			d.bcdUSB             = 0x110;
#else
			d.bcdUSB             = 0x200;
#endif
			d.bDeviceClass       = 0x02; 
			d.bDeviceSubClass    = 0x00; 
			d.bDeviceProtocol    = 0x00; 

			d.bMaxPacketSize0    = 0x40;
			d.idVendor           = DEVICE_VENDOR_ID;
			d.idProduct          = DEVICE_PRODUCT_ID;
			d.bcdDevice          = DEVICE_BCD;
			d.iManufacturer      = DEVICE_STRING_MANUFACTURER_INDEX;
			d.iProduct           = DEVICE_STRING_PRODUCT_INDEX;
			d.iSerialNumber      = 0;
			d.bNumConfigurations = 1;

			memcpy (&usbtty_fifo, &d, d.bLength);
			for (byteLoop = 0; byteLoop < d.bLength; byteLoop++)
				write_fifo_8 (usbtty_fifo[byteLoop]);

			TX_LAST();
		}
		else if (USB_DT_CONFIG == (req.wValue >> 8))
		{
			struct usb_configuration_descriptor c;
			struct usb_interface_descriptor i[6];
				
			struct usb_class_header_function_descriptor fh;
			struct usb_class_call_management_descriptor cm;
			struct usb_class_abstract_control_descriptor acm;
			struct usb_class_union_function_descriptor uf;
			
			struct usb_endpoint_descriptor e1, e2, e11;

			int bytes_remaining = req.wLength;
			unsigned char bytes_total = 0;
			
			c.bLength             = MIN(bytes_remaining, sizeof (c));
			c.bDescriptorType     = USB_DT_CONFIG;

			c.wTotalLength = sizeof (c) + 
				sizeof(i[0]) + sizeof(e11) + sizeof(fh) + sizeof (cm) + sizeof (acm) + sizeof(uf) + 
				sizeof(i[1])+ sizeof(e1) + sizeof(e2) +
				sizeof(i[2])+ sizeof(e1) + sizeof(e2);
			
 			
			
			c.bNumInterfaces      = 3; 

			c.bConfigurationValue = CONFIGURATION_NORMAL;
			c.iConfiguration      = DEVICE_STRING_CONFIG_INDEX;
			c.bmAttributes        = 0xA0;
			c.bMaxPower           = 0xFA; 

			memcpy (&usbtty_fifo[0], &c, c.bLength);
			bytes_total += c.bLength;
			bytes_remaining -= c.bLength;

			if (bytes_remaining <= 0){
				for (byteLoop = 0; byteLoop < bytes_total; byteLoop++)
					write_fifo_8 (usbtty_fifo[byteLoop]);
			
				TX_LAST();
				return 0;
			}

			e1.bDescriptorType    = USB_DT_ENDPOINT;
			e1.bmAttributes       = USB_ENDPOINT_XFER_BULK;
			if (high_speed) e1.wMaxPacketSize = TX_ENDPOINT_MAXIMUM_PACKET_SIZE_2_0;
			else	e1.wMaxPacketSize = TX_ENDPOINT_MAXIMUM_PACKET_SIZE_1_1;
			e1.bInterval          = 0x00;

			e2.bDescriptorType    = USB_DT_ENDPOINT;
			e2.bmAttributes       = USB_ENDPOINT_XFER_BULK;
			if (high_speed)	e2.wMaxPacketSize = RX_ENDPOINT_MAXIMUM_PACKET_SIZE_2_0;
			else	e2.wMaxPacketSize = RX_ENDPOINT_MAXIMUM_PACKET_SIZE_1_1;
			e2.bInterval          = 0x00;

			i[0].bLength             = MIN (bytes_remaining, sizeof(struct usb_interface_descriptor));
			i[0].bDescriptorType     = USB_DT_INTERFACE;
			i[0].bInterfaceNumber    = 0x00;
			i[0].bAlternateSetting   = 0x00;
			i[0].bNumEndpoints       = 0x01;
			i[0].bInterfaceClass     = 0x02; 
			i[0].bInterfaceSubClass  = 0x02; 
			i[0].bInterfaceProtocol  = 0x01; 

			memcpy (&usbtty_fifo[bytes_total], &i[0], i[0].bLength);
			bytes_total += i[0].bLength;
			bytes_remaining -= i[0].bLength;

			fh.bFunctionLength   =	MIN (bytes_remaining, sizeof(fh));	
			fh.bDescriptorType   = 0x24;
			fh.bDescriptorSubtype = 0x00; 
			fh.bcdCDC	      = 0x0110; 
			memcpy (&usbtty_fifo[bytes_total], &fh, fh.bFunctionLength);
			bytes_total += fh.bFunctionLength;
			bytes_remaining -= fh.bFunctionLength;

			cm.bFunctionLength    =	MIN (bytes_remaining, sizeof(cm));	
			cm.bDescriptorType    = 0x24;
			cm.bDescriptorSubtype  = 0x01;
			
			cm.bmCapabilities = 0x00; 
			cm.bDataInterface = 0x01;
			memcpy (&usbtty_fifo[bytes_total], &cm, cm.bFunctionLength);
			bytes_total += cm.bFunctionLength;
			bytes_remaining -= cm.bFunctionLength;

			acm.bFunctionLength =	MIN (bytes_remaining, sizeof(acm));	
			acm.bDescriptorType = 0x24;
			acm.bDescriptorSubtype = 0x02;

			acm.bmCapabilities   = 0x02;
			memcpy (&usbtty_fifo[bytes_total], &acm, acm.bFunctionLength);
			bytes_total += acm.bFunctionLength;
			bytes_remaining -= acm.bFunctionLength;

			uf.bFunctionLength     = MIN (bytes_remaining, sizeof(uf));	
			uf.bDescriptorType     = 0x24;
			uf.bDescriptorSubtype  = 0x06;
			uf.bMasterInterface    = 0x00;
			uf.bSlaveInterface0[0]    = 0x01;
			memcpy (&usbtty_fifo[bytes_total], &uf, uf.bFunctionLength);
			bytes_total += uf.bFunctionLength;
			bytes_remaining -= uf.bFunctionLength;

			e11.bLength            = MIN (bytes_remaining, sizeof (e1));
			e11.bDescriptorType    = USB_DT_ENDPOINT;
			e11.bEndpointAddress   = 0x82; 
			e11.bmAttributes     = USB_ENDPOINT_XFER_INT;  
			e11.wMaxPacketSize  = 0x0A;  
			e11.bInterval       = 0x09; 
			memcpy (&usbtty_fifo[bytes_total], &e11, e11.bLength);
			bytes_total += e11.bLength;
			bytes_remaining -= e11.bLength;

			i[1].bLength             = MIN (bytes_remaining, sizeof(struct usb_interface_descriptor));
			i[1].bDescriptorType     = USB_DT_INTERFACE;
			i[1].bInterfaceNumber    = 0x01;
			i[1].bAlternateSetting   = 0x00;
			i[1].bNumEndpoints       = 0x02;
			i[1].bInterfaceClass     = 0x0A; 

			memcpy (&usbtty_fifo[bytes_total], &i[1], i[1].bLength);
			bytes_total += i[1].bLength;
			bytes_remaining -= i[1].bLength;

			e1.bLength            = MIN (bytes_remaining, sizeof (e1));

			e1.bEndpointAddress   = 0x83; 
			memcpy (&usbtty_fifo[bytes_total], &e1, e1.bLength);
			bytes_total += e1.bLength;
			bytes_remaining -= e1.bLength;

			e2.bLength            = MIN (bytes_remaining, sizeof (e2));

			e2.bEndpointAddress   = 0x2; 
			memcpy (&usbtty_fifo[bytes_total], &e2, e2.bLength);
			bytes_total += e2.bLength;
			bytes_remaining -= e2.bLength;

			i[2].bLength             = MIN (bytes_remaining, sizeof(struct usb_interface_descriptor));
			i[2].bDescriptorType     = USB_DT_INTERFACE;
			i[2].bInterfaceNumber    = 0x02;
			i[2].bAlternateSetting   = 0x00;
			i[2].bNumEndpoints       = 0x02;
			i[2].bInterfaceClass     = 0xFF; 
			i[2].bInterfaceSubClass  = 0xFF; 
 			i[2].bInterfaceProtocol  = 0xFF; 
			memcpy (&usbtty_fifo[bytes_total], &i[2], i[2].bLength);
			bytes_total += i[2].bLength;
			bytes_remaining -= i[2].bLength;

			e1.bLength            = MIN (bytes_remaining, sizeof (e1));
			e1.bEndpointAddress   = 0x83; 
			memcpy (&usbtty_fifo[bytes_total], &e1, e1.bLength);
			bytes_total += e1.bLength;
			bytes_remaining -= e1.bLength;

			e2.bLength            = MIN (bytes_remaining, sizeof (e2));
			e2.bEndpointAddress   = 0x02; 
			memcpy (&usbtty_fifo[bytes_total], &e2, e2.bLength);
			bytes_total += e2.bLength;
			bytes_remaining -= e2.bLength;
#if 0
		
			i[3].bLength             = MIN (bytes_remaining, sizeof(struct usb_interface_descriptor));
			i[3].bDescriptorType     = USB_DT_INTERFACE;
			i[3].bInterfaceNumber    = 0x03;
			i[3].bAlternateSetting   = 0x00;
			i[3].bNumEndpoints       = 0x03;
			i[3].bInterfaceClass     = 0xFF; 
			i[3].bInterfaceSubClass  = 0xFF; 
 			i[3].bInterfaceProtocol  = 0xFF; 
			memcpy (&usbtty_fifo[bytes_total], &i[3], i[3].bLength);
			bytes_total += i[3].bLength;
			bytes_remaining -= i[3].bLength;

			e1.bLength            = MIN (bytes_remaining, sizeof (e1));
			e1.bEndpointAddress   = 0x84; 
			memcpy (&usbtty_fifo[bytes_total], &e1, e1.bLength);
			bytes_total += e1.bLength;
			bytes_remaining -= e1.bLength;

			e2.bLength            = MIN (bytes_remaining, sizeof (e2));
			e2.bEndpointAddress   =0x03; 
			memcpy (&usbtty_fifo[bytes_total], &e2, e2.bLength);
			bytes_total += e2.bLength;
			bytes_remaining -= e2.bLength;

			e3.bLength            = MIN (bytes_remaining, sizeof (e3));
			e3.bDescriptorType    = USB_DT_ENDPOINT;
			e3.bEndpointAddress   = 0x85; 
			e3.bmAttributes       = USB_ENDPOINT_XFER_INT;
			e3.wMaxPacketSize     = 0x0A; 
			e3.bInterval          = 0x09; 
			memcpy (&usbtty_fifo[bytes_total], &e3, e3.bLength);
			bytes_total += e3.bLength;
			bytes_remaining -= e3.bLength;

			i[4].bLength             = MIN (bytes_remaining, sizeof(struct usb_interface_descriptor));
			i[4].bDescriptorType     = USB_DT_INTERFACE;
			i[4].bInterfaceNumber    = 0x04;
			i[4].bAlternateSetting   = 0x00;
			i[4].bNumEndpoints       = 0x02;
			i[4].bInterfaceClass     = 0x08; 
			i[4].bInterfaceSubClass  = 0x06; 
 			i[4].bInterfaceProtocol  = 0x50; 
			memcpy (&usbtty_fifo[bytes_total], &i[4], i[4].bLength);
			bytes_total += i[4].bLength;
			bytes_remaining -= i[4].bLength;

			e1.bLength            = MIN (bytes_remaining, sizeof (e1));
			e1.bEndpointAddress   = 0x86; 
			memcpy (&usbtty_fifo[bytes_total], &e1, e1.bLength);
			bytes_total += e1.bLength;
			bytes_remaining -= e1.bLength;

			e2.bLength            = MIN (bytes_remaining, sizeof (e2));
			e2.bEndpointAddress   = 0x04; 
			bytes_remaining -= e2.bLength;	
			memcpy (&usbtty_fifo[bytes_total], &e2, e2.bLength);
			bytes_total += e2.bLength;

			i[5].bLength             = MIN (bytes_remaining, sizeof(struct usb_interface_descriptor));
			i[5].bDescriptorType     = USB_DT_INTERFACE;
			i[5].bInterfaceNumber    = 0x05;
			i[5].bAlternateSetting   = 0x00;
			i[5].bNumEndpoints       = 0x02;
			i[5].bInterfaceClass     = 0xFF; 
			i[5].bInterfaceSubClass  = 0xFF; 
 			i[5].bInterfaceProtocol  = 0xFF; 

			bytes_remaining -= i[5].bLength;	
			memcpy (&usbtty_fifo[bytes_total], &i[5], i[5].bLength);
			bytes_total += i[5].bLength;

			e1.bLength            = MIN (bytes_remaining, sizeof (e1));
			e1.bEndpointAddress   = 0x87; 
			bytes_remaining -= e1.bLength;	
			memcpy (&usbtty_fifo[bytes_total], &e1, e1.bLength);
			bytes_total += e1.bLength;

			e2.bLength            = MIN (bytes_remaining, sizeof (e2));
			e2.bEndpointAddress   = 0x05; 
			bytes_remaining -= e2.bLength;	
			memcpy (&usbtty_fifo[bytes_total], &e2, e2.bLength);
			bytes_total += e2.bLength;
#endif
			for (byteLoop = 0; byteLoop < bytes_total; byteLoop++){
				write_fifo_8 (usbtty_fifo[byteLoop]);
				if (byteLoop && ((byteLoop + 1) % 64) == 0){
					*csr0 |= (MUSB_CSR0_TXPKTRDY);	
					while(*csr0 & MUSB_CSR0_TXPKTRDY) j = 1;
				}
			}
			TX_LAST();
			printf("Done sending the descriptors\n");

			usb_connected = 1;

		}
		else if (USB_DT_STRING == (req.wValue >> 8))
		{
			unsigned char bLength;
			unsigned char string_index = req.wValue & 0xff;

			if (string_index > DEVICE_STRING_MAX_INDEX)
			{

			}
			else if (0 == string_index)
			{
				
				bLength = MIN(4, req.wLength);

				usbtty_fifo[0] = bLength;        
				usbtty_fifo[1] = USB_DT_STRING;  
				usbtty_fifo[2] = DEVICE_STRING_LANGUAGE_ID & 0xff;
				usbtty_fifo[3] = DEVICE_STRING_LANGUAGE_ID >> 8;

				for (byteLoop = 0; byteLoop < bLength; byteLoop++)
					write_fifo_8 (usbtty_fifo[byteLoop]);

				TX_LAST();
			}
			else
			{
				
				unsigned char s;
				unsigned char sl = strlen (&device_strings[string_index][0]);

				unsigned char bLength = 2 + (2 * sl);
				bLength = MIN(bLength, req.wLength);

				usbtty_fifo[0] = bLength;        
				usbtty_fifo[1] = USB_DT_STRING;  

				for (s = 0; s < sl; s++)
				{
					usbtty_fifo[2+ 2*s + 0] = device_strings[string_index][s];
					usbtty_fifo[2+ 2*s + 1] = 0;
				}

				for (byteLoop = 0; byteLoop < bLength; byteLoop++)
					write_fifo_8 (usbtty_fifo[byteLoop]);

				TX_LAST();
			}
		} else if (USB_DT_DEVICE_QUALIFIER == (req.wValue >> 8)) {

#ifdef CONFIG_USB_1_1_DEVICE
			
			NAK_REQ();
#else
			struct usb_qualifier_descriptor d;
			d.bLength = MIN(req.wLength, sizeof(d));
			d.bDescriptorType    = USB_DT_DEVICE_QUALIFIER;
			d.bcdUSB             = 0x200;
			d.bDeviceClass       = 0xff;
			d.bDeviceSubClass    = 0xff;
			d.bDeviceProtocol    = 0xff;
			d.bMaxPacketSize0    = 0x40;
			d.bNumConfigurations = 1;
			d.bRESERVED          = 0;

			memcpy(&usbtty_fifo, &d, d.bLength);
			for (byteLoop = 0; byteLoop < d.bLength; byteLoop++)
				write_fifo_8(usbtty_fifo[byteLoop]);

			TX_LAST();
#endif
		}
		else
		{
			NAK_REQ();
		}
	}

	return ret;
}

static int do_usb_req_get_status(void)
{
	int ret = TTYUSB_OK;

	if (0 == req.wLength)
	{
		ACK_REQ();
	}
	else
	{
		
		unsigned int byteLoop;
		unsigned char bLength;

		bLength = MIN (req.wValue, 2);

		usbtty_fifo[0] = USB_STATUS_SELFPOWERED;
		usbtty_fifo[1] = 0;

		for (byteLoop = 0; byteLoop < bLength; byteLoop++)
			write_fifo_8 (usbtty_fifo[byteLoop]);

		TX_LAST();
	}

	return ret;
}

__inline static int usbtty_poll_h (void)
{
	int ret = TTYUSB_INACTIVE;
	u16 count0;

	if (*csr0 & MUSB_CSR0_RXPKTRDY)
	{
		count0 = inw (OMAP34XX_USB_COUNT0);
		ret = TTYUSB_OK;
		
		if (count0 != 8)
		{
			CONFUSED();
			ret = TTYUSB_ERROR;
		}
		else
		{
			read_request();

			if (USB_REQ_TYPE_STANDARD == (req.bmRequestType & USB_REQ_TYPE_MASK))
			{
				
				if (0 == (req.bmRequestType & USB_REQ_DIRECTION_MASK))
				{
					
					if (USB_RECIP_DEVICE == (req.bmRequestType & USB_REQ_RECIPIENT_MASK))
					{
						
						switch (req.bRequest)
						{
						case USB_REQ_SET_ADDRESS:
							ret = do_usb_req_set_address();
							break;

						case USB_REQ_SET_FEATURE:
							ret = do_usb_req_set_feature();
							break;

						case USB_REQ_SET_CONFIGURATION:
							ret = do_usb_req_set_configuration();
							break;

						default:
							NAK_REQ();
							ret = TTYUSB_ERROR;
							break;
						}
					}
					else if (USB_RECIP_INTERFACE == (req.bmRequestType & USB_REQ_RECIPIENT_MASK))
					{
						switch (req.bRequest)
						{
						case USB_REQ_SET_INTERFACE:
							ret = do_usb_req_set_interface();
							break;

						default:
							NAK_REQ();
							ret = TTYUSB_ERROR;
							break;
						}
					}
					else if (USB_RECIP_ENDPOINT == (req.bmRequestType & USB_REQ_RECIPIENT_MASK))
					{    
					 

						switch (req.bRequest)
                                                {
							case USB_REQ_GET_STATUS:      
							case USB_REQ_CLEAR_FEATURE:   
							case USB_REQ_SET_FEATURE:       
							case USB_REQ_SET_ADDRESS:       
							case USB_REQ_GET_DESCRIPTOR:   
							case USB_REQ_SET_DESCRIPTOR:   
							case USB_REQ_GET_CONFIGURATION:
							case USB_REQ_SET_CONFIGURATION:
							case USB_REQ_GET_INTERFACE:    
							case USB_REQ_SET_INTERFACE:    
							case USB_REQ_SYNCH_FRAME:
							printf(" NOT DOING ANYTHING  ACKING it \n");
							PRINT_REQ (req);
							ACK_REQ();
                	                                break;
                                                default:
                                                        NAK_REQ();
                                                        ret = TTYUSB_ERROR;
                                                        break;
                                                }

					}
					else
					{
						NAK_REQ();
                                                ret = TTYUSB_ERROR;
					}
				}
				else
				{
					
					if (USB_RECIP_DEVICE == (req.bmRequestType & USB_REQ_RECIPIENT_MASK))
					{
						switch (req.bRequest)
						{
						case USB_REQ_GET_DESCRIPTOR:
							ret = do_usb_req_get_descriptor();
							break;

						case USB_REQ_GET_STATUS:
							ret = do_usb_req_get_status();
							break;

						default:
							NAK_REQ();
							ret = TTYUSB_ERROR;
							break;
						}
					}
					else
					{
						NAK_REQ();
						ret = TTYUSB_ERROR;
					}
				}
			}
			else
			{
				
				NAK_REQ();
				printf("NAK ing the request (D -> H) but not exiting..\n");
				PRINT_REQ (req);
				ret = TTYUSB_OK; 
			
			}
		}
		if (TTYUSB_OK > ret)
		{
			printf ("Unhandled req\n");
			PRINT_REQ (req);
		}
	}

	return ret;
}

static int usbtty_resume (void)
{
	
	if (*csr0 & MUSB_CSR0_P_SENTSTALL)
	{
		*csr0 &= ~MUSB_CSR0_P_SENTSTALL;
		return TTYUSB_OK;
	}

	if (*csr0 & MUSB_CSR0_P_SETUPEND)
	{
		
		*csr0 |= MUSB_CSR0_P_SVDSETUPEND;
	}

	if (set_address)
	{
		outb (faddr, OMAP34XX_USB_FADDR);
		set_address = 0;

	}

	return usbtty_poll_h();
}

static void usbtty_rx_error(void)
{
	
	*peri_rxcsr &= ~MUSB_RXCSR_RXPKTRDY;

	*peri_rxcsr |= MUSB_RXCSR_P_SENDSTALL;

	while (!(*peri_rxcsr & MUSB_RXCSR_P_SENTSTALL))
		udelay(1);

	*peri_rxcsr &= ~MUSB_RXCSR_P_SENTSTALL;

}

static int usbtty_rx (void)
{
	int ret = TTYUSB_INACTIVE;

	if (*peri_rxcsr & MUSB_RXCSR_RXPKTRDY)
	{
		u16 count = *rxcount;
		ret = TTYUSB_OK;
		u16 limit = (high_speed) ? 512 : 64;

		if (0 == *rxcount) {
			
			*peri_rxcsr &= ~MUSB_RXCSR_RXPKTRDY;
		} else {
			int i = 0;
			int err = 0;

			if (count >= limit) {
				if (read_bulk_fifo_dma((usbtty_bulk_fifo + usbtty_bulk_fifo_index), count)){
					
					usbtty_rx_error();
					err = 1;
				}
			}
			else if (count < limit) {
				for (i = 0; i < count; i++)
					usbtty_bulk_fifo[usbtty_bulk_fifo_index + i] = read_bulk_fifo_8();
				*peri_rxcsr &= ~MUSB_RXCSR_RXPKTRDY;
			}
			usbtty_bulk_fifo_index += count;

			if (err) {
				CONFUSED();
				ret = TTYUSB_ERROR;
			}
		}
	}
	return ret;
}

int poll_usbtty(void)
{
	
	int ret = TTYUSB_INACTIVE;

	u8 intrusb;
	u16 intrrx;

	u16 frame_num;
	static u16 pre_frame_num = 0xffff;
	static int poll_cnt = 0;

  if (++poll_cnt % 25000 == 0) 
  {
	  frame_num = inw(OMAP34XX_USB_FRAME);
		if (usb_connected == 1 && pre_frame_num == frame_num) {
			usb_connected = 0;
			pre_frame_num = 0xffff;
			return TTYUSB_DISCONNECT;
		}
		pre_frame_num = frame_num;
  }

	intrusb = inb (OMAP34XX_USB_INTRUSB);

	if (intrusb & OMAP34XX_USB_INTRUSB_DISCON)
		return TTYUSB_DISCONNECT;

	if (intrusb & OMAP34XX_USB_INTRUSB_RESUME)
	{
		ret = usbtty_resume ();
		if (TTYUSB_OK > ret)
			return ret;
	}
	else
	{
		if (deferred_rx) ret = TTYUSB_OK;

		if (intrusb & OMAP34XX_USB_INTRUSB_SOF)
		{
			ret = usbtty_resume ();
			if (TTYUSB_OK > ret)
				return ret;

			if (deferred_rx)
				ret = usbtty_rx ();
			deferred_rx = 0;
			if (TTYUSB_OK > ret)
				return ret;
		}
		if (intrusb & OMAP34XX_USB_INTRUSB_SUSPEND)
		{
			return TTYUSB_OK;
		}
		intrrx = inw (OMAP34XX_USB_INTRRX);
		if (intrrx)
		{
			
			deferred_rx = 1;
		}
	}

	return ret;
}

void usbtty_shutdown(void)
{
	
	*pwr &= ~MUSB_POWER_SOFTCONN;

	faddr = 0xff;
	high_speed = 0;
	deferred_rx = 0;
}
int usbtty_tx(unsigned char *buffer, int buf_size)
{
	unsigned int i, fifo_size = usbtty_fifo_size(); 
	volatile int dummy;
	unsigned int transfer_size;

	while(*peri_txcsr & MUSB_TXCSR_TXPKTRDY)
		dummy = 1; 

	while(buf_size > 0){
		transfer_size = MIN(fifo_size, buf_size);
	
		for (i = 0; i < transfer_size; i++)
			write_bulk_fifo_8(buffer[i]);
		*peri_txcsr |= MUSB_TXCSR_TXPKTRDY;

		while(*peri_txcsr & MUSB_TXCSR_TXPKTRDY)
			dummy = 1; 
		
		buffer += transfer_size;
		buf_size -= transfer_size;

	}
	
	TX_LAST();
	return 0;
}

int usbtty_init()
{
	u8 devctl;

	device_strings[DEVICE_STRING_MANUFACTURER_INDEX]  = "LG Electronics";
	device_strings[DEVICE_STRING_PRODUCT_INDEX]        = "LG Mobile USB Modem";
	device_strings[DEVICE_STRING_SERIAL_NUMBER_INDEX]  = "80A000000000000000";

	device_strings[DEVICE_STRING_CONFIG_INDEX]        = "DUMMY LGE Config";
	device_strings[DEVICE_STRING_INTERFACE_INDEX]     = "Virtual COMM interface";

  
  
  *otg_sysconfig = (0x1008);
  if (*otghs_control != 0x15) {
		*otg_interfsel &= 0;

		twl6030_usb_device_settings();

		*otghs_control = 0x15;
	}

	usbtty_reset();

	devctl = inb (OMAP34XX_USB_DEVCTL);
	if (!(devctl & MUSB_DEVCTL_BDEVICE) ||
	    (devctl & MUSB_DEVCTL_HM))
	{
		printf ("ERROR : Unsupport USB mode\n");
		printf ("Check that mini-B USB cable is attached to the device\n");
	}
	return 0;
}

#endif
