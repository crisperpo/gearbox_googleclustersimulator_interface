//============================================================================
// Name        : main.cpp
// Author      : Cristina Restrepo
// Copyright   : Individual
// Description : Implement the interface to connect Gearbox with the cluster simulator.
//============================================================================
#include <iostream>
#include <string>
#include <list>
#include <sstream>
#include <cstdlib>
#include <map>
#include <vector>
#include <fstream>
#include <cstring>
#include <string>
#include <time.h>

using namespace std;

/**********************************************************************************************/
/*GLOBAL VARIABLES AND ITERATORS*/
/**********************************************************************************************/
// Define number of slots as Gearbox plan has 100 steps
int slots=100;	
time_t start,end;
// Variable to contain the simulator output after every iteration
map <string, map <string, int> > simulator_output;
// Variable to contain the Gearbox plan
map <string, vector<int> > gearbox_plan;
// Variable to contain the total tasks scheduled in the simulator until now for each job
map <string, int> sched_tasks;
//Variable to contain the total tasks to be scheduled
map <string, int> totaltasks;

//iterators
typedef map<string, map <string, int> >::iterator it_simoutput;
typedef map<string, vector<int> >::iterator it_gearboxplan;
typedef map<string, int>::iterator it_schtasks;

/**********************************************************************************************/
/*FUNCTIONS*/
/**********************************************************************************************/
//Function to update the input of gearbox in order to run it again
//Receives as an argument the iteration number between 100 and 0 that is gonna be lthe new PfEnd
void UpdateGearboxInput(int NewPfEnd){
	string jobid;
	string line;
	int NewTotalTasks;
	
	//Load number tasks defined in the Gearbox job_status into the variable totaltasks
	ifstream gearboxfile;
	gearboxfile.open("GearBoxScheduler/resources/scheduler/job_status");
	if (gearboxfile.is_open()){
    	while ( getline (gearboxfile,line) ){
    		stringstream   linestream(line);
    		string         data;
    		int i=1;
    		while(getline(linestream, data, ',')){
    			if(i==1)jobid=data;
    			if(i==2)totaltasks[jobid]=atoi(data.c_str());
    			++i;
    		}
    	}
    }
    gearboxfile.close();

	//Clean Gearbox job_status file in order to regenerate it
	ofstream myfile;
	myfile.open("GearBoxScheduler/resources/scheduler/job_status", ofstream::out | ofstream::trunc);
	
	//Regenerate Gearbox job_status file updating the PfEnd and the number of tasks left
	myfile << "jobId, jobSize, startTime, pfBegin, pfEnd, slope, intercept" << endl;
	for(it_simoutput iterator = simulator_output.begin(); iterator != simulator_output.end(); iterator++) {
		jobid=iterator->first;
		//Calculate new tasks number for gearbox to generate the new plan 
		NewTotalTasks=totaltasks[jobid]-sched_tasks[jobid];
		//Load the line into the file
		myfile <<jobid<<","<<NewTotalTasks<<",0,0,"<<NewPfEnd<<",-1,100"<<endl;
	}
	myfile.close();

	//Restart scheduled tasks counter to 0 after loading them in gearbox input
	for(it_schtasks iterator = sched_tasks.begin(); iterator != sched_tasks.end(); iterator++) {
		sched_tasks[iterator->first]=0;;
    	cout<<iterator->second<<"\n";
    }
}

//Receives as argument the iteration number from 100 to 0
//Get Gearbox plan step for this iteration from variable 
//and load it into simulator input for this iteration (example-init-cluster-state.log)
void LoadGearboxOutputIntoSimInput(int it){
	//The simulator input format is like the following line
	//12 7.000000 111 0 0 501 0.30000000149011612 1074000000000
	
	string jobid;
	int numtasks;
	ofstream myfile;
	
	//Clean simulator input file example-init-cluster-state.log before loading a new one
	myfile.open("ClusterSimulator/traces/example-init-cluster-state.log", ofstream::out | ofstream::trunc);
	
	//Generate the new simulator input file
	for(it_gearboxplan iterator = gearbox_plan.begin(); iterator != gearbox_plan.end(); iterator++) {
		jobid=iterator->first;
		numtasks=gearbox_plan[jobid][(slots-it)%10];
		myfile <<"11 7.000000 "<<jobid<<" 0 0 "<<numtasks<<" 0.30000000149011612 107400000000"<<endl;
			
	}
	myfile.close();
}

