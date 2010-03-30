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

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TRD MCM (Multi Chip Module) simulator                                    //
//  which simulates the TRAP processing after the AD-conversion.             //
//  The relevant parameters (i.e. configuration settings of the TRAP)        //
//  are taken from AliTRDtrapConfig.                                         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TLine.h"
#include "TRandom.h"
#include "TClonesArray.h"

#include "AliLog.h"
#include "AliRunLoader.h"
#include "AliLoader.h"

#include "AliTRDfeeParam.h"
#include "AliTRDtrapConfig.h"
#include "AliTRDdigitsManager.h"
#include "AliTRDarrayADC.h"
#include "AliTRDarrayDictionary.h"
#include "AliTRDtrackletMCM.h"
#include "AliTRDmcmSim.h"

ClassImp(AliTRDmcmSim)

Bool_t AliTRDmcmSim::fgApplyCut = kTRUE;
Int_t  AliTRDmcmSim::fgAddBaseline = 0;

const Int_t AliTRDmcmSim::fgkFormatIndex = std::ios_base::xalloc(); 

const Int_t AliTRDmcmSim::fgkNADC = AliTRDfeeParam::GetNadcMcm();
const UShort_t AliTRDmcmSim::fgkFPshifts[4] = {11, 14, 17, 21}; 


AliTRDmcmSim::AliTRDmcmSim() : 
  TObject(),
  fInitialized(kFALSE),
  fDetector(-1),
  fRobPos(-1),
  fMcmPos(-1),
  fRow (-1),
  fNTimeBin(-1),
  fADCR(NULL),
  fADCF(NULL),
  fMCMT(NULL),
  fTrackletArray(NULL),
  fZSMap(NULL),
  fFeeParam(NULL),
  fTrapConfig(NULL),
  fDigitsManager(NULL),
  fPedAcc(NULL),
  fGainCounterA(NULL),
  fGainCounterB(NULL),
  fTailAmplLong(NULL),
  fTailAmplShort(NULL),
  fNHits(0),
  fFitReg(NULL)
{
  //
  // AliTRDmcmSim default constructor
  // By default, nothing is initialized.
  // It is necessary to issue Init before use.
}

AliTRDmcmSim::~AliTRDmcmSim() 
{
  //
  // AliTRDmcmSim destructor
  //

  if(fInitialized) {
    for( Int_t iAdc = 0 ; iAdc < fgkNADC; iAdc++ ) {
      delete [] fADCR[iAdc];
      delete [] fADCF[iAdc];
    }
    delete [] fADCR;
    delete [] fADCF;
    delete [] fZSMap;
    delete [] fMCMT;
 
    delete [] fPedAcc;
    delete [] fGainCounterA;
    delete [] fGainCounterB;
    delete [] fTailAmplLong;
    delete [] fTailAmplShort;
    delete [] fFitReg;
    
    fTrackletArray->Delete();
    delete fTrackletArray;
  }
}

void AliTRDmcmSim::Init( Int_t det, Int_t robPos, Int_t mcmPos, Bool_t /* newEvent */ ) 
{
  //
  // Initialize the class with new MCM position information
  // memory is allocated in the first initialization
  //
   
  if (!fInitialized) {
    fFeeParam      = AliTRDfeeParam::Instance();
    fTrapConfig    = AliTRDtrapConfig::Instance();
  }

  fDetector      = det;
  fRobPos        = robPos;
  fMcmPos        = mcmPos;
  fNTimeBin      = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kC13CPUA);
  fRow           = fFeeParam->GetPadRowFromMCM( fRobPos, fMcmPos );
  
  if (!fInitialized) {
    fADCR    = new Int_t *[fgkNADC];
    fADCF    = new Int_t *[fgkNADC];
    fZSMap   = new Int_t  [fgkNADC];
    fGainCounterA = new UInt_t[fgkNADC];
    fGainCounterB = new UInt_t[fgkNADC];
    for( Int_t iAdc = 0 ; iAdc < fgkNADC; iAdc++ ) {
      fADCR[iAdc] = new Int_t[fNTimeBin];
      fADCF[iAdc] = new Int_t[fNTimeBin];
    }
    
    // filter registers
    fPedAcc = new UInt_t[fgkNADC]; // accumulator for pedestal filter
    fTailAmplLong = new UShort_t[fgkNADC];
    fTailAmplShort = new UShort_t[fgkNADC];
    
    // tracklet calculation
    fFitReg = new FitReg_t[fgkNADC]; 
    fTrackletArray = new TClonesArray("AliTRDtrackletMCM", fgkMaxTracklets);
    
    fMCMT = new UInt_t[fgkMaxTracklets];
  }

  fInitialized = kTRUE;

  Reset();
}

void AliTRDmcmSim::Reset()
{
  // Resets the data values and internal filter registers
  // by re-initialising them

  if( !CheckInitialized() ) 
    return;

  for( Int_t iAdc = 0 ; iAdc < fgkNADC; iAdc++ ) {
    for( Int_t it = 0 ; it < fNTimeBin ; it++ ) {
      fADCR[iAdc][it] = 0;
      fADCF[iAdc][it] = 0;
    }
    fZSMap[iAdc] = -1;      // Default unread, low active bit mask
    fGainCounterA[iAdc] = 0;
    fGainCounterB[iAdc] = 0;
  }
  
  for(Int_t i = 0; i < fgkMaxTracklets; i++) {
    fMCMT[i] = 0;
  }

  for (Int_t iDict = 0; iDict < 3; iDict++)
    fDict[iDict] = 0x0;
  
  FilterPedestalInit();
  FilterGainInit();
  FilterTailInit();
}

void AliTRDmcmSim::SetNTimebins(Int_t ntimebins) 
{
  // Reallocate memory if a change in the number of timebins 
  // is needed (should not be the case for real data)

  if( !CheckInitialized() ) 
    return;

  fNTimeBin = ntimebins;
  for( Int_t iAdc = 0 ; iAdc < fgkNADC; iAdc++ ) {
    delete fADCR[iAdc];
    delete fADCF[iAdc];
    fADCR[iAdc] = new Int_t[fNTimeBin];
    fADCF[iAdc] = new Int_t[fNTimeBin];
  }
}

Bool_t AliTRDmcmSim::LoadMCM(AliRunLoader* const runloader, Int_t det, Int_t rob, Int_t mcm) 
{
  // loads the ADC data as obtained from the digitsManager for the specified MCM.
  // This method is meant for rare execution, e.g. in the visualization. When called 
  // frequently use SetData(...) instead. 

  Init(det, rob, mcm);

  if (!runloader) {
    AliError("No Runloader given");
    return kFALSE;
  }

  AliLoader *trdLoader = runloader->GetLoader("TRDLoader");
  if (!trdLoader) {
    AliError("Could not get TRDLoader");
    return kFALSE;
  }

  Bool_t retval = kTRUE;
  trdLoader->LoadDigits();
  fDigitsManager = 0x0;
  AliTRDdigitsManager *digMgr = new AliTRDdigitsManager();
  digMgr->SetSDigits(0);
  digMgr->CreateArrays();
  digMgr->ReadDigits(trdLoader->TreeD());
  AliTRDarrayADC *digits = (AliTRDarrayADC*) digMgr->GetDigits(det);
  if (digits->HasData()) {
    digits->Expand();

    if (fNTimeBin != digits->GetNtime()) {
      AliWarning(Form("Changing no. of timebins from %i to %i", fNTimeBin, digits->GetNtime()));
      SetNTimebins(digits->GetNtime());
    }

    SetData(digits);
  }
  else 
    retval = kFALSE;
  
  delete digMgr;
  
  return retval;
}

void AliTRDmcmSim::NoiseTest(Int_t nsamples, Int_t mean, Int_t sigma, Int_t inputGain, Int_t inputTail)
{
  // This function can be used to test the filters. 
  // It feeds nsamples of ADC values with a gaussian distribution specified by mean and sigma.
  // The filter chain implemented here consists of:
  // Pedestal -> Gain -> Tail
  // With inputGain and inputTail the input to the gain and tail filter, respectively, 
  // can be chosen where 
  // 0: noise input
  // 1: pedestal output
  // 2: gain output
  // The input has to be chosen from a stage before. 
  // The filter behaviour is controlled by the TRAP parameters from AliTRDtrapConfig in the 
  // same way as in normal simulation.
  // The functions produces four histograms with the values at the different stages.

  if( !CheckInitialized() ) 
    return;

  TString nameInputGain;
  TString nameInputTail; 

  switch (inputGain) {
      case 0:
        nameInputGain = "Noise";
        break;

      case 1:
        nameInputGain = "Pedestal";
        break;

      default:
        AliError("Undefined input to tail cancellation filter");
        return;
  }

  switch (inputTail) {
      case 0:
        nameInputTail = "Noise";
        break;

      case 1:
        nameInputTail = "Pedestal";
        break;

      case 2:
        nameInputTail = "Gain";
        break;

      default:
        AliError("Undefined input to tail cancellation filter");
        return;
  }

  TH1F *h   = new TH1F("noise", "Gaussian Noise;sample;ADC count",
                       nsamples, 0, nsamples);
  TH1F *hfp = new TH1F("ped", "Noise #rightarrow Pedestal filter;sample;ADC count", nsamples, 0, nsamples);
  TH1F *hfg = new TH1F("gain", 
                       (nameInputGain + "#rightarrow Gain;sample;ADC count").Data(), 
                       nsamples, 0, nsamples);
  TH1F *hft = new TH1F("tail", 
                       (nameInputTail + "#rightarrow Tail;sample;ADC count").Data(), 
                       nsamples, 0, nsamples);
  h->SetStats(kFALSE);
  hfp->SetStats(kFALSE);
  hfg->SetStats(kFALSE);
  hft->SetStats(kFALSE);
  
  Int_t value;  // ADC count with noise (10 bit)
  Int_t valuep; // pedestal filter output (12 bit)
  Int_t valueg; // gain filter output (12 bit)
  Int_t valuet; // tail filter value (12 bit)
  
  for (Int_t i = 0; i < nsamples; i++) {
    value = (Int_t) gRandom->Gaus(mean, sigma);  // generate noise with gaussian distribution 
    h->SetBinContent(i, value);

    valuep = FilterPedestalNextSample(1, 0, ((Int_t) value) << 2);
    
    if (inputGain == 0)
      valueg = FilterGainNextSample(1, ((Int_t) value) << 2);
    else 
      valueg = FilterGainNextSample(1, valuep); 
    
    if (inputTail == 0)
      valuet = FilterTailNextSample(1, ((Int_t) value) << 2);
    else if (inputTail == 1)
      valuet = FilterTailNextSample(1, valuep); 
    else
      valuet = FilterTailNextSample(1, valueg); 

    hfp->SetBinContent(i, valuep >> 2);
    hfg->SetBinContent(i, valueg >> 2);
    hft->SetBinContent(i, valuet >> 2);
  }

  TCanvas *c = new TCanvas; 
  c->Divide(2,2);
  c->cd(1);
  h->Draw();
  c->cd(2);
  hfp->Draw();
  c->cd(3);
  hfg->Draw();
  c->cd(4);
  hft->Draw();
}

