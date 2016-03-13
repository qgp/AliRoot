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

/// \class AliTPCCalPad
///
///  TPC calibration class for parameters which are saved per pad
///  Each AliTPCCalPad consists of 72 AliTPCCalROC-objects

#include "AliTPCCalPad.h"
#include "AliTPCCalROC.h"
#include <TObjArray.h>
#include <TAxis.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TH2F.h>
#include "TTreeStream.h"
#include "TLinearFitter.h"
#include "TFile.h"
#include "TKey.h"
#include <TFormula.h>
#include <TString.h>
#include <TObjString.h>
#include <iostream>
#include <AliLog.h>

//graphic includes
#include <TTree.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TCut.h>
#include <TVirtualPad.h>
#include "AliTPCPreprocessorOnline.h"
#include "AliTPCCalibViewer.h"
#include "TVectorD.h"

/// \cond CLASSIMP
ClassImp(AliTPCCalPad)
/// \endcond

//_____________________________________________________________________________
AliTPCCalPad::AliTPCCalPad():TNamed()
{
  //
  // AliTPCCalPad default constructor
  //

  for (Int_t isec = 0; isec < kNsec; isec++) {
    fROC[isec] = 0;
  }

}

//_____________________________________________________________________________
AliTPCCalPad::AliTPCCalPad(const Text_t *name, const Text_t *title)
                :TNamed(name,title)
{
  /// AliTPCCalPad constructor

  for (Int_t isec = 0; isec < kNsec; isec++) {
    fROC[isec] = new AliTPCCalROC(isec);
  }
}


//_____________________________________________________________________________
AliTPCCalPad::AliTPCCalPad(const AliTPCCalPad &c):TNamed(c)
{
  /// AliTPCCalPad copy constructor

  for (Int_t isec = 0; isec < kNsec; isec++) {
         fROC[isec] = 0;
     if (c.fROC[isec])
       fROC[isec] = new AliTPCCalROC(*(c.fROC[isec]));
  }
}

//_____________________________________________________________________________
AliTPCCalPad::AliTPCCalPad(TObjArray * array):TNamed(array->GetName(),array->GetName())
{
  /// AliTPCCalPad default constructor

  for (Int_t isec = 0; isec < kNsec; isec++) {
    fROC[isec] = (AliTPCCalROC *)array->At(isec);
  }

}


///_____________________________________________________________________________
AliTPCCalPad::~AliTPCCalPad()
{
  /// AliTPCCalPad destructor

  for (Int_t isec = 0; isec < kNsec; isec++) {
    if (fROC[isec]) {
      delete fROC[isec];
      fROC[isec] = 0;
    }
  }

}

//_____________________________________________________________________________
AliTPCCalPad &AliTPCCalPad::operator=(const AliTPCCalPad &c)
{
  /// Assignment operator

  if (this != &c) ((AliTPCCalPad &) c).Copy(*this);
  return *this;

}

//_____________________________________________________________________________
void AliTPCCalPad::Copy(TObject &c) const
{
  /// Copy function

  for (Int_t isec = 0; isec < kNsec; isec++) {
    if (fROC[isec]) {
      fROC[isec]->Copy(*((AliTPCCalPad &) c).fROC[isec]);
    }
  }
  TObject::Copy(c);
}


void AliTPCCalPad::Print(Option_t* option) const{
  //
  // Print summary content
  //
  printf("TPC %s\t%s\n",GetName(),GetTitle());
  //
  printf("|\t|%s|\t%s|\t%s|\n", "Mean","Median","RMS");
  printf("|ALL\t|%f|\t%f|\t%f|\n", GetMean(),GetMedian(),GetRMS());
  if (TString(option).Contains("list")){
    for (Int_t isec=0; isec<72; isec++){
      if (fROC[isec]) fROC[isec]->Print(option);
    }
  }
}




void AliTPCCalPad::SetCalROC(AliTPCCalROC* roc, Int_t sector){
   /// Set AliTPCCalROC copies values from 'roc'
   /// if sector == -1 the sector specified in 'roc' is used
   /// else sector specified in 'roc' is ignored and specified sector is filled

   if (sector == -1) sector = roc->GetSector();
   if (!fROC[sector]) fROC[sector] = new AliTPCCalROC(sector);
   for (UInt_t ichannel = 0; ichannel < roc->GetNchannels(); ichannel++)
      fROC[sector]->SetValue(ichannel, roc->GetValue(ichannel));
}

Bool_t  AliTPCCalPad::MedianFilter(Int_t deltaRow, Int_t deltaPad, AliTPCCalPad*outlierPad,  Bool_t doEdge){
  /// replace constent with median in the neigborhood

  Bool_t isOK=kTRUE;
  for (Int_t isec = 0; isec < kNsec; isec++) {
    AliTPCCalROC *outlierROC=(outlierPad==NULL)?NULL:outlierPad->GetCalROC(isec);
    if (fROC[isec]){
      isOK&=fROC[isec]->MedianFilter(deltaRow,deltaPad,outlierROC,doEdge);
    }
  }
  return isOK;
}

Bool_t  AliTPCCalPad::LTMFilter(Int_t deltaRow, Int_t deltaPad, Float_t fraction, Int_t type, AliTPCCalPad*outlierPad,  Bool_t doEdge){
  /// replace constent with LTM statistic  in  neigborhood

  Bool_t isOK=kTRUE;
  for (Int_t isec = 0; isec < kNsec; isec++) {
    AliTPCCalROC *outlierROC=(outlierPad==NULL)?NULL:outlierPad->GetCalROC(isec);
    if (fROC[isec]){
      isOK&=fROC[isec]->LTMFilter(deltaRow, deltaPad,fraction,type,outlierROC,doEdge);
    }
  }
  return isOK;
}

