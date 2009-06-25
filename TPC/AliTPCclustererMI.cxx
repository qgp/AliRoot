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

//-------------------------------------------------------
//          Implementation of the TPC clusterer
//
//  1. The Input data for reconstruction - Options
//      1.a Simulated data  - TTree - invoked Digits2Clusters()
//      1.b Raw data        - Digits2Clusters(AliRawReader* rawReader); 
//
//  2. The Output data
//      2.a TTree with clusters - if  SetOutput(TTree * tree) invoked
//      2.b TObjArray           - Faster option for HLT
//      2.c TClonesArray        - Faster option for HLT (smaller memory consumption), activate with fBClonesArray flag
//
//  3. Reconstruction setup
//     see AliTPCRecoParam for list of parameters 
//     The reconstruction parameterization taken from the 
//     AliTPCReconstructor::GetRecoParam()
//     Possible to setup it in reconstruction macro  AliTPCReconstructor::SetRecoParam(...)
//     
//
//
//   Origin: Marian Ivanov 
//-------------------------------------------------------

#include "Riostream.h"
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include <TRandom.h>
#include <TTree.h>
#include <TTreeStream.h>

#include "AliDigits.h"
#include "AliLoader.h"
#include "AliLog.h"
#include "AliMathBase.h"
#include "AliRawEventHeaderBase.h"
#include "AliRawReader.h"
#include "AliRunLoader.h"
#include "AliSimDigits.h"
#include "AliTPCCalPad.h"
#include "AliTPCCalROC.h"
#include "AliTPCClustersArray.h"
#include "AliTPCClustersRow.h"
#include "AliTPCParam.h"
#include "AliTPCRawStream.h"
#include "AliTPCRawStreamV3.h"
#include "AliTPCRecoParam.h"
#include "AliTPCReconstructor.h"
#include "AliTPCcalibDB.h"
#include "AliTPCclusterInfo.h"
#include "AliTPCclusterMI.h"
#include "AliTPCTransform.h"
#include "AliTPCclustererMI.h"

ClassImp(AliTPCclustererMI)



AliTPCclustererMI::AliTPCclustererMI(const AliTPCParam* par, const AliTPCRecoParam * recoParam):
  fBins(0),
  fSigBins(0),
  fNSigBins(0),
  fLoop(0),
  fMaxBin(0),
  fMaxTime(0),
  fMaxPad(0),
  fSector(-1),
  fRow(-1),
  fSign(0),
  fRx(0),
  fPadWidth(0),
  fPadLength(0),
  fZWidth(0),
  fPedSubtraction(kFALSE),
  fEventHeader(0),
  fTimeStamp(0),
  fEventType(0),
  fInput(0),
  fOutput(0),
  fOutputArray(0),
  fOutputClonesArray(0),
  fRowCl(0),
  fRowDig(0),
  fParam(0),
  fNcluster(0),
  fNclusters(0),
  fDebugStreamer(0),
  fRecoParam(0),
  fBDumpSignal(kFALSE),
  fBClonesArray(kFALSE)
{
  //
  // COSNTRUCTOR
  // param     - tpc parameters for given file
  // recoparam - reconstruction parameters 
  //
  fInput =0;
  fParam = par;
  if (recoParam) {
    fRecoParam = recoParam;
  }else{
    //set default parameters if not specified
    fRecoParam = AliTPCReconstructor::GetRecoParam();
    if (!fRecoParam)  fRecoParam = AliTPCRecoParam::GetLowFluxParam();
  }
 
  if(AliTPCReconstructor::StreamLevel()>0) {
    fDebugStreamer = new TTreeSRedirector("TPCsignal.root");
  }

  //  Int_t nPoints = fRecoParam->GetLastBin()-fRecoParam->GetFirstBin();
  fRowCl= new AliTPCClustersRow();
  fRowCl->SetClass("AliTPCclusterMI");
  fRowCl->SetArray(1);

}
//______________________________________________________________
AliTPCclustererMI::AliTPCclustererMI(const AliTPCclustererMI &param)
              :TObject(param),
  fBins(0),
  fSigBins(0),
  fNSigBins(0),
  fLoop(0),
  fMaxBin(0),
  fMaxTime(0),
  fMaxPad(0),
  fSector(-1),
  fRow(-1),
  fSign(0),
  fRx(0),
  fPadWidth(0),
  fPadLength(0),
  fZWidth(0),
  fPedSubtraction(kFALSE),
  fEventHeader(0),
  fTimeStamp(0),
  fEventType(0),
  fInput(0),
  fOutput(0),
  fOutputArray(0),
  fOutputClonesArray(0),
  fRowCl(0),
  fRowDig(0),
  fParam(0),
  fNcluster(0),
  fNclusters(0),
  fDebugStreamer(0),
  fRecoParam(0),
  fBDumpSignal(kFALSE),
  fBClonesArray(kFALSE)
{
  //
  // dummy
  //
  fMaxBin = param.fMaxBin;
}
//______________________________________________________________
AliTPCclustererMI & AliTPCclustererMI::operator =(const AliTPCclustererMI & param)
{
  //
  // assignment operator - dummy
  //
  fMaxBin=param.fMaxBin;
  return (*this);
}
//______________________________________________________________
AliTPCclustererMI::~AliTPCclustererMI(){
  //
  //
  //
  if (fDebugStreamer) delete fDebugStreamer;
  if (fOutputArray){
    //fOutputArray->Delete();
    delete fOutputArray;
  }
  if (fOutputClonesArray){
    fOutputClonesArray->Delete();
    delete fOutputClonesArray;
  }
}

void AliTPCclustererMI::SetInput(TTree * tree)
{
  //
  // set input tree with digits
  //
  fInput = tree;  
  if  (!fInput->GetBranch("Segment")){
    cerr<<"AliTPC::Digits2Clusters(): no porper input tree !\n";
    fInput=0;
    return;
  }
}

void AliTPCclustererMI::SetOutput(TTree * tree) 
{
  //
  // Set the output tree
  // If not set the ObjArray used - Option for HLT 
  //
  if (!tree) return;
  fOutput= tree;
  AliTPCClustersRow clrow;
  AliTPCClustersRow *pclrow=&clrow;  
  clrow.SetClass("AliTPCclusterMI");
  clrow.SetArray(1); // to make Clones array
  fOutput->Branch("Segment","AliTPCClustersRow",&pclrow,32000,200);    
}


void AliTPCclustererMI::FillRow(){
  //
  // fill the output container - 
  // 2 Options possible
  //          Tree       
  //          TObjArray
  //
  if (fOutput) fOutput->Fill();
  if (!fOutput && !fBClonesArray){
    //
    if (!fOutputArray) fOutputArray = new TObjArray(fParam->GetNRowsTotal());
    if (fRowCl && fRowCl->GetArray()->GetEntriesFast()>0) fOutputArray->AddAt(fRowCl->Clone(), fRowCl->GetID());
  }
}

Float_t  AliTPCclustererMI::GetSigmaY2(Int_t iz){
  // sigma y2 = in digits  - we don't know the angle
  Float_t z = iz*fParam->GetZWidth()+fParam->GetNTBinsL1()*fParam->GetZWidth();
  Float_t sd2 = (z*fParam->GetDiffL()*fParam->GetDiffL())/
    (fPadWidth*fPadWidth);
  Float_t sres = 0.25;
  Float_t res = sd2+sres;
  return res;
}


