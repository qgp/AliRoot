// -*- Mode: C++ -*-
// $Id$

#ifndef ALIHLTCOMPSTATCOLLECTOR_H
#define ALIHLTCOMPSTATCOLLECTOR_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/// @file   AliHLTCompStatCollector.h
/// @author Matthias Richter
/// @date   
/// @brief  Collector component for the component statistics information.
///

#include "AliHLTProcessor.h"
#include <ctime>
#include <map>

class TStopwatch;
class TH1F;
class TH2F;
class TH2C;
class TTree;
class TFolder;
class TFile;

/**
 * @class AliHLTCompStatCollector
 * Collector component for the statistics entries produced by the
 * AliHLTComponent base class.
 *
 * <h2>General properties:</h2>
 * This components collects all data blocks of types
 * ::kAliHLTDataTypeComponentStatistics and ::kAliHLTDataTypeComponentTable
 * which can be produced by the AliHLTComponent base class for every component
 * and event. Component statistics entries are data blocks of
 * ::AliHLTComponentStatistics arrays containing a couple of informations
 * about each component. The information is extracted and stored into a
 * TTree. The component table entries (AliHLTComponentTable structs) are sent
 * on SOR and EOR events and are arranged in a TFolder hierarchy.
 *
 * The objects are published or/and saved according to the setup. An
 * event modulo marameter can be used to publish every nth event, a period
 * argument to publish every nth second. The objects can be optionally saved
 * directly to file and the publishing can be suppressed. The objects are
 * published/saved at the EOR event.
 *
 * Component ID: \b StatisticsCollector                                 <br>
 * Library: \b libAliHLTUtil.so					        <br>
 * Input Data Types: ::kAliHLTDataTypeComponentStatistics                 <br>
 * Output Data Types: ::kAliHLTDataTypeHistogram, ::kAliHLTDataTypeTNtuple  <br>
 *
 * <h2>Mandatory arguments:</h2>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 *
 * <h2>Optional arguments:</h2>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 * \li -file     <i> filename   </i> <br>
 *      name of root file
 * \li -publish     <i> 0/1   </i> <br>
 *      enable/disable publishing to HLT output, default is on
 * \li -period     <i> n   </i> <br>
 *      publish/save every n-th second
 * \li -modulo     <i> n   </i> <br>
 *      publish/save every n-th event
 *
 * <h2>Configuration:</h2>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 * Configuration by component arguments.
 *
 * <h2>Default CDB entries:</h2>
 * The component loads no CDB entries.
 *
 * <h2>Performance:</h2>
 *
 * <h2>Memory consumption:</h2>
 *
 * <h2>Output size:</h2>
 *
 * @ingroup alihlt_util_components
 */
class AliHLTCompStatCollector : public AliHLTProcessor
{
 public:
  /** standard constructor */
  AliHLTCompStatCollector();
  /** destructor */
  virtual ~AliHLTCompStatCollector();

  const char* GetComponentID() {return "StatisticsCollector";};
  AliHLTComponent* Spawn() {return new AliHLTCompStatCollector;}
  void GetInputDataTypes( vector<AliHLTComponentDataType>& );
  AliHLTComponentDataType GetOutputDataType();
  int GetOutputDataTypes(AliHLTComponentDataTypeList& tgtList);
  void GetOutputDataSize( unsigned long& constBase, double& inputMultiplier );

  /**
   * 
   */
  class AliHLTCompStatInstance {
  public:
    AliHLTCompStatInstance();
    AliHLTCompStatInstance(AliHLTUInt32_t CRCId,
			   const char* chainId,
			   const char* componentId,
			   const char* componentParam,
			   const vector<AliHLTUInt32_t>& parents,
			   const int level=-1,
			   const int tag=-1);
    AliHLTCompStatInstance(AliHLTUInt32_t CRCId,
			   const string& chainId,
			   const string& componentId,
			   const string& componentParam,
			   const vector<AliHLTUInt32_t>& parents,
			   const int level=-1,
			   const int tag=-1);
    AliHLTCompStatInstance(const AliHLTCompStatInstance& src);
    AliHLTCompStatInstance& operator=(const AliHLTCompStatInstance& src);

    ~AliHLTCompStatInstance();

    AliHLTUInt32_t GetCRCId() const {return fCRCId;}
    const string&  GetChainId() const {return fChainId;}
    const string&  GetComponentId() const {return fComponentId;}
    const string&  GetComponentParam() const {return fComponentParam;}
    const vector<AliHLTUInt32_t>& GetParents() const {return fParents;}
    int GetLevel() const {return fLevel;}
    int GetTag() const {return fTag;}
    int SetTag(int tag) {fTag=tag; return fTag;}
    bool IsProcessed() const {return fProcessed;}
    void MarkProcessed() {fProcessed=true;}
    void ResetProcessed() {fProcessed=false;}

    void Print(const char* option="") const;
    string Description(const char* option="") const;

