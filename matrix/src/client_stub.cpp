/*
 * client_stub.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "client_stub.h"
#include "math.h"

MatrixClient::MatrixClient(const string &configFile) :
		Peer(configFile) {
	clock_gettime(0, &start);

	taskVec = read_from_file(config->workloadFile);

	string base("");
	base.append(num_to_str<int>(schedulerVec.size()));
	base.append(".");
	base.append(num_to_str<long>(config->numTaskPerClient));
	string suffix = base + "." + num_to_str<int>(get_index());

	/* only the first client (index=0) is logging */
	if (config->clientLog == 1 && get_index() == 0) {
		string clientLogFile("./client." + suffix);
		clientLogOS.open(clientLogFile.c_str());
	}

	if (config->systemLog == 1 && get_index() == 0) {
		string systemLogFile("./system." + suffix);
		systemLogOS.open(systemLogFile.c_str());
	}

	clock_gettime(0, &end);
	timespec diff = time_diff(start, end);

#ifdef PRINT_OUT
	cout << "I am a Matrix Client, it takes me " << diff.tv_sec << "s, "
	"and " << diff.tv_nsec << " ns for initialization!" << endl;
#endif

	if (clientLogOS.is_open()) {
		clientLogOS << "I am a Matrix Client, it takes me " << diff.tv_sec
				<< "s, and " << diff.tv_nsec << " ns for initialization!"
				<< endl;
	}

	srand(time(NULL));
}

MatrixClient::~MatrixClient() {

}
/* insert task information to ZHT
 * the tasks have already been represented as DAGs
 * that are formed with adjecency list (dagAdjList)
 * and parent list (dagInDegree)
 * */
