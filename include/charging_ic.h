typedef enum
{
	CHARGING_DEACTIVE,  
	CHARGING_USB,		
	CHARGING_TA,		
	CHARGING_FACTORY,	
	CHARGING_END,		
} CAHRGING_STATUS;

typedef enum {          
	PW_CON_CHARGING,
	PW_CON_BATTERY_FULL,
	PW_CON_POWER_BUTTON_PUSHED,
	PW_CON_NO_CHARGER,
} POWER_CONDITION ;

int chr_ic_init(int muic_mode);
int chr_ic_active_for_usb (void);
int chr_ic_active_for_ta (void);
int chr_ic_active_for_factory_mode (void);
int chr_ic_deactivation (void);
u32 get_chr_status (void);
int get_charging_mode(void);

#define DEBUG
#if 0
#define D(fmt, args...) printf(fmt " :: file=%s, func=%s, line=%d\n", ##args, __FILE__, __func__, __LINE__ ) 

#else
#define D(fmt, args...)
#endif
