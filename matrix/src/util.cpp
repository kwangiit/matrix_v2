/*
 * util.cpp
 *
 *  Created on: Dec 4, 2013
 *      Author: kwang
 */

#include "util.h"
#include "matrix_tcp_proxy_stub.h"

uint _BUF_SIZE = 8192;
Mutex tokenMutex = Mutex();
Mutex sockMutex = Mutex();

vector<string> tokenize(const std::string &source,
		const char *delimiter = " ") {
	//tokenMutex.lock();
	vector<string> results;
	size_t prev = 0, next = 0;

	if (source.empty()) {
		return results;
	}

	while ((next = source.find_first_of(delimiter, prev)) != string::npos) {
		if (next - prev != 0) {
			results.push_back(source.substr(prev, next - prev));
		}
		prev = next + 1;
	}

	if (prev < source.size()) {
		results.push_back(source.substr(prev));
	}
	//tokenMutex.unlock();
	return results;
}

int get_ip(char *outIP) {
	int i = 0, sockfd;
	struct ifconf ifconf_local;
	char buf[512];
	struct ifreq *ifreq_local;
	char* ip;
	ifconf_local.ifc_len = 512;
	ifconf_local.ifc_buf = buf;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return -1;
	}

	ioctl(sockfd, SIOCGIFCONF, &ifconf_local);
	close(sockfd);

	ifreq_local = (struct ifreq*) buf;
	for (i = (ifconf_local.ifc_len / sizeof(struct ifreq)); i > 0; i--) {
		ip = inet_ntoa(
				((struct sockaddr_in*) &(ifreq_local->ifr_addr))->sin_addr);
		if (strcmp(ip, "127.0.0.1") == 0) {
			ifreq_local++;
			continue;
		}

		strcpy(outIP, ip);
		return 0;
	}
	return -1;
}

string exec(const char *cmd) {
	FILE* pipe = popen(cmd, "r");
	if (!pipe) {
		return "ERROR";
	}

	char buffer[128];
	string result = "";

	while (!feof(pipe)) {
		if (fgets(buffer, 128, pipe) != NULL) {
			result += buffer;
		}
	}

	pclose(pipe);

	if (!result.empty() && result[result.length() - 1] == '\n') {
		result.erase(result.length() - 1);
	}

	return result;
}

string get_host_id(const string &type) {
	string id;

	if (type.compare("localhost") == 0) {
		id = "localhost";
	} else if (type.compare("ip") == 0) {
		char ip_cstr[30];
		memset(ip_cstr, '\0', 30);
		get_ip(ip_cstr);
		id.assign(ip_cstr);
	} else if (type.compare("hostname") == 0) {
		id = exec("hostname");
	}

	return id;
}

vector<string> read_from_file(const string &fileName) {
	ifstream fileStream(fileName.c_str());
	vector<string> fileVec;
	string line;

	if (!fileStream.good()) {
		return fileVec;
	}

	while (getline(fileStream, line)) {
		fileVec.push_back(line);
	}

	return fileVec;
}

int get_self_idx(const string &str, vector<string> strVec) {
	int idx = -1;

	for (int i = 0; i < strVec.size(); i++) {
		if (str.compare(strVec.at(i)) == 0) {
			idx = i;
			break;
		}
	}

	return idx;
}

/* generate adjecency list for BOT independent tasks */
void gen_bot_adjlist(adjList &dagAdjList, long numTask) {
	for (long i = 0; i < numTask; i++) {
		vector<long> newList;
		dagAdjList.insert(make_pair(i, newList));
	}
}

/*
 * generate adjacency list for fanout dags,
 * the argument is the fan out degree
 * */
