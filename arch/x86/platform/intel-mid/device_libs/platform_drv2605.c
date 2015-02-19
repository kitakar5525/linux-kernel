/*
 * platform_drv2605.c: TI drv2605 vibrator platform data initilization file
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
#include <linux/drv2605_vibra.h>
#include <asm/intel-mid.h>
#include "platform_drv2605.h"

void *drv2605_platform_data(void *info)
{
	static struct drv2605_platform_data drv2605_platform_data = {
		.gpio_en = -1,
		.gpio_pwm = -1,
	};

		drv2605_platform_data.gpio_en = get_gpio_by_name("haptics_en");
		drv2605_platform_data.gpio_pwm = get_gpio_by_name("haptics_pwm");
	return &drv2605_platform_data;
}
