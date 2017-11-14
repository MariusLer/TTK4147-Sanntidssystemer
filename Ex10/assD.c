#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <pthread.h>

#include "fifo.h"

dispatch_t              *dpp;
resmgr_attr_t           resmgr_attr;
dispatch_context_t      *ctp;
resmgr_connect_funcs_t  connect_funcs;
resmgr_io_funcs_t       io_funcs;
iofunc_attr_t           io_attr;

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb);
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);

char buf[255];
char buf2[255];
struct fifo_t fifo;
pthread_mutex_t read_write_mutex;


void error(char *s)
{
	perror(s);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	printf("Start resource manager\n");
	pthread_mutex_init(&read_write_mutex, NULL);

	init_fifo(&fifo);

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

	// wait forever, handling messages.
	while(1)
	{
		if (!(ctp = dispatch_block(ctp)))
			error("Dispatch block");
		dispatch_handler(ctp);
	}
	pthread_mutex_destroy(&read_write_mutex);
	pthread_mutex_destroy(&fifo.resource_mutex);
	exit(EXIT_SUCCESS);
}

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb)
{
	pthread_mutex_lock(&read_write_mutex);
	if(!ocb->offset)
	{
		if (fifo_status(&fifo) == 0){
			int nonblock;
			iofunc_read_verify (ctp, msg, ocb,&nonblock);
			if(nonblock){
				pthread_mutex_unlock(&read_write_mutex);
				return (EAGAIN);
				}
			else{
				pthread_mutex_lock(&fifo.resource_mutex);
				fifo_add_blocked_id(&fifo,ctp->rcvid);
				pthread_mutex_unlock(&fifo.resource_mutex);
				pthread_mutex_unlock(&read_write_mutex);
				return (_RESMGR_NOREPLY);
				}

		}
		pthread_mutex_lock(&fifo.resource_mutex);
		fifo_rem_string(&fifo, buf);
		pthread_mutex_unlock(&fifo.resource_mutex);
		// set data to return
		SETIOV(ctp->iov, buf, strlen(buf));
		_IO_SET_READ_NBYTES(ctp, strlen(buf));

		// increase the offset (new reads will not get the same data)
		ocb->offset = 1;

		// return
		pthread_mutex_unlock(&read_write_mutex);
		return (_RESMGR_NPARTS(1));
	}
	else
	{
		// set to return no data
		_IO_SET_READ_NBYTES(ctp, 0);

		// return
		pthread_mutex_unlock(&read_write_mutex);
		return (_RESMGR_NPARTS(0));
	}
}

int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb){
	pthread_mutex_lock(&read_write_mutex);
	printf("%s\n", "io_write called");

	_IO_SET_WRITE_NBYTES (ctp, msg->i.nbytes);
	resmgr_msgread(ctp, buf2, msg->i.nbytes, sizeof(msg->i));
	buf2 [msg->i.nbytes] = '\0'; /* just in case the text is not NULL terminated */
	printf ("Received %d bytes = '%s'\n", msg -> i.nbytes, buf2);
	pthread_mutex_lock(&fifo.resource_mutex);

	int blockedID = fifo_rem_blocked_id(&fifo);
	if(blockedID != -1){
		MsgReply(blockedID,0,buf2,msg->i.nbytes);
	}
	else{
		fifo_add_string(&fifo, buf2);
	}
	pthread_mutex_unlock(&fifo.resource_mutex);
	pthread_mutex_unlock(&read_write_mutex);

	return (_RESMGR_NPARTS (0));
}
