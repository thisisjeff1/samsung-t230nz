/* drivers/input/misc/gp2ap110s.c - GP2AP110S00F v1.0.1 proximity sensor driver
 *
 * Copyright (C) 2018 Sharp Corporation
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/wakelock.h>
#include <linux/of_gpio.h>
#include <asm/uaccess.h>
#include <linux/sensor/sensors_core.h>
#include "gp2ap110s.h"

#define DEVICE_NAME              "GP2AP110S"
#define CHIP_DEV_VENDOR 	 "SHARP"
#define MODULE_NAME     	 "proximity_sensor"
#define PROX_READ_NUM   	 40 

// Temporary Threshold Settings
#define HIGH_THD	800
#define LOW_THD		550

static int gp2ap_i2c_read(u8 reg, unsigned char *rbuf, int len, struct i2c_client *client)
{
	int               err = -1;
	struct i2c_msg    i2cMsg[2];
	uint8_t           buff;

	if (client == NULL)
		return -ENODEV;

	i2cMsg[0].addr = client->addr;
	i2cMsg[0].flags = 0;
	i2cMsg[0].len = 1;
	i2cMsg[0].buf = &buff;

	buff = reg;

	i2cMsg[1].addr = client->addr;
	i2cMsg[1].flags = I2C_M_RD;
	i2cMsg[1].len = len;
	i2cMsg[1].buf = rbuf;

	err = i2c_transfer(client->adapter, &i2cMsg[0], 2);
	if (err < 0) {
		SENSOR_ERR("i2c transfer error(%d)!!\n", err);
	}

	return err;
}

static int gp2ap_i2c_write(u8 reg, u8 wbuf, struct i2c_client *client)
{
	int               err = 0;
	struct i2c_msg    i2cMsg;
	unsigned char     buff[2];
	int               retry = 10;

	if (client == NULL || (!client->adapter))
		return -ENODEV;

	buff[0] = reg;
	buff[1] = wbuf;

	i2cMsg.addr = client->addr;
	i2cMsg.flags = 0;
	i2cMsg.len = 2;
	i2cMsg.buf = buff;

	while(retry--) {
		err = i2c_transfer(client->adapter, &i2cMsg, 1);
		SENSOR_INFO("0x%x, 0x%x\n", reg, wbuf);
		if (err >= 0)
			return 0;
	}

	SENSOR_ERR("i2c transfer error(%d)!!\n", err);

	return err;
}

static void gp2ap_StartMeasurement(struct gp2ap_data *data)
{
	u8    wdata;
	wdata = 0xA0;

	gp2ap_i2c_write(REG_COM1, wdata, data->client);
}

static void gp2ap_StopMeasurement(struct gp2ap_data *data)
{
	u8    wdata;
	wdata = 0x00;

	gp2ap_i2c_write(REG_COM1, wdata, data->client);
	gp2ap_i2c_write(REG_COM2, wdata, data->client);
}

static void gp2ap_InitData(struct gp2ap_data *data)
{
	u8    wdata;

	SENSOR_INFO("\n");

	wdata = 0x00;    gp2ap_i2c_write(0x81, wdata, data->client);
	wdata = 0x00;    gp2ap_i2c_write(0x82, wdata, data->client);
	wdata = 0x13;    gp2ap_i2c_write(0x83, wdata, data->client);
	wdata = 0x10;    gp2ap_i2c_write(0x85, wdata, data->client);
	wdata = 0x24;    gp2ap_i2c_write(0x86, wdata, data->client);
	wdata = 0x20;    gp2ap_i2c_write(0x87, wdata, data->client);
	wdata = (u8)data->ps_low_th;         gp2ap_i2c_write(0x88, wdata, data->client);
	wdata = (u8)(data->ps_low_th>>8);    gp2ap_i2c_write(0x89, wdata, data->client);
	wdata = (u8)data->ps_high_th;        gp2ap_i2c_write(0x8A, wdata, data->client);
	wdata = (u8)(data->ps_high_th>>8);   gp2ap_i2c_write(0x8B, wdata, data->client);
	wdata = 0x88;    gp2ap_i2c_write(0x8E, wdata, data->client);
	wdata = 0x10;    gp2ap_i2c_write(0xB1, wdata, data->client);
	wdata = 0x02;    gp2ap_i2c_write(0xF0, wdata, data->client);
	wdata = 0x01;    gp2ap_i2c_write(0xF1, wdata, data->client);
}

static void gp2ap_InitDataDynamicCalibration(struct gp2ap_data *data)
{
	u8    wdata;

	SENSOR_INFO("\n");

	wdata = 0x00;    gp2ap_i2c_write(0x81, wdata, data->client);
	wdata = 0x10;    gp2ap_i2c_write(0x82, wdata, data->client);
	wdata = 0x10;    gp2ap_i2c_write(0x83, wdata, data->client);
	wdata = 0x10;    gp2ap_i2c_write(0x85, wdata, data->client);
	wdata = 0x24;    gp2ap_i2c_write(0x86, wdata, data->client);
	wdata = 0x70;    gp2ap_i2c_write(0x87, wdata, data->client);
	wdata = 0x88;    gp2ap_i2c_write(0x8E, wdata, data->client);
	wdata = 0x10;    gp2ap_i2c_write(0xB1, wdata, data->client);
	wdata = 0x01;    gp2ap_i2c_write(0xC1, wdata, data->client);
	wdata = 0x21;    gp2ap_i2c_write(0xC2, wdata, data->client);
	wdata = 0x02;    gp2ap_i2c_write(0xF0, wdata, data->client);
	wdata = 0x01;    gp2ap_i2c_write(0xF1, wdata, data->client);
}

static ssize_t ps_enable_show(struct device *dev,
					struct device_attribute *attr,
						char *buf)
{
	struct gp2ap_data *data = dev_get_drvdata(dev);
	int                 enabled;

	SENSOR_INFO("ps_enable_show\n");

	mutex_lock(&data->mutex);
	enabled = data->ps_enabled;
	mutex_unlock(&data->mutex);

	return sprintf(buf, "%d", enabled);
}

static int gp2ap_ps_onoff(u8 onoff, struct gp2ap_data *data)
{
	SENSOR_INFO("proximity_sensor onoff = %d\n", onoff);

	if (onoff) {
		if (data->dynamic_calib_enabled) {
			gp2ap_InitDataDynamicCalibration(data);
			data->dynamic_calib_done = 0;
			data->auto_calib_offset = 0;
		} else {
			gp2ap_InitData(data);
		}
		gp2ap_StartMeasurement(data);
	} else {
		gp2ap_StopMeasurement(data);
	}

	return 0;
}

static ssize_t gp2ap_ps_enable_store(struct device *dev,
					struct device_attribute *attr, const char *buf, size_t size)
{
	struct gp2ap_data *data = dev_get_drvdata(dev);
	bool new_value;

	SENSOR_INFO("data=%s\n", buf);

	if (sysfs_streq(buf, "1"))
		new_value = true;
	else if (sysfs_streq(buf, "0"))
		new_value = false;
	else {
		SENSOR_ERR("invalid value %d\n", *buf);
		return size;
	}

	SENSOR_INFO("new_value = %d, ps_enabled = %d\n",
		new_value, data->ps_enabled);

	if (!data->ps_enabled && new_value) {
		mutex_lock(&data->mutex);

		enable_irq_wake(data->ps_irq);
		msleep(200);
		enable_irq(data->ps_irq);
		gp2ap_ps_onoff(1, data);
		data->ps_distance = 1;
		data->ps_enabled = new_value;

		mutex_unlock(&data->mutex);

		SENSOR_INFO("proximity_sensor enable!! \n");

		input_report_abs(data->ps_input_dev, ABS_DISTANCE, data->ps_distance);
		input_sync(data->ps_input_dev);
	} else if (data->ps_enabled && !new_value) {
		cancel_delayed_work_sync(&data->dynamic_calib_work);
		data->dynamic_calib_timer_enabled = 0;

		mutex_lock(&data->mutex);

		disable_irq(data->ps_irq);
		gp2ap_ps_onoff(0, data);
		data->ps_distance = 1;
		data->ps_enabled = new_value;
		disable_irq_wake(data->ps_irq);
		data->ps_distance = 1;

		mutex_unlock(&data->mutex);

		SENSOR_INFO("proximity_sensor disable!! \n");
	}

	return size;
}

static DEVICE_ATTR(enable, 0664, ps_enable_show, gp2ap_ps_enable_store);

static struct attribute *ps_attributes[] =
{
	&dev_attr_enable.attr,
	NULL
};

static struct attribute_group ps_attribute_group =
{
	.attrs = ps_attributes
};

static int ps_input_init(struct gp2ap_data *data)
{
	struct input_dev *dev;
	int err = 0;

	/* Create the input device */
	dev = input_allocate_device();
	if (!dev) {
		SENSOR_ERR("%s, input_allocate_device error!!\n", __func__);
		return -ENOMEM;
	}

	dev->name = MODULE_NAME;
	dev->id.bustype = BUS_I2C;
	input_set_drvdata(dev, data);
	input_set_capability(dev, EV_ABS, ABS_DISTANCE);
	input_set_abs_params(dev, ABS_DISTANCE, 0, 1, 0, 0);

	err = input_register_device(dev);

	if (err < 0) {
		input_free_device(dev);
		SENSOR_INFO("%s, input_register_device error(%d)!!\n", __func__, err);
		return err;
	}

	err = sensors_create_symlink(&dev->dev.kobj, dev->name);

	if (err < 0) {
		SENSOR_ERR("create sysfs symlink error\n");
		input_unregister_device(dev);
		return err;
	}

	err = sysfs_create_group(&dev->dev.kobj, &ps_attribute_group);
	if (err) {
		SENSOR_INFO("sysfs_create_group failed[%s]\n", dev->name);
		return err;
	}

	data->ps_input_dev = dev;

	return err;
}

