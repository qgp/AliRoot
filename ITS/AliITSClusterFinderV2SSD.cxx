/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
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

////////////////////////////////////////////////////////////////////////////
//            Implementation of the ITS clusterer V2 class                //
//                                                                        //
//          Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch            //
//          Revised: Enrico Fragiacomo, enrico.fragiacomo@ts.infn.it      //
//          Revised 23/06/08: Marco Bregant
//                                                                        //
///////////////////////////////////////////////////////////////////////////

#include <Riostream.h>


#include "AliITSClusterFinderV2SSD.h"
#include "AliITSRecPoint.h"
#include "AliITSgeomTGeo.h"
#include "AliITSDetTypeRec.h"
#include "AliRawReader.h"
#include "AliITSRawStreamSSD.h"
#include <TClonesArray.h>
#include "AliITSdigitSSD.h"
#include "AliITSReconstructor.h"
#include "AliITSCalibrationSSD.h"
#include "AliITSsegmentationSSD.h"

Short_t *AliITSClusterFinderV2SSD::fgPairs = 0x0;
Int_t    AliITSClusterFinderV2SSD::fgPairsSize = 0;
const Float_t AliITSClusterFinderV2SSD::fgkCosmic2008StripShifts[16][9] = 
  {{-0.35,-0.35,-0.35,-0.35,-0.35,-0.35,-0.35,-0.35,-0.35},  // DDL 512
   {-0.35,-0.35,-0.35,-0.35,-0.35,-0.35,-0.35,-0.35,-0.35},  // DDL 513
   {-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15},  // DDL 514
   {-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15},  // DDL 515
   { 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},  // DDL 516
   { 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},  // DDL 517
   {-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15},  // DDL 518
   {-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15},  // DDL 519
   {-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.25,-0.15},  // DDL 520
   {-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15,-0.15},  // DDL 521
   {-0.10,-0.10,-0.10,-0.40,-0.40,-0.40,-0.10,-0.10,-0.45},  // DDL 522
   {-0.10,-0.10,-0.10,-0.35,-0.35,-0.35,-0.10,-0.35,-0.50},  // DDL 523
   { 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},  // DDL 524
   { 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00},  // DDL 525
   { 0.35, 0.35, 0.35, 0.35, 0.35, 0.35, 0.35, 0.35, 0.35},  // DDL 526
   { 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45, 0.45}}; // DDL 527

ClassImp(AliITSClusterFinderV2SSD)


AliITSClusterFinderV2SSD::AliITSClusterFinderV2SSD(AliITSDetTypeRec* dettyp):AliITSClusterFinder(dettyp),
									     fLastSSD1(AliITSgeomTGeo::GetModuleIndex(6,1,1)-1)
{
//Default constructor

}
 
//______________________________________________________________________
AliITSClusterFinderV2SSD::AliITSClusterFinderV2SSD(const AliITSClusterFinderV2SSD &cf) : AliITSClusterFinder(cf),						fLastSSD1(cf.fLastSSD1)
{
  // Copy constructor
}

//______________________________________________________________________
AliITSClusterFinderV2SSD& AliITSClusterFinderV2SSD::operator=(const AliITSClusterFinderV2SSD&  cf ){
  // Assignment operator

  this->~AliITSClusterFinderV2SSD();
  new(this) AliITSClusterFinderV2SSD(cf);
  return *this;
}


void AliITSClusterFinderV2SSD::FindRawClusters(Int_t mod){

  //Find clusters V2
  SetModule(mod);
  FindClustersSSD(fDigits);

}

void AliITSClusterFinderV2SSD::FindClustersSSD(TClonesArray *alldigits) {
  //------------------------------------------------------------
  // Actual SSD cluster finder
  //------------------------------------------------------------

  static AliITSRecoParam *repa = NULL;
  
  
  if(!repa){
    repa = (AliITSRecoParam*) AliITSReconstructor::GetRecoParam();
    if(!repa){
      repa = AliITSRecoParam::GetHighFluxParam();
      AliWarning("Using default AliITSRecoParam class");
    }
  }

  AliITSCalibrationSSD* cal = (AliITSCalibrationSSD*)GetResp(fModule);
  Float_t gain=0;

  Int_t smaxall=alldigits->GetEntriesFast();
  if (smaxall==0) return;
  //  TObjArray *digits = new TObjArray;
  TObjArray digits;
  for (Int_t i=0;i<smaxall; i++){
    AliITSdigitSSD *d=(AliITSdigitSSD*)alldigits->UncheckedAt(i);

    if(d->IsSideP()) gain = cal->GetGainP(d->GetStripNumber());  
    else gain = cal->GetGainN(d->GetStripNumber());

    Float_t q=gain*d->GetSignal(); // calibration brings mip peaks around 120 (in ADC units)
    q=cal->ADCToKeV(q); // converts the charge in KeV from ADC units
    d->SetSignal(Int_t(q));

    if (d->GetSignal()<3) continue;
    digits.AddLast(d);
  }
  Int_t smax = digits.GetEntriesFast();
  if (smax==0) return;
  
  const Int_t kMax=1000;
  Int_t np=0, nn=0; 
  Ali1Dcluster pos[kMax], neg[kMax];
  Float_t y=0., q=0., qmax=0.; 
  Int_t lab[4]={-2,-2,-2,-2};
  
  AliITSdigitSSD *d=(AliITSdigitSSD*)digits.UncheckedAt(0);
  q += d->GetSignal();
  y += d->GetCoord2()*d->GetSignal();
  qmax=d->GetSignal();
  lab[0]=d->GetTrack(0); lab[1]=d->GetTrack(1); lab[2]=d->GetTrack(2);
  Int_t curr=d->GetCoord2();
  Int_t flag=d->GetCoord1();
  Int_t *n=&nn;
  Ali1Dcluster *c=neg;
  Int_t nd=1;
  Int_t milab[10];
  for (Int_t ilab=0;ilab<10;ilab++){
    milab[ilab]=-2;
  }
  milab[0]=d->GetTrack(0); milab[1]=d->GetTrack(1); milab[2]=d->GetTrack(2);

  for (Int_t s=1; s<smax; s++) {
      d=(AliITSdigitSSD*)digits.UncheckedAt(s);      
      Int_t strip=d->GetCoord2();
      if ((strip-curr) > 1 || flag!=d->GetCoord1()) {
         c[*n].SetY(y/q);
         c[*n].SetQ(q);
         c[*n].SetNd(nd);
	 CheckLabels2(milab);
         c[*n].SetLabels(milab);

	 if(repa->GetUseUnfoldingInClusterFinderSSD()==kTRUE) {
	   
	   //Split suspiciously big cluster
	   if (nd>4&&nd<25) {
	     c[*n].SetY(y/q-0.25*nd);
	     c[*n].SetQ(0.5*q);
	     (*n)++;
	     if (*n==kMax) {
	       Error("FindClustersSSD","Too many 1D clusters !");
	       return;
	     }
	     c[*n].SetY(y/q+0.25*nd);
	     c[*n].SetQ(0.5*q);
	     c[*n].SetNd(nd);
	     c[*n].SetLabels(milab);
	   }	 
	   
	 } // unfolding is on

         (*n)++;
         if (*n==kMax) {
          Error("FindClustersSSD","Too many 1D clusters !");
          return;
         }
         y=q=qmax=0.;
         nd=0;
         lab[0]=lab[1]=lab[2]=-2;
	 //
	 for (Int_t ilab=0;ilab<10;ilab++){
	   milab[ilab]=-2;
	 }
	 //
         if (flag!=d->GetCoord1()) { n=&np; c=pos; }
      }
      flag=d->GetCoord1();
      q += d->GetSignal();
      y += d->GetCoord2()*d->GetSignal();
      nd++;
      if (d->GetSignal()>qmax) {
         qmax=d->GetSignal();
         lab[0]=d->GetTrack(0); lab[1]=d->GetTrack(1); lab[2]=d->GetTrack(2);
      }
      for (Int_t ilab=0;ilab<10;ilab++) {
	if (d->GetTrack(ilab)>=0) AddLabel(milab, (d->GetTrack(ilab))); 
      }
      curr=strip;
  }
  c[*n].SetY(y/q);
  c[*n].SetQ(q);
  c[*n].SetNd(nd);
  c[*n].SetLabels(lab);

  if(repa->GetUseUnfoldingInClusterFinderSSD()==kTRUE) {
    
    //Split suspiciously big cluster
    if (nd>4 && nd<25) {
      c[*n].SetY(y/q-0.25*nd);
      c[*n].SetQ(0.5*q);
      (*n)++;
      if (*n==kMax) {
        Error("FindClustersSSD","Too many 1D clusters !");
        return;
      }
      c[*n].SetY(y/q+0.25*nd);
      c[*n].SetQ(0.5*q);
      c[*n].SetNd(nd);
      c[*n].SetLabels(lab);
    }
  } // unfolding is on
  
  (*n)++;
  if (*n==kMax) {
     Error("FindClustersSSD","Too many 1D clusters !");
     return;
  }

  FindClustersSSD(neg, nn, pos, np);
}


