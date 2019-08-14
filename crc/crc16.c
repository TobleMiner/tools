#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

uint16_t poly = 0x8408;

void usage(char* prog) {
	fprintf(stderr, "Usage: %s [-p poly] <filename>\n", prog);
}

uint16_t crc16_final(uint16_t crc) {
	return ~crc;
}

uint16_t crc16_update(uint16_t crc, unsigned char *data_p, unsigned short length) {
	unsigned char i;
	unsigned int data;

	if (length == 0)
		return crc;

	do {
		for (i=0, data=(unsigned int)0xff & *data_p++; i < 8; i++, data >>= 1) {
			if ((crc & 0x0001) ^ (data & 0x0001)) {
				crc = (crc >> 1) ^ poly;
			} else {
				crc >>= 1;
			}
		}
	} while (--length);

	return crc;
}

int main(int argc, char** argv) {
	char* fname;
	unsigned char buff[4096];
	int fd, err = 0, opt;
	ssize_t read_len;
	uint16_t crc;
	unsigned long int poly_;

	while((opt = getopt(argc, argv, "p:h")) != -1) {
		switch(opt) {
			case 'p':
				poly_ = strtoul(optarg, NULL, 0);
				if(poly_ == ULONG_MAX) {
					fprintf(stderr, "Invalid polynome specified\n");
					usage(argv[0]);
				}
				if(poly_ > UINT16_MAX) {
					fprintf(stderr, "Polynome value too large\n");
					usage(argv[0]);
				}
				poly = (uint16_t)poly_;
				break;
			default:
				fprintf(stderr, "Unknown option '%c'\n", opt);
			case 'h':
				usage(argv[0]);
		}
	}

	if(argc <= optind) {
		usage(argv[0]);
		return 1;
	}

	fname = argv[optind];

	if((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "Failed to open file '%s': %s(%d)\n", fname, strerror(errno), errno);
		err = errno;
		goto fail;
	}

	crc = 0xFFFF;
	while((read_len = read(fd, buff, sizeof(buff))) > 0) {
		crc = crc16_update(crc, buff, read_len);
	}
	crc = crc16_final(crc);

	if(read_len < 0) {
		fprintf(stderr, "Failed to read from file: %s(%d)\n", strerror(errno), errno);
		err = errno;
		goto fail_fd;
	}

	printf("%04x\n", crc);

fail_fd:
	close(fd);
fail:
	return err;
}
