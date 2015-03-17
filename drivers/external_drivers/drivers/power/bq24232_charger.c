/*
 * bq24232_charger.c - BQ24232 Battery Charger
 *
 * Copyright (C) 2015 Intel Corporation
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
 *
 */

#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/power/bq24232_charger.h>
#include <linux/usb/otg.h>

struct bq24232_charger {
	const struct bq24232_plat_data *pdata;
	struct device *dev;

	struct power_supply pow_sply;

	struct work_struct otg_evt_work;
	struct notifier_block otg_nb;
	struct list_head otg_queue;
	spinlock_t otg_queue_lock;
	spinlock_t pgood_lock;
	struct mutex stat_lock;

	struct usb_phy *transceiver;

	int cc;
	enum power_supply_type cable_type;

	/*
	 * @pgood_valid:	set to 1 only if the bq24232 input voltage is good,
	 *					it indicates a valid power source from hw perspective;
	 *					it is verified before to start charging
	 */
	bool pgood_valid;

	/*
	 * @online: is set to 1 only if POWER_SUPPLY_PROP_TYPE reports any kind of USB,
	 *			otherwise 0 es. wireless charging
	 */
	bool online;

	/*
	 * @is_charger_enabled: is set to 1 only if POWER_SUPPLY_CHARGER_EVENT_{CONNECT/UPDATE/RESUME},
	 *						it relies on otg notifications, it indicates a valid power source
	 *						from sw perspective;
	 */
	bool is_charger_enabled;

	/*
	 * @is_charging_enabled:	is set to 1 only when a charging process has started,
	 *							after actions to assert low CE_N signal have been taken
	 */
	bool is_charging_enabled;

	/*
	 * @boost_mode: 0
	 */
	bool boost_mode;
};

static enum power_supply_property bq24232_charger_properties[] = {
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_TYPE,
	POWER_SUPPLY_PROP_CHARGE_CURRENT,
	POWER_SUPPLY_PROP_CABLE_TYPE,
	POWER_SUPPLY_PROP_ENABLE_CHARGING,
	POWER_SUPPLY_PROP_ENABLE_CHARGER
};


struct bq24232_otg_event {
	struct list_head node;
	struct power_supply_cable_props cap;
};

static inline int bq24232_enable_charging(struct bq24232_charger *chip, bool val);

static enum power_supply_type get_power_supply_type(
			enum power_supply_charger_cable_type cable)
{
	switch (cable) {
	case POWER_SUPPLY_CHARGER_TYPE_USB_DCP:
		return POWER_SUPPLY_TYPE_USB_DCP;
	case POWER_SUPPLY_CHARGER_TYPE_USB_CDP:
		return POWER_SUPPLY_TYPE_USB_CDP;
	case POWER_SUPPLY_CHARGER_TYPE_USB_ACA:
	case POWER_SUPPLY_CHARGER_TYPE_ACA_DOCK:
		return POWER_SUPPLY_TYPE_USB_ACA;
	case POWER_SUPPLY_CHARGER_TYPE_AC:
		return POWER_SUPPLY_TYPE_MAINS;
	case POWER_SUPPLY_CHARGER_TYPE_SE1:
		return POWER_SUPPLY_TYPE_USB_DCP;
	case POWER_SUPPLY_CHARGER_TYPE_USB_SDP:
		return POWER_SUPPLY_TYPE_USB;
	case POWER_SUPPLY_CHARGER_TYPE_NONE:
	default:
		return POWER_SUPPLY_TYPE_UNKNOWN;
	}
}

static enum power_supply_charger_cable_type get_cable_type(
		enum power_supply_type psy)
{
	switch (psy) {
	case POWER_SUPPLY_TYPE_USB_DCP:
		return POWER_SUPPLY_CHARGER_TYPE_USB_DCP;
	case POWER_SUPPLY_TYPE_USB_CDP:
		return POWER_SUPPLY_CHARGER_TYPE_USB_CDP;
	case POWER_SUPPLY_TYPE_USB_ACA:
		return POWER_SUPPLY_CHARGER_TYPE_USB_ACA;
	case POWER_SUPPLY_TYPE_MAINS:
		return POWER_SUPPLY_CHARGER_TYPE_AC;
	case POWER_SUPPLY_TYPE_USB:
		return POWER_SUPPLY_CHARGER_TYPE_USB_SDP;
	default:
		return POWER_SUPPLY_CHARGER_TYPE_NONE;
	}
}

