export MODULE  := L1
NAME           := kernel
SRCS           := framework/main.c $(shell find -L ./src/ -name "*.c")
INC_PATH       := include/ framework/

export AM_HOME := $(PWD)/../abstract-machine
ifeq ($(ARCH),)
export ARCH := x86_64-qemu
endif

include $(AM_HOME)/Makefile
include ../Makefile.lab
image: git

COMMA :=,

ifeq ($(ARCH_H),)
export ARCH_H := x86_64-qemu
endif

test: git 
	@gcc $(shell find src/ -name "*.c") \
		$(shell find test/ -name "*.c") \
		-Iframework/ -Itest/ -Iinclude/ \
		-I/home/zjw/os-workbench/kernel/../abstract-machine/am/include/ \
    	-I/home/zjw/os-workbench/kernel/../abstract-machine/klib/include/ \
		-DTEST -lpthread \	# -DTEST define macro TEST
		-fno-asynchronous-unwind-tables -fno-builtin -fno-stack-protector \
		-o build/test

testall: test
	@build/test 0