/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/serial_core.h>
#include <linux/persistent_ram.h>
#include <linux/cma.h>
#include <linux/io.h>
#include <linux/notifier.h>
#include <linux/reboot.h>

#include <asm/mach/arch.h>
#include <asm/hardware/gic.h>
#include <asm/mach-types.h>

#include <plat/clock.h>
#include <plat/cpu.h>
#include <plat/devs.h>
#include <plat/regs-serial.h>
#include <plat/watchdog.h>
#include <plat/adc.h>

#include <mach/exynos_fiq_debugger.h>
#include <mach/map.h>
#include <mach/pmu.h>

#include "common.h"
#include "board-rex5260.h"

/* Following are default values for UCON, ULCON and UFCON UART registers */
#define REX5260_UCON_DEFAULT	(S3C2410_UCON_TXILEVEL |	\
				 S3C2410_UCON_RXILEVEL |	\
				 S3C2410_UCON_TXIRQMODE |	\
				 S3C2410_UCON_RXIRQMODE |	\
				 S3C2410_UCON_RXFIFO_TOI |	\
				 S3C2443_UCON_RXERR_IRQEN)

#define REX5260_ULCON_DEFAULT	S3C2410_LCON_CS8

#define REX5260_UFCON_DEFAULT	(S3C2410_UFCON_FIFOMODE |	\
				 S5PV210_UFCON_TXTRIG4 |	\
				 S5PV210_UFCON_RXTRIG4)

static struct s3c2410_uartcfg rex5260_uartcfgs[] __initdata = {
	[0] = {
		.hwport		= 0,
		.flags		= 0,
		.ucon		= REX5260_UCON_DEFAULT,
		.ulcon		= REX5260_ULCON_DEFAULT,
		.ufcon		= REX5260_UFCON_DEFAULT,
	},
	[1] = {
		.hwport		= 1,
		.flags		= 0,
		.ucon		= REX5260_UCON_DEFAULT,
		.ulcon		= REX5260_ULCON_DEFAULT,
		.ufcon		= REX5260_UFCON_DEFAULT,
	},
	[2] = {
#ifndef CONFIG_EXYNOS_FIQ_DEBUGGER
	/*
	 * Don't need to initialize hwport 2, when FIQ debugger is
	 * enabled. Because it will be handled by fiq_debugger.
	 */
		.hwport		= 2,
		.flags		= 0,
		.ucon		= REX5260_UCON_DEFAULT,
		.ulcon		= REX5260_ULCON_DEFAULT,
		.ufcon		= REX5260_UFCON_DEFAULT,
#endif
	},
	[3] = {
		.hwport		= 3,
		.flags		= 0,
		.ucon		= REX5260_UCON_DEFAULT,
		.ulcon		= REX5260_ULCON_DEFAULT,
		.ufcon		= REX5260_UFCON_DEFAULT,
#if defined(CONFIG_PM_RUNTIME)
		.cfg_gpio	= exynos_uart3_cfg_gpio,
		.is_aud_uart	= true,
#endif
	},
};

/* ADC */
static struct s3c_adc_platdata rex5260_adc_data __initdata = {
	.phy_init	= s3c_adc_phy_init,
	.phy_exit	= s3c_adc_phy_exit,
};

/* WDT */
static struct s3c_watchdog_platdata rex5260_watchdog_platform_data = {
	exynos_pmu_wdt_control,
	PMU_WDT_RESET_TYPE2,
};

static struct platform_device *rex5260_devices[] __initdata = {
#ifdef CONFIG_MALI_T6XX
	&exynos5_device_g3d,
#endif
	&s3c_device_wdt,
	&s3c_device_rtc,
	&s3c_device_adc,
#ifdef CONFIG_S5P_DEV_ACE
	&s5p_device_sss,
	&s5p_device_slimsss,
#endif
};

