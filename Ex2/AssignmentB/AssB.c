#include <pthread.h>
#include <stdio.h>

int global = 0;


void* increment(void* arg){
	int local = 0;
	int i;
	for(i = 0; i < 50000; i++){
		global++;
		local++;
	}
	printf("Global :%d\n",global);
	printf("Local :%d\n",local);
	return NULL;
}

int main(){

	pthread_t thread1;
	pthread_t thread2;
	pthread_create(&thread1,NULL,increment,NULL);
	pthread_create(&thread2,NULL,increment,NULL);
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);

	printf("Global :%d\n",global);
	return 0;
}