irqreturn_t gp2ap_ps_irq_handler(int irq, void *id_data)
{
	struct gp2ap_data *data = id_data;
	u8 val;

	val = gpio_get_value(data->p_out);

	SENSOR_INFO("val:%d\n", val);

	schedule_work(&data->ps_int_work);

	return IRQ_HANDLED;
}

static void gp2ap_ps_work_int_func(struct work_struct *work)
{
	struct gp2ap_data *data = container_of((struct work_struct *)work, 
					struct gp2ap_data, ps_int_work);
	char  distance;
	u8    rdata;
	u8    rdata_d0[2];
	int   ps_count;

	if (data == NULL)
		return;

	if ((data->dynamic_calib_enabled) &&(data->dynamic_calib_done == 0)) {
		SENSOR_INFO("dynamic calibration done \n");
		data->dynamic_calib_done = 1;
		gp2ap_InitData(data);
		gp2ap_StartMeasurement(data);
		schedule_delayed_work(&data->dynamic_calib_work, 0);
		return;
	}

	mutex_lock(&data->mutex);

	// 0 : proximity, 1 : away 
	gp2ap_i2c_read(0x81, &rdata, sizeof(rdata), data->client);
	if ((rdata & 0x08) == 0x08)
		distance = 0;
	else
		distance = 1;
	if (data->ps_distance != distance) {
		data->ps_distance = distance;
		SENSOR_INFO("proximity:%d\n", distance);
	}

	gp2ap_i2c_read(REG_D0_LSB, rdata_d0, sizeof(rdata_d0), data->client);
	ps_count = (rdata_d0[1] << 8) | rdata_d0[0];

	SENSOR_INFO("proximity:%d, ps_count:%d\n", distance, ps_count);

	mutex_unlock(&data->mutex);

	input_report_abs(data->ps_input_dev, ABS_DISTANCE, distance);
	input_sync(data->ps_input_dev);
}

