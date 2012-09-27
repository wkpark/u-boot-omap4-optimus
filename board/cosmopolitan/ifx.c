#include <common.h>
#include <asm/arch/bits.h>
#include <asm/io.h>
#include <asm/arch/gpio.h>

#if defined(CONFIG_COSMO_EVB_LAB4)
#define MODEM_PWRON_GPIO	92	
#else
#define MODEM_PWRON_GPIO	164
#endif
#define USIF1_SW_GPIO		165
#define IFX_PWR_ON_SW_GPIO 	3   	

void ifx_uart_sw_ctrl(int sel);
void ifx_start_low();

void ifx_boot_start()
{
	u32 val;

		printf("[e] ifx_start_low\n");

		*((volatile unsigned int *)0x4A10016C) &= ~(0x7 << 16);
		*((volatile unsigned int*) 0x4A10016C) |= 0x00030000; 

		omap_set_gpio_direction(MODEM_PWRON_GPIO, 0); 	

	udelay(100000);
		omap_set_gpio_dataout(MODEM_PWRON_GPIO, 1); 		

	*((volatile unsigned int *)0x4A31E044) &= ~(0x7 << 16);
	*((volatile unsigned int *)0x4A31E044) |= (0x3<<16); 

	omap_set_gpio_direction(IFX_PWR_ON_SW_GPIO, 0);  

	omap_set_gpio_dataout(IFX_PWR_ON_SW_GPIO, 0);
	printf("[e] IFX_PWR_ON_SW_GPIO low\n"); 									 
	udelay(500000);  

	omap_set_gpio_dataout(IFX_PWR_ON_SW_GPIO, 1);
	printf("[e] IFX_PWR_ON_SW_GPIO high\n");				

	*((volatile unsigned int*) 0x4A100110) &= 0x00000000; 
	*((volatile unsigned int*) 0x4A100110) |= 0x00030000; 	
	*((volatile unsigned int*) 0x4A100110) |= 0x00000003; 	
	*((volatile unsigned int*) 0x4A100110) |= 0x00000100; 
	*((volatile unsigned int*) 0x4A100110) |= 0x00004200; 

	printf("[e] MRDY SRDY Set\n");	
}

void ifx_start_low()
{
	u32 val;

    printf("[e] ifx_start_low\n");

   	
   	*((volatile unsigned int *)0x4A10016C) &= ~(0x7 << 16);
   	*((volatile unsigned int*) 0x4A10016C) |= 0x00030000; 

   	omap_set_gpio_direction(MODEM_PWRON_GPIO, 0);  	

   	omap_set_gpio_dataout(MODEM_PWRON_GPIO, 1);    	

	*((volatile unsigned int *)0x4A31E044) &= ~(0x7 << 16);
	*((volatile unsigned int *)0x4A31E044) |= (0x3<<16); 

	omap_set_gpio_direction(IFX_PWR_ON_SW_GPIO, 0);  

	omap_set_gpio_dataout(IFX_PWR_ON_SW_GPIO, 0);
	printf("[e] IFX_PWR_ON_SW_GPIO low\n");                    
	udelay(10);  

	omap_set_gpio_dataout(IFX_PWR_ON_SW_GPIO, 1);
	printf("[e] IFX_PWR_ON_SW_GPIO high\n");        

#if !defined(CONFIG_COSMO_MIPI_HSI)
	
	*((volatile unsigned int*) 0x4A100110) &= 0x00000000; 
	*((volatile unsigned int*) 0x4A100110) |= 0x00030000; 	
	*((volatile unsigned int*) 0x4A100110) |= 0x00000003; 	
	*((volatile unsigned int*) 0x4A100110) |= 0x00000100; 
	*((volatile unsigned int*) 0x4A100110) |= 0x00004200; 

	printf("[e] MRDY SRDY Set\n");	
#endif	

}

void ifx_uart_sw_ctrl(int sel)
{
	printf("[e] IFX_UART_SW_CTRL \n");

	*((volatile unsigned int *)0x4A100170) &= ~(0x7);
	*((volatile unsigned int*) 0x4A100170)|=0x00000003;

	omap_set_gpio_direction(USIF1_SW_GPIO, 0);

	if (sel)
		omap_set_gpio_dataout(USIF1_SW_GPIO, 1); 
	else
		omap_set_gpio_dataout(USIF1_SW_GPIO, 0); 
}

void ifx_dl_reset()
{
	u32 val;

    printf("[e] ifx_start_low\n");

  	
   	*((volatile unsigned int *)0x4A10016C) &= ~(0x7 << 16);
   	*((volatile unsigned int*) 0x4A10016C) |= 0x00030000; 

   	omap_set_gpio_direction(MODEM_PWRON_GPIO, 0);  	
	omap_set_gpio_dataout(MODEM_PWRON_GPIO, 0); 	
	udelay(20000);

	*((volatile unsigned int *)0x4A31E044) &= ~(0x7 << 16);
	*((volatile unsigned int *)0x4A31E044) |= (0x3<<16); 

	omap_set_gpio_direction(IFX_PWR_ON_SW_GPIO, 0);  

	omap_set_gpio_dataout(IFX_PWR_ON_SW_GPIO, 0);
	udelay(2000000);  

extern  muic_for_download(int);
         muic_for_download(1);
	printf("[e] IFX_PWR_ON_SW_GPIO low\n");                    
	udelay(3000000);  
	omap_set_gpio_dataout(MODEM_PWRON_GPIO, 1);
	udelay(100000);  

	omap_set_gpio_dataout(IFX_PWR_ON_SW_GPIO, 1);
	printf("[e] IFX_PWR_ON_SW_GPIO high\n");        

#if !defined(CONFIG_COSMO_MIPI_HSI)
	
	*((volatile unsigned int*) 0x4A100110) &= 0x00000000; 
	*((volatile unsigned int*) 0x4A100110) |= 0x00030000; 	
	*((volatile unsigned int*) 0x4A100110) |= 0x00000003; 	
	*((volatile unsigned int*) 0x4A100110) |= 0x00000100; 
	*((volatile unsigned int*) 0x4A100110) |= 0x00004200; 

	printf("[e] MRDY SRDY Set\n");	
#endif	

}