void AliITSClusterFinderV2SSD::RawdataToClusters(AliRawReader* rawReader,TClonesArray** clusters){

    //------------------------------------------------------------
  // This function creates ITS clusters from raw data
  //------------------------------------------------------------
  rawReader->Reset();
  AliITSRawStreamSSD inputSSD(rawReader);
  FindClustersSSD(&inputSSD,clusters);
  
}

void AliITSClusterFinderV2SSD::FindClustersSSD(AliITSRawStreamSSD* input, 
					TClonesArray** clusters) 
{
  //------------------------------------------------------------
  // Actual SSD cluster finder for raw data
  //------------------------------------------------------------

  static AliITSRecoParam *repa = NULL;
  if(!repa){
    repa = (AliITSRecoParam*) AliITSReconstructor::GetRecoParam();
    if(!repa){
      repa = AliITSRecoParam::GetHighFluxParam();
      AliWarning("Using default AliITSRecoParam class");
    }
  }

  Int_t nClustersSSD = 0;
  const Int_t kMax = 1000;
  Ali1Dcluster clusters1D[2][kMax];
  Int_t nClusters[2] = {0, 0};
  Int_t lab[3]={-2,-2,-2};
  Float_t q = 0.;
  Float_t y = 0.;
  Int_t nDigits = 0;
  Float_t gain=0;
  Float_t noise=0.;
  //  Float_t pedestal=0.;
  Float_t oldnoise=0.;
  AliITSCalibrationSSD* cal=NULL;

  Int_t matrix[12][1536];
  Int_t iddl=-1;
  Int_t iad=-1;
  Int_t oddl = -1;
  Int_t oad = -1;
  Int_t oadc = -1;
  Int_t ostrip = -1;
  Int_t osignal = 65535;
  Int_t n=0;
  Bool_t next=0;

  // read raw data input stream
  while (kTRUE) {
    
    // reset signal matrix
    for(Int_t i=0; i<12; i++) { for(Int_t j=0; j<1536; j++) { matrix[i][j] = 65535;} }
    
    if((osignal!=65535)&&(ostrip>0)&&(ostrip<1536)) { 
      n++;
      matrix[oadc][ostrip] = osignal; // recover data from previous occurence of input->Next() 
    }
    
    // buffer data for ddl=iddl and ad=iad
    while(kTRUE) {
        
      next = input->Next();
      if((!next)&&(input->flag)) continue;
      Int_t ddl=input->GetDDL(); 
      Int_t ad=input->GetAD();
      Int_t adc = input->GetADC(); adc = (adc<6)? adc : adc - 2;
      Int_t strip = input->GetStrip();
      if(input->GetSideFlag()) strip=1535-strip;
      Int_t signal = input->GetSignal();

      if((ddl==iddl)&&(ad==iad)&&(strip>0)&&(strip<1536)) {n++; matrix[adc][strip] = signal;}
      else {if ((strip<1536) && (strip>0)) {oddl=iddl; oad=iad; oadc = adc; ostrip = strip; osignal=signal; iddl=ddl; iad=ad; break;}}
      
      if(!next)  {oddl=iddl; oad=iad; oadc = adc; ostrip = strip; osignal=signal; iddl=ddl; iad=ad; break;}
      //break;
    }
    
    // No SSD data
    if(!next && oddl<0) break;
    
    if(n==0) continue; // first occurence
    n=0; //osignal=0;

    Float_t dStrip = 0;
    if (repa->GetUseCosmicRunShiftsSSD()) {  // Special condition for 2007/2008 cosmic data
      dStrip = fgkCosmic2008StripShifts[oddl][oad-1];
    }
    if (fabs(dStrip) > 1.5)
      printf("Indexing error ? oddl = %d, dStrip %f\n",oddl,dStrip);
    // fill 1Dclusters
    for(Int_t iadc=0; iadc<12; iadc++) {  // loop over ADC index for ddl=oddl and ad=oad
      
      Int_t iimod = (oad - 1)  * 12 + iadc;
      Int_t iModule = AliITSRawStreamSSD::GetModuleNumber(oddl,iimod);
      if(iModule==-1) continue;
      cal = (AliITSCalibrationSSD*)GetResp(iModule);

      Bool_t first = 0;
      
      /*
      for(Int_t istrip=0; istrip<768; istrip++) { // P-side
	Int_t signal = matrix[iadc][istrip];
	pedestal = cal->GetPedestalP(istrip);
	matrix[iadc][istrip]=signal-(Int_t)pedestal;
      } 
      */

      /*
      Float_t cmode=0;
      for(Int_t l=0; l<6; l++) {
	cmode=0;
	for(Int_t n=20; n<108; n++) cmode+=matrix[iadc][l*128+n];
	cmode/=88.;
	for(Int_t n=0; n<128; n++) matrix[iadc][l*128+n]-=(Int_t)cmode;
	
      }
      */

      Int_t istrip=0;
      for(istrip=0; istrip<768; istrip++) { // P-side
	
	Int_t signal = TMath::Abs(matrix[iadc][istrip]);
	
	oldnoise = noise;
	noise = cal->GetNoiseP(istrip); if(noise<1.) signal = 65535;
	if(signal<3*noise) signal = 65535; // in case ZS was not done in hw do it now

	//        if(cal->IsPChannelBad(istrip)) signal=0;

	if (signal!=65535) {
	  gain = cal->GetGainP(istrip);
	  signal = (Int_t) ( signal * gain ); // signal is corrected for gain
	  signal = (Int_t) cal->ADCToKeV( signal ); // signal is  converted in KeV 
	  
	  q += signal;	  // add digit to current cluster
	  y += istrip * signal;	  
	  nDigits++;
	  first=1;
	}
	
	else if(first) {
	  
	  if ( ( (nDigits==1) && ( (q==0) || (q>5*oldnoise)) ) || (nDigits>1) ) {
	    
	    Ali1Dcluster& cluster = clusters1D[0][nClusters[0]++];

	    if(q!=0) cluster.SetY(y/q + dStrip);
	    else cluster.SetY(istrip + dStrip -1);

	    cluster.SetQ(q);
	    cluster.SetNd(nDigits);
	    cluster.SetLabels(lab);
	    
	    if(repa->GetUseUnfoldingInClusterFinderSSD()==kTRUE) {
	      
	      //Split suspiciously big cluster
	      if (nDigits > 4&&nDigits < 25) {
		if(q!=0) cluster.SetY(y/q + dStrip - 0.25*nDigits);
		else cluster.SetY(istrip-1 + dStrip - 0.25*nDigits);
		cluster.SetQ(0.5*q);
		if (nClusters[0] == kMax) {
		  Error("FindClustersSSD", "Too many 1D clusters !");
		  return;
		}
		Ali1Dcluster& cluster2 = clusters1D[0][nClusters[0]++];
		if(q!=0) cluster2.SetY(y/q + dStrip + 0.25*nDigits);
		else cluster2.SetY(istrip-1 + dStrip + 0.25*nDigits);
		cluster2.SetQ(0.5*q);
		cluster2.SetNd(nDigits);
		cluster2.SetLabels(lab);
	      }
	    } // unfolding is on	    
	  }
	  
	  y = q = 0.;
	  nDigits = 0;
	  first=0;
	}
	
      } // loop over strip on P-side
      
      // if last strip does have signal
      if(first) {
	
	  if ( ( (nDigits==1) && ( (q==0) || (q>5*oldnoise)) ) || (nDigits>1) ) {
	  
	  Ali1Dcluster& cluster = clusters1D[0][nClusters[0]++];

	  if(q!=0) cluster.SetY(y/q + dStrip);
	  else cluster.SetY(istrip - 1 + dStrip);

	  cluster.SetQ(q);
	  cluster.SetNd(nDigits);
	  cluster.SetLabels(lab);
	  
	  if(repa->GetUseUnfoldingInClusterFinderSSD()==kTRUE) {
	    
	    //Split suspiciously big cluster
	    if (nDigits > 4&&nDigits < 25) {
	      if(q!=0) cluster.SetY(y/q + dStrip - 0.25*nDigits);
	      else cluster.SetY(istrip-1 + dStrip - 0.25*nDigits);
	      cluster.SetQ(0.5*q);
	      if (nClusters[0] == kMax) {
		Error("FindClustersSSD", "Too many 1D clusters !");
		return;
	      }
	      Ali1Dcluster& cluster2 = clusters1D[0][nClusters[0]++];
	      if(q!=0) cluster2.SetY(y/q + dStrip + 0.25*nDigits);
	      else cluster2.SetY(istrip-1 + dStrip + 0.25*nDigits);
	      cluster2.SetQ(0.5*q);
	      cluster2.SetNd(nDigits);
	      cluster2.SetLabels(lab);
	    }
	  } // unfolding is on    
	  
	}
	y = q = 0.;
	nDigits = 0;
	first=0;
      }
      
      /*
      for(Int_t istrip=768; istrip<1536; istrip++) { // P-side
	Int_t signal = matrix[iadc][istrip];
	pedestal = cal->GetPedestalN(1535-istrip);
	matrix[iadc][istrip]=signal-(Int_t)pedestal;
      }	
      */

      /*
      for(Int_t l=6; l<12; l++) {
	Float_t cmode=0;
	for(Int_t n=20; n<108; n++) cmode+=matrix[iadc][l*128+n];
	cmode/=88.;
	for(Int_t n=0; n<128; n++) matrix[iadc][l*128+n]-=(Int_t)cmode;
      }
      */

      oldnoise = 0.;
      noise = 0.;
      Int_t strip=0;
      for(Int_t iistrip=768; iistrip<1536; iistrip++) { // N-side
	
	Int_t signal = TMath::Abs(matrix[iadc][iistrip]);
	strip = 1535-iistrip;

	oldnoise = noise;
	noise = cal->GetNoiseN(strip); if(noise<1.) signal=65535;

	//        if(cal->IsNChannelBad(strip)) signal=0;

	if(signal<3*noise) signal = 65535; // in case ZS was not done in hw do it now

	if (signal!=65535) {
	  gain = cal->GetGainN(strip);
	  signal = (Int_t) ( signal * gain); // signal is corrected for gain
	  signal = (Int_t) cal->ADCToKeV( signal ); // signal is  converted in KeV 
	  
	  // add digit to current cluster
	  q += signal;
	  y += strip * signal;
	  nDigits++;
	  first=1;
	}

	else if(first) {
	  
	  if ( ( (nDigits==1) && ( (q==0) || (q>5*oldnoise)) ) || (nDigits>1) ) {
	    
	    Ali1Dcluster& cluster = clusters1D[1][nClusters[1]++];

	    if(q!=0) cluster.SetY(y/q - dStrip);
	    else cluster.SetY(strip+1 - dStrip);

	    cluster.SetQ(q);
	    cluster.SetNd(nDigits);
	    cluster.SetLabels(lab);
	    
	    if(repa->GetUseUnfoldingInClusterFinderSSD()==kTRUE) {

	      //Split suspiciously big cluster
	      if (nDigits > 4&&nDigits < 25) {
		cluster.SetY(y/q - dStrip - 0.25*nDigits);
		cluster.SetQ(0.5*q);
		if (nClusters[1] == kMax) {
		  Error("FindClustersSSD", "Too many 1D clusters !");
		  return;
		}
		Ali1Dcluster& cluster2 = clusters1D[1][nClusters[1]++];
		cluster2.SetY(y/q - dStrip + 0.25*nDigits);
		cluster2.SetQ(0.5*q);
		cluster2.SetNd(nDigits);
		cluster2.SetLabels(lab);
	      }	      
	    } // unfolding is on
	  } 

	  y = q = 0.;
	  nDigits = 0;
	  first=0;	  
	}
	
      } // loop over strips on N-side

      if(first) {
	
	  if ( ( (nDigits==1) && ( (q==0) || (q>5*oldnoise)) ) || (nDigits>1) ) {
	  
	  Ali1Dcluster& cluster = clusters1D[1][nClusters[1]++];
	  
	  if(q!=0) cluster.SetY(y/q - dStrip);
	  else cluster.SetY(strip - dStrip + 1);

	  cluster.SetQ(q);
	  cluster.SetNd(nDigits);
	  cluster.SetLabels(lab);
	  
	  if(repa->GetUseUnfoldingInClusterFinderSSD()==kTRUE) {
	    
	    //Split suspiciously big cluster
	    if (nDigits > 4&&nDigits < 25) {
	      if(q!=0) cluster.SetY(y/q - dStrip - 0.25*nDigits);
	      else cluster.SetY(strip+1 - dStrip - 0.25*nDigits);
	      cluster.SetQ(0.5*q);
	      if (nClusters[1] == kMax) {
		Error("FindClustersSSD", "Too many 1D clusters !");
		return;
	      }
	      Ali1Dcluster& cluster2 = clusters1D[1][nClusters[1]++];
	      if(q!=0) cluster2.SetY(y/q - dStrip + 0.25*nDigits);
	      else cluster2.SetY(strip+1 - dStrip + 0.25*nDigits);
	      cluster2.SetQ(0.5*q);
	      cluster2.SetNd(nDigits);
	      cluster2.SetLabels(lab);
	    }
	  } // unfolding is on	    
	}

	y = q = 0.;
	nDigits = 0;
	first=0;	  
      }
      
      // create recpoints
      if((nClusters[0])&&(nClusters[1])) {
	
	clusters[iModule] = new TClonesArray("AliITSRecPoint");
	fModule = iModule;
	FindClustersSSD(&clusters1D[0][0], nClusters[0], 
			&clusters1D[1][0], nClusters[1], clusters[iModule]);
	Int_t nClustersn = clusters[iModule]->GetEntriesFast();
	nClustersSSD += nClustersn;
      }

      nClusters[0] = nClusters[1] = 0;
      y = q = 0.;
      nDigits = 0;

    } // loop over adc

    if(!next) break;
  }
  
  Info("FindClustersSSD", "found clusters in ITS SSD: %d", nClustersSSD);
}

