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
	if (argc != 2)
	{
		fprintf(stderr, "The usage is: client\t"
						 "configuration file!\n");
		exit(-1);
	}

	MatrixClient mc = new MatrixClient(argv[1]);
	initZHTClient(zc, mc.config->zht_config_file, mc.config->zht_memList_file);
	adj_list dag_adj_list;
	genDagAdjlist(dag_adj_list, mc.config->dag_type, mc.config->dag_argu,
							mc.config->num_task_per_client);
	in_degree dag_in_degree;
	genDagInDegree(dag_adj_list, dag_in_degree);
	mc.insertTaskInfoToZHT(zc, dag_adj_list, dag_in_degree);
}

