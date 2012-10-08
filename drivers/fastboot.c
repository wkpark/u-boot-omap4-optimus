#include <common.h>
#include <asm/arch/cpu.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/arch/bits.h>
#include <asm/arch/mem.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/sys_info.h>
#include <asm/arch/mux.h>
#ifdef CONFIG_OMAP3430
#include <asm/arch/led.h>
#endif
#include <environment.h>
#include <command.h>
#include <usbdcore.h>
#include "usbomap.h"
#include <fastboot.h>
#include <twl4030.h>
#include <twl6030.h>

#if defined(CONFIG_FASTBOOT)

#define OTG_SYSCONFIG 0x4A0AB404
#define OTG_INTERFSEL 0x4A0AB40C
#define USBOTGHS_CONTROL 0x4A00233C
#define CONTROL_DEV_CONF 0x4A002300

#define USBPHY_PD 0x1

#include "usb_debug_macros.h"

#define CONFUSED() printf ("How did we get here %s %d ? \n", __FILE__, __LINE__)

static volatile	u8  *pwr        = (volatile u8  *) OMAP34XX_USB_POWER;
static volatile u16 *csr0       = (volatile u16 *) OMAP34XX_USB_CSR0;
static volatile u8  *index      = (volatile u8  *) OMAP34XX_USB_INDEX;
static volatile u8  *txfifosz   = (volatile u8  *) OMAP34XX_USB_TXFIFOSZ;
static volatile u8  *rxfifosz   = (volatile u8  *) OMAP34XX_USB_RXFIFOSZ;
static volatile u16 *txfifoadd  = (volatile u16 *) OMAP34XX_USB_TXFIFOADD;
static volatile u16 *rxfifoadd  = (volatile u16 *) OMAP34XX_USB_RXFIFOADD;

#define BULK_ENDPOINT 1
static volatile u16 *peri_rxcsr = (volatile u16 *) OMAP34XX_USB_RXCSR(BULK_ENDPOINT);
static volatile u16 *rxmaxp     = (volatile u16 *) OMAP34XX_USB_RXMAXP(BULK_ENDPOINT);
static volatile u16 *rxcount    = (volatile u16 *) OMAP34XX_USB_RXCOUNT(BULK_ENDPOINT);
static volatile u16 *peri_txcsr = (volatile u16 *) OMAP34XX_USB_TXCSR(BULK_ENDPOINT);
static volatile u16 *txmaxp     = (volatile u16 *) OMAP34XX_USB_TXMAXP(BULK_ENDPOINT);
static volatile u8  *bulk_fifo  = (volatile u8  *) OMAP34XX_USB_FIFO(BULK_ENDPOINT);

#define DMA_CHANNEL 1
static volatile u8  *peri_dma_intr	= (volatile u8  *) OMAP34XX_USB_DMA_INTR;
static volatile u16 *peri_dma_cntl	= (volatile u16 *) OMAP34XX_USB_DMA_CNTL_CH(DMA_CHANNEL);
static volatile u32 *peri_dma_addr	= (volatile u32 *) OMAP34XX_USB_DMA_ADDR_CH(DMA_CHANNEL);
static volatile u32 *peri_dma_count	= (volatile u32 *) OMAP34XX_USB_DMA_COUNT_CH(DMA_CHANNEL);

#if 1 
static volatile u32 *otg_sysconfig	= (volatile u32 *) OMAP34XX_OTG_SYSCONFIG;
static volatile u32 *otg_interfsel	= (volatile u32 *) OMAP34XX_OTG_INTERFSEL;
static volatile u32 *otg_forcestdby	= (volatile u32 *) OMAP34XX_OTG_FORCESTDBY;
static volatile u32 *otghs_control = (volatile u32  *)USBOTGHS_CONTROL;
#endif
static volatile u32 *control_dev_conf = (volatile u32  *)CONTROL_DEV_CONF;

#define DEVICE_VENDOR_ID  0x0451

#define DEVICE_PRODUCT_ID 0xD022

#define DEVICE_BCD        0x0100;
/* This is used to get the serial number */
#if defined(CONFIG_4430SDP) || defined(CONFIG_4430PANDA)
#define DIE_ID_REG_BASE		(OMAP44XX_L4_IO_BASE + 0x2000)
#define DIE_ID_REG_OFFSET		0x200
#define MAX_USB_SERIAL_NUM              17
#endif

/* String 0 is the language id */
#define DEVICE_STRING_PRODUCT_INDEX       1
#define DEVICE_STRING_SERIAL_NUMBER_INDEX 2
#define DEVICE_STRING_CONFIG_INDEX        3
#define DEVICE_STRING_INTERFACE_INDEX     4
#define DEVICE_STRING_MANUFACTURER_INDEX  5
#define DEVICE_STRING_PROC_REVISION       6
#define DEVICE_STRING_PROC_TYPE           7
#define DEVICE_STRING_MAX_INDEX           DEVICE_STRING_PROC_TYPE
#define DEVICE_STRING_LANGUAGE_ID         0x0409 /* English (United States) */

