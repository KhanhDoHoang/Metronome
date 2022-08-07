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
		printf( "Metronome Resource Manager (ResMgr)\n"
				"\nUsage: metronome <bpm> <ts-top> <ts-bottom>\n\nAPI:\n"
				"pause[1-9]                     -pause the metronome for 1-9 seconds\n"
				"quit                           -quit the metronome\n"
				"set <bpm> <ts-top> <ts-bottom> -set the metronome to <bpm> ts-top/ts-bottom\n"
				"start                          -start the metronome from stopped state\n"
				"stop                           -stop the metronome; use 'start' to resume\n");
		exit (EXIT_FAILURE);
	}

	input_obj.beatsPerMinute = atoi(argv[1]);
	input_obj.timeSignatureTop = atoi(argv[2]);
	input_obj.timeSignatureBottom = atoi(argv[3]);

	//We then need to put these new functions into an array which defines which functions to call when doing memory resizing.
	iofunc_funcs_t metocb_funcs = { _IOFUNC_NFUNCS, metro_calloc, metro_t_free };
	iofunc_mount_t metocb_mount = { 0, 0, 0, 0, &metocb_funcs};

	//Create dispatch interface.
	if ((dpp = dispatch_create()) == NULL) {
		fprintf (stderr, "%s:  Unable to allocate dispatch context.\n", argv [0]);
		exit (EXIT_FAILURE);
	}

	//Initialize the default io funcs.
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);
	connect_funcs.open = io_open;
	//Overload the io funcs we care about handling
	io_funcs.read = io_read;
	io_funcs.write = io_write;

	//The final step is to add these functions to the iofunc_attr_t structure we setup before calling resmgr_attach.
	for (int i = 0; i < NumDevices; i++) {
		iofunc_attr_init(&ioattrs[i].attr, S_IFCHR | 0666, NULL, NULL);
		ioattrs[i].device = i;
		ioattrs[i].attr.mount = &metocb_mount;
		//Call resmgr_attach
		resmgr_attach(dpp, NULL, devnames[i], _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &ioattrs[i]);
	}

	//	iofunc_attr_init(&ioattrs, S_IFCHR | 0666, NULL, NULL);
	ctp = dispatch_context_alloc(dpp);

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
	struct sigevent sigevent;
	struct itimerspec itime;
	my_message_t msg;
	timer_t timer;
	int index = 0;
	int rcvid;
	char *pattern;

	if ((attach = name_attach(NULL, METRO_ATTACH, 0)) == NULL) {
		perror("ERROR: name_attach failure\n");
		exit(EXIT_FAILURE);
	}

	sigevent.sigev_notify = SIGEV_PULSE;
	sigevent.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, attach->chid, _NTO_SIDE_CHANNEL, 0);
	sigevent.sigev_priority = SIGEV_PULSE_PRIO_INHERIT;
	sigevent.sigev_code = MET_PULSE_CODE;

	timer_create(CLOCK_REALTIME, &sigevent, &timer);

	for (int i = 0; i < 8; i++) {
		if (t[i].tsbot == input_obj.timeSignatureBottom && t[i].tstop == input_obj.timeSignatureTop)
			index = i;
	}

	input_obj.timer.length = (double) 60 / input_obj.beatsPerMinute;
	input_obj.timer.measure = input_obj.timer.length * 2;
	input_obj.timer.interval = input_obj.timer.measure / input_obj.timeSignatureBottom;
	input_obj.timer.nano = input_obj.timer.interval * 1e+9;

	itime.it_value.tv_sec = 1;
	itime.it_value.tv_nsec = 0;
	itime.it_interval.tv_sec = input_obj.timer.interval;
	itime.it_interval.tv_nsec = input_obj.timer.nano;
	timer_settime(timer, 0, &itime, NULL);
	pattern = t[index].pattern;

	for (;;) {
		if ((rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL)) == -1) {
			printf("ERROR: Could not receive message\n");
			exit (EXIT_FAILURE);
		}

		if (rcvid == 0) {
			switch (msg.pulse.code) {
			case MET_PULSE_CODE:
				if (*pattern == '|') {
					printf("%.2s", pattern);
					pattern = (pattern + 2);
				}
				else if (*pattern == '\0') {
					printf("\n");
					pattern = t[index].pattern;
				}
				else printf("%c", *pattern++);
				break;
			case START_PULSE_CODE:
				itime.it_value.tv_sec = 1;
				itime.it_value.tv_nsec = 0;
				itime.it_interval.tv_sec = input_obj.timer.interval;
				itime.it_interval.tv_nsec = input_obj.timer.nano;
				timer_settime(timer, 0, &itime, NULL);
				pattern = t[index].pattern;
				break;
			case STOP_PULSE_CODE:
				itime.it_value.tv_sec = 0;
				timer_settime(timer, 0, &itime, NULL);
				break;
			case PAUSE_PULSE_CODE:
				itime.it_value.tv_sec = msg.pulse.value.sival_int;
				timer_settime(timer, 0, &itime, NULL);
				break;
			case QUIT_PULSE_CODE:
				timer_delete(timer);
				name_detach(attach, 0);
				name_close(srvr_coid);
				exit(EXIT_SUCCESS);
			case SET_PULSE_CODE:
				for (int i = 0; i < 8; i++) {
					if (t[i].tsbot == input_obj.timeSignatureBottom && t[i].tstop == input_obj.timeSignatureTop)
						index = i;
				}

				pattern = t[index].pattern;
				input_obj.timer.length = (double) 60 / input_obj.beatsPerMinute;
				input_obj.timer.measure = input_obj.timer.length * 2;
				input_obj.timer.interval = input_obj.timer.measure / input_obj.timeSignatureBottom;
				input_obj.timer.nano = input_obj.timer.interval * 1e+9;

				itime.it_value.tv_sec = 1;
				itime.it_value.tv_nsec = 0;
				itime.it_interval.tv_sec = input_obj.timer.interval;
				itime.it_interval.tv_nsec = input_obj.timer.nano;
				timer_settime(timer, 0, &itime, NULL);
				pattern = t[index].pattern;
				printf("\n");
				break;
			}
		}
		fflush(stdout);
	}
	return NULL;
}

