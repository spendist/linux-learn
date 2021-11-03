// SPDX-License-Identifier: GPL-2.0+
/*
 * FB driver for the SSD1322 OLED Controller
 */


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>

#include "fbtft.h"

#define DRVNAME     "fb_ssd1322"
#define WIDTH       256
#define HEIGHT      64
#define GAMMA_NUM   1
#define GAMMA_LEN   15
#define DEFAULT_GAMMA "7 1 1 1 1 2 2 3 3 4 4 5 5 6 6"

static int init_display(struct fbtft_par *par)
{
	par->fbtftops.reset(par);

	if (par->gpio.cs)
		gpiod_set_value(par->gpio.cs, 0);

	write_reg(par, 0xFD, 0x12); 		/*SET COMMAND LOCK -> UNLOCK */
	write_reg(par, 0xAE);				/*DISPLAY OFF*/
	write_reg(par, 0xB3, 0x91);			/*DISPLAYDIVIDE CLOCKRATIO/OSCILLATOR FREQUENCY*/ 
	write_reg(par, 0xCA, 0x3F);			/*multiplex ratio duty = 1/64*/
	write_reg(par, 0xA2, 0x00);			/*set offset*/
	write_reg(par, 0xA1, 0x00);			/*start line*/
	switch (par->info->var.rotate){
		case 0:
			write_reg(par, 0xA0, 0x14, 0x11);	/*set remap*/
			break;
		case 180:
			write_reg(par, 0xA0, 0x06, 0x11);	/*set remap*/
			break;
		default:
			write_reg(par, 0xA0, 0x14, 0x11);
	}
	write_reg(par, 0xAB, 0x01);			/*funtion selection = Enable internal VDD regulator*/ 
	write_reg(par, 0xB4, 0xA0, 0xFD);	/*Display Enhancement A = Enable external VSL, Enhanced low GS display quality */
	write_reg(par, 0xC1, 0x80);			/*set contrast current */
	write_reg(par, 0xC7, 0x0F);			/*master contrast current control*/ 
	write_reg(par, 0xB1, 0xE2);			/*SET PHASE LENGTH*/
	write_reg(par, 0xD1, 0x82, 0x20);	/* Display Enhancement B = default reset */ 
	write_reg(par, 0xBB, 0x1F);			/*SET PRE-CHANGE VOLTAGE*/ 
	write_reg(par, 0xB6, 0x08);			/*SET SECOND PRE-CHARGE PERIOD*/
	write_reg(par, 0xBE, 0x07);			/* SET VCOMH */
	write_reg(par, 0xA6);				/*normal display*/
	write_reg(par, 0xAF);				/*display ON*/

	return 0;
}

static void set_addr_win(struct fbtft_par *par, int xs, int ys, int xe, int ye)
{
	int width = par->info->var.xres;
	int offset = (480 - width) / 8;

	fbtft_par_dbg(DEBUG_SET_ADDR_WIN, par, "%s(xs=%d, ys=%d, xe=%d, ye=%d)\n", __func__, xs, ys, xe, ye);

	write_reg(par, 0x15, offset, offset + (width / 4) - 1);
	write_reg(par, 0x75, ys, ye);
	write_reg(par, 0x5c);
}

/*
	Grayscale Lookup Table
	GS1 - GS15
	The "Gamma curve" contains the relative values between the entries in the Lookup table.

	0 = Setting of GS1 < Setting of GS2 < Setting of GS3..... < Setting of GS14 < Setting of GS15

*/
static int set_gamma(struct fbtft_par *par, u32 *curves)
{
	unsigned long tmp[GAMMA_LEN * GAMMA_NUM];
	int i, acc = 0;

	fbtft_par_dbg(DEBUG_INIT_DISPLAY, par, "%s()\n", __func__);

	for (i = 0; i < GAMMA_LEN; i++) {
		if (i > 0 && curves[i] < 1) {
			dev_err(par->info->device,
				"Illegal value in Grayscale Lookup Table at index %d. " \
				"Must be greater than 0\n", i);
			return -EINVAL;
		}
		acc += curves[i];
		tmp[i] = acc;
		if (acc > 180) {
			dev_err(par->info->device,
				"Illegal value(s) in Grayscale Lookup Table. " \
				"At index=%d, the accumulated value has exceeded 180\n", i);
			return -EINVAL;
		}
	}

	write_reg(par, 0xB8,
	tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7],
	tmp[8], tmp[9], tmp[10], tmp[11], tmp[12], tmp[13], tmp[14]);
	//write_reg(par, 0x00);
	return 0;
}

static int blank(struct fbtft_par *par, bool on)
{
	fbtft_par_dbg(DEBUG_BLANK, par, "%s(blank=%s)\n", __func__, on ? "true" : "false");
	if (on)
		write_reg(par, 0xAE);
	else
		write_reg(par, 0xAF);
	return 0;
}


#define CYR     613    /* 2.392 */
#define CYG     601    /* 2.348 */
#define CYB     233    /* 0.912 */

static unsigned int rgb565_to_y(unsigned int rgb)
{
	rgb = cpu_to_le16(rgb);
	return CYR * (rgb >> 11) + CYG * (rgb >> 5 & 0x3F) + CYB * (rgb & 0x1F);
}

static int write_vmem(struct fbtft_par *par, size_t offset, size_t len)
{
	u16 *vmem16 = (u16 *)(par->info->screen_base);
	u8 *buf = par->txbuf.buf;
	int y, x, bl_height, bl_width;
	int ret = 0;

	/* Set data line beforehand */
	gpiod_set_value(par->gpio.dc, 1);

	/* convert offset to word index from byte index */
	offset /= 2;
	bl_width = par->info->var.xres;
	bl_height = len / par->info->fix.line_length;

	for (y = 0; y < bl_height; y++) {
		for (x = 0; x < bl_width / 2; x++) {
			*buf = cpu_to_le16(rgb565_to_y(vmem16[offset++])) >> 8 & 0xF0;
			*buf++ |= cpu_to_le16(rgb565_to_y(vmem16[offset++])) >> 12;
		}
	}

	/* Write data */
	ret = par->fbtftops.write(par, par->txbuf.buf, bl_width/2*bl_height);
	if (ret < 0)
		dev_err(par->info->device, "%s: write failed and returned: %d\n", __func__, ret);

	return ret;
}

static struct fbtft_display display = {
	.regwidth = 8,
	.width = WIDTH,
	.height = HEIGHT,
	.txbuflen = WIDTH * HEIGHT/2,
	.gamma_num = GAMMA_NUM,
	.gamma_len = GAMMA_LEN,
	.gamma = DEFAULT_GAMMA,
	.fbtftops = {
		.write_vmem = write_vmem,
		.init_display = init_display,
		.set_addr_win  = set_addr_win,
		.blank = blank,
		.set_gamma = set_gamma,
	},
};

FBTFT_REGISTER_DRIVER(DRVNAME, "solomon,ssd1322", &display);

MODULE_ALIAS("spi:" DRVNAME);
MODULE_ALIAS("platform:" DRVNAME);
MODULE_ALIAS("spi:ssd1322");
MODULE_ALIAS("platform:ssd1322");

MODULE_DESCRIPTION("SSD1322 OLED Driver");
MODULE_AUTHOR("Marko Bursic");
MODULE_LICENSE("GPL");