#if defined(CONFIG_CMA)
#include "reserve-mem.h"
static void __init exynos_reserve_mem(void)
{
	static struct cma_region regions[] = {
#if defined(CONFIG_ION_EXYNOS_CONTIGHEAP_SIZE) && \
		(CONFIG_ION_EXYNOS_CONTIGHEAP_SIZE != 0)
		{
			.name = "ion",
			.size = CONFIG_ION_EXYNOS_CONTIGHEAP_SIZE * SZ_1K,
		},
#endif
#ifdef CONFIG_EXYNOS_CONTENT_PATH_PROTECTION
#ifdef CONFIG_ION_EXYNOS_DRM_MFC_SH
		{
			.name = "drm_mfc_sh",
			.size = SZ_1M,
		},
#endif
#if defined(CONFIG_ION_EXYNOS_DRM_MEMSIZE_G2D_WFD) && \
		(CONFIG_ION_EXYNOS_DRM_MEMSIZE_G2D_WFD != 0)
		{
			.name = "drm_g2d_wfd",
			.size = CONFIG_ION_EXYNOS_DRM_MEMSIZE_G2D_WFD *
				SZ_1K,
			{
				.alignment = SZ_1M,
			}
		},
#endif
#endif
#ifdef CONFIG_BL_SWITCHER
		{
			.name = "bl_mem",
			.size = SZ_8K,
		},
#endif
		{
			.size = 0
		},
	};
#ifdef CONFIG_EXYNOS_CONTENT_PATH_PROTECTION
	static struct cma_region regions_secure[] = {
#if defined(CONFIG_ION_EXYNOS_DRM_VIDEO) && \
		(CONFIG_ION_EXYNOS_DRM_VIDEO != 0)
	       {
			.name = "drm_video",
			.size = CONFIG_ION_EXYNOS_DRM_VIDEO *
				SZ_1K,
			{
				.alignment = SZ_1M,
			}
	       },
#endif
#if defined(CONFIG_ION_EXYNOS_DRM_MEMSIZE_MFC_INPUT) && \
		(CONFIG_ION_EXYNOS_DRM_MEMSIZE_MFC_INPUT != 0)
	       {
			.name = "drm_mfc_input",
			.size = CONFIG_ION_EXYNOS_DRM_MEMSIZE_MFC_INPUT *
				SZ_1K,
			{
				.alignment = SZ_1M,
			}
	       },
#endif
#ifdef CONFIG_ION_EXYNOS_DRM_MFC_FW
		{
			.name = "drm_mfc_fw",
			.size = SZ_1M,
			{
				.alignment = SZ_1M,
			}
		},
#endif
#ifdef CONFIG_ION_EXYNOS_DRM_SECTBL
		{
			.name = "drm_sectbl",
			.size = SZ_1M,
			{
				.alignment = SZ_1M,
			}
		},
#endif
		{
			.size = 0
		},
	};
#else /* !CONFIG_EXYNOS_CONTENT_PATH_PROTECTION */
	struct cma_region *regions_secure = NULL;
#endif /* CONFIG_EXYNOS_CONTENT_PATH_PROTECTION */
	static const char map[] __initconst =
#ifdef CONFIG_EXYNOS_CONTENT_PATH_PROTECTION
		"ion-exynos/mfc_sh=drm_mfc_sh;"
		"ion-exynos/g2d_wfd=drm_g2d_wfd;"
		"ion-exynos/video=drm_video;"
		"ion-exynos/mfc_input=drm_mfc_input;"
		"ion-exynos/mfc_fw=drm_mfc_fw;"
		"ion-exynos/sectbl=drm_sectbl;"
		"s5p-smem/mfc_sh=drm_mfc_sh;"
		"s5p-smem/g2d_wfd=drm_g2d_wfd;"
		"s5p-smem/video=drm_video;"
		"s5p-smem/mfc_input=drm_mfc_input;"
		"s5p-smem/mfc_fw=drm_mfc_fw;"
		"s5p-smem/sectbl=drm_sectbl;"
#endif
#ifdef CONFIG_BL_SWITCHER
		"b.L_mem=bl_mem;"
#endif
		"ion-exynos=ion;";
	exynos_cma_region_reserve(regions, regions_secure, NULL, map);
}
#else /*!CONFIG_CMA*/
static inline void exynos_reserve_mem(void)
{
}
#endif

static void __init rex5260_map_io(void)
{
	clk_xusbxti.rate = 24000000;
	clk_xxti.rate = 24000000;

	exynos_init_io(NULL, 0);
	s3c24xx_init_clocks(clk_xusbxti.rate);
	s3c24xx_init_uarts(rex5260_uartcfgs, ARRAY_SIZE(rex5260_uartcfgs));
}

#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/clk.h>

#include <linux/delay.h>
#include <plat/ehci.h>
#include <plat/devs.h>
#include <plat/gpio-cfg.h>

#define HW_REV0_GPIO         EXYNOS5260_GPB2(2)
#define HW_REV1_GPIO         EXYNOS5260_GPB2(3)

static int rex5260_hw_rev;

