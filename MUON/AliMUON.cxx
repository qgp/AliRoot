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

//-----------------------------------------------------------------------------
// Class AliMUON
// ------------------
// AliDetector class for MUON subsystem 
// providing simulation data management 
//-----------------------------------------------------------------------------

#include "Riostream.h"

#include <AliPDG.h>
#include <TBRIK.h>
#include <TCanvas.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TGeometry.h>
#include <TMinuit.h>
#include <TNode.h> 
#include <TNtuple.h>
#include <TObjArray.h>
#include <TObject.h>
#include <TObjectTable.h>
#include <TPad.h>
#include <TParticle.h>
#include <TROOT.h>
#include <TRandom.h> 
#include <TRotMatrix.h>
#include <TTUBE.h>
#include <TTUBE.h>
#include <TTree.h> 
#include <TVector.h>
#include <TVirtualMC.h>

//#include "AliHeader.h"
#include "AliLoader.h"
#include "AliRunDigitizer.h"
#include "AliMC.h"
#include "AliRun.h"	
#include "AliMUON.h"
#include "AliMUONChamberTrigger.h"
#include "AliMUONConstants.h"
#include "AliMUONHit.h"	
#include "AliMUONGeometry.h"
#include "AliMUONGeometryTransformer.h"
#include "AliMUONGeometryBuilder.h"
#include "AliMUONCommonGeometryBuilder.h"
#include "AliMUONVGeometryBuilder.h"	
#include "AliMUONRawWriter.h"
#include "AliLog.h"

#include "AliMUONSDigitizerV2.h"
#include "AliMUONDigitizerV3.h"
#include "AliMUONDigitMaker.h"

#include "AliMUONSt1GeometryBuilderV2.h"
#include "AliMUONSt2GeometryBuilderV2.h"
#include "AliMUONSlatGeometryBuilder.h"
#include "AliMUONTriggerGeometryBuilder.h"

#include "AliMUONDigitStoreV1.h"
#include "AliMUONVTriggerStore.h"
#include "AliMUONHitStoreV1.h"

// Defaults parameters for Z positions of chambers
// taken from values for "stations" in AliMUON::AliMUON
//     const Float_t zch[7]={528, 690., 975., 1249., 1449., 1610, 1710.};
// and from array "dstation" in AliMUONv1::CreateGeometry
//          Float_t dstation[5]={20., 20., 20, 20., 20.};
//     for tracking chambers,
//          according to (Z1 = zch - dstation) and  (Z2 = zch + dstation)
//          for the first and second chambers in the station, respectively,
// and from "DTPLANES" in AliMUONv1::CreateGeometry
//           const Float_t DTPLANES = 15.;
//     for trigger chambers,
//          according to (Z1 = zch) and  (Z2 = zch + DTPLANES)
//          for the first and second chambers in the station, respectively

/// \cond CLASSIMP
ClassImp(AliMUON)  
/// \endcond

//__________________________________________________________________
AliMUON::AliMUON()
  : AliDetector(),
    fNCh(0),
    fNTrackingCh(0),
    fSplitLevel(0),
    fChambers(0),
    fGeometryBuilder(0),
    fAccCut(kFALSE),
    fAccMin(0.),
    fAccMax(0.),   
    fMaxStepGas(0.),
    fMaxStepAlu(0.),
    fMaxDestepGas(0.),
    fMaxDestepAlu(0.),
    fMaxIterPad(0),
    fCurIterPad(0),
    fTriggerScalerEvent(kFALSE),
    fTriggerResponseV1(kFALSE),
    fTriggerCoinc44(0),
    fTriggerEffCells(0),
    fDigitizerWithNoise(1),
    fIsTailEffect(kFALSE),
    fRawWriter(0x0),
    fDigitMaker(0x0),
    fHitStore(0x0),
  fDigitStoreConcreteClassName()
{
/// Default Constructor
    
    AliDebug(1,Form("default (empty) ctor this=%p",this));
    fIshunt          = 0;
}

