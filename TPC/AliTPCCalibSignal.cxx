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





//-------------------------------------------------------
//          Implementation of the TPC pulser calibration
//
//   Origin: Jens Wiechula, Marian Ivanov   J.Wiechula@gsi.de, Marian.Ivanov@cern.ch
// 
// 
//-------------------------------------------------------


/* $Id$ */



//Root includes
#include <TObjArray.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TH2F.h>
#include <TH2S.h>
#include <TH1S.h>
#include <TString.h>
#include <TVectorF.h>
#include <TMath.h>
#include <TF1.h>


#include <TStopwatch.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TDirectory.h>
#include <TSystem.h>
#include <TFile.h>

//AliRoot includes
#include "AliRawReader.h"
#include "AliRawReaderRoot.h"
#include "AliTPCRawStream.h"
#include "AliTPCCalROC.h"
#include "AliTPCCalPad.h"
#include "AliTPCROC.h"
#include "AliTPCParam.h"
#include "AliTPCCalibSignal.h"
#include "AliTPCcalibDB.h"

#include "TTreeStream.h"
ClassImp(AliTPCCalibSignal) /*FOLD00*/

AliTPCCalibSignal::AliTPCCalibSignal() : /*FOLD00*/
    TObject(),
    fFirstTimeBin(60),
    fLastTimeBin(120),
    fFirstTimeBinT0(-15),
    fLastTimeBinT0(15),
    fLastSector(-1),
    fROC(AliTPCROC::Instance()),
    fParam(new AliTPCParam),
    fPedestalTPC(0),
    fBpedestal(kFALSE),
    fCalRocArrayT0(72),
    fCalRocArrayQ(72),
    fCalRocArrayRMS(72),
    fCalRocArrayOutliers(72),
    fHistoQArray(72),
    fHistoT0Array(72),
    fHistoRMSArray(72),
    fPadTimesArrayEvent(72),
    fPadQArrayEvent(72),
    fPadRMSArrayEvent(72),
    fPadPedestalArrayEvent(72),
    fCurrentChannel(-1),
    fCurrentSector(-1),
    fCurrentRow(-1),
    fMaxPadSignal(-1),
    fMaxTimeBin(-1),
    fPadSignal(1024),
    fVTime0Offset1(72),
    fVTime0Offset1Counter(72),
    fEvent(-1),
    fDebugStreamer(0x0),
    fDebugLevel(0)
{
    //
    // AliTPCSignal default constructor
    //

    //debug stream
    TDirectory *backup = gDirectory;
    fDebugStreamer = new TTreeSRedirector("deb2.root");
    if ( backup ) backup->cd();  //we don't want to be cd'd to the debug streamer
    
}
//_____________________________________________________________________
AliTPCCalibSignal::AliTPCCalibSignal(const AliTPCCalibSignal &sig) :
    TObject(sig),
    fFirstTimeBin(sig.fFirstTimeBin),
    fLastTimeBin(sig.fLastTimeBin),
    fFirstTimeBinT0(sig.fFirstTimeBinT0),
    fLastTimeBinT0(sig.fLastTimeBinT0),
    fLastSector(-1),
    fROC(AliTPCROC::Instance()),
    fParam(new AliTPCParam),
    fPedestalTPC(sig.fPedestalTPC),
    fBpedestal(sig.fBpedestal),
    fCalRocArrayT0(72),
    fCalRocArrayQ(72),
    fCalRocArrayRMS(72),
    fCalRocArrayOutliers(72),
    fHistoQArray(72),
    fHistoT0Array(72),
    fHistoRMSArray(72),
    fPadTimesArrayEvent(72),
    fPadQArrayEvent(72),
    fPadRMSArrayEvent(72),
    fPadPedestalArrayEvent(72),
    fCurrentChannel(-1),
    fCurrentSector(-1),
    fCurrentRow(-1),
    fMaxPadSignal(-1),
    fMaxTimeBin(-1),
    fPadSignal(1024),
    fVTime0Offset1(72),
    fVTime0Offset1Counter(72),
    fEvent(-1),
    fDebugStreamer(0x0),
    fDebugLevel(sig.fDebugLevel)
{
    //
    // AliTPCSignal default constructor
    //

    for (Int_t iSec = 0; iSec < 72; iSec++){
	const AliTPCCalROC *calQ   = (AliTPCCalROC*)sig.fCalRocArrayQ.UncheckedAt(iSec);
	const AliTPCCalROC *calT0  = (AliTPCCalROC*)sig.fCalRocArrayT0.UncheckedAt(iSec);
	const AliTPCCalROC *calRMS = (AliTPCCalROC*)sig.fCalRocArrayRMS.UncheckedAt(iSec);
        const AliTPCCalROC *calOut = (AliTPCCalROC*)sig.fCalRocArrayOutliers.UncheckedAt(iSec);

	const TH2S *hQ   = (TH2S*)sig.fHistoQArray.UncheckedAt(iSec);
	const TH2S *hT0  = (TH2S*)sig.fHistoT0Array.UncheckedAt(iSec);
        const TH2S *hRMS = (TH2S*)sig.fHistoRMSArray.UncheckedAt(iSec);

	if ( calQ   != 0x0 ) fCalRocArrayQ.AddAt(new AliTPCCalROC(*calQ), iSec);
	if ( calT0  != 0x0 ) fCalRocArrayT0.AddAt(new AliTPCCalROC(*calT0), iSec);
	if ( calRMS != 0x0 ) fCalRocArrayRMS.AddAt(new AliTPCCalROC(*calRMS), iSec);
        if ( calOut != 0x0 ) fCalRocArrayOutliers.AddAt(new AliTPCCalROC(*calOut), iSec);

	if ( hQ != 0x0 ){
	    TH2S *hNew = new TH2S(*hQ);
	    hNew->SetDirectory(0);
	    fHistoQArray.AddAt(hNew,iSec);
	}
	if ( hT0 != 0x0 ){
	    TH2S *hNew = new TH2S(*hT0);
	    hNew->SetDirectory(0);
	    fHistoQArray.AddAt(hNew,iSec);
	}
	if ( hRMS != 0x0 ){
	    TH2S *hNew = new TH2S(*hRMS);
	    hNew->SetDirectory(0);
	    fHistoQArray.AddAt(hNew,iSec);
	}
    }


    //debug stream
    TDirectory *backup = gDirectory;
    fDebugStreamer = new TTreeSRedirector("deb2.root");
    if ( backup ) backup->cd();  //we don't want to be cd'd to the debug streamer
    
}
//_____________________________________________________________________
AliTPCCalibSignal& AliTPCCalibSignal::operator = (const  AliTPCCalibSignal &source)
{
  //
  // assignment operator
  //
  if (&source == this) return *this;
  new (this) AliTPCCalibSignal(source);

  return *this;
}
//_____________________________________________________________________
AliTPCCalibSignal::~AliTPCCalibSignal()
{
    //
    // destructor
    //

    fCalRocArrayT0.Delete();
    fCalRocArrayQ.Delete();
    fCalRocArrayRMS.Delete();

    fHistoQArray.Delete();
    fHistoT0Array.Delete();
    fHistoRMSArray.Delete();

    fPadTimesArrayEvent.Delete();
    fPadQArrayEvent.Delete();
    fPadRMSArrayEvent.Delete();
    fPadPedestalArrayEvent.Delete();

    if ( fDebugStreamer) delete fDebugStreamer;
    delete fROC;
    delete fParam;
}
//_____________________________________________________________________
Int_t AliTPCCalibSignal::Update(const Int_t icsector, /*FOLD00*/
				const Int_t icRow,
				const Int_t icPad,
				const Int_t icTimeBin,
				const Float_t csignal)
{
    //
    // Signal filling methode on the fly pedestal and Time offset correction if necessary.
    // no extra analysis necessary. Assumes knowledge of the signal shape!
    // assumes that it is looped over consecutive time bins of one pad
    //
    if ( (icTimeBin>fLastTimeBin) || (icTimeBin<fFirstTimeBin)   ) return 0;

    Int_t iChannel  = fROC->GetRowIndexes(icsector)[icRow]+icPad; //  global pad position in sector

    //init first pad and sector in this event
    if ( fCurrentChannel == -1 ) {
	fCurrentChannel = iChannel;
	fCurrentSector  = icsector;
        fCurrentRow     = icRow;
    }

    //process last pad if we change to a new one
    if ( iChannel != fCurrentChannel ){
        ProcessPad();
	fCurrentChannel = iChannel;
	fCurrentSector  = icsector;
        fCurrentRow     = icRow;
    }

    if ( fDebugLevel > 6 )
        if ( icTimeBin == fLastTimeBin )
	    printf("Filling sector: %d, channel: %d\n",icsector,iChannel);

    //fill signals for current pad
    fPadSignal[icTimeBin]=csignal;
    if ( csignal > fMaxPadSignal ){
	fMaxPadSignal = csignal;
	fMaxTimeBin   = icTimeBin;
    }
    return 0;
}
//_____________________________________________________________________
void AliTPCCalibSignal::ProcessPad() /*FOLD00*/
{
    //
    //  Process data of current pad
    //
    if ( fDebugLevel > 4 )
	printf("process: sector-pad: %.2d-%.4d ... ", fCurrentSector, fCurrentChannel);

    Float_t pedestal = 0;

    if ( fBpedestal ){
        //!!!!!!! does not work like this
        //use pedestal database
	AliTPCCalROC *pedestalROC = 0x0;

        //only load new pedestals if the sector has changed
	if ( fCurrentSector!=fLastSector ){
	    pedestalROC = fPedestalTPC->GetCalROC(fCurrentSector);
	    fLastSector=fCurrentSector;
	}

	pedestal = pedestalROC->GetValue(fCurrentChannel);

    } else {
	if ( fDebugLevel > 4 )
	    printf("pedestals ... ");

	//find pedestal for pad on the fly
        //using a few timebins before the signal
	Int_t pminus1 = 10, pminus2=5;
	Float_t sumN=0;

	for (Int_t i=fMaxTimeBin-pminus1; i<fMaxTimeBin-pminus2+1; i++){
	    if ( i>fFirstTimeBin && i<fLastTimeBin ){
		pedestal+=fPadSignal[i];
		sumN+=1.;
	    }
	}

	if ( sumN>0 ) pedestal/=sumN;
	if ( fDebugLevel > 4 )
	    printf("%.2f ... ",pedestal);
    }



    //!!!! check borders
    //find signal mean and sigma
    Int_t tminus = 2, tplus=7;
    Double_t meanT=0, sigmaT=0, Qsum=0;

    if ( fDebugLevel > 4 )
	printf(" mean +- sigma (sum) ... ");

    for (Int_t i=fMaxTimeBin-tminus; i<fMaxTimeBin+tplus; i++){
	if ( i>fFirstTimeBin && i<fLastTimeBin ){
	    Double_t val=fPadSignal[i]-pedestal;
	    meanT+=val*(i+.5);      //+.5: center of the timebin
            sigmaT+=val*(i+.5)*(i+.5);
	    Qsum+=val;
	}
    }



    //!!!! What to do if Qsum == 0???
    //!!!! Should there be some threshold for max - pedestal and/or Qsum???
    //!!!! What if Qsum < 0
    //!!!! only fill time0 offset if Qsum > 0???
    if ( Qsum > 0 ){
	meanT/=Qsum;
	sigmaT/=Qsum;
	sigmaT = TMath::Sqrt(TMath::Abs(meanT*meanT - sigmaT));

	//fill Time0 offset data for this event
	fVTime0Offset1[fCurrentSector]+=meanT;
	fVTime0Offset1Counter[fCurrentSector]++;
    } else {
        Qsum=0;
	meanT  = fLastTimeBinT0+1;               //put to overflow bin
	sigmaT = fLastTimeBinT0-fFirstTimeBinT0; //put to overflow bin
    }

    if ( fDebugLevel > 4 )
	printf("%.3f +- %.3f (%.3f) ... ",meanT,sigmaT,Qsum);


    if ( fDebugLevel > 4 )
	printf("filling ... ");


    //Fill Event T0 counter
    (*GetPadTimesEvent(fCurrentSector,kTRUE))[fCurrentChannel] = meanT;


    //Normalise Q to pad area of irocs
    Float_t norm = fParam->GetPadPitchWidth(0)*fParam->GetPadPitchLength(0,0)/(
	fParam->GetPadPitchWidth(fCurrentSector)*fParam->GetPadPitchLength(fCurrentSector,fCurrentRow));

//    if (fCurrentChannel == 0) printf("sec, norm: %d, %f\n", fCurrentSector, norm);

    //Fill Q histogram
    GetHistoQ(fCurrentSector,kTRUE)->Fill(fCurrentChannel, TMath::Sqrt(Qsum*norm));

    //Fill RMS histogram
    GetHistoRMS(fCurrentSector,kTRUE)->Fill(fCurrentChannel, sigmaT);


    //Fill debugging info
    if ( fDebugLevel>0 ){
	(*GetPadPedestalEvent(fCurrentSector,kTRUE))[fCurrentChannel]=pedestal;
	(*GetPadRMSEvent(fCurrentSector,kTRUE))[fCurrentChannel]=sigmaT;
	(*GetPadQEvent(fCurrentSector,kTRUE))[fCurrentChannel]=Qsum;
    }

    if ( fDebugLevel > 4 )
	printf("reset pad ...");

    ResetPad();

    if ( fDebugLevel > 4 )
	printf("end\n");
}
//_____________________________________________________________________
void AliTPCCalibSignal::EndEvent() /*FOLD00*/
{
    //
    //  Process data of current pad
    //
    printf("end event\n");
    for ( Int_t iSec = 0; iSec<72; iSec++ ){
	TVectorF *vTimes = GetPadTimesEvent(iSec);
        if ( !vTimes ) continue;

	for ( UInt_t iChannel=0; iChannel<fROC->GetNChannels(iSec); iChannel++ ){
	    Float_t Time0 = fVTime0Offset1[iSec]/fVTime0Offset1Counter[iSec];
	    Float_t Time  = (*vTimes)[iChannel];

            GetHistoT0(iSec,kTRUE)->Fill(iChannel,Time-Time0);


	    //Debug start
	    if ( fDebugLevel>0 ){
		Int_t row=0;
		Int_t pad=0;
		Int_t padc=0;

		Float_t Q   = (*GetPadQEvent(iSec))[iChannel];
                Float_t RMS = (*GetPadRMSEvent(iSec))[iChannel];

		UInt_t channel=iChannel;
		Int_t sector=iSec;

		while ( channel > (fROC->GetRowIndexes(sector)[row]+fROC->GetNPads(sector,row)-1) ) row++;
		pad = channel-fROC->GetRowIndexes(sector)[row];
		padc = pad-(fROC->GetNPads(sector,row)/2);

		TH1F *h1 = new TH1F(Form("hSignalD%d.%d.%d",sector,row,pad),
				    Form("hSignalD%d.%d.%d",sector,row,pad),
				    fLastTimeBin-fFirstTimeBin,
				    fFirstTimeBin,fLastTimeBin);
		h1->SetDirectory(0);

		for (Int_t i=fFirstTimeBin; i<fLastTimeBin+1; i++)
		    h1->Fill(i,fPadSignal(i));

		(*fDebugStreamer) << "DataPad" <<
		    "Event=" << fEvent <<
		    "Sector="<< sector <<
		    "Row="   << row<<
		    "Pad="   << pad <<
		    "PadC="  << padc <<
		    "PadSec="<< channel <<
		    "Time0="  << Time0 <<
		    "Time="  << Time <<
		    "RMS="   << RMS <<
		    "Sum="   << Q <<
		    "hist.=" << h1 <<
		    "\n";

		delete h1;
	    }
	    //Debug end

	}
    }

}
//_____________________________________________________________________
Bool_t AliTPCCalibSignal::ProcessEvent(AliRawReader *rawReader) /*FOLD00*/
{
  //
  //
  //


    AliTPCRawStream input(rawReader);

  rawReader->Select("TPC");

  input.SetOldRCUFormat(1);
  printf("start event processing\n");

  ResetEvent();

  Bool_t withInput = kFALSE;

  while (input.Next()) {

      Int_t isector  = input.GetSector();                       //  current sector
      Int_t iRow     = input.GetRow();                          //  current row
      Int_t iPad     = input.GetPad();                          //  current pad
      Int_t iTimeBin = input.GetTime();                         //  current time bin
      Float_t signal = input.GetSignal();                       //  current ADC signal

      Update(isector,iRow,iPad,iTimeBin,signal);
      withInput = kTRUE;
  }

  if (withInput){
      ProcessPad();
      EndEvent();
  }

  printf("end event processing\n");
  if ( fDebugLevel>0 )
      fDebugStreamer->GetFile()->Write();
  return withInput;
}
//_____________________________________________________________________
TH2S* AliTPCCalibSignal::GetHisto(Int_t sector, TObjArray *arr, /*FOLD00*/
				  Int_t nbinsY, Float_t ymin, Float_t ymax,
				  Char_t *type, Bool_t force)
{
    //
    // return pointer to Q histogram
    // if force is true create a new histogram if it doesn't exist allready
    //
    if ( !force || arr->UncheckedAt(sector) )
	return (TH2S*)arr->UncheckedAt(sector);

    // if we are forced and histogram doesn't yes exist create it
    Char_t name[255], title[255];

    sprintf(name,"hCalib%s%.2d",type,sector);
    sprintf(title,"%s calibration histogram sector %.2d",type,sector);

    // new histogram with Q calib information. One value for each pad!
    TH2S* hist = new TH2S(name,title,
			  fROC->GetNChannels(sector),0,fROC->GetNChannels(sector),
			  nbinsY, ymin, ymax);
    hist->SetDirectory(0);
    arr->AddAt(hist,sector);
    return hist;
}
//_____________________________________________________________________
TH2S* AliTPCCalibSignal::GetHistoT0(Int_t sector, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to T0 histogram
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fHistoT0Array;
    return GetHisto(sector, arr, 200, -2, 2, "T0", force);
}
//_____________________________________________________________________
TH2S* AliTPCCalibSignal::GetHistoQ(Int_t sector, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Q histogram
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fHistoQArray;
    return GetHisto(sector, arr, 150, 24, 55, "Q", force);
}
//_____________________________________________________________________
TH2S* AliTPCCalibSignal::GetHistoRMS(Int_t sector, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Q histogram
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fHistoRMSArray;
    return GetHisto(sector, arr, 100, 0, 5, "RMS", force);
}
//_____________________________________________________________________
TVectorF* AliTPCCalibSignal::GetPadInfoEvent(Int_t sector, TObjArray *arr, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Pad Info from 'arr' for the current event and sector
    // if force is true create it if it doesn't exist allready
    //
    if ( !force || arr->UncheckedAt(sector) )
	return (TVectorF*)arr->UncheckedAt(sector);

    TVectorF *vect = new TVectorF(fROC->GetNChannels(sector));
    arr->AddAt(vect,sector);
    return vect;
}
//_____________________________________________________________________
TVectorF* AliTPCCalibSignal::GetPadTimesEvent(Int_t sector, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Pad Times Array for the current event and sector
    // if force is true create it if it doesn't exist allready
    //
    TObjArray *arr = &fPadTimesArrayEvent;
    return GetPadInfoEvent(sector,arr,force);
}
//_____________________________________________________________________
TVectorF* AliTPCCalibSignal::GetPadQEvent(Int_t sector, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Pad Q Array for the current event and sector
    // if force is true create it if it doesn't exist allready
    // for debugging purposes only
    //

    TObjArray *arr = &fPadQArrayEvent;
    return GetPadInfoEvent(sector,arr,force);
}
//_____________________________________________________________________
TVectorF* AliTPCCalibSignal::GetPadRMSEvent(Int_t sector, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Pad RMS Array for the current event and sector
    // if force is true create it if it doesn't exist allready
    // for debugging purposes only
    //
    TObjArray *arr = &fPadRMSArrayEvent;
    return GetPadInfoEvent(sector,arr,force);
}
//_____________________________________________________________________
TVectorF* AliTPCCalibSignal::GetPadPedestalEvent(Int_t sector, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Pad RMS Array for the current event and sector
    // if force is true create it if it doesn't exist allready
    // for debugging purposes only
    //
    TObjArray *arr = &fPadPedestalArrayEvent;
    return GetPadInfoEvent(sector,arr,force);
}
//_____________________________________________________________________
AliTPCCalROC* AliTPCCalibSignal::GetCalRoc(Int_t sector, TObjArray* arr, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    if ( !force || arr->UncheckedAt(sector) )
	return (AliTPCCalROC*)arr->UncheckedAt(sector);

    // if we are forced and histogram doesn't yes exist create it

    // new AliTPCCalROC for T0 information. One value for each pad!
    AliTPCCalROC *croc = new AliTPCCalROC(sector);
    //init values
    for ( UInt_t iChannel = 0; iChannel<croc->GetNchannels(); iChannel++){
	croc->SetValue(iChannel, 0);
    }
    arr->AddAt(croc,sector);
    return croc;
}
//_____________________________________________________________________
AliTPCCalROC* AliTPCCalibSignal::GetCalRocT0(Int_t sector, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Carge ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalRocArrayT0;
    return GetCalRoc(sector, arr, force);
}
//_____________________________________________________________________
AliTPCCalROC* AliTPCCalibSignal::GetCalRocQ(Int_t sector, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to T0 ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalRocArrayQ;
    return GetCalRoc(sector, arr, force);
}
//_____________________________________________________________________
AliTPCCalROC* AliTPCCalibSignal::GetCalRocRMS(Int_t sector, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to signal width ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalRocArrayRMS;
    return GetCalRoc(sector, arr, force);
}
//_____________________________________________________________________
AliTPCCalROC* AliTPCCalibSignal::GetCalRocOutliers(Int_t sector, Bool_t force)
{
    //
    // return pointer to Outliers
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalRocArrayOutliers;
    return GetCalRoc(sector, arr, force);
}
//_____________________________________________________________________
void AliTPCCalibSignal::ResetEvent() /*FOLD00*/
{
    //
    //  Reset global counters  -- Should be called before each event is processed
    //
    fLastSector=-1;
    fCurrentSector=-1;
    fCurrentRow=-1;
    fCurrentChannel=-1;

    ResetPad();

    fPadTimesArrayEvent.Delete();
    fPadQArrayEvent.Delete();
    fPadRMSArrayEvent.Delete();
    fPadPedestalArrayEvent.Delete();

    for ( Int_t i=0; i<72; i++ ){
	fVTime0Offset1[i]=0;
	fVTime0Offset1Counter[i]=0;
    }
}
//_____________________________________________________________________
void AliTPCCalibSignal::ResetPad() /*FOLD00*/
{
    //
    //  Reset pad infos -- Should be called after a pad has been processed
    //
    for (Int_t i=fFirstTimeBin; i<fLastTimeBin+1; i++)
	fPadSignal[i] = 0;
    fMaxTimeBin = -1;
    fMaxPadSignal = -1;
}
//_____________________________________________________________________
void AliTPCCalibSignal::Analyse() /*FOLD00*/
{
    //
    //  Calculate calibration constants
    //


    for (Int_t iSec=0; iSec<72; iSec++){
	TH2S *hT0 = GetHistoT0(iSec);
        if (!hT0 ) continue;

	AliTPCCalROC *rocQ   = GetCalRocQ  (iSec,kTRUE);
	AliTPCCalROC *rocT0  = GetCalRocT0 (iSec,kTRUE);
	AliTPCCalROC *rocRMS = GetCalRocRMS(iSec,kTRUE);
        AliTPCCalROC *rocOut = GetCalRocOutliers(iSec,kTRUE);

	TH2S *hQ   = GetHistoQ(iSec);
	TH2S *hRMS = GetHistoRMS(iSec);

	//debug
	Int_t row=0;
	Int_t pad=0;
	Int_t padc=0;
	//! debug

	for (UInt_t iChannel=0; iChannel<fROC->GetNChannels(iSec); iChannel++){


	    Float_t cogTime0 = -1000;
	    Float_t cogQ     = -1000;
	    Float_t cogRMS   = -1000;
            Float_t cogOut   = 0;

	    hQ->SetAxisRange(iChannel,iChannel);
	    hT0->SetAxisRange(iChannel,iChannel);
	    hRMS->SetAxisRange(iChannel,iChannel);

	    cogTime0 = hT0->GetMean(2);
	    cogQ     = hQ->GetMean(2);
	    cogRMS   = hRMS->GetMean(2);
/*
	    if ( (cogQ < ??) && (cogTime0 > ??) && (cogTime0<??) && ( cogRMS>??) ){
		cogOut = 1;
		cogTime0 = 0;
		cogQ     = 0;
		cogRMS   = 0;
	    }
*/
       	    rocQ->SetValue(iChannel, cogQ*cogQ);
	    rocT0->SetValue(iChannel, cogTime0);
	    rocRMS->SetValue(iChannel, cogRMS);
	    rocOut->SetValue(iChannel, cogOut);


	    //debug
	    if ( fDebugLevel > 0 ){
		while ( iChannel > (fROC->GetRowIndexes(iSec)[row]+fROC->GetNPads(iSec,row)-1) ) row++;
		pad = iChannel-fROC->GetRowIndexes(iSec)[row];
		padc = pad-(fROC->GetNPads(iSec,row)/2);

		(*fDebugStreamer) << "DataEnd" <<
		    "Sector="  << iSec      <<
		    "Pad="     << pad       <<
		    "PadC="    << padc      <<
		    "Row="     << row       <<
		    "PadSec="  << iChannel   <<
		    "Q="       << cogQ      <<
		    "T0="      << cogTime0  <<
		    "RMS="     << cogRMS    <<
		    "\n";
	    }
	    //! debug

	}

    }
    delete fDebugStreamer;
    fDebugStreamer = 0x0;
}
//_____________________________________________________________________
void AliTPCCalibSignal::DumpToFile(const Char_t *filename, const Char_t *dir, Bool_t append)
{
    //
    //  Write class to file
    //

    TDirectory *backup = gDirectory;
    TString sDir(dir);
    TString option;

    if ( append )
	option = "update";
    else
        option = "recreate";

    TFile f(filename,option.Data());
    if ( !sDir.IsNull() ){
	f.mkdir(sDir.Data());
	f.cd(sDir);
    }
    gDirectory->WriteTObject(this);
    f.Close();

    if ( backup ) backup->cd();

}