Bool_t  AliTPCCalPad::Convolute(Double_t sigmaPad, Double_t sigmaRow,  AliTPCCalPad*outlierPad, TF1 *fpad, TF1 *frow){
  /// replace constent with median in the neigborhood

  Bool_t isOK=kTRUE;
  for (Int_t isec = 0; isec < kNsec; isec++) {
    AliTPCCalROC *outlierROC=(outlierPad==NULL)?NULL:outlierPad->GetCalROC(isec);
    if (fROC[isec]){
      isOK&=fROC[isec]->Convolute(sigmaPad,sigmaRow,outlierROC,fpad,frow);
    }
  }
  return isOK;
}


//_____________________________________________________________________________
void AliTPCCalPad::Add(Float_t c1)
{
    /// add constant c1 to all channels of all ROCs

    for (Int_t isec = 0; isec < kNsec; isec++) {
	if (fROC[isec]){
	    fROC[isec]->Add(c1);
	}
    }
}

//_____________________________________________________________________________
void AliTPCCalPad::Multiply(Float_t c1)
{
  /// multiply each channel of all ROCs with c1

    for (Int_t isec = 0; isec < kNsec; isec++) {
	if (fROC[isec]){
	    fROC[isec]->Multiply(c1);
	}
    }
}

//_____________________________________________________________________________
void AliTPCCalPad::Add(const AliTPCCalPad * pad, Double_t c1)
{
  /// multiply AliTPCCalPad 'pad' by c1 and add each channel to the coresponing channel in all ROCs
  ///  - pad by pad -

    for (Int_t isec = 0; isec < kNsec; isec++) {
	if (fROC[isec] && pad->GetCalROC(isec)){
	    fROC[isec]->Add(pad->GetCalROC(isec),c1);
	}
    }
}

//_____________________________________________________________________________
void AliTPCCalPad::Multiply(const AliTPCCalPad * pad)
{
  /// multiply each channel of all ROCs with the coresponding channel of 'pad'
  ///     - pad by pad -

   for (Int_t isec = 0; isec < kNsec; isec++) {
	if (fROC[isec]){
	    fROC[isec]->Multiply(pad->GetCalROC(isec));
	}
    }
}

//_____________________________________________________________________________
void AliTPCCalPad::Divide(const AliTPCCalPad * pad)
{
  /// divide each channel of all ROCs by the coresponding channel of 'pad'
  ///     - pad by pad -

    for (Int_t isec = 0; isec < kNsec; isec++) {
	if (fROC[isec]){
	    fROC[isec]->Divide(pad->GetCalROC(isec));
	}
    }
}

//_____________________________________________________________________________
void AliTPCCalPad::Reset()
{
  /// Reset all cal Rocs

  for (Int_t isec = 0; isec < kNsec; isec++) {
    if (fROC[isec]){
      fROC[isec]->Reset();
    }
  }
}

//_____________________________________________________________________________
TGraph  *  AliTPCCalPad::MakeGraph(Int_t type, Float_t ratio, AliTPCCalROC::EPadType padType){
  ///   type=1 - mean
  ///        2 - median
  ///        3 - LTM
  ///   padType = AliTPCCalROC::kAll full oroc sectors
  ///             kOROCmedium,kOROClong split oroc sector

  Int_t npoints = 0;
  for (Int_t i=0;i<36;i++) if (fROC[i]) npoints++;//iroc
  if(padType==AliTPCCalROC::kAll) {
    for (Int_t i=36;i<72;i++) if (fROC[i]) npoints++;//oroc full
  } else {
    for (Int_t i=36;i<72;i++) if (fROC[i]) npoints+=2;//oroc split
  }
  TGraph * graph = new TGraph(npoints);
  npoints=0;
  for (Int_t isec=0;isec<36;isec++){//iroc
    if (!fROC[isec]) continue;
    if (type==0)  graph->SetPoint(npoints,isec,fROC[isec]->GetMean());
    if (type==1)  graph->SetPoint(npoints,isec,fROC[isec]->GetMedian());
    if (type==2)  graph->SetPoint(npoints,isec,fROC[isec]->GetLTM(0,ratio));
    npoints++;
  }
  if(padType==AliTPCCalROC::kAll) {
    for (Int_t isec=36;isec<72;isec++){//oroc full
      if (!fROC[isec]) continue;
      if (type==0)  graph->SetPoint(npoints,isec,fROC[isec]->GetMean());
      if (type==1)  graph->SetPoint(npoints,isec,fROC[isec]->GetMedian());
      if (type==2)  graph->SetPoint(npoints,isec,fROC[isec]->GetLTM(0,ratio));
      npoints++;
    }
  }else{
    for (Int_t isec=36;isec<72;isec++){//oroc split
      if (!fROC[isec]) continue;
      if (type==0)  graph->SetPoint(npoints,isec,fROC[isec]->GetMean(0,AliTPCCalROC::kOROCmedium));
      if (type==1)  graph->SetPoint(npoints,isec,fROC[isec]->GetMedian(0,AliTPCCalROC::kOROCmedium));
      if (type==2)  graph->SetPoint(npoints,isec,fROC[isec]->GetLTM(0,ratio,0,AliTPCCalROC::kOROCmedium));
      npoints++;
      if (type==0)  graph->SetPoint(npoints,isec+36,fROC[isec]->GetMean(0,AliTPCCalROC::kOROClong));
      if (type==1)  graph->SetPoint(npoints,isec+36,fROC[isec]->GetMedian(0,AliTPCCalROC::kOROClong));
      if (type==2)  graph->SetPoint(npoints,isec+36,fROC[isec]->GetLTM(0,ratio,0,AliTPCCalROC::kOROClong));
      npoints++;
    }
  }

  graph->GetXaxis()->SetTitle("Sector"); 
  if (type==0) {
    graph->GetYaxis()->SetTitle("Mean");   
    graph->SetMarkerStyle(22);    
  }
  if (type==1) {
    graph->GetYaxis()->SetTitle("Median");   
    graph->SetMarkerStyle(22);    
  }
  if (type==2) {
      graph->GetYaxis()->SetTitle(Form("Mean%f",ratio));      
      graph->SetMarkerStyle(24);
  }

  return graph;
}

