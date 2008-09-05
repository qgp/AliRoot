/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
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

/* $Id $*/

///////////////////////////////////////////////////////////////////
//                                                               //
// Class to decode compressed SDD Raw Data format                //
// The 32 bits for a data word are defined as follows:           //
//   31 control bit (0=data word, 1= control word)               //
//   30 -                                                        //
//   29  |                                                       //
//   28  |-> 4 bits to identify the Carlos (0-11) inside the DDL //
//   27 -                                                        //
//   26 detecot side (0= left, =right)                           //
//   25 -                                                        //
//   24  |                                                       //
//   23  |                                                       //
//   22  |                                                       //
//   21  |-> 8 bits to identify the anode number (0-255)         //
//   20  |                                                       //
//   19  |                                                       //
//   18 -                                                        //
//   17 -                                                        //
//   16  |                                                       //
//   15  |                                                       //
//   14  |                                                       //
//   13  |-> 8 bits to identify the time bin (0-255)             //
//   12  |                                                       //
//   11  |                                                       //
//   10 -                                                        //
//    9 -                                                        //
//    8  |                                                       //
//    7  |                                                       //
//    6  |                                                       //
//    5  |                                                       //
//    4  |-> 10 bit for the ADC counts                           //
//    3  |                                                       //
//    2  |                                                       //
//    1  |                                                       //
//    0 -                                                        //
//                                                               //
// Plus 2 types of control words:                                //
// - DDL identifier with the 4 more significant bits     = 1000  //
// - End of module data (needed by the Cluster Finder)   = 1111  //
//                                                               //
// Origin: F.Prino, Torino, prino@to.infn.it                     //
//                                                               //
///////////////////////////////////////////////////////////////////



#include "AliITSRawStreamSDDCompressed.h"
#include "AliRawReader.h"
#include "AliLog.h"

ClassImp(AliITSRawStreamSDDCompressed)
  


//______________________________________________________________________
AliITSRawStreamSDDCompressed::AliITSRawStreamSDDCompressed(AliRawReader* rawReader) :
  AliITSRawStream(rawReader),
fDDLModuleMap(0),
fData(0),
fCarlosId(-1),
fChannel(0),
fJitter(0),
fDDL(0)
{
// create an object to read ITS SDD raw digits
  fDDLModuleMap=new AliITSDDLModuleMapSDD();
  fDDLModuleMap->SetDefaultMap();
  for(Int_t im=0;im<kSDDModules;im++){
    fLowThresholdArray[im][0]=0;
    fLowThresholdArray[im][1]=0;
  }
  fRawReader->Reset();
  fRawReader->Select("ITSSDD");


}

//______________________________________________________________________
AliITSRawStreamSDDCompressed::AliITSRawStreamSDDCompressed(const AliITSRawStreamSDDCompressed& rs) :
AliITSRawStream(rs.fRawReader),
fDDLModuleMap(rs.fDDLModuleMap),
fData(0),
fCarlosId(-1),
fChannel(0),
fJitter(0),
fDDL(0)
{
  // copy constructor
  AliError("Copy constructor should not be used.");
}
//__________________________________________________________________________
AliITSRawStreamSDDCompressed& AliITSRawStreamSDDCompressed::operator=(const AliITSRawStreamSDDCompressed& rs) {
  // assignment operator
  if (this!=&rs) {}
  AliError("Assignment opertator should not be used.");
  return *this;
}

//______________________________________________________________________
AliITSRawStreamSDDCompressed::~AliITSRawStreamSDDCompressed(){
  if(fDDLModuleMap) delete fDDLModuleMap;
}


//______________________________________________________________________
Bool_t AliITSRawStreamSDDCompressed::Next()
{
// read the next raw digit
// returns kFALSE if there is no digit left
// returns kTRUE and fCompletedModule=kFALSE when a digit is found
// returns kTRUE and fCompletedModule=kTRUE  when a module is completed 


//  UInt_t masksod=8;    // start of DDL has the 4 most significant bits = 1000
  UInt_t maskeom=15;   // end of module has the 4 most significant bits = 1111
  UInt_t maskmod=15;   // last 4 bits for module number in end of module word
  //  UInt_t maskDDL=0xFF; // last 8 bits for DDL number in start of DDL word
    
  UInt_t maskCarlos=15<<27; // 4 bits  (27-30) for CarlosId in data word
  UInt_t maskSide=1<<26;    // 1 bit   (26)    for side     in data word
  UInt_t maskAnode=255<<18; // 8 bits  (18-25) for Nanode   in data word
  UInt_t maskTb=255<<10;    // 8 bits  (10-27) for Ntimebin in data word
  UInt_t maskADC=1023;      // 10 bits (0-9)   for ADC      in data word
    
  while(kTRUE){
    if (!fRawReader->ReadNextInt(fData)) return kFALSE;  // read next word
    UInt_t mostsigbits=fData>>28; 
    if(mostsigbits==maskeom){
      fCarlosId=fData&maskmod;
      fDDL=fRawReader->GetDDLID();
      fModuleID = GetModuleNumber(fDDL,fCarlosId);
      fCompletedModule=kTRUE;
      return kTRUE;
    }else{
      fCarlosId=(fData&maskCarlos)>>27;
      fDDL=fRawReader->GetDDLID();
      fModuleID = GetModuleNumber(fDDL,fCarlosId);
      fChannel=(fData&maskSide)>>26;
      fCoord1=(fData&maskAnode)>>18;
      fCoord2=(fData&maskTb)>>10;
      fSignal=fData&maskADC;
      fSignal+=fLowThresholdArray[fModuleID-kSPDModules][fChannel];
      fCompletedModule=kFALSE;
      return kTRUE;
    }
  }
  return kFALSE;
}