Bool_t AliTRDmcmSim::CheckInitialized() const
{
  //
  // Check whether object is initialized
  //

  if( ! fInitialized ) 
    AliError(Form ("AliTRDmcmSim is not initialized but function other than Init() is called."));

  return fInitialized;
}

void AliTRDmcmSim::Print(Option_t* const option) const
{
  // Prints the data stored and/or calculated for this MCM.
  // The output is controlled by option which can be a sequence of any of 
  // the following characters:
  // R - prints raw ADC data
  // F - prints filtered data 
  // H - prints detected hits
  // T - prints found tracklets
  // The later stages are only meaningful after the corresponding calculations 
  // have been performed.

  if ( !CheckInitialized() ) 
    return;

  printf("MCM %i on ROB %i in detector %i\n", fMcmPos, fRobPos, fDetector);

  TString opt = option;
  if (opt.Contains("R") || opt.Contains("F")) {
    std::cout << *this;
  }

  if (opt.Contains("H")) {
    printf("Found %i hits:\n", fNHits);
    for (Int_t iHit = 0; iHit < fNHits; iHit++) {
      printf("Hit %3i in timebin %2i, ADC %2i has charge %3i and position %3i\n",
             iHit,  fHits[iHit].fTimebin, fHits[iHit].fChannel, fHits[iHit].fQtot, fHits[iHit].fYpos);
    }
  }

  if (opt.Contains("T")) {
    printf("Tracklets:\n");
    for (Int_t iTrkl = 0; iTrkl < fTrackletArray->GetEntriesFast(); iTrkl++) {
      printf("tracklet %i: 0x%08x\n", iTrkl, ((AliTRDtrackletMCM*) (*fTrackletArray)[iTrkl])->GetTrackletWord());
    }
  }
}

void AliTRDmcmSim::Draw(Option_t* const option) 
{
  // Plots the data stored in a 2-dim. timebin vs. ADC channel plot.
  // The option selects what data is plotted and can be a sequence of 
  // the following characters:
  // R - plot raw data (default)
  // F - plot filtered data (meaningless if R is specified)
  // In addition to the ADC values:
  // H - plot hits 
  // T - plot tracklets

  if( !CheckInitialized() ) 
    return;

  TString opt = option;

  TH2F *hist = new TH2F("mcmdata", Form("Data of MCM %i on ROB %i in detector %i", \
                                        fMcmPos, fRobPos, fDetector), \
                        fgkNADC, -0.5, fgkNADC-.5, fNTimeBin, -.5, fNTimeBin-.5);
  hist->GetXaxis()->SetTitle("ADC Channel");
  hist->GetYaxis()->SetTitle("Timebin");
  hist->SetStats(kFALSE);

  if (opt.Contains("R")) {
    for (Int_t iTimeBin = 0; iTimeBin < fNTimeBin; iTimeBin++) {
      for (Int_t iAdc = 0; iAdc < fgkNADC; iAdc++) {
        hist->SetBinContent(iAdc+1, iTimeBin+1, fADCR[iAdc][iTimeBin] >> fgkAddDigits);
      }
    }
  }
  else {
    for (Int_t iTimeBin = 0; iTimeBin < fNTimeBin; iTimeBin++) {
      for (Int_t iAdc = 0; iAdc < fgkNADC; iAdc++) {
        hist->SetBinContent(iAdc+1, iTimeBin+1, fADCF[iAdc][iTimeBin] >> fgkAddDigits);
      }
    }
  }
  hist->Draw("colz");

  if (opt.Contains("H")) {
    TGraph *grHits = new TGraph();
    for (Int_t iHit = 0; iHit < fNHits; iHit++) {
      grHits->SetPoint(iHit, 
                       fHits[iHit].fChannel + 1 + fHits[iHit].fYpos/256., 
                       fHits[iHit].fTimebin);
    }
    grHits->Draw("*");
  }

  if (opt.Contains("T")) {
    TLine *trklLines = new TLine[4];
    for (Int_t iTrkl = 0; iTrkl < fTrackletArray->GetEntries(); iTrkl++) {
      AliTRDtrackletMCM *trkl = (AliTRDtrackletMCM*) (*fTrackletArray)[iTrkl];
      Float_t padWidth = 0.635 + 0.03 * (fDetector % 6);
      Float_t offset   = padWidth/256. * ((((((fRobPos & 0x1) << 2) + (fMcmPos & 0x3)) * 18) << 8) - ((18*4*2 - 18*2 - 3) << 7)); // revert adding offset in FitTracklet
      Int_t   ndrift   = fTrapConfig->GetDmem(0xc025, fDetector, fRobPos, fMcmPos) >> 5; 
      Float_t slope    = trkl->GetdY() * 140e-4 / ndrift; 

      Int_t t0 = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPFS);
      Int_t t1 = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPFE);

      trklLines[iTrkl].SetX1((offset - (trkl->GetY() - slope * t0)) / padWidth); // ??? sign?
      trklLines[iTrkl].SetY1(t0);
      trklLines[iTrkl].SetX2((offset - (trkl->GetY() - slope * t1)) / padWidth); // ??? sign?
      trklLines[iTrkl].SetY2(t1);
      trklLines[iTrkl].SetLineColor(2);
      trklLines[iTrkl].SetLineWidth(2);
      printf("Tracklet %i: y = %f, dy = %f, offset = %f\n", iTrkl, trkl->GetY(), (trkl->GetdY() * 140e-4), offset);
      trklLines[iTrkl].Draw();
    }
  }
}

void AliTRDmcmSim::SetData( Int_t adc, Int_t* const data )
{
  //
  // Store ADC data into array of raw data
  //

  if( !CheckInitialized() ) return;

  if( adc < 0 || adc >= fgkNADC ) {
    AliError(Form ("Error: ADC %i is out of range (0 .. %d).", adc, fgkNADC-1));
    return;
  }

  for( Int_t it = 0 ;  it < fNTimeBin ; it++ ) {
    fADCR[adc][it] = (Int_t) (data[it]) << fgkAddDigits;
    fADCF[adc][it] = (Int_t) (data[it]) << fgkAddDigits;
  }
}

void AliTRDmcmSim::SetData( Int_t adc, Int_t it, Int_t data )
{
  //
  // Store ADC data into array of raw data
  //

  if( !CheckInitialized() ) return;

  if( adc < 0 || adc >= fgkNADC ) {
    AliError(Form ("Error: ADC %i is out of range (0 .. %d).", adc, fgkNADC-1));
    return;
  }

  fADCR[adc][it] = data << fgkAddDigits;
  fADCF[adc][it] = data << fgkAddDigits;
}

void AliTRDmcmSim::SetData(AliTRDarrayADC* const adcArray, AliTRDdigitsManager *digitsManager)
{
  // Set the ADC data from an AliTRDarrayADC

  if( !CheckInitialized() ) 
    return;

  fDigitsManager = digitsManager;
  if (fDigitsManager) {
    for (Int_t iDict = 0; iDict < 3; iDict++) {
      AliTRDarrayDictionary *newDict = (AliTRDarrayDictionary*) fDigitsManager->GetDictionary(fDetector, iDict);
      if (fDict[iDict] != 0x0 && newDict != 0x0) {
        
        if (fDict[iDict] == newDict)
          continue;

        fDict[iDict] = newDict;
        
        if (fDict[iDict]->GetDim() == 0) {
          AliError(Form("Dictionary %i of det. %i has dim. 0", fDetector, iDict));
          continue;
        }
        fDict[iDict]->Expand(); 
      }
      else {
        fDict[iDict] = newDict;
        if (fDict[iDict])
          fDict[iDict]->Expand();
      }
    }
  }

  if (fNTimeBin != adcArray->GetNtime())
    SetNTimebins(adcArray->GetNtime());
  
  Int_t offset = (fMcmPos % 4 + 1) * 21 + (fRobPos % 2) * 84 - 1;

  for (Int_t iTimeBin = 0; iTimeBin < fNTimeBin; iTimeBin++) {
    for (Int_t iAdc = 0; iAdc < fgkNADC; iAdc++) {
      Int_t value = adcArray->GetDataByAdcCol(GetRow(), offset - iAdc, iTimeBin);
      if (value < 0 || (offset - iAdc < 1) || (offset - iAdc > 165)) {
        fADCR[iAdc][iTimeBin] = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFPNP) + (fgAddBaseline << fgkAddDigits);
        fADCF[iAdc][iTimeBin] = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPFP) + (fgAddBaseline << fgkAddDigits);
      }
      else {
        fZSMap[iAdc] = 0;
        fADCR[iAdc][iTimeBin] = (value << fgkAddDigits) + (fgAddBaseline << fgkAddDigits);
        fADCF[iAdc][iTimeBin] = (value << fgkAddDigits) + (fgAddBaseline << fgkAddDigits);
      }
    }
  }
}

