
# TOPDIR should be defined by the BUILD engine

# define the targets we can do - before including the Master template
LOCAL_TARGETS=all clean distclean install source save-config restore-config \
	baseline
.PHONY: $(LOCAL_TARGETS)

# get the master definitions 
include $(TOPDIR)/BUILD/Master.mk

# remove cmd line set variables from interfering with the sub make
MAKEOVERRIDES=

ifdef CONFIG_BUILD_UBOOT
all install:: u-boot_build
else
all install:: 
endif

CONFIG_NAME=$(shell echo $(CONFIG_UBOOT_DEFCONFIG))

u-boot_build: $(IMAGEDIR)/u-boot-$(PRODUCTID).bin \
	$(HOSTDIR)/bin/mkimage

.PHONY: force_build
force_build: u-boot_make u-boot_build

include/config.mk:
ifdef CONFIG_NAME
	$(MAKE) $(CONFIG_NAME)
else
	$(error U-Boot doesn't support this build configuration)
endif

$(IMAGEDIR)/u-boot-$(PRODUCTID).bin: u-boot.bin
	@$(call MarkStartOfAction,"Installing U-Boot")
	@echo "*************************************"
	@echo "* Installing U-Boot"
	@echo "*************************************"
	$(INSTALL) $< $@
	@$(call MarkEndOfAction,"Installing U-Boot")

$(HOSTDIR)/bin/mkimage: tools/mkimage
	$(INSTALL) $< $@

u-boot_make u-boot.bin tools/mkimage: include/config.mk
	@$(call MarkStartOfAction,"Building U-Boot")
	@echo "*************************************"
	@echo "* Building U-Boot"
	@echo "*************************************"
	$(MAKE) CROSS_COMPILE=$(CROSS)
	@$(call MarkEndOfAction,"Building U-Boot")

# catch published sub-targets and call ourself with the force_ removed
$(LOCAL_TARGETS:%=force_%):
	@$(call MarkStartOfAction,"Building U-Boot %")
	@$(MAKE) -f .bld-adapter.mk TOPDIR=$(TOPDIR) $(@:force_%=%)

# call all direct targets and just pass blindly - after removing force_
force_%:
	@$(call MarkStartOfAction,"Building $(@:force_%=%)")
	$(MAKE) CROSS_COMPILE=$(CROSS) $(@:force_%=%)
	@$(call MarkEndOfAction,"Building $(@:force_%=%)")

# create a rule for the local targets.  Nothing to do, just pass them
# on to the master Makefile.
clean distclean::
	$(MAKE) $@

############################################################
# Release
############################################################

save-config::

restore-config::

source: $(RELEASEDIR) $(CONFSAVEDIR)
	$(call ReleaseCCFileList,.,U-Boot)
	$(call ReleasePkgCCsrc,.,U-Boot)

baseline:
	@$(call make_baseline,$(shell pwd))
