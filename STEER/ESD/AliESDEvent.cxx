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

/* $Id: AliESDEvent.cxx 64008 2013-08-28 13:09:59Z hristov $ */

//-----------------------------------------------------------------
//           Implementation of the AliESDEvent class
//   This is the class to deal with during the physics analysis of data.
//   It also ensures the backward compatibility with the old ESD format.
/*
   AliESDEvent *ev= new AliESDEvent();
   ev->ReadFromTree(esdTree);
   ...
    for (Int_t i=0; i<nev; i++) {
      esdTree->GetEntry(i);
      if(ev->GetAliESDOld())ev->CopyFromOldESD();
*/
//   The AliESDInputHandler does this automatically for you
//
// Origin: Christian Klein-Boesing, CERN, Christian.Klein-Boesing@cern.ch
//-----------------------------------------------------------------

#include "TList.h"
#include "TRefArray.h"
#include <TNamed.h>
#include <TROOT.h>
#include <TInterpreter.h>

#include "event.h"
#include "AliESDEvent.h"
#include "AliESDfriend.h"
#include "AliESDVZERO.h"
#include "AliESDFMD.h"
#include "AliESD.h"
#include "AliESDMuonTrack.h"
#include "AliESDMuonCluster.h"
#include "AliESDMuonPad.h"
#include "AliESDMuonGlobalTrack.h"       // AU
#include "AliESDPmdTrack.h"
#include "AliESDTrdTrack.h"
#include "AliESDVertex.h"
#include "AliESDcascade.h"
#include "AliESDPmdTrack.h"
#include "AliESDTrdTrigger.h"
#include "AliESDTrdTrack.h"
#include "AliESDTrdTracklet.h"
#include "AliESDVertex.h"
#include "AliVertexerTracks.h"
#include "AliESDcascade.h"
#include "AliESDkink.h"
#include "AliESDtrack.h"
#include "AliESDHLTtrack.h"
#include "AliESDCaloCluster.h"
#include "AliESDCaloCells.h"
#include "AliESDv0.h"
#include "AliESDFMD.h"
#include "AliESDVZERO.h"
#include "AliMultiplicity.h"
#include "AliRawDataErrorLog.h"
#include "AliLog.h"
#include "AliESDACORDE.h"
#include "AliESDAD.h"
#include "AliESDHLTDecision.h"
#include "AliCentrality.h"
#include "AliESDCosmicTrack.h"
#include "AliTriggerConfiguration.h"
#include "AliTriggerClass.h"
#include "AliTriggerCluster.h"
#include "AliEventplane.h"

ClassImp(AliESDEvent)

// here we define the names, some classes are no TNamed, therefore the classnames 
// are the Names
  const char* AliESDEvent::fgkESDListName[kESDListN] = {"AliESDRun",
							"AliESDHeader",
							"AliESDZDC",
							"AliESDFMD",
							"AliESDVZERO",
							"AliESDTZERO",
							"TPCVertex",
							"SPDVertex",
							"PrimaryVertex",
							"AliMultiplicity",
							"PHOSTrigger",
							"EMCALTrigger",
							"SPDPileupVertices",
							"TrkPileupVertices",
							"Tracks",
							"MuonTracks",
							"MuonClusters",
							"MuonPads",
							"MuonGlobalTracks",      // AU
							"PmdTracks",
							"AliESDTrdTrigger",
							"TrdTracks",
  						        "TrdTracklets",
							"V0s",
							"Cascades",
							"Kinks",
							"CaloClusters",
							"EMCALCells",
							"PHOSCells",
							"AliRawDataErrorLogs",
							"AliESDACORDE",
							"AliESDAD",
							"AliTOFHeader",
                                                        "CosmicTracks",
							"AliESDTOFCluster",
							"AliESDTOFHit",
							"AliESDTOFMatch",
							"AliESDFIT"};


//______________________________________________________________________________
AliESDEvent::AliESDEvent():
  AliVEvent(),
  fESDObjects(new TList()),
  fESDRun(0),
  fHeader(0),
  fESDZDC(0),
  fESDFMD(0),
  fESDVZERO(0),
  fESDTZERO(0),
  fESDFIT(0),
  fTPCVertex(0),
  fSPDVertex(0),
  fPrimaryVertex(0),
  fSPDMult(0),
  fPHOSTrigger(0),
  fEMCALTrigger(0),
  fESDACORDE(0),
  fESDAD(0),
  fTrdTrigger(0),
  fSPDPileupVertices(0),
  fTrkPileupVertices(0),
  fTracks(0),
  fMuonTracks(0),
  fMuonClusters(0),
  fMuonPads(0),
  fMuonGlobalTracks(0),    // AU
  fPmdTracks(0),
  fTrdTracks(0),
  fTrdTracklets(0),
  fV0s(0),  
  fCascades(0),
  fKinks(0),
  fCaloClusters(0),
  fEMCALCells(0), fPHOSCells(0),
  fCosmicTracks(0),
  fESDTOFClusters(0),
  fESDTOFHits(0),
  fESDTOFMatches(0),
  fErrorLogs(0),
  fOldMuonStructure(kFALSE),
  fESDOld(0),
  fESDFriendOld(0),
  fConnected(kFALSE),
  fUseOwnList(kFALSE),
  fTracksConnected(kFALSE),
  fTOFHeader(0),
  fCentrality(0),
  fEventplane(0),
  fDetectorStatus(0xFFFFFFFF),
  fDAQDetectorPattern(0xFFFF),
  fDAQAttributes(0xFFFF)
{
}
//______________________________________________________________________________
AliESDEvent::AliESDEvent(const AliESDEvent& esd):
  AliVEvent(esd),
  fESDObjects(new TList()),
  fESDRun(new AliESDRun(*esd.fESDRun)),
  fHeader(new AliESDHeader(*esd.fHeader)),
  fESDZDC(new AliESDZDC(*esd.fESDZDC)),
  fESDFMD(new AliESDFMD(*esd.fESDFMD)),
  fESDVZERO(new AliESDVZERO(*esd.fESDVZERO)),
  fESDTZERO(new AliESDTZERO(*esd.fESDTZERO)),
  fESDFIT(new AliESDFIT(*esd.fESDFIT)),
  fTPCVertex(new AliESDVertex(*esd.fTPCVertex)),
  fSPDVertex(new AliESDVertex(*esd.fSPDVertex)),
  fPrimaryVertex(new AliESDVertex(*esd.fPrimaryVertex)),
  fSPDMult(new AliMultiplicity(*esd.fSPDMult)),
  fPHOSTrigger(new AliESDCaloTrigger(*esd.fPHOSTrigger)),
  fEMCALTrigger(new AliESDCaloTrigger(*esd.fEMCALTrigger)),
  fESDACORDE(new AliESDACORDE(*esd.fESDACORDE)),
  fESDAD(new AliESDAD(*esd.fESDAD)),
  fTrdTrigger(new AliESDTrdTrigger(*esd.fTrdTrigger)),
  fSPDPileupVertices(new TClonesArray(*esd.fSPDPileupVertices)),
  fTrkPileupVertices(new TClonesArray(*esd.fTrkPileupVertices)),
  fTracks(new TClonesArray(*esd.fTracks)),
  fMuonTracks(new TClonesArray(*esd.fMuonTracks)),
  fMuonClusters(new TClonesArray(*esd.fMuonClusters)),
  fMuonPads(new TClonesArray(*esd.fMuonPads)),
  fMuonGlobalTracks(new TClonesArray(*esd.fMuonGlobalTracks)),     // AU
  fPmdTracks(new TClonesArray(*esd.fPmdTracks)),
  fTrdTracks(new TClonesArray(*esd.fTrdTracks)),
  fTrdTracklets(new TClonesArray(*esd.fTrdTracklets)),
  fV0s(new TClonesArray(*esd.fV0s)),  
  fCascades(new TClonesArray(*esd.fCascades)),
  fKinks(new TClonesArray(*esd.fKinks)),
  fCaloClusters(new TClonesArray(*esd.fCaloClusters)),
  fEMCALCells(new AliESDCaloCells(*esd.fEMCALCells)),
  fPHOSCells(new AliESDCaloCells(*esd.fPHOSCells)),
  fCosmicTracks(new TClonesArray(*esd.fCosmicTracks)),
  fESDTOFClusters(esd.fESDTOFClusters ? new TClonesArray(*esd.fESDTOFClusters) : 0),
  fESDTOFHits(esd.fESDTOFHits ? new TClonesArray(*esd.fESDTOFHits) : 0),
  fESDTOFMatches(esd.fESDTOFMatches ? new TClonesArray(*esd.fESDTOFMatches) : 0),
  fErrorLogs(new TClonesArray(*esd.fErrorLogs)),
  fOldMuonStructure(esd.fOldMuonStructure),
  fESDOld(esd.fESDOld ? new AliESD(*esd.fESDOld) : 0),
  fESDFriendOld(esd.fESDFriendOld ? new AliESDfriend(*esd.fESDFriendOld) : 0),
  fConnected(esd.fConnected),
  fUseOwnList(esd.fUseOwnList),
  fTracksConnected(kFALSE),
  fTOFHeader(new AliTOFHeader(*esd.fTOFHeader)),
  fCentrality(new AliCentrality(*esd.fCentrality)),
  fEventplane(new AliEventplane(*esd.fEventplane)),
  fDetectorStatus(esd.fDetectorStatus),
  fDAQDetectorPattern(esd.fDAQDetectorPattern),
  fDAQAttributes(esd.fDAQAttributes)
{
  printf("copying ESD event...\n");   // AU
  // CKB init in the constructor list and only add here ...
  AddObject(fESDRun);
  AddObject(fHeader);
  AddObject(fESDZDC);
  AddObject(fESDFMD);
  AddObject(fESDVZERO);
  AddObject(fESDTZERO);
  AddObject(fTPCVertex);
  AddObject(fSPDVertex);
  AddObject(fPrimaryVertex);
  AddObject(fSPDMult);
  AddObject(fPHOSTrigger);
  AddObject(fEMCALTrigger);
  AddObject(fTrdTrigger);
  AddObject(fSPDPileupVertices);
  AddObject(fTrkPileupVertices);
  AddObject(fTracks);
  AddObject(fMuonTracks);
  AddObject(fMuonGlobalTracks);    // AU
  AddObject(fPmdTracks);
  AddObject(fTrdTracks);
  AddObject(fTrdTracklets);
  AddObject(fV0s);
  AddObject(fCascades);
  AddObject(fKinks);
  AddObject(fCaloClusters);
  AddObject(fEMCALCells);
  AddObject(fPHOSCells);
  AddObject(fCosmicTracks);
  AddObject(fESDTOFClusters);
  AddObject(fESDTOFHits);
  AddObject(fESDTOFMatches);
  AddObject(fErrorLogs);
  AddObject(fESDACORDE);
  AddObject(fESDAD);
  AddObject(fTOFHeader);
  AddObject(fMuonClusters);
  AddObject(fMuonPads);
  //
  AddObject(fESDFIT);
  GetStdContent();
  ConnectTracks();

}

