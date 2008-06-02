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

/* $Id: AliAnalysisTaskESDfilter.cxx 24535 2008-03-16 22:43:30Z fca $ */
 
#include <TChain.h>
#include <TFile.h>
#include <TArrayI.h>

#include "AliAnalysisTaskESDfilter.h"
#include "AliAnalysisManager.h"
#include "AliESDEvent.h"
#include "AliAODEvent.h"
#include "AliESDInputHandler.h"
#include "AliAODHandler.h"
#include "AliAnalysisFilter.h"
#include "AliESDtrack.h"
#include "AliESDMuonTrack.h"
#include "AliESDVertex.h"
#include "AliESDv0.h"
#include "AliESDkink.h"
#include "AliESDcascade.h"
#include "AliESDPmdTrack.h"
#include "AliESDCaloCluster.h"
#include "AliESDCaloCells.h"
#include "AliMultiplicity.h"
#include "AliLog.h"

ClassImp(AliAnalysisTaskESDfilter)

////////////////////////////////////////////////////////////////////////

AliAnalysisTaskESDfilter::AliAnalysisTaskESDfilter():
    AliAnalysisTaskSE(),
    fTrackFilter(0x0),
    fKinkFilter(0x0),
    fV0Filter(0x0)
{
  // Default constructor
}

AliAnalysisTaskESDfilter::AliAnalysisTaskESDfilter(const char* name):
    AliAnalysisTaskSE(name),
    fTrackFilter(0x0),
    fKinkFilter(0x0),
    fV0Filter(0x0)
{
  // Constructor
}

void AliAnalysisTaskESDfilter::UserCreateOutputObjects()
{
// Create the output container
    OutputTree()->GetUserInfo()->Add(fTrackFilter);
}

void AliAnalysisTaskESDfilter::Init()
{
    // Initialization
    if (fDebug > 1) AliInfo("Init() \n");
    // Call configuration file
}


void AliAnalysisTaskESDfilter::UserExec(Option_t */*option*/)
{
// Execute analysis for current event
//
					    
  Long64_t ientry = Entry();
  if (fDebug > 0) printf("Filter: Analysing event # %5d\n", (Int_t) ientry);

  ConvertESDtoAOD();
}

