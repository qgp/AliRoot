
/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Øystein Djuvsland <oysteind@ift.uib.no>                       *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/



#include "AliHLTPHOSPhysicsAnalyzerPeakFitter.h"
#include "TMath.h"
#include "TF1.h"
#include "TFile.h"


ClassImp(AliHLTPHOSPhysicsAnalyzerPeakFitter);

AliHLTPHOSPhysicsAnalyzerPeakFitter::AliHLTPHOSPhysicsAnalyzerPeakFitter() : fGainLow(80), fGainHigh(5),
									     fRootHistPtr(0)
{
  //Constructor
}

AliHLTPHOSPhysicsAnalyzerPeakFitter::~AliHLTPHOSPhysicsAnalyzerPeakFitter()
{
  //Destructor
}

AliHLTPHOSPhysicsAnalyzerPeakFitter::AliHLTPHOSPhysicsAnalyzerPeakFitter(const AliHLTPHOSPhysicsAnalyzerPeakFitter&): fGainLow(80), fGainHigh(5),
														      fRootHistPtr(0)
{
  //Copy constructor
}


Int_t
AliHLTPHOSPhysicsAnalyzerPeakFitter::FitGaussian()
{
  //FitGaussian

  Int_t maxBin = fRootHistPtr->GetMaximumBin();
  Float_t binWidth = fRootHistPtr->GetBinWidth(maxBin);
  Float_t maxBinValue = (Float_t)(maxBin * binWidth);
  Float_t lowRange = maxBinValue - 0.03;
  Float_t highRange = maxBinValue + 0.03;

  TF1* gaussian = new TF1("gaussian", "gaus", 0.1, 0.2);
    
  fRootHistPtr->Fit(gaussian->GetName(), "", "",lowRange, highRange);
  
  return 0;

}

Int_t
AliHLTPHOSPhysicsAnalyzerPeakFitter::FitLorentzian()
{
  //FitLorentzian
  Int_t maxBin = fRootHistPtr->GetMaximumBin();
  Float_t binWidth = fRootHistPtr->GetBinWidth(maxBin);
  Float_t maxBinValue = (Float_t)(maxBin * binWidth);
  Double_t lowRange = maxBinValue - 0.03;
  Double_t highRange = maxBinValue + 0.03;

  char* name = "lorentzian";
  
  TF1* lorentzian = new TF1(name, "([0]*1/TMath::Pi())*[1]/((x[0]-[2])*(x[0]-[2])+[1]*[1])", lowRange, highRange);

  Double_t params[3] = {fRootHistPtr->GetBinContent(maxBin)/20, 0.01, 0.135};
  lorentzian->SetParameters(params);

  fRootHistPtr->Fit(lorentzian->GetName(), "", "", lowRange, highRange);

  lorentzian->GetParameters(params);

  TFile *outfile = new TFile("/afsuser/odjuvsland","recreate");  
  fRootHistPtr->Write();
  outfile->Close();

  return 0;
}

