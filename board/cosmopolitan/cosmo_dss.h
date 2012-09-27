/* u-boot/board/hub/hub_dss.h 
  *
  * Author: KwangJo Hwang <klaus.hwang@lge.com>
  *
  * This software is licensed under the terms of the GNU General Public
  * License version 2, as published by the Free Software Foundation, and
  * may be copied, distributed, and modified under those terms.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  */

#ifndef __OMAP2_DSS_H
#define __OMAP2_DSS_H

typedef _Bool			bool;
extern void sr32(u32 addr, u32 start_bit, u32 num_bits, u32 value);

enum omap_plane {
	OMAP_DSS_GFX	= 0,
	OMAP_DSS_VIDEO1	= 1,
	OMAP_DSS_VIDEO2	= 2,
	OMAP_DSS_VIDEO3 = 3		/* OMAP4 */
};

enum omap_channel {
	OMAP_DSS_CHANNEL_LCD	= 0,
	OMAP_DSS_CHANNEL_DIGIT	= 1,
	OMAP_DSS_CHANNEL_LCD2	= 2,	/* OMAP4 */
};

enum omap_color_mode {
	OMAP_DSS_COLOR_CLUT1	= 1 << 0,  /* BITMAP 1 */
	OMAP_DSS_COLOR_CLUT2	= 1 << 1,  /* BITMAP 2 */
	OMAP_DSS_COLOR_CLUT4	= 1 << 2,  /* BITMAP 4 */
	OMAP_DSS_COLOR_CLUT8	= 1 << 3,  /* BITMAP 8 */
	OMAP_DSS_COLOR_RGB12U	= 1 << 4,  /* RGB12x, 16-bit container */
	OMAP_DSS_COLOR_ARGB16	= 1 << 5,  /* ARGB16 */
	OMAP_DSS_COLOR_RGB16	= 1 << 6,  /* RGB16 */
	OMAP_DSS_COLOR_RGB24U	= 1 << 7,  /* RGB24, 32-bit container */
	OMAP_DSS_COLOR_RGB24P	= 1 << 8,  /* RGB24, 24-bit container */
	OMAP_DSS_COLOR_YUV2	= 1 << 9,  /* YUV2 4:2:2 co-sited */
	OMAP_DSS_COLOR_UYVY	= 1 << 10, /* UYVY 4:2:2 co-sited */
	OMAP_DSS_COLOR_ARGB32	= 1 << 11, /* ARGB32 */
	OMAP_DSS_COLOR_RGBA32	= 1 << 12, /* RGBA32 */
	OMAP_DSS_COLOR_RGBX32	= 1 << 13, /* RGBx32 */
	/* OMAP4 */
	OMAP_DSS_COLOR_NV12     = 1 << 14, /* NV12 format: YUV 4:2:0 */
	OMAP_DSS_COLOR_RGBA12	= 1 << 15, /* RGBA12 - 4444 */
	OMAP_DSS_COLOR_XRGB12	= 1 << 16, /* xRGB12, 16-bit container */
	OMAP_DSS_COLOR_ARGB16_1555	= 1 << 17, /* ARGB16-1555 */
	OMAP_DSS_COLOR_RGBX24_32_ALGN	= 1 << 18, /* 32-msb aligned 24bit */
	OMAP_DSS_COLOR_XRGB15	= 1 << 19, /* xRGB15: 1555*/


	OMAP_DSS_COLOR_GFX_OMAP2 =
		OMAP_DSS_COLOR_CLUT1 | OMAP_DSS_COLOR_CLUT2 |
		OMAP_DSS_COLOR_CLUT4 | OMAP_DSS_COLOR_CLUT8 |
		OMAP_DSS_COLOR_RGB12U | OMAP_DSS_COLOR_RGB16 |
		OMAP_DSS_COLOR_RGB24U | OMAP_DSS_COLOR_RGB24P,

	OMAP_DSS_COLOR_VID_OMAP2 =
		OMAP_DSS_COLOR_RGB16 | OMAP_DSS_COLOR_RGB24U |
		OMAP_DSS_COLOR_RGB24P | OMAP_DSS_COLOR_YUV2 |
		OMAP_DSS_COLOR_UYVY,