//_____________________________________________________________________________
Double_t AliTPCCalPad::GetMeanRMS(Double_t &rms) const
{
    /// Calculates mean and RMS of all ROCs

    Double_t sum = 0, sum2 = 0, n=0, val=0;
    for (Int_t isec = 0; isec < kNsec; isec++) {
        AliTPCCalROC *calRoc = fROC[isec];
	if ( calRoc ){
	    for (UInt_t irow=0; irow<calRoc->GetNrows(); irow++){
		for (UInt_t ipad=0; ipad<calRoc->GetNPads(irow); ipad++){
		    val = calRoc->GetValue(irow,ipad);
		    sum+=val;
		    sum2+=val*val;
                    n++;
		}
	    }

	}
    }
    Double_t n1 = 1./n;
    Double_t mean = sum*n1;
    rms  = TMath::Sqrt(TMath::Abs(sum2*n1-mean*mean));
    return mean;
}

//_____________________________________________________________________________
Double_t AliTPCCalPad::GetStats(AliTPCCalROC::EStatType statType, AliTPCCalPad *const outlierPad, AliTPCCalROC::EPadType padType) const
{
  /// return mean of statType(kMean,kMedian,kRMS) 
  /// for padType(kAll,kOROCmedium,kOROClong)

  Double_t arr[kNsecSplit];
  Int_t n=0;
  for (Int_t isec = 0; isec < kNsec; isec++) {
    AliTPCCalROC *calRoc = fROC[isec];
    if ( calRoc ){
      AliTPCCalROC* outlierROC = 0;
      if (outlierPad) outlierROC = outlierPad->GetCalROC(isec);
      if(isec>=36 && padType!=AliTPCCalROC::kAll){
        arr[n] = calRoc->GetStats(statType,outlierROC,AliTPCCalROC::kOROCmedium);
	n++;
        arr[n] = calRoc->GetStats(statType,outlierROC,AliTPCCalROC::kOROClong);
	n++;
      } else {
        arr[n] = calRoc->GetStats(statType,outlierROC,AliTPCCalROC::kAll);
	n++;
      }

    }
  }

  Double_t val=0.;
  if      (statType==AliTPCCalROC::kMinElement) val=TMath::MinElement(n,arr);
  else if (statType==AliTPCCalROC::kMaxElement) val=TMath::MaxElement(n,arr);
  else                                          val=TMath::Mean      (n,arr);
  return val;
}

//_____________________________________________________________________________
Double_t AliTPCCalPad::GetMean(AliTPCCalPad* outlierPad, AliTPCCalROC::EPadType padType) const
{
  /// return mean of the mean of all ROCs

  return GetStats(AliTPCCalROC::kMean, outlierPad, padType);
}

//_____________________________________________________________________________
Double_t AliTPCCalPad::GetRMS(AliTPCCalPad* outlierPad, AliTPCCalROC::EPadType padType) const
{
  /// return mean of the RMS of all ROCs

  return GetStats(AliTPCCalROC::kRMS, outlierPad, padType);
}

//_____________________________________________________________________________
Double_t AliTPCCalPad::GetMedian(AliTPCCalPad* outlierPad, AliTPCCalROC::EPadType padType) const
{
  /// return mean of the median of all ROCs

  return GetStats(AliTPCCalROC::kMedian, outlierPad, padType);
}

//_____________________________________________________________________________
Double_t AliTPCCalPad::GetMinElement(AliTPCCalPad* outlierPad, AliTPCCalROC::EPadType padType) const
{
  /// return MinElement all ROCs

  return GetStats(AliTPCCalROC::kMinElement, outlierPad, padType);
}

//_____________________________________________________________________________
Double_t AliTPCCalPad::GetMaxElement(AliTPCCalPad* outlierPad, AliTPCCalROC::EPadType padType) const
{
  /// return MaxElement of all ROCs

  return GetStats(AliTPCCalROC::kMaxElement, outlierPad, padType);
}

//_____________________________________________________________________________
Double_t AliTPCCalPad::GetLTM(Double_t *sigma, Double_t fraction, AliTPCCalPad* outlierPad, AliTPCCalROC::EPadType padType) const
{
  /// return mean of the LTM and sigma of all ROCs

  Double_t arrm[kNsecSplit];
  Double_t arrs[kNsecSplit];
  Double_t *sTemp=0x0;
  Int_t n=0;

  for (Int_t isec = 0; isec < kNsec; isec++) {
    AliTPCCalROC *calRoc = fROC[isec];
    if ( calRoc ){
      if ( sigma ) sTemp=arrs+n;
      AliTPCCalROC* outlierROC = 0;
      if (outlierPad) outlierROC = outlierPad->GetCalROC(isec);
      if(isec>=36 && padType!=AliTPCCalROC::kAll){
	arrm[n] = calRoc->GetLTM(sTemp,fraction, outlierROC,AliTPCCalROC::kOROCmedium);
	n++;
	if ( sigma ) sTemp=arrs+n;
	arrm[n] = calRoc->GetLTM(sTemp,fraction, outlierROC,AliTPCCalROC::kOROClong);
	n++;
      } else {
	arrm[n] = calRoc->GetLTM(sTemp,fraction, outlierROC,AliTPCCalROC::kAll);
	n++;
      }

    }
  }
  if ( sigma ) *sigma = TMath::Mean(n,arrs);
  return TMath::Mean(n,arrm);
}

