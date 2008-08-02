#ifndef AliITSQASSDDataMakerRec_H
#define AliITSQASSDDataMakerRec_H
/* Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*  $Id$  */

//
//  Checks the quality assurance. 
//  By comparing with reference data
//  contained in a DB
//  -------------------------------------------------------------
//  W. Ferrarese + P. Cerello Feb 2008
//  INFN Torino

#include "AliQA.h"
#include "AliITSQADataMakerRec.h"
#include "AliQADataMakerRec.h"

class TObjArray;
class TH1D;

class AliRawReader;
class AliITSQADataMakerRec;
class AliCDBManager;

class AliITSQASSDDataMakerRec: public TObject {

public:
  AliITSQASSDDataMakerRec(AliITSQADataMakerRec *aliITSQADataMakerRec, Bool_t kMode = kFALSE, Int_t ldc=0);  //ctor
  AliITSQASSDDataMakerRec(const AliITSQASSDDataMakerRec& qadm);
  AliITSQASSDDataMakerRec& operator = (const AliITSQASSDDataMakerRec& qac);
  virtual void InitRaws();
  virtual void InitRecPoints();
  virtual void MakeRaws(AliRawReader *rawReader);
  virtual void MakeRecPoints(TTree *clustersTree);
  virtual void StartOfDetectorCycle();
  virtual void EndOfDetectorCycle(AliQA::TASKINDEX_t task, TObjArray * list);
  virtual ~AliITSQASSDDataMakerRec(); // dtor
  Int_t GetOffset() { return fGenOffset; }
  Int_t GetTaskHisto() { return fSSDhTask; }

 private:

  void GetOccupancyStrip(TH1 *lHisto, Int_t *occupancyMatrix); 
  Double_t GetOccupancyModule(TH1 *lHisto, Int_t stripside); 
  void MonitorOCDBObjects();

  static const Int_t fgkNumOfLDCs = 3;      //number of SSD LDCs
  static const Int_t fgkNumOfDDLs = 16;      //number of SSD DDLs
  static const Int_t fgkSSDMODULES = 1698;      //total number of SSD modules
  static const Int_t fgkSSDLADDERSLAYER5 = 34; //ladders on layer 5
  static const Int_t fgkSSDLADDERSLAYER6 = 38; //ladders on layer 6
  static const Int_t fgkSSDMODULESPERLADDERLAYER5 = 22; //modules per ladder - layer 5
  static const Int_t fgkSSDMODULESPERLADDERLAYER6 = 25; //modules per ladder - layer 6
  static const Int_t fgkSSDMODULESLAYER5 = 748; //total number of SSD modules - layer5
  static const Int_t fgkSSDMODULESLAYER6 = 950; //total number of SSD modules - layer6
  static const Int_t fgkNumberOfPSideStrips = 768; //number of P-side strips
  
  AliITSQADataMakerRec *fAliITSQADataMakerRec;  //pointer to the main ctor
  Int_t   fSSDEvent;                            //event counter
  Int_t   fSSDEventPerCycle;                    //event counter per cycle
  Bool_t  fkOnline;                             //online (1) or offline (0) use
  Int_t   fLDC;                                 //LDC number (0 for offline, 1 to 4 for online) 
  Int_t   fSSDRawsOffset;                       //SSD raw data plot offset
  Int_t   fSSDRawsDAOffset;                     //SSD DA plot offset
  Int_t   fSSDRawsCommonLevelOffset;            //Raw data QA - top level offset - histos used both online and offline 
  Int_t   fSSDhTask;                            //number of histo booked for each SSD task
  Int_t   fGenOffset;                           //qachecking offset
  TH1D   *fHistSSDRawSignalModule[fgkSSDMODULES]; //raw signal vs strip number - SSD                   
  Int_t   fOccupancyMatrix[fgkSSDMODULES][2*fgkNumberOfPSideStrips]; //occupancy values per strip

  AliCDBManager *fCDBManager; //CDB manager

  ClassDef(AliITSQASSDDataMakerRec,4)           // description 
};

#endif