	OMAP_DSS_COLOR_GFX_OMAP3 =
		OMAP_DSS_COLOR_CLUT1 | OMAP_DSS_COLOR_CLUT2 |
		OMAP_DSS_COLOR_CLUT4 | OMAP_DSS_COLOR_CLUT8 |
		OMAP_DSS_COLOR_RGB12U | OMAP_DSS_COLOR_ARGB16 |
		OMAP_DSS_COLOR_RGB16 | OMAP_DSS_COLOR_RGB24U |
		OMAP_DSS_COLOR_RGB24P | OMAP_DSS_COLOR_ARGB32 |
		OMAP_DSS_COLOR_RGBA32 | OMAP_DSS_COLOR_RGBX32,

	OMAP_DSS_COLOR_VID1_OMAP3 =
#ifdef CONFIG_ARCH_OMAP4
		OMAP_DSS_COLOR_NV12 | OMAP_DSS_COLOR_RGBA12 |
		OMAP_DSS_COLOR_XRGB12 | OMAP_DSS_COLOR_ARGB16_1555 |
		OMAP_DSS_COLOR_RGBX24_32_ALGN | OMAP_DSS_COLOR_XRGB15 |
#endif
		OMAP_DSS_COLOR_RGB12U | OMAP_DSS_COLOR_ARGB16 |
		OMAP_DSS_COLOR_RGB16 | OMAP_DSS_COLOR_RGB24U |
		OMAP_DSS_COLOR_RGB24P | OMAP_DSS_COLOR_YUV2 |
		OMAP_DSS_COLOR_UYVY | OMAP_DSS_COLOR_ARGB32 |
		OMAP_DSS_COLOR_RGBA32 | OMAP_DSS_COLOR_RGBX32,

	OMAP_DSS_COLOR_VID2_OMAP3 = OMAP_DSS_COLOR_VID1_OMAP3,
	OMAP_DSS_COLOR_VID3_OMAP3 = OMAP_DSS_COLOR_VID2_OMAP3,
};

enum omap_dss_load_mode {
	OMAP_DSS_LOAD_CLUT_AND_FRAME	= 0,
	OMAP_DSS_LOAD_CLUT_ONLY		= 1,
	OMAP_DSS_LOAD_FRAME_ONLY	= 2,
	OMAP_DSS_LOAD_CLUT_ONCE_FRAME	= 3,
};

enum omap_dss_trans_key_type {
	OMAP_DSS_COLOR_KEY_GFX_DST = 0,
	OMAP_DSS_COLOR_KEY_VID_SRC = 1,
};


enum omap_panel_config {
	OMAP_DSS_LCD_IVS		= 1<<0,
	OMAP_DSS_LCD_IHS		= 1<<1,
	OMAP_DSS_LCD_IPC		= 1<<2,
	OMAP_DSS_LCD_IEO		= 1<<3,
	OMAP_DSS_LCD_RF			= 1<<4,
	OMAP_DSS_LCD_ONOFF		= 1<<5,

	OMAP_DSS_LCD_TFT		= 1<<20,
};

enum omap_display_caps {
	OMAP_DSS_DISPLAY_CAP_MANUAL_UPDATE	= 1 << 0,
	OMAP_DSS_DISPLAY_CAP_TEAR_ELIM		= 1 << 1,
};

enum omap_dss_update_mode {
	OMAP_DSS_UPDATE_DISABLED = 0,
	OMAP_DSS_UPDATE_AUTO,
	OMAP_DSS_UPDATE_MANUAL,
};

enum omap_dss_display_state {
	OMAP_DSS_DISPLAY_DISABLED = 0,
	OMAP_DSS_DISPLAY_ACTIVE,
	OMAP_DSS_DISPLAY_SUSPENDED,
};


enum omap_dss_rotation_type {
	OMAP_DSS_ROT_DMA = 0,
	OMAP_DSS_ROT_VRFB = 1,
	OMAP_DSS_ROT_TILER = 2,	/* OMAP4 */
};

