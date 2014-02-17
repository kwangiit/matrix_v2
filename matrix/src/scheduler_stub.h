/*
 * scheduler_stub.h
 *
 *  Created on: Feb 14, 2014
 *      Author: kwang
 */

#ifndef SCHEDULER_STUB_H_
#define SCHEDULER_STUB_H_

#include "util.h"

class MatrixScheduler
{
	public:
		MatrixScheduler(const string&);
		virtual ~MatrixScheduler();

		void regist(ZHTClient&);
		void waitAllScheduler(ZHTClient&);


		void resetChooseBM();
		void chooseNeigh();
		void findMostLoadedNeigh();
		bool stealTask();

		void* workstealing(void*);
		void forkWSThread(void);

		bool checkAReadyTask(const string&, ZHTClient*);
		void* checkingReadyTask(void*);
		void forkCRTThread(ZHTClient &zc);

		void execOneTask(string&);
		void* executingTask(void*);
		void forkETThread();

		void decreaseIndegree(const string&, ZHTClient*);
		void* checkingCompleteTask(void*);
		void forkCCTThread(ZHTClient &zc);

		void* recordingStat(void*);
		void forkRecordStatThread(ZHTClient &zc);

		void setHostName(string);
		string getHostName();

		void setIndex(int);
		int getIndex();

		Configuration *config;
		vector<string> scheduler_vector;

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

	private:
		string hostname;
		int index;
};


#endif /* SCHEDULER_STUB_H_ */
