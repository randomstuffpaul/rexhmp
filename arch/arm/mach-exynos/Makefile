# arch/arm/mach-exynos/Makefile
#
# Copyright (c) 2010-2011 Samsung Electronics Co., Ltd.
#		http://www.samsung.com/
#
# Licensed under GPLv2

obj-y				:=
obj-m				:=
obj-n				:=
obj-				:=

# Core

obj-$(CONFIG_ARCH_EXYNOS)	+= common.o
obj-$(CONFIG_ARM_TRUSTZONE)	+= irq-sgi.o
obj-$(CONFIG_CPU_EXYNOS4210)	+= clock-exynos4210.o
obj-$(CONFIG_SOC_EXYNOS4212)	+= clock-exynos4212.o
obj-$(CONFIG_SOC_EXYNOS5250)	+= clock-exynos5250.o
obj-$(CONFIG_SOC_EXYNOS5260)	+= clock-exynos5260.o asv-exynos.o asv-exynos5260.o
obj-$(CONFIG_SOC_EXYNOS5410)	+= clock-exynos5410.o asv-exynos.o asv-exynos5410.o cci.o exynos-power-mode.o
obj-$(CONFIG_SOC_EXYNOS5260)	+= cci.o asv-exynos.o
obj-$(CONFIG_SOC_EXYNOS5410)	+= exynos-interface.o
obj-$(CONFIG_BL_SWITCHER)	+= sys-if-switcher.o
obj-$(CONFIG_EXYNOS5_MP)	+= core_sysfs.o

obj-$(CONFIG_EXYNOS5_DEV_BTS)		+= dev-bts.o
obj-$(CONFIG_EXYNOS5410_BTS)		+= bts-exynos5410.o
obj-$(CONFIG_EXYNOS5260_BTS)		+= bts-exynos5260.o

obj-$(CONFIG_EXYNOS_RCG)		+= rcg-exynos.o
obj-$(CONFIG_PM)			+= pm.o

ifeq ($(CONFIG_SOC_EXYNOS5260),y)
obj-$(CONFIG_PM_GENERIC_DOMAINS)	+= pm_domains_v2.o pm_domains-exynos5260.o
else
obj-$(CONFIG_PM_GENERIC_DOMAINS)	+= pm_domains.o pm_domains5.o pm_interrupt_domains.o
endif
obj-$(CONFIG_PM_RUNTIME_TEST_SYSFS)	+= pm_domains_sysfs.o

ifeq ($(CONFIG_SOC_EXYNOS5250),y)
obj-$(CONFIG_CPU_IDLE)         += cpuidle-exynos5250.o
else
obj-$(CONFIG_CPU_IDLE)         += cpuidle.o
obj-$(CONFIG_ARCH_EXYNOS5)	+= exynos-pm.o
endif

obj-$(CONFIG_SOC_EXYNOS5250)	+= ori-asv-exynos.o ori-abb-exynos.o ori-asv-exynos5250.o

obj-$(CONFIG_ARCH_EXYNOS)	+= pmu.o

obj-$(CONFIG_SMP)		+= platsmp.o headsmp.o

obj-$(CONFIG_EXYNOS4_MCT)	+= mct.o

obj-$(CONFIG_HOTPLUG_CPU)	+= hotplug.o

ifeq ($(CONFIG_SOC_EXYNOS5260),y)
obj-y				+= clock-audss-exynos5260.o
else
obj-$(CONFIG_ARCH_EXYNOS)	+= clock-audss.o
endif

obj-$(CONFIG_EXYNOS_FIQ_DEBUGGER)	+= exynos_fiq_debugger.o

obj-$(CONFIG_EXYNOS_BUSFREQ_OPP)     += ppmu.o busfreq_opp_exynos4.o busfreq_opp_4x12.o

obj-$(CONFIG_EXYNOS5_CORESIGHT)		+= coresight-exynos5.o
obj-$(CONFIG_EXYNOS_PERSISTENT_CLOCK)	+= persistent_clock.o

obj-$(CONFIG_EXYNOS54XX_DEBUG)		+= debug_exynos54XX.o

