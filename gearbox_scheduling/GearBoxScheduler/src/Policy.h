/*
 * Policy.h
 *
 *  Created on: Jun 1, 2013
 *      Author: lm111
 */

#ifndef POLICY_H_
#define POLICY_H_


#include <list>
#include <map>
#include <iostream>

#include "Job.h"

using namespace std;

class Policy {
public:
	Policy();
	virtual ~Policy();
	virtual bool schedule(list<Job>& jobList, map<string, string>& pars) = 0;
};

#endif /* POLICY_H_ */
