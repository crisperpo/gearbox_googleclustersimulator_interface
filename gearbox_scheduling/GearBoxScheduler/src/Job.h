/*
 * Job.h
 *
 *  Created on: May 31, 2013
 *      Author: lm111
 */

#ifndef JOB_H_
#define JOB_H_

#include <string>
#include <list>
#include "Function.h"
using namespace std;

class Job {
public:
	string jobName; // The name of the job
	long jobSize; // The size of the job
	double startTime; // The time this job began
	Function* pricingFunc; // Pricing function
	list<long> slotPlan; // The allocation plan

	Job();
	virtual ~Job();
	Job(string name, long size,  double time, Function* func) :
		jobName(name), jobSize(size), startTime(time), pricingFunc(func) {}
};

#endif /* JOB_H_ */