//______________________________________________________________________________
AliESDEvent & AliESDEvent::operator=(const AliESDEvent& source) {

  // Assignment operator
  printf("operator = ESD\n");
  if(&source == this) return *this;
  AliVEvent::operator=(source);

  // This assumes that the list is already created
  // and that the virtual void Copy(Tobject&) function
  // is correctly implemented in the derived class
  // otherwise only TObject::Copy() will be used



  if((fESDObjects->GetSize()==0)&&(source.fESDObjects->GetSize()>=kESDListN)){
    // We cover the case that we do not yet have the 
    // standard content but the source has it
    CreateStdContent();
  }

  TIter next(source.GetList());
  TObject *its = 0;
  TString name;
  while ((its = next())) {
    name.Form("%s", its->GetName());
    TObject *mine = fESDObjects->FindObject(name.Data());
    if(!mine){
      TClass* pClass=TClass::GetClass(its->ClassName());
      if (!pClass) {
	AliWarning(Form("Can not find class description for entry %s (%s)\n",
			its->ClassName(), name.Data()));
	continue;
      }

      mine=(TObject*)pClass->New();
      if(!mine){
      // not in this: can be added to list
	AliWarning(Form("%s:%d Could not find %s for copying \n",
			(char*)__FILE__,__LINE__,name.Data()));
	continue;
      }  
      if(mine->InheritsFrom("TNamed")){
	((TNamed*)mine)->SetName(name);
      }
      else if(mine->InheritsFrom("TCollection")){
	if(mine->InheritsFrom("TClonesArray")) {
	  TClonesArray* tcits = dynamic_cast<TClonesArray*>(its);
	  if (tcits)
	    dynamic_cast<TClonesArray*>(mine)->SetClass(tcits->GetClass());
	}
	dynamic_cast<TCollection*>(mine)->SetName(name);
      }
      AliDebug(1, Form("adding object %s of type %s", mine->GetName(), mine->ClassName()));
      AddObject(mine);
    }  
   
    if(!its->InheritsFrom("TCollection")){
      // simple objects
      its->Copy(*mine);
    }
    else if(its->InheritsFrom("TClonesArray")){
      // Create or expand the tclonesarray pointers
      // so we can directly copy to the object
      TClonesArray *itstca = (TClonesArray*)its;
      TClonesArray *minetca = (TClonesArray*)mine;

      // this leaves the capacity of the TClonesArray the same
      // except for a factor of 2 increase when size > capacity
      // does not release any memory occupied by the tca
      minetca->ExpandCreate(itstca->GetEntriesFast());
      for(int i = 0;i < itstca->GetEntriesFast();++i){
	// copy 
	TObject *minetcaobj = minetca->At(i);
	TObject *itstcaobj = itstca->At(i);
	// no need to delete first
	// pointers within the class should be handled by Copy()...
	// Can there be Empty slots?
	itstcaobj->Copy(*minetcaobj);
      }
    }
    else{
      AliWarning(Form("%s:%d cannot copy TCollection \n",
		      (char*)__FILE__,__LINE__));
    }
  }

  fOldMuonStructure = source.fOldMuonStructure;
  
  fCentrality = source.fCentrality;
  fEventplane = source.fEventplane;

  fConnected  = source.fConnected;
  fUseOwnList = source.fUseOwnList;

  fDetectorStatus = source.fDetectorStatus;
  fDAQDetectorPattern = source.fDAQDetectorPattern;
  fDAQAttributes = source.fDAQAttributes;

  fTracksConnected = kFALSE;
  ConnectTracks();
  return *this;
}


//______________________________________________________________________________
AliESDEvent::~AliESDEvent()
{
  //
  // Standard destructor
  //

  // everthing on the list gets deleted automatically

  
  if(fESDObjects&&!fConnected)
    {
      delete fESDObjects;
      fESDObjects = 0;
    }
  if (fCentrality) delete fCentrality;
  if (fEventplane) delete fEventplane;
  

}

void AliESDEvent::Copy(TObject &obj) const {

  // interface to TOBject::Copy
  // Copies the content of this into obj!
  // bascially obj = *this

  if(this==&obj)return;
  AliESDEvent *robj = dynamic_cast<AliESDEvent*>(&obj);
  if(!robj)return; // not an AliESEvent
  *robj = *this;
  return;
}

//______________________________________________________________________________
void AliESDEvent::Reset()
{

  // Handle the cases
  // Std content + Non std content

  // Reset the standard contents
  ResetStdContent(); 
  fDetectorStatus = 0xFFFFFFFF;
  fDAQDetectorPattern = 0xFFFF;
  fDAQAttributes = 0xFFFF;
  //  reset for the old data without AliESDEvent...
  if(fESDOld)fESDOld->Reset();
  if(fESDFriendOld){
    fESDFriendOld->~AliESDfriend();
    new (fESDFriendOld) AliESDfriend();
  }
  // 

  if(fESDObjects->GetSize()>kESDListN){
    // we have non std content
    // this also covers esdfriends
    for(int i = kESDListN;i < fESDObjects->GetSize();++i){
      TObject *pObject = fESDObjects->At(i);
      // TClonesArrays
      if(pObject->InheritsFrom(TClonesArray::Class())){
	((TClonesArray*)pObject)->Delete();
      }
      else if(!pObject->InheritsFrom(TCollection::Class())){
	TClass *pClass = TClass::GetClass(pObject->ClassName());
	if (pClass && pClass->GetListOfMethods()->FindObject("Clear")) {
	  AliDebug(1, Form("Clear for object %s class %s", pObject->GetName(), pObject->ClassName()));
	  pObject->Clear();
	}
	else {
	  AliDebug(1, Form("ResetWithPlacementNew for object %s class %s", pObject->GetName(), pObject->ClassName()));
	  ResetWithPlacementNew(pObject);
	}
      }
      else{
	AliWarning(Form("No reset for %s \n",
			pObject->ClassName()));
      }
    }
  }

}

//______________________________________________________________________________
Bool_t AliESDEvent::ResetWithPlacementNew(TObject *pObject){
  //
  // funtion to reset using the already allocated space
  //
  Long_t dtoronly = TObject::GetDtorOnly();
  TClass *pClass = TClass::GetClass(pObject->ClassName()); 
  TObject::SetDtorOnly(pObject);
  delete pObject;
  // Recreate with placement new
  pClass->New(pObject);
  // Restore the state.
  TObject::SetDtorOnly((void*)dtoronly);
  return kTRUE;
}

//______________________________________________________________________________
void AliESDEvent::ResetStdContent()
{
  // Reset the standard contents
  if(fESDRun) fESDRun->Reset();
  if(fHeader) fHeader->Reset();
  if(fCentrality) fCentrality->Reset();
  if(fEventplane) fEventplane->Reset();
  if(fESDZDC) fESDZDC->Reset();
  if(fESDFMD) {
    fESDFMD->Clear();
  }
  if(fESDVZERO){
    // reset by callin d'to /c'tor keep the pointer
    fESDVZERO->~AliESDVZERO();
    new (fESDVZERO) AliESDVZERO();
  }  
  if(fESDACORDE){
    fESDACORDE->~AliESDACORDE();
    new (fESDACORDE) AliESDACORDE();	
  } 

  if(fESDAD){
    fESDAD->~AliESDAD();
    new (fESDAD) AliESDAD();	
  } 

  if(fESDFIT) fESDFIT->Reset(); 

  if(fESDTZERO) fESDTZERO->Reset(); 
  // CKB no clear/reset implemented
  if(fTPCVertex){
    fTPCVertex->~AliESDVertex();
    new (fTPCVertex) AliESDVertex();
    fTPCVertex->SetName(fgkESDListName[kTPCVertex]);
  }
  if(fSPDVertex){
    fSPDVertex->~AliESDVertex();
    new (fSPDVertex) AliESDVertex();
    fSPDVertex->SetName(fgkESDListName[kSPDVertex]);
  }
  if(fPrimaryVertex){
    fPrimaryVertex->~AliESDVertex();
    new (fPrimaryVertex) AliESDVertex();
    fPrimaryVertex->SetName(fgkESDListName[kPrimaryVertex]);
  }
  if(fSPDMult){
    fSPDMult->~AliMultiplicity();
    new (fSPDMult) AliMultiplicity();
  }
  if(fTOFHeader){
    fTOFHeader->~AliTOFHeader();
    new (fTOFHeader) AliTOFHeader();
    //fTOFHeader->SetName(fgkESDListName[kTOFHeader]);
  }
  if (fTrdTrigger) {
    fTrdTrigger->~AliESDTrdTrigger();
    new (fTrdTrigger) AliESDTrdTrigger();
  }
	
  if(fPHOSTrigger)fPHOSTrigger->DeAllocate(); 
  if(fEMCALTrigger)fEMCALTrigger->DeAllocate(); 
  if(fSPDPileupVertices)fSPDPileupVertices->Delete();
  if(fTrkPileupVertices)fTrkPileupVertices->Delete();
  fTracksConnected = kFALSE;
  if(fTracks)fTracks->Delete();
  if(fMuonTracks)fMuonTracks->Clear("C");
  if(fMuonClusters)fMuonClusters->Clear("C");
  if(fMuonPads)fMuonPads->Clear("C");
  if(fMuonGlobalTracks)fMuonGlobalTracks->Clear("C");     // AU
  if(fPmdTracks)fPmdTracks->Delete();
  if(fTrdTracks)fTrdTracks->Delete();
  if(fTrdTracklets)fTrdTracklets->Delete();
  if(fV0s)fV0s->Delete();
  if(fCascades)fCascades->Delete();
  if(fKinks)fKinks->Delete();
  if(fCaloClusters)fCaloClusters->Delete();
  if(fPHOSCells)fPHOSCells->DeleteContainer();
  if(fEMCALCells)fEMCALCells->DeleteContainer();
  if(fCosmicTracks)fCosmicTracks->Delete();
  if(fESDTOFClusters)fESDTOFClusters->Clear();
  if(fESDTOFHits)fESDTOFHits->Clear();
  if(fESDTOFMatches)fESDTOFMatches->Clear();
  if(fErrorLogs) fErrorLogs->Delete();

  // don't reset fconnected fConnected and the list

}


//______________________________________________________________________________
Int_t AliESDEvent::AddV0(const AliESDv0 *v) {
  //
  // Add V0
  //
  TClonesArray &fv = *fV0s;
  Int_t idx=fV0s->GetEntriesFast();
  new(fv[idx]) AliESDv0(*v);
  return idx;
}  

//______________________________________________________________________________
Bool_t AliESDEvent::IsDetectorInTriggerCluster(TString detector, AliTriggerConfiguration* trigConf) const {
  // Check if a given detector was read-out in the analyzed event
  const TObjArray& classesArray=trigConf->GetClasses();
  ULong64_t trigMask=GetTriggerMask();
  ULong64_t trigMaskNext50=GetTriggerMaskNext50();
  Int_t nclasses = classesArray.GetEntriesFast();
  for(Int_t iclass=0; iclass < nclasses; iclass++ ) {
    AliTriggerClass* trclass = (AliTriggerClass*)classesArray.At(iclass);
    ULong64_t classMask=trclass->GetMask();
    ULong64_t classMaskNext50=trclass->GetMaskNext50();
    if(trigMask & classMask){
      TString detList=trclass->GetCluster()->GetDetectorsInCluster();
      if(detList.Contains(detector.Data())){
	return kTRUE;
      }
    }
    if(trigMaskNext50 & classMaskNext50){
      TString detList=trclass->GetCluster()->GetDetectorsInCluster();
      if(detList.Contains(detector.Data())){
	return kTRUE;
      }
    }
  }
  return kFALSE; 
}
//______________________________________________________________________________
void AliESDEvent::Print(Option_t *) const 
{
  //
  // Print header information of the event
  //
  printf("ESD run information\n");
  printf("Event # in file %d Bunch crossing # %d Orbit # %d Period # %d Run # %d Trigger %lld %lld Magnetic field %f \n",
	 GetEventNumberInFile(),
	 GetBunchCrossNumber(),
	 GetOrbitNumber(),
	 GetPeriodNumber(),
	 GetRunNumber(),
	 GetTriggerMask(),
	 GetTriggerMaskNext50(),
	 GetMagneticField() );
  if (fPrimaryVertex)
    printf("Vertex: (%.4f +- %.4f, %.4f +- %.4f, %.4f +- %.4f) cm\n",
	   fPrimaryVertex->GetX(), fPrimaryVertex->GetXRes(),
	   fPrimaryVertex->GetY(), fPrimaryVertex->GetYRes(),
	   fPrimaryVertex->GetZ(), fPrimaryVertex->GetZRes());
  printf("Mean vertex in RUN: X=%.4f Y=%.4f Z=%.4f cm\n",
	 GetDiamondX(),GetDiamondY(),GetDiamondZ());
  if(fSPDMult)
    printf("SPD Multiplicity. Number of tracklets %d \n",
           fSPDMult->GetNumberOfTracklets());
  printf("Number of pileup primary vertices reconstructed with SPD %d\n", 
	 GetNumberOfPileupVerticesSPD());
  printf("Number of pileup primary vertices reconstructed using the tracks %d\n",
	 GetNumberOfPileupVerticesTracks());
  printf("Number of tracks: \n");
  printf("                 charged   %d\n", GetNumberOfTracks());
  printf("                 muon      %d\n", GetNumberOfMuonTracks());
  printf("                 glob muon %d\n", GetNumberOfMuonGlobalTracks());    // AU
  printf("                 pmd       %d\n", GetNumberOfPmdTracks());
  printf("                 trd       %d\n", GetNumberOfTrdTracks());
  printf("                 trd trkl  %d\n", GetNumberOfTrdTracklets());
  printf("                 v0        %d\n", GetNumberOfV0s());
  printf("                 cascades  %d\n", GetNumberOfCascades());
  printf("                 kinks     %d\n", GetNumberOfKinks());
  if(fPHOSCells)printf("                 PHOSCells %d\n", fPHOSCells->GetNumberOfCells());
  else printf("                 PHOSCells not in the Event\n");
  if(fEMCALCells)printf("                 EMCALCells %d\n", fEMCALCells->GetNumberOfCells());
  else printf("                 EMCALCells not in the Event\n");
  printf("                 CaloClusters %d\n", GetNumberOfCaloClusters());
  printf("                 FMD       %s\n", (fESDFMD ? "yes" : "no"));
  printf("                 VZERO     %s\n", (fESDVZERO ? "yes" : "no"));
  printf("                 muClusters %d\n", fMuonClusters ? fMuonClusters->GetEntriesFast() : 0);
  printf("                 muPad     %d\n", fMuonPads ? fMuonPads->GetEntriesFast() : 0);
  if (fCosmicTracks) printf("                 Cosmics   %d\n",  GetNumberOfCosmicTracks());
	
  TObject* pHLTDecision=GetHLTTriggerDecision();
  printf("HLT trigger decision: %s\n", pHLTDecision?pHLTDecision->GetOption():"not available");
  if (pHLTDecision) pHLTDecision->Print("compact");

  return;
}

