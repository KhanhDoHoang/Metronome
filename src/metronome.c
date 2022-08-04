#include "metronome.h"

metronome_t input_obj;
name_attach_t *attach;
int srvr_coid;
char data[512];

int main(int argc, char *argv[]) {

	//======================

	// verify number of command-line arguments == 4
	//   if argc != 4
	//	    then usage message and exit with FAILURE
	//
	//	  process the command-line arguments:
	//	    beats-per-minute
	//	    time-signature (top)
	//	    time-signature (bottom)
	//
	//	  implement main(), following simple_resmgr2.c and Lab7 as a guide
	//	    device path (FQN): /dev/local/metronome
	//	    create the metronome thread in-between calling resmgr_attach() and while(1) { ctp = dispatch_block(... }

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

	//===========

	//TODO: calculations for secs-per-beat, nanoSecs
	sprintf(data, "[metronome: %d beats/min, time signature %d/%d, secs-per-beat: %.2f, nanoSecs: %d]\n",
			input_obj.bpm,
			t[999].tstop,
			t[999].tsbot,
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

//metocb_t *metocb_calloc(resmgr_context_t *ctp, ioattr_t *ioattr) {
//
//}

void metocb_free(metro_t *metocb) {
	free(metocb);
}

