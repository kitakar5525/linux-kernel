// SPDX-License-Identifier: GPL-2.0
/* Author: Dan Scally <djrscally@gmail.com> */
#include <linux/acpi.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/property.h>
#include <media/v4l2-subdev.h>

// fix me ===================
#include <media/v4l2-device.h>
#include <media/videobuf2-dma-sg.h>
// ==========================

#include "ipu3-cio2.h"
#include "cio2-bridge.h"

/*
 * Extend this array with ACPI Hardware ID's of devices known to be working.
 * Do not add a HID for a sensor that is not actually supported.
 */
static const char * const cio2_supported_devices[] = {
	"INT33BE",
	"OVTI2680",
};

static const struct software_node cio2_hid_node = { CIO2_HID };

static const char * const cio2_port_names[] = {
	"port0", "port1", "port2", "port3",
};

static int cio2_bridge_read_acpi_buffer(struct acpi_device *adev, char *id,
					void *data, u32 size)
{
	struct acpi_buffer buffer = { ACPI_ALLOCATE_BUFFER, NULL };
	union acpi_object *obj;
	acpi_status status;
	int ret;

	status = acpi_evaluate_object(adev->handle, id, NULL, &buffer);
	if (ACPI_FAILURE(status))
		return -ENODEV;

	obj = buffer.pointer;
	if (!obj) {
		dev_err(&adev->dev, "Couldn't locate ACPI buffer\n");
		return -ENODEV;
	}

	if (obj->type != ACPI_TYPE_BUFFER) {
		dev_err(&adev->dev, "Not an ACPI buffer\n");
		ret = -ENODEV;
		goto out_free_buff;
	}

	if (obj->buffer.length > size) {
		dev_err(&adev->dev, "Given buffer is too small\n");
		ret = -EINVAL;
		goto out_free_buff;
	}

	memcpy(data, obj->buffer.pointer, obj->buffer.length);
	ret = obj->buffer.length;

out_free_buff:
	kfree(buffer.pointer);
	return ret;
}

static void cio2_bridge_create_fwnode_properties(struct cio2_sensor *sensor)
{
        u32 data_lanes[4] = {1,2,3,4};

        /*
         * Can't use PROPERTY_ENTRY_REF because it creates a new variable to
         * point to, which doesn't survive the function.
         */
        sensor->local_ref[0] = (struct software_node_ref_args){
                .node = &sensor->swnodes[SWNODE_CIO2_ENDPOINT]
                };
        sensor->remote_ref[0] = (struct software_node_ref_args){
                .node = &sensor->swnodes[SWNODE_SENSOR_ENDPOINT]
                };

	sensor->dev_properties[0] = PROPERTY_ENTRY_U32("clock-frequency", sensor->ssdb.mclkspeed);
	sensor->dev_properties[1] = PROPERTY_ENTRY_U8("rotation", sensor->ssdb.degree);

	sensor->ep_properties[0] = PROPERTY_ENTRY_U32("bus-type", 5);
	sensor->ep_properties[1] = PROPERTY_ENTRY_U32_ARRAY_LEN("data-lanes", data_lanes, sensor->ssdb.lanes);
	sensor->ep_properties[2] = PROPERTY_ENTRY_REF_ARRAY("remote-endpoint", sensor->local_ref);

	sensor->cio2_properties[0] = PROPERTY_ENTRY_U32_ARRAY_LEN("data-lanes", data_lanes, sensor->ssdb.lanes);
	sensor->cio2_properties[1] = PROPERTY_ENTRY_REF_ARRAY("remote-endpoint", sensor->remote_ref);
}

static void cio2_bridge_create_connection_swnodes(struct cio2_sensor *sensor)
{
	struct software_node *nodes = sensor->swnodes;

	nodes[SWNODE_SENSOR_HID] = NODE_SENSOR(sensor->name,
					       sensor->dev_properties);
	nodes[SWNODE_SENSOR_PORT] = NODE_PORT("port0",
					      &nodes[SWNODE_SENSOR_HID]);
	nodes[SWNODE_SENSOR_ENDPOINT] = NODE_ENDPOINT("endpoint0",
						      &nodes[SWNODE_SENSOR_PORT],
						      sensor->ep_properties);
	nodes[SWNODE_CIO2_PORT] = NODE_PORT(cio2_port_names[sensor->ssdb.link],
					    &cio2_hid_node);
	nodes[SWNODE_CIO2_ENDPOINT] = NODE_ENDPOINT("endpoint0",
						    &nodes[SWNODE_CIO2_PORT],
						    sensor->cio2_properties);
}