#define RX_ENDPOINT_MAXIMUM_PACKET_SIZE_2_0  (0x0200)
#define RX_ENDPOINT_MAXIMUM_PACKET_SIZE_1_1  (0x0040)
#define TX_ENDPOINT_MAXIMUM_PACKET_SIZE      (0x0200)

#define RX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS_2_0 (6)
#define RX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS_1_1 (3)
#define TX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS (6)

#define CONFIGURATION_NORMAL      1

#define TX_LAST()						\
	*csr0 |= (MUSB_CSR0_TXPKTRDY | MUSB_CSR0_P_DATAEND);	\
	while  (*csr0 & MUSB_CSR0_RXPKTRDY)			\
		udelay(1);			      

#define NAK_REQ() *csr0 |= MUSB_CSR0_P_SENDSTALL
#define ACK_REQ() *csr0 |= MUSB_CSR0_P_DATAEND

#define ACK_RX()  *peri_rxcsr |= (MUSB_CSR0_P_SVDRXPKTRDY | MUSB_CSR0_P_DATAEND)

static u8 fastboot_fifo[MUSB_EP0_FIFOSIZE];
static u16 fastboot_fifo_used = 0;

static unsigned int set_address = 0;
static u8 faddr = 0xff;
static unsigned int current_config = 0;

static unsigned int high_speed = 1;

static unsigned int deferred_rx = 0;

static struct usb_device_request req;

static u8 fastboot_bulk_fifo[0x0200] __attribute__ ((aligned(0x4)));

static char *device_strings[DEVICE_STRING_MAX_INDEX+1];

static struct cmd_fastboot_interface *fastboot_interface = NULL;

#ifdef DEBUG_FASTBOOT
static void fastboot_db_regs(void) 
{
	printf("fastboot_db_regs\n");
	u8 b;
	u16 s;

	b = inb (OMAP34XX_USB_FADDR);
	printf ("\tfaddr   0x%2.2x\n", b);

	b = inb (OMAP34XX_USB_POWER);
	PRINT_PWR(b);

	s = inw (OMAP34XX_USB_CSR0);
	PRINT_CSR0(s);

	b = inb (OMAP34XX_USB_DEVCTL);
	PRINT_DEVCTL(b);

	b = inb (OMAP34XX_USB_CONFIGDATA);
	PRINT_CONFIG(b);

	s = inw (OMAP34XX_USB_FRAME);
	printf ("\tframe   0x%4.4x\n", s);
	b = inb (OMAP34XX_USB_INDEX);
	printf ("\tindex   0x%2.2x\n", b);

	s = *rxmaxp;
	PRINT_RXMAXP(s);

	s = *peri_rxcsr;
	PRINT_RXCSR(s);

	s = *txmaxp;
	PRINT_TXMAXP(s);

	s = *peri_txcsr;
	PRINT_TXCSR(s);
}
#endif

static void fastboot_bulk_endpoint_reset (void)
{
	u8 old_index;
	
	old_index = *index;

	*index = 0;
	*rxfifoadd = 0;
	*txfifoadd = 0;
	*rxfifosz = 3; 
	*txfifosz = 3;

	*index = BULK_ENDPOINT;

	*txfifoadd = MUSB_EP0_FIFOSIZE >> 3;
	*rxfifoadd = (MUSB_EP0_FIFOSIZE + 512) >> 3;

	*txfifosz = TX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS;

#if defined(CONFIG_4430PANDA)
	if (high_speed)
		*rxfifosz =
			RX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS_2_0;
	else
		*rxfifosz =
			RX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS_1_1;
#else
	if (high_speed)
		*rxfifosz =
			RX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS_2_0 |
			MUSB_RXFIFOSZ_DPB;
	else
		*rxfifosz =
			RX_ENDPOINT_MAXIMUM_PACKET_SIZE_BITS_1_1 |
			MUSB_RXFIFOSZ_DPB;
#endif

	*index = old_index;

	*rxmaxp = fastboot_fifo_size();

	while (*peri_rxcsr & MUSB_RXCSR_RXPKTRDY)
	{
		*peri_rxcsr |= MUSB_RXCSR_FLUSHFIFO;
		udelay(1);
	}
	
	*peri_rxcsr &= ~(MUSB_RXCSR_DMAENAB | MUSB_RXCSR_P_ISO);
	
	*peri_rxcsr |= MUSB_RXCSR_CLRDATATOG;

	*txmaxp = TX_ENDPOINT_MAXIMUM_PACKET_SIZE;

	while (*peri_txcsr & MUSB_TXCSR_FIFONOTEMPTY)
	{
		*peri_txcsr |= MUSB_TXCSR_FLUSHFIFO;
		udelay(1);
	}

	*peri_txcsr &= ~(MUSB_TXCSR_DMAENAB | MUSB_TXCSR_P_ISO | MUSB_TXCSR_P_UNDERRUN);
	
	*peri_txcsr |= (MUSB_TXCSR_CLRDATATOG | MUSB_TXCSR_MODE);
}

