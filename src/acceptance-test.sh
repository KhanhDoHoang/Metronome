echo "********************************************"
echo "Unit Test [1] ./metronome"
echo "Expected: ussage message and process terminates (gracefully)"
./metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [2] ./metronome 120 2 4 &"
echo "Expected:1 measure per second. I will use this unit-test to verify the correct cadence of your metronome."
./metronome 120 2 4 &
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [3] cat /dev/local/metronome"
echo "Expected: metronome runs at [metronome: 120 beats/min, time signature 2/4, secs-per-interval: 0.25, nanoSecs: 250000000] " 
cat /dev/local/metronome
sleep 15

echo "\n"

echo "Unit Test [4] cat /dev/local/metronome-help"
echo "Expected: information regarding the metronome resmgr’s API, as seen above."
cat /dev/local/metronome-help
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [5] echo set 100 2 4 > /dev/local/metronome"
echo "Expected: metronome regmgr changes settings to: 100 bpm in 2/4 time; run-time behaviour of metronome changes to 100 bpm in 2/4 time."
echo set 100 2 4 > /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [6] cat /dev/local/metronome"
echo "Expected:  [metronome: 100 beats/min, time signature 2/4, secs-per- interval: 0.30, nanoSecs: 300000000]"
cat /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [7] echo set 200 5 4 > /dev/local/metronome"
echo "Expected: metronome regmgr changes settings to: 200 bpm in 5/4 time; run-time behaviour of metronome changes to 200 bpm in 5/4 time."
echo set 200 5 4 > /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [8] cat /dev/local/metronome"
echo "Expected: [metronome: 200 beats/min, time signature 5/4, secs-per- interval: 0.15, nanoSecs: 150000000]"
cat /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [9] echo stop > /dev/local/metronome"
echo "Expected: metronome stops running; metronome resmgr is still running as a process: pidin | grep metronome."
echo stop > /dev/local/metronome 
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [10] start > /dev/local/metronome"
echo "Expected:  metronome starts running again at 200 bpm in 5/4 time, which is the last setting;"
echo "metronome resmgr is still running as a process: pidin | grep metronome."
echo start > /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [11] cat /dev/local/metronome"
echo "Expected: [metronome: 200 beats/min, time signature 5/4, secs-per- interval: 0.15, nanoSecs: 150000000]"
cat /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [12] echo stop > /dev/local/metronome"
echo "Expected: metronome stops running; metronome resmgr is still running as a process: pidin | grep metronome."
echo stop > /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [12] echo stop > /dev/local/metronome"
echo "Expected: metronome remains stopped; metronome resmgr is still running as a process: pidin | grep metronome."
echo stop > /dev/local/metronome
sleep 15
 
echo "\n"
 

echo "********************************************"
echo "Unit Test [14] echo start > /dev/local/metronome"
echo "Expected: metronome starts running again at 200 bpm in 5/4 time, which is the last setting;"
echo "metronome resmgr is still running as a process: pidin | grep metronome"
echo start > /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [15] echo start > /dev/local/metronome"
echo "Expected: metronome is still running again at 200 bpm in 5/4 time, which is the last setting;"
echo "metronome resmgr is still running as a process: pidin | grep metronome"
echo start > /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [16] echo cat /dev/local/metronome"
echo "Expected: [metronome: 200 beats/min, time signature 5/4, secs-per- interval: 0.15, nanoSecs: 150000000]"
cat /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [17] echo pause 3 > /dev/local/metronome"
echo "Expected: metronome continues on next beat (not next measure)"
echo pause 3 > /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [18] echo pause 10 > /dev/local/metronome"
echo "Expected:  properly formatted error message, and metronome continues to run."
echo pause 10 > /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [19] echo bogus > /dev/local/metronome"
echo "Expected:  properly formatted error message, and metronome continues to run."
echo bogus > /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [20] echo set 120 2 4 > /dev/local/metronome"
echo "Expected: 1 measure per second. I will use this unit-test to verify the correct cadence of your metronome."
echo "Let your metronome run for 3 to 5 seconds so we can observe the metronome’s run-time behaviour."
echo set 120 2 4 > /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [20] cat /dev/local/metronome"
echo "Expected: [metronome: 120 beats/min, time signature 2/4, secs-per-interval: 0.25, nanoSecs: 250000000]"
cat /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [22] cat /dev/local/metronome-help"
echo "Expected: information regarding the metronome resmgr’s API, as seen above."
cat /dev/local/metronome-help
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [24] cat /dev/local/metronome"
echo "Expected: [metronome: 120 beats/min, time signature 2/4, secs-per-interval: 0.25, nanoSecs: 250000000]"
cat /dev/local/metronome
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [25] echo Writes-Not-Allowed > /dev/local/metronome-help"
echo "Expected: properly formatted error message, and metronome continues to run."
echo Writes-Not-Allowed > /dev/local/metronome-help
sleep 15

echo "\n"

echo "********************************************"
echo "Unit Test [26] echo quit > /dev/local/metronome && pidin | grep metronome"
echo "Expected: metronome gracefully terminates."
echo quit > /dev/local/metronome && pidin | grep metronome
sleep 15

echo "\n"

echo "********************************************"
echo quit > /dev/local/metronome
exit 0