obj-$(CONFIG_ARM_TRUSTZONE)		+= smc.o
plus_sec := $(call as-instr,.arch_extension sec,+sec)
AFLAGS_smc.o :=-Wa,-march=armv7-a$(plus_sec)

# machine support

obj-$(CONFIG_MACH_SMDKC210)		+= mach-smdkv310.o
obj-$(CONFIG_MACH_SMDKV310)		+= mach-smdkv310.o
obj-$(CONFIG_MACH_ARMLEX4210)		+= mach-armlex4210.o
obj-$(CONFIG_MACH_UNIVERSAL_C210)	+= mach-universal_c210.o
obj-$(CONFIG_MACH_NURI)			+= mach-nuri.o
obj-$(CONFIG_MACH_ORIGEN)		+= mach-origen.o

obj-$(CONFIG_MACH_SMDK4212)		+= mach-smdk4x12.o
obj-$(CONFIG_MACH_SMDK4412)		+= mach-smdk4x12.o
obj-$(CONFIG_MACH_SMDK4412)		+= board-smdk4x12-mmc.o
obj-$(CONFIG_MACH_SMDK4412)		+= board-smdk4x12-audio.o
obj-$(CONFIG_MACH_SMDK4412)		+= board-smdk4x12-display.o
obj-$(CONFIG_MACH_SMDK4412)		+= board-smdk4x12-usb.o
obj-$(CONFIG_MACH_SMDK4412)		+= board-smdk4x12-media.o
obj-$(CONFIG_MACH_SMDK4412)		+= board-smdk4x12-power.o

obj-$(CONFIG_MACH_SMDK5410)		+= mach-smdk5410.o
obj-$(CONFIG_MACH_SMDK5410)		+= board-smdk5410-mmc.o
obj-$(CONFIG_MACH_SMDK5410)		+= board-smdk5410-power.o
obj-$(CONFIG_MACH_SMDK5410)		+= board-smdk5410-usb.o
obj-$(CONFIG_MACH_SMDK5410)		+= board-smdk5410-audio.o
obj-$(CONFIG_MACH_SMDK5410)		+= board-smdk5410-input.o
obj-$(CONFIG_MACH_SMDK5410)		+= board-smdk5410-clock.o
obj-$(CONFIG_MACH_SMDK5410)		+= board-smdk5410-media.o
obj-$(CONFIG_MACH_SMDK5410)		+= board-smdk5410-display.o

obj-$(CONFIG_MACH_EXYNOS4_DT)		+= mach-exynos4-dt.o
obj-$(CONFIG_MACH_EXYNOS5_DT)		+= mach-exynos5-dt.o

obj-$(CONFIG_MACH_SMDK5250)		+= mach-smdk5250.o
obj-$(CONFIG_SEC_GPIO_DVS)              += board-rex5260-gpiodvs.o
#Samsung sec sysfs Feature
obj-$(CONFIG_SEC_SYSFS)                 += sec_sysfs.o

# Samsung Debug Feature
obj-$(CONFIG_SEC_DEBUG)                 += sec_debug.o sec_getlog.o sec_gaf.o
obj-$(CONFIG_SEC_LOG)                   += sec_log.o

# Samsung Reset Feature
obj-$(CONFIG_MACH_REX5260)        	+= sec_reboot.o
obj-$(CONFIG_MACH_REX5260)		+= mach-rex5260.o
obj-$(CONFIG_MACH_REX5260)		+= board-rex5260-clock.o
obj-$(CONFIG_MACH_REX5260)		+= board-rex5260-mmc.o
obj-$(CONFIG_MACH_REX5260)		+= board-rex5260-power.o
obj-$(CONFIG_MACH_REX5260)		+= board-rex5260-display.o
obj-$(CONFIG_MACH_REX5260)		+= board-rex5260-clock.o
obj-$(CONFIG_MACH_REX5260)		+= board-rex5260-input.o
obj-$(CONFIG_MACH_REX5260)		+= board-rex5260-usb.o
obj-$(CONFIG_MACH_REX5260)		+= board-rex5260-audio.o
obj-$(CONFIG_MACH_REX5260)		+= board-rex5260-media.o
obj-$(CONFIG_MACH_REX5260)		+= board-rex5260-camera.o
obj-$(CONFIG_MACH_REX5260)		+= board-rex5260-gps.o

