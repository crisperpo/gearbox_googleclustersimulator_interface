/*
 * JobStatusReader.h
 *
 *  Created on: May 31, 2013
 *      Author: lm111
 */

#ifndef JOBSTATUSREADER_H_
#define JOBSTATUSREADER_H_

#include <iostream>
#include <list>
#include <string>

#include "Job.h"

using namespace std;

class JobStatusReader {
private:
	string path;
public:
	JobStatusReader();
	JobStatusReader(string path);
	list<Job>& readJobs();
	virtual ~JobStatusReader();
};

#endif /* JOBSTATUSREADER_H_ */