//_____________________________________________________________________________
TH1F * AliTPCCalPad::MakeHisto1D(Float_t min, Float_t max,Int_t type, Int_t side){
  /// make 1D histo
  /// type -1 = user defined range
  ///       0 = nsigma cut nsigma=min

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
  TString name=Form("%s Pad 1D",GetTitle());
  TH1F * his = new TH1F(name.Data(),name.Data(),100, min,max);
    for (Int_t isec = 0; isec < kNsec; isec++) {
      if (side==1 && isec%36>18) continue;
      if (side==-1 && isec%36<18) continue;
	if (fROC[isec]){
	    for (UInt_t irow=0; irow<fROC[isec]->GetNrows(); irow++){
		UInt_t npads = (Int_t)fROC[isec]->GetNPads(irow);
		for (UInt_t ipad=0; ipad<npads; ipad++){
		    his->Fill(fROC[isec]->GetValue(irow,ipad));
		}
	    }
	}
    }
  return his;
}

//_____________________________________________________________________________
TH2F *AliTPCCalPad::MakeHisto2D(Int_t side){
  /// Make 2D graph
  /// side  -  specify the side A = 0 C = 1
  /// type  -  used types of determination of boundaries in z

  Float_t kEpsilon = 0.000000000001;
  TH2F * his = new TH2F(GetName(), GetName(), 250,-250,250,250,-250,250);
  AliTPCROC * roc  = AliTPCROC::Instance();
  for (Int_t isec=0; isec<72; isec++){
    if (side==0 && isec%36>=18) continue;
    if (side>0 && isec%36<18) continue;
    if (fROC[isec]){
      AliTPCCalROC * calRoc = fROC[isec];
      for (UInt_t irow=0; irow<calRoc->GetNrows(); irow++)
	for (UInt_t ipad=0; ipad<calRoc->GetNPads(irow); ipad++)
	  if (TMath::Abs(calRoc->GetValue(irow,ipad))>kEpsilon){
	    Float_t xyz[3];
	    roc->GetPositionGlobal(isec,irow,ipad,xyz);
	    Int_t binx = 1+TMath::Nint((xyz[0]+250.)*0.5);
	    Int_t biny = 1+TMath::Nint((xyz[1]+250.)*0.5);
	    Float_t value = calRoc->GetValue(irow,ipad);
	    his->SetBinContent(binx,biny,value);
	  }
    }
  }
  his->SetXTitle("x (cm)");
  his->SetYTitle("y (cm)");
  return his;
}


AliTPCCalPad* AliTPCCalPad::LocalFit(const char* padName, Int_t rowRadius, Int_t padRadius, AliTPCCalPad* PadOutliers, Bool_t robust, Double_t chi2Threshold, Double_t robustFraction, Bool_t printCurrentSector) const {
   /// Loops over all AliTPCCalROCs and performs a localFit in each ROC
   /// AliTPCCalPad with fit-data is returned
   /// rowRadius and padRadius specifies a window around a given pad in one sector.
   /// The data of this window are fitted with a parabolic function.
   /// This function is evaluated at the pad's position.
   /// At the edges the window is shifted, so that the specified pad is not anymore in the center of the window.
   /// rowRadius  -  radius - rows to be used for smoothing
   /// padradius  -  radius - pads to be used for smoothing
   /// ROCoutlier -  map of outliers - pads not to be used for local smoothing
   /// robust     -  robust method of fitting  - (much slower)
   /// chi2Threshold: Threshold for chi2 when EvalRobust is called
   /// robustFraction: Fraction of data that will be used in EvalRobust

   AliTPCCalPad* pad = new AliTPCCalPad(padName, padName);
   for (Int_t isec = 0; isec < 72; isec++){
      if (printCurrentSector) std::cout << "LocalFit in sector " << isec << "\r" << std::flush;
      if (PadOutliers)
         pad->SetCalROC(GetCalROC(isec)->LocalFit(rowRadius, padRadius, PadOutliers->GetCalROC(isec), robust, chi2Threshold, robustFraction));
      else 
         pad->SetCalROC(GetCalROC(isec)->LocalFit(rowRadius, padRadius, 0, robust, chi2Threshold, robustFraction));
   }
   return pad;
}