static void rex5260_init_hw_rev(void)
{
    int GREV0=0, GREV1=0, hw_rev=0;

    s3c_gpio_cfgpin(HW_REV0_GPIO, S3C_GPIO_SFN(0x0));
    s3c_gpio_setpull(HW_REV0_GPIO, S3C_GPIO_PULL_DOWN);

    s3c_gpio_cfgpin(HW_REV0_GPIO, S3C_GPIO_SFN(0x0));
    s3c_gpio_setpull(HW_REV0_GPIO, S3C_GPIO_PULL_DOWN);

    GREV0=gpio_get_value(HW_REV0_GPIO);
    GREV1=gpio_get_value(HW_REV1_GPIO);

    hw_rev = (GREV0<<0) | (GREV1<<1);            

    if(hw_rev==0) 
	    rex5260_hw_rev = REX5260_REV_0_0;
    else
	    rex5260_hw_rev = REX5260_REV_0_1;

    pr_info("REX5260 HW revision: %d\n", 
	    rex5260_hw_rev);
}

int exynos5_rex5260_get_revision(void)
{
            return rex5260_hw_rev;
}


#define WLAN_PDN           EXYNOS5260_GPX2(6)
#define WLAN_RESET         EXYNOS5260_GPX2(3)

#if 0

int i2s_audio_clk_set(void)
{
	void __iomem *i2s_clk_base;
	int ret;


	i2s_clk_base = ioremap(EXYNOS5260_PA_I2S_CLK_CON, SZ_4K);
	if (!i2s_clk_base)
		return -1;

	ret = __raw_readl(i2s_clk_base + 0x4);
	__raw_writel((1<<1),i2s_clk_base + 0x4);
	printk("%s  ============================== 0x%x\n",__func__, __raw_readl(i2s_clk_base + 0x0));
	printk("%s  ============================== 0x%x\n",__func__, __raw_readl(i2s_clk_base + 0x4));

	iounmap(i2s_clk_base);

	return ret;
}
EXPORT_SYMBOL_GPL(i2s_audio_clk_set);
#endif


#define WLAN_PDN           EXYNOS5260_GPX2(6)
#define WLAN_RESET         EXYNOS5260_GPX2(3)


static void mrvl_init(void)
{
    if (gpio_request_one(WLAN_PDN, GPIOF_DIR_OUT, "WLAN_PDN")) {
	pr_err("failed to request WLAN_PDNn\n");
    }
    s3c_gpio_cfgpin_range(WLAN_PDN, 1, S3C_GPIO_SFN(1));
    s3c_gpio_setpull(WLAN_PDN, S3C_GPIO_PULL_UP);
    s5p_gpio_set_pd_cfg(WLAN_PDN, 0);

    gpio_set_value(WLAN_PDN,0);
    mdelay(300);
    gpio_set_value(WLAN_PDN,1);
    mdelay(20);
    gpio_free(WLAN_PDN);

    if (gpio_request_one(WLAN_RESET, GPIOF_DIR_OUT, "WLAN_RESET")) {
	pr_err("failed to request WLAN_RESETn\n");
    }
    s3c_gpio_cfgpin_range(WLAN_RESET, 1, S3C_GPIO_SFN(1));
    s5p_gpio_set_pd_cfg(WLAN_RESET, 0);

    s3c_gpio_setpull(WLAN_RESET, S3C_GPIO_PULL_UP);
    gpio_set_value(WLAN_RESET,0);
    mdelay(10);
    gpio_set_value(WLAN_RESET,1);
    mdelay(20);
    gpio_free(WLAN_RESET);

}

static void __init rex5260_machine_init(void)
{
        rex5260_init_hw_rev();
	s3c_adc_set_platdata(&rex5260_adc_data);
	s3c_watchdog_set_platdata(&rex5260_watchdog_platform_data);
#ifdef CONFIG_EXYNOS_FIQ_DEBUGGER
	exynos_serial_debug_init(2, 0);
#endif
	exynos5_rex5260_clock_init();
#if 0
	i2s_audio_clk_set();
#endif
	exynos5_rex5260_mmc_init();
	exynos5_rex5260_power_init();
	exynos5_rex5260_display_init();
	exynos5_rex5260_input_init();
	exynos5_rex5260_usb_init();
	exynos5_rex5260_audio_init();
	exynos5_rex5260_media_init();
	exynos5_rex5260_camera_init();
	exynos5_rex5260_gps_init();
    	mrvl_init();

	platform_add_devices(rex5260_devices, ARRAY_SIZE(rex5260_devices));
}

MACHINE_START(REX5260, "REX5260")
	.atag_offset	= 0x100,
	.init_irq	= exynos5_init_irq,
	.map_io		= rex5260_map_io,
	.handle_irq	= gic_handle_irq,
	.init_machine	= rex5260_machine_init,
	.timer		= &exynos4_timer,
	.restart	= exynos5_restart,
	.reserve	= exynos_reserve_mem,
MACHINE_END