void MatrixClient::insert_taskinfo_to_zht(adjList &dagAdjList,
		inDegree &dagInDegree) {
#ifdef PRINT_OUT
	cout << "------------------------------------------------------------" << endl;
	cout << "Now, I am going to insert task information to ZHT" << endl;
#endif

	if (clientLogOS.is_open()) {
		clientLogOS
				<< "------------------------------------------------------------"
				<< endl;
		clientLogOS << "Now, I am going to insert task information to ZHT"
				<< endl;
	}

	clock_gettime(0, &start);

	for (adjList::iterator it = dagAdjList.begin(); it != dagAdjList.end();
			++it) {
		stringstream ss;
		ss << get_index() << it->first;
		string taskId(ss.str());

		vector<long> existList = it->second;
		long inDegree = dagInDegree[it->first];

		Value value;
		value.set_id(taskId);
		value.set_indegree(inDegree);

		for (long i = 0; i < existList.size(); i++) {
			stringstream ssChild;
			ssChild << get_index() << existList.at(i);
			string sChild(ssChild.str());

			value.add_children(sChild);
		}

		/*value.set_nummove(0);
		 value.set_history("|" + get_id());
		 value.set_submittime(0.0);
		 value.set_arrivetime(0.0);
		 value.set_rqueuedtime(0.0);
		 value.set_exetime(0.0);
		 value.set_fintime(0.0);*/

		string seriValue;
		seriValue = value_to_str(value);
		zc.insert(taskId, seriValue);
	}

//	map<string, int> fileMap; // storing file locations <filename, location>
//
//	/*string filePath("/users/kwangiit/sc14/matrix_v2/matrix/src/workload_dag/file_" + num_to_str<int>(schedulerVec.size()) + "_" +
//	 num_to_str<double>(config->locality) + ".0_" + num_to_str<int>(
//	 config->numFile) + "_" + num_to_str<int>(config->numTaskPerClient));
//	 string taskPath("/users/kwangiit/sc14/matrix_v2/matrix/src/workload_dag/task_" + num_to_str<int>(schedulerVec.size()) + "_" +
//	 num_to_str<double>(config->locality) + ".0_" + num_to_str<int>(
//	 config->numFile) + "_" + num_to_str<int>(config->numTaskPerClient));*/
//
//	//cout << "file path is:" << filePath << ", and task path is:" << taskPath << endl;
//	/* file that defines file locations */
//	string filePath(
//			"./workload_dag/file." + num_to_str<int>(schedulerVec.size()) + "."
//					+ num_to_str<int>(config->numTaskPerClient));
//
//	/* task specifications: task input/out datasize,
//	 * task input data files, task type: map/reduce, task length */
//	string taskPath(
//			"./workload_dag/task." + num_to_str<int>(schedulerVec.size()) + "."
//					+ num_to_str<int>(config->numTaskPerClient));
//	/*vector<string> fileStr = read_from_file(filePath);
//	 for (int i = 0; i < fileStr.size(); i++)
//	 {
//	 vector<string> lineVec = tokenize(fileStr.at(i), " ");
//	 fileMap.insert(make_pair(lineVec.at(0), str_to_num<int>(lineVec.at(1))));
//	 }*/
//
//	/* This is to know which file is at which compute node */
//	vector<string> fileStr = read_from_file(filePath);
//	for (int i = 0; i < fileStr.size(); i++) {
//		vector<string> lineVec = tokenize(fileStr.at(i), " ");
//		fileMap.insert(
//				make_pair(lineVec.at(0), str_to_num<int>(lineVec.at(1))));
//	}
//
//	vector<string> taskStr = read_from_file(taskPath);
//	//string lastTaskId(num_to_str<int>(get_index()) + num_to_str<int>(taskStr.size() + 1));
//
////	for (int i = 0; i < taskStr.size(); i++)
////	{
////		vector<string> lineVec = tokenize(taskStr.at(i), " ");
////		string taskId(num_to_str<int>(get_index()) + lineVec.at(0));
////		Value value;
////		value.set_id(taskId);
////		value.set_indegree(0);
////		value.add_parents(schedulerVec.at(fileMap.find(lineVec.at(1))->second));
////		value.add_datanamelist(lineVec.at(1));
////		value.add_datasize(2097152);
////		value.set_alldatasize(2097152);
////		//value.add_children(lastTaskId);
////		string seriValue = value_to_str(value);
////		zc.insert(taskId, seriValue);
////		//cout << "Task id is:" << taskId << ", and value is:" << seriValue << endl;
////		//
////		//		for (long i = 0; i < existList.size(); i++)
////		//		{
////		//			stringstream ssChild;
////		//			ssChild << get_index() << existList.at(i);
////		//			string sChild(ssChild.str());
////		//
////		//			value.add_children(sChild);
////		//		}
////	}
//
//	/* task has the format: taskid map/reduce filename inputfilesize outputfilesize executelength*/
//	for (int i = 0; i < taskStr.size(); i++) {
//		vector<string> lineVec = tokenize(taskStr.at(i), " ");
//		string taskId(num_to_str<int>(get_index()) + lineVec.at(0));
//		Value value;
//		value.set_id(taskId);
//		if (lineVec.at(1).compare("map") == 0) {
//			value.set_indegree(0);
//			value.add_datasize(str_to_num<long>(lineVec.at(2)));
//			value.set_alldatasize(str_to_num<long>(lineVec.at(2)));
//			value.set_outputsize(str_to_num<long>(lineVec.at(3)));
//			value.set_tasklength(str_to_num<long>(lineVec.at(4)));
//			value.add_parents(
//					schedulerVec.at(fileMap.find(lineVec.at(5))->second));
//			value.add_datanamelist(lineVec.at(5));
//			for (int i = 0; i < config->numReduceTask; i++) {
//				stringstream ssChild;
//				ssChild << get_index() << (i + config->numMapTask);
//				string sChild(ssChild.str());
//				value.add_children(sChild);
//			}
//		} else {
//			value.set_indegree(config->numMapTask);
//			value.set_tasklength(str_to_num<long>(lineVec.at(2)));
//		}
//
//		//value.add_children(lastTaskId);
//		string seriValue = value_to_str(value);
//		zc.insert(taskId, seriValue);
//		//cout << "Task id is:" << taskId << ", and value is:" << seriValue << endl;
//		//
//		//		for (long i = 0; i < existList.size(); i++)
//		//		{
//		//			stringstream ssChild;
//		//			ssChild << get_index() << existList.at(i);
//		//			string sChild(ssChild.str());
//		//
//		//			value.add_children(sChild);
//		//		}
//	}

//	Value value;
//	value.set_id(lastTaskId);
//	value.set_indegree(taskStr.size());
//	string seriValue = value_to_str(value);
//	zc.insert(lastTaskId, seriValue);

	incre_ZHT_msg_count(config->numTaskPerClient);

	clock_gettime(0, &end);
	timespec diff = time_diff(start, end);

#ifdef PRINT_OUT
	cout << "I am done, the time taken is:" << diff.tv_sec
	<< " s, and " << diff.tv_nsec << " ns" << endl;
	cout << "--------------------------------"
	"----------------------------" << endl;
#endif

	if (clientLogOS.is_open()) {
		clientLogOS << "I am done, the time taken is:" << diff.tv_sec
				<< " s, and " << diff.tv_nsec << " ns" << endl;
		clientLogOS << "--------------------------------"
				"----------------------------" << endl;
	}
}

