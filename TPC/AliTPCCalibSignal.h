#ifndef ALITPCCALIBSIGNAL_H
#define ALITPCCALIBSIGNAL_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

class TObjArray;
class TH2S;
class TTreeSRedirector;
class AliTPCCalPad;
class AliTPCROC;
class AliTPCParam;
class AliRawReader;


class AliTPCCalibSignal : public TObject {

public:
    AliTPCCalibSignal();
    AliTPCCalibSignal(const AliTPCCalibSignal &sig);
    virtual ~AliTPCCalibSignal();

    AliTPCCalibSignal& operator = (const  AliTPCCalibSignal &source);


    Bool_t ProcessEvent(AliRawReader *rawReader);  //center of gravity approach event by event

    Int_t Update(const Int_t isector, const Int_t iRow, const Int_t iPad,
	       const Int_t iTimeBin, const Float_t signal);
    void Analyse();
    //
    AliTPCCalROC* GetCalRocT0 (Int_t sector, Bool_t force=kFALSE);  // get calibration object - sector
    AliTPCCalROC* GetCalRocQ  (Int_t sector, Bool_t force=kFALSE);  // get calibration object - sector
    AliTPCCalROC* GetCalRocRMS(Int_t sector, Bool_t force=kFALSE);  // get calibration object - sector
    AliTPCCalROC* GetCalRocOutliers(Int_t sector, Bool_t force=kFALSE);  // get calibration object - sector

    const TObjArray* GetCalPadT0() { return &fCalRocArrayT0; }      // get calibration object
    const TObjArray* GetCalPadQ()  { return &fCalRocArrayQ;  }      // get calibration object
    const TObjArray* GetCalPadRMS(){ return &fCalRocArrayRMS;}      // get calibration object
    const TObjArray* GetCalPadOutliers(){ return &fCalRocArrayOutliers;}      // get calibration object

    TH2S* GetHistoQ  (Int_t sector, Bool_t force=kFALSE);           // get refernce histogram
    TH2S* GetHistoT0 (Int_t sector, Bool_t force=kFALSE);           // get refernce histogram
    TH2S* GetHistoRMS(Int_t sector, Bool_t force=kFALSE);           // get refernce histogram

    Short_t GetDebugLevel()     const { return fDebugLevel;    }
    //
    void  SetRangeTime (Int_t firstTimeBin, Int_t lastTimeBin) { fFirstTimeBin=firstTimeBin;   fLastTimeBin=lastTimeBin;  } //Set range in which the pulser signal is expected
    void  SetRangeTime0(Int_t firstTimeBin, Int_t lastTimeBin) { fFirstTimeBinT0=firstTimeBin; fLastTimeBinT0=lastTimeBin;} //Set range for analysis after T0 substraction. Should be smaller than the above and around 0
    void  SetDebugLevel(Short_t debug=1){ fDebugLevel = debug;}

    Int_t GetFirstTimeBin()   const { return fFirstTimeBin;  }
    Int_t GetLastTimeBin()    const { return fLastTimeBin;   }
    Int_t GetFirstTimeBinT0() const { return fFirstTimeBinT0;}
    Int_t GetLastTimeBinT0()  const { return fLastTimeBinT0; }

    void DumpToFile(const Char_t *filename, const Char_t *dir="", Bool_t append=kFALSE);

private:
    Int_t fFirstTimeBin;              //  First Time bin needed for analysis
    Int_t fLastTimeBin;               //  Last Time bin needed for analysis
    Int_t fFirstTimeBinT0;            //  First Time bin after T0 correction
    Int_t fLastTimeBinT0;             //  Last Time bin after T0 correction

    Int_t     fLastSector;            //! Last sector processed

    AliTPCROC   *fROC;                //! ROC information
    AliTPCParam *fParam;              //! TPC information

    AliTPCCalPad *fPedestalTPC;       //! Pedestal Information
    Bool_t fBpedestal;                //! are we running with pedestal substraction


    TObjArray fCalRocArrayT0;         //  Array of AliTPCCalROC class for Time0 calibration
    TObjArray fCalRocArrayQ;          //  Array of AliTPCCalROC class for Charge calibration
    TObjArray fCalRocArrayRMS;        //  Array of AliTPCCalROC class for signal width calibration
    TObjArray fCalRocArrayOutliers;  //  Array of AliTPCCalROC class for signal outliers

    TObjArray fHistoQArray;           //  Calibration histograms for Charge distribution
    TObjArray fHistoT0Array;          //  Calibration histograms for Time0  distribution
    TObjArray fHistoRMSArray;         //  Calibration histograms for signal width distribution

    TObjArray fPadTimesArrayEvent;    //! Pad Times for the event, before mean Time0 corrections
    TObjArray fPadQArrayEvent;        //! Charge for the event, only needed for debugging streamer
    TObjArray fPadRMSArrayEvent;      //! Signal width for the event, only needed for debugging streamer
    TObjArray fPadPedestalArrayEvent; //! Signal width for the event, only needed for debugging streamer

    Int_t     fCurrentChannel;         //! current channel processed
    Int_t     fCurrentSector;          //! current sector processed
    Int_t     fCurrentRow;             //! current row processed
    Float_t   fMaxPadSignal;           //! maximum bin of current pad
    Int_t     fMaxTimeBin;             //! time bin with maximum value
    TVectorF  fPadSignal;              //! signal of current Pad

    TVectorF  fVTime0Offset1;          //!  Time0 Offset from preprocessing for each sector;
    TVectorF  fVTime0Offset1Counter;   //!  Time0 Offset from preprocessing for each sector;

    //debugging
    Int_t fEvent;
    TTreeSRedirector *fDebugStreamer;  //! debug streamer

    Short_t fDebugLevel;
    //! debugging

    TH2S* GetHisto(Int_t sector, TObjArray *arr,
		   Int_t nbinsY, Float_t ymin, Float_t ymax,
		   Char_t *type, Bool_t force);


    AliTPCCalROC* GetCalRoc(Int_t sector, TObjArray* arr, Bool_t force);

    TVectorF* GetPadTimesEvent(Int_t sector, Bool_t force=kFALSE);

    void ResetEvent();
    void ResetPad();
    void ProcessPad();
    void EndEvent();


    //debug
    TVectorF* GetPadInfoEvent(Int_t sector, TObjArray *arr, Bool_t force=kFALSE);
    TVectorF* GetPadQEvent(Int_t sector, Bool_t force=kFALSE);
    TVectorF* GetPadRMSEvent(Int_t sector, Bool_t force=kFALSE);
    TVectorF* GetPadPedestalEvent(Int_t sector, Bool_t force=kFALSE);


public:


  ClassDef(AliTPCCalibSignal,1)
};



#endif

