/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
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


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TPC calibration class for parameters which saved per pad                 //
//  Authors: Marian Ivanov and Haavard Helstrup                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliTPCSensorTempArray.h"

ClassImp(AliTPCSensorTempArray)


//_____________________________________________________________________________
AliTPCSensorTempArray::AliTPCSensorTempArray():AliDCSSensorArray()
{
  //
  // AliTPCSensorTempArray default constructor
  //
 
}
//_____________________________________________________________________________
AliTPCSensorTempArray::AliTPCSensorTempArray(Int_t run) : AliDCSSensorArray() 
{
  //
  // Read configuration from OCDB
  //

     
  AliCDBEntry *entry =
            AliCDBManager::Instance()->Get("TPC/Config/Temperature",run); 
  TTree *tree = (TTree*) entry->GetObject();
  fSensors = AliTPCSensorTemp::ReadTree(tree);
  fSensors->BypassStreamer(kFALSE);
  
}
//_____________________________________________________________________________
AliTPCSensorTempArray::AliTPCSensorTempArray(UInt_t startTime, UInt_t endTime,
                       TTree* confTree)
             :AliDCSSensorArray()
{
  //
  // AliTPCSensorTempArray constructor for Shuttle preprocessor 
  //  (confTree read from OCDB)
  //
  fSensors = AliTPCSensorTemp::ReadTree(confTree);
  fSensors->BypassStreamer(kFALSE);
  fStartTime = TTimeStamp(startTime);
  fEndTime   = TTimeStamp(endTime);
}

//_____________________________________________________________________________
AliTPCSensorTempArray::AliTPCSensorTempArray(const char *fname) : 
                                                  AliDCSSensorArray()
{
  //
  // AliTPCSensorTempArray constructor
  //
  fSensors = AliTPCSensorTemp::ReadList(fname);
  fSensors->BypassStreamer(kFALSE);
}


//_____________________________________________________________________________
AliTPCSensorTempArray::AliTPCSensorTempArray(const AliTPCSensorTempArray &c):
  AliDCSSensorArray(c)
{
  //
  // AliTPCSensorTempArray copy constructor
  //

}

///_____________________________________________________________________________
AliTPCSensorTempArray::~AliTPCSensorTempArray()
{
  //
  // AliTPCSensorTempArray destructor
  //
}

//_____________________________________________________________________________
AliTPCSensorTempArray &AliTPCSensorTempArray::operator=(const AliTPCSensorTempArray &c)
{
  //
  // Assignment operator
  //

  if (this != &c) ((AliTPCSensorTempArray &) c).Copy(*this);
  return *this;

}

//_____________________________________________________________________________
void AliTPCSensorTempArray::Copy(TObject &c) const
{
  //
  // Copy function
  //

  TObject::Copy(c);
}
//_____________________________________________________________________________
void AliTPCSensorTempArray::ReadSensors(const char *dbEntry) 
{
  //
  // Read list of temperature sensors from text file
  //
  AliCDBEntry *entry = AliCDBManager::Instance()->Get(dbEntry); 
  TTree *tree = (TTree*) entry->GetObject();
  fSensors = AliTPCSensorTemp::ReadTree(tree);

}  

//_____________________________________________________________________________
AliTPCSensorTemp* AliTPCSensorTempArray::GetSensor(Int_t type, Int_t side, Int_t sector, Int_t num) 
{
 //
 //  Return sensor information for sensor specified by type, side, sector and num
 //
 Int_t nsensors = fSensors->GetEntries();
 for (Int_t isensor=0; isensor<nsensors; isensor++) {
   AliTPCSensorTemp *entry = (AliTPCSensorTemp*)fSensors->At(isensor);
   if (entry->GetSide() == side &&
       entry->GetType() == type &&
       entry->GetSector() == sector &&
       entry->GetNum() == num ) return entry;
 }
 return 0;
}
//_____________________________________________________________________________

AliTPCSensorTemp* AliTPCSensorTempArray::GetSensor(Int_t IdDCS){
  return dynamic_cast<AliTPCSensorTemp*>(AliDCSSensorArray::GetSensor(IdDCS));
}
//_____________________________________________________________________________

AliTPCSensorTemp* AliTPCSensorTempArray::GetSensor(Double_t x, Double_t y, Double_t z){
  return dynamic_cast<AliTPCSensorTemp*>(AliDCSSensorArray::GetSensor(x,y,z));
}
