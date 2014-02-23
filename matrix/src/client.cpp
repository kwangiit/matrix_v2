/*
 * client.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "client_stub.h"

using namespace std;

ZHTClient zc;


int main(int argc, char* argv[])
{
	/* check whether specified a configuration file */
	if (argc != 2)
	{
		fprintf(stderr, "The usage is: client\t"
						 "configuration_file!\n");
		exit(-1);
	}

	srand(time(NULL));

	/* create a new matrix client */
	string configFileStr(argv[1]);
	MatrixClient mc = new MatrixClient(configFileStr);

	/* generate task dag adjecent list (children) */
	adjList dagAdjList;
	gen_dag_adjlist(dagAdjList, mc.config->dagType, mc.config->dagArg,
							mc.config->numTaskPerClient);

	/* calculate indegrees (number of parents) for every tasks */
	inDegree dagInDegree;
	gen_dag_indegree(dagAdjList, dagInDegree);

	/* initialize zc as a ZHT client */
	init_zht_client(zc, mc.config->zhtConfigFile, mc.config->zhtMemFile);

	/* insert the task information to ZHT */
	mc.insert_taskinfo_to_zht(zc, dagAdjList, dagInDegree);

	/* wait until all schedulers have registered to ZHT */
	cout << "--------------------------------"
					"----------------------------" << endl;
	cout << "Now, I am waiting until all the schedulers are running!" << endl;
	if (mc.clientLogOS.is_open())
	{
		mc.clientLogOS << "--------------------------------"
				"----------------------------" << endl;
		mc.clientLogOS << "Now, I am waiting until all the "
				"schedulers are running!" << endl;
	}

	clock_gettime(0, &mc.start);

	mc.wait_all_scheduler(zc);

	clock_gettime(0, &mc.end);
	timespec diff = time_diff(mc.start, mc. end);

	cout << "It took " << diff.tv_sec << "s, and "
			<< diff.tv_nsec << " ns" << endl;
	cout << "--------------------------------"
						"----------------------------" << endl;
	if (mc.clientLogOS.is_open())
	{
		mc.clientLogOS << "It took " << diff.tv_sec << "s, and "
				<< diff.tv_nsec << " ns" << endl;
		mc.clientLogOS << "--------------------------------"
						"----------------------------" << endl;
	}

	/* initalize tasks by assigning taskId information to each task */
	mc.init_task();

	/* submit tasks to the schedulers */
	mc.submit_task();

	/* do the monitoring to watch th executing progress */
	mc.do_monitoring(zc);
}
