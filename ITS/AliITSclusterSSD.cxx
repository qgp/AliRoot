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

#include <Riostream.h>
#include "TArrayI.h"
#include "TClonesArray.h"
#include "AliITSdigit.h"
#include "AliITSclusterSSD.h"

ClassImp(AliITSclusterSSD)

//______________________________________________________________________
AliITSclusterSSD::AliITSclusterSSD(){
    // default constructor

    fSide        = kTRUE;
    fDigits      = 0;
    fNDigits     = 0;
    fDigitsIndex = 0;
    fNCrosses    = 0;
    fTotalSignal = -1;
    fNTracks      = -1;
    fLeftNeighbour  = kFALSE;
    fRightNeighbour = kFALSE;
    fCrossedClusterIndexes = 0;
    fConsumed=kFALSE;
}
//______________________________________________________________________
AliITSclusterSSD::AliITSclusterSSD(Int_t ndigits, Int_t *DigitIndexes, 
				   TObjArray *Digits, Bool_t side){
    // non-default constructor

    fNDigits = ndigits;
    fDigits = Digits;
    fSide = side;
    fDigitsIndex = new TArrayI(fNDigits,DigitIndexes );	
    fNCrosses    = 0;
    fCrossedClusterIndexes = new TArrayI(300);
    fLeftNeighbour  = kFALSE;
    fRightNeighbour = kFALSE;
    fTotalSignal =-1;
    fNTracks    = -1;
    fConsumed=kFALSE;
}
//______________________________________________________________________
AliITSclusterSSD::~AliITSclusterSSD(){
    // destructor

    delete fDigitsIndex;
    delete fCrossedClusterIndexes;
}
//______________________________________________________________________
AliITSclusterSSD::AliITSclusterSSD(const AliITSclusterSSD &OneSCluster){
    // copy constructor

    if (this == &OneSCluster) return;
    fNDigits = OneSCluster.fNDigits;
    fSide=OneSCluster.fSide;
    fDigits=OneSCluster.fDigits;
    fDigitsIndex = new TArrayI(fNDigits);
    fLeftNeighbour  = OneSCluster.fLeftNeighbour;
    fRightNeighbour = OneSCluster.fRightNeighbour;
    fTotalSignal =-1;
    fNTracks     = -1;
    fNCrosses = OneSCluster.fNCrosses;
    fConsumed = OneSCluster.fConsumed;
    Int_t i;
    for (i = 0; i< fNCrosses ; i++){
	fCrossedClusterIndexes[i] = OneSCluster.fCrossedClusterIndexes[i];
    }
    for (i = 0; i< fNDigits ; i++){
	fDigitsIndex[i]=OneSCluster.fDigitsIndex[i];
    }
    return;
}
//______________________________________________________________________
AliITSclusterSSD& AliITSclusterSSD::operator=(const AliITSclusterSSD 
					      &OneSCluster){
    // assignment operator

    if (this == &OneSCluster) return *this;
    fNDigits = OneSCluster.fNDigits;
    fSide=OneSCluster.fSide;
    fDigits=OneSCluster.fDigits;
    fDigitsIndex = new TArrayI(fNDigits);
    fLeftNeighbour  = OneSCluster.fLeftNeighbour;
    fRightNeighbour = OneSCluster.fRightNeighbour;
    fTotalSignal =-1;
    fNTracks     = -1;
    fNCrosses = OneSCluster.fNCrosses;
    fConsumed = OneSCluster.fConsumed;
    Int_t i;
    for (i = 0; i< fNCrosses ; i++){
	fCrossedClusterIndexes[i] = OneSCluster.fCrossedClusterIndexes[i];
    }
    for (i = 0; i< fNDigits ; i++){
	fDigitsIndex[i]=OneSCluster.fDigitsIndex[i];
    }
    return *this;
}
//______________________________________________________________________
Int_t AliITSclusterSSD::SplitCluster(Int_t where, Int_t *outdigits){
    //This methods generate data necessery to make new object of this class
    //I choosen this way, because methods TClonesArray::Add* dont work
    //so I have to use constraction: new (a[i]) Creator(params...);
    //where 'a' is a TClonesArray 
    //This method generate params - see AliITSmoduleSSD::SplitCluster;
    Int_t tmp = fNDigits;
    Int_t ind = 0;

    outdigits[ind++]=(*fDigitsIndex)[where];
    //coping border strip (it is shared by this two clusters)
    for (Int_t i = (where+1); i < tmp; i++) {
	outdigits[ind++]=(*fDigitsIndex)[i];  
	//"moving" strips from this to the new one 
	(*fDigitsIndex)[i]=-1;   
	fNDigits--;   //deleting strips from this cluster
    } 
    return ind;		
}
//______________________________________________________________________
Int_t AliITSclusterSSD::GetDigitStripNo(Int_t digit){
    // return strip no of a digit
    if (digit<0) return -1;
    return (digit>(fNDigits-1)) ? -1 :
	((AliITSdigitSSD*)(fDigits->At((*fDigitsIndex)[digit])))->GetStripNumber();
}
//______________________________________________________________________
Int_t AliITSclusterSSD::GetDigitSignal(Int_t digit){
    // returns digit signal
    Int_t index,signal;
    if (digit<0||digit>=fNDigits) return -1;
    index  = (*fDigitsIndex)[digit];
    signal = ((AliITSdigitSSD*)(fDigits->At(index)))->GetSignal();
    /*
      if(signal>1.e5) printf("GetDigitSignal: digit %d index %d signal %d\n",
      digit,index, signal);
    */
    return  signal;
}
//______________________________________________________________________
void  AliITSclusterSSD::AddCross(Int_t clIndex){
    // add cluster cross to list of cluster crosses
  
    (*fCrossedClusterIndexes)[fNCrosses++] = clIndex;
}
//______________________________________________________________________
Int_t AliITSclusterSSD::GetCross(Int_t crIndex){
    // return crossing cluster

    return ((crIndex>-1)&&(crIndex<fNCrosses))?(*fCrossedClusterIndexes)[crIndex]:-1;
}
//______________________________________________________________________
Double_t AliITSclusterSSD::CentrOfGravity(){
    // return center of gravity of the cluster
    Float_t ret=0;
  
    if (fLeftNeighbour) ret+=(GetDigitStripNo(0)*0.5*GetDigitSignal(0));
    else ret+=(GetDigitStripNo(0)*GetDigitSignal(0));
    if (fRightNeighbour) ret+=(GetDigitStripNo(fNDigits -1)*0.5*GetDigitSignal(fNDigits -1));
    else ret+=(GetDigitStripNo(fNDigits -1)*GetDigitSignal(fNDigits-1));
    for (Int_t i=1;i<fNDigits-1;i++){
	ret +=GetDigitStripNo(i)*GetDigitSignal(i);
    }// end for i
  
    if (fTotalSignal<0) GetTotalSignal();
	
    return (ret/fTotalSignal);
}
//______________________________________________________________________
Float_t AliITSclusterSSD::GetTotalSignal(){
    // return total signal
  
    if(fTotalSignal <0){
	fTotalSignal=0;
	if (fNDigits ==1)  {
	    fTotalSignal = (Float_t)GetDigitSignal(0);
	    //printf("1 digit: signal %d \n",GetDigitSignal(0)); 
	    return fTotalSignal;
	}
	if (fLeftNeighbour) fTotalSignal += (Float_t)(0.5*GetDigitSignal(0));
	else fTotalSignal += (Float_t) GetDigitSignal(0);
	//printf("GetTotalSignal :i DigitSignal %d %d \n",0,GetDigitSignal(0));
	if (fRightNeighbour) fTotalSignal += (Float_t)(0.5*GetDigitSignal(
                                                                 fNDigits -1));
      else fTotalSignal += (Float_t)GetDigitSignal(fNDigits-1);
	//printf("GetTotalSignal :i  DigitSignal %d %d \n",fNDigits -1,GetDigitSignal(fNDigits -1));
	for (Int_t i = 1;i<fNDigits -1;i++){
	    fTotalSignal += (Float_t)GetDigitSignal(i);
	    //printf("GetTotalSignal :i  DigitSignal %d %d \n",i,GetDigitSignal(i)); 
      	}
	//printf("GetTotalSignal: fNDigits %d fTotalSignal %.0f \n",fNDigits,fTotalSignal); 
    }
    return fTotalSignal;
}
//______________________________________________________________________
Float_t  AliITSclusterSSD::GetTotalSignalError(){
    // return the error on the signal
    Float_t err =0;
    for (Int_t i =1; i<fNDigits -1; i++){
	err+=0.1*GetDigitSignal(i);   
    } 
    if (GetLeftNeighbour()){
	err+=GetDigitSignal(0);
    }else{
	err+=0.1*GetDigitSignal(0);
    } 
    if (GetRightNeighbour()){
	err+=GetDigitSignal(fNDigits -1);
    }else{
	err+=0.1*GetDigitSignal(fNDigits -1);
    }
    return err;
}
//______________________________________________________________________
void AliITSclusterSSD::DelCross(Int_t index){
    // remove cross clusters from the list of cross clusters
    Int_t i,j; //iterators

    for (i =0;i<fNCrosses;i++){
	if ((*fCrossedClusterIndexes)[i] == index){
	    for (j=i;j<fNCrosses-1;j++){
		(*fCrossedClusterIndexes)[j]=(*fCrossedClusterIndexes)[j+1];
	    }
	    fNCrosses--;
	    return; 
	}
    }
}
//______________________________________________________________________
Int_t  *AliITSclusterSSD::GetTracks(Int_t &nt){
    // return the track number of the cluster
    Int_t ntrk = GetDigit(0)->GetNTracks();
    Int_t ndig = GetNumOfDigits();
    Int_t *idig = new Int_t[ndig];
    Int_t *sdig = new Int_t[ndig];
    Int_t *itrk = new Int_t[ndig*ntrk];
    Int_t i,j,k,l,trk;
    Bool_t b;

    for(i=0;i<ndig;i++){idig[i] = i;sdig[i] = GetDigit(i)->GetSignal();}
    TMath::Sort(ndig,sdig,idig,kTRUE);
    for(i=0;i<ndig*ntrk;i++) itrk[i] = -3;
    j = k = l = 0;
    for(i=0;i<ndig;i++){ // fill itrk with track numbers in order of digit size
	j = idig[i];
	for(k=0;k<ntrk;k++) if((trk = GetDigit(j)->GetTrack(k))>=0) {
	    itrk[l] = trk;
	    l++;
	} // end for k/if
    } // end for i
    for(i=0;i<10;i++) fTrack[i] = -3;
    fTrack[0] = itrk[0]; // first element
    k = 1;
    b = kTRUE;
    for(i=1;i<l;i++){
	for(j=0;j<k;j++) if(fTrack[j]==itrk[i]) b = kFALSE;
	if(b){fTrack[k] = itrk[i]; k++;}
	if(k>9) break;
    } // end for i
    nt = k;

    delete[] idig;
    delete[] sdig;
    delete[] itrk;

    return fTrack;
/*
    Int_t *tidx=0;
    Int_t i, j,n;
    Int_t bit =0;
    Int_t ntracks=0;
    nt=0;

    for (i=0;i<10;i++) fTrack[i] = -3;
   
    //cout<<"GetTrack start -------: fNDigits ="<<fNDigits<<endl;

    for (i = 0; i<fNDigits; i++) {
	tidx = GetDigit(i)->GetTracks();
	n    = GetDigit(i)->GetNTracks();
	for (j = 0; j<n && j<10;j++) {
	    if (tidx[j] >= 0) {
		if(ntracks == 0){
		    fTrack[ntracks] = tidx[j];
		    ntracks++; 
		}else if(tidx[j] != fTrack[ntracks-1]){
		    ntracks++; 
		    if(ntracks > 9) {
			bit = 1;
			break;
		    } // end if ntracks > 9
		    fTrack[ntracks-1] = tidx[j];
		} // end if ntracke == 0
	    } // end if tidx[j] >=0
	} // 3-tracks loop for the digit 
	if(bit == 1) break;
    } // digit loop

    SetNTracks(ntracks); 
    nt = ntracks;
    return &(fTrack[0]);
*/
}
//______________________________________________________________________
Double_t AliITSclusterSSD::GetPosition(){
    // return position of the cluster
    Float_t ret;

    switch(fNDigits){
    case  1:
	ret = GetDigitStripNo(0);
	break;
    case  2:
	ret = EtaAlgorithm();
	break;       
    default:
	ret = CentrOfGravity();   
    }
    return ret;
}
//______________________________________________________________________
Double_t AliITSclusterSSD::EtaAlgorithm(){
    // algorithm for determing cluster position
    if (fNDigits != 2) return -1;

    Int_t strip1  = GetDigit(0)->GetStripNumber(); 
    Int_t strip2  = GetDigit(1)->GetStripNumber();
    Int_t signal1 = GetDigit(0)->GetSignal();
    Int_t signal2 = GetDigit(1)->GetSignal();

    Double_t eta;
  
 
    if (strip1<strip2){
	eta = ((Double_t)signal2)/((Double_t)(signal1+signal2));
	if (eta<0.04) return strip1;
	if (eta>0.96) return strip2;
	return (strip1 + 0.43478261*eta + 0.2826087);   
    } else{
	eta = ((Double_t)signal1)/((Double_t)(signal1+signal2));
	if (eta<0.04) return strip2;
	if (eta>0.96) return strip1;
	return (strip2 + 0.43478261*eta + 0.2826087);   
    }
}
//______________________________________________________________________
Double_t  AliITSclusterSSD::GetPositionError(){
    // return the position error
    return (GetNumOfDigits()+1)/2;
}
//______________________________________________________________________
Bool_t AliITSclusterSSD::IsCrossingWith(Int_t idx){
    // return the cluster to which he crosses

    for (Int_t i =0; i< fNCrosses;i++){
	if (GetCross(i) == idx) return kTRUE;
    }
    return kFALSE;
}