AliTPCCalPad* AliTPCCalPad::GlobalFit(const char* padName, AliTPCCalPad* PadOutliers, Bool_t robust, Int_t fitType, Double_t chi2Threshold, Double_t robustFraction, Double_t err, TObjArray *fitParArr, TObjArray *fitCovArr, AliTPCCalROC::EPadType padType){
   /// Loops over all AliTPCCalROCs and performs a globalFit in each ROC
   /// AliTPCCalPad with fit-data is returned
   /// chi2Threshold: Threshold for chi2 when EvalRobust is called
   /// robustFraction: Fraction of data that will be used in EvalRobust
   /// chi2Threshold: Threshold for chi2 when EvalRobust is called
   /// robustFraction: Fraction of data that will be used in EvalRobust
   /// err: error of the data points
   /// if fitParArr and/or fitCovArr is given, write fitParameters and/or covariance Matrices into the array
   /// padType shows whether full oroc or splited oroc is taken into account

   AliTPCCalPad* pad = new AliTPCCalPad(padName, padName);
   TVectorD fitParam(0);
   TMatrixD covMatrix(0,0);
   Float_t chi2 = 0;
   for (Int_t isec = 0; isec < 72; isec++){
     if(isec>=36 && padType!=AliTPCCalROC::kAll){
       //part for medium pads
       if (PadOutliers)
         GetCalROC(isec)->GlobalFit(PadOutliers->GetCalROC(isec), robust, fitParam, covMatrix, chi2, fitType, chi2Threshold, robustFraction, err, AliTPCCalROC::kOROCmedium);
       else 
         GetCalROC(isec)->GlobalFit(0, robust, fitParam, covMatrix, chi2, fitType, chi2Threshold, robustFraction, err, AliTPCCalROC::kOROCmedium);
       
       AliTPCCalROC *roc=AliTPCCalROC::CreateGlobalFitCalROC(fitParam, isec, AliTPCCalROC::kOROCmedium);
       if ( fitParArr ) fitParArr->AddAtAndExpand(new TVectorD(fitParam), isec);
       if ( fitCovArr ) fitCovArr->AddAtAndExpand(new TMatrixD(covMatrix), isec);
       //part for long pads
       if (PadOutliers)
         GetCalROC(isec)->GlobalFit(PadOutliers->GetCalROC(isec), robust, fitParam, covMatrix, chi2, fitType, chi2Threshold, robustFraction, err, AliTPCCalROC::kOROClong);
       else 
         GetCalROC(isec)->GlobalFit(0, robust, fitParam, covMatrix, chi2, fitType, chi2Threshold, robustFraction, err, AliTPCCalROC::kOROClong);
       
       roc=AliTPCCalROC::CreateGlobalFitCalROC(fitParam, isec, AliTPCCalROC::kOROClong,roc);
       if ( fitParArr ) fitParArr->AddAtAndExpand(new TVectorD(fitParam), isec+36);
       if ( fitCovArr ) fitCovArr->AddAtAndExpand(new TMatrixD(covMatrix), isec+36);

       pad->SetCalROC(roc);
       delete roc;
     } else {
       if (PadOutliers)
         GetCalROC(isec)->GlobalFit(PadOutliers->GetCalROC(isec), robust, fitParam, covMatrix, chi2, fitType, chi2Threshold, robustFraction, err);
       else 
         GetCalROC(isec)->GlobalFit(0, robust, fitParam, covMatrix, chi2, fitType, chi2Threshold, robustFraction, err);
       
       AliTPCCalROC *roc=AliTPCCalROC::CreateGlobalFitCalROC(fitParam, isec);
       pad->SetCalROC(roc);
       delete roc;
       if ( fitParArr ) fitParArr->AddAtAndExpand(new TVectorD(fitParam), isec);
       if ( fitCovArr ) fitCovArr->AddAtAndExpand(new TMatrixD(covMatrix), isec);
     }
   }//end of loop over sectors

   return pad;
}
//_____________________________________________________________________________
TObjArray* AliTPCCalPad::CreateFormulaArray(const char *fitFormula)
{
  /// create an array of TFormulas for the each parameter of the fit function

  // split fit string in single parameters
  // find dimension of the fit:
  TString fitString(fitFormula);
  fitString.ReplaceAll("++","#");
  fitString.ReplaceAll(" ","");
  TObjArray *arrFitParams = fitString.Tokenize("#");
  Int_t ndim = arrFitParams->GetEntries();
  //create array of TFormulas to evaluate the parameters
  TObjArray *arrFitFormulas = new TObjArray(ndim);
  arrFitFormulas->SetOwner(kTRUE);
  for (Int_t idim=0;idim<ndim;++idim){
    TString s=((TObjString*)arrFitParams->At(idim))->GetString();
    s.ReplaceAll("gx","[0]");
    s.ReplaceAll("gy","[1]");
    s.ReplaceAll("lx","[2]");
    s.ReplaceAll("ly","[3]");
    s.ReplaceAll("sector","[4]");
    arrFitFormulas->AddAt(new TFormula(Form("param%02d",idim),s.Data()),idim);
  }
  delete arrFitParams;
  
  return arrFitFormulas;
}
//_____________________________________________________________________________
void AliTPCCalPad::EvalFormulaArray(const TObjArray &arrFitFormulas, TVectorD &results,
                                    const Int_t sec, const Int_t row, const Int_t pad)
{
  /// evaluate the fit formulas

  Int_t ndim=arrFitFormulas.GetEntries();
  results.ResizeTo(ndim);
  
  AliTPCROC* tpcROCinstance = AliTPCROC::Instance();  // to calculate the pad's position
  Float_t localXYZ[3];
  Float_t globalXYZ[3];
  tpcROCinstance->GetPositionLocal(sec, row, pad, localXYZ);
  tpcROCinstance->GetPositionGlobal(sec, row, pad, globalXYZ);
  //calculate parameter values
  for (Int_t idim=0;idim<ndim;++idim){
    TFormula *f=(TFormula*)arrFitFormulas.At(idim);
    f->SetParameters(globalXYZ[0],globalXYZ[1],localXYZ[0],localXYZ[1],sec);
    results[idim]=f->Eval(0);
  }
}
//_____________________________________________________________________________
void AliTPCCalPad::GlobalSidesFit(const AliTPCCalPad* PadOutliers, const char* fitFormula, TVectorD &fitParamSideA, TVectorD &fitParamSideC,TMatrixD &covMatrixSideA, TMatrixD &covMatrixSideC, Float_t & chi2SideA, Float_t & chi2SideC, AliTPCCalPad *pointError, Bool_t robust, Double_t robustFraction){
  /// Performs a fit on both sides.
  /// Valid information for the fitFormula are the variables
  /// - gx, gy, lx ,ly: meaning global x, global y, local x, local y value of the padName
  /// - sector:         the sector number.
  ///  eg. a formula might look 'gy' or '(sector<36) ++ gy' or 'gx ++ gy' or 'gx ++ gy ++ lx ++ lx^2' and so on
  ///
  /// PadOutliers - pads with value !=0 are not used in fitting procedure
  /// chi2Threshold: Threshold for chi2 when EvalRobust is called
  /// robustFraction: Fraction of data that will be used in EvalRobust

  TObjArray* arrFitFormulas=CreateFormulaArray(fitFormula);
  Int_t ndim = arrFitFormulas->GetEntries();
  //resize output data arrays
  fitParamSideA.ResizeTo(ndim+1);
  fitParamSideC.ResizeTo(ndim+1);
  covMatrixSideA.ResizeTo(ndim+1,ndim+1);
  covMatrixSideC.ResizeTo(ndim+1,ndim+1);
  // create linear fitter for A- and C- Side
  TLinearFitter* fitterGA = new TLinearFitter(ndim+1,Form("hyp%d",ndim));
  TLinearFitter* fitterGC = new TLinearFitter(ndim+1,Form("hyp%d",ndim));
  fitterGA->StoreData(kTRUE);
  fitterGC->StoreData(kTRUE);
  //parameter values
  TVectorD parValues(ndim);

  AliTPCCalROC *rocErr=0x0;
  
  for (UInt_t isec = 0; isec<kNsec; ++isec){
    AliTPCCalROC *rocOut=PadOutliers->GetCalROC(isec);
    AliTPCCalROC *rocData=GetCalROC(isec);
    if (pointError) rocErr=pointError->GetCalROC(isec);
    if (!rocData) continue;
    for (UInt_t irow = 0; irow < GetCalROC(isec)->GetNrows(); irow++) {
      for (UInt_t ipad = 0; ipad < GetCalROC(isec)->GetNPads(irow); ipad++) {
        //check for outliers
        if (rocOut && rocOut->GetValue(irow,ipad)) continue;
        //calculate parameter values
        EvalFormulaArray(*arrFitFormulas,parValues,isec,irow,ipad);
        //get value
        Float_t value=rocData->GetValue(irow,ipad);
        //point error
        Int_t err=1;
        if (rocErr) {
          err=TMath::Nint(rocErr->GetValue(irow,ipad));
          if (err==0) err=1;
        }
        //add points to the fitters
        if (isec/18%2==0){
          fitterGA->AddPoint(parValues.GetMatrixArray(),value,err);
        }else{
          fitterGC->AddPoint(parValues.GetMatrixArray(),value,err);
        }
      }
    }
  }
  if (robust){
    fitterGA->EvalRobust(robustFraction);
    fitterGC->EvalRobust(robustFraction);
  } else {
    fitterGA->Eval();
    fitterGC->Eval();
  }
  chi2SideA=fitterGA->GetChisquare()/(fitterGA->GetNpoints()-(ndim+1));
  chi2SideC=fitterGC->GetChisquare()/(fitterGC->GetNpoints()-(ndim+1));
  fitterGA->GetParameters(fitParamSideA);
  fitterGC->GetParameters(fitParamSideC);
  fitterGA->GetCovarianceMatrix(covMatrixSideA);
  fitterGC->GetCovarianceMatrix(covMatrixSideC);
  
  delete arrFitFormulas;
  delete fitterGA;
  delete fitterGC;
  
}
//
AliTPCCalPad *AliTPCCalPad::CreateCalPadFit(const char* fitFormula, const TVectorD &fitParamSideA, const TVectorD &fitParamSideC)
{
  ///

  TObjArray *arrFitFormulas=CreateFormulaArray(fitFormula);
  Int_t ndim = arrFitFormulas->GetEntries();
  //check if dimension of fit formula and fit parameters agree
  if (ndim!=fitParamSideA.GetNrows()||ndim!=fitParamSideC.GetNrows()){
    printf("AliTPCCalPad::CreateCalPadFit: Dimensions of fit formula and fit Parameters does not match!");
    return 0;
  }
  //create cal pad
  AliTPCCalPad *pad=new AliTPCCalPad("fitResultPad",Form("Fit result: %s",fitFormula));
  //fill cal pad with fit results if requested
  for (UInt_t isec = 0; isec<kNsec; ++isec){
    AliTPCCalROC *roc=pad->GetCalROC(isec);
    for (UInt_t irow = 0; irow < roc->GetNrows(); irow++) {
      for (UInt_t ipad = 0; ipad < roc->GetNPads(irow); ipad++) {
        const TVectorD *fitPar=0;
        TVectorD fitResArray;
        if (isec/18%2==0){
          fitPar=&fitParamSideA;
        }else{
          fitPar=&fitParamSideC;
        }
        EvalFormulaArray(*arrFitFormulas,fitResArray, isec, irow, ipad);
        for (Int_t idim=0;idim<ndim;++idim)
          fitResArray(idim)*=(*fitPar)(idim);
        roc->SetValue(irow,ipad,fitResArray.Sum());
      }
    }
  }
  delete arrFitFormulas;
  return pad;
}



