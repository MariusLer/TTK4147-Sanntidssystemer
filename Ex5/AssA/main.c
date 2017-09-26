#include "io.h"
#include <unistd.h>
#include <pthread.h>

void* thread(void* arg){
	int* ptr = (int*) arg;
	int channel = *ptr;
	io_write(channel,1);
	while(1){
		if (!io_read(channel)){
			io_write(channel, 0);
			usleep(5);
			io_write(channel,1);
		}
	}
	return NULL;
}



int main(int argc, char const *argv[])
{	io_init();

	int channelA = 1;
	int channelB = 2;
	int channelC = 3;

	pthread_t threadA;
	pthread_t threadB;
	pthread_t threadC;

	pthread_create(&threadA,NULL,thread,(void*)&channelA);
	pthread_create(&threadB,NULL,thread,(void*)&channelB);
	pthread_create(&threadC,NULL,thread,(void*)&channelC);

	pthread_join(threadA,NULL);
	pthread_join(threadB,NULL);
	pthread_join(threadC,NULL);
	return 0;
}
