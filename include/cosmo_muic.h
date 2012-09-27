#define	I2C1	0
#define	I2C2	1
#define	I2C3	2
#define	I2C4	3

#define OMAP_I2C_STANDARD	100
#define OMAP_I2C_FAST_MODE	400
#define OMAP_I2C_HIGH_SPEED	3400

#define MUIC_SLAVE_ADDR	(u8)0x44

#define DEVICE_ID	(u8)0x00
#define CONTROL_1	(u8)0x01
#define CONTROL_2	(u8)0x02
#define SW_CONTROL	(u8)0x03
#define INT_STATUS	(u8)0x04
#if defined (CONFIG_LGE_CX2) 
#define STATUS		(u8)0x06
#else
#define STATUS		(u8)0x05
#endif

#define ID_2P2	(u8)0x40
#define ID_620	(u8)0x20
#define ID_200	(u8)0x10	
#if defined (CONFIG_LGE_CX2) 
#define VLDO	(u8)0x00	
#else
#define VLDO	(u8)0x08	
#endif

#define SEMREN	(u8)0x04	
#define ADC_EN	(u8)0x02	
#define CP_EN	(u8)0x01	

#define	INTPOL		(u8)0x80
#define	INT_EN		(u8)0x40	
#define	MIC_LP		(u8)0x20
#define	CP_AUD		(u8)0x10
#define	CHG_TYP		(u8)0x02	
#define	USB_DET_DIS	(u8)0x01

#define VID_ON		(u8)0x80	
#define MIC_ON		(u8)0x40
#define DP_USB		(u8)0x00
#define DP_UART 	(u8)0x08
#define DP_AUDIO	(u8)0x10
#define DP_OPEN		(u8)0x38
#define DM_USB		(u8)0x00
#define DM_UART		(u8)0x01
#define DM_AUDIO	(u8)0x02
#define DM_OPEN		(u8)0x07

#define USB		DP_USB   | DM_USB 	
#define UART 	DP_UART  | DM_UART 	
#define AUDIO	DP_AUDIO | DM_AUDIO 
#define OPEN	DP_OPEN  | DM_OPEN 	

#define COMP2_TO_DP2  	0x00
#define COMP2_TO_U2   	0x08
#define COMP2_TO_AUD2 	0x10
#define COMP2_TO_HZ   	0x20

#define COMN1_TO_DN1  	0x00
#define COMN1_TO_U1   	0x01
#define COMN1_TO_AUD1 	0x02
#define COMN1_TO_C1COMP 0x03
#define COMN1_TO_HZ   	0x04

#define CHGDET		(u8)0x80
#define MRCOMP		(u8)0x40	
#define SENDEND		(u8)0x20	
#define VBUS		(u8)0x10	
#define IDNO		(u8)0x0f	

#define	DCPORT		(u8)0x80
#define	CHPORT		(u8)0x40
#define C1COMP    	(u8)0x01	

#define TS5USBA33402 	1
#define MAX14526 		2
#define ANY_VANDOR 		3
#define ANY_VANDOR_ID 	0xff

#define NORMAL_MODE		0x00
#define AT_CMD_MODE		0x01

#define FIRST_BOOT_MODE 0x01

#define UART_SW1_GPIO	11
#define UART_SW2_GPIO	12

#define USIF1_SW_GPIO	165

#define IFX_USB_VBUS_EN_GPIO 55

typedef enum {
	USIF1_IPC,		
	USIF1_UART,		
	USIF1_NONE,
} USIF1_MODE_TYPE;

typedef enum {
	DP3T_NC,
	DP3T_S1_AP_UART,	
	DP3T_S2_CP_UART,	
	DP3T_S3_CP_USB,		
} DP3T_MODE_TYPE;

typedef enum {
	NO_RETAIN,
	FIRST_BOOT,
	RETAIN_AP_USB,
	RETAIN_CP_USB,
} TYPE_RETAIN_MODE;

typedef enum {
	MUIC_OPEN,   			
	MUIC_CHRGER,   			
	MUIC_FACTORY_MODE,   	
	MUIC_HAEDSET,   		
	MUIC_USB,  	 			
	MUIC_TV_OUT,   			
	MUIC_FULL_USB,		   	
	MUIC_OTG,  	 			
	MUIC_DEVELOP_MODE,   	
	MUIC_TA_1A,				
	MUIC_RESERVE_1,
	MUIC_RESERVE_2,
	MUIC_END,
} MUIC_MODE_TYPE;

extern void ifx_uart_sw_ctrl(int sel);
extern int select_bus(int bus, int speed);

int get_dload_mode();
int get_dp3t_mode(void);

int muic_init(int);

void ifx_vbus_ctrl(int output);
void dp3t_switch_ctrl(DP3T_MODE_TYPE mode);
void usif1_switch_ctrl(USIF1_MODE_TYPE mode);

int muic_i2c_write_byte(unsigned char addr, unsigned char data);
int muic_i2c_read_byte(unsigned char addr, unsigned char *value);

void write_muic_mode_of_dload(unsigned char flag);
int read_muic_mode_of_dload(void);

void muic_AP_USB(void);
