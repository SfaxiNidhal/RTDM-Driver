ifneq ($(KERNELRELEASE),)
    # Ajout des modules
	obj-m += buzzer_driver.o
	obj-m += ultrasound-rtdm.o

    # Options de compilation pour Xenomai
	EXTRA_CFLAGS := -I /usr/xenomai/include/
else

    # Configuration Xenomai
	XENOCONFIG=/usr/xenomai/bin/xeno-config
	CC=$(shell      $(XENOCONFIG) --cc)
	CFLAGS=$(shell  $(XENOCONFIG) --skin=posix --cflags)
	LDFLAGS=$(shell $(XENOCONFIG) --skin=posix --ldflags)
	LIBDIR=$(shell  $(XENOCONFIG) --skin=posix --libdir)

	CROSS_COMPILE ?= 
	KERNEL_DIR ?= /usr/src/linux
	 MODULE_DIR := $(shell pwd)

.PHONY: all
all:: modules executable

.PHONY: modules
modules:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(MODULE_DIR) CROSS_COMPILE=$(CROSS_COMPILE) modules

.PHONY: executable
executable: user-app

# Compilation de l'application utilisateur
user-app: user-app.c
	$(CC) $(CFLAGS) user-app.c -o user-app $(LDFLAGS)

.PHONY: clean
clean::
	rm -f  *.o  .*.o  .*.o.* *.ko  .*.ko  *.mod.* .*.mod.* .*.cmd *~
	rm -f Module.symvers Module.markers modules.order
	rm -rf .tmp_versions
	rm -f user-app

endif

