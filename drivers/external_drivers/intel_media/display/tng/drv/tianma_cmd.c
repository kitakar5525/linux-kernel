/*
 * Copyright © 2015 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Authors: Sophia Gong <sophia.gong@intel.com>
 *
 */

#include "mdfld_dsi_dbi.h"
#include "mdfld_dsi_esd.h"
#include <asm/intel_scu_pmic.h>
#include <asm/intel_mid_rpmsg.h>
#include <asm/intel_mid_remoteproc.h>
#include <linux/lnw_gpio.h>

#include "displays/tianma_cmd.h"
#include "tianma_init.h"

static int select_init_code;

static int __init parse_panel_init_code(char *arg)
{
	sscanf(arg, "%d", &select_init_code);

	return 1;
}
early_param("panel_init_code", parse_panel_init_code);

static int readback_initcode(struct mdfld_dsi_config *);

static int mipi_reset_gpio;
static int bias_en_gpio;

static
int tianma_cmd_drv_ic_init(struct mdfld_dsi_config *dsi_config)
{
	struct mdfld_dsi_pkg_sender *sender
		= mdfld_dsi_get_pkg_sender(dsi_config);
	int err = 0;

	PSB_DEBUG_ENTRY("\n");

	if (!sender) {
		DRM_ERROR("Cannot get sender\n");
		return -EINVAL;
	}

	msleep(120);

	/* set LCD panel CMD1 */
	err = mdfld_dsi_send_mcs_short_lp(sender,
			0xff, 0x10, 1,
			MDFLD_DSI_SEND_PACKAGE);
	if (err) {
		DRM_ERROR("%s: %d: 0xff cmd\n",
		__func__, __LINE__);
		goto ic_init_err;
	}

	/* set DC VCOM */
	err = mdfld_dsi_send_mcs_short_lp(sender,
			0xb3, 0x00, 1,
			MDFLD_DSI_SEND_PACKAGE);
	if (err) {
		DRM_ERROR("%s: %d: 0xb3 cmd\n",
		__func__, __LINE__);
		goto ic_init_err;
	}

	/* set pixel format */
	err = mdfld_dsi_send_mcs_short_lp(sender,
			0xc0, 0x01, 1,
			MDFLD_DSI_SEND_PACKAGE);
	if (err) {
		DRM_ERROR("%s: %d: 0xc0 cmd\n",
		__func__, __LINE__);
		goto ic_init_err;
	}

	/* set panel command mode */
	err = mdfld_dsi_send_mcs_short_lp(sender,
			0xbb, 0x10, 1,
			MDFLD_DSI_SEND_PACKAGE);
	if (err) {
		DRM_ERROR("%s: %d: 0xbb cmd\n",
		__func__, __LINE__);
		goto ic_init_err;
	}

	/* set LCD panel CMD1 */
	err = mdfld_dsi_send_mcs_short_lp(sender,
			0xff, 0x10, 1,
			MDFLD_DSI_SEND_PACKAGE);
	if (err) {
		DRM_ERROR("%s: %d: 0xfb cmd\n",
		__func__, __LINE__);
		goto ic_init_err;
	}

	/* turn off MTP reload */
	err = mdfld_dsi_send_mcs_short_lp(sender,
			0xfb, 0x01, 1,
			MDFLD_DSI_SEND_PACKAGE);
	if (err) {
		DRM_ERROR("%s: %d: 0xfb cmd\n",
		__func__, __LINE__);
		goto ic_init_err;
	}

	readback_initcode(dsi_config);

	return 0;

ic_init_err:
	err = -EIO;
	return err;
}

