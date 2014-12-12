/*
 * Copyright 2010-2020 DatasysLab@iit.edu(http://datasys.cs.iit.edu/index.html)
 *      Director: Ioan Raicu(iraicu@cs.iit.edu)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of ZHT library(http://datasys.cs.iit.edu/projects/ZHT/index.html).
 *      Tonglin Li(tli13@hawk.iit.edu) with nickname Tony,
 *      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname Xiaobingo,
 *      Ke Wang(kwang22@hawk.iit.edu) with nickname KWang,
 *      Dongfang Zhao(dzhao8@@hawk.iit.edu) with nickname DZhao,
 *      Ioan Raicu(iraicu@cs.iit.edu).
 *
 * ZHTUtil.cpp
 *
 *  Created on: Jun 25, 2013
 *      Author: Tony
 *      Contributor: Xiaobingo, KWang, DZhao
 */

#include "ZHTUtil.h"

#include "Util.h"
#include "ConfHandler.h"

#include <arpa/inet.h>
#include <algorithm>
#include <netdb.h>

#include  "zpack.pb.h"

using namespace iit::datasys::zht::dm;

ZHTUtil::ZHTUtil() {
}

ZHTUtil::~ZHTUtil() {
}

HostEntity ZHTUtil::getHostEntityByKey(const string& msg) {

	ZPack zpack = str_to_zpack(msg);
	//zpack.ParseFromString(msg); //to debug

	uint64_t hascode = HashUtil::genHash(zpack.key());
	size_t node_size = ConfHandler::NeighborVector.size();
	int index = hascode % node_size;

	ConfEntry ce = ConfHandler::NeighborVector.at(index);

	return buildHostEntity(ce.name(), atoi(ce.value().c_str()));

}

HostEntity ZHTUtil::buildHostEntity(const string& host, const uint& port) {

	HostEntity he;

	/*
	 struct sockaddr_in si_other;
	 hostent *record;
	 in_addr *address;
	 string ip_address;

	 record = gethostbyname(host.c_str());
	 address = (in_addr *) record->h_addr;
	 ip_address = inet_ntoa(*address);

	 memset((char *) &si_other, 0, sizeof(si_other));
	 si_other.sin_family = AF_INET;
	 si_other.sin_port = htons(port);
	 if (inet_aton(ip_address.c_str(), &si_other.sin_addr) == 0) {
	 fprintf(stderr, "inet_aton() failed\n");
	 }

	 he.si = si_other;
	 he.host = host;
	 he.port = port;
	 he.valid = true;
	 he.sock = -1;*/

	he.host = host;
	he.port = port;
	he.sock = -1;

	return he;
}

const uint IdHelper::ID_LEN = 20;

IdHelper::IdHelper() {
}

IdHelper::~IdHelper() {
}

uint64_t IdHelper::genId() {

	return HashUtil::genHash(HashUtil::randomString(62).c_str());
}

vector<string> zht_tokenize(const std::string &source,
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

extern string zpack_to_str(const ZPack &zpack) {
	string str("");

	if (zpack.has_opcode())
		str.append(zpack.opcode());
	else
		str.append("noopcode");
	str.append("//");

	if (zpack.has_key())
		str.append(zpack.key());
	else
		str.append("nokey");
	str.append("//");

	if (zpack.has_val())
		str.append(zpack.val());
	else
		str.append("noval");
	str.append("//");

	if (zpack.has_newval())
		str.append(zpack.newval());
	else
		str.append("nonewval");
	str.append("//");

	if (zpack.has_lease())
		str.append(zpack.lease());
	else
		str.append("nolease");
	str.append("//");

	if (zpack.has_valnull())
		str.append(zht_num_to_str<bool>(zpack.valnull()));
	else
		str.append("novaln");
	str.append("//");

	if (zpack.has_newvalnull())
		str.append(zht_num_to_str<bool>(zpack.newvalnull()));
	else
		str.append("nonewvaln");
	str.append("//");

	if (zpack.has_replicanum())
		str.append(zht_num_to_str<int>(zpack.replicanum()));
	else
		str.append("noreplicanum");
	str.append("//");
	return str;
}

extern ZPack str_to_zpack(const string &str) {
	ZPack zpack;
	if (str.empty())
		return zpack;
	vector<string> vec = zht_tokenize(str, "//");
	if (vec.size() < 8) {
		cout << "have some problem, the value to be converted is:" << str
				<< endl;
		return zpack;
		//exit(1);
	}

	if (vec.at(0).compare("noopcode") != 0)
		zpack.set_opcode(vec.at(0));

	if (vec.at(1).compare("nokey") != 0)
		zpack.set_key(vec.at(1));

	if (vec.at(2).compare("noval") != 0)
		zpack.set_val(vec.at(2));

	if (vec.at(3).compare("nonewval") != 0)
		zpack.set_newval(vec.at(3));

	if (vec.at(4).compare("nolease") != 0)
		zpack.set_lease(vec.at(4));

	if (vec.at(5).compare("novaln") != 0)
		zpack.set_valnull(zht_str_to_num<bool>(vec.at(5)));

	if (vec.at(6).compare("nonewvaln") != 0)
		zpack.set_newvalnull(zht_str_to_num<bool>(vec.at(6)));

	if (vec.at(7).compare("noreplicanum") != 0)
		zpack.set_replicanum(zht_str_to_num<int>(vec.at(7)));

	return zpack;
}
