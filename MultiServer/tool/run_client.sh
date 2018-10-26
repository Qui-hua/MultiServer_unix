#!/bin/bash
i=100  # 這是累計的數值，亦即是 1, 2, 3....
while [ "${i}" != "200" ]
do
	
        perl client.pl ${i} &
        i=$(($i+1))   # 每次 i 都會增加 1
done
