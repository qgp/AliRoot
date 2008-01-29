#ifndef AliMUONQADataMakerRec_H
#define AliMUONQADataMakerRec_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/// \ingroup rec
/// \class AliMUONQADataMakerRec
/// \brief MUON Quality assurance data maker
///
//  Author Christian Finck

// dummy function for simulation part
// to avoid circular dependencie

// --- ROOT system ---
class TObjArray; 

// --- AliRoot header files ---
class AliMUONVTrackStore;
class AliMUONDigitMaker;
class AliMUONVDigitStore;
class AliMUONVTriggerStore;

#include "AliQADataMakerRec.h"

class AliMUONQADataMakerRec: public AliQADataMakerRec {

public:
  AliMUONQADataMakerRec();         
  AliMUONQADataMakerRec(const AliMUONQADataMakerRec& qadm);   
  AliMUONQADataMakerRec& operator=(const AliMUONQADataMakerRec& qadm);
  virtual ~AliMUONQADataMakerRec();
  
private:
  virtual void   StartOfDetectorCycle(); 

  virtual void   InitRaws(); 
  virtual void   InitRecPoints(); 
  virtual void   InitESDs(); 
  
  virtual void   MakeRaws(AliRawReader* rawReader); 
  virtual void   MakeRecPoints(TTree* recpo); 
  virtual void   MakeESDs(AliESDEvent* esd) ;
  virtual void   EndOfDetectorCycle(AliQA::TASKINDEX task, TObjArray* list);

  AliMUONVDigitStore* fDigitStore; //!< pointer to digits store
  AliMUONVTriggerStore* fTriggerStore; //!< pointer to trigger store
  AliMUONDigitMaker* fDigitMaker;  //!< pointer to digit maker

  ClassDef(AliMUONQADataMakerRec,2)  // MUON Quality assurance data maker

};
#endif