void gen_fanout_adjlist(adjList &dagAdjList, long dagArg, long numTask) {
	long next = -1;

	for (long i = 0; i < numTask; i++) {
		vector<long> newList;

		for (long j = 1; j <= dagArg; j++) {
			next = i * dagArg + j;

			if (next >= numTask) {
				break;
			} else {
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
void gen_fanin_adjlist(adjList &dagAdjList, long dagArg, long numTask) {
	adjList tmpAdjList;

	/* first generate an adjacency list for fan out dag,
	 * and then flip it over, and switch the left with the
	 * right to get the adjacency list for fan in dags */
	gen_fanout_adjlist(tmpAdjList, dagArg, numTask);

	for (long i = 0; i < numTask; i++) {
		long reverseId = numTask - 1 - i;

		vector<long> newList;
		newList.push_back(reverseId);

		vector<long> tmpList = tmpAdjList.find(i)->second;

		for (long j = 0; j < tmpList.size(); j++) {
			dagAdjList.insert(
					make_pair(numTask - 1 - tmpList.at(tmpList.size() - 1 - j),
							newList));
		}
	}

	vector<long> lastVec;
	dagAdjList.insert(make_pair(numTask - 1, lastVec));
}

void gen_pipeline_adjlist(adjList &dagAdjList, long dagArg, long numTask) {
	long numPipe = numTask / dagArg, index = -1, next = -1;

	for (long i = 0; i < numPipe; i++) {
		for (long j = 0; j < dagArg; j++) {
			index = i * dagArg + j;
			next = index + 1;
			vector<long> newList;

			if (next % dagArg != 0 && next < numTask) {
				newList.push_back(next);
			}

			dagAdjList.insert(make_pair(index, newList));
		}
	}

	for (index = numPipe * dagArg; index < numTask; index++) {
		next = index + 1;
		vector<long> newList;

		if (next % dagArg != 0 && next < numTask) {
			newList.push_back(next);
		}

		dagAdjList.insert(make_pair(index, newList));
	}
}

void print_adjlist(adjList &dagAdjList) {
	for (adjList::iterator it = dagAdjList.begin(); it != dagAdjList.end();
			++it) {
		vector<long> existList = it->second;
		cout << " " << it->first << " -> ";

		for (long i = 0; i < existList.size(); i++) {
			cout << " " << existList[i] << ",";
		}

		cout << endl;
	}
}

void gen_dag_adjlist(adjList &dagAdjList, string &dagType, long dagArg, long numTask) {
	if (dagType.compare("BOT") == 0)
		gen_bot_adjlist(dagAdjList, numTask);
	else if (dagType.compare("FanOut") == 0)
		gen_fanout_adjlist(dagAdjList, dagArg, numTask);
	else if (dagType.compare("FanIn") == 0)
		gen_fanin_adjlist(dagAdjList, dagArg, numTask);
	else if (dagType.compare("Pipeline") == 0)
		gen_pipeline_adjlist(dagAdjList, dagArg, numTask);
	//print_adjlist(dagAdjList);
}

void gen_dag_indegree(adjList &dagAdjList, inDegree &dagInDegree) {
	for (long i = 0; i < dagAdjList.size(); i++) {
		dagInDegree[i] = 0;
	}
	for (adjList::iterator it = dagAdjList.begin(); it != dagAdjList.end();
			++it) {
		long index = it->first;
		vector<long> existList = it->second;

		for (long j = 0; j < existList.size(); j++) {
			dagInDegree[existList.at(j)]++;
		}
	}
}

void gen_dag_parents(adjList &dagAdjList, adjList &dagParentList) {
	for (long i = 0; i < dagAdjList.size(); i++) {
		vector<long> parents;
		dagParentList.insert(make_pair(i, parents));
	}

	for (adjList::iterator it = dagAdjList.begin(); it != dagAdjList.end();
			++it) {
		long index = it->first;
		vector<long> existList = it->second;

		for (long i = 0; i < existList.size(); i++) {
			dagParentList.find(existList.at(i))->second.push_back(index);
		}
	}
}

long get_time_usec() {
	struct timeval currentTime;

	gettimeofday(&currentTime, NULL);
	long time = (currentTime.tv_sec) * 1000000L + currentTime.tv_usec;

	//cout << "the time is:" << time << endl;
	return time;
}

double get_time_msec() {
	struct timeval currentTime;

	gettimeofday(&currentTime, NULL);
	return static_cast<double>(currentTime.tv_sec) * 1000.0
			+ static_cast<double>(currentTime.tv_usec) / 1000.0;
}

double get_time_sec() {
	struct timeval currentTime;

	gettimeofday(&currentTime, NULL);
	return static_cast<double>(currentTime.tv_sec)
			+ static_cast<double>(currentTime.tv_usec) / 1000000.0;
}

timespec time_diff(timespec start, timespec end) {
	timespec diff;
	uint64_t ts, te;

	ts = (uint64_t) start.tv_sec * 1000000000 + (uint64_t) start.tv_nsec;
	te = (uint64_t) end.tv_sec * 1000000000 + (uint64_t) end.tv_nsec;
	diff.tv_sec = (te - ts) / 1000000000;
	diff.tv_nsec = (te - ts) % 1000000000;

	return diff;
}

extern string taskmsg_to_str(const TaskMsg &taskMsg) {
	string str("");

	str.append(taskMsg.taskid());
	str.append("@@");
	str.append(taskMsg.user());
	str.append("@@");
	str.append(taskMsg.dir());
	str.append("@@");
	str.append(taskMsg.cmd());
	str.append("@@");
	str.append(num_to_str<long>(taskMsg.datalength()));
	str.append("@@");

	return str;
}

extern TaskMsg str_to_taskmsg(const string &str) {
	vector<string> vecStr = tokenize(str, "@@");

	if (vecStr.size() == 0) {
		cout << "have some problem" << endl;
		exit(1);
	}
	TaskMsg tm;
	tm.set_taskid(vecStr.at(0));
	if (vecStr.size() > 1) {
		tm.set_user(vecStr.at(1));
	} else {
		cout << "has problem, the vector size is:1" << endl;
		tm.set_user("kwang");
	}
	if (vecStr.size() > 2) {
		tm.set_dir(vecStr.at(2));
	} else {
		cout << "has problem, the vector size is:2" << endl;
		tm.set_dir("/home/kwang/Documents");
	}
	if (vecStr.size() > 3) {
		tm.set_cmd(vecStr.at(3));
	} else {
		cout << "has problem, the vector size is:3" << endl;
		tm.set_cmd("hostname");
	}
	if (vecStr.size() > 4) {
		tm.set_datalength(str_to_num<long>(vecStr.at(4)));
	} else {
		cout << "has problem, the vector size is:4" << endl;
		tm.set_datalength(0);
	}
	return tm;
}

extern string value_to_str(const Value &value) {
	string str("");

	str.append(value.id());
	str.append("~~");

	if (value.has_indegree()) {
		str.append(num_to_str<long>(value.indegree()));
	} else {
		str.append("noindegree");
	}
	str.append("~~");

	if (value.parents_size() > 0) {
		for (int i = 0; i < value.parents_size(); i++) {
			str.append(value.parents(i));
			str.append("??");
		}
	} else {
		str.append("noparents");
	}
	str.append("~~");

	if (value.children_size() > 0) {
		for (int i = 0; i < value.children_size(); i++) {
			str.append(value.children(i));
			str.append("??");
		}
	} else {
		str.append("nochildren");
	}
	str.append("~~");

	if (value.datanamelist_size() > 0) {
		for (int i = 0; i < value.datanamelist_size(); i++) {
			str.append(value.datanamelist(i));
			str.append("??");
		}
	} else {
		str.append("nodataname");
	}
	str.append("~~");

	if (value.datasize_size() > 0) {
		for (int i = 0; i < value.datasize_size(); i++) {
			str.append(num_to_str<long>(value.datasize(i)));
			str.append("??");
		}
	} else {
		str.append("nodatasize");
	}
	str.append("~~");

	if (value.has_alldatasize()) {
		str.append(num_to_str<long>(value.alldatasize()));
	} else {
		str.append("noalldatasize");
	}
	str.append("~~");

	if (value.has_tasklength())
		str.append(num_to_str<long>(value.tasklength()));
	else
		str.append("notasklength");
	str.append("~~");

	if (value.has_numtaskfin()) {
		str.append(num_to_str<long>(value.numtaskfin()));
	} else {
		str.append("nonumtaskfin");
	}
	str.append("~~");

	if (value.has_numworksteal()) {
		str.append(num_to_str<long>(value.numworksteal()));
	} else {
		str.append("nonumworksteal");
	}
	str.append("~~");

	if (value.has_numworkstealfail()) {
		str.append(num_to_str<long>(value.numworkstealfail()));
	} else {
		str.append("nonumworkstealfail");
	}
	str.append("~~");

	if (value.has_numtaskwait()) {
		str.append(num_to_str<int>(value.numtaskwait()));
	} else {
		str.append("nonumtaskwait");
	}
	str.append("~~");

	if (value.has_numtaskready()) {
		str.append(num_to_str<int>(value.numtaskready()));
	} else {
		str.append("nonumtaskready");
	}
	str.append("~~");

	if (value.has_numcoreavilable()) {
		str.append(num_to_str<int>(value.numcoreavilable()));
	} else {
		str.append("nonumcoreavail");
	}
	str.append("~~");

	if (value.has_numallcore()) {
		str.append(num_to_str<int>(value.numallcore()));
	} else {
		str.append("nonumallcore");
	}
	str.append("~~");

	if (value.has_outputsize())
		str.append(num_to_str<long>(value.outputsize()));
	else
		str.append("nooutputsize");
	str.append("~~");

	if (value.has_submittime())
		str.append(num_to_str<long>(value.submittime()));
	else
		str.append("nosubmittime");
	str.append("~~");

	return str;
}

extern Value str_to_value(const string &str) {
	Value value;
	vector<string> vec = tokenize(str, "~~");

	if (vec.size() < 17) {
		cout << "have some problem, the value to be converted is:" << str
				<< endl;
		exit(1);
	}

	value.set_id(vec.at(0));

	if (vec.at(1).compare("noindegree") != 0) {
		value.set_indegree(str_to_num<long>(vec.at(1)));
	}

	if (vec.at(2).compare("noparents") != 0) {
		vector<string> parentVec = tokenize(vec.at(2), "??");
		for (int i = 0; i < parentVec.size(); i++) {
			value.add_parents(parentVec.at(i));
		}
	}

	if (vec.at(3).compare("nochildren") != 0) {
		vector<string> childVec = tokenize(vec.at(3), "??");
		for (int i = 0; i < childVec.size(); i++) {
			value.add_children(childVec.at(i));
		}
	}

	if (vec.at(4).compare("nodataname") != 0) {
		vector<string> dataNameVec = tokenize(vec.at(4), "??");
		for (int i = 0; i < dataNameVec.size(); i++) {
			value.add_datanamelist(dataNameVec.at(i));
		}
	}

	if (vec.at(5).compare("nodatasize") != 0) {
		vector<string> dataSizeVec = tokenize(vec.at(5), "??");
		for (int i = 0; i < dataSizeVec.size(); i++) {
			value.add_datasize(str_to_num<long>(dataSizeVec.at(i)));
		}
	}

	if (vec.at(6).compare("noalldatasize") != 0) {
		value.set_alldatasize(str_to_num<long>(vec.at(6)));
	}

	if (vec.at(7).compare("notasklength") != 0)
		value.set_tasklength(str_to_num<long>(vec.at(7)));

	if (vec.at(8).compare("nonumtaskfin") != 0) {
		value.set_numtaskfin(str_to_num<long>(vec.at(8)));
	}

	if (vec.at(9).compare("nonumworksteal") != 0) {
		value.set_numworksteal(str_to_num<long>(vec.at(9)));
	}

	if (vec.at(10).compare("nonumworkstealfail") != 0) {
		value.set_numworkstealfail(str_to_num<long>(vec.at(10)));
	}

	if (vec.at(11).compare("nonumtaskwait") != 0) {
		value.set_numtaskwait(str_to_num<int>(vec.at(11)));
	}

	if (vec.at(12).compare("nonumtaskready") != 0) {
		value.set_numtaskready(str_to_num<int>(vec.at(12)));
	}

	if (vec.at(13).compare("nonumcoreavail") != 0) {
		value.set_numcoreavilable(str_to_num<int>(vec.at(13)));
	}

	if (vec.at(14).compare("nonumallcore") != 0) {
		value.set_numallcore(str_to_num<int>(vec.at(14)));
	}

	if (vec.at(15).compare("nooutputsize") != 0)
		value.set_outputsize(str_to_num<int>(vec.at(15)));

	if (vec.at(16).compare("nosubmittime") != 0)
		value.set_submittime(str_to_num<long>(vec.at(16)));
	return value;
}

/*required string msgType = 1;
 optional string extraInfo = 2;
 optional int64 count = 3;
 repeated string tasks = 4;*/
extern string mm_to_str(const MatrixMsg &mm) {
	string str("");
	str.append(mm.msgtype());
	str.append("&&");

	if (mm.has_extrainfo()) {
		str.append(mm.extrainfo());
	} else {
		str.append("noextrainfo");
	}
	str.append("&&");

	if (mm.has_count()) {
		str.append(num_to_str<int>(mm.count()));
	} else {
		str.append("nocount");
	}
	str.append("&&");

	if (mm.tasks_size() > 0) {
		for (int i = 0; i < mm.tasks_size(); i++) {
			str.append(mm.tasks(i));
			str.append("!!");
		}
	} else {
		str.append("notask");
	}
	str.append("&&");

	return str;
}

extern MatrixMsg str_to_mm(const string &str) {
	vector<string> vec = tokenize(str, "&&");
	MatrixMsg mm;
	mm.set_msgtype(vec.at(0));

	if (vec.at(1).compare("noextrainfo") != 0) {
		mm.set_extrainfo(vec.at(1));
	}
	if (vec.at(2).compare("nocount") != 0) {
		mm.set_count(str_to_num<int>(vec.at(2)));
	}
	if (vec.at(3).compare("notask") != 0) {
		vector<string> taskVec = tokenize(vec.at(3), "!!");
		for (int i = 0; i < taskVec.size(); i++) {
			mm.add_tasks(taskVec.at(i));
		}
	}

	return mm;
}

Mutex::Mutex() {
	int ret = pthread_mutex_init(&mutex, NULL);
}

Mutex::~Mutex() {

}

int Mutex::lock() {
	return (pthread_mutex_lock(&mutex));
}

int Mutex::unlock() {
	return (pthread_mutex_unlock(&mutex));
}

Peer::Peer(const string &configFile) {
	config = new Configuration(configFile);
	set_id(get_host_id(config->hostIdType));
	schedulerVec = read_from_file(config->schedulerMemFile);
	set_index(get_self_idx(get_id(), schedulerVec));
	running = true;
	numZHTMsg = 0;
	init_zht_client(config->zhtConfigFile, config->zhtMemFile);
}

Peer::~Peer() {

}

bool Peer::init_zht_client(const string &zhtcfgFile, const string &neighFile) {
	if (zhtcfgFile.empty() || neighFile.empty()) {
		return false;
	} else {
		if (zc.init(zhtcfgFile, neighFile) != 0) {
			return false;
		} else {
			return true;
		}
	}
}

void Peer::insert_wrap(const string &key, const string &val) {
	if (key.empty()) {
		cout << "There is empty key!" << endl;
		return;
	}
	while (zc.insert(key, val) != 0) {
		usleep(1);
	}
}

void Peer::insert_wrap(const char *key, const char *val) {
	if (key == NULL) {
		cout << "There is empty key!" << endl;
		return;
	}
	while (zc.insert(key, val) != 0) {
		usleep(1);
	}
}

void Peer::lookup_wrap(const string &key, string &result) {
	if (key.empty()) {
		return;
	}
	while (zc.lookup(key, result) != 0 && result.empty()) {
		cout << "the key is:" << key << endl;
		usleep(1);
	}
}

void Peer::lookup_wrap(const char *key, char *result) {
	if (key == NULL) {
		return;
	}
	while (zc.lookup(key, result) != 0 && result == NULL) {
		usleep(1);
	}
}

void Peer::set_id(const string &id) {
	this->id = id;
}

string Peer::get_id() {
	return id;
}

void Peer::set_index(int index) {
	this->index = index;
}

int Peer::get_index() {
	return index;
}

void Peer::wait_all_scheduler() {
	string key("number of scheduler registered");
	string value;
	zc.lookup(key, value);
	while (str_to_num<int>(value) != schedulerVec.size()) {
		usleep(10000);
		zc.lookup(key, value);
	}

//	string expValue = num_to_str<int>(schedulerVec.size());
//
//	while (zc.state_change_callback(key, expValue, config->sleepLength) != 0) {
//		usleep(1000);
//	}
}

void Peer::wait_all_task_recv() {
	string key("num tasks recv");
	string value;
	zc.lookup(key, value);
	while (str_to_num<long>(value) != config->numAllTask) {
		usleep(10000);
		zc.lookup(key, value);
	}
//	string expValue = num_to_str<long>(config->numAllTask);
//
//	while (zc.state_change_callback(key, expValue, config->sleepLength) != 0) {
//		usleep(1000);
//	}
}

void Peer::incre_ZHT_msg_count(long increment) {
	numZHTMsg += increment;
}

void Peer::send_batch_tasks(vector<TaskMsg> taskVec,
		int sockfd, const string& peer) {
	long numTaskLeft = taskVec.size();
	long numTaskBeenSent = 0;
	long numTaskSendPerPkg = config->maxTaskPerPkg;

	MatrixMsg mmNumTask;
	mmNumTask.set_msgtype(peer + " send tasks");
	mmNumTask.set_count(numTaskLeft);
	string strNumTask = mm_to_str(mmNumTask);

	if (numTaskLeft <= 0) {
		send_mul(sockfd, strNumTask, true);
	} else {
		bool end = false;
		send_mul(sockfd, strNumTask, end);
		while (numTaskLeft > 0) {
			if (numTaskLeft <= config->maxTaskPerPkg) {
				numTaskSendPerPkg = numTaskLeft;
				end = true;
			}

			MatrixMsg mmTask;
			mmTask.set_msgtype(peer + " send tasks");
			mmTask.set_count(numTaskSendPerPkg);

			for (int i = 0; i < numTaskSendPerPkg; i++) {
				mmTask.add_tasks(taskmsg_to_str(
						taskVec.at(i + numTaskBeenSent)));
			}
			string strTasks = mm_to_str(mmTask);
			send_mul(sockfd, strTasks, end);
			numTaskLeft -= numTaskSendPerPkg;
			numTaskBeenSent += numTaskSendPerPkg;
		}
	}
}

void Peer::recv_batch_tasks(vector<TaskMsg> , int) {

}
