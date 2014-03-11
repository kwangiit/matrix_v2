/*
 * util.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "util.h"

uint _BUF_SIZE = 1000;

vector<string> tokenize(const string &source, const char *delimiter)
{
	vector<string> results;
	size_t prev = 0, next = 0;

	if (source.empty())
	{
		return results;
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
	int i = 0, sockfd;
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

	if (type.compare("localhost") == 0)
	{
		id = "localhost";
	}
	else if (type.compare("ip") == 0)
	{
		char ip_cstr[30];
		memset(ip_cstr, '\0', 30);
		get_ip(ip_cstr);
		id.assign(ip_cstr);
	}
	else if (type.compare("hostname") == 0)
	{
		id = exec("hostname");
	}

	cout << "The id is: " << id << endl;
	return id;
}

vector<string> read_from_file(const string &fileName)
{
	ifstream fileStream(fileName.c_str());
	vector<string> fileVec;
	string line;

	if (!fileStream.good())
	{
		return fileVec;
	}

	while(getline(fileStream, line))
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
		cout << "string is:" << str << " and target is:" << strVec.at(i) << endl;
		if (str.compare(strVec.at(i)) == 0)
		{
			idx = i;
			break;
		}
	}

	return idx;
}

 /* generate adjecency list for BOT independent tasks */
void gen_bot_adjlist(adjList &dagAdjList, long numTask)
{
	for (long i = 0; i < numTask; i++)
	{
		vector<long> newList;
		dagAdjList.insert(make_pair(i, newList));
	}
}

/*
 * generate adjacency list for fanout dags,
 * the argument is the fan out degree
 * */
void gen_fanout_adjlist(adjList &dagAdjList, long dagArg, long numTask)
{
	long next = -1;

	for (long i = 0; i < numTask; i++)
	{
		vector<long> newList;

		for (long j = 1; j <= dagArg; j++)
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

/*
 * generate adjacency list for fan in dags,
 * the argument is the fan in degree
 * */
void gen_fanin_adjlist(adjList &dagAdjList, long dagArg, long numTask)
{
	adjList tmpAdjList;

	/* first generate an adjacency list for fan out dag,
	 * and then flip it over, and switch the left with the
	 * right to get the adjacency list for fan in dags */
	gen_fanout_adjlist(tmpAdjList, dagArg, numTask);

	for (long i = 0; i < numTask; i++)
	{
		long reverseId = numTask - 1 - i;

		vector<long> newList;
		newList.push_back(numTask - 1 - i);

		vector<long> tmpList = tmpAdjList.find(i)->second;

		for (long j = 0; j < tmpList.size(); j++)
		{
			dagAdjList.insert(make_pair(numTask - 1 -
					tmpList.at(tmpList.size() - 1 - j), newList));
		}
	}
}

void gen_pipeline_adjlist(adjList &dagAdjList, long dagArg, long numTask)
{
	long numPipe = numTask / dagArg, index = -1, next = -1;

	for (long i = 0; i < numPipe; i++)
	{
		for (long j = 0; j < dagArg; j++)
		{
			index = i * dagArg + j;
			next = index + 1;
			vector<long> newList;

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
		vector<long> newList;

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
		vector<long> existList = it->second;
        cout << " " << it->first << " -> ";

        for(long i = 0; i < existList.size(); i++)
        {
        	cout << " " << existList[i] << ",";
        }

        cout << endl;
	}
}

void gen_dag_adjlist(adjList &dagAdjList, string &dagType,
							long dagArg, long numTask)
{
	if (dagType.compare("BOT") == 0)
	{
		gen_bot_adjlist(dagAdjList, numTask);
	}
	else if (dagType.compare("FanOut") == 0)
	{
		gen_fanout_adjlist(dagAdjList, dagArg, numTask);
	}
	else if (dagType.compare("FanIn") == 0)
	{
		gen_fanin_adjlist(dagAdjList, dagArg, numTask);
	}
	else if (dagType.compare("Pipeline") == 0)
	{
		gen_pipeline_adjlist(dagAdjList, dagArg, numTask);
	}
	print_adjlist(dagAdjList);
}

void gen_dag_indegree(adjList &dagAdjList, inDegree &dagInDegree)
{
	for (long i = 0; i < dagAdjList.size(); i++)
	{
		dagInDegree[i] = 0;
	}
	for(adjList::iterator it = dagAdjList.begin();
						it != dagAdjList.end(); ++it)
	{
		long index = it->first;
		vector<long> existList = it->second;

		for (long j = 0; j < existList.size(); j++)
		{
			dagInDegree[existList.at(j)]++;
		}
	}
}

void gen_dag_parents(adjList &dagAdjList, adjList &dagParentList)
{
	for (long i = 0; i < dagAdjList.size(); i++)
	{
		vector<long> parents;
		dagParentList.insert(make_pair(i, parents));
	}

	for (adjList::iterator it = dagAdjList.begin();
						it != dagAdjList.end(); ++it)
	{
		long index = it->first;
		vector<long> existList = it->second;

		for (long i = 0; i < existList.size(); i++)
		{
			dagParentList.find(existList.at(i))->second.push_back(index);
		}
	}
}

double get_time_usec()
{
	struct timeval currentTime;

	gettimeofday(&currentTime, NULL);
	return static_cast<double>(currentTime.tv_sec) * 1E6
				+ static_cast<double>(currentTime.tv_usec);
}

double get_time_msec()
{
	struct timeval currentTime;

	gettimeofday(&currentTime, NULL);
	return static_cast<double>(currentTime.tv_sec) * 1E3
			+ static_cast<double>(currentTime.tv_usec) / 1E3;
}

double get_time_sec()
{
	struct timeval currentTime;

	gettimeofday(&currentTime, NULL);
	return static_cast<double>(currentTime.tv_sec)
			+ static_cast<double>(currentTime.tv_usec) / 1E6;
}

timespec time_diff(timespec start, timespec end)
{
	timespec diff;
	uint64_t ts, te;

	ts = (uint64_t)start.tv_sec * 1000000000 + (uint64_t)start.tv_nsec;
	te = (uint64_t)end.tv_sec * 1000000000 + (uint64_t)end.tv_nsec;
	diff.tv_sec = (te - ts) / 1000000000;
	diff.tv_nsec = (te - ts) % 1000000000;

	return diff;
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

Peer::Peer(const string &configFile)
{
	config = new Configuration(configFile);
	set_id(get_host_id(config->hostIdType));
	schedulerVec = read_from_file(config->schedulerMemFile);
	set_index(get_self_idx(get_id(), schedulerVec));
	running = true;
	numZHTMsg = 0;
	init_zht_client(config->zhtConfigFile, config->zhtMemFile);
}

Peer::~Peer()
{

}

bool Peer::init_zht_client(const string &zhtcfgFile, const string &neighFile)
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

void Peer::set_id(const string &id)
{
	this->id = id;
}

string Peer::get_id()
{
	return id;
}

void Peer::set_index(int index)
{
	this->index = index;
}

int Peer::get_index()
{
	return index;
}

void Peer::wait_all_scheduler()
{
	string key("number of scheduler registered");
	string expValue = num_to_str<int>(schedulerVec.size());

	while (zc.state_change_callback(key, expValue, config->sleepLength) != 0)
	{
		usleep(1);
	}
}

void Peer::incre_ZHT_msg_count(long increment)
{
	numZHTMsg += increment;
}
