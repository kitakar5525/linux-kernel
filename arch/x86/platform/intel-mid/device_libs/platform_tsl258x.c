/*
 * platform_tsl258x.c: TAOS TSL258x light sensor platform data initilization file
 *
 * (C) Copyright 2015 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#include <linux/input.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/i2c.h>
#include <linux/tsl258x_als.h>
#include <asm/intel-mid.h>
#include <linux/tsl258x_als.h>
#include "platform_tsl258x.h"

void *tsl258x_als_platform_data(void *info)
{
	static struct tsl258x_platform_data tsl258x_platform_data;

	tsl258x_platform_data.als_def_als_time = TSL258X_ALS_DEF_TIME;
	tsl258x_platform_data.als_def_gain = TSL258X_ALS_DEF_GAIN;
	tsl258x_platform_data.als_def_gain_trim = TSL258X_ALS_DEF_GAIN_TRIM;
	tsl258x_platform_data.als_def_cal_target = TSL258X_ALS_DEF_CAL_TARGET;

	return &tsl258x_platform_data;
}
