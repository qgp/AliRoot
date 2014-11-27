/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors:                                                       *
 *   Philippe Pillot <pillot@subatech.in2p3.fr>                           *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

// $Id$

///
///  @file   AliHLTMUONClusterWriterComponent.cxx
///  @author Philippe Pillot <pillot@subatech.in2p3.fr>
///  @date   23 October 2014
///  @brief  Implementation of cluster writer sink component for the dimuon HLT.
///

#include <cassert>

// ROOT
#include <TFile.h>
#include <TTree.h>

// HLT MUON
#include "AliHLTMUONClusterWriterComponent.h"
#include "AliHLTMUONConstants.h"

// MUON
#include "AliMUONVCluster.h"
#include "AliMUONClusterStoreV2.h"


ClassImp(AliHLTMUONClusterWriterComponent)


//_________________________________________________________________________________________________
AliHLTMUONClusterWriterComponent::AliHLTMUONClusterWriterComponent() :
AliHLTDataSink(),
fFile(0x0),
fTree(0x0),
fClusterStore(0x0)
{
  /// Default constructor
}

//_________________________________________________________________________________________________
AliHLTMUONClusterWriterComponent::~AliHLTMUONClusterWriterComponent()
{
  /// Default destructor
}

//_________________________________________________________________________________________________
void AliHLTMUONClusterWriterComponent::CleanMemory()
{
  /// delete allocated memory
  delete fClusterStore;
  fClusterStore = 0x0;
  delete fFile;
  fFile = 0x0;
  fTree = 0x0;
}

//_________________________________________________________________________________________________
const char* AliHLTMUONClusterWriterComponent::GetComponentID()
{
  /// Inherited from AliHLTComponent. Returns the component ID.
  return "MUONClusterWriter";
}

//_________________________________________________________________________________________________
void AliHLTMUONClusterWriterComponent::GetInputDataTypes(AliHLTComponentDataTypeList& list)
{
  /// Inherited from AliHLTComponent. Returns the list of expected input data types.
  list.clear();
  list.push_back(AliHLTMUONConstants::ClusterStoreDataType());
}

//_________________________________________________________________________________________________
AliHLTComponent* AliHLTMUONClusterWriterComponent::Spawn()
{
  /// Inherited from AliHLTComponent. Creates a new object instance.
  return new AliHLTMUONClusterWriterComponent;
}

//_________________________________________________________________________________________________
int AliHLTMUONClusterWriterComponent::DoInit(int argc, const char** argv)
{
  /// Inherited from AliHLTComponent.
  /// Parses the command line parameters and initialises the component.
  
  HLTInfo("Initialising dHLT cluster writer component.");
  
  const char* fileName = NULL;
  
  for (int i = 0; i < argc; i++) {
    
    // file name
    if (strcmp(argv[i], "-datafile") == 0) {
      
      if (fileName != NULL) HLTWarning("file name was already specified. Will replace previous value given by -datafile.");
      
      if (argc <= i+1) {
        HLTError("The file name was not specified.");
        return -EINVAL;
      }
      
      fileName = argv[++i];
      
    }
    
  }
  
  // prepare storage of clusters in an AliMUONVCluster format
  fFile = new TFile(fileName,"RECREATE");
  if (!fFile) {
    HLTError("Cannot open file named %s.",fileName);
    return -EPERM;
  }
  
  fTree = new TTree("TreeR","Clusters");
  if (!fTree) {
    HLTError("Cannot create cluster tree \"TreeR\".");
    CleanMemory();
    return -ENOMEM;
  }
  
  fClusterStore = new AliMUONClusterStoreV2();
  if (!fClusterStore) {
    HLTError("Cannot create cluster store.");
    CleanMemory();
    return -ENOMEM;
  }
  
  if (!fClusterStore->Connect(*fTree, kTRUE)) {
    HLTError("Cannot connect cluster store to the tree.");
    CleanMemory();
    return -EIO;
  }
  
  return 0;
}

//_________________________________________________________________________________________________
int AliHLTMUONClusterWriterComponent::DoDeinit()
{
  /// Inherited from AliHLTComponent. Performs a cleanup of the component.
  
  HLTInfo("Deinitialising dHLT cluster writer component.");
  
  assert(fFile != 0x0 && fTree != 0x0);
  
  // save clusters
  fFile->cd();
  fTree->Write();
  fFile->Close();
  
  CleanMemory();
  
  return 0;
}

//_________________________________________________________________________________________________
int AliHLTMUONClusterWriterComponent::DumpEvent(const AliHLTComponentEventData& /*evtData*/,
                                                AliHLTComponentTriggerData& /*trigData*/)
{
  /// Inherited from AliHLTDataSink. Processes the new event data.
  
  assert(fTree != 0x0 && fClusterStore != 0x0);
  
  int iResult(0);
  
  // skip non physics events (e.g. SOR, EOR)
  if (!IsDataEvent()) return iResult;
  
  fClusterStore->Clear();
  
  // loop over objects in blocks of data type "ClusterStore"
  const TObject* pObj = GetFirstInputObject(AliHLTMUONConstants::ClusterStoreDataType());
  while (pObj) {
    
    // make sure the block indeed contains a cluster store
    const AliMUONVClusterStore *pClStore = dynamic_cast<const AliMUONVClusterStore*>(pObj);
    if (! pClStore) {
      HLTError("The data block of type \"ClusterStore\" does not contains a cluster store.");
      iResult = -EFTYPE;
      continue;
    }
    
    // transfert the clusters to the internal cluster store
    StoreClusters(pClStore);
    
    pObj = GetNextInputObject();
    
  }
  
  fTree->Fill();
  
  return iResult;
}

//_________________________________________________________________________________________________
void AliHLTMUONClusterWriterComponent::StoreClusters(const AliMUONVClusterStore* clStore)
{
  /// transfert the clusters to the internal cluster store
  
  Int_t clIndex = fClusterStore->GetSize();
  UInt_t clId(0);
  
  TIter nextCl(clStore->CreateIterator());
  AliMUONVCluster *cluster(0x0);
  while ((cluster = static_cast<AliMUONVCluster*>(nextCl()))) {
    
    // change the cluster index (its ID has to be unique to add it to the new store)
    clId = AliMUONVCluster::BuildUniqueID(cluster->GetChamberId(), cluster->GetDetElemId(), clIndex);
    cluster->SetUniqueID(clId);
    
    // add the cluster to the internal store
    if (fClusterStore->Add(*cluster)) ++clIndex;
    
  }
  
}

