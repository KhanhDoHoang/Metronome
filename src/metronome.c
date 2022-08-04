#include "metronome.h"

metronome_t input_obj;
name_attach_t *attach;
int srvr_coid;
char data[512];

int main(int argc, char *argv[]) {

	dispatch_t *dpp;
	resmgr_io_funcs_t io_funcs;
	resmgr_connect_funcs_t connect_funcs;
	ioattr_t ioattrs[NumDevices];
	dispatch_context_t *ctp;
	pthread_attr_t thread_attr;

	if(argc != 4){
		perror("Incorrect number of command line args\n");
		return (EXIT_FAILURE);
	}

	input_obj.beatsPerMinute = atoi(argv[1]);
	input_obj.timeSignatureTop = atoi(argv[2]);
	input_obj.timeSignatureBottom = atoi(argv[3]);

	//We then need to put these new functions into an array which defines which functions to call when doing memory resizing.
	iofunc_funcs_t metocb_funcs = { _IOFUNC_NFUNCS, metocb_calloc, metocb_free };
	iofunc_mount_t metocb_mount = { 0, 0, 0, 0, &metocb_funcs};

	//Create dispatch interface.
	if ((dpp = dispatch_create()) == NULL) {
		fprintf (stderr, "%s:  Unable to allocate dispatch context.\n", argv [0]);
		return (EXIT_FAILURE);
	}

	//Initialize the default io funcs.
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);
	connect_funcs.open = io_open;
	//Overload the io funcs we care about handling
	io_funcs.read = io_read;
	io_funcs.write = io_write;

	//The final step is to add these functions to the iofunc_attr_t structure we setup before calling resmgr_attach.
	for (int i = 0; i < NumDevices; i++) {
		// Create the device handler iofunc_attr_t
		iofunc_attr_init(&ioattrs[i].attr, S_IFCHR | 0666, NULL, NULL);
		ioattrs[i].device = i;
		ioattrs[i].attr.mount = &metocb_mount;
		//Call resmgr_attach
		resmgr_attach(dpp, NULL, devnames[i], _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &ioattrs[i]);
	}

	ctp = dispatch_context_alloc(dpp);

	//spin up thread
	//create the metronome thread in-between calling resmgr_attach() and while(1) { ctp = dispatch_block(... }
	pthread_attr_init(&thread_attr);
	pthread_create(NULL, &thread_attr, &metronome_thread, &input_obj);

	while (1) {
		ctp = dispatch_block(ctp);
		dispatch_handler(ctp);
	}

	pthread_attr_destroy(&thread_attr);
	name_detach(attach, 0);
	name_close(srvr_coid);
	return EXIT_SUCCESS;
}

void *metronome_thread() {

}

int tableLookup(metronome_t *input_obj) {

}

void setTimer(metronome_t *input_obj) {

}

void stopTimer(struct itimerspec *itime, timer_t timer_id) {

}

void startTimer(struct itimerspec *itime, timer_t timer_id, metronome_t *input_obj) {

}

int io_read(resmgr_context_t *ctp, io_read_t *msg, metro_t *metocb) {
	int nb;


	if(data == NULL){
		return 0;
	}

	int index;
	//For loop to check DataTableRow for matches
	for(int i = 0; i > 8; i++){
		if(input_obj.tsbot == t[i].tsbot && input_obj.tstop == t[i].tstop)
			index = i;
	}

	sprintf(data, "[metronome: %d beats/min, time signature %d/%d, secs-per-beat: %.2f, nanoSecs: %d]\n",
			input_obj.bpm,
			t[index].tstop,
			t[index].tsbot,
			input_obj.timer.interval,
			input_obj.timer.nano);


	nb = strlen(data);

	if (metocb->ocb.offset == nb)
		return 0;

	nb = min(nb, msg->i.nbytes);
	_IO_SET_READ_NBYTES(ctp, nb);
	SETIOV(ctp->iov, data, nb);
	metocb->ocb.offset += nb;

	if (nb > 0)
		metocb->ocb.flags |= IOFUNC_ATTR_ATIME;

	return(_RESMGR_NPARTS(1));
}

int io_write(resmgr_context_t *ctp, io_write_t *msg, metro_t *metocb) {

	int nb = 0;

	if( msg->i.nbytes == ctp->info.msglen - (ctp->offset + sizeof(*msg) ))
	{
		/* have all the data */
		char *buf;
		char *pause_msg;
		char *set_msg;
		int i, small_integer;
		buf = (char *)(msg+1);


		if(strstr(buf, "pause") != NULL){
			for(i = 0; i < 2; i++){
				pause_msg = strsep(&buf, " ");
			}
			small_integer = atoi(pause_msg);
			if(small_integer >= 1 && small_integer <= 9){
				MsgSendPulse(srvr_coid, SchedGet(0,0,NULL), PAUSE_PULSE_CODE, small_integer);
			} else {
				printf("Integer is not between 1 and 9.\n");
			}
		}
		else if (strstr(buf, "quit") != NULL) {
			MsgSendPulse(srvr_coid, SchedGet(0,0,NULL), QUIT_PULSE_CODE, small_integer);
		}
		else if (strstr(buf, "start") != NULL) {
			MsgSendPulse(srvr_coid, SchedGet(0,0,NULL), START_PULSE_CODE, small_integer);
		}
		else if (strstr(buf, "stop") != NULL) {
			MsgSendPulse(srvr_coid, SchedGet(0,0,NULL), STOP_PULSE_CODE, small_integer);
		}
		else if (strstr(buf, "set") != NULL) {

			//set values of the metro
			//order is bpm tstop tsbot
			set_msg = strsep(&buf," ");
			set_msg = strsep(&buf," ");
			input_obj.bpm = atoi(set_msg);
			set_msg = strsep(&buf," ");
			input_obj.tstop = atoi(set_msg);
			set_msg = strsep(&buf," ");
			input_obj.tsbot = atoi(set_msg);

			MsgSendPulse(srvr_coid, SchedGet(0,0,NULL), SET_PULSE_CODE, small_integer);
		}

		nb = msg->i.nbytes;
	}
	_IO_SET_WRITE_NBYTES (ctp, nb);

	if (msg->i.nbytes > 0)
		metocb->ocb.flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

	return (_RESMGR_NPARTS (0));

}

int io_open(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle, void *extra){
	if ((srvr_coid = name_open(METRO_ATTACH, 0)) == -1) {
		perror("name_open failed.");
		return EXIT_FAILURE;
	}
	return (iofunc_open_default (ctp, msg, handle, extra));
}

metro_t *metocb_calloc(resmgr_context_t *ctp, ioattr_t *ioattr) {

}

void metocb_free(metro_t *metocb) {
	free(metocb);
}

