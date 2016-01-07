/*
 * Scheduler.h
 *
 *  Created on: Jun 1, 2013
 *      Author: lm111
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <map>
#include <string>
#include <iostream>
#include <list>
#include "OptMIPPolicy.h"
using namespace std;

class Scheduler {
public:
	map<string, string> pars;
	Policy* policy;
	list<Job>& jobList;
	Scheduler(list<Job>& jobList);
	Scheduler(list<Job>& jobList, string configPath);
	void schedule();
	void writeResultsToFile(string out);
	virtual ~Scheduler();
};

#endif /* SCHEDULER_H_ */
