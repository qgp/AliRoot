/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

// --- Root ---
#include <Riostream.h>
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>
#include <TList.h>

// --- AliRoot ---
#include "AliAnalysisTaskSE.h"
#include "AliAnalysisManager.h"
#include "AliStack.h"
#include "AliESDEvent.h"
#include "AliESDInputHandler.h"
#include "AliAODEvent.h"
#include "AliMCEvent.h"
#include "AliEMCALGeometry.h"
#include "AliCaloCalibPedestal.h"
#include "AliEMCALFastORPatch.h"

#include "AliAnalysisTaskEMCALFastOR2Trigger.h"

ClassImp(AliAnalysisTaskEMCALFastOR2Trigger)

//________________________________________________________________________
AliAnalysisTaskEMCALFastOR2Trigger::AliAnalysisTaskEMCALFastOR2Trigger() 
  : AliAnalysisTaskSE(),
    fNcol(2),
    fNrow(2),
    fShiftCol(1),
    fShiftRow(1),
    fTriggerClustersName("triggerClusters"),
    fTimeCutOn(1),
    fMinL0Time(7),
    fMaxL0Time(9),
    fCheckDeadClusters(0),
    fPedestal(0)
{
  // Constructor
}

//________________________________________________________________________
AliAnalysisTaskEMCALFastOR2Trigger::AliAnalysisTaskEMCALFastOR2Trigger(const char *name)
  : AliAnalysisTaskSE(name),
    fNcol(2),
    fNrow(2),
    fShiftCol(1),
    fShiftRow(1),
    fTriggerClustersName("triggerClusters"),
    fTimeCutOn(1),
    fMinL0Time(7),
    fMaxL0Time(9),
    fCheckDeadClusters(0),
    fPedestal(0)
{
  // Constructor

}

//________________________________________________________________________
AliAnalysisTaskEMCALFastOR2Trigger::~AliAnalysisTaskEMCALFastOR2Trigger()
{
  // Destructor
}

//________________________________________________________________________
void AliAnalysisTaskEMCALFastOR2Trigger::UserCreateOutputObjects()
{
  // Create output objects
}

