#!/bin/bash

if [ $# -ne 8 ] then
	echo Usage: $0 $numNode $numTaskPerClient $matrixPath $zhtSrcPath $DagType $DagArgument $numMapTask $numReduceTask
	exit 1
fi

numNode=$1
numTaskPerClient=$2
matrixPath=$3
zhtSrcPath=$4
DagType=$5
DagArgument=$6
numMapTask=$7
numReduceTask=$8

IFS=$'\n'
set -f

#copy the ssh config file to all nodes
echo "copy the ssh config file to all nodes"
rm -rf ~/.ssh/config
echo "Host *" >> ~/.ssh/config
echo " IdentityFile ~/MATRIX_KEY.pem" >> ~/.ssh/config
echo " StrictHostKeyChecking no" >> ~/.ssh/config
echo " UserKnownHostsFile /dev/null" >> ~/.ssh/config
for i in $(cat host); do
	scp -o 'StrictHostKeyChecking no' -o 'UserKnownHostsFile /dev/null' ~/.ssh/config $i:/home/ubuntu/.ssh/config
done
echo "done with copying the ssh config file"

#copy the MATRIX member list file to all nodes
echo "copy the MATRIX member list file to all nodes"
for i in $(cat host); do
	scp -o 'StrictHostKeyChecking no' -o 'UserKnownHostsFile /dev/null' ./host $i:$matrixPath/src/memlist
done
echo "done with copying the MATRIX member list file"

#change the system maximum number of open files
echo "change the system maximum number of open files"
bash sysChange.sh $matrixPath/script
echo "done with changing the system maximum number of open files"

#start the zht servers on all the nodes
echo "start the zht servers on all the nodes"
bash startZhtServer.sh $zhtSrcPath
echo "zht server has been started"

#generate MATRIX configuration file
echo "generate MATRIX configuration file"
bash genConfig.sh $numTaskPerClient $matrixPath/src $zhtSrcPath $DagType $DagArgument 0 0
echo "MATRIX configuration file has already been generated"

#generate workload
echo "generate workload"
bash genWorkload.sh $numTaskPerClient $matrixPath/src
echo "has generated workload"

#start all MATRIX schedulers
echo "start all MATRIX schedulers"
bash startScheduler.sh $matrixPath/src
echo "MATRIX schedulers have been started"

#start MATRIX client
echo "start MATRIX client"
rm -rf client.config
$matrixPath/src/client $matrixPath/src/config >& client.config
echo "experiment has been finished"

#kill all the programs
echo "before killing"
bash killing.sh
echo "I am done"
