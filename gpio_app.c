#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(void) {
	int dev;
	int value;

	dev = open("/dev/gpio_dev", O_RDWR);
	if(dev > 0){
		printf("open successful! \n");
	} else {
		perror("open()\n");
	}

	while(1){
		read(dev, &value, sizeof(value));
		printf("%d\n", value);

		sleep(1);
	}
	
	close(dev);
	return 0;
}

	
