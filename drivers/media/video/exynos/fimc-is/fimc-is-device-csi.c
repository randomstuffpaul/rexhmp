/*
 * Samsung Exynos5 SoC series FIMC-IS driver
 *
 * exynos5 fimc-is/mipi-csi functions
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <mach/map.h>
#include <mach/regs-clock.h>
#include <mach/exynos-mipiphy.h>

#include "fimc-is-config.h"
#include "fimc-is-regs.h"
#include "fimc-is-hw.h"
#include "fimc-is-device-sensor.h"
#include "fimc-is-device-csi.h"

#if (FIMC_IS_CSI_VERSION == CSI_VERSION_0000_0000)
extern void s5pcsis_enable_interrupts(unsigned long __iomem *base_reg, struct fimc_is_image *image, bool on);
extern void s5pcsis_set_hsync_settle(unsigned long __iomem *base_reg, int settle);
extern void s5pcsis_set_params(unsigned long __iomem *base_reg, struct fimc_is_image *image);
extern void s5pcsis_reset(unsigned long __iomem *base_reg);
extern void s5pcsis_system_enable(unsigned long __iomem *base_reg, int on);
#endif

static u32 get_hsync_settle(struct fimc_is_sensor_cfg *cfg,
	u32 cfgs, u32 width, u32 height, u32 framerate)
{
	u32 settle;
	u32 max_settle;
	u32 proximity_framerate, proximity_settle;
	u32 i;

	settle = 0;
	max_settle = 0;
	proximity_framerate = 0;
	proximity_settle = 0;

	for (i = 0; i < cfgs; i++) {
		if ((cfg[i].width == width) &&
		    (cfg[i].height == height) &&
		    (cfg[i].framerate == framerate)) {
			settle = cfg[i].settle;
			break;
		}

		if ((cfg[i].width == width) &&
		    (cfg[i].height == height) &&
		    (cfg[i].framerate > proximity_framerate)) {
			proximity_settle = cfg[i].settle;
			proximity_framerate = cfg[i].framerate;
		}

		if (cfg[i].settle > max_settle)
			max_settle = cfg[i].settle;
	}

	if (!settle) {
		if (proximity_settle) {
			settle = proximity_settle;
		} else {
			/*
			 * return a max settle time value in above table
			 * as a default depending on the channel
			 */
			settle = max_settle;

			warn("could not find proper settle time: %dx%d@%dfps",
				width, height, framerate);
		}
	}

	return settle;
}

int fimc_is_csi_open(struct v4l2_subdev *subdev)
{
	int ret = 0;
	struct fimc_is_device_csi *csi;

	BUG_ON(!subdev);

	csi = v4l2_get_subdevdata(subdev);
	if (!csi) {
		err("csi is NULL");
		ret = -EINVAL;
		goto p_err;
	}

	csi->sensor_cfgs = 0;
	csi->sensor_cfg = NULL;
	memset(&csi->image, 0, sizeof(struct fimc_is_image));

p_err:
	return ret;
}

int fimc_is_csi_close(struct v4l2_subdev *subdev)
{
	return 0;
}

/* value : module enum */
static int csi_init(struct v4l2_subdev *subdev, u32 value)
{
	int ret = 0;
	struct fimc_is_device_csi *csi;
	struct fimc_is_module_enum *module;

	BUG_ON(!subdev);
	BUG_ON(!value);

	csi = v4l2_get_subdevdata(subdev);
	if (!csi) {
		err("csi is NULL");
		ret = -EINVAL;
		goto p_err;
	}

	module = (struct fimc_is_module_enum *)value;
	csi->sensor_cfgs = module->cfgs;
	csi->sensor_cfg = module->cfg;
	csi->image.framerate = SENSOR_DEFAULT_FRAMERATE; /* default frame rate */
	csi->image.num_lanes = module->ext.mipi_lane_num;

p_err:
	return ret;
}

