#if !defined(_TH_I2C_)
#define _TH_I2C_

//#include "bastypes.h"

//---------------------------------------------------------------------------------------
// Global defines.
//---------------------------------------------------------------------------------------

/***************************************************************************
 * Definitions for I2C software
 **************************************************************************/

#define FALSE	0

//#define MUIC_I2C_CONTROLLER_ADDR 0x88
#define FEUL_I2C_CONTROLLER_ADDR	0x6C

#define	I2C_4_7_MICROSEC_WAIT	 WAIT_FOR_USECS(1)  /* 5.2 us */
#define	I2C_4_MICROSEC_WAIT 	 WAIT_FOR_USECS(1)  /* 4.3 us */  // 125KHz

/* minimum time SCL must remain low while clocking a data bit              */
#define	I2C_TLOW_DELAY         		I2C_4_7_MICROSEC_WAIT
/* minimum time SCL must remain high while clocking a data bit             */
#define	I2C_THIGH_DELAY        		I2C_4_MICROSEC_WAIT
/* minimum time SCL must remain low after a stop condition                 */
#define	I2C_TBUF_DELAY         		I2C_4_7_MICROSEC_WAIT
/* Repeated Start condition setup time                                     */
#define	I2C_TSU_STA_DELAY      	I2C_4_7_MICROSEC_WAIT
/* Start condition hold time                                               */
#define	I2C_THD_STA_DELAY      	I2C_4_MICROSEC_WAIT
/* Stop condition setup time                                               */
#define	I2C_TSU_STO_DELAY      	I2C_4_MICROSEC_WAIT

#define	ACK_TIME_OUT	5000		/*100 --> 5000*/  /* charary 2006.07.04 */

#define STM_FACTOR         1



void fuel_busy_delay(unsigned int count);

#if 0
#define WAIT_FOR_USECS(A)  do {\
                               signed int gpio_i2c_wait_endtime;\
                               gpio_i2c_wait_endtime = (signed int)getStm_STM_TIM0(&stm) + (A * STM_FACTOR);\
                               while ((gpio_i2c_wait_endtime-(signed int)getStm_STM_TIM0(&stm)) > 0); \
                               }while (0)
#endif


#if 0
#define WAIT_FOR_USECS(A)  do {\
								busy_delay = 0; \
								while(busy_delay < A*STM_FACTOR) \
								{ \
									busy_delay = busy_delay + 1; \
								} \
							}while (0)
#else
#define WAIT_FOR_USECS(A)  /*fuel_busy_delay(A)*/
#endif

/***************************************************************************
 * Type Definitions for I2C software
 **************************************************************************/
 /* I2C status types for return values from i2c_read() and i2c_write()      */
typedef enum i2c_status_typeTag
{
	I2C_SUCCESS,                       	/* operation successful               */
	I2C_FAILURE,                       	/* operation failed                   */
	I2C_INVALID_LEN,                   	/* invalid length specified for I/O   */
	I2C_INVALID_BUS,                   	/* invalid bus id                     */
	I2C_INVALID_ACCESS                 	/* illegal access                     */
} i2c_status_type;

/* I2C bus states */
typedef enum i2c_state_typeTag
{
	I2C_IDLE_S,                      		/* I2C bus idle state                   */
	I2C_TX_S,                        		/* I2C Tx data state                    */
	I2C_RX_S,                        		/* I2C Rx data state                    */
	I2C_UNINIT_S,                    		/* I2C Uninit state                     */
	I2C_FAIL_S                       		/* I2C Failure state                    */
} i2c_state_type;

/* I2C bus identifiers. curr we use only one i2c port*/
typedef enum i2c_bus_typeTag
{
	I2C_BUS_IO_CTRL,
	I2C_BUS_IO1_CTRL,
	I2C_BUS_IO2_CTRL
}i2c_bus_type;

/* I2C I/O operation types */
typedef enum i2c_op_typeTag
{
	I2C_READ,
	I2C_WRITE
} i2c_op_type;

typedef u8 i2c_dev_addr_type;      /* I2C device address type            */

