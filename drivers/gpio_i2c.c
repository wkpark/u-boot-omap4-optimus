/*---------------------------------------------*/
/* Include files.                              */
/*---------------------------------------------*/
#include <twl6030.h>
#include <asm-arm/arch-omap4/gpio.h>
#include "gpio_i2c.h"
#include <charging_ic.h>

/*------------------------------------------------------------------------------------*/
/* Local defines.                                                                     */
/*------------------------------------------------------------------------------------*/
#define I2C1_GPIO_SCLK				157  //*((volatile unsigned int*) 0x4A100130)  //DPM_EMU2 //GPIO 13
#define I2C1_GPIO_SCLK_REG     
#define I2C1_GPIO_SCLK_LOW		fuel_set_gpio_dataout(I2C1_GPIO_SCLK, 0)
#define I2C1_GPIO_SCLK_HIGH		fuel_set_gpio_dataout(I2C1_GPIO_SCLK, 1)
#define I2C1_GPIO_SCLK_INPUT	omap_set_gpio_direction(I2C1_GPIO_SCLK, 1)

#define I2C1_GPIO_SDAT				163 //*((volatile unsigned int*) 0x4A1000E0)  //C2C_DATA11 //GPIO 100
#define I2C1_GPIO_SDAT_REG     
#define I2C1_GPIO_SDAT_LOW		fuel_set_gpio_dataout(I2C1_GPIO_SDAT, 0)
#define I2C1_GPIO_SDAT_HIGH		fuel_set_gpio_dataout(I2C1_GPIO_SDAT, 1)
#define I2C1_GPIO_SDAT_INPUT	omap_set_gpio_direction(I2C1_GPIO_SDAT, 1)

#if 1
#define I2C1_GPIO_SCLK_READ 			omap_get_gpio_datain(I2C1_GPIO_SCLK)
#define I2C1_GPIO_SDAT_READ 			omap_get_gpio_datain(I2C1_GPIO_SDAT)
#else
#define I2C1_GPIO_SCLK_READ 			(__raw_readl(0x4A310138) & (1<<I2C1_GPIO_SCLK)) != 0 
#define I2C1_GPIO_SDAT_READ 			(__raw_readl(0x48059138) & (1<<4)) != 0
#endif

void fuel_set_gpio_dataout(int gpio, int value)
{
	omap_set_gpio_direction(gpio, 0);
	omap_set_gpio_dataout(gpio, value);

	return;
}

static unsigned int busy_delay = 0;

void fuel_busy_delay(unsigned int count)
{
	return;
#if 1
		busy_delay = 0; 
#else
		while(busy_delay < 5 * count)
		{
			busy_delay++;
		}
		//fuel_udelay(count);
#endif
}

/*------------------------------------------------------------------------------------*/
/* Local data definitions.                                                            */
/*------------------------------------------------------------------------------------*/
i2c_gpio_type i2c_io_info =
{
	I2C_UNINIT_S			  /* I2C controller state					  */
};

/*------------------------------------------------------------------------------------*/
/* Local function prototypes.                                                         */
/*------------------------------------------------------------------------------------*/
void SDAT_IN_MODE(void);
void SCLK_IN_MODE(void);
void SDAT_HIGH(void);
void SCLK_HIGH(void);
void SDAT_LOW(void);
void SCLK_LOW(void);
void I2C_START_COND(void);
void I2C_STOP_COND(void);
void I2C_REP_START_COND(void);
void I2C_CLK_DATA_IO(void);
void I2C_ACK_BYTE(void);
void I2C_NACK_BYTE(void);
u8 fuel_gpio_i2c_read_ack(void);
u8 fuel_gpio_i2c_write_byte_io(u8	val/* value to write */);
u8 fuel_gpio_i2c_read_byte_io(void);
void i2c_clk_start_before_rw(void);
void fuel_gpio_i2c_stop(u8 wait_rd_buf_full);
i2c_status_type fuel_gpio_fuel_gpio_i2c_write_addr(u8 val);
i2c_status_type fuel_gpio_fuel_gpio_i2c_write_bytes(u8	*buf, unsigned short len, unsigned short	*r_index);
i2c_status_type fuel_gpio_fuel_gpio_i2c_write_last(u8 val);
i2c_status_type fuel_gpio_fuel_gpio_i2c_read_bytes(u8 *buf, unsigned short len, unsigned short	*r_index);
i2c_status_type fuel_gpio_fuel_gpio_i2c_read_last(u8	*val);
i2c_status_type fuel_gpio_i2c_setup_slave(i2c_rw_cmd_type* cmd_ptr,  /* pointer to user command buffer */
		i2c_op_type	  op_type	/* operation type */);
