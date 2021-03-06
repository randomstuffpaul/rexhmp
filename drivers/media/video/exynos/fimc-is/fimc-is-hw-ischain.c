#include <linux/module.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/io.h>

#include <mach/map.h>
#include <mach/regs-clock.h>
#include <mach/exynos-mipiphy.h>

#include "fimc-is-config.h"
#include "fimc-is-type.h"
#include "fimc-is-regs.h"
#include "fimc-is-core.h"
#include "fimc-is-dvfs.h"

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0))
#define PM_QOS_CAM_THROUGHPUT	PM_QOS_RESERVED
#endif
extern struct pm_qos_request exynos_isp_qos_int;
extern struct pm_qos_request exynos_isp_qos_mem;
extern struct pm_qos_request exynos_isp_qos_cam;
extern struct pm_qos_request exynos_isp_qos_disp;

#if (FIMC_IS_VERSION == FIMC_IS_VERSION_250)
int fimc_is_runtime_suspend(struct device *dev)
{
	int ret = 0;
#ifndef CONFIG_PM_RUNTIME
	u32 val;
#endif
	struct platform_device *pdev = to_platform_device(dev);
	struct fimc_is_core *core = (struct fimc_is_core *)platform_get_drvdata(pdev);
#if defined(CONFIG_PM_DEVFREQ)
	int int_qos, mif_qos;
#endif

	BUG_ON(!core);
	BUG_ON(!core->pdata);
	BUG_ON(!core->pdata->clk_off);

	info("FIMC_IS runtime suspend in\n");

#if defined(CONFIG_VIDEOBUF2_ION)
	if (core->mem.alloc_ctx)
		vb2_ion_detach_iommu(core->mem.alloc_ctx);
#endif

#if defined(CONFIG_FIMC_IS_BUS_DEVFREQ)
	exynos5_update_media_layers(TYPE_FIMC_LITE, false);
#endif

#ifndef CONFIG_PM_RUNTIME
	/* ISP1 */
	/* 1. set internal clock reset */
	val = __raw_readl(PMUREG_CMU_RESET_ISP1_SYS_PWR);
	val = (val & ~(0x1 << 0)) | (0x0 << 0);
	__raw_writel(val, PMUREG_CMU_RESET_ISP1_SYS_PWR);

	/* 2. change to OSCCLK */
	ret = core->pdata->clk_off(pdev);
	if (ret) {
		err("clk_off is fail(%d)", ret);
		goto p_err;
	}

	/* 3. set feedback mode */
	val = __raw_readl(PMUREG_ISP1_OPTION);
	val = (val & ~(0x3 << 0)) | (0x2 << 0);
	__raw_writel(val, PMUREG_ISP1_OPTION);

	/* 4. power off */
	val = __raw_readl(PMUREG_ISP1_CONFIGURATION);
	val = (val & ~(0x7 << 0)) | (0x0 << 0);
	__raw_writel(val, PMUREG_ISP1_CONFIGURATION);

	/* ISP0 */
	/* 1. set internal clock reset */
	val = __raw_readl(PMUREG_CMU_RESET_ISP0_SYS_PWR);
	val = (val & ~(0x1 << 0)) | (0x0 << 0);
	__raw_writel(val, PMUREG_CMU_RESET_ISP0_SYS_PWR);

	/* 2. set standbywfi a5 */
	val = __raw_readl(PMUREG_CENTRAL_SEQ_OPTION);
	val = (val & ~(0x1 << 18)) | (0x1 << 18);
	__raw_writel(val, PMUREG_CENTRAL_SEQ_OPTION);

	/* 3. stop a5 */
	__raw_writel(0x00010000, PMUREG_ISP_ARM_OPTION);

	/* 4. reset a5 */
	val = __raw_readl(PMUREG_ISP_ARM_SYS_PWR_REG);
	val = (val & ~(0x1 << 0)) | (0x1 << 0);
	__raw_writel(val, PMUREG_ISP_ARM_SYS_PWR_REG);

	/* 5. change to OSCCLK */

	/* 6. set feedback mode */
	val = __raw_readl(PMUREG_ISP0_OPTION);
	val = (val & ~(0x3 << 0)) | (0x2 << 0);
	__raw_writel(val, PMUREG_ISP0_OPTION);

	/* 7. power off */
	val = __raw_readl(PMUREG_ISP0_CONFIGURATION);
	val = (val & ~(0x7 << 0)) | (0x0 << 0);
	__raw_writel(val, PMUREG_ISP0_CONFIGURATION);

	/* 8. a5 power off */
	val = __raw_readl(PMUREG_ISP_ARM_CONFIGURATION);
	val = (val & ~(0x1 << 0)) | (0x0 << 0);
	__raw_writel(val, PMUREG_ISP_ARM_CONFIGURATION);
#endif

#if defined(CONFIG_PM_DEVFREQ)
		/* DEVFREQ release */
	pr_info("[RSC] %s: QoS UNLOCK\n", __func__);
	int_qos = fimc_is_get_qos(core, FIMC_IS_DVFS_INT, FIMC_IS_SN_MAX);
	mif_qos = fimc_is_get_qos(core, FIMC_IS_DVFS_MIF, FIMC_IS_SN_MAX);

	if (int_qos > 0)
		pm_qos_remove_request(&exynos_isp_qos_int);
	if (mif_qos > 0)
		pm_qos_remove_request(&exynos_isp_qos_mem);
#endif

#ifndef CONFIG_PM_RUNTIME
p_err:
#endif
	info("FIMC_IS runtime suspend out\n");
	pm_relax(dev);
	return ret;
}

