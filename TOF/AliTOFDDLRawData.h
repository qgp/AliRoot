/* Copyright(c) 1998-2003, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////////////////////////////////
// Class used for generating the files containing raw data,               //
// required for  Data Challenge                                           //
////////////////////////////////////////////////////////////////////////////

#ifndef AliTOFDDLRAWDATA_H
#define AliTOFDDLRAWDATA_H

class AliTOF;
class TTree;

class AliTOFDDLRawData:public TObject{
 public:
  AliTOFDDLRawData();                               // default constructor
  virtual ~AliTOFDDLRawData(){;}                    // destructor
  AliTOFDDLRawData(const AliTOFDDLRawData &source); // copy constructor
  AliTOFDDLRawData& operator=(const AliTOFDDLRawData &source); // ass. op.
  Int_t RawDataTOF(TBranch* branch); 
  // This method generates the files with the TOF detector data
  void SetVerbose(Int_t Verbose){fVerbose=Verbose;}
  // To set the verbose level
 private:
  void  GetDigits(TClonesArray *TOFdigits, Int_t ddl,UInt_t *buf);
  //This method formats and stores in buf all the digits of a TOF module

  /*
  void  WriteChipHeader(Int_t ChipAddr,Int_t EventCnt,UInt_t &BaseWord);
  void  WriteChipTrailer(UInt_t *buf,Int_t ChipHitCount,UInt_t &BaseWord);
  //The three previous  methods are used to store the data according to the 
  //TOF detector data format

  void  ReadChipHeader(Int_t &ChipAddr,Int_t &EventCnt,UInt_t BaseWord);
  void  ReadChipTrailer(Int_t &ChipHitCount,UInt_t BaseWord);
  //Methods used for reading and dubugging TOF data files
  */

  Int_t fVerbose;            //Verbose level (0:no msg, 1:msg, 2:digits in txt files)
  Int_t fIndex;              //number of 32 words to be stored into the output file
  enum {kDDLOffset = 0x500};  //offset for DDL number
  ClassDef(AliTOFDDLRawData,1)
};
    
#endif
