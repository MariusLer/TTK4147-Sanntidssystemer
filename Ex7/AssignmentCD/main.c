#include <native/sem.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>
#include <unistd.h>
#include <sys/mman.h> 
#include <time.h>
#include <native/mutex.h>


RT_SEM semaphore;

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
	RT_MUTEX* mutexes;
	mutexes = (RT_MUTEX*)arg;
	RT_MUTEX mutexA = mutexes[0];
	RT_MUTEX mutexB = mutexes[1];

	print_pri(self,"Task L Waiting \n");
	rt_sem_p(&semaphore,TM_INFINITE);

	while(1){
	 rt_mutex_acquire(&mutexA,TM_INFINITE);
	 rt_task_set_priority(self,15);
	 busy_wait_ms(3);
	 print_pri(self,"Task L got mutexA \n");
	 rt_mutex_acquire(&mutexB, TM_INFINITE);
	 print_pri(self,"Task L got mutexB \n");
	 busy_wait_ms(3);
	 rt_mutex_release(&mutexB);
	 rt_mutex_release(&mutexA);
	 rt_task_set_priority(self,5);
	 rt_task_sleep_ms(1);
	}
}

void H(void* arg){
	RT_TASK* self = rt_task_self();
	RT_MUTEX* mutexes;
	mutexes = (RT_MUTEX*)arg;
	RT_MUTEX mutexA = mutexes[0];
	RT_MUTEX mutexB = mutexes[1];


	print_pri(self,"Task H Waiting \n");
	rt_sem_p(&semaphore,TM_INFINITE);

	while(1){
		rt_task_sleep_ms(1);
		rt_mutex_acquire(&mutexB,TM_INFINITE);
	 	busy_wait_ms(1);
	 	print_pri(self,"Task H got mutexB \n");
	 	rt_mutex_acquire(&mutexA, TM_INFINITE);
	  	print_pri(self,"Task H got mutexA \n");
	 	busy_wait_ms(2);
	 	rt_mutex_release(&mutexA);
	 	rt_mutex_release(&mutexB);
	 	busy_wait_ms(1);
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

	RT_MUTEX mutex[2];
	RT_TASK tasks[3];
	rt_mutex_create(&mutex[0], NULL);
	rt_mutex_create(&mutex[1], NULL);
	rt_sem_create(&semaphore, NULL,0, S_PRIO);

	rt_task_create(&tasks[0], NULL, 0, 5, T_CPU(3) | T_JOINABLE);
	rt_task_create(&tasks[1], NULL, 0, 10, T_CPU(3) | T_JOINABLE);
	rt_task_create(&tasks[2], NULL, 0, 15, T_CPU(3) | T_JOINABLE);
	rt_task_start(&tasks[0], &L, (void*)mutex);
	rt_task_start(&tasks[1], &H, (void*)mutex);
	rt_task_start(&tasks[2], &synchronize, (void*)&semaphore);

	rt_task_join(&tasks[0]);
	rt_task_join(&tasks[1]);
	rt_task_join(&tasks[2]);
	rt_sem_delete(&semaphore);
	int i;
	for (i = 0; i < 2; i++){
		rt_mutex_delete(&mutex[i]);
	}
	return 0;
}