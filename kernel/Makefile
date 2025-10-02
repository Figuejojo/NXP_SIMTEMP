# Author: Jose Jorge Figueroa Figueroa
# Github: https://github.com/Figuejojo
#
# THis makefile is for compiling the simtemp module.

obj-m := nxp_simtemp.o
nxp_simtemp-objs := nxp_simtemp_main.o nxp_simtemp_helpers.o nxp_simtemp_chardev.o

KDIR ?= /lib/modules/$(shell uname -r)/build
SRC  := $(shell pwd)
OUT  := $(SRC)/build

.PHONY: all clean
all:
	mkdir -p $(OUT)
	$(MAKE) -C $(KDIR) M=$(SRC) modules
	@mv -t $(OUT) *.mod *.mod.c *.o *.d *.ko Module.symvers modules.order .*.cmd 2>/dev/null || true

clean:
	$(MAKE) -C $(KDIR) M=$(SRC) clean
	rm -rf $(OUT)