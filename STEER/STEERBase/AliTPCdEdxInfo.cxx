#include "AliTPCdEdxInfo.h"

//##################################################################
//
// Simple class to store TPC dE/dx info for different pad regions.
//
// Origin: Marian Ivanov, Alexander Kalweit 
//
//##################################################################



ClassImp(AliTPCdEdxInfo)

AliTPCdEdxInfo::AliTPCdEdxInfo():
  TObject(),
  fTPCsignalRegion(),
  fTPCsignalRegionQmax(),
  fTPCsignalNRegion(),
  fTPCsignalNRowRegion()
{
  // Default constructor
  for (Int_t i=0;i<3; i++){
    fTPCsignalRegion[i]=0;
    fTPCsignalRegionQmax[i]=0;
    fTPCsignalNRegion[i]=0;
    fTPCsignalNRowRegion[i]=0;
  }
  fTPCsignalRegion[3]=0;
  fTPCsignalRegionQmax[3]=0;
  
}

//_______________________________________________________________________________________________
AliTPCdEdxInfo::AliTPCdEdxInfo(const AliTPCdEdxInfo& source):
    TObject(),
    fTPCsignalRegion(),
    fTPCsignalRegionQmax(),
    fTPCsignalNRegion(),
    fTPCsignalNRowRegion()
{
    //
    // copy constructor
    //
    for (Int_t i=0;i<3; i++){
      fTPCsignalRegion[i]     = source.fTPCsignalRegion[i];
      fTPCsignalRegionQmax[i] = source.fTPCsignalRegionQmax[i];
      fTPCsignalNRegion[i]    = source.fTPCsignalNRegion[i];
      fTPCsignalNRowRegion[i] = source.fTPCsignalNRowRegion[i];
    }
    fTPCsignalRegion[3]       = source.fTPCsignalRegion[3];
    fTPCsignalRegionQmax[3]   = source.fTPCsignalRegionQmax[3];
    
}

//_______________________________________________________________________________________________
AliTPCdEdxInfo& AliTPCdEdxInfo::operator=(const AliTPCdEdxInfo& source)
{
    //
    // assignment operator
    //

  if (&source == this) return *this;
  TObject::operator=(source);

  for (Int_t i=0;i<3; i++){
    fTPCsignalRegion[i]     = source.fTPCsignalRegion[i];
    fTPCsignalRegionQmax[i] = source.fTPCsignalRegionQmax[i];
    fTPCsignalNRegion[i]    = source.fTPCsignalNRegion[i];
    fTPCsignalNRowRegion[i] = source.fTPCsignalNRowRegion[i];
  }
  fTPCsignalRegion[3]       = source.fTPCsignalRegion[3];
  fTPCsignalRegionQmax[3]   = source.fTPCsignalRegionQmax[3];
  
  return *this;

}

//_______________________________________________________________________________________________
void  AliTPCdEdxInfo::GetTPCSignalRegionInfo(Double_t signal[4], Char_t ncl[3], Char_t nrows[3]) const {
  //
  // Get the TPC dEdx variables per region
  //
  // Double32_t  fTPCsignalRegion[4]; // TPC dEdx signal in 4 different regions - 0 - IROC, 1- OROC medium, 2 - OROC long, 3- OROC all, (default truncation used)  
  // Char_t      fTPCsignalNRegion[3]; // number of clusters above threshold used in the dEdx calculation
  // Char_t      fTPCsignalNRowRegion[3]; // number of crosed rows used in the dEdx calculation - signal below threshold included
  //
  for (Int_t i=0; i<3; i++){
    signal[i]=fTPCsignalRegion[i];
    ncl[i]=fTPCsignalNRegion[i];
    nrows[i]=fTPCsignalNRowRegion[i];
  }
  signal[3]=fTPCsignalRegion[3];
  return; 
}

//_______________________________________________________________________________________________
void  AliTPCdEdxInfo::GetTPCSignals(Double_t signal[4]) const {
  //
  // Set the TPC dEdx variables per region
  //
  // Double32_t  fTPCsignalRegionQmax[4]; // TPC dEdx signal in 4 different regions - 0 - IROC, 1- OROC medium, 2 - OROC long, 3- OROC all, (default truncation used)
  //
  for (Int_t i=0;i<4; i++){
    signal[i]=fTPCsignalRegion[i];
  }
}

//_______________________________________________________________________________________________
void  AliTPCdEdxInfo::SetTPCSignalRegionInfo(Double_t signal[4], Char_t ncl[3], Char_t nrows[3]){
  //
  // Set the TPC dEdx variables per region
  //
  // Double32_t  fTPCsignalRegion[4]; // TPC dEdx signal in 4 different regions - 0 - IROC, 1- OROC medium, 2 - OROC long, 3- OROC all, (default truncation used)  
  // Char_t      fTPCsignalNRegion[3]; // number of clusters above threshold used in the dEdx calculation
  // Char_t      fTPCsignalNRowRegion[3]; // number of crosed rows used in the dEdx calculation - signal below threshold included
  //
  for (Int_t i=0;i<3; i++){
    fTPCsignalRegion[i]=signal[i];
    fTPCsignalNRegion[i]=ncl[i];
    fTPCsignalNRowRegion[i]=nrows[i];
  }
  fTPCsignalRegion[3]=signal[3];
  return;
}

