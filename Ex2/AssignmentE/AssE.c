#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t fork1;
pthread_mutex_t fork2;
pthread_mutex_t fork3;
pthread_mutex_t fork4;
pthread_mutex_t fork5;

int number = 1;

pthread_mutex_t counter;

void* philosopher(void* arg){
	int id;
	pthread_mutex_lock(&counter);
	id = number;
	number++;
	pthread_mutex_unlock(&counter);
	pthread_mutex_t** list = ((pthread_mutex_t**) arg);
	while(1){
		pthread_mutex_lock(list[0]);
		pthread_mutex_lock(list[1]);
		printf("%s%d\n","Philosor eating, ID: ",id);
		pthread_mutex_unlock(list[0]);
		pthread_mutex_unlock(list[1]);
		printf("%s%d\n","Philosor thinking, ID: ",id);
		//usleep(1000);
	}
}

int main(){
	pthread_mutex_t** list1 = malloc(2*sizeof(pthread_mutex_t*));
	pthread_mutex_t** list2 = malloc(2*sizeof(pthread_mutex_t*));
	pthread_mutex_t** list3 = malloc(2*sizeof(pthread_mutex_t*));
	pthread_mutex_t** list4 = malloc(2*sizeof(pthread_mutex_t*));
	pthread_mutex_t** list5 = malloc(2*sizeof(pthread_mutex_t*));
	list1[0] = &fork1;
	list1[1] = &fork2;
	list2[0] = &fork2;
	list2[1] = &fork3;
	list3[0] = &fork3;
	list3[1] = &fork4;
	list4[0] = &fork4;
	list4[1] = &fork5;
	list5[0] = &fork1;
	list5[1] = &fork5;
	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;
	pthread_t thread5;
	pthread_create(&thread1,NULL,philosopher,(void *) list1);
	pthread_create(&thread2,NULL,philosopher,(void *) list2);
	pthread_create(&thread3,NULL,philosopher,(void *) list3);
	pthread_create(&thread4,NULL,philosopher,(void *) list4);
	pthread_create(&thread5,NULL,philosopher,(void *) list5);
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	pthread_join(thread3,NULL);
	pthread_join(thread4,NULL);
	pthread_join(thread5,NULL);
	return 0;
}