int fimc_is_runtime_resume(struct device *dev)
{
	int ret = 0;
#ifndef CONFIG_PM_RUNTIME
	u32 val;
#endif
	struct platform_device *pdev = to_platform_device(dev);
	struct fimc_is_core *core = (struct fimc_is_core *)platform_get_drvdata(pdev);
#if defined(CONFIG_PM_DEVFREQ)
	int int_qos, mif_qos;
#endif

	BUG_ON(!core);
	BUG_ON(!core->pdata);
	BUG_ON(!core->pdata->clk_cfg);
	BUG_ON(!core->pdata->clk_on);

	info("FIMC_IS runtime resume in\n");

#ifndef CONFIG_PM_RUNTIME
	/* ISP0 */
	/* 1. set feedback mode */
	val = __raw_readl(PMUREG_ISP0_OPTION);
	val = (val & ~(0x3<< 0)) | (0x2 << 0);
	__raw_writel(val, PMUREG_ISP0_OPTION);

	/* 2. power on isp0 */
	val = __raw_readl(PMUREG_ISP0_CONFIGURATION);
	val = (val & ~(0x7 << 0)) | (0x7 << 0);
	__raw_writel(val, PMUREG_ISP0_CONFIGURATION);

	/* ISP1 */
	/* 3. set feedback mode */
	val = __raw_readl(PMUREG_ISP1_OPTION);
	val = (val & ~(0x3<< 0)) | (0x2 << 0);
	__raw_writel(val, PMUREG_ISP1_OPTION);

	/* 4. power on isp1 */
	val = __raw_readl(PMUREG_ISP1_CONFIGURATION);
	val = (val & ~(0x7 << 0)) | (0x7 << 0);
	__raw_writel(val, PMUREG_ISP1_CONFIGURATION);
#endif

	ret = core->pdata->clk_cfg(pdev);
	if (ret) {
		err("clk_cfg is fail(%d)", ret);
		goto p_err;
	}

	/* HACK: DVFS lock sequence is change.
	 * DVFS level should be locked after power on.
	 */
#if defined(CONFIG_PM_DEVFREQ)
	int_qos = fimc_is_get_qos(core, FIMC_IS_DVFS_INT, FIMC_IS_SN_MAX);
	mif_qos = fimc_is_get_qos(core, FIMC_IS_DVFS_MIF, FIMC_IS_SN_MAX);

	pm_qos_add_request(&exynos_isp_qos_int, PM_QOS_DEVICE_THROUGHPUT, int_qos);
	pm_qos_add_request(&exynos_isp_qos_mem, PM_QOS_BUS_THROUGHPUT, mif_qos);

	info("[RSC] DVFS LOCK(int(%d), mif(%d))\n", int_qos, mif_qos);

	pm_qos_update_request(&exynos_isp_qos_int, int_qos);
	pm_qos_update_request(&exynos_isp_qos_mem, mif_qos);
#endif

	/* Clock on */
	ret = core->pdata->clk_on(pdev);
	if (ret) {
		err("clk_on is fail(%d)", ret);
		goto p_err;
	}

#if defined(CONFIG_VIDEOBUF2_ION)
	if (core->mem.alloc_ctx)
		vb2_ion_attach_iommu(core->mem.alloc_ctx);
#endif

#if defined(CONFIG_FIMC_IS_BUS_DEVFREQ)
	exynos5_update_media_layers(TYPE_FIMC_LITE, true);
#endif

	pm_stay_awake(dev);

p_err:
	info("FIMC-IS runtime resume out\n");
	return ret;
}

