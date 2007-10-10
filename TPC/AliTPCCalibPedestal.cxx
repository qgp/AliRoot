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


//Root includes
#include <TH1F.h>
#include <TH2F.h>
#include <TString.h>
#include <TMath.h>
#include <TF1.h>
#include <TRandom.h>
#include <TDirectory.h>
#include <TFile.h>
//AliRoot includes
#include "AliRawReader.h"
#include "AliRawReaderRoot.h"
#include "AliRawReaderDate.h"
#include "AliTPCRawStream.h"
#include "AliTPCCalROC.h"
#include "AliTPCROC.h"
#include "AliMathBase.h"
#include "TTreeStream.h"

//date
#include "event.h"

//header file
#include "AliTPCCalibPedestal.h"


///////////////////////////////////////////////////////////////////////////////////////
//          Implementation of the TPC pedestal and noise calibration
//
//   Origin: Jens Wiechula, Marian Ivanov   J.Wiechula@gsi.de, Marian.Ivanov@cern.ch
// 
// 
// *************************************************************************************
// *                                Class Description                                  *
// *************************************************************************************
//
// Working principle:
// ------------------
// Raw pedestal data is processed by calling one of the ProcessEvent(...) functions
// (see below). These in the end call the Update(...) function, where the data is filled
// into histograms.
//
// For each ROC one TH2F histo (ROC channel vs. ADC channel) is created when
// it is filled for the first time (GetHistoPedestal(ROC,kTRUE)). All histos are stored in the
// TObjArray fHistoPedestalArray.
//
// For a fast filling of the histogram the corresponding bin number of the channel and ADC channel
// is computed by hand and the histogram array is accessed directly via its pointer.
// ATTENTION: Doing so the the entry counter of the histogram is not increased
//            this means that e.g. the colz draw option gives an empty plot unless
//	    calling 'histo->SetEntries(1)' before drawing.
//
// After accumulating the desired statistics the Analyse() function has to be called.
// Whithin this function the pedestal and noise values are calculated for each pad, using
// the fast gaus fit function  AliMathBase::FitGaus(...), and the calibration
// storage classes (AliTPCCalROC) are filled for each ROC.
// The calibration information is stored in the TObjArrays fCalRocArrayPedestal and fCalRocArrayRMS;
//
//
//
// User interface for filling data:
// --------------------------------
//
// To Fill information one of the following functions can be used:
//
// Bool_t ProcessEvent(eventHeaderStruct *event);
//   - process Date event
//   - use AliTPCRawReaderDate and call ProcessEvent(AliRawReader *rawReader)
//
// Bool_t ProcessEvent(AliRawReader *rawReader);
//  - process AliRawReader event
//   - use AliTPCRawStream to loop over data and call ProcessEvent(AliTPCRawStream *rawStream)
//
// Bool_t ProcessEvent(AliTPCRawStream *rawStream);
//   - process event from AliTPCRawStream
//   - call Update function for signal filling
//
// Int_t Update(const Int_t isector, const Int_t iRow, const Int_t
//              iPad,  const Int_t iTimeBin, const Float_t signal);
//   - directly  fill signal information (sector, row, pad, time bin, pad)
//     to the reference histograms
//
// It is also possible to merge two independently taken calibrations using the function
//
// void Merge(AliTPCCalibPedestal *ped)
//   - copy histograms in 'ped' if the do not exist in this instance
//   - Add histograms in 'ped' to the histograms in this instance if the allready exist
//   - After merging call Analyse again!
//
//
//
// -- example: filling data using root raw data:
// void fillPedestal(Char_t *filename)
// {
//    rawReader = new AliRawReaderRoot(fileName);
//    if ( !rawReader ) return;
//    AliTPCCalibPedestal *calib = new AliTPCCalibPedestal;
//    while (rawReader->NextEvent()){
//      calib->ProcessEvent(rawReader);
//    }
//    calib->Analyse();
//    calib->DumpToFile("PedestalData.root");
//    delete rawReader;
//    delete calib;
// }
//
//
// What kind of information is stored and how to retrieve them:
// ------------------------------------------------------------
//
// - Accessing the 'Reference Histograms' (pedestal distribution histograms):
//
// TH2F *GetHistoPedestal(Int_t sector);
//
// - Accessing the calibration storage objects:
//
// AliTPCCalROC *GetCalRocPedestal(Int_t sector);  - for the pedestal values
// AliTPCCalROC *GetCalRocNoise(Int_t sector);     - for the Noise values
//
// example for visualisation:
// if the file "PedestalData.root" was created using the above example one could do the following:
//
// TFile filePedestal("PedestalData.root")
// AliTPCCalibPedestal *ped = (AliTPCCalibPedestal*)filePedestal->Get("AliTPCCalibPedestal");
// ped->GetCalRocPedestal(0)->Draw("colz");
// ped->GetCalRocRMS(0)->Draw("colz");
//
// or use the AliTPCCalPad functionality:
// AliTPCCalPad padPedestal(ped->GetCalPadPedestal());
// AliTPCCalPad padNoise(ped->GetCalPadRMS());
// padPedestal->MakeHisto2D()->Draw("colz");  //Draw A-Side Pedestal Information
// padNoise->MakeHisto2D()->Draw("colz");  //Draw A-Side Noise Information
//
/*
 example: fill pedestal with gausschen noise
 AliTPCCalibPedestal ped;
 ped.TestEvent();
 ped.Analyse();
 //Draw output;
 TCanvas* c1 = new TCanvas;
 c1->Divide(1,2);
 c1->cd(1);
 ped.GetHistoPedestal(0)->SetEntries(1); //needed in order for colz to work, reason: fast filling does not increase the entries counter
 ped.GetHistoPedestal(0)->Draw("colz");
 c1->cd(2);
 ped.GetHistoPedestal(36)->SetEntries(1); //needed in order for colz to work, reason: fast filling does not increase the entries counter
 ped.GetHistoPedestal(36)->Draw("colz");
 TCanvas* c2 = new TCanvas;
 c2->Divide(2,2);
 c2->cd(1);
 ped.GetCalRocPedestal(0)->Draw("colz");
 c2->cd(2);
 ped.GetCalRocRMS(0)->Draw("colz");
 c2->cd(3);
 ped.GetCalRocPedestal(36)->Draw("colz");
 c2->cd(4);
 ped.GetCalRocRMS(36)->Draw("colz");


*/



