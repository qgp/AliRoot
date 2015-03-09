#ifndef ALIPHOSCPVRAWDIGIPRODUCER_H
#define ALIPHOSCPVRAWDIGIPRODUCER_H

// This class produces CPV digits from raw data stream of one event
// Raw data is supplied by the object AliRawReader,
// then AliPHOSCpvRawDigiProducer decodes CPV data and converts is to AliPHOSDigits.
//!
// Author: Mikhail Stolpovskiy, mikhail.stolpovskiy@cern.ch
// modified by Sergey.Evdokimov@cern.ch 30 July 2014

#include "TObject.h"
#include "TH1I.h"

#include "AliAltroMapping.h"
#include "AliRawReaderDate.h"
#include "AliPHOSCpvParam.h"

class AliPHOSCpvRawStream;
class AliPHOSDigit ;
class AliPHOSGeometry ;

class AliPHOSCpvRawDigiProducer: public TObject {

public:

  AliPHOSCpvRawDigiProducer() ;
  AliPHOSCpvRawDigiProducer(AliRawReader * rawReader); // creates an AliPHOSCpvRawStream(rawReader) object to read data
 
  virtual ~AliPHOSCpvRawDigiProducer(); 

  Bool_t LoadPedFiles(); //returns true if ok. Must be called when you process a phys run
                         //If ped files are loaded, then MakeDigits returns digits with 
                         //substruct pedestals from ADCs

  Bool_t LoadNewEvent(AliRawReader * rawReader); // returns true, if ok
  void   SetTurbo(Bool_t turbo);                 // if turbo==true then do read without error checking
  Bool_t GetTurbo() const {return fTurbo;}

  void   MakeDigits(TClonesArray * digits) const;   // digits is an array of AliPHOSCpvPHOSDigit objects
  TH1I * GetErrorsHist() const { return fhErrors; } // takes histogram of errors from AliPHOSCpvRawStream

  void   SetCpvMinAmp(Int_t cpvMin) { fCpvMinE=cpvMin; } // thresholds would be ped + fCpvMinE

protected:
  void CreateErrHist();             // initialize histogram of errors
private:
  AliPHOSGeometry * fGeom ;         //! PHOS geometry
  Bool_t fTurbo;                    // if true, then read without error checking
  Int_t  fCpvMinE ;                 // minimum energy of digit (ADC)
  AliPHOSCpvRawStream * fRawStream; //! Raw data stream 

  TH1I * fhErrors;         // ! histogram of errors

  Int_t ** fPed[2][2*AliPHOSCpvParam::kNDDL]; // pedestals    ped[0][iddl][x][y] = pedestal; ped[1][iddl][x][y] = N*sigma (N was used while creating ped files)
  Bool_t fPedFilesRLoaded;

  ClassDef(AliPHOSCpvRawDigiProducer,2);
};

#endif
