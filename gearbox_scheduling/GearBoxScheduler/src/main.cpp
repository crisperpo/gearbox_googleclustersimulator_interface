//============================================================================
// Name        : main.cpp
// Author      : Luo Mai
// Version     : beta 0.1
// Copyright   : Individual
// Description : Implement the mapping slots scheduling for Hadoop.
//============================================================================

#include <iostream>
#include <string>
#include <list>

#include "JobStatusReader.h"
#include "Scheduler.h"

using namespace std;

int main(int argc, char *argv[]) {
	string configFilePath;
	string jobStatusFilePath;
	string outputFilePath;

	cout << "\n\n" << endl;
	// Print the running information of the scheduler.
	cout << "Scheduler program: " << argv[0] << " is running." << endl;

	if (argc < 4) {
		cout << "\nInput parameters are not enough." << endl;
		cout << "Correct command should be: " << argv[0] << " pathToConfigurationFile pathToJobStatusFile pathToOutputFile" << endl;
		cout << "Using the default setting." << endl;
		cout << "Config file: ../resources/scheduler/config" << endl;
		cout << "JobStatus file: ../resources/scheduler/job_status" << endl;
		cout << "Ouput file: ../resources/scheduler/out" << endl;
		configFilePath = "../resources/scheduler/config";
		jobStatusFilePath =  "../resources/scheduler/job_status";
		outputFilePath = "../resources/scheduler/out";
	} else {
		configFilePath = argv[1];
		jobStatusFilePath = argv[2];
		outputFilePath = argv[3];
		cout << "\nConfig file: " << configFilePath << endl;
		cout << "JobStatus file: "<< jobStatusFilePath << endl;
		cout << "Output file: " << outputFilePath << endl;
	}

	JobStatusReader reader(jobStatusFilePath);
	list<Job>& jobList = reader.readJobs();

	Scheduler scheduler(jobList, configFilePath);
	scheduler.schedule();
	scheduler.writeResultsToFile(outputFilePath);

	return 0;
}
