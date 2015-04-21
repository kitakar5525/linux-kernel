/*
 * bq51003_power.c - bq51003 wireless power supply driver
 *
 * Copyright (c) 2015 Intel Corporation
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/power/bq51003_power.h>

#define DEV_NAME "bq51003_power"

static int bq51003_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val);
static int bq51003_set_property(struct power_supply *psy,
					enum power_supply_property psp,
					const union power_supply_propval *val);
static int bq51003_property_is_writeable(struct power_supply *psy,
					enum power_supply_property psp);


struct bq51003_power {
	const struct bq51003_plat_data *pdata;
	struct power_supply psy;
	unsigned int irq;
	int online; /* Wireless Power Transmission ongoing */
	int enable; /* enable/disable Wireless Power */
};


static enum power_supply_property bq51003_power_props[] = {
	POWER_SUPPLY_PROP_ENABLE_CHARGER,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_TYPE,
};

static void bq51003_enable(struct bq51003_power *wc_charger, int enable)
{
	/* set EN1/EN2 accordingly to enable value */
	/* order is important to avoid a transition by the dynamic state */
	if (enable) {
		gpio_set_value(wc_charger->pdata->gpio_en2, !enable);
		gpio_set_value(wc_charger->pdata->gpio_en1, !enable);
	} else {
		gpio_set_value(wc_charger->pdata->gpio_en1, !enable);
		gpio_set_value(wc_charger->pdata->gpio_en2, !enable);
	}
	wc_charger->enable = !!enable;
}

static irqreturn_t bq51003_power_irq(int irq, void *devid)
{
	struct power_supply *psy = devid;

	power_supply_changed(psy);

	return IRQ_HANDLED;
}

/*
 * bq51003_get_property() may be called in atomic
 * context, make sure you do not use locks
 */
