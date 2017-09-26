#include "io.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <sched.h>

int set_cpu(int cpu_number){
	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(cpu_number,&cpu);

	return pthread_setaffinity_np(pthread_self(),sizeof(cpu_set_t),&cpu);
}

void* disturbance(void* arg){
	set_cpu(3);
	unsigned long dummy = 0;
	while(1){
		dummy++;
	}
}

void timespec_add_us(struct timespec *t, long us){
	t->tv_nsec += us*1000;

	if (t->tv_nsec > 1000000000){
		t->tv_nsec -= 1000000000;
		t->tv_sec += 1;
	}
}

void* thread(void* arg){
	set_cpu(3);
	int* ptr = (int*) arg;
	int channel = *ptr;
	io_write(channel,1);
	struct timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	while(1){
		if (!io_read(channel)){
				io_write(channel, 0);
				usleep(5);
				io_write(channel,1);
		}
		timespec_add_us(&time, 50);
	    clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME, &time, NULL);
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
	pthread_t disturbancethreads[10];

	pthread_create(&threadA,NULL,thread,(void*)&channelA);
	pthread_create(&threadB,NULL,thread,(void*)&channelB);
	pthread_create(&threadC,NULL,thread,(void*)&channelC);

	int i;
	for (i = 0; i < 10; ++i)
	{
		pthread_create(&disturbancethreads[i],NULL,disturbance,NULL);
	}

	pthread_join(threadA,NULL);
	pthread_join(threadB,NULL);
	pthread_join(threadC,NULL);
	return 0;
}