    bool operator==(const AliHLTCompStatInstance &b) const;
    bool operator!=(const AliHLTCompStatInstance &b) const {return !(*this==b);}

    /// helper function for sort, like operator<
    static bool SortByLevelAndComponentId(const AliHLTCompStatInstance &a,
					  const AliHLTCompStatInstance &b);

  private:
    AliHLTUInt32_t fCRCId;           /// CRC id of this entry
    string fChainId;                 /// chain id string
    string fComponentId;             /// component id string
    string fComponentParam;          /// component arguments
    vector<AliHLTUInt32_t> fParents; /// list of parent chain ids
    int fLevel;                      /// level in the reconstruction chain
    int fTag;                        /// tag for recording
    bool fProcessed;                 /// marks instance as processed
  };

  void Print(const char* option) const;

 protected:
  int DoInit( int argc, const char** argv );
  int DoDeinit();
  int DoEvent( const AliHLTComponentEventData& evtData, AliHLTComponentTriggerData& trigData);
  
  using AliHLTProcessor::DoEvent;

  /** mode definition */
  enum {
    /** publish objects according to the given period */
    kPublishObjects = 0x1,

    /** save objects according to the given period */
    kSaveObjects = 0x2
  };

 private:
  /** not a valid copy constructor, defined according to effective C++ style */
  AliHLTCompStatCollector(const AliHLTCompStatCollector&);
  /** not a valid assignment op, but defined according to effective C++ style */
  AliHLTCompStatCollector& operator=(const AliHLTCompStatCollector&);

  /**
   * Reset all filling variables and lists.
   */
  void ResetFillingVariables();

  /**
   * Fill the lists from the component statistics block.
   */
  int FillVariablesSorted(void* ptr, int size, AliHLTUInt32_t eventType);

  /** allocate the statistics tree and the branch arrays */
  int AllocateStatTree(AliHLTUInt32_t size);

  /** delete all internal objects */
  void ClearAll();
  /** clear the statistics tree and the branch arrays */
  void ClearStatTree();

  /**
   * Remove entries from the parent list if they occur further down in the
   * hierarchy.
   */
  int RemoveRecurrence(TFolder* pRoot) const;

  /**
   * Check event modulo and time period.
   * If the result is true, the internal counter and time backup is
   * updated enabled
   * @param bUpdate   update internal backups if condition was true
   * @return true if period has excceded
   */
  bool CheckPeriod(bool bUpdate=true);

  /** event cycle timer */
  TStopwatch* fpTimer; //!transient

  /** top folder */
  TFolder* fpFolder; //!transient
  /** map between crc id and instance description */
  map<AliHLTUInt32_t, AliHLTCompStatInstance> fInstances;

  /** statistics tree */
  TTree* fpStatTree; //!transient

  /** branch filling variable */
  Float_t fCycleTime; //!transient
  /** branch filling variable */
  Int_t fNofSets; //!transient
  /** array size */
  UInt_t fArraySize; //!transient
  /** current position */
  UInt_t fPosition; //!transient
  /** branch filling variable */
  UInt_t* fpLevelArray; //!transient
  /** branch filling variable */
  UInt_t* fpSpecArray; //!transient
  /** branch filling variable */
  UInt_t* fpBlockNoArray; //!transient
  /** branch filling variable */
  UInt_t* fpIdArray; //!transient
  /** branch filling variable */
  UInt_t* fpTimeArray; //!transient
  /** branch filling variable */
  UInt_t* fpCTimeArray; //!transient
  /** branch filling variable */
  UInt_t* fpInputBlockCountArray; //!transient
  /** branch filling variable */
  UInt_t* fpTotalInputSizeArray; //!transient
  /** branch filling variable */
  UInt_t* fpNormalizedInputSizeArray; //!transient
  /** branch filling variable */
  UInt_t* fpOutputBlockCountArray; //!transient
  /** branch filling variable */
  UInt_t* fpTotalOutputSizeArray; //!transient
  /** branch filling variable */
  UInt_t* fpInputOutputRatioArray; //!transient
  /** branch filling variable */
  UInt_t* fpNormalizedInputOutputRatioArray; //!transient
  /** branch filling variable */
  UInt_t* fpComponentCycleTimeArray; //!transient
  /** branch filling variable */
  UInt_t* fpEventTypeArray; //!transient
  /** branch filling variable */
  UInt_t* fpEventCountArray; //!transient

  /** const base of GetOutputSize, updated on error in DoEvent */
  int fSizeEstimator; //! transient

  /** mode flags */
  unsigned int fMode; //! transient

  /** file name to store the objects */
  string fFileName; //! transient

  /** root file to save objects */
  TFile* fFile; // !transient

  /** last time, objects have been published or saved */
  time_t fLastTime; //! transient

  /** period in seconds to save/publish objects */
  unsigned int fPeriod; //! transient

  /** event modulo to save/publish onjects */
  unsigned int fEventModulo; //! transient

  ClassDef(AliHLTCompStatCollector, 0)
};
#endif
