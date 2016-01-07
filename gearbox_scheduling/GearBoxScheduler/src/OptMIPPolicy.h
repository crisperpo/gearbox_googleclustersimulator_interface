/*
 * OptMIPPolicy.h
 *
 *  Created on: Jun 1, 2013
 *      Author: lm111
 */

#ifndef OPTMIPPOLICY_H_
#define OPTMIPPOLICY_H_

#include "Policy.h"

class OptMIPPolicy: public Policy {
public:
	OptMIPPolicy();
	virtual ~OptMIPPolicy();
	virtual bool schedule(list<Job>& jobList, map<string, string>& pars);
};

#endif /* OPTMIPPOLICY_H_ */
