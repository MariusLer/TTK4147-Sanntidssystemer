#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/times.h>
#include <unistd.h>
void busyWait(int secounds){
	int i;	
	int tps = sysconf(_SC_CLK_TCK);
	int dummy;
	clock_t start;
	struct tms exec_time;
	times(&exec_time);
	start= exec_time.tms_utime;
	while( (exec_time.tms_utime -start)< (secounds*tps)){
		for(i=0;i<1000;i++){
			dummy=i;
			dummy=dummy+dummy;
		}
		times(&exec_time);
	}
	
}


void* printEvery5(void* x){
	//int id =(int*)idpointer;
		//busyWait(5);	
	printf("hei\n");
	busyWait(5);
	printf("I am number\n");
		
	return NULL;
}

int main(){
	//int id1 =1;
	//int id2 =2;
	pthread_t thread1;
	pthread_t thread2;
	pthread_create(&thread1, NULL, printEvery5, NULL);
	pthread_create(&thread2, NULL, printEvery5, NULL);
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	return 0;
}
