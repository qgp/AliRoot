#ifndef ALITOFRAWSTREAM_H
#define ALITOFRAWSTREAM_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////
//                                                           //
//   This class provides the key-reading for TOF raw data.   //
//                                                           //
///////////////////////////////////////////////////////////////

#include "TObject.h"

class TClonesArray;

class AliRawReader;
class AliTOFGeometry;
class AliTOFrawData;

class AliTOFRawStream: public TObject {
 public:

  AliTOFRawStream(); // default ctr
  AliTOFRawStream(AliRawReader* rawReader); // ctr
  virtual ~AliTOFRawStream(); // default dtr

  virtual Bool_t Next();
  
  virtual void   LoadRawData(Int_t indexDDL);

  Int_t GetDDL()        const {return fDDL;};
  Int_t GetTRM()        const {return fTRM;};
  Int_t GetTDC()        const {return fTDC;};
  Int_t GetTRMchain()   const {return fTRMchain;};
  Int_t GetTDCchannel() const {return fTDCchannel;};
  
  Int_t GetSector() const {return fSector;};
  Int_t GetPlate()  const {return fPlate;};
  Int_t GetStrip()  const {return fStrip;};
  Int_t GetPadZ()   const {return fPadZ;};
  Int_t GetPadX()   const {return fPadX;};
  Int_t GetIndex(Int_t *detId); // Get channel index for Online Calibration 
  
  Float_t GetTofBin() const {return fTime;};
  Float_t GetToTbin() const {return fToT;};
  Float_t GetLeadingEdge() const {return fLeadingEdge;};
  Float_t GetTrailingEdge() const {return fTrailingEdge;};

  Int_t GetPSbit()  const {return fPSbit;};
  Int_t GetACQ()    const {return fACQ;};
    
  Int_t GetErrorFlag()  const {return fErrorFlag;};

  void SetDDL(Int_t nDDL)            {fDDL = nDDL;};
  void SetTRM(Int_t nTRM)            {fTRM = nTRM;};
  void SetTDC(Int_t nTDC)            {fTDC = nTDC;};
  void SetTRMchain(Int_t nChain)     {fTRMchain = nChain;};
  void SetTDCchannel(Int_t nChannel) {fTDCchannel = nChannel;};

  TClonesArray *GetRawData() const {return fTOFrawData;};

  void SetSector();
  void SetPlate();
  void SetStrip();
  void SetPadZ();
  void SetPadX();
  
  void  EquipmentId2VolumeId(Int_t nDDL, Int_t nTRM, Int_t iChain,
			     Int_t iTDC, Int_t iCH, Int_t *volume) const;
  Int_t Equip2VolNplate(Int_t iDDL, Int_t nTRM, Int_t nTDC) const ;
  Int_t Equip2VolNstrip(Int_t iDDL, Int_t nTRM, Int_t nTDC) const ;
  Int_t Equip2VolNpad(Int_t iDDL, Int_t iChain, Int_t nTDC, Int_t iCH) const ;
  Int_t GetDDLnumberPerSector(Int_t nDDL) const;
  Int_t GetSectorNumber(Int_t nDDL) const;

 private:

  Int_t GetField(UInt_t word, Int_t fieldMask, Int_t fieldPosition) const;

  AliTOFRawStream(const AliTOFRawStream& stream); // copy ctr
  AliTOFRawStream& operator = (const AliTOFRawStream& stream); // ass. op.

  AliRawReader*  fRawReader; // object for reading the raw data

  TClonesArray *fTOFrawData; // pointer to AliTOFrawData TClonesArray

  Int_t         fDDL;          // DDL file number [0;71]
  Int_t         fTRM;          // TRM number [1;12]
  Int_t         fTRMchain;     // TRM chain number [0;1]
  Int_t         fTDC;          // TDC number [0;14]
  Int_t         fTDCchannel;   // TDC channel number [0;7]
  Float_t       fTime;         // time-of-flight measurement [0;8191]
  Float_t       fToT;          // time-over-threshould measurement [0;255]
  Float_t       fLeadingEdge;  // leading edge measurement
  Float_t       fTrailingEdge; // trailing edge measurement
  Int_t         fErrorFlag;    // error flag
  
  Int_t         fSector;     // sector number [0;17]
  Int_t         fPlate;      // plate number [0;4]
  Int_t         fStrip;      // strip number [0;14/18]
  Int_t         fPadX;       // pad number along the strip [0;47]
  Int_t         fPadZ;       // pad-row number [0;1]

  AliTOFGeometry *fTOFGeometry; // pointer to the TOF geometry

  Int_t fPackedDigits;       // counter for packed digits

  Int_t fWordType;           // word type
  Int_t fSlotID;             // crate slot ID number
  Int_t fACQ;                // flag to identify the aquisition kind
  Int_t fPSbit;              // flag for packing 
  Int_t fTDCerrorFlag;       // TDC error flag
  Bool_t fInsideDRM;         // inside/outside DRM
  Bool_t fInsideTRM;         // inside/outside TRM
  Bool_t fInsideLTM;         // inside/outside LTM
  Bool_t fInsideTRMchain0;   // inside/outside chain 0
  Bool_t fInsideTRMchain1;   // inside/outside chain 1

  ClassDef(AliTOFRawStream, 1)  // class for reading TOF raw digits
};

#endif