Float_t  AliTPCclustererMI::GetSigmaZ2(Int_t iz){
  //sigma z2 = in digits - angle estimated supposing vertex constraint
  Float_t z = iz*fZWidth+fParam->GetNTBinsL1()*fParam->GetZWidth();
  Float_t sd2 = (z*fParam->GetDiffL()*fParam->GetDiffL())/(fZWidth*fZWidth);
  Float_t angular = fPadLength*(fParam->GetZLength(fSector)-z)/(fRx*fZWidth);
  angular*=angular;
  angular/=12.;
  Float_t sres = fParam->GetZSigma()/fZWidth;
  sres *=sres;
  Float_t res = angular +sd2+sres;
  return res;
}

void AliTPCclustererMI::MakeCluster(Int_t k,Int_t max,Float_t *bins, UInt_t /*m*/,
AliTPCclusterMI &c) 
{
  //
  //  k    - Make cluster at position k  
  //  bins - 2 D array of signals mapped to 1 dimensional array - 
  //  max  - the number of time bins er one dimension
  //  c    - refernce to cluster to be filled
  //
  Int_t i0=k/max;  //central pad
  Int_t j0=k%max;  //central time bin

  // set pointers to data
  //Int_t dummy[5] ={0,0,0,0,0};
  Float_t * matrix[5]; //5x5 matrix with digits  - indexing i = 0 ..4  j = -2..2
  for (Int_t di=-2;di<=2;di++){
    matrix[di+2]  =  &bins[k+di*max];
  }
  //build matrix with virtual charge
  Float_t sigmay2= GetSigmaY2(j0);
  Float_t sigmaz2= GetSigmaZ2(j0);

  Float_t vmatrix[5][5];
  vmatrix[2][2] = matrix[2][0];
  c.SetType(0);
  c.SetMax((UShort_t)(vmatrix[2][2])); // write maximal amplitude
  for (Int_t di =-1;di <=1;di++)
    for (Int_t dj =-1;dj <=1;dj++){
      Float_t amp = matrix[di+2][dj];
      if ( (amp<2) && (fLoop<2)){
	// if under threshold  - calculate virtual charge
	Float_t ratio = TMath::Exp(-1.2*TMath::Abs(di)/sigmay2)*TMath::Exp(-1.2*TMath::Abs(dj)/sigmaz2);
	amp = ((matrix[2][0]-2)*(matrix[2][0]-2)/(matrix[-di+2][-dj]+2))*ratio;
	if (amp>2) amp = 2;
	vmatrix[2+di][2+dj]=amp;
	vmatrix[2+2*di][2+2*dj]=0;
	if ( (di*dj)!=0){       
	  //DIAGONAL ELEMENTS
	  vmatrix[2+2*di][2+dj] =0;
	  vmatrix[2+di][2+2*dj] =0;
	}
	continue;
      }
      if (amp<4){
	//if small  amplitude - below  2 x threshold  - don't consider other one	
	vmatrix[2+di][2+dj]=amp;
	vmatrix[2+2*di][2+2*dj]=0;  // don't take to the account next bin
	if ( (di*dj)!=0){       
	  //DIAGONAL ELEMENTS
	  vmatrix[2+2*di][2+dj] =0;
	  vmatrix[2+di][2+2*dj] =0;
	}
	continue;
      }
      //if bigger then take everything
      vmatrix[2+di][2+dj]=amp;
      vmatrix[2+2*di][2+2*dj]= matrix[2*di+2][2*dj] ;      
      if ( (di*dj)!=0){       
	  //DIAGONAL ELEMENTS
	  vmatrix[2+2*di][2+dj] = matrix[2*di+2][dj];
	  vmatrix[2+di][2+2*dj] = matrix[2+di][dj*2];
	}      
    }


  
  Float_t sumw=0;
  Float_t sumiw=0;
  Float_t sumi2w=0;
  Float_t sumjw=0;
  Float_t sumj2w=0;
  //
  for (Int_t i=-2;i<=2;i++)
    for (Int_t j=-2;j<=2;j++){
      Float_t amp = vmatrix[i+2][j+2];

      sumw    += amp;
      sumiw   += i*amp;
      sumi2w  += i*i*amp;
      sumjw   += j*amp;
      sumj2w  += j*j*amp;
    }    
  //   
  Float_t meani = sumiw/sumw;
  Float_t mi2   = sumi2w/sumw-meani*meani;
  Float_t meanj = sumjw/sumw;
  Float_t mj2   = sumj2w/sumw-meanj*meanj;
  //
  Float_t ry = mi2/sigmay2;
  Float_t rz = mj2/sigmaz2;
  
  //
  if ( ( (ry<0.6) || (rz<0.6) ) && fLoop==2) return;
  if ( ((ry <1.2) && (rz<1.2)) || (!fRecoParam->GetDoUnfold())) {
    //
    //if cluster looks like expected or Unfolding not switched on
    //standard COG is used
    //+1.2 deviation from expected sigma accepted
    //    c.fMax = FitMax(vmatrix,meani,meanj,TMath::Sqrt(sigmay2),TMath::Sqrt(sigmaz2));

    meani +=i0;
    meanj +=j0;
    //set cluster parameters
    c.SetQ(sumw);
    c.SetPad(meani-2.5);
    c.SetTimeBin(meanj-3);
    c.SetSigmaY2(mi2);
    c.SetSigmaZ2(mj2);
    c.SetType(0);
    AddCluster(c,(Float_t*)vmatrix,k);
    return;     
  }
  //
  //unfolding when neccessary  
  //
  
  Float_t * matrix2[7]; //7x7 matrix with digits  - indexing i = 0 ..6  j = -3..3
  Float_t dummy[7]={0,0,0,0,0,0};
  for (Int_t di=-3;di<=3;di++){
    matrix2[di+3] =  &bins[k+di*max];
    if ((k+di*max)<3)  matrix2[di+3] = &dummy[3];
    if ((k+di*max)>fMaxBin-3)  matrix2[di+3] = &dummy[3];
  }
  Float_t vmatrix2[5][5];
  Float_t sumu;
  Float_t overlap;
  UnfoldCluster(matrix2,vmatrix2,meani,meanj,sumu,overlap);
  //
  //  c.fMax = FitMax(vmatrix2,meani,meanj,TMath::Sqrt(sigmay2),TMath::Sqrt(sigmaz2));
  meani +=i0;
  meanj +=j0;
  //set cluster parameters
  c.SetQ(sumu);
  c.SetPad(meani-2.5);
  c.SetTimeBin(meanj-3);
  c.SetSigmaY2(mi2);
  c.SetSigmaZ2(mj2);
  c.SetType(Char_t(overlap)+1);
  AddCluster(c,(Float_t*)vmatrix,k);

  //unfolding 2
  meani-=i0;
  meanj-=j0;
}