static int csi_s_power(struct v4l2_subdev *subdev,
	int on)
{
	int ret = 0;
	struct fimc_is_device_csi *csi;

	BUG_ON(!subdev);

	csi = (struct fimc_is_device_csi *)v4l2_get_subdevdata(subdev);
	if (!csi) {
		err("csi is NULL");
		return -EINVAL;
	}

	ret = exynos_csis_phy_enable(csi->instance, on);
	if (ret) {
		err("fail to csi%d power on", csi->instance);
		goto p_err;
	}

p_err:
	mdbgd_front("%s(%d, %d)\n", csi, __func__, on, ret);
	return ret;
}

static const struct v4l2_subdev_core_ops core_ops = {
	.init = csi_init,
	.s_power = csi_s_power
};

static int csi_stream_on(struct fimc_is_device_csi *csi)
{
	int ret = 0;
	u32 settle;
	unsigned long __iomem *base_reg;

	BUG_ON(!csi);
	BUG_ON(!csi->sensor_cfg);

	base_reg = csi->base_reg;

	settle = get_hsync_settle(
		csi->sensor_cfg,
		csi->sensor_cfgs,
		csi->image.window.width,
		csi->image.window.height,
		csi->image.framerate);

	info("[CSI:D:%d] settle(%dx%d@%d) = %d\n",
		csi->instance,
		csi->image.window.width,
		csi->image.window.height,
		csi->image.framerate,
		settle);

#if (FIMC_IS_CSI_VERSION == CSI_VERSION_0000_0000)
	s5pcsis_reset(base_reg);
	s5pcsis_set_hsync_settle(base_reg, settle);
	s5pcsis_set_params(base_reg, &csi->image);
	s5pcsis_system_enable(base_reg, true);
	s5pcsis_enable_interrupts(base_reg, &csi->image, true);
#else
	csi_hw_reset(base_reg);
	csi_hw_s_settle(base_reg, settle);
	csi_hw_s_datalane(base_reg, CSI_DATA_LANES_4);
	csi_hw_s_config(base_reg,
		CSI_VIRTUAL_CH_0,
		csi->image.format.pixelformat,
		csi->image.window.width,
		csi->image.window.height);
	csi_hw_s_interrupt(base_reg, true);
	csi_hw_enable(base_reg);
#endif

	return ret;
}

static int csi_stream_off(struct fimc_is_device_csi *csi)
{
	unsigned long __iomem *base_reg;

	BUG_ON(!csi);

	base_reg = csi->base_reg;

#if (FIMC_IS_CSI_VERSION == CSI_VERSION_0000_0000)
	s5pcsis_enable_interrupts(base_reg, &csi->image, false);
	s5pcsis_system_enable(base_reg, false);
#else
	csi_hw_s_interrupt(base_reg, false);
	csi_hw_disable(base_reg);
#endif
	return 0;
}

static int csi_s_stream(struct v4l2_subdev *subdev, int enable)
{
	int ret = 0;
	struct fimc_is_device_csi *csi;

	BUG_ON(!subdev);

	csi = (struct fimc_is_device_csi *)v4l2_get_subdevdata(subdev);
	if (!csi) {
		err("csi is NULL");
		return -EINVAL;
	}

	if (enable) {
		ret = csi_stream_on(csi);
		if (ret) {
			err("csi_stream_on is fail(%d)", ret);
			goto p_err;
		}
	} else {
		ret = csi_stream_off(csi);
		if (ret) {
			err("csi_stream_off is fail(%d)", ret);
			goto p_err;
		}
	}

p_err:
	return 0;
}

static int csi_s_param(struct v4l2_subdev *subdev, struct v4l2_streamparm *param)
{
	int ret = 0;
	struct fimc_is_device_csi *csi;
	struct v4l2_captureparm *cp;
	struct v4l2_fract *tpf;

	BUG_ON(!subdev);
	BUG_ON(!param);

	cp = &param->parm.capture;
	tpf = &cp->timeperframe;

	csi = (struct fimc_is_device_csi *)v4l2_get_subdevdata(subdev);
	if (!csi) {
		err("csi is NULL");
		return -EINVAL;
	}

	csi->image.framerate = tpf->denominator / tpf->numerator;

	mdbgd_front("%s(%d, %d)\n", csi, __func__, csi->image.framerate, ret);
	return ret;
}