//______________________________________________________________________________
void AliESDEvent::SetESDfriend(const AliESDfriend *ev) const 
{
//
// Attaches the complementary info to the ESD
//
  if (!ev) return;

  // to be sure that we set the tracks also
  // in case of old esds 
  // if(fESDOld)CopyFromOldESD();

  Int_t ntrk=ev->GetNumberOfTracks();
 
  for (Int_t i=0; i<ntrk; i++) {
    AliESDtrack* trc = GetTrack(i);
    if (trc->GetFriendNotStored()) continue;
    const AliESDfriendTrack *f=ev->GetTrack(i);
    if (!f) {AliFatal(Form("NULL pointer for ESD friend track %d",i));}
    trc->SetFriendTrack(f);
  }
}

//______________________________________________________________________________
Bool_t  AliESDEvent::RemoveKink(Int_t rm) const 
{
// ---------------------------------------------------------
// Remove a kink candidate and references to it from ESD,
// if this candidate does not come from a reconstructed decay
// Not yet implemented...
// ---------------------------------------------------------
  Int_t last=GetNumberOfKinks()-1;
  if ((rm<0)||(rm>last)) return kFALSE;

  return kTRUE;
}

//______________________________________________________________________________
Bool_t  AliESDEvent::RemoveV0(Int_t rm) const 
{
// ---------------------------------------------------------
// Remove a V0 candidate and references to it from ESD,
// if this candidate does not come from a reconstructed decay
// ---------------------------------------------------------
  Int_t last=GetNumberOfV0s()-1;
  if ((rm<0)||(rm>last)) return kFALSE;

  AliESDv0 *v0=GetV0(rm);
  Int_t idxP=v0->GetPindex(), idxN=v0->GetNindex();

  v0=GetV0(last);
  Int_t lastIdxP=v0->GetPindex(), lastIdxN=v0->GetNindex();

  Int_t used=0;

  // Check if this V0 comes from a reconstructed decay
  Int_t ncs=GetNumberOfCascades();
  for (Int_t n=0; n<ncs; n++) {
    AliESDcascade *cs=GetCascade(n);

    Int_t csIdxP=cs->GetPindex();
    Int_t csIdxN=cs->GetNindex();

    if (idxP==csIdxP)
       if (idxN==csIdxN) return kFALSE;

    if (csIdxP==lastIdxP)
       if (csIdxN==lastIdxN) used++;
  }

  //Replace the removed V0 with the last V0 
  TClonesArray &a=*fV0s;
  delete a.RemoveAt(rm);

  if (rm==last) return kTRUE;

  //v0 is pointing to the last V0 candidate... 
  new (a[rm]) AliESDv0(*v0);
  delete a.RemoveAt(last);

  if (!used) return kTRUE;
  

  // Remap the indices of the daughters of reconstructed decays
  for (Int_t n=0; n<ncs; n++) {
    AliESDcascade *cs=GetCascade(n);


    Int_t csIdxP=cs->GetPindex();
    Int_t csIdxN=cs->GetNindex();

    if (csIdxP==lastIdxP)
      if (csIdxN==lastIdxN) {
         cs->AliESDv0::SetIndex(1,idxP);
         cs->AliESDv0::SetIndex(0,idxN);
         used--;
         if (!used) return kTRUE;
      }
  }

  return kTRUE;
}

//______________________________________________________________________________
Bool_t  AliESDEvent::RemoveTrack(Int_t rm) const 
{
// ---------------------------------------------------------
// Remove a track and references to it from ESD,
// if this track does not come from a reconstructed decay
// ---------------------------------------------------------
  Int_t last=GetNumberOfTracks()-1;
  if ((rm<0)||(rm>last)) return kFALSE;

  Int_t used=0;

  // Check if this track comes from the reconstructed primary vertices
  if (fTPCVertex && fTPCVertex->GetStatus()) {
     UShort_t *primIdx=fTPCVertex->GetIndices();
     Int_t n=fTPCVertex->GetNIndices();
     while (n--) {
       Int_t idx=Int_t(primIdx[n]);
       if (rm==idx) return kFALSE;
       if (idx==last) used++; 
     }
  }
  if (fPrimaryVertex && fPrimaryVertex->GetStatus()) {
     UShort_t *primIdx=fPrimaryVertex->GetIndices();
     Int_t n=fPrimaryVertex->GetNIndices();
     while (n--) {
       Int_t idx=Int_t(primIdx[n]);
       if (rm==idx) return kFALSE;
       if (idx==last) used++; 
     }
  }
  
  // Check if this track comes from a reconstructed decay
  Int_t nv0=GetNumberOfV0s();
  for (Int_t n=0; n<nv0; n++) {
    AliESDv0 *v0=GetV0(n);

    Int_t idx=v0->GetNindex();
    if (rm==idx) return kFALSE;
    if (idx==last) used++;

    idx=v0->GetPindex();
    if (rm==idx) return kFALSE;
    if (idx==last) used++;
  }

  Int_t ncs=GetNumberOfCascades();
  for (Int_t n=0; n<ncs; n++) {
    AliESDcascade *cs=GetCascade(n);

    Int_t idx=cs->GetIndex();
    if (rm==idx) return kFALSE;
    if (idx==last) used++;

    AliESDv0 *v0=cs;
    idx=v0->GetNindex();
    if (rm==idx) return kFALSE;
    if (idx==last) used++;

    idx=v0->GetPindex();
    if (rm==idx) return kFALSE;
    if (idx==last) used++;
  }

  Int_t nkn=GetNumberOfKinks();
  for (Int_t n=0; n<nkn; n++) {
    AliESDkink *kn=GetKink(n);

    Int_t idx=kn->GetIndex(0);
    if (rm==idx) return kFALSE;
    if (idx==last) used++;

    idx=kn->GetIndex(1);
    if (rm==idx) return kFALSE;
    if (idx==last) used++;
  }

  // Check if this track is associated with a CaloCluster
  Int_t ncl=GetNumberOfCaloClusters();
  for (Int_t n=0; n<ncl; n++) {
    AliESDCaloCluster *cluster=GetCaloCluster(n);
    TArrayI *arr=cluster->GetTracksMatched();
    Int_t s=arr->GetSize();
    while (s--) {
      Int_t idx=arr->At(s);
      if (rm==idx) return kFALSE;
      if (idx==last) used++;     
    }
  }

  // from here on we remove the track
  //
  //Replace the removed track with the last track 
  TClonesArray &a=*fTracks;
  AliESDtrack* trm = GetTrack(rm);
  trm->SuppressTOFMatches(); // remove reference to this track from stored TOF clusters
  delete a.RemoveAt(rm);
  //
  if (rm==last) return kTRUE;

  AliESDtrack *t=GetTrack(last);
  if (!t) {AliFatal(Form("NULL pointer for ESD track %d",last));}
  t->SetID(rm);
  new (a[rm]) AliESDtrack(*t);
  delete a.RemoveAt(last);

  if (!used) return kTRUE;
  

  // Remap the indices of the tracks used for the primary vertex reconstruction
  if (fTPCVertex && fTPCVertex->GetStatus()) {
     UShort_t *primIdx=fTPCVertex->GetIndices();
     Int_t n=fTPCVertex->GetNIndices();
     while (n--) {
       Int_t idx=Int_t(primIdx[n]);
       if (idx==last) {
          primIdx[n]=Short_t(rm); 
          used--;
          if (!used) return kTRUE;
       }
     }
  }  
  if (fPrimaryVertex && fPrimaryVertex->GetStatus()) {
     UShort_t *primIdx=fPrimaryVertex->GetIndices();
     Int_t n=fPrimaryVertex->GetNIndices();
     while (n--) {
       Int_t idx=Int_t(primIdx[n]);
       if (idx==last) {
          primIdx[n]=Short_t(rm); 
          used--;
          if (!used) return kTRUE;
       }
     }
  }  

  // Remap the indices of the daughters of reconstructed decays
  for (Int_t n=0; n<nv0; n++) {
    AliESDv0 *v0=GetV0(n);
    if (v0->GetIndex(0)==last) {
       v0->SetIndex(0,rm);
       used--;
       if (!used) return kTRUE;
    }
    if (v0->GetIndex(1)==last) {
       v0->SetIndex(1,rm);
       used--;
       if (!used) return kTRUE;
    }
  }

  for (Int_t n=0; n<ncs; n++) {
    AliESDcascade *cs=GetCascade(n);
    if (cs->GetIndex()==last) {
       cs->SetIndex(rm);
       used--;
       if (!used) return kTRUE;
    }
    AliESDv0 *v0=cs;
    if (v0->GetIndex(0)==last) {
       v0->SetIndex(0,rm);
       used--;
       if (!used) return kTRUE;
    }
    if (v0->GetIndex(1)==last) {
       v0->SetIndex(1,rm);
       used--;
       if (!used) return kTRUE;
    }
  }

  for (Int_t n=0; n<nkn; n++) {
    AliESDkink *kn=GetKink(n);
    if (kn->GetIndex(0)==last) {
       kn->SetIndex(rm,0);
       used--;
       if (!used) return kTRUE;
    }
    if (kn->GetIndex(1)==last) {
       kn->SetIndex(rm,1);
       used--;
       if (!used) return kTRUE;
    }
  }

  // Remap the indices of the tracks accosicated with CaloClusters
  for (Int_t n=0; n<ncl; n++) {
    AliESDCaloCluster *cluster=GetCaloCluster(n);
    TArrayI *arr=cluster->GetTracksMatched();
    Int_t s=arr->GetSize();
    while (s--) {
      Int_t idx=arr->At(s);
      if (idx==last) {
         arr->AddAt(rm,s);
         used--; 
         if (!used) return kTRUE;
      }
    }
  }

  return kTRUE;
}

