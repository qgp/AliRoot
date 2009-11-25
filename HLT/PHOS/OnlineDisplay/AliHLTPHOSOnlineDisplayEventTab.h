//-*- Mode: C++ -*-
// $Id$

//**************************************************************************
//* Copyright(c) 2006, ALICE Experiment at CERN, All rights reserved.      *
//*                                                                        *
//* Authors: Per Thomas Hille for the ALICE                                *
//* offline/HLT Project. Contributors are mentioned in the code where      *
//* appropriate.                                                           *
//*                                                                        *
//* Permission to use, copy, modify and distribute this software and its   *
//* documentation strictly for non-commercial purposes is hereby granted   *
//* without fee, provided that the above copyright notice appears in all   *
//* copies and that both the copyright notice and this permission notice   *
//* appear in the supporting documentation. The authors make no claims     *
//* about the suitability of this software for any purpose. It is          *
//* provided "as is" without express or implied warranty.                  *
//**************************************************************************/

#ifndef ALIHLTPHOSONLINEDISPLAYEVENTTAB_H
#define ALIHLTPHOSONLINEDISPLAYEVENTTAB_H

//#include <TGTab.h>
// #include <TRootEmbeddedCanvas.h>
#include "AliHLTPHOSOnlineDisplayTab.h"
// #include <TCanvas.h>
// #include <TH2D.h>
// #include <TH1D.h>
// #include "AliHLTPHOSOnlineDisplayTH2D.h"
#include "AliHLTPHOSConstants.h"

#include "AliHLTPHOSMapper.h"

#define NZRCUCOORD 2
#define NXRCUCOORD 2

using namespace PhosHLTConst;

class TGTab;
class TRootEmbeddedCanvas;
class TCanvas;
class TH2D;
class TH1D;
class AliHLTPHOSOnlineDisplayTH2D;
class AliHLTPHOSGetEventButton;
class AliHLTHOMERReader;
class AliHLTPHOSRcuCellEnergyDataStruct;
class AliHLTPHOSOnlineDisplay;
//class AliHLTPHOSSharedMemoryInterface;
class AliHLTPHOSSharedMemoryInterfacev2;
class AliHLTPHOSChannelRawDataStruct;

class AliHLTPHOSOnlineDisplayEventTab : public AliHLTPHOSOnlineDisplayTab
{
 public:
 
  virtual ~AliHLTPHOSOnlineDisplayEventTab();


  AliHLTPHOSOnlineDisplayEventTab(AliHLTPHOSOnlineDisplay * onlineDisplayPtr, TGTab  *tabPtr, 
				  AliHLTHOMERReader * homerSyncPtr, 
				  AliHLTHOMERReader * homerPtrs[MAXHOSTS], 
				  int nHosts,  int runnumber = -1);
//   AliHLTPHOSOnlineDisplayEventTab(AliHLTPHOSOnlineDisplay *onlineDisplayPtr, TGTab *tabPtr, 
// 				  AliHLTHOMERReader *fgHomerReaderPtr, 
// 				  AliHLTHOMERReader *fgHomerReadersPtr[MAXHOSTS], 
// 				  int nHosts, const int runnumber = -1);
    //    {

 
  
 

/* 
  void SetRunNumber(const int runnumber) 
  {
    
    fRunNumber = runnumber ;
    cout << __FILE__ <<":"<< __LINE__ << "RunNumber was set to "<< fRunNumber  <<endl; ;
  };
  */

  Int_t GetRawData(TH1D *histPtr, int x, int z, int gain);

  void UpdateDisplay();
  int GetNextEvent();
  virtual void ReadBlockData(AliHLTHOMERReader *homeReaderPtr);
  void FindFourierBlocks(AliHLTHOMERReader *homeReaderPtr) const;

  void ResetDisplay();
  TGTab               *fTab; //!
  TGTab               *fSubTab1; //!
  TRootEmbeddedCanvas *fEc1, *fEc2, *fEc3, *fEc4, *fEc5, *fEc6; //!
  TGCompositeFrame    *fSubF1, *fSubF2, *fSubF3; //!
  TCanvas *fgCanvasPtr[NGAINS]; //!
  AliHLTPHOSOnlineDisplayTH2D *fgLegoPlotPtr[NGAINS]; //!

  /*
  int *fChannelData[NMODULES][NXRCUCOORD][NZRCUCOORD][NXCOLUMNSRCU][NZROWSRCU][NGAINS];
  Int_t fNChannelSamples[NMODULES][NXRCUCOORD][NZRCUCOORD][NXCOLUMNSRCU][NZROWSRCU][NGAINS];
  Int_t fChannelEnergy[NMODULES][NXRCUCOORD][NZRCUCOORD][NXCOLUMNSRCU][NZROWSRCU][NGAINS];
  */  

  int *fChannelData[NMODULES][NZROWSMOD][NXCOLUMNSMOD][NGAINS];
  Int_t fNChannelSamples[NMODULES][NZROWSMOD][NXCOLUMNSMOD][NGAINS];
  Int_t fChannelEnergy[NMODULES][NZROWSMOD][NXCOLUMNSMOD][NGAINS];

  //protected:
  //  Bool_t fgAccumulate;

 private:
  AliHLTPHOSOnlineDisplayEventTab();
  AliHLTPHOSOnlineDisplayEventTab(const AliHLTPHOSOnlineDisplayEventTab&);
  AliHLTPHOSOnlineDisplayEventTab& operator=(const AliHLTPHOSOnlineDisplayEventTab&);

  void FillRawData(const AliHLTPHOSChannelRawDataStruct &rawStr);
  
  // void ChannelId2Coordinates(const UShort_t chid) const ;

  AliHLTPHOSGetEventButton* fgEventButtPtr; 
  void InitDisplay(TGTab *tabPtr){};
  void InitDisplay(TGTab * tabPtr, int runnumber);
  AliHLTPHOSOnlineDisplay *fOnlineDisplayPtr;
  // AliHLTPHOSSharedMemoryInterface *fShmPtr;   
  AliHLTPHOSSharedMemoryInterfacev2 *fShmPtr;   
//   AliHLTPHOSOnlineDisplayEventTab(const AliHLTPHOSOnlineDisplayEventTab& v);
//   AliHLTPHOSOnlineDisplayEventTab & operator=(const AliHLTPHOSOnlineDisplayEventTab v){}


  ///int fEvent

};


#endif