/* clockwise rotation angle */
enum omap_dss_rotation_angle {
	OMAP_DSS_ROT_0   = 0,
	OMAP_DSS_ROT_90  = 1,
	OMAP_DSS_ROT_180 = 2,
	OMAP_DSS_ROT_270 = 3,
};


/* DSI */
enum dsi {
			dsi1 = 0,
			dsi2 = 1,
			};

//dss.h
/* OMAP TRM gives bitfields as start:end, where start is the higher bit
   number. For example 7:0 */
#define FLD_MASK(start, end)	(((1 << ((start) - (end) + 1)) - 1) << (end))
#define FLD_VAL(val, start, end) (((val) << (end)) & FLD_MASK(start, end))
#define FLD_GET(val, start, end) (((val) & FLD_MASK(start, end)) >> (end))
#define FLD_MOD(orig, val, start, end) \
	(((orig) & ~FLD_MASK(start, end)) | FLD_VAL(val, start, end))

#define DISPC_MAX_FCK 173000000

enum omap_burst_size {
	OMAP_DSS_BURST_4x32	= 0, /*OMAP_DSS_BURST_2x128 in OMAP4*/
	OMAP_DSS_BURST_8x32	= 1, /*OMAP_DSS_BURST_4x128 in OMAP4*/
	OMAP_DSS_BURST_16x32	= 2, /*OMAP_DSS_BURST_8x128 in OMAP4*/
};

enum dss_clock {
	DSS_CLK_ICK	= 1 << 0,
	DSS_CLK_FCK1	= 1 << 1,
	DSS_CLK_FCK2	= 1 << 2,
	DSS_CLK_54M	= 1 << 3,
	DSS_CLK_96M	= 1 << 4,
};

struct dss_clock_info {
	/* rates that we get with dividers below */
	unsigned long fck;

	/* dividers */
	u16 fck_div;
};

struct dispc_clock_info {
	/* rates that we get with dividers below */
	unsigned long lck;
	unsigned long pck;

	/* dividers */
	u16 lck_div;
	u16 pck_div;
};

struct dsi_clock_info {
	/* rates that we get with dividers below */
	unsigned long fint;
	unsigned long clkin4ddr;
	unsigned long clkin;
	unsigned long dsi1_pll_fclk;
	unsigned long dsi2_pll_fclk;

	unsigned long lp_clk;

	/* dividers */
	u16 regn;
	u16 regm;
	u16 regm3;
	u16 regm4;

	u16 lp_clk_div;

	u8 highfreq;
	bool use_dss2_fck;
	bool use_dss2_sys_clk;
};

struct omap_video_timings {
	/* Unit: pixels */
	u16 x_res;
	/* Unit: pixels */
	u16 y_res;
	/* Unit: KHz */
	u32 pixel_clock;
	/* Unit: pixel clocks */
	u16 hsw;	/* Horizontal synchronization pulse width */
	/* Unit: pixel clocks */
	u16 hfp;	/* Horizontal front porch */
	/* Unit: pixel clocks */
	u16 hbp;	/* Horizontal back porch */
	/* Unit: line clocks */
	u16 vsw;	/* Vertical synchronization pulse width */
	/* Unit: line clocks */
	u16 vfp;	/* Vertical front porch */
	/* Unit: line clocks */
	u16 vbp;	/* Vertical back porch */
};

struct overlay_cache_data {
	/* If TRUE, cache changed, but not written to shadow registers. Set
	 * in apply(), cleared when registers written. */
	bool dirty;
	/* If TRUE, shadow registers contain changed values not yet in real
	 * registers. Set when writing to shadow registers, cleared at
	 * VSYNC/EVSYNC */
	bool shadow_dirty;

	bool enabled;

	u32 paddr;
	void volatile *vaddr;
	u16 screen_width;
	u16 width;
	u16 height;
	enum omap_color_mode color_mode;
	u8 rotation;
	enum omap_dss_rotation_type rotation_type;
	bool mirror;