//______________________________________________________________________________
Bool_t AliESDEvent::Clean(Float_t *cleanPars) 
{
  //
  // Remove the data which are not needed for the physics analysis.
  //
  // 1) Cleaning the V0 candidates
  //    ---------------------------
  //    If the cosine of the V0 pointing angle "csp" and 
  //    the DCA between the daughter tracks "dca" does not satisfy 
  //    the conditions 
  //
  //     csp > cleanPars[1] + dca/cleanPars[0]*(1.- cleanPars[1])
  //
  //    an attempt to remove this V0 candidate from ESD is made.
  //
  //    The V0 candidate gets removed if it does not belong to any 
  //    recosntructed cascade decay
  //
  //    12.11.2007, optimal values: cleanPars[0]=0.5, cleanPars[1]=0.999
  //
  // 2) Cleaning the tracks
  //    ----------------------
  //    If track's transverse parameter is larger than cleanPars[2]
  //                       OR
  //    track's longitudinal parameter is larger than cleanPars[3]
  //    an attempt to remove this track from ESD is made.
  //
  //    The track gets removed if it does not come 
  //    from a reconstructed decay
  //
  Bool_t rc=kFALSE;

  Float_t dcaMax=cleanPars[0];
  Float_t cspMin=cleanPars[1];

  Int_t nV0s=GetNumberOfV0s();
  for (Int_t i=nV0s-1; i>=0; i--) {
    AliESDv0 *v0=GetV0(i);

    Float_t dca=v0->GetDcaV0Daughters();
    Float_t csp=v0->GetV0CosineOfPointingAngle();
    Float_t cspcut=cspMin + dca/dcaMax*(1.-cspMin);
    if (csp > cspcut) continue;
    if (RemoveV0(i)) rc=kTRUE;
  }


  Float_t dmax=cleanPars[2], zmax=cleanPars[3];

  const AliESDVertex *vertex=GetPrimaryVertexSPD();
  Bool_t vtxOK=vertex->GetStatus();
  
  Int_t nTracks=GetNumberOfTracks();
  for (Int_t i=nTracks-1; i>=0; i--) {
    AliESDtrack *track=GetTrack(i);
    if (!track) {AliFatal(Form("NULL pointer for ESD track %d",i));}
    Float_t xy,z; track->GetImpactParameters(xy,z);
    if ((TMath::Abs(xy) > dmax) || (vtxOK && (TMath::Abs(z) > zmax))) {
      if (RemoveTrack(i)) rc=kTRUE;
    }
  }

  return rc;
}

//______________________________________________________________________________
Char_t  AliESDEvent::AddPileupVertexSPD(const AliESDVertex *vtx) 
{
    // Add a pileup primary vertex reconstructed with SPD
    TClonesArray &ftr = *fSPDPileupVertices;
    Char_t n=Char_t(ftr.GetEntriesFast());
    AliESDVertex *vertex = new(ftr[n]) AliESDVertex(*vtx);
    vertex->SetID(n);
    return n;
}

//______________________________________________________________________________
Char_t  AliESDEvent::AddPileupVertexTracks(const AliESDVertex *vtx) 
{
    // Add a pileup primary vertex reconstructed with SPD
    TClonesArray &ftr = *fTrkPileupVertices;
    Char_t n=Char_t(ftr.GetEntriesFast());
    AliESDVertex *vertex = new(ftr[n]) AliESDVertex(*vtx);
    vertex->SetID(n);
    return n;
}

//______________________________________________________________________________
Int_t  AliESDEvent::AddTrack(const AliESDtrack *t) 
{
    // Add track
    TClonesArray &ftr = *fTracks;
    AliESDtrack * track = new(ftr[fTracks->GetEntriesFast()])AliESDtrack(*t);
    track->SetID(fTracks->GetEntriesFast()-1);
    track->SetESDEvent(this);
    return  track->GetID();    
}

//______________________________________________________________________________
AliESDtrack*  AliESDEvent::NewTrack() 
{
    // Add a new track
    TClonesArray &ftr = *fTracks;
    AliESDtrack * track = new(ftr[fTracks->GetEntriesFast()])AliESDtrack();
    track->SetID(fTracks->GetEntriesFast()-1);
    track->SetESDEvent(this);
    return  track;
}

//______________________________________________________________________________
Bool_t AliESDEvent::MoveMuonObjects() 
{
  // move MUON clusters and pads to the new ESD structure in needed.
  // to ensure backward compatibility
  
  if (!fOldMuonStructure) return kTRUE;
  
  if (!fMuonTracks || !fMuonClusters || !fMuonPads) return kFALSE;
  
  Bool_t reset = kTRUE;
  Bool_t containTrackerData = kFALSE;
  for (Int_t i = 0; i < fMuonTracks->GetEntriesFast(); i++) {
    
    AliESDMuonTrack *track = (AliESDMuonTrack*) fMuonTracks->UncheckedAt(i);
    
    if (track->ContainTrackerData()) containTrackerData = kTRUE;
    else continue;
    
    if (!track->IsOldTrack()) continue;
    
    // remove objects connected to previous event if needed
    if (reset) {
      if (fMuonClusters->GetEntriesFast() > 0) fMuonClusters->Clear("C");
      if (fMuonPads->GetEntriesFast() > 0) fMuonPads->Clear("C");
      reset = kFALSE;
    }
    
    track->MoveClustersToESD(*this);
    
  }
  
  // remove objects connected to previous event if needed
  if (!containTrackerData) {
    if (fMuonClusters->GetEntriesFast() > 0) fMuonClusters->Clear("C");
    if (fMuonPads->GetEntriesFast() > 0) fMuonPads->Clear("C");
  }
  
  return kTRUE;
}

//______________________________________________________________________________
AliESDMuonTrack* AliESDEvent::GetMuonTrack(Int_t i)
{
  // get the MUON track at the position i in the internal array of track
  if (!fMuonTracks) return 0x0;
  if (!MoveMuonObjects()) return 0x0;
  AliESDMuonTrack *track = (AliESDMuonTrack*) fMuonTracks->UncheckedAt(i);
  track->SetESDEvent(this);
  return track;
}

//______________________________________________________________________________
AliESDMuonGlobalTrack* AliESDEvent::GetMuonGlobalTrack(Int_t i)                      // AU
{
  // get the MUON+MFT track at the position i in the internal array of track
  if (!fMuonGlobalTracks) return 0x0;
  AliESDMuonGlobalTrack *track = (AliESDMuonGlobalTrack*) fMuonGlobalTracks->UncheckedAt(i);
  track->SetESDEvent(this);
  return track;
}

//______________________________________________________________________________
void AliESDEvent::AddMuonTrack(const AliESDMuonTrack *t) 
{
  // add a MUON track
  TClonesArray &fmu = *fMuonTracks;
  AliESDMuonTrack *track = new(fmu[fMuonTracks->GetEntriesFast()]) AliESDMuonTrack(*t);
  track->MoveClustersToESD(*this);
}

//______________________________________________________________________________
void AliESDEvent::AddMuonGlobalTrack(const AliESDMuonGlobalTrack *t)                             // AU
{
  // add a MUON+MFT track
  TClonesArray &fmu = *fMuonGlobalTracks;
  new (fmu[fMuonGlobalTracks->GetEntriesFast()]) AliESDMuonGlobalTrack(*t);
}

//______________________________________________________________________________

AliESDMuonTrack* AliESDEvent::NewMuonTrack() 
{
  // create a new MUON track at the end of the internal array of track
  TClonesArray &fmu = *fMuonTracks;
  return new(fmu[fMuonTracks->GetEntriesFast()]) AliESDMuonTrack();
}

//______________________________________________________________________________
AliESDMuonGlobalTrack* AliESDEvent::NewMuonGlobalTrack()                                         // AU
{
  // create a new MUON+MFT track at the end of the internal array of track
  TClonesArray &fmu = *fMuonGlobalTracks;
  return new(fmu[fMuonGlobalTracks->GetEntriesFast()]) AliESDMuonGlobalTrack();
}

//______________________________________________________________________________
Int_t AliESDEvent::GetNumberOfMuonClusters()
{
  // get the number of MUON clusters
  if (!fMuonClusters) return 0;
  if (!MoveMuonObjects()) return 0;
  return fMuonClusters->GetEntriesFast();
}

//______________________________________________________________________________
AliESDMuonCluster* AliESDEvent::GetMuonCluster(Int_t i)
{
  // get the MUON cluster at the position i in the internal array of cluster
  if (!fMuonClusters) return 0x0;
  if (!MoveMuonObjects()) return 0x0;
  return (AliESDMuonCluster*) fMuonClusters->UncheckedAt(i);
}

//______________________________________________________________________________
AliESDMuonCluster* AliESDEvent::FindMuonCluster(UInt_t clusterId)
{
  // find the MUON cluster with this Id in the internal array of cluster
  if (!fMuonClusters) return 0x0;
  if (!MoveMuonObjects()) return 0x0;
  for (Int_t i = 0; i < fMuonClusters->GetEntriesFast(); i++) {
    AliESDMuonCluster *cluster = (AliESDMuonCluster*) fMuonClusters->UncheckedAt(i);
    if (cluster->GetUniqueID() == clusterId) return cluster;
  }
  return 0x0;
}

//______________________________________________________________________________
AliESDMuonCluster* AliESDEvent::NewMuonCluster() 
{
  // create a new MUON cluster at the end of the internal array of cluster
  TClonesArray &fmu = *fMuonClusters;
  return new(fmu[fMuonClusters->GetEntriesFast()]) AliESDMuonCluster();
}

//______________________________________________________________________________
Int_t AliESDEvent::GetNumberOfMuonPads()
{
  // get the number of MUON pads
  if (!fMuonPads) return 0;
  if (!MoveMuonObjects()) return 0;
  return fMuonPads->GetEntriesFast();
}

//______________________________________________________________________________
AliESDMuonPad* AliESDEvent::GetMuonPad(Int_t i)
{
  // get the MUON pad at the position i in the internal array of pad
  if (!fMuonPads) return 0x0;
  if (!MoveMuonObjects()) return 0x0;
  return (AliESDMuonPad*) fMuonPads->UncheckedAt(i);
}

//______________________________________________________________________________
AliESDMuonPad* AliESDEvent::FindMuonPad(UInt_t padId)
{
  // find the MUON pad with this Id in the internal array of pad
  if (!fMuonPads) return 0x0;
  if (!MoveMuonObjects()) return 0x0;
  for (Int_t i = 0; i < fMuonPads->GetEntriesFast(); i++) {
    AliESDMuonPad *pad = (AliESDMuonPad*) fMuonPads->UncheckedAt(i);
    if (pad->GetUniqueID() == padId) return pad;
  }
  return 0x0;
}

//______________________________________________________________________________
AliESDMuonPad* AliESDEvent::NewMuonPad() 
{
  // create a new MUON pad at the end of the internal array of pad
  TClonesArray &fmu = *fMuonPads;
  return new(fmu[fMuonPads->GetEntriesFast()]) AliESDMuonPad();
}

//______________________________________________________________________________
void AliESDEvent::AddPmdTrack(const AliESDPmdTrack *t) 
{
  TClonesArray &fpmd = *fPmdTracks;
  new(fpmd[fPmdTracks->GetEntriesFast()]) AliESDPmdTrack(*t);
}

//______________________________________________________________________________
void AliESDEvent::SetTrdTrigger(const AliESDTrdTrigger *t)
{
  *fTrdTrigger = *t;
}

//______________________________________________________________________________
void AliESDEvent::AddTrdTrack(const AliESDTrdTrack *t) 
{
  TClonesArray &ftrd = *fTrdTracks;
  new(ftrd[fTrdTracks->GetEntriesFast()]) AliESDTrdTrack(*t);
}

//______________________________________________________________________________
void AliESDEvent::AddTrdTracklet(const AliESDTrdTracklet *trkl)
{
  new ((*fTrdTracklets)[fTrdTracklets->GetEntriesFast()]) AliESDTrdTracklet(*trkl);
}

