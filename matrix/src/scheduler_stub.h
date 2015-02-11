/*
 * scheduler_stub.h
 *
 *  Created on: Feb 14, 2014
 *      Author: kwang
 */

#ifndef SCHEDULER_STUB_H_
#define SCHEDULER_STUB_H_

#include "matrix_tcp_proxy_stub.h"
#include <queue>

class CmpQueueItem
{
	public:
		CmpQueueItem(const string &taskId, const string &key, long dataSize);
		CmpQueueItem();
		~CmpQueueItem();

		string taskId;
		string key;
		long dataSize;
};

class MatrixScheduler: public Peer
{
	public:
		MatrixScheduler(const string&);
		virtual ~MatrixScheduler();

		void regist();	// regist to ZHT server

		void load_data();

		void get_task_from_file();
		/* receive tasks from another scheduler as a
		 * consequence of successful work stealing
		 * */
		bool recv_task_from_scheduler(int);

		void recv_pushing_task(MatrixMsg&, int);

		/* receive tasks submitted by client */
		void recv_task_from_client(string&, int);

		/* pack and send tasks to another thief scheduler */
		void pack_send_task(int, int, sockaddr, bool, deque<TaskMsg>&);

		/* send tasks to another thief scheduler */
		void send_task(int);

		/* processing requests received by the epoll server */
		int proc_req(int, char*);

		void fork_es_thread();	// fork epoll server thread

		void reset_choosebm();	// reset the bitmap of neighbors chosen

		void choose_neigh();	// choose candidate neighbors to steal tasks

		/* find the neighbor with the maximum load */
		void find_most_loaded_neigh();

		/* try to steal tasks from the most-loaded neighbor */
		bool steal_task();

		//void* workstealing(void*);	// work stealing thread function

		void fork_ws_thread(void);	// fork work stealing thread

		int task_ready_process(const Value&, TaskMsg&);
		/* check if a given task is ready to run, and put it in the right queue */
		bool check_a_ready_task(TaskMsg&);

		void fork_crt_thread();	// fork check ready task thread

		void exec_a_task(TaskMsg&);	// execute a task

		void fork_exec_task_thread();	// fork execute task threads

		/* decrease the number of waiting parents for a given task */
		long notify_children(const CmpQueueItem&);

		/* fork check compute task thread */
		void fork_cct_thread();

		/* fork recording status thread */
		void fork_record_stat_thread();

		void fork_record_task_thread();

		void fork_localQueue_monitor_thread();

		Mutex ZHTMsgCountMutex;	// Mutex of ZHT message count
		Mutex numIdleCoreMutex;	// Mutex of number of idle cores
		Mutex numTaskFinMutex;	// Mutex of number of tasks done

		int numIdleCore;	// number of idle cores
		long prevNumTaskFin;	// number of tasks done last time
		long numTaskFin;	// number of tasks done up to now
		long numTaskSteal;	// number of tasks stolen from other schedulers
		long numTaskStolen;	// number of tasks being stolen by other schedulers
		long numWS;	// number of work stealing operations
		long numWSFail;	// number of failed work stealing operations

		bool *chooseBitMap;	// bitmap of neighbors chosen
		int numNeigh;	// number of neighbors
		int *neighIdx;	// the indeces of all chosen neighbors
		int maxLoadedIdx;	// the neighbor index with the maximum load
		long maxLoad;	// the maximum load of all the neighbors
		long pollInterval;	// the work stealing polling interval
		bool startWS;

		Mutex wqMutex;	// Mutex of waiting queue
		Mutex rqMutex;	// Mutex of ready queue
		Mutex cqMutex;	// Mutex of complete queue
		Mutex lqMutex;
		Mutex wsqMutex;
		Mutex ldMutex;
		Mutex tteMutex;

		priority_queue<TaskMsg, vector<TaskMsg>, HighPriorityByDataSize> localQueue;

		priority_queue<TaskMsg, vector<TaskMsg>, HighPriorityByDataSize> wsQueue;

		deque<TaskMsg> waitQueue;	// waiting queue
		//deque<string> readyQueue;	// ready queue
		deque<CmpQueueItem> completeQueue;	// complete queue

		map<string, string> localData;
		bool cache;

		ofstream schedulerLogOS;	// scheduler log output stream

		ofstream taskLogOS;
		vector<string> taskTimeEntry;

		timespec start, end;
};

#endif /* SCHEDULER_STUB_H_ */
