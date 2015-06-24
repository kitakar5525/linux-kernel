/*
 * isl97698.c - Brightness Driver
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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/sysfs.h>
#include <linux/gpio.h>
#include <linux/isl97698.h>

#define ISL97698_REG_LED_H8		0x00
#define ISL97698_REG_LED_L3		0x01
#define ISL97698_REG_CONF		0x02
#define ISL97698_REG_PFM		0x03
#define ISL97698_REG_BOOST		0x04
#define ISL97698_REG_STATUS		0x10

#define ISL97698_ST_FAULT_MASK	0xF0
#define ISL97698_ST_BMOD_MASK	0x08
#define ISL97698_ST_LEDON_MASK	0x04
#define ISL97698_ST_CH1OK_MASK	0x02
#define ISL97698_ST_CH0OK_MASK	0x01

#define ISL97698_PEAK_CUR_MASK		0xF0
#define ISL97698_PEAK_CUR_N			0x00
#define ISL97698_AVERG_CUR_MASK		0X0F
#define ISL97698_AVERG_CUR_N		0x00

#define ISL97698_BST_RATE_MASK		0xC0
#define ISL97698_BST_RATE_SLOWEST	0x00
#define ISL97698_BST_RATE_SLOW		0x40
#define ISL97698_BST_RATE_FAST		0x80
#define ISL97698_BST_RATE_FASTEST	0xC0

#define ISL97698_BST_LOAD_MASK		0x20
#define ISL97698_BST_SYNC_PULSE		0x20
#define ISL97698_BST_PURE_PFM		0x00

#define ISL97698_BST_ABS_MASK		0x10
#define ISL97698_BST_ABS_ENABLE		0x10
#define ISL97698_BST_ABS_DISABLE	0x00

#define ISL97698_BST_FREQ_464KHZ	0x00
#define ISL97698_BST_FREQ_486KHZ	0x01
#define ISL97698_BST_FREQ_510KHZ	0x02
#define ISL97698_BST_FREQ_537KHZ	0x03
#define ISL97698_BST_FREQ_567KHZ	0x04
#define ISL97698_BST_FREQ_600KHZ	0x05
#define ISL97698_BST_FREQ_638KHZ	0x06
#define ISL97698_BST_FREQ_680KHZ	0x07
#define ISL97698_BST_FREQ_729KHZ	0x08
#define ISL97698_BST_FREQ_785KHZ	0x09
#define ISL97698_BST_FREQ_850KHZ	0x0A
#define ISL97698_BST_FREQ_927KHZ	0x0B
#define ISL97698_BST_FREQ_1020KHZ	0x0C
#define ISL97698_BST_FREQ_1133KHZ	0x0D
#define ISL97698_BST_FREQ_1275KHZ	0x0E
#define ISL97698_BST_FREQ_1457KHZ	0x0F

/* Brightness max value, limit to 50% to avoid current overload */
#define ISL_BRIGHTNESS_VAL_MAX		(0xFF>>1)

/* Brightness level: Min(0), Max (100), Defalut(50) */
#define ISL_BRIGHTNESS_LEVEL_MIN	0
#define ISL_BRIGHTNESS_LEVEL_MAX	100
#define ISL_BRIGHTNESS_LEVEL_DEF	50

/* Default config:  PWMI x I2C, Enable Fault(OPCP, OTP), Enable VSC,
 * Use 16V OVP, Disable dither, enable channel0, disable channel 1.
 */
#define ISL_CONF_DEF		0xB5

/* PFM peak current: 296mA, Average inductor current to enter PFM mode: 93mA. */
#define ISL_PFM_MODE_DEF	0x83

#define ISL_BST_MODE_DEF	(ISL97698_BST_RATE_FASTEST | \
							ISL97698_BST_SYNC_PULSE | \
							ISL97698_BST_ABS_ENABLE | \
							ISL97698_BST_FREQ_850KHZ)

#define ISL_CHIP_DISABLE	0
#define ISL_CHIP_ENABLE		1

struct isl97698_st {
	struct mutex isl_mutex;
	struct i2c_client *client;
	int bias_en;
	int enable;
	int brightness;
};

