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


#include "AliHLTVertexer.h"
#include "AliTracker.h"
#include "TMath.h"
#include "AliESDtrack.h"
#include "AliESDv0.h"
#include "AliESDVertex.h"
#include "AliESDEvent.h"
#include "AliKFParticle.h"
#include "AliKFVertex.h"

ClassImp(AliHLTVertexer)

AliHLTVertexer::AliHLTVertexer():
  fESD(0),
  fTrackInfos(0),
  fPrimaryVtx(),
  fFillVtxConstrainedTracks(1)
{
}

AliHLTVertexer::AliHLTVertexer(const AliHLTVertexer & ):
  fESD(0),
  fTrackInfos(0),
  fPrimaryVtx(),
  fFillVtxConstrainedTracks(1)
{
}

void AliHLTVertexer::SetESD( AliESDEvent *event )
{
  //* Fill fTrackInfo array

  delete[] fTrackInfos;
  fESD = event;

  AliKFParticle::SetField( fESD->GetMagneticField() );

  Int_t nESDTracks=event->GetNumberOfTracks(); 
  fTrackInfos = new AliESDTrackInfo[ nESDTracks ];

  for (Int_t iTr=0; iTr<nESDTracks; iTr++){ 
  
    AliESDTrackInfo &info = fTrackInfos[iTr];
    info.fOK = 0;
    info.fPrimUsedFlag = 0;
    
    //* track quality check

    AliESDtrack *pTrack = event->GetTrack(iTr);    
    if( !pTrack  ) continue;
    if (pTrack->GetKinkIndex(0)>0) continue;
    if ( !( pTrack->GetStatus()&AliESDtrack::kTPCin ) ) continue;
    
    //* Construct KFParticle for the track

    info.fParticle = AliKFParticle( *pTrack->GetInnerParam(), 211 );    
    info.fOK = 1;
  }
}

void AliHLTVertexer::FindPrimaryVertex(  )
{
  //* Find event primary vertex

  int nTracks = fESD->GetNumberOfTracks();

  const AliKFParticle **vSelected = new const AliKFParticle*[nTracks]; //* Selected particles for vertex fit
  Int_t *vIndex = new int [nTracks];                    //* Indices of selected particles
  Bool_t *vFlag = new bool [nTracks];                    //* Flags returned by the vertex finder

  fPrimaryVtx.Initialize();
  fPrimaryVtx.SetBeamConstraint(fESD->GetDiamondX(),fESD->GetDiamondY(),0,
				TMath::Sqrt(fESD->GetSigma2DiamondX()),TMath::Sqrt(fESD->GetSigma2DiamondY()),5.3);
  
  Int_t nSelected = 0;
  for( Int_t i = 0; i<nTracks; i++){ 
    if(!fTrackInfos[i].fOK ) continue;
    //if( fESD->GetTrack(i)->GetTPCNcls()<60  ) continue;
    const AliKFParticle &p = fTrackInfos[i].fParticle;
    Double_t chi = p.GetDeviationFromVertex( fPrimaryVtx );      
    if( chi > 3.5 ) continue;
    vSelected[nSelected] = &(fTrackInfos[i].fParticle);
    vIndex[nSelected] = i;
    nSelected++;  
  }
  fPrimaryVtx.ConstructPrimaryVertex( vSelected, nSelected, vFlag, 3. );
  for( Int_t i = 0; i<nSelected; i++){ 
    if( vFlag[i] ) fTrackInfos[vIndex[i]].fPrimUsedFlag = 1;
  }
  
  if( fPrimaryVtx.GetNContributors()>3 ){
    AliESDVertex vESD( fPrimaryVtx.Parameters(), fPrimaryVtx.CovarianceMatrix(), fPrimaryVtx.GetChi2(), fPrimaryVtx.GetNContributors() );
    fESD->SetPrimaryVertexTracks( &vESD );

    // relate the tracks to vertex

    if( fFillVtxConstrainedTracks ){      
      for( Int_t i = 0; i<nTracks; i++ ){
	if( !fTrackInfos[i].fPrimUsedFlag ) continue;	
	fESD->GetTrack(i)->RelateToVertex( &vESD, fESD->GetMagneticField(),100. );
      }
    }

  } else {
    for( Int_t i = 0; i<nTracks; i++)
      fTrackInfos[i].fPrimUsedFlag = 0;
  }


  delete[] vSelected;
  delete[] vIndex;
  delete[] vFlag;
}