static void gp2ap_ps_dyanmic_func(struct work_struct *work)
{
	struct gp2ap_data *data = container_of((struct delayed_work *)work,
				struct gp2ap_data, dynamic_calib_work);
	u8 rdata[2];

	gp2ap_i2c_read(REG_D0_LSB, rdata, sizeof(rdata), data->client);
	data->ps_count = (rdata[1] << 8) | rdata[0];

	SENSOR_INFO("ps_dynamic_func, ps_count:%d\n", data->ps_count);

	// ps_distance:1 --> far 
	if ((data->ps_distance == 1) && (data->ps_count == 0)) {
		SENSOR_INFO("zero detection!!!\n");
		data->dynamic_calib_timer_enabled = 0;
		data->auto_calib_offset = 0;
		gp2ap_StopMeasurement(data);
		gp2ap_ps_onoff(1, data);
	} else {
		schedule_delayed_work(&data->dynamic_calib_work,
			msecs_to_jiffies(data->dynamic_calib_delay));
	}
}

static int gp2ap_setup_irq(struct gp2ap_data *gp2ap)
{
	int ret;
	ret = gpio_request(gp2ap->p_out, "gpio_proximity_out");

	if (ret < 0) {
		SENSOR_ERR("gpio %d request failed (%d)\n", gp2ap->p_out, ret);
		return ret;
	}

	ret = gpio_direction_input(gp2ap->p_out);
	if (ret < 0) {
		SENSOR_ERR("failed gpio %d as input (%d)\n", gp2ap->p_out, ret);
		goto err_gpio_direction_input;
	}

	gp2ap->ps_irq = gpio_to_irq(gp2ap->p_out);
		ret = request_irq(gp2ap->ps_irq, gp2ap_ps_irq_handler,
			  IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "proximity_int", gp2ap);
	
	if (ret < 0) {
		SENSOR_ERR("request_irq(%d) failed for gpio %d (%d)\n",
			gp2ap->ps_irq, gp2ap->p_out, ret);
		goto err_request_irq;
	}

	SENSOR_INFO("request_irq(%d) success (%d)\n", gp2ap->ps_irq, gp2ap->p_out);

	disable_irq(gp2ap->ps_irq);

	goto done;
err_request_irq:
err_gpio_direction_input:
	gpio_free(gp2ap->p_out);
done:
	return ret;
}