static int isl97698_i2c_write(struct i2c_client *client, u8 addr, u8 val)
{
	u8 txbuf[2];
	struct i2c_msg msg = {
		.addr = client->addr,
		.flags = 0,
		.len = 2,
		.buf = txbuf,
	};
	txbuf[0] = addr;
	txbuf[1] = val;
	return i2c_transfer(client->adapter, &msg, 1);
}

static int isl97698_i2c_read(struct i2c_client *client, u8 addr, u8 *val)
{
	struct i2c_msg msgs[2] = {
		{
			.addr = client->addr,
			.flags = I2C_M_NOSTART,
			.len = 1,
			.buf = &addr,
		},
		{
			.addr = client->addr,
			.flags = I2C_M_RD,
			.len = 1,
			.buf = val,
		}
	};

	return i2c_transfer(client->adapter, msgs, 2);
}

static int brightness_set_value(struct isl97698_st *isl, int level)
{
	int ret;
	u8 regval;

	mutex_lock(&isl->isl_mutex);
	/* PWMI x I2C mode use reg<0x0> for brightness only */
	regval = (ISL_BRIGHTNESS_VAL_MAX * level) / ISL_BRIGHTNESS_LEVEL_MAX;
	ret = isl97698_i2c_write(isl->client, ISL97698_REG_LED_H8, regval);
	mutex_unlock(&isl->isl_mutex);
	if (ret < 0) {
		dev_err(&isl->client->dev, "Write brightness current H8 reg failed.");
		return ret;
	}
	isl->brightness = level;

	return 0;
}

static void brightness_set_chip_enable(struct isl97698_st *isl, int enable)
{
	gpio_set_value(isl->bias_en, enable);
	isl->enable = enable;
}

static int brightness_defconfig(struct isl97698_st *isl)
{
	int ret;

	mutex_lock(&isl->isl_mutex);
	ret = isl97698_i2c_write(isl->client, ISL97698_REG_CONF, ISL_CONF_DEF);
	if (ret < 0) {
		dev_err(&isl->client->dev, "ISL97698_REG_CONF write failed.");
		mutex_unlock(&isl->isl_mutex);
		return ret;
	}

	ret = isl97698_i2c_write(isl->client, ISL97698_REG_PFM, ISL_PFM_MODE_DEF);
	if (ret < 0) {
		dev_err(&isl->client->dev, "ISL97698_REG_PFM write failed.");
		mutex_unlock(&isl->isl_mutex);
		return ret;
	}

	ret = isl97698_i2c_write(isl->client, ISL97698_REG_BOOST, ISL_BST_MODE_DEF);
	mutex_unlock(&isl->isl_mutex);
	if (ret < 0) {
		dev_err(&isl->client->dev, "ISL97698_REG_BOOST write failed.");
		return ret;
	}

	ret = brightness_set_value(isl, ISL_BRIGHTNESS_LEVEL_DEF);
	if (ret < 0) {
		dev_err(&isl->client->dev, "ISL97698_REG_BOOST write failed.");
		return ret;
	}

	return 0;
}

static int brightness_chip_check(struct isl97698_st *isl)
{
	int ret;
	u8 val;

	mutex_lock(&isl->isl_mutex);
	ret = isl97698_i2c_read(isl->client, ISL97698_REG_LED_H8, &val);
	if (ret < 0) {
		dev_err(&isl->client->dev, "ISL97698_REG_LED_H8 read failed.");
		mutex_unlock(&isl->isl_mutex);
		return ret;
	}

	ret = isl97698_i2c_read(isl->client, ISL97698_REG_STATUS, &val);
	mutex_unlock(&isl->isl_mutex);
	if (ret < 0) {
		dev_err(&isl->client->dev, "ISL97698_REG_STATUS read failed.");
		return ret;
	}
	if (val & ISL97698_ST_FAULT_MASK) {
		dev_err(&isl->client->dev,
			"brightness fault(OTP/OVP/VSC/BoostHCL), status=%02x", val);
	}

	return 0;
}

static ssize_t brightness_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct isl97698_st *isl = i2c_get_clientdata(to_i2c_client(dev));

	return sprintf(buf, "%d\n", isl->brightness);
}

static ssize_t brightness_store(struct device *dev,
		struct device_attribute *attr, const  char *buf, size_t count)
{
	int ret, level;
	struct isl97698_st *isl = i2c_get_clientdata(to_i2c_client(dev));

