#!/bin/bash

numNode=$1
numTaskPerClient=$2
numAllTask=$3
matrixSrcPath=$4
zhtSrcPath=$5
DagType=$6
DagArgument=$7
numMapTask=$8
numReduceTask=$9

numClient=$(($numAllTask/$numTaskPerClient))

#rm -rf host

#for i in `seq 1 $numNode`; do
#        echo node-$i.matrix.usrc.kodiak.nx >> host
#done

#cp host $matrixSrcPath/memlist


#copy the memberlist file to all the nodes
IFS=$'\n'
set -f

echo "Host *" >> ~/.ssh/config
echo " IdentityFile ~/MATRIX_KEY.pem" >> ~/.ssh/config
echo " StrictHostKeyChecking no" >> ~/.ssh/config
echo " UserKnownHostsFile /dev/null" >> ~/.ssh/config

for i in $(cat host); do
	ssh -o 'StrictHostKeyChecking no' -o 'UserKnownHostsFile /dev/null' ~/.ssh/config $i:/home/ubuntu/.ssh/config
done

for i in $(cat host); do
	scp -o 'StrictHostKeyChecking no' -o 'UserKnownHostsFile /dev/null' host $i:$matrixSrcPath/memlist
done

#parallel-ssh -t 0 -o /tmp/ -p 250 -O StrictHostKeyChecking=no -O UserKnownHostsFile=/dev/null --hosts=host "sudo cp /users/kwangiit/sc14/matrix_v2/script/ld.so.conf /etc/; sudo ldconfig"


echo "change the system maximum number of open files"
bash sysChange.sh
echo "done with changing the system maximum number of open files"

echo "before start zht server"
bash startZhtServer.sh $zhtSrcPath $numNode
echo "zht server has been started"

bash genConfig.sh $numTaskPerClient $numAllTask $matrixSrcPath $zhtSrcPath $DagType $DagArgument

bash genWorkload.sh $numTaskPerClient $matrixSrcPath

#$matrixSrcPath/client $matrixSrcPath/config &

echo "before start schedulers"
bash startScheduler.sh $matrixSrcPath
echo "schedulers have been started"

rm -rf client.config
$matrixSrcPath/client $matrixSrcPath/config >& client.config
#echo "before start the client"
#bash startClient.sh $matrixSrcPath $numClient
#echo "client has been finished"

echo "before killing"
bash killing.sh
echo "I am done"
