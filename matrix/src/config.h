/*
 * Configure.h
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#ifndef CONFIGURE_H_
#define CONFIGURE_H_

#include <fstream>
#include "util.h"

using namespace std;

class Configuration
{
	public:
		Configuration(const string&);
		virtual ~Configuration();

		void parse_config(const string&);

		long numTaskPerClient;
		long numAllTask;
		int numCorePerExecutor;
		long maxTaskPerPkg;
		long monitorInterval;
		long schedulerPortNo;
		long sleepLength;
		int workStealingOn;
		long wsPollIntervalStart;
		long wsPollIntervalUb;

		string schedulerMemFile;
		string netProtoc;
		string dagType;
		int dagArg;
		string hostIdType;
		string submitMode;
		string workloadFile;
		string clientLog;
		string taskLog;
		string systemLog;
		string schedulerLog;
		string zhtMemFile;
		string zhtConfigFile;
};

#endif /* CONFIGURE_H_ */
