#!/bin/bash

zhtSrcPath=$1
numNode=$2

zhtMemFile=$zhtSrcPath
zhtMemFile+=/neighbor.conf


#rm -rf host
rm -rf $zhtMemFile

IFS=$'\n'
set -f

for i in $(cat host); do
        echo "$i 50000" >> $zhtMemFile
	#ssh -o 'StrictHostKeyChecking no' -o 'UserKnownHostsFile /dev/null' ~/.ssh/config $i:/home/ubuntu/.ssh/config
done

#for i in `seq 1 $numNode`; do
	#echo node-$i.matrix.usrc.kodiak.nx >> host
#	echo node-$i.matrix.usrc.kodiak.nx 50000 >> $zhtMemFile
#done

for i in $(cat host); do
	scp -o 'StrictHostKeyChecking no' -o 'UserKnownHostsFile /dev/null' $zhtMemFile $i:/home/ubuntu/.ssh/config
done

echo $zhtSrcPath/zhtserver -z $zhtSrcPath/zht.conf -n $zhtMemFile

#parallel-ssh -t 0 -o /tmp/ -p 250 -O StrictHostKeyChecking=no -O UserKnownHostsFile=/dev/null --hosts=host "rm -rf /tmp/zht.log"

parallel-ssh -t 0 -o /tmp/ -p 250 -O StrictHostKeyChecking=no -O UserKnownHostsFile=/dev/null --hosts=host "$zhtSrcPath/zhtserver -z $zhtSrcPath/zht.conf -n $zhtMemFile >& /tmp/zht.log" &

sleep 3

echo "Hello"
