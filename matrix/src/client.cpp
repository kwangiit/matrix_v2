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
	mc.wait_all_scheduler(zc);

	/* initalize tasks by assigning taskId information to each task */
	mc.init_task();

	/* submit tasks to the schedulers */
	mc.submit_task();

	/* do the monitoring to watch th executing progress */
	mc.do_monitoring(zc);
}