void AliTRDmcmSim::SetDataPedestal( Int_t adc )
{
  //
  // Store ADC data into array of raw data
  //

  if( !CheckInitialized() ) 
    return;

  if( adc < 0 || adc >= fgkNADC ) {
    return;
  }

  for( Int_t it = 0 ; it < fNTimeBin ; it++ ) {
    fADCR[adc][it] = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFPNP) + (fgAddBaseline << fgkAddDigits);
    fADCF[adc][it] = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPFP) + (fgAddBaseline << fgkAddDigits);
  }
}

Bool_t AliTRDmcmSim::GetHit(Int_t index, Int_t &channel, Int_t &timebin, Int_t &qtot, Int_t &ypos, Float_t &y, Int_t &label) const
{
  // retrieve the MC hit information (not available in TRAP hardware)

  if (index < 0 || index >= fNHits)
    return kFALSE;
  
  channel = fHits[index].fChannel;
  timebin = fHits[index].fTimebin;
  qtot    = fHits[index].fQtot;
  ypos    = fHits[index].fYpos;
  y       = (Float_t) ((((((fRobPos & 0x1) << 2) + (fMcmPos & 0x3)) * 18) << 8) - ((18*4*2 - 18*2 - 1) << 7) -
                        (channel << 8) - ypos) 
    * (0.635 + 0.03 * (fDetector % 6))
    / 256.0;
  label   = fHits[index].fLabel;

  return kTRUE;
}

Int_t AliTRDmcmSim::GetCol( Int_t adc )
{
  //
  // Return column id of the pad for the given ADC channel
  //

  if( !CheckInitialized() ) 
    return -1;

  Int_t col = fFeeParam->GetPadColFromADC(fRobPos, fMcmPos, adc);
  if (col < 0 || col >= fFeeParam->GetNcol()) 
    return -1;
  else 
    return col;
}

Int_t AliTRDmcmSim::ProduceRawStream( UInt_t *buf, Int_t bufSize, UInt_t iEv) const
{
  //
  // Produce raw data stream from this MCM and put in buf
  // Returns number of words filled, or negative value 
  // with -1 * number of overflowed words
  //

  if( !CheckInitialized() ) 
    return 0;

  UInt_t  x;
  Int_t   nw  = 0;  // Number of written words
  Int_t   of  = 0;  // Number of overflowed words
  Int_t   rawVer   = fFeeParam->GetRAWversion();
  Int_t **adc;
  Int_t   nActiveADC = 0;	// number of activated ADC bits in a word

  if( !CheckInitialized() ) 
    return 0;

  if (fTrapConfig->GetTrapReg(AliTRDtrapConfig::kEBSF) != 0) // store unfiltered data
    adc = fADCR;
  else 
    adc = fADCF;
  
  // Produce MCM header
  x = (1<<31) | (fRobPos << 28) | (fMcmPos << 24) | ((iEv % 0x100000) << 4) | 0xC;

  if (nw < bufSize) {
    buf[nw++] = x;
  }
  else {
    of++;
  }

  // Produce ADC mask : nncc cccm mmmm mmmm mmmm mmmm mmmm 1100
  // 				n : unused , c : ADC count, m : selected ADCs
  if( rawVer >= 3 ) {
    x = 0;
    for( Int_t iAdc = 0 ; iAdc < fgkNADC ; iAdc++ ) {
      if( ~fZSMap[iAdc] != 0 ) { //  0 means not suppressed
		x = x | (1 << (iAdc+4) );	// last 4 digit reserved for 1100=0xc
		nActiveADC++;		// number of 1 in mmm....m
      }
    }
	x = x | (1 << 30) | ( ( 0x3FFFFFFC ) & (~(nActiveADC) << 25) ) | 0xC;	// nn = 01, ccccc are inverted, 0xc=1100

    if (nw < bufSize) {
      buf[nw++] = x;
    }
    else {
      of++;
    }
  }

  // Produce ADC data. 3 timebins are packed into one 32 bits word
  // In this version, different ADC channel will NOT share the same word

  UInt_t aa=0, a1=0, a2=0, a3=0;

  for (Int_t iAdc = 0; iAdc < 21; iAdc++ ) {
    if( rawVer>= 3 && ~fZSMap[iAdc] == 0 ) continue; // Zero Suppression, 0 means not suppressed
    aa = !(iAdc & 1) + 2;
    for (Int_t iT = 0; iT < fNTimeBin; iT+=3 ) {
      a1 = ((iT    ) < fNTimeBin ) ? adc[iAdc][iT  ] >> fgkAddDigits : 0;
      a2 = ((iT + 1) < fNTimeBin ) ? adc[iAdc][iT+1] >> fgkAddDigits : 0;
      a3 = ((iT + 2) < fNTimeBin ) ? adc[iAdc][iT+2] >> fgkAddDigits : 0;
      x = (a3 << 22) | (a2 << 12) | (a1 << 2) | aa;
      if (nw < bufSize) {
        buf[nw++] = x;
      }
      else {
        of++;
      }
    }
  }

  if( of != 0 ) return -of; else return nw;
}

Int_t AliTRDmcmSim::ProduceTrackletStream( UInt_t *buf, Int_t bufSize )
{
  //
  // Produce tracklet data stream from this MCM and put in buf
  // Returns number of words filled, or negative value 
  // with -1 * number of overflowed words
  //

  if( !CheckInitialized() ) 
    return 0;

  Int_t   nw  = 0;  // Number of written words
  Int_t   of  = 0;  // Number of overflowed words
    
  // Produce tracklet data. A maximum of four 32 Bit words will be written per MCM 
  // fMCMT is filled continuously until no more tracklet words available

  for (Int_t iTracklet = 0; iTracklet < fTrackletArray->GetEntriesFast(); iTracklet++) {
    if (nw < bufSize) 
      buf[nw++] = ((AliTRDtrackletMCM*) (*fTrackletArray)[iTracklet])->GetTrackletWord();
    else 
      of++;
  }
  
  if( of != 0 ) return -of; else return nw;
}

void AliTRDmcmSim::Filter()
{
  //
  // Filter the raw ADC values. The active filter stages and their
  // parameters are taken from AliTRDtrapConfig.
  // The raw data is stored separate from the filtered data. Thus, 
  // it is possible to run the filters on a set of raw values 
  // sequentially for parameter tuning.
  //

  if( !CheckInitialized() ) 
    return;

  // Apply filters sequentially. Bypass is handled by filters
  // since counters and internal registers may be updated even 
  // if the filter is bypassed.
  // The first filter takes the data from fADCR and 
  // outputs to fADCF. 
  
  // Non-linearity filter not implemented.
  FilterPedestal();
  FilterGain();
  FilterTail();
  // Crosstalk filter not implemented.
}

void AliTRDmcmSim::FilterPedestalInit(Int_t baseline) 
{
  // Initializes the pedestal filter assuming that the input has 
  // been constant for a long time (compared to the time constant).

  UShort_t    fptc = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFPTC); // 0..3, 0 - fastest, 3 - slowest

  for (Int_t iAdc = 0; iAdc < fgkNADC; iAdc++)
    fPedAcc[iAdc] = (baseline << 2) * (1 << fgkFPshifts[fptc]); 
}

UShort_t AliTRDmcmSim::FilterPedestalNextSample(Int_t adc, Int_t timebin, UShort_t value)
{
  // Returns the output of the pedestal filter given the input value.
  // The output depends on the internal registers and, thus, the 
  // history of the filter.

  UShort_t    fpnp = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFPNP); // 0..511 -> 0..127.75, pedestal at the output
  UShort_t    fptc = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFPTC); // 0..3, 0 - fastest, 3 - slowest
  UShort_t    fpby = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFPBY); // 0..1 bypass, active low

  UShort_t accumulatorShifted;
  Int_t correction;
  UShort_t inpAdd;
  
  inpAdd = value + fpnp;

  accumulatorShifted = (fPedAcc[adc] >> fgkFPshifts[fptc]) & 0x3FF;   // 10 bits
  if (timebin == 0) // the accumulator is disabled in the drift time
  {
    correction = (value & 0x3FF) - accumulatorShifted;
    fPedAcc[adc] = (fPedAcc[adc] + correction) & 0x7FFFFFFF;             // 31 bits
  }
  
  if (fpby == 0)
    return value;

  if (inpAdd <= accumulatorShifted)
    return 0;
  else
  {
    inpAdd = inpAdd - accumulatorShifted;
    if (inpAdd > 0xFFF) 
      return 0xFFF;
    else 
      return inpAdd;
  }
}

void AliTRDmcmSim::FilterPedestal()
{
  //
  // Apply pedestal filter
  //
  // As the first filter in the chain it reads data from fADCR 
  // and outputs to fADCF. 
  // It has only an effect if previous samples have been fed to 
  // find the pedestal. Currently, the simulation assumes that 
  // the input has been stable for a sufficiently long time.

  for (Int_t iTimeBin = 0; iTimeBin < fNTimeBin; iTimeBin++) {
    for (Int_t iAdc = 0; iAdc < fgkNADC; iAdc++) {
      fADCF[iAdc][iTimeBin] = FilterPedestalNextSample(iAdc, iTimeBin, fADCR[iAdc][iTimeBin]);
    }
  }
}

