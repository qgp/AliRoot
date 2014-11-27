#ifndef ALIHLTMUONCLUSTERWRITERCOMPONENT_H
#define ALIHLTMUONCLUSTERWRITERCOMPONENT_H

/* This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

// $Id$

///
///  @file   AliHLTMUONClusterWriterComponent.h
///  @author Philippe Pillot <pillot@subatech.in2p3.fr>
///  @date   23 October 2014
///  @brief  cluster writer sink component for the dimuon HLT.
///

#include "AliHLTDataSink.h"

class TFile;
class TTree;
class AliMUONVClusterStore;

/**
 * @class AliHLTMUONClusterWriterComponent
 * @brief A sink component for the dHLT cluster writing in root file.
 *
 * <h2>General properties:</h2>
 *
 * Component ID: \b MUONClusterWriter <br>
 * Library: \b libAliHLTMUON.so  <br>
 * Input Data Types: ('CLUSTORE', 'MUON') <br>
 * Output Data Types: none <br>
 *
 * @ingroup alihlt_muon_components
 */
class AliHLTMUONClusterWriterComponent : public AliHLTDataSink
{
public:
  AliHLTMUONClusterWriterComponent();
  virtual ~AliHLTMUONClusterWriterComponent();
  
  // Public functions to implement AliHLTDataSink's interface.
  // These functions are required for the registration process
  
  virtual const char* GetComponentID();
  virtual void GetInputDataTypes(AliHLTComponentDataTypeList& list);
  virtual AliHLTComponent* Spawn();
  
protected:
  
  // Protected functions to implement AliHLTDataSink's interface.
  // These functions provide initialization as well as the actual processing
  // capabilities of the component.
  
  virtual int DoInit(int argc, const char** argv);
  virtual int DoDeinit();
  virtual int DumpEvent(const AliHLTComponentEventData& evtData, AliHLTComponentTriggerData& trigData);
  using AliHLTDataSink::DumpEvent;
  
private:
  
  // Do not allow copying of this class.
  /// Not implemented.
  AliHLTMUONClusterWriterComponent(const AliHLTMUONClusterWriterComponent& /*obj*/);
  /// Not implemented.
  AliHLTMUONClusterWriterComponent& operator = (const AliHLTMUONClusterWriterComponent& /*obj*/);
  
  void CleanMemory();
  
  void StoreClusters(const AliMUONVClusterStore* clStore);
  
  TFile* fFile; ///< output file
  TTree* fTree; ///< output tree
  AliMUONVClusterStore *fClusterStore; ///< list of clusters in an AliMUONVCluster format
  
  ClassDef(AliHLTMUONClusterWriterComponent, 0)
};

#endif // ALIHLTMUONCLUSTERWRITERCOMPONENT_H
