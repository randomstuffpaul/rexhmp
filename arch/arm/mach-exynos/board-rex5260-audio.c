/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/fixed.h>

#include <linux/clk.h>
#include <linux/module.h>

#if defined(CONFIG_SND_SOC_ES515) || defined(CONFIG_SND_SOC_ES515_MODULE)
#include <linux/i2c/esxxx.h>
#endif
#include <linux/io.h>

#include <plat/gpio-cfg.h>
#include <plat/devs.h>
#include <plat/iic.h>

#include <mach/hs-iic.h>
#include <mach/map.h>

#include "board-rex5260.h"


static struct i2c_board_info __initdata i2c_devs1[] = {
#if defined(CONFIG_SND_SOC_SAMSUNG_SMDK_WM1800)
{
	I2C_BOARD_INFO("wm1800", 0x1a),
},
#endif 
};

#ifdef CONFIG_SND_SAMSUNG_PCM
static struct platform_device exynos_smdk_pcm = {
	.name = "samsung-smdk-pcm",
	.id = -1,
};
#endif

#ifdef CONFIG_SND_SOC_DUMMY_CODEC
static struct platform_device exynos_dummy_codec = {
	.name = "dummy-codec",
	.id = -1,
};
#endif

static struct platform_device *rex5260_audio_devices[] __initdata = {
        &exynos5_device_lpass,
	&exynos5_device_hs_i2c1,
#ifdef CONFIG_SND_SAMSUNG_I2S
	&exynos5_device_i2s0,
#endif
#ifdef CONFIG_SND_SAMSUNG_AUX_HDMI
	&exynos5_device_i2s1,
#endif
#ifdef CONFIG_SND_SAMSUNG_PCM
	&exynos5_device_pcm0,
	&exynos_smdk_pcm,
#endif
#if defined(CONFIG_SND_SAMSUNG_SPDIF) || defined(CONFIG_SND_SAMSUNG_AUX_SPDIF)
	&exynos5_device_spdif,
#endif
#ifdef CONFIG_SND_SAMSUNG_ALP
        &exynos5_device_srp,
#endif
	&samsung_asoc_dma,
	&samsung_asoc_idma, // eric seo
#ifdef CONFIG_SND_SOC_DUMMY_CODEC
	&exynos_dummy_codec,
#endif
};

static void exynos_cfg_hs_i2c1_gpio(struct platform_device *pdev)
{
            s3c_gpio_cfgpin_range(EXYNOS5260_GPB3(2), 2,  S3C_GPIO_SFN(2));
}

struct exynos5_platform_i2c hs_i2c1_data __initdata = {
	.bus_number = 1,
	.operation_mode = HSI2C_POLLING,
	.speed_mode = HSI2C_FAST_SPD,
	.fast_speed = 400000,
	.high_speed = 2500000,
	.cfg_gpio = exynos_cfg_hs_i2c1_gpio,
};

void __init exynos5_rex5260_audio_init(void)
{	
	exynos5_hs_i2c1_set_platdata(&hs_i2c1_data);
	i2c_register_board_info(1, i2c_devs1, ARRAY_SIZE(i2c_devs1));

	platform_add_devices(rex5260_audio_devices,
			ARRAY_SIZE(rex5260_audio_devices));

	/* gpio power-down config */
	s5p_gpio_set_pd_cfg(EXYNOS5260_GPD0(5), S5P_GPIO_PD_PREV_STATE);
	s5p_gpio_set_pd_cfg(EXYNOS5260_GPD0(6), S5P_GPIO_PD_PREV_STATE);
	s5p_gpio_set_pd_cfg(EXYNOS5260_GPX0(5), S5P_GPIO_PD_PREV_STATE);

}