void AliTPCclustererMI::UnfoldCluster(Float_t * matrix2[7], Float_t recmatrix[5][5], Float_t & meani, Float_t & meanj, 
				      Float_t & sumu, Float_t & overlap )
{
  //
  //unfold cluster from input matrix
  //data corresponding to cluster writen in recmatrix
  //output meani and meanj

  //take separatelly y and z

  Float_t sum3i[7] = {0,0,0,0,0,0,0};
  Float_t sum3j[7] = {0,0,0,0,0,0,0};

  for (Int_t k =0;k<7;k++)
    for (Int_t l = -1; l<=1;l++){
      sum3i[k]+=matrix2[k][l];
      sum3j[k]+=matrix2[l+3][k-3];
    }
  Float_t mratio[3][3]={{1,1,1},{1,1,1},{1,1,1}};
  //
  //unfold  y 
  Float_t sum3wi    = 0;  //charge minus overlap
  Float_t sum3wio   = 0;  //full charge
  Float_t sum3iw    = 0;  //sum for mean value
  for (Int_t dk=-1;dk<=1;dk++){
    sum3wio+=sum3i[dk+3];
    if (dk==0){
      sum3wi+=sum3i[dk+3];     
    }
    else{
      Float_t ratio =1;
      if (  ( ((sum3i[dk+3]+3)/(sum3i[3]-3))+1 < (sum3i[2*dk+3]-3)/(sum3i[dk+3]+3))||
	    (sum3i[dk+3]<=sum3i[2*dk+3] && sum3i[dk+3]>2 )){
	Float_t xm2 = sum3i[-dk+3];
	Float_t xm1 = sum3i[+3];
	Float_t x1  = sum3i[2*dk+3];
	Float_t x2  = sum3i[3*dk+3]; 	
	Float_t w11   = TMath::Max((Float_t)(4.*xm1-xm2),(Float_t)0.000001);	  
	Float_t w12   = TMath::Max((Float_t)(4 *x1 -x2),(Float_t)0.);
	ratio = w11/(w11+w12);	 
	for (Int_t dl=-1;dl<=1;dl++)
	  mratio[dk+1][dl+1] *= ratio;
      }
      Float_t amp = sum3i[dk+3]*ratio;
      sum3wi+=amp;
      sum3iw+= dk*amp;      
    }
  }
  meani = sum3iw/sum3wi;
  Float_t overlapi = (sum3wio-sum3wi)/sum3wio;



  //unfold  z 
  Float_t sum3wj    = 0;  //charge minus overlap
  Float_t sum3wjo   = 0;  //full charge
  Float_t sum3jw    = 0;  //sum for mean value
  for (Int_t dk=-1;dk<=1;dk++){
    sum3wjo+=sum3j[dk+3];
    if (dk==0){
      sum3wj+=sum3j[dk+3];     
    }
    else{
      Float_t ratio =1;
      if ( ( ((sum3j[dk+3]+3)/(sum3j[3]-3))+1 < (sum3j[2*dk+3]-3)/(sum3j[dk+3]+3)) ||
	   (sum3j[dk+3]<=sum3j[2*dk+3] && sum3j[dk+3]>2)){
	Float_t xm2 = sum3j[-dk+3];
	Float_t xm1 = sum3j[+3];
	Float_t x1  = sum3j[2*dk+3];
	Float_t x2  = sum3j[3*dk+3]; 	
	Float_t w11   = TMath::Max((Float_t)(4.*xm1-xm2),(Float_t)0.000001);	  
	Float_t w12   = TMath::Max((Float_t)(4 *x1 -x2),(Float_t)0.);
	ratio = w11/(w11+w12);	 
	for (Int_t dl=-1;dl<=1;dl++)
	  mratio[dl+1][dk+1] *= ratio;
      }
      Float_t amp = sum3j[dk+3]*ratio;
      sum3wj+=amp;
      sum3jw+= dk*amp;      
    }
  }
  meanj = sum3jw/sum3wj;
  Float_t overlapj = (sum3wjo-sum3wj)/sum3wjo;  
  overlap = Int_t(100*TMath::Max(overlapi,overlapj)+3);  
  sumu = (sum3wj+sum3wi)/2.;
  
  if (overlap ==3) {
    //if not overlap detected remove everything
    for (Int_t di =-2; di<=2;di++)
      for (Int_t dj =-2; dj<=2;dj++){
	recmatrix[di+2][dj+2] = matrix2[3+di][dj];
      }
  }
  else{
    for (Int_t di =-1; di<=1;di++)
      for (Int_t dj =-1; dj<=1;dj++){
	Float_t ratio =1;
	if (mratio[di+1][dj+1]==1){
	  recmatrix[di+2][dj+2]     = matrix2[3+di][dj];
	  if (TMath::Abs(di)+TMath::Abs(dj)>1){
	    recmatrix[2*di+2][dj+2] = matrix2[3+2*di][dj];
	    recmatrix[di+2][2*dj+2] = matrix2[3+di][2*dj];
	  }	  
	  recmatrix[2*di+2][2*dj+2] = matrix2[3+2*di][2*dj];
	}
	else
	  {
	    //if we have overlap in direction
	    recmatrix[di+2][dj+2] = mratio[di+1][dj+1]* matrix2[3+di][dj];    
	    if (TMath::Abs(di)+TMath::Abs(dj)>1){
	      ratio =  TMath::Min((Float_t)(recmatrix[di+2][dj+2]/(matrix2[3+0*di][1*dj]+1)),(Float_t)1.);
	      recmatrix[2*di+2][dj+2] = ratio*recmatrix[di+2][dj+2];
	      //
	      ratio =  TMath::Min((Float_t)(recmatrix[di+2][dj+2]/(matrix2[3+1*di][0*dj]+1)),(Float_t)1.);
	      recmatrix[di+2][2*dj+2] = ratio*recmatrix[di+2][dj+2];
	    }
	    else{
	      ratio =  recmatrix[di+2][dj+2]/matrix2[3][0];
	      recmatrix[2*di+2][2*dj+2] = ratio*recmatrix[di+2][dj+2];
	    }
	  }
      }
  }
  
}

Float_t AliTPCclustererMI::FitMax(Float_t vmatrix[5][5], Float_t y, Float_t z, Float_t sigmay, Float_t sigmaz)
{
  //
  // estimate max
  Float_t sumteor= 0;
  Float_t sumamp = 0;

  for (Int_t di = -1;di<=1;di++)
    for (Int_t dj = -1;dj<=1;dj++){
      if (vmatrix[2+di][2+dj]>2){
	Float_t teor = TMath::Gaus(di,y,sigmay*1.2)*TMath::Gaus(dj,z,sigmaz*1.2);
	sumteor += teor*vmatrix[2+di][2+dj];
	sumamp  += vmatrix[2+di][2+dj]*vmatrix[2+di][2+dj];
      }
    }    
  Float_t max = sumamp/sumteor;
  return max;
}

