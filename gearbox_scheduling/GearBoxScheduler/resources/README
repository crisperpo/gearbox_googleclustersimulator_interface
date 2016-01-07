Author: Luo Mai
Date: 5th June 2013

1. Prerequisites for compiling the scheduler
-- boost library 1.53
-- cplex 12.4
After installing these two libraries, change the corresponding directories in the resources/MakeFile so that g++ know the correct paths to
the libraries.

2. Running the scheduler
This can be easily inferred from the main.cpp
A simple example is given in the resources/example folder to give a sense how the scheduler works.

3. What is jobStatus file?
jobStatus file contains the status of the running jobs and the new arrival jobs. The scheduler
computes a resource allocation plan depending on the current status of all the jobs.
The file use the following format: (rf is the abbrevation is the return function, and bf is the budget function):

jobId, jobSize, startTime, pfBegin, pfEnd, pfSlope, pfIntercept

where the jobId is the job identifier, startTime is the time when the job enters the DC, pfBegin and pfEnd depict the soft and hard deadlines,
pfSlope and the pfIntercept gives the slope and the intercept of the pricing function (reference to the hotcloud paper).
