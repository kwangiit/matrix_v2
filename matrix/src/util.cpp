/*
 * util.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "util.h"

int getInt(string &str)
{
	if (str.empty())
	{
		return -1;
	}
	else
	{
		return atoi(str.c_str());
	}
}

long getLong(string &str)
{
	if (str.empty())
	{
		return -1;
	}
	else
	{
		return atol(str.c_str());
	}
}

int getIP(char *out_ip)
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
		strcpy(out_ip, ip);
		return 0;
	}
	return -1;
}

string exec(char* cmd)
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

string getHostIdentity(string &type)
{
	string hostname;

	if (!type.compare("localhost"))
	{
		hostname = "localhost";
	}
	else if (!type.compare("ip"))
	{
		char ip_cstr[30];
		memset(ip_cstr, '\0', 30);
		getIP(ip_cstr);
		hostname.assign(ip_cstr);
	}
	else
	{
		hostname = exec("hostname");
	}

	return hostname;
}

vector<string> readFromFile(string &file_name)
{
	ifstream file_stream(file_name.c_str());
	vector<string> file_vector;
	string line;

	if (!file_stream.good())
	{
		return NULL;
	}

	while(file_stream >> line)
	{
		file_vector.push_back(line);
	}

	return file_vector;
}

int getSelfIndex(const string &str, vector<string> str_vec)
{
	int index = -1;
	for (int i = 0; i < str_vec.size(); i++)
	{
		if (!str.compare(str_vec.at(i)))
		{
			index = i;
			break;
		}
	}
	return index;
}

void genBOTDagAdjlist(adj_list &dag_adj_list, long num_task)
{
	for (int i = 0; i < num_task; i++)
	{
		vector<int> new_list;
		dag_adj_list.insert(make_pair(i, new_list));
	}
}

void genFanOutDagAdjlist(adj_list &dag_adj_list, int dag_aug, long num_task)
{
	int next = -1;
	for (int i = 0; i < num_task; i++)
	{
		vector<int> new_list;
		for (int j = 1; j <= dag_aug; j++)
		{
			next = i * dag_aug + j;
			if (next >= num_task)
			{
				break;
			}
			else
			{
				new_list.push_back(next);
			}
		}
		dag_adj_list.insert(make_pair(i, new_list));
	}
}

void genFanInDagAdjlist(adj_list &dag_adj_list, int dag_aug, long num_task)
{
	adj_list tmp_adj_list;
	genFanOutDagAdjlist(tmp_adj_list, dag_aug, num_task);
	for (int i = 0; i < num_task; i++)
	{
		int reverse_id = num_task - 1 - i;
		vector<int> new_list;
		new_list.push_back(num_task - 1 - i);
		vector<int> tmp_list = tmp_adj_list.find(i)->second;
		for (int j = 0; j < tmp_list.size(); j++)
		{
			dag_adj_list.insert(make_pair(num_task - 1 -
					tmp_list.at(tmp_list.size() - 1 - j), new_list));
		}
	}
}

void genPipelineDagAdjlist(adj_list &dag_adj_list, int dag_aug, long num_task)
{
	int num_pipe = num_task / dag_aug;
	int index = -1, next = -1;
	for (int i = 0; i < num_pipe; i++)
	{
		for (int j = 0; j < dag_aug; j++)
		{
			index = i * dag_aug + j;
			next = index + 1;
			vector<int> new_list;
			if (next % dag_aug != 0 && next < num_task)
			{
				new_list.push_back(next);
			}
			dag_adj_list.insert(make_pair(index, new_list));
		}
	}
	for (index = num_pipe * dag_aug; index < num_task; index++)
	{
		next = index + 1;
		vector<int> new_list;
		if (next % dag_aug != 0 && next < num_task)
		{
			new_list.push_back(next);
		}
		dag_adj_list.insert(make_pair(index, new_list));
	}
}

void print_AdjList(adj_list &dag_adj_list)
{
	for(adj_list::iterator it = dag_adj_list.begin();
			it != dag_adj_list.end(); ++it)
	{
		vector<int> exist_list = it->second;
        cout << " " << it->first << " -> ";
        for(int i = 0; i < exist_list.size(); i++)
        {
        	cout << " " << exist_list[i] << ",";
        }
        cout << endl;
	}
}

void genDagAdjlist(adj_list &dag_adj_list, string &dag_type,
							int dag_aug, long num_task)
{
	if (!dag_type.compare("BOT"))
	{
		genBOTDagAdjlist(dag_adj_list, num_task);
	}
	else if (!dag_type.compare("FanOut"))
	{
		genFanOutDagAdjlist(dag_adj_list, dag_aug, num_task);
	}
	else if (!dag_type.compare("FanIn"))
	{
		genFanInDagAdjlist(dag_adj_list, dag_aug, num_task);
	}
	else if (!dag_type.compare("Pipeline"))
	{
		genPipelineDagAdjlist(dag_adj_list, dag_aug, num_task);
	}
}

void genDagInDegree(adj_list &dag_adj_list, in_degree &dag_in_degree)
{
	for (int i = 0; i < dag_adj_list.size(); i++)
	{
		dag_in_degree[i] = 0;
	}
	for(adj_list::iterator it = dag_adj_list.begin();
						it != dag_adj_list.end(); ++it)
	{
		int index = it->first;
		vector<int> existList = it->second;
		for (int j = 0; j < existList.size(); j++)
		{
			dag_in_degree[existList.at(j)]++;
		}
	}
}

bool initZHTClient(ZHTClient &zc, const string &zhtcfgFile, const string &neighFile)
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

