#include <native/sem.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <unistd.h>
#include <sys/mman.h> 
#include <time.h>

void timespec_add_us(struct timespec *t, long us){
	t->tv_nsec += us*1000;

	if (t->tv_nsec > 1000000000){
		t->tv_nsec -= 1000000000;
		t->tv_sec += 1;
	}
}
void L(void* arg){
	rt_printf("Task 1 Waiting \n");
	RT_SEM* ptr = (RT_SEM*)arg;
	rt_sem_p(ptr,TM_INFINITE);
	rt_printf("Task1 exiting \n");

	while(1){
	 rt_sem_p(ptr,TM_INFINITE);
	 //wait 3
	 rt_sem_v(ptr);
	}

}

void M(void* arg){
	rt_printf("Task 1 Waiting \n");
	RT_SEM* ptr = (RT_SEM*)arg;
	rt_sem_p(ptr,TM_INFINITE);
	rt_printf("Task1 exiting \n");


	while(1){
		//sleep1

		//busy wait5
	}

}
void H(void* arg){
	rt_printf("Task 2 waiting \n");
	RT_SEM* ptr = (RT_SEM*)arg;
	rt_sem_p(ptr,TM_INFINITE);
	rt_printf("TasK 2 exiting \n");

	while(1){
		//sleep 2
		rt_sem_p(ptr);

		//busy wait 2

		rt_sem_v(ptr);
	}

}

void task3(void* arg){
	RT_SEM* ptr = (RT_SEM*)arg;
	
	struct timespec tid;
	clock_gettime(CLOCK_REALTIME,&tid);
	timespec_add_us(&tid, 1000000);

	clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME, &tid, NULL);

	rt_printf("Unblocking semaphore\n");
	rt_sem_broadcast(ptr);
	rt_sem_v(ptr);
}

int main()
{
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);

	RT_SEM semaphore;
	RT_TASK tasks[4];
	rt_sem_create(&semaphore, NULL, 0, S_PRIO);

	rt_task_create(&tasks[0], NULL, 0, 10, T_CPU(3) | T_JOINABLE);
	rt_task_create(&tasks[1], NULL, 0, 11, T_CPU(3) | T_JOINABLE);
	rt_task_create(&tasks[2], NULL, 0, 13, T_CPU(3) | T_JOINABLE);
	rt_task_create(&tasks[3], NULL, 0, 20, T_CPU(3) | T_JOINABLE);
	rt_task_start(&tasks[0], &L, (void*)&semaphore);
	rt_task_start(&tasks[1], &M, (void*)&semaphore);
	rt_task_start(&tasks[2], &H, (void*)&semaphore);
	usleep(10);
	rt_task_start(&tasks[3], &task3, (void*)&semaphore);


	rt_task_join(&tasks[0]);
	rt_task_join(&tasks[1]);
	rt_task_join(&tasks[2]);
	rt_sem_delete(&semaphore);
	return 0;
}