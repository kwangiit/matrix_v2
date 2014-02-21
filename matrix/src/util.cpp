/*
 * util.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "util.h"

uint _BUF_SIZE = 512 + 38;

vector<string> tokenize(const string &source, const char *delimiter)
{
	vector<string> results;
	size_t prev = 0, next = 0;

	if (source.empty())
	{
		return NULL;
	}

	while ((next = source.find_first_of(delimiter, prev)) != string::npos)
	{
		if (next - prev != 0)
		{
			results.push_back(source.substr(prev, next - prev));
		}
		prev = next + 1;
	}

	if (prev < source.size())
	{
		results.push_back(source.substr(prev));
	}

	return results;
}

int get_ip(char *outIP)
{
	int i = 0;
	int sockfd;
	struct ifconf ifconf_local;
	char buf[512];
	struct ifreq *ifreq_local;
	char* ip;
	ifconf_local.ifc_len = 512;
	ifconf_local.ifc_buf = buf;
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		return -1;
	}
	ioctl(sockfd, SIOCGIFCONF, &ifconf_local);
	close(sockfd);
	ifreq_local = (struct ifreq*)buf;
	for(i=(ifconf_local.ifc_len / sizeof(struct ifreq));i > 0;i--)
	{
		ip = inet_ntoa(((struct sockaddr_in*)&(ifreq_local->ifr_addr))->sin_addr);
		if(strcmp(ip, "127.0.0.1") == 0)
		{
			ifreq_local++;
			continue;
		}
		strcpy(outIP, ip);
		return 0;
	}
	return -1;
}

string exec(const char *cmd)
{
	FILE* pipe = popen(cmd, "r");
	if (!pipe)
	{
		return "ERROR";
	}
	char buffer[128];
	string result = "";
	while (!feof(pipe))
	{
		if(fgets(buffer, 128, pipe) != NULL)
		{
	    	result += buffer;
	    }
	}
	pclose(pipe);
	return result;
}

string get_host_id(const string &type)
{
	string id;

	if (!type.compare("localhost"))
	{
		id = "localhost";
	}
	else if (!type.compare("ip"))
	{
		char ip_cstr[30];
		memset(ip_cstr, '\0', 30);
		get_ip(ip_cstr);
		id.assign(ip_cstr);
	}
	else
	{
		id = exec("hostname");
	}

	return id;
}

vector<string> read_from_file(const string &fileName)
{
	ifstream fileStream(fileName.c_str());
	vector<string> fileVec;
	string line;

	if (!fileStream.good())
	{
		return NULL;
	}

	while(fileStream >> line)
	{
		fileVec.push_back(line);
	}

	return fileVec;
}

int get_self_idx(const string &str, vector<string> strVec)
{
	int idx = -1;

	for (int i = 0; i < strVec.size(); i++)
	{
		if (!str.compare(strVec.at(i)))
		{
			idx = i;
			break;
		}
	}

	return idx;
}

void gen_bot_adjlist(adjList &dagAdjList, long numTask)
{
	for (int i = 0; i < numTask; i++)
	{
		vector<int> newList;
		dagAdjList.insert(make_pair(i, newList));
	}
}

void gen_fanout_adjlist(adjList &dagAdjList, int dagArg, long numTask)
{
	int next = -1;

	for (int i = 0; i < numTask; i++)
	{
		vector<int> newList;

		for (int j = 1; j <= dagArg; j++)
		{
			next = i * dagArg + j;

			if (next >= numTask)
			{
				break;
			}
			else
			{
				newList.push_back(next);
			}
		}

		dagAdjList.insert(make_pair(i, newList));
	}
}

void gen_fanin_adjlist(adjList &dagAdjList, int dagArg, long numTask)
{
	adjList tmpAdjList;
	gen_fanout_adjlist(tmpAdjList, dagArg, numTask);

	for (int i = 0; i < numTask; i++)
	{
		int reverseId = numTask - 1 - i;

		vector<int> newList;
		newList.push_back(numTask - 1 - i);

		vector<int> tmpList = tmpAdjList.find(i)->second;

		for (int j = 0; j < tmpList.size(); j++)
		{
			dagAdjList.insert(make_pair(numTask - 1 -
					tmpList.at(tmpList.size() - 1 - j), newList));
		}
	}
}

void gen_pipeline_adjlist(adjList &dagAdjList, int dagArg, long numTask)
{
	int numPipe = numTask / dagArg, index = -1, next = -1;

	for (int i = 0; i < numPipe; i++)
	{
		for (int j = 0; j < dagArg; j++)
		{
			index = i * dagArg + j;
			next = index + 1;
			vector<int> newList;

			if (next % dagArg != 0 && next < numTask)
			{
				newList.push_back(next);
			}

			dagAdjList.insert(make_pair(index, newList));
		}
	}

	for (index = numPipe * dagArg; index < numTask; index++)
	{
		next = index + 1;
		vector<int> newList;

		if (next % dagArg != 0 && next < numTask)
		{
			newList.push_back(next);
		}

		dagAdjList.insert(make_pair(index, newList));
	}
}

void print_adjlist(adjList &dagAdjList)
{
	for(adjList::iterator it = dagAdjList.begin();
			it != dagAdjList.end(); ++it)
	{
		vector<int> existList = it->second;
        cout << " " << it->first << " -> ";

        for(int i = 0; i < existList.size(); i++)
        {
        	cout << " " << existList[i] << ",";
        }

        cout << endl;
	}
}

void gen_dag_adjlist(adjList &dagAdjList, string &dagType,
							int dagArg, long numTask)
{
	if (!dagType.compare("BOT"))
	{
		gen_bot_adjlist(dagAdjList, numTask);
	}
	else if (!dagType.compare("FanOut"))
	{
		gen_fanout_adjlist(dagAdjList, dagArg, numTask);
	}
	else if (!dagType.compare("FanIn"))
	{
		gen_fanin_adjlist(dagAdjList, dagArg, numTask);
	}
	else if (!dagType.compare("Pipeline"))
	{
		gen_pipeline_adjlist(dagAdjList, dagArg, numTask);
	}
}

void gen_dag_indegree(adjList &dagAdjList, inDegree &dagInDegree)
{
	for (int i = 0; i < dagAdjList.size(); i++)
	{
		dagInDegree[i] = 0;
	}
	for(adjList::iterator it = dagAdjList.begin();
						it != dagAdjList.end(); ++it)
	{
		int index = it->first;
		vector<int> existList = it->second;

		for (int j = 0; j < existList.size(); j++)
		{
			dagInDegree[existList.at(j)]++;
		}
	}
}

bool init_zht_client(ZHTClient &zc, const string &zhtcfgFile, const string &neighFile)
{
	if (zhtcfgFile.empty() || neighFile.empty())
	{
		return false;
	}
	else
	{
		if (zc.init(zhtcfgFile, neighFile) != 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

Mutex::Mutex()
{
	int ret = pthread_mutex_init (&mutex, NULL);
}

Mutex::~Mutex()
{

}

int Mutex::lock()
{
	return (pthread_mutex_lock (&mutex));
}

int Mutex::unlock()
{
	return (pthread_mutex_unlock (&mutex));
}
