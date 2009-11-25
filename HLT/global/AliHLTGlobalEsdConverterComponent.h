//-*- Mode: C++ -*-
// $Id$
#ifndef ALIHLTGLOBALESDCONVERTERCOMPONENT_H
#define ALIHLTGLOBALESDCONVERTERCOMPONENT_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/** @file   AliHLTGlobalEsdConverterComponent.h
    @author Matthias Richter
    @date   
    @brief  Global ESD converter component.
*/

#include "AliHLTProcessor.h"
#include <vector>

// forward declarations
class AliESDEvent;
class TTree;
struct AliHLTTracksData;

/**
 * @class AliHLTGlobalEsdConverterComponent
 * Global collector for information designated for the HLT ESD.
 *
 * componentid: \b GlobalEsdConverter <br>
 * componentlibrary: \b libAliHLTGlobal.so <br>
 * Arguments: <br>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 * \li -notree                                                          <br>
 *      write ESD directly to output (::kAliHLTDataTypeESDObject)
 *      this has been made the default behavior in Sep 2008.
 * \li -tree                                                            <br>
 *      write ESD directly to TTree and to output (::kAliHLTDataTypeESDTree)
 *
 * @ingroup alihlt_tpc_components
 */
class AliHLTGlobalEsdConverterComponent : public AliHLTProcessor
{
 public:
  /** standard constructor */
  AliHLTGlobalEsdConverterComponent();
  /** destructor */
  virtual ~AliHLTGlobalEsdConverterComponent();

  // interface methods of base class
  const char* GetComponentID() {return "GlobalEsdConverter";};
  void GetInputDataTypes(AliHLTComponentDataTypeList& list);
  AliHLTComponentDataType GetOutputDataType();
  void GetOutputDataSize(unsigned long& constBase, double& inputMultiplier);
  AliHLTComponent* Spawn() {return new AliHLTGlobalEsdConverterComponent;}

 protected:
  // interface methods of base class
  int DoInit(int argc, const char** argv);
  int DoDeinit();
  int DoEvent( const AliHLTComponentEventData& evtData,
	       AliHLTComponentTriggerData& trigData);

  using AliHLTProcessor::DoEvent;

  /**
   * Process the input data blocks.
   * @param pTree    tree to be filled
   * @param pESD     ESD to be filled
   * @return neg. error code if failed
   */
  int ProcessBlocks(TTree* pTree, AliESDEvent* pESD);

 private:
  /** copy constructor prohibited */
  AliHLTGlobalEsdConverterComponent(const AliHLTGlobalEsdConverterComponent&);
  /** assignment operator prohibited */
  AliHLTGlobalEsdConverterComponent& operator=(const AliHLTGlobalEsdConverterComponent&);

  /**
   * (Re)Configure from the CDB
   * Loads the following objects:
   * - HLT/ConfigHLT/SolenoidBz
   */
  int Reconfigure(const char* cdbEntry, const char* chainId);

  /**
   * Configure the component.
   * Parse a string for the configuration arguments and set the component
   * properties.
   */
  int Configure(const char* arguments);

  /// write object to TTree or directly
  int fWriteTree; //!transient

  /// verbosity level
  int fVerbosity; //!transient

protected:

  /// the ESD
  AliESDEvent* fESD; //! transient value

  /// solenoid b field
  Double_t fSolenoidBz; //! transient

  ClassDef(AliHLTGlobalEsdConverterComponent, 0)
};
#endif