static void fastboot_reset (void)
{
#ifdef CONFIG_OMAP3430
	OMAP3_LED_ERROR_ON ();
#endif
	
	*pwr &= ~MUSB_POWER_SOFTCONN;
	udelay(2 * 500000); 

#ifdef CONFIG_OMAP3430
	OMAP3_LED_ERROR_OFF ();
#endif

	faddr = 0xff;

#ifdef CONFIG_USB_1_1_DEVICE
	*pwr &= ~MUSB_POWER_HSENAB;
	*pwr |= MUSB_POWER_SOFTCONN;
#else
	*pwr |= (MUSB_POWER_SOFTCONN | MUSB_POWER_HSENAB);
#endif
	
	fastboot_bulk_endpoint_reset ();
	
#ifdef CONFIG_OMAP3430
	OMAP3_LED_ERROR_ON ();
#endif
	
}

static u8 read_fifo_8(void)
{
	u8 val;
  
	val = inb (OMAP34XX_USB_FIFO_0);
	return val;
}

static u8 read_bulk_fifo_8(void)
{
	u8 val;
  
	val = *bulk_fifo;
	return val;
}

static int read_bulk_fifo_dma(u8 *buf, u32 size)
{
	int ret = 0;

	*peri_dma_cntl = 0;

	*peri_dma_addr = (u32) buf;
	
	*peri_dma_count = size;

	*peri_dma_cntl =
		MUSB_DMA_CNTL_BUSRT_MODE_3 |
		MUSB_DMA_CNTL_END_POINT(BULK_ENDPOINT) |
		MUSB_DMA_CNTL_INTERRUPT_ENABLE |
		MUSB_DMA_CNTL_WRITE;

	*peri_dma_cntl |= MUSB_DMA_CNTL_ENABLE;

	while (1) {

		if (MUSB_DMA_CNTL_ERR & *peri_dma_cntl) {
			ret = 1;
			break;
		}

		if (0 == *peri_dma_count)
			break;
	}

	return ret;
}

static void write_fifo_8(u8 val)
{
	outb (val, OMAP34XX_USB_FIFO_0);
}

static void write_bulk_fifo_8(u8 val)
{
	*bulk_fifo = val;
}

static void read_request(void)
{
	int i;
  
	for (i = 0; i < 8; i++)
		fastboot_fifo[i] = read_fifo_8 ();
	memcpy (&req, &fastboot_fifo[0], 8);
	fastboot_fifo_used = 0;

	*csr0 |= MUSB_CSR0_P_SVDRXPKTRDY;

	while  (*csr0 & MUSB_CSR0_RXPKTRDY)
		udelay(1);

}

static int do_usb_req_set_interface(void)
{
	int ret = 0;

	if ((0 == req.wIndex) &&
	    (0 == req.wValue))
	{
		fastboot_bulk_endpoint_reset ();
		ACK_REQ();
	}
	else
	{
		NAK_REQ();
	}

	return ret;
}

