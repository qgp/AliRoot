#ifndef AliADBUFFER_H
#define AliADBUFFER_H
/* Copyright(c) 1998-2003, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/////////////////////////////////////////////////////////////////////
// Class used for storing AD digits according to the DDLs format//
/////////////////////////////////////////////////////////////////////

#ifdef __CINT__
class fstream;
#else
#include "Riostream.h"
#endif

#include "AliFstream.h"

class AliADBuffer:public TObject{

public:
  AliADBuffer();
  AliADBuffer(const char* fileName); //constructor
  virtual ~AliADBuffer(); //destructor
  
  void    WriteTriggerInfo(UInt_t trigger);
  void    WriteTriggerScalers();
  void    WriteBunchNumbers();  

  void    WriteChannel(Int_t channel, Short_t *adc, Bool_t integrator);
  void    WriteBeamFlags(Bool_t *bbFlag, Bool_t *bgFlag);
  
  void    WriteMBInfo();
  void    WriteMBFlags();
    
  void    WriteBeamScalers();

  void    WriteTiming(Float_t time, Float_t width);
  
  void	  WriteEmptyCIU();

private:
  AliADBuffer(const AliADBuffer &source); // copy constructor
  AliADBuffer& operator=(const AliADBuffer &source); // ass. op.

  UInt_t      fRemainingWord; // Remaining data word between even and odd channel's data
  AliFstream* f;      //The IO file name
  ClassDef(AliADBuffer,2)
};

#endif
