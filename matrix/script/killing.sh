parallel-ssh -t 0 -o /tmp/ -p 250 -O StrictHostKeyChecking=no -O UserKnownHostsFile=/dev/null --hosts=host "killall -9 zhtserver"

parallel-ssh -t 0 -o /tmp/ -p 250 -O StrictHostKeyChecking=no -O UserKnownHostsFile=/dev/null --hosts=host "killall -9 scheduler"
