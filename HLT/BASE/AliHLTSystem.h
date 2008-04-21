//-*- Mode: C++ -*-
// @(#) $Id$

#ifndef ALIHLTSYSTEM_H
#define ALIHLTSYSTEM_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/** @file   AliHLTSystem.h
    @author Matthias Richter
    @date   
    @brief  Global HLT module management and AliRoot integration.
    @note   The class is used in Offline (AliRoot) context
*/

/**
 * @defgroup alihlt_system HLT integration into AliRoot
 * This section describes the HLT integration into AliRoot.
 */

#include "AliHLTLogging.h"
#include <TList.h>
#include <TString.h>

class AliHLTComponentHandler;
class AliHLTConfiguration;
class AliHLTConfigurationHandler;
class AliHLTTask;
class AliHLTOUT;
class AliRunLoader;
class AliRawReader;
class AliESDEvent;
class TObjArray;
class TStopwatch;

/**
 * @class AliHLTSystem
 * Main class for the HLT integration into AliRoot.
 * The class handles a list of configurations. Configurations are translated
 * into task lists which can be executed. 
 *
 * @note This class is only used for the @ref alihlt_system.
 *
 * @ingroup alihlt_system
 */
class AliHLTSystem : public AliHLTLogging {
 public:
  /** default constructor */
  AliHLTSystem();
  /** destructor */
  virtual ~AliHLTSystem();

  /**
   * Pointer to an instance of @ref AliHLTComponentHandler.
   */
  AliHLTComponentHandler* fpComponentHandler;                      //! transient

  /**
   * Pointer to an instance of @ref AliHLTConfigurationHandler.
   */
  AliHLTConfigurationHandler* fpConfigurationHandler;              //! transient

  /**
   * Add a configuration to the end of the list.
   * @param pConf    pointer to configuration to add
   */
  int AddConfiguration(AliHLTConfiguration* pConf);

  /**
   * Insert a configuration to the end of the list after the specified
   * configuration.
   * @param pConf    pointer to configuration to insert
   * @param pPrec    pointer to configuration to insert the new one after
   */
  int InsertConfiguration(AliHLTConfiguration* pConf, AliHLTConfiguration* pPrec);

  /**
   * Remove a configuration from the list.
   * @param pConf    pointer to configuration to delete
   */
  int DeleteConfiguration(AliHLTConfiguration* pConf);

  /**
   * Build a task list 
   * This method is used to build the tasks from the 'master' configuration
   * objects which are added to the HLT system handler. This is an iterative
   * process since the task might depend upon other configurations. For each
   * configuration object which has not yet been converted into a task, the
   * method will be called iteratively. Finally, after building all tasks which
   * the current one depends on have been created, the task is inserted to the
   * list of tasks with the InsertTask method.
   * @param pConf    configuration name/id
   */
  int BuildTaskList(const char* pConf);

  /**
   * Build task list from a configuration object.
   * This method is kept for backward compatibility. Use the version
   * with the configuration name.
   * @param pConf    pointer to configuration to build the task list from
   */
  int BuildTaskList(AliHLTConfiguration* pConf);

  /**
   * Clean the list of tasks and delete all the task objects.
   */
  int CleanTaskList();

  /**
   * Insert a task to the task list.
   * The method first checks whether all dependencies are resolved (i.e. exist 
   * already in the task list). During this iteration the cross links between
   * the tasks are set as well. If all dependencies are resolved, the task is
   * added at the end of the list.
   * @param pTask    pointer to task to add
   */
  int InsertTask(AliHLTTask* pTask);

  /**
   * Find a task with an id.
   * @param id       CONFIGURATION id (not a COMPONENT id!)
   */
  AliHLTTask* FindTask(const char* id);

  /**
   * Print the task list.
   */
  void PrintTaskList();

  /**
   * Run the task list.
   * The method checks whether the task list has already been build. If not,
   * or the configuration list has been changed, the @ref BuildTaskList
   * method is called.                                                    <br>
   * All tasks of the list will be subsequently processed for each event.
   * The system can remain started if the \em bStop parameter is 0. In that
   * case the system just waits for the next event. A specific call with
   * nofEvents==0 is needed to execute the stop sequence.
   * @param iNofEvents number of events
   * @param bStop      stop the chain after processing
   * @return number of reconstructed events, neg error code if failed
   */
  int Run(Int_t iNofEvents=1, int bStop=1);

  /**
   * Init all tasks from the list.
   * The @ref AliHLTTask::Init method is called for each task, the components
   * will be created.
   * @return neg error code if failed
   */
  int InitTasks();

  /**
   * Init the stopwatches for all tasks.
   * @param pStopwatches    object array of stopwatches, for types
   *                        @see AliHLTComponent::AliHLTStopwatchType
   * @return neg error code if failed
   */
  int InitBenchmarking(TObjArray* pStopwatches);

