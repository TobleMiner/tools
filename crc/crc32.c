#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zlib.h>

int main(int argc, char** argv) {
	char* fname;
	unsigned char* buff[4096];
	int fd, err = 0;
	ssize_t read_len;
	uint32_t crc;

	if(argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		err = EINVAL;
		goto fail;
	}

	fname = argv[1];

	if((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "Failed to open file '%s': %s(%d)\n", fname, strerror(errno), errno);
		err = errno;
		goto fail;
	}

	crc = crc32(0L, Z_NULL, 0);
	while((read_len = read(fd, buff, sizeof(buff)))) {
		crc = crc32(crc, buff, read_len);
	}

	if(read_len < 0) {
		fprintf(stderr, "Failed to read from file: %s(%d)\n", strerror(errno), errno);
		err = errno;
		goto fail_fd;
	}

	printf("%08x\n", crc);

fail_fd:
	close(fd);
fail:
	return err;
}