void AliAnalysisTaskESDfilter::ConvertESDtoAOD() {
    // ESD Filter analysis task executed for each event
    AliESDEvent* esd = dynamic_cast<AliESDEvent*>(InputEvent());
    AliESD* old = esd->GetAliESDOld();
    
    // set arrays and pointers
    Float_t posF[3];
    Double_t pos[3];
    Double_t p[3];
    Double_t p_pos[3];
    Double_t p_neg[3];
    Double_t covVtx[6];
    Double_t covTr[21];
    Double_t pid[10];

    for (Int_t i = 0; i < 6; i++)  covVtx[i] = 0.;
    for (Int_t i = 0; i < 21; i++) covTr [i] = 0.;

    
  // loop over events and fill them
  
  // Multiplicity information needed by the header (to be revised!)
    Int_t nTracks    = esd->GetNumberOfTracks();
    Int_t nPosTracks = 0;
//    for (Int_t iTrack = 0; iTrack < nTracks; ++iTrack) 
//	if (esd->GetTrack(iTrack)->GetSign()> 0) nPosTracks++;
    
    // Update the header

    AliAODHeader* header = AODEvent()->GetHeader();
    header->SetRunNumber(esd->GetRunNumber());
    if (old) {
	header->SetBunchCrossNumber(0);
	header->SetOrbitNumber(0);
	header->SetPeriodNumber(0);
	header->SetEventType(0);
	header->SetMuonMagFieldScale(-999.); // FIXME
	header->SetCentrality(-999.);        // FIXME
    } else {
	header->SetBunchCrossNumber(esd->GetBunchCrossNumber());
	header->SetOrbitNumber(esd->GetOrbitNumber());
	header->SetPeriodNumber(esd->GetPeriodNumber());
	header->SetEventType(esd->GetEventType());
	header->SetMuonMagFieldScale(-999.); // FIXME
	header->SetCentrality(-999.);        // FIXME
    }
    
    header->SetTriggerMask(esd->GetTriggerMask()); 
    header->SetTriggerCluster(esd->GetTriggerCluster());
    header->SetMagneticField(esd->GetMagneticField());
    header->SetZDCN1Energy(esd->GetZDCN1Energy());
    header->SetZDCP1Energy(esd->GetZDCP1Energy());
    header->SetZDCN2Energy(esd->GetZDCN2Energy());
    header->SetZDCP2Energy(esd->GetZDCP2Energy());
    header->SetZDCEMEnergy(esd->GetZDCEMEnergy(0),esd->GetZDCEMEnergy(1));
//
//    
    Int_t nV0s      = esd->GetNumberOfV0s();
    Int_t nCascades = esd->GetNumberOfCascades();
    Int_t nKinks    = esd->GetNumberOfKinks();
    Int_t nVertices = nV0s + 2*nCascades /*could lead to two vertices, one V0 and the Xi */+ nKinks + 1 /* = prim. vtx*/;    
    Int_t nJets     = 0;
    Int_t nCaloClus = esd->GetNumberOfCaloClusters();
    Int_t nFmdClus  = 0;
    Int_t nPmdClus  = esd->GetNumberOfPmdTracks();
    
    if (fDebug > 0) 
	printf("   NV0=%d  NCASCADES=%d  NKINKS=%d\n", nV0s, nCascades, nKinks);

    AODEvent()->ResetStd(nTracks, nVertices, nV0s+nCascades, nJets, nCaloClus, nFmdClus, nPmdClus);

    AliAODTrack *aodTrack = 0x0;
    
    // Array to take into account the tracks already added to the AOD
    Bool_t * usedTrack = NULL;
    if (nTracks>0) {
	usedTrack = new Bool_t[nTracks];
	for (Int_t iTrack=0; iTrack<nTracks; ++iTrack) usedTrack[iTrack]=kFALSE;
    }
    // Array to take into account the V0s already added to the AOD
    Bool_t * usedV0 = NULL;
    if (nV0s>0) {
	usedV0 = new Bool_t[nV0s];
	for (Int_t iV0=0; iV0<nV0s; ++iV0) usedV0[iV0]=kFALSE;
    }
    // Array to take into account the kinks already added to the AOD
    Bool_t * usedKink = NULL;
    if (nKinks>0) {
	usedKink = new Bool_t[nKinks];
	for (Int_t iKink=0; iKink<nKinks; ++iKink) usedKink[iKink]=kFALSE;
    }
    
    // Access to the AOD container of vertices
    TClonesArray &vertices = *(AODEvent()->GetVertices());
    Int_t jVertices=0;
    
    // Access to the AOD container of tracks
    TClonesArray &tracks = *(AODEvent()->GetTracks());
    Int_t jTracks=0; 
    
    // Access to the AOD container of V0s
    TClonesArray &V0s = *(AODEvent()->GetV0s());
    Int_t jV0s=0;
    
    // Add primary vertex. The primary tracks will be defined
    // after the loops on the composite objects (V0, cascades, kinks)
    const AliESDVertex *vtx = esd->GetPrimaryVertex();
    
    vtx->GetXYZ(pos); // position
    vtx->GetCovMatrix(covVtx); //covariance matrix
    
    AliAODVertex * primary = new(vertices[jVertices++])
	AliAODVertex(pos, covVtx, vtx->GetChi2toNDF(), NULL, AliAODVertex::kPrimary);
    if (fDebug > 0) primary->Print();

    // Create vertices starting from the most complex objects
    Double_t chi2 = 0.;
    
    // Cascades
    for (Int_t nCascade = 0; nCascade < nCascades; ++nCascade) {
	AliESDcascade *cascade = esd->GetCascade(nCascade);
	
	cascade->GetXYZ(pos[0], pos[1], pos[2]);

	if (!old) {
	    chi2 = cascade->GetChi2Xi(); // = chi2/NDF since NDF = 2*2-3
	    cascade->GetPosCovXi(covVtx);
	} else {
	    chi2 = -999.;
	    for (Int_t i = 0; i < 6; i++)  covVtx[i] = 0.;
	}
	// Add the cascade vertex
	AliAODVertex * vcascade = new(vertices[jVertices++]) AliAODVertex(pos,
									  covVtx,
									  chi2,
									  primary,
									  nCascade,
									  AliAODVertex::kCascade);
	
	primary->AddDaughter(vcascade);
	
	// Add the V0 from the cascade. The ESD class have to be optimized...
	// Now we have to search for the corresponding Vo in the list of V0s
	// using the indeces of the positive and negative tracks
	
	Int_t posFromV0 = cascade->GetPindex();
	Int_t negFromV0 = cascade->GetNindex();
	
      
	AliESDv0 * v0 = 0x0;
	Int_t indV0 = -1;
	
	for (Int_t iV0=0; iV0<nV0s; ++iV0) {
	    
	    v0 = esd->GetV0(iV0);
	    Int_t posV0 = v0->GetPindex();
	    Int_t negV0 = v0->GetNindex();
	    
	    if (posV0==posFromV0 && negV0==negFromV0) {
		indV0 = iV0;
		break;
	    }
	}
	
	AliAODVertex * vV0FromCascade = 0x0;
	
	if (indV0>-1 && !usedV0[indV0] ) {
	    
	    // the V0 exists in the array of V0s and is not used
	    
	    usedV0[indV0] = kTRUE;
	    
	    v0->GetXYZ(pos[0], pos[1], pos[2]);
	    if (!old) {
		chi2 = v0->GetChi2V0();  // = chi2/NDF since NDF = 2*2-3			     
		v0->GetPosCov(covVtx);
	    } else {
		chi2 = -999.;
		for (Int_t i = 0; i < 6; i++)  covVtx[i] = 0.;
	    }

	    vV0FromCascade = new(vertices[jVertices++]) AliAODVertex(pos,
								     covVtx,
								     chi2,
								     vcascade,
								     indV0,
								     AliAODVertex::kV0);
	} else {
	    
	    // the V0 doesn't exist in the array of V0s or was used
//	    cerr << "Error: event " << esd->GetEventNumberInFile() << " cascade " << nCascade
//		 << " The V0 " << indV0 
//		 << " doesn't exist in the array of V0s or was used!" << endl;
	    
	    cascade->GetXYZ(pos[0], pos[1], pos[2]);
	    
	    if (!old) {
		chi2 = v0->GetChi2V0();
		cascade->GetPosCov(covVtx);
	    } else {
		chi2 = -999.;
		for (Int_t i = 0; i < 6; i++)  covVtx[i] = 0.;
	    }

	    vV0FromCascade = new(vertices[jVertices++]) AliAODVertex(pos,
								     covVtx,
								     chi2, // = chi2/NDF since NDF = 2*2-3 (AM)
								     vcascade,
								     indV0,
								     AliAODVertex::kV0);
	    vcascade->AddDaughter(vV0FromCascade);
	}
	
	// Add the positive tracks from the V0
	
	if (posFromV0>-1 && !usedTrack[posFromV0]) {
	    
	    usedTrack[posFromV0] = kTRUE;
	    
	    AliESDtrack *esdTrack = esd->GetTrack(posFromV0);
	    esdTrack->GetPxPyPz(p_pos);
	    esdTrack->GetXYZ(pos);
	    esdTrack->GetCovarianceXYZPxPyPz(covTr);
	    esdTrack->GetESDpid(pid);
	    
	    vV0FromCascade->AddDaughter(aodTrack =
					new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
									   esdTrack->GetLabel(), 
									   p_pos, 
									   kTRUE,
									   pos,
									   kFALSE,
									   covTr, 
									   (Short_t)esdTrack->GetSign(),
									   esdTrack->GetITSClusterMap(), 
									   pid,
									   vV0FromCascade,
									   kTRUE,  // check if this is right
									   kFALSE, // check if this is right
									   AliAODTrack::kSecondary)
		);
       if (esdTrack->GetSign() > 0) nPosTracks++;
	    aodTrack->ConvertAliPIDtoAODPID();
	    aodTrack->SetFlags(esdTrack->GetStatus());
	}
	else {
//	    cerr << "Error: event " << esd->GetEventNumberInFile() << " cascade " << nCascade
//		 << " track " << posFromV0 << " has already been used!" << endl;
	}
	
	// Add the negative tracks from the V0
	
	if (negFromV0>-1 && !usedTrack[negFromV0]) {
	    
	    usedTrack[negFromV0] = kTRUE;
	    
	    AliESDtrack *esdTrack = esd->GetTrack(negFromV0);
	    esdTrack->GetPxPyPz(p_neg);
	    esdTrack->GetXYZ(pos);
	    esdTrack->GetCovarianceXYZPxPyPz(covTr);
	    esdTrack->GetESDpid(pid);
	    
	    vV0FromCascade->AddDaughter(aodTrack =
					new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
									   esdTrack->GetLabel(),
									   p_neg,
									   kTRUE,
									   pos,
									   kFALSE,
									   covTr, 
									   (Short_t)esdTrack->GetSign(),
									   esdTrack->GetITSClusterMap(), 
									   pid,
									   vV0FromCascade,
									   kTRUE,  // check if this is right
									   kFALSE, // check if this is right
									   AliAODTrack::kSecondary)
		);
       if (esdTrack->GetSign() > 0) nPosTracks++;
	    aodTrack->ConvertAliPIDtoAODPID();
	    aodTrack->SetFlags(esdTrack->GetStatus());	    
	}
	else {
//	    cerr << "Error: event " << esd->GetEventNumberInFile() << " cascade " << nCascade
//		 << " track " << negFromV0 << " has already been used!" << endl;
	}
	
	// add it to the V0 array as well
	Double_t d0[2] = { -999., -99.};
	// counting is probably wrong
	new(V0s[jV0s++]) AliAODv0(vV0FromCascade, -999., -99., p_pos, p_neg, d0); // to be refined

	// Add the bachelor track from the cascade
	
	Int_t bachelor = cascade->GetBindex();
	
	if(bachelor>-1 && !usedTrack[bachelor]) {
	    
	    usedTrack[bachelor] = kTRUE;
	    
	    AliESDtrack *esdTrack = esd->GetTrack(bachelor);
	    esdTrack->GetPxPyPz(p);
	    esdTrack->GetXYZ(pos);
	    esdTrack->GetCovarianceXYZPxPyPz(covTr);
	    esdTrack->GetESDpid(pid);
	    
	    vcascade->AddDaughter(aodTrack =
				  new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
								     esdTrack->GetLabel(),
								     p,
								     kTRUE,
								     pos,
								     kFALSE,
								     covTr, 
								     (Short_t)esdTrack->GetSign(),
								     esdTrack->GetITSClusterMap(), 
								     pid,
								     vcascade,
								     kTRUE,  // check if this is right
								     kFALSE, // check if this is right
								     AliAODTrack::kSecondary)
		);
       if (esdTrack->GetSign() > 0) nPosTracks++;
	    aodTrack->ConvertAliPIDtoAODPID();
	    aodTrack->SetFlags(esdTrack->GetStatus());
	}
	else {
//	    cerr << "Error: event " << esd->GetEventNumberInFile() << " cascade " << nCascade
//		 << " track " << bachelor << " has already been used!" << endl;
	}
	
	// Add the primary track of the cascade (if any)
	
    } // end of the loop on cascades
   
    // V0s
    
    for (Int_t nV0 = 0; nV0 < nV0s; ++nV0) {
	
	if (usedV0[nV0]) continue; // skip if aready added to the AOD
	
	AliESDv0 *v0 = esd->GetV0(nV0);
	
	v0->GetXYZ(pos[0], pos[1], pos[2]);

	if (!old) {
	    chi2 = v0->GetChi2V0(); // = chi2/NDF since NDF = 2*2-3
	    v0->GetPosCov(covVtx);
	} else {
	    chi2 = -999.;
	    for (Int_t i = 0; i < 6; i++)  covVtx[i] = 0.;
	}


	AliAODVertex * vV0 = 
	  new(vertices[jVertices++]) AliAODVertex(pos,
						  covVtx,
						  chi2,
						  primary,
						  nV0,
						  AliAODVertex::kV0);
	primary->AddDaughter(vV0);
	
	Int_t posFromV0 = v0->GetPindex();
	Int_t negFromV0 = v0->GetNindex();
	
	// Add the positive tracks from the V0
	
	if (posFromV0>-1 && !usedTrack[posFromV0]) {
	    
	    usedTrack[posFromV0] = kTRUE;
	    
	    AliESDtrack *esdTrack = esd->GetTrack(posFromV0);
	    esdTrack->GetPxPyPz(p_pos);
	    esdTrack->GetXYZ(pos);
	    esdTrack->GetCovarianceXYZPxPyPz(covTr);
	    esdTrack->GetESDpid(pid);
	    
	    vV0->AddDaughter(aodTrack =
			     new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
								esdTrack->GetLabel(), 
								p_pos, 
								kTRUE,
								pos,
								kFALSE,
								covTr, 
								(Short_t)esdTrack->GetSign(),
								esdTrack->GetITSClusterMap(), 
								pid,
								vV0,
								kTRUE,  // check if this is right
								kFALSE, // check if this is right
								AliAODTrack::kSecondary)
		);
       if (esdTrack->GetSign() > 0) nPosTracks++;
	    aodTrack->ConvertAliPIDtoAODPID();
	    aodTrack->SetFlags(esdTrack->GetStatus());
	}
	else {
//	    cerr << "Error: event " << esd->GetEventNumberInFile() << " V0 " << nV0
//		 << " track " << posFromV0 << " has already been used!" << endl;
	}
	
	// Add the negative tracks from the V0
	
	if (negFromV0>-1 && !usedTrack[negFromV0]) {
	    
	    usedTrack[negFromV0] = kTRUE;
	    
	    AliESDtrack *esdTrack = esd->GetTrack(negFromV0);
	    esdTrack->GetPxPyPz(p_neg);
	    esdTrack->GetXYZ(pos);
	    esdTrack->GetCovarianceXYZPxPyPz(covTr);
	    esdTrack->GetESDpid(pid);
	    
	    vV0->AddDaughter(aodTrack =
			     new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
								esdTrack->GetLabel(),
								p_neg,
								kTRUE,
								pos,
								kFALSE,
								covTr, 
								(Short_t)esdTrack->GetSign(),
								esdTrack->GetITSClusterMap(), 
								pid,
								vV0,
								kTRUE,  // check if this is right
								kFALSE, // check if this is right
								AliAODTrack::kSecondary)
		);
       if (esdTrack->GetSign() > 0) nPosTracks++;
	    aodTrack->ConvertAliPIDtoAODPID();
	    aodTrack->SetFlags(esdTrack->GetStatus());
	}
	else {
//	    cerr << "Error: event " << esd->GetEventNumberInFile() << " V0 " << nV0
//		 << " track " << negFromV0 << " has already been used!" << endl;
	}

	// add it to the V0 array as well
	Double_t d0[2] = { 999., 99.};
	new(V0s[jV0s++]) AliAODv0(vV0, 999., 99., p_pos, p_neg, d0); // to be refined
    } 
    V0s.Expand(jV0s);	 
    // end of the loop on V0s
    
    // Kinks: it is a big mess the access to the information in the kinks
    // The loop is on the tracks in order to find the mother and daugther of each kink
    
    
    for (Int_t iTrack=0; iTrack<nTracks; ++iTrack) {
	
	AliESDtrack * esdTrack = esd->GetTrack(iTrack);
	
	Int_t ikink = esdTrack->GetKinkIndex(0);
	
	if (ikink && nKinks) {
	    // Negative kink index: mother, positive: daughter
	    
	    // Search for the second track of the kink
	    
	    for (Int_t jTrack = iTrack+1; jTrack<nTracks; ++jTrack) {
		
		AliESDtrack * esdTrack1 = esd->GetTrack(jTrack);
		
		Int_t jkink = esdTrack1->GetKinkIndex(0);
		
		if ( TMath::Abs(ikink)==TMath::Abs(jkink) ) {
		    
		    // The two tracks are from the same kink
		    
		    if (usedKink[TMath::Abs(ikink)-1]) continue; // skip used kinks
		    
		    Int_t imother = -1;
		    Int_t idaughter = -1;
		    
		    if (ikink<0 && jkink>0) {
			
			imother = iTrack;
			idaughter = jTrack;
		    }
		    else if (ikink>0 && jkink<0) {
			
			imother = jTrack;
			idaughter = iTrack;
		    }
		    else {
//			cerr << "Error: Wrong combination of kink indexes: "
//			     << ikink << " " << jkink << endl;
			continue;
		    }
		    
		    // Add the mother track
		    
		    AliAODTrack * mother = NULL;
		    
		    if (!usedTrack[imother]) {
			
			usedTrack[imother] = kTRUE;
			
			AliESDtrack *esdTrackM = esd->GetTrack(imother);
			esdTrackM->GetPxPyPz(p);
			esdTrackM->GetXYZ(pos);
			esdTrackM->GetCovarianceXYZPxPyPz(covTr);
			esdTrackM->GetESDpid(pid);
			
			mother = 
			    new(tracks[jTracks++]) AliAODTrack(esdTrackM->GetID(),
							       esdTrackM->GetLabel(),
							       p,
							       kTRUE,
							       pos,
							       kFALSE,
							       covTr, 
							       (Short_t)esdTrackM->GetSign(),
							       esdTrackM->GetITSClusterMap(), 
							       pid,
							       primary,
							       kTRUE, // check if this is right
							       kTRUE, // check if this is right
							       AliAODTrack::kPrimary);
         if (esdTrack->GetSign() > 0) nPosTracks++;
			mother->SetFlags(esdTrack->GetStatus());
			mother->ConvertAliPIDtoAODPID();
			primary->AddDaughter(mother);
			mother->ConvertAliPIDtoAODPID();
		    }
		    else {
//			cerr << "Error: event " << esd->GetEventNumberInFile() << " kink " << TMath::Abs(ikink)-1
//			     << " track " << imother << " has already been used!" << endl;
		  }
		    
		    // Add the kink vertex
		    AliESDkink * kink = esd->GetKink(TMath::Abs(ikink)-1);
		    
		    AliAODVertex * vkink = 
			new(vertices[jVertices++]) AliAODVertex(kink->GetPosition(),
								NULL,
								0.,
								mother,
								esdTrack->GetID(),  // This is the track ID of the mother's track!
								AliAODVertex::kKink);
		    // Add the daughter track
		  
		    AliAODTrack * daughter = NULL;
		    
		    if (!usedTrack[idaughter]) {
			
			usedTrack[idaughter] = kTRUE;
			
			AliESDtrack *esdTrackD = esd->GetTrack(idaughter);
			esdTrackD->GetPxPyPz(p);
			esdTrackD->GetXYZ(pos);
			esdTrackD->GetCovarianceXYZPxPyPz(covTr);
			esdTrackD->GetESDpid(pid);
			
			daughter = 
			    new(tracks[jTracks++]) AliAODTrack(esdTrackD->GetID(),
							       esdTrackD->GetLabel(),
							       p,
							       kTRUE,
							       pos,
							       kFALSE,
							       covTr, 
							       (Short_t)esdTrackD->GetSign(),
							       esdTrackD->GetITSClusterMap(), 
							       pid,
							       vkink,
							       kTRUE, // check if this is right
							       kTRUE, // check if this is right
							       AliAODTrack::kPrimary);
         if (esdTrack->GetSign() > 0) nPosTracks++;
			daughter->SetFlags(esdTrack->GetStatus());
			daughter->ConvertAliPIDtoAODPID();
			vkink->AddDaughter(daughter);
			daughter->ConvertAliPIDtoAODPID();
		    }
		    else {
//			cerr << "Error: event " << esd->GetEventNumberInFile() << " kink " << TMath::Abs(ikink)-1
//			     << " track " << idaughter << " has already been used!" << endl;
		    }
		}
	    }
	}      
    }
    
  
    // Tracks (primary and orphan)

    if (fDebug > 0) printf("NUMBER OF ESD TRACKS %5d\n", nTracks);
    
    for (Int_t nTrack = 0; nTrack < nTracks; ++nTrack) {
	
	
	if (usedTrack[nTrack]) continue;

	AliESDtrack *esdTrack = esd->GetTrack(nTrack);
	UInt_t selectInfo = 0;
	//
	// Track selection
	if (fTrackFilter) {
	    selectInfo = fTrackFilter->IsSelected(esdTrack);
	    if (!selectInfo) continue;
	}
	
	//
	esdTrack->GetPxPyPz(p);
	esdTrack->GetXYZ(pos);
	esdTrack->GetCovarianceXYZPxPyPz(covTr);
	esdTrack->GetESDpid(pid);
	
	Float_t impactXY, impactZ;
	
	esdTrack->GetImpactParameters(impactXY,impactZ);
	
	if (impactXY<3) {
	    // track inside the beam pipe
	    
	    primary->AddDaughter(aodTrack =
				 new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
								    esdTrack->GetLabel(),
								    p,
								    kTRUE,
								    pos,
								    kFALSE,
								    covTr, 
								    (Short_t)esdTrack->GetSign(),
								    esdTrack->GetITSClusterMap(), 
								    pid,
								    primary,
								    kTRUE, // check if this is right
								    kTRUE, // check if this is right
								    AliAODTrack::kPrimary, 
								    selectInfo)
		);
       if (esdTrack->GetSign() > 0) nPosTracks++;
	    aodTrack->SetFlags(esdTrack->GetStatus());
	    aodTrack->ConvertAliPIDtoAODPID();
	}
	else {
	  // outside the beam pipe: orphan track
	  // Don't write them anymore!
	  continue;
	  /*
	  aodTrack =
	    new(tracks[jTracks++]) AliAODTrack(esdTrack->GetID(),
					       esdTrack->GetLabel(),
					       p,
					       kTRUE,
					       pos,
					       kFALSE,
					       covTr, 
					       (Short_t)esdTrack->GetSign(),
					       esdTrack->GetITSClusterMap(), 
					       pid,
					       NULL,
					       kFALSE, // check if this is right
					       kFALSE, // check if this is right
					       AliAODTrack::kOrphan,
					       selectInfo);
     if (esdTrack->GetSign() > 0) nPosTracks++;
	  aodTrack->SetFlags(esdTrack->GetStatus());
	  aodTrack->ConvertAliPIDtoAODPID();
	  */
	}	
    } // end of loop on tracks
    
    // Update number of AOD tracks in header at the end of track loop (M.G.)
    header->SetRefMultiplicity(jTracks);
    header->SetRefMultiplicityPos(nPosTracks);
    header->SetRefMultiplicityNeg(jTracks - nPosTracks);
    if (fDebug > 0) 
      printf("   NAODTRACKS=%d  NPOS=%d  NNEG=%d\n", jTracks, nPosTracks, jTracks - nPosTracks);
    // Do not shrink the array of tracks - other filters may add to it (M.G)
