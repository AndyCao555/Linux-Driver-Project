//this is our variable(helps improve readability and we can change it if we decide to use a raspberry pi or a custom kernal)
Kernal_Dir := /lib/modules/$(shell uname -r)/build
//our present working directory(where makefile is located)
PWD := $(shell pwd)

obj-m += "ourdriverfilenamehere".o

all:
	make -C $(Kernal_Dir) M=$(PWD) modules

clean:
	make -C $(Kernal_Dir) M=$(PWD) clean