static int csi_s_format(struct v4l2_subdev *subdev, struct v4l2_mbus_framefmt *fmt)
{
	int ret = 0;
	struct fimc_is_device_csi *csi;

	BUG_ON(!subdev);
	BUG_ON(!fmt);

	csi = (struct fimc_is_device_csi *)v4l2_get_subdevdata(subdev);
	if (!csi) {
		err("csi is NULL");
		return -EINVAL;
	}

	csi->image.window.offs_h = 0;
	csi->image.window.offs_v = 0;
	csi->image.window.width = fmt->width;
	csi->image.window.height = fmt->height;
	csi->image.window.o_width = fmt->width;
	csi->image.window.o_height = fmt->height;
	csi->image.format.pixelformat = fmt->code;
	csi->image.format.field = fmt->field;

	mdbgd_front("%s(%dx%d, %X)\n", csi, __func__, fmt->width, fmt->height, fmt->code);
	return ret;
}

static const struct v4l2_subdev_video_ops video_ops = {
	.s_stream = csi_s_stream,
	.s_parm = csi_s_param,
	.s_mbus_fmt = csi_s_format
};

static const struct v4l2_subdev_ops subdev_ops = {
	.core = &core_ops,
	.video = &video_ops
};

int fimc_is_csi_probe(void *parent,
	u32 instance)
{
	int ret = 0;
	struct v4l2_subdev *subdev_csi;
	struct fimc_is_device_csi *csi;
	struct fimc_is_device_sensor *device = parent;

	BUG_ON(!device);

	subdev_csi = kzalloc(sizeof(struct v4l2_subdev), GFP_KERNEL);
	if (!subdev_csi) {
		merr("subdev_csi is NULL", device);
		ret = -ENOMEM;
		goto err_alloc_subdev_csi;
	}
	device->subdev_csi = subdev_csi;

	csi = kzalloc(sizeof(struct fimc_is_device_csi), GFP_KERNEL);
	if (!csi) {
		merr("csi is NULL", device);
		ret = -ENOMEM;
		goto err_alloc_csi;
	}

	csi->instance = instance;
	switch (instance) {
	case CSI_ID_A:
		csi->base_reg = (unsigned long *)MIPICSI0_REG_BASE;
		break;
	case CSI_ID_B:
		csi->base_reg = (unsigned long *)MIPICSI1_REG_BASE;
		break;
	case CSI_ID_C:
		csi->base_reg = (unsigned long *)MIPICSI2_REG_BASE;
		break;
	default:
		err("instance is invalid(%d)", instance);
		ret = -EINVAL;
		goto err_invalid_instance;
	}

	v4l2_subdev_init(subdev_csi, &subdev_ops);
	v4l2_set_subdevdata(subdev_csi, csi);
	v4l2_set_subdev_hostdata(subdev_csi, device);
	snprintf(subdev_csi->name, V4L2_SUBDEV_NAME_SIZE, "csi-subdev.%d", instance);
	ret = v4l2_device_register_subdev(&device->v4l2_dev, subdev_csi);
	if (ret) {
		merr("v4l2_device_register_subdev is fail(%d)", device, ret);
		goto err_reg_v4l2_subdev;
	}

	info("[%d][CSI:D] %s(%d)\n", instance, __func__, ret);
	return 0;

err_reg_v4l2_subdev:
err_invalid_instance:
	kfree(csi);

err_alloc_csi:
	kfree(subdev_csi);
	device->subdev_csi = NULL;

err_alloc_subdev_csi:
	err("[FRT:D:%d] %s(%d)\n", instance, __func__, ret);
	return ret;
}