void AliTRDmcmSim::FilterGainInit()
{
  // Initializes the gain filter. In this case, only threshold 
  // counters are reset.

  for (Int_t iAdc = 0; iAdc < fgkNADC; iAdc++) {
    // these are counters which in hardware continue 
    // until maximum or reset
    fGainCounterA[iAdc] = 0;
    fGainCounterB[iAdc] = 0;
  }
}

UShort_t AliTRDmcmSim::FilterGainNextSample(Int_t adc, UShort_t value)
{
  // Apply the gain filter to the given value.
  // BEGIN_LATEX O_{i}(t) = #gamma_{i} * I_{i}(t) + a_{i} END_LATEX
  // The output depends on the internal registers and, thus, the 
  // history of the filter.

  UShort_t    fgby = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFGBY); // bypass, active low
  UShort_t    fgf  = fTrapConfig->GetTrapReg(AliTRDtrapConfig::TrapReg_t(AliTRDtrapConfig::kFGF0 + adc)); // 0x700 + (0 & 0x1ff);
  UShort_t    fga  = fTrapConfig->GetTrapReg(AliTRDtrapConfig::TrapReg_t(AliTRDtrapConfig::kFGA0 + adc)); // 40;
  UShort_t    fgta = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFGTA); // 20;
  UShort_t    fgtb = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFGTB); // 2060;

  UInt_t corr; // corrected value

  value &= 0xFFF;
  corr = (value * fgf) >> 11;
  corr = corr > 0xfff ? 0xfff : corr;
  corr = AddUintClipping(corr, fga, 12);

  // Update threshold counters 
  // not really useful as they are cleared with every new event
  if (!((fGainCounterA[adc] == 0x3FFFFFF) || (fGainCounterB[adc] == 0x3FFFFFF)))
  // stop when full
  {
    if (corr >= fgtb) 
      fGainCounterB[adc]++;
    else if (corr >= fgta) 
      fGainCounterA[adc]++;
  }

  if (fgby == 1)
    return corr; 
  else
    return value;
}

void AliTRDmcmSim::FilterGain()
{
  // Read data from fADCF and apply gain filter.

  for (Int_t iAdc = 0; iAdc < fgkNADC; iAdc++) {
    for (Int_t iTimeBin = 0; iTimeBin < fNTimeBin; iTimeBin++) {
        fADCF[iAdc][iTimeBin] = FilterGainNextSample(iAdc, fADCF[iAdc][iTimeBin]);
    }
  }
}

void AliTRDmcmSim::FilterTailInit(Int_t baseline)
{
  // Initializes the tail filter assuming that the input has 
  // been at the baseline value (configured by FTFP) for a 
  // sufficiently long time.

  // exponents and weight calculated from configuration
  UShort_t    alphaLong = 0x3ff & fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFTAL); // the weight of the long component
  UShort_t    lambdaLong = (1 << 10) | (1 << 9) | (fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFTLL) & 0x1FF); // the multiplier
  UShort_t    lambdaShort = (0 << 10) | (1 << 9) | (fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFTLS) & 0x1FF); // the multiplier

  Float_t lambdaL = lambdaLong  * 1.0 / (1 << 11);
  Float_t lambdaS = lambdaShort * 1.0 / (1 << 11);
  Float_t alphaL  = alphaLong   * 1.0 / (1 << 11);
  Float_t qup, qdn;
  qup = (1 - lambdaL) * (1 - lambdaS);
  qdn = 1 - lambdaS * alphaL - lambdaL * (1 - alphaL);
  Float_t kdc = qup/qdn;

  Float_t kt, ql, qs;
  UShort_t aout;

  if (baseline < 0)
    baseline = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFPNP);
  
  ql = lambdaL * (1 - lambdaS) *      alphaL;
  qs = lambdaS * (1 - lambdaL) * (1 - alphaL);

  for (Int_t iAdc = 0; iAdc < fgkNADC; iAdc++) {
    Int_t value = baseline & 0xFFF;
    Int_t corr = (value * fTrapConfig->GetTrapReg(AliTRDtrapConfig::TrapReg_t(AliTRDtrapConfig::kFGF0 + iAdc))) >> 11;
    corr = corr > 0xfff ? 0xfff : corr;
    corr = AddUintClipping(corr, fTrapConfig->GetTrapReg(AliTRDtrapConfig::TrapReg_t(AliTRDtrapConfig::kFGA0 + iAdc)), 12);

    kt = kdc * baseline;
    aout = baseline - (UShort_t) kt;

    fTailAmplLong[iAdc]  = (UShort_t) (aout * ql / (ql + qs));
    fTailAmplShort[iAdc] = (UShort_t) (aout * qs / (ql + qs));
  }
}

UShort_t AliTRDmcmSim::FilterTailNextSample(Int_t adc, UShort_t value)
{
  // Returns the output of the tail filter for the given input value. 
  // The output depends on the internal registers and, thus, the 
  // history of the filter.

  // exponents and weight calculated from configuration
  UShort_t    alphaLong   = 0x3ff & fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFTAL);                          // the weight of the long component
  UShort_t    lambdaLong  = (1 << 10) | (1 << 9) | (fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFTLL) & 0x1FF); // the multiplier of the long component
  UShort_t    lambdaShort = (0 << 10) | (1 << 9) | (fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFTLS) & 0x1FF); // the multiplier of the short component

  // intermediate signals
  UInt_t   aDiff;
  UInt_t   alInpv;
  UShort_t aQ;
  UInt_t   tmp;
  
  UShort_t inpVolt = value & 0xFFF;    // 12 bits
      
  // add the present generator outputs
  aQ = AddUintClipping(fTailAmplLong[adc], fTailAmplShort[adc], 12);

  // calculate the difference between the input and the generated signal
  if (inpVolt > aQ) 
    aDiff = inpVolt - aQ;
  else                
    aDiff = 0;
  
  // the inputs to the two generators, weighted
  alInpv = (aDiff * alphaLong) >> 11;
  
  // the new values of the registers, used next time
  // long component
  tmp = AddUintClipping(fTailAmplLong[adc], alInpv, 12);
  tmp =  (tmp * lambdaLong) >> 11;
  fTailAmplLong[adc] = tmp & 0xFFF;
  // short component
  tmp = AddUintClipping(fTailAmplShort[adc], aDiff - alInpv, 12);
  tmp =  (tmp * lambdaShort) >> 11;
  fTailAmplShort[adc] = tmp & 0xFFF;
  
  // the output of the filter
  if (fTrapConfig->GetTrapReg(AliTRDtrapConfig::kFTBY) == 0) // bypass mode, active low
    return value;
  else
    return aDiff;
}

void AliTRDmcmSim::FilterTail()
{
  // Apply tail cancellation filter to all data. 

  for (Int_t iTimeBin = 0; iTimeBin < fNTimeBin; iTimeBin++) {
    for (Int_t iAdc = 0; iAdc < fgkNADC; iAdc++) {
      fADCF[iAdc][iTimeBin] = FilterTailNextSample(iAdc, fADCF[iAdc][iTimeBin]);
    }
  }
}

void AliTRDmcmSim::ZSMapping()
{
  //
  // Zero Suppression Mapping implemented in TRAP chip
  // only implemented for up to 30 timebins
  //
  // See detail TRAP manual "Data Indication" section:
  // http://www.kip.uni-heidelberg.de/ti/TRD/doc/trap/TRAP-UserManual.pdf
  //

  if( !CheckInitialized() ) 
    return;

  Int_t eBIS = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kEBIS); 
  Int_t eBIT = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kEBIT); 
  Int_t eBIL = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kEBIL); 
  Int_t eBIN = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kEBIN); 

  Int_t **adc = fADCF;

  for (Int_t iAdc = 0; iAdc < fgkNADC; iAdc++) 
    fZSMap[iAdc] = -1;

  for( Int_t it = 0 ; it < fNTimeBin ; it++ ) {
    Int_t iAdc; // current ADC channel
    Int_t ap;
    Int_t ac;
    Int_t an;
    Int_t mask;
    Int_t supp; // suppression of the current channel (low active)
    
    // ----- first channel -----
    iAdc = 0;
    
    ap = 0;               // previous
    ac = adc[iAdc  ][it]; // current
    an = adc[iAdc+1][it]; // next
    
    mask  = ( ac >=  ap && ac >=  an ) ? 0 : 0x1; // peak center detection
    mask += ( ap + ac + an > eBIT )    ? 0 : 0x2; // cluster
    mask += ( ac > eBIS )              ? 0 : 0x4; // absolute large peak
    
    supp = (eBIL >> mask) & 1;
    
    fZSMap[iAdc] &= ~((1-supp) << it);
    if( eBIN == 0 ) {  // neighbour sensitivity
      fZSMap[iAdc+1] &= ~((1-supp) << it);
    }
    
    // ----- last channel -----
    iAdc = fgkNADC - 1;
    
    ap = adc[iAdc-1][it]; // previous
    ac = adc[iAdc  ][it]; // current
    an = 0;               // next
    
    mask  = ( ac >=  ap && ac >=  an ) ? 0 : 0x1; // peak center detection
    mask += ( ap + ac + an > eBIT )    ? 0 : 0x2; // cluster
    mask += ( ac > eBIS )              ? 0 : 0x4; // absolute large peak
    
    supp = (eBIL >> mask) & 1;
    
    fZSMap[iAdc] &= ~((1-supp) << it);
    if( eBIN == 0 ) {  // neighbour sensitivity
      fZSMap[iAdc-1] &= ~((1-supp) << it);
    }
    
    // ----- middle channels -----
    for( iAdc = 1 ; iAdc < fgkNADC-1; iAdc++ ) {
      ap = adc[iAdc-1][it]; // previous
      ac = adc[iAdc  ][it]; // current
      an = adc[iAdc+1][it]; // next
      
      mask  = ( ac >=  ap && ac >=  an ) ? 0 : 0x1; // peak center detection
      mask += ( ap + ac + an > eBIT )    ? 0 : 0x2; // cluster
      mask += ( ac > eBIS )              ? 0 : 0x4; // absolute large peak
      
      supp = (eBIL >> mask) & 1;
      
      fZSMap[iAdc] &= ~((1-supp) << it);
      if( eBIN == 0 ) {  // neighbour sensitivity
        fZSMap[iAdc-1] &= ~((1-supp) << it);
        fZSMap[iAdc+1] &= ~((1-supp) << it);
      }
    }

  }
}