ClassImp(AliTPCCalibPedestal)

AliTPCCalibPedestal::AliTPCCalibPedestal() : /*FOLD00*/
  TObject(),
  fFirstTimeBin(60),
  fLastTimeBin(1000),
  fAdcMin(1),
  fAdcMax(100),
  fOldRCUformat(kTRUE),
  fROC(AliTPCROC::Instance()),
  fCalRocArrayPedestal(72),
  fCalRocArrayRMS(72),
  fHistoPedestalArray(72)
{
    //
    // default constructor
    //
}
//_____________________________________________________________________
AliTPCCalibPedestal::AliTPCCalibPedestal(const AliTPCCalibPedestal &ped) : /*FOLD00*/
  TObject(ped),
  fFirstTimeBin(ped.GetFirstTimeBin()),
  fLastTimeBin(ped.GetLastTimeBin()),
  fAdcMin(ped.GetAdcMin()),
  fAdcMax(ped.GetAdcMax()),
  fOldRCUformat(kTRUE),
  fROC(AliTPCROC::Instance()),
  fCalRocArrayPedestal(72),
  fCalRocArrayRMS(72),
  fHistoPedestalArray(72)
{
    //
    // copy constructor
    //
    for (Int_t iSec = 0; iSec < 72; ++iSec){
	const AliTPCCalROC *calPed = (AliTPCCalROC*)ped.fCalRocArrayPedestal.UncheckedAt(iSec);
	const AliTPCCalROC *calRMS = (AliTPCCalROC*)ped.fCalRocArrayRMS.UncheckedAt(iSec);
	const TH2F         *hPed   = (TH2F*)ped.fHistoPedestalArray.UncheckedAt(iSec);

	if ( calPed != 0x0 ) fCalRocArrayPedestal.AddAt(new AliTPCCalROC(*calPed), iSec);
	if ( calRMS != 0x0 ) fCalRocArrayRMS.AddAt(new AliTPCCalROC(*calRMS), iSec);

	if ( hPed != 0x0 ){
	    TH2F *hNew = new TH2F(*hPed);
	    hNew->SetDirectory(0);
	    fHistoPedestalArray.AddAt(hNew,iSec);
	}
    }
}
//_____________________________________________________________________
AliTPCCalibPedestal& AliTPCCalibPedestal::operator = (const  AliTPCCalibPedestal &source)
{
  //
  // assignment operator
  //
  if (&source == this) return *this;
  new (this) AliTPCCalibPedestal(source);

  return *this;
}
//_____________________________________________________________________
AliTPCCalibPedestal::~AliTPCCalibPedestal() /*FOLD00*/
{
  //
  // destructor
  //

    fCalRocArrayPedestal.Delete();
    fCalRocArrayRMS.Delete();
    fHistoPedestalArray.Delete();
    delete fROC;
}
//_____________________________________________________________________
Int_t AliTPCCalibPedestal::Update(const Int_t icsector, /*FOLD00*/
				const Int_t icRow,
				const Int_t icPad,
				const Int_t icTimeBin,
				const Float_t csignal)
{
    //
    // Signal filling methode 
    //

    //return if we are out of the specified time bin or adc range
    if ( (icTimeBin>fLastTimeBin) || (icTimeBin<fFirstTimeBin) ) return 0;
    if ( ((Int_t)csignal>fAdcMax) || ((Int_t)csignal<fAdcMin)  ) return 0;

    Int_t iChannel  = fROC->GetRowIndexes(icsector)[icRow]+icPad; //  global pad position in sector

    // fast filling methode.
    // Attention: the entry counter of the histogram is not increased
    //            this means that e.g. the colz draw option gives an empty plot
    Int_t bin = (iChannel+1)*(fAdcMax-fAdcMin+2)+((Int_t)csignal-fAdcMin+1);

    GetHistoPedestal(icsector,kTRUE)->GetArray()[bin]++;

    return 0;
}
//_____________________________________________________________________
Bool_t AliTPCCalibPedestal::ProcessEvent(AliTPCRawStream *rawStream)
{
  //
  // Event Processing loop - AliTPCRawStream
  //

  rawStream->SetOldRCUFormat(fOldRCUformat);

  Bool_t withInput = kFALSE;

  while (rawStream->Next()) {

      Int_t isector  = rawStream->GetSector();                       //  current sector
      Int_t iRow     = rawStream->GetRow();                          //  current row
      Int_t iPad     = rawStream->GetPad();                          //  current pad
      Int_t iTimeBin = rawStream->GetTime();                         //  current time bin
      Float_t signal = rawStream->GetSignal();                       //  current ADC signal

      Update(isector,iRow,iPad,iTimeBin,signal);
      withInput = kTRUE;
  }

  return withInput;
}
//_____________________________________________________________________
Bool_t AliTPCCalibPedestal::ProcessEvent(AliRawReader *rawReader)
{
  //
  //  Event processing loop - AliRawReader
  //


  AliTPCRawStream rawStream(rawReader);

  rawReader->Select("TPC");

  return ProcessEvent(&rawStream);
}
//_____________________________________________________________________
Bool_t AliTPCCalibPedestal::ProcessEvent(eventHeaderStruct *event)
{
  //
  //  process date event
  //
    AliRawReader *rawReader = new AliRawReaderDate((void*)event);
    Bool_t result=ProcessEvent(rawReader);
    delete rawReader;
    return result;
}
//_____________________________________________________________________
Bool_t AliTPCCalibPedestal::TestEvent() /*FOLD00*/
{
  //
  //  Test event loop
  // fill one oroc and one iroc with random gaus
  //

    gRandom->SetSeed(0);

    for (UInt_t iSec=0; iSec<72; ++iSec){
        if (iSec%36>0) continue;
	for (UInt_t iRow=0; iRow < fROC->GetNRows(iSec); ++iRow){
	    for (UInt_t iPad=0; iPad < fROC->GetNPads(iSec,iRow); ++iPad){
		for (UInt_t iTimeBin=0; iTimeBin<1024; ++iTimeBin){
		    Float_t signal=(Int_t)(iRow+3+gRandom->Gaus(0,.7));
		    if ( signal>0 )Update(iSec,iRow,iPad,iTimeBin,signal);
		}
	    }
	}
    }
    return kTRUE;
}
//_____________________________________________________________________
TH2F* AliTPCCalibPedestal::GetHisto(Int_t sector, TObjArray *arr, /*FOLD00*/
				  Int_t nbinsY, Float_t ymin, Float_t ymax,
				  Char_t *type, Bool_t force)
{
    //
    // return pointer to Q histogram
    // if force is true create a new histogram if it doesn't exist allready
    //
    if ( !force || arr->UncheckedAt(sector) )
	return (TH2F*)arr->UncheckedAt(sector);

    // if we are forced and histogram doesn't yes exist create it
    Char_t name[255], title[255];

    sprintf(name,"hCalib%s%.2d",type,sector);
    sprintf(title,"%s calibration histogram sector %.2d;ADC channel;Channel (pad)",type,sector);

    // new histogram with Q calib information. One value for each pad!
    TH2F* hist = new TH2F(name,title,
			  nbinsY, ymin, ymax,
			  fROC->GetNChannels(sector),0,fROC->GetNChannels(sector)
			 );
    hist->SetDirectory(0);
    arr->AddAt(hist,sector);
    return hist;
}
//_____________________________________________________________________
TH2F* AliTPCCalibPedestal::GetHistoPedestal(Int_t sector, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to T0 histogram
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fHistoPedestalArray;
    return GetHisto(sector, arr, fAdcMax-fAdcMin, fAdcMin, fAdcMax, "Pedestal", force);
}
//_____________________________________________________________________
AliTPCCalROC* AliTPCCalibPedestal::GetCalRoc(Int_t sector, TObjArray* arr, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    if ( !force || arr->UncheckedAt(sector) )
	return (AliTPCCalROC*)arr->UncheckedAt(sector);

    // if we are forced and the histogram doesn't yet exist create it

    // new AliTPCCalROC for T0 information. One value for each pad!
    AliTPCCalROC *croc = new AliTPCCalROC(sector);
    arr->AddAt(croc,sector);
    return croc;
}
//_____________________________________________________________________
AliTPCCalROC* AliTPCCalibPedestal::GetCalRocPedestal(Int_t sector, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Carge ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalRocArrayPedestal;
    return GetCalRoc(sector, arr, force);
}
//_____________________________________________________________________
AliTPCCalROC* AliTPCCalibPedestal::GetCalRocRMS(Int_t sector, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to signal width ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalRocArrayRMS;
    return GetCalRoc(sector, arr, force);
}
//_____________________________________________________________________
void AliTPCCalibPedestal::Merge(AliTPCCalibPedestal *ped)
{
    //
    //  Merge reference histograms of sig to the current AliTPCCalibSignal
    //

    //merge histograms
    for (Int_t iSec=0; iSec<72; ++iSec){
	TH2F *hRefPedMerge   = ped->GetHistoPedestal(iSec);


	if ( hRefPedMerge ){
	    TDirectory *dir = hRefPedMerge->GetDirectory(); hRefPedMerge->SetDirectory(0);
	    TH2F *hRefPed   = GetHistoPedestal(iSec);
	    if ( hRefPed ) hRefPed->Add(hRefPedMerge);
	    else {
		TH2F *hist = new TH2F(*hRefPedMerge);
                hist->SetDirectory(0);
		fHistoPedestalArray.AddAt(hist, iSec);
	    }
	    hRefPedMerge->SetDirectory(dir);
	}
    }
}
//_____________________________________________________________________
void AliTPCCalibPedestal::Analyse() /*FOLD00*/
{
    //
    //  Calculate calibration constants
    //

    Int_t nbinsAdc = fAdcMax-fAdcMin;

    TVectorD param(3);
    TMatrixD dummy(3,3);

    Float_t *array_hP=0;


    for (Int_t iSec=0; iSec<72; ++iSec){
	TH2F *hP = GetHistoPedestal(iSec);
        if ( !hP ) continue;

	AliTPCCalROC *rocPedestal = GetCalRocPedestal(iSec,kTRUE);
	AliTPCCalROC *rocRMS      = GetCalRocRMS(iSec,kTRUE);

	array_hP = hP->GetArray();
        UInt_t nChannels = fROC->GetNChannels(iSec);

	for (UInt_t iChannel=0; iChannel<nChannels; ++iChannel){
            Int_t offset = (nbinsAdc+2)*(iChannel+1)+1;
	    Double_t ret = AliMathBase::FitGaus(array_hP+offset,nbinsAdc,fAdcMin,fAdcMax,&param,&dummy);
            // if the fitting failed set noise and pedestal to 0
	    if ( ret == -4 ) {
		param[1]=0;
		param[2]=0;
	    }
	    rocPedestal->SetValue(iChannel,param[1]);
            rocRMS->SetValue(iChannel,param[2]);
	}
    }
}
//_____________________________________________________________________
void AliTPCCalibPedestal::DumpToFile(const Char_t *filename, const Char_t *dir, Bool_t append) /*FOLD00*/
{
    //
    //  Write class to file
    //

    TString sDir(dir);
    TString option;

    if ( append )
	option = "update";
    else
        option = "recreate";

    TDirectory *backup = gDirectory;
    TFile f(filename,option.Data());
    f.cd();
    if ( !sDir.IsNull() ){
	f.mkdir(sDir.Data());
	f.cd(sDir);
    }
    this->Write();
    f.Close();

    if ( backup ) backup->cd();
}
