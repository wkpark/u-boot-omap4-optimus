UBOOT_DIR := $(ANDROID_BUILD_TOP)/bootable/bootloader/u-boot
UBOOT_IMAGE_FILE_NAME := u-boot.bin
UBOOT_INTERNAL_IMAGE := $(UBOOT_DIR)/$(UBOOT_IMAGE_FILE_NAME)
UBOOT_OUTPUT_IMAGE := $(PRODUCT_OUT)/$(UBOOT_IMAGE_FILE_NAME)
UBOOT_CONFIG_OUT := $(UBOOT_DIR)/include/asm
UBOOT_CROSS_COMPILE := arm-none-linux-gnueabi-
ifeq ($(TARGET_BUILD_VARIANT),user)
           UBOOT_USERBUILD := cosmo_build_usermode
else
           UBOOT_USERBUILD := 
endif

ifeq ($(TARGETNAME),p925)
           UBOOT_BOOT_VIBRATOR := cosmo_build_boot_vibrator
else
           UBOOT_BOOT_VIBRATOR := 
endif

# add "-m32" option to make uboot tools as 32-bit LSB executable ELF,
# even though it is built in 64-bit host
CHECK_LONG_BIT = $(shell getconf LONG_BIT)
ifeq ($(CHECK_LONG_BIT),64)
UBOOT_HOSTCC := "HOSTCC=gcc -m32"
endif

.PHONY: uboot
uboot: $(UBOOT_OUTPUT_IMAGE)

$(UBOOT_OUTPUT_IMAGE): $(UBOOT_INTERNAL_IMAGE)
	cp -u $< $@

.PHONY: $(UBOOT_INTERNAL_IMAGE)
$(UBOOT_INTERNAL_IMAGE): $(UBOOT_CONFIG_OUT)
	+$(MAKE) -C $(UBOOT_DIR) HW_VER=$(TARGET_HW) HWVER=$(TARGET_HW) SHOW_LOGO_IMG=y CROSS_COMPILE=$(UBOOT_CROSS_COMPILE)

.PHONY: $(UBOOT_CONFIG_OUT)
$(UBOOT_CONFIG_OUT):
	$(MAKE) -C $(UBOOT_DIR) $(UBOOT_CONFIG) $(UBOOT_USERBUILD) $(UBOOT_BOOT_VIBRATOR) CROSS_COMPILE=$(UBOOT_CROSS_COMPILE)

cuboot:
	$(MAKE) -C $(UBOOT_DIR) distclean
	-rm -f $(UBOOT_OUTPUT_IMAGE)