obj-$(CONFIG_MACH_XYREF5260)		+= mach-xyref5260.o
obj-$(CONFIG_MACH_XYREF5260)		+= board-xyref5260-clock.o
obj-$(CONFIG_MACH_XYREF5260)		+= board-xyref5260-mmc.o
obj-$(CONFIG_MACH_XYREF5260)		+= board-xyref5260-power.o
obj-$(CONFIG_MACH_XYREF5260)		+= board-xyref5260-display.o
obj-$(CONFIG_MACH_XYREF5260)		+= board-xyref5260-clock.o
obj-$(CONFIG_MACH_XYREF5260)		+= board-xyref5260-input.o
obj-$(CONFIG_MACH_XYREF5260)		+= board-xyref5260-usb.o
obj-$(CONFIG_MACH_XYREF5260)		+= board-xyref5260-audio.o
obj-$(CONFIG_MACH_XYREF5260)		+= board-xyref5260-media.o
obj-$(CONFIG_MACH_XYREF5260)		+= board-xyref5260-camera.o
obj-$(CONFIG_MACH_XYREF5260)		+= board-xyref5260-gps.o


# device support

obj-y					+= dev-uart.o
obj-$(CONFIG_SND_SOC_SAMSUNG)		+= dev-audio.o
obj-$(CONFIG_EXYNOS4_DEV_AHCI)		+= dev-ahci.o
obj-$(CONFIG_EXYNOS_DEV_DWMCI)		+= dev-dwmci.o
obj-$(CONFIG_EXYNOS3_DEV_FIMC_IS)	+= dev-fimc-is.o
obj-$(CONFIG_EXYNOS4_DEV_FIMC_IS)	+= dev-exynos4-fimc-is.o
obj-$(CONFIG_EXYNOS5_DEV_FIMC_IS)	+= dev-exynos5-fimc-is.o
obj-$(CONFIG_EXYNOS_DEV_FIMC_IS)	+= dev-fimc-is.o

obj-$(CONFIG_EXYNOS4_DEV_FIMC_LITE)	+= dev-fimc-lite.o
obj-$(CONFIG_EXYNOS_DEV_GSC)		+= dev-gsc.o setup-gsc.o
obj-$(CONFIG_EXYNOS5_DEV_SCALER)	+= dev-scaler.o
obj-$(CONFIG_EXYNOS_DEV_ROTATOR)	+= dev-rotator.o
obj-$(CONFIG_EXYNOS_DEV_SYSMMU)		+= dev-sysmmu.o
obj-$(CONFIG_EXYNOS_DEV_DMA)		+= dma.o
obj-$(CONFIG_EXYNOS_DEV_USB_SWITCH)	+= dev-usb-switch.o
obj-$(CONFIG_EXYNOS4_DEV_USB_OHCI)	+= dev-ohci.o
obj-$(CONFIG_EXYNOS5_DEV_USB3_DRD)	+= dev-usb3-drd.o
obj-$(CONFIG_EXYNOS4_DEV_JPEG)		+= dev-jpeg.o
obj-$(CONFIG_EXYNOS5_DEV_JPEG)			+= dev-jpeg.o
obj-$(CONFIG_EXYNOS5_DEV_JPEG_HX)			+= dev-jpeg-hx.o
obj-$(CONFIG_EXYNOS_DEV_TMU)		+= dev-tmu.o
obj-$(CONFIG_EXYNOS5_DEV_HSI2C0)	+= dev-hs-i2c0.o
obj-$(CONFIG_EXYNOS5_DEV_HSI2C1)	+= dev-hs-i2c1.o
obj-$(CONFIG_EXYNOS5_DEV_HSI2C2)	+= dev-hs-i2c2.o
obj-$(CONFIG_EXYNOS5_DEV_HSI2C3)	+= dev-hs-i2c3.o
obj-$(CONFIG_EXYNOS5_DEV_HSI2C4)	+= dev-hs-i2c4.o
obj-$(CONFIG_EXYNOS5_DEV_HSI2C5)	+= dev-hs-i2c5.o
obj-$(CONFIG_EXYNOS5_DEV_HSI2C6)	+= dev-hs-i2c6.o
obj-$(CONFIG_S5P_DEV_FIMG2D)		+= dev-fimg2d.o

