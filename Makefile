# This is our variable (helps improve readability and we can change it if we decide to use a Raspberry Pi or a custom kernel)
Kernal_Dir := /lib/modules/$(shell uname -r)/build

# Our present working directory (where Makefile is located)
PMD := $(shell pwd)

obj-m += Driver.o

all:
	make -C $(Kernal_Dir) M=$(PMD) modules

clean:
	make -C $(Kernal_Dir) M=$(PMD) clean
