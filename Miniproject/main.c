#include <stdio.h>
#include <stdlib.h>
#include "client/miniproject.h"

#define BUFFER_LENGTH 25
#define PORT 9999;

const int period_us = 2500;
const double Kp = 10;
const double Ki = 800;
const double reference = 1;


pthread_mutex_t send_udp_msg_mutex
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
	
	
	int time_runnning_us = 0;

	udp_send(&udp,"START",6);
	clock_gettime(CLOCK_REALTIME, &t)
	while(1){
		//ask y fix period
		if (time_runnning_us >= 500*1000){
			pthread_mutex_lock(send_udp_msg_mutex);
			udp_send(&udp,"STOP",5);
			pthread_mutex_unlock(send_udp_msg_mutex);
			break;
		}
		pthread_mutex_lock(send_udp_msg_mutex);
		udp_send(&udp, "GET", 4);
		pthread_mutex_unlock(send_udp_msg_mutex);
		

		while (!y_received){
         //////////////////////////////////////////////////////////////////
		}

		u = calculate_u(y);

		sprintf(buffer,"SET:%6.3f",u);
		
		pthread_mutex_lock(send_udp_msg_mutex);
		udp_send(&udp,buffer,BUFFER_LENGTH);
		pthread_mutex_unlock(send_udp_msg_mutex);

		time_runnning_us += period_us;
		
		timespec_add_us(&t, period_us);
		clock_nanosleep(&t);
	}
}

void* udp_listener(void* arg){
	char buffer[BUFFER_LENGTH];
	while (1){
		udp_receive(&udp, buffer, BUFFER_LENGTH);
		if (buffer[0] == 'S'){ // Special case where server sends out SIGNAL message
			// set condition variable ...
			continue;
		}
		y = atof(&buffer[8]);
		// must set condiution variable

	}
	
} 

void* replySignal(void* arg){

	while(1){
		// check if we should send ack


		pthread_mutex_lock(send_udp_msg_mutex);
		udp_send(&udp,"SIGNAL_ACK",11);
		pthread_mutex_unlock(send_udp_msg_mutex);
	}
}

int main()
{	
	char* ip = "129.241.187.154";

	struct udp_conn udp;
	struct timespec t;

	int sock = udp_init_client(&udp, PORT, ip);
	if (sock == -1){
		printf("%s\n","Could not establish connection to server");
	}

	pthread_mutex_init(send_udp_msg_mutex,NULL);


	printf("%s\n","Done" );
	return 0;
}