static void bq24232_update_online_status(struct bq24232_charger *chip)
{
	if (chip->is_charger_enabled &&
			(chip->pow_sply.type != POWER_SUPPLY_TYPE_UNKNOWN))
		chip->online = 1;
	else
		chip->online = 0;
}

static void bq24232_update_pgood_status(struct bq24232_charger *chip)
{
	int pgood_val;
	unsigned long flags;
	spin_lock_irqsave(&chip->pgood_lock, flags);
	pgood_val = gpio_get_value(chip->pdata->pgood_gpio);
	chip->pgood_valid = !pgood_val;
	spin_unlock_irqrestore(&chip->pgood_lock, flags);
}

static inline bool bq24232_can_enable_charging(struct bq24232_charger *chip)
{
	int ret = false;
	bq24232_update_pgood_status(chip);
	if (chip->pgood_valid && chip->is_charger_enabled)
		ret = true;
	else
		dev_warn(chip->dev, "%s: cannot enable charging, pgood_valid = %d is_charger_enabled = %d",
				__func__,
				chip->pgood_valid,
				chip->is_charger_enabled);
	return ret;
}

static int bq24232_enable_charging(
	struct bq24232_charger *chip, bool val)
{
	int ret = -ENODATA;
	if (chip->pdata->enable_charging) {
		/*
		 * Each time the charger is enabled/disabled slow charge current is selected by default
		 */
		gpio_set_value(chip->pdata->chg_rate_temp_gpio, 0);
		ret = chip->pdata->enable_charging(val);
		if (ret)
			dev_err(chip->dev, "%s: error(%d) in master enable-charging\n", __func__, ret);
		else
			dev_info(chip->dev, "%s = %d complete\n", __func__, val);
	} else
		dev_err(chip->dev, "%s: no enable_charging function\n", __func__);
	return ret;
}

static void bq24232_update_charger_status(struct bq24232_charger *chip,
		enum power_supply_charger_event evt, enum power_supply_charger_cable_type cable_type)
{
	unsigned long flags;
	int ret;
	mutex_lock(&chip->stat_lock);

	spin_lock_irqsave(&chip->pow_sply.changed_lock, flags);
	chip->pow_sply.type = get_power_supply_type(cable_type);
	chip->cable_type = cable_type;
	spin_unlock_irqrestore(&chip->pow_sply.changed_lock, flags);

	if (evt == POWER_SUPPLY_CHARGER_EVENT_CONNECT ||
			evt == POWER_SUPPLY_CHARGER_EVENT_UPDATE ||
			evt == POWER_SUPPLY_CHARGER_EVENT_RESUME) {
		chip->is_charger_enabled = true;
	} else
		chip->is_charger_enabled = false;

	if (bq24232_can_enable_charging(chip)) {
		ret = bq24232_enable_charging(chip, true);
		if (!ret) {
			chip->is_charging_enabled = true;
			chip->cc = BQ24232_CHARGE_CURRENT_LOW;
		}
	} else {
		bq24232_enable_charging(chip, false);
		chip->is_charging_enabled = false;
		chip->cc = 0;
	}
	mutex_unlock(&chip->stat_lock);

	bq24232_update_online_status(chip);
}

static int bq24232_charger_get_property(struct power_supply *psy,
		enum power_supply_property psp, union power_supply_propval *val)
{
	struct bq24232_charger *bq24232_charger = container_of(psy, struct bq24232_charger, pow_sply);
	int ret = 0;

