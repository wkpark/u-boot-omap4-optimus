#include <config.h>
#include <asm/arch/cpu.h>  

#ifdef CFG_NS16550

#include <ns16550.h>

#define LCRVAL LCR_8N1					
#define MCRVAL (MCR_DTR | MCR_RTS)			
#define FCRVAL (FCR_FIFO_EN | FCR_RXSR | FCR_TXSR)	

void NS16550_init (NS16550_t com_port, int baud_divisor)
{
	com_port->ier = 0x00;
#if defined(CONFIG_OMAP) && !defined(CONFIG_3430ZOOM2)
	com_port->mdr1 = 0x7;	
#endif

#if defined(CONFIG_3430ZOOM2)
#error1      

	com_port->lcr = 0xBF;
	com_port->fcr |= 0x10;
	com_port->mcr &= 0x7F;
#endif
	com_port->lcr = LCR_BKSE | LCRVAL;
	com_port->dll = baud_divisor & 0xff;
	com_port->dlm = (baud_divisor >> 8) & 0xff;
	com_port->lcr = LCRVAL;
	com_port->mcr = MCRVAL;
	com_port->fcr = FCRVAL;

#if 0
#if defined(CONFIG_OMAP) && !defined(CONFIG_3430ZOOM2)

 if(com_port == CFG_NS16550_COM1)
	com_port->mdr1 = 3;
else
	com_port->mdr1 = 0;	

#endif
#endif

#if defined(CONFIG_COSMO_REV_B) || defined(CONFIG_COSMO_REV_C)
 	if(com_port == CFG_NS16550_COM1)
		com_port->mdr1 = 3;
	else
		com_port->mdr1 = 0;
#else
		com_port->mdr1 = 0;
#endif

}

#if 1	
void bc_NS16550_init (NS16550_t com_port, int baud_divisor)
{
	com_port->ier = 0x00;
#if defined(CONFIG_OMAP) && !defined(CONFIG_3430ZOOM2)
	com_port->mdr1 = 0x7;	
#endif

#if defined(CONFIG_3430ZOOM2)
#error1

	com_port->lcr = 0xBF;
	com_port->fcr |= 0x10;
	com_port->mcr &= 0x7F;
#endif
	com_port->lcr = LCR_BKSE | LCRVAL;
	com_port->dll = baud_divisor & 0xff;
	com_port->dlm = (baud_divisor >> 8) & 0xff;
	com_port->lcr = LCRVAL;
	com_port->mcr = MCRVAL;
	com_port->fcr = FCRVAL;

#if defined(CONFIG_OMAP) && !defined(CONFIG_3430ZOOM2)
#if defined(CONFIG_APTIX)
	com_port->mdr1 = 3;	
#else

	com_port->mdr1 = 0;	
#endif
#endif
}
#endif	

void NS16550_reinit (NS16550_t com_port, int baud_divisor)
{
	com_port->ier = 0x00;
	com_port->lcr = LCR_BKSE;
	com_port->dll = baud_divisor & 0xff;
	com_port->dlm = (baud_divisor >> 8) & 0xff;
	com_port->lcr = LCRVAL;
	com_port->mcr = MCRVAL;
	com_port->fcr = FCRVAL;
}

void NS16550_putc (NS16550_t com_port, char c)
{
	while ((com_port->lsr & LSR_THRE) == 0);
	com_port->thr = c;
}

int NS16650_putcB(NS16550_t com_port, char c, unsigned int escape_cnt)
{
	while ((com_port->lsr & LSR_THRE) == 0)
	{
		if (escape_cnt-- == 0)
			return -1;
	}
	
	com_port->thr = c;
	return 0;
}

int NS16550_getcB (NS16550_t com_port, unsigned int escape_cnt, char* pCh)
{
	while ((com_port->lsr & LSR_DR) == 0)
	{
		if (escape_cnt-- == 0)
		{
			*pCh = 0;
			return -1;
		}
	}

	*pCh = (com_port->rbr);
	return 0;
}

char NS16550_getc (NS16550_t com_port)
{
	while ((com_port->lsr & LSR_DR) == 0) {
#ifdef CONFIG_USB_TTY
		extern void usbtty_poll(void);
		usbtty_poll();
#endif
	}
	return (com_port->rbr);
}

int NS16550_tstc (NS16550_t com_port)
{
	return ((com_port->lsr & LSR_DR) != 0);
}

#endif
