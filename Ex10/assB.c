#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <pthread.h>

dispatch_t              *dpp;
resmgr_attr_t           resmgr_attr;
dispatch_context_t      *ctp;
resmgr_connect_funcs_t  connect_funcs;
resmgr_io_funcs_t       io_funcs;
iofunc_attr_t           io_attr;

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb);
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);

int direction = 1;
int variable = 0;
char buf[12];
pthread_mutex_t mutex;
pthread_mutex_t mutex2;


void error(char *s)
{
	perror(s);
	exit(EXIT_FAILURE);
}

void* thread_func(void* arg){
	while(1){
		pthread_mutex_lock(&mutex);
		pthread_mutex_lock(&mutex2);
		variable += direction;
		pthread_mutex_unlock(&mutex);
		pthread_mutex_unlock(&mutex2);
		delay(100);
	}
}

int main(int argc, char *argv[]) {
	printf("Start resource manager\n");

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex2, NULL);

	// create dispatch.
	if (!(dpp = dispatch_create()))
		error("Create dispatch");

	// initialize resource manager attributes.
	memset(&resmgr_attr, 0, sizeof(resmgr_attr));
	resmgr_attr.nparts_max = 1;
	resmgr_attr.msg_max_size = 2048;

	// set standard connect and io functions.
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);
	iofunc_attr_init(&io_attr, S_IFNAM | 0666, 0, 0);

	// override functions
	io_funcs.read = io_read;
	io_funcs.write = io_write;

	// establish resource manager
	if (resmgr_attach(dpp, &resmgr_attr, "/dev/myresource", _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &io_attr) < 0)
		error("Resmanager attach");
	ctp = dispatch_context_alloc(dpp);

	pthread_t thread;
	pthread_create(&thread, NULL, thread_func, NULL);

	// wait forever, handling messages.
	while(1)
	{
		if (!(ctp = dispatch_block(ctp)))
			error("Dispatch block");
		dispatch_handler(ctp);
	}

	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&mutex2);
	exit(EXIT_SUCCESS);
}

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb)
{
	pthread_mutex_lock(&mutex);
	sprintf(buf, "%d", variable);
	pthread_mutex_unlock(&mutex);
	if(!ocb->offset)
	{
		printf("variable %s direction:%d\n ", buf, direction);
		// set data to return
		SETIOV(ctp->iov, buf, strlen(buf));
		_IO_SET_READ_NBYTES(ctp, strlen(buf));

		// increase the offset (new reads will not get the same data)
		ocb->offset = 1;

		// return
		return (_RESMGR_NPARTS(1));
	}
	else
	{
		// set to return no data
		_IO_SET_READ_NBYTES(ctp, 0);

		// return
		return (_RESMGR_NPARTS(0));
	}
}

int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb){
	char cmd[10];
	printf("%s\n", "io_write called");

	_IO_SET_WRITE_NBYTES (ctp, msg->i.nbytes);
	resmgr_msgread(ctp, cmd, msg->i.nbytes, sizeof(msg->i));
	cmd [msg->i.nbytes] = '\0'; /* just in case the text is not NULL terminated */
	printf("Command:%s \n", cmd);
	pthread_mutex_lock(&mutex2);
	if (cmd[0] == 'u'){
		direction = 1;
	}
	if (cmd[0] == 'd'){
			direction = -1;
	}
	if (cmd[0] == 's'){
			direction = 0;
	}
	pthread_mutex_unlock(&mutex2);
	return (_RESMGR_NPARTS (0));
}