static ssize_t name_read(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", DEVICE_NAME);
}

static ssize_t vendor_read(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", CHIP_DEV_VENDOR);
}

static ssize_t proximity_register_read_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct gp2ap_data *data = dev_get_drvdata(dev);

	u8 reg = 0;
	int offset = 0;
	u8 val = 0;

	for (reg = 0x80; reg <= 0x8E; reg++) {
		gp2ap_i2c_read(reg, &val, sizeof(val), data->client);
		SENSOR_INFO("Read Reg: 0x%2x Value: 0x%2x\n", reg, val);
		offset += snprintf(buf + offset, PAGE_SIZE - offset,
			"Reg: 0x%2x Value: 0x%2x\n", reg, val);
	}

	for (reg = 0x90; reg <= 0x91; reg++) {
		gp2ap_i2c_read(reg, &val, sizeof(val), data->client);
		SENSOR_INFO("Read Reg: 0x%2x Value: 0x%2x\n", reg, val);
		offset += snprintf(buf + offset, PAGE_SIZE - offset,
			"Reg: 0x%2x Value: 0x%2x\n", reg, val);
	}

	gp2ap_i2c_read(0xA0, &val, sizeof(val), data->client);
	SENSOR_INFO("Read Reg: 0x%2x Value: 0x%2x\n", reg, val);
	offset += snprintf(buf + offset, PAGE_SIZE - offset,
		"Reg: 0x%2x Value: 0x%2x\n", reg, val);

	gp2ap_i2c_read(0xB1, &val, sizeof(val), data->client);
	SENSOR_INFO("Read Reg: 0x%2x Value: 0x%2x\n", reg, val);
	offset += snprintf(buf + offset, PAGE_SIZE - offset,
		"Reg: 0x%2x Value: 0x%2x\n", reg, val);

	for (reg = 0xC0; reg <= 0xC2; reg++) {
		gp2ap_i2c_read(reg, &val, sizeof(val), data->client);
		SENSOR_INFO("Read Reg: 0x%2x Value: 0x%2x\n", reg, val);
		offset += snprintf(buf + offset, PAGE_SIZE - offset,
			"Reg: 0x%2x Value: 0x%2x\n", reg, val);
	}

	for (reg = 0xF0; reg <= 0xF1; reg++) {
		gp2ap_i2c_read(reg, &val, sizeof(val), data->client);
		SENSOR_INFO("Read Reg: 0x%2x Value: 0x%2x\n", reg, val);
		offset += snprintf(buf + offset, PAGE_SIZE - offset,
			"Reg: 0x%2x Value: 0x%2x\n", reg, val);
	}

	return offset;
}

static ssize_t proximity_register_write_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct gp2ap_data *data = dev_get_drvdata(dev);

	unsigned int reg, val;
	int ret;

	if (sscanf(buf, "%2x,%2x", &reg, &val) != 2) {
		SENSOR_ERR("invalid value\n");
		return count;
	}

	ret = gp2ap_i2c_write(reg, val, data->client);
	if(ret < 0)
		SENSOR_ERR("failed %d\n", ret);
	else
		SENSOR_INFO("Register(0x%2x) data(0x%2x)\n", reg, val);

    return count;
}