//__________________________________________________________________
AliMUON::AliMUON(const char *name, const char* title)
  : AliDetector(name, title),
    fNCh(AliMUONConstants::NCh()),
    fNTrackingCh(AliMUONConstants::NTrackingCh()),
    fSplitLevel(0),
    fChambers(0),
    fGeometryBuilder(0),
    fAccCut(kFALSE),
    fAccMin(0.),
    fAccMax(0.),   
    fMaxStepGas(0.1),
    fMaxStepAlu(0.1),
    fMaxDestepGas(-1), // Negatives values are ignored by geant3 CONS200 
    fMaxDestepAlu(-1), // in the calculation of the tracking parameters
    fMaxIterPad(0),
    fCurIterPad(0),
    fTriggerScalerEvent(kFALSE),
    fTriggerResponseV1(kFALSE),
    fTriggerCoinc44(0),
    fTriggerEffCells(0),
    fDigitizerWithNoise(1),
    fIsTailEffect(kFALSE),
    fRawWriter(0x0),
    fDigitMaker(new AliMUONDigitMaker),
    fHitStore(0x0),
  fDigitStoreConcreteClassName("AliMUONDigitStoreV2S")
{
/// Standard constructor  
  
  AliDebug(1,Form("ctor this=%p",this));
  fIshunt =  0;

  //PH SetMarkerColor(kRed);//
    
  // Geometry builder
  fGeometryBuilder = new AliMUONGeometryBuilder(this);
  
  // Common geometry definitions
  fGeometryBuilder
    ->AddBuilder(new AliMUONCommonGeometryBuilder(this));

  // By default, add also all the needed geometry builders.
  // If you want to change this from outside, please use ResetGeometryBuilder
  // method, followed by AddGeometryBuilder ones.

  AddGeometryBuilder(new AliMUONSt1GeometryBuilderV2(this));
  AddGeometryBuilder(new AliMUONSt2GeometryBuilderV2(this));
  AddGeometryBuilder(new AliMUONSlatGeometryBuilder(this));
  AddGeometryBuilder(new AliMUONTriggerGeometryBuilder(this));
  
  //
  // Creating List of Chambers
    Int_t ch;
    fChambers = new TObjArray(AliMUONConstants::NCh());
    fChambers->SetOwner(kTRUE);
    
    // Loop over stations
    for (Int_t st = 0; st < AliMUONConstants::NCh() / 2; st++) {
      // Loop over 2 chambers in the station
      for (Int_t stCH = 0; stCH < 2; stCH++) {
	//
	//    
	//    Default Parameters for Muon Tracking Stations
	ch = 2 * st + stCH;
	if (ch < AliMUONConstants::NTrackingCh()) {
	  fChambers->AddAt(new AliMUONChamber(ch),ch);
	} else {
	  fChambers->AddAt(new AliMUONChamberTrigger(ch, GetGeometryTransformer()),ch);
	}
      } // Chamber stCH (0, 1) in 
    }     // Station st (0...)

}

//____________________________________________________________________
AliMUON::~AliMUON()
{
/// Destructor

  AliDebug(1,Form("dtor this=%p",this));
  delete fChambers;
  delete fGeometryBuilder;
  delete fRawWriter;
  delete fDigitMaker;
  delete fHitStore;
}

//_____________________________________________________________________________
void AliMUON::AddGeometryBuilder(AliMUONVGeometryBuilder* geomBuilder)
{
/// Add the geometry builder to the list

  fGeometryBuilder->AddBuilder(geomBuilder);
}

//____________________________________________________________________
void AliMUON::BuildGeometry()
{
/// Geometry for event display


//     for (Int_t i = 0; i < AliMUONConstants::NCh(); i++)     
//       this->Chamber(i).SegmentationModel2(1)->Draw("eventdisplay");// to be check !
     
  
}

//____________________________________________________________________
const AliMUONGeometry*  AliMUON::GetGeometry() const
{
/// Return geometry parametrisation

  if ( !fGeometryBuilder) {
    AliWarningStream() << "GeometryBuilder not defined." << std::endl;
    return 0;
  }
  
  return fGeometryBuilder->GetGeometry();
}   

//____________________________________________________________________
const AliMUONGeometryTransformer*  AliMUON::GetGeometryTransformer() const
{
/// Return geometry parametrisation

  const AliMUONGeometry* kGeometry = GetGeometry();
  
  if ( !kGeometry) return 0;

  return kGeometry->GetTransformer();
}   

//__________________________________________________________________
void 
AliMUON::MakeBranch(Option_t* opt)
{
  /// Create branche(s) to hold MUON hits
  AliDebug(1,"");
  
  TString sopt(opt);
  if ( sopt != "H" ) return;
    
  if (!fHitStore)
  {
    fHitStore = new AliMUONHitStoreV1;
    if ( gAlice->GetMCApp() )
    {
      if ( gAlice->GetMCApp()->GetHitLists() ) 
      {
        // AliStack::PurifyKine needs to be able to loop on our hits
        // to remap the track numbers.
        gAlice->GetMCApp()->AddHitList(fHitStore->Collection()); 
      }  
    }
  }

  TTree* treeH = fLoader->TreeH();
  
  if (!treeH)
  {
    AliFatal("No TreeH");
  }
  
  fHitStore->Connect(*treeH);
}

