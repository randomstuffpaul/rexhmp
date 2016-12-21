/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd.
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

#ifndef __MACH_EXYNOS_BOARD_REX5260_H
#define __MACH_EXYNOS_BOARD_REX5260_H

#define REX5260_REV_0_0        0x0
#define REX5260_REV_0_1        0x1

void exynos5_rex5260_clock_init(void);
void exynos5_rex5260_mmc_init(void);
void exynos5_rex5260_power_init(void);
void exynos5_rex5260_display_init(void);
void exynos5_rex5260_clock_init(void);
void exynos5_rex5260_input_init(void);
void exynos5_rex5260_usb_init(void);
void exynos5_rex5260_audio_init(void);
void exynos5_rex5260_media_init(void);
void exynos5_rex5260_camera_init(void);
void exynos5_rex5260_gps_init(void);
void exynos5_rex5260_mrvl_init(void);
int exynos5_rex5260_get_revision(void);
#endif
