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


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Calibration base class for a single ROC                                  //
//  Contains one float value per pad                                         //
//     mapping of the pads taken form AliTPCROC                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//
// ROOT includes 
//
#include "TMath.h"
#include "TClass.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLinearFitter.h"
#include "TArrayI.h"
//
//
#include "AliTPCCalROC.h"
#include "AliMathBase.h"

ClassImp(AliTPCCalROC)


//_____________________________________________________________________________
AliTPCCalROC::AliTPCCalROC()
             :TObject(),
	      fSector(0),
	      fNChannels(0),
	      fNRows(0),
	      fIndexes(0),
	      fData(0)
{
  //
  // Default constructor
  //

}

//_____________________________________________________________________________
AliTPCCalROC::AliTPCCalROC(UInt_t  sector)
             :TObject(),
	      fSector(0),
	      fNChannels(0),
	      fNRows(0),
	      fIndexes(0),
	      fData(0)
{
  //
  // Constructor that initializes a given sector
  //
  fSector = sector;
  fNChannels    =  AliTPCROC::Instance()->GetNChannels(fSector);
  fNRows        =  AliTPCROC::Instance()->GetNRows(fSector);
  fIndexes      =  AliTPCROC::Instance()->GetRowIndexes(fSector);
  fData = new Float_t[fNChannels];
  for (UInt_t  idata = 0; idata< fNChannels; idata++) fData[idata] = 0.;
}

//_____________________________________________________________________________
AliTPCCalROC::AliTPCCalROC(const AliTPCCalROC &c)
             :TObject(c),
	      fSector(0),
	      fNChannels(0),
	      fNRows(0),
	      fIndexes(0),
	      fData(0)
{
  //
  // AliTPCCalROC copy constructor
  //
  fSector = c.fSector;
  fNChannels    =  AliTPCROC::Instance()->GetNChannels(fSector);
  fNRows        =  AliTPCROC::Instance()->GetNRows(fSector);
  fIndexes      =  AliTPCROC::Instance()->GetRowIndexes(fSector);
  //
  fData   = new Float_t[fNChannels];
  for (UInt_t  idata = 0; idata< fNChannels; idata++) fData[idata] = c.fData[idata];
}
//____________________________________________________________________________
AliTPCCalROC & AliTPCCalROC::operator =(const AliTPCCalROC & param)
{
  //
  // assignment operator - dummy
  //
  fData=param.fData;
  return (*this);
}


//_____________________________________________________________________________
AliTPCCalROC::~AliTPCCalROC()
{
  //
  // AliTPCCalROC destructor
  //
  if (fData) {
    delete [] fData;
    fData = 0;
  }
}



void AliTPCCalROC::Streamer(TBuffer &R__b)
{
   // Stream an object of class AliTPCCalROC.
   if (R__b.IsReading()) {
      AliTPCCalROC::Class()->ReadBuffer(R__b, this);
      fIndexes =  AliTPCROC::Instance()->GetRowIndexes(fSector);
   } else {
      AliTPCCalROC::Class()->WriteBuffer(R__b,this);
   }
}

//  //
//   // algebra
//   void Add(Float_t c1);
//   void Multiply(Float_t c1);
//   void Add(const AliTPCCalROC * roc, Double_t c1 = 1);
//   void Divide(const AliTPCCalROC * roc);   

void AliTPCCalROC::Add(Float_t c1){
  //
  // add constant
  //
  for (UInt_t  idata = 0; idata< fNChannels; idata++) fData[idata]+=c1;
}
void AliTPCCalROC::Multiply(Float_t c1){
  //
  // add constant
  //
  for (UInt_t  idata = 0; idata< fNChannels; idata++) fData[idata]*=c1;
}

void AliTPCCalROC::Add(const AliTPCCalROC * roc, Double_t c1){
  //
  // add values 
  //
  for (UInt_t  idata = 0; idata< fNChannels; idata++){
    fData[idata]+=roc->fData[idata]*c1;
  }
}


void AliTPCCalROC::Multiply(const AliTPCCalROC*  roc) {
  //
  // multiply values - per by pad
  //
  for (UInt_t  idata = 0; idata< fNChannels; idata++){
    fData[idata]*=roc->fData[idata];
  }
}


void AliTPCCalROC::Divide(const AliTPCCalROC*  roc) {
  //
  // divide values 
  //
  Float_t kEpsilon=0.00000000000000001;
  for (UInt_t  idata = 0; idata< fNChannels; idata++){
    if (TMath::Abs(roc->fData[idata])>kEpsilon)
      fData[idata]/=roc->fData[idata];
  }
}




