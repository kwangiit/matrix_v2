/*
 * client_stub.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "client_stub.h"
#include "ZHT/src/meta.pb.h"

MatrixClient::MatrixClient(const string
		&configFile):Peer(configFile)
{
	clock_gettime(0, &start);

	taskVec = read_from_file(config->workloadFile);

	string base;
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
	if (config->taskLog == 1 && get_index() == 0)
	{
		string taskLogFile("./task_" + suffix);
		taskLogOS.open(taskLogFile.c_str());
	}
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

		value.set_nummove(0);
		value.set_history("");
		value.set_submittime(0.0);
		value.set_arrivetime(0.0);
		value.set_rqueuedtime(0.0);
		value.set_exetime(0.0);
		value.set_fintime(0.0);

		string seriValue = value.SerializeAsString();

		zc.insert(taskId, seriValue);
	}

	incre_ZHT_msg_count(config->numTaskPerClient);

	clock_gettime(0, &end);
	timespec diff = time_diff(start, end);

#ifdef PRINT_OUT
	cout << "I am done, the time taken is:" << diff.tv_sec
			<< " s, and " << diff.tv_nsec + " ns" << endl;
	cout << "--------------------------------"
			"----------------------------" << endl;
#endif

	if (clientLogOS.is_open())
	{
		clientLogOS << "I am done, the time taken is:" << diff.tv_sec
					<< " s, and " << diff.tv_nsec + " ns" << endl;
		clientLogOS << "--------------------------------"
				"----------------------------" << endl;
	}
}

/* initialize all the tasks by assigning
 * taskId for each individual task
 * */
void MatrixClient::init_task()
{
	long numTask = config->numTaskPerClient;

	for (long i = 0; i < numTask; i++)
	{
		stringstream ss;
		ss << index << i;
		string taskId(ss.str());

		taskVec.at(i) = taskId + " " + taskVec.at(i);
	}
}

/* submit tasks to the schedulers, either with
 * the best case scenario or worst case scenario
 * */
