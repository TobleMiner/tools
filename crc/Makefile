CCFLAGS=-Wall `pkg-config --cflags --libs zlib`
CC?=gcc

all: crc32

crc32: crc32.c
	$(CC) $(CCFLAGS) $< -o crc32
