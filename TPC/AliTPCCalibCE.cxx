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
#include <TObjArray.h>
#include <TH1F.h>
#include <TH2S.h>
#include <TString.h>
#include <TVectorF.h>
#include <TVectorD.h>
#include <TMatrixD.h>
#include <TMath.h>
#include <TGraph.h>
#include <TString.h>

#include <TDirectory.h>
#include <TSystem.h>
#include <TFile.h>

//AliRoot includes
#include "AliRawReader.h"
#include "AliRawReaderRoot.h"
#include "AliRawReaderDate.h"
#include "AliRawEventHeaderBase.h"
#include "AliTPCRawStream.h"
#include "AliTPCcalibDB.h"
#include "AliTPCCalROC.h"
#include "AliTPCCalPad.h"
#include "AliTPCROC.h"
#include "AliTPCParam.h"
#include "AliTPCCalibCE.h"
#include "AliMathBase.h"
#include "TTreeStream.h"

//date
#include "event.h"
ClassImp(AliTPCCalibCE)

//////////////////////////////////////////////////////////////////////////////////////
//          Implementation of the TPC Central Electrode calibration
//
//   Origin: Jens Wiechula, Marian Ivanov   J.Wiechula@gsi.de, Marian.Ivanov@cern.ch
// 
//
//
// *************************************************************************************
// *                                Class Description                                  *
// *************************************************************************************
//
/* BEGIN_HTML
 <h4>The AliTPCCalibCE class is used to get calibration data from the Central Electrode
 using laser runs.</h4>

 The information retrieved is
 <ul style="list-style-type: square;">
   <li>Time arrival from the CE</li>
   <li>Signal width</li>
   <li>Signal sum</li>
 </ul>

<h4>Overview:</h4>
 <ol style="list-style-type: upper-roman;">
   <li><a href="#working">Working principle</a></li>
   <li><a href="#user">User interface for filling data</a></li>
   <li><a href="#info">Stored information</a></li>
 </ol>

 <h3><a name="working">I. Working principle</a></h3>

 <h4>Raw laser data is processed by calling one of the ProcessEvent(...) functions
 (see below). These in the end call the Update(...) function.</h4>

 <ul style="list-style-type: square;">
   <li>the Update(...) function:<br />
       In this function the array fPadSignal is filled with the adc signals between the specified range
       fFirstTimeBin and fLastTimeBin for the current pad.
       before going to the next pad the ProcessPad() function is called, which analyses the data for one pad
       stored in fPadSignal.
   </li>
   <ul style="list-style-type: square;">
   <li>the ProcessPad() function:</li>
   <ol style="list-style-type: decimal;">
     <li>Find Pedestal and Noise information</li>
     <ul style="list-style-type: square;">
       <li>use database information which has to be set by calling<br />
           SetPedestalDatabase(AliTPCCalPad *pedestalTPC, AliTPCCalPad *padNoiseTPC)</li>
       <li>if no information from the pedestal data base
           is available the informaion is calculated on the fly
           ( see FindPedestal() function )</li>
     </ul>
     <li>Find local maxima of the pad signal</li>
     <ul style="list-style-type: square;">
       <li>maxima arise from the laser tracks, the CE and also periodic postpeaks after the CE signal have
       have been observed ( see FindLocalMaxima(...) )</li>
     </ul>
     <li>Find the CE signal information</li>
     <ul style="list-style-type: square;">
       <li>to find the position of the CE signal the Tmean information from the previos event is used
           as the CE signal the local maximum closest to this Tmean is identified</li>
       <li>calculate  mean = T0, RMS = signal width and Q sum in a range of -4+7 timebins around Q max position
           the Q sum is scaled by pad area (see FindPulserSignal(...) function)</li>
     </ul>
     <li>Fill a temprary array for the T0 information (GetPadTimesEvent(fCurrentSector,kTRUE)) (why see below)</li>
     <li>Fill the Q sum and RMS values in the histograms (GetHisto[RMS,Q](ROC,kTRUE))</li>
     </ol>
   </ul>
 </ul>

 <h4>At the end of each event the EndEvent() function is called</h4>

 <ul style="list-style-type: square;">
   <li>the EndEvent() function:</li>
   <ul style="list-style-type: square;">
     <li>calculate the mean T0 for side A and side C. Fill T0 histogram with Time0-<Time0 for side[A,C]>
         This is done to overcome syncronisation problems between the trigger and the fec clock.</li>
     <li>calculate Mean T for each ROC using the COG aroud the median of the LocalMaxima distribution in one sector</li>
     <li>calculate Mean Q</li>
     <li>calculate Global fit parameters for Pol1 and Pol2 fits</li>
   </ul>
 </ul>

 <h4>After accumulating the desired statistics the Analyse() function has to be called.</h4>
  <ul style="list-style-type: square;">
  <li>the Analyse() function:</li>
    <ul style="list-style-type: square;">
      <li>calculate the mean values of T0, RMS, Q for each pad, using
          the AliMathBase::GetCOG(...) function</li>
      <li>fill the calibration storage classes (AliTPCCalROC) for each ROC</li>
         (The calibration information is stored in the TObjArrays fCalRocArrayT0, fCalRocArrayRMS and fCalRocArrayQ</li>
    </ul>
  </ul>

 <h3><a name="user">II. User interface for filling data</a></h3>

 <h4>To Fill information one of the following functions can be used:</h4>

 <ul style="list-style-type: none;">
  <li> Bool_t ProcessEvent(eventHeaderStruct *event);</li>
    <ul style="list-style-type: square;">
      <li>process Date event</li>
      <li>use AliTPCRawReaderDate and call ProcessEvent(AliRawReader *rawReader)</li>
    </ul>
    <br />

  <li> Bool_t ProcessEvent(AliRawReader *rawReader);</li>
    <ul style="list-style-type: square;">
      <li>process AliRawReader event</li>
      <li>use AliTPCRawStream to loop over data and call ProcessEvent(AliTPCRawStream *rawStream)</li>
    </ul>
    <br />

  <li> Bool_t ProcessEvent(AliTPCRawStream *rawStream);</li>
    <ul style="list-style-type: square;">
      <li>process event from AliTPCRawStream</li>
      <li>call Update function for signal filling</li>
    </ul>
    <br />

  <li> Int_t Update(const Int_t isector, const Int_t iRow, const Int_t
              iPad,  const Int_t iTimeBin, const Float_t signal);</li>
    <ul style="list-style-type: square;">
      <li>directly  fill signal information (sector, row, pad, time bin, pad)
          to the reference histograms</li>
    </ul>
 </ul>

 <h4>It is also possible to merge two independently taken calibrations using the function</h4>

 <ul style="list-style-type: none;">
  <li> void Merge(AliTPCCalibSignal *sig)</li>
    <ul style="list-style-type: square;">
      <li>copy histograms in 'sig' if they do not exist in this instance</li>
      <li>Add histograms in 'sig' to the histograms in this instance if the allready exist</li>
      <li>After merging call Analyse again!</li>
    </ul>
 </ul>


 <h4>example: filling data using root raw data:</h4>
 <pre> 
 void fillCE(Char_t *filename)
 {
    rawReader = new AliRawReaderRoot(fileName);
    if ( !rawReader ) return;
    AliTPCCalibCE *calib = new AliTPCCalibCE;
    while (rawReader->NextEvent()){
      calib->ProcessEvent(rawReader);
    }
    calib->Analyse();
    calib->DumpToFile("CEData.root");
    delete rawReader;
    delete calib;
 }
 </pre>

 <h3><a name="info">III. What kind of information is stored and how to retrieve it</a></h4>

 <h4><a name="info:stored">III.1 Stored information</a></h4>
 <ul style="list-style-type: none;">
  <li>Histograms:</li>
  <ul style="list-style-type: none;">
    <li>For each ROC three TH2S histos 'Reference Histograms'  (ROC channel vs. [Time0, signal width, Q sum])
        is created when it is filled for the first time (GetHisto[T0,RMS,Q](ROC,kTRUE)). The histos are
        stored in the TObjArrays fHistoT0Array, fHistoRMSArray and fHistoQArray.</li>
  </ul>
  <br />

 <li>Calibration Data:</li>
 <ul style="list-style-type: none;">
      <li>For each ROC three types of calibration data (AliTPCCalROC) is stored: for the mean arrival Time,
          the signal width and the signal Sum. The AliTPCCalROC objects are stored in the TObjArrays
          fCalRocArrayT0, fCalRocArrayRMS , fCalRocArrayQ. The object for each roc is created the first time it
          is accessed (GetCalRoc[T0,RMS,Q](ROC,kTRUE));</li>
 </ul>
 <br />

 <li>For each event the following information is stored:</li>
   
 <ul style="list-style-type: square;">
   <li>event time ( TVectorD  fVEventTime )</li>
   <li>event id   ( TVectorD  fVEventNumber )</li>
   <br />
   <li>mean arrival time for each ROC                ( TObjArray fTMeanArrayEvent )</li>
   <li>mean Q for each ROC                           ( TObjArray fQMeanArrayEvent )</li>
   <li>parameters of a plane fit for each ROC        ( TObjArray fParamArrayEventPol1 )</li>
   <li>parameters of a 2D parabola fit for each ROC  ( TObjArray fParamArrayEventPol2 )</li>
  </ul>
 </ul>

 <h4><a name="info:retrieve">III.2 Retrieving information</a></h4>
 <ul style="list-style-type: none;">
  <li>Accessing the 'Reference Histograms' (Time0, signal width and Q sum information pad by pad):</li>
    <ul style="list-style-type: square;">
      <li>TH2F *GetHistoT0(Int_t sector);</li>
      <li>TH2F *GetHistoRMS(Int_t sector);</li>
      <li>TH2F *GetHistoQ(Int_t sector);</li>
    </ul>
    <br />
    
  <li>Accessing the calibration storage objects:</li>
    <ul style="list-style-type: square;">
      <li>AliTPCCalROC *GetCalRocT0(Int_t sector);   // for the Time0 values</li>
      <li>AliTPCCalROC *GetCalRocRMS(Int_t sector);  // for the signal width values</li>
      <li>AliTPCCalROC *GetCalRocQ(Int_t sector);    // for the Q sum values</li>
    </ul>
    <br />

  <li>Accessin the event by event information:</li>
    <ul style="list-style-type: square;">
      <li>The event by event information can be displayed using the</li>
      <li>MakeGraphTimeCE(Int_t sector, Int_t xVariable, Int_t fitType, Int_t fitParameter)</li>
      <li>which creates a graph from the specified variables</li>
    </ul>
  </ul>
  
  <h4>example for visualisation:</h4>
  <pre>
  //if the file "CEData.root" was created using the above example one could do the following:
  TFile fileCE("CEData.root")
  AliTPCCalibCE *ce = (AliTPCCalibCE*)fileCE->Get("AliTPCCalibCE");
  ce->GetCalRocT0(0)->Draw("colz");
  ce->GetCalRocRMS(0)->Draw("colz");

  //or use the AliTPCCalPad functionality:
  AliTPCCalPad padT0(ped->GetCalPadT0());
  AliTPCCalPad padSigWidth(ped->GetCalPadRMS());
  padT0->MakeHisto2D()->Draw("colz");       //Draw A-Side Time0 Information
  padSigWidth->MakeHisto2D()->Draw("colz"); //Draw A-Side signal width Information

  //display event by event information:
  //Draw mean arrival time as a function of the event time for oroc sector A00
  ce->MakeGraphTimeCE(36, 0, 2)->Draw("alp");
  //Draw first derivative in local x from a plane fit as a function of the event time for oroc sector A00
  ce->MakeGraphTimeCE(36, 0, 0, 1)->Draw("alp");  
  </pre>
END_HTML */
//////////////////////////////////////////////////////////////////////////////////////