	u16 pos_x;
	u16 pos_y;
	u16 out_width;	/* if 0, out_width == width */
	u16 out_height;	/* if 0, out_height == height */
	u8 global_alpha;
	u8 pre_alpha_mult;

	enum omap_channel channel;
	bool replication;
	bool ilace;

	enum omap_burst_size burst_size;
	u32 fifo_low;
	u32 fifo_high;

	bool manual_update;

	u32 p_uv_addr; /* relevant for NV12 format only */
};

static struct overlay_cache_data cosmo_overlay = {
	.screen_width = 480,
	.width = 480,
	.height = 800,
	.color_mode = OMAP_DSS_COLOR_ARGB32,	
	.ilace = 0,
	.rotation = 0,
	.rotation_type = OMAP_DSS_ROT_DMA,
	.mirror = 0,
	.pos_x = 0,
	.pos_y = 0,
	.out_width = 480,
	.out_height = 800,
	.global_alpha = 255,
	.channel = OMAP_DSS_GFX,
	.replication = 0,
	.burst_size = OMAP_DSS_BURST_16x32,
	.fifo_low = 0,
	.fifo_high = 960,
	.manual_update = 0,
};

void dispc_pck_free_enable(bool enable);
void dispc_set_lcd_size(enum omap_channel channel, u16 width, u16 height);
void dispc_go(enum omap_channel channel);
void dispc_enable_lcd_out(bool enable);
void dispc_set_control2_reg();


void dispc_set_lcd_timings(enum omap_channel channel, struct omap_video_timings *timings);
unsigned long dispc_fclk_rate(void);
unsigned long dispc_lclk_rate(enum omap_channel channel);
unsigned long dispc_pclk_rate(enum omap_channel channel);
void dispc_set_pol_freq(enum omap_panel_config config, u8 acbi, u8 acb);
void dispc_find_clk_divs(bool is_tft, unsigned long req_pck, unsigned long fck,
		struct dispc_clock_info *cinfo);
int dispc_calc_clock_rates(unsigned long dispc_fclk_rate,
		struct dispc_clock_info *cinfo);
int dispc_set_clock_div(enum omap_channel channel,
		struct dispc_clock_info *cinfo);

int configure_dispc(void);


#define BUG() do	{ \
	 \
	while (1);	/* dead-loop */ \
} while (0)

#define BUG_ON(condition) do { if (condition) ; } while(0)

#define mdelay(n) ({ unsigned long msec = (n); while (msec--) udelay(1000); })
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define do_div(n,base) ({ \
	int __res; \
	__res = ((unsigned long) n) % (unsigned) base; \
	n = ((unsigned long) n) / (unsigned) base; \
	__res; \
})

#ifndef	TRUE
#define TRUE            1
#endif
#ifndef FALSE
#define FALSE           0
#endif

#define abs(x) ({				\
		int __x = (x);			\
		(__x < 0) ? -__x : __x;		\
	})

struct dss_reg {
	u16 idx;
};

#define DSS_REG(idx)			((const struct dss_reg) { idx })

struct dispc_reg { u16 idx; };

#define DISPC_REG(idx)			((const struct dispc_reg) { idx })



#define DSS_REVISION			DSS_REG(0x0000)
#define DSS_SYSCONFIG			DSS_REG(0x0010)
#define DSS_SYSSTATUS			DSS_REG(0x0014)
#define DSS_CONTROL				DSS_REG(0x0040)
#define DSS_SDI_CONTROL			DSS_REG(0x0044)
#define DSS_PLL_CONTROL			DSS_REG(0x0048)
#define DSS_SDI_STATUS			DSS_REG(0x005C)

#ifdef CONFIG_ARCH_OMAP4
#define DSS_STATUS				DSS_REG(0x005C)
#endif


#define DISPC_SZ_REGS			SZ_1K

/* DISPC common */
#define DISPC_REVISION			DISPC_REG(0x0000)
#define DISPC_SYSCONFIG			DISPC_REG(0x0010)
#define DISPC_SYSSTATUS			DISPC_REG(0x0014)
#define DISPC_IRQSTATUS			DISPC_REG(0x0018)
#define DISPC_IRQENABLE			DISPC_REG(0x001C)
#define DISPC_CONTROL			DISPC_REG(0x0040)
	/* DISPC_CONTROL1 in OMAP4 */
