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

static char *bq24232_supplied_to[] = {
				"max17047_battery",
};

static struct bq24232_plat_data bq24232_pdata;

/*
 * Battery temperature limits in 0.1 °C
 * BQ24232_NORM_CHARGE_TEMP_LOW, BQ24232_NORM_CHARGE_TEMP_HIGH and
 * BQ24232_BOOST_CHARGE_TEMP_HIHG set to be outside the range allowed
 * by charger hw component [0°C, 50°C]. Relying on hw to stop charging
 * in case of over/underheat
 */
static int bq24232_bat_temp_profile[] = {
		-200,	/* BQ24232_NORM_CHARGE_TEMP_LOW */
		110,	/* BQ24232_BOOST_CHARGE_TEMP_LOW */
		700,	/* BQ24232_BOOST_CHARGE_TEMP_HIHG */
		700	/* BQ24232_NORM_CHARGE_TEMP_HIGH */
};

/*
 * Battery temperature limits in 0.1 °C
 * for higher battery voltage
 * BQ24232_NORM_CHARGE_TEMP_LOW and BQ24232_NORM_CHARGE_TEMP_HIGH
 * set to be outside the range allowed by charger hw component [0°C, 50°C].
 * Relying on hw to stop charging in case of over/underheat
 */
static int bq24232_bat_highvolt_temp_profile[] = {
		-200,	/* BQ24232_NORM_CHARGE_TEMP_LOW */
		110,	/* BQ24232_BOOST_CHARGE_TEMP_LOW */
		440,	/* BQ24232_BOOST_CHARGE_TEMP_HIHG */
		440	/* BQ24232_NORM_CHARGE_TEMP_HIGH */
};

/*
 * Battery temperature offset between sw measured value and
 * battery NTC. Expressed in 0.1 °C
 */
#define BQ24232_BATTERY_TEMP_OFFSET		30

#define BQ24232_BAT_HIGH_VOLT_THRESHOLD		4100000	/* in uV */

void *bq24232_charger_platform_data(void *info)
{
	bq24232_pdata.name = BQ24232_CHRGR_DEV_NAME;

	bq24232_pdata.chg_rate_temp_gpio = get_gpio_by_name("chg_rate_temp");
	bq24232_pdata.pgood_gpio = get_gpio_by_name("chg_pgood");
	bq24232_pdata.charger_ce_n_gpio = get_gpio_by_name("charger_ce_n");
	bq24232_pdata.enable_charging = bq24232_assert_ce_n;
#if CONFIG_PMIC_CCSM
	if (bq24232_pdata.charger_ce_n_gpio < 0)
		bq24232_pdata.enable_charging = pmic_enable_charging;
	bq24232_pdata.get_charging_status = pmic_get_ext_charging_status;
	bq24232_pdata.enable_vbus = pmic_enable_vbus;
#endif
	bq24232_pdata.bat_temp_profile = bq24232_bat_temp_profile;
	bq24232_pdata.bat_hv_temp_profile = bq24232_bat_highvolt_temp_profile;
	bq24232_pdata.bat_hv_threshold = BQ24232_BAT_HIGH_VOLT_THRESHOLD;
	bq24232_pdata.supplied_to = bq24232_supplied_to;
	bq24232_pdata.num_supplicants = ARRAY_SIZE(bq24232_supplied_to);
	if  (INTEL_MID_BOARD(2, PHONE, MRFL, RBY, PRO) ||
			INTEL_MID_BOARD(2, PHONE, MRFL, RBY, ENG) ||
			INTEL_MID_BOARD(2, PHONE, MRFL, MVN, PRO) ||
			INTEL_MID_BOARD(2, PHONE, MRFL, MVN, ENG))
		bq24232_pdata.wc_direct_support = true;
	else
		bq24232_pdata.wc_direct_support = false;

	bq24232_pdata.bat_temp_offset = BQ24232_BATTERY_TEMP_OFFSET;

	return &bq24232_pdata;
}
