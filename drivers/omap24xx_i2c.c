#include <common.h>

#if defined(CONFIG_DRIVER_OMAP24XX_I2C) || defined(CONFIG_DRIVER_OMAP34XX_I2C) || defined(CONFIG_DRIVER_OMAP44XX_I2C)

#include <asm/arch/i2c.h>
#include <asm/io.h>
#include <i2c.h>

static u32 i2c_base = I2C_DEFAULT_BASE;
static u32 i2c_speed = CFG_I2C_SPEED;

#ifdef DEBUG

#define DBG(ARGS...) {printf ("[%d]",__LINE__);printf(ARGS);}
#define inb(a) ({u8 v=__raw_readb(i2c_base + (a));printf("%d:Rb[%x<=%x]\n",__LINE__,a,v);v;})
#define outb(v,a) {printf("%d:Wb[%x<=%x]\n",__LINE__,a,v);__raw_writeb((v), (i2c_base + (a)));}
#define inw(a) ({u16 v=__raw_readb(i2c_base + (a));printf("%d:Rw[%x<=%x]\n",__LINE__,a,v);v;})
#define outw(v,a) {printf("%d:Ww[%x<=%x]\n",__LINE__,a,v);__raw_writew((v), (i2c_base + (a)));}

#else
#define DBG(ARGS...)
#define inb(a) __raw_readb(i2c_base + (a))
#define outb(v,a) __raw_writeb((v), (i2c_base + (a)))
#define inw(a) __raw_readw(i2c_base +(a))
#define outw(v,a) __raw_writew((v), (i2c_base + (a)))
#endif

static void wait_for_bb(void);
static u16 wait_for_pin(void);
static void flush_fifo(void);

#if defined(CONFIG_OMAP34XX) || defined(CONFIG_OMAP44XX)
#define I2C_NUM_IF 4
#else
#define I2C_NUM_IF 2
#endif

int select_bus(int bus, int speed)
{
	if ((bus < 0) || (bus >= I2C_NUM_IF)) {
		printf("Bad bus ID-%d\n", bus);
		return -1;
	}

#if defined(CONFIG_OMAP243X) || defined(CONFIG_OMAP34XX) || defined(CONFIG_OMAP44XX)
	
	if ((speed != OMAP_I2C_STANDARD) && (speed != OMAP_I2C_FAST_MODE)
	    && (speed != OMAP_I2C_HIGH_SPEED)) {
		printf("Invalid Speed for i2c init-%d\n", speed);
		return -1;
	}
#else
	if ((speed != OMAP_I2C_STANDARD) && (speed != OMAP_I2C_FAST_MODE)) {
		printf("Invalid Speed for i2c init-%d\n", speed);
		return -1;
	}
#endif

#if defined(CONFIG_OMAP34XX) || defined(CONFIG_OMAP44XX)
	if (bus == 3)
		i2c_base = I2C_BASE4;
	else if (bus == 2)
		i2c_base = I2C_BASE3;
	else 
#endif
	if (bus == 1)
		i2c_base = I2C_BASE2;
	else
		i2c_base = I2C_BASE1;

	i2c_init(speed, CFG_I2C_SLAVE);
	return 0;
}

