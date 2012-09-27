/*
 * Copyleft
*/

#ifndef _OMAP44XX_GPIO_H_
#define _OMAP44XX_GPIO_H_

void omap_set_gpio_direction(int gpio, int is_input);
void omap_set_gpio_dataout(int gpio, int enable);
int omap_get_gpio_datain(int gpio);

#endif

