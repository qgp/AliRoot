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

//
//  Base class for detectors quality assurance checkers 
//  Compares Data made by QADataMakers with reference data
//  Y. Schutz CERN August 2007
//

// --- ROOT system ---
#include <TCanvas.h>
#include <TClass.h>
#include <TH1F.h> 
#include <TH1I.h> 
#include <TIterator.h> 
#include <TKey.h> 
#include <TFile.h> 
#include <TList.h>
#include <TNtupleD.h>
#include <TParameter.h>
#include <TPaveText.h>

// --- Standard library ---

// --- AliRoot header files ---
#include "AliCDBEntry.h"
#include "AliLog.h"
#include "AliQAv1.h"
#include "AliQAChecker.h"
#include "AliQACheckerBase.h"
#include "AliQADataMaker.h"
#include "AliQAManager.h"
#include "AliDetectorRecoParam.h"

ClassImp(AliQACheckerBase)

           
//____________________________________________________________________________ 
AliQACheckerBase::AliQACheckerBase(const char * name, const char * title) : 
  TNamed(name, title), 
  fDataSubDir(0x0),
  fRefSubDir(0x0), 
  fRefOCDBSubDir(new TObjArray*[AliRecoParam::kNSpecies]), 
  fLowTestValue(0x0),
  fUpTestValue(0x0),
  fImage(new TCanvas*[AliRecoParam::kNSpecies]), 
  fPrintImage(kTRUE), 
  fExternParamList(new TList())
{
  // ctor
  fLowTestValue = new Float_t[AliQAv1::kNBIT] ; 
  fUpTestValue  = new Float_t[AliQAv1::kNBIT] ; 
  fLowTestValue[AliQAv1::kINFO]    =  0.5   ; 
  fUpTestValue[AliQAv1::kINFO]     = 1.0 ; 
  fLowTestValue[AliQAv1::kWARNING] =  0.002 ; 
  fUpTestValue[AliQAv1::kWARNING]  = 0.5 ; 
  fLowTestValue[AliQAv1::kERROR]   =  0.0   ; 
  fUpTestValue[AliQAv1::kERROR]    = 0.002 ; 
  fLowTestValue[AliQAv1::kFATAL]   = -1.0   ; 
  fUpTestValue[AliQAv1::kFATAL]    = 0.0 ; 
  
  AliDebug(AliQAv1::GetQADebugLevel(), "Default setting is:") ;
  if ( AliDebugLevel()  == AliQAv1::GetQADebugLevel() ) {
    const Char_t * text= Form(" INFO    -> %1.5f <  value <  %1.5f  WARNING -> %1.5f <  value <= %1.5f \n ERROR   -> %1.5f <  value <= %1.5f \n FATAL   -> %1.5f <= value <  %1.5f \n", 
                              fLowTestValue[AliQAv1::kINFO], fUpTestValue[AliQAv1::kINFO], 
                              fLowTestValue[AliQAv1::kWARNING], fUpTestValue[AliQAv1::kWARNING], 
                              fLowTestValue[AliQAv1::kERROR], fUpTestValue[AliQAv1::kERROR], 
                              fLowTestValue[AliQAv1::kFATAL], fUpTestValue[AliQAv1::kFATAL]) ; 
    AliInfo(Form("%s", text)) ; 
  }
  
  for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {
    fImage[specie] = NULL ; 
    fRefOCDBSubDir[specie] = NULL ;
  }
}

//____________________________________________________________________________ 
AliQACheckerBase::AliQACheckerBase(const AliQACheckerBase& qac) :
  TNamed(qac.GetName(), qac.GetTitle()),
  fDataSubDir(qac.fDataSubDir), 
  fRefSubDir(qac.fRefSubDir), 
  fRefOCDBSubDir(qac.fRefOCDBSubDir), 
  fLowTestValue(qac.fLowTestValue),
  fUpTestValue(qac.fLowTestValue), 
  fImage(NULL),  
  fPrintImage(kTRUE), 
  fExternParamList(new TList())  
{
  //copy ctor
  for (Int_t index = 0 ; index < AliQAv1::kNBIT ; index++) {
    fLowTestValue[index]  = qac.fLowTestValue[index] ; 
    fUpTestValue[index] = qac.fUpTestValue[index] ; 
  }
    for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {
      fImage[specie] = qac.fImage[specie] ; 
      fRefOCDBSubDir[specie] = qac.fRefOCDBSubDir[specie] ; 
    }
  if (qac.fExternParamList) {
    TIter next(qac.fExternParamList) ; 
    TParameter<double> * p ; 
    while ( (p = (TParameter<double>*)next()) )
      fExternParamList->Add(p) ;
  }
}