void i2c_init(int speed, int slaveadd)
{
	int scl_lh = 0;
	int psc = 0;
	int iclk = 0;
	int reset_timeout = 10;

#if defined(CONFIG_OMAP243X)
	{
		u32 v = 0;

		v = __raw_readl(CM_ICLKEN1_CORE) | (0x3 << 19);	
		__raw_writel(v, CM_ICLKEN1_CORE);
		v = __raw_readl(CM_FCLKEN1_CORE) & ~(0x3 << 19);
		__raw_writel(v, CM_FCLKEN1_CORE);
		v = __raw_readl(CM_FCLKEN2_CORE) | (0x3 << 19);	
		__raw_writel(v, CM_FCLKEN2_CORE);
	}
#endif				

	if (inw(I2C_CON) & I2C_CON_EN) {
		outw(0, I2C_CON);
		udelay(50000);
	}
	outw(I2C_SYSC_SRST, I2C_SYSC);	
	udelay(1000);
	
	psc = I2C_PSC_MAX;
	while (psc >= I2C_PSC_MIN) {
		iclk = I2C_IP_CLK / (psc + 1);
		switch (speed) {
		case OMAP_I2C_STANDARD:
			scl_lh = (iclk * 10 / (OMAP_I2C_STANDARD * 2));
			break;
		case OMAP_I2C_HIGH_SPEED:
			
		case OMAP_I2C_FAST_MODE:
			scl_lh = (iclk * 10 / (OMAP_I2C_FAST_MODE * 2));
			break;
			
		}
		DBG("Search- speed= %d SysClk=%d, iclk=%d,psc=0x%x[%d],scl_lh=0x%x[%d]\n",
	       speed, I2C_IP_CLK, iclk, psc, psc, scl_lh, scl_lh);
		
		if (scl_lh % 10) {
			scl_lh = -1;
		} else {
			scl_lh /= 10;
			scl_lh -= 7;
		}
		if (scl_lh >= 0) {
			break;
		}
		psc--;
	}
	
	if (psc < I2C_PSC_MIN) {
		printf
		    ("Unable to set Prescalar for i2c_clock=%d syI2C_IP_CLK=%d\n",
		     speed, I2C_IP_CLK);
		psc = 0;
		return;

	}
	iclk = I2C_IP_CLK / (psc + 1);

	switch (speed) {
	case OMAP_I2C_STANDARD:
		scl_lh =
		    (((iclk / (OMAP_I2C_STANDARD * 2)) - 7) &
		     I2C_SCLL_SCLL_M) << I2C_SCLL_SCLL;
		break;
	case OMAP_I2C_HIGH_SPEED:
		scl_lh =
		    (((I2C_IP_CLK / (OMAP_I2C_HIGH_SPEED * 2)) - 7) &
		     I2C_SCLH_HSSCLL_M) << I2C_SCLL_HSSCLL;
		
	case OMAP_I2C_FAST_MODE:
		scl_lh |=
		    (((iclk / (OMAP_I2C_FAST_MODE * 2)) - 7) &
		     I2C_SCLL_SCLL_M) << I2C_SCLL_SCLL;
		break;
		
	}

	DBG(" speed= %d SysClk=%d, iclk=%d,psc=0x%x[%d],scl_lh=0x%x[%d]\n",
	       speed, I2C_IP_CLK, iclk, psc, psc, scl_lh, scl_lh);
	outw(I2C_CON_EN, I2C_CON);
	while (!(inw(I2C_SYSS) & I2C_SYSS_RDONE) && reset_timeout--) {
		if (reset_timeout <= 0)
			printf("ERROR: Timeout while waiting for soft-reset to complete\n");
		udelay(1000);
	}

	outw(0, I2C_CON);  

	outw(psc, I2C_PSC);
	outw(scl_lh, I2C_SCLL);
	outw(scl_lh, I2C_SCLH);
	
	outw(slaveadd, I2C_OA);
	outw(I2C_CON_EN, I2C_CON);

	outw(I2C_IE_XRDY_IE | I2C_IE_RRDY_IE | I2C_IE_ARDY_IE |
	     I2C_IE_NACK_IE | I2C_IE_AL_IE, I2C_IE);
	udelay(1000);
	flush_fifo();
	outw(0xFFFF, I2C_STAT);
	outw(0, I2C_CNT);
	i2c_speed = speed;
}

static int i2c_read_byte(u8 devaddr, u8 regoffset, u8 * value)
{
	int err;
	int i2c_error = 0;
	u16 status;

	wait_for_bb();

	outw(1, I2C_CNT);
	
	outw(devaddr, I2C_SA);
	
	outw(I2C_CON_EN | ((i2c_speed == OMAP_I2C_HIGH_SPEED) ? 0x1 << 12 : 0) |
	     I2C_CON_MST | I2C_CON_STT | I2C_CON_TRX, I2C_CON);

	status = wait_for_pin();

	if (status & I2C_STAT_XRDY) {
		
		outb(regoffset, I2C_DATA);

		err = 2000;
		while (!(inw(I2C_STAT) & I2C_STAT_ARDY) && err--)
			;
		if (err <= 0)
			i2c_error = 1;

		if (inw(I2C_STAT) & I2C_STAT_NACK) {
			i2c_error = 1;
		}
	} else {
		i2c_error = 1;
	}

	if (!i2c_error) {
		err = 2000;
		outw(I2C_CON_EN, I2C_CON);
		while (inw(I2C_STAT) || (inw(I2C_CON) & I2C_CON_MST)) {
			
			outw(0xFFFF, I2C_STAT);
			if (!err--) {
				break;
			}
		}

		outw(devaddr, I2C_SA);
		
		outw(1, I2C_CNT);
		
		outw(I2C_CON_EN |
		     ((i2c_speed ==
		       OMAP_I2C_HIGH_SPEED) ? 0x1 << 12 : 0) | I2C_CON_MST |
		     I2C_CON_STT | I2C_CON_STP, I2C_CON);

		status = wait_for_pin();
		if (status & I2C_STAT_RRDY) {
#if defined(CONFIG_OMAP243X) || defined(CONFIG_OMAP34XX) || defined(CONFIG_OMAP44XX)
			*value = inb(I2C_DATA);
#else
			*value = inw(I2C_DATA);
#endif
			
			err = 20000;
			while (!(inw(I2C_STAT) & I2C_STAT_ARDY) && err--)
				;
			if (err <= 0){
				printf("i2c_read_byte -- I2C_STAT_ARDY error\n");
				i2c_error = 1;
			}
		} else {
			i2c_error = 1;
		}

		if (!i2c_error) {
			int err = 1000;
			outw(I2C_CON_EN, I2C_CON);
			while (inw(I2C_STAT)
			       || (inw(I2C_CON) & I2C_CON_MST)) {
				outw(0xFFFF, I2C_STAT);
				if (!err--) {
					break;
				}
			}
		}
	}
	flush_fifo();
	outw(0xFFFF, I2C_STAT);
	outw(0, I2C_CNT);
	return i2c_error;
}

