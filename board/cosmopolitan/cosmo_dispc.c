/*
 * linux/drivers/video/omap2/dss/dispc.c
 *
 * Copyright (C) 2009 Nokia Corporation
 * Author: Tomi Valkeinen <tomi.valkeinen@nokia.com>
 *
 * Some code and ideas taken from drivers/video/omap/ driver
 * by Imre Deak.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define DSS_SUBSYS_NAME "DISPC"

#include <common.h>
#include <asm-arm/arch-omap4/bits.h>
#include <asm-arm/io.h>
#include <asm-arm/arch-omap4/gpio.h>
#include <asm-arm/arch-omap4/cpu.h>
#include <asm-arm/errno.h>
#include "cosmo_dss.h"
#include "tiler.h"
#include <linux/mtd/compat.h>

   	/* DSS */
#define DSS_BASE                0x58000000
   	/* DISPLAY CONTROLLER */
#define DISPC_BASE              0x58001000

#define DSS_SZ_REGS				0x00000200

#define REG_GET(idx, start, end) \
	FLD_GET(dispc_read_reg(idx), start, end)

#define REG_FLD_MOD(idx, val, start, end)				\
	dispc_write_reg(idx, FLD_MOD(dispc_read_reg(idx), val, start, end))

static inline void dispc_write_reg(const struct dispc_reg idx, u32 val)
{
	__raw_writel(val, DISPC_BASE + idx.idx);
}

static inline u32 dispc_read_reg(const struct dispc_reg idx)
{
	return __raw_readl(DISPC_BASE + idx.idx);
}

void dispc_go(enum omap_channel channel)
{
	if (REG_GET(DISPC_CONTROL2, 0, 0) == 0)
		return;

	if (REG_GET(DISPC_CONTROL2, 5, 5) == 1) {
		printf("GO bit not down for \n");
		return;
	}

	printf("GO LCD2");

	REG_FLD_MOD(DISPC_CONTROL2, 1, 5, 5);	
}

void dispc_set_lcd_size(enum omap_channel channel, u16 width, u16 height)
{
	u32 val;
	BUG_ON((width > (1 << 11)) || (height > (1 << 11)));
	val = FLD_VAL(height - 1, 26, 16) | FLD_VAL(width - 1, 10, 0);
#ifdef CONFIG_ARCH_OMAP4
	if (OMAP_DSS_CHANNEL_LCD2 == channel)
		dispc_write_reg(DISPC_SIZE_LCD2, val);
	else
#endif
	dispc_write_reg(DISPC_SIZE_LCD, val);
}

void dispc_enable_lcd_out(bool enable)
{
	bool is_on;

	/* When we disable LCD output, we need to wait until frame is done.
	 * Otherwise the DSS is still working, and turning off the clocks
	 * prevents DSS from going to OFF mode */
	is_on = REG_GET(DISPC_CONTROL2, 0, 0);

	if (!enable && is_on) {
		mdelay(100);
	}

	REG_FLD_MOD(DISPC_CONTROL2, enable ? 1 : 0, 0, 0);

	if (!enable && is_on) {
		mdelay(100);
	}

}


void dispc_pck_free_enable(bool enable)
{
	REG_FLD_MOD(DISPC_CONTROL, enable ? 1 : 0, 27, 27);
}

void dispc_set_control2_reg()
{
	u32 l;
	int stallmode = 0; //OMAP_DSS_PARALLELMODE_BYPASS
	int lcd_mode = 1; //SET TFT
	int fifo_hancheck = 0;
	int tftline_code = 3; // tft line = 24 -> code 3
	
	l = dispc_read_reg(DISPC_CONTROL2);

	l = FLD_MOD(l, stallmode, 11, 11);
	l = FLD_MOD(l, lcd_mode, 3, 3);
	l = FLD_MOD(l, fifo_hancheck, 16, 16);
	l = FLD_MOD(l, tftline_code, 9, 8);

	dispc_write_reg(DISPC_CONTROL2, l);
}