TCanvas * AliTPCCalPad::MakeReportPadSector(TTree *chain, const char* varName, const char*varTitle, const char *axisTitle, Float_t min, Float_t max, const char *cutUser){
  /// Make a report - cal pads per sector
  /// mean valeus per sector and local X

  TH1* his=0;
  TLegend *legend = 0;
  TCanvas *canvas = new TCanvas(Form("Sector: %s",varTitle),Form("Sector: %s",varTitle),1500,1100);

  canvas->Divide(2);
  chain->SetAlias("lX","lx.fElements"); 
  //
  canvas->cd(1);
  TString strDraw=varName;
  strDraw+=":lX";
  legend = new TLegend(0.5,0.50,0.9,0.9, Form("%s TPC A side", varTitle));
  for (Int_t isec=-1; isec<18; isec+=1){
    TCut cutSec=Form("sector%%36==%d",isec);
    cutSec+=cutUser;
    if (isec==-1) cutSec="sector%36<18";
    chain->SetMarkerColor(1+(isec+2)%5);
    chain->SetLineColor(1+(isec+2)%5);
    chain->SetMarkerStyle(25+(isec+2)%4);
    //
    chain->Draw(strDraw.Data(),cutSec,"profgoff");
    his=(TH1*)chain->GetHistogram()->Clone();
    delete chain->GetHistogram();
    his->SetMaximum(max);
    his->SetMinimum(min);
    his->GetXaxis()->SetTitle("R (cm)");
    his->GetYaxis()->SetTitle(axisTitle);
    his->SetTitle(Form("%s- sector %d",varTitle, isec));
    his->SetName(Form("%s- sector %d",varTitle, isec));
    if (isec==-1) his->SetTitle(Form("%s A side",varTitle));
    if (isec==-1) his->Draw();
    his->Draw("same");
    legend->AddEntry(his);
  }
  legend->Draw();
  canvas->cd(2);
  //
  legend = new TLegend(0.5,0.50,0.9,0.9, Form("%s TPC C side", varTitle));
  for (Int_t isec=-1; isec<18; isec+=1){
    TCut cutSec=Form("(sector+18)%%36==%d",isec);
    cutSec+=cutUser;
    if (isec==-1) cutSec="sector%36>18";
    chain->SetMarkerColor(1+(isec+2)%5);
    chain->SetLineColor(1+(isec+2)%5);
    chain->SetMarkerStyle(25+isec%4);
    //
    chain->Draw(strDraw.Data(),cutSec,"profgoff");
    his=(TH1*)chain->GetHistogram()->Clone();
    delete chain->GetHistogram();
    his->SetMaximum(max);
    his->SetMinimum(min);
    his->GetXaxis()->SetTitle("R (cm)");
    his->GetYaxis()->SetTitle(axisTitle);
    his->SetTitle(Form("%s- sector %d",varTitle,isec));
    his->SetName(Form("%s- sector %d",varTitle,isec));
    if (isec==-1) his->SetTitle(Form("%s C side",varTitle));
    if (isec==-1) his->Draw();
    his->Draw("same");
    legend->AddEntry(his);
  }
  legend->Draw();
  //
  //
  return canvas;
}