void AliTRDmcmSim::AddHitToFitreg(Int_t adc, UShort_t timebin, UShort_t qtot, Short_t ypos, Int_t label) 
{
  // Add the given hit to the fit register which is lateron used for 
  // the tracklet calculation. 
  // In addition to the fit sums in the fit register MC information 
  // is stored.

  if ((timebin >= fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPQS0)) && 
      (timebin <  fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPQE0)))
    fFitReg[adc].fQ0 += qtot;
  
  if ((timebin >= fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPQS1)) && 
      (timebin <  fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPQE1)))
    fFitReg[adc].fQ1 += qtot;
  
  if ((timebin >= fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPFS) ) && 
      (timebin <  fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPFE)))
  {
    fFitReg[adc].fSumX  += timebin;
    fFitReg[adc].fSumX2 += timebin*timebin;
    fFitReg[adc].fNhits++;
    fFitReg[adc].fSumY  += ypos;
    fFitReg[adc].fSumY2 += ypos*ypos;
    fFitReg[adc].fSumXY += timebin*ypos;
  }

  // register hits (MC info)
  fHits[fNHits].fChannel = adc;
  fHits[fNHits].fQtot = qtot;
  fHits[fNHits].fYpos = ypos;
  fHits[fNHits].fTimebin = timebin;
  fHits[fNHits].fLabel = label;
  fNHits++;
}

void AliTRDmcmSim::CalcFitreg() 
{
  // Preprocessing.
  // Detect the hits and fill the fit registers.
  // Requires 12-bit data from fADCF which means Filter() 
  // has to be called before even if all filters are bypassed.

  //??? to be clarified:
  UInt_t adcMask = 0xffffffff;
  
  UShort_t timebin, adcch, adcLeft, adcCentral, adcRight, hitQual, timebin1, timebin2, qtotTemp;
  Short_t ypos, fromLeft, fromRight, found;
  UShort_t qTotal[19]; // the last is dummy
  UShort_t marked[6], qMarked[6], worse1, worse2;
  
  timebin1 = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPFS); 
  if (fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPQS0) 
      < timebin1)
    timebin1 = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPQS0);
  timebin2 = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPFE); 
  if (fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPQE1) 
      > timebin2)
    timebin2 = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPQE1);

  // reset the fit registers
  fNHits = 0; 
  for (adcch = 0; adcch < fgkNADC-2; adcch++) // due to border channels
  {
    fFitReg[adcch].fNhits = 0;
    fFitReg[adcch].fQ0    = 0;
    fFitReg[adcch].fQ1    = 0;
    fFitReg[adcch].fSumX  = 0;
    fFitReg[adcch].fSumY  = 0;
    fFitReg[adcch].fSumX2 = 0;
    fFitReg[adcch].fSumY2 = 0;
    fFitReg[adcch].fSumXY = 0;
  }
  
  for (timebin = timebin1; timebin < timebin2; timebin++)
  {
    // first find the hit candidates and store the total cluster charge in qTotal array
    // in case of not hit store 0 there.
    for (adcch = 0; adcch < fgkNADC-2; adcch++) {
      if ( ( (adcMask >> adcch) & 7) == 7) //??? all 3 channels are present in case of ZS
      {
        adcLeft  = fADCF[adcch  ][timebin];
        adcCentral  = fADCF[adcch+1][timebin];
        adcRight = fADCF[adcch+2][timebin];
        if (fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPVBY) == 1) 
          hitQual = ( (adcLeft * adcRight) < 
                       (fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPVT) * adcCentral) );
        else            
          hitQual = 1;
        // The accumulated charge is with the pedestal!!!
        qtotTemp = adcLeft + adcCentral + adcRight;
        if ( (hitQual) &&
             (qtotTemp >= fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPHT)) &&
             (adcLeft <= adcCentral) &&
             (adcCentral > adcRight) )
          qTotal[adcch] = qtotTemp;
        else
          qTotal[adcch] = 0;
      }
      else
        qTotal[adcch] = 0; //jkl
      if (qTotal[adcch] != 0) 
        AliDebug(10,Form("ch %2d   qTotal %5d",adcch, qTotal[adcch]));
    }

    fromLeft = -1;
    adcch = 0;
    found = 0;
    marked[4] = 19; // invalid channel
    marked[5] = 19; // invalid channel
    qTotal[19] = 0;
    while ((adcch < 16) && (found < 3))
    {
      if (qTotal[adcch] > 0)
      {
        fromLeft = adcch;
        marked[2*found+1]=adcch;
        found++;
      }
      adcch++;
    }
    
    fromRight = -1;
    adcch = 18;
    found = 0;
    while ((adcch > 2) && (found < 3))
    {
      if (qTotal[adcch] > 0)
      {
        marked[2*found]=adcch;
        found++;
        fromRight = adcch;
      }
      adcch--;
    }

    AliDebug(10,Form("Fromleft=%d, Fromright=%d",fromLeft, fromRight));
    // here mask the hit candidates in the middle, if any
    if ((fromLeft >= 0) && (fromRight >= 0) && (fromLeft < fromRight))
      for (adcch = fromLeft+1; adcch < fromRight; adcch++)
        qTotal[adcch] = 0;
    
    found = 0;
    for (adcch = 0; adcch < 19; adcch++)
      if (qTotal[adcch] > 0) found++;
    // NOT READY

    if (found > 4) // sorting like in the TRAP in case of 5 or 6 candidates!
    {
      if (marked[4] == marked[5]) marked[5] = 19;
      for (found=0; found<6; found++)
      {
        qMarked[found] = qTotal[marked[found]] >> 4;
        AliDebug(10,Form("ch_%d qTotal %d qTotals %d",marked[found],qTotal[marked[found]],qMarked[found]));
      }
      
      Sort6To2Worst(marked[0], marked[3], marked[4], marked[1], marked[2], marked[5],
                    qMarked[0],
                    qMarked[3],
                    qMarked[4],
                    qMarked[1],
                    qMarked[2],
                    qMarked[5],
                    &worse1, &worse2);
      // Now mask the two channels with the smallest charge
      if (worse1 < 19)
      {
        qTotal[worse1] = 0;
        AliDebug(10,Form("Kill ch %d\n",worse1));
      }
      if (worse2 < 19)
      {
        qTotal[worse2] = 0;
        AliDebug(10,Form("Kill ch %d\n",worse2));
      }
    }
    
    for (adcch = 0; adcch < 19; adcch++) {
      if (qTotal[adcch] > 0) // the channel is marked for processing
      {
        adcLeft  = fADCF[adcch  ][timebin];
        adcCentral  = fADCF[adcch+1][timebin];
        adcRight = fADCF[adcch+2][timebin];
        // hit detected, in TRAP we have 4 units and a hit-selection, here we proceed all channels!
        // subtract the pedestal TPFP, clipping instead of wrapping
        
        Int_t regTPFP = fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPFP);
        AliDebug(10, Form("Hit found, time=%d, adcch=%d/%d/%d, adc values=%d/%d/%d, regTPFP=%d, TPHT=%d\n",
               timebin, adcch, adcch+1, adcch+2, adcLeft, adcCentral, adcRight, regTPFP, 
               fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPHT)));

        if (adcLeft  < regTPFP) adcLeft  = 0; else adcLeft  -= regTPFP;
        if (adcCentral  < regTPFP) adcCentral  = 0; else adcCentral  -= regTPFP;
        if (adcRight < regTPFP) adcRight = 0; else adcRight -= regTPFP;

        // Calculate the center of gravity
        // checking for adcCentral != 0 (in case of "bad" configuration)
        if (adcCentral == 0)
          continue;
        ypos = 128*(adcLeft - adcRight) / adcCentral;
        if (ypos < 0) ypos = -ypos;
        // make the correction using the position LUT
        ypos = ypos + fTrapConfig->GetTrapReg((AliTRDtrapConfig::TrapReg_t) (AliTRDtrapConfig::kTPL00 + (ypos & 0x7F))); 
        if (adcLeft > adcRight) ypos = -ypos;

        // label calculation
        Int_t mcLabel = -1;
        if (fDigitsManager) {
          Int_t label[9] = { 0 }; // up to 9 different labels possible
          Int_t count[9] = { 0 };
          Int_t maxIdx = -1;
          Int_t maxCount = 0;
          Int_t nLabels = 0;
          Int_t padcol[3]; 
          padcol[0] = fFeeParam->GetPadColFromADC(fRobPos, fMcmPos, adcch);
          padcol[1] = fFeeParam->GetPadColFromADC(fRobPos, fMcmPos, adcch+1);
          padcol[2] = fFeeParam->GetPadColFromADC(fRobPos, fMcmPos, adcch+2);
          Int_t padrow = fFeeParam->GetPadRowFromMCM(fRobPos, fMcmPos);
          for (Int_t iDict = 0; iDict < 3; iDict++) {
            if (!fDict[iDict])
              continue;
            for (Int_t iPad = 0; iPad < 3; iPad++) {
              if (padcol[iPad] < 0) 
                continue;
              Int_t currLabel = fDict[iDict]->GetData(padrow, padcol[iPad], timebin); //fDigitsManager->GetTrack(iDict, padrow, padcol, timebin, fDetector);
	      AliDebug(10, Form("Read label: %4i for det: %3i, row: %i, col: %i, tb: %i\n", currLabel, fDetector, padrow, padcol[iPad], timebin));
              for (Int_t iLabel = 0; iLabel < nLabels; iLabel++) {
                if (currLabel == label[iLabel]) {
                  count[iLabel]++;
                  if (count[iLabel] > maxCount) {
                    maxCount = count[iLabel];
                    maxIdx = iLabel;
                  }
                  currLabel = 0;
                  break;
                }
              } 
              if (currLabel > 0) {
                label[nLabels++] = currLabel;
              }
            }
          }
          if (maxIdx >= 0)
            mcLabel = label[maxIdx];
        }

        // add the hit to the fitregister
        AddHitToFitreg(adcch, timebin, qTotal[adcch], ypos, mcLabel);
      }
    }
  }

  for (Int_t iAdc = 0; iAdc < fgkNADC; iAdc++) {
    if (fFitReg[iAdc].fNhits != 0) {
      AliDebug(2, Form("fitreg[%i]: nHits = %i, sumX = %i, sumY = %i, sumX2 = %i, sumY2 = %i, sumXY = %i", iAdc,
                       fFitReg[iAdc].fNhits,
                       fFitReg[iAdc].fSumX,
                       fFitReg[iAdc].fSumY,
                       fFitReg[iAdc].fSumX2,
                       fFitReg[iAdc].fSumY2,
                       fFitReg[iAdc].fSumXY
                 ));
    }
  }
}

