/*
 *  ASoc DPCM Machine driver for Intel Marvin MID platform
 *
 *  Copyright (C) 2014-2015 Intel Corp
 *  Author: Michael Soares <michaelx.soares@intel.com>
 *  Author: Mattijs Korpershoek <mattijsx.korpershoek@intel.com>
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <asm/intel_scu_pmic.h>
#include <asm/intel_sst_mrfld.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>

#ifdef CONFIG_PM_SLEEP
static int snd_merr_dpcm_prepare(struct device *dev)
{
	pr_debug("In %s device name\n", __func__);
	snd_soc_suspend(dev);
	return 0;
}

static void snd_merr_dpcm_complete(struct device *dev)
{
	pr_debug("In %s\n", __func__);
	snd_soc_resume(dev);
	return;
}

static int snd_merr_dpcm_poweroff(struct device *dev)
{
	pr_debug("In %s\n", __func__);
	snd_soc_poweroff(dev);
	return 0;
}
#else
#define snd_merr_dpcm_prepare NULL
#define snd_merr_dpcm_complete NULL
#define snd_merr_dpcm_poweroff NULL
#endif

const struct dev_pm_ops snd_merr_dpcm_mc_pm_ops = {
	.prepare = snd_merr_dpcm_prepare,
	.complete = snd_merr_dpcm_complete,
	.poweroff = snd_merr_dpcm_poweroff,
};

static unsigned int rates_48000[] = {
	48000,
};

static struct snd_pcm_hw_constraint_list constraints_48000 = {
	.count = ARRAY_SIZE(rates_48000),
	.list  = rates_48000,
};

static int merr_mvn_startup(struct snd_pcm_substream *substream)
{
	return snd_pcm_hw_constraint_list(substream->runtime, 0,
			SNDRV_PCM_HW_PARAM_RATE,
			&constraints_48000);
}

static struct snd_soc_ops merr_mvn_ops = {
		.startup = merr_mvn_startup,
};

struct snd_soc_dai_link merr_msic_dailink[] = {
	[MERR_DPCM_AUDIO] = {
		.name = "Media Audio Port",
		.stream_name = "Marvin Audio",
		.cpu_dai_name = "Headset-cpu-dai",
		.codec_name = "snd-soc-dummy",
		.codec_dai_name = "snd-soc-dummy-dai",
		.platform_name = "sst-platform",
		.ignore_suspend = 1,
		.dynamic = 1,
		.ops = &merr_mvn_ops,
	},
	/* back ends */
	{
		.name = "SSP2-Codec",
		.be_id = 1,
		.cpu_dai_name = "ssp2-codec",
		.platform_name = "sst-platform",
		.no_pcm = 1,
		.codec_dai_name = "snd-soc-dummy-dai",
		.codec_name = "snd-soc-dummy",
		.ignore_suspend = 1,
	},

};

static const struct snd_soc_dapm_route map[] = {
	{ "Dummy Playback", NULL, "codec_out0"  },
	{ "Dummy Playback", NULL, "codec_out1"  },
	{ "codec_in0", NULL, "Dummy Capture" },
	{ "codec_in1", NULL, "Dummy Capture" },
};

/* SoC card */
static struct snd_soc_card snd_soc_card_merr = {
	.name = "marvin-audio",
	.dai_link = merr_msic_dailink,
	.num_links = ARRAY_SIZE(merr_msic_dailink),
	.dapm_routes = map,
	.num_dapm_routes = ARRAY_SIZE(map),
};

static int snd_merr_dpcm_probe(struct platform_device *pdev)
{
	int ret_val = 0;
	pr_debug("%s enter\n", __func__);

	/* register the soc card */
	snd_soc_card_merr.dev = &pdev->dev;
	ret_val = snd_soc_register_card(&snd_soc_card_merr);
	if (ret_val) {
		pr_err("snd_soc_register_card failed %d\n", ret_val);
		return ret_val;
	}
	platform_set_drvdata(pdev, &snd_soc_card_merr);
	pr_info("%s successful\n", __func__);
	return ret_val;
}

static int snd_merr_dpcm_remove(struct platform_device *pdev)
{
	struct snd_soc_card *soc_card = platform_get_drvdata(pdev);
	pr_err("snd_merr_dpcm_remove");
	snd_soc_unregister_card(soc_card);
	platform_set_drvdata(pdev, NULL);
	return 0;
}

static struct platform_driver snd_merr_dpcm_drv = {
	.driver = {
			.owner = THIS_MODULE,
			.name = "merr_dpcm_mvn",
			.pm = &snd_merr_dpcm_mc_pm_ops,
	},
	.probe = snd_merr_dpcm_probe,
	.remove = snd_merr_dpcm_remove,
};
module_platform_driver(snd_merr_dpcm_drv);

MODULE_DESCRIPTION("ASoC Intel(R) Marvin MID Machine driver");
MODULE_AUTHOR("Michael Soares <michaelx.soares@intel.com>");
MODULE_AUTHOR("Mattijs Korpershoek <mattijsx.korpershoek@intel.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:merr_dpcm_mvn");
