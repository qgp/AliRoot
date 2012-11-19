#ifndef ALIPMDPATCHBUSHEADER_H
#define ALIPMDPATCHBUSHEADER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// Author - Basanta K. Nandi

#include <TObject.h>

class AliPMDPatchBusHeader : public TObject {

public:
   AliPMDPatchBusHeader();
   AliPMDPatchBusHeader(const AliPMDPatchBusHeader &pbush);
   AliPMDPatchBusHeader& operator=(const AliPMDPatchBusHeader &pbush);

   virtual ~AliPMDPatchBusHeader();

   // PatchBus header
   void  SetDataKey(Int_t dkey)            {fDataKey = dkey;}
   void  SetTotalLength(Int_t totlength)   {fTotalLength = totlength;}
   void  SetRawDataLength(Int_t rawlength) {fRawDataLength = rawlength;}
   void  SetPatchBusId(Int_t pbusid)       {fPatchBusId = pbusid;}

   void  SetHeader(Int_t *header);


   Int_t GetHeaderLength()  const {return fgkHeaderLength;}
   Int_t GetDataKey()       const {return fDataKey;}  
   Int_t GetTotalLength()   const {return fTotalLength;}
   Int_t GetRawDataLength() const {return fRawDataLength;}
   Int_t GetPatchBusId()    const {return fPatchBusId;}



 private:

   Int_t     fDataKey;        // data key
   Int_t     fTotalLength;    // total length of block structure
   Int_t     fRawDataLength;  // length of raw data
   Int_t     fPatchBusId;     // Patch bus id


   static const Int_t fgkHeaderLength;   // header length in word

   ClassDef(AliPMDPatchBusHeader,1)  // PMD PatchBus Header
};
#endif