//____________________________________________________________________________
AliQACheckerBase& AliQACheckerBase::operator = (const AliQACheckerBase& qac )
{
  // Equal operator.
  this->~AliQACheckerBase();
  new(this) AliQACheckerBase(qac);
  return *this;
}

//____________________________________________________________________________ 
AliQACheckerBase::~AliQACheckerBase()
{
  delete [] fLowTestValue ; 
  delete [] fUpTestValue ; 
  for (Int_t esIndex = 0 ; esIndex < AliRecoParam::kNSpecies ; esIndex++) {
    if ( fImage[esIndex] ) 
      delete fImage[esIndex] ;
    if ( fRefOCDBSubDir[esIndex] ) 
      delete fRefOCDBSubDir[esIndex] ; 
  }
  delete[] fImage ; 
  delete[] fRefOCDBSubDir ; 
  AliQAv1::GetQAResultFile()->Close() ; 
  if (fExternParamList) {
    fExternParamList->Clear() ; 
    delete fExternParamList ; 
  }
}

//____________________________________________________________________________
Double_t * AliQACheckerBase::Check(AliQAv1::ALITASK_t index, AliDetectorRecoParam * recoParam) 
{
  // Performs a basic checking
  // Compares all the histograms stored in the directory
  // With reference histograms either in a file of in OCDB  

  TObjArray ** list = new TObjArray *[AliRecoParam::kNSpecies] ; 
  Int_t specie ;
  for (specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {
    list[specie] =  new TObjArray(AliQAv1::GetMaxQAObj()) ; 
    if ( !AliQAv1::Instance()->IsEventSpecieSet(specie) ) 
      continue ; 
    if (fDataSubDir) {
      TList * keyList = fDataSubDir->GetListOfKeys() ; 
      TIter next(keyList) ; 
      TKey * key ;
      while ( (key = static_cast<TKey *>(next())) ) {
        TDirectory * specieDir = fDataSubDir->GetDirectory(key->GetName()) ; 
        TList * keykeyList = specieDir->GetListOfKeys() ; 
        TIter next2(keykeyList) ; 
        TKey * keykey ;
        while ( (keykey = static_cast<TKey *>(next2())) ) {
          TObject * odata = specieDir->Get(keykey->GetName()) ; 
          if ( odata->IsA()->InheritsFrom("TH1") ) {
            TH1 * hdata = static_cast<TH1*>(odata) ;
            list[specie]->Add(hdata) ; 
          } else if (!odata->IsA()->InheritsFrom("TDirectory")) // skip the expert directory
            AliError(Form("%s Is a Classname that cannot be processed", key->GetClassName())) ;
        }
      }
    }
  }
 
  Double_t * test = Check(index, list, recoParam) ;
  
  delete[] list ; 
    
  return test ;
}  

//____________________________________________________________________________
Double_t * AliQACheckerBase::Check(AliQAv1::ALITASK_t task, TObjArray ** list, AliDetectorRecoParam * /*recoParam*/) 
{
  // Performs a basic checking
  // Compares all the histograms in the list

	Double_t * test = new Double_t[AliRecoParam::kNSpecies] ;
	Int_t count[AliRecoParam::kNSpecies]   = { 0 }; 

//  TDirectory * refDir     = NULL ; 
//	TObjArray ** refOCDBDir = NULL  ;	
  GetRefSubDir(GetName(), AliQAv1::GetTaskName(task), fRefSubDir, fRefOCDBSubDir) ;
 // SetRefandData(refDir, refOCDBDir) ; 
  
  for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {
    test[specie] = 1.0 ; 
    if ( !AliQAv1::Instance()->IsEventSpecieSet(specie)) 
      continue ; 
    if (list[specie]->GetEntries() == 0)  
      test[specie] = 0. ; // nothing to check
    else {
      if (!fRefSubDir && !fRefOCDBSubDir)
        test[specie] = -1 ; // no reference data
      else {
        TIter next(list[specie]) ; 
        TH1 * hdata ;
        count[specie] = 0 ; 
        while ( (hdata = static_cast<TH1 *>(next())) ) {
          if ( hdata->IsA()->InheritsFrom("TH1") ) {
            if ( hdata->TestBit(AliQAv1::GetExpertBit()) )  // does not perform the test for expert data
              continue ; 
            TH1 * href = NULL ; 
            if (fRefSubDir) 
              href  = static_cast<TH1*>(fRefSubDir->Get(hdata->GetName())) ;
            else if (fRefOCDBSubDir[specie])
              href  = static_cast<TH1*>(fRefOCDBSubDir[specie]->FindObject(hdata->GetName())) ;
            if (!href) 
              test[specie] = -1 ; // no reference data ; 
            else {
              Double_t rv =  DiffK(hdata, href) ;
              AliDebug(AliQAv1::GetQADebugLevel(), Form("%s ->Test = %f", hdata->GetName(), rv)) ; 
              test[specie] += rv ; 
              count[specie]++ ;
            }
          } else
            AliError("Data type cannot be processed") ;
          if (count[specie] != 0) 
            test[specie] /= count[specie] ;
        }
      }
    }
  }
  return test ;
}  


//____________________________________________________________________________ 
Double_t AliQACheckerBase::DiffC(const TH1 * href, const TH1 * hin) const
{
  // compares two histograms using the Chi2 test
  if ( hin->Integral() == 0 ) {
    AliDebug(AliQAv1::GetQADebugLevel(), Form("Spectrum %s is empty", hin->GetName())) ; 
    return 0. ;
  }
    
  return hin->Chi2Test(href) ;  
}

//____________________________________________________________________________ 
Double_t AliQACheckerBase::DiffK(const TH1 * href, const TH1 * hin) const
{
  // compares two histograms using the Kolmogorov test
  if ( hin->Integral() == 0 || href->Integral() == 0) {
    AliDebug(AliQAv1::GetQADebugLevel(), Form("Spectrum %s or its reference is empty", hin->GetName())) ; 
    return 0. ;
  }
    
  return hin->KolmogorovTest(href) ;  
}

  //_____________________________________________________________________________
void AliQACheckerBase::GetRefSubDir(const char * det, const char * task, TDirectory *& dirFile, TObjArray **& dirOCDB)     
{ 
    // Opens and returns the file with the reference data 
  dirFile = NULL ; 
  TString refStorage(AliQAv1::GetQARefStorage()) ;
  if (!refStorage.Contains(AliQAv1::GetLabLocalOCDB()) && !refStorage.Contains(AliQAv1::GetLabAliEnOCDB())) {
    AliError(Form("%s is not a valid location for reference data", refStorage.Data())) ; 
    return ; 
  } else {
    AliQAManager* manQA = AliQAManager::QAManager(AliQAv1::GetTaskIndex(task)) ;
    dirOCDB = new TObjArray*[AliRecoParam::kNSpecies] ;	
    for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {
      dirOCDB[specie] = NULL ; 
      if ( !AliQAv1::Instance()->IsEventSpecieSet(specie) ) 
        continue ; 
      AliQAv1::SetQARefDataDirName(specie) ;
      if ( ! manQA->GetLock() ) { 
        manQA->SetDefaultStorage(AliQAv1::GetQARefStorage()) ; 
        manQA->SetSpecificStorage("*", AliQAv1::GetQARefStorage()) ;
        manQA->SetRun(AliCDBManager::Instance()->GetRun()) ; 
        manQA->SetLock() ; 
      }
      char * detOCDBDir = Form("%s/%s/%s", det, AliQAv1::GetRefOCDBDirName(), AliQAv1::GetRefDataDirName()) ; 
      AliCDBEntry * entry = manQA->Get(detOCDBDir, manQA->GetRun()) ;
      if (entry) {
        TList * listDetQAD =static_cast<TList *>(entry->GetObject()) ;
        if ( strcmp(listDetQAD->ClassName(), "TList") != 0 ) {
          AliError(Form("Expected a Tlist and found a %s for detector %s", listDetQAD->ClassName(), det)) ; 
          listDetQAD = NULL ; 
          continue ; 
        } 
        if ( listDetQAD ) {
          TIter next(listDetQAD) ;
          TObjArray * ar ; 
          while ( (ar = (TObjArray*)next()) ) 
            dirOCDB[specie] = static_cast<TObjArray *>(listDetQAD->FindObject(Form("%s/%s", task, AliRecoParam::GetEventSpecieName(specie)))) ;             
        }
      }
    }
  }
}

//____________________________________________________________________________
void AliQACheckerBase::PrintExternParam() 
{
    // Print the list of external parameter list
  TIter next(fExternParamList) ; 
  TParameter<double> *pp ; 
  TString printit("\n") ;
  while( (pp = (TParameter<double>*)next()) )
    printit += Form("%s = %f\n", pp->GetName(), pp->GetVal());  
  AliInfo(Form("%s", printit.Data())) ;
}
  
//____________________________________________________________________________
void AliQACheckerBase::Run(AliQAv1::ALITASK_t index, AliDetectorRecoParam * recoParam) 
{ 
	AliDebug(AliQAv1::GetQADebugLevel(), Form("Processing %s", AliQAv1::GetAliTaskName(index))) ; 
  
	Double_t * rv = NULL ;
  rv = Check(index, recoParam) ;
	SetQA(index, rv) ; 	
	
  AliDebug(AliQAv1::GetQADebugLevel(), Form("Test result of %s", AliQAv1::GetAliTaskName(index))) ;
	
  if (rv) 
    delete [] rv ; 
  Finish() ; 
}

//____________________________________________________________________________
void AliQACheckerBase::Run(AliQAv1::ALITASK_t index, TObjArray ** list, AliDetectorRecoParam * recoParam) 
{ 
	AliDebug(AliQAv1::GetQADebugLevel(), Form("Processing %s", AliQAv1::GetAliTaskName(index))) ; 
  
	Double_t * rv = NULL ;
  rv = Check(index, list, recoParam) ;
	SetQA(index, rv) ; 	
	
  AliDebug(AliQAv1::GetQADebugLevel(), Form("Test result of %s", AliQAv1::GetAliTaskName(index))) ;
	
  if (rv) 
    delete [] rv ; 
  Finish() ; 
}

//____________________________________________________________________________
void AliQACheckerBase::Finish() const 
{
	// wrap up and save QA in proper file
	AliQAv1::GetQAResultFile() ; 
	AliQAv1 * qa = AliQAv1::Instance() ; 
	qa->Write(AliQAv1::GetQAName(), kWriteDelete) ;   
}

//____________________________________________________________________________ 
void AliQACheckerBase::MakeImage( TObjArray ** list, AliQAv1::TASKINDEX_t task, AliQAv1::MODE_t mode) 
{
  // makes the QA image for sim and rec
  Int_t nImages = 0 ;
  for (Int_t esIndex = 0 ; esIndex < AliRecoParam::kNSpecies ; esIndex++) {
    if (! AliQAv1::Instance(AliQAv1::GetDetIndex(GetName()))->IsEventSpecieSet(AliRecoParam::ConvertIndex(esIndex)) ) 
      continue ;
    TIter next(list[esIndex]) ;  
    TH1 * hdata = NULL ; 
    while ( (hdata=static_cast<TH1 *>(next())) ) {
      TString cln(hdata->ClassName()) ; 
      if ( ! cln.Contains("TH") )
        continue ; 
      if ( hdata->TestBit(AliQAv1::GetImageBit()) )
        nImages++; 
    }
    break ; 
  }
  if ( nImages == 0 ) {
    AliDebug(AliQAv1::GetQADebugLevel(), Form("No histogram will be plotted for %s %s\n", GetName(), AliQAv1::GetTaskName(task).Data())) ;  
  } else {
    AliDebug(AliQAv1::GetQADebugLevel(), Form("%d histograms will be plotted for %s %s\n", nImages, GetName(), AliQAv1::GetTaskName(task).Data())) ;  
    for (Int_t esIndex = 0 ; esIndex < AliRecoParam::kNSpecies ; esIndex++) {
      if (! AliQAv1::Instance(AliQAv1::GetDetIndex(GetName()))->IsEventSpecieSet(AliRecoParam::ConvertIndex(esIndex)) || list[esIndex]->GetEntries() == 0) 
        continue ;
      const Char_t * title = Form("QA_%s_%s_%s", GetName(), AliQAv1::GetTaskName(task).Data(), AliRecoParam::GetEventSpecieName(esIndex)) ; 
      if ( !fImage[esIndex] ) {
        fImage[esIndex] = new TCanvas(title, title) ;
      }
      fImage[esIndex]->Clear() ; 
      fImage[esIndex]->SetTitle(title) ; 
      fImage[esIndex]->cd() ; 
      TPaveText someText(0.015, 0.015, 0.98, 0.98) ;
      someText.AddText(title) ;
      someText.Draw() ; 
      fImage[esIndex]->Print(Form("%s%s%d.%s", AliQAv1::GetImageFileName(), AliQAv1::GetModeName(mode), AliQAChecker::Instance()->GetRunNumber(), AliQAv1::GetImageFileFormat()), "ps") ; 
      fImage[esIndex]->Clear() ; 
      Int_t nx = TMath::Sqrt(nImages) ; 
      Int_t ny = nx  ; 
      while ( nx*ny <= nImages) 
        ny++ ; 
      
      fImage[esIndex]->Divide(nx, ny) ; 
      TIter nexthist(list[esIndex]) ; 
      TH1* hist = NULL ;
      Int_t npad = 1 ; 
      fImage[esIndex]->cd(npad) ; 
      while ( (hist=static_cast<TH1*>(nexthist())) ) {
        TString cln(hist->ClassName()) ; 
        if ( ! cln.Contains("TH") )
          continue ; 
        if(hist->TestBit(AliQAv1::GetImageBit())) {
          hist->Draw() ; 
          fImage[esIndex]->cd(++npad) ; 
        }
      }
      fImage[esIndex]->Print(Form("%s%s%d.%s", AliQAv1::GetImageFileName(), AliQAv1::GetModeName(mode), AliQAChecker::Instance()->GetRunNumber(), AliQAv1::GetImageFileFormat()), "ps") ; 
    }
  }  
}

//____________________________________________________________________________
void AliQACheckerBase::SetHiLo(Float_t * hiValue, Float_t * lowValue) 
{
  AliDebug(AliQAv1::GetQADebugLevel(), "Previous setting was:") ;
  if ( AliDebugLevel() == AliQAv1::GetQADebugLevel() ) {
    const Char_t * text= Form(" INFO    -> %1.5f <  value <  %1.5f  WARNING -> %1.5f <  value <= %1.5f \n ERROR   -> %1.5f <  value <= %1.5f \n FATAL   -> %1.5f <= value <  %1.5f \n", 
                              fLowTestValue[AliQAv1::kINFO], fUpTestValue[AliQAv1::kINFO], 
                              fLowTestValue[AliQAv1::kWARNING], fUpTestValue[AliQAv1::kWARNING], 
                              fLowTestValue[AliQAv1::kERROR], fUpTestValue[AliQAv1::kERROR], 
                              fLowTestValue[AliQAv1::kFATAL], fUpTestValue[AliQAv1::kFATAL]) ; 
    AliInfo(Form("%s", text)) ; 
  }
  
  for (Int_t index = 0 ; index < AliQAv1::kNBIT ; index++) {
    fLowTestValue[index]  = lowValue[index] ; 
    fUpTestValue[index]   = hiValue[index] ; 
  }
  AliDebug(AliQAv1::GetQADebugLevel(), "Current setting is:") ;
  if ( AliDebugLevel()  == AliQAv1::GetQADebugLevel() ) {
    const Char_t * text= Form(" INFO    -> %1.5f <  value <  %1.5f  WARNING -> %1.5f <  value <= %1.5f \n ERROR   -> %1.5f <  value <= %1.5f \n FATAL   -> %1.5f <= value <  %1.5f \n", 
                              fLowTestValue[AliQAv1::kINFO], fUpTestValue[AliQAv1::kINFO], 
                              fLowTestValue[AliQAv1::kWARNING], fUpTestValue[AliQAv1::kWARNING], 
                              fLowTestValue[AliQAv1::kERROR], fUpTestValue[AliQAv1::kERROR], 
                              fLowTestValue[AliQAv1::kFATAL], fUpTestValue[AliQAv1::kFATAL]) ;     AliInfo(Form("%s", text)) ; 
  }
}

//____________________________________________________________________________
void AliQACheckerBase::SetQA(AliQAv1::ALITASK_t index, Double_t * value) const
{
	// sets the QA according the return value of the Check

  AliQAv1 * qa = AliQAv1::Instance(index) ;

  for (Int_t specie = 0 ; specie < AliRecoParam::kNSpecies ; specie++) {
    if (! qa->IsEventSpecieSet(AliRecoParam::ConvertIndex(specie)))
      continue ;
    if (  value == NULL ) { // No checker is implemented, set all QA to Fatal
      qa->Set(AliQAv1::kFATAL, specie) ; 
    } else {
      if ( value[specie] >= fLowTestValue[AliQAv1::kFATAL] && value[specie] < fUpTestValue[AliQAv1::kFATAL] ) 
        qa->Set(AliQAv1::kFATAL, AliRecoParam::ConvertIndex(specie)) ; 
      else if ( value[specie] > fLowTestValue[AliQAv1::kERROR] && value[specie] <= fUpTestValue[AliQAv1::kERROR]  )
        qa->Set(AliQAv1::kERROR, AliRecoParam::ConvertIndex(specie)) ; 
      else if ( value[specie] > fLowTestValue[AliQAv1::kWARNING] && value[specie] <= fUpTestValue[AliQAv1::kWARNING]  )
        qa->Set(AliQAv1::kWARNING, AliRecoParam::ConvertIndex(specie)) ;
      else if ( value[specie] > fLowTestValue[AliQAv1::kINFO] && value[specie] <= fUpTestValue[AliQAv1::kINFO] ) 
        qa->Set(AliQAv1::kINFO, AliRecoParam::ConvertIndex(specie)) ; 	
    }
  }
}