static int i2c_write_byte(u8 devaddr, u8 regoffset, u8 value)
{
	int eout;
	int i2c_error = 0;
	u16 status, stat;

	wait_for_bb();

	outw(2, I2C_CNT);
	
	outw(devaddr, I2C_SA);
	
	outw(I2C_CON_EN | ((i2c_speed == OMAP_I2C_HIGH_SPEED) ? 0x1 << 12 : 0) |
	     I2C_CON_MST | I2C_CON_STT | I2C_CON_TRX | I2C_CON_STP, I2C_CON);

	status = wait_for_pin();

	if (status & I2C_STAT_XRDY) {
#if defined(CONFIG_OMAP243X) || defined(CONFIG_OMAP34XX) || defined(CONFIG_OMAP44XX)
		
		outb(regoffset, I2C_DATA);
		outw(I2C_STAT_XRDY, I2C_STAT);
		status = wait_for_pin();
		if ((status & I2C_STAT_XRDY)) {
			
			outb(value, I2C_DATA);
			outw(I2C_STAT_XRDY, I2C_STAT);
		} else {
			i2c_error = 1;
		}
#else
		
		outw((value << 8) | regoffset, I2C_DATA);
#endif
		
		eout= 20000;
		while (!(inw(I2C_STAT) & I2C_STAT_ARDY) && eout--)
			;
		if (eout <= 0)
			printf("timed out in i2c_write_byte: I2C_STAT=%x\n", inw(I2C_STAT));

		if (inw(I2C_STAT) & I2C_STAT_NACK) {
			i2c_error = 1;
		}
	} else {
		i2c_error = 1;
	}
	if (!i2c_error) {
		eout = 2000;

		outw(I2C_CON_EN, I2C_CON);
		while ((stat = inw(I2C_STAT)) || (inw(I2C_CON) & I2C_CON_MST)) {
			
			outw(0xFFFF, I2C_STAT);
			if (--eout == 0)	
				break;
		}
	}
	flush_fifo();
	outw(0xFFFF, I2C_STAT);
	outw(0, I2C_CNT);
	return i2c_error;
}

int i2c_write_bytes(u8 devaddr, u8 regoffset, u8* data, u8 len)
{
	int eout;
	int i2c_error = 0;
	u16 status, stat;

	wait_for_bb();

	outw(3, I2C_CNT);
	
	outw(devaddr, I2C_SA);
	
	outw(I2C_CON_EN | ((i2c_speed == OMAP_I2C_HIGH_SPEED) ? 0x1 << 12 : 0) |
			I2C_CON_MST | I2C_CON_STT | I2C_CON_TRX | I2C_CON_STP, I2C_CON);

	status = wait_for_pin();

	if (status & I2C_STAT_XRDY) {
#if defined(CONFIG_OMAP243X) || defined(CONFIG_OMAP34XX) || defined(CONFIG_OMAP44XX)
		if (status & I2C_STAT_XRDY) {
			
			outb(regoffset, I2C_DATA);
			outw(I2C_STAT_XRDY, I2C_STAT);
			while(len--)
			{
				status = wait_for_pin();
				if ((status & I2C_STAT_XRDY)) {
					
					outb(*data, I2C_DATA);
					outw(I2C_STAT_XRDY, I2C_STAT);
				} else {
					i2c_error = 1;
				}
				data++;
			}
		}
#else
		
		outw((value << 8) | regoffset, I2C_DATA);
#endif
		
		eout= 20000;
		while (!(inw(I2C_STAT) & I2C_STAT_ARDY) && eout--)
			;
		if (eout <= 0)
			printf("timed out in i2c_write_byte: I2C_STAT=%x\n", inw(I2C_STAT));

		if (inw(I2C_STAT) & I2C_STAT_NACK) {
			i2c_error = 1;
		}
	} else {
		i2c_error = 1;
	}
	if (!i2c_error) {
		eout = 2000;

		outw(I2C_CON_EN, I2C_CON);
		while ((stat = inw(I2C_STAT)) || (inw(I2C_CON) & I2C_CON_MST)) {
			
			outw(0xFFFF, I2C_STAT);
			if (--eout == 0)	
				break;
		}
	}
	flush_fifo();
	outw(0xFFFF, I2C_STAT);
	outw(0, I2C_CNT);
	return i2c_error;
}