void dispc_set_lcd_timings(enum omap_channel channel, struct omap_video_timings *timings)
{
	u32 timing_h, timing_v;

	timing_h = FLD_VAL(timings->hsw-1, 7, 0) | FLD_VAL(timings->hfp-1, 19, 8) |
				FLD_VAL(timings->hbp-1, 31, 20);

	timing_v = FLD_VAL(timings->vsw-1, 7, 0) | FLD_VAL(timings->vfp, 19, 8) |
				FLD_VAL(timings->vbp, 31, 20);	 

#ifdef CONFIG_ARCH_OMAP4
	dispc_write_reg(DISPC_TIMING_H2, timing_h);
	dispc_write_reg(DISPC_TIMING_V2, timing_v);
#endif
	printf("DOLCOM : timing %x %x\n", timing_h, timing_v);
}

static void dispc_get_lcd_divisor(enum omap_channel channel,
					int *lck_div, int *pck_div)
{
	u32 l;

#ifdef CONFIG_ARCH_OMAP4
	if (OMAP_DSS_CHANNEL_LCD2 == channel)
		l = dispc_read_reg(DISPC_DIVISOR2);
	else
		l = dispc_read_reg(DISPC_DIVISOR1);
#else
		l = dispc_read_reg(DISPC_DIVISOR);
#endif
	*lck_div = FLD_GET(l, 23, 16);
	*pck_div = FLD_GET(l, 7, 0);
}
/* TODO: Check with Senthil on handling of clocks */

static inline u32 dss_read_reg(const struct dss_reg idx)
{
	return __raw_readl(DSS_BASE + idx.idx);
}


int dss_get_dispc_clk_source(void)
{
	return FLD_GET(dss_read_reg(DSS_CONTROL), 0, 0);  
}

unsigned long dispc_fclk_rate(void)
{
	unsigned long r = 0;

	if (dss_get_dispc_clk_source() == 0)
		r = dss_clk_get_rate(DSS_CLK_FCK1);
	else
#ifdef CONFIG_OMAP2_DSS_DSI
		r = dsi_get_dsi1_pll_rate(0);
#else
	BUG();
#endif
	return r;
}

unsigned long dispc_lclk_rate(enum omap_channel channel)
{
	int lcd;
	unsigned long r;
	u32 l;
#ifdef CONFIG_ARCH_OMAP4
	if (OMAP_DSS_CHANNEL_LCD2 == channel)
		l = dispc_read_reg(DISPC_DIVISOR2);
	else
		l = dispc_read_reg(DISPC_DIVISOR1);
#else
	l = dispc_read_reg(DISPC_DIVISOR);
#endif
	lcd = FLD_GET(l, 23, 16);

	r = dispc_fclk_rate();

	return r / lcd;
}

unsigned long dispc_pclk_rate(enum omap_channel channel)
{
	int lcd, pcd;
	unsigned long r;
	u32 l;
#ifdef CONFIG_ARCH_OMAP4
	if (OMAP_DSS_CHANNEL_LCD2 == channel)
		l = dispc_read_reg(DISPC_DIVISOR2);
	else
		l = dispc_read_reg(DISPC_DIVISOR1);
#else
	l = dispc_read_reg(DISPC_DIVISOR);
#endif
	lcd = FLD_GET(l, 23, 16);
	pcd = FLD_GET(l, 7, 0);

	r = dispc_fclk_rate();

	return r / lcd / pcd;
}


void dispc_set_pol_freq(enum omap_panel_config config,
							u8 acbi, u8 acb)
{
	u32 l = 0;
	
	l |= FLD_VAL((config & OMAP_DSS_LCD_ONOFF) != 0, 17, 17);
	l |= FLD_VAL((config & OMAP_DSS_LCD_RF) != 0, 16, 16);
	l |= FLD_VAL((config & OMAP_DSS_LCD_IEO) != 0, 15, 15);
	l |= FLD_VAL((config & OMAP_DSS_LCD_IPC) != 0, 14, 14);
	l |= FLD_VAL((config & OMAP_DSS_LCD_IHS) != 0, 13, 13);
	l |= FLD_VAL((config & OMAP_DSS_LCD_IVS) != 0, 12, 12);
	l |= FLD_VAL(acbi, 11, 8);
	l |= FLD_VAL(acb, 7, 0);
	dispc_write_reg(DISPC_POL_FREQ2, l);
}

/* with fck as input clock rate, find dispc dividers that produce req_pck */
void dispc_find_clk_divs(bool is_tft, unsigned long req_pck, unsigned long fck,
		struct dispc_clock_info *cinfo)
{
	u16 pcd_min = is_tft ? 2 : 3;
	unsigned long best_pck;
	u16 best_ld, cur_ld;
	u16 best_pd, cur_pd;

