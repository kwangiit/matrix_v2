/*
 * client.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>

#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <pthread.h>
#include <error.h>

#include "config.h"

using namespace std;

int main(int argc, char* argv[])
{
	Configuration* config;

	if (argc != 2)
	{
		fprintf(stderr, "The usage is: client\t"
						 "configuration file!\n");
		exit(-1);
	}

	config = new Configuration(argv[1]);
}