//______________________________________________________________________________
void AliESDEvent::AddTrdTracklet(UInt_t trackletWord, Short_t hcid, Int_t label)
{
  new ((*fTrdTracklets)[fTrdTracklets->GetEntriesFast()]) AliESDTrdTracklet(trackletWord, hcid, label);
}

//______________________________________________________________________________
Int_t AliESDEvent::AddKink(const AliESDkink *c) 
{
  // Add kink
  TClonesArray &fk = *fKinks;
  AliESDkink * kink = new(fk[fKinks->GetEntriesFast()]) AliESDkink(*c);
  kink->SetID(fKinks->GetEntriesFast()); // CKB different from the other imps..
  return fKinks->GetEntriesFast()-1;
}


//______________________________________________________________________________
void AliESDEvent::AddCascade(const AliESDcascade *c) 
{
  TClonesArray &fc = *fCascades;
  new(fc[fCascades->GetEntriesFast()]) AliESDcascade(*c);
}

//______________________________________________________________________________
void AliESDEvent::AddCosmicTrack(const AliESDCosmicTrack *t) 
{
  TClonesArray &ft = *fCosmicTracks;
  new(ft[fCosmicTracks->GetEntriesFast()]) AliESDCosmicTrack(*t);
} 


//______________________________________________________________________________
Int_t AliESDEvent::AddCaloCluster(const AliESDCaloCluster *c) 
{
  // Add calocluster
  TClonesArray &fc = *fCaloClusters;
  AliESDCaloCluster *clus = new(fc[fCaloClusters->GetEntriesFast()]) AliESDCaloCluster(*c);
  clus->SetID(fCaloClusters->GetEntriesFast()-1);
  return fCaloClusters->GetEntriesFast()-1;
}


//______________________________________________________________________________
void  AliESDEvent::AddRawDataErrorLog(const AliRawDataErrorLog *log) const {
  TClonesArray &errlogs = *fErrorLogs;
  new(errlogs[errlogs.GetEntriesFast()])  AliRawDataErrorLog(*log);
}

//______________________________________________________________________________
void AliESDEvent::SetZDCData(const AliESDZDC * obj)
{ 
  // use already allocated space
  if(fESDZDC)
    *fESDZDC = *obj;
}

//______________________________________________________________________________
void  AliESDEvent::SetPrimaryVertexTPC(const AliESDVertex *vertex) 
{
  // Set the TPC vertex
  // use already allocated space
  if(fTPCVertex){
    *fTPCVertex = *vertex;
    fTPCVertex->SetName(fgkESDListName[kTPCVertex]);
  }
}

//______________________________________________________________________________
void  AliESDEvent::SetPrimaryVertexSPD(const AliESDVertex *vertex) 
{
  // Set the SPD vertex
  // use already allocated space
  if(fSPDVertex){
    *fSPDVertex = *vertex;
    fSPDVertex->SetName(fgkESDListName[kSPDVertex]);
  }
}

//______________________________________________________________________________
void  AliESDEvent::SetPrimaryVertexTracks(const AliESDVertex *vertex) 
{
  // Set the primary vertex reconstructed using he ESD tracks.
  // use already allocated space
  if(fPrimaryVertex){
    *fPrimaryVertex = *vertex;
    fPrimaryVertex->SetName(fgkESDListName[kPrimaryVertex]);
  }
}

//______________________________________________________________________________
const AliESDVertex * AliESDEvent::GetPrimaryVertex() const 
{
  //
  // Get the "best" available reconstructed primary vertex.
  //
  if(fPrimaryVertex){
    if (fPrimaryVertex->GetStatus()) return fPrimaryVertex;
  }
  if(fSPDVertex){
    if (fSPDVertex->GetStatus()) return fSPDVertex;
  }
  if(fTPCVertex) return fTPCVertex;
  
  AliWarning("No primary vertex available. Returning the \"default\"...");
  return fSPDVertex;
}

//______________________________________________________________________________
AliESDVertex * AliESDEvent::PrimaryVertexTracksUnconstrained() const 
{
  //
  // Removes diamond constraint from fPrimaryVertex (reconstructed with tracks)
  // Returns a AliESDVertex which has to be deleted by the user
  //
  if(!fPrimaryVertex) {
    AliWarning("No primary vertex from tracks available.");
    return 0;
  }
  if(!fPrimaryVertex->GetStatus()) {
    AliWarning("No primary vertex from tracks available.");
    return 0;
  }

  AliVertexerTracks vertexer(GetMagneticField());
  Float_t diamondxyz[3]={(Float_t)GetDiamondX(),(Float_t)GetDiamondY(),0.};
  Float_t diamondcovxy[3]; GetDiamondCovXY(diamondcovxy);
  Float_t diamondcov[6]={diamondcovxy[0],diamondcovxy[1],diamondcovxy[2],0.,0.,7.};
  AliESDVertex *vertex = 
    (AliESDVertex*)vertexer.RemoveConstraintFromVertex(fPrimaryVertex,diamondxyz,diamondcov);

  return vertex;
}

//______________________________________________________________________________
void AliESDEvent::SetMultiplicity(const AliMultiplicity *mul) 
{
  // Set the SPD Multiplicity
  if(fSPDMult){
    *fSPDMult = *mul;
  }
}


//______________________________________________________________________________
void AliESDEvent::SetFMDData(AliESDFMD * obj) 
{ 
  // use already allocated space
  if(fESDFMD){
    *fESDFMD = *obj;
  }
}

//______________________________________________________________________________
void AliESDEvent::SetVZEROData(const AliESDVZERO * obj)
{ 
  // use already allocated space
  if(fESDVZERO)
    *fESDVZERO = *obj;
}

//______________________________________________________________________________
void AliESDEvent::SetTZEROData(const AliESDTZERO * obj)
{ 
  // use already allocated space
  if(fESDTZERO)
    *fESDTZERO = *obj;
}

//______________________________________________________________________________
void AliESDEvent::SetFITData(const AliESDFIT * obj)
{ 
  // use already allocated space
  if(fESDFIT)
    *fESDFIT = *obj;
}


//______________________________________________________________________________
void AliESDEvent::SetACORDEData(AliESDACORDE * obj)
{
  if(fESDACORDE)
    *fESDACORDE = *obj;
}

//______________________________________________________________________________
void AliESDEvent::SetADData(AliESDAD * obj)
{
  if(fESDAD)
    *fESDAD = *obj;
}

//______________________________________________________________________________
void AliESDEvent::GetESDfriend(AliESDfriend *ev) const 
{
  //
  // Extracts the complementary info from the ESD
  //
  if (!ev) return;

  Int_t ntrk=GetNumberOfTracks();

  for (Int_t i=0; i<ntrk; i++) {
    AliESDtrack *t=GetTrack(i);
    if (!t) {AliFatal(Form("NULL pointer for ESD track %d",i));}
    if (!t->GetFriendNotStored()) {
      const AliESDfriendTrack *f=t->GetFriendTrack();
      ev->AddTrack(f);
      // RS: now we store the pointer, no copy
      t->ReleaseESDfriendTrack();// Not to have two copies of "friendTrack" 
    }
  }
  AliESDfriend *fr = (AliESDfriend*)(const_cast<AliESDEvent*>(this)->FindFriend());
  if (fr) ev->SetVZEROfriend(fr->GetVZEROfriend());
}

//______________________________________________________________________________
void AliESDEvent::AddObject(TObject* obj) 
{
  // Add an object to the list of object.
  // Please be aware that in order to increase performance you should
  // refrain from using TObjArrays (if possible). Use TClonesArrays, instead.
  fESDObjects->SetOwner(kTRUE);
  fESDObjects->AddLast(obj);
}

//______________________________________________________________________________
void AliESDEvent::GetStdContent() 
{
  // set pointers for standard content
  // get by name much safer and not a big overhead since not called very often
 
  fESDRun = (AliESDRun*)fESDObjects->FindObject(fgkESDListName[kESDRun]);
  fHeader = (AliESDHeader*)fESDObjects->FindObject(fgkESDListName[kHeader]);
  fESDZDC = (AliESDZDC*)fESDObjects->FindObject(fgkESDListName[kESDZDC]);
  fESDFMD = (AliESDFMD*)fESDObjects->FindObject(fgkESDListName[kESDFMD]);
  fESDVZERO = (AliESDVZERO*)fESDObjects->FindObject(fgkESDListName[kESDVZERO]);
  fESDTZERO = (AliESDTZERO*)fESDObjects->FindObject(fgkESDListName[kESDTZERO]);
  fESDFIT = (AliESDFIT*)fESDObjects->FindObject(fgkESDListName[kESDFIT]);
  fTPCVertex = (AliESDVertex*)fESDObjects->FindObject(fgkESDListName[kTPCVertex]);
  fSPDVertex = (AliESDVertex*)fESDObjects->FindObject(fgkESDListName[kSPDVertex]);
  fPrimaryVertex = (AliESDVertex*)fESDObjects->FindObject(fgkESDListName[kPrimaryVertex]);
  fSPDMult =       (AliMultiplicity*)fESDObjects->FindObject(fgkESDListName[kSPDMult]);
  fPHOSTrigger = (AliESDCaloTrigger*)fESDObjects->FindObject(fgkESDListName[kPHOSTrigger]);
  fEMCALTrigger = (AliESDCaloTrigger*)fESDObjects->FindObject(fgkESDListName[kEMCALTrigger]);
  fSPDPileupVertices = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kSPDPileupVertices]);
  fTrkPileupVertices = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kTrkPileupVertices]);
  fTracks = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kTracks]);
  fMuonTracks = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kMuonTracks]);
  fMuonClusters = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kMuonClusters]);
  fMuonPads = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kMuonPads]);
  fMuonGlobalTracks = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kMuonGlobalTracks]);         // AU
  fPmdTracks = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kPmdTracks]);
  fTrdTrigger = (AliESDTrdTrigger*)fESDObjects->FindObject(fgkESDListName[kTrdTrigger]);
  fTrdTracks = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kTrdTracks]);
  fTrdTracklets = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kTrdTracklets]);
  fV0s = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kV0s]);
  fCascades = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kCascades]);
  fKinks = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kKinks]);
  fCaloClusters = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kCaloClusters]);
  fEMCALCells = (AliESDCaloCells*)fESDObjects->FindObject(fgkESDListName[kEMCALCells]);
  fPHOSCells = (AliESDCaloCells*)fESDObjects->FindObject(fgkESDListName[kPHOSCells]);
  fErrorLogs = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kErrorLogs]);
  fESDACORDE = (AliESDACORDE*)fESDObjects->FindObject(fgkESDListName[kESDACORDE]);
  fESDAD = (AliESDAD*)fESDObjects->FindObject(fgkESDListName[kESDAD]);
  fTOFHeader = (AliTOFHeader*)fESDObjects->FindObject(fgkESDListName[kTOFHeader]);
  fCosmicTracks = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kCosmicTracks]);
  fESDTOFClusters = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kTOFclusters]);
  fESDTOFHits = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kTOFhit]);
  fESDTOFMatches = (TClonesArray*)fESDObjects->FindObject(fgkESDListName[kTOFmatch]);
}

//______________________________________________________________________________
void AliESDEvent::SetStdNames(){
  // Set the names of the standard contents
  // 
  if(fESDObjects->GetEntries()>=kESDListN){
    for(int i = 0;i < fESDObjects->GetEntries() && i<kESDListN;i++){
      TObject *fObj = fESDObjects->At(i);
      if(fObj->InheritsFrom("TNamed")){
	((TNamed*)fObj)->SetName(fgkESDListName[i]);
      }
      else if(fObj->InheritsFrom("TClonesArray")){
	((TClonesArray*)fObj)->SetName(fgkESDListName[i]);
      }
    }
  }
  else{
     AliWarning("Std Entries missing");
  }
} 

