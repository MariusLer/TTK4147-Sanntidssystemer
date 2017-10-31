#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "client/miniproject.h"

#define BUFFER_LENGTH 25
#define PORT 9999

const int period_us = 3800;
const double Kp = 10;
const double Ki = 800;
const double reference = 1;

pthread_mutex_t send_udp_msg_mutex;
sem_t got_signal;
sem_t got_y;
double y;

double calculate_u(double current_value){
	double error = reference - current_value;
	static double integral;
	integral = integral + error*period_us/1000000;
	return Kp*error + Ki* integral;
}

void* pi_controller(void* arg){
	struct timespec t;
	char buffer[BUFFER_LENGTH];
	struct udp_conn* udp = (struct udp_conn*)(arg);
	int time_runnning_us = 0;
	double u;

	udp_send(udp,"START",6);
	clock_gettime(CLOCK_REALTIME, &t);
	while(1){
		if (time_runnning_us >= 500*1000){
			pthread_mutex_lock(&send_udp_msg_mutex);
			udp_send(udp,"STOP",5);
			pthread_mutex_unlock(&send_udp_msg_mutex);
			return NULL;
		}

		pthread_mutex_lock(&send_udp_msg_mutex);
		udp_send(udp, "GET", 4);
		pthread_mutex_unlock(&send_udp_msg_mutex);

		sem_wait(&got_y);

		u = calculate_u(y);

		sprintf(buffer,"SET:%6.3f",u);
		
		pthread_mutex_lock(&send_udp_msg_mutex);
		udp_send(udp,buffer,BUFFER_LENGTH);
		pthread_mutex_unlock(&send_udp_msg_mutex);

		time_runnning_us += period_us;
		
		timespec_add_us(&t, period_us);
		clock_nanosleep(&t);
	}
}

void* udp_listener(void* arg){
	struct udp_conn* udp = (struct udp_conn*)(arg);
	char buffer[BUFFER_LENGTH];

	while (1){
		udp_receive(udp, buffer, BUFFER_LENGTH);
		if (buffer[0] == 'S'){
			sem_post(&got_signal);
			continue;
		}
		y = atof(&buffer[8]);
		sem_post(&got_y);
	}
} 

void* replySignal(void* arg){
	struct udp_conn* udp = (struct udp_conn*)(arg);
	while(1){
		sem_wait(&got_signal);
		pthread_mutex_lock(&send_udp_msg_mutex);
		udp_send(udp,"SIGNAL_ACK",11);
		pthread_mutex_unlock(&send_udp_msg_mutex);
	}
}

int main()
{	
	char* ip = "129.241.187.154";

	struct udp_conn udp;

	int sock = udp_init_client(&udp, PORT, ip);
	if (sock == -1){
		printf("%s\n","Could not establish connection to server");
	}

	pthread_mutex_init(&send_udp_msg_mutex,NULL);
	sem_init(&got_signal, 0, 0);
	sem_init(&got_y, 0, 0);
	
	pthread_t threads[3];
	
	pthread_create(&threads[1],NULL,replySignal,(void*)&udp);
	pthread_create(&threads[2],NULL,udp_listener,(void*)&udp);
	pthread_create(&threads[0],NULL,pi_controller,(void*)&udp);

	pthread_join(threads[0],NULL);
	printf("%s\n","Done" );
	pthread_mutex_destroy(&send_udp_msg_mutex);
	sem_destroy(&got_y);
	sem_destroy(&got_signal);
	return 0;
}
