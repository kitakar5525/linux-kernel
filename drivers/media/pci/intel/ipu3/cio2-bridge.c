// SPDX-License-Identifier: GPL-2.0
// Author: Dan Scally <djrscally@gmail.com>
#include <linux/acpi.h>
#include <linux/device.h>
#include <linux/fwnode.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/property.h>
#include <media/v4l2-subdev.h>

#include "cio2-bridge.h"

/*
 * Extend this array with ACPI Hardware ID's of devices known to be
 * working
 */
static const char * const supported_devices[] = {
	"INT33BE",
	"OVTI2680",
};

static struct software_node cio2_hid_node = { CIO2_HID };

static struct cio2_bridge bridge;

static const char * const port_names[] = {
	"port0", "port1", "port2", "port3"
};

static const struct property_entry remote_endpoints[] = {
	PROPERTY_ENTRY_REF("remote-endpoint", /* Sensor 0, Sensor Property */
			   &bridge.sensors[0].swnodes[SWNODE_CIO2_ENDPOINT]),
	PROPERTY_ENTRY_REF("remote-endpoint", /* Sensor 0, CIO2 Property */
			   &bridge.sensors[0].swnodes[SWNODE_SENSOR_ENDPOINT]),
	PROPERTY_ENTRY_REF("remote-endpoint",
			   &bridge.sensors[1].swnodes[SWNODE_CIO2_ENDPOINT]),
	PROPERTY_ENTRY_REF("remote-endpoint",
			   &bridge.sensors[1].swnodes[SWNODE_SENSOR_ENDPOINT]),
	PROPERTY_ENTRY_REF("remote-endpoint",
			   &bridge.sensors[2].swnodes[SWNODE_CIO2_ENDPOINT]),
	PROPERTY_ENTRY_REF("remote-endpoint",
			   &bridge.sensors[2].swnodes[SWNODE_SENSOR_ENDPOINT]),
	PROPERTY_ENTRY_REF("remote-endpoint",
			   &bridge.sensors[3].swnodes[SWNODE_CIO2_ENDPOINT]),
	PROPERTY_ENTRY_REF("remote-endpoint",
			   &bridge.sensors[3].swnodes[SWNODE_SENSOR_ENDPOINT]),
};

static int read_acpi_block(struct device *dev, char *id, void *data, u32 size)
{
	struct acpi_buffer buffer = { ACPI_ALLOCATE_BUFFER, NULL };
	struct acpi_handle *handle;
	union acpi_object *obj;
	acpi_status status;
	int ret;

	handle = ACPI_HANDLE(dev);

	status = acpi_evaluate_object(handle, id, NULL, &buffer);
	if (ACPI_FAILURE(status))
		return -ENODEV;

	obj = buffer.pointer;
	if (!obj) {
		dev_err(dev, "Couldn't locate ACPI buffer\n");
		return -ENODEV;
	}

	if (obj->type != ACPI_TYPE_BUFFER) {
		dev_err(dev, "Couldn't read ACPI buffer\n");
		ret = -ENODEV;
		goto out_free_buff;
	}

	if (obj->buffer.length > size) {
		dev_err(dev, "Given buffer is too small\n");
		ret = -ENODEV;
		goto out_free_buff;
	}

	memcpy(data, obj->buffer.pointer, obj->buffer.length);
	ret = obj->buffer.length;

out_free_buff:
	kfree(buffer.pointer);
	return ret;
}

static int get_acpi_ssdb_sensor_data(struct device *dev,
				     struct sensor_bios_data *sensor)
{
	struct sensor_bios_data_packed sensor_data;
	int ret;

	ret = read_acpi_block(dev, "SSDB", &sensor_data, sizeof(sensor_data));
	if (ret < 0)
		return ret;

	sensor->link = sensor_data.link;
	sensor->lanes = sensor_data.lanes;
	sensor->mclkspeed = sensor_data.mclkspeed;
	sensor->degree = sensor_data.degree;

	return 0;
}

static int create_fwnode_properties(struct sensor *sensor,
				    struct sensor_bios_data *ssdb)
{
	struct property_entry *cio2_properties = sensor->cio2_properties;
	struct property_entry *dev_properties = sensor->dev_properties;
	struct property_entry *ep_properties = sensor->ep_properties;
	int i;

	/* device fwnode properties */
	memset(dev_properties, 0, sizeof(struct property_entry) * 3);

	dev_properties[0] = PROPERTY_ENTRY_U32("clock-frequency",
					       ssdb->mclkspeed);
	dev_properties[1] = PROPERTY_ENTRY_U8("rotation", ssdb->degree);

	/* endpoint fwnode properties */
	memset(ep_properties, 0, sizeof(struct property_entry) * 4);

	sensor->data_lanes = kmalloc_array(ssdb->lanes, sizeof(u32),
					   GFP_KERNEL);

	if (!sensor->data_lanes)
		return -ENOMEM;

	for (i = 0; i < ssdb->lanes; i++)
		sensor->data_lanes[i] = i + 1;

	ep_properties[0] = PROPERTY_ENTRY_U32("bus-type", 5);
	ep_properties[1] = PROPERTY_ENTRY_U32_ARRAY_LEN("data-lanes",
							sensor->data_lanes,
							ssdb->lanes);
	ep_properties[2] = remote_endpoints[(bridge.n_sensors * 2) + ENDPOINT_SENSOR];

	/* cio2 endpoint props */
	memset(cio2_properties, 0, sizeof(struct property_entry) * 3);

