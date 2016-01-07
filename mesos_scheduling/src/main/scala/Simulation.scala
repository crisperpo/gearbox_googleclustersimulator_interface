/*Import Libraries*/

import org.scalatest.FunSuite
import ClusterSchedulingSimulation.Workload
import ClusterSchedulingSimulation.WorkloadDesc
import ClusterSchedulingSimulation.Job
import ClusterSchedulingSimulation.UniformWorkloadGenerator
import ClusterSchedulingSimulation.CellState
import ClusterSchedulingSimulation.ClusterSimulator
import ClusterSchedulingSimulation.MonolithicScheduler
import ClusterSchedulingSimulation.MesosSimulator
import ClusterSchedulingSimulation.MesosScheduler
import ClusterSchedulingSimulation.MesosAllocator
import ClusterSchedulingSimulation.ClaimDelta
import ClusterSchedulingSimulation.OmegaSimulator
import ClusterSchedulingSimulation.OmegaScheduler
import ClusterSchedulingSimulation.PrefillPbbTraceWorkloadGenerator
import ClusterSchedulingSimulation.InterarrivalTimeTraceExpExpWLGenerator
import collection.mutable.HashMap
import collection.mutable.ListBuffer
import sys.process._

import ClusterSchedulingSimulation.Seed
import ClusterSchedulingSimulation.ClusterSimulator
import ClusterSchedulingSimulation.ExpExpExpWorkloadGenerator
import ClusterSchedulingSimulation.Experiment
import ClusterSchedulingSimulation.Job
import ClusterSchedulingSimulation.SchedulerDesc
import ClusterSchedulingSimulation.Workload
import ClusterSchedulingSimulation.WorkloadDesc
import ClusterSchedulingSimulation.Workloads._
import ClusterSchedulingSimulation.MonolithicScheduler
import ClusterSchedulingSimulation.MonolithicSimulatorDesc
import ClusterSchedulingSimulation.MesosSchedulerDesc
import ClusterSchedulingSimulation.MesosSimulatorDesc
import ClusterSchedulingSimulation.OmegaSchedulerDesc
import ClusterSchedulingSimulation.OmegaSimulatorDesc
import java.io.File
import java.io.FileInputStream
import java.io.FileOutputStream
import java.nio.channels.FileChannel
import ca.zmatrix.utils._
import scala.collection.mutable.ArrayBuffer
import scala.collection.immutable.NumericRange

object Simulation {

