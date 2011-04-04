//-*- Mode: C++ -*-
// $Id$
#ifndef ALIHLTITSCLUSTERFINDERCOMPONENT_H
#define ALIHLTITSCLUSTERFINDERCOMPONENT_H

//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/// @file   AliHLTITSClusterFinderComponent.cxx
/// @author Gaute Ovrebekk <st05886@alf.uib.no>
/// @date   
/// @brief  Component to run the offline clusterfinder.
///

#include "AliHLTProcessor.h"
#include "AliRawReaderMemory.h"
#include "AliITSDetTypeRec.h"
#include "AliITSgeom.h"
#include "AliITSInitGeometry.h"
#include "TClonesArray.h"
#include "AliHLTDataTypes.h"
#include "TTree.h"
#include "AliHLTComponentBenchmark.h"
#include "AliITSRecPoint.h"

class AliHLTITSClusterFinderSPD;
class AliHLTITSClusterFinderSSD;
class AliHLTITSClusterData;
class AliLoader;

/**
 * @class AliHLTITSClusterFinderComponent
 * HLT Component to run the ITS offline clusterfinders.
 *
 * <h2>General properties:</h2>
 *
 * Component ID: \b ITSClusterFinderSPD or ITSClusterFinderSDD or ITSClusterFinderSSD <br>
 * Library: \b libAliHLTITS.so                              <br>
 * Input Data Types:                                        <br> 
 *    kAliHLTDataTypeDDLRaw|kAliHLTDataOriginITSSPD or kAliHLTDataTypeDDLRaw|kAliHLTDataOriginITSSDD or kAliHLTDataTypeDDLRaw|kAliHLTDataOriginITSSD <br>
 *      
 * Output Data Types:                                       <br>
 *    kAliHLTDataTypeClusters|kAliHLTDataOriginITSSPD or kAliHLTDataTypeClusters|kAliHLTDataOriginITSSDD or kAliHLTDataTypeClusters|kAliHLTDataOriginITSSSD <br>
 *
 * <h2>Mandatory arguments:</h2>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 *
 * <h2>Optional arguments:</h2>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 *
 * <h2>Configuration:</h2>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 * \li -config1      <i> teststring   </i> <br>
 *      a configuration argument with one parameter
 * \li -config2                            <br>
 *      a configuration argument without parameters
 *
 * <h2>Default CDB entries:</h2>
 * ITS/Calib/SPDNoisy
 * ITS/Calib/SPDDead
 * TRIGGER/SPD/PITConditions
 * ITS/Calib/CalibSDD
 * ITS/Calib/RespSDD
 * ITS/Calib/DriftSpeedSDD
 * ITS/Calib/DDLMapSDD
 * ITS/Calib/MapsTimeSDD
 * ITS/Calib/NoiseSSD
 * ITS/Calib/GainSSD
 * ITS/Calib/BadChannelsSSD
 * GRP/CTP/Scalers
 *
 * <h2>Performance:</h2>
 * TODO
 *
 * <h2>Memory consumption:</h2>
 * TODO
 *
 * <h2>Output size:</h2>
 * TODO
 *
 * <h2>Pending issues:</h2>
 * There is a general problem with the streaming of the digit tree. A description
 * is in the GetEvent function. For many events the hotfix for that caused bug
 * https://savannah.cern.ch/bugs/?72815
 * This has been fixed by accessing the AliLoader in the CF component via the
 * global AliRunLoader. This breaks the idea of components, however it's used in
 * simulation inside AliRoot only.
 *
 * @ingroup alihlt_its_components
 */
class AliHLTITSClusterFinderComponent : public AliHLTProcessor
{
 public:
  /**
   * Defines for selecting clusterfinder for SPD, SDD or SSD.
   */
  enum {
    kClusterFinderSPD,
    kClusterFinderSDD,
    kClusterFinderSSD,
    kClusterFinderDigits    
  };
  /*
   * ---------------------------------------------------------------------------------
   *                            Constructor / Destructor
   * ---------------------------------------------------------------------------------
   */
  
  /** constructor
   *  @param mode    input type see e.g. @ref kClusterFinderSPD
   */
  AliHLTITSClusterFinderComponent(int mode);

  /** destructor */
  virtual ~AliHLTITSClusterFinderComponent();

  /*
   * ---------------------------------------------------------------------------------
   * Public functions to implement AliHLTComponent's interface.
   * These functions are required for the registration process
   * ---------------------------------------------------------------------------------
   */

  /** interface function, see @ref AliHLTComponent for description */
  const char* GetComponentID();

  /** interface function, see @ref AliHLTComponent for description */
   void GetInputDataTypes( vector<AliHLTComponentDataType>& list);

  /** interface function, see @ref AliHLTComponent for description */
  AliHLTComponentDataType GetOutputDataType();

