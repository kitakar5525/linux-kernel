KVERSION := "$(shell uname -r)"
KDIR := "/lib/modules/$(KVERSION)/build"
ATOMISP_INC := "drivers/staging/media/atomisp/include"
ccflags-y += -I $(KDIR)/$(ATOMISP_INC)

obj-$(CONFIG_VIDEO_ATOMISP_MICAM_OV5693) += atomisp-ov5693.o
obj-$(CONFIG_VIDEO_ATOMISP_MICAM_T4KA3) += atomisp-t4ka3drv.o

atomisp-t4ka3drv-objs := atomisp-t4ka3.o dw9761.o

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