static int do_usb_req_set_address(void)
{
	int ret = 0;
  
	if (0xff == faddr) 
	{
		faddr = (u8) (req.wValue & 0x7f);
		set_address = 1;

		if (*pwr & MUSB_POWER_HSMODE)
		  high_speed = 1;
		else
		  high_speed = 0;

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
	int ret = 0;

	if (0xff == faddr) {
		NAK_REQ(); 
	} else {
		if (0 == req.wValue) {
			
			faddr = 0xff;
			current_config = req.wValue;
			ACK_REQ();
		} else if (CONFIGURATION_NORMAL == req.wValue) {

			fastboot_bulk_endpoint_reset();
			current_config = req.wValue;

			ACK_REQ();
		} else {
			
			NAK_REQ();
		}
	}

	return ret;
}

static int do_usb_req_set_feature(void)
{
	int ret = 0;
  
	NAK_REQ();

	return ret;
}

static int do_usb_req_get_descriptor(void)
{
	int ret = 0;
  
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
			d.bDeviceClass       = 0x00;
			d.bDeviceSubClass    = 0x00;
			d.bDeviceProtocol    = 0x00;
			d.bMaxPacketSize0    = 0x40;
			d.idVendor           = DEVICE_VENDOR_ID;
			d.idProduct          = DEVICE_PRODUCT_ID;
			d.bcdDevice          = DEVICE_BCD;
			d.iManufacturer      = DEVICE_STRING_MANUFACTURER_INDEX;
			d.iProduct           = DEVICE_STRING_PRODUCT_INDEX;
			d.iSerialNumber      = DEVICE_STRING_SERIAL_NUMBER_INDEX;
			d.bNumConfigurations = 1;
	  
			memcpy (&fastboot_fifo, &d, d.bLength);
			for (byteLoop = 0; byteLoop < d.bLength; byteLoop++) 
				write_fifo_8 (fastboot_fifo[byteLoop]);

			TX_LAST();
		}
		else if (USB_DT_CONFIG == (req.wValue >> 8))
		{
			struct usb_configuration_descriptor c;
			struct usb_interface_descriptor i;
			struct usb_endpoint_descriptor e1, e2;
			unsigned char bytes_remaining = req.wLength;
			unsigned char bytes_total = 0;

			c.bLength             = MIN(bytes_remaining, sizeof (c));
			c.bDescriptorType     = USB_DT_CONFIG;
			
			c.wTotalLength = sizeof (c) + sizeof (i) + sizeof (e1) + sizeof (e2); 
			c.bNumInterfaces      = 1;
			c.bConfigurationValue = CONFIGURATION_NORMAL;
			c.iConfiguration      = DEVICE_STRING_CONFIG_INDEX;
			c.bmAttributes        = 0xc0;
			c.bMaxPower           = 0x32;

			bytes_remaining -= c.bLength;
			memcpy (&fastboot_fifo[0], &c, c.bLength);
			bytes_total += c.bLength;
	  
			i.bLength             = MIN (bytes_remaining, sizeof(i));
			i.bDescriptorType     = USB_DT_INTERFACE;	
			i.bInterfaceNumber    = 0x00;
			i.bAlternateSetting   = 0x00;
			i.bNumEndpoints       = 0x02;
			i.bInterfaceClass     = FASTBOOT_INTERFACE_CLASS;
			i.bInterfaceSubClass  = FASTBOOT_INTERFACE_SUB_CLASS;
			i.bInterfaceProtocol  = FASTBOOT_INTERFACE_PROTOCOL;
			i.iInterface          = DEVICE_STRING_INTERFACE_INDEX;

			bytes_remaining -= i.bLength;
			memcpy (&fastboot_fifo[bytes_total], &i, i.bLength);
			bytes_total += i.bLength;

			e1.bLength            = MIN (bytes_remaining, sizeof (e1));
			e1.bDescriptorType    = USB_DT_ENDPOINT;
			e1.bEndpointAddress   = 0x80 | BULK_ENDPOINT; 
			e1.bmAttributes       = USB_ENDPOINT_XFER_BULK;
			e1.wMaxPacketSize     = TX_ENDPOINT_MAXIMUM_PACKET_SIZE;
			e1.bInterval          = 0x00;

			bytes_remaining -= e1.bLength;
			memcpy (&fastboot_fifo[bytes_total], &e1, e1.bLength);
			bytes_total += e1.bLength;

			e2.bLength            = MIN (bytes_remaining, sizeof (e2));
			e2.bDescriptorType    = USB_DT_ENDPOINT;
			e2.bEndpointAddress   = BULK_ENDPOINT; 
			e2.bmAttributes       = USB_ENDPOINT_XFER_BULK;
			if (high_speed)
				e2.wMaxPacketSize = RX_ENDPOINT_MAXIMUM_PACKET_SIZE_2_0;
			else
				e2.wMaxPacketSize = RX_ENDPOINT_MAXIMUM_PACKET_SIZE_1_1;
			e2.bInterval          = 0x00;

			bytes_remaining -= e2.bLength;
			memcpy (&fastboot_fifo[bytes_total], &e2, e2.bLength);
			bytes_total += e2.bLength;

			for (byteLoop = 0; byteLoop < bytes_total; byteLoop++) 
				write_fifo_8 (fastboot_fifo[byteLoop]);

			TX_LAST();
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
		  
				fastboot_fifo[0] = bLength;        
				fastboot_fifo[1] = USB_DT_STRING;  
				fastboot_fifo[2] = DEVICE_STRING_LANGUAGE_ID & 0xff;
				fastboot_fifo[3] = DEVICE_STRING_LANGUAGE_ID >> 8;

				for (byteLoop = 0; byteLoop < bLength; byteLoop++) 
					write_fifo_8 (fastboot_fifo[byteLoop]);

				TX_LAST();
			}
			else
			{
				
				unsigned char s;
				unsigned char sl = strlen (&device_strings[string_index][0]);

				unsigned char bLength = 2 + (2 * sl);
				unsigned char numbytes_to_send;

				numbytes_to_send = MIN(bLength, req.wLength);

				fastboot_fifo[0] = bLength;        
				fastboot_fifo[1] = USB_DT_STRING;  
	      
				
				for (s = 0; s < sl; s++)
				{
					fastboot_fifo[2+ 2*s + 0] = device_strings[string_index][s];
					fastboot_fifo[2+ 2*s + 1] = 0;
				}

				for (byteLoop = 0; byteLoop < numbytes_to_send; byteLoop++)
					write_fifo_8 (fastboot_fifo[byteLoop]);

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

			memcpy(&fastboot_fifo, &d, d.bLength);
			for (byteLoop = 0; byteLoop < d.bLength; byteLoop++)
				write_fifo_8(fastboot_fifo[byteLoop]);

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

static int do_usb_req_get_configuration(void)
{
	int ret = 0;

	if (0 == req.wLength) {
		printf ("Get config with length 0 is unexpected\n");
		NAK_REQ();
	} else {
		write_fifo_8 (current_config);
		TX_LAST();
	}

	return ret;
}

static int do_usb_req_get_status(void)
{
	int ret = 0;

	if (0 == req.wLength)
	{
		ACK_REQ();
	}
	else
	{
		
		unsigned int byteLoop;
		unsigned char bLength;

		bLength = MIN (req.wValue, 2);
      
		fastboot_fifo[0] = USB_STATUS_SELFPOWERED;
		fastboot_fifo[1] = 0;

		for (byteLoop = 0; byteLoop < bLength; byteLoop++) 
			write_fifo_8 (fastboot_fifo[byteLoop]);

		TX_LAST();
	}
  
	return ret;
}

static int fastboot_poll_h (void)
{
	int ret = 0;
	u16 count0;

	if (*csr0 & MUSB_CSR0_RXPKTRDY) 
	{
		count0 = inw (OMAP34XX_USB_COUNT0);

		if (count0 != 8) 
		{
#ifdef CONFIG_OMAP3430
			OMAP3_LED_ERROR_ON ();
#endif
			CONFUSED();
			ret = 1;
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
							ret = -1;
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
							ret = -1;
							break;
						}
					}
					else
					{
						NAK_REQ();
						ret = -1;
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

						case USB_REQ_GET_CONFIGURATION:
							ret = do_usb_req_get_configuration();
							break;

						default:
							NAK_REQ();
							ret = -1;
							break;
						}
					}
					else
					{
						NAK_REQ();
						ret = -1;
					}
				}
			}
			else
			{
				
				NAK_REQ();
				ret = -1;
			}
		}
		if (0 > ret)
		{
			printf ("Unhandled req\n");
			PRINT_REQ (req);
		}
	}
  
	return ret;
}