//__________________________________________________________________
void  
AliMUON::SetTreeAddress()
{
  /// Set Hits tree address  
 
//  if ( gAlice->GetMCApp() && fHitStore )
//  {
//    TList* l = gAlice->GetMCApp()->GetHitLists();
//    if ( l )
//    {
//      TObject* o = l->First();
//      if (o!=fHitStore->HitCollection())
//      {
//        AliError(Form("Something is strange hitcollection=%x",fHitStore->HitCollection()));
//        l->Print();        
//      }
//    }  
//  }  
}

//_________________________________________________________________
void
AliMUON::ResetHits()
{
  /// Reset hits
  
  AliDebug(1,"");
  if (fHitStore) fHitStore->Clear();
}

//_________________________________________________________________
void AliMUON::SetChargeSlope(Int_t id, Float_t p1)
{
/// Set the inverse charge slope for chamber id

    Int_t i=2*(id-1);    //PH    ((AliMUONChamber*) (*fChambers)[i])->SetSigmaIntegration(p1);
    //PH    ((AliMUONChamber*) (*fChambers)[i+1])->SetSigmaIntegration(p1);
    ((AliMUONChamber*) fChambers->At(i))->SetChargeSlope(p1);
    ((AliMUONChamber*) fChambers->At(i+1))->SetChargeSlope(p1);
}
//__________________________________________________________________
void AliMUON::SetChargeSpread(Int_t id, Float_t p1, Float_t p2)
{
/// Set sigma of charge spread for chamber id

    Int_t i=2*(id-1);
    ((AliMUONChamber*) fChambers->At(i))->SetChargeSpread(p1,p2);
    ((AliMUONChamber*) fChambers->At(i+1))->SetChargeSpread(p1,p2);
}
//___________________________________________________________________
void AliMUON::SetSigmaIntegration(Int_t id, Float_t p1)
{
/// Set integration limits for charge spread
    Int_t i=2*(id-1);
    ((AliMUONChamber*) fChambers->At(i))->SetSigmaIntegration(p1);
    ((AliMUONChamber*) fChambers->At(i+1))->SetSigmaIntegration(p1);
}

//__________________________________________________________________
void AliMUON::SetMaxAdc(Int_t id, Int_t p1)
{
/// Set maximum number for ADCcounts (saturation)

    Int_t i=2*(id-1);
    ((AliMUONChamber*) fChambers->At(i))->SetMaxAdc(p1);
    ((AliMUONChamber*) fChambers->At(i+1))->SetMaxAdc(p1);
}

//__________________________________________________________________
void AliMUON::SetMaxStepGas(Float_t p1)
{
/// Set stepsize in gas

  fMaxStepGas=p1;
}
//__________________________________________________________________
void AliMUON::SetMaxStepAlu(Float_t p1)
{
/// Set step size in Alu

    fMaxStepAlu=p1;
}
//__________________________________________________________________
void AliMUON::SetMaxDestepGas(Float_t p1)
{
/// Set maximum step size in Gas

    fMaxDestepGas=p1;
}
//__________________________________________________________________
void AliMUON::SetMaxDestepAlu(Float_t p1)
{
/// Set maximum step size in Alu

  fMaxDestepAlu=p1;
}

//____________________________________________________________________
Float_t  AliMUON::GetMaxStepGas() const
{
/// Return stepsize in gas
  
  return fMaxStepGas;
}  

//____________________________________________________________________
Float_t  AliMUON::GetMaxStepAlu() const
{
/// Return step size in Alu
  
  return fMaxStepAlu;
}
  
//____________________________________________________________________
Float_t  AliMUON::GetMaxDestepGas() const
{
/// Return maximum step size in Gas
  
  return fMaxDestepGas;
}
  
//____________________________________________________________________
Float_t  AliMUON::GetMaxDestepAlu() const
{
/// Return maximum step size in Gas
  
  return fMaxDestepAlu;
}

//____________________________________________________________________
 void  AliMUON::SetAlign(Bool_t align)
{
/// Set option for alignement to geometry builder
 
   fGeometryBuilder->SetAlign(align);
}   

//____________________________________________________________________
 void  AliMUON::SetAlign(const TString& fileName, Bool_t align)
{
/// Set option for alignement to geometry builder
 
   fGeometryBuilder->SetAlign(fileName, align);
}   

//____________________________________________________________________
void   AliMUON::SetResponseModel(Int_t id, const AliMUONResponse& response)
{
/// Set the response for chamber id
    ((AliMUONChamber*) fChambers->At(id))->SetResponseModel(response);
}

//____________________________________________________________________
AliDigitizer* AliMUON::CreateDigitizer(AliRunDigitizer* manager) const
{
/// Return digitizer
  
  return new AliMUONDigitizerV3(manager, fDigitizerWithNoise);
}