#else
int fimc_is_runtime_suspend(struct device *dev)
{
	int ret = 0;
	u32 debug;
	struct platform_device *pdev = to_platform_device(dev);
	struct fimc_is_core *core
		= (struct fimc_is_core *)platform_get_drvdata(pdev);
#if defined(CONFIG_PM_DEVFREQ)
	int int_qos, mif_qos, cam_qos, disp_qos;
#endif

  /* disable A5 */
	if(test_bit(IS_IF_STATE_START, &core->interface.state))
		writel(0x10000, PMUREG_ISP_ARM_OPTION);
	else
		writel(0x00000, PMUREG_ISP_ARM_OPTION);

	debug = readl(core->interface.regs + ISSR6);
	printk(KERN_INFO "%s: A5 state(0x%x)\n", __func__, debug);

	pr_info("FIMC_IS runtime suspend in\n");

#if defined(CONFIG_VIDEOBUF2_ION)
	if (core->mem.alloc_ctx)
		vb2_ion_detach_iommu(core->mem.alloc_ctx);
#endif

#if defined(CONFIG_PM_DEVFREQ)
	/* DEVFREQ release */
	pr_info("[RSC] %s: QoS UNLOCK\n", __func__);
	int_qos = fimc_is_get_qos(core, FIMC_IS_DVFS_INT, FIMC_IS_SN_MAX);
	mif_qos = fimc_is_get_qos(core, FIMC_IS_DVFS_MIF, FIMC_IS_SN_MAX);
	cam_qos = fimc_is_get_qos(core, FIMC_IS_DVFS_CAM, FIMC_IS_SN_MAX);
	disp_qos = fimc_is_get_qos(core, FIMC_IS_DVFS_DISP, FIMC_IS_SN_MAX);

	if (int_qos > 0)
		pm_qos_remove_request(&exynos_isp_qos_int);
	if (mif_qos > 0)
		pm_qos_remove_request(&exynos_isp_qos_mem);
	if (cam_qos > 0)
		pm_qos_remove_request(&exynos_isp_qos_cam);
	if (disp_qos > 0)
		pm_qos_remove_request(&exynos_isp_qos_disp);
#endif

#if defined(CONFIG_FIMC_IS_BUS_DEVFREQ)
	/* BTS */
#if defined(CONFIG_SOC_EXYNOS5260)
	bts_initialize("spd-flite-a", false);
	bts_initialize("spd-flite-b", false);
#endif
	/* media layer */
	exynos5_update_media_layers(TYPE_FIMC_LITE, false);
#endif

#if defined(CONFIG_MACH_SMDK5410) || defined(CONFIG_MACH_SMDK5420)
	/* Sensor power off */
        /* TODO : need close_sensor */
	if (core->pdata->cfg_gpio) {
		core->pdata->cfg_gpio(core->pdev, 0, false);
		core->pdata->cfg_gpio(core->pdev, 2, false);
	} else {
		err("failed to sensor_power_on\n");
		ret = -EINVAL;
		goto p_err;
	}
#endif

	if (core->pdata->clk_off) {
		core->pdata->clk_off(core->pdev);
	} else {
		err("clk_off is fail\n");
		ret = -EINVAL;
		goto p_err;
	}

	pr_info("FIMC_IS runtime suspend out\n");

p_err:
	pm_relax(dev);
	return ret;
}