	mutex_lock(&bq24232_charger->stat_lock);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = bq24232_charger->online;
		break;
	case POWER_SUPPLY_PROP_CHARGE_CURRENT:
		val->intval = bq24232_charger->cc;
		break;
	case POWER_SUPPLY_PROP_CABLE_TYPE:
		val->intval = bq24232_charger->cable_type;
		break;
	case POWER_SUPPLY_PROP_ENABLE_CHARGING:
		val->intval = bq24232_charger->is_charging_enabled;
		break;
	case POWER_SUPPLY_PROP_ENABLE_CHARGER:
		val->intval = bq24232_charger->is_charger_enabled;
		break;
	default:
		ret = -EINVAL;
	}
	mutex_unlock(&bq24232_charger->stat_lock);

	dev_dbg(bq24232_charger->dev, "bq24232_charger_get_property: property = %d, value = %d\n",
			psp,
			val->intval);
	return ret;
}

static int bq24232_charger_set_property(struct power_supply *psy,
				    enum power_supply_property psp,
				    const union power_supply_propval *val)
{
#ifdef BQ24232_DBG
	struct bq24232_charger *chip = container_of(psy, struct bq24232_charger, pow_sply);
	int ret = 0;

	mutex_lock(&chip->stat_lock);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		chip->online = val->intval;
		break;
	case POWER_SUPPLY_PROP_CHARGE_CURRENT:
		ret = -EPERM;
		break;
	case POWER_SUPPLY_PROP_CABLE_TYPE:
		chip->cable_type = val->intval;
		chip->pow_sply.type = get_power_supply_type(chip->cable_type);
		break;
	case POWER_SUPPLY_PROP_ENABLE_CHARGING:
		if (val->intval) {
			if (bq24232_can_enable_charging(chip)) {
				ret = bq24232_enable_charging(chip, true);
				if (!ret)
					chip->is_charging_enabled = val->intval;
				else
					dev_warn(chip->dev, "%s: enable_charging(%d) failed with error %d\n", __func__, val->intval, ret);
			} else
				dev_err(chip->dev, "%s: conditions not met to enable charging\n", __func__);
		} else {
			ret = bq24232_enable_charging(chip, false);
			if (!ret)
				chip->is_charging_enabled = val->intval;
			else
				dev_err(chip->dev, "%s: enable_charging(%d) failed with error %d\n", __func__, val->intval, ret);
		}
		power_supply_changed(&chip->pow_sply);
		break;
	case POWER_SUPPLY_PROP_ENABLE_CHARGER:
		chip->is_charger_enabled = val->intval;
		break;
	default:
		ret = -ENODATA;
	}
	mutex_unlock(&chip->stat_lock);

	dev_dbg(chip->dev, "bq24232_charger_set_property: property = %d, value = %d\n", psp, val->intval);
	return ret;
#else
	return -EPERM;
#endif
}

static int otg_handle_notification(struct notifier_block *nb,
		unsigned long event, void *param) {
	struct bq24232_charger *chip = container_of(nb, struct bq24232_charger, otg_nb);
	struct bq24232_otg_event *evt;
	int vbus_detected;
	unsigned long flags;

	dev_dbg(chip->dev, "OTG notification: event %lu\n", event);

	if (!param)
		return NOTIFY_DONE;

	switch (event) {
	case USB_EVENT_CHARGER:
		evt = kzalloc(sizeof(*evt), GFP_ATOMIC);
		if (!evt)
			goto evt_err;
		memcpy(&evt->cap, (struct power_supply_cable_props *)param, sizeof(evt->cap));
		break;
	case USB_EVENT_VBUS:
		evt = kzalloc(sizeof(*evt), GFP_ATOMIC);
		if (!evt)
			goto evt_err;
		vbus_detected = *(int *)param;
		evt->cap.chrg_type = POWER_SUPPLY_CHARGER_TYPE_NONE;
		if (vbus_detected)
			evt->cap.chrg_evt = POWER_SUPPLY_CHARGER_EVENT_CONNECT;
		else
			evt->cap.chrg_evt = POWER_SUPPLY_CHARGER_EVENT_DISCONNECT;
		evt->cap.ma = BQ24232_CHARGE_CURRENT_LOW;
		break;
	default:
		return NOTIFY_DONE;
	}

	dev_info(chip->dev, "OTG notification: chrg_type %d, current limit %d,  event %d\n",
					evt->cap.chrg_type,
					evt->cap.ma,
					evt->cap.chrg_evt);

	INIT_LIST_HEAD(&evt->node);

	spin_lock_irqsave(&chip->otg_queue_lock, flags);
	list_add_tail(&evt->node, &chip->otg_queue);
	spin_unlock_irqrestore(&chip->otg_queue_lock, flags);

	queue_work(system_nrt_wq, &chip->otg_evt_work);

	return NOTIFY_OK;

evt_err:
	dev_err(chip->dev,
			"failed to allocate memory for OTG event\n");
	return NOTIFY_DONE;
}

