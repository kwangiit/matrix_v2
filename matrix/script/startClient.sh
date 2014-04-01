matrixSrcPath=$1
numClient=$2

rm -rf clientHost

for i in `seq 1 $numClient`; do
	echo node-$i.matrix.usrc.kodiak.nx >> clientHost
done

echo $matrixSrcPath/client $matrixSrcPath/config

parallel-ssh -t 0 -o /tmp/ -p 250 -O StrictHostKeyChecking=no -O UserKnownHostsFile=/dev/null --hosts=clientHost "$matrixSrcPath/client $matrixSrcPath/config"