//_____________________________________________________________________
void AliMUON::SDigits2Digits()
{
/// Write TreeD here only 

    char hname[30];
    //    sprintf(hname,"TreeD%d",fLoader->GetHeader()->GetEvent());
    fLoader->TreeD()->Write(hname,TObject::kOverwrite);
    fLoader->TreeD()->Reset();
}

//_____________________________________________________________________
void AliMUON::Hits2SDigits()
{
/// Perform Hits2Digits using SDigitizerV2
  
  AliMUONSDigitizerV2 sdigitizer;
  sdigitizer.ExecuteTask();
}

//_____________________________________________________________________
void AliMUON::Digits2Raw()
{
/// Convert digits of the current event to raw data

  if (!fRawWriter)
  {
    fRawWriter = new AliMUONRawWriter;
    AliDebug(1,Form("Creating %s",fRawWriter->ClassName()));
    if (fTriggerScalerEvent == kTRUE) 
    {
      fRawWriter->SetScalersNumbers();
    }
  }
  
  fLoader->LoadDigits("READ");
  
  TTree* treeD = fLoader->TreeD();
  
  if (!treeD)
  {
    AliError("Could not get TreeD");
    return;
  }
  
  AliMUONVTriggerStore*  triggerStore = AliMUONVTriggerStore::Create(*treeD);
  AliMUONVDigitStore* digitStore = AliMUONVDigitStore::Create(*treeD);

  triggerStore->Connect(*treeD,kFALSE);
  digitStore->Connect(*treeD,kFALSE);
  
  treeD->GetEvent(0);
  
  if (!fRawWriter->Digits2Raw(digitStore,triggerStore))
  {
    AliError("pb writting raw data");
  }
  
  delete triggerStore;
  delete digitStore;
  
  fLoader->UnloadDigits();
}

//_____________________________________________________________________
Bool_t AliMUON::Raw2SDigits(AliRawReader* rawReader)
{
/// Convert  raw data to SDigit
/// Only for tracking for the moment (ChF) 

  fLoader->LoadDigits("READ");
  if (!fLoader->TreeS()) fLoader->MakeSDigitsContainer();

  TTree* treeS = fLoader->TreeS();
  
  AliMUONVDigitStore* sDigitStore = new AliMUONDigitStoreV1;
  sDigitStore->Connect(*treeS);
  
  fDigitMaker->Raw2Digits(rawReader,sDigitStore,0x0);

  fLoader->WriteSDigits("OVERWRITE");

  fLoader->UnloadSDigits();

  delete sDigitStore;
  
  return kTRUE;
}

//_______________________________________________________________________
AliLoader* AliMUON::MakeLoader(const char* topfoldername)
{ 
/// Build standard getter (AliLoader type);
/// if detector wants to use castomized getter, it must overload this method
 
 AliDebug(1,Form("Creating standard getter for detector %s. Top folder is %s.",
         GetName(),topfoldername));
 fLoader   = new AliLoader(GetName(),topfoldername);

 return fLoader;
}

//________________________________________________________________________
void
AliMUON::ResetGeometryBuilder()
{
/// Only to be used by "experts" wanting to change the geometry builders
/// to be used. 
/// As the ctor of AliMUON now defines a default geometrybuilder, this
/// ResetGeometryBuilder() must be called prior to call the 
/// AddGeometryBuilder()

  delete fGeometryBuilder;
  fGeometryBuilder = new AliMUONGeometryBuilder(this);
  fGeometryBuilder
    ->AddBuilder(new AliMUONCommonGeometryBuilder(this));
}

//____________________________________________________________________
Bool_t  AliMUON::GetTriggerResponseV1() const
{
///
/// Returns fTriggerResponseV1
///  
    return fTriggerResponseV1;
    
}  

//____________________________________________________________________
Int_t  AliMUON::GetTriggerCoinc44() const
{
///
/// Returns fTriggerCoinc44
///  
    return fTriggerCoinc44;
    
}

//____________________________________________________________________
Bool_t  AliMUON::GetTriggerEffCells() const
{
///
/// Returns fTriggerEffCells
///  
    return fTriggerEffCells;
    
}  

//____________________________________________________________________
Int_t  AliMUON::GetDigitizerWithNoise() const
{
///
/// Returns fDigitizerWithNoise
///  
    return fDigitizerWithNoise;
    
}  

//____________________________________________________________________
void AliMUON::SetFastDecoder(Bool_t useFastDecoder)
{
/// Set fast raw data decoder 

  if ( ! fDigitMaker ) {
    AliError("Digit maker is not instantiated.");
    return;
  }   

  fDigitMaker->SetFastDecoder(useFastDecoder);
}  
