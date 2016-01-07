/*
 * JobStatusReader.cpp
 *
 *  Created on: May 31, 2013
 *      Author: lm111
 */

#include "JobStatusReader.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <vector>

JobStatusReader::JobStatusReader() {
	std::cout << "Try to read jobStatus file from the default directory: ../resources/jobStatus" << std::endl;
	this->path = "../resources/jobStatus";
}

JobStatusReader::~JobStatusReader() {
	// TODO Auto-generated destructor stub
}

JobStatusReader::JobStatusReader(string path) {
	this->path = path;
}

list<Job>& JobStatusReader::readJobs() {
	ifstream file(path.c_str());
	string line;
	vector<string> strs;
	list<Job>* jobList = new list<Job>();
	if (file.is_open()) {
		getline(file, line); // Skip the title line.
		cout << "\nJobs info: " << endl;
		while (!file.eof()) {
			getline(file, line);
			boost::split(strs, line, boost::is_any_of(","));
			if (strs.size() >= 7) {
				string jobName = strs[0];
				long jobSize = atol(strs[1].c_str());
				double startTime = atof(strs[2].c_str());
				double x1 = atof(strs[3].c_str());
				double x2 = atof(strs[4].c_str()); // x1 < x2
				double slope = atof(strs[5].c_str());
				double intercept = atof(strs[6].c_str());
				cout << jobName << "," << jobSize << "," << startTime << "," << x1 << "," << x2 << "," << slope << "," << intercept << endl;
				LinearFunction* func = new LinearFunction(x1, x2, intercept, slope);
				Job job(jobName, jobSize, startTime, func);
				jobList->push_back(job);
			} else {
				cout << "Job status data has errors." << endl;
				//file.close();
				//exit(EXIT_FAILURE);
			}
			strs.clear();
		}
	} else {
		cout << "Job status file cannot be opened." << endl;
		file.close();
		exit(EXIT_FAILURE);
	}

	file.close();
	return *jobList;
}