void fuel_gpio_i2c_init(void);
i2c_status_type fuel_gpio_i2c_read(i2c_rw_cmd_type   *cmd_ptr  /* pointer to user command buffer */);
i2c_status_type fuel_gpio_i2c_write(i2c_rw_cmd_type	*cmd_ptr  /* pointer to user command buffer */);

/*------------------------------------------------------------------------------------*/
/**************************** Internal (static) functions *****************************/
/*------------------------------------------------------------------------------------*/
void  SDAT_IN_MODE(void)
{
	I2C1_GPIO_SDAT_INPUT;
}

void  SCLK_IN_MODE(void)
{
	I2C1_GPIO_SCLK_INPUT;
}

void  SDAT_HIGH(void)
{
	I2C1_GPIO_SDAT_INPUT;
	//I2C1_GPIO_SDAT_HIGH;
}

void  SCLK_HIGH(void)
{
	I2C1_GPIO_SCLK_INPUT;
	//I2C1_GPIO_SCLK_HIGH;
}

void  SDAT_LOW(void)
{
	I2C1_GPIO_SDAT_LOW;
}

void  SCLK_LOW(void)
{
	I2C1_GPIO_SCLK_LOW;
}

void I2C_START_COND(void)
{
	/* The rise and fall times of SCL and SDA are much smaller than 	 */
	/* the SCL and SDA GPIO pin access times. So, they are not considered*/
	/* in defining the macro below. 									 */
	SDAT_HIGH(); /* raise SDA */
	SCLK_HIGH(); /* raise SCL */

	SDAT_LOW();  /* lower SDA */
	I2C_TSU_STA_DELAY;
	I2C_TSU_STA_DELAY;

	SCLK_LOW();  /* lower SCL */
} /* i2c_start_cond */

void I2C_STOP_COND(void)
{
	/* Data line starts in an unknown state */
	SCLK_LOW();
	I2C_TSU_STO_DELAY;

	SCLK_HIGH();
	I2C_TSU_STO_DELAY;

	SDAT_HIGH();
	I2C_TBUF_DELAY;
} /* i2c_set_stop_cond */

void I2C_REP_START_COND(void)
{
	/* always enable SDA for output incase ERR_FATAL interrupting a read */
	SDAT_HIGH() ;
	SCLK_HIGH() ;
	I2C_TSU_STA_DELAY;

	/* SDA transition while SCL is high */
	SDAT_LOW() ;
	I2C_THD_STA_DELAY;

	SCLK_LOW() ;

	/* Default data line to high postion. */
	SDAT_HIGH() ;
} /* I2C_REP_START_COND_INT */

void I2C_CLK_DATA_IO(void)
{
	SCLK_HIGH() ;
	I2C_THIGH_DELAY;
	SCLK_LOW() ;
	I2C_TLOW_DELAY;
}

void I2C_ACK_BYTE(void)
{
	/* Transmit a '0' */
	SDAT_LOW(); /* lower SDA */
	/* Clock the bit out */
	I2C_CLK_DATA_IO(); 
}

void I2C_NACK_BYTE(void)
{
	/* Transmit a '1' */
	SDAT_HIGH(); /* raise SDA */
	/* Clock the bit out */
	I2C_CLK_DATA_IO();
}

u8 fuel_gpio_i2c_read_ack(void)
{
	u8		ack_bit 	= I2C_NACK; 													/* storage of the ack bit */
	unsigned short	time_out	= 0;

	SDAT_IN_MODE();
	SCLK_IN_MODE();

	while(time_out++ < ACK_TIME_OUT )
	{
		I2C_4_7_MICROSEC_WAIT;

		if(I2C1_GPIO_SCLK_READ)
		{
			if (I2C1_GPIO_SDAT_READ) /* read ack bit */
			{
				ack_bit = 1;   
			}
			else
			{
				ack_bit = 0;   /* read ack bit */
			}
			break;
		}
	}

	I2C_THIGH_DELAY;

	SCLK_LOW() ;

	return (ack_bit);  /* Return the ack bit */
} /* fuel_gpio_i2c_read_ack */

