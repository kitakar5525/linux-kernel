obj-$(CONFIG_X86_INTEL_MID) += intel-mid.o intel_mid_vrtc.o mfld.o mrfl.o

obj-$(CONFIG_VIDEO_ATOMISP) += intel_mid_atomisp.o
obj-y += intel_mid_pcihelpers.o

obj-y += device_libs/

# SFI specific code
ifdef CONFIG_X86_INTEL_MID
obj-$(CONFIG_SFI) += sfi.o
endif
