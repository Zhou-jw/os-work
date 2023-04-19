NAME := $(shell basename $(PWD))
export MODULE := M2
all: $(NAME)-64 $(NAME)-32
LDFLAGS += -lpthread -g3

run: $(NAME)-64
	./$(NAME)-64 5
include ../Makefile