	if (kstrtoint(buf, 10, &level))
		return -EINVAL;
	if ((level < ISL_BRIGHTNESS_LEVEL_MIN) ||
			(level > ISL_BRIGHTNESS_LEVEL_MAX))
		return -EINVAL;

	ret = brightness_set_value(isl, level);
	if (ret < 0)
		return -EIO;

	return count;
}

static ssize_t max_brightness_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", ISL_BRIGHTNESS_LEVEL_MAX);
}

static DEVICE_ATTR(brightness, S_IRUGO|S_IWUSR|S_IWGRP, brightness_show, brightness_store);
static DEVICE_ATTR(max_brightness, S_IRUGO|S_IWUSR|S_IWGRP, max_brightness_show, NULL);


static struct attribute *isl_att_brightness[] = {
	&dev_attr_brightness.attr,
	&dev_attr_max_brightness.attr,
	NULL
};

static struct attribute_group isl_brightness_group = {
	.attrs = isl_att_brightness
};

static int  isl97698_probe(struct i2c_client *client,
					const struct i2c_device_id *id)
{
	int res;
	struct isl97698_platform_data *pdata = client->dev.platform_data;
	struct isl97698_st *chip = NULL;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "i2c functions unsupported\n");
		return -EOPNOTSUPP;
	}

	if (pdata->bias_en == -1) {
		dev_err(&client->dev, "bias_en invalid\n");
		return -EIO;
	}

	chip = kzalloc(sizeof(struct isl97698_st), GFP_KERNEL);
	if (chip == NULL) {
		dev_err(&client->dev, "struct isl97698_st allocation failed\n");
		return -ENOMEM;
	}
	chip->client = client;
	chip->bias_en = pdata->bias_en;
	i2c_set_clientdata(client, chip);
	mutex_init(&chip->isl_mutex);

	brightness_set_chip_enable(chip, ISL_CHIP_ENABLE);
	res = brightness_chip_check(chip);
	if (res <  0) {
		dev_err(&client->dev, "brightness_chip_check failed\n");
		res = -ENXIO;
		goto isl_probe_fail;
	}

	res = brightness_defconfig(chip);
	if (res <  0) {
		dev_err(&client->dev, "brightness_defconfig failed\n");
		res = -EIO;
		goto isl_probe_fail;
	}

	res = sysfs_create_group(&client->dev.kobj, &isl_brightness_group);
	if (res) {
		dev_err(&client->dev, "device create sys file failed\n");
		goto isl_probe_fail;
	}
	dev_info(&client->dev, "Brightness isl97698_probe successed\n");

	return res;

isl_probe_fail:
	kfree(chip);
	dev_err(&client->dev, "Brightness isl97698_probe failed\n");
	return res;
}

static int isl97698_remove(struct i2c_client *client)
{
	sysfs_remove_group(&client->dev.kobj, &isl_brightness_group);
	return 0;
}

static struct i2c_device_id isl97698_id[] = {
	{ "isl97698", 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, isl97698_id);

#if (defined ENABLE_BACKLIGHT_PM) && (defined CONFIG_PM)
static int isl97698_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct isl97698_st *isl = i2c_get_clientdata(client);

	brightness_set_chip_enable(isl, ISL_CHIP_DISABLE);
	return 0;
}

static int isl97698_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct isl97698_st *isl = i2c_get_clientdata(client);

	brightness_set_chip_enable(isl, ISL_CHIP_ENABLE);
	return 0;
}
static SIMPLE_DEV_PM_OPS(isl97698_pm_ops, isl97698_suspend, isl97698_resume);
#define ISL97698_PM_OPS (&isl97698_pm_ops)
#else
#define ISL97698_PM_OPS NULL
#endif


static struct i2c_driver isl97698_driver = {
	.driver = {
		.name = "isl97698",
		.pm = ISL97698_PM_OPS,
	},
	.probe = isl97698_probe,
	.remove = isl97698_remove,
	.id_table = isl97698_id,
};

module_i2c_driver(isl97698_driver);

MODULE_AUTHOR("Tan Baixing <baixingx.tan@intel.com>");
MODULE_DESCRIPTION("Isl97698 brightness Driver");
MODULE_LICENSE("GPL");