Double_t AliTPCCalROC::GetLTM(Double_t *sigma, Double_t fraction){
  //
  //  Calculate LTM mean and sigma
  //
  Double_t *ddata = new Double_t[fNChannels];
  Double_t mean=0, lsigma=0;
  Int_t hh = TMath::Min(TMath::Nint(fraction *fNChannels), Int_t(fNChannels));
  for (UInt_t i=0;i<fNChannels;i++) ddata[i]= fData[i];
  AliMathBase::EvaluateUni(UInt_t(fNChannels),ddata, mean, lsigma, hh);
  if (sigma) *sigma=lsigma;
  delete [] ddata;
  return mean;
}

TH1F * AliTPCCalROC::MakeHisto1D(Float_t min, Float_t max,Int_t type){
  //
  // make 1D histo
  // type -1 = user defined range
  //       0 = nsigma cut nsigma=min
  //       1 = delta cut around median delta=min
  if (type>=0){
    if (type==0){
      // nsigma range
      Float_t mean  = GetMean();
      Float_t sigma = GetRMS();
      Float_t nsigma = TMath::Abs(min);
      min = mean-nsigma*sigma;
      max = mean+nsigma*sigma;
    }
    if (type==1){
      // fixed range
      Float_t mean   = GetMedian();
      Float_t  delta = min;
      min = mean-delta;
      max = mean+delta;
    }
    if (type==2){
      //
      // LTM mean +- nsigma
      //
      Double_t sigma;
      Float_t mean  = GetLTM(&sigma,max);
      sigma*=min;
      min = mean-sigma;
      max = mean+sigma;
    }
  }
  char  name[1000];
  sprintf(name,"%s ROC 1D%d",GetTitle(),fSector);
  TH1F * his = new TH1F(name,name,100, min,max);
  for (UInt_t irow=0; irow<fNRows; irow++){
    UInt_t npads = (Int_t)GetNPads(irow);
    for (UInt_t ipad=0; ipad<npads; ipad++){
      his->Fill(GetValue(irow,ipad));
    }
  }
  return his;
}



TH2F * AliTPCCalROC::MakeHisto2D(Float_t min, Float_t max,Int_t type){
  //
  // make 2D histo
  // type -1 = user defined range
  //       0 = nsigma cut nsigma=min
  //       1 = delta cut around median delta=min
  if (type>=0){
    if (type==0){
      // nsigma range
      Float_t mean  = GetMean();
      Float_t sigma = GetRMS();
      Float_t nsigma = TMath::Abs(min);
      min = mean-nsigma*sigma;
      max = mean+nsigma*sigma;
    }
    if (type==1){
      // fixed range
      Float_t mean   = GetMedian();
      Float_t  delta = min;
      min = mean-delta;
      max = mean+delta;
    }
    if (type==2){
      Double_t sigma;
      Float_t mean  = GetLTM(&sigma,max);
      sigma*=min;
      min = mean-sigma;
      max = mean+sigma;

    }
  }
  UInt_t maxPad = 0;
  for (UInt_t irow=0; irow<fNRows; irow++){
    if (GetNPads(irow)>maxPad) maxPad = GetNPads(irow);
  }
  char  name[1000];
  sprintf(name,"%s ROC%d",GetTitle(),fSector);
  TH2F * his = new TH2F(name,name,fNRows+10,-5, fNRows+5, maxPad+10, -(Int_t(maxPad/2))-5, maxPad/2+5);
  for (UInt_t irow=0; irow<fNRows; irow++){
    UInt_t npads = (Int_t)GetNPads(irow);
    for (UInt_t ipad=0; ipad<npads; ipad++){
      his->Fill(irow+0.5,Int_t(ipad)-Int_t(npads/2)+0.5,GetValue(irow,ipad));
    }
  }
  his->SetMaximum(max);
  his->SetMinimum(min);
  return his;
}

TH2F * AliTPCCalROC::MakeHistoOutliers(Float_t delta, Float_t fraction, Int_t type){
  //
  // Make Histogram with outliers
  // mode = 0 - sigma cut used
  // mode = 1 - absolute cut used
  // fraction - fraction of values used to define sigma
  // delta - in mode 0 - nsigma cut
  //            mode 1 - delta cut
  Double_t sigma;
  Float_t mean  = GetLTM(&sigma,fraction);  
  if (type==0) delta*=sigma; 
  UInt_t maxPad = 0;
  for (UInt_t irow=0; irow<fNRows; irow++){
    if (GetNPads(irow)>maxPad) maxPad = GetNPads(irow);
  }

  char  name[1000];
  sprintf(name,"%s ROC Outliers%d",GetTitle(),fSector);
  TH2F * his = new TH2F(name,name,fNRows+10,-5, fNRows+5, maxPad+10, -(Int_t(maxPad/2))-5, maxPad/2+5);
  for (UInt_t irow=0; irow<fNRows; irow++){
    UInt_t npads = (Int_t)GetNPads(irow);
    for (UInt_t ipad=0; ipad<npads; ipad++){
      if (TMath::Abs(GetValue(irow,ipad)-mean)>delta)
	his->Fill(irow+0.5,Int_t(ipad)-Int_t(npads/2)+0.5,1);
    }
  }
  return his;
}



