# Metronome

The program operates a metronome using timers, and accepts pulses to pause the metronome for a number of seconds. To avoid noisy chaos in the lab, we’ll use visual output for the “click” of the metronome. A functional requirement has the metronome pause, which is done from the console using the echo command to send (i.e. write) pause 4 to the metronome device:

    # echo pause 4 > /dev/local/metronome
    
This means that the metronome will be implemented as a QNX resource manager for the “/dev/local/metronome” device. The resource manager code (i.e., the io_write(…) function) should send a pulse to the main thread of the metronome to have the metronome thread pause for the specified number of seconds. The “pause x” should pause the metronome for x seconds, where x is any integer value from 1 through 9, inclusive. Notice the metronome resmgr is multi-threaded: main thread (resmgr) and metronome thread (interval timer).

The metronome program accepts three parameters from the command-line:

    # metronome beats-per-minute time-signature-top time-signature-bottom
    
For example: # metronome 100 2 4

With output:
|1&2&
|1&2&
|1&2&
|…

(the “|1” characters occur every 2*60/100 = 1.2 sec per measure)

Another example: # metronome 200 5 4

With output:
|1&2&3&4-5-
|1&2&3&4-5-
|1&2&3&4-5-
|…
(the “|1” characters occur every 5*60/200 = 1.5 sec per measure)
