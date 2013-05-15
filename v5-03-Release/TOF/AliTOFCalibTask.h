#ifndef ALITOFCALIBTASK_H
#define ALITOFCALIBTASK_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */

////////////////////////////////////////////
//    task for TOF calibration            //
//         C.Zampolli                     //
////////////////////////////////////////////

/* $Id$ */

#define TOFCHANNELS       157248   // number TOF channels 
#define CHENTRIES            500   // number of entries per TOF channel per run
                                   // (to be divided by 5 to get the 
                                   // real number of entries), bigarray 
#define LOWERMOMBOUND        1.0   // [GeV/c] default value Pb-Pb
#define UPPERMOMBOUND        1.8   // [GeV/c] default value Pb-Pb
#define MINTIME                5   // min delta time of flight value (ns)
#define NIDX                   5   // number of values stored 
                                   // in big/smallarray per ESD track
#define NIDXSMALL              3   // number of values stored 
                                   // after Comb PID per ESD track
#define DELTAIDXTOT            0   // index for ToT in bigarray
#define DELTAIDXTIME           1   // index for TOF time in big/smallarray
#define DELTAIDXEXTIMEPI       2   // index for Exp Time Pi in bigarray, will
                                   // be the same element in the array where 
                                   // the assigned time after Comb PID will
                                   // be stored
#define DELTAIDXEXTIMEKA       3   // index for Exp Time Ka in bigarray
#define DELTAIDXEXTIMEPR       4   // index for Exp Time Pr in bigarray
#define TRACKERROR       90*1E-3   // error on the tracks for 
                                   // Combinatorial PID (ns)

#include "AliAnalysisTaskSE.h"  

class TTree;
class AliESDtrack ;  
class TFile; 
class TH1F ;
class TH1I ;
class TH1D ;
class TH2F ;
class AliESDEvent ; 
class TList ; 
class AliTOFArray;

class AliTOFCalibTask : public AliAnalysisTaskSE {

public:
	//  AliTOFCalibTask() ; //ctor
  AliTOFCalibTask(const char *name = "TOFCalibTask") ; //ctor
  AliTOFCalibTask(const AliTOFCalibTask & calibtask); // copy constructor
  AliTOFCalibTask& operator=(const AliTOFCalibTask & calibtask); // assignment operator
  virtual ~AliTOFCalibTask(); //dtor
  virtual void UserExec(Option_t * opt="") ;
  virtual void UserCreateOutputObjects();
  virtual void Terminate(Option_t * opt = "") ;

private:
  Bool_t  Select(AliESDtrack *t);
  Int_t   SelectOnTime(Float_t *charray, Int_t ntracks, Int_t ich);
  Bool_t  CombPID(Float_t *smallarray, Int_t size);
  void    BookHistos();
  void    DrawHistos();
  Float_t LoopCombPID(Int_t itrkinset, Int_t ntrkinset, Float_t **exptof, Float_t *texp, Float_t *timeofflight, Int_t *index, Float_t chisquarebest);

  //leaf types
  AliESDEvent* fESD ;              //! Declaration of leave types
  Float_t fToT;                 // Time over Threshold, ns
  Float_t fTime;                // TOF time, ns
  Float_t fExpTimePi;           // exp time, Pions, ns
  Float_t fExpTimeKa;           // exp time, Kaons, ns
  Float_t fExpTimePr;           // exp time, Protons, ns
  Float_t fMinTime;             // min TOF time for track selection; not used
  AliTOFArray* fTOFArray;       //! object for TOF signal, arranged per channel
  Int_t fnESD;                  // number of analyzed ESD tracks 
  Int_t fnESDselected;          // number of selected ESD tracks 
  Int_t fnESDkTOFout;           // number of ESD tracks with kTOFout
  Int_t fnESDkTIME;             // number of ESD tracks with kTIME
  Int_t fnESDassTOFcl;          // number of ESD tracks with assigned TOF cluster
  Int_t fnESDTIMEcut;           // number of ESD tracks with TOF time < 17 ns
  Int_t fnESDTRDcut;            // number of ESD tracks with TRD ok

  // Histos
  TH1F* fhToT;                  // ToT histo
  TH1F* fhTime;                 // Time histo
  TH1F* fhExpTimePi;            // Exp Time Pi histo                  
  TH1F* fhExpTimeKa;            // Exp Time Ka histo                  
  TH1F* fhExpTimePr;            // Exp Time Pr histo                  
  TH1I* fhPID;                  // PID histo                  
  TH1D* fhch;                   // TOF channel histo               
  TH1I* fhESD;                  // n. of analyzed histo               
  TH1I* fhESDselected;          // n. of selected ESD tracks histo            
  TH1I* fhESDkTOFout;           // n. of ESD tracks with kTOFout histo            
  TH1I* fhESDkTIME;             // n. of ESD tracks with kTIME histo             
  TH1I* fhESDassTOFcl;          // n. of ESD tracks with assTOFcl histo               
  TH1I* fhESDTIMEcut;           // n. of ESD tracks with TIMEcut histo               
  TH1I* fhESDTRDcut;            // n. of ESD tracks with TRDcut histo               

  Int_t fassparticle[11];       // array for assigned identities

  TList *fListOfHistos;         //! list of Histos to be stored in the output container 1
  TList *fListArray;            //! list of Array for output container 2

  ClassDef(AliTOFCalibTask, 2); //  TOF Calib task 
};
#endif // ALITOFCALIBTASK_H