//______________________________________________________________________________
void AliESDEvent::CreateStdContent(Bool_t bUseThisList){
  fUseOwnList = bUseThisList;
  CreateStdContent();
}

//______________________________________________________________________________
void AliESDEvent::CreateStdContent() 
{
  // create the standard AOD content and set pointers

  // create standard objects and add them to the TList of objects
  AddObject(new AliESDRun());
  AddObject(new AliESDHeader());
  AddObject(new AliESDZDC());
  AddObject(new AliESDFMD());
  AddObject(new AliESDVZERO());
  AddObject(new AliESDTZERO());
  AddObject(new AliESDVertex());
  AddObject(new AliESDVertex());
  AddObject(new AliESDVertex());
  AddObject(new AliMultiplicity());
  AddObject(new AliESDCaloTrigger());
  AddObject(new AliESDCaloTrigger());
  AddObject(new TClonesArray("AliESDVertex",0));
  AddObject(new TClonesArray("AliESDVertex",0));
  AddObject(new TClonesArray("AliESDtrack",0));
  AddObject(new TClonesArray("AliESDMuonTrack",0));
  AddObject(new TClonesArray("AliESDMuonCluster",0));
  AddObject(new TClonesArray("AliESDMuonPad",0));
  AddObject(new TClonesArray("AliESDMuonGlobalTrack",0));   // AU
  AddObject(new TClonesArray("AliESDPmdTrack",0));
  AddObject(new AliESDTrdTrigger());
  AddObject(new TClonesArray("AliESDTrdTrack",0));
  AddObject(new TClonesArray("AliESDTrdTracklet",0));
  AddObject(new TClonesArray("AliESDv0",0));
  AddObject(new TClonesArray("AliESDcascade",0));
  AddObject(new TClonesArray("AliESDkink",0));
  AddObject(new TClonesArray("AliESDCaloCluster",0));
  AddObject(new AliESDCaloCells());
  AddObject(new AliESDCaloCells());
  AddObject(new TClonesArray("AliRawDataErrorLog",0));
  AddObject(new AliESDACORDE()); 
  AddObject(new AliESDAD()); 
  AddObject(new AliTOFHeader());
  AddObject(new TClonesArray("AliESDCosmicTrack",0));
  AddObject(new TClonesArray("AliESDTOFCluster",0));
  AddObject(new TClonesArray("AliESDTOFHit",0));
  AddObject(new TClonesArray("AliESDTOFMatch",0));
  AddObject(new AliESDFIT());	
  // check the order of the indices against enum...

  // set names
  SetStdNames();
  // read back pointers
  GetStdContent();
}

//______________________________________________________________________________
void AliESDEvent::CompleteStdContent() 
{
  // Create missing standard objects and add them to the TList of objects
  //
  // Add cosmic tracks for cases where esd files were created 
  // before adding them to the std content
  if (!fESDObjects->FindObject(fgkESDListName[kCosmicTracks])) {
    TClonesArray* cosmics = new TClonesArray("AliESDCosmicTrack",0);
    fESDObjects->AddAt(cosmics, kCosmicTracks);
    fESDObjects->SetOwner(kTRUE);
  }
  // Add new MUON containers if missing (for backward compatibility)
  if (!fESDObjects->FindObject(fgkESDListName[kMuonClusters])) {
    TClonesArray* muonClusters = new TClonesArray("AliESDMuonCluster",0);
    muonClusters->SetName(fgkESDListName[kMuonClusters]);
    fESDObjects->AddAt(muonClusters, kMuonClusters);
    fESDObjects->SetOwner(kTRUE);
  }
  if (!fESDObjects->FindObject(fgkESDListName[kMuonPads])) {
    TClonesArray* muonPads = new TClonesArray("AliESDMuonPad",0);
    muonPads->SetName(fgkESDListName[kMuonPads]);
    fESDObjects->AddAt(muonPads, kMuonPads);
    fESDObjects->SetOwner(kTRUE);
  }
}

//______________________________________________________________________________
TObject* AliESDEvent::FindListObject(const char *name) const {
//
// Find object with name "name" in the list of branches
//
  if(fESDObjects){
    return fESDObjects->FindObject(name);
  }
  return 0;
} 

//______________________________________________________________________________
Int_t AliESDEvent::GetPHOSClusters(TRefArray *clusters) const
{
  // fills the provided TRefArray with all found phos clusters
  
  clusters->Clear();
  
  AliESDCaloCluster *cl = 0;
  for (Int_t i = 0; i < GetNumberOfCaloClusters(); i++) {
    
    if ( (cl = GetCaloCluster(i)) ) {
      if (cl->IsPHOS()){
	clusters->Add(cl);
	AliDebug(1,Form("IsPHOS cluster %d Size: %d \n",i,clusters->GetEntriesFast()));
      }
    }
  }
  return clusters->GetEntriesFast();
}

//______________________________________________________________________________
Int_t AliESDEvent::GetEMCALClusters(TRefArray *clusters) const
{
  // fills the provided TRefArray with all found emcal clusters

  clusters->Clear();

  AliESDCaloCluster *cl = 0;
  for (Int_t i = 0; i < GetNumberOfCaloClusters(); i++) {

    if ( (cl = GetCaloCluster(i)) ) {
      if (cl->IsEMCAL()){
	clusters->Add(cl);
	AliDebug(1,Form("IsEMCAL cluster %d Size: %d \n",i,clusters->GetEntriesFast()));
      }
    }
  }
  return clusters->GetEntriesFast();
}

//______________________________________________________________________________
void AliESDEvent::WriteToTree(TTree* tree) const {
  // Book the branches as in TTree::Branch(TCollection*)
  // but add a "." at the end of top level branches which are
  // not a TClonesArray


  TString branchname;
  TIter next(fESDObjects);
  const Int_t kSplitlevel = 99; // default value in TTree::Branch()
  const Int_t kBufsize = 32000; // default value in TTree::Branch()
  TObject *obj = 0;

  while ((obj = next())) {
    branchname.Form("%s", obj->GetName());
    if(branchname.CompareTo("AliESDfriend")==0)branchname = "ESDfriend.";
    if ((kSplitlevel > 1) &&  !obj->InheritsFrom(TClonesArray::Class())) {
      if(!branchname.EndsWith("."))branchname += ".";
    }
    if (!tree->FindBranch(branchname)) {
      // For the custom streamer to be called splitlevel
      // has to be negative, only needed for HLT
      Int_t splitLevel = (TString(obj->ClassName()) == "AliHLTGlobalTriggerDecision") ? -1 : kSplitlevel - 1;
      tree->Bronch(branchname, obj->ClassName(), fESDObjects->GetObjectRef(obj),kBufsize, splitLevel);
    }
  }

  tree->Branch("fDetectorStatus",(void*)&fDetectorStatus,"fDetectorStatus/l");
  tree->Branch("fDAQDetectorPattern",(void*)&fDAQDetectorPattern,"fDAQDetectorPattern/i");
  tree->Branch("fDAQAttributes",(void*)&fDAQAttributes,"fDAQAttributes/i");
}

//______________________________________________________________________________
void AliESDEvent::ReadFromTree(TTree *tree, Option_t* opt){
//
// Connect the ESDEvent to a tree
//
  if(!tree){
    AliWarning("AliESDEvent::ReadFromTree() Zero Pointer to Tree \n");
    return;
  }
  // load the TTree
  if(!tree->GetTree())tree->LoadTree(0);

  // if we find the "ESD" branch on the tree we do have the old structure
  if(tree->GetBranch("ESD")) {
    fOldMuonStructure = kFALSE;
    char ** address  = (char **)(tree->GetBranch("ESD")->GetAddress());
    // do we have the friend branch
    TBranch * esdFB = tree->GetBranch("ESDfriend.");
    char ** addressF = 0;
    if(esdFB)addressF = (char **)(esdFB->GetAddress());
    if (!address) {
      AliInfo("AliESDEvent::ReadFromTree() Reading old Tree");
      tree->SetBranchAddress("ESD",       &fESDOld);
      if(esdFB){
	tree->SetBranchAddress("ESDfriend.",&fESDFriendOld);
      }
    } else {
      AliInfo("AliESDEvent::ReadFromTree() Reading old Tree");
      AliInfo("Branch already connected. Using existing branch address.");
      fESDOld       = (AliESD*)       (*address);
      // addressF can still be 0, since branch needs to switched on
      if(addressF)fESDFriendOld = (AliESDfriend*) (*addressF);
    }
				       
    //  have already connected the old ESD structure... ?
    // reuse also the pointer of the AlliESDEvent
    // otherwise create new ones
    TList* connectedList = (TList*) (tree->GetUserInfo()->FindObject("ESDObjectsConnectedToTree"));
  
    if(connectedList){
      // If connected use the connected list of objects
      if(fESDObjects!= connectedList){
	// protect when called twice 
	fESDObjects->Delete();
	fESDObjects = connectedList;
      }
      GetStdContent(); 

      
      // The pointer to the friend changes when called twice via InitIO
      // since AliESDEvent is deleted
      TObject* oldf = FindListObject("AliESDfriend");
      TObject* newf = 0;
      if(addressF){
	newf = (TObject*)*addressF;
      }
      if(newf!=0&&oldf!=newf){
	// remove the old reference
	// Should we also delete it? Or is this handled in TTree I/O
	// since it is created by the first SetBranchAddress
	fESDObjects->Remove(oldf);
	// add the new one 
	fESDObjects->Add(newf);
      }
      
      fConnected = true;
      return;
    }
    // else...    
    CreateStdContent(); // create for copy
    // if we have the esdfriend add it, so we always can access it via the userinfo
    if(fESDFriendOld)AddObject(fESDFriendOld);
    // we are not owner of the list objects 
    // must not delete it
    fESDObjects->SetOwner(kTRUE);
    fESDObjects->SetName("ESDObjectsConnectedToTree");
    tree->GetUserInfo()->Add(fESDObjects);
    fConnected = true;
    return;
  }
  

    delete fESDOld;
    fESDOld = 0;
  // Try to find AliESDEvent
  AliESDEvent *esdEvent = 0;
  esdEvent = (AliESDEvent*)tree->GetTree()->GetUserInfo()->FindObject("AliESDEvent");
  if(esdEvent){   
      // Check if already connected to tree
    esdEvent->Reset();
    TList* connectedList = (TList*) (tree->GetUserInfo()->FindObject("ESDObjectsConnectedToTree"));

    
    if (connectedList && (strcmp(opt, "reconnect"))) {
      // If connected use the connected list if objects
      fESDObjects->Delete();
      fESDObjects = connectedList;
      tree->SetBranchAddress("fDetectorStatus",&fDetectorStatus); //PH probably redundant
      tree->SetBranchAddress("fDAQDetectorPattern",&fDAQDetectorPattern);
      tree->SetBranchAddress("fDAQAttributes",&fDAQAttributes);
      GetStdContent(); 
      fOldMuonStructure = fESDObjects->TestBit(BIT(23));
      fConnected = true;
      return;
    }

    // Connect to tree
    // prevent a memory leak when reading back the TList
    // if (!(strcmp(opt, "reconnect"))) fESDObjects->Delete();
    
    if(!fUseOwnList){
      // create a new TList from the UserInfo TList... 
      // copy constructor does not work...
      fESDObjects = (TList*)(esdEvent->GetList()->Clone());
      fESDObjects->SetOwner(kTRUE);
    }
    else if ( fESDObjects->GetEntries()==0){
      // at least create the std content if we want to read to our list
      CreateStdContent(); 
    }

    // in principle
    // we only need new things in the list if we do no already have it..
    // TODO just add new entries
    CompleteStdContent();

    if(fESDObjects->GetEntries()<kESDListN){
      AliWarning(Form("AliESDEvent::ReadFromTree() TList contains less than the standard contents %d < %d \n",
		      fESDObjects->GetEntries(),kESDListN));
    }
    // set the branch addresses
    fOldMuonStructure = kFALSE;
    TIter next(fESDObjects);
    TNamed *el;
    while((el=(TNamed*)next())){
      TString bname(el->GetName());
      if(bname.CompareTo("AliESDfriend")==0)
	{
	  // AliESDfriend does not have a name ...
	    TBranch *br = tree->GetBranch("ESDfriend.");
	    if (br) tree->SetBranchAddress("ESDfriend.",fESDObjects->GetObjectRef(el));
	}
      else{
	// check if branch exists under this Name
        TBranch *br = tree->GetBranch(bname.Data());
        if(br){
          tree->SetBranchAddress(bname.Data(),fESDObjects->GetObjectRef(el));
        }
        else{
          br = tree->GetBranch(Form("%s.",bname.Data()));
          if(br){
            tree->SetBranchAddress(Form("%s.",bname.Data()),fESDObjects->GetObjectRef(el));
          }
          else{
            AliWarning(Form("AliESDEvent::ReadFromTree() No Branch found with Name %s or %s.",bname.Data(),bname.Data()));
	    if (bname == fgkESDListName[kMuonClusters]) {
	      fOldMuonStructure = kTRUE;
	    }
          }

	}
      }
    }
    tree->SetBranchAddress("fDetectorStatus",&fDetectorStatus);
    tree->SetBranchAddress("fDAQDetectorPattern",&fDAQDetectorPattern);
    tree->SetBranchAddress("fDAQAttributes",&fDAQAttributes);

    GetStdContent();
    // when reading back we are not owner of the list 
    // must not delete it
    fESDObjects->SetOwner(kTRUE);
    fESDObjects->SetName("ESDObjectsConnectedToTree");
    fESDObjects->SetBit(BIT(23), fOldMuonStructure);
    // we are not owner of the list objects 
    // must not delete it
    tree->GetUserInfo()->Add(fESDObjects);
    tree->GetUserInfo()->SetOwner(kFALSE);
    fConnected = true;
  }// no esdEvent -->
  else {
    // we can't get the list from the user data, create standard content
    // and set it by hand (no ESDfriend at the moment
    CreateStdContent();
    fOldMuonStructure = kFALSE;
    TIter next(fESDObjects);
    TNamed *el;
    while((el=(TNamed*)next())){
      TString bname(el->GetName());    
      TBranch *br = tree->GetBranch(bname.Data());
      if(br){
	tree->SetBranchAddress(bname.Data(),fESDObjects->GetObjectRef(el));
      }
      else{
	br = tree->GetBranch(Form("%s.",bname.Data()));
	if(br){
	  tree->SetBranchAddress(Form("%s.",bname.Data()),fESDObjects->GetObjectRef(el));
	}
	else if (bname == fgkESDListName[kMuonClusters]) {
	  fOldMuonStructure = kTRUE;
	}
      }
    }
    tree->SetBranchAddress("fDetectorStatus",&fDetectorStatus);
    tree->SetBranchAddress("fDAQDetectorPattern",&fDAQDetectorPattern);
    tree->SetBranchAddress("fDAQAttributes",&fDAQAttributes);

    GetStdContent();
    // when reading back we are not owner of the list 
    // must not delete it
    fESDObjects->SetOwner(kTRUE);
  }
}

