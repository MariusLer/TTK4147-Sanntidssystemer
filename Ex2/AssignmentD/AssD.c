#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int running = 1;
int var1 = 0;
int var2 = 1;

pthread_mutex_t mutex;

void* func1(void* arg){
	while(running){
		pthread_mutex_lock(&mutex);
		var1++;
		var2 = var1;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void* func2(void* arg){
	int i;
	for(i = 0; i < 20; i++){
		pthread_mutex_lock(&mutex);
		printf("%s%d%s%d\n","Number 1 is ",var1," number 2 is ",var2);
		pthread_mutex_unlock(&mutex);
		usleep(100000);
	}
	running = 0;
	return NULL;
}

int main(){
	pthread_t thread1;
	pthread_t thread2;
	pthread_create(&thread1,NULL,func1,NULL);
	pthread_create(&thread2,NULL,func2,NULL);
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	return 0;
}