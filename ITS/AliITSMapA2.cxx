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

#include <TH1.h>
#include <TObjArray.h>
#include <TMath.h>

#include "AliITSMapA2.h"
#include "AliITSsegmentation.h"
#include "AliITSresponse.h"
#include "AliITSdigit.h"

////////////////////////////////////////////////////////////////////////
//  Map Class for ITS. Implementation A2. In this implementation, the //
// 2 dimensional (iz,ix) map is filled with Double precision floating //
// point values. Since this class is derived for AliITSMapA1 it also  //
// has all of the functionality of that class as well. For each       //
// cell a corresponding TObject, a hit, can also be stored.           //
//  The detector geometry is accessed via the that detectors          //
// segmentation class and stored here for conveniance.                //
////////////////////////////////////////////////////////////////////////

ClassImp(AliITSMapA2)

//______________________________________________________________________
AliITSMapA2::AliITSMapA2(){
    // default constructor

    fSegmentation  = 0;
    fNpz           = 0;
    fNpx           = 0;
    fMaxIndex      = 0;
    fHitMapD       = 0;
    fObjects       = 0;
    fNobjects      = 0;
    fMapThresholdD =0.;
}
//______________________________________________________________________
AliITSMapA2::AliITSMapA2(AliITSsegmentation *seg){
    //constructor

    fScaleSizeZ    = 1;
    fScaleSizeX    = 1;
    fSegmentation  = seg;
    fNpz           = fSegmentation->Npz();
    fNpx           = fSegmentation->Npx();
    fMaxIndex      = fNpz*fNpx+fNpx;       // 2 halves of detector
    fHitMapD       = new Double_t[fMaxIndex+1];
    fMapThresholdD = 0.;
    fObjects       = 0;
    fNobjects      = 0;
    ClearMap();
}
//______________________________________________________________________
AliITSMapA2::AliITSMapA2(AliITSsegmentation *seg,
			 Int_t scalesizeX, Int_t scalesizeZ){
    //constructor

    fSegmentation  = seg;
    fScaleSizeX    = scalesizeX;
    fScaleSizeZ    = scalesizeZ;
    fNpz           = fScaleSizeZ*fSegmentation->Npz();
    fNpx           = fScaleSizeX*fSegmentation->Npx();
    fMaxIndex      = fNpz*fNpx+fNpx;             // 2 halves of detector
    fHitMapD       = new Double_t[fMaxIndex+1];
    fMapThresholdD = 0.;
    fObjects       = 0;
    fNobjects      = 0;
    ClearMap();
}
//______________________________________________________________________
AliITSMapA2::AliITSMapA2(AliITSsegmentation *seg, TObjArray *obj, 
			 Double_t thresh){
    //constructor

    fNobjects      = 0;
    fScaleSizeZ    = 1;
    fScaleSizeX    = 1;
    fSegmentation  = seg;
    fNpz           = fSegmentation->Npz();
    fNpx           = fSegmentation->Npx();
    fMaxIndex      = fNpz*fNpx+fNpx;             // 2 halves of detector  
    fHitMapD       = new Double_t[fMaxIndex+1];
    fObjects       =  obj;
    if (fObjects) fNobjects = fObjects->GetEntriesFast();
    fMapThresholdD = thresh;
    ClearMap();
}
//______________________________________________________________________
AliITSMapA2::~AliITSMapA2(){
    //destructor

    if (fHitMapD) delete[] fHitMapD;
}
//______________________________________________________________________
AliITSMapA2::AliITSMapA2(const AliITSMapA2 &source){
    //     Copy Constructor 

    if(&source == this) return;

    this->fMapThresholdD = source.fMapThresholdD;
    this->fScaleSizeX    = source.fScaleSizeX;
    this->fScaleSizeZ    = source.fScaleSizeZ;
    this->fHitMapD       = source.fHitMapD;
    return;
}
//______________________________________________________________________
AliITSMapA2& AliITSMapA2::operator=(const AliITSMapA2 &source) {
    //    Assignment operator

    if(&source == this) return *this;

    this->fMapThresholdD = source.fMapThresholdD;
    this->fScaleSizeX    = source.fScaleSizeX;
    this->fScaleSizeZ    = source.fScaleSizeZ;
    this->fHitMapD       = source.fHitMapD;
    return *this;
}
//______________________________________________________________________
void AliITSMapA2::ClearMap(){
    //clear array

    memset(fHitMapD,0,sizeof(Double_t)*fMaxIndex);
}
//______________________________________________________________________
void  AliITSMapA2::FillMap(){
    // fills signal map from digits - apply a threshold for signal
  
    if (!fObjects) return;

    Int_t ndigits = fObjects->GetEntriesFast();
    if (!ndigits) return;

    AliITSdigit *dig;
    for (Int_t ndig=0; ndig<ndigits; ndig++) {
	dig = (AliITSdigit*)fObjects->UncheckedAt(ndig);
	Double_t signal = (Double_t)(dig->fSignal);
	if (signal > fMapThresholdD) SetHit(dig->fCoord1,dig->fCoord2,signal);
    } // end for ndig
}
//______________________________________________________________________
void AliITSMapA2::FlagHit(Int_t iz, Int_t ix){
  //flag an entry

    fHitMapD[CheckedIndex(iz, ix)]=
                -1000.*TMath::Abs((Int_t)(fHitMapD[CheckedIndex(iz, ix)])+1.);
}
//______________________________________________________________________
TObject* AliITSMapA2::GetHit(Int_t i, Int_t dummy){
  //return a pointer to the 1D histogram

    if (fObjects) {
	return fObjects->UncheckedAt(i);
    } else return NULL;
}
//______________________________________________________________________
Double_t AliITSMapA2::GetSignal(Int_t index){
    //get signal in a cell 

    if (index<fMaxIndex) return (index <0) ? 0. : fHitMapD[index];
    else return 0.;
}
//______________________________________________________________________
FlagType AliITSMapA2::TestHit(Int_t iz, Int_t ix){
    // check if the entry has already been flagged

    if (CheckedIndex(iz, ix) < 0) return kEmpty;
    Int_t inf=(Int_t)fHitMapD[CheckedIndex(iz, ix)];
    
    if (inf <= -1000) {
	return kUsed;
    } else if (inf == 0) {
	return kEmpty;
    } else {
	return kUnused;
    } // end if inf...
}
//______________________________________________________________________
void  AliITSMapA2::FillMapFromHist(){
    // fills map from 1D histograms

    if (!fObjects) return;

    // an example
    for( Int_t i=0; i<fNobjects; i++) {
	TH1F *hist =(TH1F *)fObjects->UncheckedAt(i);
	Int_t nsamples = hist->GetNbinsX();
	for( Int_t j=0; j<nsamples; j++) {
	    Double_t signal = (Double_t)(hist->GetBinContent(j+1));
	    if (signal > fMapThresholdD) SetHit(i,j,signal);
	} // end for j
    } // end for i
}
//______________________________________________________________________
void  AliITSMapA2::FillHist(){
    // fill 1D histograms from map

    if (!fObjects || fScaleSizeX != 1) return;

    // an example
    for( Int_t i=0; i<fNobjects; i++) {
	TH1F *hist =(TH1F *)fObjects->UncheckedAt(i);
	for( Int_t j=0; j<fNpx; j++) {
	    Double_t signal=GetSignal(i,j);
	    if (signal > fMapThresholdD) hist->Fill((Float_t)j,signal);
	} // end for j
    } // end for i
}
//______________________________________________________________________
void  AliITSMapA2::ResetHist(){
    // Reset histograms

    if (!fObjects) return;

    for( Int_t i=0; i<fNobjects; i++) {
	if ((*fObjects)[i])    ((TH1F*)(*fObjects)[i])->Reset();
    } // end for i
}
//______________________________________________________________________
void AliITSMapA2::AddSignal(Int_t iz,Int_t ix,Double_t sig){
    // Addes sig to cell iz. equivalent to the very common
    // sig = fMapA2->GetSignal(iz,ix) + sig; fMapA2->SetHit(iz,ix,sig);


    Int_t index=GetHitIndex(iz,ix);
    if(index<0) return;
    fHitMapD[CheckedIndex(iz, ix)] += sig;    
}
