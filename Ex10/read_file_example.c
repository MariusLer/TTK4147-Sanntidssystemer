/*
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	printf("Reading from file example \n");
	int res;
	char buf[255];
	int fd;
	if (argc > 1){
		fd = open("/dev/myresource", O_RDONLY | O_NONBLOCK);
	}
	else {
		fd = open("/dev/myresource", O_RDONLY);
	}
	res = read(fd, buf, 255);
	buf[res] = '\0';

	printf("Read: %s\n", buf);

	close(fd);
	return EXIT_SUCCESS;
}
*/
