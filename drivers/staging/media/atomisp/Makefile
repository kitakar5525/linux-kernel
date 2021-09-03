# SPDX-License-Identifier: GPL-2.0
#
# Makefile for camera drivers.
#
obj-$(CONFIG_INTEL_ATOMISP) += i2c/
obj-$(CONFIG_VIDEO_ATOMISP) += atomisp.o
obj-$(CONFIG_VIDEO_ATOMISP) += pci/atomisp_gmin_platform.o

# While on staging, keep debug enabled
DEFINES += -DDEBUG

atomisp = $(srctree)/drivers/staging/media/atomisp/

# SPDX-License-Identifier: GPL-2.0
atomisp-objs += \
	pci/atomisp2/atomisp_acc.o \
	pci/atomisp2/atomisp_cmd.o \
	pci/atomisp2/atomisp_compat_css20.o \
	pci/atomisp2/atomisp_csi2.o \
	pci/atomisp2/atomisp_drvfs.o \
	pci/atomisp2/atomisp_file.o \
	pci/atomisp2/atomisp_fops.o \
	pci/atomisp2/atomisp_ioctl.o \
	pci/atomisp2/atomisp_subdev.o \
	pci/atomisp2/atomisp_tpg.o \
	pci/atomisp2/atomisp_v4l2.o \
	pci/atomisp2/css2400/sh_css_firmware.o \
	pci/atomisp2/css2400/sh_css_host_data.o \
	pci/atomisp2/css2400/sh_css_hrt.o \
	pci/atomisp2/css2400/sh_css_metadata.o \
	pci/atomisp2/css2400/sh_css_metrics.o \
	pci/atomisp2/css2400/sh_css_mipi.o \
	pci/atomisp2/css2400/sh_css_mmu.o \
	pci/atomisp2/css2400/sh_css_morph.o \
	pci/atomisp2/css2400/sh_css.o \
	pci/atomisp2/css2400/sh_css_param_dvs.o \
	pci/atomisp2/css2400/sh_css_param_shading.o \
	pci/atomisp2/css2400/sh_css_params.o \
	pci/atomisp2/css2400/sh_css_properties.o \
	pci/atomisp2/css2400/sh_css_shading.o \
	pci/atomisp2/css2400/sh_css_sp.o \
	pci/atomisp2/css2400/sh_css_stream_format.o \
	pci/atomisp2/css2400/sh_css_stream.o \
	pci/atomisp2/css2400/sh_css_version.o \
	pci/atomisp2/css2400/base/circbuf/src/circbuf.o \
	pci/atomisp2/css2400/base/refcount/src/refcount.o \
	pci/atomisp2/css2400/camera/pipe/src/pipe_binarydesc.o \
	pci/atomisp2/css2400/camera/pipe/src/pipe_stagedesc.o \
	pci/atomisp2/css2400/camera/pipe/src/pipe_util.o \
	pci/atomisp2/css2400/camera/util/src/util.o \
	pci/atomisp2/hmm/hmm_bo.o \
	pci/atomisp2/hmm/hmm_dynamic_pool.o \
	pci/atomisp2/hmm/hmm.o \
	pci/atomisp2/hmm/hmm_reserved_pool.o \
	pci/atomisp2/css2400/ia_css_device_access.o \
	pci/atomisp2/css2400/isp/kernels/aa/aa_2/ia_css_aa2.host.o \
	pci/atomisp2/css2400/isp/kernels/anr/anr_1.0/ia_css_anr.host.o \
	pci/atomisp2/css2400/isp/kernels/anr/anr_2/ia_css_anr2.host.o \
	pci/atomisp2/css2400/isp/kernels/anr/anr_2/ia_css_anr2_table.host.o \
	pci/atomisp2/css2400/isp/kernels/bh/bh_2/ia_css_bh.host.o \
	pci/atomisp2/css2400/isp/kernels/bnlm/ia_css_bnlm.host.o \
	pci/atomisp2/css2400/isp/kernels/bnr/bnr_1.0/ia_css_bnr.host.o \
	pci/atomisp2/css2400/isp/kernels/bnr/bnr2_2/ia_css_bnr2_2.host.o \
	pci/atomisp2/css2400/isp/kernels/cnr/cnr_1.0/ia_css_cnr.host.o \
	pci/atomisp2/css2400/isp/kernels/cnr/cnr_2/ia_css_cnr2.host.o \
	pci/atomisp2/css2400/isp/kernels/conversion/conversion_1.0/ia_css_conversion.host.o \
	pci/atomisp2/css2400/isp/kernels/copy_output/copy_output_1.0/ia_css_copy_output.host.o \
	pci/atomisp2/css2400/isp/kernels/crop/crop_1.0/ia_css_crop.host.o \
	pci/atomisp2/css2400/isp/kernels/csc/csc_1.0/ia_css_csc.host.o \
	pci/atomisp2/css2400/isp/kernels/ctc/ctc_1.0/ia_css_ctc.host.o \
	pci/atomisp2/css2400/isp/kernels/ctc/ctc_1.0/ia_css_ctc_table.host.o \
	pci/atomisp2/css2400/isp/kernels/ctc/ctc1_5/ia_css_ctc1_5.host.o \
	pci/atomisp2/css2400/isp/kernels/ctc/ctc2/ia_css_ctc2.host.o \
	pci/atomisp2/css2400/isp/kernels/de/de_1.0/ia_css_de.host.o \
	pci/atomisp2/css2400/isp/kernels/de/de_2/ia_css_de2.host.o \
	pci/atomisp2/css2400/isp/kernels/dpc2/ia_css_dpc2.host.o \
	pci/atomisp2/css2400/isp/kernels/dp/dp_1.0/ia_css_dp.host.o \
	pci/atomisp2/css2400/isp/kernels/dvs/dvs_1.0/ia_css_dvs.host.o \
	pci/atomisp2/css2400/isp/kernels/eed1_8/ia_css_eed1_8.host.o \
	pci/atomisp2/css2400/isp/kernels/fc/fc_1.0/ia_css_formats.host.o \
	pci/atomisp2/css2400/isp/kernels/fpn/fpn_1.0/ia_css_fpn.host.o \
	pci/atomisp2/css2400/isp/kernels/gc/gc_1.0/ia_css_gc.host.o \
	pci/atomisp2/css2400/isp/kernels/gc/gc_1.0/ia_css_gc_table.host.o \
	pci/atomisp2/css2400/isp/kernels/gc/gc_2/ia_css_gc2.host.o \
	pci/atomisp2/css2400/isp/kernels/gc/gc_2/ia_css_gc2_table.host.o \
	pci/atomisp2/css2400/isp/kernels/hdr/ia_css_hdr.host.o \
	pci/atomisp2/css2400/isp/kernels/ipu2_io_ls/bayer_io_ls/ia_css_bayer_io.host.o \
	pci/atomisp2/css2400/isp/kernels/ipu2_io_ls/bayer_io_ls/ia_css_bayer_io.host.o \
	pci/atomisp2/css2400/isp/kernels/ipu2_io_ls/yuv444_io_ls/ia_css_yuv444_io.host.o \
	pci/atomisp2/css2400/isp/kernels/iterator/iterator_1.0/ia_css_iterator.host.o \
	pci/atomisp2/css2400/isp/kernels/macc/macc_1.0/ia_css_macc.host.o \
	pci/atomisp2/css2400/isp/kernels/macc/macc_1.0/ia_css_macc_table.host.o \
	pci/atomisp2/css2400/isp/kernels/macc/macc1_5/ia_css_macc1_5.host.o \
	pci/atomisp2/css2400/isp/kernels/macc/macc1_5/ia_css_macc1_5_table.host.o \
	pci/atomisp2/css2400/isp/kernels/norm/norm_1.0/ia_css_norm.host.o \
	pci/atomisp2/css2400/isp/kernels/ob/ob_1.0/ia_css_ob.host.o \
	pci/atomisp2/css2400/isp/kernels/ob/ob2/ia_css_ob2.host.o \
	pci/atomisp2/css2400/isp/kernels/output/output_1.0/ia_css_output.host.o \
	pci/atomisp2/css2400/isp/kernels/qplane/qplane_2/ia_css_qplane.host.o \
	pci/atomisp2/css2400/isp/kernels/raw_aa_binning/raw_aa_binning_1.0/ia_css_raa.host.o \
	pci/atomisp2/css2400/isp/kernels/raw/raw_1.0/ia_css_raw.host.o \
	pci/atomisp2/css2400/isp/kernels/ref/ref_1.0/ia_css_ref.host.o \
	pci/atomisp2/css2400/isp/kernels/s3a/s3a_1.0/ia_css_s3a.host.o \
	pci/atomisp2/css2400/isp/kernels/sc/sc_1.0/ia_css_sc.host.o \
	pci/atomisp2/css2400/isp/kernels/sdis/sdis_1.0/ia_css_sdis.host.o \
	pci/atomisp2/css2400/isp/kernels/sdis/sdis_2/ia_css_sdis2.host.o \
	pci/atomisp2/css2400/isp/kernels/tdf/tdf_1.0/ia_css_tdf.host.o \
	pci/atomisp2/css2400/isp/kernels/tnr/tnr_1.0/ia_css_tnr.host.o \
	pci/atomisp2/css2400/isp/kernels/vf/vf_1.0/ia_css_vf.host.o \
	pci/atomisp2/css2400/isp/kernels/wb/wb_1.0/ia_css_wb.host.o \
	pci/atomisp2/css2400/isp/kernels/xnr/xnr_1.0/ia_css_xnr.host.o \
	pci/atomisp2/css2400/isp/kernels/xnr/xnr_1.0/ia_css_xnr_table.host.o \
	pci/atomisp2/css2400/isp/kernels/xnr/xnr_3.0/ia_css_xnr3.host.o \
	pci/atomisp2/css2400/isp/kernels/ynr/ynr_1.0/ia_css_ynr.host.o \
	pci/atomisp2/css2400/isp/kernels/ynr/ynr_2/ia_css_ynr2.host.o \
	pci/atomisp2/mmu/isp_mmu.o \
	pci/atomisp2/mmu/sh_mmu_mrfld.o \
	pci/atomisp2/css2400/runtime/binary/src/binary.o \
	pci/atomisp2/css2400/runtime/bufq/src/bufq.o \
	pci/atomisp2/css2400/runtime/debug/src/ia_css_debug.o \
	pci/atomisp2/css2400/runtime/eventq/src/eventq.o \
	pci/atomisp2/css2400/runtime/event/src/event.o \
	pci/atomisp2/css2400/runtime/frame/src/frame.o \
	pci/atomisp2/css2400/runtime/ifmtr/src/ifmtr.o \
	pci/atomisp2/css2400/runtime/inputfifo/src/inputfifo.o \
	pci/atomisp2/css2400/runtime/isp_param/src/isp_param.o \
	pci/atomisp2/css2400/runtime/isys/src/csi_rx_rmgr.o \
	pci/atomisp2/css2400/runtime/isys/src/isys_dma_rmgr.o \
	pci/atomisp2/css2400/runtime/isys/src/isys_init.o \
	pci/atomisp2/css2400/runtime/isys/src/isys_stream2mmio_rmgr.o \
	pci/atomisp2/css2400/runtime/isys/src/rx.o \
	pci/atomisp2/css2400/runtime/isys/src/virtual_isys.o \
	pci/atomisp2/css2400/runtime/pipeline/src/pipeline.o \
	pci/atomisp2/css2400/runtime/queue/src/queue_access.o \
	pci/atomisp2/css2400/runtime/queue/src/queue.o \
	pci/atomisp2/css2400/runtime/rmgr/src/rmgr.o \
	pci/atomisp2/css2400/runtime/rmgr/src/rmgr_vbuf.o \
	pci/atomisp2/css2400/runtime/spctrl/src/spctrl.o \
	pci/atomisp2/css2400/runtime/timer/src/timer.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/debug.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/dma.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/event_fifo.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/fifo_monitor.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/gdc.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/gp_device.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/gp_timer.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/hmem.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/input_formatter.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/input_system.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/irq.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/isp.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/mmu.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/sp.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/timed_ctrl.o \
	pci/atomisp2/css2400/hive_isp_css_common/host/vmem.o \
	pci/atomisp2/css2400/hive_isp_css_shared/host/tag.o \
	pci/atomisp2/css2400/system_local.o \

