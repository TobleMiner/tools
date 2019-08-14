CCFLAGS=-Wall -ggdb -O0
CCFLAGS32=`pkg-config --cflags --libs zlib`
CC?=gcc

all: crc32 crc16

crc32: crc32.c
	$(CC) $(CCFLAGS) $(CCFLAGS32) $< -o crc32

crc16: crc16.c
	$(CC) $(CCFLAGS) $< -o crc16

clean: crc32 crc16
	$(RM) $^
