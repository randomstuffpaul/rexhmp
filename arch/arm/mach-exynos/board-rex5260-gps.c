/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/stat.h>
#include <linux/string.h>

#include <plat/gpio-cfg.h>

#include "board-rex5260.h"

#define GPIO_GPS_RST_N EXYNOS5260_GPD0(2)

static ssize_t gps_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	if (!strcmp(attr->attr.name, "gps_on_off"))
	return 0;
	else if (!strcmp(attr->attr.name, "gps_nrst"))
		return snprintf(buf, 4, "%d\n", gpio_get_value(GPIO_GPS_RST_N));
	else
	return 0;
}

static ssize_t gps_store(struct device *dev, struct device_attribute *attr,
			    const char *buff, size_t size)
{
	int gpio, gpio_value;

	if (!strcmp(attr->attr.name, "gps_nrst"))
		gpio = GPIO_GPS_RST_N;

	if (!sscanf(buff, "%d", &gpio_value))
		return -EINVAL;

	s3c_gpio_cfgpin(gpio, S3C_GPIO_OUTPUT);
	gpio_set_value(gpio, gpio_value);

	return size;
}

static DEVICE_ATTR(gps_nrst, S_IRUGO | S_IWUSR | S_IWGRP, gps_show, gps_store);

static struct platform_device gps_device = {
	.name		= "gps",
	.id		= -1,
};

void __init exynos5_rex5260_gps_init(void)
{
	platform_device_register(&gps_device);
	device_create_file(&gps_device.dev, &dev_attr_gps_nrst);
}
