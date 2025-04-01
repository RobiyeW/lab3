ifneq (${KERNELRELEASE},)

# KERNELRELEASE defined: we are being compiled as part of the Kernel
obj-m := vga_ball.o

else

# We are being compiled as a module: use the Kernel build system
KERNEL_SOURCE := /usr/src/linux-headers-$(shell uname -r)
PWD := $(shell pwd)

default: module bouncing_ball

# Compile the kernel module
module:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules

# Compile the user-space bouncing ball program
bouncing_ball:
	gcc -o bouncing_ball bouncing_ball.c

# Clean the build files
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
	${RM} bouncing_ball

# Tarball for submission
TARFILES = Makefile README vga_ball.h vga_ball.c bouncing_ball.c
TARFILE = lab3-sw.tar.gz
.PHONY : tar
tar : $(TARFILE)

$(TARFILE) : $(TARFILES)
	tar zcfC $(TARFILE) .. $(TARFILES:%=lab3-sw/%)

endif