void AliITSClusterFinderV2SSD::
FindClustersSSD(Ali1Dcluster* neg, Int_t nn, 
		Ali1Dcluster* pos, Int_t np,
		TClonesArray *clusters) {
  //------------------------------------------------------------
  // Actual SSD cluster finder
  //------------------------------------------------------------

  const TGeoHMatrix *mT2L=AliITSgeomTGeo::GetTracking2LocalMatrix(fModule);

  TClonesArray &cl=*clusters;
  
  AliITSsegmentationSSD *seg = dynamic_cast<AliITSsegmentationSSD*>(fDetTypeRec->GetSegmentationModel(2));
  if (fModule>fLastSSD1) 
    seg->SetLayer(6);
  else 
    seg->SetLayer(5);

  Float_t hwSSD = seg->Dx()*1e-4/2;
  Float_t hlSSD = seg->Dz()*1e-4/2;

  Int_t idet=fNdet[fModule];
  Int_t ncl=0;

  //
  Int_t *cnegative = new Int_t[np];  
  Int_t *cused1 = new Int_t[np];
  Int_t *negativepair = new Int_t[10*np];
  Int_t *cpositive = new Int_t[nn];  
  Int_t *cused2 = new Int_t[nn];  
  Int_t *positivepair = new Int_t[10*nn];  
  for (Int_t i=0;i<np;i++) {cnegative[i]=0; cused1[i]=0;}
  for (Int_t i=0;i<nn;i++) {cpositive[i]=0; cused2[i]=0;}
  for (Int_t i=0;i<10*np;i++) {negativepair[i]=0;}
  for (Int_t i=0;i<10*nn;i++) {positivepair[i]=0;}

  if ((np*nn) > fgPairsSize) {

    if (fgPairs) delete [] fgPairs;
    fgPairsSize = 4*np*nn;
    fgPairs = new Short_t[fgPairsSize];
  }
  memset(fgPairs,0,sizeof(Short_t)*np*nn);

  //
  // find available pairs
  //
  for (Int_t i=0; i<np; i++) {
    Float_t yp=pos[i].GetY(); 
    if ( (pos[i].GetQ()>0) && (pos[i].GetQ()<3) ) continue;
    for (Int_t j=0; j<nn; j++) {
      if ( (neg[j].GetQ()>0) && (neg[j].GetQ()<3) ) continue;
      Float_t yn=neg[j].GetY();

      Float_t xt, zt;
      seg->GetPadCxz(yn, yp, xt, zt);
      
      if (TMath::Abs(xt)<hwSSD+0.01)
      if (TMath::Abs(zt)<hlSSD+0.01*(neg[j].GetNd()+pos[i].GetNd())) {
	negativepair[i*10+cnegative[i]] =j;  //index
	positivepair[j*10+cpositive[j]] =i;
	cnegative[i]++;  //counters
	cpositive[j]++;	
	fgPairs[i*nn+j]=100;
      }
    }
  }

  //
  // try to recover points out of but close to the module boundaries 
  //
  for (Int_t i=0; i<np; i++) {
    Float_t yp=pos[i].GetY(); 
    if ( (pos[i].GetQ()>0) && (pos[i].GetQ()<3) ) continue;
    for (Int_t j=0; j<nn; j++) {
      if ( (neg[j].GetQ()>0) && (neg[j].GetQ()<3) ) continue;
      // if both 1Dclusters have an other cross continue
      if (cpositive[j]&&cnegative[i]) continue;
      Float_t yn=neg[j].GetY();
      
      Float_t xt, zt;
      seg->GetPadCxz(yn, yp, xt, zt);
      
      if (TMath::Abs(xt)<hwSSD+0.1)
      if (TMath::Abs(zt)<hlSSD+0.15) {
	// tag 1Dcluster (eventually will produce low quality recpoint)
	if (cnegative[i]==0) pos[i].SetNd(100);  // not available pair
	if (cpositive[j]==0) neg[j].SetNd(100);  // not available pair
	negativepair[i*10+cnegative[i]] =j;  //index
	positivepair[j*10+cpositive[j]] =i;
	cnegative[i]++;  //counters
	cpositive[j]++;	
	fgPairs[i*nn+j]=100;
      }
    }
  }

  //
  Float_t lp[5];
  Int_t milab[10];
  Double_t ratio;
  

  static AliITSRecoParam *repa = NULL;
  if(!repa){
    repa = (AliITSRecoParam*) AliITSReconstructor::GetRecoParam();
    if(!repa){
      repa = AliITSRecoParam::GetHighFluxParam();
      AliWarning("Using default AliITSRecoParam class");
    }
  }

  if(repa->GetUseChargeMatchingInClusterFinderSSD()==kTRUE) {


    //
    // sign gold tracks
    //
    for (Int_t ip=0;ip<np;ip++){
      Float_t xbest=1000,zbest=1000,qbest=0;
      //
      // select gold clusters
      if ( (cnegative[ip]==1) && cpositive[negativepair[10*ip]]==1){ 
	Float_t yp=pos[ip].GetY(); 
	Int_t j = negativepair[10*ip];      

	if( (pos[ip].GetQ()==0) && (neg[j].GetQ() ==0) ) { 
	  // both bad, hence continue;	  
	  // mark both as used (to avoid recover at the end)
	  cused1[ip]++; 
	  cused2[j]++;
	  continue;
	}

	ratio = (pos[ip].GetQ()-neg[j].GetQ())/(pos[ip].GetQ()+neg[j].GetQ());

	// charge matching (note that if posQ or negQ is 0 -> ratio=1 and the following condition is met
	if (TMath::Abs(ratio)>0.2) continue; // note: 0.2=3xsigma_ratio calculated in cosmics tests

	//
	Float_t yn=neg[j].GetY();
	
	Float_t xt, zt;
	seg->GetPadCxz(yn, yp, xt, zt);
	
	xbest=xt; zbest=zt; 

	
	qbest=0.5*(pos[ip].GetQ()+neg[j].GetQ());
	if( (pos[ip].GetQ()==0)||(neg[ip].GetQ()==0)) qbest*=2; // in case of bad strips on one side keep all charge from the other one
	
	{
	  Double_t loc[3]={xbest,0.,zbest},trk[3]={0.,0.,0.};
	  mT2L->MasterToLocal(loc,trk);
	  lp[0]=trk[1];
	  lp[1]=trk[2];
	}
	lp[2]=0.0025*0.0025;  //SigmaY2
	lp[3]=0.110*0.110;  //SigmaZ2
	
	lp[4]=qbest;        //Q
	for (Int_t ilab=0;ilab<10;ilab++) milab[ilab]=-2;
	for (Int_t ilab=0;ilab<3;ilab++){
	  milab[ilab] = pos[ip].GetLabel(ilab);
	  milab[ilab+3] = neg[j].GetLabel(ilab);
	}
	//
	CheckLabels2(milab);
	milab[3]=(((ip<<10) + j)<<10) + idet; // pos|neg|det
	Int_t info[3] = {pos[ip].GetNd(),neg[j].GetNd(),fNlayer[fModule]};
	AliITSRecPoint * cl2;
	
	if(clusters){  // Note clusters != 0 when method is called for rawdata
	  
	  
	  cl2 = new (cl[ncl]) AliITSRecPoint(milab,lp,info);
	  
	  cl2->SetChargeRatio(ratio);    	
	  cl2->SetType(1);
	  fgPairs[ip*nn+j]=1;

	  if ((pos[ip].GetNd()+neg[j].GetNd())>6){ //multi cluster
	    cl2->SetType(2);
	    fgPairs[ip*nn+j]=2;
	  }

	  if(pos[ip].GetQ()==0) cl2->SetType(3);
	  if(neg[ip].GetQ()==0) cl2->SetType(4);

	  cused1[ip]++;
	  cused2[j]++;
	  
	}
	else{ // Note clusters == 0 when method is called for digits
	  
	  cl2 = new AliITSRecPoint(milab,lp,info);	
	  
	  cl2->SetChargeRatio(ratio);    	
	  cl2->SetType(1);
	  fgPairs[ip*nn+j]=1;

	  if ((pos[ip].GetNd()+neg[j].GetNd())>6){ //multi cluster
	    cl2->SetType(2);
	    fgPairs[ip*nn+j]=2;
	  }

	  if(pos[ip].GetQ()==0) cl2->SetType(3);
	  if(neg[ip].GetQ()==0) cl2->SetType(4);

	  cused1[ip]++;
	  cused2[j]++;

	  fDetTypeRec->AddRecPoint(*cl2);
	}
	ncl++;
      }
    }
    
    for (Int_t ip=0;ip<np;ip++){
      Float_t xbest=1000,zbest=1000,qbest=0;
      //
      //
      // select "silber" cluster
      if ( cnegative[ip]==1 && cpositive[negativepair[10*ip]]==2){
	Int_t in  = negativepair[10*ip];
	Int_t ip2 = positivepair[10*in];
	if (ip2==ip) ip2 =  positivepair[10*in+1];
	Float_t pcharge = pos[ip].GetQ()+pos[ip2].GetQ();
	
	if ( (TMath::Abs(pcharge-neg[in].GetQ())<30) && (pcharge!=0) ) { // 
	  
	  //
	  // add first pair
	  if ( (fgPairs[ip*nn+in]==100)&&(pos[ip].GetQ() ) ) {  //
	    
	    Float_t yp=pos[ip].GetY(); 
	    Float_t yn=neg[in].GetY();
	    
	    Float_t xt, zt;
	    seg->GetPadCxz(yn, yp, xt, zt);
	    
	    xbest=xt; zbest=zt; 

	    qbest =pos[ip].GetQ();
	    Double_t loc[3]={xbest,0.,zbest},trk[3]={0.,0.,0.};
	    mT2L->MasterToLocal(loc,trk);
	    lp[0]=trk[1];
	    lp[1]=trk[2];
	    
	    lp[2]=0.0025*0.0025;  //SigmaY2
	    lp[3]=0.110*0.110;  //SigmaZ2
	    
	    lp[4]=qbest;        //Q
	    for (Int_t ilab=0;ilab<10;ilab++) milab[ilab]=-2;
	    for (Int_t ilab=0;ilab<3;ilab++){
	      milab[ilab] = pos[ip].GetLabel(ilab);
	      milab[ilab+3] = neg[in].GetLabel(ilab);
	    }
	    //
	    CheckLabels2(milab);
	    ratio = (pos[ip].GetQ()-neg[in].GetQ())/(pos[ip].GetQ()+neg[in].GetQ());
	    milab[3]=(((ip<<10) + in)<<10) + idet; // pos|neg|det
	    Int_t info[3] = {pos[ip].GetNd(),neg[in].GetNd(),fNlayer[fModule]};
	    
	    AliITSRecPoint * cl2;
	    if(clusters){
	      
	      cl2 = new (cl[ncl]) AliITSRecPoint(milab,lp,info);
	      cl2->SetChargeRatio(ratio);    	
	      cl2->SetType(5);
	      fgPairs[ip*nn+in] = 5;
	      if ((pos[ip].GetNd()+neg[in].GetNd())>6){ //multi cluster
		cl2->SetType(6);
		fgPairs[ip*nn+in] = 6;
	      }	    
	    }
	    else{
	      cl2 = new AliITSRecPoint(milab,lp,info);
	      cl2->SetChargeRatio(ratio);    	
	      cl2->SetType(5);
	      fgPairs[ip*nn+in] = 5;
	      if ((pos[ip].GetNd()+neg[in].GetNd())>6){ //multi cluster
		cl2->SetType(6);
		fgPairs[ip*nn+in] = 6;
	      }
	      
	      fDetTypeRec->AddRecPoint(*cl2);
	    }
	    ncl++;
	  }
	  
	  
	  //
	  // add second pair
	  
	  //	if (!(cused1[ip2] || cused2[in])){  //
	  if ( (fgPairs[ip2*nn+in]==100) && (pos[ip2].GetQ()) ) {
	    
	    Float_t yp=pos[ip2].GetY();
	    Float_t yn=neg[in].GetY();
	    
	    Float_t xt, zt;
	    seg->GetPadCxz(yn, yp, xt, zt);
	    
	    xbest=xt; zbest=zt; 

	    qbest =pos[ip2].GetQ();
	    
	    Double_t loc[3]={xbest,0.,zbest},trk[3]={0.,0.,0.};
	    mT2L->MasterToLocal(loc,trk);
	    lp[0]=trk[1];
	    lp[1]=trk[2];
	    
	    lp[2]=0.0025*0.0025;  //SigmaY2
	    lp[3]=0.110*0.110;  //SigmaZ2
	    
	    lp[4]=qbest;        //Q
	    for (Int_t ilab=0;ilab<10;ilab++) milab[ilab]=-2;
	    for (Int_t ilab=0;ilab<3;ilab++){
	      milab[ilab] = pos[ip2].GetLabel(ilab);
	      milab[ilab+3] = neg[in].GetLabel(ilab);
	    }
	    //
	    CheckLabels2(milab);
	    ratio = (pos[ip2].GetQ()-neg[in].GetQ())/(pos[ip2].GetQ()+neg[in].GetQ());
	    milab[3]=(((ip2<<10) + in)<<10) + idet; // pos|neg|det
	    Int_t info[3] = {pos[ip2].GetNd(),neg[in].GetNd(),fNlayer[fModule]};
	    
	    AliITSRecPoint * cl2;
	    if(clusters){
	      cl2 = new (cl[ncl]) AliITSRecPoint(milab,lp,info);
	      
	      cl2->SetChargeRatio(ratio);    	
	      cl2->SetType(5);
	      fgPairs[ip2*nn+in] =5;
	      if ((pos[ip2].GetNd()+neg[in].GetNd())>6){ //multi cluster
		cl2->SetType(6);
		fgPairs[ip2*nn+in] =6;
	      }
	    }
	    else{
	      cl2 = new AliITSRecPoint(milab,lp,info);
	      cl2->SetChargeRatio(ratio);    	
	      cl2->SetType(5);
	      fgPairs[ip2*nn+in] =5;
	      if ((pos[ip2].GetNd()+neg[in].GetNd())>6){ //multi cluster
		cl2->SetType(6);
		fgPairs[ip2*nn+in] =6;
	      }
	      
	      fDetTypeRec->AddRecPoint(*cl2);
	    }
	    ncl++;
	  }
	  
	  cused1[ip]++;
	  cused1[ip2]++;
	  cused2[in]++;

	} // charge matching condition

      } // 2 Pside cross 1 Nside
    } // loop over Pside clusters
    
    
      
      //  
    for (Int_t jn=0;jn<nn;jn++){
      if (cused2[jn]) continue;
      Float_t xbest=1000,zbest=1000,qbest=0;
      // select "silber" cluster
      if ( cpositive[jn]==1 && cnegative[positivepair[10*jn]]==2){
	Int_t ip  = positivepair[10*jn];
	Int_t jn2 = negativepair[10*ip];
	if (jn2==jn) jn2 =  negativepair[10*ip+1];
	Float_t pcharge = neg[jn].GetQ()+neg[jn2].GetQ();
	//
	
	if ( (TMath::Abs(pcharge-pos[ip].GetQ())<30) &&  // charge matching 
	     (pcharge!=0) ) { // reject combinations of bad strips
	  
	  //
	  // add first pair
	  //	if (!(cused1[ip]||cused2[jn])){
	  if ( (fgPairs[ip*nn+jn]==100) && (neg[jn].GetQ()) ) {  //
	    
	    Float_t yn=neg[jn].GetY(); 
	    Float_t yp=pos[ip].GetY();

	    Float_t xt, zt;
	    seg->GetPadCxz(yn, yp, xt, zt);
	    
	    xbest=xt; zbest=zt; 

	    qbest =neg[jn].GetQ();

	    {
	      Double_t loc[3]={xbest,0.,zbest},trk[3]={0.,0.,0.};
	      mT2L->MasterToLocal(loc,trk);
	      lp[0]=trk[1];
	      lp[1]=trk[2];
          }
	  lp[2]=0.0025*0.0025;  //SigmaY2
	  lp[3]=0.110*0.110;  //SigmaZ2
	  
	  lp[4]=qbest;        //Q
	  for (Int_t ilab=0;ilab<10;ilab++) milab[ilab]=-2;
	  for (Int_t ilab=0;ilab<3;ilab++){
	    milab[ilab] = pos[ip].GetLabel(ilab);
	    milab[ilab+3] = neg[jn].GetLabel(ilab);
	  }
	  //
	  CheckLabels2(milab);
	  ratio = (pos[ip].GetQ()-neg[jn].GetQ())/(pos[ip].GetQ()+neg[jn].GetQ());
	  milab[3]=(((ip<<10) + jn)<<10) + idet; // pos|neg|det
	  Int_t info[3] = {pos[ip].GetNd(),neg[jn].GetNd(),fNlayer[fModule]};

	  AliITSRecPoint * cl2;
	  if(clusters){
	    cl2 = new (cl[ncl]) AliITSRecPoint(milab,lp,info);

	    cl2->SetChargeRatio(ratio);    	
	    cl2->SetType(7);
	    fgPairs[ip*nn+jn] =7;
	    if ((pos[ip].GetNd()+neg[jn].GetNd())>6){ //multi cluster
	      cl2->SetType(8);
	      fgPairs[ip*nn+jn]=8;
	    }

	  }
	  else{
	    cl2 = new AliITSRecPoint(milab,lp,info);
	    cl2->SetChargeRatio(ratio);    	
	    cl2->SetType(7);
	    fgPairs[ip*nn+jn] =7;
	    if ((pos[ip].GetNd()+neg[jn].GetNd())>6){ //multi cluster
	      cl2->SetType(8);
	      fgPairs[ip*nn+jn]=8;
	    }

	    fDetTypeRec->AddRecPoint(*cl2);
	  }
	  ncl++;
	}
	//
	// add second pair
	//	if (!(cused1[ip]||cused2[jn2])){
	if ( (fgPairs[ip*nn+jn2]==100)&&(neg[jn2].GetQ() ) ) {  //

	  Float_t yn=neg[jn2].GetY(); 
	  Double_t yp=pos[ip].GetY(); 

	  Float_t xt, zt;
	  seg->GetPadCxz(yn, yp, xt, zt);
	  
	  xbest=xt; zbest=zt; 

	  qbest =neg[jn2].GetQ();

          {
          Double_t loc[3]={xbest,0.,zbest},trk[3]={0.,0.,0.};
          mT2L->MasterToLocal(loc,trk);
          lp[0]=trk[1];
          lp[1]=trk[2];
          }
	  lp[2]=0.0025*0.0025;  //SigmaY2
	  lp[3]=0.110*0.110;  //SigmaZ2
	  
	  lp[4]=qbest;        //Q
	  for (Int_t ilab=0;ilab<10;ilab++) milab[ilab]=-2;
	  for (Int_t ilab=0;ilab<3;ilab++){
	    milab[ilab] = pos[ip].GetLabel(ilab);
	    milab[ilab+3] = neg[jn2].GetLabel(ilab);
	  }
	  //
	  CheckLabels2(milab);
	  ratio = (pos[ip].GetQ()-neg[jn2].GetQ())/(pos[ip].GetQ()+neg[jn2].GetQ());
	  milab[3]=(((ip<<10) + jn2)<<10) + idet; // pos|neg|det
	  Int_t info[3] = {pos[ip].GetNd(),neg[jn2].GetNd(),fNlayer[fModule]};
	  AliITSRecPoint * cl2;
	  if(clusters){
	    cl2 = new (cl[ncl]) AliITSRecPoint(milab,lp,info);


	    cl2->SetChargeRatio(ratio);    	
	    fgPairs[ip*nn+jn2]=7;
	    cl2->SetType(7);
	    if ((pos[ip].GetNd()+neg[jn2].GetNd())>6){ //multi cluster
	      cl2->SetType(8);
	      fgPairs[ip*nn+jn2]=8;
	    }
	    
	  }
	  else{
	    cl2 = new AliITSRecPoint(milab,lp,info);
	    cl2->SetChargeRatio(ratio);    	
	    fgPairs[ip*nn+jn2]=7;
	    cl2->SetType(7);
	    if ((pos[ip].GetNd()+neg[jn2].GetNd())>6){ //multi cluster
	      cl2->SetType(8);
	      fgPairs[ip*nn+jn2]=8;
	    }

	    fDetTypeRec->AddRecPoint(*cl2);
	  }

	  ncl++;
	}
	cused1[ip]++;
	cused2[jn]++;
	cused2[jn2]++;

	} // charge matching condition

      } // 2 Nside cross 1 Pside
    } // loop over Pside clusters

  
    
    for (Int_t ip=0;ip<np;ip++){
      Float_t xbest=1000,zbest=1000,qbest=0;
      //
      // 2x2 clusters
      //
      if ( (cnegative[ip]<5) && cpositive[negativepair[10*ip]]<5){ 
	Float_t minchargediff =4.;
	Int_t j=-1;
	for (Int_t di=0;di<cnegative[ip];di++){
	  Int_t   jc = negativepair[ip*10+di];
	  Float_t chargedif = pos[ip].GetQ()-neg[jc].GetQ();
	  if (TMath::Abs(chargedif)<minchargediff){
	    j =jc;
	    minchargediff = TMath::Abs(chargedif);
	  }
	}
	if (j<0) continue;  // not proper cluster      
	
	Int_t count =0;
	for (Int_t di=0;di<cnegative[ip];di++){
	  Int_t   jc = negativepair[ip*10+di];
	  Float_t chargedif = pos[ip].GetQ()-neg[jc].GetQ();
	  if (TMath::Abs(chargedif)<minchargediff+3.) count++;
	}
	if (count>1) continue;  // more than one "proper" cluster for positive
	//
	
	count =0;
	for (Int_t dj=0;dj<cpositive[j];dj++){
	  Int_t   ic  = positivepair[j*10+dj];
	  Float_t chargedif = pos[ic].GetQ()-neg[j].GetQ();
	  if (TMath::Abs(chargedif)<minchargediff+3.) count++;
	}
	if (count>1) continue;  // more than one "proper" cluster for negative
	
	Int_t jp = 0;
	
	count =0;
	for (Int_t dj=0;dj<cnegative[jp];dj++){
	  Int_t   ic = positivepair[jp*10+dj];
	  Float_t chargedif = pos[ic].GetQ()-neg[jp].GetQ();
	  if (TMath::Abs(chargedif)<minchargediff+4.) count++;
	}
	if (count>1) continue;   
	if (fgPairs[ip*nn+j]<100) continue;
	//
	
	//almost gold clusters
	Float_t yp=pos[ip].GetY(); 
	Float_t yn=neg[j].GetY();
      

	Float_t xt, zt;
	seg->GetPadCxz(yn, yp, xt, zt);
	
	xbest=xt; zbest=zt; 

	qbest=0.5*(pos[ip].GetQ()+neg[j].GetQ());

	{
	  Double_t loc[3]={xbest,0.,zbest},trk[3]={0.,0.,0.};
	  mT2L->MasterToLocal(loc,trk);
	  lp[0]=trk[1];
	  lp[1]=trk[2];
	}
	lp[2]=0.0025*0.0025;  //SigmaY2
	lp[3]=0.110*0.110;  //SigmaZ2	
	lp[4]=qbest;        //Q
	for (Int_t ilab=0;ilab<10;ilab++) milab[ilab]=-2;
	for (Int_t ilab=0;ilab<3;ilab++){
	  milab[ilab] = pos[ip].GetLabel(ilab);
	  milab[ilab+3] = neg[j].GetLabel(ilab);
	}
	//
	CheckLabels2(milab);
        if ((neg[j].GetQ()==0)&&(pos[ip].GetQ()==0)) continue; // reject crosses of bad strips!!
	ratio = (pos[ip].GetQ()-neg[j].GetQ())/(pos[ip].GetQ()+neg[j].GetQ());
	milab[3]=(((ip<<10) + j)<<10) + idet; // pos|neg|det
	Int_t info[3] = {pos[ip].GetNd(),neg[j].GetNd(),fNlayer[fModule]};
	AliITSRecPoint * cl2;
	if(clusters){
	  cl2 = new (cl[ncl]) AliITSRecPoint(milab,lp,info);
	  	  
	  cl2->SetChargeRatio(ratio);    	
	  cl2->SetType(10);
	  fgPairs[ip*nn+j]=10;
	  if ((pos[ip].GetNd()+neg[j].GetNd())>6){ //multi cluster
	    cl2->SetType(11);
	    fgPairs[ip*nn+j]=11;
	  }
	  cused1[ip]++;
	  cused2[j]++;      
	}
	else{
	  cl2 = new AliITSRecPoint(milab,lp,info);
	  cl2->SetChargeRatio(ratio);    	
	  cl2->SetType(10);
	  fgPairs[ip*nn+j]=10;
	  if ((pos[ip].GetNd()+neg[j].GetNd())>6){ //multi cluster
	    cl2->SetType(11);
	    fgPairs[ip*nn+j]=11;
	  }
	  cused1[ip]++;
	  cused2[j]++;      
	  
	  fDetTypeRec->AddRecPoint(*cl2);
	}      
	ncl++;
	
      } // manyXmany
    } // loop over Pside 1Dclusters
    
    
  } // use charge matching
  
  
  // recover all the other crosses
  //  
  for (Int_t i=0; i<np; i++) {
    Float_t xbest=1000,zbest=1000,qbest=0;
    Float_t yp=pos[i].GetY(); 
    if ((pos[i].GetQ()>0)&&(pos[i].GetQ()<3)) continue;
    for (Int_t j=0; j<nn; j++) {
    //    for (Int_t di = 0;di<cpositive[i];di++){
    //  Int_t j = negativepair[10*i+di];
      if ((neg[j].GetQ()>0)&&(neg[j].GetQ()<3)) continue;

      if ((neg[j].GetQ()==0)&&(pos[i].GetQ()==0)) continue; // reject crosses of bad strips!!

      if (cused2[j]||cused1[i]) continue;      
      if (fgPairs[i*nn+j]>0 &&fgPairs[i*nn+j]<100) continue;
      ratio = (pos[i].GetQ()-neg[j].GetQ())/(pos[i].GetQ()+neg[j].GetQ());      
      Float_t yn=neg[j].GetY();
      
      Float_t xt, zt;
      seg->GetPadCxz(yn, yp, xt, zt);
      
      if (TMath::Abs(xt)<hwSSD+0.01)
      if (TMath::Abs(zt)<hlSSD+0.01*(neg[j].GetNd()+pos[i].GetNd())) {
	xbest=xt; zbest=zt; 

        qbest=0.5*(pos[i].GetQ()+neg[j].GetQ());

        {
        Double_t loc[3]={xbest,0.,zbest},trk[3]={0.,0.,0.};
        mT2L->MasterToLocal(loc,trk);
        lp[0]=trk[1];
        lp[1]=trk[2];
        }
        lp[2]=0.0025*0.0025;  //SigmaY2
        lp[3]=0.110*0.110;  //SigmaZ2

        lp[4]=qbest;        //Q
	for (Int_t ilab=0;ilab<10;ilab++) milab[ilab]=-2;
	for (Int_t ilab=0;ilab<3;ilab++){
	  milab[ilab] = pos[i].GetLabel(ilab);
	  milab[ilab+3] = neg[j].GetLabel(ilab);
	}
	//
	CheckLabels2(milab);
	milab[3]=(((i<<10) + j)<<10) + idet; // pos|neg|det
	Int_t info[3] = {pos[i].GetNd(),neg[j].GetNd(),fNlayer[fModule]};
	AliITSRecPoint * cl2;
	if(clusters){
	  cl2 = new (cl[ncl]) AliITSRecPoint(milab,lp,info);

	  cl2->SetChargeRatio(ratio);
	  cl2->SetType(100+cpositive[j]+cnegative[i]);	  

	  if(pos[i].GetQ()==0) cl2->SetType(200+cpositive[j]+cnegative[i]);
	  if(neg[j].GetQ()==0) cl2->SetType(300+cpositive[j]+cnegative[i]);

	}
	else{
	  cl2 = new AliITSRecPoint(milab,lp,info);
	  cl2->SetChargeRatio(ratio);
	  cl2->SetType(100+cpositive[j]+cnegative[i]);
	  
	  if(pos[i].GetQ()==0) cl2->SetType(200+cpositive[j]+cnegative[i]);
	  if(neg[j].GetQ()==0) cl2->SetType(300+cpositive[j]+cnegative[i]);

	  fDetTypeRec->AddRecPoint(*cl2);
	}
      	ncl++;
      }
    }
  }
  delete [] cnegative;
  delete [] cused1;
  delete [] negativepair;
  delete [] cpositive;
  delete [] cused2;
  delete [] positivepair;

}