static void cio2_bridge_unregister_sensors(struct cio2_bridge *bridge)
{
	struct cio2_sensor *sensor;
	unsigned int i;

	for (i = 0; i < bridge->n_sensors; i++) {
		sensor = &bridge->sensors[i];
		software_node_unregister_nodes(sensor->swnodes);
		acpi_dev_put(sensor->adev);
	}
}

static int cio2_bridge_connect_sensors(struct cio2_bridge *bridge)
{
	struct fwnode_handle *fwnode;
	struct cio2_sensor *sensor;
	struct acpi_device *adev;
        unsigned int i;
	int ret = 0;

	for (i = 0; i < ARRAY_SIZE(cio2_supported_devices); i++) {
		const char *this_device = cio2_supported_devices[i];

		for_each_acpi_dev_match(adev, this_device, NULL, -1) {
			if (!adev || !(adev->status.present && adev->status.enabled))
				return 0;

			sensor = &bridge->sensors[bridge->n_sensors];
			sensor->adev = adev;
                        strscpy(sensor->name, this_device, sizeof(sensor->name));

			ret = cio2_bridge_read_acpi_buffer(adev, "SSDB", &sensor->ssdb, sizeof(sensor->ssdb));
			if (ret < 0)
				goto err_put_adev;

			if (sensor->ssdb.lanes > 4) {
				dev_err(&adev->dev, "Number of lanes in SSDB is invalid\n");
				goto err_put_adev;
			}

			cio2_bridge_create_fwnode_properties(sensor);
			cio2_bridge_create_connection_swnodes(sensor);

			ret = software_node_register_nodes(sensor->swnodes);
			if (ret)
				goto err_put_adev;

			fwnode = software_node_fwnode(&sensor->swnodes[SWNODE_SENSOR_HID]);
			if (!fwnode) {
				ret = -ENODEV;
				goto err_free_swnodes;
			}

			adev->fwnode.secondary = fwnode;

			dev_info(bridge->cio2_dev, "Found supported sensor %s\n", acpi_dev_name(adev));

			bridge->n_sensors++;
		}
	}

	return ret;

err_free_swnodes:
	software_node_unregister_nodes(sensor->swnodes);
err_put_adev:
        acpi_dev_put(sensor->adev);

	return ret;
}

struct cio2_bridge *cio2_bridge_init(struct pci_dev *cio2)
{
	struct device *dev = &cio2->dev;	
	struct fwnode_handle *fwnode;
        struct cio2_bridge *bridge;
	int ret;

        bridge = kzalloc(sizeof(*bridge), GFP_KERNEL);
        if (!bridge)
                return ERR_PTR(-ENOMEM);

	pci_dev_get(cio2);
	bridge->cio2_dev = dev;

	ret = software_node_register(&cio2_hid_node);
	if (ret < 0) {
		dev_err(dev, "Failed to register the CIO2 HID node\n");
		goto err_put_cio2;    
	}

	ret = cio2_bridge_connect_sensors(bridge);
	if (ret)
		goto err_unregister_cio2;

	if (bridge->n_sensors == 0)
		goto err_unregister_cio2;

	dev_info(dev, "Connected %d cameras\n", bridge->n_sensors);

	fwnode = software_node_fwnode(&cio2_hid_node);
	if (!fwnode) {
		dev_err(dev, "Error getting fwnode from cio2 software_node\n");
		ret = -ENODEV;
		goto err_unregister_sensors;
	}

	set_secondary_fwnode(dev, fwnode);

	return bridge;

err_unregister_sensors:
	cio2_bridge_unregister_sensors(bridge);
err_unregister_cio2:
	software_node_unregister(&cio2_hid_node);
err_put_cio2:
	pci_dev_put(cio2);

        kfree(bridge);
	return ERR_PTR(ret);
}

void cio2_bridge_clean(struct cio2_device *cio2)
{
	/*
	 * On some platforms, the cio2 device's fwnode is initially null until
	 * set by cio2_bridge_init(). We need to reset it to null, otherwise
	 * removing and inserting the module will cause issues.
	 */

	if (is_software_node(dev_fwnode(&cio2->pci_dev->dev)))
		cio2->pci_dev->dev.fwnode = NULL;

	pci_dev_put(cio2->pci_dev);
	cio2_bridge_unregister_sensors(cio2->bridge);
        kfree(cio2->bridge);
	software_node_unregister(&cio2_hid_node);
}