void AliTRDmcmSim::TrackletSelection() 
{
  // Select up to 4 tracklet candidates from the fit registers  
  // and assign them to the CPUs.

  UShort_t adcIdx, i, j, ntracks, tmp;
  UShort_t trackletCand[18][2]; // store the adcch[0] and number of hits[1] for all tracklet candidates

  ntracks = 0;
  for (adcIdx = 0; adcIdx < 18; adcIdx++) // ADCs
    if ( (fFitReg[adcIdx].fNhits 
          >= fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPCL)) &&
         (fFitReg[adcIdx].fNhits+fFitReg[adcIdx+1].fNhits
          >= fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPCT)))
    {
      trackletCand[ntracks][0] = adcIdx;
      trackletCand[ntracks][1] = fFitReg[adcIdx].fNhits+fFitReg[adcIdx+1].fNhits;
      AliDebug(10,Form("%d  %2d %4d\n", ntracks, trackletCand[ntracks][0], trackletCand[ntracks][1]));
      ntracks++;
    };

  for (i=0; i<ntracks;i++) 
    AliDebug(10,Form("%d %d %d\n",i,trackletCand[i][0], trackletCand[i][1]));

  if (ntracks > 4)
  {
    // primitive sorting according to the number of hits
    for (j = 0; j < (ntracks-1); j++)
    {
      for (i = j+1; i < ntracks; i++)
      {
        if ( (trackletCand[j][1]  < trackletCand[i][1]) ||
             ( (trackletCand[j][1] == trackletCand[i][1]) && (trackletCand[j][0] < trackletCand[i][0]) ) )
        {
          // swap j & i
          tmp = trackletCand[j][1];
          trackletCand[j][1] = trackletCand[i][1];
          trackletCand[i][1] = tmp;
          tmp = trackletCand[j][0];
          trackletCand[j][0] = trackletCand[i][0];
          trackletCand[i][0] = tmp;
        }
      }
    }
    ntracks = 4; // cut the rest, 4 is the max
  }
  // else is not necessary to sort
  
  // now sort, so that the first tracklet going to CPU0 corresponds to the highest adc channel - as in the TRAP
  for (j = 0; j < (ntracks-1); j++)
  {
    for (i = j+1; i < ntracks; i++)
    {
      if (trackletCand[j][0] < trackletCand[i][0])
      {
        // swap j & i
        tmp = trackletCand[j][1];
        trackletCand[j][1] = trackletCand[i][1];
        trackletCand[i][1] = tmp;
        tmp = trackletCand[j][0];
        trackletCand[j][0] = trackletCand[i][0];
        trackletCand[i][0] = tmp;
      }
    }
  }
  for (i = 0; i < ntracks; i++)  // CPUs with tracklets.
    fFitPtr[i] = trackletCand[i][0]; // pointer to the left channel with tracklet for CPU[i]
  for (i = ntracks; i < 4; i++)  // CPUs without tracklets
    fFitPtr[i] = 31;            // pointer to the left channel with tracklet for CPU[i] = 31 (invalid)
  AliDebug(10,Form("found %i tracklet candidates\n", ntracks));
  for (i = 0; i < 4; i++)
    AliDebug(10,Form("fitPtr[%i]: %i\n", i, fFitPtr[i]));
}

