/* SPDX-License-Identifier: GPL-2.0 */
/* Author: Dan Scally <djrscally@gmail.com> */
#ifndef __CIO2_BRIDGE_H
#define __CIO2_BRIDGE_H

#define MAX_CONNECTED_DEVICES			4
#define SWNODE_SENSOR_HID			0
#define SWNODE_SENSOR_PORT			1
#define SWNODE_SENSOR_ENDPOINT			2
#define SWNODE_CIO2_PORT			3
#define SWNODE_CIO2_ENDPOINT			4
#define SWNODE_NULL_TERMINATOR			5

#define CIO2_HID				"INT343E"
#define CIO2_PCI_ID				0x9d32

#define ENDPOINT_SENSOR				0
#define ENDPOINT_CIO2				1

#define NODE_SENSOR(_HID, _PROPS)		\
	((const struct software_node) {		\
		.name = _HID,			\
		.properties = _PROPS,		\
	})

#define NODE_PORT(_PORT, _SENSOR_NODE)		\
	((const struct software_node) {		\
		_PORT,				\
		_SENSOR_NODE,			\
	})

#define NODE_ENDPOINT(_EP, _PORT, _PROPS)	\
	((const struct software_node) {		\
		_EP,				\
		_PORT,				\
		_PROPS,				\
	})

#define IPU3_SENSOR(_CLIENT)		\
	{					\
		.i2c_id = {			\
			{_CLIENT, 0},		\
			{ },			\
		}				\
	}

struct ipu3_sensor {
	const struct i2c_device_id i2c_id[2];
};

struct sensor {
	char name[ACPI_ID_LEN];
	struct device *dev;
	struct acpi_device *adev;
	struct software_node swnodes[6];
	struct property_entry dev_properties[3];
	struct property_entry ep_properties[4];
	struct property_entry cio2_properties[3];
	u32 *data_lanes;

	struct fwnode_handle *fwnode;
	struct i2c_driver *old_drv;
	struct i2c_driver new_drv;
};

struct cio2_bridge {
	int n_sensors;
	struct sensor sensors[MAX_CONNECTED_DEVICES];

	struct fwnode_handle *cio2_fwnode;
};

/* Data representation as it is in ACPI SSDB buffer */
struct sensor_bios_data_packed {
	u8 version;
	u8 sku;
	u8 guid_csi2[16];
	u8 devfunction;
	u8 bus;
	u32 dphylinkenfuses;
	u32 clockdiv;
	u8 link;
	u8 lanes;
	u32 csiparams[10];
	u32 maxlanespeed;
	u8 sensorcalibfileidx;
	u8 sensorcalibfileidxInMBZ[3];
	u8 romtype;
	u8 vcmtype;
	u8 platforminfo;
	u8 platformsubinfo;
	u8 flash;
	u8 privacyled;
	u8 degree;
	u8 mipilinkdefined;
	u32 mclkspeed;
	u8 controllogicid;
	u8 reserved1[3];
	u8 mclkport;
	u8 reserved2[13];
} __packed__;

/* Fields needed by bridge driver */
struct sensor_bios_data {
	struct device *dev;
	u8 link;
	u8 lanes;
	u8 degree;
	u32 mclkspeed;
};

#endif