void MatrixClient::submit_task()
{
#ifdef PRINT_OUT
	cout << "--------------------------------"
			"----------------------------" << endl;
	cout << "Now, I am going to submit tasks to the schedulers" << endl;
#endif

	if (clientLogOS.is_open())
	{
		clientLogOS << "--------------------------------"
				"----------------------------" << endl;
		clientLogOS << "Now, I am going to submit "
				"tasks to the schedulers" << endl;
	}

	clock_gettime(0, &start);

	/* current time to be set as the submission
	 * time of all the tasks. This might be not
	 * accurate with tasks sent batch by batch
	 * */
	double time_us = get_time_usec();
	long increment = 0;

	for (long i = 0; i < config->numTaskPerClient; i++)
	{
		vector<string> taskSpec = tokenize(taskVec.at(i), " ");
		string taskDetail;
		zc.lookup(taskSpec.at(0), taskDetail);

		Value value;
		value.ParseFromString(taskDetail);
		value.set_submittime(time_us);

		taskDetail = value.SerializeAsString();
		zc.insert(taskSpec.at(0), taskDetail);

		increment += 2;
	}

	incre_ZHT_msg_count(increment);

	/* if the submission mode is best case */
	if (config->submitMode.compare("best case") == 0)
	{
		submit_task_bc();
	}
	else if (config->submitMode.compare("worst case") == 0)
	{
		/* otherwise, do the worst case scenario by randomly
		 * selecting a scheduler to submit all the tasks
		 * */
		int toScheIdx = rand() % schedulerVec.size();
		submit_task_wc(taskVec, toScheIdx);
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
}

/* submit tasks with the best case scenario, in which,
 * all the tasks are splited evenly to all the
 * schedulers in a interleaved way
 * */
void MatrixClient::submit_task_bc()
{
	int toScheIdx = -1, numSche = schedulerVec.size();
	vector<vector<string>> taskVecs(numSche);

	for (int i = 0; i < config->numTaskPerClient; i++)
	{
		toScheIdx = i % numSche;	// task index modular number of scheduler
		taskVecs.at(toScheIdx).push_back(taskVec.at(i));
	}

	/* as long as all the tasks are distributed evenly,
	 * the submission to individual scheduler is like
	 * the worst case
	 * */
	for (int i = 0; i < numSche; i++)
	{
		submit_task_wc(taskVecs.at(i), i);
	}
}

/* submit tasks with the worst case scenario, in which,
 * all the tasks (listed in "taskVec") are submitted to
 * one scheduler (index is "toScheIdx")
 * */
void MatrixClient::submit_task_wc(const vector<string> &taskVec, int toScheIdx)
{
	long numTaskLeft = taskVec.size();
	long numTaskBeenSent = 0;
	long numTaskSendPerPkg = config->maxTaskPerPkg;

	while (numTaskLeft > 0)
	{
		if (numTaskLeft <= config->maxTaskPerPkg)
		{
			numTaskSendPerPkg = numTaskLeft;
		}

		string tasks;

		numTaskBeenSent = config->numTaskPerClient - numTaskLeft;

		for (long i = 0; i < numTaskSendPerPkg; i++)
		{
			tasks += taskVec.at(i + numTaskBeenSent);
			tasks += "eot";
		}

		Package taskPkg;
		taskPkg.set_virtualpath("send task");
		taskPkg.set_realfullpath(tasks);

		string taskPkgStr = taskPkg.SerializeAsString();
		// send the taskPkgStr to the server scheduler_vector.at(toScheIdx)
		// and receive acks

		numTaskLeft -= numTaskSendPerPkg;
	}
}

/* monitoring thread function, monitoring is conducted only by client 0.
 * It can monitor the execution progress of all the tasks, the system
 * status, and log all the task details
 * */
void* MatrixClient::monitoring(void*)
{
	string key("num tasks done");

	long numAllCore = config->numCorePerExecutor * schedulerVec.size();
	long numIdleCore = 0;
	long numTaskWait = 0, numTaskReady = 0;
	long preNumTaskDone = 0, numTaskDone = 0;
	double prevTimeUs = 0.0, currentTimeUs = 0.0, instantThr = 0.0;

	string numTaskFinStr;

	/* system status log head */
	if (systemLogOS.is_open())
	{
		systemLogOS << "Time(us)\tNumAllCore\tNumIdleCore\tNumTaskWait\t"
				"NumTaskReady\tNumTaskDone\tThroughput" << endl;
	}

	long increment = 0;

	while (1)
	{
		zc.lookup(key, numTaskFinStr);	// lookup how many tasks are done
		numTaskDone = str_to_num<long>(numTaskFinStr);

		increment++;

		/* log the instant system status */
		if (systemLogOS.is_open())
		{
			currentTimeUs = get_time_usec();

			for (int i = 0; i < schedulerVec.size(); i++)
			{
				string schedulerStat;
				zc.lookup(schedulerVec.at(i), schedulerStat);

				Value value;
				value.ParseFromString(schedulerStat);

				numIdleCore += value.numcoreavilable();
				numTaskWait += value.numtaskwait();
				numTaskReady += value.numtaskready();
			}

			increment += schedulerVec.size();

			instantThr = (double)(numTaskDone - preNumTaskDone) /
						(currentTimeUs - prevTimeUs) * 1E6;

			systemLogOS << currentTimeUs << "\t" << numAllCore << "\t"
					<< numIdleCore << "\t" << numTaskWait << "\t"
					<< numTaskReady << "\t" << numTaskDone << "\t"
					<< instantThr << endl;

			preNumTaskDone = numTaskDone;
			prevTimeUs = currentTimeUs;
			numIdleCore = 0; numTaskWait = 0; numTaskReady = 0;
		}

		if (numTaskDone == config->numAllTask)	// all the tasks are done
		{
			break;
		}
		else
		{
			usleep(config->monitorInterval);	// sleep sometime
		}
	}

	clock_gettime(0, &end);
	timespec diff = time_diff(start, end);

	double time = (double)diff.tv_sec + (double)diff.tv_nsec / 1E9;
	double throughput = config->numAllTask / time;

#ifdef PRINT_OUT
	cout << "It takes " << diff.tv_sec << "s, and " << diff.tv_nsec <<
			" ns to finish " << config->numAllTask << " tasks" << endl;
	cout << "The overall throughput is:" << throughput << endl;
#endif

	if (clientLogOS.is_open())
	{
		clientLogOS << "It takes " << diff.tv_sec << "s, and " <<
				diff.tv_nsec << " ns to finish " << config->numAllTask
				<< " tasks" << endl;
		clientLogOS << "The overall throughput is:" << throughput << endl;
	}

	if (systemLogOS.is_open())
	{
		systemLogOS.flush(); systemLogOS.close();
	}

	/* now start to long the execution details of each individual task */
	if (taskLogOS.is_open())
	{
		taskLogOS << "TaskId\tNumMove\tHistory\tSubmitTime\tArriveTime\t"
				"ReadyQueuedTime\tExeTime\tFinTime" << endl;

		for (int i = 0; i < schedulerVec.size(); i++)
		{
			for (long j = 0; j < config->numTaskPerClient; j++)
			{
				string taskId = num_to_str<int>(i) + num_to_str<long>(j);
				string taskDetail;
				zc.lookup(taskId, taskDetail);

				Value value;
				value.ParseFromString(taskDetail);

				taskLogOS << taskId << "\t" << value.nummove() << "\t" <<
						value.history() << "\t" << value.submittime() << "\t" <<
						value.arrivetime() << "\t" << value.rqueuedtime() << "\t" <<
						value.exetime() << "\t" << value.fintime() << endl;
			}
		}

		increment += config->numAllTask;

		taskLogOS.flush(); taskLogOS.close();
	}

	incre_ZHT_msg_count(increment);

#ifdef PRINT_OUT
	cout << "The number of ZHT message is:" << numZHTMsg << endl;
#endif

	if (clientLogOS.is_open())
	{
		clientLogOS << "The number of ZHT message is:" << numZHTMsg << endl;
		clientLogOS.flush(); clientLogOS.close();
	}

	return NULL;
}

void MatrixClient::do_monitoring()
{
	if (index != 0)
	{
		return;
	}

	pthread_t monThread;

	while (pthread_create(&monThread, NULL, monitoring, NULL) != 0)
	{
		sleep(1);
	}

	pthread_join(monThread, NULL);
}