#define DISPC_CONFIG			DISPC_REG(0x0044)
	/* DISPC_CONFIG1 in OMAP4 */
#define DISPC_CAPABLE			DISPC_REG(0x0048)
#define DISPC_DEFAULT_COLOR0		DISPC_REG(0x004C)
#define DISPC_DEFAULT_COLOR1		DISPC_REG(0x0050)
#define DISPC_TRANS_COLOR0		DISPC_REG(0x0054)
#define DISPC_TRANS_COLOR1		DISPC_REG(0x0058)
#define DISPC_LINE_STATUS		DISPC_REG(0x005C)
#define DISPC_LINE_NUMBER		DISPC_REG(0x0060)
#define DISPC_TIMING_H			DISPC_REG(0x0064)
	/* DISPC_TIMING_H1 in OMAP4 */
#define DISPC_TIMING_V			DISPC_REG(0x0068)
	/* DISPC_TIMING_V1 in OMAP4 */
#define DISPC_POL_FREQ			DISPC_REG(0x006C)
	/* DISPC_POL_FREQ1 in OMAP4 */

#ifndef CONFIG_ARCH_OMAP4
#define DISPC_DIVISOR			DISPC_REG(0x0070)
#else
#define DISPC_DIVISOR			DISPC_REG(0x0804)
#define DISPC_DIVISOR1			DISPC_REG(0x0070)
#endif

#define DISPC_GLOBAL_ALPHA		DISPC_REG(0x0074)
#define DISPC_SIZE_DIG			DISPC_REG(0x0078)
	/* DISPC_SIZE_TV in OMAP4 */
#define DISPC_SIZE_LCD			DISPC_REG(0x007C)
	/* DISPC_SIZE_LCD1 in OMAP4 */

#ifdef CONFIG_ARCH_OMAP4
#define DISPC_GLOBAL_BUFFER		DISPC_REG(0x0800)
#endif

/* DISPC GFX plane */
#define DISPC_GFX_BA0			DISPC_REG(0x0080)
#define DISPC_GFX_BA1			DISPC_REG(0x0084)
#define DISPC_GFX_POSITION		DISPC_REG(0x0088)
#define DISPC_GFX_SIZE			DISPC_REG(0x008C)
#define DISPC_GFX_ATTRIBUTES		DISPC_REG(0x00A0)
#define DISPC_GFX_FIFO_THRESHOLD	DISPC_REG(0x00A4)
	/* DISPC_GFX_BUF_THRESHOLD in OMAP4 */
#define DISPC_GFX_FIFO_SIZE_STATUS	DISPC_REG(0x00A8)
	/* DISPC_GFX_BUF_SIZE_STATUS in OMAP4 */
#define DISPC_GFX_ROW_INC		DISPC_REG(0x00AC)
#define DISPC_GFX_PIXEL_INC		DISPC_REG(0x00B0)
#define DISPC_GFX_WINDOW_SKIP	DISPC_REG(0x00B4)
#define DISPC_GFX_TABLE_BA		DISPC_REG(0x00B8)

#define DISPC_DATA_CYCLE1		DISPC_REG(0x01D4)
	/* DISPC_DATA1_CYCLE1 in OMAP4 */
#define DISPC_DATA_CYCLE2		DISPC_REG(0x01D8)
	/* DISPC_DATA1_CYCLE2 in OMAP4 */
#define DISPC_DATA_CYCLE3		DISPC_REG(0x01DC)
	/* DISPC_DATA1_CYCLE3 in OMAP4 */

#define DISPC_CPR_COEF_R		DISPC_REG(0x0220)
	/* DISPC_CPR1_COEFF_R in OMAP4 */
#define DISPC_CPR_COEF_G		DISPC_REG(0x0224)
	/* DISPC_CPR1_COEFF_G in OMAP4 */
#define DISPC_CPR_COEF_B		DISPC_REG(0x0228)
	/* DISPC_CPR1_COEFF_B in OMAP4 */

