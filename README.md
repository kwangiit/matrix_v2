MATRIX: a MAny-Task computing execution fabRIc at eXascale
===============================================================================



===============================================================================
Section 1: MATRIX overview

MATRIX is a fully-distributed task execution framework for Many-Task Computing data intensive applications. MATRIX delegates a scheduler on each compute node to manage local resources and schedule tasks. MATRIX utilizes a distributed key-value store, i.e. ZHT, for distributed task metadata management. MATRIX implements a data-aware work stealing technique to optimize both load balancing and data-locality. MATRIX aims to deliver high throughput, high efficiency, and low latency for data-intensive applications in both scientific computing and Cloud domains.

===============================================================================



===============================================================================
Section 2: MATRIX Dependencies

[1] Google protocol buffers c binding
       https://github.com/protobuf-c/protobuf-c

[2] Google protocol buffers c++ binding
       https://github.com/google/protobuf

[3] ZHT distributed key-value store (no need to download the ZHT codebase, which is included in MATRIX)
       https://github.com/mierl/ZHT
===============================================================================



===============================================================================
Section 3: MATRIX compile and install (single node)

[step 1]  download MATRIX code to local machine, assuming the directory to download MATRIX is $dir:
            git clone https://github.com/kwangiit/matrix_v2.git

[step 2]  install google protocol buffer for both c and c++ bindings, refers to:
            https://github.com/mierl/ZHT

[step 3]  go to the matrix directory to compile ZHT:
            1. cd $dir/matrix_v2/ZHT/src
            2. make
            3. check whether ZHT is compiled correcly:
                if the command "./zhtserver -z zht.conf -n neighbor.conf" is running, then ZHT is compiled correctly

[step 4]  go to the matrix directory to compile MATRIX:
            1. cd $dir/matrix_v2/matrix/src
            2. make
===============================================================================



===============================================================================
Section 4: Running MATRIX (single node), assuming the directory to download MATRIX is $dir

[step 1]  specify the MATRIX membership list of all the schedulers (typically node ip):
            1. cd $dir/matrix_v2/matrix/src
            2. rm -rf memlist
            3. cat "your_ip_address" >> memlist (e.g. cat "138.32.143.100" >> memlist)
 
[step 2]  specify the ZHT membership list of all the servers:
            1. cd $dir/matrix_v2/ZHT/src
            2. rm -rf neighbor.conf
            3. cat "your_ip_address port_number" >> neighbor.conf (e.g. cat "138.32.143.100 50000" >> neighbor.conf)

[step 3]  specify the tasks that client is going to submit:
            1. the tasks are put in a workload file with each line specifying a task
            2. a task has the format: user directory command (e.g. kewang /bin/ hostname)
            3. assuming that you have a workload file: $dir/matrix_v2/matrix/src/workload

[step 4]  sepcify the configure file of MATRIX:
            1. the example configure file is located at: $dir/matrix_v2/matrix/src/config
            2. the specifications of the configure file is as follows:
		(1)	NumTaskPerClient        1000 // specify the number of tasks per client, usually there is one client, but you can have more
		(2)	NumAllTask      1000 // specify the number of all tasks (NumTaskPerClient * number of clients) 
		(3)	NumMapTask      0 // if it is a map reduce job, specify the number of map tasks. Otherwise set it to 0
		(4)	NumReduceTask   0 // if it is a map reduce job, specify the number of reduce tasks. Otherwise set it to 0
		(5)	NumCorePerExecutor      1 // specify the number of executing threads to exeucte tasks, usually equals to the number of physical cores of the machine
		(6) 	MaxTaskPerPkg   100 // specify the maximum number of tasks to send in a package, this is to limit the send and receive the buffer sizes of networking communication

		(7)	MonitorInterval 10000 // the monitoring interval of the client in micro-second
		(8)	SchedulerPortNo 60000 // the scheduler port number
		(9)	SleepLength     10000 // the sleep length used in the code whenever there is needed in micro-second
		(10)	WorkStealOn     1 // turn on/off the work stealing (1 means on, 0 means off)
		(11)	WorkStealPollIntervalStart      1000 // the initial polling interval of work stealing in micro-second
		(12)	WorkStealPollIntervalUpperBound 1000000 // the upper bound of the work stealing polling interval in micro-second

		(13)	DataSizeThreshold       1000 // the data size threshold to determine whether a task can be migrated or not in byte
		(14)	Policy  MDL // the scheduling policies (MDL, MLB, RLDS, FLDS)
		(15)	EstimatedTimeThreshold  20 // used by the FLDS policy to determine moving some tasks from local ready queue to work stealing ready queue
		(16)	SchedulerMemlistFile    /home/kwang/Documents/work_kwang/cppprogram/matrix/matrix_v2/matrix/src/memlist // the scheduler membership list
		(17)	NetworkProtocol TCP // the network communication protocol (TCP, UDP)
		(18)	DagType Pipeline // the workload DAG pattern (BOT, FanIn, FanOut, Pipeline)
		(19)	DagArgument     10 // the parameter of the workload DAG
		(20)	HostIdentityType        ip // the identity used to identify a scheduler (ip, hostname)

		(21)	SubmissionMode  bestcase // the workload submission mode used by client, either bestcase or worstcase. bestcase means to submit all the tasks in a load balanced way, while worst case menas to submit all the tasks to a random scheduler
		(22)	WorkloadFile    /home/kwang/Documents/work_kwang/cppprogram/matrix/matrix_v2/matrix/src/workload // the workload file
		(23)	SchedulerWorkloadPath   /home/kwang/Documents/work_kwang/cppprogram/matrix/matrix_v2/matrix/src/workload_dag // this is for complicated workload dags specified in a workload file. For simplicity, just specify it as $dir/matrix/src/workload_dag
		(24)	ClientLog       1 // whether to do the client-side logging (1 means do, 0 means don't)
		(25)	TaskLog 1 // whether to do per-task logging (1 means do, 0 means don't)
		(26)	SystemLog       1 // whether to do the system logging (1 means do, 0 means don't)
		(27)	SchedulerLog    1 // whether to do the scheduler logging (1 means do, 0 means don't)

		(28)	ZhtMemlistFile  /home/kwang/Documents/work_kwang/cppprogram/matrix/matrix_v2/ZHT/src/neighbor.conf // specify the ZHT memberlist file
		(30)	ZhtConfigFile   /home/kwang/Documents/work_kwang/cppprogram/matrix/matrix_v2/ZHT/src/zht.conf // specify the ZHT configuration file

[step 5]  run MATRIX:
            1. run ZHT server:
                (1) go to the ZHT src directory: cd $dir/matrix_v2/ZHT/src
                (2) run command: ./zhtserver -z zht.conf -n neighbor.conf
            2. run MATRIX server:
                (1) go to the MATRIX src directory: cd $dir/matrix_v2/matrix/src
                (2) run MATRIX scheduler: ./scheduler the_matrix_config_file
                (3) run MATRIX client: ./client the_matrix_config_file
===============================================================================



===============================================================================
Section 5: Potential Problems

[1]  google protocol buffer is not compatible
     solutions: recompile the .proto files in the source files of both matrix and ZHT:
	(1) go to the ZHT src folder: cd $dir/matrix_v2/ZHT/src
	(2) compile c++ bindings: protoc --cpp_out=. *.proto
	(3) compile c bindings: protoc-c --c_out=. *.proto
        (4) go to the matrix src folder: cd $dir/matrix_v2/matrix/src
	(5) repeat step 2 and step 4 
===============================================================================
