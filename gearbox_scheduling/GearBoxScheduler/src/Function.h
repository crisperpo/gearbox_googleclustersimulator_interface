/*
 * Function.h
 *
 *  Created on: May 31, 2013
 *      Author: lm111
 */

#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <ilcplex/ilocplex.h>
#include <assert.h>
using namespace std;
ILOSTLBEGIN

class Function {
public:
	double minX;
	double maxX;
	Function(double a, double e);
	virtual ~Function();
	virtual double returnY(double x) = 0;
	virtual double returnX(double y) = 0;
	virtual IloExpr& returnExpr(IloEnv& env, IloNumVar& x) = 0;
	virtual IloExpr& returnExpr(IloEnv& env, IloExpr& x) = 0;
	virtual Function* clone() = 0;
};

/**
 * y = _slope * x + _intercept
 */
class LinearFunction: public Function {
public:
	double intercept;
	double slope;
	LinearFunction(double b, double e, double i, double s) :
			Function(b, e), intercept(i), slope(s) {
	}
	double returnY(double x) {
		return (x * slope + intercept);
	}
	double returnX(double y) {
		return (y - intercept) / slope;
	}

	IloExpr& returnExpr(IloEnv& env, IloNumVar& x) {
		assert(slope < 0);

		IloExpr* expr = new IloExpr(env);
		(*expr) += slope * x + intercept;
		return (*expr);
	}

	IloExpr& returnExpr(IloEnv& env, IloExpr& x) {
		assert(slope < 0);

		IloExpr* expr = new IloExpr(env);
		(*expr) += slope * x + intercept;
		return (*expr);
	}

	Function* clone() {
		return new LinearFunction(minX, maxX, intercept, slope);
	}

	virtual ~LinearFunction() {
	}
};

/**
 *  y = alpha / x + beta
 */
class MultiplicativeInverseFunction: public Function {
public:
	double alpha;
	double beta;
	MultiplicativeInverseFunction(double begin, double end, double a, double b) :
			Function(begin, end), alpha(a), beta(b) {
		assert(alpha < 0);
	}

	double returnY(double x) {
		return (alpha / x + beta);
	}

	double returnX(double y) {
		return alpha / (y - beta);
	}

	IloExpr& returnExpr(IloEnv& env, IloNumVar& x) {
		assert(alpha < 0);

		IloExpr* expr = new IloExpr(env);
		(*expr) += alpha / x + beta;
		return (*expr);
	}

	IloExpr& returnExpr(IloEnv& env, IloExpr& x) {
		assert(alpha < 0);

		IloExpr* expr = new IloExpr(env);
		(*expr) += alpha / x + beta;
		return (*expr);
	}

	Function* clone() {
		return new MultiplicativeInverseFunction(minX, maxX, alpha, beta);
	}

	virtual ~MultiplicativeInverseFunction() {
	}
};

#endif /* FUNCTION_H_ */