void AliTRDmcmSim::FitTracklet()
{
  // Perform the actual tracklet fit based on the fit sums 
  // which have been filled in the fit registers. 

  // parameters in fitred.asm (fit program)
  Int_t decPlaces = 5;
  Int_t rndAdd = 0;
  if (decPlaces >  1) 
    rndAdd = (1 << (decPlaces-1)) + 1;
  else if (decPlaces == 1)
    rndAdd = 1;
  Int_t ndriftDp = 5;  // decimal places for drift time
  Long64_t shift = ((Long64_t) 1 << 32);

  // calculated in fitred.asm
  Int_t padrow = ((fRobPos >> 1) << 2) | (fMcmPos >> 2);
  Int_t yoffs = (((((fRobPos & 0x1) << 2) + (fMcmPos & 0x3)) * 18) << 8) - 
    ((18*4*2 - 18*2 - 1) << 7);
  yoffs = yoffs << decPlaces; // holds position of ADC channel 1
  Int_t layer = fDetector % 6;
  UInt_t scaleY = (UInt_t) ((0.635 + 0.03 * layer)/(256.0 * 160.0e-4) * shift);
  UInt_t scaleD = (UInt_t) ((0.635 + 0.03 * layer)/(256.0 * 140.0e-4) * shift);

  Int_t deflCorr = fTrapConfig->GetDmem(0xc022, fDetector, fRobPos, fMcmPos);
  Int_t ndrift   = fTrapConfig->GetDmem(0xc025, fDetector, fRobPos, fMcmPos); 

  // local variables for calculation
  Long64_t mult, temp, denom; //???
  UInt_t q0, q1, qTotal;          // charges in the two windows and total charge
  UShort_t nHits;                 // number of hits
  Int_t slope, offset;            // slope and offset of the tracklet
  Int_t sumX, sumY, sumXY, sumX2; // fit sums from fit registers
  //int32_t SumY2;                // not used in the current TRAP program
  FitReg_t *fit0, *fit1;          // pointers to relevant fit registers
  
//  const uint32_t OneDivN[32] = {  // 2**31/N : exactly like in the TRAP, the simple division here gives the same result!
//      0x00000000, 0x80000000, 0x40000000, 0x2AAAAAA0, 0x20000000, 0x19999990, 0x15555550, 0x12492490,
//      0x10000000, 0x0E38E380, 0x0CCCCCC0, 0x0BA2E8B0, 0x0AAAAAA0, 0x09D89D80, 0x09249240, 0x08888880,
//      0x08000000, 0x07878780, 0x071C71C0, 0x06BCA1A0, 0x06666660, 0x06186180, 0x05D17450, 0x0590B210,
//      0x05555550, 0x051EB850, 0x04EC4EC0, 0x04BDA120, 0x04924920, 0x0469EE50, 0x04444440, 0x04210840};

  for (Int_t cpu = 0; cpu < 4; cpu++) {
    if (fFitPtr[cpu] == 31)
    {
      fMCMT[cpu] = 0x10001000; //??? AliTRDfeeParam::GetTrackletEndmarker(); 
    }
    else
    {
      fit0 = &fFitReg[fFitPtr[cpu]  ];
      fit1 = &fFitReg[fFitPtr[cpu]+1]; // next channel

      mult = 1;
      mult = mult << (32 + decPlaces);
      mult = -mult;

      // Merging
      nHits   = fit0->fNhits + fit1->fNhits; // number of hits
      sumX    = fit0->fSumX  + fit1->fSumX;
      sumX2   = fit0->fSumX2 + fit1->fSumX2;
      denom   = nHits*sumX2 - sumX*sumX;

      mult    = mult / denom; // exactly like in the TRAP program
      q0      = fit0->fQ0    + fit1->fQ0;
      q1      = fit0->fQ1    + fit1->fQ1;
      sumY    = fit0->fSumY  + fit1->fSumY  + 256*fit1->fNhits;
      sumXY   = fit0->fSumXY + fit1->fSumXY + 256*fit1->fSumX;

      slope   = nHits*sumXY - sumX * sumY;
      offset  = sumX2*sumY  - sumX * sumXY;
      temp    = mult * slope;
      slope   = temp >> 32; // take the upper 32 bits
      slope   = -slope;
      temp    = mult * offset;
      offset  = temp >> 32; // take the upper 32 bits

      offset = offset + yoffs;
      AliDebug(10, Form("slope = %i, slope * ndrift = %i, deflCorr: %i", 
                       slope, slope * ndrift, deflCorr));
      slope  = ((slope * ndrift) >> ndriftDp) + deflCorr;
      offset = offset - (fFitPtr[cpu] << (8 + decPlaces));
      
      temp    = slope;
      temp    = temp * scaleD;
      slope   = (temp >> 32);
      temp    = offset;
      temp    = temp * scaleY;
      offset  = (temp >> 32);
        
      // rounding, like in the TRAP
      slope   = (slope  + rndAdd) >> decPlaces;
      offset  = (offset + rndAdd) >> decPlaces;

      AliDebug(5, Form("Det: %3i, ROB: %i, MCM: %2i: deflection: %i, min: %i, max: %i", 
                       fDetector, fRobPos, fMcmPos, slope, 
                       fTrapConfig->GetDmem(0xc030 + 2*fFitPtr[cpu], fDetector, fRobPos, fMcmPos), 
                       fTrapConfig->GetDmem(0xc031 + 2*fFitPtr[cpu], fDetector, fRobPos, fMcmPos)));

      Bool_t rejected = kFALSE;
      // deflection range table from DMEM
      if ((slope < fTrapConfig->GetDmem(0xc030 + 2*fFitPtr[cpu], fDetector, fRobPos, fMcmPos)) || 
          (slope > fTrapConfig->GetDmem(0xc031 + 2*fFitPtr[cpu], fDetector, fRobPos, fMcmPos)))
        rejected = kTRUE;

      if (rejected && GetApplyCut())
      {
        fMCMT[cpu] = 0x10001000; //??? AliTRDfeeParam::GetTrackletEndmarker();
      }
      else
      {
        if (slope > 63 || slope < -64) { // wrapping in TRAP!
          AliError(Form("Overflow in slope: %i, tracklet discarded!", slope));
          fMCMT[cpu] = 0x10001000;
          continue;
        }

        slope   = slope  &   0x7F; // 7 bit
        
        if (offset > 0xfff || offset < -0xfff) 
          AliWarning("Overflow in offset");
        offset  = offset & 0x1FFF; // 13 bit

	qTotal = 0; // set to zero as long as no reasonable PID calculation is available 
                    // before: GetPID(q0/length/fgChargeNorm, q1/length/fgChargeNorm);

        if (qTotal > 0xff)
          AliWarning("Overflow in charge");
        qTotal  = qTotal & 0xFF; // 8 bit, exactly like in the TRAP program
        
        // assemble and store the tracklet word
        fMCMT[cpu] = (qTotal << 24) | (padrow << 20) | (slope << 13) | offset;

        // calculate MC label
        Int_t mcLabel = -1;
	Int_t nHits0 = 0;
	Int_t nHits1 = 0;
        if (fDigitsManager) {
          Int_t label[30] = {0}; // up to 30 different labels possible
          Int_t count[30] = {0};
          Int_t maxIdx = -1;
          Int_t maxCount = 0;
          Int_t nLabels = 0;
          for (Int_t iHit = 0; iHit < fNHits; iHit++) {
            if ((fHits[iHit].fChannel - fFitPtr[cpu] < 0) ||
                (fHits[iHit].fChannel - fFitPtr[cpu] > 1))
              continue;

	    // counting contributing hits
	    if (fHits[iHit].fTimebin >= fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPQS0) &&
		fHits[iHit].fTimebin <  fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPQE0))
	      nHits0++;
	    if (fHits[iHit].fTimebin >= fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPQS1) &&
		fHits[iHit].fTimebin <  fTrapConfig->GetTrapReg(AliTRDtrapConfig::kTPQE1))
	      nHits1++;

            Int_t currLabel = fHits[iHit].fLabel;
            for (Int_t iLabel = 0; iLabel < nLabels; iLabel++) {
              if (currLabel == label[iLabel]) {
                count[iLabel]++;
                if (count[iLabel] > maxCount) {
                  maxCount = count[iLabel];
                  maxIdx = iLabel;
                }
                currLabel = 0;
                break;
              }
            }
            if (currLabel > 0) {
              label[nLabels++] = currLabel;
            }
          }
          if (maxIdx >= 0)
            mcLabel = label[maxIdx];
        }
        new ((*fTrackletArray)[fTrackletArray->GetEntriesFast()]) AliTRDtrackletMCM((UInt_t) fMCMT[cpu], fDetector*2 + fRobPos%2, fRobPos, fMcmPos);
        ((AliTRDtrackletMCM*) (*fTrackletArray)[fTrackletArray->GetEntriesFast()-1])->SetLabel(mcLabel);

       
        ((AliTRDtrackletMCM*) (*fTrackletArray)[fTrackletArray->GetEntriesFast()-1])->SetNHits(fit0->fNhits + fit1->fNhits);
	((AliTRDtrackletMCM*) (*fTrackletArray)[fTrackletArray->GetEntriesFast()-1])->SetNHits0(nHits0);
        ((AliTRDtrackletMCM*) (*fTrackletArray)[fTrackletArray->GetEntriesFast()-1])->SetNHits1(nHits1);
        ((AliTRDtrackletMCM*) (*fTrackletArray)[fTrackletArray->GetEntriesFast()-1])->SetQ0(q0);
        ((AliTRDtrackletMCM*) (*fTrackletArray)[fTrackletArray->GetEntriesFast()-1])->SetQ1(q1);
      }
    }
  }
}

void AliTRDmcmSim::Tracklet()
{
  // Run the tracklet calculation by calling sequentially:
  // CalcFitreg(); TrackletSelection(); FitTracklet()
  // and store the tracklets 

  if (!fInitialized) {
    AliError("Called uninitialized! Nothing done!");
    return;
  }

  fTrackletArray->Delete();

  CalcFitreg();
  if (fNHits == 0)
    return;
  TrackletSelection();
  FitTracklet();
}

Bool_t AliTRDmcmSim::StoreTracklets() 
{
  // store the found tracklets via the loader

  if (fTrackletArray->GetEntriesFast() == 0) 
    return kTRUE;

  AliRunLoader *rl = AliRunLoader::Instance();
  AliDataLoader *dl = 0x0;
  if (rl)
    dl = rl->GetLoader("TRDLoader")->GetDataLoader("tracklets");
  if (!dl) {
    AliError("Could not get the tracklets data loader!");
    return kFALSE;
  }

  TTree *trackletTree = dl->Tree();
  if (!trackletTree) {
    dl->MakeTree();
    trackletTree = dl->Tree();
  }
  
  AliTRDtrackletMCM *trkl = 0x0;
  TBranch *trkbranch = trackletTree->GetBranch("mcmtrklbranch");
  if (!trkbranch)
    trkbranch = trackletTree->Branch("mcmtrklbranch", "AliTRDtrackletMCM", &trkl, 32000);
  
  for (Int_t iTracklet = 0; iTracklet < fTrackletArray->GetEntriesFast(); iTracklet++) {
    trkl = ((AliTRDtrackletMCM*) (*fTrackletArray)[iTracklet]);
    trkbranch->SetAddress(&trkl);
    trkbranch->Fill();
  }

  return kTRUE;
}

void AliTRDmcmSim::WriteData(AliTRDarrayADC *digits)
{
  // write back the processed data configured by EBSF
  // EBSF = 1: unfiltered data; EBSF = 0: filtered data
  // zero-suppressed valued are written as -1 to digits

  if( !CheckInitialized() ) 
    return;

  Int_t offset = (fMcmPos % 4 + 1) * 21 + (fRobPos % 2) * 84 - 1;

  if (fTrapConfig->GetTrapReg(AliTRDtrapConfig::kEBSF) != 0) // store unfiltered data
  {
    for (Int_t iAdc = 0; iAdc < fgkNADC; iAdc++) {
      if (~fZSMap[iAdc] == 0) {
        for (Int_t iTimeBin = 0; iTimeBin < fNTimeBin; iTimeBin++) {
          digits->SetDataByAdcCol(GetRow(), offset - iAdc, iTimeBin, -1);
        }
      }
    }
  }
  else {
    for (Int_t iAdc = 0; iAdc < fgkNADC; iAdc++) {
      if (~fZSMap[iAdc] != 0) {
        for (Int_t iTimeBin = 0; iTimeBin < fNTimeBin; iTimeBin++) {
          digits->SetDataByAdcCol(GetRow(), offset - iAdc, iTimeBin, (fADCF[iAdc][iTimeBin] >> fgkAddDigits) - fgAddBaseline);
        }
      }
      else {
        for (Int_t iTimeBin = 0; iTimeBin < fNTimeBin; iTimeBin++) {
          digits->SetDataByAdcCol(GetRow(), offset - iAdc, iTimeBin, -1);
        }
      }
    }
  }
}

// help functions, to be cleaned up

UInt_t AliTRDmcmSim::AddUintClipping(UInt_t a, UInt_t b, UInt_t nbits) const
{
  // 
  // This function adds a and b (unsigned) and clips to 
  // the specified number of bits. 
  //  

  UInt_t sum = a + b;
  if (nbits < 32)
  {
    UInt_t maxv = (1 << nbits) - 1;;
    if (sum > maxv) 
      sum = maxv;
  }
  else
  {
    if ((sum < a) || (sum < b)) 
      sum = 0xFFFFFFFF;
  }
  return sum;
}

void AliTRDmcmSim::Sort2(UShort_t  idx1i, UShort_t  idx2i, \
                            UShort_t  val1i, UShort_t  val2i, \
                            UShort_t *idx1o, UShort_t *idx2o, \
                            UShort_t *val1o, UShort_t *val2o) const
{
  // sorting for tracklet selection

    if (val1i > val2i)
    {
        *idx1o = idx1i;
        *idx2o = idx2i;
        *val1o = val1i;
        *val2o = val2i;
    }
    else
    {
        *idx1o = idx2i;
        *idx2o = idx1i;
        *val1o = val2i;
        *val2o = val1i;
    }
}

