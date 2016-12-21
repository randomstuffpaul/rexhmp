/*
 *  smdkv2xx_wm1800.c
 *
 *  Copyright (c) 2009 Samsung Electronics Co. Ltd
 *  Author: Jaswinder Singh <jassi.brar@samsung.com>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <mach/regs-clock.h>
#include <asm/mach-types.h>
#include <asm/gpio.h>
#include <plat/gpio-cfg.h>
#include "../codecs/wm1800.h"
#include "i2s.h"

#include "i2s-regs.h"
#include <mach/map.h>

#include "s3c-i2s-v2.h"
#ifdef CONFIG_SND_SAMSUNG_AUX_HDMI
#include <linux/firmware.h>
#include <linux/completion.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <sound/control.h>

#include <mach/regs-pmu.h>
#include "spdif.h"
#endif

#include <asm/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/kthread.h>
#define USE_EAR_JACK_DETECT
#ifdef USE_EAR_JACK_DETECT
#define EAR_DELAY			1
//#define GPIO_EAR_JACK_DET	EXYNOS5260_GPX3(5)
#define GPIO_EAR_JACK_DET	EXYNOS5260_GPX0(1)

static DECLARE_WAIT_QUEUE_HEAD(idle_wait);
static struct task_struct *EarJackTask;
static int earjack_thread(void *data);
#endif

static int smdk_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	unsigned int pll_out;
	int bfs, rfs, psr, ret;
	struct clk *clk_epll;

	switch (params_format(params)) {
		case SNDRV_PCM_FORMAT_U8:
		case SNDRV_PCM_FORMAT_S8:
			bfs = 16;
			break;
		case SNDRV_PCM_FORMAT_U16_LE:
		case SNDRV_PCM_FORMAT_S16_LE:
			bfs = 32;
			break;
		case SNDRV_PCM_FORMAT_U24:
		case SNDRV_PCM_FORMAT_S24:
			bfs = 48;
			break;
		default:
			return -EINVAL;
	}

	/* The Fvco for WM1800 PLLs must fall within [90,100]MHz.
	 * This criterion can't be met if we request PLL output
	 * as {8000x256, 64000x256, 11025x256}Hz.
	 * As a wayout, we rather change rfs to a minimum value that
	 * results in (params_rate(params) * rfs), and itself, acceptable
	 * to both - the CODEC and the CPU.
	 */
	switch (params_rate(params)) {
		case 16000:
		case 22050:
		case 24000:
		case 32000:
		case 44100:
		case 48000:
		case 88200:
		case 96000: rfs = (bfs==48) ? 384 : 256; break;
		case 64000: rfs = 384; break;
		case 8000:
		case 11025: rfs = 512; break;
		default:
			return -EINVAL;
	}
	pll_out = params_rate(params) * rfs;

	
	switch (pll_out) {
		case 4096000:
		case 5644800:
		case 6144000:
		case 8467200:
		case 9216000: psr = 8; break;
		case 8192000:
		case 11289600:
		case 12288000:
		case 16934400:
		case 18432000: psr = 4; break;
		case 22579200:
		case 24576000:
		case 33868800:
		case 36864000: psr = 2; break;
		case 67737600:
		case 73728000: psr = 1; break;
		default:
			printk(KERN_ERR "Not yet supported!\n");
			return -EINVAL;
	}

	clk_epll = clk_get(NULL, "fout_epll");
	
	if(IS_ERR(clk_epll)) {
		printk(KERN_ERR "failed to get fount_epll\n");
		return -EBUSY;
	}
	clk_set_rate(clk_epll, pll_out * psr);
	/* Set the Codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai,
			SND_SOC_DAIFMT_I2S|SND_SOC_DAIFMT_NB_NF|SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;


	/* Set the AP BFS */
	ret = snd_soc_dai_set_clkdiv(cpu_dai,  SAMSUNG_I2S_DIV_BCLK, bfs);

	if (ret < 0) {
		printk(KERN_ERR "AP bfs setting error\n");
		return ret;
	}

  	ret = snd_soc_dai_set_sysclk(cpu_dai, SAMSUNG_I2S_OPCLK,
                                 0, MOD_OPCLK_PCLK);
      if (ret < 0)
        return ret;

#if 1
	ret = snd_soc_dai_set_sysclk(cpu_dai, SAMSUNG_I2S_CDCLK,
								rfs, SND_SOC_CLOCK_OUT);
	if (ret < 0) {
		printk(KERN_ERR "AP set_sysclk setting error\n");
		return ret;
	}
#endif
#if 1
	ret = snd_soc_dai_set_sysclk(cpu_dai, SAMSUNG_I2S_RCLKSRC_1,
								params_rate(params), SND_SOC_CLOCK_OUT);
	if (ret < 0) {
		printk(KERN_ERR "AP set_sysclk setting error\n");
		return ret;
	}
#endif
	ret = snd_soc_dai_set_clkdiv(codec_dai,
								WM1800_SYSCLKSEL,0x0);
        
	clk_put(clk_epll);
	return 0;
}