/* initialize all the tasks by assigning
 * taskId for each individual task
 * */
void MatrixClient::init_task() {
	for (long i = 0; i < config->numTaskPerClient; i++) {
		stringstream ss;
		ss << get_index() << i;
		string taskId(ss.str());

		vector<string> taskItemStr = tokenize(taskId + " " + taskVec.at(i),
				" ");
		TaskMsg tm;
		tm.set_taskid(taskItemStr.at(0));
		tm.set_user(taskItemStr.at(1));
		tm.set_dir(taskItemStr.at(2));
		tm.set_cmd(taskItemStr.at(3));
		tm.set_datalength(0);
		tasks.push_back(tm);
	}
}

/* submit tasks to the schedulers, either with
 * the best case scenario or worst case scenario
 * */
void MatrixClient::submit_task() {
	/* current time to be set as the submission
	 * time of all the tasks. This might be not
	 * accurate with tasks sent batch by batch
	 * */
	long increment = 0;

#ifdef PRINT_OUT
	cout << "--------------------------------"
	"----------------------------" << endl;
	cout << "Now, I am going to submit tasks to the schedulers" << endl;
#endif

	if (clientLogOS.is_open()) {
		clientLogOS << "--------------------------------"
				"----------------------------" << endl;
		clientLogOS << "Now, I am going to submit "
				"tasks to the schedulers" << endl;
	}

	clock_gettime(0, &start);

	int size = -1;
	for (long i = 0; i < config->numTaskPerClient; i++) {
		string taskId = tasks.at(i).taskid();
		string taskDetail;
		zc.lookup(taskId, taskDetail);
		Value value = str_to_value(taskDetail);
		value.set_submittime(get_time_usec());

		taskDetail = value_to_str(value);
		zc.insert(taskId, taskDetail);

		increment += 2;
	}

	incre_ZHT_msg_count(increment);

	/* if the submission mode is best case */
	if (config->submitMode.compare("bestcase") == 0) {
		submit_task_bc();
	} else if (config->submitMode.compare("worstcase") == 0) {
		/* otherwise, do the worst case scenario by randomly
		 * selecting a scheduler to submit all the tasks
		 * */
		int scheIdx = rand() % schedulerVec.size();
		submit_task_wc(tasks, scheIdx);
	}

	clock_gettime(0, &end);
	timespec diff = time_diff(start, end);

#ifdef PRINT_OUT
	cout << "It took " << diff.tv_sec << "s, and "
	<< diff.tv_nsec << " ns" << endl;
	cout << "--------------------------------"
	"----------------------------" << endl;
#endif

	if (clientLogOS.is_open()) {
		clientLogOS << "It took " << diff.tv_sec << "s, and " << diff.tv_nsec
				<< " ns" << endl;
		clientLogOS << "--------------------------------"
				"----------------------------" << endl;
	}
	//wait_all_task_recv();

	clock_gettime(0, &start);
}

/* This is to wait until all the tasks been queued in the wait queue in all schedulers */
//void MatrixClient::split_task() {
//	zc.insert(string("Split Workload"), string("done"));
//	wait_all_task_recv();
//	clock_gettime(0, &start);
//}
/* submit tasks with the best case scenario, in which,
 * all the tasks are splited evenly to all the
 * schedulers in a interleaved way
 * */

void MatrixClient::submit_task_bc() {
	int toScheIdx = -1, numSche = schedulerVec.size();

	vector<vector<TaskMsg> > tasksVec;
	for (int i = 0; i < numSche; i++) {
		tasksVec.push_back(vector<TaskMsg>());
	}

	for (int i = 0; i < config->numTaskPerClient; i++) {
		toScheIdx = i % numSche;
		tasksVec[toScheIdx].push_back(tasks.at(i));
	}

	/* as long as all the tasks are distributed evenly,
	 * the submission to individual scheduler is like
	 * the worst case
	 * */
	for (int i = 0; i < numSche; i++) {
		submit_task_wc(tasksVec[i], i);
	}
}

/* submit tasks with the worst case scenario, in which,
 * all the tasks (listed in "taskVec") are submitted to
 * one scheduler (index is "toScheIdx")
 * */
