numTaskPerClient=$1
matrixSrcPath=$2
zhtSrcPath=$3
DagType=$4
DagArgument=$5
numMapTask=$6
numReduceTask=$7

workloadFile=$matrixSrcPath
workloadFile+=/workload

memFile=$matrixSrcPath
memFile+=/memlist

zhtCfgFile=$zhtSrcPath
zhtCfgFile+=/zht.conf

zhtmemFile=$zhtSrcPath
zhtmemFile+=/neighbor.conf

matrixConfig=$matrixSrcPath
matrixConfig+=/config

echo -e "NumTaskPerClient\t$numTaskPerClient\nNumAllTask\t$numTaskPerClient\nNumMapTask\t$numMapTask\nNumReduceTask\t$numReduceTask\nNumCorePerExecutor\t2\nMaxTaskPerPkg\t100\n\nMonitorInterval\t10000\nSchedulerPortNo\t60000\nSleepLength\t100000\nWorkStealOn\t1\nWorkStealPollIntervalStart\t1000\nWorkStealPollIntervalUpperBound\t1000000\n\nDataSizeThreshold\t1000\nPolicy\tMDL\nEstimatedTimeThreshold\t20\nSchedulerMemlistFile\t$memFile\nNetworkProtocol\tTCP\nDagType\t$DagType\nDagArgument\t$DagArgument\nHostIdentityType\tip\n\nSubmissionMode\tbestcase\nWorkloadFile\t$workloadFile\nSchedulerWorkloadPath\t$matrixSrcPath/workload_dag\nClientLog\t1\nTaskLog\t1\nSystemLog\t1\nSchedulerLog\t1\n\nZhtMemlistFile\t$zhtmemFile\nZhtConfigFile\t$zhtCfgFile" > $matrixConfig

