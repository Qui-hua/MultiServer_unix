#!/bin/sh
#
#
######
######執行此shell要用絕對路徑執行
#



TDATE_TDY="$(date +'%Y%m%d')"
 
echo "Today is $TDATE_TDY"

# 停止 accsox
PIDS=`ps -ef | grep accsox |grep bin   | cut -c 10-17`

for PID in ${PIDS}; do
	echo "Kill ${PID}"
	kill -9 ${PID}
done
	
# 執行 accsox
nohup /hua_test/MultiServer/bin/accsox /hua_test/MultiServer/bin  &> /hua_test/MultiServer/bin/out  &
PIDS=`ps -ef | grep accsox |grep bin  | cut -c 10-17`
for PID in ${PIDS}; do
        echo "Run ${PID}"
done

