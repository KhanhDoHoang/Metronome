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

void startTimer(struct itimerspec *itime, timer_t timer_id, metronome_t *input_obj) {

}

void stopTimer(struct itimerspec *itime, timer_t timer_id) {

}

int io_read(resmgr_context_t *ctp, io_read_t *msg, metro_t *metocb) {
	int nb;

	if(data == NULL){
		return 0;
	}

	nb = strlen(data);

	//test to see if we have already sent the whole message.
	if (ocb->offset == nb)
		return 0;

	//We will return which ever is smaller the size of our data or the size of the buffer
	nb = min(nb, msg->i.nbytes);

	//Set the number of bytes we will return
	_IO_SET_READ_NBYTES(ctp, nb);

	//Copy data into reply buffer.
	SETIOV(ctp->iov, data, nb);

	//update offset into our data used to determine start position for next read.
	ocb->offset += nb;

	//If we are going to send any bytes update the access time for this resource.
	if (nb > 0)
		ocb->attr->flags |= IOFUNC_ATTR_ATIME;


	//===========

	//TODO: calculations for secs-per-beat, nanoSecs
	//sprintf(data, "[metronome: %d beats/min, time signature %d/%d, secs-per-beat: %.2f, nanoSecs: %d]\n",

	//nb = strlen(data);
	return(_RESMGR_NPARTS(1));
}

int io_write(resmgr_context_t *ctp, io_write_t *msg, metro_t *metocb) {
	/*
	 * if (buf == "pause")     // similar to alert <int> from Lab7
	 * process pauseValue
	 * validate pauseValue for range check
	 * MsgSendPulse(metronome_coid, priority, PAUSE_PULSE_CODE, pauseValue);
	 * if (buf == "quit")
	 * MsgSendPulse(metronome_coid, priority, QUIT_PULSE_CODE, NULL); // like Lab7
	 *
	 */
}

int io_open(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle, void *extra){
	//metronome_coid = name_open( "metronome", 0 );
	//return iofunc_default_open(...);
}

metocb_t *metocb_calloc(resmgr_context_t *ctp, ioattr_t *ioattr) {

}

void metocb_free(metro_t *metocb) {
	free(metocb);
}