void AliTPCclustererMI::AddCluster(AliTPCclusterMI &c, Float_t * /*matrix*/, Int_t /*pos*/){
  //
  //
  // Transform cluster to the rotated global coordinata
  // Assign labels to the cluster
  // add the cluster to the array
  // for more details - See  AliTPCTranform::Transform(x,i,0,1) 
  Float_t meani = c.GetPad();
  Float_t meanj = c.GetTimeBin();

  Int_t ki = TMath::Nint(meani);
  if (ki<0) ki=0;
  if (ki>=fMaxPad) ki = fMaxPad-1;
  Int_t kj = TMath::Nint(meanj);
  if (kj<0) kj=0;
  if (kj>=fMaxTime-3) kj=fMaxTime-4;
  // ki and kj shifted as integers coordinata
  if (fRowDig) {
    c.SetLabel(fRowDig->GetTrackIDFast(kj,ki,0)-2,0);
    c.SetLabel(fRowDig->GetTrackIDFast(kj,ki,1)-2,1);
    c.SetLabel(fRowDig->GetTrackIDFast(kj,ki,2)-2,2);
  }
  
  c.SetRow(fRow);
  c.SetDetector(fSector);
  Float_t s2 = c.GetSigmaY2();
  Float_t w=fParam->GetPadPitchWidth(fSector);
  c.SetSigmaY2(s2*w*w);
  s2 = c.GetSigmaZ2(); 
  c.SetSigmaZ2(s2*fZWidth*fZWidth);
  //
  //
  //
  AliTPCTransform *transform = AliTPCcalibDB::Instance()->GetTransform() ;
  if (!transform) {
    AliFatal("Tranformations not in calibDB");
  }
  Double_t x[3]={c.GetRow(),c.GetPad(),c.GetTimeBin()};
  Int_t i[1]={fSector};
  transform->Transform(x,i,0,1);
  c.SetX(x[0]);
  c.SetY(x[1]);
  c.SetZ(x[2]);
  //
  //
  if (ki<=1 || ki>=fMaxPad-1 || kj==1 || kj==fMaxTime-2) {
    c.SetType(-(c.GetType()+3));  //edge clusters
  }
  if (fLoop==2) c.SetType(100);
  if (!AcceptCluster(&c)) return;

  // select output 
  TClonesArray * arr = 0;
  AliTPCclusterMI * cl = 0;

  if(fBClonesArray==kFALSE) {
     arr = fRowCl->GetArray();
     cl = new ((*arr)[fNcluster]) AliTPCclusterMI(c);
  } else {
     cl = new ((*fOutputClonesArray)[fNclusters+fNcluster]) AliTPCclusterMI(c);
  }

  // if (fRecoParam->DumpSignal() &&matrix ) {
//     Int_t nbins=0;
//     Float_t *graph =0;
//     if (fRecoParam->GetCalcPedestal() && cl->GetMax()>fRecoParam->GetDumpAmplitudeMin() &&fBDumpSignal){
//       nbins = fMaxTime;
//       graph = &(fBins[fMaxTime*(pos/fMaxTime)]);
//     }
//     AliTPCclusterInfo * info = new AliTPCclusterInfo(matrix,nbins,graph);
//     cl->SetInfo(info);
//   }
  if (!fRecoParam->DumpSignal()) {
    cl->SetInfo(0);
  }
  
  if (AliTPCReconstructor::StreamLevel()>1) {
    Float_t xyz[3];
    cl->GetGlobalXYZ(xyz);
     (*fDebugStreamer)<<"Clusters"<<
       "Cl.="<<cl<<
       "gx="<<xyz[0]<<
       "gy="<<xyz[1]<<
       "gz="<<xyz[2]<<
       "\n";
  }

  fNcluster++;
}


//_____________________________________________________________________________
void AliTPCclustererMI::Digits2Clusters()
{
  //-----------------------------------------------------------------
  // This is a simple cluster finder.
  //-----------------------------------------------------------------

  if (!fInput) { 
    Error("Digits2Clusters", "input tree not initialised");
    return;
  }
  fRecoParam = AliTPCReconstructor::GetRecoParam();
  if (!fRecoParam){
    AliFatal("Can not get the reconstruction parameters");
  }
  if(AliTPCReconstructor::StreamLevel()>5) {
    AliInfo("Parameter Dumps");
    fParam->Dump();
    fRecoParam->Dump();
  }

  AliTPCCalPad * gainTPC = AliTPCcalibDB::Instance()->GetPadGainFactor();
  AliTPCCalPad * noiseTPC = AliTPCcalibDB::Instance()->GetPadNoise();
  AliSimDigits digarr, *dummy=&digarr;
  fRowDig = dummy;
  fInput->GetBranch("Segment")->SetAddress(&dummy);
  Stat_t nentries = fInput->GetEntries();
  
  fMaxTime=fRecoParam->GetLastBin()+6; // add 3 virtual time bins before and 3   after
    
  Int_t nclusters  = 0;

  for (Int_t n=0; n<nentries; n++) {
    fInput->GetEvent(n);
    if (!fParam->AdjustSectorRow(digarr.GetID(),fSector,fRow)) {
      cerr<<"AliTPC warning: invalid segment ID ! "<<digarr.GetID()<<endl;
      continue;
    }
    Int_t row = fRow;
    AliTPCCalROC * gainROC = gainTPC->GetCalROC(fSector);  // pad gains per given sector
    AliTPCCalROC * noiseROC   = noiseTPC->GetCalROC(fSector); // noise per given sector
    //

    fRowCl->SetID(digarr.GetID());
    if (fOutput) fOutput->GetBranch("Segment")->SetAddress(&fRowCl);
    fRx=fParam->GetPadRowRadii(fSector,row);
    
    
    const Int_t kNIS=fParam->GetNInnerSector(), kNOS=fParam->GetNOuterSector();
    fZWidth = fParam->GetZWidth();
    if (fSector < kNIS) {
      fMaxPad = fParam->GetNPadsLow(row);
      fSign = (fSector < kNIS/2) ? 1 : -1;
      fPadLength = fParam->GetPadPitchLength(fSector,row);
      fPadWidth = fParam->GetPadPitchWidth();
    } else {
      fMaxPad = fParam->GetNPadsUp(row);
      fSign = ((fSector-kNIS) < kNOS/2) ? 1 : -1;
      fPadLength = fParam->GetPadPitchLength(fSector,row);
      fPadWidth  = fParam->GetPadPitchWidth();
    }
    
    
    fMaxBin=fMaxTime*(fMaxPad+6);  // add 3 virtual pads  before and 3 after
    fBins    =new Float_t[fMaxBin];
    fSigBins =new Int_t[fMaxBin];
    fNSigBins = 0;
    memset(fBins,0,sizeof(Float_t)*fMaxBin);
    
    if (digarr.First()) //MI change
      do {
	Float_t dig=digarr.CurrentDigit();
	if (dig<=fParam->GetZeroSup()) continue;
	Int_t j=digarr.CurrentRow()+3, i=digarr.CurrentColumn()+3;
	Float_t gain = gainROC->GetValue(row,digarr.CurrentColumn());
	Int_t bin = i*fMaxTime+j;
	if (gain>0){
	  fBins[bin]=dig/gain;
	}else{
	  fBins[bin]=0;
	}
	fSigBins[fNSigBins++]=bin;
      } while (digarr.Next());
    digarr.ExpandTrackBuffer();

    FindClusters(noiseROC);
    FillRow();
    fRowCl->GetArray()->Clear();    
    nclusters+=fNcluster;    

    delete[] fBins;
    delete[] fSigBins;
  }  
 
  Info("Digits2Clusters", "Number of found clusters : %d", nclusters);
}

