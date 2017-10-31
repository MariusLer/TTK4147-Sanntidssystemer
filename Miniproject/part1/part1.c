#include <stdio.h>
#include <stdlib.h>
#include "client/miniproject.h"

#define BUFFER_LENGTH 25

const int port = 9999;
const int period_us = 2500;
const double Kp = 10;
const double Ki = 800;
const double reference = 1;

double calculate_u(double current_value){
	double error = reference - current_value;
	static double integral;
	integral = integral + error*period_us/1000000;
	return Kp*error + Ki* integral;
}

int main()
{	
	char* ip = "129.241.187.154";

	struct udp_conn udp;
	struct timespec t;

	int sock = udp_init_client(&udp, port, ip);
	if (sock == -1){
		printf("%s\n","Could not establish connection to server");
	}

	char buffer[BUFFER_LENGTH];
	double current_value;
	double u;
	int time_runnning_us = 0;

	udp_send(&udp,"START",6);
	clock_gettime(CLOCK_REALTIME, &t);
	while(1){
		if (time_runnning_us >= 500*1000){
			udp_send(&udp,"STOP",5);
			break;
		}
		udp_send(&udp, "GET", 4);
		askAck:
		udp_receive(&udp, buffer, BUFFER_LENGTH);
		if (buffer[0] == 'S'){ // Special case where server sends out SIGNAL message
			goto askAck;
		}
		current_value = atof(&buffer[8]);
		u = calculate_u(current_value);
		sprintf(buffer,"SET:%6.3f",u);
		udp_send(&udp,buffer,BUFFER_LENGTH);
		time_runnning_us += period_us;
		
		timespec_add_us(&t, period_us);
		clock_nanosleep(&t);
	}
	printf("%s\n","Done" );
	return 0;
}