u8 fuel_gpio_i2c_write_byte_io(u8	val/* value to write */)
{
	int count;

	u8 return_ACK;

	//ICU_disable_global_irq();

	for (count = 0; count < 8; count++)
	{

		/*
		 ** To transmit a bit to slave, raise the data bit and then run
		 ** a clock cycle with timings as shown below.
		 **   SDA            ---------------
		 **                 |             |
		 **                 |             |
		 **      ------------             -----------
		 **             Tsu,dat(.25us)
		 **               ->| |<-
		 **                   |
		 **      |<---Tlow--->|<--Thigh-->|
		 **      |   4.7us    |   4us     |
		 **  SCL--            -------------
		 **      |            |           |
		 **      |            |           |
		 **      --------------           -----
		 */
		/* equivalent to: I2C_WRITE_SDA (bus_id, bit_value); */
		if( val & 0x80 )
			SDAT_HIGH();
		else
			SDAT_LOW();
		/* clk_nano_wait will wait at least 200ns.	The delay needed is
		   250.  However, as each instruction takes 74, the instructions
		   between will take at least 50. */
		WAIT_FOR_USECS(1);

		SCLK_HIGH() ;
		/* Wait on raising clock. */
		WAIT_FOR_USECS(1);

		SCLK_LOW() ;
		/* Wait on low.  The delay of 4.7us could be reduced here for a
		   performance increase.  The INTFREE and INTLOCK are somewhat
		   expensive.  However, depending on their timing will affect
		   how this code is implemented.  So, for now, guarantee our
		   timing. */
		WAIT_FOR_USECS(1);

		/* Go get the next bit. */
		val <<= 1;
	}

	/* Free interrupts to allow a chance for any to happen. */
	//ICU_enable_global_irq();

	return_ACK = fuel_gpio_i2c_read_ack();

	return (return_ACK);
}


u8 fuel_gpio_i2c_read_byte_io(void)
{
	u8 result = 0;
	u8 val =  0;
	int count;

	//ICU_disable_global_irq();

	SDAT_IN_MODE();

	for (count = 0; count < 8; count++)
	{
		/*
		 ** To read a data bit from the slave, read data bit in the high
		 ** period of the clock. The required timings are shown below.
		 **
		 **	  |<---Tlow--->|<--Thigh-->|
		 **	  |   4.7us    |   4us	   |
		 **  SCL-- 		   -------------
		 **	  | 		   |		   |
		 **	  | 		   |		   |
		 **	  --------------		   -----
		 **
		 */
		/* Wait initial time.  Could be shortened due to time spent in
		   INTLOCK and INTFREE, but then we would depend on their
		   implementation.  Should be visited if this is a performance
		   issue. */
		WAIT_FOR_USECS(1);

		/* Now raise the clock: I2C_WRITE_SCL (bus_id, I2C_BIT_HI); */
		SCLK_HIGH() ;

		/* Wait the high delay time.	The best time to sample the data
		   line is right before we lower the clock again. */
		WAIT_FOR_USECS(2);

		/* Read in the data bit. */
		result <<= 1;

		if( val = I2C1_GPIO_SDAT_READ)
		{
			result |= 1;
		}

		WAIT_FOR_USECS(1);

		/* Lower the clock: I2C_WRITE_SCL (bus_id, I2C_BIT_LO); */	  
		SCLK_LOW();
	}

	//ICU_enable_global_irq();

	return (result);
}

void i2c_clk_start_before_rw(void)
{
	int count=0;

	count = 0;
	//????
}


void fuel_gpio_i2c_stop(u8 wait_rd_buf_full)
{
	i2c_state_type state = I2C_FAIL_S;

	I2C_STOP_COND();				 /* Generate a STOP condition	 */
	state = I2C_IDLE_S;			 /* Reset bus state to idle	   */

	i2c_io_info.state = state;
}

