/*
 * client_stub.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "client_stub.h"

MatrixClient::MatrixClient(const string &configFile) : Peer(configFile)
{
	clock_gettime(0, &start);

	taskVec = read_from_file(config->workloadFile);

	string base("");
	base.append(num_to_str<int>(schedulerVec.size()));
	base.append("_");
	base.append(num_to_str<long>(config->numTaskPerClient));

	string indexStr = num_to_str<int>(get_index());
	string suffix = base + "_" + indexStr;

	/* only the first client (index=0) is logging */
	if (config->clientLog == 1 && get_index() == 0)
	{
		string clientLogFile("./client_" + suffix);
		clientLogOS.open(clientLogFile.c_str());
	}
	/*if (config->taskLog == 1 && get_index() == 0)
	{
		string taskLogFile("./task_" + suffix);
		taskLogOS.open(taskLogFile.c_str());
	}*/
	if (config->systemLog == 1 && get_index() == 0)
	{
		string systemLogFile("./system_" + suffix);
		systemLogOS.open(systemLogFile.c_str());
	}

	clock_gettime(0, &end);
	timespec diff = time_diff(start, end);

#ifdef PRINT_OUT
	cout << "I am a Matrix Client, it takes me " << diff.tv_sec << "s, "
			"and " << diff.tv_nsec << " ns for initialization!" << endl;
#endif

	if (clientLogOS.is_open())
	{
		clientLogOS << "I am a Matrix Client, it takes me " << diff.tv_sec << "s, "
				"and " << diff.tv_nsec << " ns for initialization!" << endl;
	}
}

MatrixClient::~MatrixClient()
{

}
/* insert task information to ZHT
 * the tasks have already been represented as DAGs
 * that are formed with adjecency list (dagAdjList)
 * and parent list (dagInDegree)
 * */