int io_read(resmgr_context_t *ctp, io_read_t *msg, metro_t *metocb) {
	int nb;

	if(data == NULL){
		return 0;
	}

	if (metocb->ocb.attr->device == METRONOME) {
		sprintf(data, "[metronome: %d beats/min, time signature %d/%d, secs-per-beat: %.2f, nanoSecs: %ld]\n",
				input_obj.beatsPerMinute,
				input_obj.timeSignatureTop,
				input_obj.timeSignatureBottom,
				input_obj.timer.interval,
				input_obj.timer.nano);
	} else if (metocb->ocb.attr->device == HELP){
		sprintf(data,
				"Metronome Resource Manager (ResMgr)\n"
				"\nUsage: metronome <bpm> <ts-top> <ts-bottom>\n\nAPI:\n"
				"pause[1-9]                     -pause the metronome for 1-9 seconds\n"
				"quit                           -quit the metronome\n"
				"set <bpm> <ts-top> <ts-bottom> -set the metronome to <bpm> ts-top/ts-bottom\n"
				"start                          -start the metronome from stopped state\n"
				"stop                           -stop the metronome; use 'start' to resume\n");
	}

	nb = strlen(data);

	//test to see if we have already sent the whole message
	if (metocb->ocb.offset == nb)
		return 0;

	//We will return which ever is smaller the size of our data or the size of the buffer
	nb = min(nb, msg->i.nbytes);
	//Set the number of bytes we will return
	_IO_SET_READ_NBYTES(ctp, nb);
	//Copy data into reply buffer
	SETIOV(ctp->iov, data, nb);
	//update offset into our data used to determine start position for next read
	metocb->ocb.offset += nb;

	//If we are going to send any bytes update the access time for this resource
	if (nb > 0)
		metocb->ocb.flags |= IOFUNC_ATTR_ATIME;

	return(_RESMGR_NPARTS(1));
}

int io_write(resmgr_context_t *ctp, io_write_t *msg, metro_t *metocb) {

	int nb = 0;

	if (metocb->ocb.attr->device == HELP) {
		nb = msg->i.nbytes;
		_IO_SET_WRITE_NBYTES(ctp, nb);
		return _RESMGR_NPARTS(0);
	}

	if( msg->i.nbytes == ctp->info.msglen - (ctp->offset + sizeof(*msg) ))
	{
		/* have all the data */
		char *buf;
		char *pause_msg;
		char *set_msg;
		int small_integer = 0;
		buf = (char *)(msg+1);


		if(strstr(buf, "pause") != NULL){
			for(int i = 0; i < 2; i++){
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
			input_obj.beatsPerMinute = atoi(set_msg);
			set_msg = strsep(&buf," ");
			input_obj.timeSignatureTop = atoi(set_msg);
			set_msg = strsep(&buf," ");
			input_obj.timeSignatureBottom = atoi(set_msg);

			MsgSendPulse(srvr_coid, SchedGet(0,0,NULL), SET_PULSE_CODE, small_integer);
		} else {
			printf("\nPlease enter a valid command (cat /dev/local/metronome-help for legal commands\n");
			strcpy(data, buf);
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
	return (iofunc_open_default (ctp, msg, &handle->attr, extra));
}

metro_t *metro_calloc(resmgr_context_t *ctp, ioattr_t *ioattr) {
	metro_t *metro;
	metro = calloc(1, sizeof(metro_t));
	metro->ocb.offset = 0;
	return metro;
}

void metro_t_free(metro_t *metocb) {
	free(metocb);
}