obj-$(CONFIG_ARCH_EXYNOS)		+= setup-i2c0.o
obj-$(CONFIG_EXYNOS4_SETUP_FIMC)	+= setup-fimc.o
obj-$(CONFIG_EXYNOS4_SETUP_CSIS)	+= setup-csis.o
obj-$(CONFIG_EXYNOS4_SETUP_FIMD0)	+= setup-fimd0.o
obj-$(CONFIG_EXYNOS_SETUP_FIMD1)	+= setup-fimd1.o setup-disp-clock.o
obj-$(CONFIG_EXYNOS_SETUP_DP)		+= setup-dp.o
obj-$(CONFIG_FB_MIPI_DSIM)		+= setup-mipidsim.o
obj-$(CONFIG_EXYNOS_CONTENT_PATH_PROTECTION) += secmem.o
obj-$(CONFIG_EXYNOS3_SETUP_FIMC_IS)	+= setup-exynos-fimc-is.o
obj-$(CONFIG_EXYNOS4_SETUP_FIMC_IS)	+= setup-exynos-fimc-is.o
obj-$(CONFIG_EXYNOS5_SETUP_FIMC_IS)	+= setup-exynos-fimc-is.o
obj-$(CONFIG_EXYNOS_SETUP_FIMC_IS)	+= setup-fimc-is.o
obj-$(CONFIG_EXYNOS_SETUP_FIMC_IS)	+= setup-fimc-is-sensor.o
obj-$(CONFIG_VISION_MODE)		+= setup-fimc-is-vision.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C1)	+= setup-i2c1.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C2)	+= setup-i2c2.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C3)	+= setup-i2c3.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C4)	+= setup-i2c4.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C5)	+= setup-i2c5.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C6)	+= setup-i2c6.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C7)	+= setup-i2c7.o
obj-$(CONFIG_EXYNOS5_SETUP_HSI2C0)	+= setup-hs-i2c0.o
obj-$(CONFIG_EXYNOS5_SETUP_HSI2C1)	+= setup-hs-i2c1.o
obj-$(CONFIG_EXYNOS5_SETUP_HSI2C2)	+= setup-hs-i2c2.o
obj-$(CONFIG_EXYNOS5_SETUP_HSI2C3)	+= setup-hs-i2c3.o
obj-$(CONFIG_EXYNOS5_SETUP_HSI2C4)	+= setup-hs-i2c4.o
obj-$(CONFIG_EXYNOS5_SETUP_HSI2C5)	+= setup-hs-i2c5.o
obj-$(CONFIG_EXYNOS5_SETUP_HSI2C6)	+= setup-hs-i2c6.o
obj-$(CONFIG_EXYNOS4_SETUP_KEYPAD)	+= setup-keypad.o
obj-$(CONFIG_EXYNOS4_SETUP_MFC)		+= setup-mfc.o
obj-$(CONFIG_EXYNOS4_SETUP_SDHCI_GPIO)	+= setup-sdhci-gpio.o
obj-$(CONFIG_EXYNOS4_SETUP_USB_PHY)	+= setup-usb-phy.o
obj-$(CONFIG_EXYNOS_SETUP_SPI)		+= setup-spi.o
obj-$(CONFIG_EXYNOS5_SETUP_TVOUT)	+= setup-tvout.o
obj-$(CONFIG_EXYNOS_SETUP_ADC)		+= setup-adc.o
obj-$(CONFIG_ION_EXYNOS)		+= dev-ion.o
obj-$(CONFIG_CMA)			+= reserve-mem.o
obj-$(CONFIG_EXYNOS4_SETUP_JPEG)	+= setup-jpeg.o
obj-$(CONFIG_EXYNOS5_SETUP_JPEG)			+= setup-jpeg.o
obj-$(CONFIG_ARCH_EXYNOS5)		+= resetreason.o
obj-$(CONFIG_EXYNOS_TMU)		+= tmu-exynos.o
obj-$(CONFIG_EXYNOS_SETUP_MIPIPHY)	+= setup-exynos-mipiphy.o

obj-$(CONFIG_BL_SWITCHER)		+= bL_control.o bL_setup.o