void MatrixClient::insert_taskinfo_to_zht(
		adjList &dagAdjList, inDegree &dagInDegree)
{
#ifdef PRINT_OUT
	cout << "------------------------------"
			"------------------------------" << endl;
	cout << "Now, I am going to insert task information to ZHT" << endl;
#endif

	if (clientLogOS.is_open())
	{
		clientLogOS << "-------------------------------"
				"-----------------------------" << endl;
		clientLogOS << "Now, I am going to insert "
				"task information to ZHT" << endl;
	}

	clock_gettime(0, &start);

	for (adjList::iterator it = dagAdjList.begin();
						it != dagAdjList.end(); ++it)
	{
		stringstream ss;
		ss << get_index() << it->first;
		string taskId(ss.str());

		vector<long> existList = it->second;
		long inDegree = dagInDegree[it->first];

		Value value;
		value.set_id(taskId);
		value.set_indegree(inDegree);

		for (long i = 0; i < existList.size(); i++)
		{
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
		insert_wrap(taskId, seriValue);
	}

	incre_ZHT_msg_count(config->numTaskPerClient);

	clock_gettime(0, &end);
	timespec diff = time_diff(start, end);

#ifdef PRINT_OUT
	cout << "I am done, the time taken is:" << diff.tv_sec
			<< " s, and " << diff.tv_nsec << " ns" << endl;
	cout << "--------------------------------"
			"----------------------------" << endl;
#endif

	if (clientLogOS.is_open())
	{
		clientLogOS << "I am done, the time taken is:" << diff.tv_sec
					<< " s, and " << diff.tv_nsec << " ns" << endl;
		clientLogOS << "--------------------------------"
				"----------------------------" << endl;
	}
}

/* initialize all the tasks by assigning
 * taskId for each individual task
 * */
void MatrixClient::init_task()
{
	for (long i = 0; i < config->numTaskPerClient; i++)
	{
		stringstream ss;
		ss << get_index() << i;
		string taskId(ss.str());
		taskVec.at(i) = taskId + " " + taskVec.at(i);
	}
//		vector<string> taskItemStr = tokenize(taskId + " " + taskVec.at(i), " ");
//		TaskMsg tm;
//		tm.set_taskid(taskItemStr.at(0));
//		tm.set_user(taskItemStr.at(1));
//		tm.set_dir(taskItemStr.at(2));
//		tm.set_cmd(taskItemStr.at(3));
//		tm.set_datalength(0);
//		tasks.push_back(tm);
}

/* submit tasks to the schedulers, either with
 * the best case scenario or worst case scenario
 * */
void MatrixClient::split_task()
{
	/* current time to be set as the submission
	 * time of all the tasks. This might be not
	 * accurate with tasks sent batch by batch
	 * */
	long increment = 0;

	/* for (long i = 0; i < config->numTaskPerClient; i++)
	{
		string taskId = tasks.at(i).taskid();
		string taskDetail;
		//zc.lookup(taskId, taskDetail);
		lookup_wrap(taskId, taskDetail);
		Value value = str_to_value(taskDetail);
		value.set_submittime(get_time_usec());

		taskDetail = value_to_str(value);
		insert_wrap(taskId, taskDetail);

		increment += 2;
	}

	incre_ZHT_msg_count(increment); */

#ifdef PRINT_OUT
	cout << "--------------------------------"
			"----------------------------" << endl;
	cout << "Now, I am going to split tasks for the schedulers" << endl;
#endif

	if (clientLogOS.is_open())
	{
		clientLogOS << "--------------------------------"
				"----------------------------" << endl;
		clientLogOS << "Now, I am going to split "
				"tasks for the schedulers" << endl;
	}

	clock_gettime(0, &start);
	/* if the submission mode is best case */
	if (config->submitMode.compare("bestcase") == 0)
	{
		split_task_bc();
	}
	else if (config->submitMode.compare("worstcase") == 0)
	{
		/* otherwise, do the worst case scenario by randomly
		 * selecting a scheduler to submit all the tasks
		 * */
		int toScheIdx = rand() % schedulerVec.size();
		split_task_wc(taskVec, toScheIdx);
	}

	clock_gettime(0, &end);
	timespec diff = time_diff(start, end);

#ifdef PRINT_OUT
	cout << "It took " << diff.tv_sec << "s, and "
			<< diff.tv_nsec << " ns" << endl;
	cout << "--------------------------------"
			"----------------------------" << endl;
#endif

	if (clientLogOS.is_open())
	{
		clientLogOS << "It took " << diff.tv_sec << "s, and "
				<< diff.tv_nsec << " ns" << endl;
		clientLogOS << "--------------------------------"
				"----------------------------" << endl;
	}

	insert_wrap("Split Workload", "done");

	wait_all_task_recv();

	clock_gettime(0, &start);
}

/* submit tasks with the best case scenario, in which,
 * all the tasks are splited evenly to all the
 * schedulers in a interleaved way
 * */
void MatrixClient::split_task_bc()
{
	int toScheIdx = -1, numSche = schedulerVec.size();

	vector< vector<string> > tasksVec;
	for (int i = 0; i < numSche; i++)
	{
		tasksVec.push_back(vector<string>());
	}

	for (int i = 0; i < config->numTaskPerClient; i++)
	{
		toScheIdx = i % numSche;	// task index modular number of scheduler
		tasksVec[toScheIdx].push_back(taskVec.at(i));
	}

	/* as long as all the tasks are distributed evenly,
	 * the submission to individual scheduler is like
	 * the worst case
	 * */
	for (int i = 0; i < numSche; i++)
	{
		split_task_wc(tasksVec[i], i);
	}
}

/* submit tasks with the worst case scenario, in which,
 * all the tasks (listed in "taskVec") are submitted to
 * one scheduler (index is "toScheIdx")
 * */

void MatrixClient::split_task_wc(vector<string> taskStrVec, int toScheIdx)
{
	string path = config->schedulerWorkloadPath +
			"/workload." + num_to_str<int>(toScheIdx);

	ofstream workloadFS;
	workloadFS.open(path.c_str());

	for (int i = 0; i < taskStrVec.size(); i++)
	{
		workloadFS << taskStrVec.at(i) << endl;
	}

	workloadFS.flush(); workloadFS.close();
}

//void MatrixClient::split_task_wc(vector<string> tmVec, int toScheIdx)
//{
//	long numTaskLeft = tmVec.size();
//	long numTaskBeenSent = 0;
//	long numTaskSendPerPkg = config->maxTaskPerPkg;
//
//	int sockfd = -1;
//
//	while (numTaskLeft > 0)
//	{
//		if (numTaskLeft < config->maxTaskPerPkg)
//		{
//			numTaskSendPerPkg = numTaskLeft;
//		}
//
//		numTaskBeenSent = tmVec.size() - numTaskLeft;
//
//		MatrixMsg mm;
//		mm.set_msgtype("client send task");
//		mm.set_count(numTaskSendPerPkg);
//
//		long pos = 0;
//		for (long i = 0; i < numTaskSendPerPkg; i++)
//		{
//			pos = i + numTaskBeenSent;
//			mm.add_tasks(taskmsg_to_str(tmVec.at(pos)));
//		}
//
//		string taskPkgStr = mm.SerializeAsString();
//		//cout<< "The length is:" << taskPkgStr.length() << endl;
//		//cout << "The message to send is:" << taskPkgStr << endl;
//		//timespec before, after;
//		//clock_gettime(0, &before);
//		if (sockfd == -1)
//		{
//			sockfd = send_first(schedulerVec.at(toScheIdx), config->schedulerPortNo, taskPkgStr);
//		}
//		else
//		{
//			send_bf(sockfd, taskPkgStr);
//		}
//		//clock_gettime(0, &after);
//		//timespec diff = time_diff(before, after);
//		//cout << "it takes " << diff.tv_sec << "s, and " << diff.tv_nsec << " ns for one send!" << endl;
//		//int sockfd = send_first(schedulerVec.at(toScheIdx), config->schedulerPortNo, taskPkgStr);
//		string recvBuf;
//		recv_bf(sockfd, recvBuf);
//		//clock_gettime(0, &after);
//		//diff = time_diff(before, after);
//		//cout << "it takes " << diff.tv_sec << "s, and " << diff.tv_nsec << " ns for one send and recv!" << endl;
//		numTaskLeft -= numTaskSendPerPkg;
//	}
//}

/* monitoring thread function, monitoring is conducted only by client 0.
 * It can monitor the execution progress of all the tasks, the system
 * status, and log all the task details
 * */
void *monitoring(void* args)
{
	MatrixClient *mc = (MatrixClient*)args;
	string key("num tasks done");

	long numAllCore = mc->config->numCorePerExecutor * mc->schedulerVec.size();
	long numIdleCore = 0;
	long numTaskWait = 0, numTaskReady = 0;
	long preNumTaskDone = 0, numTaskDone = 0;
	long prevTimeUs = 0L, currentTimeUs = 0L;
	double instantThr = 0.0;

	string numTaskFinStr;

	/* system status log head */
	if (mc->systemLogOS.is_open())
	{
		mc->systemLogOS << "Time(us)\tNumAllCore\tNumIdleCore\tNumTaskWait\t"
				"NumTaskReady\tNumTaskDone\tThroughput" << endl;
	}

	long increment = 0;

	while (1)
	{
		//mc->zc.lookup(key, numTaskFinStr);	// lookup how many tasks are done
		mc->lookup_wrap(key, numTaskFinStr);
		numTaskDone = str_to_num<long>(numTaskFinStr);

		increment++;

		/* log the instant system status */
		if (mc->systemLogOS.is_open())
		{
			currentTimeUs = get_time_usec();

			for (int i = 0; i < mc->schedulerVec.size(); i++)
			{
				string schedulerStat;
				//mc->zc.lookup(mc->schedulerVec.at(i), schedulerStat);
				mc->lookup_wrap(mc->schedulerVec.at(i), schedulerStat);
				if (schedulerStat.empty())
				{
					continue;
				}
				Value value = str_to_value(schedulerStat);

				numIdleCore += value.numcoreavilable();
				numTaskWait += value.numtaskwait();
				numTaskReady += value.numtaskready();
			}

			increment += mc->schedulerVec.size();

			instantThr = (double)(numTaskDone - preNumTaskDone) /
						(currentTimeUs - prevTimeUs) * 1E6;

			mc->systemLogOS << currentTimeUs << "\t" << numAllCore << "\t"
					<< numIdleCore << "\t" << numTaskWait << "\t"
					<< numTaskReady << "\t" << numTaskDone << "\t"
					<< instantThr << endl;

			preNumTaskDone = numTaskDone;
			prevTimeUs = currentTimeUs;
			numIdleCore = 0; numTaskWait = 0; numTaskReady = 0;
		}

		if (numTaskDone == mc->config->numAllTask)	// all the tasks are done
		{
			break;
		}
		else
		{
			usleep(mc->config->monitorInterval);	// sleep sometime
		}
	}

	clock_gettime(0, &mc->end);
	timespec diff = time_diff(mc->start, mc->end);

	double time = (double)diff.tv_sec + (double)diff.tv_nsec / 1E9;
	double throughput = mc->config->numAllTask / time;

#ifdef PRINT_OUT
	cout << "It takes " << diff.tv_sec << "s, and " << diff.tv_nsec <<
			" ns to finish " << mc->config->numAllTask << " tasks" << endl;
	cout << "The overall throughput is:" << throughput << endl;
#endif

	if (mc->clientLogOS.is_open())
	{
		mc->clientLogOS << "It takes " << diff.tv_sec << "s, and " <<
				diff.tv_nsec << " ns to finish " << mc->config->numAllTask
				<< " tasks" << endl;
		mc->clientLogOS << "The overall throughput is:" << throughput << endl;
	}

	if (mc->systemLogOS.is_open())
	{
		mc->systemLogOS.flush(); mc->systemLogOS.close();
	}

	/* now start to long the execution details of each individual task */
	/*if (mc->taskLogOS.is_open())
	{
		cout << "Now, log the task info!" << endl;
		mc->taskLogOS << "TaskId\tNumMove\tHistory\tSubmitTime\tArriveTime\t"
				"ReadyQueuedTime\tExeTime\tFinTime" << endl;

		int numClient = mc->config->numAllTask / mc->config->numTaskPerClient;

		for (int i = 0; i < numClient; i++)
		{
			for (long j = 0; j < mc->config->numTaskPerClient; j++)
			{
				string taskId = num_to_str<int>(i) + num_to_str<long>(j);
				string taskDetail;
				//mc->zc.lookup(taskId, taskDetail);
				mc->lookup_wrap(taskId, taskDetail);
				Value value = str_to_value(taskDetail);

				mc->taskLogOS << taskId << "\t" << value.nummove() << "\t" <<
						value.history() << "\t" << value.submittime() << "\t" <<
						value.arrivetime() << "\t" << value.rqueuedtime() << "\t" <<
						value.exetime() << "\t" << value.fintime() << endl;
			}
		}

		increment += mc->config->numAllTask;

		mc->taskLogOS.flush(); mc->taskLogOS.close();
	}*/

	mc->incre_ZHT_msg_count(increment);

#ifdef PRINT_OUT
	cout << "The number of ZHT message is:" << mc->numZHTMsg << endl;
#endif

	if (mc->clientLogOS.is_open())
	{
		mc->clientLogOS << "The number of ZHT message is:" << mc->numZHTMsg << endl;
		mc->clientLogOS.flush(); mc->clientLogOS.close();
	}

	return NULL;
}

void MatrixClient::do_monitoring()
{
	if (get_index() != 0)
	{
		return;
	}

	pthread_t monThread;

	while (pthread_create(&monThread, NULL, monitoring, this) != 0)
	{
		sleep(1);
	}

	pthread_join(monThread, NULL);
}