int fimc_is_runtime_resume(struct device *dev)
{
	int ret = 0;
	struct platform_device *pdev = to_platform_device(dev);
	struct fimc_is_core *core
		= (struct fimc_is_core *)platform_get_drvdata(pdev);
#if defined(CONFIG_PM_DEVFREQ)
	int int_qos, mif_qos, cam_qos, disp_qos;
#endif

	pm_stay_awake(dev);
	pr_info("FIMC_IS runtime resume in\n");

	/* HACK: DVFS lock sequence is change.
	 * DVFS level should be locked after power on.
	 */
#if defined(CONFIG_PM_DEVFREQ)
	int_qos = fimc_is_get_qos(core, FIMC_IS_DVFS_INT, FIMC_IS_SN_MAX);
	mif_qos = fimc_is_get_qos(core, FIMC_IS_DVFS_MIF, FIMC_IS_SN_MAX);
	cam_qos = fimc_is_get_qos(core, FIMC_IS_DVFS_CAM, FIMC_IS_SN_MAX);
	disp_qos = fimc_is_get_qos(core, FIMC_IS_DVFS_DISP, FIMC_IS_SN_MAX);

	/* DEVFREQ lock */
	if (int_qos > 0)
		pm_qos_add_request(&exynos_isp_qos_int, PM_QOS_DEVICE_THROUGHPUT, int_qos);
	if (mif_qos > 0)
		pm_qos_add_request(&exynos_isp_qos_mem, PM_QOS_BUS_THROUGHPUT, mif_qos);
	if (cam_qos > 0)
		pm_qos_add_request(&exynos_isp_qos_cam, PM_QOS_CAM_THROUGHPUT, cam_qos);
	if (disp_qos > 0)
		pm_qos_add_request(&exynos_isp_qos_disp, PM_QOS_DISPLAY_THROUGHPUT, disp_qos);

	pr_info("[RSC] %s: QoS LOCK [INT(%d), MIF(%d), CAM(%d), DISP(%d)]\n",
		__func__, int_qos, mif_qos, cam_qos, disp_qos);
#endif

	/* Low clock setting */
	if (CALL_POPS(core, clk_cfg, core->pdev) < 0) {
		err("clk_cfg is fail\n");
		ret = -EINVAL;
		goto p_err;
	}

	/* Clock on */
	if (CALL_POPS(core, clk_on, core->pdev) < 0) {
		err("clk_on is fail\n");
		ret = -EINVAL;
		goto p_err;
	}

#if defined(CONFIG_VIDEOBUF2_ION)
	if (core->mem.alloc_ctx)
		vb2_ion_attach_iommu(core->mem.alloc_ctx);
#endif

#if defined(CONFIG_FIMC_IS_BUS_DEVFREQ)
	/* BTS */
#if defined(CONFIG_SOC_EXYNOS5260)
	bts_initialize("spd-flite-a", true);
	bts_initialize("spd-flite-b", true);
#endif
	/* media layer */
	exynos5_update_media_layers(TYPE_FIMC_LITE, true);
#endif

	pr_info("FIMC-IS runtime resume out\n");

	return 0;

p_err:
	pm_relax(dev);
	return ret;
}
#endif