static int bq51003_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val)
{
	int ret = 0;
	struct bq51003_power *wc_charger = container_of(psy, struct bq51003_power , psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		wc_charger->online = !gpio_get_value(wc_charger->pdata->wc_chg_n);
		val->intval = wc_charger->online;

		break;
	case POWER_SUPPLY_PROP_ENABLE_CHARGER:
		val->intval = wc_charger->enable;
		break;

	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int bq51003_set_property(struct power_supply *psy,
					enum power_supply_property psp,
					const union power_supply_propval *val)
{
	struct bq51003_power *wc_charger = container_of(psy, struct bq51003_power , psy);

	if (psp == POWER_SUPPLY_PROP_ENABLE_CHARGER) {
		bq51003_enable(wc_charger, val->intval);
		return 0;
	} else
		return -EPERM;
}

static int bq51003_property_is_writeable(struct power_supply *psy,
					enum power_supply_property psp)
{
	if (psp == POWER_SUPPLY_PROP_ENABLE_CHARGER)
		return 1;
	else
		return 0;
}

static int bq51003_power_probe(struct platform_device *pdev)
{
	const struct bq51003_plat_data *pdata = pdev->dev.platform_data;
	struct bq51003_power *wc_charger;
	struct power_supply *psy;
	int ret;
	int irq;

	if (!pdata) {
		dev_err(&pdev->dev, "No platform data\n");
		return -EINVAL;
	}

	if (!gpio_is_valid(pdata->wc_chg_n)) {
		dev_err(&pdev->dev, "Invalid gpio wc_chg_n pin\n");
		return -EINVAL;
	}

	if (!gpio_is_valid(pdata->gpio_en1)) {
		dev_err(&pdev->dev, "Invalid gpio EN1 pin\n");
		return -EINVAL;
	}

	if (!gpio_is_valid(pdata->gpio_en2)) {
		dev_err(&pdev->dev, "Invalid gpio EN2 pin\n");
		return -EINVAL;
	}

	ret = gpio_request(pdata->wc_chg_n, dev_name(&pdev->dev));
	if (ret) {
		dev_err(&pdev->dev, "Failed to request gpio pin wc_chg_n: %d\n", ret);
		goto err_gpio_free;
	}
	ret = gpio_direction_input(pdata->wc_chg_n);
	if (ret) {
		dev_err(&pdev->dev, "Failed to set gpio wc_chg_n to input: %d\n", ret);
		goto err_gpio_free;
	}

	ret = gpio_request(pdata->gpio_en1, dev_name(&pdev->dev));
	if (ret) {
		dev_err(&pdev->dev, "Failed to request gpio pin gpio_en1: %d\n", ret);
		goto err_gpio_free;
	}
	ret = gpio_direction_output(pdata->gpio_en1, 0);
	if (ret) {
		dev_err(&pdev->dev, "Failed to set gpio gpio_en1 to output: %d\n", ret);
		goto err_gpio_free;
	}

	ret = gpio_request(pdata->gpio_en2, dev_name(&pdev->dev));
	if (ret) {
		dev_err(&pdev->dev, "Failed to request gpio pin gpio_en2: %d\n", ret);
		goto err_gpio_free;
	}
	ret = gpio_direction_output(pdata->gpio_en2, 0);
	if (ret) {
		dev_err(&pdev->dev, "Failed to set gpio gpio_en2 to output: %d\n", ret);
		goto err_gpio_free;
	}

	wc_charger = devm_kzalloc(&pdev->dev, sizeof(*wc_charger), GFP_KERNEL);
	if (!wc_charger) {
		dev_err(&pdev->dev, "Driver struct alloc failed\n");
		ret = -ENOMEM;
		goto err_gpio_free;
	}

	psy = &wc_charger->psy;
	psy->name = DEV_NAME;
	psy->type = POWER_SUPPLY_TYPE_WIRELESS,
	psy->properties = bq51003_power_props;
	psy->num_properties = ARRAY_SIZE(bq51003_power_props);
	psy->get_property = bq51003_get_property;
	psy->set_property = bq51003_set_property;
	psy->property_is_writeable = bq51003_property_is_writeable;
	wc_charger->pdata = pdata;

	ret = power_supply_register(&pdev->dev, psy);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to register wireless power supply: %d\n",
			ret);
		goto err_gpio_free;
	}

	ret = irq = gpio_to_irq(pdata->wc_chg_n);
	if (irq < 0) {
		dev_err(&pdev->dev, "gpio_to_irq fails: %d\n", ret);
		goto err_free;
	} else {
		ret = request_irq(irq, bq51003_power_irq,
				IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
				dev_name(&pdev->dev), &wc_charger->psy);
		if (ret < 0) {
			dev_err(&pdev->dev, "Failed to request irq: %d\n", ret);
			goto err_free;
		} else
			wc_charger->irq = irq;
	}

	platform_set_drvdata(pdev, wc_charger);

	/* align SW behavior on HW default conf */
	bq51003_enable(wc_charger, 1);
	dev_dbg(&pdev->dev, "bq51003: Wireless power ready\n");
	return 0;

err_free:
	if (wc_charger->irq >= 0)
		free_irq(wc_charger->irq, &wc_charger->psy);
	power_supply_unregister(&wc_charger->psy);

err_gpio_free:
	gpio_free(pdata->wc_chg_n);
	gpio_free(pdata->gpio_en1);
	gpio_free(pdata->gpio_en2);

	return ret;
}

static int bq51003_power_remove(struct platform_device *pdev)
{
	struct bq51003_power *wc_charger = platform_get_drvdata(pdev);

	if (wc_charger->irq)
		free_irq(wc_charger->irq, &wc_charger->psy);

	power_supply_unregister(&wc_charger->psy);

	gpio_free(wc_charger->pdata->wc_chg_n);
	gpio_free(wc_charger->pdata->gpio_en1);
	gpio_free(wc_charger->pdata->gpio_en2);

	platform_set_drvdata(pdev, NULL);
	return 0;
}


static struct platform_driver bq51003_power_driver = {
	.probe = bq51003_power_probe,
	.remove = bq51003_power_remove,
	.driver = {
		.name = DEV_NAME,
		.owner = THIS_MODULE,
	},
};

module_platform_driver(bq51003_power_driver);

MODULE_AUTHOR("Honore TRICOT <honorex.tricot@intel.com>");
MODULE_DESCRIPTION("bq51003 wireless power supply Driver");
MODULE_LICENSE("GPL");

