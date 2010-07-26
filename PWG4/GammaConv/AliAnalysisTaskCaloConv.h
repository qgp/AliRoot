#ifndef ALIANALYSISTASKCALOCONV_H
#define ALIANALYSISTASKCALOCONV_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////
//--------------------------------------------- 
// Class used to do analysis on conversion pairs
//---------------------------------------------
////////////////////////////////////////////////

#include "AliAnalysisTaskSE.h"

class AliESDInputHandler;
class AliESDEvent;
class AliAODEvent;
class AliMCEvent;
class TList;
class AliCFContainer ;
class AliStack;
class AliESDpid ;
class AliEMCALGeoUtils ;
class AliPHOSGeoUtils ;
class AliExternalTrackParam ;


class AliAnalysisTaskCaloConv : public AliAnalysisTaskSE
{
	
 public:
  AliAnalysisTaskCaloConv();
  AliAnalysisTaskCaloConv(const char* name);
  virtual ~AliAnalysisTaskCaloConv() ;// virtual destructor
		
  // Implementation of interface methods
  virtual void Init();
  virtual void LocalInit() {Init();}
  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t *option);
  virtual void Terminate(Option_t * /*option*/){}
  virtual void ConnectInputData(Option_t * option);
		
  void SetPi0Threshold1(Double_t thrs=0.5){fPi0Thresh1=thrs ; } //Threshold 1 for Calo photon
  void SetPi0Threshold2(Double_t thrs=1.){fPi0Thresh2=thrs ; }  //Threshold 2 for Calo Photon
		
  void SetTriggerFlags(Bool_t flag){fTriggerCINT1B=flag;}

  //PID setters
  void SetDEdxCuts(Double_t sEUp=5., Double_t sEDn=-3., Double_t sPiUp=0., Double_t sPiDn=1.){
     fnSigmaAboveElectronLine= sEUp; fnSigmaBelowElectronLine=sEDn;
     fnSigmaAbovePionLine=sPiUp; fpnSigmaAbovePionLine=sPiDn;}
  void SetConvProbCut(Double_t prob=0.){ fprobCut=prob;}
  void SetConvMaxRCut(Double_t maxR=180.){fmaxR=maxR ;} 
  void SetConvMaxZCut(Double_t maxZ=240.){fmaxZ=maxZ ;}
  void SetConvMaxEtaCut(Double_t eta=0.9){fetaCut=eta ;}
  void SetConvMinPtCut(Double_t minPt=0.02){fptCut=minPt ;}
  void SetConvMaxChi2Cut(Double_t chi2=30.){fchi2CutConversion=chi2 ;}

 protected:
   void InitGeometry() ; //Create PHOS/EMCAL geometry
   void ProcessMC();
   void SelectConvPhotons() ; //collects V0s in event
   void SelectPHOSPhotons(); //collects PHOS photons in event
   void SelectEMCALPhotons(); //collects EMCAL photons in event
   void FillRealMixed() ;     //Fills Real and Mixed inv.mass distributions
   void FillHistogram(const char * key,Double_t x) const ; //Fill 1D histogram witn name key
   void FillHistogram(const char * key,Double_t x, Double_t y) const ; //Fill 2D histogram witn name key
   void FillHistogram(const char * key,Double_t x, Double_t y, Double_t z) const ; //Fill 3D histogram witn name key
   Double_t PlanarityAngle(const AliExternalTrackParam * pos, const AliExternalTrackParam * neg)const ;


 private:
  AliAnalysisTaskCaloConv(const AliAnalysisTaskCaloConv&); // Not implemented
  AliAnalysisTaskCaloConv& operator=(const AliAnalysisTaskCaloConv&); // Not implemented
		
  enum{
    kCaloPIDdisp = BIT(14),
    kCaloPIDtof  = BIT(15),
    kCaloPIDneutral= BIT(16)
  };
  enum{
    kConvOnFly= BIT(14),
    kConvKink = BIT(15),
    kConvdEdx = BIT(16),
    kConvProb = BIT(17),
    kConvR    = BIT(18),
    kConvZR   = BIT(19),
    kConvNDF  = BIT(20),
    kConvEta  = BIT(21),
    kConvPlan = BIT(22)
  };
  
		
  AliESDEvent* fESDEvent; //!pointer to the ESDEvent
  AliESDpid * fESDpid ;   //class for Track PID calculation
  AliStack * fStack;      //! pointer to the MC particle stack
  TList * fOutputContainer; //final histogram container
  TList * fCFOutputContainer; //Correction Fremework conntainer

  AliCFContainer * fConvCFCont ;  //Container for Conv. photons correction calculation
  AliCFContainer * fPHOSCFCont ;  //Container for Conv. photons correction calculation
  AliCFContainer * fEMCALCFCont ; //Container for Conv. photons correction calculation
  AliCFContainer * fPi0CFCont ;   //Container for Conv. photons correction calculation
		
  Bool_t fTriggerCINT1B; //Flag to select trigger CINT1B
		
  Double_t fMinOpeningAngleGhostCut; // minimum angle cut
		
  AliPHOSGeoUtils  *fPHOSgeom;      //!PHOS geometry
  AliEMCALGeoUtils *fEMCALgeom;     //!EMCAL geometry
  Double_t fPi0Thresh1 ; //Threshold 1 for pi0 calibration
  Double_t fPi0Thresh2 ; //Threshold 2 for pi0 calibration

  //Containers for storing previous events
  // 10 bins for vtx class 
  TList * fPHOSEvents[10] ;      //Container for PHOS photons
  TList * fEMCALEvents[10] ;     //Container for EMCAL photons
  TList * fConvEvents[10] ;      //Container for conversion photons

  TClonesArray * fConvEvent ;    //Conversion photons in current event
  TClonesArray * fPHOSEvent ;    //PHOS photons in current event
  TClonesArray * fEMCALEvent ;   //EMCAL  photons in current event

  Double_t fnSigmaAboveElectronLine;
  Double_t fnSigmaBelowElectronLine;
  Double_t fnSigmaAbovePionLine;
  Double_t fpnSigmaAbovePionLine;
  Double_t fprobCut;
  Double_t fmaxR ;
  Double_t fmaxZ ;
  Double_t fetaCut ;
  Double_t fptCut ;
  Double_t fchi2CutConversion ;

  ClassDef(AliAnalysisTaskCaloConv, 1); // Analysis task for conversion + calorimeters
};

#endif //ALIANALYSISTASKCALOCO_H
