/*
 * drv2605 vibrator Controller Driver
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

#ifndef _DRV2605_VIBRA_H_
#define _DRV2605_VIBRA_H_

struct drv2605_platform_data {
	int gpio_en;
	int gpio_pwm;
};

#endif /* _DRV2605_VIBRA_H_ */
