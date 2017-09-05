#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

sem_t semaphore;

void* thread(void* arg){
	sem_wait(&semaphore);
	int i;
	for(i = 0; i < 4; i++){
		printf("%s%d\n","Count ",i);
		usleep(100000);
	}
	sem_post(&semaphore);
	return NULL;
}

int main(){
	sem_init(&semaphore,0,3);

	pthread_t thread1;
	pthread_t thread2;
	pthread_t thread3;
	pthread_t thread4;
	pthread_t thread5;
	pthread_create(&thread1,NULL,thread,NULL);
	pthread_create(&thread2,NULL,thread,NULL);
	pthread_create(&thread3,NULL,thread,NULL);
	pthread_create(&thread4,NULL,thread,NULL);
	pthread_create(&thread5,NULL,thread,NULL);
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	pthread_join(thread3,NULL);
	pthread_join(thread4,NULL);
	pthread_join(thread5,NULL);

	return 0;
}