i2c_status_type fuel_gpio_fuel_gpio_i2c_write_addr(u8 val)
{
	i2c_state_type	state	= I2C_FAIL_S;
	i2c_status_type status = I2C_FAILURE;
	i2c_gpio_type	   *bus   = &i2c_io_info;

	if (bus->state == I2C_TX_S) 
	{			/* Bus already STARTed */
		I2C_REP_START_COND();
	} 
	else if (bus->state == I2C_IDLE_S) 
	{  /* bus not STARTed */
		I2C_START_COND();
	}

	if (fuel_gpio_i2c_write_byte_io(val) == I2C_ACK)
	{
		/* Write addr byte */
		/* State change depend on READ/WRITE */
		state = ((val & I2C_IO_MASK) == I2C_IO_MASK_R)? I2C_RX_S: I2C_TX_S;
		status = I2C_SUCCESS;
	}
	else
	{
		status = I2C_SUCCESS;
	}

	bus->state = state;

	return (status);
}

i2c_status_type fuel_gpio_fuel_gpio_i2c_write_bytes(u8	*buf, unsigned short len, unsigned short	*r_index)
{
	i2c_status_type status = I2C_SUCCESS;
	int 			index  = 0;

	for (index = 0; (index < len) && (status == I2C_SUCCESS); index++, (*r_index)++) 
	{
		status = (i2c_status_type) fuel_gpio_i2c_write_byte_io(buf[index]);
	}

	return (status);
}

i2c_status_type fuel_gpio_fuel_gpio_i2c_write_last(u8 val)
{
	i2c_state_type  state   = I2C_FAIL_S;
	i2c_status_type status = I2C_FAILURE;
	i2c_gpio_type 	*bus   = &i2c_io_info;

	if (fuel_gpio_i2c_write_byte_io(val) == I2C_ACK) 
	{  /* Write byte		*/
		I2C_STOP_COND();							/* Stop if ACK	   */
		state  = I2C_IDLE_S;						/* Change state	 */
		status = I2C_SUCCESS;
	}
	else
	{
		status = I2C_SUCCESS;
	}

	bus->state = state;
	return (status);
}

i2c_status_type fuel_gpio_fuel_gpio_i2c_read_bytes(u8 *buf, unsigned short len, unsigned short	*r_index)
{
	i2c_status_type 	status = I2C_SUCCESS;
	unsigned short							index  = 0;

	for (index =0; index < len; index++, (*r_index)++) 
	{
		buf[index] = fuel_gpio_i2c_read_byte_io(); /* Read byte from SDA line  */
		I2C_TLOW_DELAY;
		I2C_ACK_BYTE(); 			/* Ack the read byte */
	}

	return (status);
}

i2c_status_type fuel_gpio_fuel_gpio_i2c_read_last(u8	*val)
{
	i2c_status_type status		= I2C_FAILURE;
	i2c_state_type		state		= I2C_FAIL_S;
	i2c_gpio_type		*bus		= &i2c_io_info;

	*val   = fuel_gpio_i2c_read_byte_io();   /* Read byte from SDA line  */
	/* LGE_L1_BB: ooky Tpad patch */
	I2C_THIGH_DELAY;
	/* LGE_L1_BB: ooky Tpad patch */
	I2C_NACK_BYTE();			   /* Nack the last byte */
	I2C_STOP_COND();			   /* Generate a stop signal */
	state  = I2C_IDLE_S;		   /* State change */
	status = I2C_SUCCESS;

	bus->state = state; 				   /* Updata bus state */
	return (status);
}