static int fastboot_resume (void)
{
	
	if (*csr0 & MUSB_CSR0_P_SENTSTALL)
	{
		*csr0 &= ~MUSB_CSR0_P_SENTSTALL;
		return 0;
	}

	if (*csr0 & MUSB_CSR0_P_SETUPEND)
	{
		
		*csr0 |= MUSB_CSR0_P_SVDSETUPEND;

#if 0
		if (0xff != faddr)
			fastboot_reset ();

		if (fastboot_interface &&
		    fastboot_interface->reset_handler) 
		  {
			  fastboot_interface->reset_handler();
		  }

		return 0;
#endif

	}

	if (set_address) 
	{
		outb (faddr, OMAP34XX_USB_FADDR);
		set_address = 0;
#ifdef CONFIG_OMAP3430
		
		OMAP3_LED_OK_ON ();
#endif
	}
  
	return fastboot_poll_h();
}

static void fastboot_rx_error (void)
{
	
	*peri_rxcsr &= ~MUSB_RXCSR_RXPKTRDY;

	*peri_rxcsr |= MUSB_RXCSR_P_SENDSTALL;

	while (!(*peri_rxcsr & MUSB_RXCSR_P_SENTSTALL))
		udelay(1);

	*peri_rxcsr &= ~MUSB_RXCSR_P_SENTSTALL;

}

