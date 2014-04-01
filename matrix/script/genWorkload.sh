numTaskPerClient=$1
matrixSrcPath=$2

workloadFile=$matrixSrcPath
workloadFile+=/workload

rm -rf $workloadFile

for i in `seq 1 $numTaskPerClient`; do echo "kwang /home/kwang/Documents hostname" >> $workloadFile ; done
