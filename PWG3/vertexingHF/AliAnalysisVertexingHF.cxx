/**************************************************************************
 * Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
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

//----------------------------------------------------------------------------
//    Implementation of the heavy-flavour vertexing analysis class
// Candidates are stored in the AOD as objects deriving from AliAODRecoDecay.
// To be used as a task of AliAnalysisManager by means of the interface
// class AliAnalysisTaskSEVertexingHF. 
// An example of usage in the macro AliAnalysisTaskSEVertexingHFTest.C.
//
//  Origin: E.Bruna, G.E.Bruno, A.Dainese, F.Prino, R.Romita
//  Contact: andrea.dainese@lnl.infn.it
//----------------------------------------------------------------------------
#include <TFile.h>
#include <TDatabasePDG.h>
#include <TString.h>
#include "AliLog.h"
#include "AliVEvent.h"
#include "AliVVertex.h"
#include "AliVTrack.h"
#include "AliVertexerTracks.h"
#include "AliKFParticle.h"
#include "AliESDEvent.h"
#include "AliESDVertex.h"
#include "AliESDtrack.h"
#include "AliAODEvent.h"
#include "AliAODRecoDecay.h"
#include "AliAODRecoDecayHF.h"
#include "AliAODRecoDecayHF2Prong.h"
#include "AliAODRecoDecayHF3Prong.h"
#include "AliAODRecoDecayHF4Prong.h"
#include "AliAnalysisVertexingHF.h"

ClassImp(AliAnalysisVertexingHF)

//----------------------------------------------------------------------------
AliAnalysisVertexingHF::AliAnalysisVertexingHF():
fInputAOD(kFALSE),
fAODMap(0),
fBzkG(0.),
fSecVtxWithKF(kFALSE),
fRecoPrimVtxSkippingTrks(kFALSE),
fRmTrksFromPrimVtx(kFALSE),
fV1(0x0),
fD0toKpi(kTRUE),
fJPSItoEle(kTRUE),
f3Prong(kTRUE),
f4Prong(kTRUE),
fITSrefit(kFALSE),
fBothSPD(kTRUE),
fMinITSCls(5),
fMinPtCut(0.),
fMind0rphiCut(0.)
{
  // Default constructor

  SetD0toKpiCuts();
  SetBtoJPSICuts();
  SetDplusCuts();
  SetDsCuts();
  SetLcCuts();
}
//--------------------------------------------------------------------------
AliAnalysisVertexingHF::AliAnalysisVertexingHF(const AliAnalysisVertexingHF &source) : 
TNamed(source),
fInputAOD(source.fInputAOD),
fAODMap(source.fAODMap),
fBzkG(source.fBzkG),
fSecVtxWithKF(source.fSecVtxWithKF),
fRecoPrimVtxSkippingTrks(source.fRecoPrimVtxSkippingTrks),
fRmTrksFromPrimVtx(source.fRmTrksFromPrimVtx),
fV1(source.fV1),
fD0toKpi(source.fD0toKpi),
fJPSItoEle(source.fJPSItoEle),
f3Prong(source.f3Prong),
f4Prong(source.f4Prong),
fITSrefit(source.fITSrefit),
fBothSPD(source.fBothSPD),
fMinITSCls(source.fMinITSCls),
fMinPtCut(source.fMinPtCut),
fMind0rphiCut(source.fMind0rphiCut)
{
  //
  // Copy constructor
  //
  for(Int_t i=0; i<9; i++)  fD0toKpiCuts[i]=source.fD0toKpiCuts[i];
  for(Int_t i=0; i<9; i++)  fBtoJPSICuts[i]=source.fBtoJPSICuts[i];
  for(Int_t i=0; i<12; i++) fDplusCuts[i]=source.fDplusCuts[i];
  for(Int_t i=0; i<13; i++)  fDsCuts[i]=source.fDsCuts[i];
  for(Int_t i=0; i<12; i++)  fLcCuts[i]=source.fLcCuts[i];
}
//--------------------------------------------------------------------------
AliAnalysisVertexingHF &AliAnalysisVertexingHF::operator=(const AliAnalysisVertexingHF &source)
{
  //
  // assignment operator
  //
  if(&source == this) return *this;
  fInputAOD = source.fInputAOD;
  fBzkG = source.fBzkG;
  fSecVtxWithKF = source.fSecVtxWithKF;
  fRecoPrimVtxSkippingTrks = source.fRecoPrimVtxSkippingTrks;
  fRmTrksFromPrimVtx = source.fRmTrksFromPrimVtx;
  fV1 = source.fV1;
  fD0toKpi = source.fD0toKpi;
  fJPSItoEle = source.fJPSItoEle;
  f3Prong = source.f3Prong;
  f4Prong = source.f4Prong;
  fITSrefit = source.fITSrefit;
  fBothSPD = source.fBothSPD;
  fMinITSCls = source.fMinITSCls;
  fMinPtCut = source.fMinPtCut;
  fMind0rphiCut = source.fMind0rphiCut;

  for(Int_t i=0; i<9; i++)  fD0toKpiCuts[i]=source.fD0toKpiCuts[i];
  for(Int_t i=0; i<9; i++)  fBtoJPSICuts[i]=source.fBtoJPSICuts[i];
  for(Int_t i=0; i<12; i++) fDplusCuts[i]=source.fDplusCuts[i];
  for(Int_t i=0; i<13; i++)  fDsCuts[i]=source.fDsCuts[i];
  for(Int_t i=0; i<12; i++)  fLcCuts[i]=source.fLcCuts[i];

  return *this;
}
//----------------------------------------------------------------------------
AliAnalysisVertexingHF::~AliAnalysisVertexingHF() {
  // Destructor
  if(fV1) { delete fV1; fV1=0; }
}
//----------------------------------------------------------------------------
void AliAnalysisVertexingHF::FindCandidates(AliVEvent *event,
					    TClonesArray *aodVerticesHFTClArr,
					    TClonesArray *aodD0toKpiTClArr,
					    TClonesArray *aodJPSItoEleTClArr,
					    TClonesArray *aodCharm3ProngTClArr,
					    TClonesArray *aodCharm4ProngTClArr)
{
  // Find heavy-flavour vertex candidates
  // Input:  ESD or AOD
  // Output: AOD (additional branches added)


  TString evtype = event->IsA()->GetName();
  fInputAOD = ((evtype=="AliAODEvent") ? kTRUE : kFALSE);

  if(!aodVerticesHFTClArr) {
    printf("ERROR: no aodVerticesHFTClArr");
    return;
  }
  if(fD0toKpi && !aodD0toKpiTClArr) {
    printf("ERROR: no aodD0toKpiTClArr");
    return;
  }
  if(fJPSItoEle && !aodJPSItoEleTClArr) {
    printf("ERROR: no aodJPSItoEleTClArr");
    return;
  }
  if(f3Prong && !aodCharm3ProngTClArr) {
    printf("ERROR: no aodCharm3ProngTClArr");
    return;
  }
  if(f4Prong && !aodCharm4ProngTClArr) {
    printf("ERROR: no aodCharm4ProngTClArr");
    return;
  }

  // delete candidates from previous event and create references
  Int_t iVerticesHF=0,iD0toKpi=0,iJPSItoEle=0,i3Prong=0,i4Prong=0;
  aodVerticesHFTClArr->Delete();
  iVerticesHF = aodVerticesHFTClArr->GetEntriesFast();
  TClonesArray &verticesHFRef = *aodVerticesHFTClArr;
  if(fD0toKpi)   {
    aodD0toKpiTClArr->Delete();
    iD0toKpi = aodD0toKpiTClArr->GetEntriesFast();
  }
  if(fJPSItoEle) {
    aodJPSItoEleTClArr->Delete();
    iJPSItoEle = aodJPSItoEleTClArr->GetEntriesFast();
  }
  if(f3Prong) {   
    aodCharm3ProngTClArr->Delete();
    i3Prong = aodCharm3ProngTClArr->GetEntriesFast();
  }
  if(f4Prong) {
    aodCharm4ProngTClArr->Delete();
    i4Prong = aodCharm4ProngTClArr->GetEntriesFast();
  }
  TClonesArray &aodD0toKpiRef     = *aodD0toKpiTClArr;
  TClonesArray &aodJPSItoEleRef   = *aodJPSItoEleTClArr;
  TClonesArray &aodCharm3ProngRef = *aodCharm3ProngTClArr;
  TClonesArray &aodCharm4ProngRef = *aodCharm4ProngTClArr;
  

  AliAODRecoDecayHF2Prong *io2Prong = 0;
  AliAODRecoDecayHF3Prong *io3Prong = 0;
  AliAODRecoDecayHF4Prong *io4Prong = 0;

  Int_t    iTrkP1,iTrkP2,iTrkN1,iTrkN2,trkEntries;
  Int_t    nTrksP=0,nTrksN=0;
  Double_t xdummy,ydummy,dcap1n1,dcap1n2,dcap2n1,dcap1p2,dcan1n2;
  Bool_t   okD0=kFALSE,okJPSI=kFALSE,ok3Prong=kFALSE,ok4Prong=kFALSE;
  AliESDtrack *postrack1 = 0;
  AliESDtrack *postrack2 = 0;
  AliESDtrack *negtrack1 = 0;
  AliESDtrack *negtrack2 = 0;
  Double_t dcaMax = fD0toKpiCuts[1];
  if(dcaMax < fBtoJPSICuts[1]) dcaMax=fBtoJPSICuts[1];
  if(dcaMax < fDplusCuts[11])  dcaMax=fDplusCuts[11];
  AliDebug(2,Form(" dca cut set to %f cm",dcaMax));


  // get Bz
  fBzkG = (Double_t)event->GetMagneticField(); 

  trkEntries = (Int_t)event->GetNumberOfTracks();
  AliDebug(1,Form(" Number of tracks: %d",trkEntries));
    
  if(trkEntries<2 || !event->GetPrimaryVertex()) {
    return;
  }

  // call function that applies sigle-track selection,
  // separates positives and negatives,
  // and retrieves primary vertex
  TObjArray trksP(trkEntries/2);
  TObjArray trksN(trkEntries/2);
  SelectTracksAndCopyVertex(event,trksP,nTrksP,trksN,nTrksN);
    
  AliDebug(1,Form(" Pos. tracks: %d    Neg. tracks: %d",nTrksP,nTrksN));
    
  TObjArray *twoTrackArray1  = new TObjArray(2);
  TObjArray *twoTrackArray2  = new TObjArray(2);
  TObjArray *threeTrackArray = new TObjArray(3);
  TObjArray *fourTrackArray  = new TObjArray(4);
  
  Double_t dispersion;

  // LOOP ON  POSITIVE  TRACKS
  for(iTrkP1=0; iTrkP1<nTrksP; iTrkP1++) {
    if(iTrkP1%1==0) AliDebug(1,Form("  Processing positive track number %d of %d",iTrkP1,nTrksP));  
    // get track from tracks array
    postrack1 = (AliESDtrack*)trksP.UncheckedAt(iTrkP1);
      
    // LOOP ON  NEGATIVE  TRACKS
    for(iTrkN1=0; iTrkN1<nTrksN; iTrkN1++) {
      if(iTrkN1%1==0) AliDebug(1,Form("    Processing negative track number %d of %d",iTrkN1,nTrksN));  
      // get track from tracks array
      negtrack1 = (AliESDtrack*)trksN.UncheckedAt(iTrkN1);
      // back to primary vertex
      postrack1->PropagateToDCA(fV1,fBzkG,kVeryBig);
      negtrack1->PropagateToDCA(fV1,fBzkG,kVeryBig);
      // DCA between the two tracks
      dcap1n1 = postrack1->GetDCA(negtrack1,fBzkG,xdummy,ydummy);
      if(dcap1n1>dcaMax) { negtrack1=0; continue; }
      // Vertexing
      twoTrackArray1->AddAt(postrack1,0);
      twoTrackArray1->AddAt(negtrack1,1);
      AliAODVertex *vertexp1n1 = ReconstructSecondaryVertex(twoTrackArray1,dispersion);
      if(!vertexp1n1) { 
	twoTrackArray1->Clear();
	negtrack1=0; 
	continue; 
      }
      if(fD0toKpi || fJPSItoEle) { 
	io2Prong = Make2Prong(twoTrackArray1,event,vertexp1n1,dcap1n1,okD0,okJPSI);
	if(okD0 || okJPSI) {
	  AliAODVertex *v = new(verticesHFRef[iVerticesHF++])AliAODVertex(*vertexp1n1);
	  if(fInputAOD) AddDaughterRefs(v,event,twoTrackArray1);
	  if(okD0) {  
	    AliAODRecoDecayHF2Prong *rd = new(aodD0toKpiRef[iD0toKpi++])AliAODRecoDecayHF2Prong(*io2Prong);
	    rd->SetSecondaryVtx(v);
	    v->SetParent(rd);
	  }
	  if(okJPSI) {
	    AliAODRecoDecayHF2Prong *rd = new(aodJPSItoEleRef[iJPSItoEle++])AliAODRecoDecayHF2Prong(*io2Prong);
	    rd->SetSecondaryVtx(v);
	    if(!okD0) v->SetParent(rd); // do something better here...
	  }
	}
	io2Prong=NULL;
      }
      
      twoTrackArray1->Clear(); 
      if(!f3Prong && !f4Prong)  { 
	negtrack1=0; 
	delete vertexp1n1; 
	continue; 
      }

	
      // 2nd LOOP  ON  POSITIVE  TRACKS 
      for(iTrkP2=iTrkP1+1; iTrkP2<nTrksP; iTrkP2++) {
	// get track from tracks array
	postrack2 = (AliESDtrack*)trksP.UncheckedAt(iTrkP2);
	// back to primary vertex
	postrack1->PropagateToDCA(fV1,fBzkG,kVeryBig);
	postrack2->PropagateToDCA(fV1,fBzkG,kVeryBig);
	negtrack1->PropagateToDCA(fV1,fBzkG,kVeryBig);
	dcap2n1 = postrack2->GetDCA(negtrack1,fBzkG,xdummy,ydummy);
	if(dcap2n1>dcaMax) { postrack2=0; continue; }
	dcap1p2 = postrack2->GetDCA(postrack1,fBzkG,xdummy,ydummy);
	if(dcap1p2>dcaMax) { postrack2=0; continue; }
	
	// Vertexing
	twoTrackArray2->AddAt(postrack2,0);
	twoTrackArray2->AddAt(negtrack1,1);
	AliAODVertex *vertexp2n1 = ReconstructSecondaryVertex(twoTrackArray2,dispersion);
	if(!vertexp2n1) { 
	  twoTrackArray2->Clear();
	  postrack2=0; 
	  continue; 
	}
	if(f3Prong) { 
	  threeTrackArray->AddAt(postrack1,0);
	  threeTrackArray->AddAt(negtrack1,1);
	  threeTrackArray->AddAt(postrack2,2);
	  AliAODVertex* secVert3PrAOD = ReconstructSecondaryVertex(threeTrackArray,dispersion);
	  io3Prong = Make3Prong(threeTrackArray,event,secVert3PrAOD,dispersion,vertexp1n1,vertexp2n1,dcap1n1,dcap2n1,dcap1p2,ok3Prong);
	  if(ok3Prong) {
	    AliAODVertex *v = new(verticesHFRef[iVerticesHF++])AliAODVertex(*secVert3PrAOD);
	    if(fInputAOD) AddDaughterRefs(v,event,threeTrackArray);
	    AliAODRecoDecayHF3Prong *rd = new(aodCharm3ProngRef[i3Prong++])AliAODRecoDecayHF3Prong(*io3Prong);
	    rd->SetSecondaryVtx(v);
	    v->SetParent(rd);
	  }
	  if(io3Prong) io3Prong=NULL; 
	}
	if(f4Prong) {
	  // 3rd LOOP  ON  NEGATIVE  TRACKS (for 4 prong) 
	  for(iTrkN2=iTrkN1+1; iTrkN2<nTrksN; iTrkN2++) {
	    // get track from tracks array
	    negtrack2 = (AliESDtrack*)trksN.UncheckedAt(iTrkN2);
	    // back to primary vertex
	    postrack1->PropagateToDCA(fV1,fBzkG,kVeryBig);
	    postrack2->PropagateToDCA(fV1,fBzkG,kVeryBig);
	    negtrack1->PropagateToDCA(fV1,fBzkG,kVeryBig);
	    negtrack2->PropagateToDCA(fV1,fBzkG,kVeryBig);
	    dcap1n2 = postrack1->GetDCA(negtrack2,fBzkG,xdummy,ydummy);
	    if(dcap1n2>dcaMax) { negtrack2=0; continue; }
	    // Vertexing
	    fourTrackArray->AddAt(postrack1,0);
	    fourTrackArray->AddAt(negtrack1,1);
	    fourTrackArray->AddAt(postrack2,2);
	    fourTrackArray->AddAt(negtrack2,3);
	    AliAODVertex* secVert4PrAOD = ReconstructSecondaryVertex(fourTrackArray,dispersion);
	    io4Prong = Make4Prong(fourTrackArray,event,secVert4PrAOD,vertexp1n1,vertexp2n1,dcap1n1,dcap1n2,dcap2n1,ok4Prong);
	    if(ok4Prong) {
	      AliAODVertex *v = new(verticesHFRef[iVerticesHF++])AliAODVertex(*secVert4PrAOD);
	      if(fInputAOD) AddDaughterRefs(v,event,fourTrackArray);
	      AliAODRecoDecayHF4Prong *rd = new(aodCharm4ProngRef[i4Prong++])AliAODRecoDecayHF4Prong(*io4Prong);
	      rd->SetSecondaryVtx(v);
	      v->SetParent(rd);
	    }
	    if(io4Prong) io4Prong=NULL; 
	    fourTrackArray->Clear();
	    negtrack2 = 0;
	  } // end loop on negative tracks
	}
	postrack2 = 0;
	delete vertexp2n1;
      } // end 2nd loop on positive tracks
      twoTrackArray2->Clear();
      
      // 2nd LOOP  ON  NEGATIVE  TRACKS 
      for(iTrkN2=iTrkN1+1; iTrkN2<nTrksN; iTrkN2++) {
	// get track from tracks array
	negtrack2 = (AliESDtrack*)trksN.UncheckedAt(iTrkN2);
	// back to primary vertex
	postrack1->PropagateToDCA(fV1,fBzkG,kVeryBig);
	negtrack1->PropagateToDCA(fV1,fBzkG,kVeryBig);
	negtrack2->PropagateToDCA(fV1,fBzkG,kVeryBig);
	dcap1n2 = postrack1->GetDCA(negtrack2,fBzkG,xdummy,ydummy);
	if(dcap1n2>dcaMax) { negtrack2=0; continue; }
	dcan1n2 = negtrack1->GetDCA(negtrack2,fBzkG,xdummy,ydummy);
	if(dcan1n2>dcaMax) { negtrack2=0; continue; }
	
	// Vertexing
	twoTrackArray2->AddAt(postrack1,0);
	twoTrackArray2->AddAt(negtrack2,1);

	AliAODVertex *vertexp1n2 = ReconstructSecondaryVertex(twoTrackArray2,dispersion);
	if(!vertexp1n2) { 
	  twoTrackArray2->Clear();
	  negtrack2=0; 
	  continue; 
	}
	if(f3Prong) { 
	  threeTrackArray->AddAt(negtrack1,0);
	  threeTrackArray->AddAt(postrack1,1);
	  threeTrackArray->AddAt(negtrack2,2);
	  AliAODVertex* secVert3PrAOD = ReconstructSecondaryVertex(threeTrackArray,dispersion);
	  io3Prong = Make3Prong(threeTrackArray,event,secVert3PrAOD,dispersion,vertexp1n1,vertexp1n2,dcap1n1,dcap1n2,dcan1n2,ok3Prong);
	  if(ok3Prong) {
	    AliAODVertex *v = new(verticesHFRef[iVerticesHF++])AliAODVertex(*secVert3PrAOD);
	    if(fInputAOD) AddDaughterRefs(v,event,threeTrackArray);
	    AliAODRecoDecayHF3Prong *rd = new(aodCharm3ProngRef[i3Prong++])AliAODRecoDecayHF3Prong(*io3Prong);
	    rd->SetSecondaryVtx(v);
	    v->SetParent(rd);
	  }
	  if(io3Prong) io3Prong=NULL; 
	}
	negtrack2 = 0;
	delete vertexp1n2;
      } // end 2nd loop on negative tracks
      twoTrackArray2->Clear();
      
      negtrack1 = 0;
      delete vertexp1n1; 
    } // end 1st loop on negative tracks
    
    postrack1 = 0;
  }  // end 1st loop on positive tracks


  if(fD0toKpi) {
    AliDebug(1,Form(" D0->Kpi in event = %d;",
		    (Int_t)aodD0toKpiTClArr->GetEntriesFast()));
  }
  if(fJPSItoEle) {
    AliDebug(1,Form(" JPSI->ee in event = %d;",
		    (Int_t)aodJPSItoEleTClArr->GetEntriesFast()));
  }
  if(f3Prong) {
    AliDebug(1,Form(" Charm->3Prong in event = %d;",
		    (Int_t)aodCharm3ProngTClArr->GetEntriesFast()));
  }
  if(f4Prong) {
    AliDebug(1,Form(" Charm->4Prong in event = %d;\n",
		    (Int_t)aodCharm4ProngTClArr->GetEntriesFast()));
  }
    

  twoTrackArray1->Delete();  delete twoTrackArray1;
  twoTrackArray2->Delete();  delete twoTrackArray2;
  threeTrackArray->Clear(); 
  threeTrackArray->Delete(); delete threeTrackArray;
  fourTrackArray->Delete();  delete fourTrackArray;

  if(fInputAOD) {
    trksP.Delete();
    trksN.Delete();
  }

  return;
}
//----------------------------------------------------------------------------
void AliAnalysisVertexingHF::AddDaughterRefs(AliAODVertex *v,AliVEvent *event,
					     TObjArray *trkArray) const
{
  // Add the AOD tracks as daughters of the vertex (TRef)

  Int_t nTrks = trkArray->GetEntriesFast();

  AliESDtrack *esdTrack = 0;
  AliAODTrack *aodTrack = 0;
  Int_t id;

  for(Int_t i=0; i<nTrks; i++) {
    esdTrack = (AliESDtrack*)trkArray->UncheckedAt(i);
    id = (Int_t)esdTrack->GetID();
    aodTrack = (AliAODTrack*)event->GetTrack(fAODMap[id]);
    v->AddDaughter(aodTrack);
  }

  return;
}	
//----------------------------------------------------------------------------
AliAODRecoDecayHF2Prong *AliAnalysisVertexingHF::Make2Prong(
				   TObjArray *twoTrackArray1,AliVEvent *event,
				   AliAODVertex *secVert,Double_t dca,
				   Bool_t &okD0,Bool_t &okJPSI) const
{
  // Make 2Prong candidates and check if they pass D0toKpi or BtoJPSI
  // reconstruction cuts
  // G.E.Bruno (J/psi), A.Dainese (D0->Kpi)

  okD0=kFALSE; okJPSI=kFALSE;

  Double_t px[2],py[2],pz[2],d0[2],d0err[2];

  AliESDtrack *postrack = (AliESDtrack*)twoTrackArray1->UncheckedAt(0);
  AliESDtrack *negtrack = (AliESDtrack*)twoTrackArray1->UncheckedAt(1);

  // propagate tracks to secondary vertex, to compute inv. mass
  postrack->PropagateToDCA(secVert,fBzkG,kVeryBig);
  negtrack->PropagateToDCA(secVert,fBzkG,kVeryBig);

  Double_t momentum[3];
  postrack->GetPxPyPz(momentum);
  px[0] = momentum[0]; py[0] = momentum[1]; pz[0] = momentum[2]; 
  negtrack->GetPxPyPz(momentum);
  px[1] = momentum[0]; py[1] = momentum[1]; pz[1] = momentum[2]; 


  // invariant mass cut (try to improve coding here..)
  Bool_t okMassCut=kFALSE;
  if(!okMassCut && fD0toKpi)   if(SelectInvMass(0,2,px,py,pz)) okMassCut=kTRUE;
  if(!okMassCut && fJPSItoEle) if(SelectInvMass(1,2,px,py,pz)) okMassCut=kTRUE;
  if(!okMassCut) {
    AliDebug(2," candidate didn't pass mass cut");
    return 0x0;    
  }

  // primary vertex to be used by this candidate
  AliAODVertex *primVertexAOD  = OwnPrimaryVertex(twoTrackArray1,event);
  if(!primVertexAOD) return 0x0;

  Double_t d0z0[2],covd0z0[3];
  postrack->PropagateToDCA(primVertexAOD,fBzkG,kVeryBig,d0z0,covd0z0);
  d0[0] = d0z0[0];
  d0err[0] = TMath::Sqrt(covd0z0[0]);
  negtrack->PropagateToDCA(primVertexAOD,fBzkG,kVeryBig,d0z0,covd0z0);
  d0[1] = d0z0[0];
  d0err[1] = TMath::Sqrt(covd0z0[0]);

  // create the object AliAODRecoDecayHF2Prong
  AliAODRecoDecayHF2Prong *the2Prong = new AliAODRecoDecayHF2Prong(secVert,px,py,pz,d0,d0err,dca);
  the2Prong->SetOwnPrimaryVtx(primVertexAOD);
  UShort_t id[2]={(UShort_t)postrack->GetID(),(UShort_t)negtrack->GetID()};
  the2Prong->SetProngIDs(2,id);


  // select D0->Kpi
  Int_t checkD0,checkD0bar;
  if(fD0toKpi) okD0 = the2Prong->SelectD0(fD0toKpiCuts,checkD0,checkD0bar);
  //if(fDebug && fD0toKpi) printf("   %d\n",(Int_t)okD0);
  // select J/psi from B
  Int_t checkJPSI;
  if(fJPSItoEle) okJPSI = the2Prong->SelectBtoJPSI(fBtoJPSICuts,checkJPSI);
  //if(fDebug && fJPSItoEle) printf("   %d\n",(Int_t)okJPSI);

  if(!fRecoPrimVtxSkippingTrks && !fRmTrksFromPrimVtx) the2Prong->UnsetOwnPrimaryVtx();

  if(okD0 || okJPSI) {
    // get PID info from ESD
    Double_t esdpid0[5];
    postrack->GetESDpid(esdpid0);
    Double_t esdpid1[5];
    negtrack->GetESDpid(esdpid1);
    Double_t esdpid[10];
    for(Int_t i=0;i<5;i++) {
      esdpid[i]   = esdpid0[i];
      esdpid[5+i] = esdpid1[i];
    }
    the2Prong->SetPID(2,esdpid);
  }

  return the2Prong;  
}
//----------------------------------------------------------------------------
AliAODRecoDecayHF3Prong* AliAnalysisVertexingHF::Make3Prong(
                             TObjArray *threeTrackArray,AliVEvent *event,
			     AliAODVertex *secVert,Double_t dispersion,
			     AliAODVertex *vertexp1n1,AliAODVertex *vertexp2n1,
			     Double_t dcap1n1,Double_t dcap2n1,Double_t dcap1p2,
			     Bool_t &ok3Prong) const
{
  // Make 3Prong candidates and check if they pass Dplus or Ds or Lambdac
  // reconstruction cuts 
  // E.Bruna, F.Prino


  ok3Prong=kFALSE;
  if(!secVert) return 0x0; 

  Double_t px[3],py[3],pz[3],d0[3],d0err[3];
  Double_t momentum[3];


  AliESDtrack *postrack1 = (AliESDtrack*)threeTrackArray->UncheckedAt(0);
  AliESDtrack *negtrack  = (AliESDtrack*)threeTrackArray->UncheckedAt(1);
  AliESDtrack *postrack2 = (AliESDtrack*)threeTrackArray->UncheckedAt(2);

  postrack1->PropagateToDCA(secVert,fBzkG,kVeryBig);
  negtrack->PropagateToDCA(secVert,fBzkG,kVeryBig);
  postrack2->PropagateToDCA(secVert,fBzkG,kVeryBig);
  postrack1->GetPxPyPz(momentum);
  px[0] = momentum[0]; py[0] = momentum[1]; pz[0] = momentum[2]; 
  negtrack->GetPxPyPz(momentum);
  px[1] = momentum[0]; py[1] = momentum[1]; pz[1] = momentum[2]; 
  postrack2->GetPxPyPz(momentum);
  px[2] = momentum[0]; py[2] = momentum[1]; pz[2] = momentum[2]; 

  // invariant mass cut for D+, Ds, Lc
  Bool_t okMassCut=kFALSE;
  if(!okMassCut && f3Prong) if(SelectInvMass(2,3,px,py,pz)) okMassCut=kTRUE;
  if(!okMassCut) {
    AliDebug(2," candidate didn't pass mass cut");
    return 0x0;    
  }

  // primary vertex to be used by this candidate
  AliAODVertex *primVertexAOD  = OwnPrimaryVertex(threeTrackArray,event);
  if(!primVertexAOD) return 0x0;

  Double_t d0z0[2],covd0z0[3];
  postrack1->PropagateToDCA(primVertexAOD,fBzkG,kVeryBig,d0z0,covd0z0);
  d0[0]=d0z0[0];
  d0err[0] = TMath::Sqrt(covd0z0[0]);
  negtrack->PropagateToDCA(primVertexAOD,fBzkG,kVeryBig,d0z0,covd0z0);
  d0[1]=d0z0[0];
  d0err[1] = TMath::Sqrt(covd0z0[0]);
  postrack2->PropagateToDCA(primVertexAOD,fBzkG,kVeryBig,d0z0,covd0z0);
  d0[2]=d0z0[0];
  d0err[2] = TMath::Sqrt(covd0z0[0]);


  // create the object AliAODRecoDecayHF3Prong
  Double_t pos[3]; primVertexAOD->GetXYZ(pos);
  Double_t dca[3]={dcap1n1,dcap2n1,dcap1p2};
  Double_t dist12=TMath::Sqrt((vertexp1n1->GetX()-pos[0])*(vertexp1n1->GetX()-pos[0])+(vertexp1n1->GetY()-pos[1])*(vertexp1n1->GetY()-pos[1])+(vertexp1n1->GetZ()-pos[2])*(vertexp1n1->GetZ()-pos[2]));
  Double_t dist23=TMath::Sqrt((vertexp2n1->GetX()-pos[0])*(vertexp2n1->GetX()-pos[0])+(vertexp2n1->GetY()-pos[1])*(vertexp2n1->GetY()-pos[1])+(vertexp2n1->GetZ()-pos[2])*(vertexp2n1->GetZ()-pos[2]));
  Short_t charge=(Short_t)(postrack1->Charge()*postrack2->Charge()*negtrack->Charge());
  AliAODRecoDecayHF3Prong *the3Prong = new AliAODRecoDecayHF3Prong(secVert,px,py,pz,d0,d0err,dca,dispersion,dist12,dist23,charge);
  the3Prong->SetOwnPrimaryVtx(primVertexAOD);
  UShort_t id[3]={(UShort_t)postrack1->GetID(),(UShort_t)negtrack->GetID(),(UShort_t)postrack2->GetID()};
  the3Prong->SetProngIDs(3,id);


  // select D+->Kpipi, Ds->KKpi, Lc->pKpi
  if(f3Prong) {
    ok3Prong = kFALSE;
    Int_t ok1,ok2;
    if(the3Prong->SelectDplus(fDplusCuts))   ok3Prong = kTRUE;
    if(the3Prong->SelectDs(fDsCuts,ok1,ok2)) ok3Prong = kTRUE;
    if(the3Prong->SelectLc(fLcCuts,ok1,ok2)) ok3Prong = kTRUE;
  }
  //if(fDebug) printf("ok3Prong: %d\n",(Int_t)ok3Prong);

  if(!fRecoPrimVtxSkippingTrks && !fRmTrksFromPrimVtx) the3Prong->UnsetOwnPrimaryVtx();

  if(ok3Prong) {
    // get PID info from ESD
    Double_t esdpid0[5];
    postrack1->GetESDpid(esdpid0);
    Double_t esdpid1[5];
    negtrack->GetESDpid(esdpid1);
    Double_t esdpid2[5];
    postrack2->GetESDpid(esdpid2);

    Double_t esdpid[15];
    for(Int_t i=0;i<5;i++) {
      esdpid[i]   = esdpid0[i];
      esdpid[5+i] = esdpid1[i];
      esdpid[10+i] = esdpid2[i];
    }
    the3Prong->SetPID(3,esdpid);
  }

  return the3Prong;
}
//----------------------------------------------------------------------------
AliAODRecoDecayHF4Prong* AliAnalysisVertexingHF::Make4Prong(
                             TObjArray *fourTrackArray,AliVEvent *event,
			     AliAODVertex *secVert,
			     AliAODVertex *vertexp1n1,AliAODVertex *vertexp2n1,
			     Double_t dcap1n1,Double_t dcap1n2,Double_t dcap2n1,
			     Bool_t &ok4Prong) const
{
  // Make 4Prong candidates and check if they pass D0toKpipipi
  // reconstruction cuts
  // G.E.Bruno, R.Romita

  ok4Prong=kFALSE;
  if(!secVert) return 0x0; 

  Double_t px[4],py[4],pz[4],d0[4],d0err[4];//d0z[3];

  px[0]=dcap1n1*dcap1n2*dcap2n1; // TO BE CHANGED (done just to removed compilation warning about dca... not used)

  AliESDtrack *postrack1 = (AliESDtrack*)fourTrackArray->UncheckedAt(0);
  AliESDtrack *negtrack1 = (AliESDtrack*)fourTrackArray->UncheckedAt(1);
  AliESDtrack *postrack2 = (AliESDtrack*)fourTrackArray->UncheckedAt(2);
  AliESDtrack *negtrack2 = (AliESDtrack*)fourTrackArray->UncheckedAt(3);

  postrack1->PropagateToDCA(secVert,fBzkG,kVeryBig);
  negtrack1->PropagateToDCA(secVert,fBzkG,kVeryBig);
  postrack2->PropagateToDCA(secVert,fBzkG,kVeryBig);
  negtrack2->PropagateToDCA(secVert,fBzkG,kVeryBig);

  Double_t momentum[3];
  postrack1->GetPxPyPz(momentum);
  px[0] = momentum[0]; py[0] = momentum[1]; pz[0] = momentum[2];
  negtrack1->GetPxPyPz(momentum);
  px[1] = momentum[0]; py[1] = momentum[1]; pz[1] = momentum[2];
  postrack2->GetPxPyPz(momentum);
  px[2] = momentum[0]; py[2] = momentum[1]; pz[2] = momentum[2];
  negtrack2->GetPxPyPz(momentum);
  px[3] = momentum[0]; py[3] = momentum[1]; pz[3] = momentum[2];

  // invariant mass cut for rho or D0 (try to improve coding here..)
  //Bool_t okMassCut=kFALSE;
  //if(!okMassCut) if(SelectInvMass(2,3,px,py,pz)) okMassCut=kTRUE;
  //if(!okMassCut) {
  //  if(fDebug) printf(" candidate didn't pass mass cut\n");
  //  return 0x0;
  //}

  // primary vertex to be used by this candidate
  AliAODVertex *primVertexAOD  = OwnPrimaryVertex(fourTrackArray,event);
  if(!primVertexAOD) return 0x0;

  /*
    Double_t d0z0[2],covd0z0[3];
    postrack1->PropagateToDCA(primVertexAOD,fBzkG,kVeryBig,d0z0,covd0z0);
    negtrack1->PropagateToDCA(primVertexAOD,fBzkG,kVeryBig,d0z0,covd0z0);
    postrack2->PropagateToDCA(primVertexAOD,fBzkG,kVeryBig,d0z0,covd0z0);
    negtrack2->PropagateToDCA(primVertexAOD,fBzkG,kVeryBig,d0z0,covd0z0);
  */

  // create the object AliAODRecoDecayHF4Prong
  Double_t pos[3]; primVertexAOD->GetXYZ(pos);
  Double_t dca[6]={0.,0.,0.,0.,0.,0.}; //  modify it
  Double_t dist12=TMath::Sqrt((vertexp1n1->GetX()-pos[0])*(vertexp1n1->GetX()-pos[0])+(vertexp1n1->GetY()-pos[1])*(vertexp1n1->GetY()-pos[1])+(vertexp1n1->GetZ()-pos[2])*(vertexp1n1->GetZ()-pos[2]));
  Double_t dist23=TMath::Sqrt((vertexp2n1->GetX()-pos[0])*(vertexp2n1->GetX()-pos[0])+(vertexp2n1->GetY()-pos[1])*(vertexp2n1->GetY()-pos[1])+(vertexp2n1->GetZ()-pos[2])*(vertexp2n1->GetZ()-pos[2]));
  Double_t dist14=0.; // to be implemented
  Double_t dist34=0.; // to be implemented
  Short_t charge=0;
  AliAODRecoDecayHF4Prong *the4Prong = new AliAODRecoDecayHF4Prong(secVert,px,py,pz,d0,d0err,dca,dist12,dist23,dist14,dist34,charge);
  the4Prong->SetOwnPrimaryVtx(primVertexAOD);
  UShort_t id[4]={(UShort_t)postrack1->GetID(),(UShort_t)negtrack1->GetID(),(UShort_t)postrack2->GetID(),(UShort_t)negtrack2->GetID()};
  the4Prong->SetProngIDs(4,id);


  // use the following two lines once AliAODRecoDecayHF4Prong::SelectD0 is available
  // select D0->Kpipipi
  //Int_t checkD0,checkD0bar;   
  // ok4Prong=the4Prong->SelectD0(fD04pCuts,checkD0,checkD0bar); 
  ok4Prong=kFALSE;  //for the time being ...

  if(!fRecoPrimVtxSkippingTrks && !fRmTrksFromPrimVtx) the4Prong->UnsetOwnPrimaryVtx();

  // get PID info from ESD
  Double_t esdpid0[5];
  postrack1->GetESDpid(esdpid0);
  Double_t esdpid1[5];
  negtrack1->GetESDpid(esdpid1);
  Double_t esdpid2[5];
  postrack2->GetESDpid(esdpid2);
  Double_t esdpid3[5];
  negtrack2->GetESDpid(esdpid3);

  Double_t esdpid[20];
  for(Int_t i=0;i<5;i++) {
    esdpid[i]   = esdpid0[i];
    esdpid[5+i] = esdpid1[i];
    esdpid[10+i] = esdpid2[i];
    esdpid[15+i] = esdpid3[i];
  }
  the4Prong->SetPID(4,esdpid);

  return the4Prong;
}
//-----------------------------------------------------------------------------
AliAODVertex* AliAnalysisVertexingHF::OwnPrimaryVertex(TObjArray *trkArray,
						       AliVEvent *event) const
{
  // Returns primary vertex specific to this candidate
 
  AliESDVertex *vertexESD = 0;
  AliAODVertex *vertexAOD = 0;

  Int_t nTrks = trkArray->GetEntriesFast();

  if(!fRecoPrimVtxSkippingTrks && !fRmTrksFromPrimVtx) { 
    // primary vertex from the input event
    
    vertexESD = new AliESDVertex(*fV1);

  } else {
    // primary vertex specific to this candidate

    AliVertexerTracks *vertexer = new AliVertexerTracks(event->GetMagneticField());

    if(fRecoPrimVtxSkippingTrks) { 
      // recalculating the vertex
      
      if(strstr(fV1->GetTitle(),"VertexerTracksWithConstraint")) {
	Float_t diamondcovxy[3];
	event->GetDiamondCovXY(diamondcovxy);
	Double_t pos[3]={event->GetDiamondX(),event->GetDiamondY(),0.};
	Double_t cov[6]={diamondcovxy[0],diamondcovxy[1],diamondcovxy[2],0.,0.,10.};
	AliESDVertex *diamond = new AliESDVertex(pos,cov,1.,1);
	vertexer->SetVtxStart(diamond);
	delete diamond; diamond=NULL;
	if(strstr(fV1->GetTitle(),"VertexerTracksWithConstraintOnlyFitter")) 
	  vertexer->SetOnlyFitter();
      }
      Int_t skipped[10];
      AliESDtrack *t = 0;
      for(Int_t i=0; i<nTrks; i++) {
	t = (AliESDtrack*)trkArray->UncheckedAt(i);
	skipped[i] = (Int_t)t->GetID();
      }
      vertexer->SetSkipTracks(nTrks,skipped);
      vertexESD = (AliESDVertex*)vertexer->FindPrimaryVertex(event); 
      
    } else if(fRmTrksFromPrimVtx) { 
      // removing the prongs tracks
      
      TObjArray rmArray(nTrks);
      UShort_t *rmId = new UShort_t[nTrks];
      AliESDtrack *esdTrack = 0;
      AliESDtrack *t = 0;
      for(Int_t i=0; i<nTrks; i++) {
	t = (AliESDtrack*)trkArray->UncheckedAt(i);
	esdTrack = new AliESDtrack(*t);
	rmArray.AddLast(esdTrack);
	rmId[i]=(UShort_t)esdTrack->GetID();
      }
      Float_t diamondxy[2]={event->GetDiamondX(),event->GetDiamondY()};
      vertexESD = vertexer->RemoveTracksFromVertex(fV1,&rmArray,rmId,diamondxy);
      delete [] rmId; rmId=NULL;
      rmArray.Delete();
      
    }

    if(!vertexESD) return vertexAOD;
    if(vertexESD->GetNContributors()<=0) { 
      AliDebug(2,"vertexing failed"); 
      delete vertexESD; vertexESD=NULL;
      return vertexAOD;
    }

    delete vertexer; vertexer=NULL;

  }

  // convert to AliAODVertex
  Double_t pos[3],cov[6],chi2perNDF;
  vertexESD->GetXYZ(pos); // position
  vertexESD->GetCovMatrix(cov); //covariance matrix
  chi2perNDF = vertexESD->GetChi2toNDF();
  delete vertexESD; vertexESD=NULL;

  vertexAOD = new AliAODVertex(pos,cov,chi2perNDF);

  return vertexAOD;
}
//-----------------------------------------------------------------------------
void AliAnalysisVertexingHF::PrintStatus() const {
  // Print parameters being used

  printf("Preselections:\n");
  printf("    fITSrefit   = %d\n",(Int_t)fITSrefit);
  printf("    fBothSPD   = %d\n",(Int_t)fBothSPD);
  printf("    fMinITSCls   = %d\n",fMinITSCls);
  printf("    fMinPtCut   = %f GeV/c\n",fMinPtCut);
  printf("    fMind0rphiCut   = %f cm\n",fMind0rphiCut);
  if(fSecVtxWithKF) {
    printf("Secondary vertex with Kalman filter package (AliKFParticle)\n");
  } else {
    printf("Secondary vertex with AliVertexerTracks\n");
  }
  if(fRecoPrimVtxSkippingTrks) printf("RecoPrimVtxSkippingTrks\n");
  if(fRmTrksFromPrimVtx) printf("RmTrksFromPrimVtx\n");
  if(fD0toKpi) {
    printf("Reconstruct D0->Kpi candidates with cuts:\n");
    printf("    |M-MD0| [GeV]    < %f\n",fD0toKpiCuts[0]);
    printf("    dca    [cm]  < %f\n",fD0toKpiCuts[1]);
    printf("    cosThetaStar     < %f\n",fD0toKpiCuts[2]);
    printf("    pTK     [GeV/c]    > %f\n",fD0toKpiCuts[3]);
    printf("    pTpi    [GeV/c]    > %f\n",fD0toKpiCuts[4]);
    printf("    |d0K|  [cm]  < %f\n",fD0toKpiCuts[5]);
    printf("    |d0pi| [cm]  < %f\n",fD0toKpiCuts[6]);
    printf("    d0d0  [cm^2] < %f\n",fD0toKpiCuts[7]);
    printf("    cosThetaPoint    > %f\n",fD0toKpiCuts[8]);
  }
  if(fJPSItoEle) {
    printf("Reconstruct J/psi from B candidates with cuts:\n");
    printf("    |M-MJPSI| [GeV]    < %f\n",fBtoJPSICuts[0]);
    printf("    dca    [cm]  < %f\n",fBtoJPSICuts[1]);
    printf("    cosThetaStar     < %f\n",fBtoJPSICuts[2]);
    printf("    pTP     [GeV/c]    > %f\n",fBtoJPSICuts[3]);
    printf("    pTN    [GeV/c]    > %f\n",fBtoJPSICuts[4]);
    printf("    |d0P|  [cm]  < %f\n",fBtoJPSICuts[5]);
    printf("    |d0N| [cm]  < %f\n",fBtoJPSICuts[6]);
    printf("    d0d0  [cm^2] < %f\n",fBtoJPSICuts[7]);
    printf("    cosThetaPoint    > %f\n",fBtoJPSICuts[8]);
  }
  if(f3Prong) {
    printf("Reconstruct 3 prong candidates.\n");
    printf("  D+->Kpipi cuts:\n");
    printf("    |M-MD+| [GeV]    < %f\n",fDplusCuts[0]);
    printf("    pTK     [GeV/c]    > %f\n",fDplusCuts[1]);
    printf("    pTPi    [GeV/c]    > %f\n",fDplusCuts[2]);
    printf("    |d0K|  [cm]  > %f\n",fDplusCuts[3]);
    printf("    |d0Pi| [cm]  > %f\n",fDplusCuts[4]);
    printf("    dist12    [cm]  < %f\n",fDplusCuts[5]);
    printf("    sigmavert [cm]   < %f\n",fDplusCuts[6]);
    printf("    dist prim-sec [cm] > %f\n",fDplusCuts[7]);
    printf("    pM=Max{pT1,pT2,pT3} [GeV/c] > %f\n",fDplusCuts[8]);
    printf("    cosThetaPoint    > %f\n",fDplusCuts[9]);
    printf("    Sum d0^2 [cm^2]  > %f\n",fDplusCuts[10]);
    printf("    dca cut [cm]  < %f\n",fDplusCuts[11]);
    printf("  Ds->KKpi cuts:\n");
    printf("    |M-MDs| [GeV]    < %f\n",fDsCuts[0]);
    printf("    pTK     [GeV/c]    > %f\n",fDsCuts[1]);
    printf("    pTPi    [GeV/c]    > %f\n",fDsCuts[2]);
    printf("    |d0K|  [cm]  > %f\n",fDsCuts[3]);
    printf("    |d0Pi| [cm]  > %f\n",fDsCuts[4]);
    printf("    dist12    [cm]  < %f\n",fDsCuts[5]);
    printf("    sigmavert [cm]   < %f\n",fDsCuts[6]);
    printf("    dist prim-sec [cm] > %f\n",fDsCuts[7]);
    printf("    pM=Max{pT1,pT2,pT3} [GeV/c] > %f\n",fDsCuts[8]);
    printf("    cosThetaPoint    > %f\n",fDsCuts[9]);
    printf("    Sum d0^2 [cm^2]  > %f\n",fDsCuts[10]);
    printf("    dca cut [cm]  < %f\n",fDsCuts[11]);
    printf("    Inv. Mass  phi/K0* [GeV]  < %f\n",fDsCuts[12]);
    printf("  Lc->pKpi cuts:\n");
    printf("    |M-MLc| [GeV]    < %f\n",fLcCuts[0]);
    printf("    pTP     [GeV/c]    > %f\n",fLcCuts[1]);
    printf("    pTPi and pTK [GeV/c]    > %f\n",fLcCuts[2]);
    printf("    |d0P|  [cm]  > %f\n",fLcCuts[3]);
    printf("    |d0Pi| and |d0K| [cm]  > %f\n",fLcCuts[4]);
    printf("    dist12    [cm]  < %f\n",fLcCuts[5]);
    printf("    sigmavert [cm]   < %f\n",fLcCuts[6]);
    printf("    dist prim-sec [cm] > %f\n",fLcCuts[7]);
    printf("    pM=Max{pT1,pT2,pT3} [GeV/c] > %f\n",fLcCuts[8]);
    printf("    cosThetaPoint    > %f\n",fLcCuts[9]);
    printf("    Sum d0^2 [cm^2]  > %f\n",fLcCuts[10]);
    printf("    dca cut [cm]  < %f\n",fLcCuts[11]);
    printf("  Ds->KKpi cuts:\n");
  }

  return;
}
//-----------------------------------------------------------------------------
AliAODVertex* AliAnalysisVertexingHF::ReconstructSecondaryVertex(TObjArray *trkArray,
								 Double_t &dispersion) const
{
  // Secondary vertex reconstruction with AliVertexerTracks or AliKFParticle

  AliESDVertex *vertexESD = 0;
  AliAODVertex *vertexAOD = 0;

  if(!fSecVtxWithKF) { // AliVertexerTracks

    AliVertexerTracks *vertexer = new AliVertexerTracks(fBzkG);
    vertexer->SetVtxStart(fV1);
    vertexESD = (AliESDVertex*)vertexer->VertexForSelectedESDTracks(trkArray);
    delete vertexer; vertexer=NULL;

    if(!vertexESD) return vertexAOD;

    if(vertexESD->GetNContributors()!=trkArray->GetEntriesFast()) { 
      AliDebug(2,"vertexing failed"); 
      delete vertexESD; vertexESD=NULL;
      return vertexAOD;
    }

  } else { // Kalman Filter vertexer (AliKFParticle)

    AliKFParticle::SetField(fBzkG);

    AliKFParticle vertexKF;

    Int_t nTrks = trkArray->GetEntriesFast();
    for(Int_t i=0; i<nTrks; i++) {
      AliESDtrack *esdTrack = (AliESDtrack*)trkArray->At(i);
      AliKFParticle daughterKF(*esdTrack,211);
      vertexKF.AddDaughter(daughterKF);
    }
    vertexESD = new AliESDVertex();
    vertexKF.CopyToESDVertex(*vertexESD);

  }

  // convert to AliAODVertex
  Double_t pos[3],cov[6],chi2perNDF;
  vertexESD->GetXYZ(pos); // position
  vertexESD->GetCovMatrix(cov); //covariance matrix
  chi2perNDF = vertexESD->GetChi2toNDF();
  dispersion = vertexESD->GetDispersion();
  delete vertexESD; vertexESD=NULL;

  vertexAOD = new AliAODVertex(pos,cov,chi2perNDF);
  vertexAOD->SetType(AliAODVertex::kUndef); // to be changed

  return vertexAOD;
}
//-----------------------------------------------------------------------------
Bool_t AliAnalysisVertexingHF::SelectInvMass(Int_t decay,
					     Int_t nprongs,
					     Double_t *px,
					     Double_t *py,
					     Double_t *pz) const {
  // Check invariant mass cut

  Short_t dummycharge=0;
  Double_t *dummyd0 = new Double_t[nprongs];
  for(Int_t ip=0;ip<nprongs;ip++) dummyd0[ip]=0.;
  AliAODRecoDecay *rd = new AliAODRecoDecay(0x0,nprongs,dummycharge,px,py,pz,dummyd0);
  delete [] dummyd0;

  UInt_t pdg2[2],pdg3[3];
  Double_t mPDG,minv;

  Bool_t retval=kFALSE;
  switch (decay) 
    { 
    case 0:                  // D0->Kpi
      pdg2[0]=211; pdg2[1]=321;
      mPDG=TDatabasePDG::Instance()->GetParticle(421)->Mass();
      minv = rd->InvMass(nprongs,pdg2);
      if(TMath::Abs(minv-mPDG)<fD0toKpiCuts[0]) retval=kTRUE;
      pdg2[0]=321; pdg2[1]=211;
      minv = rd->InvMass(nprongs,pdg2);
      if(TMath::Abs(minv-mPDG)<fD0toKpiCuts[0]) retval=kTRUE;
      break;
    case 1:                  // JPSI->ee
      pdg2[0]=11; pdg2[1]=11;
      mPDG=TDatabasePDG::Instance()->GetParticle(443)->Mass();
      minv = rd->InvMass(nprongs,pdg2);
      if(TMath::Abs(minv-mPDG)<fBtoJPSICuts[0]) retval=kTRUE;
      break;
    case 2:                  // D+->Kpipi
      pdg3[0]=211; pdg3[1]=321; pdg3[2]=211;
      mPDG=TDatabasePDG::Instance()->GetParticle(411)->Mass();
      minv = rd->InvMass(nprongs,pdg3);
      if(TMath::Abs(minv-mPDG)<fDplusCuts[0]) retval=kTRUE;
                            // Ds+->KKpi
      pdg3[0]=321; pdg3[1]=321; pdg3[2]=211;
      mPDG=TDatabasePDG::Instance()->GetParticle(431)->Mass();
      minv = rd->InvMass(nprongs,pdg3);
      if(TMath::Abs(minv-mPDG)<fDsCuts[0]) retval=kTRUE;
      pdg3[0]=211; pdg3[1]=321; pdg3[2]=321;
      minv = rd->InvMass(nprongs,pdg3);
      if(TMath::Abs(minv-mPDG)<fDsCuts[0]) retval=kTRUE;
                            // Lc->pKpi
      pdg3[0]=2212; pdg3[1]=321; pdg3[2]=211;
      mPDG=TDatabasePDG::Instance()->GetParticle(4122)->Mass();
      minv = rd->InvMass(nprongs,pdg3);
      if(TMath::Abs(minv-mPDG)<fLcCuts[0]) retval=kTRUE;
      pdg3[0]=211; pdg3[1]=321; pdg3[2]=2212;
      minv = rd->InvMass(nprongs,pdg3);
      if(TMath::Abs(minv-mPDG)<fLcCuts[0]) retval=kTRUE; 
      break;
    default:
      break;
    }

  delete rd;

  return retval;
}
//-----------------------------------------------------------------------------
void AliAnalysisVertexingHF::SelectTracksAndCopyVertex(AliVEvent *event,
					  TObjArray &trksP,Int_t &nTrksP,
					  TObjArray &trksN,Int_t &nTrksN)
{
  // Fill two TObjArrays with positive and negative tracks and 
  // apply single-track preselection.
  // Create the AliESDVertex object (convert from AliAODVertex if necessary)
  // In case of AOD input, also fill fAODMap for track index<->ID

  const AliVVertex *vprimary = event->GetPrimaryVertex();

  if(fV1) { delete fV1; fV1=NULL; }
  if(fAODMap) { delete [] fAODMap; fAODMap=NULL; }

  Int_t nindices=0;
  UShort_t *indices = 0;
  Double_t pos[3],cov[6];

  if(!fInputAOD) { // ESD
    fV1 = new AliESDVertex(*((AliESDVertex*)vprimary));
  } else {         // AOD
    vprimary->GetXYZ(pos);
    vprimary->GetCovarianceMatrix(cov);
    fV1 = new AliESDVertex(pos,cov,100.,100,vprimary->GetName());
    indices = new UShort_t[event->GetNumberOfTracks()];
    fAODMap = new Int_t[100000];
  }


    for(Int_t itr=0; itr<event->GetNumberOfTracks(); itr++) {

    }


  nTrksP=0,nTrksN=0;

  Int_t entries = (Int_t)event->GetNumberOfTracks();
 
  // transfer ITS tracks from event to arrays
  for(Int_t i=0; i<entries; i++) {

    AliVTrack *track = (AliVTrack*)event->GetTrack(i);

    if(fInputAOD) {
      AliAODTrack *aodt = (AliAODTrack*)track;
      if(aodt->GetUsedForPrimVtxFit()) { 
	indices[nindices]=aodt->GetID(); nindices++; 
      }
      fAODMap[(Int_t)aodt->GetID()] = i;
    }

    // require refit in ITS 
    UInt_t status = track->GetStatus();
    if(fITSrefit && !(status&AliESDtrack::kITSrefit)) {
      AliDebug(2,Form("track %d is not kITSrefit",i));
      continue;
    }

    Int_t ncls0=0;
    for(Int_t l=0;l<6;l++) if(TESTBIT(track->GetITSClusterMap(),l)) ncls0++;

    // require minimum # of ITS points    
    if(ncls0<fMinITSCls)  {
      AliDebug(2,Form("track %d has %d ITS cls",i,ncls0));
      continue;
    }
    // require points on the 2 pixel layers
    if(fBothSPD) 
      if(!TESTBIT(track->GetITSClusterMap(),0) || 
	 !TESTBIT(track->GetITSClusterMap(),1)) continue;

    AliESDtrack *esdt = 0;
    if(!fInputAOD) {
      esdt = (AliESDtrack*)track;
    } else {
      esdt = new AliESDtrack(track);
    }

    // single track selection
    if(!SingleTrkCuts(*esdt)) {
      if(fInputAOD) delete esdt; 
      esdt = NULL;
      continue;
    }

    if(track->Charge()<0) { // negative track
      trksN.AddLast(esdt);
      nTrksN++;
    } else {                 // positive track
      trksP.AddLast(esdt);
      nTrksP++;
    }

  } // end loop on tracks

  // primary vertex from AOD
  if(fInputAOD) {
    delete fV1; fV1=NULL;
    vprimary->GetXYZ(pos);
    vprimary->GetCovarianceMatrix(cov);
    Double_t chi2toNDF = vprimary->GetChi2perNDF();
    Int_t ncontr=nindices;
    if(vprimary->GetTitle()=="VertexerTracksWithContraint") ncontr += 1;
    Double_t chi2=chi2toNDF*(2.*(Double_t)ncontr-3.); 
    fV1 = new AliESDVertex(pos,cov,chi2,ncontr,vprimary->GetName());
    fV1->SetTitle(vprimary->GetTitle());
    fV1->SetIndices(nindices,indices);
  }
  if(indices) { delete [] indices; indices=NULL; }


  return;
}
//-----------------------------------------------------------------------------
void AliAnalysisVertexingHF::SetD0toKpiCuts(Double_t cut0,Double_t cut1,
				   Double_t cut2,Double_t cut3,Double_t cut4,
				   Double_t cut5,Double_t cut6,
				   Double_t cut7,Double_t cut8) 
{
  // Set the cuts for D0 selection
  fD0toKpiCuts[0] = cut0;
  fD0toKpiCuts[1] = cut1;
  fD0toKpiCuts[2] = cut2;
  fD0toKpiCuts[3] = cut3;
  fD0toKpiCuts[4] = cut4;
  fD0toKpiCuts[5] = cut5;
  fD0toKpiCuts[6] = cut6;
  fD0toKpiCuts[7] = cut7;
  fD0toKpiCuts[8] = cut8;

  return;
}
//-----------------------------------------------------------------------------
void AliAnalysisVertexingHF::SetD0toKpiCuts(const Double_t cuts[9]) 
{
  // Set the cuts for D0 selection

  for(Int_t i=0; i<9; i++) fD0toKpiCuts[i] = cuts[i];

  return;
}
//-----------------------------------------------------------------------------
void AliAnalysisVertexingHF::SetBtoJPSICuts(Double_t cut0,Double_t cut1,
				   Double_t cut2,Double_t cut3,Double_t cut4,
				   Double_t cut5,Double_t cut6,
				   Double_t cut7,Double_t cut8) 
{
  // Set the cuts for J/psi from B selection
  fBtoJPSICuts[0] = cut0;
  fBtoJPSICuts[1] = cut1;
  fBtoJPSICuts[2] = cut2;
  fBtoJPSICuts[3] = cut3;
  fBtoJPSICuts[4] = cut4;
  fBtoJPSICuts[5] = cut5;
  fBtoJPSICuts[6] = cut6;
  fBtoJPSICuts[7] = cut7;
  fBtoJPSICuts[8] = cut8;

  return;
}
//-----------------------------------------------------------------------------
void AliAnalysisVertexingHF::SetBtoJPSICuts(const Double_t cuts[9]) 
{
  // Set the cuts for J/psi from B selection

  for(Int_t i=0; i<9; i++) fBtoJPSICuts[i] = cuts[i];

  return;
}
//-----------------------------------------------------------------------------
void AliAnalysisVertexingHF::SetDplusCuts(Double_t cut0,Double_t cut1,
				   Double_t cut2,Double_t cut3,Double_t cut4,
				   Double_t cut5,Double_t cut6,
				   Double_t cut7,Double_t cut8,
				   Double_t cut9,Double_t cut10,Double_t cut11)
{
  // Set the cuts for Dplus->Kpipi selection
  fDplusCuts[0] = cut0;
  fDplusCuts[1] = cut1;
  fDplusCuts[2] = cut2;
  fDplusCuts[3] = cut3;
  fDplusCuts[4] = cut4;
  fDplusCuts[5] = cut5;
  fDplusCuts[6] = cut6;
  fDplusCuts[7] = cut7;
  fDplusCuts[8] = cut8;
  fDplusCuts[9] = cut9;
  fDplusCuts[10] = cut10;
  fDplusCuts[11] = cut11;

  return;
}
//-----------------------------------------------------------------------------
void AliAnalysisVertexingHF::SetDplusCuts(const Double_t cuts[12]) 
{
  // Set the cuts for Dplus->Kpipi selection

  for(Int_t i=0; i<12; i++) fDplusCuts[i] = cuts[i];

  return;
}
//-----------------------------------------------------------------------------
void AliAnalysisVertexingHF::SetDsCuts(Double_t cut0,Double_t cut1,
				   Double_t cut2,Double_t cut3,Double_t cut4,
				   Double_t cut5,Double_t cut6,
				   Double_t cut7,Double_t cut8,
				   Double_t cut9,Double_t cut10,
				   Double_t cut11,Double_t cut12)
{
  // Set the cuts for Ds->KKpi selection
  fDsCuts[0] = cut0;
  fDsCuts[1] = cut1;
  fDsCuts[2] = cut2;
  fDsCuts[3] = cut3;
  fDsCuts[4] = cut4;
  fDsCuts[5] = cut5;
  fDsCuts[6] = cut6;
  fDsCuts[7] = cut7;
  fDsCuts[8] = cut8;
  fDsCuts[9] = cut9;
  fDsCuts[10] = cut10;
  fDsCuts[11] = cut11;
  fDsCuts[12] = cut12;

  return;
}
//-----------------------------------------------------------------------------
void AliAnalysisVertexingHF::SetDsCuts(const Double_t cuts[13]) 
{
  // Set the cuts for Ds->KKpi selection

  for(Int_t i=0; i<13; i++) fDsCuts[i] = cuts[i];

  return;
}
//-----------------------------------------------------------------------------
void AliAnalysisVertexingHF::SetLcCuts(Double_t cut0,Double_t cut1,
				   Double_t cut2,Double_t cut3,Double_t cut4,
				   Double_t cut5,Double_t cut6,
				   Double_t cut7,Double_t cut8,
				   Double_t cut9,Double_t cut10,Double_t cut11)
{
  // Set the cuts for Lc->pKpi selection
  fLcCuts[0] = cut0;
  fLcCuts[1] = cut1;
  fLcCuts[2] = cut2;
  fLcCuts[3] = cut3;
  fLcCuts[4] = cut4;
  fLcCuts[5] = cut5;
  fLcCuts[6] = cut6;
  fLcCuts[7] = cut7;
  fLcCuts[8] = cut8;
  fLcCuts[9] = cut9;
  fLcCuts[10] = cut10;
  fLcCuts[11] = cut11;

  return;
}
//-----------------------------------------------------------------------------
void AliAnalysisVertexingHF::SetLcCuts(const Double_t cuts[12]) 
{
  // Set the cuts for Lc->pKpi selection

  for(Int_t i=0; i<12; i++) fLcCuts[i] = cuts[i];

  return;
}
//-----------------------------------------------------------------------------
Bool_t AliAnalysisVertexingHF::SingleTrkCuts(AliESDtrack& trk) const 
{
  // Check if track passes some kinematical cuts  

  if(trk.Pt() < fMinPtCut) {
    //printf("pt %f\n",1./trk.GetParameter()[4]);
    return kFALSE;
  }
  Double_t d0z0[2],covd0z0[3];
  trk.PropagateToDCA(fV1,fBzkG,kVeryBig,d0z0,covd0z0);
  if(TMath::Abs(d0z0[0]) < fMind0rphiCut) {
    //printf("d0rphi %f\n",TMath::Abs(d0z0[0]));
    return kFALSE;
  }

  return kTRUE;
}
//-----------------------------------------------------------------------------