void AliHLTVertexer::FindV0s(  )
{
  //* V0 finder

  int nTracks = fESD->GetNumberOfTracks();
  //AliKFVertex primVtx( *fESD->GetPrimaryVertexTracks() );
  AliKFVertex &primVtx = fPrimaryVtx;
  if( primVtx.GetNContributors()<3 ) return;

  bool *constrainedV0   = new bool[nTracks];
  for( Int_t iTr = 0; iTr<nTracks; iTr++ ){ 
    AliESDTrackInfo &info = fTrackInfos[iTr];
    info.fPrimDeviation = info.fParticle.GetDeviationFromVertex( primVtx );
    constrainedV0[iTr] = 0;
  }
  

  for( Int_t iTr = 0; iTr<nTracks; iTr++ ){ //* first daughter

    AliESDTrackInfo &info = fTrackInfos[iTr];
    if( !info.fOK ) continue;    
    if( info.fParticle.GetQ() >0 ) continue;    
    if( info.fPrimDeviation <2.5 ) continue;

    for( Int_t jTr = 0; jTr<nTracks; jTr++ ){  //* second daughter

      AliESDTrackInfo &jnfo = fTrackInfos[jTr];
      if( !jnfo.fOK ) continue;
      if( jnfo.fParticle.GetQ() < 0 ) continue;
      if( jnfo.fPrimDeviation <2.5 ) continue;
   
      //* construct V0 mother

      AliKFParticle v0( info.fParticle, jnfo.fParticle );     

      //* check V0 Chi^2
      
      if( v0.GetChi2()<0 || v0.GetChi2() > 9.*v0.GetNDF() ) continue;

      //* subtruct daughters from primary vertex 

      AliKFVertex primVtxCopy = primVtx;    
       
      if( info.fPrimUsedFlag ){	
	if( primVtxCopy.GetNContributors()<=2 ) continue;
	primVtxCopy -= info.fParticle;
      }
      if( jnfo.fPrimUsedFlag ){
	if( primVtxCopy.GetNContributors()<=2 ) continue;
	primVtxCopy -= jnfo.fParticle;
      }
      //* Check v0 Chi^2 deviation from primary vertex 

      if( v0.GetDeviationFromVertex( primVtxCopy ) >3. ) continue;

      //* Add V0 to primary vertex to improve the primary vertex resolution

      primVtxCopy += v0;      

      //* Set production vertex for V0

      v0.SetProductionVertex( primVtxCopy );

      //* Check chi^2 for a case

      if( v0.GetChi2()<0 || v0.GetChi2()> 9.*v0.GetNDF() ) continue;
      
      //* Get V0 decay length with estimated error

      Double_t length, sigmaLength;
      if( v0.GetDecayLength( length, sigmaLength ) ) continue;

      //* Reject V0 if it decays too close[sigma] to the primary vertex

      if( length  <3.5*sigmaLength ) continue;

      //* add ESD v0 
      
      AliESDv0 v0ESD( *fESD->GetTrack( iTr ), iTr, *fESD->GetTrack( jTr ), jTr );  
      fESD->AddV0( &v0ESD );

      // relate the tracks to vertex

      if( fFillVtxConstrainedTracks ){
	if( constrainedV0[iTr] || constrainedV0[jTr]
	    || info.fPrimDeviation < 4. || jnfo.fPrimDeviation <4. ) continue;
	AliESDVertex vESD(v0.Parameters(), v0.CovarianceMatrix(), v0.GetChi2(), 2);
	fESD->GetTrack(iTr)->RelateToVertex( &vESD, fESD->GetMagneticField(),100. );
	fESD->GetTrack(jTr)->RelateToVertex( &vESD, fESD->GetMagneticField(),100. );
	constrainedV0[iTr] = 1;
	constrainedV0[jTr] = 1;	
      }
    }
  }
  delete[] constrainedV0;
}

