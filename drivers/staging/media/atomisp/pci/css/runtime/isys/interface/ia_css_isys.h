/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Support for Intel Camera Imaging ISP subsystem.
 * Copyright (c) 2010 - 2015, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef __IA_CSS_ISYS_H__
#define __IA_CSS_ISYS_H__

#include <type_support.h>
#include <input_system.h>
#include <ia_css_input_port.h>
#include <ia_css_stream_format.h>
#include <ia_css_stream_public.h>
#include <system_global.h>
#include "ia_css_isys_comm.h"


input_system_err_t ia_css_isys_init(void);
void ia_css_isys_uninit(void);
enum mipi_port_id ia_css_isys_port_to_mipi_port(
    enum mipi_port_id api_port);


/* CSS Receiver */
void ia_css_isys_rx_configure(
    const rx_cfg_t *config,
    const enum ia_css_input_mode input_mode);

void ia_css_isys_rx_disable(void);

void ia_css_isys_rx_enable_all_interrupts(enum mipi_port_id port);

unsigned int ia_css_isys_rx_get_interrupt_reg(enum mipi_port_id port);
void ia_css_isys_rx_get_irq_info(enum mipi_port_id port,
				 unsigned int *irq_infos);
void ia_css_isys_rx_clear_irq_info(enum mipi_port_id port,
				   unsigned int irq_infos);
unsigned int ia_css_isys_rx_translate_irq_infos(unsigned int bits);


/* @brief Translate format and compression to format type.
 *
 * @param[in]	input_format	The input format.
 * @param[in]	compression	The compression scheme.
 * @param[out]	fmt_type	Pointer to the resulting format type.
 * @return			Error code.
 *
 * Translate an input format and mipi compression pair to the fmt_type.
 * This is normally done by the sensor, but when using the input fifo, this
 * format type must be sumitted correctly by the application.
 */
int ia_css_isys_convert_stream_format_to_mipi_format(
    enum atomisp_input_format input_format,
    mipi_predictor_t compression,
    unsigned int *fmt_type);


#endif				/* __IA_CSS_ISYS_H__ */