  /**
   * Print benchmarking summary.
   * Optionak: clean up stop watches.
   * @param pStopwatches    object array of stopwatches
   * @param bClean          delete stop watches if 1
   * @return neg error code if failed
   */
  int PrintBenchmarking(TObjArray* pStopwatches, int bClean=0);

  /**
   * Start task list.
   * The @ref AliHLTTask::StartRun method is called for each task, the
   * components will be prepared for event processing.
   * @return neg error code if failed
   */
  int StartTasks();

  /**
   * Process task list.
   * The @ref AliHLTTask::ProcessTask method is called for each task.
   * @return neg error code if failed
   */
  int ProcessTasks(Int_t eventNo);

  /**
   * Stop task list.
   * The @ref AliHLTTask::EndRun method is called for each task, the components
   * will be cleaned after event processing.
   * @return neg error code if failed
   */
  int StopTasks();

  /**
   * Send a control event trough the chain.
   * All data sources in the chain are switched to publish a control event like
   * SOR or EOR. The event is propagated in the same way as a normal event.
   * @param dt       type of the event
   */
  int SendControlEvent(AliHLTComponentDataType dt);

  /**
   * De-init all tasks from the list.
   * The @ref AliHLTTask::Deinit method is called for each task, the components
   * will be deleted.
   * @return neg error code if failed
   */
  int DeinitTasks();

  /**
   * The memory allocation function for components.
   * This function is part of the running environment of the components.
   */
  static void* AllocMemory( void* param, unsigned long size );

  /**
   * AliRoot embedded reconstruction.
   * Main entry point to execute the HLT reconstruction from AliRoot. Called
   * either by the AliHLTReconstructor plugin during AliRoot reconstruction
   * of raw data, or AliHLTSimulation during simulation of data.
   *
   * The two cases are distinguished by the availablility of the run loader
   * and raw reader.
   * - AliRoot simulation: run loader is available and is propagated to the
   *   module agents (AliHLTModuleAgent) to generate the corresponding
   *   configurations and chains, and to the AliHLTOfflineSource components.
   * - AliRoot reconstruction: raw reader is available and is propagated to
   *   the agents and AliHLTOfflineSource components.
   *
   * The system remains started after the processing and just waits for the
   * next event. A specific call with nofEvents==0 is needed to execute the
   * stop sequence.
   *
   * The 'runLoader' and 'rawReader' parameters are set to all active
   * AliHLTOfflineDataSource's and the HLT chain is processed for the given
   * number of events. If the rawReader is NULL, reconstruction is done on
   * simulated data, from real data if a RawReader is specified.
   * @param nofEvents     number of events
   * @param runLoader     the AliRoot runloader
   * @param rawReader     the AliRoot RawReader
   * @return number of reconstructed events, neg. error code if failed 
   */
  int Reconstruct(int nofEvents, AliRunLoader* runLoader, 
		  AliRawReader* rawReader=NULL);

  /**
   * Fill ESD for one event.
   * To be called by the AliHLTReconstructor plugin during the event loop
   * and FillESD method of the AliRoot reconstruction.
   *
   * The method is most likely deprecated as the scheme has been slightly
   * changed. The ESD is filled by the HLTOUT handlers u=implemented by the
   * HLT modules rather than by components within the reconstruction chain.
   * Still, HLT reconstruction chains can be run during the AliRoot
   * reconstruction, data produced by such chains is automatically added
   * to the HLTOUT stream in order to be equivalent to the online HLT.
   * The HLTOUT is processed during AliReconstruction at the end of the
   * HLT event processing, literally during the FillESD method of the AliRoot
   * reconstruction interface. The HLT module must implement HLTOUT handlers
   * and provide those through the module agent.
   *
   * This method is called on event basis, and thus must copy the previously
   * reconstructed data of the event from the 'ESD' recorder. The FillESD
   * method of all active AliHLTOfflineDataSink's is called.
   * @param eventNo       current event no (Note: this event number is just a
   *                      processing counter and is not related to the nature/
   *                      origin of the event
   * @param runLoader     the AliRoot runloader
   * @param esd           an AliESDEvent instance
   * @return neg. error code if failed 
   */
  int FillESD(int eventNo, AliRunLoader* runLoader, AliESDEvent* esd);

  /**
   * Process the HLTOUT data.
   */
  int ProcessHLTOUT(AliHLTOUT* pHLTOUT, AliESDEvent* esd);

  /**
   * Load component libraries.
   * @param libs          string of blank separated library names
   * @return neg. error code if failed 
   */
  int LoadComponentLibraries(const char* libs);