#define DISPC_GFX_PRELOAD		DISPC_REG(0x022C)


/* coef index i = {0, 1, 2, 3, 4, 5, 6, 7} */
#define DISPC_VID_FIR_COEF_H(n, i)	DISPC_REG(0x00F0 + (n)*0x90 + (i)*0x8)
/* coef index i = {0, 1, 2, 3, 4, 5, 6, 7} */
#define DISPC_VID_FIR_COEF_HV(n, i)	DISPC_REG(0x00F4 + (n)*0x90 + (i)*0x8)
/* coef index i = {0, 1, 2, 3, 4} */
#define DISPC_VID_CONV_COEF(n, i)	DISPC_REG(0x0130 + (n)*0x90 + (i)*0x4)
/* coef index i = {0, 1, 2, 3, 4, 5, 6, 7} */
#define DISPC_VID_FIR_COEF_V(n, i)	DISPC_REG(0x01E0 + (n)*0x20 + (i)*0x4)

#define DISPC_VID_PRELOAD(n)		DISPC_REG(0x230 + (n)*0x04)

#define DISPC_CONTROL2			DISPC_REG(0x0238)
#define DISPC_CONFIG2			DISPC_REG(0x0620)

#ifdef CONFIG_ARCH_OMAP4
/********** OMAP4 new global registers  **/
#define DISPC_DEFAULT_COLOR2		DISPC_REG(0x03AC)
#define DISPC_TRANS_COLOR2			DISPC_REG(0x03B0)
#define DISPC_CPR2_COEF_B			DISPC_REG(0x03B4)
#define DISPC_CPR2_COEF_G			DISPC_REG(0x03B8)
#define DISPC_CPR2_COEF_R			DISPC_REG(0x03BC)
#define DISPC_DATA2_CYCLE1			DISPC_REG(0x03C0)
#define DISPC_DATA2_CYCLE2			DISPC_REG(0x03C4)
#define DISPC_DATA2_CYCLE3			DISPC_REG(0x03C8)
#define DISPC_SIZE_LCD2				DISPC_REG(0x03CC)
#define DISPC_TIMING_H2				DISPC_REG(0x0400)
#define DISPC_TIMING_V2				DISPC_REG(0x0404)
#define DISPC_POL_FREQ2				DISPC_REG(0x0408)
#define DISPC_DIVISOR2				DISPC_REG(0x040C)
#endif

/* DSI Virtual channel. Hardcoded for now. */
#define TCH 0

#define DCS_READ_NUM_ERRORS		0x05
#define DCS_READ_POWER_MODE		0x0a
#define DCS_READ_MADCTL			0x0b
#define DCS_READ_PIXEL_FORMAT	0x0c
#define DCS_RDDSDR				0x0f
#define DCS_SLEEP_IN			0x10
#define DCS_SLEEP_OUT			0x11
#define DCS_DISPLAY_OFF			0x28
#define DCS_DISPLAY_ON			0x29
#define DCS_COLUMN_ADDR			0x2a
#define DCS_PAGE_ADDR			0x2b
#define DCS_MEMORY_WRITE		0x2c
#define DCS_TEAR_OFF			0x34
#define DCS_TEAR_ON				0x35
#define DCS_MEM_ACC_CTRL		0x36
#define DCS_PIXEL_FORMAT		0x3a
#define DCS_BRIGHTNESS			0x51
#define DCS_CTRL_DISPLAY		0x53
#define DCS_WRITE_CABC			0x55
#define DCS_READ_CABC			0x56
#define DCS_GET_ID				0xf8

#define END_OF_COMMAND	0xFF

void cosmo_panel_logo_draw();
//#ifdef CONFIG_COSMO_SU760
void cosmo_panel_emergency_logo_draw();
//#endif
	// LGE CahngeS Darren.Kang@lge.com	2011.1.11 Kernel panic dump logic [ST]
void cosmo_panel_ap_trap_draw();
void cosmo_panel_cp_trap_draw();
	// LGE CahngeS Darren.Kang@lge.com	2011.1.11 Kernel panic dump logic [END]
#endif

