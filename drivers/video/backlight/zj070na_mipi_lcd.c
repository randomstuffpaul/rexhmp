/* linux/drivers/video/backlight/zj070na_mipi_lcd.c
 *
 *
 * Copyright (c) 2011 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/ctype.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/workqueue.h>
#include <linux/backlight.h>
#include <linux/lcd.h>

#include <video/mipi_display.h>

#include <plat/gpio-cfg.h>
#include <plat/regs-mipidsim.h>

#include <plat/dsim.h>
#include <plat/mipi_dsi.h>



void zj070na_mipi_lcd_off(struct mipi_dsim_device *dsim)
{
}

static int zj070na_mipi_lcd_suspend(struct mipi_dsim_device *dsim)
{
	return 1;
}

static int zj070na_mipi_lcd_displayon(struct mipi_dsim_device *dsim)
{
	return 1;
}

static int zj070na_mipi_lcd_resume(struct mipi_dsim_device *dsim)
{
	return 1;
}

struct mipi_dsim_lcd_driver zj070na_mipi_lcd_driver = {
	.suspend =  zj070na_mipi_lcd_suspend,
	.displayon = zj070na_mipi_lcd_displayon,
	.resume = zj070na_mipi_lcd_resume,
};