TCanvas * AliTPCCalPad::MakeReportPadSector2D(TTree *chain, const char* varName, const char*varTitle, const char *axisTitle, Float_t min, Float_t max, const char *cutUser){
  /// Make a report - cal pads per sector
  /// 2D view
  /// Input tree should be created using AliPreprocesorOnline before

  TH1* his=0;
  TCanvas *canvas = new TCanvas(Form("%s2D",varTitle),Form("%s2D",varTitle),1500,1100);
  canvas->Divide(2);
  //
  TString strDraw=varName;
  strDraw+=":gy.fElements:gx.fElements>>his(250,-250,250,250,-250,250)";
  //
  TVirtualPad * pad=0;
  pad=canvas->cd(1);
  pad->SetMargin(0.15,0.15,0.15,0.15);
  TCut cut=cutUser;
  chain->Draw(strDraw.Data(),"sector%36<18"+cut,"profgoffcolz2");
  his=(TH1*)chain->GetHistogram()->Clone();
  delete chain->GetHistogram();
  his->SetMaximum(max);
  his->SetMinimum(min);
  his->GetXaxis()->SetTitle("x (cm)");
  his->GetYaxis()->SetTitle("y (cm)");
  his->GetZaxis()->SetTitle(axisTitle);
  his->SetTitle(Form("%s A side",varTitle));
  his->SetName(Form("%s A side",varTitle));
  his->Draw("colz2");
  //
  pad=canvas->cd(2);
  pad->SetMargin(0.15,0.15,0.15,0.15);

  chain->Draw(strDraw.Data(),"sector%36>=18"+cut,"profgoffcolz2");
  his=(TH1*)chain->GetHistogram()->Clone();
  delete chain->GetHistogram();
  his->SetMaximum(max);
  his->SetMinimum(min);
  his->GetXaxis()->SetTitle("x (cm)");
  his->GetYaxis()->SetTitle("y (cm)");
  his->GetZaxis()->SetTitle(axisTitle);
  his->SetTitle(Form("%s C side",varTitle));
  his->SetName(Form("%s C side",varTitle));
  his->Draw("colz2");
  //
  //
  return canvas;
}

void  AliTPCCalPad::Draw(Option_t* option){
  /// Draw function - standard 2D view

  TH1* his=0;
  TCanvas *canvas = new TCanvas(Form("%s2D",GetTitle()),Form("%s2D",GetTitle()),900,900);
  canvas->Divide(2,2);
  //
  //
  TVirtualPad * pad=0;
  pad=canvas->cd(1);
  pad->SetMargin(0.15,0.15,0.15,0.15);
  his=MakeHisto2D(0);
  his->GetXaxis()->SetTitle("x (cm)");
  his->GetYaxis()->SetTitle("y (cm)");
  his->GetZaxis()->SetTitle(GetTitle());
  his->SetTitle(Form("%s A side",GetTitle()));
  his->SetName(Form("%s A side",GetTitle()));
  his->Draw(option);
  //
  pad=canvas->cd(2);
  pad->SetMargin(0.15,0.15,0.15,0.15);
  his=MakeHisto2D(1);
  his->GetXaxis()->SetTitle("x (cm)");
  his->GetYaxis()->SetTitle("y (cm)");
  his->GetZaxis()->SetTitle(GetTitle());
  his->SetTitle(Form("%s C side",GetTitle()));
  his->SetName(Form("%s C side",GetTitle()));
  his->Draw(option);
  //
  pad=canvas->cd(3);
  pad->SetMargin(0.15,0.15,0.15,0.15);
  his=MakeHisto1D(-8,8,0,1);
  his->GetXaxis()->SetTitle(GetTitle());
  his->SetTitle(Form("%s A side",GetTitle()));
  his->SetName(Form("%s A side",GetTitle()));
  his->Draw("err");
  //
  pad=canvas->cd(4);
  pad->SetMargin(0.15,0.15,0.15,0.15);
  his=MakeHisto1D(-8,8,0,-1);
  his->GetXaxis()->SetTitle(GetTitle());
  his->SetTitle(Form("%s C side",GetTitle()));
  his->SetName(Form("%s C side",GetTitle()));
  his->Draw("err");


}


