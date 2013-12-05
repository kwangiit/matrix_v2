/*
 * util.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "util.h"

int getInt(string str)
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

long getLong(string str)
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

string getHostName(string type)
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

vector<string> readFromFile(string file_name)
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

int getSelfIndex(string str, vector<string> str_vec)
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