	def main (args: Array[String]) {

    /***************************************************************************************/
    //                                CREATE WORKLOAD	
		/***************************************************************************************/
    //Create workload  from init file
    /***************************************************************************************/
  
    val filename = "traces/example-init-cluster-state.log"
    val batchPrefillTraceWLGenerator = new PrefillPbbTraceWorkloadGenerator(
        "PrefillBatch", filename)
    val prefillBatchWL = batchPrefillTraceWLGenerator.newWorkload(1000.0)  
    val numBatchJobsInFile
      = Seq("awk",
            "$1 == 11 && ($4 != 1 || $5 == 0 || $5 == 1)",
            filename)
           .!!.split("\n").length
    

    /***************************************************************************************/
    //                                CREATE SCHEDULERS
    /***************************************************************************************/
    //    Mesos Scheduler
    /***************************************************************************************/
    // Create a simple scheduler, turn on partial job scheduling
    // So that we can check our think time calculations by just
    // assuming that each job only has to be scheduled once.
    val mesos_scheduler = new MesosScheduler(name = "mesos_test_sched",
                                       constantThinkTimes = Map("PrefillBatch" -> 1),
                                       perTaskThinkTimes = Map("PrefillBatch" -> 1),
                                       schedulePartialJobs = true)

    /***************************************************************************************/
    //                                   CREATE CELLSTATE
    /***************************************************************************************/
    //    Mesos CellState
    /***************************************************************************************/
    // Set up a CellState with plenty of space so that no jobs left 
    // It shouldn't matter which transactionMode we choose, but it does
    // matter that we use "resource-fit" conflictMode or else
    // responses to resource offers will likely fail.
    // Mesos requires cellstate to be set up with resource-fit conflictMode"
    // 1 machines, 100 cpus per machine, 400 mem per machine
    // set transaction to incremental instead all-or-nothing
    val mesosCellState = new CellState(1, 100.0, 400.0,
                                       conflictMode = "resource-fit",
                                       transactionMode = "incremental")
    
    /***************************************************************************************/
    //                                CREATE SIMULATOR
    /***************************************************************************************/
    //    Mesos Simulator
    /***************************************************************************************/
    // Create a round-robin allocator
    val allocatorConstantThinkTime = 1.0
    val mesosDRFAllocator = new MesosAllocator(allocatorConstantThinkTime)

    val mesosSimulator = new MesosSimulator(
          mesosCellState,
          Map(mesos_scheduler.name -> mesos_scheduler),
          Map("PrefillBatch" -> Seq("mesos_test_sched")),
          List(prefillBatchWL),
          List(),
          mesosDRFAllocator,
          logging = true,
          monitorUtilization = false)

    /***************************************************************************************/
    //                                  RUN SIMULATOR
    /***************************************************************************************/
    /*
   * Run the simulation for {@code runTime} virtual (i.e., simulated)
   * seconds or until {@code wallClockTimeout} seconds of execution
   * time elapses.
   */
   
   /***************************************************************************************/
   //    RUN SIMULATOR
   /***************************************************************************************/
   //    Mesos Run
   /***************************************************************************************/
   
   val globalRunTime = 10.0
   mesosSimulator.run(Some(globalRunTime),None)
   
   prefillBatchWL.getJobs.foreach(job => {
      println(("Job %d:\n Tasks:%d, UnschedTasks:%d, SchedTasks:%d\n").format(job.id,job.numTasks,job.unscheduledTasks,job.numTasks-job.unscheduledTasks))
    })

  /***************************************************************************************/
  //     MAKE EXPERIMENTS FOLDER AND OUTPUT FILE
  /***************************************************************************************/
  val formatter = new java.text.SimpleDateFormat("yyyy-MM-dd")
  val dateTimeStamp = formatter.format(new java.util.Date)
  val formatterIt = new java.text.SimpleDateFormat("yyyy-MM-dd-HH-mm-ss")
  val dateTimeStampIt = formatterIt.format(new java.util.Date)


  val experName="EXPERIMENT2"
  val experDir = new java.io.File("experiment_results")
  if (!experDir.exists) {
      experDir.mkdir()
  }
  
  val outputDirName = "%s/%s-%s-RUNTIME%.0f"
                        .format(
                        experDir.toString,
                        dateTimeStampIt,
                        experName,
                        mesosSimulator.finalruntime)

  val outputDir = new java.io.File(outputDirName)
  if (!outputDir.exists) {
      outputDir.mkdir()
  }
  //Create temporary output file
  val tempoutputFileName = "%s/output"
                        .format(
                        experDir.toString
                        )
  val tempexpOutput = new java.io.PrintWriter(new File(tempoutputFileName))
  prefillBatchWL.getJobs.foreach(job => {
      tempexpOutput.println(("%d:%d-%d-%d").format(job.id,job.numTasks,job.numTasks-job.unscheduledTasks,job.unscheduledTasks))
  })
  tempexpOutput.close

  //Create output file
  val outputFileName = "%s/%s-output"
                        .format(
                        outputDir.toString,
                        dateTimeStampIt
                        )
  val expOutput = new java.io.PrintWriter(new File(outputFileName))
  
  //Fill output file
  prefillBatchWL.getJobs.foreach(job => {
      expOutput.println(("%d:%d-%d-%d").format(job.id,job.numTasks,job.numTasks-job.unscheduledTasks,job.unscheduledTasks))
  })
  expOutput.close

  /***************************************************************************************/
  //COPY OF THE INPUT File in the experiments_results folder
  /***************************************************************************************/
  
  val settingsFileName = "example-init-cluster-state.log"
    val sourceFile = new File("traces/" + settingsFileName)
    val destFile = new File(outputDirName + "/"+dateTimeStampIt +
                            "-input")
    // Create the output directory if it doesn't exist.
    (new File(outputDirName)).mkdirs()
    if(!destFile.exists()) {
        destFile.createNewFile();
    }
    var source: FileChannel = null
    var destination: FileChannel = null

    try {
        source = new FileInputStream(sourceFile).getChannel();
        destination = new FileOutputStream(destFile).getChannel();
        destination.transferFrom(source, 0, source.size());
    }
    finally {
        if(source != null) {
            source.close();
        }
        if(destination != null) {
            destination.close();
        }
    }

	}
}