//Function to load the Gearbox allocation plan from its output file 
//into the variable gearbox_plan
void LoadAllocationPlan(){

 	gearbox_plan.clear();
 	string line;

  	ifstream myfile ("GearBoxScheduler/resources/scheduler/out");
  	if (myfile.is_open()){
    	while ( getline (myfile,line) ){

    		stringstream   linestream(line);
    		string         data;
    		string jobid;
    		string allocationplan;
    		int j=1;
			while(getline(linestream, data, ':')){

				stringstream   linestream2(data);
    			string         alloc;
				if(j==1){
					jobid=data;
					gearbox_plan[jobid];
					sched_tasks[jobid]=0;
				}else if(j==2){
					allocationplan=data;
					while(getline(linestream2,alloc,' ')){
						gearbox_plan[jobid].push_back( atoi(alloc.c_str()) );
					}
				}
				++j;
			} 
		}
    	myfile.close();
  	}
	else cout << "Unable to open file"; 
	
}

//Function to load the simulator's output into the script to use as Gearbox feedback
//Receives as an argument the iteration number from 100 to 0
void ProcessSimulatorOutput(int it){
	
	string line;

  	ifstream myfile ("ClusterSimulator/experiment_results/output");
  	if (myfile.is_open()){
    	while ( getline (myfile,line) ){
			stringstream   linestream(line);
    		string data;
    		string jobid;
    		string jobtasks;
    		while(getline(linestream, data, ':')){
				jobid=data;
				linestream >> jobtasks;
				stringstream linestream2(jobtasks);
    			string data2;
    			int i=1;
    			//Load total, unscheduled and scheduled tasks into the variable
				while(getline(linestream2,data2,'-')){
					if(i==1)simulator_output[jobid]["total"]=atoi(data2.c_str());
					if(i==2)simulator_output[jobid]["sched"]=atoi(data2.c_str());
					if(i==3)simulator_output[jobid]["unsched"]=atoi(data2.c_str());
					++i;
				}

				//Update unscheduled tasks counter
				if(simulator_output[jobid]["sched"]>0){
					sched_tasks[jobid]=sched_tasks[jobid]+simulator_output[jobid]["sched"];
				}
			} 
		}
    	myfile.close();
  	}
	else cout << "Unable to open file"; 
}

/****************************************************************************************/
/*									MAIN FUNCTIONS  									*/
/****************************************************************************************/

int main(int argc, char *argv[]) {
	for(int i=slots;i>0;i--){
	
		//Update and run Gearbox every 10 iterations
		//It will be executed in iterations 100,90,80,70,60,50,40,30,20,10
		if(i%10==0){

			//Update Gearbox input with new PfEnd and new number of tasks
			if(i!=slots){
				//update gearbox input but not in the first iteration
				UpdateGearboxInput(i);
				cout<<"GEARBOX INPUT HAS BEEN UPDATED\n\n";
			}

			//Run Gearbox
			system("./GearBoxScheduler/src/scheduler GearBoxScheduler/resources/scheduler/config GearBoxScheduler/resources/scheduler/job_status GearBoxScheduler/resources/scheduler/out");
			cout<<"GEARBOX FINISHED RUNNING\n\n";

			//Load Gearbox allocation plan into variable gearbox_plan
			LoadAllocationPlan();
			cout<<"ALLOCATION PLAN LOADED\n\n";
		}
			
		//GET GEARBOX PLAN STEP FROM VARIABLE AND LOAD IT INTO SIMULATOR INPUT
		LoadGearboxOutputIntoSimInput(i);
		cout<<"ALLOCATION PLAN LOADED INTO SIMULATOR\n\n";
		
		//RUN SIMULATOR
		system("cd ClusterSimulator;bin/sbt run");
		cout<<"SIMULATOR FINISHED RUNNING\n\n";

		//READ SIMULATOR OUTPUT, UPDATE UNSCHEDULED TASKS COUNTER
		ProcessSimulatorOutput(i);		
		
	}
	//End of cycle
	return 0;
}