void AliTRDmcmSim::Sort3(UShort_t  idx1i, UShort_t  idx2i, UShort_t  idx3i, \
                            UShort_t  val1i, UShort_t  val2i, UShort_t  val3i, \
                            UShort_t *idx1o, UShort_t *idx2o, UShort_t *idx3o, \
                            UShort_t *val1o, UShort_t *val2o, UShort_t *val3o)
{
  // sorting for tracklet selection

    Int_t sel;


    if (val1i > val2i) sel=4; else sel=0;
    if (val2i > val3i) sel=sel + 2;
    if (val3i > val1i) sel=sel + 1;
    switch(sel)
    {
        case 6 : // 1 >  2  >  3            => 1 2 3
        case 0 : // 1 =  2  =  3            => 1 2 3 : in this case doesn't matter, but so is in hardware!
            *idx1o = idx1i;
            *idx2o = idx2i;
            *idx3o = idx3i;
            *val1o = val1i;
            *val2o = val2i;
            *val3o = val3i;
            break;

        case 4 : // 1 >  2, 2 <= 3, 3 <= 1  => 1 3 2
            *idx1o = idx1i;
            *idx2o = idx3i;
            *idx3o = idx2i;
            *val1o = val1i;
            *val2o = val3i;
            *val3o = val2i;
            break;

        case 2 : // 1 <= 2, 2 > 3, 3 <= 1   => 2 1 3
            *idx1o = idx2i;
            *idx2o = idx1i;
            *idx3o = idx3i;
            *val1o = val2i;
            *val2o = val1i;
            *val3o = val3i;
            break;

        case 3 : // 1 <= 2, 2 > 3, 3  > 1   => 2 3 1
            *idx1o = idx2i;
            *idx2o = idx3i;
            *idx3o = idx1i;
            *val1o = val2i;
            *val2o = val3i;
            *val3o = val1i;
            break;

        case 1 : // 1 <= 2, 2 <= 3, 3 > 1   => 3 2 1
            *idx1o = idx3i;
            *idx2o = idx2i;
            *idx3o = idx1i;
            *val1o = val3i;
            *val2o = val2i;
            *val3o = val1i;
        break;

        case 5 : // 1 > 2, 2 <= 3, 3 >  1   => 3 1 2
            *idx1o = idx3i;
            *idx2o = idx1i;
            *idx3o = idx2i;
            *val1o = val3i;
            *val2o = val1i;
            *val3o = val2i;
        break;

        default: // the rest should NEVER happen!
            AliError("ERROR in Sort3!!!\n");
        break;
    }
}

void AliTRDmcmSim::Sort6To4(UShort_t  idx1i, UShort_t  idx2i, UShort_t  idx3i, UShort_t  idx4i, UShort_t  idx5i, UShort_t  idx6i, \
                               UShort_t  val1i, UShort_t  val2i, UShort_t  val3i, UShort_t  val4i, UShort_t  val5i, UShort_t  val6i, \
                               UShort_t *idx1o, UShort_t *idx2o, UShort_t *idx3o, UShort_t *idx4o, \
                               UShort_t *val1o, UShort_t *val2o, UShort_t *val3o, UShort_t *val4o)
{
  // sorting for tracklet selection

    UShort_t idx21s, idx22s, idx23s, dummy;
    UShort_t val21s, val22s, val23s;
    UShort_t idx23as, idx23bs;
    UShort_t val23as, val23bs;

    Sort3(idx1i, idx2i, idx3i, val1i, val2i, val3i,
                 idx1o, &idx21s, &idx23as,
                 val1o, &val21s, &val23as);

    Sort3(idx4i, idx5i, idx6i, val4i, val5i, val6i,
                 idx2o, &idx22s, &idx23bs,
                 val2o, &val22s, &val23bs);

    Sort2(idx23as, idx23bs, val23as, val23bs, &idx23s, &dummy, &val23s, &dummy);

    Sort3(idx21s, idx22s, idx23s, val21s, val22s, val23s,
                 idx3o, idx4o, &dummy,
                 val3o, val4o, &dummy);

}

void AliTRDmcmSim::Sort6To2Worst(UShort_t  idx1i, UShort_t  idx2i, UShort_t  idx3i, UShort_t  idx4i, UShort_t  idx5i, UShort_t  idx6i, \
                                    UShort_t  val1i, UShort_t  val2i, UShort_t  val3i, UShort_t  val4i, UShort_t  val5i, UShort_t  val6i, \
                                    UShort_t *idx5o, UShort_t *idx6o)
{
  // sorting for tracklet selection

    UShort_t idx21s, idx22s, idx23s, dummy1, dummy2, dummy3, dummy4, dummy5;
    UShort_t val21s, val22s, val23s;
    UShort_t idx23as, idx23bs;
    UShort_t val23as, val23bs;

    Sort3(idx1i, idx2i,   idx3i, val1i, val2i, val3i,
                 &dummy1, &idx21s, &idx23as,
                 &dummy2, &val21s, &val23as);

    Sort3(idx4i, idx5i, idx6i, val4i, val5i, val6i,
                 &dummy1, &idx22s, &idx23bs,
                 &dummy2, &val22s, &val23bs);

    Sort2(idx23as, idx23bs, val23as, val23bs, &idx23s, idx5o, &val23s, &dummy1);

    Sort3(idx21s, idx22s, idx23s, val21s, val22s, val23s,
                 &dummy1, &dummy2, idx6o,
                 &dummy3, &dummy4, &dummy5);
}


// ----- I/O implementation -----

ostream& AliTRDmcmSim::Text(ostream& os)
{
  // manipulator to activate output in text format (default)

  os.iword(fgkFormatIndex) = 0;
  return os;
}

ostream& AliTRDmcmSim::Cfdat(ostream& os)
{
  // manipulator to activate output in CFDAT format 
  // to send to the FEE via SCSN

  os.iword(fgkFormatIndex) = 1; 
  return os;
}

ostream& AliTRDmcmSim::Raw(ostream& os)
{
  // manipulator to activate output as raw data dump

  os.iword(fgkFormatIndex) = 2;
  return os;
}

ostream& operator<<(ostream& os, const AliTRDmcmSim& mcm)
{
  // output implementation
  
  // no output for non-initialized MCM
  if (!mcm.CheckInitialized())
    return os;

  // ----- human-readable output -----
  if (os.iword(AliTRDmcmSim::fgkFormatIndex) == 0) {
    
    os << "MCM " << mcm.fMcmPos << " on ROB " << mcm.fRobPos << 
      " in detector " << mcm.fDetector << std::endl;
    
    os << "----- Unfiltered ADC data (10 bit) -----" << std::endl;
    os << "ch    ";
    for (Int_t iChannel = 0; iChannel < mcm.fgkNADC; iChannel++) 
      os << std::setw(5) << iChannel;
    os << std::endl;
    for (Int_t iTimeBin = 0; iTimeBin < mcm.fNTimeBin; iTimeBin++) {
      os << "tb " << std::setw(2) << iTimeBin << ":";
      for (Int_t iChannel = 0; iChannel < mcm.fgkNADC; iChannel++) {
        os << std::setw(5) << (mcm.fADCR[iChannel][iTimeBin] >> mcm.fgkAddDigits);
      }
      os << std::endl;
    }
    
    os << "----- Filtered ADC data (10+2 bit) -----" << std::endl;
    os << "ch    ";
    for (Int_t iChannel = 0; iChannel < mcm.fgkNADC; iChannel++) 
      os << std::setw(4) << iChannel
         << ((~mcm.fZSMap[iChannel] != 0) ? "!" : " ");
    os << std::endl;
    for (Int_t iTimeBin = 0; iTimeBin < mcm.fNTimeBin; iTimeBin++) {
      os << "tb " << std::setw(2) << iTimeBin << ":";
      for (Int_t iChannel = 0; iChannel < mcm.fgkNADC; iChannel++) {
        os << std::setw(4) << (mcm.fADCF[iChannel][iTimeBin])
           << (((mcm.fZSMap[iChannel] & (1 << iTimeBin)) == 0) ? "!" : " ");
      }
      os << std::endl;
    }
  }

  // ----- CFDAT output -----
  else if(os.iword(AliTRDmcmSim::fgkFormatIndex) == 1) {
    Int_t dest       = 127;
    Int_t addrOffset = 0x2000;
    Int_t addrStep   = 0x80;
    
    for (Int_t iTimeBin = 0; iTimeBin < mcm.fNTimeBin; iTimeBin++) {
      for (Int_t iChannel = 0; iChannel < mcm.fgkNADC; iChannel++) {
        os << std::setw(5) << 10 
           << std::setw(5) << addrOffset + iChannel * addrStep + iTimeBin 
           << std::setw(5) << (mcm.fADCF[iChannel][iTimeBin])
           << std::setw(5) << dest << std::endl;
      }
      os << std::endl;
    }
  }

  // ----- raw data ouptut -----
  else if (os.iword(AliTRDmcmSim::fgkFormatIndex) == 2) {
    Int_t   bufSize   = 300;
    UInt_t *buf       = new UInt_t[bufSize];
    
    Int_t bufLength   = mcm.ProduceRawStream(&buf[0], bufSize);
    
    for (Int_t i = 0; i < bufLength; i++) 
      std::cout << "0x" << std::hex << buf[i] << std::endl;
    
    delete [] buf;
  }

  else {
    os << "unknown format set" << std::endl;
  }

  return os;
}