static int fastboot_rx (void)
{
	if (*peri_rxcsr & MUSB_RXCSR_RXPKTRDY) {
		u16 count = *rxcount;
		int fifo_size = fastboot_fifo_size();

		if (0 == *rxcount) {
			
			*peri_rxcsr &= ~MUSB_RXCSR_RXPKTRDY;
		} else if (fifo_size < count) {
			fastboot_rx_error();
		} else {
			int i = 0;
			int err = 1;

			if (fifo_size == count) {

				*peri_rxcsr &= ~MUSB_RXCSR_AUTOCLEAR;
				*peri_rxcsr |= MUSB_RXCSR_DMAENAB;
				*peri_rxcsr |= MUSB_RXCSR_DMAMODE;
				*peri_rxcsr |= MUSB_RXCSR_DMAENAB;

				if (read_bulk_fifo_dma
				    (fastboot_bulk_fifo, fifo_size)) {
					
					fastboot_rx_error();
				}

				*peri_rxcsr &= ~(MUSB_RXCSR_DMAENAB |
						 MUSB_RXCSR_DMAMODE);

			} else {
				for (i = 0; i < count; i++)
					fastboot_bulk_fifo[i] =
						read_bulk_fifo_8();
			}
			
			*peri_rxcsr &= ~MUSB_RXCSR_RXPKTRDY;

			if (fastboot_interface &&
			    fastboot_interface->rx_handler) {
				if (!fastboot_interface->rx_handler
				    (&fastboot_bulk_fifo[0], count))
					err = 0;
			}

			memset(&fastboot_bulk_fifo[0], 0, fifo_size);

			if (err) {
#ifdef CONFIG_OMAP3430
				OMAP3_LED_ERROR_ON ();
#endif
				CONFUSED();
			}
		}
	}
	return 0;
}

static int fastboot_suspend (void)
{

	return 0;
}

int fastboot_poll(void) 
{
	int ret = 0;

	u8 intrusb;
	u16 intrtx;
	u16 intrrx;
	static u32 blink = 0;

#if defined(CONFIG_4430PANDA)
	#define OMAP44XX_WKUP_CTRL_BASE 0x4A31E000
	#define PRECEPTION_FACTOR 100000

	if (blink  == 0x7fff + PRECEPTION_FACTOR)
		__raw_writew((PTU | M3),
			OMAP44XX_WKUP_CTRL_BASE + CONTROL_WKUP_PAD1_FREF_CLK4_REQ);

	if (blink  == (0xffff + PRECEPTION_FACTOR)) {
		__raw_writew((M3),
			OMAP44XX_WKUP_CTRL_BASE + CONTROL_WKUP_PAD1_FREF_CLK4_REQ);
		blink = 0;
	}

	blink ++ ;
#endif

	intrusb = inb (OMAP34XX_USB_INTRUSB);

	if (intrusb & OMAP34XX_USB_INTRUSB_DISCON)
	{
		return 1;
	}

	if (intrusb & OMAP34XX_USB_INTRUSB_RESUME)
	{
		ret = fastboot_resume ();
		if (ret)
			return ret;
	}
	else 
	{
		if (intrusb & OMAP34XX_USB_INTRUSB_SOF)
		{
			ret = fastboot_resume ();
			if (ret)
				return ret;

			if (deferred_rx)
				ret = fastboot_rx ();
			deferred_rx = 0;
			if (ret)
				return ret;

		}
		if (intrusb & OMAP34XX_USB_INTRUSB_SUSPEND)
		{
			/* USB cable disconnected, reset faddr */
			faddr = 0xff;

			ret = fastboot_suspend ();
			if (ret)
				return ret;
		}

		intrtx = inw (OMAP34XX_USB_INTRTX);
		if (intrtx)
		{

		}

		intrrx = inw (OMAP34XX_USB_INTRRX);
		if (intrrx)
		{
			
			deferred_rx = 1;
		}
	}

	return ret;
}

void fastboot_shutdown(void)
{
	
	*pwr &= ~MUSB_POWER_SOFTCONN;

	faddr = 0xff;
	fastboot_interface = NULL;
	high_speed = 0;
}

int fastboot_is_highspeed(void)
{
	int ret = 0;
	if (*pwr & MUSB_POWER_HSMODE)
		ret = 1;
	return ret;
}

int fastboot_fifo_size(void)
{
	return high_speed ? RX_ENDPOINT_MAXIMUM_PACKET_SIZE_2_0 : RX_ENDPOINT_MAXIMUM_PACKET_SIZE_1_1;
}

int fastboot_tx_status(const char *buffer, unsigned int buffer_size)
{
	int ret = 1;
	unsigned int i;
	
	unsigned int transfer_size = MIN(64, buffer_size);

	while  (*peri_txcsr & MUSB_TXCSR_TXPKTRDY)
		udelay(1);

	for (i = 0; i < transfer_size; i++)
		write_bulk_fifo_8 (buffer[i]);

	*peri_txcsr |= MUSB_TXCSR_TXPKTRDY;

	while  (*peri_txcsr & MUSB_TXCSR_TXPKTRDY)
		udelay(1);

	TX_LAST();

	ret = 0;

	return ret;
}

