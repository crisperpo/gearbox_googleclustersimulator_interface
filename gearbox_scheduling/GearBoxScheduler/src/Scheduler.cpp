/*
 * Scheduler.cpp
 *
 *  Created on: Jun 1, 2013
 *      Author: lm111
 */

#include "Scheduler.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <vector>

Scheduler::Scheduler(list<Job>& list) :
		jobList(list) {
	cout << "The slot scheduler uses the default setting." << endl;
	pars.insert(pair<string, string>("policy", "MIP"));
	pars.insert(pair<string, string>("job_size_error", "1"));
	pars.insert(pair<string, string>("slot_count", "50"));
	pars.insert(pair<string, string>("time_slice_length", "1"));
	pars.insert(pair<string, string>("solution_error", "0.05"));
	pars.insert(pair<string, string>("computation_time_limit", "60"));

	policy = new OptMIPPolicy();
}

Scheduler::~Scheduler() {
	// TODO Auto-generated destructor stub
}

Scheduler::Scheduler(list<Job>& list, string configPath) :
		jobList(list) {
	// Read configuration info from the configPath
	ifstream file(configPath.c_str());
	string line;
	vector<string> strs;
	if (file.is_open()) {
		getline(file, line); // Skip the title line.
		cout << "\nScheduler configuration info: " << endl;
		while (!file.eof()) {
			getline(file, line);
			boost::split(strs, line, boost::is_any_of(":"));
			if (strs.size() >= 2) {
				string key = strs[0];
				string value = strs[1];
				cout << key << ": " << value << endl;
				pars.insert(pair<string, string>(key, value));
			} else {
				cout << "Configuration has errors." << endl;
				//exit(EXIT_FAILURE);
			}
			strs.clear();
		}
	} else {
		cout << "Configuration file " << configPath << " cannot be opened."
				<< endl;
		file.close();
		exit(EXIT_FAILURE);
	}

	file.close();

	string policyName = pars.find("policy")->second;
	if (policyName.compare("MIP") == 0) {
		policy = new OptMIPPolicy();
	} else {
		cout << "The policy " << policyName << " is not supported yet." << endl;
		exit(EXIT_FAILURE);
	}
}

void Scheduler::schedule() {
	policy->schedule(jobList, pars);
}

void Scheduler::writeResultsToFile(string out) {
	ofstream file(out.c_str());
	if (file.is_open()) {
		cout << "\nWrite results into the file: " << out << endl;
		for (list<Job>::iterator itr = jobList.begin(); itr != jobList.end();
				itr++) {
			file << itr->jobName << ":";
			for (list<long>::iterator itr2 = itr->slotPlan.begin(); itr2 != itr->slotPlan.end(); itr2++) {
				file << (*itr2) << " ";
			}
			file << "\n";
		}
		file.close();
	} else {
		cout << "Output file cannot be opened" << endl;
		exit(EXIT_FAILURE);
	}
}
