matrixscriptsrc=$1

parallel-ssh -t 0 -o /tmp/ -p 250 -O StrictHostKeyChecking=no -O UserKnownHostsFile=/dev/null --hosts=host "sudo cp $matrixscriptsrc/sysctl.conf /etc/; sudo sysctl -p"

parallel-ssh -t 0 -o /tmp/ -p 250 -O StrictHostKeyChecking=no -O UserKnownHostsFile=/dev/null --hosts=host "sudo cp $matrixscriptsrc/limits.conf /etc/security/"

sleep 3
