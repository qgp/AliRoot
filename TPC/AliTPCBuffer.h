#ifndef AliTPCBUFFER_H
#define AliTPCBUFFER_H
/* Copyright(c) 1998-2003, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

///////////////////////////////////////////////////////////////////
// Class used for storing TPC digits according to the DDLs format//
//////////////////////////////////////////////////////////////////

#ifdef __CINT__
class fstream;
#else
#include "Riostream.h"
#endif

class AliSimDigits;
//class TTree;
//class TFile;
class AliTPCBuffer:public TObject{
public:
  AliTPCBuffer(){
    //default constructor
  }
  AliTPCBuffer(const char* fileName);//constructor
  virtual ~AliTPCBuffer();//destructor
  AliTPCBuffer(const AliTPCBuffer &source); // copy constructor
  AliTPCBuffer& operator=(const AliTPCBuffer &source); // ass. op.
  void    WriteRowBinary(Int_t eth,AliSimDigits *digrow,Int_t minPad,Int_t maxPad,Int_t flag,Int_t sec,Int_t SubSec,Int_t row);
  //  void    WriteRow(Int_t eth,AliSimDigits *digrow,Int_t minPad,Int_t maxPad,Int_t flag,Int_t sec,Int_t SubSec,Int_t row);
  ULong_t GetDigNumber()const{return fNumberOfDigits;}
  void    SetVerbose(Int_t val){fVerbose=val;}
private:
  Int_t fVerbose; //Verbosity level: 0-silent, not 0-all printout
  fstream f;      //The IO file name
  //TFile *fout;
  //TTree *tree;
  ULong_t fNumberOfDigits; //Number of TPC digits
  ClassDef(AliTPCBuffer,1)
};

#endif
