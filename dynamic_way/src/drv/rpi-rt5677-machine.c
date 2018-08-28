/*
 * ASoC driver for PROTO AudioCODEC (with a RT5677)
 * connected to a Raspberry Pi
 *
 * Author:      Steve Liu, <steveliu121@163.com>
 *	      Copyright 2018
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/platform_device.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/jack.h>


#define RT5677_SCLK_S_MCLK	0

static const unsigned int rt5677_rates_24576000[] = {
	8000, 32000, 48000, 96000,
};

static struct snd_pcm_hw_constraint_list rt5677_constraints_24576000 = {
	.list = rt5677_rates_24576000,
	.count = ARRAY_SIZE(rt5677_rates_24576000),
};

static int snd_rpi_proto_startup(struct snd_pcm_substream *substream)
{
	/* Setup constraints, because there is a 24.576 MHz XTAL on the board */
	snd_pcm_hw_constraint_list(substream->runtime, 0,
				SNDRV_PCM_HW_PARAM_RATE,
				&rt5677_constraints_24576000);
	return 0;
}

static int snd_rpi_proto_hw_params(struct snd_pcm_substream *substream,
				       struct snd_pcm_hw_params *params)
{
	int ret = 0;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	int sysclk = 24576000; /* This is fixed on this board */

	/* Set codec_dai fmt*/
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S
				| SND_SOC_DAIFMT_NB_NF
				| SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0) {
		dev_err(codec->dev,
				"Failed to set RT5677 DAI fmt\n");
		return ret;
	}

	/* Set cpu_dai fmt*/
	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S
				| SND_SOC_DAIFMT_NB_NF
				| SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0) {
		dev_err(codec->dev,
				"Failed to set RT5677 DAI fmt\n");
		return ret;
	}

	/* Set cpu_dai bclk */
	ret = snd_soc_dai_set_bclk_ratio(cpu_dai,32*2);
	if (ret < 0){
		dev_err(codec->dev,
				"Failed to set BCLK ratio %d\n", ret);
		return ret;
	}

	/* Set codec_dai sysclk */
	ret = snd_soc_dai_set_sysclk(codec_dai, RT5677_SCLK_S_MCLK,
			sysclk, 0);/*actually, only the first two params works*/
	if (ret < 0) {
		dev_err(codec->dev,
				"Failed to set RT5677 SYSCLK: %d\n", ret);
		return ret;
	}

	return 0;
}

/* machine stream operations */
static struct snd_soc_ops snd_rpi_proto_ops = {
	.startup = snd_rpi_proto_startup,
	.hw_params = snd_rpi_proto_hw_params,
};

static struct snd_soc_dai_link snd_rpi_proto_dai[] = {
{
	.name		= "RT5677 CARD AIF1 ",
	.stream_name	= "RT5677 HiFi",
	.cpu_dai_name	= "bcm2835-i2s.0",
	.codec_dai_name	= "rt5677-aif1",
	.platform_name	= "bcm2835-i2s.0",
	.codec_name	= "rt5677.1-002d",
	.dai_fmt	= SND_SOC_DAIFMT_I2S
				| SND_SOC_DAIFMT_NB_NF
				| SND_SOC_DAIFMT_CBM_CFM,
	.ops		= &snd_rpi_proto_ops,
},
};

/* audio machine driver */
static struct snd_soc_card snd_rpi_proto = {
	.name		= "snd_rpi_rt5677_machine",
	.owner		= THIS_MODULE,
	.dai_link	= snd_rpi_proto_dai,
	.num_links	= ARRAY_SIZE(snd_rpi_proto_dai),
};

static int snd_rpi_proto_probe(struct platform_device *pdev)
{
	int ret = 0;

	printk("~~~~~~~~~~~rt5677 machine driver match\n");


	snd_rpi_proto.dev = &pdev->dev;

	if (pdev->dev.of_node) {
		struct device_node *i2s_node;
		struct snd_soc_dai_link *dai = &snd_rpi_proto_dai[0];
		i2s_node = of_parse_phandle(pdev->dev.of_node,
				            "i2s-controller", 0);

		if (i2s_node) {
			dai->cpu_dai_name = NULL;
			dai->cpu_of_node = i2s_node;
			dai->platform_name = NULL;
			dai->platform_of_node = i2s_node;
		}
	}

	ret = snd_soc_register_card(&snd_rpi_proto);
	if (ret && ret != -EPROBE_DEFER)
		dev_err(&pdev->dev,
				"snd_soc_register_card() failed: %d\n", ret);

	return ret;
}


static int snd_rpi_proto_remove(struct platform_device *pdev)
{
	return snd_soc_unregister_card(&snd_rpi_proto);
}

static const struct of_device_id snd_rpi_proto_of_match[] = {
	{ .compatible = "rpi,rpi-rt5677-machine", },
	{},
};
MODULE_DEVICE_TABLE(of, snd_rpi_proto_of_match);

static struct platform_driver snd_rpi_proto_driver = {
	.driver = {
		.name   = "snd-rpi-rt5677-machine",
		.owner  = THIS_MODULE,
		.of_match_table = snd_rpi_proto_of_match,
	},
	.probe	  = snd_rpi_proto_probe,
	.remove	 = snd_rpi_proto_remove,
};

module_platform_driver(snd_rpi_proto_driver);

MODULE_AUTHOR("Steve Liu");
MODULE_DESCRIPTION("ASoC Driver for Raspberry Pi connected to PROTO board (RT5677)");
MODULE_LICENSE("GPL");