//________________________________________________________________________
void AliAnalysisTaskEMCALFastOR2Trigger::UserExec(Option_t *) 
{
  // Main loop, called for each event
	
  // Create pointer to reconstructed event
  AliVEvent *event = InputEvent();
  if (!event) { 
    AliWarning("Could not retrieve event"); 
    return; 
  }
    
  // create pointer to event
  AliESDEvent* esd = dynamic_cast<AliESDEvent*>(event);
  if (!esd) {
    AliError("Cannot get the ESD event");
    return;
  }  
  
  AliESDCaloTrigger *triggers = esd->GetCaloTrigger("EMCAL");
  
  if (!triggers || !(triggers->GetEntries() > 0))
    return;
  
  AliEMCALGeometry *fGeom = AliEMCALGeometry::GetInstance(AliEMCALGeometry::GetDefaultGeometryName());
	Int_t nSupMod = 0, nModule = 0, nIphi = 0, nIeta = 0, iphi = 0, ieta = 0;
	  
  // -------------------------------
  // Trigger clusterizer parameters
  
  Int_t totalCols = 48;
  Int_t totalRows = 60;
  Int_t nTRURow = 15;
  Int_t nTRUCol = 2;
  Int_t nColFastOR = totalCols / nTRUCol;
  Int_t nRowFastOR = totalRows / nTRURow;
  Int_t maxiShiftRow = fNrow / fShiftRow;
  Int_t maxiShiftCol = fNcol / fShiftCol;
  Int_t nFastORCluster = fNcol * fNrow;
  Int_t nTriggerClusters = totalRows * totalCols / fNcol / fNrow;
  Int_t nTotalClus = nTriggerClusters * maxiShiftCol * maxiShiftRow;
  Int_t nClusRowNoShift = nRowFastOR / fNrow;
  //Int_t nClusColNoShift = nColFastOR / fNcol;
  
  // -------------------------------
  
  TClonesArray *triggers_array = dynamic_cast<TClonesArray*>(InputEvent()->FindListObject(fTriggerClustersName));
  if(!triggers_array){
    triggers_array = new TClonesArray("AliESDCaloTrigger", nTotalClus);
    triggers_array->SetName(fTriggerClustersName);
    InputEvent()->AddObject(triggers_array);
  }
  else {
    triggers_array->Delete();
  }
  
  Bool_t *dead_clusters = new Bool_t[nTotalClus];
  for (Int_t i = 0; i < nTotalClus; i++){
    dead_clusters[i] = kFALSE;
  }
  
  triggers->Reset();
  
  while (triggers->Next()) {
    Float_t L0FastORamp = 0;
    
    triggers->GetAmplitude(L0FastORamp);
    
    if (L0FastORamp < 0)
      continue;
    
    if (fTimeCutOn){
      Int_t ntimes = 0;
      triggers->GetNL0Times(ntimes);
    
      if (ntimes < 1)
        continue;

      Int_t trgtimes[25];
      triggers->GetL0Times(trgtimes);
      
      Bool_t trgInTimeWindow = 0;
      for (Int_t i = 0; i < ntimes; i++) {
        if ((fMaxL0Time >= trgtimes[i]) && (fMinL0Time <= trgtimes[i]))
          trgInTimeWindow = 1;
      }

      if (!trgInTimeWindow)
        continue;
    }

    Int_t gCol = 0, gRow = 0;
    
    triggers->GetPosition(gCol, gRow);
    
    Int_t find = -1;
    fGeom->GetAbsFastORIndexFromPositionInEMCAL(gCol, gRow, find);
    
    if (find<0)
      continue;
    
    Int_t cidx[4] = {-1};
    Bool_t ret = fGeom->GetCellIndexFromFastORIndex(find, cidx);
    
    if (!ret)
      continue;
    
    Bool_t deadCluster = kFALSE;
    
    if (fCheckDeadClusters && fPedestal){
      for (Int_t i = 0; i < 4; i++){
        fGeom->GetCellIndex (cidx[i], nSupMod, nModule, nIphi, nIeta);
        fGeom->GetCellPhiEtaIndexInSModule (nSupMod, nModule, nIphi, nIeta, iphi, ieta);
        
        Double_t d = fPedestal->GetDeadMap(nSupMod)->GetBinContent(ieta,iphi);
        if (d == AliCaloCalibPedestal::kDead || d == AliCaloCalibPedestal::kHot){
          deadCluster = kTRUE;
        }
      }
    }
    
    // --------------------------
    // Trigger clusterizer
    
    for (Int_t ishiftRow = 0; ishiftRow < maxiShiftRow; ishiftRow++){
      Int_t nClusRow = (nRowFastOR - fShiftRow * ishiftRow) / fNrow;
      
      for (Int_t ishiftCol = 0; ishiftCol < maxiShiftCol; ishiftCol++){
        Int_t iTotalClus = nTriggerClusters * (ishiftRow * maxiShiftCol + ishiftCol);

        Int_t nClusCol = (nColFastOR - fShiftCol * ishiftCol) / fNcol; 
        
        Int_t irow_eff = gRow - fShiftRow * ishiftRow; 
        Int_t iTRUrow = irow_eff / nRowFastOR;
        irow_eff -= iTRUrow * nRowFastOR;
        Int_t iClusRow = irow_eff / fNrow; 
        
        if (irow_eff < 0 || iClusRow >= nClusRow) 
          continue;
        
        Int_t icol_eff = gCol - fShiftCol * ishiftCol;
        Int_t iTRUcol = icol_eff / nColFastOR;
        icol_eff -= iTRUcol * nColFastOR;
        Int_t iClusCol = icol_eff / fNcol; 
        
        if (icol_eff < 0 || iClusCol >= nClusCol) 
          continue;

        irow_eff += iTRUrow * nRowFastOR;
        iClusRow = irow_eff / fNrow; 
        
        icol_eff += iTRUcol * nColFastOR;
        iClusCol = icol_eff / fNcol; 
        
        Int_t iTriggerCluster = iClusRow + iClusCol * nClusRowNoShift * nTRURow + iTotalClus;
        Int_t iTriggerDigit = irow_eff % fNrow + (icol_eff % fNcol) * fNrow;
        
        if (dead_clusters[iTriggerCluster] && fCheckDeadClusters) deadCluster = kTRUE;
        
        if (deadCluster){
          dead_clusters[iTriggerCluster] = kTRUE;
          if (triggers_array->At(iTriggerCluster)){
            triggers_array->RemoveAt(iTriggerCluster);
          }
          continue;
        }
        
        if (!triggers_array->At(iTriggerCluster)){
          (*triggers_array)[iTriggerCluster] = new AliEMCALFastORPatch(iTriggerCluster, nFastORCluster);
        }
        
        AliEMCALFastORPatch *triggerCluster = dynamic_cast<AliEMCALFastORPatch*>(triggers_array->At(iTriggerCluster));
        
        if (triggerCluster->GetFastORamplitude(iTriggerDigit) > -1){
          AliWarning("FastOR already added!");
          return;
        }
        
        triggerCluster->AddFastORat(L0FastORamp, gCol, gRow, iTriggerDigit);
        
      } // loop on col shift
      
    } // loop on row shift

    // ------------------------------
    
  } // loop on L0 FastOR triggers
    
  delete[] dead_clusters;
  
  triggers_array->Compress();
}