//______________________________________________________________________________
void AliESDEvent::CopyFromOldESD()
{
  // Method which copies over everthing from the old esd structure to the 
  // new  
  if(fESDOld){
    ResetStdContent();
     // Run
    SetRunNumber(fESDOld->GetRunNumber());
    SetPeriodNumber(fESDOld->GetPeriodNumber());
    SetMagneticField(fESDOld->GetMagneticField());
  
    // leave out diamond ...
    // SetDiamond(const AliESDVertex *vertex) { fESDRun->SetDiamond(vertex);}

    // header
    SetTriggerMask(fESDOld->GetTriggerMask());
    SetOrbitNumber(fESDOld->GetOrbitNumber());
    SetTimeStamp(fESDOld->GetTimeStamp());
    SetEventType(fESDOld->GetEventType());
    SetEventNumberInFile(fESDOld->GetEventNumberInFile());
    SetBunchCrossNumber(fESDOld->GetBunchCrossNumber());
    SetTriggerCluster(fESDOld->GetTriggerCluster());

    // ZDC

    SetZDC(fESDOld->GetZDCN1Energy(),
           fESDOld->GetZDCP1Energy(),
           fESDOld->GetZDCEMEnergy(),
           0,
           fESDOld->GetZDCN2Energy(),
           fESDOld->GetZDCP2Energy(),
           fESDOld->GetZDCParticipants(),
	   0,
	   0,
	   0,
	   0,
	   0,
	   0);

    // FMD
    
    if(fESDOld->GetFMDData())SetFMDData(fESDOld->GetFMDData());

    // T0

    SetT0zVertex(fESDOld->GetT0zVertex());
    SetT0(fESDOld->GetT0());
    //  leave amps out

    // VZERO
    if (fESDOld->GetVZEROData()) SetVZEROData(fESDOld->GetVZEROData());

    if(fESDOld->GetVertex())SetPrimaryVertexSPD(fESDOld->GetVertex());

    if(fESDOld->GetPrimaryVertex())SetPrimaryVertexTracks(fESDOld->GetPrimaryVertex());

    if(fESDOld->GetMultiplicity())SetMultiplicity(fESDOld->GetMultiplicity());

    for(int i = 0;i<fESDOld->GetNumberOfTracks();i++){
      AddTrack(fESDOld->GetTrack(i));
    }

    for(int i = 0;i<fESDOld->GetNumberOfMuonTracks();i++){
      AddMuonTrack(fESDOld->GetMuonTrack(i));
    }

    for(int i = 0;i<fESDOld->GetNumberOfPmdTracks();i++){
      AddPmdTrack(fESDOld->GetPmdTrack(i));
    }

    for(int i = 0;i<fESDOld->GetNumberOfTrdTracks();i++){
      AddTrdTrack(fESDOld->GetTrdTrack(i));
    }

    for(int i = 0;i<fESDOld->GetNumberOfV0s();i++){
      AddV0(fESDOld->GetV0(i));
    }

    for(int i = 0;i<fESDOld->GetNumberOfCascades();i++){
      AddCascade(fESDOld->GetCascade(i));
    }

    for(int i = 0;i<fESDOld->GetNumberOfKinks();i++){
      AddKink(fESDOld->GetKink(i));
    }


    for(int i = 0;i<fESDOld->GetNumberOfCaloClusters();i++){
      AddCaloCluster(fESDOld->GetCaloCluster(i));
    }
	  
  }// if fesdold
}

//______________________________________________________________________________
Bool_t AliESDEvent::IsEventSelected(const char *trigExpr) const
{
  // Check if the event satisfies the trigger
  // selection expression trigExpr.
  // trigExpr can be any logical expression
  // of the trigger classes defined in AliESDRun
  // In case of wrong syntax return kTRUE.
  // Modified by rl for 100 classes - to be tested

  TString expr(trigExpr);
  if (expr.IsNull()) return kTRUE;

  ULong64_t mask = GetTriggerMask();
  for(Int_t itrig = 0; itrig < AliESDRun::kNTriggerClasses/2; itrig++) {
    if (mask & (1ull << itrig)) {
      expr.ReplaceAll(GetESDRun()->GetTriggerClass(itrig),"1");
    }
    else {
      expr.ReplaceAll(GetESDRun()->GetTriggerClass(itrig),"0");
    }
  }
  ULong64_t maskNext50 = GetTriggerMaskNext50();
  for(Int_t itrig = 0; itrig < AliESDRun::kNTriggerClasses/2; itrig++) {
    if (maskNext50 & (1ull << itrig)) {
      expr.ReplaceAll(GetESDRun()->GetTriggerClass(itrig+50),"1");
    }
    else {
      expr.ReplaceAll(GetESDRun()->GetTriggerClass(itrig+50),"0");
    }
  }

  Int_t error;
  if ((gROOT->ProcessLineFast(expr.Data(),&error) == 0) &&
      (error == TInterpreter::kNoError)) {
    return kFALSE;
  }

  return kTRUE;

}

//______________________________________________________________________________
TObject*  AliESDEvent::GetHLTTriggerDecision() const
{
  // get the HLT trigger decission object

  // cast away const'nes because the FindListObject method
  // is not const
  AliESDEvent* pNonConst=const_cast<AliESDEvent*>(this);
  return pNonConst->FindListObject("HLTGlobalTrigger");
}

TString   AliESDEvent::GetHLTTriggerDescription() const
{
  // get the HLT trigger decission description
  TString description;
  TObject* pDecision=GetHLTTriggerDecision();
  if (pDecision) {
    description=pDecision->GetTitle();
  }

  return description;
}

//______________________________________________________________________________
Bool_t    AliESDEvent::IsHLTTriggerFired(const char* name) const
{
  // get the HLT trigger decission description
  TObject* pDecision=GetHLTTriggerDecision();
  if (!pDecision) return kFALSE;

  Option_t* option=pDecision->GetOption();
  if (option==NULL || *option!='1') return kFALSE;

  if (name) {
    TString description=GetHLTTriggerDescription();
    Int_t index=description.Index(name);
    if (index<0) return kFALSE;
    index+=strlen(name);
    if (index>=description.Length()) return kFALSE;
    if (description[index]!=0 && description[index]!=' ') return kFALSE;
  }
  return kTRUE;
}

//______________________________________________________________________________
Bool_t  AliESDEvent::IsPileupFromSPD(Int_t minContributors, 
				     Double_t minZdist, 
				     Double_t nSigmaZdist, 
				     Double_t nSigmaDiamXY, 
				     Double_t nSigmaDiamZ) const{
  //
  // This function checks if there was a pile up
  // reconstructed with SPD
  //
  Int_t nc1=fSPDVertex->GetNContributors();
  if(nc1<1) return kFALSE;
  Int_t nPileVert=GetNumberOfPileupVerticesSPD();
  if(nPileVert==0) return kFALSE;
  
  for(Int_t i=0; i<nPileVert;i++){
    const AliESDVertex* pv=GetPileupVertexSPD(i);
    Int_t nc2=pv->GetNContributors();
    if(nc2>=minContributors){
      Double_t z1=fSPDVertex->GetZ();
      Double_t z2=pv->GetZ();
      Double_t distZ=TMath::Abs(z2-z1);
      Double_t distZdiam=TMath::Abs(z2-GetDiamondZ());
      Double_t cutZdiam=nSigmaDiamZ*TMath::Sqrt(GetSigma2DiamondZ());
      if(GetSigma2DiamondZ()<0.0001)cutZdiam=99999.; //protection for missing z diamond information
      if(distZ>minZdist && distZdiam<cutZdiam){
	Double_t x2=pv->GetX();
	Double_t y2=pv->GetY();
	Double_t distXdiam=TMath::Abs(x2-GetDiamondX());
	Double_t distYdiam=TMath::Abs(y2-GetDiamondY());
	Double_t cov1[6],cov2[6];	
	fSPDVertex->GetCovarianceMatrix(cov1);
	pv->GetCovarianceMatrix(cov2);
	Double_t errxDist=TMath::Sqrt(cov2[0]+GetSigma2DiamondX());
	Double_t erryDist=TMath::Sqrt(cov2[2]+GetSigma2DiamondY());
	Double_t errzDist=TMath::Sqrt(cov1[5]+cov2[5]);
	Double_t cutXdiam=nSigmaDiamXY*errxDist;
	if(GetSigma2DiamondX()<0.0001)cutXdiam=99999.; //protection for missing diamond information
	Double_t cutYdiam=nSigmaDiamXY*erryDist;
	if(GetSigma2DiamondY()<0.0001)cutYdiam=99999.; //protection for missing diamond information
	if( (distXdiam<cutXdiam) && (distYdiam<cutYdiam) && (distZ>nSigmaZdist*errzDist) ){
	  return kTRUE;
	}
      }
    }
  }
  return kFALSE;
}