static void flush_fifo(void)
{
	u16 stat;

	while (1) {
		stat = inw(I2C_STAT);
		if (stat == I2C_STAT_RRDY) {
#if defined(CONFIG_OMAP243X) || defined(CONFIG_OMAP34XX) || defined(CONFIG_OMAP44XX)
			inb(I2C_DATA);
#else
			inw(I2C_DATA);
#endif
			outw(I2C_STAT_RRDY, I2C_STAT);
		} else
			break;
	}
}

int i2c_probe(uchar chip)
{
	int res = 1;		

	if (chip == inw(I2C_OA)) {
		return res;
	}

	wait_for_bb();

	outw(1, I2C_CNT);
	
	outw(chip, I2C_SA);
	
	outw(I2C_CON_EN | ((i2c_speed == OMAP_I2C_HIGH_SPEED) ? 0x1 << 12 : 0) |
	     I2C_CON_MST | I2C_CON_STT | I2C_CON_STP, I2C_CON);
	
	udelay(50000);

	if (!(inw(I2C_STAT) & I2C_STAT_NACK)) {
		res = 0;	
		flush_fifo();
		outw(0xFFFF, I2C_STAT);
	} else {
		outw(0xFFFF, I2C_STAT);	
		outw(inw(I2C_CON) | I2C_CON_STP, I2C_CON);	
		udelay(20000);
		wait_for_bb();
	}
	flush_fifo();
	outw(0, I2C_CNT);	
	outw(0xFFFF, I2C_STAT);
	return res;
}

int i2c_read(uchar chip, uint addr, int alen, uchar * buffer, int len)
{
	int i;

	if (alen > 1) {
		printf("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) {
		printf("I2C read: address out of range\n");
		return 1;
	}

	for (i = 0; i < len; i++) {
		if (i2c_read_byte(chip, addr + i, &buffer[i])) {
			printf("I2C read: I/O error %x\n", chip);
			i2c_init(i2c_speed, CFG_I2C_SLAVE);
			return 1;
		}
	}

	return 0;
}

int i2c_write(uchar chip, uint addr, int alen, uchar * buffer, int len)
{
	int i;

	if (alen > 1) {
		printf("I2C write: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) {
		printf("I2C write: address out of range\n");
		return 1;
	}

	for (i = 0; i < len; i++) {
		if (i2c_write_byte(chip, addr + i, buffer[i])) {
			printf("I2C write: I/O error %x\n", chip);
			i2c_init(i2c_speed, CFG_I2C_SLAVE);
			return 1;
		}
	}

	return 0;
}

static void wait_for_bb(void)
{
	int timeout = 5000;
	u16 stat;

	outw(0xFFFF, I2C_STAT);	
	while ((stat = inw(I2C_STAT) & I2C_STAT_BB) && timeout--) {
		outw(stat, I2C_STAT);
	}

	if (timeout <= 0) {
		printf("timed out in wait_for_bb: I2C_STAT=%x\n",
		       inw(I2C_STAT));
	}
	outw(0xFFFF, I2C_STAT);	
}

static u16 wait_for_pin(void)
{
	u16 status;
	int timeout = 9000;

	do {
		status = inw(I2C_STAT);
	} while (!(status &
		   (I2C_STAT_ROVR | I2C_STAT_XUDF | I2C_STAT_XRDY |
		    I2C_STAT_RRDY | I2C_STAT_ARDY | I2C_STAT_NACK |
		    I2C_STAT_AL)) && timeout--);

	if (timeout <= 0) {
		printf("timed out in wait_for_pin: I2C_STAT=%x\n",
		       inw(I2C_STAT));
		outw(0xFFFF, I2C_STAT);
	}
	return status;
}

#endif
