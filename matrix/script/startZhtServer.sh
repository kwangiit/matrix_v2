zhtSrcPath=$1
numNode=$2

zhtMemFile=$zhtSrcPath
zhtMemFile+=/neighbor


#rm -rf host
rm -rf $zhtMemFile

for i in `seq 1 $numNode`; do
	#echo node-$i.matrix.usrc.kodiak.nx >> host
	echo node-$i.matrix.usrc.kodiak.nx 50000 >> $zhtMemFile
done

echo $zhtSrcPath/zhtserver -z $zhtSrcPath/zht.conf -n $zhtMemFile

parallel-ssh -t 0 -o /tmp/ -p 250 -O StrictHostKeyChecking=no -O UserKnownHostsFile=/dev/null --hosts=host "rm -rf /tmp/zht.log"

parallel-ssh -t 0 -o /tmp/ -p 250 -O StrictHostKeyChecking=no -O UserKnownHostsFile=/dev/null --hosts=host "$zhtSrcPath/zhtserver -z $zhtSrcPath/zht.conf -n $zhtMemFile >& /tmp/zht.log" &

sleep 3

echo "Hello"