  /** interface function, see @ref AliHLTComponent for description */
  virtual void GetOutputDataSize( unsigned long& constBase, double& inputMultiplier );

  /** interface function, see @ref AliHLTComponent for description */
  void GetOCDBObjectDescription( TMap* const targetMap);

  /** interface function, see @ref AliHLTComponent for description */
  AliHLTComponent* Spawn();

 protected:
	
  /*
   * ---------------------------------------------------------------------------------
   * Protected functions to implement AliHLTComponent's interface.
   * These functions provide initialization as well as the actual processing
   * capabilities of the component. 
   * ---------------------------------------------------------------------------------
   */
	
  /** Initialization */
  Int_t DoInit( int argc, const char** argv );

  /** DeInitialization */
  Int_t DoDeinit();
  
  /** EventLoop */
 
  Int_t DoEvent(
		const AliHLTComponentEventData& evtData,
		const AliHLTComponentBlockData* blocks,
		AliHLTComponentTriggerData& /*trigData*/,
		AliHLTUInt8_t* outputPtr,
		AliHLTUInt32_t& size,
		vector<AliHLTComponentBlockData>& outputBlocks );

  //Int_t DoEvent( const AliHLTComponentEventData& /*evtData*/, AliHLTComponentTriggerData& /*trigData*/);

  int Reconfigure(const char* cdbEntry, const char* chainId);

  using AliHLTProcessor::DoEvent;
 
  ///////////////////////////////////////////////////////////////////////////////////
    
 private:
  /** standard constructor prohibited */
  AliHLTITSClusterFinderComponent();
  /** copy constructor prohibited */
  AliHLTITSClusterFinderComponent(const AliHLTITSClusterFinderComponent&);
  /** assignment operator prohibited */
  AliHLTITSClusterFinderComponent& operator=(const AliHLTITSClusterFinderComponent&);
  /**
   * Configure the component.
   * Parse a string for the configuration arguments and set the component
   * properties.
   */
  int Configure(const char* arguments);

  void RecPointToSpacePoint(AliHLTUInt8_t* outputPtr,AliHLTUInt32_t& size);
  void RecpointToOutput(AliHLTITSClusterData *outputClusters, AliITSRecPoint *recpoint, int &clustIdx);
  /*
   * ---------------------------------------------------------------------------------
   *                             Members - private
   * ---------------------------------------------------------------------------------
   */

  /**
   * switch to indicated the ClusterFinder
   * use fModeSwitch = 0 for SPD
   * use fModeSwitch = 1 for SDD
   * use fModeSwitch = 2 for SSD
   * use fModeSwitch = 3 for ClusterFinding on Digits (Full ITS)
   */
  Int_t fModeSwitch;      // !
  AliHLTComponentDataType fInputDataType; // !
  AliHLTComponentDataType fOutputDataType; // !
  
  Bool_t fUseOfflineFinder; // flag to use the offline clusterfinder
  Int_t fNModules;             // total number of modules
  Int_t fId;                   // ddl offset
  Int_t fNddl;                 // number of ddl's
  
  /** the reader object for data decoding */

  AliRawReaderMemory* fRawReader;                             //!transient
  AliITSDetTypeRec* fDettype;                                 //!transient
  AliITSgeom* fgeom;                                          //!transient
  AliITSInitGeometry* fgeomInit;                              //!transient
 
  AliHLTITSClusterFinderSPD *fSPD;                            //!transient
  AliHLTITSClusterFinderSSD *fSSD;                            //!transient

  TTree *tD;                                                  //!transient
  TTree *tR;                                                  //!transient

  Int_t fSPDNModules;                                         //!transient 
  Int_t fSDDNModules;                                         //!transient
  Int_t fSSDNModules;                                         //!transient 
  
  Int_t fFirstModule;                                         //!transient    
  Int_t fLastModule;                                          //!transient
  Int_t fnClusters;                                           //!transient

  std::vector<AliITSRecPoint> fclusters;                      //!transient

  AliHLTComponentBenchmark fBenchmark;// benchmark

  unsigned long fOutputSizeOffset; //! const offset for output size estimation
  // 2011-01-27: the transport of digits via the AliLoaderPublisher is
  // not working stably at the moment. In particular the size of the
  // streamed digit tree seems not to be correlated with the content
  // furthermore the CF implements a hotfix accessing the root file
  // directly. The variable fInputMultiplierDigits will be used for a
  // dynamic adaption of the buffer size. The component will then return
  // -ENOSPC and AliHLTTask carries out the event processing once again
  // with adjusted buffer size
  float fInputMultiplierDigits; //! variable input multiplier for CF type digits
  AliLoader* fpLoader; //! ITS loader for getting digit tree

  ClassDef(AliHLTITSClusterFinderComponent, 0)
    
};
#endif
