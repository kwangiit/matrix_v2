/*
 * scheduler.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: kwang
 */

#include "scheduler_stub.h"

using namespace std;

ZHTClient zc;

int main(int argc, char *argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "The usage is: client\t"
						 "configuration_file!\n");
		exit(-1);
	}

	string configFileStr(argv[1]);

	MatrixScheduler ms = new MatrixScheduler(configFileStr);

	ms.regist(zc);

	ms.waitAllScheduler(zc);

	ms.forkCRTThread(zc);

	ms.forkETThread();

	ms.forkCCTThread(zc);

	ms.forkWSThread();

	ms.forkRecordStatThread(zc);
}
