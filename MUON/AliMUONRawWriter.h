#ifndef ALIMUONRAWWRITER_H
#define ALIMUONRAWWRITER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*$Id$*/

/// \ingroup rec
/// \class AliMUONRawWriter
/// \brief Raw data class for trigger and tracker chambers
///
/// Writring Raw data class for trigger and tracker chambers

#include <TObject.h>
#include <TClonesArray.h>
#include "AliMUONBusStruct.h"
#include "AliRawDataHeader.h"
#include "TStopwatch.h"

class AliMUONData;
class AliMUONDigit;
class AliMUONDspHeader;
class AliMUONBlockHeader;
class AliMUONDarcHeader;
class AliMUONRegHeader;
class AliMUONLocalStruct;
class AliMUONGlobalTrigger;
class AliMpBusPatch;
class AliMpSegFactory;

class AliMUONRawWriter : public TObject 
{
 public:
  AliMUONRawWriter(AliMUONData* data); // Constructor
  virtual ~AliMUONRawWriter(); // Destructor
    
  // write raw data
  Int_t Digits2Raw();

  void  SetScalerEvent() {fScalerEvent = kTRUE;}
  
protected:
  AliMUONRawWriter();                  // Default constructor
  AliMUONRawWriter (const AliMUONRawWriter& rhs); // copy constructor
  AliMUONRawWriter& operator=(const AliMUONRawWriter& rhs); // assignment operator

  // writing raw data
  Int_t WriteTrackerDDL(Int_t iCh);
  Int_t WriteTriggerDDL();
  
private:

  void AddData(const AliMUONBusStruct& event)
  {
    TClonesArray &temp = *fBusArray;
    new(temp[temp.GetEntriesFast()]) AliMUONBusStruct(event); 
  }

  Int_t GetBusPatch(const AliMUONDigit& digit);
  
  Int_t GetGlobalTriggerPattern(const AliMUONGlobalTrigger* gloTrg) const;

private:

  AliMUONData*  fMUONData;           //!< Data container for MUON subsystem 
 
  FILE*         fFile[2];            //!< DDL binary file pointer one per 1/2 chamber

  TClonesArray* fBusArray;           //!< array to sub event tracker
   
  AliMUONBlockHeader* fBlockHeader;  //!< DDL block header class pointers
  AliMUONDspHeader*   fDspHeader;    //!< DDL Dsp header class pointers
  AliMUONBusStruct*   fBusStruct;    //!< DDL bus patch structure class pointers
  AliMUONDarcHeader*  fDarcHeader;   //!< DDL darc header class pointers
  AliMUONRegHeader*   fRegHeader;    //!< DDL regional header class pointers
  AliMUONLocalStruct* fLocalStruct;  //!< DDL local structure class pointers

  AliMpBusPatch* fBusPatchManager;   //!< buspatch versus DE's & DDL

  Bool_t fScalerEvent;               ///< flag to generates scaler event

  AliRawDataHeader    fHeader;           ///< header of DDL

  static Int_t fgManuPerBusSwp1B[12];   //!< array containing the first manuId for each buspatch st1, Bending
  static Int_t fgManuPerBusSwp1NB[12];  //!< array containing the first manuId for each buspatch st1, NBending

  static Int_t fgManuPerBusSwp2B[12];   //!< array containing the first manuId for each buspatch st2, Bending
  static Int_t fgManuPerBusSwp2NB[12];  //!< array containing the first manuId for each buspatch st2, NBending
  
  TStopwatch fTrackerTimer;             //!< 
  TStopwatch fTriggerTimer;             //!< 
  TStopwatch fMappingTimer;             //!< 
  
  AliMpSegFactory* fSegFactory;         //!< mapping segmentation factory
  
  ClassDef(AliMUONRawWriter,1) // MUON cluster reconstructor in ALICE
};
	
#endif
