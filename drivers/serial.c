#include <common.h>

#if 1	
#include <asm/arch/bits.h>
#include <asm/arch/clocks.h>
#endif	

#ifdef CFG_NS16550_SERIAL

#include <ns16550.h>
#ifdef CFG_NS87308
#include <ns87308.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#if !defined(CONFIG_CONS_INDEX)
#error	"No console index specified."
#elif (CONFIG_CONS_INDEX < 1) || (CONFIG_CONS_INDEX > 4)
#error	"Invalid console index value."
#endif

#if CONFIG_CONS_INDEX == 1 && !defined(CFG_NS16550_COM1)
#error	"Console port 1 defined but not configured."
#elif CONFIG_CONS_INDEX == 2 && !defined(CFG_NS16550_COM2)
#error	"Console port 2 defined but not configured."
#elif CONFIG_CONS_INDEX == 3 && !defined(CFG_NS16550_COM3)
#error	"Console port 3 defined but not configured."
#elif CONFIG_CONS_INDEX == 4 && !defined(CFG_NS16550_COM4)
#error	"Console port 4 defined but not configured."
#endif

static NS16550_t serial_ports[4] = {
#ifdef CFG_NS16550_COM1
	(NS16550_t)CFG_NS16550_COM1,
#else
	NULL,
#endif
#ifdef CFG_NS16550_COM2
	(NS16550_t)CFG_NS16550_COM2,
#else
	NULL,
#endif
#ifdef CFG_NS16550_COM3
	(NS16550_t)CFG_NS16550_COM3,
#else
	NULL,
#endif
#ifdef CFG_NS16550_COM4
	(NS16550_t)CFG_NS16550_COM4
#else
	NULL
#endif
};

#define PORT	serial_ports[port-1]
#define CONSOLE	(serial_ports[CONFIG_CONS_INDEX-1])

static int calc_divisor (NS16550_t port)
{
#ifdef CONFIG_OMAP1510
	
	if ((CFG_NS16550_CLK == 12000000) && (gd->baudrate == 115200)) {
		port->osc_12m_sel = OSC_12M_SEL;	
		return (1);				
	}
	port->osc_12m_sel = 0;			
#endif
#ifdef CONFIG_OMAP1610
	
	if ((CFG_NS16550_CLK == 48000000) && (gd->baudrate == 115200)) {
		return (26);		
	}
#endif

#ifdef CONFIG_APTIX
#define MODE_X_DIV 13
#else
#define MODE_X_DIV 16
#endif

#define MODE_X_DIV_DOWNLOAD 13
#define DOWNLOAD_BAUDRATE 460800

#if defined(CONFIG_COSMO_REV_B) || defined(CONFIG_COSMO_REV_C)
	if(port == CFG_NS16550_COM1)
		return (CFG_NS16550_CLK / MODE_X_DIV_DOWNLOAD / DOWNLOAD_BAUDRATE);
#endif

	return (CFG_NS16550_CLK / MODE_X_DIV / gd->baudrate);

}

int serial_init (void)
{
	int clock_divisor;

#ifdef CFG_NS87308
	initialise_ns87308();
#endif

#ifdef CFG_NS16550_COM1
	clock_divisor = calc_divisor(serial_ports[0]);
	NS16550_init(serial_ports[0], clock_divisor);
#endif
#ifdef CFG_NS16550_COM2
	clock_divisor = calc_divisor(serial_ports[1]);
	NS16550_init(serial_ports[1], clock_divisor);
#endif
#ifdef CFG_NS16550_COM3
	clock_divisor = calc_divisor(serial_ports[2]);
	NS16550_init(serial_ports[2], clock_divisor);
#endif
#ifdef CFG_NS16550_COM4
	clock_divisor = calc_divisor(serial_ports[3]);
	NS16550_init(serial_ports[3], clock_divisor);
#endif

	return (0);
}

void dload_serial_init(void)
{
	int clock_divisor;

	clock_divisor = calc_divisor(serial_ports[0]);
	NS16550_init(serial_ports[0], clock_divisor);
}

void
_serial_putc(const char c,const int port)
{
	if (c == '\n')
		NS16550_putc(PORT, '\r');

	NS16550_putc(PORT, c);
}

void
_serial_putc_raw(const char c,const int port)
{
	NS16550_putc(PORT, c);
}

void
_serial_puts (const char *s,const int port)
{
	while (*s) {
		_serial_putc (*s++,port);
	}
}

int
_serial_getc(const int port)
{
	return NS16550_getc(PORT);
}

int
_serial_putcB(const char c,const int port, unsigned int escape_cnt)
{
	return NS16650_putcB(PORT, c, escape_cnt);
}

int
_serial_putsB (const char *s,const int port, unsigned int len, unsigned int escape_cnt)
{
	while (len > 0) {
		if (NS16650_putcB(PORT, *s++, escape_cnt) == -1)
			return -1;
		len--;
	}

	return 0;
}

int
_serial_getcB(const int port, unsigned int escape_cnt, char* pCh)
{
	return NS16550_getcB(PORT, escape_cnt, pCh);
}

void _serial_reinit(const int port, int baud_divisor)
{
#if 1	
extern void bc_NS16550_init (NS16550_t com_port, int baud_divisor);
bc_NS16550_init(PORT, baud_divisor);
#else
	NS16550_init(PORT, baud_divisor);
#endif	
}

int
_serial_tstc(const int port)
{
	return NS16550_tstc(PORT);
}

void
_serial_setbrg (const int port)
{
	int clock_divisor;

	clock_divisor = calc_divisor(PORT);
	NS16550_reinit(PORT, clock_divisor);
}

void
serial_putc(const char c)
{
	_serial_putc(c,CONFIG_CONS_INDEX);
}

void
serial_putc_raw(const char c)
{
	_serial_putc_raw(c,CONFIG_CONS_INDEX);
}

void
serial_puts(const char *s)
{
	_serial_puts(s,CONFIG_CONS_INDEX);
}

int
serial_getc(void)
{
	return _serial_getc(CONFIG_CONS_INDEX);
}

int
serial_tstc(void)
{
	return _serial_tstc(CONFIG_CONS_INDEX);
}

void
serial_setbrg(void)
{
	_serial_setbrg(CONFIG_CONS_INDEX);
}

#endif
