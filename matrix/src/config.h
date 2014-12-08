/*
 * Configure.h
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#ifndef CONFIGURE_H_
#define CONFIGURE_H_

#include <fstream>

using namespace std;

class Configuration {
public:
	Configuration(const string&);
	virtual ~Configuration();

	void parse_config(const string&);

	long numTaskPerClient;	// number of task per client
	//int numFile;
	//double locality;
	long numAllTask;	// number of all tasks
	long numMapTask;
	long numReduceTask;
	int numCorePerExecutor;	// number of cores per executor
	long maxTaskPerPkg;	// maximum number of tasks per package
	long monitorInterval;	//monitor interval in microsecond
	long schedulerPortNo;	// scheduler port number
	long sleepLength;	// time duration in microsecond
	int workStealingOn;	// indicate whether to do work staling (1) or not (0)
	long wsPollIntervalStart;// the initial value of poll interval in microsecond
	long wsPollIntervalUb;	// the upper bound of poll inerval in microsecond

	string policy;
	long dataSizeThreshold;
	long estTimeThreadshold;

	string schedulerMemFile;	// the memberlist file of all the schedulers
	string netProtoc;	// network protocol type: TCP, UDP, UDT, etc
	string dagType;	// the type of workload DAGs: BOT, Fan-In, Fan-Out, Pipeline
	long dagArg;	// the argument to the workload DAG
	string hostIdType;// the host identity type: localhost, hostname, ip address
	string submitMode;// the mode that clients submit tasks: best case, worst case
	string workloadFile;	// the workload file
	string schedulerWorkloadPath;// the workload root directory for all the schedulers
	int clientLog;	// indicate whether to logging (1) for client or not (0)
	int taskLog;// indicate whether to logging (1) for each individual task or not (0)
	int systemLog;// indicate whether to logging (1) for the system status or not (0)
	int schedulerLog;// indicate whether to logging (1) for scheduler or not (0)
	string zhtMemFile;	// the ZHT memeberlist file
	string zhtConfigFile;	// the ZHT configuration file
};

#endif /* CONFIGURE_H_ */
