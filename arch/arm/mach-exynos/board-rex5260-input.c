/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/export.h>
#if defined(CONFIG_TOUCHSCREEN_FT5X06)
#include <linux/interrupt.h>
#include <linux/i2c/ft5x06_ts.h>
#endif
#if defined(CONFIG_TOUCHSCREEN_PIXCIR)
#include <linux/input/pixcir_ts.h>
#endif

#include <plat/gpio-cfg.h>
#include <plat/devs.h>
#include <plat/iic.h>

#include <mach/irqs.h>
#include <mach/hs-iic.h>
#include <mach/regs-gpio.h>

#include "board-rex5260.h"
#include <mach/sec_debug.h>

extern struct class *sec_class;


#define GPIO_TSP_INT		EXYNOS5260_GPX1(6)
#define GPIO_HOME_BUTTON	EXYNOS5260_GPX0(3)
#define GPIO_BACK_BUTTON	EXYNOS5260_GPX2(0)
#define GPIO_MENU_BUTTON	EXYNOS5260_GPX2(1)
#define GPIO_VOLUP_BUTTON	EXYNOS5260_GPX2(4)
#define GPIO_VOLDOWN_BUTTON	EXYNOS5260_GPX2(5)
#define GPIO_POWER_BUTTON	EXYNOS5260_GPX2(7)

static void rex5260_gpio_keys_config_setup(void)
{
	s3c_gpio_setpull(GPIO_HOME_BUTTON, S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(GPIO_BACK_BUTTON, S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(GPIO_MENU_BUTTON, S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(GPIO_VOLUP_BUTTON, S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(GPIO_VOLDOWN_BUTTON, S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(GPIO_POWER_BUTTON, S3C_GPIO_PULL_NONE);

}
static struct gpio_keys_button rex5260_button[] = {
	{
		.code = KEY_POWER,
		.gpio = GPIO_POWER_BUTTON,
		.desc = "gpio-keys: KEY_POWER",
		.active_low = 1,
		.wakeup = 1,
	}, {
		.code = KEY_HOMEPAGE,
		.gpio = GPIO_HOME_BUTTON,
		.desc = "gpio-keys: KEY_HOMEPAGE",
		.active_low = 1,
	}, {
		.code = KEY_BACK,
		.gpio = GPIO_BACK_BUTTON,
		.desc = "gpio-keys: KEY_BACK",
		.active_low = 1,
	}, {
		.code = KEY_MENU,
		.gpio = GPIO_MENU_BUTTON,
		.desc = "gpio-keys: KEY_MENU",
		.active_low = 1,
	}, {
		.code = KEY_VOLUMEDOWN,
		.gpio = GPIO_VOLDOWN_BUTTON,
		.desc = "gpio-keys: KEY_VOLUMEDOWN",
		.active_low = 1,
	}, {
		.code = KEY_VOLUMEUP,
		.gpio = GPIO_VOLUP_BUTTON,
		.desc = "gpio-keys: KEY_VOLUMEUP",
		.active_low = 1,
	},
};

#if defined(CONFIG_TOUCHSCREEN_FT5X06)
#define GPIO_TSP_RESET		EXYNOS5260_GPD0(7)
#define GPIO_LEVEL_LOW		0

static void exynos5260_touch_init(void)
{
	int gpio;

	printk ("%s\n",__func__);
	/* TOUCH_RESET */
	gpio = GPIO_TSP_RESET;
	if (gpio_request(gpio, "TSP_RESET")) {
		pr_err("%s : TSP_RESET request port error\n", __func__);
	} else {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
		gpio_direction_output(gpio, 0);
		mdelay(100);
		gpio_direction_output(gpio, 1);
		gpio_free(gpio);
	}
}

struct s3c2410_platform_i2c i2c_data_touch  __initdata = {
		.bus_num        = 8,
		.flags          = 0,
		.slave_addr     = 0x70,
		.frequency      = 400*1000,
		.sda_delay      = 100,
};

static struct i2c_board_info i2c_devs_touch[] __initdata = {
	{
		I2C_BOARD_INFO(FT5X0X_NAME, 0x70 >> 1),
		.irq		= IRQ_EINT(14),
	},
};

#endif

#if defined(CONFIG_TOUCHSCREEN_PIXCIR)
#define GPIO_TSP_RESET		EXYNOS5260_GPD0(7)
#define GPIO_LEVEL_LOW		0

static void exynos5260_touch_init(void)
{
	int gpio;

	printk ("%s\n",__func__);
	/* TOUCH_RESET */
	gpio = GPIO_TSP_RESET;
	if (gpio_request(gpio, "TSP_RESET")) {
		pr_err("%s : TSP_RESET request port error\n", __func__);
	} else {
		s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
		gpio_direction_output(gpio, 0);
		mdelay(100);
		gpio_direction_output(gpio, 1);
		gpio_free(gpio);
	}
}

struct s3c2410_platform_i2c i2c_data_touch  __initdata = {
		.bus_num        = 8,
		.flags          = 0,
		.slave_addr     = 0x5c,
		.frequency      = 400*1000,
		.sda_delay      = 100,
};

static int rex5260_ts_get_value(void)
{
            return gpio_get_value(EXYNOS5260_GPX1(6));
}

static struct pixcir_ts_platform_data rex5260_ts_data = {
	   .attb_read_val = rex5260_ts_get_value,
	   .x_max = 800,
	   .y_max = 600,
};

static struct i2c_board_info i2c_devs_touch[] __initdata = {
	{
		I2C_BOARD_INFO("pixcir_ts", 0x5c),
		.irq		= IRQ_EINT(14),
		.platform_data  = &rex5260_ts_data,
	},
};

#endif


static struct gpio_keys_platform_data rex5260_gpiokeys_platform_data = {
	rex5260_button,
	ARRAY_SIZE(rex5260_button),
};


static struct platform_device rex5260_gpio_keys = {
	.name	= "gpio-keys",
	.dev	= {
		.platform_data = &rex5260_gpiokeys_platform_data,
	},
};

static struct platform_device *rex5260_input_devices[] __initdata = {
    	&s3c_device_i2c4,
    	&rex5260_gpio_keys,
};


void __init exynos5_rex5260_input_init(void)
{
    pr_info("%s, is called\n", __func__);

#if defined(CONFIG_TOUCHSCREEN_FT5X06)
	s3c_i2c4_set_platdata(&i2c_data_touch);
	i2c_register_board_info(8, i2c_devs_touch, ARRAY_SIZE(i2c_devs_touch));
	exynos5260_touch_init();
#else
	s3c_i2c4_set_platdata(&i2c_data_touch);
	i2c_register_board_info(8, i2c_devs_touch, ARRAY_SIZE(i2c_devs_touch));
	exynos5260_touch_init();
#endif
	rex5260_gpio_keys_config_setup();
	platform_add_devices(rex5260_input_devices,
			ARRAY_SIZE(rex5260_input_devices));
}
