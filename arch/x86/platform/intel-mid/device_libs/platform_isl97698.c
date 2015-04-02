/*
 * platform_isl97698.c: isl97698 platform data initilization file
 *
 * (C) Copyright 2015 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include <linux/gpio.h>
#include <linux/lnw_gpio.h>
#include <asm/intel-mid.h>
#include <linux/isl97698.h>

void *isl97698_brightness_platform_data(void *info)
{
	static struct isl97698_platform_data isl97698_pdata;

	isl97698_pdata.bias_en = get_gpio_by_name("disp0_bias_en");
	return &isl97698_pdata;
}
