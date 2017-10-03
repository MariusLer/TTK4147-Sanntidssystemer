#include <native/sem.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <unistd.h>
#include <sys/mman.h> 
#include <time.h>

void task(void* arg){
	rt_printf("Task 1 Waiting \n");
	RT_SEM* ptr = (RT_SEM*)arg;
	rt_sem_p(ptr,TM_INFINITE);
	rt_printf("Task1 exiting \n");

}
void task2(void* arg){
	rt_printf("Task 2 waiting \n");
	RT_SEM* ptr = (RT_SEM*)arg;
	rt_sem_p(ptr,TM_INFINITE);
	rt_printf("TasK 2 exiting \n");
}

void task3(void* arg){
	RT_SEM* ptr = (RT_SEM*)arg;
	rt_printf("Sleeping \n");

	rt_task_sleep(1000*1000*1000);
	rt_printf("Waking up \n");

	rt_printf("Unblocking semaphore\n");
	rt_sem_broadcast(ptr);
	
	rt_task_sleep(1000*1000*1000);	
}

int main()
{
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);

	RT_SEM semaphore;
	RT_TASK tasks[3];
	rt_sem_create(&semaphore, NULL, 0, S_PRIO);

	rt_task_create(&tasks[0], NULL, 0, 10, T_CPU(3) | T_JOINABLE);
	rt_task_create(&tasks[1], NULL, 0, 11, T_CPU(3) | T_JOINABLE);
	rt_task_create(&tasks[2], NULL, 0, 20, T_CPU(3) | T_JOINABLE);
	rt_task_start(&tasks[0], &task, (void*)&semaphore);
	rt_task_start(&tasks[1], &task2, (void*)&semaphore);
	usleep(10);
	rt_task_start(&tasks[2], &task3, (void*)&semaphore);


	rt_task_join(&tasks[0]);
	rt_task_join(&tasks[1]);
	rt_task_join(&tasks[2]);
	rt_sem_delete(&semaphore);
	return 0;
}