//_______________________________________________________________________________________________
void  AliTPCdEdxInfo::SetTPCSignals(Double_t signal[4]){
  //
  // Set the TPC dEdx variables per region
  //
  // Double32_t  fTPCsignalRegionQmax[4]; // TPC dEdx signal in 4 different regions - 0 - IROC, 1- OROC medium, 2 - OROC long, 3- OROC all, (default truncation used)
  //
  for (Int_t i=0;i<4; i++){
    fTPCsignalRegion[i]=signal[i];
  }
}

//_______________________________________________________________________________________________
void  AliTPCdEdxInfo::GetTPCSignalRegionInfoQmax(Double_t signal[4], Char_t ncl[3], Char_t nrows[3]) const {
  //
  // Get the TPC dEdx variables per region
  //
  // Double32_t  fTPCsignalRegionQmax[4]; // TPC dEdx signal in 4 different regions - 0 - IROC, 1- OROC medium, 2 - OROC long, 3- OROC all, (default truncation used)
  // Char_t      fTPCsignalNRegion[3]; // number of clusters above threshold used in the dEdx calculation
  // Char_t      fTPCsignalNRowRegion[3]; // number of crosed rows used in the dEdx calculation - signal below threshold included
  //
  for (Int_t i=0; i<3; i++){
    signal[i]=fTPCsignalRegionQmax[i];
    ncl[i]=fTPCsignalNRegion[i];
    nrows[i]=fTPCsignalNRowRegion[i];
  }
  signal[3]=fTPCsignalRegionQmax[3];
  return;
}

//_______________________________________________________________________________________________
void  AliTPCdEdxInfo::GetTPCSignalsQmax(Double_t signal[4]) const {
  //
  // Set the TPC dEdx variables per region
  //
  // Double32_t  fTPCsignalRegionQmax[4]; // TPC dEdx signal in 4 different regions - 0 - IROC, 1- OROC medium, 2 - OROC long, 3- OROC all, (default truncation used)
  //
  for (Int_t i=0;i<4; i++){
    signal[i]=fTPCsignalRegionQmax[i];
  }
}

//_______________________________________________________________________________________________
void  AliTPCdEdxInfo::SetTPCSignalRegionInfoQmax(Double_t signal[4], Char_t ncl[3], Char_t nrows[3]){
  //
  // Set the TPC dEdx variables per region
  //
  // Double32_t  fTPCsignalRegionQmax[4]; // TPC dEdx signal in 4 different regions - 0 - IROC, 1- OROC medium, 2 - OROC long, 3- OROC all, (default truncation used)
  // Char_t      fTPCsignalNRegion[3]; // number of clusters above threshold used in the dEdx calculation
  // Char_t      fTPCsignalNRowRegion[3]; // number of crosed rows used in the dEdx calculation - signal below threshold included
  //
  for (Int_t i=0;i<3; i++){
    fTPCsignalRegionQmax[i]=signal[i];
    fTPCsignalNRegion[i]=ncl[i];
    fTPCsignalNRowRegion[i]=nrows[i];
  }
  fTPCsignalRegionQmax[3]=signal[3];
  return;
}

//_______________________________________________________________________________________________
void  AliTPCdEdxInfo::SetTPCSignalsQmax(Double_t signal[4]){
  //
  // Set the TPC dEdx variables per region
  //
  // Double32_t  fTPCsignalRegionQmax[4]; // TPC dEdx signal in 4 different regions - 0 - IROC, 1- OROC medium, 2 - OROC long, 3- OROC all, (default truncation used)
  //
  for (Int_t i=0;i<4; i++){
    fTPCsignalRegionQmax[i]=signal[i];
  }
}


Double_t AliTPCdEdxInfo::GetWeightedMean(Int_t qType, Int_t wType, Double_t w0, Double_t w1, Double_t w2){
  //
  // Get weighted mean of the dEdx information
  //
  Double_t *info = (qType==0)? fTPCsignalRegion :  fTPCsignalRegionQmax;
  Char_t *ninfo = (wType==0)? fTPCsignalNRegion:  fTPCsignalNRowRegion;
  Double_t weight[3]={w0,w1,w2};
  Double_t sum=0;
  Double_t sumw=0;
  for (Int_t i=0; i<3; i++){
    sum+= info[i]*Double_t(ninfo[i])*weight[i];
    sumw+= ninfo[i]*weight[i];
  }
  Double_t result = (sumw>0) ? sum/sumw:0;
  return result;
}
