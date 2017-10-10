#include <native/sem.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <unistd.h>
#include <sys/mman.h> 
#include <time.h>
#include <native/mutex.h>


int rt_task_sleep_ms(unsigned long delay){
	return rt_task_sleep(1000*1000*delay);
}

void busy_wait_ms(unsigned long delay){
	unsigned long count = 0;
	while (count <= delay*10){
		rt_timer_spin(1000*100);
		count++;
	}
}


void print_pri(RT_TASK *task, char *s){
	struct rt_task_info temp;
	rt_task_inquire(task, &temp);
	rt_printf("b:%i c:%i ", temp.bprio, temp.cprio);
	rt_printf(s);
}


void L(void* arg){
	RT_TASK* self = rt_task_self();
	RT_SEM* sem = (RT_SEM*)arg;
	print_pri(self,"Task L Waiting \n");
	rt_sem_p(sem,TM_INFINITE);

	while(1){
	 rt_sem_p(sem,TM_INFINITE);
	 busy_wait_ms(3);
	 print_pri(self,"Task L signal sem \n");
	 rt_sem_v(sem);
	}
}

void M(void* arg){
	RT_TASK* self = rt_task_self();
	RT_SEM* sem = (RT_SEM*)arg;
	print_pri(self,"Task M Waiting \n");
	rt_sem_p(sem,TM_INFINITE);


	while(1){
		rt_task_sleep_ms(1);
		busy_wait_ms(5);
		print_pri(self,"Task M did work \n");
	}

}
void H(void* arg){
	RT_TASK* self = rt_task_self();
	RT_SEM* sem = (RT_SEM*)arg;
	print_pri(self,"Task H Waiting \n");
	rt_sem_p(sem,TM_INFINITE);

	while(1){
		rt_task_sleep_ms(2);
		rt_sem_p(sem,TM_INFINITE);
		busy_wait_ms(2);
		print_pri(self,"Task H signal sem \n");
		rt_sem_v(sem);
	}

}

void synchronize(void* arg){
	RT_SEM* sem = (RT_SEM*)arg;
	rt_task_sleep_ms(5);
	rt_printf("Unblocking semaphore\n");
	rt_sem_broadcast(sem);
	rt_sem_v(sem);
	rt_task_sleep_ms(5);
}

int main()
{
	rt_print_auto_init(1);
	mlockall(MCL_CURRENT | MCL_FUTURE);

	RT_SEM semaphore;
	RT_TASK tasks[4];
	rt_sem_create(&semaphore, NULL, 0, S_PRIO);

	rt_task_create(&tasks[0], NULL, 0, 5, T_CPU(3) | T_JOINABLE);
	rt_task_create(&tasks[1], NULL, 0, 10, T_CPU(3) | T_JOINABLE);
	rt_task_create(&tasks[2], NULL, 0, 15, T_CPU(3) | T_JOINABLE);
	rt_task_create(&tasks[3], NULL, 0, 20, T_CPU(3) | T_JOINABLE);
	rt_task_start(&tasks[0], &L, (void*)&semaphore);
	rt_task_start(&tasks[1], &M, (void*)&semaphore);
	rt_task_start(&tasks[2], &H, (void*)&semaphore);
	rt_task_start(&tasks[3], &synchronize, (void*)&semaphore);

	rt_task_join(&tasks[0]);
	rt_task_join(&tasks[1]);
	rt_task_join(&tasks[2]);
	rt_sem_delete(&semaphore);
	return 0;
}