void AliTPCclustererMI::ProcessSectorData(Float_t** allBins, Int_t** allSigBins, Int_t*  allNSigBins){
  //
  // Process the data for the current sector
  //

  AliTPCCalPad * pedestalTPC = AliTPCcalibDB::Instance()->GetPedestals();
  AliTPCCalPad * noiseTPC = AliTPCcalibDB::Instance()->GetPadNoise();
  AliTPCCalROC * pedestalROC = pedestalTPC->GetCalROC(fSector);  // pedestal per given sector
  AliTPCCalROC * noiseROC   = noiseTPC->GetCalROC(fSector);  // noise per given sector
  //check the presence of the calibration
  if (!noiseROC ||!pedestalROC ) {
    AliError(Form("Missing calibration per sector\t%d\n",fSector));
    return;
  }
  Int_t  nRows=fParam->GetNRow(fSector);
  Bool_t calcPedestal = fRecoParam->GetCalcPedestal();
  Int_t zeroSup = fParam->GetZeroSup();
  //    if (calcPedestal) {
  if (kFALSE ) {
    for (Int_t iRow = 0; iRow < nRows; iRow++) {
      Int_t maxPad = fParam->GetNPads(fSector, iRow);
      
      for (Int_t iPad = 3; iPad < maxPad + 3; iPad++) {
    //
    // Temporary fix for data production - !!!! MARIAN
    // The noise calibration should take mean and RMS - currently the Gaussian fit used
    // In case of double peak  - the pad should be rejected
    //
    // Line mean - if more than given digits over threshold - make a noise calculation
    // and pedestal substration
        if (!calcPedestal && allBins[iRow][iPad*fMaxTime+0]<50) continue;
    //
        if (allBins[iRow][iPad*fMaxTime+0] <1 ) continue;  // no data
        Float_t *p = &allBins[iRow][iPad*fMaxTime+3];
    //Float_t pedestal = TMath::Median(fMaxTime, p);
        Int_t id[3] = {fSector, iRow, iPad-3};
    // calib values
        Double_t rmsCalib=  noiseROC->GetValue(iRow,iPad-3);
        Double_t pedestalCalib = pedestalROC->GetValue(iRow,iPad-3);
        Double_t rmsEvent       = rmsCalib;
        Double_t pedestalEvent  = pedestalCalib;
        ProcesSignal(p, fMaxTime, id, rmsEvent, pedestalEvent);
        if (rmsEvent<rmsCalib) rmsEvent = rmsCalib;   // take worst scenario
        if (TMath::Abs(pedestalEvent-pedestalCalib)<1.0) pedestalEvent = pedestalCalib;
        
    //
        for (Int_t iTimeBin = 0; iTimeBin < fMaxTime; iTimeBin++) {
          Int_t bin = iPad*fMaxTime+iTimeBin;
          allBins[iRow][bin] -= pedestalEvent;
          if (iTimeBin < fRecoParam->GetFirstBin())
            allBins[iRow][bin] = 0;
          if (iTimeBin > fRecoParam->GetLastBin())
            allBins[iRow][bin] = 0;
          if (allBins[iRow][iPad*fMaxTime+iTimeBin] < zeroSup)
            allBins[iRow][bin] = 0;
          if (allBins[iRow][bin] < 3.0*rmsEvent)   // 3 sigma cut on RMS
            allBins[iRow][bin] = 0;
          if (allBins[iRow][bin]) allSigBins[iRow][allNSigBins[iRow]++] = bin;
        }
      }
    }
  }
  
  if (AliTPCReconstructor::StreamLevel()>3) {
    for (Int_t iRow = 0; iRow < nRows; iRow++) {
      Int_t maxPad = fParam->GetNPads(fSector,iRow);
      
      for (Int_t iPad = 3; iPad < maxPad + 3; iPad++) {
        for (Int_t iTimeBin = 0; iTimeBin < fMaxTime; iTimeBin++) {
          Int_t bin = iPad*fMaxTime+iTimeBin;
          Float_t signal = allBins[iRow][bin];
          if (AliTPCReconstructor::StreamLevel()>3 && signal>3) {
            Double_t x[]={iRow,iPad-3,iTimeBin-3};
            Int_t i[]={fSector};
            AliTPCTransform trafo;
            trafo.Transform(x,i,0,1);
            Double_t gx[3]={x[0],x[1],x[2]};
            trafo.RotatedGlobal2Global(fSector,gx);
        //        allSigBins[iRow][allNSigBins[iRow]++]
            Int_t rowsigBins = allNSigBins[iRow];
            Int_t first=allSigBins[iRow][0];
            Int_t last= 0;
        //        if (rowsigBins>0) allSigBins[iRow][allNSigBins[iRow]-1];
            
            if (AliTPCReconstructor::StreamLevel()>0) {
              (*fDebugStreamer)<<"Digits"<<
                "sec="<<fSector<<
                "row="<<iRow<<
                "pad="<<iPad<<
                "time="<<iTimeBin<<
                "sig="<<signal<<
                "x="<<x[0]<<
                "y="<<x[1]<<
                "z="<<x[2]<<
                "gx="<<gx[0]<<
                "gy="<<gx[1]<<
                "gz="<<gx[2]<<
    //
                "rowsigBins="<<rowsigBins<<
                "first="<<first<<
                "last="<<last<<
                "\n";
            }
          }
        }
      }
    }
  }
  
    // Now loop over rows and find clusters
  for (fRow = 0; fRow < nRows; fRow++) {
    fRowCl->SetID(fParam->GetIndex(fSector, fRow));
    if (fOutput) fOutput->GetBranch("Segment")->SetAddress(&fRowCl);
    
    fRx = fParam->GetPadRowRadii(fSector, fRow);
    fPadLength = fParam->GetPadPitchLength(fSector, fRow);
    fPadWidth  = fParam->GetPadPitchWidth();
    fMaxPad = fParam->GetNPads(fSector,fRow);
    fMaxBin = fMaxTime*(fMaxPad+6);  // add 3 virtual pads  before and 3 after
    
    fBins = allBins[fRow];
    fSigBins = allSigBins[fRow];
    fNSigBins = allNSigBins[fRow];
    
    FindClusters(noiseROC);
    
    FillRow();
    if(fBClonesArray == kFALSE) fRowCl->GetArray()->Clear();
    fNclusters += fNcluster;
    
  } // End of loop to find clusters
}