//    tracks.Expand(jTracks); // remove 'empty slots' due to unwritten tracks
  
    // Access to the AOD container of PMD clusters
    TClonesArray &pmdClusters = *(AODEvent()->GetPmdClusters());
    Int_t jPmdClusters=0;
  
    for (Int_t iPmd = 0; iPmd < nPmdClus; ++iPmd) {
      // file pmd clusters, to be revised!
      AliESDPmdTrack *pmdTrack = esd->GetPmdTrack(iPmd);
      Int_t nLabel = 0;
      Int_t *label = 0x0;
      Double_t posPmd[3] = { pmdTrack->GetClusterX(), pmdTrack->GetClusterY(), pmdTrack->GetClusterZ()};
      Double_t pidPmd[9] = { 0., 0., 0., 0., 0., 0., 0., 0., 0. }; // to be revised!
      // type not set!
      // assoc cluster not set
      new(pmdClusters[jPmdClusters++]) AliAODPmdCluster(iPmd, nLabel, label, pmdTrack->GetClusterADC(), posPmd, pidPmd);
    }

    // Access to the AOD container of clusters
    TClonesArray &caloClusters = *(AODEvent()->GetCaloClusters());
    Int_t jClusters=0;
 
    for (Int_t iClust=0; iClust<nCaloClus; ++iClust) {

      AliESDCaloCluster * cluster = esd->GetCaloCluster(iClust);

      Int_t id        = cluster->GetID();
      Int_t nLabel    = cluster->GetNLabels();
      TArrayI* labels = cluster->GetLabels();
      Int_t *label = 0;
      if (labels) label = (cluster->GetLabels())->GetArray();

      Float_t energy = cluster->E();
      cluster->GetPosition(posF);
      Char_t ttype = AliAODCluster::kUndef; 

      if (cluster->GetClusterType() == AliESDCaloCluster::kPHOSCluster) {
	ttype=AliAODCluster::kPHOSNeutral;
      } 
      else if (cluster->GetClusterType() == AliESDCaloCluster::kEMCALClusterv1) {
	ttype = AliAODCluster::kEMCALClusterv1;
      }

      
      AliAODCaloCluster *caloCluster = new(caloClusters[jClusters++]) AliAODCaloCluster(id,
											nLabel,
											label,
											energy,
											pos,
											NULL,
											ttype);
      
      caloCluster->SetCaloCluster(cluster->GetDistanceToBadChannel(),
				  cluster->GetClusterDisp(),
				  cluster->GetM20(), cluster->GetM02(),
				  cluster->GetEmcCpvDistance(),  
				  cluster->GetNExMax(),cluster->GetTOF()) ;

      caloCluster->SetNCells(cluster->GetNCells());
      caloCluster->SetCellsAbsId(cluster->GetCellsAbsId());
      caloCluster->SetCellsAmplitudeFraction(cluster->GetCellsAmplitudeFraction());

      TArrayI* matchedT = 	cluster->GetTracksMatched();
      if (matchedT) {	
	for (Int_t im = 0; im < matchedT->GetSize(); im++) {
	  caloCluster->AddTrackMatched((esd->GetTrack(im)));
	}
      }

    } 
    caloClusters.Expand(jClusters); // resize TObjArray to 'remove' slots for pseudo clusters	 
    // end of loop on calo clusters

    // fill EMCAL cell info
    if (esd->GetEMCALCells()) { // protection against missing ESD information
      AliESDCaloCells &esdEMcells = *(esd->GetEMCALCells());
      Int_t nEMcell = esdEMcells.GetNumberOfCells() ;
      
      AliAODCaloCells &aodEMcells = *(AODEvent()->GetEMCALCells());
      aodEMcells.CreateContainer(nEMcell);
      aodEMcells.SetType(AliAODCaloCells::kEMCAL);
      for (Int_t iCell = 0; iCell < nEMcell; iCell++) {      
	aodEMcells.SetCell(iCell,esdEMcells.GetCellNumber(iCell),esdEMcells.GetAmplitude(iCell));
      }
      aodEMcells.Sort();
    }

    // fill PHOS cell info
    if (esd->GetPHOSCells()) { // protection against missing ESD information
      AliESDCaloCells &esdPHcells = *(esd->GetPHOSCells());
      Int_t nPHcell = esdPHcells.GetNumberOfCells() ;
      
      AliAODCaloCells &aodPHcells = *(AODEvent()->GetPHOSCells());
      aodPHcells.CreateContainer(nPHcell);
      aodPHcells.SetType(AliAODCaloCells::kPHOS);
      for (Int_t iCell = 0; iCell < nPHcell; iCell++) {      
	aodPHcells.SetCell(iCell,esdPHcells.GetCellNumber(iCell),esdPHcells.GetAmplitude(iCell));
      }
      aodPHcells.Sort();
    }

    // tracklets    
    AliAODTracklets &SPDTracklets = *(AODEvent()->GetTracklets());
    const AliMultiplicity *mult = esd->GetMultiplicity();
    if (mult) {
      if (mult->GetNumberOfTracklets()>0) {
	SPDTracklets.CreateContainer(mult->GetNumberOfTracklets());

	for (Int_t n=0; n<mult->GetNumberOfTracklets(); n++) {
	  SPDTracklets.SetTracklet(n, mult->GetTheta(n), mult->GetPhi(n), mult->GetDeltaPhi(n), mult->GetLabel(n, 0), mult->GetLabel(n, 1));
	}
      }
    } else {
      //Printf("ERROR: AliMultiplicity could not be retrieved from ESD");
    }

    delete [] usedTrack;
    delete [] usedV0;
    delete [] usedKink;

    return;
}

void AliAnalysisTaskESDfilter::Terminate(Option_t */*option*/)
{
// Terminate analysis
//
    if (fDebug > 1) printf("AnalysisESDfilter: Terminate() \n");
}