void AliTPCCalROC::Draw(Option_t* opt){
  //
  // create histogram with values and draw it
  //
  TH1 * his=0; 
  TString option=opt;
  option.ToUpper();
  if (option.Contains("1D")){
    his = MakeHisto1D();
  }
  else{
    his = MakeHisto2D();
  }
  his->Draw(option);
}





void AliTPCCalROC::Test(){
  //
  // example function to show functionality and tes AliTPCCalROC
  //
  AliTPCCalROC  roc0(0);  
  for (UInt_t irow = 0; irow <roc0.GetNrows(); irow++){
    for (UInt_t ipad = 0; ipad <roc0.GetNPads(irow); ipad++){
      Float_t value  = irow+ipad/1000.;
      roc0.SetValue(irow,ipad,value);
    }
  }
  //
  AliTPCCalROC roc1(roc0);
  for (UInt_t irow = 0; irow <roc1.GetNrows(); irow++){
    for (UInt_t ipad = 0; ipad <roc1.GetNPads(irow); ipad++){
      Float_t value  = irow+ipad/1000.;
      if (roc1.GetValue(irow,ipad)!=value){
	printf("Read/Write error\trow=%d\tpad=%d\n",irow,ipad);
      }
    }
  }  
  TFile f("calcTest.root","recreate");
  roc0.Write("Roc0");
  AliTPCCalROC * roc2 = (AliTPCCalROC*)f.Get("Roc0");
  f.Close();
  //
  for (UInt_t irow = 0; irow <roc0.GetNrows(); irow++){
    if (roc0.GetNPads(irow)!=roc2->GetNPads(irow))
      printf("NPads - Read/Write error\trow=%d\n",irow);
    for (UInt_t ipad = 0; ipad <roc1.GetNPads(irow); ipad++){
      Float_t value  = irow+ipad/1000.;
      if (roc2->GetValue(irow,ipad)!=value){
	printf("Read/Write error\trow=%d\tpad=%d\n",irow,ipad);
      }
    }
  }   
}



AliTPCCalROC * AliTPCCalROC::LocalFit(Int_t rowRadius, Int_t padRadius, AliTPCCalROC* ROCoutliers, Bool_t robust) {
  //
  // MakeLocalFit - smoothing
  // rowRadius  -  radius - rows to be used for smoothing
  // padradius  -  radius - pads to be used for smoothing
  // ROCoutlier -  map of outliers - pads not to be used for local smoothing
  // robust     -  robust method of fitting  - (much slower)
  
  AliTPCCalROC * ROCfitted = new AliTPCCalROC(fSector);
  TLinearFitter fitterQ(6,"x0++x1++x2++x3++x4++x5");
  fitterQ.StoreData(kTRUE);
  for (Int_t row=0; row < GetNrows(); row++) {
    //std::cout << "Entering row " << row << " of " << GetNrows() << " @ sector "<< fSector << " for local fitting... "<< std::endl;
    for (Int_t pad=0; pad < GetNPads(row); pad++)
      ROCfitted->SetValue(row, pad, GetNeighbourhoodValue(&fitterQ, row, pad, rowRadius, padRadius, ROCoutliers, robust));
  }
  return ROCfitted;
}