void AliTPCclustererMI::Digits2Clusters(AliRawReader* rawReader)
{
//-----------------------------------------------------------------
// This is a cluster finder for the TPC raw data.
// The method assumes NO ordering of the altro channels.
// The pedestal subtraction can be switched on and off
// using an option of the TPC reconstructor
//-----------------------------------------------------------------
  fRecoParam = AliTPCReconstructor::GetRecoParam();
  if (!fRecoParam){
    AliFatal("Can not get the reconstruction parameters");
  }
  if(AliTPCReconstructor::StreamLevel()>5) {
    AliInfo("Parameter Dumps");
    fParam->Dump();
    fRecoParam->Dump();
  }
  fRowDig = NULL;
  AliTPCROC * roc = AliTPCROC::Instance();
  AliTPCCalPad * gainTPC = AliTPCcalibDB::Instance()->GetPadGainFactor();
  AliTPCAltroMapping** mapping =AliTPCcalibDB::Instance()->GetMapping();
  //
  AliTPCRawStreamV3 input(rawReader,(AliAltroMapping**)mapping);
  fEventHeader = (AliRawEventHeaderBase*)rawReader->GetEventHeader();
  if (fEventHeader){
    fTimeStamp = fEventHeader->Get("Timestamp");
    fEventType = fEventHeader->Get("Type");
  }
  
  // creaate one TClonesArray for all clusters
  if(fBClonesArray && !fOutputClonesArray) fOutputClonesArray = new TClonesArray("AliTPCclusterMI",1000);
  // reset counter
  fNclusters  = 0;
  
  fMaxTime = fRecoParam->GetLastBin() + 6; // add 3 virtual time bins before and 3 after
//   const Int_t kNIS = fParam->GetNInnerSector();
//   const Int_t kNOS = fParam->GetNOuterSector();
//   const Int_t kNS = kNIS + kNOS;
  fZWidth = fParam->GetZWidth();
  Int_t zeroSup = fParam->GetZeroSup();
  //
  //alocate memory for sector - maximal case
  //
  Float_t** allBins = NULL;
  Int_t** allSigBins = NULL;
  Int_t*  allNSigBins = NULL;
  Int_t nRowsMax = roc->GetNRows(roc->GetNSector()-1);
  Int_t nPadsMax = roc->GetNPads(roc->GetNSector()-1,nRowsMax-1);
  allBins = new Float_t*[nRowsMax];
  allSigBins = new Int_t*[nRowsMax];
  allNSigBins = new Int_t[nRowsMax];
  for (Int_t iRow = 0; iRow < nRowsMax; iRow++) {
    //
    Int_t maxBin = fMaxTime*(nPadsMax+6);  // add 3 virtual pads  before and 3 after
    allBins[iRow] = new Float_t[maxBin];
    memset(allBins[iRow],0,sizeof(Float_t)*maxBin);
    allSigBins[iRow] = new Int_t[maxBin];
    allNSigBins[iRow]=0;
  }

  Int_t prevSector=-1;
  rawReader->Reset();
  Int_t digCounter=0;
  //
  // Loop over DDLs
  //
  const Int_t kNIS = fParam->GetNInnerSector();
  const Int_t kNOS = fParam->GetNOuterSector();
  const Int_t kNS = kNIS + kNOS;
  
  for(fSector = 0; fSector < kNS; fSector++) {
    
    Int_t nRows = 0;
    Int_t nDDLs = 0, indexDDL = 0;
    if (fSector < kNIS) {
      nRows = fParam->GetNRowLow();
      fSign = (fSector < kNIS/2) ? 1 : -1;
      nDDLs = 2;
      indexDDL = fSector * 2;
    }
    else {
      nRows = fParam->GetNRowUp();
      fSign = ((fSector-kNIS) < kNOS/2) ? 1 : -1;
      nDDLs = 4;
      indexDDL = (fSector-kNIS) * 4 + kNIS * 2;
    }
    
    // load the raw data for corresponding DDLs
    rawReader->Reset();
    rawReader->Select("TPC",indexDDL,indexDDL+nDDLs-1);
    
  while (input.NextDDL()){
    if (input.GetSector() != fSector)
      AliFatal(Form("Sector index mismatch ! Expected (%d), but got (%d) !",fSector,input.GetSector()));
    
    Int_t nRows = fParam->GetNRow(fSector);
    
    AliTPCCalROC * gainROC    = gainTPC->GetCalROC(fSector);  // pad gains per given sector
    // Begin loop over altro data
    Bool_t calcPedestal = fRecoParam->GetCalcPedestal();
    Float_t gain =1;
    
    //loop over pads
    while ( input.NextChannel() ) {
      Int_t iRow = input.GetRow();
      if (iRow < 0){
        continue;
      }
      if (iRow >= nRows){
        AliError(Form("Pad-row index (%d) outside the range (%d -> %d) !",
                      iRow, 0, nRows -1));
        continue;
      }
      //pad
      Int_t iPad = input.GetPad();
      if (iPad < 0 || iPad >= nPadsMax) {
        AliError(Form("Pad index (%d) outside the range (%d -> %d) !",
                      iPad, 0, nPadsMax-1));
        continue;
      }
      gain    = gainROC->GetValue(iRow,iPad);
      iPad+=3;

      //loop over bunches
      while ( input.NextBunch() ){
        Int_t  startTbin    = (Int_t)input.GetStartTimeBin();
        Int_t  bunchlength  = (Int_t)input.GetBunchLength();
        const UShort_t *sig = input.GetSignals();
        for (Int_t iTime = 0; iTime<bunchlength; iTime++){
          Int_t iTimeBin=startTbin-iTime;
          if ( iTimeBin < fRecoParam->GetFirstBin() || iTimeBin >= fRecoParam->GetLastBin()){
            continue;
            AliFatal(Form("Timebin index (%d) outside the range (%d -> %d) !",
                          iTimeBin, 0, iTimeBin -1));
          }
          iTimeBin+=3;
          //signal
          Float_t signal=(Float_t)sig[iTime];
          if (!calcPedestal && signal <= zeroSup) continue;
      
          if (!calcPedestal) {
            Int_t bin = iPad*fMaxTime+iTimeBin;
            if (gain>0){
              allBins[iRow][bin] = signal/gain;
            }else{
              allBins[iRow][bin] =0;
            }
            allSigBins[iRow][allNSigBins[iRow]++] = bin;
          }else{
            allBins[iRow][iPad*fMaxTime+iTimeBin] = signal;
          }
          allBins[iRow][iPad*fMaxTime+0]+=1.;  // pad with signal
          
          // Temporary
          digCounter++;
        }// end loop signals in bunch
      }// end loop bunches
    } // end loop pads
    //
    //
    //
    //
    // Now loop over rows and perform pedestal subtraction
    if (digCounter==0) continue;
  } // End of loop over sectors
  //process last sector
  if ( digCounter>0 ){
    ProcessSectorData(allBins, allSigBins, allNSigBins);
    for (Int_t iRow = 0; iRow < fParam->GetNRow(fSector); iRow++) {
      Int_t maxPad = fParam->GetNPads(fSector,iRow);
      Int_t maxBin = fMaxTime*(maxPad+6);  // add 3 virtual pads  before and 3 after
      memset(allBins[iRow],0,sizeof(Float_t)*maxBin);
      allNSigBins[iRow] = 0;
    }
    prevSector=fSector;
    digCounter=0;
  }
  }
  

  
  for (Int_t iRow = 0; iRow < nRowsMax; iRow++) {
    delete [] allBins[iRow];
    delete [] allSigBins[iRow];
  }
  delete [] allBins;
  delete [] allSigBins;
  delete [] allNSigBins;
  
  if (rawReader->GetEventId() && fOutput ){
    Info("Digits2Clusters", "File  %s Event\t%d\tNumber of found clusters : %d\n", fOutput->GetName(),*(rawReader->GetEventId()), fNclusters);
  }
  
  if(rawReader->GetEventId()) {
    Info("Digits2Clusters", "Event\t%d\tNumber of found clusters : %d\n",*(rawReader->GetEventId()), fNclusters);
  }
  
  if(fBClonesArray) {
    //Info("Digits2Clusters", "Number of found clusters : %d\n",fOutputClonesArray->GetEntriesFast());
  }
}