i2c_status_type fuel_gpio_i2c_setup_slave(i2c_rw_cmd_type* cmd_ptr,  /* pointer to user command buffer */
		i2c_op_type	  op_type	/* operation type */)
{
	u8								   slave_addr;		  /* temporary to hold slave addr */
	i2c_status_type 	 status = I2C_INVALID_ACCESS;
	unsigned short								   len	  = 0;


	if (cmd_ptr->len >= I2C_MAX_DATA_LEN) 
	{
		return (I2C_INVALID_LEN);
	}

	if (i2c_io_info.state == I2C_IDLE_S) 
	{
		if ((cmd_ptr->options & I2C_DFLT_ADDR_DEV) == I2C_DFLT_ADDR_DEV) 
		{
			if (op_type == I2C_READ) 
			{
				slave_addr = I2C_ADDR_MASK_R(cmd_ptr->slave_addr);
			}
			else 
			{
				slave_addr = I2C_ADDR_MASK_W(cmd_ptr->slave_addr);
			}
		}
		else 
		{
			if((cmd_ptr->options & I2C_STOP_START_BEFORE_READ) &&	(op_type == I2C_READ))
			{
				slave_addr = I2C_ADDR_MASK_W(cmd_ptr->slave_addr);
			}else
			{
				if(op_type == I2C_READ) 
					slave_addr = I2C_ADDR_MASK_R(cmd_ptr->slave_addr);
				else 
					slave_addr = I2C_ADDR_MASK_W(cmd_ptr->slave_addr);
			}  
		}

		status = fuel_gpio_fuel_gpio_i2c_write_addr(slave_addr);

		if (status == I2C_SUCCESS) 
		{ 
			if(/*(i2c_io_info.state == I2C_TX_S)*/
					((cmd_ptr->options & I2C_DFLT_ADDR_DEV) != I2C_DFLT_ADDR_DEV)) 
			{
				if (cmd_ptr->options & I2C_MEM_DEV) 
				{
					/* Transmit high word of offset */
					status = fuel_gpio_fuel_gpio_i2c_write_bytes(((u8 *)&(cmd_ptr->addr.offset))+1, 1, &len);

					/* Validate if slave ACKed. Transmit low word of offset */
					if (status == I2C_SUCCESS) 
					{
						if ((cmd_ptr->options & I2C_STOP_START_BEFORE_READ) &&
								(op_type == I2C_READ)) 
						{
							status = fuel_gpio_fuel_gpio_i2c_write_last((u8)(0x00FF & cmd_ptr->addr.offset));
						} 
						else 
						{
							status = fuel_gpio_fuel_gpio_i2c_write_bytes(((u8 *)&(cmd_ptr->addr.offset)), 1, &len);
						}
					}
				}
				else if (cmd_ptr->options & I2C_REG_DEV) 
				{
					/* Transmit register address of slave device */
					if ((cmd_ptr->options & I2C_STOP_START_BEFORE_READ) &&	(op_type == I2C_READ)) 
					{
						status = fuel_gpio_fuel_gpio_i2c_write_last(cmd_ptr->addr.reg);
					} 
					else 
					{
						status = fuel_gpio_fuel_gpio_i2c_write_bytes(&(cmd_ptr->addr.reg), 1, &len);
					}
				}

				if ((cmd_ptr->options & I2C_SLAVE_INTERLEAVE) 
						&&(status == I2C_SUCCESS)) 
				{
					if ((cmd_ptr->options & I2C_STOP_START_BEFORE_WRITE) ) 
					{
						i2c_clk_start_before_rw();
					}

					status = fuel_gpio_fuel_gpio_i2c_write_addr(slave_addr);
				} 
				else if((op_type == I2C_READ) && (status == I2C_SUCCESS)) /* Write slave control byte with 'r' bit set. */
				{
					status = fuel_gpio_fuel_gpio_i2c_write_addr(I2C_ADDR_MASK_R(cmd_ptr->slave_addr));
				}
			}
		}
	}
	return (status);
} /* fuel_gpio_i2c_setup_slave */

void fuel_gpio_i2c_init(void)
{
	int ret;

	SDAT_IN_MODE();
	SCLK_IN_MODE();

	/* Initialize the bus to IDLE state */
	i2c_io_info.state = I2C_IDLE_S;
} /* fuel_gpio_i2c_init */

