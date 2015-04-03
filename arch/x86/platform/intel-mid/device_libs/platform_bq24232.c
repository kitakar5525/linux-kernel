/*
 * platform_bq24232.c: platform data for bq24232 driver
 *
 * Copyright (c) 2015 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#include <linux/gpio.h>
#include <linux/lnw_gpio.h>
#include <linux/power_supply.h>
#include <linux/power/bq24232_charger.h>
#include <asm/intel-mid.h>
#include <asm/pmic_pdata.h>
#include "platform_bq24232.h"

char *bq24232_supplied_to[] = {
				"max17047_battery",
};

static struct bq24232_plat_data bq24232_pdata;

void *bq24232_charger_platform_data(void *info)
{
	bq24232_pdata.name = BQ24232_CHRGR_DEV_NAME;

	bq24232_pdata.chg_rate_temp_gpio = get_gpio_by_name("chg_rate_temp");
	bq24232_pdata.pgood_gpio = get_gpio_by_name("chg_pgood");
#if CONFIG_PMIC_CCSM
	bq24232_pdata.enable_charging = pmic_enable_charging;
#endif

	bq24232_pdata.supplied_to = bq24232_supplied_to;
	bq24232_pdata.num_supplicants = ARRAY_SIZE(bq24232_supplied_to);

	return &bq24232_pdata;
}