int fastboot_getvar(const char *rx_buffer, char *tx_buffer)
{
	
	return 0;
}

int fastboot_preboot(void)
{
	char *cmd[3];
#if (defined(CONFIG_TWL4030_KEYPAD) && (CONFIG_TWL4030_KEYPAD))
	int i;
	unsigned char key1, key2;
	int keys;
	udelay(CFG_FASTBOOT_PREBOOT_INITIAL_WAIT);
	for (i = 0; i < CFG_FASTBOOT_PREBOOT_LOOP_MAXIMUM; i++) {
		key1 = key2 = 0;
		keys = twl4030_keypad_keys_pressed(&key1, &key2);
		if ((1 == CFG_FASTBOOT_PREBOOT_KEYS) &&
		    (1 == keys)) {
			if (CFG_FASTBOOT_PREBOOT_KEY1 == key1)
				return 1;
		} else if ((2 == CFG_FASTBOOT_PREBOOT_KEYS) &&
			   (2 == keys)) {
			if ((CFG_FASTBOOT_PREBOOT_KEY1 == key1) &&
			    (CFG_FASTBOOT_PREBOOT_KEY2 == key2))
				return 1;
		}
		udelay(CFG_FASTBOOT_PREBOOT_LOOP_WAIT);
	}
#endif

#if defined(CONFIG_OMAP44XX)
#define KBD_STATEMACHINE	(0x4a31c038)

#if 0
	
	if (__raw_readl(KBD_STATEMACHINE))
		return 1;

#if defined(CONFIG_4430PANDA)
	if (!(__raw_readl(OMAP44XX_GPIO4_BASE + DATA_IN_OFFSET) & (1<<25))){
		printf("Panda: GPIO_121 pressed: entering fastboot....\n");
		return 1;
	}
#endif

#endif

	if (__raw_readl(PRM_RSTST) & PRM_RSTST_RESET_WARM_BIT) {

		printf("\n reboot command [%s]\n", PUBLIC_SAR_RAM_1_FREE);
		/* Warm reset case:
		* %adb reboot recovery
		*/
		if (!strcmp(PUBLIC_SAR_RAM_1_FREE, "recovery")) {

			printf("\n Case: \%reboot recovery\n");
start_recovery:
			printf("\n Starting recovery img.....\n");
			cmd[0] = malloc(10);
			cmd[1] = malloc(10);
			cmd[2] = malloc(10);

			/* pass: booti mmci<N> recovery */
			strcpy(cmd[0], "booti");
#if defined(CONFIG_4430PANDA)
			strcpy(cmd[1], "mmc0");
#else
			strcpy(cmd[1], "mmc1");
#endif
			strcpy(cmd[2], "recovery");

			do_booti(NULL, 0, 3, cmd);
			/* returns if recovery.img is bad
			 * Default to normal boot
			 */
			free(cmd[0]);
			free(cmd[1]);
			free(cmd[2]);

			printf("\nfastboot: Error: Invalid recovery img\n");
			return 0;
		} else if (!strcmp(PUBLIC_SAR_RAM_1_FREE, "off")) {
			fastboot_shutdown();
		} else if (!strcmp(PUBLIC_SAR_RAM_1_FREE, "bootloader")) {
			/* Warm reset case
			 * Case: %fastboot reboot-bootloader
			 * Case: %adb reboot bootloader
			 * Case: %adb reboot reboot-bootloader
			 */
			return 1;
		}
		return 0;
	}
#endif
	return 1;
}