AliTPCCalibCE::AliTPCCalibCE() :
    TObject(),
    fFirstTimeBin(650),
    fLastTimeBin(1000),
    fNbinsT0(200),
    fXminT0(-5),
    fXmaxT0(5),
    fNbinsQ(200),
    fXminQ(1),
    fXmaxQ(40),
    fNbinsRMS(100),
    fXminRMS(0.1),
    fXmaxRMS(5.1),
    fLastSector(-1),
    fOldRCUformat(kTRUE),
    fROC(AliTPCROC::Instance()),
    fParam(new AliTPCParam),
    fPedestalTPC(0x0),
    fPadNoiseTPC(0x0),
    fPedestalROC(0x0),
    fPadNoiseROC(0x0),
    fCalRocArrayT0(72),
    fCalRocArrayQ(72),
    fCalRocArrayRMS(72),
    fCalRocArrayOutliers(72),
    fHistoQArray(72),
    fHistoT0Array(72),
    fHistoRMSArray(72),
    fHistoTmean(72),
    fParamArrayEventPol1(72),
    fParamArrayEventPol2(72),
    fTMeanArrayEvent(72),
    fQMeanArrayEvent(72),
    fVEventTime(10),
    fVEventNumber(10),
    fNevents(0),
    fTimeStamp(0),
    fEventId(-1),
    fRunNumber(-1),
    fOldRunNumber(-1),
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
    fPadPedestal(0),
    fPadNoise(0),
    fVTime0Offset(72),
    fVTime0OffsetCounter(72),
    fVMeanQ(72),
    fVMeanQCounter(72),
    fEvent(-1),
    fDebugStreamer(0x0),
    fDebugLevel(0)
{
    //
    // AliTPCSignal default constructor
    //
//    fHTime0 = new TH1F("hTime0Event","hTime0Event",(fLastTimeBin-fFirstTimeBin)*10,fFirstTimeBin,fLastTimeBin);
}
//_____________________________________________________________________
AliTPCCalibCE::AliTPCCalibCE(const AliTPCCalibCE &sig) :
    TObject(sig),
    fFirstTimeBin(sig.fFirstTimeBin),
    fLastTimeBin(sig.fLastTimeBin),
    fNbinsT0(sig.fNbinsT0),
    fXminT0(sig.fXminT0),
    fXmaxT0(sig.fXmaxT0),
    fNbinsQ(sig.fNbinsQ),
    fXminQ(sig.fXminQ),
    fXmaxQ(sig.fXmaxQ),
    fNbinsRMS(sig.fNbinsRMS),
    fXminRMS(sig.fXminRMS),
    fXmaxRMS(sig.fXmaxRMS),
    fLastSector(-1),
    fOldRCUformat(kTRUE),
    fROC(AliTPCROC::Instance()),
    fParam(new AliTPCParam),
    fPedestalTPC(0x0),
    fPadNoiseTPC(0x0),
    fPedestalROC(0x0),
    fPadNoiseROC(0x0),
    fCalRocArrayT0(72),
    fCalRocArrayQ(72),
    fCalRocArrayRMS(72),
    fCalRocArrayOutliers(72),
    fHistoQArray(72),
    fHistoT0Array(72),
    fHistoRMSArray(72),
    fHistoTmean(72),
    fParamArrayEventPol1(72),
    fParamArrayEventPol2(72),
    fTMeanArrayEvent(72),
    fQMeanArrayEvent(72),
    fVEventTime(1000),
    fVEventNumber(1000),
    fNevents(sig.fNevents),
    fTimeStamp(0),
    fEventId(-1),
    fRunNumber(-1),
    fOldRunNumber(-1),
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
    fPadPedestal(0),
    fPadNoise(0),
    fVTime0Offset(72),
    fVTime0OffsetCounter(72),
    fVMeanQ(72),
    fVMeanQCounter(72),
    fEvent(-1),
    fDebugStreamer(0x0),
    fDebugLevel(sig.fDebugLevel)
{
    //
    // AliTPCSignal copy constructor
    //

    for (Int_t iSec = 0; iSec < 72; ++iSec){
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
//	    TDirectory *dir = hQ->GetDirectory();
//	    hQ->SetDirectory(0);
	    TH2S *hNew = new TH2S(*hQ);
	    hNew->SetDirectory(0);
	    fHistoQArray.AddAt(hNew,iSec);
//            hQ->SetDirectory(dir);
	}
	if ( hT0 != 0x0 ){
//	    TDirectory *dir = hT0->GetDirectory();
//	    hT0->SetDirectory(0);
	    TH2S *hNew = new TH2S(*hT0);
	    hNew->SetDirectory(0);
	    fHistoT0Array.AddAt(hNew,iSec);
//            hT0->SetDirectory(dir);
	}
	if ( hRMS != 0x0 ){
//	    TDirectory *dir = hRMS->GetDirectory();
//	    hRMS->SetDirectory(0);
	    TH2S *hNew = new TH2S(*hRMS);
	    hNew->SetDirectory(0);
	    fHistoRMSArray.AddAt(hNew,iSec);
//            hRMS->SetDirectory(dir);
	}
    }

    //copy fit parameters event by event
    TObjArray *arr=0x0;
    for (Int_t iSec=0; iSec<72; ++iSec){
	arr = (TObjArray*)sig.fParamArrayEventPol1.UncheckedAt(iSec);
	if ( arr ){
            TObjArray *arrEvents = new TObjArray(arr->GetSize());
	    fParamArrayEventPol1.AddAt(arrEvents, iSec);
	    for (Int_t iEvent=0; iEvent<arr->GetSize(); ++iEvent)
		if ( TVectorD *vec=(TVectorD*)arr->UncheckedAt(iEvent) )
		    arrEvents->AddAt(new TVectorD(*vec),iEvent);
	}

	arr = (TObjArray*)sig.fParamArrayEventPol2.UncheckedAt(iSec);
	if ( arr ){
            TObjArray *arrEvents = new TObjArray(arr->GetSize());
	    fParamArrayEventPol2.AddAt(arrEvents, iSec);
	    for (Int_t iEvent=0; iEvent<arr->GetSize(); ++iEvent)
		if ( TVectorD *vec=(TVectorD*)arr->UncheckedAt(iEvent) )
		    arrEvents->AddAt(new TVectorD(*vec),iEvent);
	}

	TVectorF *vMeanTime = (TVectorF*)sig.fTMeanArrayEvent.UncheckedAt(iSec);
	TVectorF *vMeanQ    = (TVectorF*)sig.fQMeanArrayEvent.UncheckedAt(iSec);
	if ( vMeanTime )
	    fTMeanArrayEvent.AddAt(new TVectorF(*vMeanTime), iSec);
	if ( vMeanQ )
	    fQMeanArrayEvent.AddAt(new TVectorF(*vMeanQ), iSec);
    }


    fVEventTime.ResizeTo(sig.fVEventTime);
    fVEventNumber.ResizeTo(sig.fVEventNumber);
    fVEventTime.SetElements(sig.fVEventTime.GetMatrixArray());
    fVEventNumber.SetElements(sig.fVEventNumber.GetMatrixArray());

}
//_____________________________________________________________________
AliTPCCalibCE& AliTPCCalibCE::operator = (const  AliTPCCalibCE &source)
{
  //
  // assignment operator
  //
  if (&source == this) return *this;
  new (this) AliTPCCalibCE(source);

  return *this;
}
//_____________________________________________________________________
AliTPCCalibCE::~AliTPCCalibCE()
{
    //
    // destructor
    //

    fCalRocArrayT0.Delete();
    fCalRocArrayQ.Delete();
    fCalRocArrayRMS.Delete();
    fCalRocArrayOutliers.Delete();

    fHistoQArray.Delete();
    fHistoT0Array.Delete();
    fHistoRMSArray.Delete();

    fHistoTmean.Delete();

    fParamArrayEventPol1.Delete();
    fParamArrayEventPol2.Delete();
    fTMeanArrayEvent.Delete();
    fQMeanArrayEvent.Delete();

    fPadTimesArrayEvent.Delete();
    fPadQArrayEvent.Delete();
    fPadRMSArrayEvent.Delete();
    fPadPedestalArrayEvent.Delete();

    if ( fDebugStreamer) delete fDebugStreamer;
//    if ( fHTime0 ) delete fHTime0;
    delete fROC;
    delete fParam;
}
//_____________________________________________________________________
Int_t AliTPCCalibCE::Update(const Int_t icsector,
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

    //fill signals for current pad
    fPadSignal.GetMatrixArray()[icTimeBin]=csignal;
    if ( csignal > fMaxPadSignal ){
	fMaxPadSignal = csignal;
	fMaxTimeBin   = icTimeBin;
    }
    return 0;
}
//_____________________________________________________________________
void AliTPCCalibCE::FindPedestal(Float_t part)
{
    //
    // find pedestal and noise for the current pad. Use either database or
    // truncated mean with part*100%
    //
    Bool_t noPedestal = kTRUE;

    //use pedestal database if set
    if (fPedestalTPC&&fPadNoiseTPC){
        //only load new pedestals if the sector has changed
	if ( fCurrentSector!=fLastSector ){
	    fPedestalROC = fPedestalTPC->GetCalROC(fCurrentSector);
            fPadNoiseROC = fPadNoiseTPC->GetCalROC(fCurrentSector);
	    fLastSector=fCurrentSector;
	}

	if ( fPedestalROC&&fPadNoiseROC ){
	    fPadPedestal = fPedestalROC->GetValue(fCurrentChannel);
	    fPadNoise    = fPadNoiseROC->GetValue(fCurrentChannel);
            noPedestal   = kFALSE;
	}

    }

    //if we are not running with pedestal database, or for the current sector there is no information
    //available, calculate the pedestal and noise on the fly
    if ( noPedestal ) {
	const Int_t kPedMax = 100;  //maximum pedestal value
	Float_t  max    =  0;
	Float_t  maxPos =  0;
	Int_t    median =  -1;
	Int_t    count0 =  0;
	Int_t    count1 =  0;
	//
	Float_t padSignal=0;
        //
	UShort_t histo[kPedMax];
	memset(histo,0,kPedMax*sizeof(UShort_t));

        //fill pedestal histogram
	for (Int_t i=fFirstTimeBin; i<=fLastTimeBin; ++i){
            padSignal = fPadSignal.GetMatrixArray()[i];
	    if (padSignal<=0) continue;
	    if (padSignal>max && i>10) {
		max = padSignal;
		maxPos = i;
	    }
	    if (padSignal>kPedMax-1) continue;
	    histo[int(padSignal+0.5)]++;
	    count0++;
	}
	//find median
	for (Int_t i=1; i<kPedMax; ++i){
	    if (count1<count0*0.5) median=i;
	    count1+=histo[i];
	}
	// truncated mean
	//
	Float_t count=histo[median] ,mean=histo[median]*median,  rms=histo[median]*median*median ;
	//
	for (Int_t idelta=1; idelta<10; ++idelta){
	    if (median-idelta<=0) continue;
	    if (median+idelta>kPedMax) continue;
	    if (count<part*count1){
		count+=histo[median-idelta];
		mean +=histo[median-idelta]*(median-idelta);
		rms  +=histo[median-idelta]*(median-idelta)*(median-idelta);
		count+=histo[median+idelta];
		mean +=histo[median+idelta]*(median+idelta);
		rms  +=histo[median+idelta]*(median+idelta)*(median+idelta);
	    }
	}
	fPadPedestal = 0;
	fPadNoise    = 0;
	if ( count > 0 ) {
	    mean/=count;
	    rms    = TMath::Sqrt(TMath::Abs(rms/count-mean*mean));
	    fPadPedestal = mean;
	    fPadNoise    = rms;
	} 
    }
}
//_____________________________________________________________________
void AliTPCCalibCE::FindCESignal(TVectorD &param, Float_t &qSum, const TVectorF maxima)
{
    //
    //  Find position, signal width and height of the CE signal (last signal)
    //  param[0] = Qmax, param[1] = mean time, param[2] = rms;
    //  maxima: array of local maxima of the pad signal use the one closest to the mean CE position
    //

    Float_t ceQmax  =0, ceQsum=0, ceTime=0, ceRMS=0;
    Int_t   cemaxpos       = 0;
    Float_t ceSumThreshold = 8.*fPadNoise;  // threshold for the signal sum
    const Int_t    kCemin  = 4;             // range for the analysis of the ce signal +- channels from the peak
    const Int_t    kCemax  = 7;

    Float_t minDist  = 25;  //initial minimum distance betweek roc mean ce signal and pad ce signal

    // find maximum closest to the sector mean from the last event
    for ( Int_t imax=0; imax<maxima.GetNrows(); ++imax){
        // get sector mean of last event
	Float_t tmean = (*GetTMeanEvents(fCurrentSector))[fNevents-1];
	    if ( TMath::Abs( tmean-maxima[imax] ) < minDist ) {
		minDist  = tmean-maxima[imax];
                cemaxpos = (Int_t)maxima[imax];
	    }
    }

    if (cemaxpos!=0){
        ceQmax = fPadSignal.GetMatrixArray()[cemaxpos]-fPadPedestal;
	for (Int_t i=cemaxpos-kCemin; i<cemaxpos+kCemax; ++i){
	    if ( (i>fFirstTimeBin) && (i<fLastTimeBin) ){
		Float_t signal = fPadSignal.GetMatrixArray()[i]-fPadPedestal;
		if (signal>0) {
		    ceTime+=signal*(i+0.5);
		    ceRMS +=signal*(i+0.5)*(i+0.5);
		    ceQsum+=signal;
		}
	    }
	}
    }
    if (ceQmax&&ceQsum>ceSumThreshold) {
	ceTime/=ceQsum;
	ceRMS  = TMath::Sqrt(TMath::Abs(ceRMS/ceQsum-ceTime*ceTime));
	fVTime0Offset.GetMatrixArray()[fCurrentSector]+=ceTime;   // mean time for each sector
	fVTime0OffsetCounter.GetMatrixArray()[fCurrentSector]++;

	//Normalise Q to pad area of irocs
	Float_t norm = fParam->GetPadPitchWidth(fCurrentSector)*fParam->GetPadPitchLength(fCurrentSector,fCurrentRow);

	ceQsum/=norm;
	fVMeanQ.GetMatrixArray()[fCurrentSector]+=ceQsum;
        fVMeanQCounter.GetMatrixArray()[fCurrentSector]++;
    } else {
	ceQmax=0;
	ceTime=0;
	ceRMS =0;
	ceQsum=0;
    }
    param[0] = ceQmax;
    param[1] = ceTime;
    param[2] = ceRMS;
    qSum     = ceQsum;
}
//_____________________________________________________________________
Bool_t AliTPCCalibCE::IsPeak(Int_t pos, Int_t tminus, Int_t tplus)
{
    //
    // Check if 'pos' is a Maximum. Consider 'tminus' timebins before
    // and 'tplus' timebins after 'pos'
    //
    if ( (pos-tminus)<fFirstTimeBin || (pos+tplus)>fLastTimeBin ) return kFALSE;
    for (Int_t iTime = pos; iTime>pos-tminus; --iTime)
	if ( fPadSignal[iTime-1] >= fPadSignal[iTime] ) return kFALSE;
    for (Int_t iTime = pos, iTime2=pos; iTime<pos+tplus; ++iTime, ++iTime2){
	if ( (iTime==pos) && (fPadSignal[iTime+1]==fPadSignal[iTime]) ) // allow two timebins with same adc value
	    ++iTime2;
	if ( fPadSignal[iTime2+1] >= fPadSignal[iTime2] ) return kFALSE;
    }
    return kTRUE;
}
//_____________________________________________________________________
void AliTPCCalibCE::FindLocalMaxima(TVectorF &maxima)
{
    //
    // Find local maxima on the pad signal and Histogram them
    //
    Float_t ceThreshold = 5.*fPadNoise;  // threshold for the signal
    Int_t   count       = 0;
    Int_t   tminus      = 2;
    Int_t   tplus       = 3;
    for (Int_t i=fLastTimeBin-tplus-1; i>=fFirstTimeBin+tminus; --i){
	if ( (fPadSignal[i]-fPadPedestal)>ceThreshold && IsPeak(i,tminus,tplus) ){
	  if (count<maxima.GetNrows()){
	    maxima.GetMatrixArray()[count++]=i;
	    GetHistoTmean(fCurrentSector,kTRUE)->Fill(i);
	  }
	}
    }
}
//_____________________________________________________________________
void AliTPCCalibCE::ProcessPad()
{
    //
    //  Process data of current pad
    //
    FindPedestal();

    TVectorF maxima(15);     // the expected maximum number of maxima in the complete TPC should be 8 laser beam layers
                             // + central electrode and possibly post peaks from the CE signal
                             // however if we are on a high noise pad a lot more peaks due to the noise might occur
    FindLocalMaxima(maxima);
    if ( (fNevents == 0) || (fOldRunNumber!=fRunNumber) ) return;  // return because we don't have Time0 info for the CE yet



    TVectorD param(3);
    Float_t  Qsum;
    FindCESignal(param, Qsum, maxima);

    Double_t meanT  = param[1];
    Double_t sigmaT = param[2];

    //Fill Event T0 counter
    (*GetPadTimesEvent(fCurrentSector,kTRUE)).GetMatrixArray()[fCurrentChannel] = meanT;

    //Fill Q histogram
    GetHistoQ(fCurrentSector,kTRUE)->Fill( TMath::Sqrt(Qsum), fCurrentChannel );

    //Fill RMS histogram
    GetHistoRMS(fCurrentSector,kTRUE)->Fill( sigmaT, fCurrentChannel );


    //Fill debugging info
    if ( fDebugLevel>0 ){
	(*GetPadPedestalEvent(fCurrentSector,kTRUE)).GetMatrixArray()[fCurrentChannel]=fPadPedestal;
	(*GetPadRMSEvent(fCurrentSector,kTRUE)).GetMatrixArray()[fCurrentChannel]=sigmaT;
	(*GetPadQEvent(fCurrentSector,kTRUE)).GetMatrixArray()[fCurrentChannel]=Qsum;
    }

    ResetPad();
}
//_____________________________________________________________________
void AliTPCCalibCE::EndEvent()
{
    //
    //  Process data of current pad
    //  The Functions 'SetTimeStamp' and 'SetRunNumber'  should be called
    //  before the EndEvent function to set the event timestamp and number!!!
    //  This is automatically done if the ProcessEvent(AliRawReader *rawReader)
    //  function was called
    //

    //check if last pad has allready been processed, if not do so
    if ( fMaxTimeBin>-1 ) ProcessPad();

    TVectorD param(3);
    TMatrixD dummy(3,3);
//    TVectorF vMeanTime(72);
//    TVectorF vMeanQ(72);
    AliTPCCalROC *calIroc=new AliTPCCalROC(0);
    AliTPCCalROC *calOroc=new AliTPCCalROC(36);

    //find mean time0 offset for side A and C
    Double_t time0Side[2];       //time0 for side A:0 and C:0
    Double_t time0SideCount[2];  //time0 counter for side A:0 and C:0
    time0Side[0]=0;time0Side[1]=0;time0SideCount[0]=0;time0SideCount[1]=0;
    for ( Int_t iSec = 0; iSec<72; ++iSec ){
	time0Side[(iSec/18)%2] += fVTime0Offset.GetMatrixArray()[iSec];
	time0SideCount[(iSec/18)%2] += fVTime0OffsetCounter.GetMatrixArray()[iSec];
    }
    if ( time0SideCount[0] >0  )
	time0Side[0]/=time0SideCount[0];
    if ( time0SideCount[1] >0 )
	time0Side[1]/=time0SideCount[1];
    // end find time0 offset

    //loop over all ROCs, fill CE Time histogram corrected for the mean Time0 of each ROC
    for ( Int_t iSec = 0; iSec<72; ++iSec ){
      //find median and then calculate the mean around it
	TH1S *hMeanT    = GetHistoTmean(iSec); //histogram with local maxima position information
	if ( !hMeanT ) continue;

	Double_t entries = hMeanT->GetEntries();
	Double_t sum     = 0;
	Short_t *arr     = hMeanT->GetArray()+1;
        Int_t ibin=0;
	for ( ibin=0; ibin<hMeanT->GetNbinsX(); ++ibin){
	    sum+=arr[ibin];
            if ( sum>=(entries/2) ) break;
	}
	Int_t delta = 4;
        Int_t firstBin = fFirstTimeBin+ibin-delta;
	Int_t lastBin  = fFirstTimeBin+ibin+delta;
        if ( firstBin<fFirstTimeBin ) firstBin=fFirstTimeBin;
        if ( lastBin>fLastTimeBin   ) lastBin =fLastTimeBin;
	Float_t median =AliMathBase::GetCOG(arr+ibin-delta,2*delta,firstBin,lastBin);

	// check boundaries for ebye info of mean time
	TVectorF *vMeanTime=GetTMeanEvents(iSec,kTRUE);
        Int_t vSize=vMeanTime->GetNrows();
	if ( vSize < fNevents+1 )
            vMeanTime->ResizeTo(vSize+100);

	vMeanTime->GetMatrixArray()[fNevents]=median;
      // end find median

	TVectorF *vTimes = GetPadTimesEvent(iSec);
	if ( !vTimes ) continue;                     //continue if no time information for this sector is available


	AliTPCCalROC calIrocOutliers(0);
	AliTPCCalROC calOrocOutliers(36);

        // calculate mean Q of the sector
	Float_t meanQ = 0;
	if ( fVMeanQCounter.GetMatrixArray()[iSec]>0 ) meanQ=fVMeanQ.GetMatrixArray()[iSec]/fVMeanQCounter.GetMatrixArray()[iSec];
	TVectorF *vMeanQ=GetQMeanEvents(iSec,kTRUE);
	if ( vSize < fNevents+1 )           // vSize is the same as for vMeanTime!
            vMeanQ->ResizeTo(vSize+100);

	vMeanQ->GetMatrixArray()[fNevents]=meanQ;

	for ( UInt_t iChannel=0; iChannel<fROC->GetNChannels(iSec); ++iChannel ){
	    Float_t Time  = (*vTimes).GetMatrixArray()[iChannel];

	    //set values for temporary roc calibration class
	    if ( iSec < 36 ) {
		calIroc->SetValue(iChannel, Time);
                if ( Time == 0 ) calIrocOutliers.SetValue(iChannel,1);

	    } else {
		calOroc->SetValue(iChannel, Time);
                if ( Time == 0 ) calOrocOutliers.SetValue(iChannel,1);
	    }

	    if ( (fNevents>0) && (fOldRunNumber==fRunNumber) )
		GetHistoT0(iSec,kTRUE)->Fill( Time-time0Side[(iSec/18)%2],iChannel );



	    //-------------------------------  Debug start  ------------------------------
	    if ( fDebugLevel>0 ){
		if ( !fDebugStreamer ) {
                        //debug stream
		    TDirectory *backup = gDirectory;
		    fDebugStreamer = new TTreeSRedirector("debugCalibCE.root");
		    if ( backup ) backup->cd();  //we don't want to be cd'd to the debug streamer
		}

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

//		TH1F *h1 = new TH1F(Form("hSignalD%d.%d.%d",sector,row,pad),
//				    Form("hSignalD%d.%d.%d",sector,row,pad),
//				    fLastTimeBin-fFirstTimeBin,
//				    fFirstTimeBin,fLastTimeBin);
//		h1->SetDirectory(0);
//
//		for (Int_t i=fFirstTimeBin; i<fLastTimeBin+1; ++i)
//		    h1->Fill(i,fPadSignal(i));

		Double_t T0Sec = 0;
		if (fVTime0OffsetCounter.GetMatrixArray()[iSec]>0)
		    T0Sec = fVTime0Offset.GetMatrixArray()[iSec]/fVTime0OffsetCounter.GetMatrixArray()[iSec];
		Double_t T0Side = time0Side[(iSec/18)%2];
		(*fDebugStreamer) << "DataPad" <<
		    "Event=" << fNevents <<
		    "RunNumber=" << fRunNumber <<
		    "TimeStamp="   << fTimeStamp <<
		    "Sector="<< sector <<
		    "Row="   << row<<
		    "Pad="   << pad <<
		    "PadC="  << padc <<
		    "PadSec="<< channel <<
		    "Time0Sec="  << T0Sec <<
		    "Time0Side=" << T0Side <<
		    "Time="  << Time <<
		    "RMS="   << RMS <<
		    "Sum="   << Q <<
                    "MeanQ=" << meanQ <<
		    //		    "hist.=" << h1 <<
		    "\n";

		//		delete h1;

	    }
	    //-----------------------------  Debug end  ------------------------------
	}// end channel loop
	hMeanT->Reset();

	TVectorD paramPol1(3);
	TVectorD paramPol2(6);
	TMatrixD matPol1(3,3);
	TMatrixD matPol2(6,6);
	Float_t  chi2Pol1=0;
	Float_t  chi2Pol2=0;

	if ( (fNevents>0) && (fOldRunNumber==fRunNumber) ){
	    if ( iSec < 36 ){
		calIroc->GlobalFit(&calIrocOutliers,0,paramPol1,matPol1,chi2Pol1,0);
		calIroc->GlobalFit(&calIrocOutliers,0,paramPol2,matPol2,chi2Pol2,1);
	    } else {
		calOroc->GlobalFit(&calOrocOutliers,0,paramPol1,matPol1,chi2Pol1,0);
		calOroc->GlobalFit(&calOrocOutliers,0,paramPol2,matPol2,chi2Pol2,1);
	    }

	    GetParamArrayPol1(iSec,kTRUE)->AddAtAndExpand(new TVectorD(paramPol1), fNevents);
	    GetParamArrayPol2(iSec,kTRUE)->AddAtAndExpand(new TVectorD(paramPol2), fNevents);
	}
//	printf("events: %d -- size: %d\n",fNevents,GetParamArrayPol1(iSec)->GetSize());

	//-------------------------------  Debug start  ------------------------------
	if ( fDebugLevel>0 ){
	    if ( !fDebugStreamer ) {
		//debug stream
		TDirectory *backup = gDirectory;
		fDebugStreamer = new TTreeSRedirector("debugCalibCE.root");
		if ( backup ) backup->cd();  //we don't want to be cd'd to the debug streamer
	    }
	    (*fDebugStreamer) << "DataRoc" <<
		"Event=" << fEvent <<
		"RunNumber=" << fRunNumber <<
		"TimeStamp="   << fTimeStamp <<
		"Sector="<< iSec <<
		"hMeanT.=" << hMeanT <<
		"median=" << median <<
		"paramPol1.=" << &paramPol1 <<
		"paramPol2.=" << &paramPol2 <<
		"matPol1.="   << &matPol1 <<
		"matPol2.="   << &matPol2 <<
		"chi2Pol1="   << chi2Pol1 <<
                "chi2Pol2="   << chi2Pol2 <<
		"\n";
	}
	//-------------------------------  Debug end  ------------------------------
    }// end sector loop

//    fTMeanArrayEvent.AddAtAndExpand(new TVectorF(vMeanTime),fNevents);
//    fQMeanArrayEvent.AddAtAndExpand(new TVectorF(vMeanQ),fNevents);
    if ( fVEventTime.GetNrows() < fNevents+1 ) {
	fVEventTime.ResizeTo((Int_t)(fVEventTime.GetNrows()+100));
	fVEventNumber.ResizeTo((Int_t)(fVEventNumber.GetNrows()+100));
    }
    fVEventTime.GetMatrixArray()[fNevents] = fTimeStamp;
    fVEventNumber.GetMatrixArray()[fNevents] = fEventId;

    fNevents++;
    fOldRunNumber = fRunNumber;

    delete calIroc;
    delete calOroc;
}
//_____________________________________________________________________
Bool_t AliTPCCalibCE::ProcessEvent(AliTPCRawStream *rawStream)
{
  //
  // Event Processing loop - AliTPCRawStream
  // The Function 'SetTimeStamp' should be called for each event to set the event time stamp!!!
  //

  rawStream->SetOldRCUFormat(fOldRCUformat);

  ResetEvent();

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

  if (withInput){
      EndEvent();
  }

  return withInput;
}
//_____________________________________________________________________
Bool_t AliTPCCalibCE::ProcessEvent(AliRawReader *rawReader)
{
  //
  //  Event processing loop - AliRawReader
  //


    AliTPCRawStream rawStream(rawReader);
    AliRawEventHeaderBase* eventHeader = (AliRawEventHeaderBase*)rawReader->GetEventHeader();
    if (eventHeader){
	fTimeStamp   = eventHeader->Get("Timestamp");
	fRunNumber = eventHeader->Get("RunNb");
    }
    fEventId = *rawReader->GetEventId();


    rawReader->Select("TPC");

  return ProcessEvent(&rawStream);
}
//_____________________________________________________________________
Bool_t AliTPCCalibCE::ProcessEvent(eventHeaderStruct *event)
{
  //
  //  Event processing loop - date event
  //
    AliRawReader *rawReader = new AliRawReaderDate((void*)event);
    Bool_t result=ProcessEvent(rawReader);
    delete rawReader;
    return result;

}
//_____________________________________________________________________
TH2S* AliTPCCalibCE::GetHisto(Int_t sector, TObjArray *arr,
				  Int_t nbinsY, Float_t ymin, Float_t ymax,
				  Char_t *type, Bool_t force)
{
    //
    // return pointer to TH2S histogram of 'type'
    // if force is true create a new histogram if it doesn't exist allready
    //
    if ( !force || arr->UncheckedAt(sector) )
	return (TH2S*)arr->UncheckedAt(sector);

    // if we are forced and histogram doesn't exist yet create it
    Char_t name[255], title[255];

    sprintf(name,"hCalib%s%.2d",type,sector);
    sprintf(title,"%s calibration histogram sector %.2d",type,sector);

    // new histogram with Q calib information. One value for each pad!
    TH2S* hist = new TH2S(name,title,
			  nbinsY, ymin, ymax,
			  fROC->GetNChannels(sector),0,fROC->GetNChannels(sector));
    hist->SetDirectory(0);
    arr->AddAt(hist,sector);
    return hist;
}
//_____________________________________________________________________
TH2S* AliTPCCalibCE::GetHistoT0(Int_t sector, Bool_t force)
{
    //
    // return pointer to T0 histogram
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fHistoT0Array;
    return GetHisto(sector, arr, fNbinsT0, fXminT0, fXmaxT0, "T0", force);
}
//_____________________________________________________________________
TH2S* AliTPCCalibCE::GetHistoQ(Int_t sector, Bool_t force)
{
    //
    // return pointer to Q histogram
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fHistoQArray;
    return GetHisto(sector, arr, fNbinsQ, fXminQ, fXmaxQ, "Q", force);
}
//_____________________________________________________________________
TH2S* AliTPCCalibCE::GetHistoRMS(Int_t sector, Bool_t force)
{
    //
    // return pointer to Q histogram
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fHistoRMSArray;
    return GetHisto(sector, arr, fNbinsRMS, fXminRMS, fXmaxRMS, "RMS", force);
}
//_____________________________________________________________________
TH1S* AliTPCCalibCE::GetHisto(Int_t sector, TObjArray *arr,
			      Char_t *type, Bool_t force)
{
    //
    // return pointer to TH1S histogram
    // if force is true create a new histogram if it doesn't exist allready
    //
    if ( !force || arr->UncheckedAt(sector) )
	return (TH1S*)arr->UncheckedAt(sector);

    // if we are forced and histogram doesn't yes exist create it
    Char_t name[255], title[255];

    sprintf(name,"hCalib%s%.2d",type,sector);
    sprintf(title,"%s calibration histogram sector %.2d",type,sector);

    // new histogram with Q calib information. One value for each pad!
    TH1S* hist = new TH1S(name,title,
			  fLastTimeBin-fFirstTimeBin,fFirstTimeBin,fLastTimeBin);
    hist->SetDirectory(0);
    arr->AddAt(hist,sector);
    return hist;
}
//_____________________________________________________________________
TH1S* AliTPCCalibCE::GetHistoTmean(Int_t sector, Bool_t force)
{
    //
    // return pointer to Q histogram
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fHistoTmean;
    return GetHisto(sector, arr, "LastTmean", force);
}
//_____________________________________________________________________
TVectorF* AliTPCCalibCE::GetVectSector(Int_t sector, TObjArray *arr, UInt_t size, Bool_t force)
{
    //
    // return pointer to Pad Info from 'arr' for the current event and sector
    // if force is true create it if it doesn't exist allready
    //
    if ( !force || arr->UncheckedAt(sector) )
	return (TVectorF*)arr->UncheckedAt(sector);

    TVectorF *vect = new TVectorF(size);
    arr->AddAt(vect,sector);
    return vect;
}
//_____________________________________________________________________
TVectorF* AliTPCCalibCE::GetPadTimesEvent(Int_t sector, Bool_t force)
{
    //
    // return pointer to Pad Times Array for the current event and sector
    // if force is true create it if it doesn't exist allready
    //
    TObjArray *arr = &fPadTimesArrayEvent;
    return GetVectSector(sector,arr,fROC->GetNChannels(sector),force);
}
//_____________________________________________________________________
TVectorF* AliTPCCalibCE::GetPadQEvent(Int_t sector, Bool_t force)
{
    //
    // return pointer to Pad Q Array for the current event and sector
    // if force is true create it if it doesn't exist allready
    // for debugging purposes only
    //

    TObjArray *arr = &fPadQArrayEvent;
    return GetVectSector(sector,arr,fROC->GetNChannels(sector),force);
}
//_____________________________________________________________________
TVectorF* AliTPCCalibCE::GetPadRMSEvent(Int_t sector, Bool_t force)
{
    //
    // return pointer to Pad RMS Array for the current event and sector
    // if force is true create it if it doesn't exist allready
    // for debugging purposes only
    //
    TObjArray *arr = &fPadRMSArrayEvent;
    return GetVectSector(sector,arr,fROC->GetNChannels(sector),force);
}
//_____________________________________________________________________
TVectorF* AliTPCCalibCE::GetPadPedestalEvent(Int_t sector, Bool_t force)
{
    //
    // return pointer to Pad RMS Array for the current event and sector
    // if force is true create it if it doesn't exist allready
    // for debugging purposes only
    //
    TObjArray *arr = &fPadPedestalArrayEvent;
    return GetVectSector(sector,arr,fROC->GetNChannels(sector),force);
}
//_____________________________________________________________________
TVectorF* AliTPCCalibCE::GetTMeanEvents(Int_t sector, Bool_t force)
{
    //
    // return pointer to the EbyE info of the mean arrival time for 'sector'
    // if force is true create it if it doesn't exist allready
    //
    TObjArray *arr = &fTMeanArrayEvent;
    return GetVectSector(sector,arr,100,force);
}
//_____________________________________________________________________
TVectorF* AliTPCCalibCE::GetQMeanEvents(Int_t sector, Bool_t force)
{
    //
    // return pointer to the EbyE info of the mean arrival time for 'sector'
    // if force is true create it if it doesn't exist allready
    //
    TObjArray *arr = &fQMeanArrayEvent;
    return GetVectSector(sector,arr,100,force);
}
//_____________________________________________________________________
AliTPCCalROC* AliTPCCalibCE::GetCalRoc(Int_t sector, TObjArray* arr, Bool_t force)
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
    arr->AddAt(croc,sector);
    return croc;
}
//_____________________________________________________________________
AliTPCCalROC* AliTPCCalibCE::GetCalRocT0(Int_t sector, Bool_t force)
{
    //
    // return pointer to Carge ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalRocArrayT0;
    return GetCalRoc(sector, arr, force);
}
//_____________________________________________________________________
AliTPCCalROC* AliTPCCalibCE::GetCalRocQ(Int_t sector, Bool_t force)
{
    //
    // return pointer to T0 ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalRocArrayQ;
    return GetCalRoc(sector, arr, force);
}
//_____________________________________________________________________
AliTPCCalROC* AliTPCCalibCE::GetCalRocRMS(Int_t sector, Bool_t force)
{
    //
    // return pointer to signal width ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalRocArrayRMS;
    return GetCalRoc(sector, arr, force);
}
//_____________________________________________________________________
AliTPCCalROC* AliTPCCalibCE::GetCalRocOutliers(Int_t sector, Bool_t force)
{
    //
    // return pointer to Outliers
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalRocArrayOutliers;
    return GetCalRoc(sector, arr, force);
}
//_____________________________________________________________________
TObjArray* AliTPCCalibCE::GetParamArray(Int_t sector, TObjArray* arr, Bool_t force)
{
    //
    // return pointer to TObjArray of fit parameters
    // if force is true create a new histogram if it doesn't exist allready
    //
    if ( !force || arr->UncheckedAt(sector) )
	return (TObjArray*)arr->UncheckedAt(sector);

    // if we are forced and array doesn't yes exist create it

    // new TObjArray for parameters
    TObjArray *newArr = new TObjArray;
    arr->AddAt(newArr,sector);
    return newArr;
}
//_____________________________________________________________________
TObjArray* AliTPCCalibCE::GetParamArrayPol1(Int_t sector, Bool_t force)
{
    //
    // return pointer to TObjArray of fit parameters from plane fit
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fParamArrayEventPol1;
    return GetParamArray(sector, arr, force);
}
//_____________________________________________________________________
TObjArray* AliTPCCalibCE::GetParamArrayPol2(Int_t sector, Bool_t force)
{
    //
    // return pointer to TObjArray of fit parameters from parabola fit
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fParamArrayEventPol2;
    return GetParamArray(sector, arr, force);
}
//_____________________________________________________________________
void AliTPCCalibCE::ResetEvent()
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

    for ( Int_t i=0; i<72; ++i ){
	fVTime0Offset.GetMatrixArray()[i]=0;
	fVTime0OffsetCounter.GetMatrixArray()[i]=0;
	fVMeanQ.GetMatrixArray()[i]=0;
        fVMeanQCounter.GetMatrixArray()[i]=0;
    }
}
//_____________________________________________________________________
void AliTPCCalibCE::ResetPad()
{
    //
    //  Reset pad infos -- Should be called after a pad has been processed
    //
    for (Int_t i=fFirstTimeBin; i<fLastTimeBin+1; ++i)
	fPadSignal.GetMatrixArray()[i] = 0;
    fMaxTimeBin   = -1;
    fMaxPadSignal = -1;
    fPadPedestal  = -1;
    fPadNoise     = -1;
}
//_____________________________________________________________________
void AliTPCCalibCE::Merge(AliTPCCalibCE *ce)
{
    //
    //  Merge ce to the current AliTPCCalibCE
    //

    //merge histograms
    for (Int_t iSec=0; iSec<72; ++iSec){
	TH2S *hRefQmerge   = ce->GetHistoQ(iSec);
	TH2S *hRefT0merge  = ce->GetHistoT0(iSec);
	TH2S *hRefRMSmerge = ce->GetHistoRMS(iSec);


	if ( hRefQmerge ){
	    TDirectory *dir = hRefQmerge->GetDirectory(); hRefQmerge->SetDirectory(0);
	    TH2S *hRefQ   = GetHistoQ(iSec);
	    if ( hRefQ ) hRefQ->Add(hRefQmerge);
	    else {
		TH2S *hist = new TH2S(*hRefQmerge);
                hist->SetDirectory(0);
		fHistoQArray.AddAt(hist, iSec);
	    }
	    hRefQmerge->SetDirectory(dir);
	}
	if ( hRefT0merge ){
	    TDirectory *dir = hRefT0merge->GetDirectory(); hRefT0merge->SetDirectory(0);
	    TH2S *hRefT0  = GetHistoT0(iSec);
	    if ( hRefT0 ) hRefT0->Add(hRefT0merge);
	    else {
		TH2S *hist = new TH2S(*hRefT0merge);
                hist->SetDirectory(0);
		fHistoT0Array.AddAt(hist, iSec);
	    }
	    hRefT0merge->SetDirectory(dir);
	}
	if ( hRefRMSmerge ){
	    TDirectory *dir = hRefRMSmerge->GetDirectory(); hRefRMSmerge->SetDirectory(0);
	    TH2S *hRefRMS = GetHistoRMS(iSec);
	    if ( hRefRMS ) hRefRMS->Add(hRefRMSmerge);
	    else {
		TH2S *hist = new TH2S(*hRefRMSmerge);
                hist->SetDirectory(0);
		fHistoRMSArray.AddAt(hist, iSec);
	    }
	    hRefRMSmerge->SetDirectory(dir);
	}

    }

    // merge time information


    Int_t nCEevents = ce->GetNeventsProcessed();
    for (Int_t iSec=0; iSec<72; ++iSec){
	TObjArray *arrPol1CE  = ce->GetParamArrayPol1(iSec);
	TObjArray *arrPol2CE  = ce->GetParamArrayPol2(iSec);
	TVectorF *vMeanTimeCE = ce->GetTMeanEvents(iSec);
	TVectorF *vMeanQCE    = ce->GetQMeanEvents(iSec);

	TObjArray *arrPol1  = 0x0;
	TObjArray *arrPol2  = 0x0;
	TVectorF *vMeanTime = 0x0;
	TVectorF *vMeanQ    = 0x0;

	//resize arrays
	if ( arrPol1CE && arrPol2CE ){
	    arrPol1 = GetParamArrayPol1(iSec,kTRUE);
	    arrPol2 = GetParamArrayPol2(iSec,kTRUE);
	    arrPol1->Expand(fNevents+nCEevents);
            arrPol2->Expand(fNevents+nCEevents);
	}
	if ( vMeanTimeCE && vMeanQCE ){
	    vMeanTime = GetTMeanEvents(iSec);
	    vMeanQCE  = GetQMeanEvents(iSec);
	    vMeanTime->ResizeTo(fNevents+nCEevents);
	    vMeanQ->ResizeTo(fNevents+nCEevents);
	}


	for (Int_t iEvent=0; iEvent<nCEevents; ++iEvent){
	    if ( arrPol1CE && arrPol2CE ){
		TVectorD *paramPol1 = (TVectorD*)(arrPol1CE->UncheckedAt(iEvent));
		TVectorD *paramPol2 = (TVectorD*)(arrPol2CE->UncheckedAt(iEvent));
		if ( paramPol1 && paramPol2 ){
		    GetParamArrayPol1(iSec,kTRUE)->AddAt(new TVectorD(*paramPol1), fNevents+iEvent);
		    GetParamArrayPol2(iSec,kTRUE)->AddAt(new TVectorD(*paramPol2), fNevents+iEvent);
		}
	    }
	    if ( vMeanTimeCE && vMeanQCE ){
		vMeanTime->GetMatrixArray()[fNevents+iEvent]=vMeanTimeCE->GetMatrixArray()[iEvent];
                vMeanQ->GetMatrixArray()[fNevents+iEvent]=vMeanQCE->GetMatrixArray()[iEvent];
	    }
	}
    }



    TVectorD*  eventTimes  = ce->GetEventTimes();
    TVectorD*  eventIds  = ce->GetEventIds();
    fVEventTime.ResizeTo(fNevents+nCEevents);
    fVEventNumber.ResizeTo(fNevents+nCEevents);

    for (Int_t iEvent=0; iEvent<nCEevents; ++iEvent){
	Double_t evTime     = eventTimes->GetMatrixArray()[iEvent];
        Double_t evId       = eventIds->GetMatrixArray()[iEvent];
	fVEventTime.GetMatrixArray()[fNevents+iEvent] = evTime;
	fVEventNumber.GetMatrixArray()[fNevents+iEvent] = evId;
    }
    fNevents+=nCEevents; //increase event counter

}
//_____________________________________________________________________
TGraph *AliTPCCalibCE::MakeGraphTimeCE(Int_t sector, Int_t xVariable, Int_t fitType, Int_t fitParameter)
{
    //
    // Make graph from fit parameters of pol1 fit, pol2 fit, mean arrival time or mean Q for ROC 'sector'
    // xVariable:    0-event time, 1-event id, 2-internal event counter
    // fitType:      0-pol1 fit, 1-pol2 fit, 2-mean time, 3-mean Q
    // fitParameter: fit parameter ( 0-2 for pol1 ([0]+[1]*x+[2]*y),
    //                               0-5 for pol2 ([0]+[1]*x+[2]*y+[3]*x*x+[4]*y*y+[5]*x*y),
    //                               not used for mean time and mean Q )
    // for an example see class description at the beginning
    //

    Double_t *x = new Double_t[fNevents];
    Double_t *y = new Double_t[fNevents];

    TVectorD *xVar = 0x0;
    TObjArray *aType = 0x0;
    Int_t npoints=0;

    // sanity checks
    if ( (sector<0) || (sector>71) )      return 0x0;
    if ( (xVariable<0) || (xVariable>2) ) return 0x0;
    if ( (fitType<0) || (fitType>3) )     return 0x0;
    if ( fitType==0 ){
	if ( (fitParameter<0) || (fitParameter>2) ) return 0x0;
	aType = &fParamArrayEventPol1;
	if ( aType->At(sector)==0x0 ) return 0x0;
    }
    else if ( fitType==1 ){
	if ( (fitParameter<0) || (fitParameter>5) ) return 0x0;
	aType = &fParamArrayEventPol2;
	if ( aType->At(sector)==0x0 ) return 0x0;
    }


    if ( xVariable == 0 ) xVar = &fVEventTime;
    if ( xVariable == 1 ) xVar = &fVEventNumber;
    if ( xVariable == 2 ) {
	xVar = new TVectorD(fNevents);
	for ( Int_t i=0;i<fNevents; ++i) (*xVar)[i]=i;
    }

    for (Int_t ievent =0; ievent<fNevents; ++ievent){
	if ( fitType<2 ){
	    TObjArray *events = (TObjArray*)(aType->At(sector));
            if ( events->GetSize()<=ievent ) break;
	    TVectorD *v = (TVectorD*)(events->At(ievent));
	    if ( (v!=0x0) && ((*xVar)[ievent]>0) ) { x[npoints]=(*xVar)[ievent]; y[npoints]=(*v)[fitParameter]; npoints++;}
	} else if (fitType == 2) {
            Double_t xValue=(*xVar)[ievent];
	    Double_t yValue=(*GetTMeanEvents(sector))[ievent];
	    if ( yValue>0 && xValue>0 ) { x[npoints]=xValue; y[npoints]=yValue;npoints++;}
	}else if (fitType == 3) {
            Double_t xValue=(*xVar)[ievent];
            Double_t yValue=(*GetQMeanEvents(sector))[ievent];
	    if ( yValue>0 && xValue>0 ) { x[npoints]=xValue; y[npoints]=yValue;npoints++;}
	}
    }

    TGraph *gr = new TGraph(npoints);
    //sort xVariable increasing
    Int_t    *sortIndex = new Int_t[npoints];
    TMath::Sort(npoints,x,sortIndex);
    for (Int_t i=0;i<npoints;++i){
	gr->SetPoint(i,x[sortIndex[i]],y[sortIndex[i]]);
    }


    if ( xVariable == 2 ) delete xVar;
    delete x;
    delete y;
    delete sortIndex;
    return gr;
}
//_____________________________________________________________________
void AliTPCCalibCE::Analyse()
{
    //
    //  Calculate calibration constants
    //

    TVectorD paramQ(3);
    TVectorD paramT0(3);
    TVectorD paramRMS(3);
    TMatrixD dummy(3,3);

    for (Int_t iSec=0; iSec<72; ++iSec){
	TH2S *hT0 = GetHistoT0(iSec);
        if (!hT0 ) continue;

	AliTPCCalROC *rocQ   = GetCalRocQ  (iSec,kTRUE);
	AliTPCCalROC *rocT0  = GetCalRocT0 (iSec,kTRUE);
	AliTPCCalROC *rocRMS = GetCalRocRMS(iSec,kTRUE);
        AliTPCCalROC *rocOut = GetCalRocOutliers(iSec,kTRUE);

	TH2S *hQ   = GetHistoQ(iSec);
	TH2S *hRMS = GetHistoRMS(iSec);

	Short_t *array_hQ   = hQ->GetArray();
	Short_t *array_hT0  = hT0->GetArray();
	Short_t *array_hRMS = hRMS->GetArray();

        UInt_t nChannels = fROC->GetNChannels(iSec);

	//debug
	Int_t row=0;
	Int_t pad=0;
	Int_t padc=0;
	//! debug

	for (UInt_t iChannel=0; iChannel<nChannels; ++iChannel){


	    Float_t cogTime0 = -1000;
	    Float_t cogQ     = -1000;
	    Float_t cogRMS   = -1000;
            Float_t cogOut   = 0;


	    Int_t offsetQ = (fNbinsQ+2)*(iChannel+1)+1;
	    Int_t offsetT0 = (fNbinsT0+2)*(iChannel+1)+1;
	    Int_t offsetRMS = (fNbinsRMS+2)*(iChannel+1)+1;

	    cogQ     = AliMathBase::GetCOG(array_hQ+offsetQ,fNbinsQ,fXminQ,fXmaxQ);
	    cogTime0 = AliMathBase::GetCOG(array_hT0+offsetT0,fNbinsT0,fXminT0,fXmaxT0);
            cogRMS   = AliMathBase::GetCOG(array_hRMS+offsetRMS,fNbinsRMS,fXminRMS,fXmaxRMS);



	    /*
             //outlier specifications
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
		if ( !fDebugStreamer ) {
                        //debug stream
		    TDirectory *backup = gDirectory;
		    fDebugStreamer = new TTreeSRedirector("debugCalibCEAnalysis.root");
		    if ( backup ) backup->cd();  //we don't want to be cd'd to the debug streamer
		}

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
    if ( fDebugStreamer ) fDebugStreamer->GetFile()->Write();
//    delete fDebugStreamer;
//    fDebugStreamer = 0x0;
}
//_____________________________________________________________________
void AliTPCCalibCE::DumpToFile(const Char_t *filename, const Char_t *dir, Bool_t append)
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