#ifdef CONFIG_SND_SAMSUNG_AUX_HDMI
/*
 * MV HDMI I2S DAI operations.
 */

static int smdk_hdmi_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	
	unsigned int pll_out;
	int bfs, rfs, psr, ret;
	struct clk *clk_epll;
	switch (params_format(params)) {
		case SNDRV_PCM_FORMAT_U8:
		case SNDRV_PCM_FORMAT_S8:
			bfs = 16;
			break;
		case SNDRV_PCM_FORMAT_U16_LE:
		case SNDRV_PCM_FORMAT_S16_LE:
			bfs = 32;
			break;
		case SNDRV_PCM_FORMAT_U24:
		case SNDRV_PCM_FORMAT_S24:
			bfs = 48;
			break;
		default:
			return -EINVAL;
	}

	/* The Fvco for WM1800 PLLs must fall within [90,100]MHz.
	 * This criterion can't be met if we request PLL output
	 * as {8000x256, 64000x256, 11025x256}Hz.
	 * As a wayout, we rather change rfs to a minimum value that
	 * results in (params_rate(params) * rfs), and itself, acceptable
	 * to both - the CODEC and the CPU.
	 */
	switch (params_rate(params)) {
		case 16000:
		case 22050:
		case 24000:
		case 32000:
		case 44100:
		case 48000:
		case 88200:
		case 96000: rfs = (bfs==48) ? 384 : 256; break;
		case 64000: rfs = 384; break;
		case 8000:
		case 11025: rfs = 512; break;
		default:
			return -EINVAL;
	}
	pll_out = params_rate(params) * rfs;

	
	switch (pll_out) {
		case 4096000:
		case 5644800:
		case 6144000:
		case 8467200:
		case 9216000: psr = 8; break;
		case 8192000:
		case 11289600:
		case 12288000:
		case 16934400:
		case 18432000: psr = 4; break;
		case 22579200:
		case 24576000:
		case 33868800:
		case 36864000: psr = 2; break;
		case 67737600:
		case 73728000: psr = 1; break;
		default:
			printk(KERN_ERR "Not yet supported!\n");
			return -EINVAL;
	}

	clk_epll = clk_get(NULL, "fout_epll");
	
	if(IS_ERR(clk_epll)) {
		printk(KERN_ERR "failed to get fount_epll\n");
		return -EBUSY;
	}
	clk_set_rate(clk_epll, pll_out * psr);
	
	/* Set CPU DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S
					| SND_SOC_DAIFMT_NB_NF
					| SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_sysclk(cpu_dai, SAMSUNG_I2S_CDCLK,
					0, SND_SOC_CLOCK_OUT);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_sysclk(cpu_dai, SAMSUNG_I2S_OPCLK,
					0, MOD_OPCLK_PCLK);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_sysclk(cpu_dai, SAMSUNG_I2S_RCLKSRC_1, 0,SND_SOC_CLOCK_OUT);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(cpu_dai, SAMSUNG_I2S_DIV_BCLK, bfs);
	if (ret < 0)
		return ret;

	return 0;
}

static struct snd_soc_ops smdk_hdmi_ops = {
	.hw_params = smdk_hdmi_hw_params,
};
#endif


/* SMDK Playback widgets */
static const struct snd_soc_dapm_widget wm1800_dapm_widgets_pbk[] = {
	SND_SOC_DAPM_HP("SPK", NULL),
	SND_SOC_DAPM_HP("HP", NULL),
};

/* SMDK Capture widgets */
static const struct snd_soc_dapm_widget wm1800_dapm_widgets_cpt[] = {
	SND_SOC_DAPM_LINE("LineIn", NULL),
};

/* SMDK-PAIFTX connections */
static const struct snd_soc_dapm_route audio_map_tx[] = {
	/* MicIn feeds LINPUT1/2 */
	{"LINPUT3", NULL, "LineIn"},
	{"RINPUT3", NULL, "LineIn"},
};

/* SMDK-PAIFRX connections */
static const struct snd_soc_dapm_route audio_map_rx[] = {
	{"SPK", NULL, "SPK_LP"},
	{"SPK", NULL, "SPK_LN"},
	{"SPK", NULL, "SPK_RP"},
	{"SPK", NULL, "SPK_RN"},
	{"HP", NULL, "HP_L"},
	{"HP", NULL, "HP_R"},
};

static int smdk_wm1800_init_paiftx(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;

	/* Add smdk specific Capture widgets */
	snd_soc_dapm_new_controls(dapm, wm1800_dapm_widgets_cpt,
				  ARRAY_SIZE(wm1800_dapm_widgets_cpt));

	/* Set up PAIFTX audio path */
	snd_soc_dapm_add_routes(dapm, audio_map_tx, ARRAY_SIZE(audio_map_tx));

	/* Enabling the microphone requires the fitting of a 0R
	 * resistor to connect the line from the microphone jack.
	 */
	snd_soc_dapm_disable_pin(dapm, "LineIn");

	/* signal a DAPM event */
	snd_soc_dapm_sync(dapm);

	return 0;
}

