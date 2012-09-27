/*
	Copyleft
*/

#include <common.h>

#include <asm/io.h>

#define OMAP_GPIO_MODULE	6

/* omap4430 specific GPIO registers */
#define OMAP44XX_GPIO1_BASE	0x4A310000
#define OMAP44XX_GPIO2_BASE	0x48055000
#define OMAP44XX_GPIO3_BASE	0x48057000
#define OMAP44XX_GPIO4_BASE	0x48059000
#define OMAP44XX_GPIO5_BASE	0x4805B000
#define OMAP44XX_GPIO6_BASE	0x4805D000

#define OMAP44XX_GPIO_REVISION			0x000
#define OMAP44XX_GPIO_SYSCONFIG			0x010
#define OMAP44XX_GPIO_SYSSTATUS			0x114
#define OMAP44XX_GPIO_IRQSTATUS1		0x118
#define OMAP44XX_GPIO_IRQENABLE1		0x11C
#define OMAP44XX_GPIO_WAKEUPENABLE		0x120
#define OMAP44XX_GPIO_IRQSTATUS2		0x128
#define OMAP44XX_GPIO_IRQENABLE2		0x12C
#define OMAP44XX_GPIO_CTRL				0x130
#define OMAP44XX_GPIO_OE				0x134
#define OMAP44XX_GPIO_DATAIN			0x138
#define OMAP44XX_GPIO_DATAOUT			0x13C
#define OMAP44XX_GPIO_LEVELDETECT0		0x140
#define OMAP44XX_GPIO_LEVELDETECT1		0x144
#define OMAP44XX_GPIO_RISINGDETECT		0x148
#define OMAP44XX_GPIO_FALLINGDETECT		0x14C
#define OMAP44XX_GPIO_DEBOUNCENABLE		0x150
#define OMAP44XX_GPIO_DEBOUNCINGTIME	0x154
#define OMAP44XX_GPIO_CLEARIRQENABLE1	0x160
#define OMAP44XX_GPIO_SETIRQENABLE1		0x164
#define OMAP44XX_GPIO_CLEARIRQENABLE2	0x170
#define OMAP44XX_GPIO_SETIRQENABLE2		0x174
#define OMAP44XX_GPIO_CLEARWKUENA		0x180
#define OMAP44XX_GPIO_SETWKUENA			0x184
#define OMAP44XX_GPIO_CLEARDATAOUT		0x190
#define OMAP44XX_GPIO_SETDATAOUT		0x194

/* for swtching gpio bank */
struct gpio_bank {
	unsigned int base;
};


static const struct gpio_bank omap44xx_gpio_bank[OMAP_GPIO_MODULE] = {
	{OMAP44XX_GPIO1_BASE},
	{OMAP44XX_GPIO2_BASE},
	{OMAP44XX_GPIO3_BASE},
	{OMAP44XX_GPIO4_BASE},
	{OMAP44XX_GPIO5_BASE},
	{OMAP44XX_GPIO6_BASE},
};


static inline struct gpio_bank *get_gpio_bank(int gpio)
{
	return &omap44xx_gpio_bank[gpio >> 5];
}


static inline int get_gpio_index(int gpio)
{
	return gpio & 0x1f;
}


static inline int gpio_valid(int gpio)
{
	if (gpio < (32 * OMAP_GPIO_MODULE))
		return 0;

	printf("omap-gpio: invalid GPIO %d\n", gpio);
	return -1;
}


void omap_set_gpio_direction(int gpio, int is_input)
{
	unsigned int val;
	struct gpio_bank *bank;

	if (gpio_valid(gpio) < 0)
		return;

	bank = get_gpio_bank(gpio);
	
	val = __raw_readl(bank->base + OMAP44XX_GPIO_OE);
	if (is_input)
		val |= 1 << get_gpio_index(gpio);
	else
		val &= ~(1 << get_gpio_index(gpio));
	__raw_writel(val, bank->base + OMAP44XX_GPIO_OE);
}


void omap_set_gpio_dataout(int gpio, int enable)
{
	unsigned int val;
	struct gpio_bank *bank;

	if (gpio_valid(gpio) < 0)
		return;

	bank = get_gpio_bank(gpio);
	
	val = 1 << get_gpio_index(gpio);
	if (enable) {
		__raw_writel(val, bank->base + OMAP44XX_GPIO_SETDATAOUT);
	} else {
		__raw_writel(val, bank->base + OMAP44XX_GPIO_CLEARDATAOUT);
	}
}


int omap_get_gpio_datain(int gpio)
{
	struct gpio_bank *bank;

	if (gpio_valid(gpio) < 0)
		return;

	bank = get_gpio_bank(gpio);

	return (__raw_readl(bank->base + OMAP44XX_GPIO_DATAIN) & (1 << get_gpio_index(gpio))) != 0;
}
