/*
 * scheduler_stub.h
 *
 *  Created on: Feb 14, 2014
 *      Author: kwang
 */

#ifndef SCHEDULER_STUB_H_
#define SCHEDULER_STUB_H_

#include "util.h"

class MatrixScheduler:public Peer
{
	public:
		MatrixScheduler(const string&);
		virtual ~MatrixScheduler();

		void regist(ZHTClient&);

		int proc_req(int, void*, sockaddr);
		void* epoll_serving(void*);
		void fork_es_thread();

		void reset_choosebm();
		void choose_neigh();
		void find_most_loaded_neigh();
		bool steal_task();

		void* workstealing(void*);
		void fork_ws_thread(void);

		bool check_a_ready_task(const string&, ZHTClient*);
		void* checking_ready_task(void*);
		void fork_crt_thread(ZHTClient &zc);

		void exec_a_task(string&);
		void* executing_task(void*);
		void fork_exec_task_thread();

		void decrease_indegree(const string&, ZHTClient*);
		void* checking_complete_task(void*);
		void fork_cct_thread(ZHTClient &zc);

		void* recording_stat(void*);
		void fork_record_stat_thread(ZHTClient &zc);

		Mutex numIdleCoreMutex;
		Mutex numTaskFinMutex;

		int numIdleCore;
		long prevNumTaskFin;
		long numTaskFin;
		long numTaskSteal;
		long numTaskStolen;
		long numWS;
		long numWSFail;

		bool *chooseBitMap;
		int numNeigh;
		int *neighIdx;
		int maxLoadedIdx;
		int maxLoad;
		long pollInterval;

		Mutex wqMutex;
		Mutex rqMutex;
		Mutex cqMutex;

		deque<string> waitQueue;
		deque<string> readyQueue;
		deque<string> completeQueue;
};


#endif /* SCHEDULER_STUB_H_ */