Double_t AliTPCCalROC::GetNeighbourhoodValue(TLinearFitter* fitterQ, Int_t row, Int_t pad, Int_t rRadius, Int_t pRadius, AliTPCCalROC* ROCoutliers, Bool_t robust) {
  //
  //  AliTPCCalROC::GetNeighbourhoodValue - smoothing (PRIVATE)
  // rowRadius  -  radius - rows to be used for smoothing
  // padradius  -  radius - pads to be used for smoothing
  // ROCoutlier -  map of outliers - pads not to be used for local smoothing
  // robust     -  robust method of fitting  - (much slower)
  


  fitterQ->ClearPoints();
  TVectorD fitParam(6);
  Int_t    npoints=0;
  Double_t xx[6];
  Float_t dlx, dly;
  Float_t lPad[3] = {0};
  Float_t localXY[3] = {0};
  
  AliTPCROC* tpcROCinstance = AliTPCROC::Instance();
  tpcROCinstance->GetPositionLocal(fSector, row, pad, lPad);  // calculate position lPad by pad and row number
  
  TArrayI *neighbourhoodRows = 0;
  TArrayI *neighbourhoodPads = 0;
  GetNeighbourhood(neighbourhoodRows, neighbourhoodPads, row, pad, rRadius, pRadius);
  
  Int_t r, p;
  for (Int_t i=0; i < (2*rRadius+1)*(2*pRadius+1); i++) {
    r = neighbourhoodRows->At(i);
    p = neighbourhoodPads->At(i);
    if (r == -1 || p == -1) continue;
    tpcROCinstance->GetPositionLocal(fSector, r, p, localXY);   // calculate position localXY by pad and row number
    dlx = lPad[0] - localXY[0];
    dly = lPad[1] - localXY[1];
    xx[0] = 1;
    xx[1] = dlx;
    xx[2] = dly;
    xx[3] = dlx*dlx;
    xx[4] = dly*dly;
    xx[5] = dlx*dly;
    if (ROCoutliers && ROCoutliers->GetValue(r,p) != 1) {
      fitterQ->AddPoint(xx, GetValue(r, p), 1);
      npoints++;
    }
  }
  if (npoints < 0.5 * ((2*rRadius+1)*(2*pRadius+1)) ) {
    // std::cerr << "Too few data points for fitting @ row " << row << ", pad " << pad << " in sector " << fSector << std::endl;
    return 0.;  // for diagnostic
  }
  fitterQ->Eval();
  fitterQ->GetParameters(fitParam);
  Float_t chi2Q = 0;
  chi2Q = fitterQ->GetChisquare()/(npoints-6.);
  //if (robust) chi2Q = fitterQ->GetChisquare()/(npoints-6.);
  if (robust && chi2Q > 5) {
    //std::cout << "robust fitter called... " << std::endl;
    fitterQ->EvalRobust(0.7);
    fitterQ->GetParameters(fitParam);
  }
  Double_t value = fitParam[0];
  
  delete neighbourhoodRows;
  delete neighbourhoodPads;
  
  //if (value < 0) std::cerr << "negative fit-value " << value << " in sector "<< this->fSector << " @ row: " << row << " and pad: " << pad << ", with fitter Chi2 = " << chi2Q <<  std::endl;
  
  return value;
}




void AliTPCCalROC::GetNeighbourhood(TArrayI* &rowArray, TArrayI* &padArray, Int_t row, Int_t pad, Int_t rRadius, Int_t pRadius) {
  //
  //
  //
  rowArray = new TArrayI((2*rRadius+1)*(2*pRadius+1));
  padArray = new TArrayI((2*rRadius+1)*(2*pRadius+1));
  Int_t* rowArrayTemp = rowArray->GetArray();
  Int_t* padArrayTemp = padArray->GetArray();
  
  Int_t rmin = row - rRadius;
  Int_t rmax = row + rRadius;
  
  // if window goes out of ROC
  if (rmin < 0) {
    rmax = rmax - rmin;
    rmin = 0;
  }
  if (rmax >= GetNrows()) {
    rmin = rmin - (rmax - GetNrows()+1);
    rmax = GetNrows() - 1;
      if (rmin  < 0 ) rmin = 0; // if the window is bigger than the ROC
  }
  
  Int_t pmin, pmax;
  Int_t i = 0;
  
  for (Int_t r = rmin; r <= rmax; r++) {
    pmin = pad - pRadius;
    pmax = pad + pRadius;
    if (pmin < 0) {
      pmax = pmax - pmin;
      pmin = 0;
    }
    if (pmax >= GetNPads(r)) {
      pmin = pmin - (pmax - GetNPads(r)+1);
      pmax = GetNPads(r) - 1;
      if (pmin  < 0 ) pmin = 0; // if the window is bigger than the ROC
    }
    for (Int_t p = pmin; p <= pmax; p++) {
      rowArrayTemp[i] = r;
      padArrayTemp[i] = p;
      i++;
    }
  }
  for (Int_t j = i; j < rowArray->GetSize(); j++){  // unused padArray-entries, in the case that the window is bigger than the ROC
    //std::cout << "trying to write -1" << std::endl;
    rowArrayTemp[j] = -1;
    padArrayTemp[j] = -1;
    //std::cout << "writing -1" << std::endl;
  } 
}