static void bq24232_otg_evt_worker(struct work_struct *work)
{
	struct bq24232_charger *chip =
			container_of(work, struct bq24232_charger, otg_evt_work);
	struct bq24232_otg_event *evt, *tmp;
	unsigned long flags;

	spin_lock_irqsave(&chip->otg_queue_lock, flags);
	list_for_each_entry_safe(evt, tmp, &chip->otg_queue, node)
	{
		list_del(&evt->node);
		spin_unlock_irqrestore(&chip->otg_queue_lock, flags);

		bq24232_update_charger_status(chip, evt->cap.chrg_evt, evt->cap.chrg_type);

		dev_info(chip->dev, "%s: online = %d, pgood = %d, is_charging_enabled = %d\n", __func__,
				chip->online,
				chip->pgood_valid,
				chip->is_charging_enabled);

		spin_lock_irqsave(&chip->otg_queue_lock, flags);
		kfree(evt);

	}
	spin_unlock_irqrestore(&chip->otg_queue_lock, flags);

	power_supply_changed(&chip->pow_sply);
}

static inline int register_otg_notification(struct bq24232_charger *chip)
{

	int retval;
	INIT_LIST_HEAD(&chip->otg_queue);
	INIT_WORK(&chip->otg_evt_work, bq24232_otg_evt_worker);
	spin_lock_init(&chip->otg_queue_lock);

	chip->otg_nb.notifier_call = otg_handle_notification;

	/*
	 * Get the USB transceiver instance
	 */
	chip->transceiver = usb_get_phy(USB_PHY_TYPE_USB2);
	if (chip->transceiver < 0) {
		dev_err(chip->dev, "Failed to get the USB transceiver\n");
		return -ENODEV;
	}

	retval = usb_register_notifier(chip->transceiver, &chip->otg_nb);
	if (retval) {
		dev_err(chip->dev, "failed to register otg notifier\n");
		return -EINVAL;
	}

	return 0;
}


