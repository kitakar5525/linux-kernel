/*
 *  bq24232_charger.h - BQ24232 Charger
 *
 *  Copyright (C) 2015 Intel Corporation
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under  the terms of the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __LINUX_POWER_BQ24232_CHARGER_H__
#define __LINUX_POWER_BQ24232_CHARGER_H__

#include <linux/power_supply.h>
#include <linux/types.h>

#define BQ24232_CHRGR_DEV_NAME	"bq24232_charger"

#define BQ24232_CHARGE_CURRENT_LOW	100
#define BQ24232_CHARGE_CURRENT_HIGH	400

#define BAT_TEMP_MONITOR_DELAY	(60 * HZ)

#ifdef CONFIG_BQ24232_CHARGER
extern int bq24232_get_charger_status(void);
extern void bq24232_set_charging_status(bool chg_stat);
#else
int bq24232_get_charger_status(void)
{
	return 0;
}

void bq24232_set_charging_status(bool chg_stat)
{
	return;
}
#endif
int bq24232_assert_ce_n(bool val);

enum bq24232_chrgrate_temp_limit {
	BQ24232_NORM_CHARGE_TEMP_LOW,
	BQ24232_BOOST_CHARGE_TEMP_LOW,
	BQ24232_BOOST_CHARGE_TEMP_HIGH,
	BQ24232_NORM_CHARGE_TEMP_HIGH
};

/**
 * struct gpio_charger_platform_data - platform_data for gpio_charger devices
 * @name:		Name for the chargers power_supply device
 * @supplied_to:	Devices supplied by this charger
 * @num_supplicants:	Number of devices supplied by this charger
 * @bat_temp_profile:	Temperature ranges in wich normal/boost charge is allowed
 * @bat_hv_temp_profile:	Charge rates temperature ranges for higher battery voltage
 * @pgood_gpio:	GPIO which is used to indicate the chargers status
 * @chg_rate_temp_gpio:	GPIO which is used to select the charge rate
 * @charger_ce_n_gpio:	GPIO to assert low to charge enable, high to disable it
 * @enable_charging:	Function callback to activate the CE_N signal on the charger
 */
struct bq24232_plat_data {
	const char *name;

	char **supplied_to;
	size_t num_supplicants;
	int *bat_temp_profile;
	int *bat_hv_temp_profile;
	int bat_hv_threshold;
	int pgood_gpio;
	int chg_rate_temp_gpio;
	int charger_ce_n_gpio;

	int (*enable_charging) (bool val);
	int (*enable_vbus) (bool val);
	int (*get_charging_status) (bool *charging_status);

	int bat_temp_offset;
	bool wc_direct_support;
};

#endif
