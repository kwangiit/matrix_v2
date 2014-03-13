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
	return static_cast<double>(currentTime.tv_sec) * 1000000
				+ static_cast<double>(currentTime.tv_usec);
}

double get_time_msec()
{
	struct timeval currentTime;

	gettimeofday(&currentTime, NULL);
	return static_cast<double>(currentTime.tv_sec) * 1000
			+ static_cast<double>(currentTime.tv_usec) / 1000;
}

double get_time_sec()
{
	struct timeval currentTime;

	gettimeofday(&currentTime, NULL);
	return static_cast<double>(currentTime.tv_sec)
			+ static_cast<double>(currentTime.tv_usec) / 1000000;
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

extern string taskmsg_to_str(const TaskMsg &taskMsg)
{
	string str("");

	str.append(taskMsg.taskid());
	str.append(" ");
	str.append(taskMsg.user());
	str.append(" ");
	str.append(taskMsg.dir());
	str.append(" ");
	str.append(taskMsg.cmd());
	str.append(" ");
	str.append(num_to_str<long>(taskMsg.datalength()));

	return str;
}

extern TaskMsg str_to_taskmsg(const string &str)
{
	vector<string> vecStr = tokenize(str, " ");

	TaskMsg tm;
	tm.set_taskid(vecStr.at(0));
	tm.set_user(vecStr.at(1));
	tm.set_dir(vecStr.at(2));
	tm.set_cmd(vecStr.at(3));
	tm.set_datalength(str_to_num<long>(vecStr.at(4)));

	return tm;
}

extern string value_to_str(const Value &value)
{
	string str("");

	str.append(value.id()); str.append("->");

	if (value.has_indegree())
	{
		str.append(num_to_str<long>(value.indegree()));
	}
	else
	{
		str.append("noindegree");
	}
	str.append("->");

	if (value.parents_size() > 0)
	{
		for (int i = 0; i < value.parents_size(); i++)
		{
			str.append(value.parents(i));
			str.append("<eop");
		}
	}
	else
	{
		str.append("noparents");
	}
	str.append("->");

	if (value.children_size() > 0)
	{
		for (int i = 0; i < value.children_size(); i++)
		{
			str.append(value.children(i));
			str.append("<eoc");
		}
	}
	else
	{
		str.append("nochildren");
	}
	str.append("->");

	if (value.datanamelist_size() > 0)
	{
		for (int i = 0; i < value.datanamelist_size(); i++)
		{
			str.append(value.datanamelist(i));
			str.append("<eodn");
		}
	}
	else
	{
		str.append("nodataname");
	}
	str.append("->");

	if (value.datasize_size() > 0)
	{
		for (int i = 0; i < value.datasize_size(); i++)
		{
			str.append(num_to_str<long>(value.datasize(i)));
			str.append("<eods");
		}
	}
	else
	{
		str.append("nodatasize");
	}
	str.append("->");

	if (value.has_alldatasize())
	{
		str.append(num_to_str<long>(value.alldatasize()));
	}
	else
	{
		str.append("noalldatasize");
	}
	str.append("->");

	if (value.has_history())
	{
		str.append(value.history());
	}
	else
	{
		str.append("nohistory");
	}
	str.append("->");

	if (value.has_nummove())
	{
		str.append(num_to_str<int>(value.nummove()));
	}
	else
	{
		str.append("nomove");
	}
	str.append("->");

	if (value.has_submittime())
	{
		str.append(num_to_str<double>(value.submittime()));
	}
	else
	{
		str.append("nost");
	}
	str.append("->");

	if (value.has_arrivetime())
	{
		str.append(num_to_str<double>(value.arrivetime()));
	}
	else
	{
		str.append("noat");
	}
	str.append("->");

	if (value.has_rqueuedtime())
	{
		str.append(num_to_str<double>(value.rqueuedtime()));
	}
	else
	{
		str.append("noqt");
	}
	str.append("->");

	if (value.has_exetime())
	{
		str.append(num_to_str<double>(value.exetime()));
	}
	else
	{
		str.append("noet");
	}
	str.append("->");

	if (value.has_fintime())
	{
		str.append(num_to_str<double>(value.fintime()));
	}
	else
	{
		str.append("noft");
	}
	str.append("->");

	if (value.has_numtaskfin())
	{
		str.append(num_to_str<long>(value.numtaskfin()));
	}
	else
	{
		str.append("nonumtaskfin");
	}
	str.append("->");

	if (value.has_numworksteal())
	{
		str.append(num_to_str<long>(value.numworksteal()));
	}
	else
	{
		str.append("nonumworksteal");
	}
	str.append("->");

	if (value.has_numworkstealfail())
	{
		str.append(num_to_str<long>(value.numworkstealfail()));
	}
	else
	{
		str.append("nonumworkstealfail");
	}
	str.append("->");

	if (value.has_numtaskwait())
	{
		str.append(num_to_str<int>(value.numtaskwait()));
	}
	else
	{
		str.append("nonumtaskwait");
	}
	str.append("->");

	if (value.has_numtaskready())
	{
		str.append(num_to_str<int>(value.numtaskready()));
	}
	else
	{
		str.append("nonumtaskready");
	}
	str.append("->");

	if (value.has_numcoreavilable())
	{
		str.append(num_to_str<int>(value.numcoreavilable()));
	}
	else
	{
		str.append("nonumcoreavail");
	}
	str.append("->");

	if (value.has_numallcore())
	{
		str.append(num_to_str<int>(value.numallcore()));
	}
	else
	{
		str.append("nonumallcore");
	}
	str.append("->");

	return str;
}

extern Value str_to_value(const string &str)
{
	Value value;
	vector<string> vec = tokenize(str, "->");

	for (int i = 0; i < vec.size(); i++)
	{
		cout << vec.at(i) << " ";
	}
	cout << endl;
	value.set_id(vec[0]);

	if (vec[1].compare("noindegree") != 0)
	{
		value.set_indegree(str_to_num<long>(vec[1]));
	}

	if (vec[2].compare("noparents") != 0)
	{
		vector<string> parentVec = tokenize(vec[2], "<eop");
		for (int i = 0; i < parentVec.size(); i++)
		{
			value.add_parents(parentVec.at(i));
		}
	}

	if (vec[3].compare("nochildren") != 0)
	{
		vector<string> childVec = tokenize(vec[3], "<eoc");
		cout << "number of children is:" << childVec.size() << endl;
		for (int i = 0; i < childVec.size(); i++)
		{
			cout << childVec.at(i) << endl;
			value.add_children(childVec.at(i));
		}
	}

	if (vec[4].compare("nodataname") != 0)
	{
		vector<string> dataNameVec = tokenize(vec[4], "<eodn");
		for (int i = 0; i < dataNameVec.size(); i++)
		{
			value.add_datanamelist(dataNameVec.at(i));
		}
	}

	if (vec[5].compare("nodatasize") != 0)
	{
		vector<string> dataSizeVec = tokenize(vec[5], "<eods");
		for (int i = 0; i < dataSizeVec.size(); i++)
		{
			value.add_datasize(str_to_num<long>(dataSizeVec.at(i)));
		}
	}

	if (vec[6].compare("noalldatasize") != 0)
	{
		value.set_alldatasize(str_to_num<long>(vec[6]));
	}

	if (vec[7].compare("nohistory") != 0)
	{
		value.set_history(vec[7]);
	}

	if (vec[8].compare("nomove") != 0)
	{
		value.set_nummove(str_to_num<int>(vec[8]));
	}

	if (vec[9].compare("nost") != 0)
	{
		value.set_submittime(str_to_num<double>(vec[9]));
	}

	if (vec[10].compare("noat") != 0)
	{
		value.set_arrivetime(str_to_num<double>(vec[10]));
	}

	if (vec[11].compare("noqt") != 0)
	{
		value.set_rqueuedtime(str_to_num<double>(vec[11]));
	}

	if (vec[12].compare("noet") != 0)
	{
		value.set_exetime(str_to_num<double>(vec[12]));
	}

	if (vec[13].compare("noft") != 0)
	{
		value.set_fintime(str_to_num<double>(vec[13]));
	}

	if (vec[14].compare("nonumtaskfin") != 0)
	{
		value.set_numtaskfin(str_to_num<long>(vec[14]));
	}

	if (vec[15].compare("nonumworksteal") != 0)
	{
		value.set_numworksteal(str_to_num<long>(vec[15]));
	}

	if (vec[16].compare("nonumworkstealfail") != 0)
	{
		value.set_numworkstealfail(str_to_num<long>(vec[16]));
	}

	if (vec[17].compare("nonumtaskwait") != 0)
	{
		value.set_numtaskwait(str_to_num<int>(vec[17]));
	}

	if (vec[18].compare("nonumtaskready") != 0)
	{
		value.set_numtaskready(str_to_num<int>(vec[18]));
	}

	if (vec[19].compare("nonumcoreavail") != 0)
	{
		value.set_numcoreavilable(str_to_num<int>(vec[19]));
	}

	if (vec[20].compare("nonumallcore") != 0)
	{
		value.set_numallcore(str_to_num<int>(vec[20]));
	}

	return value;
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
