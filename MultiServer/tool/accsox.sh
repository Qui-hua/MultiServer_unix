#!/bin/sh
#
#
######
######���榹shell�n�ε�����|����
#



TDATE_TDY="$(date +'%Y%m%d')"
 
echo "Today is $TDATE_TDY"

# ���� accsox
PIDS=`ps -ef | grep accsox |grep bin   | cut -c 10-17`

for PID in ${PIDS}; do
	echo "Kill ${PID}"
	kill -9 ${PID}
done
	
# ���� accsox
nohup /hua_test/MultiServer/bin/accsox /hua_test/MultiServer/bin  &> /hua_test/MultiServer/bin/out  &
PIDS=`ps -ef | grep accsox |grep bin  | cut -c 10-17`
for PID in ${PIDS}; do
        echo "Run ${PID}"
done