static
int tianma_cmd_drv_ic_fullinit(struct mdfld_dsi_config *dsi_config)
{
	struct mdfld_dsi_pkg_sender *sender
		= mdfld_dsi_get_pkg_sender(dsi_config);
	int err = 0;
	int i;
	u8 *cmd, *arg;

	PSB_DEBUG_ENTRY("\n");

	if (!sender) {
		DRM_ERROR("Cannot get sender\n");
		return -EINVAL;
	}

	msleep(120);

	cmd = (u8 *)tianma_init_code;
	arg = cmd + 1;

	for (i = 0; i < ARRAY_SIZE(tianma_init_code); i++) {
		err = mdfld_dsi_send_mcs_short_lp(sender,
				*cmd, *arg, 1,
				MDFLD_DSI_SEND_PACKAGE);
		cmd += 2;
		arg += 2;
		if (err) {
			DRM_ERROR("%s: %d: 0xff cmd\n",
			__func__, __LINE__);
			goto ic_init_err2;
		}

#ifdef TIANMA_DEBUG
		DRM_INFO("panel init: %x=%x\n", *cmd, *arg);
#endif
	}

ic_init_err2:
	if (err)
		err = -EIO;
	readback_initcode(dsi_config);
	return err;
}

static
void tianma_cmd_controller_init(
		struct mdfld_dsi_config *dsi_config)
{
	struct mdfld_dsi_hw_context *hw_ctx =
				&dsi_config->dsi_hw_context;

	PSB_DEBUG_ENTRY("\n");

	/*reconfig lane configuration*/
	dsi_config->lane_count = 1;
	dsi_config->lane_config = MDFLD_DSI_DATA_LANE_2_2;

	/* DSI PLL 400 MHz, set it to 0 for 800 MHz */
	hw_ctx->cck_div = 1;
	hw_ctx->pll_bypass_mode = 0;

	hw_ctx->mipi_control = 0x0;
	hw_ctx->intr_en = 0xFFFFFFFF;
	hw_ctx->hs_tx_timeout = 0xFFFFFF;
	hw_ctx->lp_rx_timeout = 0xFFFFFF;
	hw_ctx->device_reset_timer = 0xffff;
	hw_ctx->turn_around_timeout = 0x1a;
	hw_ctx->high_low_switch_count = 0x21;
	hw_ctx->clk_lane_switch_time_cnt = 0x21000f;
	hw_ctx->lp_byteclk = 0x5;
	hw_ctx->dphy_param = 0x25155b1e;
	hw_ctx->eot_disable = 0x3;
	hw_ctx->init_count = 0xf0;
	hw_ctx->dbi_bw_ctrl = 1390;
	hw_ctx->hs_ls_dbi_enable = 0x0;
	hw_ctx->dsi_func_prg = ((DBI_DATA_WIDTH_OPT2 << 13) |
				dsi_config->lane_count);
		hw_ctx->mipi = PASS_FROM_SPHY_TO_AFE |
			BANDGAP_CHICKEN_BIT |
			TE_TRIGGER_GPIO_PIN;
}

static
int tianma_cmd_panel_connection_detect(
	struct mdfld_dsi_config *dsi_config)
{
	int status;
	int pipe = dsi_config->pipe;

	PSB_DEBUG_ENTRY("\n");

	if (pipe == 0) {
		status = MDFLD_DSI_PANEL_CONNECTED;
	} else {
		DRM_INFO("%s: do NOT support dual panel\n",
		__func__);
		status = MDFLD_DSI_PANEL_DISCONNECTED;
	}

	return status;
}

static
int tianma_cmd_power_on(
	struct mdfld_dsi_config *dsi_config)
{

	struct mdfld_dsi_pkg_sender *sender =
		mdfld_dsi_get_pkg_sender(dsi_config);
	int err = 0;

	PSB_DEBUG_ENTRY("\n");

	usleep_range(300000, 301000);

	/* Exit sleep mode */
	err = mdfld_dsi_send_mcs_short_lp(sender,
			exit_sleep_mode, 0, 0,
			MDFLD_DSI_SEND_PACKAGE);
	if (err) {
		DRM_ERROR("%s: %d: Exit Sleep Mode\n",
		__func__, __LINE__);
		goto power_err;
	}

	msleep(120);

	/* turn on display */
	err = mdfld_dsi_send_mcs_short_lp(sender,
			set_display_on, 0, 0,
			MDFLD_DSI_SEND_PACKAGE);
	if (err) {
		DRM_ERROR("%s: %d: Set Display On\n", __func__, __LINE__);
		goto power_err;
	}