static int smdk_wm1800_init_paifrx(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;
	static int kthread_ok = 0;

	/* Add smdk specific Playback widgets */

	snd_soc_dapm_new_controls(dapm, wm1800_dapm_widgets_pbk,
				  ARRAY_SIZE(wm1800_dapm_widgets_pbk));

	/* Set up PAIFRX audio path */
	snd_soc_dapm_add_routes(dapm, audio_map_rx, ARRAY_SIZE(audio_map_rx));

	/* signal a DAPM event */
	snd_soc_dapm_sync(dapm);
#ifdef USE_EAR_JACK_DETECT
	if(!kthread_ok){
		EarJackTask = kthread_run(earjack_thread, codec, "ear_detect");
		if(!IS_ERR(EarJackTask)) {
			printk(" Ear Jack Poll Task Started\n");
		}
		kthread_ok = 1;
	}
#endif

	return 0;
}

static struct snd_soc_ops smdk_ops = {
	.hw_params = smdk_hw_params,
};

static struct snd_soc_dai_link smdk_dai[] = {
	{		
		.name = "WM1800 AIF1",
		.stream_name = "Pri_Dai",
		.cpu_dai_name = "samsung-i2s.0",
		.codec_dai_name = "wm1800-aif1",
		.platform_name = "samsung-audio",
		.codec_name = "wm1800-codec.1-001a",
		.init = smdk_wm1800_init_paifrx,
		.ops = &smdk_ops,
	},{			
		.name = "Sec_FIFO TX",
		.stream_name = "Sec_Dai",
		.cpu_dai_name = "samsung-i2s.0",
		.codec_dai_name = "wm1800-aif2",
#ifdef CONFIG_SND_SAMSUNG_USE_IDMA
	    .platform_name = "samsung-idma",
#else
		.platform_name = "samsung-audio",
#endif
		.codec_name = "wm1800-codec.1-001a",
		.init = smdk_wm1800_init_paiftx,
		.ops = &smdk_ops,   	
#ifdef CONFIG_SND_SAMSUNG_AUX_HDMI
	}, { /* Aux DAI i/f */
		.name = "HDMI",
		.stream_name = "i2s1",
		.cpu_dai_name = "samsung-i2s.1",
		.codec_dai_name = "dummy-aif1",
		.platform_name = "samsung-audio",
		.codec_name = "dummy-codec",
		.ops = &smdk_hdmi_ops,
#endif
	}
	    	
};

#ifdef USE_EAR_JACK_DETECT
static int earjack_thread(void *data)
{
	int ret;
	u8 value, pvalue = 0xff;
	struct snd_soc_codec *codec = (struct snd_soc_codec *)data;
	
    s3c_gpio_cfgpin(GPIO_EAR_JACK_DET, S3C_GPIO_SFN(0xF));
    s3c_gpio_setpull(GPIO_EAR_JACK_DET, S3C_GPIO_PULL_UP);

	do {
		ret = interruptible_sleep_on_timeout(&idle_wait, HZ*EAR_DELAY);

		if(!ret) {
			value = gpio_get_value(GPIO_EAR_JACK_DET);
			if(value != pvalue) {
				printk("\t#Ear Jack Detect : value=%d\n", value);
				if(value)
				{
					wm1800_set_outpath(codec, WM1800_OUT_SPK);
					printk("\tspeak value=%d\n", value);
				}
				else
				{
					wm1800_set_outpath(codec, WM1800_OUT_EAR);
					printk("\theadphone value=%d\n", value);
				}
				pvalue = value;
			}
		}
	} while(!kthread_should_stop());
	return 0;
}
#endif

static struct snd_soc_card smdk = {
	.name = "SMDK-I2S",
	.owner = THIS_MODULE,
	.dai_link = smdk_dai,
	.num_links = ARRAY_SIZE(smdk_dai),
};

static struct platform_device *smdk_snd_device;

static int __init smdk_audio_init(void)
{
	int ret;

	smdk_snd_device = platform_device_alloc("soc-audio", -1);
	if (!smdk_snd_device) {
		printk(KERN_ERR "%s: failed to audio device alloc\n", __func__);
		return -ENOMEM;
	}
	     

	platform_set_drvdata(smdk_snd_device, &smdk);

	ret = platform_device_add(smdk_snd_device);
	if (ret) {
		platform_device_put(smdk_snd_device);
	}

	return ret;
}

static void __exit smdk_audio_exit(void)
{
	platform_device_unregister(smdk_snd_device);
}

module_init(smdk_audio_init);
module_exit(smdk_audio_exit);

MODULE_AUTHOR("Jaswinder Singh, jassi.brar@samsung.com");
MODULE_DESCRIPTION("ALSA SoC SMDK64XX WM1800");
MODULE_LICENSE("GPL");
