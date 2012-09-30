/*
 * CopyLeft
 */

#include <common.h>
#include <asm/arch/bits.h>
#include <asm/io.h>
#include <asm/arch/gpio.h>

#include <cosmo_muic.h>
#include <charging_ic.h>


#define CHG_EN_SET_GPIO 	83
#define CHG_STATUS_GPIO		51
#define CHR_IC_DEALY		300	/* 200 us */
#define CHR_IC_SET_DEALY	3000	/* 1500 us */
//#define CHR_IC_DEALY		500		/* 500 us */
//#define CHR_IC_SET_DEALY	2000	/* >= 1500 us */

#define CHR_IC_INIT_DELAY	1500	


static int charging_mode;


int get_charging_mode()
{
	return charging_mode;
}

int chr_ic_active_for_usb ()
{
	/* charging current setting: Default mode */
	D("charging_mod = %d", charging_mode);

	if(charging_mode == CHARGING_USB)
	{
		D("\n\n CHARGING_IC ::It's already chr_ic_active_for_usb \n\n");
		return charging_mode;
	}

	if(charging_mode != CHARGING_DEACTIVE)
	{
		chr_ic_deactivation();
	}

	omap_set_gpio_dataout(CHG_EN_SET_GPIO, 0);
	udelay(CHR_IC_SET_DEALY);	// 1.5 ms >
	
	D("\n\n CHARGING_IC :: chr_ic_active_for_usb\n\n");
	charging_mode = CHARGING_USB;
	return charging_mode;
}


int chr_ic_active_for_ta ()
{
	/* charging current setting: one pluse mode */
	D("charging_mod = %d", charging_mode);

	if(charging_mode == CHARGING_TA)
	{
		D("\n\n CHARGING_IC ::It's already chr_ic_active_for_ta \n\n");
		return charging_mode;
	}


	if(charging_mode != CHARGING_DEACTIVE)
	{
		chr_ic_deactivation();
	}

	omap_set_gpio_dataout(CHG_EN_SET_GPIO, 0);
	udelay(CHR_IC_SET_DEALY);	// 1.5 ms >

	omap_set_gpio_dataout(CHG_EN_SET_GPIO, 1);
	udelay(CHR_IC_DEALY);		/* 100 us > */
	omap_set_gpio_dataout(CHG_EN_SET_GPIO, 0);
	udelay(CHR_IC_SET_DEALY);	/* 1.5 ms > */

	D("\n\n CHARGING_IC :: chr_ic_active_for_ta\n\n");
	charging_mode = CHARGING_TA;
	return charging_mode;
}


int chr_ic_active_for_factory_mode ()
{
	/* charging current setting: ~ 2.3A (three pluse mode) */

	u8 cnt = 0;

	D("charging_mod = %d", charging_mode);
	
	if(charging_mode == CHARGING_TA)
	{
		D("\n\n CHARGING_IC ::It's already chr_ic_active_for_factory_mode \n\n");
		return charging_mode;
	}

	if(charging_mode != CHARGING_DEACTIVE)
	{
		chr_ic_deactivation();
	}

	omap_set_gpio_dataout(CHG_EN_SET_GPIO, 0);
	udelay(CHR_IC_SET_DEALY);	// 1.5 ms >

	for (cnt =0; cnt < 3; cnt++) {
		omap_set_gpio_dataout(CHG_EN_SET_GPIO, 1);
		udelay(CHR_IC_DEALY);	/* 100 us > */
		omap_set_gpio_dataout(CHG_EN_SET_GPIO, 0);
		udelay(CHR_IC_DEALY);	/* 100 us > */
	}

	udelay(CHR_IC_SET_DEALY);	/* 1.5 ms > */


	D("\n\n CHARGING_IC :: chr_ic_active_for_factory\n\n");
	charging_mode = CHARGING_FACTORY;
	return charging_mode;
}


int chr_ic_deactivation ()
{
	omap_set_gpio_dataout(CHG_EN_SET_GPIO, 1);
	udelay(CHR_IC_SET_DEALY);	// 1.5 ms >


	D("\n\n CHARGING_IC :: chr_ic_deactive!!!\n\n");
	charging_mode = CHARGING_DEACTIVE;
	return charging_mode;
}

int chr_ic_init(int muic_mode)
{
	omap_set_gpio_direction (CHG_STATUS_GPIO, 1);
	udelay(CHR_IC_DEALY);	/* 100 us > */

	omap_set_gpio_direction(CHG_EN_SET_GPIO, 0);
	udelay(CHR_IC_DEALY);	/* 100 us > */

	charging_mode = CHARGING_DEACTIVE;

	return 1;
}



u32 get_chr_status (void)
{
	return omap_get_gpio_datain (CHG_STATUS_GPIO);
}