int fastboot_init(struct cmd_fastboot_interface *interface) 
{
	int ret = 1;
	u8 devctl;
	int cpu_rev = 0;
	int cpu_type = 0;
	u32 usb_phy_power_down;

	device_strings[DEVICE_STRING_MANUFACTURER_INDEX]  = "Texas Instruments";
#if defined (CONFIG_3430ZOOM2)
	device_strings[DEVICE_STRING_PRODUCT_INDEX]       = "Zoom2";
#elif defined (CONFIG_3430LABRADOR)
	device_strings[DEVICE_STRING_PRODUCT_INDEX]       = "Zoom";
#elif defined(CONFIG_4430SDP)
#if defined(CONFIG_LGE_P2)
	device_strings[DEVICE_STRING_PRODUCT_INDEX]       = "LG P2";
#elif defined(CONFIG_LGE_CX2)
	device_strings[DEVICE_STRING_PRODUCT_INDEX]       = "LG CX2";
#else
	device_strings[DEVICE_STRING_PRODUCT_INDEX]       = "SDP4";
#endif
#elif defined(CONFIG_4430PANDA)
	device_strings[DEVICE_STRING_PRODUCT_INDEX]       = "PANDA";
#else
	
#error "Need a product name for fastboot"

#endif
	
#if defined(CONFIG_4430SDP) || defined(CONFIG_4430PANDA)
	unsigned int val[4] = { 0 };
	unsigned int reg;
	static char device_serial[MAX_USB_SERIAL_NUM] = "00123";

	reg = DIE_ID_REG_BASE + DIE_ID_REG_OFFSET;

	val[0] = __raw_readl(reg);
	val[1] = __raw_readl(reg + 0x8);
	val[2] = __raw_readl(reg + 0xC);
	val[3] = __raw_readl(reg + 0x10);
	printf("Device Serial Number: %08X%08X\n", val[1], val[0]);
	sprintf(device_serial, "%08X%08X", val[1], val[0]);

	device_strings[DEVICE_STRING_SERIAL_NUMBER_INDEX] = device_serial;
#else
	/* These are just made up */
	device_strings[DEVICE_STRING_SERIAL_NUMBER_INDEX] = "00123";
#endif
	device_strings[DEVICE_STRING_CONFIG_INDEX]        = "Android Fastboot";
	device_strings[DEVICE_STRING_INTERFACE_INDEX]     = "Android Fastboot";

#if defined(CONFIG_4430SDP) || defined(CONFIG_4430PANDA)
	cpu_rev = get_cpu_rev();
	switch (cpu_rev) {
		case CPU_4430_ES1:
			device_strings[DEVICE_STRING_PROC_REVISION]  = "ES1.0";
			break;
		case CPU_4430_ES20:
			device_strings[DEVICE_STRING_PROC_REVISION]  = "ES2.0";
			break;
		case CPU_4430_ES21:
			device_strings[DEVICE_STRING_PROC_REVISION]  = "ES2.1";
			break;
		case CPU_4430_ES22:
			device_strings[DEVICE_STRING_PROC_REVISION]  = "ES2.2";
			break;
		default:
			device_strings[DEVICE_STRING_PROC_REVISION]  = "Unknown";
			break;
	}
	cpu_type = get_device_type();
	switch (cpu_type) {
		case CPU_4430_GP:
			device_strings[DEVICE_STRING_PROC_TYPE]  = "GP";
			break;
		case  CPU_4430_EMU:
			device_strings[DEVICE_STRING_PROC_TYPE]  = "EMU";
			break;
		case  CPU_4430_HS:
			device_strings[DEVICE_STRING_PROC_TYPE]  = "HS";
			break;
		default:
			device_strings[DEVICE_STRING_PROC_TYPE]  = "Unknown";
			break;
	}

#endif

	fastboot_interface = interface;
	fastboot_interface->product_name                  = device_strings[DEVICE_STRING_PRODUCT_INDEX];
	fastboot_interface->serial_no                     = device_strings[DEVICE_STRING_SERIAL_NUMBER_INDEX];
#if defined(CONFIG_4430SDP) || defined(CONFIG_4430PANDA)
	fastboot_interface->storage_medium                = EMMC;
	fastboot_interface->proc_rev                  = device_strings[DEVICE_STRING_PROC_REVISION];
	fastboot_interface->proc_type                  = device_strings[DEVICE_STRING_PROC_TYPE];
#else
	fastboot_interface->storage_medium                = NAND;
#endif
	fastboot_interface->nand_block_size               = 2048;
	fastboot_interface->transfer_buffer               = (unsigned char *) CFG_FASTBOOT_TRANSFER_BUFFER;
	fastboot_interface->transfer_buffer_size          = CFG_FASTBOOT_TRANSFER_BUFFER_SIZE;

	*otg_sysconfig |= (1<<1);
	
	*otg_sysconfig = (0x1008);

	usb_phy_power_down = *control_dev_conf & USBPHY_PD;

	if(usb_phy_power_down)
	{
		 // poweron usb phy
		 *control_dev_conf &= ~USBPHY_PD;
		 udelay(200000);
	}

	if ((*otghs_control != 0x15) || usb_phy_power_down) {
		fastboot_reset();

#if 1 
	*otg_interfsel &= 0;

	twl6030_usb_device_settings();

	*otghs_control = 0x15;
#endif

	} else {
		
		fastboot_bulk_endpoint_reset();

		*otg_interfsel &= 0;

		outb(0x1,OMAP34XX_USB_DEVCTL);
	}

	devctl = inb (OMAP34XX_USB_DEVCTL);
	if (!(devctl & MUSB_DEVCTL_BDEVICE) ||
	    (devctl & MUSB_DEVCTL_HM)) 
	{
		printf ("ERROR : Unsupport USB mode\n");
		printf ("Check that mini-B USB cable is attached to the device\n");
	}
	else
	{
		ret = 0;
	}
	return ret;
}

#endif