typedef enum i2c_options_typeTag
{
	I2C_DFLT_ADDR_DEV          	= 0x0001, /* Use default address for I/O      */
	I2C_MEM_DEV                 = 0x0002, /* memory device (16 bit addresses) */
	I2C_REG_DEV                	= 0x0004, /* 8 bit register based device      */
	I2C_NACK_LAST_BYTE         	= 0x0008, /* NACK last byte read from slave   */
	I2C_START_BEFORE_READ		= 0x0020, /* During a read transaction, gen.  */
		/* a repeated START after writing   */
		/* the slave addr(just before read) */
	I2C_STOP_START_BEFORE_READ 		= 0x0040, /* During a read transaction, gen.  */
		/* a STOP & START after writing     */
		/* the slave addr(just before read) */
	I2C_SLAVE_INTERLEAVE 			= 0x100 ,
	I2C_STOP_START_BEFORE_WRITE 	= 0x0200
	/* During a read transaction, gen.  */
		/* a STOP & START after writing     */
		/* the slave addr(just before read) */
} i2c_options_type;

/* read/write command type for i2c slave I/O operation */
typedef  struct i2c_rw_cmd_typeTag
{
	i2c_bus_type      	bus_id;          	/* specified I2C bus id               */
	i2c_dev_addr_type 	slave_addr;      	/* slave address                      */
	i2c_options_type  	options;        	 /* bit mask of slave control actions  */
	void             			*buf_ptr;         	/* pointer to buffer in caller space  */
	union 
	{
		u16         		  	offset;          		/* offset in I2C device to read/write */
		u8           	 	reg;             		/* address of register in slave       */
	} addr;
	u16              		len;            		/* count of bytes to transfer         */
	u16              		last_index;      	/* index of last byte written         */
} i2c_rw_cmd_type;

/* buffer type describing GPIO id and SCL/SDA masks */
typedef struct{
  i2c_state_type        state;      	/* bus state                            */
} i2c_gpio_type;

/***************************************************************************
 * Definitions for I2C software
 **************************************************************************/
#define	CTRL_REG		  0x00
#define	WRITE_MASK		0x00

#define	I2C_ACK           	0       	/* Slave pulls down SDA during SCL hi    */
#define	I2C_NACK          	1       	/* Slave pulls up SDA during SCL hi      */
#define	I2C_MAX_DATA_LEN  	0x8000 	/* Max data transfer len < 32K           */

/* Slave address read/write bit mask definitions                           */
#define	I2C_IO_MASK       		0x01    /* r/w bit mask                          */
#define	I2C_IO_MASK_R     		0x01    /* mask for read operation               */
#define	I2C_IO_MASK_W     		0x00    /* mask for write operation              */

#define	I2C_ADDR_MASK_R(addr)	((addr & ~I2C_IO_MASK) | I2C_IO_MASK_R)
#define	I2C_ADDR_MASK_W(addr)	((addr & ~I2C_IO_MASK) | I2C_IO_MASK_W)

// KE800 NOHYONGGYUN 20060712 Move from "key_tk.c" to "key_tk.h" [START] 
#define TESTBIT(A,B)   ((A) & (1u << (B)))
#define SETBIT(A,B)    ((A) |= (1u << (B)))
#define RESETBIT(A,B)  ((A) &= ~(1u << (B)))

#define TPAD_READ_NO					9
//---------------------------------------------------------------------------------------
// Global data declarations.
//---------------------------------------------------------------------------------------
#define I2C_HIGH_CONFIG   (PCL_ENABLE | PCL_NO_PUPD | PCL_OPEN_DRAIN | PCL_GPIO | PCL_GPIO_IN)
#define I2C_LOW_CONFIG    (PCL_ENABLE | PCL_NO_PUPD | PCL_OPEN_DRAIN | PCL_GPIO | PCL_GPIO_OUT | PCL_GPIO_D0)
#define I2C_INPUT_CONFIG  (PCL_ENABLE | PCL_NO_PUPD| PCL_OPEN_DRAIN| PCL_GPIO |  PCL_GPIO_IN)

//---------------------------------------------------------------------------------------
// Global function prototypes.
//---------------------------------------------------------------------------------------
extern int ICU_disable_global_irq(void);
extern int ICU_enable_global_irq(void);

extern i2c_status_type I2C1_read(u8 reg_addr, u8 *pBuffer, u8 len, u8 retries_cnt);
extern i2c_status_type I2C1_write(u8 reg_addr, u8 *pBuffer, u8 len, u8 retries_cnt);
void fuel_set_gpio_dataout(int gpio, int value);
#endif	//#if !defined(_TH_I2C_)
