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

int main(int argc, char *argv[]) {

	// Program 1
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


	// Program 2
	/*
	int file_desc = shm_open("/sharedpid", O_RDWR, S_IRWXU);
	struct pid_data* mem_pid_ptr;
	mem_pid_ptr = mmap(0, sizeof(struct pid_data), PROT_READ | PROT_WRITE, MAP_SHARED, file_desc, 0);
	pthread_mutexattr_t myattr;
	pthread_mutexattr_init(&myattr);
	pthread_mutexattr_setpshared(&myattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&mem_pid_ptr->pid_mutex, &myattr);
	pid_t pid = mem_pid_ptr->pid;
	printf("Found pid:  %d\n", pid);
	*/

	return EXIT_SUCCESS;
}