obj-byt = \
	pci/atomisp2/css2400/css_2400_system/hive/ia_css_isp_configs.o \
	pci/atomisp2/css2400/css_2400_system/hive/ia_css_isp_params.o \
	pci/atomisp2/css2400/css_2400_system/hive/ia_css_isp_states.o \

# These will be needed when clean merge CHT support nicely into the driver
# Keep them here handy for when we get to that point
#

obj-cht = \
	pci/atomisp2/css2400/css_2401_system/hive/ia_css_isp_configs.o \
	pci/atomisp2/css2400/css_2401_system/hive/ia_css_isp_params.o \
	pci/atomisp2/css2400/css_2401_system/hive/ia_css_isp_states.o \
	pci/atomisp2/css2400/css_2401_system/host/csi_rx.o \
	pci/atomisp2/css2400/css_2401_system/host/ibuf_ctrl.o \
	pci/atomisp2/css2400/css_2401_system/host/isys_dma.o \
	pci/atomisp2/css2400/css_2401_system/host/isys_irq.o \
	pci/atomisp2/css2400/css_2401_system/host/isys_stream2mmio.o

INCLUDES += \
	-I$(atomisp)/pci/atomisp2/ \
	-I$(atomisp)/pci/atomisp2/include/ \
	-I$(atomisp)/pci/atomisp2/include/hmm/ \
	-I$(atomisp)/pci/atomisp2/include/mmu/ \
	-I$(atomisp)/pci/atomisp2/css2400/ \
	-I$(atomisp)/pci/atomisp2/css2400/base/circbuf/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/base/refcount/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/camera/pipe/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/camera/util/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/hive_isp_css_common/ \
	-I$(atomisp)/pci/atomisp2/css2400/hive_isp_css_common/host/ \
	-I$(atomisp)/pci/atomisp2/css2400/hive_isp_css_include/ \
	-I$(atomisp)/pci/atomisp2/css2400/hive_isp_css_include/device_access/ \
	-I$(atomisp)/pci/atomisp2/css2400/hive_isp_css_include/host/ \
	-I$(atomisp)/pci/atomisp2/css2400/hive_isp_css_shared/ \
	-I$(atomisp)/pci/atomisp2/css2400/hive_isp_css_shared/host/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/aa/aa_2/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/anr/anr_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/anr/anr_2/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/bh/bh_2/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/bnlm/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/bnr/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/bnr/bnr_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/bnr/bnr2_2/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/cnr/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/cnr/cnr_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/cnr/cnr_2/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/conversion/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/conversion/conversion_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/copy_output/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/copy_output/copy_output_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/crop/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/crop/crop_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/csc/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/csc/csc_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ctc/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ctc/ctc_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ctc/ctc1_5/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ctc/ctc2/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/de/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/de/de_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/de/de_2/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/dp/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/dpc2/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/dp/dp_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/dvs/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/dvs/dvs_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/eed1_8/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/fc/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/fc/fc_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/fixedbds/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/fixedbds/fixedbds_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/fpn/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/fpn/fpn_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/gc/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/gc/gc_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/gc/gc_2/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/hdr/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ipu2_io_ls/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ipu2_io_ls/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ipu2_io_ls/bayer_io_ls/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ipu2_io_ls/bayer_io_ls/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ipu2_io_ls/common/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ipu2_io_ls/common/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ipu2_io_ls/yuv444_io_ls/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ipu2_io_ls/yuv444_io_ls/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/iterator/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/iterator/iterator_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/macc/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/macc/macc_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/macc/macc1_5/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/norm/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/norm/norm_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ob/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ob/ob_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ob/ob2/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/output/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/output/output_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/qplane/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/qplane/qplane_2/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/raw/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/raw_aa_binning/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/raw_aa_binning/raw_aa_binning_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/raw/raw_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ref/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ref/ref_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/s3a/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/s3a/s3a_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/sc/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/sc/sc_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/sdis/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/sdis/common/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/sdis/sdis_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/sdis/sdis_2/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/tdf/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/tdf/tdf_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/tnr/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/tnr/tnr_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/tnr/tnr3/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/uds/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/uds/uds_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/vf/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/vf/vf_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/wb/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/wb/wb_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/xnr/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/xnr/xnr_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/xnr/xnr_3.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ynr/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ynr/ynr_1.0/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/kernels/ynr/ynr_2/ \
	-I$(atomisp)/pci/atomisp2/css2400/isp/modes/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/binary/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/bufq/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/debug/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/event/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/eventq/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/frame/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/ifmtr/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/inputfifo/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/isp_param/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/isys/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/isys/src/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/pipeline/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/queue/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/queue/src/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/rmgr/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/spctrl/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/runtime/tagger/interface/ \
	-I$(atomisp)/pci/atomisp2/css2400/css_2400_system/hive/ \
	-I$(atomisp)/pci/atomisp2/css2400/css_2401_system/ \
	-I$(atomisp)/pci/atomisp2/css2400/css_2401_system/host/ \
	-I$(atomisp)/pci/atomisp2/css2400/css_2401_system/hive/ \
	-I$(atomisp)/pci/atomisp2/css2400/css_2401_system/hrt/

DEFINES := -DHRT_HW -DHRT_ISP_CSS_CUSTOM_HOST -DHRT_USE_VIR_ADDRS -D__HOST__
#DEFINES += -DUSE_DYNAMIC_BIN
#DEFINES += -DISP_POWER_GATING
#DEFINES += -DUSE_INTERRUPTS
#DEFINES += -DUSE_SSSE3
#DEFINES += -DPUNIT_CAMERA_BUSY
#DEFINES += -DUSE_KMEM_CACHE

ifeq ($(CONFIG_VIDEO_ATOMISP_ISP2401),y)
atomisp-objs += \
	$(obj-cht) \
	pci/atomisp2/css2400/runtime/isys/src/ibuf_ctrl_rmgr.o
DEFINES += -DISP2401 -DISP2401_NEW_INPUT_SYSTEM -DSYSTEM_hive_isp_css_2401_system
else
atomisp-objs += $(obj-byt)
DEFINES += -DISP2400 -DSYSTEM_hive_isp_css_2400_system
endif

ccflags-y += $(INCLUDES) $(DEFINES) -fno-common