	msleep(60);

	/* set backlight on */
	err = mdfld_dsi_send_mcs_short_lp(sender,
			write_ctrl_display, 0x2c, 1,
			MDFLD_DSI_SEND_PACKAGE);
	if (err) {
		DRM_ERROR("%s: %d: Set Backlight On\n",
		__func__, __LINE__);
		goto power_err;
	}

	/* set backlight brightness */
	err = mdfld_dsi_send_mcs_short_lp(sender,
			write_display_brightness, 0xff, 1,
			MDFLD_DSI_SEND_PACKAGE);
	if (err) {
		DRM_ERROR("%s: %d: Set Backlight Brightness\n",
		__func__, __LINE__);
		goto power_err;
	}

	/* set CABC/IE disable */
	err = mdfld_dsi_send_mcs_short_lp(sender,
			write_ctrl_cabc, 0x00, 1,
			MDFLD_DSI_SEND_PACKAGE);
	if (err) {
		DRM_ERROR("%s: %d: Set CABC/IE Disable\n",
		__func__, __LINE__);
		goto power_err;
	}

	/* set tearing effect on */
	err = mdfld_dsi_send_mcs_short_lp(sender,
			set_tear_on, 0x00, 1,
			MDFLD_DSI_SEND_PACKAGE);
	if (err) {
		DRM_ERROR("%s: %d: Set Tear On\n",
		__func__, __LINE__);
		goto power_err;
	}

	usleep_range(20000, 20100);
	return 0;

power_err:
	return err;
}

static int tianma_cmd_power_off(
		struct mdfld_dsi_config *dsi_config)
{
	struct mdfld_dsi_pkg_sender *sender =
		mdfld_dsi_get_pkg_sender(dsi_config);
	int err;

	PSB_DEBUG_ENTRY("\n");

	if (!sender) {
		DRM_ERROR("Failed to get DSI packet sender\n");
		return -EINVAL;
	}

	err = mdfld_dsi_send_mcs_short_lp(sender,
			write_ctrl_display, 0x00, 1,
			MDFLD_DSI_SEND_PACKAGE);
	if (err) {
		DRM_ERROR("%s: %d: Set Backlight Off\n",
		__func__, __LINE__);
		goto power_off_err;
	}

	/* assert panel reset : delay > 85 ms */
	usleep_range(85000, 85100);
	gpio_set_value(mipi_reset_gpio, 0);

	return 0;

power_off_err:
	err = -EIO;
	return err;
}

static
int tianma_cmd_set_brightness(
		struct mdfld_dsi_config *dsi_config,
		int level)
{
	struct mdfld_dsi_pkg_sender *sender =
		mdfld_dsi_get_pkg_sender(dsi_config);
	u8 duty_val = 0;

	if (!sender) {
		DRM_ERROR("Failed to get DSI packet sender\n");
		return -EINVAL;
	}

	duty_val = (0xFF * level) / 255;
	mdfld_dsi_send_mcs_short_hs(sender,
			write_display_brightness, duty_val, 1,
			MDFLD_DSI_SEND_PACKAGE);
	return 0;
}

static
int tianma_cmd_panel_reset(
		struct mdfld_dsi_config *dsi_config)
{
	u8 value;

	PSB_DEBUG_ENTRY("\n");

	gpio_direction_output(bias_en_gpio, 1);
	gpio_direction_output(mipi_reset_gpio, 0);

	gpio_set_value(bias_en_gpio, 1);
	gpio_set_value(mipi_reset_gpio, 0);

	/* turn on VSWITCH */
	intel_scu_ipc_ioread8(0xAF, &value);
	intel_scu_ipc_iowrite8(0xAF, value | 0x2);

	usleep_range(10000, 12000);

	gpio_set_value(mipi_reset_gpio, 1);

	usleep_range(10000, 12000);

	return 0;
}

static
int tianma_cmd_exit_deep_standby(
		struct mdfld_dsi_config *dsi_config)
{
	PSB_DEBUG_ENTRY("\n");

	return 0;
}