static ssize_t proximity_cal_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct gp2ap_data *data = dev_get_drvdata(dev);
	u8 value;

	gp2ap_i2c_read(REG_DYNAMIC_CAL_RESULT, &value, sizeof(value), data->client);
	
	return snprintf(buf, PAGE_SIZE, "%d,%d,%d\n",
		value, data->ps_high_th, data->ps_low_th);
}

static ssize_t proximity_thresh_high_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct gp2ap_data *data = dev_get_drvdata(dev);
	return scnprintf(buf, PAGE_SIZE, "%d\n", data->ps_high_th);
}

static ssize_t proximity_thresh_high_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct gp2ap_data *data = dev_get_drvdata(dev);
	u16 value = 0;
	u8 wdata;
	int ret;

	ret = kstrtou16(buf, 10, &value);
	if (ret < 0) {
		SENSOR_ERR("kstrtoul failed, ret=0x%x\n", ret);
		return ret;
	}

	SENSOR_INFO("thresh: %d\n",  value);

	data->ps_high_th = value;
	wdata = (u8)data->ps_high_th;
	gp2ap_i2c_write(0x8A, wdata, data->client);

	wdata = (u8)(data->ps_high_th>>8);
	gp2ap_i2c_write(0x8B, wdata, data->client);

	return size;
}

static ssize_t proximity_thresh_low_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct gp2ap_data *data = dev_get_drvdata(dev);
	return scnprintf(buf, PAGE_SIZE, "%d\n", data->ps_low_th);
}

static ssize_t proximity_thresh_low_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct gp2ap_data *data = dev_get_drvdata(dev);
	u16 value = 0;
	u8 wdata;
	int ret;

	ret = kstrtou16(buf, 10, &value);
	if (ret < 0) {
		SENSOR_ERR("kstrtoul failed, ret=0x%x\n", ret);
		return ret;
	}

	SENSOR_INFO("thresh: %d\n",  value);

	data->ps_low_th = value;
	wdata = (u8)data->ps_low_th;
	gp2ap_i2c_write(0x88, wdata, data->client);

	wdata = (u8)(data->ps_low_th>>8);
	gp2ap_i2c_write(0x89, wdata, data->client);

	return size;
}

static ssize_t proximity_avg_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct gp2ap_data *data = dev_get_drvdata(dev);
	return snprintf(buf, PAGE_SIZE, "%d,%d,%d\n", data->avg[0],
		data->avg[1], data->avg[2]);
}

static ssize_t proximity_avg_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct gp2ap_data *data =  dev_get_drvdata(dev);
	bool new_value = false;

	if (sysfs_streq(buf, "1"))
		new_value = true;
	else if (sysfs_streq(buf, "0"))
		new_value = false;
	else {
		SENSOR_ERR("invalid value %d\n", *buf);
		return -EINVAL;
	}

	SENSOR_INFO("average enable = %d\n", new_value);

	if (new_value) {
		hrtimer_start(&data->prox_timer, data->prox_poll_delay,
			HRTIMER_MODE_REL);
	} else if (!new_value) {
		hrtimer_cancel(&data->prox_timer);
		cancel_work_sync(&data->work_prox);
	}

	return size;
}

static uint32_t gp2ap_get_proximity_adc(struct gp2ap_data *data)
{
	u8 value[2];
	int ret;

	ret = gp2ap_i2c_read(REG_D0_LSB, value, sizeof(value), data->client);
	if (ret < 0) {
		SENSOR_ERR("fail, ret=%d\n", ret);
		return ret;
	}

	return (value[0] | (value[1] << 8));
}

static ssize_t proximity_state_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{   
	struct gp2ap_data *data = dev_get_drvdata(dev);
	uint32_t ps_data;

	ps_data = gp2ap_get_proximity_adc(data);

	return snprintf(buf, PAGE_SIZE, "%d\n", ps_data);
}

static ssize_t proximity_trim_show(struct device *dev,
	struct device_attribute *attr, char *buf)

{
	struct gp2ap_data *data = dev_get_drvdata(dev);
	u8 value;

	gp2ap_i2c_read(REG_DYNAMIC_CAL_RESULT, &value, sizeof(value), data->client);

	return snprintf(buf, PAGE_SIZE, "%d\n", value);
}

