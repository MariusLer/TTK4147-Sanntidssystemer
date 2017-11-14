#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/mman.h>
#include <pthread.h>

struct pid_data{
	pthread_mutex_t pid_mutex;
	pid_t pid;
};


int set_priority(int priority){
	int policy;
	struct sched_param param;
	// check priority in range
	if (priority < 1 || priority > 63) return -1;
	// set priority
	pthread_getschedparam(pthread_self(), &policy, &param);
	param.sched_priority = priority;
	return pthread_setschedparam(pthread_self(),policy, &param);
}
int get_priority(){
	int policy;
	struct sched_param param;
	// get priority
	pthread_getschedparam(pthread_self(), &policy, &param);
	return param.sched_curpriority;
}

void* client_thread(void* arg){
	int pri = *(int*)arg;
	printf("my priority is : %d\n",pri);
	set_priority(pri);
	int file_desc = shm_open("/sharedpid", O_RDWR, S_IRWXU);
	struct pid_data* mem_pid_ptr;
	mem_pid_ptr = mmap(0, sizeof(struct pid_data), PROT_READ | PROT_WRITE, MAP_SHARED, file_desc, 0);
	pthread_mutexattr_t myattr;
	pthread_mutexattr_init(&myattr);
	pthread_mutexattr_setpshared(&myattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&mem_pid_ptr->pid_mutex, &myattr);
	pid_t pid = mem_pid_ptr->pid;
	printf("Found pid:  %d\n", pid);
	char buffer[]="Got pid";
	char received[10];
	int channel = ConnectAttach(0, pid, 1, 0, 0);
	int status = MsgSend(channel, buffer, sizeof(char)*8, received, sizeof(char)*10);
	printf("Received: %s\n", received);
	ConnectDetach(channel);
	return NULL;
}

int main(int argc, char *argv[]) {

	// Server

	set_priority(20);
	int file_desc = shm_open("/sharedpid", O_RDWR | O_CREAT, S_IRWXU);
	ftruncate(file_desc, sizeof(struct pid_data));
	struct pid_data* mem_pid_ptr;
	mem_pid_ptr = mmap(0, sizeof(struct pid_data), PROT_READ | PROT_WRITE, MAP_SHARED, file_desc, 0);

	pthread_mutexattr_t myattr;
	pthread_mutexattr_init(&myattr);
	pthread_mutexattr_setpshared(&myattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&mem_pid_ptr->pid_mutex, &myattr);
	pid_t mypid = getpid();
	mem_pid_ptr->pid = mypid;
	printf("%d\n", mypid);

	int channel = ChannelCreate(0);
	char buffer[10];
	char reply[] = "Replying";

	struct _msg_info info;

	while(1){
		printf("Server priority is : %d\n", get_priority());
		int msg_id = MsgReceive(channel, &buffer, sizeof(char)*10, &info);
		printf("Server priority is now : %d\n", get_priority());
		printf("Received msg : %s\n", buffer);
		printf("PID : %d TID : %d\n", info.pid, info.tid);
		MsgReply(msg_id, 0, &reply, sizeof(char)*9);
	}



	// Client
	/*
	pthread_t threads[4];
	int i;
	int pri[4];
	for (i = 0; i < 4; i++){
		pri[i] = i*7+7;
		pthread_create(&threads[i], NULL, client_thread, (void*)&pri[i]);
	}
	for (i = 0; i < 4; i++){
		pthread_join(threads[i], NULL);
	}
	*/

	return EXIT_SUCCESS;
}
