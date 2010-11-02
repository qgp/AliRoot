/* $Id: AliTriggerAnalysis.cxx 35782 2009-10-22 11:54:31Z jgrosseo $ */

/**************************************************************************
 * Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
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

//-------------------------------------------------------------------------
//                      Implementation of   Class AliTriggerAnalysis
//   This class provides function to check if events have been triggered based on the data in the ESD
//   The trigger bits, trigger class inputs and only the data (offline trigger) can be used
//   Origin: Jan Fiete Grosse-Oetringhaus, CERN
//-------------------------------------------------------------------------

#include <Riostream.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TList.h>
#include <TIterator.h>
#include "TParameter.h"
#include <TMap.h>
#include <TRandom.h>

#include <AliTriggerAnalysis.h>

#include <AliLog.h>

#include <AliESDEvent.h>

#include <AliMultiplicity.h>
#include <AliESDVZERO.h>
#include <AliESDZDC.h>
#include <AliESDFMD.h>
#include <AliESDVertex.h>
#include <AliESDtrackCuts.h>

ClassImp(AliTriggerAnalysis)

AliTriggerAnalysis::AliTriggerAnalysis() :
  fSPDGFOThreshold(2),
  fSPDGFOEfficiency(0),
  fV0TimeOffset(0),
  fV0AdcThr(0),
  fV0HwAdcThr(2.5),
  fV0HwWinLow(61.5),
  fV0HwWinHigh(86.5),
  fFMDLowCut(0.2),
  fFMDHitCut(0.5),
  fHistBitsSPD(0),
  fHistFiredBitsSPD(0),
  fHistV0A(0),       
  fHistV0C(0),
  fHistZDC(0),    
  fHistFMDA(0),    
  fHistFMDC(0),   
  fHistFMDSingle(0),
  fHistFMDSum(0),
  fTriggerClasses(0),
  fMC(kFALSE),
  fEsdTrackCuts(0)
{
  // constructor
}

AliTriggerAnalysis::~AliTriggerAnalysis()
{
  // destructor
  
  if (fHistBitsSPD)
  {
    delete fHistBitsSPD;
    fHistBitsSPD = 0;
  }

  if (fHistFiredBitsSPD)
  {
    delete fHistFiredBitsSPD;
    fHistFiredBitsSPD = 0;
  }

  if (fHistV0A)
  {
    delete fHistV0A;
    fHistV0A = 0;
  }

  if (fHistV0C)
  {
    delete fHistV0C;
    fHistV0C = 0;
  }

  if (fHistZDC)
  {
    delete fHistZDC;
    fHistZDC = 0;
  }

  if (fHistFMDA)
  {
    delete fHistFMDA;
    fHistFMDA = 0;
  }

  if (fHistFMDC)
  {
    delete fHistFMDC;
    fHistFMDC = 0;
  }

  if (fHistFMDSingle)
  {
    delete fHistFMDSingle;
    fHistFMDSingle = 0;
  }

  if (fHistFMDSum)
  {
    delete fHistFMDSum;
    fHistFMDSum = 0;
  }

  if (fTriggerClasses)
  {
    fTriggerClasses->DeleteAll();
    delete fTriggerClasses;
    fTriggerClasses = 0;
  }

  if (fEsdTrackCuts){
    delete fEsdTrackCuts;
    fEsdTrackCuts =0;
  }
}

void AliTriggerAnalysis::EnableHistograms()
{
  // creates the monitoring histograms
  
  // do not add this hists to the directory
  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  
  fHistBitsSPD = new TH2F("fHistBitsSPD", "SPD GFO;number of fired chips (offline);number of fired chips (hardware)", 1202, -1.5, 1200.5, 1202, -1.5, 1200.5);
  fHistFiredBitsSPD = new TH1F("fHistFiredBitsSPD", "SPD GFO Hardware;chip number;events", 1200, -0.5, 1199.5);
  fHistV0A = new TH1F("fHistV0A", "V0A;leading time (ns);events", 400, -100, 100);
  fHistV0C = new TH1F("fHistV0C", "V0C;leading time (ns);events", 400, -100, 100);
  fHistZDC = new TH1F("fHistZDC", "ZDC;trigger bits;events", 8, -1.5, 6.5);
  
  // TODO check limits
  fHistFMDA = new TH1F("fHistFMDA", "FMDA;combinations above threshold;events", 102, -1.5, 100.5);
  fHistFMDC = new TH1F("fHistFMDC", "FMDC;combinations above threshold;events", 102, -1.5, 100.5);
  fHistFMDSingle = new TH1F("fHistFMDSingle", "FMD single;multiplicity value;counts", 1000, 0, 10);
  fHistFMDSum = new TH1F("fHistFMDSum", "FMD sum;multiplicity value;counts", 1000, 0, 10);
  
  fTriggerClasses = new TMap;
  fTriggerClasses->SetOwner();
  
  TH1::AddDirectory(oldStatus);
}

//____________________________________________________________________
const char* AliTriggerAnalysis::GetTriggerName(Trigger trigger) 
{
  // returns the name of the requested trigger
  // the returned string will only be valid until the next call to this function [not thread-safe]
  
  static TString str;
  
  UInt_t triggerNoFlags = (UInt_t) trigger % (UInt_t) kStartOfFlags;
  
  switch (triggerNoFlags)
  {
    case kAcceptAll : str = "ACCEPT ALL (bypass!)"; break;
    case kMB1 : str = "MB1"; break;
    case kMB2 : str = "MB2"; break;
    case kMB3 : str = "MB3"; break;
    case kSPDGFO : str = "SPD GFO"; break;
    case kSPDGFOBits : str = "SPD GFO Bits"; break;
    case kV0A : str = "V0 A BB"; break;
    case kV0C : str = "V0 C BB"; break;
    case kV0OR : str = "V0 OR BB"; break;
    case kV0AND : str = "V0 AND BB"; break;
    case kV0ABG : str = "V0 A BG"; break;
    case kV0CBG : str = "V0 C BG"; break;
    case kZDC : str = "ZDC"; break;
    case kZDCA : str = "ZDC A"; break;
    case kZDCC : str = "ZDC C"; break;
    case kFMDA : str = "FMD A"; break;
    case kFMDC : str = "FMD C"; break;
    case kFPANY : str = "SPD GFO | V0 | ZDC | FMD"; break;
    case kNSD1 : str = "NSD1"; break;
    case kMB1Prime: str = "MB1prime"; break;
    default: str = ""; break;
  }
   
  if (trigger & kOfflineFlag)
    str += " OFFLINE";  
  
  if (trigger & kOneParticle)
    str += " OneParticle";  

  if (trigger & kOneTrack)
    str += " OneTrack";  

  return str;
}

Bool_t AliTriggerAnalysis::IsTriggerFired(const AliESDEvent* aEsd, Trigger trigger)
{
  // checks if an event has been triggered

  if (trigger & kOfflineFlag)
    return IsOfflineTriggerFired(aEsd, trigger);
    
  return IsTriggerBitFired(aEsd, trigger);
}

Bool_t AliTriggerAnalysis::IsTriggerBitFired(const AliESDEvent* aEsd, Trigger trigger) const
{
  // checks if an event is fired using the trigger bits

  return IsTriggerBitFired(aEsd->GetTriggerMask(), trigger);
}

Bool_t AliTriggerAnalysis::IsTriggerBitFired(ULong64_t triggerMask, Trigger trigger) const
{
  // checks if an event is fired using the trigger bits
  //
  // this function needs the branch TriggerMask in the ESD
  
  // definitions from p-p.cfg
  ULong64_t spdFO = (1 << 14);
  ULong64_t v0left = (1 << 10);
  ULong64_t v0right = (1 << 11);

  switch (trigger)
  {
    case kAcceptAll:
    {
      return kTRUE;
      break;
    }
    case kMB1:
    {
      if (triggerMask & spdFO || ((triggerMask & v0left) || (triggerMask & v0right)))
        return kTRUE;
      break;
    }
    case kMB2:
    {
      if (triggerMask & spdFO && ((triggerMask & v0left) || (triggerMask & v0right)))
        return kTRUE;
      break;
    }
    case kMB3:
    {
      if (triggerMask & spdFO && (triggerMask & v0left) && (triggerMask & v0right))
        return kTRUE;
      break;
    }
    case kSPDGFO:
    {
      if (triggerMask & spdFO)
        return kTRUE;
      break;
    }
    default:
      Printf("IsEventTriggered: ERROR: Trigger type %d not implemented in this method", (Int_t) trigger);
      break;
  }

  return kFALSE;
}

Bool_t AliTriggerAnalysis::IsTriggerBitFired(const AliESDEvent* aEsd, ULong64_t tclass) const
{
  // Checks if corresponding bit in mask is on
  
  ULong64_t trigmask = aEsd->GetTriggerMask();
  return (trigmask & (1ull << (tclass-1)));
}

Bool_t AliTriggerAnalysis::IsOfflineTriggerFired(const AliESDEvent* aEsd, Trigger trigger)
{
  // checks if an event has been triggered "offline"

  UInt_t triggerNoFlags = (UInt_t) trigger % (UInt_t) kStartOfFlags;
  
  Bool_t decision = kFALSE;
  switch (triggerNoFlags)
  {
    case kAcceptAll:
    {
      decision = kTRUE;
      break;
    }
    case kMB1:
    {
      if (SPDGFOTrigger(aEsd, 0) || V0Trigger(aEsd, kASide, kFALSE) == kV0BB || V0Trigger(aEsd, kCSide, kFALSE) == kV0BB)
        decision = kTRUE;
      break;
    }
    case kMB2:
    {
      if (SPDGFOTrigger(aEsd, 0) && (V0Trigger(aEsd, kASide, kFALSE) == kV0BB || V0Trigger(aEsd, kCSide, kFALSE) == kV0BB))
        decision = kTRUE;
      break;
    }
    case kMB3:
    {
      if (SPDGFOTrigger(aEsd, 0) && V0Trigger(aEsd, kASide, kFALSE) == kV0BB && V0Trigger(aEsd, kCSide, kFALSE) == kV0BB)
        decision = kTRUE;
      break;
    }
    case kSPDGFO:
    {
      if (SPDGFOTrigger(aEsd, 0))
        decision = kTRUE;
      break;
    }
    case kSPDGFOBits:
    {
      if (SPDGFOTrigger(aEsd, 1))
        decision = kTRUE;
      break;
    }
    case kV0A:
    {
      if (V0Trigger(aEsd, kASide, kFALSE) == kV0BB)
        decision = kTRUE;
      break;
    }
    case kV0C:
    {
      if (V0Trigger(aEsd, kCSide, kFALSE) == kV0BB)
        decision = kTRUE;
      break;
    }
    case kV0OR:
    {
      if (V0Trigger(aEsd, kASide, kFALSE) == kV0BB || V0Trigger(aEsd, kCSide, kFALSE) == kV0BB)
        decision = kTRUE;
      break;
    }
    case kV0AND:
    {
      if (V0Trigger(aEsd, kASide, kFALSE) == kV0BB && V0Trigger(aEsd, kCSide, kFALSE) == kV0BB)
        decision = kTRUE;
      break;
    }
    case kV0ABG:
    {
      if (V0Trigger(aEsd, kASide, kFALSE) == kV0BG)
        decision = kTRUE;
      break;
    }
    case kV0CBG:
    {
      if (V0Trigger(aEsd, kCSide, kFALSE) == kV0BG)
        decision = kTRUE;
      break;
    }
    case kZDC:
    {
      if (ZDCTrigger(aEsd, kASide) || ZDCTrigger(aEsd, kCentralBarrel) || ZDCTrigger(aEsd, kCSide))
        decision = kTRUE;
      break;
    }
    case kZDCA:
    {
      if (ZDCTrigger(aEsd, kASide))
        decision = kTRUE;
      break;
    }
    case kZDCC:
    {
      if (ZDCTrigger(aEsd, kCSide))
        decision = kTRUE;
      break;
    }
    case kFMDA:
    {
      if (FMDTrigger(aEsd, kASide))
        decision = kTRUE;
      break;
    }
    case kFMDC:
    {
      if (FMDTrigger(aEsd, kCSide))
        decision = kTRUE;
      break;
    }
    case kFPANY:
    {
      if (SPDGFOTrigger(aEsd, 0) || V0Trigger(aEsd, kASide, kFALSE) == kV0BB || V0Trigger(aEsd, kCSide, kFALSE) == kV0BB || ZDCTrigger(aEsd, kASide) || ZDCTrigger(aEsd, kCentralBarrel) || ZDCTrigger(aEsd, kCSide) || FMDTrigger(aEsd, kASide) || FMDTrigger(aEsd, kCSide))
        decision = kTRUE;
      break;
    }
    case kNSD1:
    {
      if (SPDFiredChips(aEsd, 0) >= 5 || (V0Trigger(aEsd, kASide, kFALSE) == kV0BB && V0Trigger(aEsd, kCSide, kFALSE) == kV0BB))
        decision = kTRUE;
       break;
    }
    case kMB1Prime:
    {
      Int_t count = 0;
      if (SPDGFOTrigger(aEsd, 0))
        count++;
      if (V0Trigger(aEsd, kASide, kFALSE) == kV0BB)
        count++;
      if (V0Trigger(aEsd, kCSide, kFALSE) == kV0BB)
        count++;
      
      if (count >= 2)
        decision = kTRUE;
        
      break;
    }
    default:
    {
      AliFatal(Form("Trigger type %d not implemented", triggerNoFlags));
    }
  }
  
  // hadron-level requirement
  if (decision && (trigger & kOneParticle))
  {
    decision = kFALSE;
    
    const AliESDVertex* vertex = aEsd->GetPrimaryVertexSPD();
    const AliMultiplicity* mult = aEsd->GetMultiplicity();

    if (mult && vertex && vertex->GetNContributors() > 0 && (!vertex->IsFromVertexerZ() || vertex->GetDispersion() < 0.02) && TMath::Abs(vertex->GetZv()) < 5.5) 
    {
      for (Int_t i=0; i<mult->GetNumberOfTracklets(); ++i)
      {
        if (TMath::Abs(mult->GetEta(i)) < 1)
        {
          decision = kTRUE;
          break;
        }
      }
    }
  }

  // hadron level definition for TPC tracks

  if (decision && (trigger & kOneTrack))
  {
    decision = kFALSE;
    const AliESDVertex* vertex =0x0;
    vertex = aEsd->GetPrimaryVertexTracks();
    if (!vertex || vertex->GetNContributors() <= 0)
    {
      vertex = aEsd->GetPrimaryVertexSPD();
    }
    Float_t ptmin, ptmax;
    fEsdTrackCuts->GetPtRange(ptmin,ptmax);
    AliDebug(3, Form("ptmin = %f, ptmax = %f\n",ptmin, ptmax));

    if (vertex && vertex->GetNContributors() > 0 && (!vertex->IsFromVertexerZ() || vertex->GetDispersion() < 0.02) && TMath::Abs(vertex->GetZv()) < 10.) {
      AliDebug(3,Form("Check on the vertex passed\n"));
      for (Int_t i=0; i<aEsd->GetNumberOfTracks(); ++i){
	if (fEsdTrackCuts->AcceptTrack(aEsd->GetTrack(i))){
	  AliDebug(2, Form("pt of track = %f --> check passed\n",aEsd->GetTrack(i)->Pt()));
	  decision = kTRUE;
	  break;
        }
      }
    }
    else{
      AliDebug(4,Form("Check on the vertex not passed\n"));
      for (Int_t i=0; i<aEsd->GetNumberOfTracks(); ++i){
        if (fEsdTrackCuts->AcceptTrack(aEsd->GetTrack(i))){
	  AliDebug(4,Form("pt of track = %f --> check would be passed if the vertex was ok\n",aEsd->GetTrack(i)->Pt()));
	  break;
	}
      }
    }
    if (!decision) AliDebug(3,("Check for kOneTrack NOT passed\n"));
  }

  return decision;
}

Bool_t AliTriggerAnalysis::IsTriggerClassFired(const AliESDEvent* aEsd, const Char_t* tclass) const 
{
  // tclass is logical function of inputs, e.g. 01 && 02 || 03 && 11 && 21
  // = L0 inp 1 && L0 inp 2 || L0 inp 3 && L1 inp 1 && L2 inp 1
  // NO brackets in logical function !
  // Spaces between operators and inputs.
  // Not all logical functions are available in CTP= 
  // =any function of first 4 inputs; 'AND' of other inputs, check not done
  // This method will be replaced/complemened by similar one
  // which works withh class and inputs names as in CTP cfg file
  
  TString TClass(tclass);
  TObjArray* tcltokens = TClass.Tokenize(" ");
  Char_t level=((TObjString*)tcltokens->At(0))->String()[0];
  UInt_t input=atoi((((TObjString*)tcltokens->At(0))->String()).Remove(0));
  Bool_t tcl = IsInputFired(aEsd,level,input);
 
  for (Int_t i=1;i<tcltokens->GetEntriesFast();i=i+2) {
    level=((TObjString*)tcltokens->At(i+1))->String()[0];
    input=atoi((((TObjString*)tcltokens->At(i+1))->String()).Remove(0));
    Bool_t inpnext = IsInputFired(aEsd,level,input);
    Char_t op =((TObjString*)tcltokens->At(i))->String()[0];
    if (op == '&') tcl=tcl && inpnext;
    else if (op == '|') tcl =tcl || inpnext;
    else {
       AliError(Form("Syntax error in %s", tclass));
       tcltokens->Delete();
       return kFALSE;
    }
  }
  tcltokens->Delete();
  return tcl;
}

Bool_t AliTriggerAnalysis::IsInputFired(const AliESDEvent* aEsd, Char_t level, UInt_t input) const
{
  // Checks trigger input of any level
  
  switch (level)
  {
    case '0': return IsL0InputFired(aEsd,input);
    case '1': return IsL1InputFired(aEsd,input);
    case '2': return IsL2InputFired(aEsd,input);
    default:
      AliError(Form("Wrong level %i",level));
      return kFALSE;
  }
}

Bool_t AliTriggerAnalysis::IsL0InputFired(const AliESDEvent* aEsd, UInt_t input) const 
{
  // Checks if corresponding bit in mask is on
  
  UInt_t inpmask = aEsd->GetHeader()->GetL0TriggerInputs();
  return (inpmask & (1<<(input-1)));
}

Bool_t AliTriggerAnalysis::IsL1InputFired(const AliESDEvent* aEsd, UInt_t input) const
{
  // Checks if corresponding bit in mask is on
  
  UInt_t inpmask = aEsd->GetHeader()->GetL1TriggerInputs();
  return (inpmask & (1<<(input-1)));
}

Bool_t AliTriggerAnalysis::IsL2InputFired(const AliESDEvent* aEsd, UInt_t input) const 
{
  // Checks if corresponding bit in mask is on
  
  UInt_t inpmask = aEsd->GetHeader()->GetL2TriggerInputs();
  return (inpmask & (1<<(input-1)));
}

void AliTriggerAnalysis::FillHistograms(const AliESDEvent* aEsd) 
{
  // fills the histograms with the info from the ESD
  
  fHistBitsSPD->Fill(SPDFiredChips(aEsd, 0), SPDFiredChips(aEsd, 1, kTRUE));
  
  V0Trigger(aEsd, kASide, kFALSE, kTRUE);
  V0Trigger(aEsd, kCSide, kFALSE, kTRUE);
  
  AliESDZDC* zdcData = aEsd->GetESDZDC();
  if (zdcData)
  {
    UInt_t quality = zdcData->GetESDQuality();
    
    // from Nora's presentation, general first physics meeting 16.10.09
    static UInt_t zpc  = 0x20;
    static UInt_t znc  = 0x10;
    static UInt_t zem1 = 0x08;
    static UInt_t zem2 = 0x04;
    static UInt_t zpa  = 0x02;
    static UInt_t zna  = 0x01;
   
    fHistZDC->Fill(1, quality & zna);
    fHistZDC->Fill(2, quality & zpa);
    fHistZDC->Fill(3, quality & zem2);
    fHistZDC->Fill(4, quality & zem1);
    fHistZDC->Fill(5, quality & znc);
    fHistZDC->Fill(6, quality & zpc);
  }
  else
  {
    fHistZDC->Fill(-1);
    AliError("AliESDZDC not available");
  }
  
  fHistFMDA->Fill(FMDHitCombinations(aEsd, kASide, kTRUE));
  fHistFMDC->Fill(FMDHitCombinations(aEsd, kCSide, kTRUE));
}
  
void AliTriggerAnalysis::FillTriggerClasses(const AliESDEvent* aEsd)
{
  // fills trigger classes map
  
  TParameter<Long64_t>* count = dynamic_cast<TParameter<Long64_t>*> (fTriggerClasses->GetValue(aEsd->GetFiredTriggerClasses().Data()));
  if (!count)
  {
    count = new TParameter<Long64_t>(aEsd->GetFiredTriggerClasses(), 0);
    fTriggerClasses->Add(new TObjString(aEsd->GetFiredTriggerClasses().Data()), count);
  }
  count->SetVal(count->GetVal() + 1);
  
  // TODO add first and last orbit number here
}

Int_t AliTriggerAnalysis::SSDClusters(const AliESDEvent* aEsd)
{
  // returns the number of clusters in the SSD
  const AliMultiplicity* mult = aEsd->GetMultiplicity();
  Int_t clusters = mult->GetNumberOfITSClusters(4)+mult->GetNumberOfITSClusters(5);
  return clusters;
}


Int_t AliTriggerAnalysis::SPDFiredChips(const AliESDEvent* aEsd, Int_t origin, Bool_t fillHists, Int_t layer)
{
  // returns the number of fired chips in the SPD
  //
  // origin = 0 --> aEsd->GetMultiplicity()->GetNumberOfFiredChips() (filled from clusters)
  // origin = 1 --> aEsd->GetMultiplicity()->TestFastOrFiredChips() (from hardware bits)
  // layer  = 0 --> both layers
  // layer  = 1 --> inner
  // layer  = 2 --> outer
  
  const AliMultiplicity* mult = aEsd->GetMultiplicity();
  if (!mult)
  {
    AliError("AliMultiplicity not available");
    return -1;
  }
  
  if (origin == 0){
    if (layer == 0) 
      return mult->GetNumberOfFiredChips(0) + mult->GetNumberOfFiredChips(1);

    return mult->GetNumberOfFiredChips(layer-1); 
  }
    
  if (origin == 1)
  {
    Int_t nChips = 0;
    Int_t firstChip = 0;
    Int_t lastChip  = 1200;
    if(layer == 1)
      lastChip  = 400;
    if(layer == 2)
      firstChip = 400;

    for (Int_t i=0; i<1200; i++)
    {
      if (mult->TestFastOrFiredChips(i) == kTRUE)
      {
        // efficiency simulation (if enabled)
        if (fSPDGFOEfficiency)
        {
          if (gRandom->Uniform() > fSPDGFOEfficiency->GetBinContent(i+1))
            continue;
        }
        
        nChips++;
        if (fillHists)
          fHistFiredBitsSPD->Fill(i);
      }
    }
    return nChips;
  }
  
  return -1;
}

Bool_t AliTriggerAnalysis::SPDGFOTrigger(const AliESDEvent* aEsd, Int_t origin)
{
  // Returns if the SPD gave a global Fast OR trigger
  
  Int_t firedChips = SPDFiredChips(aEsd, origin);
  
  if (firedChips >= fSPDGFOThreshold)
    return kTRUE;
  return kFALSE;
}

AliTriggerAnalysis::V0Decision AliTriggerAnalysis::V0Trigger(const AliESDEvent* aEsd, AliceSide side, Bool_t online, Bool_t fillHists)
{
  // Returns the V0 trigger decision in V0A | V0C
  //
  // Returns kV0Fake if the calculated average time is in a window where neither BB nor BG is expected. 
  // The rate of such triggers can be used to estimate the background. Note that the rate has to be 
  // rescaled with the size of the windows (numerical values see below in the code)
  //
  // argument 'online' is used as a switch between online and offline trigger algorithms
  //
  // Based on an algorithm by Cvetan Cheshkov

  AliESDVZERO* esdV0 = aEsd->GetVZEROData();
  if (!esdV0)
  {
    AliError("AliESDVZERO not available");
    return kV0Invalid;
  }
  AliDebug(2,Form("In V0Trigger: %f %f",esdV0->GetV0ATime(),esdV0->GetV0CTime()));

  Int_t begin = -1;
  Int_t end = -1;
  
  if (side == kASide)
  {
    begin = 32;
    end = 64;
  } 
  else if (side == kCSide)
  {
    begin = 0;
    end = 32;
  }
  else
    return kV0Invalid;
    
   if (esdV0->TestBit(AliESDVZERO::kDecisionFilled)) {
    if (online) {
      if (esdV0->TestBit(AliESDVZERO::kOnlineBitsFilled)) {
	for (Int_t i = begin; i < end; ++i) {
	  if (esdV0->GetBBFlag(i)) return kV0BB;
	}
	for (Int_t i = begin; i < end; ++i) {
	  if (esdV0->GetBGFlag(i)) return kV0BG;
	}
	return kV0Empty;
      }
      else {
	AliWarning("V0 online trigger analysis is not yet available!");
	return kV0BB;
      }
    }
    else {

      if (fillHists) {
	if (side == kASide && fHistV0A)
	  fHistV0A->Fill(esdV0->GetV0ATime());
	if (side == kCSide && fHistV0C)
	  fHistV0C->Fill(esdV0->GetV0CTime());
      }

      if (side == kASide) return (V0Decision)esdV0->GetV0ADecision();
      else if (side == kCSide) return (V0Decision)esdV0->GetV0CDecision();
      else return kV0Invalid;
    }
  }

  Float_t time = 0;
  Float_t weight = 0;
  if (fMC)
  {
    Int_t runRange;
    if (aEsd->GetRunNumber() <= 104803) runRange = 0;
    else if (aEsd->GetRunNumber() <= 104876) runRange = 1;
    else runRange = 2;

    Float_t factors[3][64] = {
      // runs: 104792-104803
      {4.6,5.9,6.3,6.0,4.7,5.9,4.9,5.4,4.8,4.1,4.9,4.6,4.5,5.5,5.1,5.8,4.3,4.0,4.0,3.3,3.1,2.9,3.0,5.6,3.3,4.9,3.9,5.3,4.1,4.4,3.9,5.5,5.7,9.5,5.1,5.3,6.6,7.1,8.9,4.4,4.1,5.9,9.0,4.5,4.1,6.0,4.7,7.1,4.2,4.7,3.9,6.3,5.9,4.8,4.7,4.5,4.7,5.4,5.8,5.0,5.1,5.9,5.3,3.6},
      // runs: 104841-104876
      {4.6,4.8,4.9,4.8,4.3,4.9,4.4,4.5,4.6,5.0,4.7,4.6,4.7,4.6,4.6,5.5,4.7,4.5,4.7,5.0,6.5,7.6,5.3,4.9,5.5,4.8,4.6,4.9,4.5,4.5,4.6,4.9,5.7,9.8,4.9,5.2,7.1,7.1,8.1,4.4,4.0,6.0,8.3,4.6,4.2,5.6,4.6,6.4,4.4,4.7,4.5,6.5,6.0,4.7,4.5,4.4,4.8,5.5,5.9,5.3,5.0,5.7,5.1,3.6},
      // runs: 104890-92
      {4.7,5.2,4.8,5.0,4.4,5.0,4.4,4.6,4.6,4.5,4.4,4.6,4.5,4.6,4.8,5.5,4.8,4.5,4.4,4.3,5.4,7.7,5.6,5.0,5.4,4.3,4.5,4.8,4.5,4.5,4.6,5.3,5.7,9.6,4.9,5.4,6.1,7.2,8.6,4.4,4.0,5.4,8.8,4.4,4.2,5.8,4.7,6.7,4.3,4.7,4.0,6.1,6.0,4.9,4.8,4.6,4.7,5.2,5.7,5.0,5.0,5.8,5.3,3.6}
    };
    Float_t dA = 77.4 - 11.0;
    Float_t dC = 77.4 - 2.9;
    // Time misalignment
    Float_t timeShift[64] = {0.477957 , 0.0889999 , 0.757669 , 0.205439 , 0.239666 , -0.183705 , 0.442873 , -0.281366 , 0.260976 , 0.788995 , 0.974758 , 0.548532 , 0.495023 , 0.868472 , 0.661167 , 0.358307 , 0.221243 , 0.530179 , 1.26696 , 1.33082 , 1.27086 , 1.77133 , 1.10253 , 0.634806 , 2.14838 , 1.50212 , 1.59253 , 1.66122 , 1.16957 , 1.52056 , 1.47791 , 1.81905 , -1.94123 , -1.29124 , -2.16045 , -1.78939 , -3.11111 , -1.87178 , -1.57671 , -1.70311 , -1.81208 , -1.94475 , -2.53058 , -1.7042 , -2.08109 , -1.84416 , -0.61073 , -1.77145 , 0.16999 , -0.0585339 , 0.00401133 , 0.397726 , 0.851111 , 0.264187 , 0.59573 , -0.158263 , 0.584362 , 1.20835 , 0.927573 , 1.13895 , 0.64648 , 2.18747 , 1.68909 , 0.451194};
    Float_t dA2 = 2.8, dC2 = 3.3;

    if (online) {
      for (Int_t i = begin; i < end; ++i) {
        Float_t tempAdc = esdV0->GetAdc(i)/factors[runRange][i];
        Float_t tempTime = (i >= 32) ? esdV0->GetTime(i)+dA+timeShift[i]+dA2 : esdV0->GetTime(i)+dC+timeShift[i]+dC2;
        if (esdV0->GetTime(i) >= 1e-6 &&
            tempTime > fV0HwWinLow && tempTime < fV0HwWinHigh &&
            tempAdc > fV0HwAdcThr)
          return kV0BB;
      }
      return kV0Empty;
    }
    else {
      for (Int_t i = begin; i < end; ++i) {
        Float_t tempAdc = esdV0->GetAdc(i)/factors[runRange][i];
        Float_t tempTime = (i >= 32) ? esdV0->GetTime(i)+dA : esdV0->GetTime(i)+dC;
        Float_t tempRawTime = (i >= 32) ? esdV0->GetTime(i)+dA+timeShift[i]+dA2 : esdV0->GetTime(i)+dC+timeShift[i]+dC2;
        if (esdV0->GetTime(i) >= 1e-6 &&
            tempRawTime < 125.0 &&
            tempAdc > fV0AdcThr) {
          weight += 1.0;
          time += tempTime;
        }
      }
    }
  }
  else {
    if (online) {
      for (Int_t i = begin; i < end; ++i) {
        if (esdV0->GetTime(i) >= 1e-6 &&
            esdV0->GetTime(i) > fV0HwWinLow && esdV0->GetTime(i) < fV0HwWinHigh &&
            esdV0->GetAdc(i) > fV0HwAdcThr)
          return kV0BB;
      }
      return kV0Empty;
    }
    else {
      for (Int_t i = begin; i < end; ++i) {
	if (esdV0->GetTime(i) > 1e-6 && esdV0->GetAdc(i) > fV0AdcThr) {
	  Float_t correctedTime = V0CorrectLeadingTime(i, esdV0->GetTime(i), esdV0->GetAdc(i),aEsd->GetRunNumber());
	  Float_t timeWeight = V0LeadingTimeWeight(esdV0->GetAdc(i));
	  time += correctedTime*timeWeight;
            
	  weight += timeWeight;
	}
      }
    }
  }

  if (weight > 0) 
    time /= weight;
  time += fV0TimeOffset;

  if (fillHists)
  {
    if (side == kASide && fHistV0A)
      fHistV0A->Fill(time);
    if (side == kCSide && fHistV0C)
      fHistV0C->Fill(time);
  }
  
  if (side == kASide)
  {
    if (time > 68 && time < 100)
      return kV0BB;
    if (time > 54 && time < 57.5) 
      return kV0BG;
    if (time > 57.5 && time < 68)
      return kV0Fake;
  }
  
  if (side == kCSide)
  {
    if (time > 75.5 && time < 100)
      return kV0BB;
    if (time > 69.5 && time < 73)
      return kV0BG; 
    if (time > 55 && time < 69.5)
      return kV0Fake;
  }
  
  return kV0Empty;
}

Float_t AliTriggerAnalysis::V0CorrectLeadingTime(Int_t i, Float_t time, Float_t adc, Int_t runNumber) const
{
  // Correct for slewing and align the channels
  //
  // Authors: Cvetan Cheshkov / Raphael Tieulent

  if (time == 0) return 0;

  // Time alignment
  Float_t timeShift[64] = {0.477957 , 0.0889999 , 0.757669 , 0.205439 , 0.239666 , -0.183705 , 0.442873 , -0.281366 , 0.260976 , 0.788995 , 0.974758 , 0.548532 , 0.495023 , 0.868472 , 0.661167 , 0.358307 , 0.221243 , 0.530179 , 1.26696 , 1.33082 , 1.27086 , 1.77133 , 1.10253 , 0.634806 , 2.14838 , 1.50212 , 1.59253 , 1.66122 , 1.16957 , 1.52056 , 1.47791 , 1.81905 , -1.94123 , -1.29124 , -2.16045 , -1.78939 , -3.11111 , -1.87178 , -1.57671 , -1.70311 , -1.81208 , -1.94475 , -2.53058 , -1.7042 , -2.08109 , -1.84416 , -0.61073 , -1.77145 , 0.16999 , -0.0585339 , 0.00401133 , 0.397726 , 0.851111 , 0.264187 , 0.59573 , -0.158263 , 0.584362 , 1.20835 , 0.927573 , 1.13895 , 0.64648 , 2.18747 , 1.68909 , 0.451194};

  if(runNumber < 106031)
    time -= timeShift[i];

  // Slewing correction
  if (adc == 0) return time;

  Float_t p1 = 1.57345e1;
  Float_t p2 =-4.25603e-1;

  if(runNumber >= 106031) adc *= (2.5/4.0);
  return (time - p1*TMath::Power(adc,p2));
}

Float_t AliTriggerAnalysis::V0LeadingTimeWeight(Float_t adc) const
{
  if (adc < 1e-6) return 0;

  Float_t p1 = 40.211;
  Float_t p2 =-4.25603e-1;
  Float_t p3 = 0.5646;

  return 1./(p1*p1*TMath::Power(adc,2.*(p2-1.))+p3*p3);
}

Bool_t AliTriggerAnalysis::ZDCTrigger(const AliESDEvent* aEsd, AliceSide side) const
{
  // Returns if ZDC triggered
  
  AliESDZDC* zdcData = aEsd->GetESDZDC();
  if (!zdcData)
  {
    AliError("AliESDZDC not available");
    return kFALSE;
  }
  
  UInt_t quality = zdcData->GetESDQuality();
  
  // from Nora's presentation, general first physics meeting 16.10.09
  static UInt_t zpc  = 0x20;
  static UInt_t znc  = 0x10;
  static UInt_t zem1 = 0x08;
  static UInt_t zem2 = 0x04;
  static UInt_t zpa  = 0x02;
  static UInt_t zna  = 0x01;
  
  if (side == kASide && ((quality & zpa) || (quality & zna)))
    return kTRUE;
  if (side == kCentralBarrel && ((quality & zem1) || (quality & zem2)))
    return kTRUE;
  if (side == kCSide && ((quality & zpc) || (quality & znc)))
    return kTRUE;
  
  return kFALSE;
}

Int_t AliTriggerAnalysis::FMDHitCombinations(const AliESDEvent* aEsd, AliceSide side, Bool_t fillHists)
{
  // returns number of hit combinations agove threshold
  //
  // Authors: FMD team, Hans Dalsgaard (code merged from FMD/AliFMDOfflineTrigger)

  // Workaround for AliESDEvent::GetFMDData is not const!
  const AliESDFMD* fmdData = (const_cast<AliESDEvent*>(aEsd))->GetFMDData();
  if (!fmdData)
  {
    AliError("AliESDFMD not available");
    return -1;
  }

  Int_t detFrom = (side == kASide) ? 1 : 3;
  Int_t detTo   = (side == kASide) ? 2 : 3;

  Int_t triggers = 0;
  Float_t totalMult = 0;
  for (UShort_t det=detFrom;det<=detTo;det++) {
    Int_t nRings = (det == 1 ? 1 : 2);
    for (UShort_t ir = 0; ir < nRings; ir++) {
      Char_t   ring = (ir == 0 ? 'I' : 'O');
      UShort_t nsec = (ir == 0 ? 20  : 40);
      UShort_t nstr = (ir == 0 ? 512 : 256);
      for (UShort_t sec =0; sec < nsec;  sec++) {
        for (UShort_t strip = 0; strip < nstr; strip++) {
          Float_t mult = fmdData->Multiplicity(det,ring,sec,strip);
          if (mult == AliESDFMD::kInvalidMult) continue;
          
          if (fillHists)
            fHistFMDSingle->Fill(mult);
          
          if (mult > fFMDLowCut)
            totalMult = totalMult + mult;
          else
          {
            if (totalMult > fFMDHitCut)
              triggers++;
              
            if (fillHists)
              fHistFMDSum->Fill(totalMult);
              
            totalMult = 0;
          }
        }
      }
    }
  }
  
  return triggers;
}

Bool_t AliTriggerAnalysis::FMDTrigger(const AliESDEvent* aEsd, AliceSide side)
{
  // Returns if the FMD triggered
  //
  // Authors: FMD team, Hans Dalsgaard (code merged from FMD/AliFMDOfflineTrigger)

  Int_t triggers = FMDHitCombinations(aEsd, side, kFALSE);
  
  if (triggers > 0)
    return kTRUE;
    
  return kFALSE;
}

Long64_t AliTriggerAnalysis::Merge(TCollection* list)
{
  // Merge a list of AliMultiplicityCorrection objects with this (needed for
  // PROOF).
  // Returns the number of merged objects (including this).

  if (!list)
    return 0;

  if (list->IsEmpty())
    return 1;

  TIterator* iter = list->MakeIterator();
  TObject* obj;

  // collections of all histograms
  const Int_t nHists = 9;
  TList collections[nHists];

  Int_t count = 0;
  while ((obj = iter->Next())) {

    AliTriggerAnalysis* entry = dynamic_cast<AliTriggerAnalysis*> (obj);
    if (entry == 0) 
      continue;

    Int_t n = 0;
    collections[n++].Add(entry->fHistV0A);
    collections[n++].Add(entry->fHistV0C);
    collections[n++].Add(entry->fHistZDC);
    collections[n++].Add(entry->fHistFMDA);
    collections[n++].Add(entry->fHistFMDC);
    collections[n++].Add(entry->fHistFMDSingle);
    collections[n++].Add(entry->fHistFMDSum);
    collections[n++].Add(entry->fHistBitsSPD);
    collections[n++].Add(entry->fHistFiredBitsSPD);

    // merge fTriggerClasses
    TIterator* iter2 = entry->fTriggerClasses->MakeIterator();
    TObjString* obj2 = 0;
    while ((obj2 = dynamic_cast<TObjString*> (iter2->Next())))
    {
      TParameter<Long64_t>* param2 = dynamic_cast<TParameter<Long64_t>*> (entry->fTriggerClasses->GetValue(obj2));
      
      TParameter<Long64_t>* param1 = dynamic_cast<TParameter<Long64_t>*> (fTriggerClasses->GetValue(obj2));
      if (param1)
      {
        param1->SetVal(param1->GetVal() + param2->GetVal());
      }
      else
      {
        param1 = dynamic_cast<TParameter<Long64_t>*> (param2->Clone());
        fTriggerClasses->Add(new TObjString(obj2->String()), param1);
      }
    }
    
    delete iter2;
  
    count++;
  }

  Int_t n = 0;
  fHistV0A->Merge(&collections[n++]);
  fHistV0C->Merge(&collections[n++]);
  fHistZDC->Merge(&collections[n++]);
  fHistFMDA->Merge(&collections[n++]);
  fHistFMDC->Merge(&collections[n++]);
  fHistFMDSingle->Merge(&collections[n++]);
  fHistFMDSum->Merge(&collections[n++]);
  fHistBitsSPD->Merge(&collections[n++]);
  fHistFiredBitsSPD->Merge(&collections[n++]);
  
  delete iter;

  return count+1;
}

void AliTriggerAnalysis::SaveHistograms() const
{
  // write histograms to current directory
  
  if (!fHistBitsSPD)
    return;
    
  fHistBitsSPD->Write();
  fHistBitsSPD->ProjectionX();
  fHistBitsSPD->ProjectionY();
  fHistFiredBitsSPD->Write();
  fHistV0A->Write();
  fHistV0C->Write();
  fHistZDC->Write();
  fHistFMDA->Write();
  fHistFMDC->Write();
  fHistFMDSingle->Write();
  fHistFMDSum->Write();
  
  if (fSPDGFOEfficiency)
    fSPDGFOEfficiency->Write("fSPDGFOEfficiency");
  
  fTriggerClasses->Write("fTriggerClasses", TObject::kSingleKey);
}

void AliTriggerAnalysis::PrintTriggerClasses() const
{
  // print trigger classes
  
  Printf("Trigger Classes:");
  
  Printf("Event count for trigger combinations:");
  
  TMap singleTrigger;
  singleTrigger.SetOwner();
  
  TIterator* iter = fTriggerClasses->MakeIterator();
  TObjString* obj = 0;
  while ((obj = dynamic_cast<TObjString*> (iter->Next())))
  {
    TParameter<Long64_t>* param = static_cast<TParameter<Long64_t>*> (fTriggerClasses->GetValue(obj));
    
    Printf(" %s: %ld triggers", obj->String().Data(), (Long_t)param->GetVal());
    
    TObjArray* tokens = obj->String().Tokenize(" ");
    for (Int_t i=0; i<tokens->GetEntries(); i++)
    {
      TParameter<Long64_t>* count = dynamic_cast<TParameter<Long64_t>*> (singleTrigger.GetValue(((TObjString*) tokens->At(i))->String().Data()));
      if (!count)
      {
        count = new TParameter<Long64_t>(((TObjString*) tokens->At(i))->String().Data(), 0);
        singleTrigger.Add(new TObjString(((TObjString*) tokens->At(i))->String().Data()), count);
      }
      count->SetVal(count->GetVal() + param->GetVal());
    }
    
    delete tokens;
  }
  delete iter;
  
  Printf("Event count for single trigger:");
  
  iter = singleTrigger.MakeIterator();
  while ((obj = dynamic_cast<TObjString*> (iter->Next())))
  {
    TParameter<Long64_t>* param = static_cast<TParameter<Long64_t>*> (singleTrigger.GetValue(obj));
    
    Printf("  %s: %ld triggers", obj->String().Data(), (Long_t)param->GetVal());
  }
  delete iter;
  
  singleTrigger.DeleteAll();
}
