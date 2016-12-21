/* linux/arch/arm/mach-exynos/include/mach/exynos-devfreq.h
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#ifndef __EXYNOS_DEVFREQ_H_
#define __EXYNOS_DEVFREQ_H_
enum devfreq_transition {
	MIF_DEVFREQ_PRECHANGE,
	MIF_DEVFREQ_POSTCHANGE,
	MIF_DEVFREQ_EN_MONITORING,
	MIF_DEVFREQ_DIS_MONITORING,
};

enum devfreq_media_type {
	TYPE_FIMC_LITE,
	TYPE_MIXER,
	TYPE_FIMD1,
	TYPE_TV,
	TYPE_GSCL_LOCAL,
	TYPE_RESOLUTION,
};

enum devfreq_media_resolution {
	RESOLUTION_FULLHD,
	RESOLUTION_WQHD,
	RESOLUTION_WQXGA,
};

enum devfreq_layer_count {
	NUM_LAYER_0,
	NUM_LAYER_1,
	NUM_LAYER_2,
	NUM_LAYER_3,
	NUM_LAYER_4,
	NUM_LAYER_5,
};

struct exynos_devfreq_platdata {
	unsigned int default_qos;
};

struct devfreq_info {
	unsigned int old;
	unsigned int new;
};

extern struct pm_qos_request exynos5_cpu_int_qos;
extern struct pm_qos_request exynos5_cpu_mif_qos;

extern int exynos5_mif_notify_transition(struct devfreq_info *info, unsigned int state);
extern int exynos5_mif_register_notifier(struct notifier_block *nb);
extern int exynos5_mif_unregister_notifier(struct notifier_block *nb);

extern int exynos5_mif_bpll_register_notifier(struct notifier_block *nb);
extern int exynos5_mif_bpll_unregister_notifier(struct notifier_block *nb);

extern spinlock_t int_div_lock;

#if defined(CONFIG_ARM_EXYNOS5260_BUS_DEVFREQ)
void exynos5_update_media_layers(enum devfreq_media_type media_type, unsigned int value);
void exynos5_disp_update_pixelclk(unsigned long pixelclk);
#else
static inline
void exynos5_update_media_layers(enum devfreq_media_type media_type, unsigned int value)
{
	return;
}
#endif

static inline
void exynos4_update_media_layers(enum devfreq_media_type media_type, unsigned int value)
{
	return;
}

#ifdef CONFIG_ARM_EXYNOS5260_BUS_DEVFREQ
void exynos5_devfreq_disp_enable(bool enable);
#endif

#endif