  /**
   * Find a symbol in a dynamically loaded library.
   * @param library      library
   * @param symbol       the symbol to find
   * @return void pointer to function
   */
  void* FindDynamicSymbol(const char* library, const char* symbol);

  /**
   * Prepare the HLT system for running.
   * - module agents are requested to register configurations
   * - task lists are built from the top configurations of the modules
   *
   * @param rawReader    instance of the raw reader or NULL
   * @param runloader    optional instance of the run loader
   * @return neg. error code if failed <br>
   *         -EBUSY      system is in kRunning state <br>
   */
  int Configure(AliRawReader* rawReader, AliRunLoader* runloader=NULL);

  /**
   * Old method kept for backward compatibilty.
   *
   * @param runloader    optional instance of the run loader
   * @return neg. error code if failed <br>
   *         -EBUSY      system is in kRunning state <br>
   */
  int Configure(AliRunLoader* runloader=NULL);

  /**
   * Scan options.
   * Known options
   * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formating -->
   * \li loglevel=<i>level</i> <br>
   *     logging level for this processing
   * \li frameworklog=<i>level</i> <br>
   *     logging level for framework classes
   * \li alilog=off
   *     disable redirection of log messages to AliLog class
   * \li config=<i>macro</i>
   *     configuration macro
   * \li chains=<i>configuration</i>
   *     comma separated list of configurations to be run during local
   *     reconstruction
   * \li libmode=<i>static,dynamic(default)</i>
   *     libraries are persistent if loaded in mode <i>static</i>, i.e. they
   *     can't be unloaded
   */
  int ScanOptions(const char* options);

  /**
   * Reset the HLT system.
   * Reset is not possible while the system is in running state.
   * @param bForce       force the reset
   * @return neg. error code if failed <br>
   *         -EBUSY      system is in kRunning state <br>
   */
  int Reset(int bForce=0);

  /**
   * Load the configurations specified by the module agents.
   * The runLoader is passed to the agent and allows configuration
   * selection.
   * @param rawReader    instance of the raw reader or NULL
   * @param runloader    optional instance of the run loader
   * @return neg. error code if failed 
   */
  int LoadConfigurations(AliRawReader* rawReader, AliRunLoader* runloader=NULL);

  /**
   * Get the top configurations of all agents and build the task lists.
   * @param rawReader    instance of the raw reader or NULL
   * @param runloader    optional instance of the run loader
   * @return neg. error code if failed 
   */
  int BuildTaskListsFromReconstructionChains(AliRawReader* rawReader, 
					     AliRunLoader* runloader=NULL);

  enum AliHLTSystemState {
    kUninitialized       = 0x0,
    kLibrariesLoaded     = 0x1,
    kConfigurationLoaded = 0x2,
    kTaskListCreated     = 0x4,
    kReady               = 0x7,
    kStarted             = 0x8,
    kRunning             = 0x10,
    kError               = 0x1000
  };

  /**
   * Check status of the system.
   * @param flag          AliHLTSystemState value to check for
   * @return 1 if set, 0 if not
   */
  int CheckStatus(int flag);

  /**
   * Get the current status.
   * @return status flags of @ref AliHLTSystemState
   */
  int GetStatusFlags();

  /**
   * Set logging level for framework classes.
   * This sets the local logging level of this instance and all subsequent
   * framework classes to \em level.
   * @param level     local logging level for the framework classes
   */
  void SetFrameworkLog(AliHLTComponentLogSeverity level);

 protected:
 
 private:
  /** copy constructor prohibited */
  AliHLTSystem(const AliHLTSystem&);
  /** assignment operator prohibited */
  AliHLTSystem& operator=(const AliHLTSystem&);

  /**
   * Set status flags.
   */
  int SetStatusFlags(int flags);

  /**
   * clear status flags.
   */
  int ClearStatusFlags(int flags);

/*   TList fConfList; */
/*   int fbListChanged; */

  /** list of tasks */
  TList fTaskList;                                                 // see above

  /** the number of instances of AliHLTSystem */
  static int fgNofInstances;                                       // see above

  /** state of the object */
  int fState;                                                      // see above

  /** chains to be run during reconstruction */
  TString fChains;                                                 //!transient

  /** array of stopwatches */
  TObjArray* fStopwatches;                                         //!transient

  /** number of events processed in total */                        
  int fEventCount;                                                 //!transient

  /** number of events processed successfully */
  int fGoodEvents;                                                 //!transient

  /** switch whether to write to the global hltesd or not
   *  since this causes a crash outside the HLT code this
   *  can be enabled by the 'globalesd' argument
   */
  bool bWriteGlobalEsd;                                            //!transient

  /** array of default libraries */
  static const char* fgkHLTDefaultLibs[];                          //!transient

  ClassDef(AliHLTSystem, 6);
};

#endif