void AliTPCclustererMI::Digits2ClustersOld
(AliRawReader* rawReader)
{
//-----------------------------------------------------------------
// This is a cluster finder for the TPC raw data.
// The method assumes NO ordering of the altro channels.
// The pedestal subtraction can be switched on and off
// using an option of the TPC reconstructor
//-----------------------------------------------------------------
  fRecoParam = AliTPCReconstructor::GetRecoParam();
  if (!fRecoParam){
    AliFatal("Can not get the reconstruction parameters");
  }
  if(AliTPCReconstructor::StreamLevel()>5) {
    AliInfo("Parameter Dumps");
    fParam->Dump();
    fRecoParam->Dump();
  }
  fRowDig = NULL;
  AliTPCROC * roc = AliTPCROC::Instance();
  AliTPCCalPad * gainTPC = AliTPCcalibDB::Instance()->GetPadGainFactor();
  AliTPCAltroMapping** mapping =AliTPCcalibDB::Instance()->GetMapping();
  //
  AliTPCRawStream input(rawReader,(AliAltroMapping**)mapping);
  fEventHeader = (AliRawEventHeaderBase*)rawReader->GetEventHeader();
  if (fEventHeader){
    fTimeStamp = fEventHeader->Get("Timestamp");  
    fEventType = fEventHeader->Get("Type");  
  }

  // creaate one TClonesArray for all clusters
  if(fBClonesArray && !fOutputClonesArray) fOutputClonesArray = new TClonesArray("AliTPCclusterMI",1000);
  // reset counter
  fNclusters  = 0;
  
  fMaxTime = fRecoParam->GetLastBin() + 6; // add 3 virtual time bins before and 3 after
  const Int_t kNIS = fParam->GetNInnerSector();
  const Int_t kNOS = fParam->GetNOuterSector();
  const Int_t kNS = kNIS + kNOS;
  fZWidth = fParam->GetZWidth();
  Int_t zeroSup = fParam->GetZeroSup();
  //
  //alocate memory for sector - maximal case
  //
  Float_t** allBins = NULL;
  Int_t** allSigBins = NULL;
  Int_t*  allNSigBins = NULL;
  Int_t nRowsMax = roc->GetNRows(roc->GetNSector()-1);
  Int_t nPadsMax = roc->GetNPads(roc->GetNSector()-1,nRowsMax-1);
  allBins = new Float_t*[nRowsMax];
  allSigBins = new Int_t*[nRowsMax];
  allNSigBins = new Int_t[nRowsMax];
  for (Int_t iRow = 0; iRow < nRowsMax; iRow++) {
    //
    Int_t maxBin = fMaxTime*(nPadsMax+6);  // add 3 virtual pads  before and 3 after
    allBins[iRow] = new Float_t[maxBin];
    memset(allBins[iRow],0,sizeof(Float_t)*maxBin);
    allSigBins[iRow] = new Int_t[maxBin];
    allNSigBins[iRow]=0;
  }
  //
  // Loop over sectors
  //
  for(fSector = 0; fSector < kNS; fSector++) {

    Int_t nRows = 0;
    Int_t nDDLs = 0, indexDDL = 0;
    if (fSector < kNIS) {
      nRows = fParam->GetNRowLow();
      fSign = (fSector < kNIS/2) ? 1 : -1;
      nDDLs = 2;
      indexDDL = fSector * 2;
    }
    else {
      nRows = fParam->GetNRowUp();
      fSign = ((fSector-kNIS) < kNOS/2) ? 1 : -1;
      nDDLs = 4;
      indexDDL = (fSector-kNIS) * 4 + kNIS * 2;
    }

    // load the raw data for corresponding DDLs
    rawReader->Reset();
    rawReader->Select("TPC",indexDDL,indexDDL+nDDLs-1);

    // select only good sector 
    input.Next();
    if(input.GetSector() != fSector) continue;

    AliTPCCalROC * gainROC    = gainTPC->GetCalROC(fSector);  // pad gains per given sector
    
    for (Int_t iRow = 0; iRow < nRows; iRow++) {
      Int_t maxPad;
      if (fSector < kNIS)
        maxPad = fParam->GetNPadsLow(iRow);
      else
        maxPad = fParam->GetNPadsUp(iRow);
      
      Int_t maxBin = fMaxTime*(maxPad+6);  // add 3 virtual pads  before and 3 after
      memset(allBins[iRow],0,sizeof(Float_t)*maxBin);
      allNSigBins[iRow] = 0;
    }
    
    Int_t digCounter=0;
    // Begin loop over altro data
    Bool_t calcPedestal = fRecoParam->GetCalcPedestal();
    Float_t gain =1;
    Int_t lastPad=-1;

    input.Reset();
    while (input.Next()) {
      if (input.GetSector() != fSector)
        AliFatal(Form("Sector index mismatch ! Expected (%d), but got (%d) !",fSector,input.GetSector()));


      Int_t iRow = input.GetRow();
      if (iRow < 0){
        continue;
      }

      if (iRow < 0 || iRow >= nRows){
        AliError(Form("Pad-row index (%d) outside the range (%d -> %d) !",
		      iRow, 0, nRows -1));
        continue;
      }
      //pad
      Int_t iPad = input.GetPad();
      if (iPad < 0 || iPad >= nPadsMax) {
        AliError(Form("Pad index (%d) outside the range (%d -> %d) !",
		      iPad, 0, nPadsMax-1));
        continue;
      }
      if (iPad!=lastPad){
        gain    = gainROC->GetValue(iRow,iPad);
        lastPad = iPad;
      }
      iPad+=3;
      //time
      Int_t iTimeBin = input.GetTime();
      if ( iTimeBin < fRecoParam->GetFirstBin() || iTimeBin >= fRecoParam->GetLastBin()){
        continue;
        AliFatal(Form("Timebin index (%d) outside the range (%d -> %d) !",
		      iTimeBin, 0, iTimeBin -1));
      }
      iTimeBin+=3;

      //signal
      Float_t signal = input.GetSignal();
      if (!calcPedestal && signal <= zeroSup) continue;

      if (!calcPedestal) {
        Int_t bin = iPad*fMaxTime+iTimeBin;
        if (gain>0){
          allBins[iRow][bin] = signal/gain;
        }else{
          allBins[iRow][bin] =0;
        }
        allSigBins[iRow][allNSigBins[iRow]++] = bin;
      }else{
        allBins[iRow][iPad*fMaxTime+iTimeBin] = signal;
      }
      allBins[iRow][iPad*fMaxTime+0]+=1.;  // pad with signal

      // Temporary
      digCounter++;
    } // End of the loop over altro data
    //
    //
    //
    //
    // Now loop over rows and perform pedestal subtraction
    if (digCounter==0) continue;
    ProcessSectorData(allBins, allSigBins, allNSigBins);
  } // End of loop over sectors

  for (Int_t iRow = 0; iRow < nRowsMax; iRow++) {
    delete [] allBins[iRow];
    delete [] allSigBins[iRow];
  }  
  delete [] allBins;
  delete [] allSigBins;
  delete [] allNSigBins;
  
  if (rawReader->GetEventId() && fOutput ){
    Info("Digits2Clusters", "File  %s Event\t%d\tNumber of found clusters : %d\n", fOutput->GetName(),*(rawReader->GetEventId()), fNclusters);
  }
  
  if(rawReader->GetEventId()) {
    Info("Digits2Clusters", "Event\t%d\tNumber of found clusters : %d\n",*(rawReader->GetEventId()), fNclusters);
  }

  if(fBClonesArray) {
    //Info("Digits2Clusters", "Number of found clusters : %d\n",fOutputClonesArray->GetEntriesFast());
  }
}

void AliTPCclustererMI::FindClusters(AliTPCCalROC * noiseROC)
{
  
  //
  // add virtual charge at the edge   
  //
  Double_t kMaxDumpSize = 500000;
  if (!fOutput) {
    fBDumpSignal =kFALSE;
  }else{
    if (fRecoParam->GetCalcPedestal() && fOutput->GetZipBytes()< kMaxDumpSize) fBDumpSignal =kTRUE;   //dump signal flag
  }
   
  fNcluster=0;
  fLoop=1;
  Int_t crtime = Int_t((fParam->GetZLength(fSector)-fRecoParam->GetCtgRange()*fRx)/fZWidth-fParam->GetNTBinsL1()-5);
  Float_t minMaxCutAbs       = fRecoParam->GetMinMaxCutAbs();
  Float_t minLeftRightCutAbs = fRecoParam->GetMinLeftRightCutAbs();
  Float_t minUpDownCutAbs    = fRecoParam->GetMinUpDownCutAbs();
  Float_t minMaxCutSigma       = fRecoParam->GetMinMaxCutSigma();
  Float_t minLeftRightCutSigma = fRecoParam->GetMinLeftRightCutSigma();
  Float_t minUpDownCutSigma    = fRecoParam->GetMinUpDownCutSigma();
  for (Int_t iSig = 0; iSig < fNSigBins; iSig++) {
    Int_t i = fSigBins[iSig];
    if (i%fMaxTime<=crtime) continue;
    Float_t *b = &fBins[i];
    //absolute custs
    if (b[0]<minMaxCutAbs) continue;   //threshold for maxima  
    //
    if (b[-1]+b[1]+b[-fMaxTime]+b[fMaxTime]<=0) continue;  // cut on isolated clusters 
    if (b[-1]+b[1]<=0) continue;               // cut on isolated clusters
    if (b[-fMaxTime]+b[fMaxTime]<=0) continue; // cut on isolated clusters
    //
    if ((b[0]+b[-1]+b[1])<minUpDownCutAbs) continue;   //threshold for up down  (TRF) 
    if ((b[0]+b[-fMaxTime]+b[fMaxTime])<minLeftRightCutAbs) continue;   //threshold for left right (PRF)    
    if (!IsMaximum(*b,fMaxTime,b)) continue;
    //
    Float_t noise = noiseROC->GetValue(fRow, i/fMaxTime);
    if (noise>fRecoParam->GetMaxNoise()) continue;
    // sigma cuts
    if (b[0]<minMaxCutSigma*noise) continue;   //threshold form maxima  
    if ((b[0]+b[-1]+b[1])<minUpDownCutSigma*noise) continue;   //threshold for up town TRF 
    if ((b[0]+b[-fMaxTime]+b[fMaxTime])<minLeftRightCutSigma*noise) continue;   //threshold for left right (PRF)    
  
    AliTPCclusterMI c;   // default cosntruction  without info
    Int_t dummy=0;
    MakeCluster(i, fMaxTime, fBins, dummy,c);
    
    //}
  }
}

