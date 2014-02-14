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

	MatrixClient mc = new MatrixClient(argv[1]); // create a new matrix client
	/* generate task dag adjecent list (children) */
	adj_list dag_adj_list;
	genDagAdjlist(dag_adj_list, mc.config->dag_type, mc.config->dag_argu,
							mc.config->num_task_per_client);

	/* calculate indegrees (number of parents) for every tasks */
	in_degree dag_in_degree;
	genDagInDegree(dag_adj_list, dag_in_degree);

	/* initialize zc as a ZHT client */
	initZHTClient(zc, mc.config->zht_config_file, mc.config->zht_memList_file);

	/* insert the task information to ZHT */
	mc.insertTaskInfoToZHT(zc, dag_adj_list, dag_in_degree);

	/* wait until all schedulers have registered to ZHT */
	mc.waitAllScheduler(zc);


}