static ssize_t dynamic_calib_enabled_show(struct device *dev,
					struct device_attribute *attr, char *buf)
{
	struct input_dev   *input_dev = to_input_dev(dev);
	struct gp2ap_data  *data = input_get_drvdata(input_dev);

	SENSOR_INFO("%s : %d\n", __func__, data->dynamic_calib_enabled);

	return sprintf(buf, "dynamic_calib_enabled:%d", data->dynamic_calib_enabled);
	
}

static ssize_t dynamic_calib_enabled_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct gp2ap_data *data = dev_get_drvdata(dev);
	char               *endp;
	int                 enabled = simple_strtoul(buf, &endp, 10);

	SENSOR_INFO("dynamic_calib_enabled = %s\n", buf);

	if (!(endp != buf &&
			(endp == (buf + strlen(buf))
				|| (endp == (buf + strlen(buf) - 1) && *endp == '\n')))) {
		SENSOR_INFO("invalid num : %s", buf);
		return count;
	}

	data->dynamic_calib_enabled = enabled;

	if (data->dynamic_calib_enabled) {
		data->ps_low_th  = data->calib_target + 205;
		data->ps_high_th = data->calib_target + 305;
	} else {
		data->ps_low_th  = 205;
		data->ps_high_th = 305;
	}

	return count;
}

static DEVICE_ATTR(name, 0444, name_read, NULL);
static DEVICE_ATTR(vendor, 0444, vendor_read, NULL);
static DEVICE_ATTR(raw_data, 0444, proximity_state_show, NULL);
static DEVICE_ATTR(prox_register, 0444, proximity_register_read_show, proximity_register_write_store);
static DEVICE_ATTR(prox_trim, 0444, proximity_trim_show, NULL);
static DEVICE_ATTR(prox_cal, 0664, proximity_cal_show, NULL);
static DEVICE_ATTR(thresh_high, S_IRUGO | S_IWUSR | S_IWGRP,
	proximity_thresh_high_show, proximity_thresh_high_store);
static DEVICE_ATTR(thresh_low, S_IRUGO | S_IWUSR | S_IWGRP,
	proximity_thresh_low_show, proximity_thresh_low_store);
static DEVICE_ATTR(dynamic_calib_enabled, 0664,
		dynamic_calib_enabled_show, dynamic_calib_enabled_store);
static DEVICE_ATTR(prox_avg, 0644,
		proximity_avg_show, proximity_avg_store);

static struct device_attribute *proximity_attrs[] = {
	&dev_attr_name,
	&dev_attr_vendor,
	&dev_attr_raw_data,
	&dev_attr_thresh_low,
	&dev_attr_thresh_high,
	&dev_attr_prox_cal,
	&dev_attr_prox_register,
	&dev_attr_prox_trim,
	&dev_attr_dynamic_calib_enabled,
	&dev_attr_prox_avg,
	NULL,
};

static enum hrtimer_restart gp2ap_prox_timer_func(struct hrtimer *timer)
{
	struct gp2ap_data *data = container_of(timer,
		struct gp2ap_data, prox_timer);
	queue_work(data->prox_wq, &data->work_prox);
	hrtimer_forward_now(&data->prox_timer, data->prox_poll_delay);
	return HRTIMER_RESTART;
}

static void proximity_get_avg_val(struct gp2ap_data *data)
{
	int min = 0, max = 0, avg = 0;
	int i;
	u16 ps_data;

	for (i = 0; i < PROX_READ_NUM; i++) {
		msleep(40);
		ps_data = gp2ap_get_proximity_adc(data);
		avg += ps_data;
		if (!i)
			min = ps_data;
		else if (ps_data < min)
			min = ps_data;
		if (ps_data > max)
			max = ps_data;
	}

	avg /= PROX_READ_NUM;
	data->avg[0] = min;
	data->avg[1] = avg;
	data->avg[2] = max;
}

static void gp2ap_work_func_prox(struct work_struct *work)
{
	struct gp2ap_data *data = container_of(work,
		struct gp2ap_data, work_prox);

	proximity_get_avg_val(data);
}

