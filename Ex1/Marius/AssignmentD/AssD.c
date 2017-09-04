#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/times.h>
#include <unistd.h>

void busy_wait_delay(int seconds){
  int i, dummy;
  int tps = sysconf(_SC_CLK_TCK);
  clock_t start;
  struct tms exec_time;

  times(&exec_time);
  start = exec_time.tms_utime;

  while( (exec_time.tms_utime - start) < (seconds * tps)){
    for(i = 0; i < 1000; i++){
      dummy = i;
    }
    times(&exec_time);
  }
}

void* print1(void* arg){
  printf("%s\n","Dette er tråd 1");
  busy_wait_delay(5);
  printf("%s\n","Tråd 1 ferdig");
  return NULL;
}

void* print2(void* arg){
  printf("%s\n","Dette er tråd 2");
  busy_wait_delay(5);
  printf("%s\n","Tråd 2 ferdig");
  return NULL;
}

int main(){
	pthread_t thread1;
	pthread_t thread2;
	pthread_create(&thread1, NULL, print1, NULL);
	pthread_create(&thread2, NULL, print2, NULL);
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	return 0;
}
