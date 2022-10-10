ifneq ($(KERNELRELEASE),)
# Kbuild part of Makefile
EXTRA_CFLAGS = -Wall -g
obj-m	:= program2.o


else
# Normal Makefile
KVERSION := $(shell uname -r)
KDIR ?= /lib/modules/$(KVERSION)/build

PWD	:= $(shell pwd)

all:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
	$(CC) test.c -o test
clean:
	$(MAKE) -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
	rm test

endif