static int gp2ap110s_parse_dt(struct device *dev, struct gp2ap_data *data)
{
	struct device_node *np = dev->of_node;
	enum of_gpio_flags flags;
	int ret;

	//of_property_read_u32(np, "gp2ap110s,irq_flags", &data->irq_flags);
	data->p_out =  of_get_named_gpio_flags(np, "gp2ap110s,gpio_int", 0, &flags);
	data->vled_ldo = of_get_named_gpio_flags(np, "gp2ap110s,vled", 0, &flags);

	ret = of_property_read_u32(np, "gp2ap110s,ps_high_th",
		&data->ps_high_th);
	if (ret < 0) {
		pr_err("[SENSOR]: %s - Cannot set ps_high_th through DTSI\n",
			__func__);
		data->ps_high_th = HIGH_THD;
	}

	ret = of_property_read_u32(np, "gp2ap110s,ps_low_th",
		&data->ps_low_th);
	if (ret < 0) {
		pr_err("[SENSOR]: %s - Cannot set ps_low_th through DTSI\n",
			__func__);
		data->ps_low_th = LOW_THD;
	}

	SENSOR_INFO("data->p_out : %d\n", data->p_out);

	return 0;
}

static int gp2ap_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	struct gp2ap_data *gp2ap;
	u8 value = 0;
	int err = 0;

	SENSOR_INFO("gp2ap_probe start !! \n");

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		SENSOR_ERR("i2c functionality failed\n");
		return -ENOMEM;
	}

	/* Allocate memory for driver data */
	gp2ap = kzalloc(sizeof(struct gp2ap_data), GFP_KERNEL);
	if (!gp2ap) {
		SENSOR_ERR("failed memory alloc\n");
		err = -ENOMEM;
		goto err_mem_alloc;
	}

	gp2ap->client = client;
	i2c_set_clientdata(client, gp2ap);

	/* Check if the device is there or not. (Shutdown operation) */
	value = 0x00;
	err = gp2ap_i2c_write(REG_COM1, value, gp2ap->client);
	if (err < 0) {
		SENSOR_ERR("gp2ap is not connected.(%d)\n", err);
		goto err_no_device;
	}

	err = gp2ap110s_parse_dt(&client->dev, gp2ap);
	if (err) {
		SENSOR_ERR("error in device tree");
		goto err_device_tree;
	}

	mutex_init(&gp2ap->mutex);

	gp2ap->ps_enabled   = 0;
	gp2ap->ps_distance  = 1;
	gp2ap->calib_target = 63;
	gp2ap->dynamic_calib_enabled = 1;

	if (gp2ap->dynamic_calib_enabled) {
		gp2ap_InitDataDynamicCalibration(gp2ap);
	} else {
		gp2ap_InitData(gp2ap);
		gp2ap->ps_low_th = 205;
		gp2ap->ps_high_th = 305;
	}

	err = ps_input_init(gp2ap);

	if (err < 0) {
		SENSOR_ERR("failed to get input dev\n");
		goto err_ps_input_device;
	}

	err = sensors_register(&gp2ap->dev, gp2ap, proximity_attrs, MODULE_NAME);
	if (err < 0) {
		SENSOR_INFO("could not sensors_register\n");
		goto err_sensors_register;
	}

	/* For factory test mode, we use timer to get average proximity data. */
	/* prox_timer settings. we poll for light values using a timer. */
	hrtimer_init(&gp2ap->prox_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	gp2ap->prox_poll_delay = ns_to_ktime(2000 * NSEC_PER_MSEC);/*2 sec*/
	gp2ap->prox_timer.function = gp2ap_prox_timer_func;
	/* the timer just fires off a work queue request.  we need a thread
	to read the i2c (can be slow and blocking). */
	gp2ap->prox_wq = create_singlethread_workqueue("gp2a_prox_wq");
	if (!gp2ap->prox_wq) {
		err = -ENOMEM;
		SENSOR_ERR("could not create prox workqueue\n");
		goto err_create_prox_workqueue;
	}

	INIT_WORK(&gp2ap->work_prox, gp2ap_work_func_prox);
	INIT_WORK(&gp2ap->ps_int_work, gp2ap_ps_work_int_func);
	INIT_DELAYED_WORK(&gp2ap->dynamic_calib_work, gp2ap_ps_dyanmic_func);

	gp2ap->dynamic_calib_timer_enabled = 0;
	gp2ap->dynamic_calib_delay = DYNAMIC_CALIBRATION_DELAY;

	err = gp2ap_setup_irq(gp2ap);

	if (err) {
		SENSOR_ERR("could not setup irq\n");
		goto err_setup_irq;
	}

	SENSOR_INFO("success\n");

	return 0;
err_setup_irq:
	destroy_workqueue(gp2ap->prox_wq);
err_create_prox_workqueue:
	sensors_unregister(gp2ap->dev, proximity_attrs);
err_sensors_register:
	sensors_remove_symlink(&gp2ap->ps_input_dev->dev.kobj,
			gp2ap->ps_input_dev->name);
	sysfs_remove_group(&gp2ap->ps_input_dev->dev.kobj,
				&ps_attribute_group);	
	input_unregister_device(gp2ap->ps_input_dev);
err_ps_input_device:    
	mutex_destroy(&gp2ap->mutex);
err_device_tree:
err_no_device:
	kfree(gp2ap);
err_mem_alloc:
	SENSOR_ERR("failed\n");
	return err;
}

