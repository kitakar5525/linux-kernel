/*
 * platform_lsm6ds3.c: lsm6ds3 platform data initilization file
 *
 * (C) Copyright 2015 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#include <linux/gpio.h>
#include <linux/lnw_gpio.h>
#include <asm/intel-mid.h>
#include <linux/platform_data/st_lsm6ds3_pdata.h>
#include "platform_lsm6ds3.h"

void *lsm6ds3_platform_data(void *info)
{
	static struct st_lsm6ds3_platform_data lsm6ds3_pdata;

	lsm6ds3_pdata.gpio_int1 = get_gpio_by_name("accel_int1");	/* ACCEL_INT_1 <-> GPIO46 */

	return &lsm6ds3_pdata;
}
