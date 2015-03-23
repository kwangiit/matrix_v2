#!/bin/bash

zhtSrcPath=$1

rm -rf $zhtSrcPath/neighbor.conf

IFS=$'\n'
set -f

#generate the ZHT member list file
for i in $(cat host); do
        echo "$i 50000" >> $zhtSrcPath/neighbor.conf
done

#copy the ZHT member list to all nodes
for i in $(cat host); do
	scp -o 'StrictHostKeyChecking no' -o 'UserKnownHostsFile /dev/null' $zhtSrcPath/neighbor.conf $i:$zhtSrcPath
done

#run the ZHT server on all nodes
echo "start to run ZHT server on all nodes"
parallel-ssh -t 0 -o /tmp/ -p 250 -O StrictHostKeyChecking=no -O UserKnownHostsFile=/dev/null --hosts=host "$zhtSrcPath/zhtserver -z $zhtSrcPath/zht.conf -n $zhtSrcPath/neighbor.conf >& /tmp/zht.log" &
echo "ZHT servers are running"

sleep 3