void MatrixClient::submit_task_wc(vector<TaskMsg> tmVec, int toScheIdx) {
	int sockfd = create_sock(schedulerVec.at(toScheIdx),
			config->schedulerPortNo);
	if (sockfd == -1)
		return;
	send_batch_tasks(tmVec, sockfd, "client");
	close(sockfd);
}

/* monitoring thread function, monitoring is conducted only by client 0.
 * It can monitor the execution progress of all the tasks, the system
 * status, and log all the task details
 * */
void *monitoring(void* args) {
	MatrixClient *mc = (MatrixClient*) args;
	string key("num tasks done");
	long numAllCore = mc->config->numCorePerExecutor * mc->schedulerVec.size();
	long numIdleCore = 0;
	long numTaskWait = 0, numTaskReady = 0;
	long preNumTaskDone = 0, numTaskDone = 0;
	long prevTimeUs = 0L, currentTimeUs = 0L;
	double instantThr = 0.0;

	string numTaskFinStr;

	/* system status log head */
	if (mc->systemLogOS.is_open()) {
		mc->systemLogOS << "Time(us)\tNumAllCore\tNumIdleCore\tNumTaskWait\t"
				"NumTaskReady\tNumTaskDone\tThroughput" << endl;
	}

	long increment = 0;

	while (1) {
		mc->zc.lookup(key, numTaskFinStr);	// lookup how many tasks are done
		numTaskDone = str_to_num<long>(numTaskFinStr);
		cout << "number of task done is:" << numTaskDone << endl;
		increment++;

		/* log the instant system status */
		if (mc->systemLogOS.is_open()) {
			currentTimeUs = get_time_usec();
			for (int i = 0; i < mc->schedulerVec.size(); i++) {
				string schedulerStat;
				mc->zc.lookup(mc->schedulerVec.at(i), schedulerStat);
				if (schedulerStat.empty())
					continue;

				Value value = str_to_value(schedulerStat);
				numIdleCore += value.numcoreavilable();
				numTaskWait += value.numtaskwait();
				numTaskReady += value.numtaskready();
			}

			increment += mc->schedulerVec.size();

			instantThr = (double) (numTaskDone - preNumTaskDone)
					/ (currentTimeUs - prevTimeUs) * 1E6;

			mc->systemLogOS << currentTimeUs << "\t" << numAllCore << "\t"
					<< numIdleCore << "\t" << numTaskWait << "\t"
					<< numTaskReady << "\t" << numTaskDone << "\t" << instantThr
					<< endl;

			preNumTaskDone = numTaskDone;
			prevTimeUs = currentTimeUs;
			numIdleCore = 0;
			numTaskWait = 0;
			numTaskReady = 0;
		}

		if (numTaskDone == mc->config->numAllTask)	// all the tasks are done
			break;
		else
			usleep(mc->config->monitorInterval);	// sleep sometime
	}

	clock_gettime(0, &mc->end);
	timespec diff = time_diff(mc->start, mc->end);
	double time = (double) diff.tv_sec + (double) diff.tv_nsec / 1E9;
	double throughput = mc->config->numAllTask / time;

#ifdef PRINT_OUT
	cout << "It takes " << diff.tv_sec << "s, and " << diff.tv_nsec <<
	" ns to finish " << mc->config->numAllTask << " tasks" << endl;
	cout << "The overall throughput is:" << throughput << endl;
#endif

	if (mc->clientLogOS.is_open()) {
		mc->clientLogOS << "It takes " << diff.tv_sec << "s, and "
				<< diff.tv_nsec << " ns to finish " << mc->config->numAllTask
				<< " tasks" << endl;
		mc->clientLogOS << "The overall throughput is:" << throughput << endl;
	}

	if (mc->systemLogOS.is_open()) {
		mc->systemLogOS.flush();
		mc->systemLogOS.close();
	}

	mc->incre_ZHT_msg_count(increment);

#ifdef PRINT_OUT
	cout << "The number of ZHT message is:" << mc->numZHTMsg << endl;
#endif

	if (mc->clientLogOS.is_open()) {
		mc->clientLogOS << "The number of ZHT message is:" << mc->numZHTMsg
				<< endl;
		mc->clientLogOS.flush();
		mc->clientLogOS.close();
	}

	return NULL;
}

void MatrixClient::do_monitoring() {
	if (get_index() != 0)
		return;

	pthread_t monThread;

	while (pthread_create(&monThread, NULL, monitoring, this) != 0)
		sleep(1);

	pthread_join(monThread, NULL);
}