	cio2_properties[0] = PROPERTY_ENTRY_U32_ARRAY_LEN("data-lanes",
							  sensor->data_lanes,
							  ssdb->lanes);
	cio2_properties[1] = remote_endpoints[(bridge.n_sensors * 2) + ENDPOINT_CIO2];

	return 0;
}

static int create_connection_swnodes(struct sensor *sensor,
				     struct sensor_bios_data *ssdb)
{
	struct software_node *nodes = sensor->swnodes;

	memset(nodes, 0, sizeof(struct software_node) * 6);

	nodes[SWNODE_SENSOR_HID] = NODE_SENSOR(sensor->name,
					       sensor->dev_properties);
	nodes[SWNODE_SENSOR_PORT] = NODE_PORT("port0",
					      &nodes[SWNODE_SENSOR_HID]);
	nodes[SWNODE_SENSOR_ENDPOINT] = NODE_ENDPOINT("endpoint0",
						      &nodes[SWNODE_SENSOR_PORT],
						      sensor->ep_properties);
	nodes[SWNODE_CIO2_PORT] = NODE_PORT(port_names[ssdb->link],
					    &cio2_hid_node);
	nodes[SWNODE_CIO2_ENDPOINT] = NODE_ENDPOINT("endpoint0",
						    &nodes[SWNODE_CIO2_PORT],
						    sensor->cio2_properties);

	return 0;
}

static void cio2_bridge_unregister_sensors(void)
{
	struct sensor *sensor;
	int i;

	for (i = 0; i < bridge.n_sensors; i++) {
		sensor = &bridge.sensors[i];

		software_node_unregister_nodes_reverse(sensor->swnodes);

		kfree(sensor->data_lanes);

		put_device(sensor->dev);
		acpi_dev_put(sensor->adev);
	}
}

static int connect_supported_devices(struct pci_dev *cio2)
{
	struct sensor_bios_data ssdb;
	struct fwnode_handle *fwnode;
	struct acpi_device *adev;
	struct sensor *sensor;
	struct device *dev;
	int i, ret;

	ret = 0;
	for (i = 0; i < ARRAY_SIZE(supported_devices); i++) {
		adev = acpi_dev_get_first_match_dev(supported_devices[i], NULL, -1);
		if (!adev)
			continue;

		dev = bus_find_device_by_acpi_dev(&i2c_bus_type, adev);
		if (!dev) {
			ret = -EPROBE_DEFER;
			goto err_rollback;
		}

		sensor = &bridge.sensors[bridge.n_sensors];
		sensor->dev = dev;
		sensor->adev = adev;

		snprintf(sensor->name, ACPI_ID_LEN, "%s",
			 supported_devices[i]);

		ret = get_acpi_ssdb_sensor_data(dev, &ssdb);
		if (ret)
			goto err_free_dev;

		ret = create_fwnode_properties(sensor, &ssdb);
		if (ret)
			goto err_free_dev;

		ret = create_connection_swnodes(sensor, &ssdb);
		if (ret)
			goto err_free_dev;

		ret = software_node_register_nodes(sensor->swnodes);
		if (ret)
			goto err_free_dev;

		fwnode = software_node_fwnode(&sensor->swnodes[SWNODE_SENSOR_HID]);
		if (!fwnode) {
			ret = -ENODEV;
			goto err_free_swnodes;
		}

		set_secondary_fwnode(dev, fwnode);

		dev_info(&cio2->dev, "Found supported device %s\n",
			 supported_devices[i]);

		bridge.n_sensors++;
		continue;
	}

	return ret;

err_free_swnodes:
	software_node_unregister_nodes_reverse(sensor->swnodes);
err_free_dev:
	put_device(dev);
err_rollback:
	acpi_dev_put(adev);

	/*
	 * If an iteration of this loop results in -EPROBE_DEFER then
	 * we need to roll back any sensors that were successfully
	 * registered. Any other error and we'll skip that step, as
	 * it seems better to have one successfully connected sensor.
	 */

	if (ret == -EPROBE_DEFER)
		cio2_bridge_unregister_sensors();

	return ret;
}

int cio2_bridge_build(struct pci_dev *cio2)
{
	struct fwnode_handle *fwnode;
	int ret;

	pci_dev_get(cio2);

	dev_info(&cio2->dev, "DEBUG: %s() called (RFC v3)\n", __func__);

	ret = software_node_register(&cio2_hid_node);
	if (ret < 0) {
		dev_err(&cio2->dev, "Failed to register the CIO2 HID node\n");
		goto err_put_cio2;
	}

	ret = connect_supported_devices(cio2);
	if (ret == -EPROBE_DEFER)
		goto err_unregister_cio2;

	if (bridge.n_sensors == 0) {
		ret = -EPROBE_DEFER;
		goto err_unregister_cio2;
	}

	dev_info(&cio2->dev, "Connected %d cameras\n", bridge.n_sensors);

	fwnode = software_node_fwnode(&cio2_hid_node);
	if (!fwnode) {
		dev_err(&cio2->dev,
			"Error getting fwnode from cio2 software_node\n");
		ret = -ENODEV;
		goto err_unregister_sensors;
	}

	set_secondary_fwnode(&cio2->dev, fwnode);

	return 0;

err_unregister_sensors:
	cio2_bridge_unregister_sensors();
err_unregister_cio2:
	software_node_unregister(&cio2_hid_node);
err_put_cio2:
	pci_dev_put(cio2);

	return ret;
}

void cio2_bridge_burn(struct pci_dev *cio2)
{
	pci_dev_put(cio2);

	cio2_bridge_unregister_sensors();

	software_node_unregister(&cio2_hid_node);
}