static int bq24232_charger_probe(struct platform_device *pdev)
{
	const struct bq24232_plat_data *pdata = pdev->dev.platform_data;
	struct bq24232_charger *bq24232_charger;
	struct power_supply *pow_sply;
	struct power_supply_charger_cap chgr_cap;
	enum power_supply_charger_cable_type cable_type;
	int ret;

	if (!pdev) {
		dev_err(&pdev->dev, "No platform device\n");
		return -EINVAL;
	}

	if (!pdata) {
		dev_err(&pdev->dev, "No platform data\n");
		return -EINVAL;
	}

	bq24232_charger = devm_kzalloc(&pdev->dev, sizeof(*bq24232_charger), GFP_KERNEL);
	if (!bq24232_charger) {
		dev_err(&pdev->dev, "Failed to alloc driver structure\n");
		return -ENOMEM;
	}

	bq24232_charger->pdata = pdata;

	ret = gpio_request_one(bq24232_charger->pdata->pgood_gpio, GPIOF_IN | GPIOF_OPEN_DRAIN, pdev->name);
	if (ret < 0)
		goto io_error1;
	ret = gpio_request_one(bq24232_charger->pdata->chg_rate_temp_gpio, GPIOF_INIT_LOW, pdev->name);
	if (ret < 0)
		goto io_error2;

	mutex_init(&bq24232_charger->stat_lock);
	spin_lock_init(&bq24232_charger->pgood_lock);

	pow_sply = &bq24232_charger->pow_sply;

	pow_sply->name = BQ24232_CHRGR_DEV_NAME;
	pow_sply->properties = bq24232_charger_properties;
	pow_sply->num_properties = ARRAY_SIZE(bq24232_charger_properties);
	pow_sply->get_property = bq24232_charger_get_property;
	pow_sply->set_property = bq24232_charger_set_property;

	pow_sply->supplied_to = bq24232_charger->pdata->supplied_to;
	pow_sply->num_supplicants = bq24232_charger->pdata->num_supplicants;
	bq24232_charger->dev = &pdev->dev;

	bq24232_charger->is_charger_enabled = false;
	bq24232_charger->is_charging_enabled = false;
	bq24232_charger->pgood_valid = false;
	bq24232_charger->cc = 0;
	bq24232_charger->boost_mode = 0;

	/*
	 * Register to get USB transceiver events
	 */
	ret = register_otg_notification(bq24232_charger);
	if (ret) {
		dev_err(bq24232_charger->dev, "REGISTER OTG NOTIFICATION FAILED\n");
		goto io_error3;
	}

	ret = power_supply_register(bq24232_charger->dev, pow_sply);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to register power supply: %d\n",
			ret);
		goto io_error4;
	}

	power_supply_query_charger_caps(&chgr_cap);
	cable_type = get_cable_type(chgr_cap.chrg_type);
	bq24232_update_charger_status(bq24232_charger, chgr_cap.chrg_evt, cable_type);

	dev_dbg(bq24232_charger->dev, "charger enabled: %d, charging enabled: %d, pgood_valid: %d, boost mode: %d charger type: %d\n",
				bq24232_charger->is_charger_enabled,
				bq24232_charger->is_charging_enabled,
				bq24232_charger->pgood_valid,
				bq24232_charger->boost_mode,
				bq24232_charger->pow_sply.type);

	dev_info(bq24232_charger->dev, "device successfully initialized");

	power_supply_changed(pow_sply);

	return 0;

io_error4:
	usb_unregister_notifier(bq24232_charger->transceiver, &bq24232_charger->otg_nb);
io_error3:
	gpio_free(bq24232_charger->pdata->chg_rate_temp_gpio);
io_error2:
	gpio_free(bq24232_charger->pdata->pgood_gpio);
io_error1:
	dev_err(bq24232_charger->dev, "%s: error occurred on initialization\n", __func__);
	return -EIO;

}


static int bq24232_charger_remove(struct platform_device *pdev)
{
	struct bq24232_charger *bq24232_charger = platform_get_drvdata(pdev);

	power_supply_unregister(&bq24232_charger->pow_sply);

	usb_unregister_notifier(bq24232_charger->transceiver, &bq24232_charger->otg_nb);

	if (bq24232_charger->pdata->chg_rate_temp_gpio)
		gpio_free(bq24232_charger->pdata->chg_rate_temp_gpio);

	if (bq24232_charger->pdata->pgood_gpio)
		gpio_free(bq24232_charger->pdata->pgood_gpio);

	return 0;
}



static struct platform_driver bq24232_charger_driver = {
	.probe = bq24232_charger_probe,
	.remove = bq24232_charger_remove,
	.driver = {
		.name = BQ24232_CHRGR_DEV_NAME,
		.owner = THIS_MODULE,
	},
};

static int __init bq24232_charger_init(void)
{
	int ret = platform_driver_register(&bq24232_charger_driver);
	if (ret)
		pr_err("Unable to register BQ24232 platform driver\n");
	return ret;
}
module_init(bq24232_charger_init);

static void __exit bq24232_charger_exit(void)
{
	platform_driver_unregister(&bq24232_charger_driver);
}
module_exit(bq24232_charger_exit);


MODULE_AUTHOR("A.Casolaro <antoniox.casolaro@intel.com>");
MODULE_AUTHOR("G.Valles <guillaumex.valles@intel.com>");
MODULE_DESCRIPTION("Driver for BQ24232 battery charger");
MODULE_LICENSE("GPL");

