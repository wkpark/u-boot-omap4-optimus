#if (CFG_NS16550_REG_SIZE == 1)
struct NS16550 {
	unsigned char rbr;		
	unsigned char ier;		
	unsigned char fcr;		
	unsigned char lcr;		
	unsigned char mcr;		
	unsigned char lsr;		
	unsigned char msr;		
	unsigned char scr;		
#if defined(CONFIG_OMAP730)
	unsigned char mdr1;		
	unsigned char reg9;		
	unsigned char regA;		
	unsigned char regB;		
	unsigned char regC;		
	unsigned char regD;		
	unsigned char regE;		
	unsigned char regF;		
	unsigned char reg10;		
	unsigned char ssr;		
#endif
} __attribute__ ((packed));
#elif (CFG_NS16550_REG_SIZE == 2)
struct NS16550 {
	unsigned short rbr;		
	unsigned short ier;		
	unsigned short fcr;		
	unsigned short lcr;		
	unsigned short mcr;		
	unsigned short lsr;		
	unsigned short msr;		
	unsigned short scr;		
} __attribute__ ((packed));
#elif (CFG_NS16550_REG_SIZE == 4)
struct NS16550 {
	unsigned long rbr;		
	unsigned long ier;		
	unsigned long fcr;		
	unsigned long lcr;		
	unsigned long mcr;		
	unsigned long lsr;		
	unsigned long msr;		
	unsigned long scr;		
}; 
#elif (CFG_NS16550_REG_SIZE == -2)
struct NS16550 {
	unsigned char rbr;		
	int pad1:8;
	unsigned char ier;		
	int pad2:8;
	unsigned char fcr;		
	int pad3:8;
	unsigned char lcr;		
	int pad4:8;
	unsigned char mcr;		
	int pad5:8;
	unsigned char lsr;		
	int pad6:8;
	unsigned char msr;		
	int pad7:8;
	unsigned char scr;		
	int pad8:8;
}; 
#elif (CFG_NS16550_REG_SIZE == -4)
struct NS16550 {
	unsigned char rbr;		
	int pad1:24;
	unsigned char ier;		
	int pad2:24;
	unsigned char fcr;		
	int pad3:24;
	unsigned char lcr;		
	int pad4:24;
	unsigned char mcr;		
	int pad5:24;
	unsigned char lsr;		
	int pad6:24;
	unsigned char msr;		
	int pad7:24;
	unsigned char scr;		
	int pad8:24;
#if defined(CONFIG_OMAP)
	unsigned char mdr1;		
#endif
#ifdef CONFIG_OMAP1510
	int pad9:24;
	unsigned long pad[10];
	unsigned char osc_12m_sel;
	int pad10:24;
#endif
} __attribute__ ((packed));
#elif (CFG_NS16550_REG_SIZE == -8)
struct NS16550 {
	unsigned char rbr;		
	unsigned char pad0[7];
	unsigned char ier;		
	unsigned char pad1[7];
	unsigned char fcr;		
	unsigned char pad2[7];
	unsigned char lcr;		
	unsigned char pad3[7];
	unsigned char mcr;		
	unsigned char pad4[7];
	unsigned char lsr;		
	unsigned char pad5[7];
	unsigned char msr;		
	unsigned char pad6[7];
	unsigned char scr;		
	unsigned char pad7[7];
} __attribute__ ((packed));
#else
#error "Please define NS16550 registers size."
#endif

#define thr rbr
#define iir fcr
#define dll rbr
#define dlm ier

typedef volatile struct NS16550 *NS16550_t;

#define FCR_FIFO_EN     0x01		
#define FCR_RXSR        0x02		
#define FCR_TXSR        0x04		

#define MCR_DTR         0x01
#define MCR_RTS         0x02
#define MCR_DMA_EN      0x04
#define MCR_TX_DFR      0x08

#define LCR_WLS_MSK	0x03		
#define LCR_WLS_5	0x00		
#define LCR_WLS_6	0x01		
#define LCR_WLS_7	0x02		
#define LCR_WLS_8	0x03		
#define LCR_STB		0x04		
#define LCR_PEN		0x08		
#define LCR_EPS		0x10		
#define LCR_STKP	0x20		
#define LCR_SBRK	0x40		
#define LCR_BKSE	0x80		

#define LSR_DR		0x01		
#define LSR_OE		0x02		
#define LSR_PE		0x04		
#define LSR_FE		0x08		
#define LSR_BI		0x10		
#define LSR_THRE	0x20		
#define LSR_TEMT	0x40		
#define LSR_ERR		0x80		

#ifdef CONFIG_OMAP1510
#define OSC_12M_SEL	0x01		
#endif

#define LCR_8N1		0x03

void	NS16550_init   (NS16550_t com_port, int baud_divisor);
void	NS16550_putc   (NS16550_t com_port, char c);
char	NS16550_getc   (NS16550_t com_port);
int	NS16550_tstc   (NS16550_t com_port);
void	NS16550_reinit (NS16550_t com_port, int baud_divisor);

int NS16650_putcB(NS16550_t com_port, char c, unsigned int escape_cnt);
int NS16550_getcB (NS16550_t com_port, unsigned int escape_cnt, char* pCh);
