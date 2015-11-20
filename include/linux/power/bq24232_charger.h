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

/**
 * struct gpio_charger_platform_data - platform_data for gpio_charger devices
 * @name:		Name for the chargers power_supply device
 * @type:		Type of the charger
 * @pgood_gpio:	GPIO which is used to indicate the chargers status
 * @chg_rate_temp_gpio:	GPIO which is used to select the charge rate
 * @enable_charging:	Function callback to activate the CE_N signal on the charger
 */
struct bq24232_plat_data {
	const char *name;

	char **supplied_to;
	size_t num_supplicants;

	int pgood_gpio;
	int chg_rate_temp_gpio;

	int (*enable_charging) (bool val);
};

#endif