	best_pck = 0;
	best_ld = 0;
	best_pd = 0;

	for (cur_ld = 1; cur_ld <= 255; ++cur_ld) {
		unsigned long lck = fck / cur_ld;

		for (cur_pd = pcd_min; cur_pd <= 255; ++cur_pd) {
			unsigned long pck = lck / cur_pd;
			long old_delta = abs(best_pck - req_pck);
			long new_delta = abs(pck - req_pck);

			if (best_pck == 0 || new_delta < old_delta) {
				best_pck = pck;
				best_ld = cur_ld;
				best_pd = cur_pd;

				if (pck == req_pck)
					goto found;
			}

			if (pck < req_pck)
				break;
		}

		if (lck / pcd_min < req_pck)
			break;
	}

found:
	cinfo->lck_div = best_ld;
	cinfo->pck_div = best_pd;
	cinfo->lck = fck / cinfo->lck_div;
	cinfo->pck = cinfo->lck / cinfo->pck_div;
}

/* calculate clock rates using dividers in cinfo */
int dispc_calc_clock_rates(unsigned long dispc_fclk_rate,
		struct dispc_clock_info *cinfo)
{
	if (cinfo->lck_div > 255 || cinfo->lck_div == 0)
		return -EINVAL;
	if (cinfo->pck_div < 2 || cinfo->pck_div > 255)
		return -EINVAL;

	cinfo->lck = dispc_fclk_rate / cinfo->lck_div;
	cinfo->pck = cinfo->lck / cinfo->pck_div;

	return 0;
}

int dispc_set_clock_div(enum omap_channel channel,
		struct dispc_clock_info *cinfo)
{
	printf("lck = %lu (%u)\n", cinfo->lck, cinfo->lck_div);
	printf("pck = %lu (%u)\n", cinfo->pck, cinfo->pck_div);

	BUG_ON(cinfo->lck_div < 1);
	BUG_ON(cinfo->pck_div < 2);
	 
	dispc_write_reg(DISPC_DIVISOR2,	FLD_VAL(cinfo->lck_div, 23, 16) | FLD_VAL(cinfo->pck_div, 7, 0));

	return 0;
}

int configure_dispc(void)
{
// == config overlay
	struct overlay_cache_data *c = &cosmo_overlay;
	u32 paddr;
	int r;
	u32 val;

	paddr = 0x87000000;

	dispc_write_reg(DISPC_GFX_ATTRIBUTES, 0x42000011); //	dispc_set_channel_out(plane, channel);
	
	dispc_write_reg(DISPC_GFX_BA0,	paddr );
	dispc_write_reg(DISPC_GFX_BA1, paddr );
	dispc_write_reg(DISPC_GFX_ROW_INC, 1);
	dispc_write_reg(DISPC_GFX_PIXEL_INC, 1);
	dispc_write_reg(DISPC_GFX_POSITION, FLD_VAL(c->pos_y, 26, 16) | FLD_VAL(c->pos_x, 10, 0));
	dispc_write_reg(DISPC_GFX_SIZE, FLD_VAL(c->height - 1, 26, 16) | FLD_VAL(c->width - 1, 10, 0));
	val =dispc_read_reg(DISPC_GLOBAL_ALPHA);
	val = FLD_MOD(val,c->global_alpha, 7, 0);	
	dispc_write_reg(DISPC_GLOBAL_ALPHA,val);
	REG_FLD_MOD(DISPC_GLOBAL_ALPHA, c->global_alpha, 7, 0);	
	
    dispc_write_reg(DISPC_GFX_FIFO_THRESHOLD, 0x03fc03bc);//dispc_setup_plane_fifo(plane, c->fifo_low, c->fifo_high);
    
// == manager config
	val = dispc_read_reg(DISPC_CONFIG2);
	val = FLD_MOD(val,OMAP_DSS_COLOR_KEY_GFX_DST, 11, 11); // dispc_set_trans_key
	val = FLD_MOD(val,0, 10, 10); //dispc_enable_trans_key	

	dispc_write_reg(DISPC_CONFIG2, val);	
	dispc_write_reg(DISPC_TRANS_COLOR2, 0);

	return r;
}

