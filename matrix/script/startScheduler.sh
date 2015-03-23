matrixSrcPath=$1

parallel-ssh -t 0 -o /tmp/ -p 250 -O StrictHostKeyChecking=no -O UserKnownHostsFile=/dev/null --hosts=host "rm -rf /tmp/scheduler.log"

parallel-ssh -t 0 -o /tmp/ -p 250 -O StrictHostKeyChecking=no -O UserKnownHostsFile=/dev/null --hosts=host "$matrixSrcPath/scheduler $matrixSrcPath/config >& /tmp/scheduler.log" &

sleep 3