static int gp2ap_remove(struct i2c_client *client)
{
	struct gp2ap_data *data = i2c_get_clientdata(client);
	u8 val;

	SENSOR_INFO("gp2ap_remove\n");

	if (data != NULL) {
		val = 0x00;
		gp2ap_i2c_write(REG_COM1, val, data->client);
		if (data->dynamic_calib_timer_enabled) {
			flush_scheduled_work();
			cancel_delayed_work(&data->dynamic_calib_work);
		}

		if (data->ps_enabled)
			disable_irq(data->ps_irq);

		free_irq(data->ps_irq, data);
		destroy_workqueue(data->prox_wq);
		sensors_unregister(data->dev, proximity_attrs);
		sensors_remove_symlink(&data->ps_input_dev->dev.kobj,
			data->ps_input_dev->name);
		sysfs_remove_group(&data->ps_input_dev->dev.kobj, &ps_attribute_group);
		input_unregister_device(data->ps_input_dev);	  	
		mutex_destroy(&data->mutex);
		kfree(data);
	}

	return 0;
}

static void gp2ap_shutdown(struct i2c_client *client)
{
	struct gp2ap_data *data = i2c_get_clientdata(client);
	u8 val;

	SENSOR_INFO("gp2ap_shutdown\n");

	if (data != NULL) {
		val = 0x00;
		gp2ap_i2c_write(REG_COM1, val, data->client);
		if (data->dynamic_calib_timer_enabled) {
			flush_scheduled_work();
			cancel_delayed_work(&data->dynamic_calib_work);
		}

		if (data->ps_enabled)
			disable_irq(data->ps_irq);
		}
}

static int gp2ap_suspend(struct device *pdev)
{
	struct gp2ap_data *data = dev_get_drvdata(pdev);

	SENSOR_INFO("is called.\n");

	if (data->dynamic_calib_timer_enabled)
		cancel_delayed_work(&data->dynamic_calib_work);

	if (data->ps_enabled)
		disable_irq(data->ps_irq);

	return 0;
}

static int gp2ap_resume(struct device *pdev)
{
	struct gp2ap_data *data = dev_get_drvdata(pdev);

	SENSOR_INFO("is called.\n");

	if (data->dynamic_calib_timer_enabled)
		schedule_delayed_work(&data->dynamic_calib_work, 0);
	if (data->ps_enabled)
		enable_irq(data->ps_irq);

	return 0;
}

static const struct dev_pm_ops gp2ap_pm_ops = {
	.suspend    = gp2ap_suspend,
	.resume     = gp2ap_resume
};

static const struct i2c_device_id gp2ap_device_id[] =
{
	{ "gp2a", 0 },
	{ }
};

static struct of_device_id gp2ap_i2c_match_table[] = {
  { .compatible = "sharp,gp2ap110s",},
  { },
};

static struct i2c_driver gp2ap_i2c_driver = {
	.driver = {
		.name = "gp2a",
		.owner = THIS_MODULE,
		.of_match_table = gp2ap_i2c_match_table,
		.pm = &gp2ap_pm_ops
	},
	.probe		= gp2ap_i2c_probe,
	.shutdown 	= gp2ap_shutdown,
	.remove 	= gp2ap_remove,
	.id_table	= gp2ap_device_id,
};

module_i2c_driver(gp2ap_i2c_driver);

MODULE_AUTHOR("Samsung Electronics");
MODULE_DESCRIPTION("Proximity Sensor driver for gp2ap110s00f");
MODULE_LICENSE("GPL");