// $Id: AliHLTCalorimeterMapper.cxx 34622 2009-09-04 13:22:01Z odjuvsla $

/**************************************************************************
 * This file is property of and copyright by the Experimental Nuclear     *
 * Physics Group, Dep. of Physics                                         *
 * University of Oslo, Norway, 2006                                       *
 *                                                                        * 
 * Author: Per Thomas Hille perthi@fys.uio.no for the ALICE DCS Project.  *
 * Contributors are mentioned in the code where appropriate.              *
 * Please report bugs to perthi@fys.uio.no                                * 
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

//
// Mapping class fro mapping
// from hardware address to geometrical address
//
//

#include "AliHLTCaloMapper.h"
#include "AliHLTLogging.h"
#include "Rtypes.h"
#include "unistd.h"
#include <iostream>
#include "AliHLTCaloCoordinate.h"
#include "AliHLTCaloConstantsHandler.h"
#include "AliHLTCaloConstants.h"

ClassImp(AliHLTCaloMapper);

AliHLTCaloMapper::AliHLTCaloMapper( const unsigned long  specification , TString det) :  
  AliHLTCaloConstantsHandler(det),
  AliHLTLogging(), 
  fHw2geomapPtr(0),
  fCellSize(0),
  fSpecification(specification),
  fIsInitializedMapping(false),
  fSpecificationMapPtr(0)
{  
  //see header file for class documentation
}


AliHLTCaloMapper::~AliHLTCaloMapper()
{
  delete []  fHw2geomapPtr;
  fHw2geomapPtr = 0;
}


//void AliHLTCaloMapper::InitAltroMapping(){
  //Virtual base class
//}

//void AliHLTCaloMapper::InitDDLSpecificationMapping() {
  //Virtual base class
//}



bool 
AliHLTCaloMapper::GetIsInitializedMapping()
{
  return  fIsInitializedMapping;
}


char* 
AliHLTCaloMapper::GetFilePath()
{
  return  fFilepath;
}


int 
AliHLTCaloMapper::GetChannelID(const AliHLTUInt32_t spec, const Int_t hadd)
{
  Short_t index = GetDDLFromSpec(spec);
  if( index < 0 )
    {
      HLTError("Specification 0x%X not consistent with single DDL in PHOS", spec);
      return index;
    }
  else
    {
      return ((fHw2geomapPtr[hadd].fXCol   ) |
	      ((fHw2geomapPtr[hadd].fZRow  ) << 6) |
	      (fHw2geomapPtr[hadd].fGain << 12) |
	      fSpecificationMapPtr[index].fModId << 13);
    }
}



// UShort_t
// AliHLTCaloMapper::GetChannelID(Int_t specification, Int_t hwAddress)
// {
  
//   Short_t index = 0;

//   if(specification == 0x00001) index = 0;
//   else if(specification == 0x00002) index = 1;
//   else if(specification == 0x00004) index = 2;
//   else if(specification == 0x00008) index = 3;

//   else if(specification == 0x00010) index = 4;
//   else if(specification == 0x00020) index = 5;
//   else if(specification == 0x00040) index = 6;
//   else if(specification == 0x00080) index = 7;

//   else if(specification == 0x00100) index = 8;
//   else if(specification == 0x00200) index = 9;
//   else if(specification == 0x00400) index = 10;
//   else if(specification == 0x00800) index = 11;

//   else if(specification == 0x01000) index = 12;
//   else if(specification == 0x02000) index = 13;
//   else if(specification == 0x04000) index = 14;
//   else if(specification == 0x08000) index = 15;

//   else if(specification == 0x10000) index = 16;
//   else if(specification == 0x20000) index = 17;
//   else if(specification == 0x40000) index = 18;
//   else if(specification == 0x80000) index = 19;

//   else HLTError("Specification 0x%X not consistent with single DDL in PHOS", specification);
//   //  HLTError("Channel ID: 0x%X Coordinates: x = %d, z = %d, gain = %d", ((fHw2geomapPtr[hwAddress].fXCol + fSpecificationMapPtr[index].fRcuXOffset) |((fHw2geomapPtr[hwAddress].fZRow + fSpecificationMapPtr[index].fRcuZOffset) << 6) | (fHw2geomapPtr[hwAddress].fGain << 12) | fSpecificationMapPtr[index].fModId << 13),
//   //	   fHw2geomapPtr[hwAddress].fXCol,
//   //	   fHw2geomapPtr[hwAddress].fZRow, 
//   //	   fHw2geomapPtr[hwAddress].fGain);
//   return ((fHw2geomapPtr[hwAddress].fXCol + fSpecificationMapPtr[index].fRcuXOffset) |
// 	  ((fHw2geomapPtr[hwAddress].fZRow + fSpecificationMapPtr[index].fRcuZOffset) << 6) |
// 	  (fHw2geomapPtr[hwAddress].fGain << 12) |
// 	  fSpecificationMapPtr[index].fModId << 13);
// }


void
AliHLTCaloMapper::GetChannelCoord(const UShort_t channelId, UShort_t* channelCoord)
{
  channelCoord[0] = channelId&0x3f;
  channelCoord[1] = (channelId >> 6)&0x3f;
  channelCoord[2] = (channelId >> 12)&0x1;
  channelCoord[3] = (channelId >> 13)&0x1f;
  //  printf("Channel ID: 0x%X Coordinates: x = %d, z = %d, gain = %d\n", channelId, channelCoord[0], channelCoord[1], channelCoord[2]);
}

void
AliHLTCaloMapper::ChannelId2Coordinate(const int channelId,    AliHLTCaloCoordinate &channelCoord)
{
  channelCoord.fX = channelId&0x3f;
  channelCoord.fZ = (channelId >> 6)&0x3f;
  channelCoord.fGain = (channelId >> 12)&0x1;
  channelCoord.fModuleId  = (channelId >> 13)&0x1f;
  //  printf("Channel ID: 0x%X Coordinates: x = %d, z = %d, gain = %d\n", channelId, channelCoord[0], channelCoord[1], channelCoord[2]);
}


void
AliHLTCaloMapper::GetLocalCoord(const int channelId, Float_t* localCoord) const
{
  localCoord[0] = (static_cast<Float_t>(channelId&0x3f) - fCaloConstants->GetNXCOLUMNSMOD()/2)* fCaloConstants->GetCELLSTEP();
  localCoord[1] = (static_cast<Float_t>((channelId >> 6)&0x3f) - fCaloConstants->GetNZROWSMOD()/2) * fCaloConstants->GetCELLSTEP();
  //  printf("Local coordinates: x = %f, z = %f\n", channelCoord[0], channelCoord[1]);
}


int  
AliHLTCaloMapper::GetDDLFromSpec( const AliHLTUInt32_t spec )
{
  int tmpIndex = -1;
  for(int i=0; i < 32; i++ )
    {
      if (spec >> i ==1)
	{
	  tmpIndex = i;
	  break;
	}
    }
   if(  tmpIndex  < 0)
    {
      //   HLTError("Specification %d, not consistent with any DDL in PHOS or EMCAL", spec  );
    }

  return tmpIndex;
}


Int_t 
AliHLTCaloMapper::GetModuleFromSpec(Int_t specification)
{
  Int_t module = -1;
      
  if(specification & 0xf) module = 0;
  else if((specification >> 4) & 0xf) module = 1;
  else if((specification >> 8) & 0xf) module = 2;
  else if((specification >> 12) & 0xf) module = 3;
  else if((specification >> 16) & 0xf) module = 4;
 
  else HLTError("Specification 0x%X not consistent with single module in PHOS", specification);

  return module;
}


unsigned long 
AliHLTCaloMapper::GetSpecFromDDLIndex( const int ddlindex )
{
  int iret = (unsigned long)1  <<  ddlindex;  

  //  return  ((unsigned long)1)  <<  ddlindex ) ;

  return iret;

}