static
struct drm_display_mode *tianma_cmd_get_config_mode(void)
{
	struct drm_display_mode *mode;

	PSB_DEBUG_ENTRY("\n");

	mode = kzalloc(sizeof(*mode), GFP_KERNEL);
	if (!mode)
		return NULL;

	mode->hdisplay = 360;
	mode->hsync_start = 392;
	mode->hsync_end = 393;
	mode->htotal = 425;

	mode->vdisplay = 326;
	mode->vsync_start = 336;
	mode->vsync_end = 337;
	mode->vtotal = 343;

	mode->vrefresh = 60;
	mode->clock =  mode->vrefresh * mode->vtotal * mode->htotal / 1000;
	mode->type |= DRM_MODE_TYPE_PREFERRED;

	drm_mode_set_name(mode);
	drm_mode_set_crtcinfo(mode, 0);

	return mode;
}

static
void tianma_cmd_get_panel_info(int pipe,
		struct panel_info *pi)
{
	PSB_DEBUG_ENTRY("\n");

	if (pipe == 0) {
		pi->width_mm = 40;
		pi->height_mm = 40;
	}
}

void tianma_cmd_init(struct drm_device *dev,
		struct panel_funcs *p_funcs)
{
	if (!dev || !p_funcs) {
		DRM_ERROR("Invalid parameters\n");
		return;
	}

	bias_en_gpio = get_gpio_by_name("disp0_bias_en");
	if (bias_en_gpio <= 0)
		bias_en_gpio = 189;
	gpio_request(bias_en_gpio, "tianma_display");

	mipi_reset_gpio = get_gpio_by_name("disp0_rst");
	if (mipi_reset_gpio <= 0)
		mipi_reset_gpio = 190;
	gpio_request(mipi_reset_gpio, "tianma_display");

	lnw_gpio_set_alt(68, 1); /* Force TE as muxmode1:
				this should not be necessary as already done in IFWI */

	PSB_DEBUG_ENTRY("\n");
	p_funcs->reset = tianma_cmd_panel_reset;
	p_funcs->power_on = tianma_cmd_power_on;
	p_funcs->power_off = tianma_cmd_power_off;
	p_funcs->drv_ic_init = (select_init_code) ? tianma_cmd_drv_ic_fullinit : tianma_cmd_drv_ic_init;
	p_funcs->get_config_mode = tianma_cmd_get_config_mode;
	p_funcs->get_panel_info = tianma_cmd_get_panel_info;
	p_funcs->dsi_controller_init = tianma_cmd_controller_init;
	p_funcs->detect = tianma_cmd_panel_connection_detect;
	p_funcs->set_brightness = tianma_cmd_set_brightness;
	p_funcs->exit_deep_standby = tianma_cmd_exit_deep_standby;
}
static
int readback_initcode(struct mdfld_dsi_config *dsi_config)
{
	struct mdfld_dsi_pkg_sender *sender
		= mdfld_dsi_get_pkg_sender(dsi_config);
	int err = 0;
	int i;
	u8 *cmd, *arg;
	u8 data;
/* By default, this function returns immediately */
#ifndef TIANMA_DEBUG
	return 0;
#endif

	DRM_INFO("%s\n", __func__);

	if (!sender) {
		DRM_ERROR("Cannot get sender\n");
		return -EINVAL;
	}

	msleep(120);

	cmd = (u8 *)tianma_init_code;
	arg = cmd + 1;

	for (i = 0; i < ARRAY_SIZE(tianma_init_code); i++) {
		switch (*cmd) {
		case 0xFF:
			err = mdfld_dsi_send_mcs_short_lp(sender,
					*cmd, *arg, 1,
					MDFLD_DSI_SEND_PACKAGE);
			break;
		default:
			err = mdfld_dsi_read_mcs_lp(sender, *cmd, &data, 1);
				DRM_ERROR("%01X %01X\n", *cmd, data);
			break;
		}
		cmd += 2;
		arg += 2;
	}
readback_err:
	if (err) {
		err = -EIO;
		DRM_ERROR("TIANMA: error in readback loop\n");
	}
	return err;
}