i2c_status_type fuel_gpio_i2c_read(i2c_rw_cmd_type   *cmd_ptr  /* pointer to user command buffer */)
{
	unsigned short							index	= 0;					  /* loop variable		  */
	i2c_status_type 	status	= I2C_INVALID_ACCESS;	  /* I2C operation status */
	u8						*buffer 	= NULL; 				  /* Read buffer's pointer*/

	if ((cmd_ptr != NULL) && (cmd_ptr->buf_ptr != NULL)) 
	{	 
		buffer = (u8 *)cmd_ptr->buf_ptr;

		if (i2c_io_info.state == I2C_IDLE_S)
		{
			/* Set up slave for read operation */
			status = fuel_gpio_i2c_setup_slave(cmd_ptr, I2C_READ);
			/* Read data until (N-1)th byte */
			if (status == I2C_SUCCESS)
			{
				status = fuel_gpio_fuel_gpio_i2c_read_bytes(&buffer[index], cmd_ptr->len - 1, &index);
			}
			/* Read the last data byte */
			if (status == I2C_SUCCESS)
			{
				status = fuel_gpio_fuel_gpio_i2c_read_last(&buffer[index++]);
			}
			/* Set the index of the last byte written to the slave */
			cmd_ptr->last_index = index;

			if (status != I2C_SUCCESS)
			{
				fuel_gpio_i2c_stop(FALSE);
			}
		}
	}

	return (status);
} /* fuel_gpio_i2c_read */


i2c_status_type fuel_gpio_i2c_write(i2c_rw_cmd_type	*cmd_ptr  /* pointer to user command buffer */)
{
	unsigned short 					index	= 0;				  /* loop variable			 */
	i2c_status_type status  = I2C_INVALID_ACCESS; /* I2C operation status    */
	u8						*buffer = NULL; 			  /* Write buffer's pointer  */

	if ((cmd_ptr != NULL) && (cmd_ptr->buf_ptr != NULL)) 
	{
		buffer = (u8 *)cmd_ptr->buf_ptr;

		if (i2c_io_info.state == I2C_IDLE_S) 
		{
			/* Set up slave for write operation. */
			status = fuel_gpio_i2c_setup_slave(cmd_ptr, I2C_WRITE);

			/* Write data until (N-1)th byte */
			if (status == I2C_SUCCESS)
			{
				status = fuel_gpio_fuel_gpio_i2c_write_bytes(&buffer[index], cmd_ptr->len - 1,
						&index);
			}

			/* Write the last data byte */
			if (status == I2C_SUCCESS)
			{
				status = fuel_gpio_fuel_gpio_i2c_write_last(buffer[index++]);
			}

			/* Set the index of the last byte written to the slave */
			cmd_ptr->last_index = index;

			/* Error if write failed. Not fatal! */
			if (status != I2C_SUCCESS)
			{
				fuel_gpio_i2c_stop(FALSE);
			}
		}
	}

	return (status);
} /* fuel_gpio_i2c_write */

i2c_status_type I2C1_read(u8 reg_addr, u8 *pBuffer, u8 len, u8 retries_cnt)
{
	i2c_status_type result = I2C_FAILURE;
	i2c_rw_cmd_type cmd;

	do
	{
		fuel_gpio_i2c_init();
		cmd.bus_id = I2C_BUS_IO_CTRL ;
		cmd.slave_addr = FEUL_I2C_CONTROLLER_ADDR;
		cmd.options = (I2C_REG_DEV | I2C_STOP_START_BEFORE_READ);
		cmd.buf_ptr = pBuffer;  
		cmd.addr.reg = reg_addr;
		cmd.len = len;
		result = fuel_gpio_i2c_read(&cmd);
	}while ((result != I2C_SUCCESS) && ((retries_cnt--) > 0));

	return result;
}

i2c_status_type I2C1_write(u8 reg_addr, u8 *pBuffer, u8 len, u8 retries_cnt)
{
	i2c_status_type result = I2C_FAILURE;
	i2c_rw_cmd_type cmd;

	do
	{
		fuel_gpio_i2c_init();
		cmd.bus_id = I2C_BUS_IO_CTRL ;
		cmd.slave_addr = FEUL_I2C_CONTROLLER_ADDR;
		cmd.options = I2C_REG_DEV;
		cmd.buf_ptr = pBuffer;  
		cmd.addr.reg = reg_addr;
		cmd.len = len;
		result = fuel_gpio_i2c_write(&cmd);
	}while ((result != I2C_SUCCESS) && ((retries_cnt--) > 0));

	return result;
}
