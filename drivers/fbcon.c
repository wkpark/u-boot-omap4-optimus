/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * LGE_CHANGE:
 *  - ported from lk
 *  - support only 32bit bpp
 */

#include <common.h>
#ifdef CONFIG_FBCON

#include "fbcon.h"
#ifdef USE_FONT5x12
#include "font5x12.h"
#else
#include "font10x18.h"
#endif

struct pos {
	int x;
	int y;
};

static struct fbcon_config *config = NULL;

#define RGB8888_BLACK	0xff000000
#define RGB8888_WHITE	0xffffffff

#ifdef USE_FONT5x12
#define FONT_WIDTH	5
#define FONT_HEIGHT	12
#else
#define FONT_WIDTH	10
#define FONT_HEIGHT	18

unsigned char *font_data = NULL;
#endif

#ifndef USE_FONT5x12
void init_font(void)
{
	unsigned short size;
	unsigned short value;
	int length = MYFONT_WIDTH * MYFONT_HEIGHT;
	unsigned char bit;
	unsigned char* src_ptr = myfont;
	unsigned char* dest_ptr = font_data;

	if (font_data)
		return;

	font_data = malloc(length * sizeof(char));
	dest_ptr = font_data;
	while (length > 0) {
		size = *src_ptr;
		src_ptr++;
		value = *src_ptr;
		src_ptr++;
		while(size--) {
			bit = value & 0xFF;
			*dest_ptr = bit;
			dest_ptr++;
			length--;
		}
	}
}
#endif

static uint32_t			BGCOLOR;
static uint32_t			FGCOLOR;

static struct pos		cur_pos;
static struct pos		max_pos;

#ifdef USE_FONT5x12
static void fbcon_drawglyph(uint32_t *pixels, uint32_t paint, unsigned stride,
			    unsigned *glyph)
{
	unsigned x, y, data;
	stride -= FONT_WIDTH;

	data = glyph[0];
	for (y = 0; y < (FONT_HEIGHT / 2); ++y) {
		for (x = 0; x < FONT_WIDTH; ++x) {
			if (data & 1)
				*pixels = paint;
			data >>= 1;
			pixels++;
		}
		pixels += stride;
	}

	data = glyph[1];
	for (y = 0; y < (FONT_HEIGHT / 2); y++) {
		for (x = 0; x < FONT_WIDTH; x++) {
			if (data & 1)
				*pixels = paint;
			data >>= 1;
			pixels++;
		}
		pixels += stride;
	}
}
#else
static void fbcon_drawglyph(uint32_t paint, char c)
{
	uint32_t *framebuffer = config->base;
	uint32_t i, j, pixel;
	int num_chars = 96;
	int off = c - 32;
        uint32_t x = cur_pos.x * (FONT_WIDTH + 1);
        uint32_t y = cur_pos.y * FONT_HEIGHT;
	uint32_t r, g, b;

	paint = 0x33ccff;

	r = paint & 0xff0000;
	g = paint & 0x00ff00;
	b = paint & 0x0000ff;

	for (i = 0; i < FONT_HEIGHT; i++) {
		for (j = 0; j < FONT_WIDTH; j++) {
			/* Get the pixel in font */
			uint8_t bit = font_data[(i * num_chars * FONT_WIDTH) + (off * FONT_WIDTH) + j];

			/* Get the pixel in the frame */
			pixel = ((y + i) * config->width + (x + j));
#if 0
			framebuffer[pixel  ] = (uint8_t)(((b->R * (255 - p)) + (c->R * p)) / 255);
			framebuffer[pixel+1] = (uint8_t)(((b->G * (255 - p)) + (c->G * p)) / 255);
			framebuffer[pixel+2] = (uint8_t)(((b->B * (255 - p)) + (c->B * p)) / 255);
#else
			framebuffer[pixel] = (0xFF << 24) | (bit << 16) & r | (bit << 8) & g | bit & b;
#endif
		}
	}
}
#endif

static void fbcon_flush(void)
{
	if (config->update_start)
		config->update_start();
	if (config->update_done)
		while (!config->update_done());
}

/* TODO: Take stride into account */
static void fbcon_scroll_up(void)
{
	unsigned short *dst = config->base;
	unsigned short *src = dst + (config->width * FONT_HEIGHT);
	unsigned count = config->width * (config->height - FONT_HEIGHT);

	while(count--) {
		*dst++ = *src++;
	}

	count = config->width * FONT_HEIGHT;
	while(count--) {
		*dst++ = BGCOLOR;
	}

	fbcon_flush();
}

/* TODO: take stride into account */
void fbcon_clear(void)
{
	unsigned count = config->width * config->height;
	memset(config->base, BGCOLOR, count * ((config->bpp) / 8));
}


static void fbcon_set_colors(unsigned bg, unsigned fg)
{
	BGCOLOR = bg;
	FGCOLOR = fg;
}

void fbcon_puts(const char *str)
{
	while (*str != 0) {
		fbcon_putc(*str++);
	}
}

void fbcon_putc(char c)
{
	uint32_t *pixels;

	/* ignore anything that happens before fbcon is initialized */
	if (!config)
		return;

	if((unsigned char)c > 127)
		return;
	if((unsigned char)c < 32) {
		if(c == '\n')
			goto newline;
		else if (c == '\r')
			cur_pos.x = 0;
		return;
	}

#ifdef USE_FONT5x12
	pixels = config->base;
	pixels += cur_pos.y * FONT_HEIGHT * config->width;
	pixels += cur_pos.x * (FONT_WIDTH + 1);
	fbcon_drawglyph(pixels, FGCOLOR, config->stride,
			font5x12 + (c - 32) * 2);
#else
	fbcon_drawglyph(FGCOLOR, c);
#endif

	cur_pos.x++;
	if (cur_pos.x < max_pos.x)
		return;

newline:
	cur_pos.y++;
	cur_pos.x = 0;
	if(cur_pos.y >= max_pos.y) {
		cur_pos.y = max_pos.y - 1;
		fbcon_scroll_up();
	} else
		fbcon_flush();
}

void fbcon_setup(struct fbcon_config *_config)
{
	uint32_t bg;
	uint32_t fg;

	if (NULL == _config) { /* WBT fixed TD#202854 */
		printf("%s: fbcon_config is NULL!\n", __func__);
		return; /* WBT fixed TD#202854 */
	}

	config = _config;

	switch (config->format) {
        case FB_FORMAT_RGB8888:
                fg = RGB8888_WHITE;
                bg = RGB8888_BLACK;
                break;
	default:
		printf("%s: unknown framebuffer pixel format\n", __func__);
		return;
	}

	fbcon_set_colors(bg, fg);

	cur_pos.x = 0;
	cur_pos.y = 0;
	max_pos.x = config->width / (FONT_WIDTH+1);
	max_pos.y = (config->height - 1) / FONT_HEIGHT;

#ifndef USE_FONT5x12
	init_font();
#endif
	//fbcon_clear();
}

struct fbcon_config* fbcon_display(void)
{
    return config;
}

#endif
