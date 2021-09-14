obj-$(CONFIG_VIDEO_ATOMISP_IMX) += atomisp-imx1x5.o

atomisp-imx1x5-objs := atomisp-imx.o drv201.o ad5816g.o dw9714.o dw9719.o dw9718.o vcm.o otp.o otp_imx.o otp_brcc064_e2prom.o otp_e2prom.o

atomisp-ov8858_driver-objs := ../atomisp-ov8858.o dw9718.o vcm.o
obj-$(CONFIG_VIDEO_ATOMISP_OV8858)     += atomisp-ov8858_driver.o

ccflags-y += -Werror