Bool_t AliTPCclustererMI::AcceptCluster(AliTPCclusterMI *cl){
  //
  // Currently hack to filter digital noise (15.06.2008)
  // To be parameterized in the AliTPCrecoParam
  // More inteligent way  to be used in future
  // Acces to the proper pedestal file needed
  //
  if (cl->GetMax()<400) return kTRUE;
  Double_t ratio = cl->GetQ()/cl->GetMax();
  if (cl->GetMax()>700){
    if ((ratio - int(ratio)>0.8)) return kFALSE;
  }
  if ((ratio - int(ratio)<0.95)) return kTRUE;
  return kFALSE;
}


Double_t AliTPCclustererMI::ProcesSignal(Float_t *signal, Int_t nchannels, Int_t id[3], Double_t &rmsEvent, Double_t &pedestalEvent){
  //
  // process signal on given pad - + streaming of additional information in special mode
  //
  // id[0] - sector
  // id[1] - row
  // id[2] - pad 

  //
  // ESTIMATE pedestal and the noise
  // 
  const Int_t kPedMax = 100;
  Float_t  max    =  0;
  Float_t  maxPos =  0;
  Int_t    median =  -1;
  Int_t    count0 =  0;
  Int_t    count1 =  0;
  Float_t  rmsCalib   = rmsEvent;       // backup initial value ( from calib)
  Float_t  pedestalCalib = pedestalEvent;// backup initial value ( from calib)
  Int_t    firstBin = fRecoParam->GetFirstBin();
  //
  UShort_t histo[kPedMax];
  //memset(histo,0,kPedMax*sizeof(UShort_t));
  for (Int_t i=0; i<kPedMax; i++) histo[i]=0;
  for (Int_t i=0; i<fMaxTime; i++){
    if (signal[i]<=0) continue;
    if (signal[i]>max && i>firstBin) {
      max = signal[i];
      maxPos = i;
    }
    if (signal[i]>kPedMax-1) continue;
    histo[int(signal[i]+0.5)]++;
    count0++;
  }
  //
  for (Int_t i=1; i<kPedMax; i++){
    if (count1<count0*0.5) median=i;
    count1+=histo[i];
  }
  // truncated mean  
  //
  Float_t count10=histo[median] ,mean=histo[median]*median,  rms=histo[median]*median*median ;
  Float_t count06=histo[median] ,mean06=histo[median]*median,  rms06=histo[median]*median*median ;
  Float_t count09=histo[median] ,mean09=histo[median]*median,  rms09=histo[median]*median*median ;
  //
  for (Int_t idelta=1; idelta<10; idelta++){
    if (median-idelta<=0) continue;
    if (median+idelta>kPedMax) continue;
    if (count06<0.6*count1){
      count06+=histo[median-idelta];
      mean06 +=histo[median-idelta]*(median-idelta);
      rms06  +=histo[median-idelta]*(median-idelta)*(median-idelta);
      count06+=histo[median+idelta];
      mean06 +=histo[median+idelta]*(median+idelta);
      rms06  +=histo[median+idelta]*(median+idelta)*(median+idelta);
    }
    if (count09<0.9*count1){
      count09+=histo[median-idelta];
      mean09 +=histo[median-idelta]*(median-idelta);
      rms09  +=histo[median-idelta]*(median-idelta)*(median-idelta);
      count09+=histo[median+idelta];
      mean09 +=histo[median+idelta]*(median+idelta);
      rms09  +=histo[median+idelta]*(median+idelta)*(median+idelta);
    }
    if (count10<0.95*count1){
      count10+=histo[median-idelta];
      mean +=histo[median-idelta]*(median-idelta);
      rms  +=histo[median-idelta]*(median-idelta)*(median-idelta);
      count10+=histo[median+idelta];
      mean +=histo[median+idelta]*(median+idelta);
      rms  +=histo[median+idelta]*(median+idelta)*(median+idelta);
    }
  }
  if (count10) {
    mean  /=count10;
    rms    = TMath::Sqrt(TMath::Abs(rms/count10-mean*mean));
  }
  if (count06) {
    mean06/=count06;
    rms06    = TMath::Sqrt(TMath::Abs(rms06/count06-mean06*mean06));
  }
  if (count09) {
    mean09/=count09;
    rms09    = TMath::Sqrt(TMath::Abs(rms09/count09-mean09*mean09));
  }
  rmsEvent = rms09;
  //
  pedestalEvent = median;
  if (AliLog::GetDebugLevel("","AliTPCclustererMI")==0) return median;
  //
  UInt_t uid[3] = {UInt_t(id[0]),UInt_t(id[1]),UInt_t(id[2])};
  //
  // Dump mean signal info
  //
    if (AliTPCReconstructor::StreamLevel()>0) {
    (*fDebugStreamer)<<"Signal"<<
    "TimeStamp="<<fTimeStamp<<
    "EventType="<<fEventType<<
    "Sector="<<uid[0]<<
    "Row="<<uid[1]<<
    "Pad="<<uid[2]<<
    "Max="<<max<<
    "MaxPos="<<maxPos<<
    //
    "Median="<<median<<
    "Mean="<<mean<<
    "RMS="<<rms<<      
    "Mean06="<<mean06<<
    "RMS06="<<rms06<<
    "Mean09="<<mean09<<
    "RMS09="<<rms09<<
    "RMSCalib="<<rmsCalib<<
    "PedCalib="<<pedestalCalib<<
    "\n";
    }
  //
  // fill pedestal histogram
  //
  //
  //
  //
  Float_t kMin =fRecoParam->GetDumpAmplitudeMin();   // minimal signal to be dumped
  Float_t *dsignal = new Float_t[nchannels];
  Float_t *dtime   = new Float_t[nchannels];
  for (Int_t i=0; i<nchannels; i++){
    dtime[i] = i;
    dsignal[i] = signal[i];
  }

  TGraph * graph=0;
  //
  // Big signals dumping
  //    
  if (AliTPCReconstructor::StreamLevel()>0) {
  if (max-median>kMin &&maxPos>fRecoParam->GetFirstBin()) 
    (*fDebugStreamer)<<"SignalB"<<     // pads with signal
      "TimeStamp="<<fTimeStamp<<
      "EventType="<<fEventType<<
      "Sector="<<uid[0]<<
      "Row="<<uid[1]<<
      "Pad="<<uid[2]<<
      "Graph="<<graph<<
      "Max="<<max<<
      "MaxPos="<<maxPos<<	
      //
      "Median="<<median<<
      "Mean="<<mean<<
      "RMS="<<rms<<      
      "Mean06="<<mean06<<
      "RMS06="<<rms06<<
      "Mean09="<<mean09<<
      "RMS09="<<rms09<<
      "\n";
  delete graph;  
  }

  delete [] dsignal;
  delete [] dtime;
  if (rms06>fRecoParam->GetMaxNoise()) {
    pedestalEvent+=1024.;
    return 1024+median; // sign noisy channel in debug mode
  }
  return median;
}





