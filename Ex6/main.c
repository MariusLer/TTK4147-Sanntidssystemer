#include "io.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <rtdk.h>
#include <sys/mman.h>  // for mlockall
#include <native/task.h>
#include <native/timer.h>

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

void print_periodic(void* arg){
	rt_task_set_periodic(NULL, TM_NOW, 500000*1000);
	while(1){
		rt_printf("Test\n");
		rt_task_wait_period(NULL);
	}
	return;
}

void task(void* arg){
	int channel = (int)arg;
	rt_printf("Channel %d\n", channel);
	io_write(channel,1);
	rt_task_set_periodic(NULL, TM_NOW, 10*1000);
	while(1){
		if (!io_read(channel)){
				io_write(channel, 0);
				usleep(5);
				io_write(channel,1);
		}
		
		rt_task_wait_period(NULL);
	}
	return;
}

int main(int argc, char const *argv[]){	
	io_init();
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);

	RT_TASK tasks[3];
	int channelA = 1;
	int channelB = 2;
	int channelC = 3;

	rt_task_create(&tasks[0], NULL, 0, 50, T_CPU(3));
	rt_task_create(&tasks[1], NULL, 0, 50, T_CPU(3));
	rt_task_create(&tasks[2], NULL, 0, 50, T_CPU(3));
	rt_task_start(&tasks[0], &task, (void*)channelA);
	rt_task_start(&tasks[1], &task, (void*)channelB);
	rt_task_start(&tasks[2], &task, (void*)channelC);
	
	pthread_t disturbancethreads[10];
	int i;
	for (i = 0; i < 10; ++i)
	{
		pthread_create(&disturbancethreads[i],NULL,disturbance,NULL);
	}
	pthread_join(disturbancethreads[5],NULL);
	
	return 0;
}