//______________________________________________________________________________
void AliESDEvent::EstimateMultiplicity(Int_t &tracklets, Int_t &trITSTPC, Int_t &trITSSApure, Double_t eta, Bool_t useDCAFlag,Bool_t useV0Flag) const
{
  //
  // calculates 3 estimators for the multiplicity in the -eta:eta range
  // tracklets   : using SPD tracklets only
  // trITSTPC    : using TPC/ITS + complementary ITS SA tracks + tracklets from clusters not used by tracks
  // trITSSApure : using ITS standalone tracks + tracklets from clusters not used by tracks
  // if useDCAFlag is true: account for the ESDtrack flag marking the tracks with large DCA
  // if useV0Flag  is true: account for the ESDtrack flag marking conversion and K0's V0s

  AliWarning("This obsolete method will be eliminated soon. Use AliESDtrackCuts::GetReferenceMultiplicity");

  tracklets = trITSSApure = trITSTPC = 0;
  int ntr = fSPDMult ? fSPDMult->GetNumberOfTracklets() : 0;
  //
  // count tracklets
  for (int itr=ntr;itr--;) { 
    if (TMath::Abs(fSPDMult->GetEta(itr))>eta) continue;
    tracklets++;
    if (fSPDMult->FreeClustersTracklet(itr,0)) trITSTPC++;    // not used in ITS/TPC or ITS_SA track
    if (fSPDMult->FreeClustersTracklet(itr,1)) trITSSApure++; // not used in ITS_SA_Pure track
  }
  //
  // count real tracks
  ntr = GetNumberOfTracks();
  for (int itr=ntr;itr--;) {
    AliESDtrack *t = GetTrack(itr);
    if (!t) {AliFatal(Form("NULL pointer for ESD track %d",itr));}
    if (TMath::Abs(t->Eta())>eta) continue;
    if (!t->IsOn(AliESDtrack::kITSin)) continue;
    if (useDCAFlag && t->IsOn(AliESDtrack::kMultSec))  continue;
    if (useV0Flag  && t->IsOn(AliESDtrack::kMultInV0)) continue;    
    if (t->IsOn(AliESDtrack::kITSpureSA)) trITSSApure++;
    else                                  trITSTPC++;
  }
  //
}

//______________________________________________________________________________
Bool_t AliESDEvent::IsPileupFromSPDInMultBins() const {
    Int_t nTracklets=GetMultiplicity()->GetNumberOfTracklets();
    if(nTracklets<20) return IsPileupFromSPD(3,0.8);
    else if(nTracklets<50) return IsPileupFromSPD(4,0.8);
    else return IsPileupFromSPD(5,0.8);
}

//______________________________________________________________________________
void  AliESDEvent::SetTOFHeader(const AliTOFHeader *header)
{
  //
  // Set the TOF event_time
  //

  if (fTOFHeader) {
    *fTOFHeader=*header;
    //fTOFHeader->SetName(fgkESDListName[kTOFHeader]);
  }
  else {
    // for analysis of reconstructed events
    // when this information is not avaliable
    fTOFHeader = new AliTOFHeader(*header);
    //AddObject(fTOFHeader);
  }

}

//______________________________________________________________________________
AliCentrality* AliESDEvent::GetCentrality()
{
    if (!fCentrality) fCentrality = new AliCentrality();
    return  fCentrality;
}

//______________________________________________________________________________
AliEventplane* AliESDEvent::GetEventplane()
{
    if (!fEventplane) fEventplane = new AliEventplane();
    return  fEventplane;
}

//______________________________________________________________________________
Float_t AliESDEvent::GetVZEROEqMultiplicity(Int_t i) const
{
  // Get VZERO Multiplicity for channel i
  // Themethod uses the equalization factors
  // stored in the ESD-run object in order to
  // get equal multiplicities within a VZERO rins (1/8 of VZERO)
  if (!fESDVZERO || !fESDRun) return -1;

  Int_t ring = i/8;
  Float_t factorSum = 0;
  for(Int_t j = 8*ring; j < (8*ring+8); ++j) {
    factorSum += fESDRun->GetVZEROEqFactors(j);
  }
  Float_t factor = fESDRun->GetVZEROEqFactors(i)*8./factorSum;

  return (fESDVZERO->GetMultiplicity(i)/factor);
}

//______________________________________________________________________________
void AliESDEvent::SetTOFcluster(Int_t ntofclusters,AliESDTOFCluster *cluster,Int_t *mapping)
{
  // Reset TClonesArray of TOF clusters
  if (!fESDTOFClusters) {
    AliError("fESDTOFClusters is not initialized");
    return;
  }
  fESDTOFClusters->Clear();
  
  Int_t goodhit[20000];
  if(mapping){
    for(Int_t i=0;i < 20000;i++){
      goodhit[i] = 0;
    }
  }

  for(Int_t i=0;i < ntofclusters;i++){
    
    if(cluster[i].GetNMatchableTracks() || !mapping){
      if(mapping)
	mapping[i] = fESDTOFClusters->GetEntriesFast();
      
      // update TClonesArray
      TClonesArray &ftr = *fESDTOFClusters;
      AliESDTOFCluster *clusterTBW = new(ftr[fESDTOFClusters->GetEntriesFast()])AliESDTOFCluster(cluster[i]);

      if(mapping){
	// loop over hit in the cluster
        for(Int_t k=0;k < clusterTBW->GetNTOFhits();k++){
	  Int_t ipos = clusterTBW->GetHitIndex(k);
	  goodhit[ipos] = 1; // hit should be kept
	}
      }
    }
  }

  if(mapping){
    AliInfo(Form("TOF cluster before of matching = %i , after = %i\n",ntofclusters,fESDTOFClusters->GetEntriesFast()));
    Int_t hitnewpos[20000]={0};
    Int_t nhitOriginal = fESDTOFHits->GetEntries();
    for(Int_t i=0;i < fESDTOFHits->GetEntries();i++){
      if(goodhit[i]){
	hitnewpos[i] = i;
      }
      else{ // remove hit and decrease the hit array
	TClonesArray &a=*fESDTOFHits;
	Int_t lastpos = fESDTOFHits->GetEntries()-1;

	if(i == lastpos)
	  delete a.RemoveAt(i);
	else{
	  Int_t nhitBefore = fESDTOFHits->GetEntries();
	  for(Int_t k=nhitBefore-1;k>i;k--){ // find the last good track
	    if(!goodhit[k]){ // remove track
	      delete a.RemoveAt(k);
	      if(k-i==1) delete a.RemoveAt(i);
	    }
	    else{ // replace last one to the "i"
	      AliESDTOFHit *last = (AliESDTOFHit *) fESDTOFHits->At(k);
	      delete a.RemoveAt(i);
	      new (a[i]) AliESDTOFHit(*last);
	      delete a.RemoveAt(k);
	      hitnewpos[k] = i;
	      k = 0;
	    }
	  }
	}
      }
    }

    // remap cluster to hits
    for(Int_t i=0;i < fESDTOFClusters->GetEntries();i++){
      AliESDTOFCluster *cl = (AliESDTOFCluster *) fESDTOFClusters->At(i);
      // loop over hit in the cluster
      for(Int_t k=0;k < cl->GetNTOFhits();k++){
	cl->SetHitIndex(k,hitnewpos[cl->GetHitIndex(k)]);
      }
    }
    AliInfo(Form("TOF hit before of matching = %i , after = %i\n",nhitOriginal,fESDTOFHits->GetEntriesFast()));
  } // end mapping

}

//______________________________________________________________________________
void AliESDEvent::SetTOFcluster(Int_t ntofclusters,AliESDTOFCluster *cluster[],Int_t *mapping)
{    
  // Reset TClonesArray of TOF clusters
  if(fESDTOFClusters)fESDTOFClusters->Delete();
   
  Int_t goodhit[20000];
  if(mapping){
    for(Int_t i=0;i < 20000;i++){
      goodhit[i] = 0;
    }
  }
      
  for(Int_t i=0;i < ntofclusters;i++){

    if(cluster[i]->GetNMatchableTracks() || !mapping){
      if(mapping)
	mapping[i] = fESDTOFClusters->GetEntriesFast();
	
      // update TClonesArray
      TClonesArray &ftr = *fESDTOFClusters;
      AliESDTOFCluster *clusterTBW = new(ftr[fESDTOFClusters->GetEntriesFast()])AliESDTOFCluster(*(cluster[i]));

      if(mapping){
	// loop over hit in the cluster
        for(Int_t k=0;k < clusterTBW->GetNTOFhits();k++){
	  Int_t ipos = clusterTBW->GetHitIndex(k);
	  goodhit[ipos] = 1; // hit should be kept
	}
      }
    }
  }

  if(mapping){
    AliInfo(Form("TOF cluster before of matching = %i , after = %i\n",ntofclusters,fESDTOFClusters->GetEntriesFast()));
    Int_t hitnewpos[20000]={0};
    Int_t nhitOriginal = fESDTOFHits->GetEntries();
    for(Int_t i=0;i < fESDTOFHits->GetEntries();i++){
      if(goodhit[i]){
	hitnewpos[i] = i;
      }
      else{ // remove hit and decrease the hit array
	TClonesArray &a=*fESDTOFHits;
	Int_t lastpos = fESDTOFHits->GetEntries()-1;

	if(i == lastpos)
	  delete a.RemoveAt(i);
	else{
	  Int_t nhitBefore = fESDTOFHits->GetEntries();
	  for(Int_t k=nhitBefore-1;k>i;k--){ // find the last good track
	    if(!goodhit[k]){ // remove track
	      delete a.RemoveAt(k);
	      if(k-i==1) delete a.RemoveAt(i);
	    }
	    else{ // replace last one to the "i"
	      AliESDTOFHit *last = (AliESDTOFHit *) fESDTOFHits->At(k);
	      delete a.RemoveAt(i);
	      new (a[i]) AliESDTOFHit(*last);
	      delete a.RemoveAt(k);
	      hitnewpos[k] = i;
	      k = 0;
	    }
	  }
	}
      }
    }

    // remap cluster to hits
    for(Int_t i=0;i < fESDTOFClusters->GetEntries();i++){
      AliESDTOFCluster *cl = (AliESDTOFCluster *) fESDTOFClusters->At(i);
      // loop over hit in the cluster
      for(Int_t k=0;k < cl->GetNTOFhits();k++){
	cl->SetHitIndex(k,hitnewpos[cl->GetHitIndex(k)]);
      }
    }
    AliInfo(Form("TOF hit before of matching = %i , after = %i\n",nhitOriginal,fESDTOFHits->GetEntriesFast()));
  } // end mapping

}

//______________________________________________________________________________
void AliESDEvent::ConnectTracks() {
// Connect tracks to this event
  if (fTracksConnected || !fTracks || !fTracks->GetEntriesFast()) return;
  AliESDtrack *track;
  TIter next(fTracks);
  while ((track=(AliESDtrack*)next())) track->SetESDEvent(this);
  //
  // The same for TOF clusters
  if (fESDTOFClusters) {
    AliESDTOFCluster *clus;
    TIter nextTOF(fESDTOFClusters);
    while ((clus=(AliESDTOFCluster*)nextTOF())) clus->SetEvent((AliVEvent *) this);
  }
  fTracksConnected = kTRUE;
  //
}

//______________________________________________________________________________
AliESDfriend* AliESDEvent::FindFriend() const 
{ 
  return static_cast<AliESDfriend*>(FindListObject("AliESDfriend")); 
}

AliVEvent::EDataLayoutType AliESDEvent::GetDataLayoutType() const {return AliVEvent::kESD;}

//______________________________________________________________________________
Bool_t AliESDEvent::IsIncompleteDAQ() 
{
  // check if DAQ has set the incomplete event attributes
  return (fDAQAttributes&ATTR_2_B(ATTR_INCOMPLETE_EVENT))!=0 
    ||   (fDAQAttributes&ATTR_2_B(ATTR_FLUSHED_EVENT))!=0;
    
}
