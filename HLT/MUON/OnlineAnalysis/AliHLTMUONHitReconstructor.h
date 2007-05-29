#ifndef ALIHLTMUONHITRECONSTRUCTOR_H
#define ALIHLTMUONHITRECONSTRUCTOR_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////
//Author : Indranil Das, SINP, INDIA
//         Sukalyan Chattopadhyay, SINP, INDIA
//         
//
//Email :  indra.das@saha.ac.in
//         sukalyan.chattopadhyay@saha.ac.in 
///////////////////////////////////////////////



#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <cmath>
#include <map>

#include <AliHLTLogging.h>
#include "AliHLTMUONRecHitsBlockStruct.h"

using namespace std;

typedef  map<int,int> BusToDetElem ;


struct DHLTLut{
  int fIdManuChannel;
  int fIX,fIY;
  float fRealX,fRealY,fRealZ;
  int fPlane,fPcbZone;
};

struct DHLTPad{
  int fDetElemId;
  int fBuspatchId;
  int fIdManuChannel;
  int fIX,fIY;
  float fRealX,fRealY,fRealZ;
  int fPlane,fPcbZone;
  int fCharge;
};




class AliHLTMUONHitReconstructor : public AliHLTLogging
{
public:

  AliHLTMUONHitReconstructor();
  virtual ~AliHLTMUONHitReconstructor(void);

  bool LoadLookUpTable(DHLTLut* lookUpTableData, int lookUpTableId);
  bool SetBusToDetMap(BusToDetElem busToDetElem);
  
  //bool Init();
  bool Run(int* rawData, int *rawDataSize, AliHLTMUONRecHitStruct recHit[], int *nofHit);

  void SetDCCut(int dcCut) {fDCCut = dcCut;}
  void SetDebugLevel(int debugLevel) {fDebugLevel = debugLevel;}
  int GetDebugLevel(){return fDebugLevel;}
  int GetDEId(int iBusPatch) {return fBusToDetElem[iBusPatch] ;}           
    
  int GetLutLine(int iDDL);
  
  static const int fgkDetectorId ;            // DDL Offset
  static const int fgkDDLOffSet ;             // DDL Offset
  static const int fgkNofDDL ;                // Number of DDL 
  static const int fgkDDLHeaderSize  ;                      // DDL header size  
protected:
  AliHLTMUONHitReconstructor(const AliHLTMUONHitReconstructor& rhs); // copy constructor
  AliHLTMUONHitReconstructor& operator=(const AliHLTMUONHitReconstructor& rhs); // assignment operator
private:
  

  static const int fgkEvenLutSize ;           // Size of the LookupTable with event DDLID
  static const int fgkOddLutSize ;            // Size of the LookupTable with odd DDLID
  static const int fgkLutLine[2];             // nof Line in LookupTable    

  static const int fgkMinIdManuChannel[2];    // Minimum value of idManuChannel in LookupTable  
  static const int fgkMaxIdManuChannel[2];    // Maximum value of idManuChannel in LookupTable  
  static const float fgkHalfPadSize[3];       // pad halflength for the pcb zones  
  

  int fkBlockHeaderSize ;                     // Block header size
  int fkDspHeaderSize  ;                      // DSP header size
  int fkBuspatchHeaderSize ;                  // buspatch header size
  
  int fDCCut;                                 // DC Cut value

  DHLTPad* fPadData;                          // pointer to the array containing the information of each padhits
  DHLTLut* fLookUpTableData;                      // pointer to the array of Lookuptable data
  
  AliHLTMUONRecHitStruct *fRecPoints;          // Reconstructed hits
  int *fRecPointsCount;                       // nof reconstructed hit  
  int fMaxRecPointsCount;                    // max nof reconstructed hit  
   
  int fCentralCountB, fCentralCountNB;                 // centeral hits 
  int fIdOffSet,fDDLId;                       // DDLId and DDL id offset
  int fDigitPerDDL;                                    // Total nof Digits perDDL 
  
  int *fDetManuChannelIdList;                          // pointer to an array of idManuChannel
  int *fCentralChargeB,*fCentralChargeNB;              // pointer to an array of central hit
  float *fRecX,*fRecY;                                 // pointer to an array of reconstructed hit
  float *fAvgChargeX,*fAvgChargeY;                     // average charge on central pad found using CG method
  int fGetIdTotalData[336][80][2] ;                    // an array of idManuChannel with argumrnt of centralX,centralY and  planeType
  int fNofFiredDetElem,fMaxFiredPerDetElem[13];        // counter for detector elements that are fired 
  int fDebugLevel;
  BusToDetElem fBusToDetElem;

  bool ReadDDL(int* rawData, int *rawDataSize);
  void FindCentralHits(int minPadId, int maxPadId);
  bool FindRecHits() ;
  void RecXRecY();
  bool MergeRecHits();

  ClassDef(AliHLTMUONHitReconstructor,0)
};


#endif // ALIHLTMUONHITRECONSTRUCTOR_H