AliTPCCalPad * AliTPCCalPad::MakeCalPadFromHistoRPHI(TH2 * hisA, TH2* hisC){
  /// Make cal pad from r-phi histograms

  AliTPCROC *proc= AliTPCROC::Instance();
  AliTPCCalPad *calPad = new AliTPCCalPad("his","his");
  Float_t globalPos[3];
  for (Int_t isec=0; isec<72; isec++){
    AliTPCCalROC* calRoc  = calPad->GetCalROC(isec);
    TH2 * his = ((isec%36<18) ? hisA:hisC);
    for (UInt_t irow=0; irow<calRoc->GetNrows(); irow+=1){
      Int_t jrow=irow;
      if (isec>=36) jrow+=63;
      for (UInt_t ipad=0;ipad<proc->GetNPads(isec,irow);ipad+=1){
        proc->GetPositionGlobal(isec,irow,ipad, globalPos);
        Double_t phi=TMath::ATan2(globalPos[1],globalPos[0]);
        //if (phi<0) phi+=TMath::Pi()*2;
        Int_t bin=his->FindBin(phi,jrow);
        Float_t value= his->GetBinContent(bin);
	calRoc->SetValue(irow,ipad,value);
      }
    }
  }
  return calPad;
}

AliTPCCalPad *AliTPCCalPad::MakePadFromTree(TTree * treePad, const char *query, const char* name, Bool_t doFast){
  /// make cal pad from the tree

  if (!treePad){
    ::Error("AliTPCCalPad::MakePadFromTree(TTree * treePad, const char *query, const char* name)","Input tree is missing");
    return 0;
  }
  if (treePad->GetEntries()!=kNsec) return 0;
  AliTPCCalPad * calPad= new AliTPCCalPad(name,name);
  if (name) calPad->SetName(name);
  if (!doFast){
    for (Int_t iSec=0; iSec<72; iSec++){
      AliTPCCalROC* calROC  = calPad->GetCalROC(iSec);
      UInt_t nchannels = (UInt_t)treePad->Draw(query,"1","goff",1,iSec);
      if (nchannels!=calROC->GetNchannels()) {
	::Error("AliTPCCalPad::MakePad","%s\t:Wrong query sector\t%d\t%d",treePad->GetName(),iSec,nchannels);
	break;
      }
      for (UInt_t index=0; index<nchannels; index++) calROC->SetValue(index,treePad->GetV1()[index]);
    }
  }else{    
    UInt_t nchannelsTree = (UInt_t)treePad->Draw(query,"1","goff");
    UInt_t nchannelsAll=0;
    for (Int_t iSec=0; iSec<72; iSec++){
      AliTPCCalROC* calROC  = calPad->GetCalROC(iSec);
      UInt_t nchannels=calROC->GetNchannels();
      for (UInt_t index=0; index<nchannels; index++) {
	if (nchannelsAll<=nchannelsTree)calROC->SetValue(index,treePad->GetV1()[nchannelsAll]);
	nchannelsAll++;
      }
    }
    if (nchannelsAll>nchannelsTree){
      ::Error("AliTPCCalPad::MakePad","%s\t:Wrong query: cout mismatch\t%d\t%d",query, nchannelsAll,nchannelsTree);
    }
  }
  return calPad;
}

void AliTPCCalPad::AddFriend(TTree * treePad, const char *friendName, const char *fname){
  ///

  TObjArray *fArray = new TObjArray(1);
  fArray->AddLast(this);
  this->SetName(friendName);
  AliTPCCalibViewer::MakeTree(fname, fArray,0);
  TFile * f = TFile::Open(fname);
  TTree * tree = (TTree*)f->Get("calPads");
  treePad->AddFriend(tree,friendName);
  //  tree->AddFriend(TString::Format("%s = calPads",friendName).Data(),fname);
}

//_____________________________________________________________________________
void AliTPCCalPad::DumpUnitTestTrees(TString fileName/*=""*/)
{
  /// Dump a unit test tree with most derived variables
  /// If filename is empty, then the "<ClassName>_UnitTest.root" is used

  if (fileName.IsNull()) fileName=TString::Format("%s_UnitTest.root",IsA()->GetName());

  //===========================================================================
  // ===| CalPad fits |========================================================
  // --- create fit cal pads
  AliTPCCalPad *localFit         = LocalFit ("localFit",5,5);
  AliTPCCalPad *globalFit        = GlobalFit("globalFit");
  AliTPCCalPad *globalFitRegions = GlobalFit("globalFitRegions",0,kFALSE,1,5,0.7,1,0x0,0x0,AliTPCCalROC::kOROCmedium);

  TObjArray arrFits(3);
  arrFits.Add(localFit);
  arrFits.Add(globalFit);
  arrFits.Add(globalFitRegions);
  arrFits.SetOwner();

  // --- dump to tree
  AliTPCCalibViewer::MakeTree(fileName, &arrFits);

  //===========================================================================
  // ===| single variables     |===============================================

  TTreeSRedirector stream(fileName,"UPDATE");

  // ---| set up all variables |-----------------------------------------------
  Double_t mean       = GetMean();
  Double_t rms        = GetRMS();
  Double_t median     = GetMedian();
  Double_t minElement = GetMinElement();
  Double_t maxElement = GetMaxElement();
  Double_t ltmSigma   = 0.;
  Double_t ltm        = GetLTM(&ltmSigma);

  // ---| Dump to Tree |-------------------------------------------------------
  stream << "vars" <<
  "mean="       << mean        <<
  "rms="        << rms         <<
  "median="     << median      <<
  "minElement=" << minElement  <<
  "maxElement=" << maxElement  <<
  "ltmSigma="   << ltmSigma    <<
  "ltm="        << ltm         <<
  "\n";


}
