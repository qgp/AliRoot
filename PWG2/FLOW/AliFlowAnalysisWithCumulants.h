/* 
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. 
 * See cxx source for full Copyright notice 
 * $Id$ 
 */

/******************************** 
 * flow analysis with cumulants * 
 *                              * 
 * author: Ante Bilandzic       * 
 *          (anteb@nikhef.nl)   *
 *******************************/ 

#ifndef ALIFLOWANALYSISWITHCUMULANTS_H
#define ALIFLOWANALYSISWITHCUMULANTS_H

#include "AliFlowCommonConstants.h"
#include "AliFlowCumuConstants.h"

class TObjArray;
class TList;
class TFile;

class TH1;
class TProfile;
class TProfile2D;
class TProfile3D;

class AliFlowEventSimple;
class AliFlowTrackSimple;
class AliFlowCommonHist;
class AliFlowCommonHistResults;
class AliFlowVector;

//================================================================================================================

class AliFlowAnalysisWithCumulants{
 public:
  AliFlowAnalysisWithCumulants();
  virtual ~AliFlowAnalysisWithCumulants(); 
  
  virtual void CreateOutputObjects();
  virtual void Make(AliFlowEventSimple* anEvent);
  virtual void Finish();
  virtual void WriteHistograms(TString* outputFileName);
  
//----------------------------------------------------------------------------------------------------------------
//                                            setters and getters                                                 
//----------------------------------------------------------------------------------------------------------------
  TList* GetHistList() const {return this->fHistList;}      //output histogram list
  
  void SetIntFlowResults(TH1D* ifr)  {this->fIntFlowResultsGFC = ifr;};
  TH1D* GetIntFlowResults() const    {return this->fIntFlowResultsGFC;};
  
  void SetDiffFlowResults2nd(TH1D* diff2nd)  {this->fDiffFlowResults2ndOrderGFC = diff2nd;};
  TH1D* GetDiffFlowResults2nd() const        {return this->fDiffFlowResults2ndOrderGFC;};
  
  void SetDiffFlowResults4th(TH1D* diff4th)  {this->fDiffFlowResults4thOrderGFC = diff4th;};
  TH1D* GetDiffFlowResults4th() const        {return this->fDiffFlowResults4thOrderGFC;};
  
  void SetDiffFlowResults6th(TH1D* diff6th)  {this->fDiffFlowResults6thOrderGFC = diff6th;};
  TH1D* GetDiffFlowResults6th() const        {return this->fDiffFlowResults6thOrderGFC;};
  
  void SetDiffFlowResults8th(TH1D* diff8th)  {this->fDiffFlowResults8thOrderGFC = diff8th;};
  TH1D* GetDiffFlowResults8th() const        {return this->fDiffFlowResults8thOrderGFC;};
  
  void SetCommonHistsResults2nd(AliFlowCommonHistResults* chr2nd)  {this->fCommonHistsResults2nd = chr2nd;};
  AliFlowCommonHistResults* GetCommonHistsResults2nd() const       {return this->fCommonHistsResults2nd;};
  
  void SetCommonHistsResults4th(AliFlowCommonHistResults* chr4th)  {this->fCommonHistsResults4th = chr4th;};
  AliFlowCommonHistResults* GetCommonHistsResults4th() const       {return this->fCommonHistsResults4th;};
  
  void SetCommonHistsResults6th(AliFlowCommonHistResults* chr6th)  {this->fCommonHistsResults6th = chr6th;};
  AliFlowCommonHistResults* GetCommonHistsResults6th() const       {return this->fCommonHistsResults6th;};
  
  void SetCommonHistsResults8th(AliFlowCommonHistResults* chr8th)  {this->fCommonHistsResults8th = chr8th;};
  AliFlowCommonHistResults* GetCommonHistsResults8th() const       {return this->fCommonHistsResults8th;};
  
  void SetIntFlowGenFun(TProfile2D* ifgf)  {this->fIntFlowGenFun = ifgf;};
  TProfile2D* GetIntFlowGenFun() const     {return this->fIntFlowGenFun;};

  void SetIntFlowGenFun4(TProfile2D* ifgf4)  {this->fIntFlowGenFun4 = ifgf4;}; //(only for other system of Eq.)
  TProfile2D* GetIntFlowGenFun4() const      {return this->fIntFlowGenFun4;};  //(only for other system of Eq.)
  
  void SetIntFlowGenFun6(TProfile2D* ifgf6)  {this->fIntFlowGenFun6 = ifgf6;}; //(only for other system of Eq.)
  TProfile2D* GetIntFlowGenFun6() const      {return this->fIntFlowGenFun6;};  //(only for other system of Eq.)
  
  void SetIntFlowGenFun8(TProfile2D* ifgf8)  {this->fIntFlowGenFun8 = ifgf8;}; //(only for other system of Eq.)
  TProfile2D* GetIntFlowGenFun8() const      {return this->fIntFlowGenFun8;};  //(only for other system of Eq.)
  
  void SetIntFlowGenFun16(TProfile2D* ifgf16)  {this->fIntFlowGenFun16 = ifgf16;}; //(only for other system of Eq.)
  TProfile2D* GetIntFlowGenFun16() const       {return this->fIntFlowGenFun16;};   //(only for other system of Eq.)
 
  void SetAverageMultiplicity4(TProfile* am4)     {this->fAvMultIntFlow4GFC = am4;};  //(only for other system of Eq.)
  TProfile* GetAverageMultiplicity4() const       {return this->fAvMultIntFlow4GFC;}; //(only for other system of Eq.)
  
  void SetAverageMultiplicity6(TProfile* am6)     {this->fAvMultIntFlow6GFC = am6;};  //(only for other system of Eq.)
  TProfile* GetAverageMultiplicity6() const       {return this->fAvMultIntFlow6GFC;}; //(only for other system of Eq.)
  
  void SetAverageMultiplicity8(TProfile* am8)     {this->fAvMultIntFlow8GFC = am8;};  //(only for other system of Eq.)
  TProfile* GetAverageMultiplicity8() const       {return this->fAvMultIntFlow8GFC;}; //(only for other system of Eq.)
  
  void SetAverageMultiplicity16(TProfile* am16)    {this->fAvMultIntFlow16GFC = am16;}; //(only for other system of Eq.)
  TProfile* GetAverageMultiplicity16() const       {return this->fAvMultIntFlow16GFC;}; //(only for other system of Eq.)
 
  void SetDiffFlowGenFunRe(TProfile3D* dfgfRe)    {this->fDiffFlowGenFunRe = dfgfRe;};
  TProfile3D* GetDiffFlowGenFunRe() const         {return this->fDiffFlowGenFunRe;};
  
  void SetDiffFlowGenFunIm(TProfile3D* dfgfIm)  {this->fDiffFlowGenFunIm = dfgfIm;};
  TProfile3D* GetDiffFlowGenFunIm() const         {return this->fDiffFlowGenFunIm;};
  
  void SetNumberOfParticlesPerPtBin(TProfile* nopppb)    {this->fBinNoOfParticles = nopppb;};
  TProfile* GetNumberOfParticlesPerPtBin() const         {return this->fBinNoOfParticles;};  
  
  void SetAverageMultiplicity(TProfile* am)      {this->fAvMultIntFlowGFC = am;};
  TProfile* GetAverageMultiplicity() const       {return this->fAvMultIntFlowGFC;};
  
  void SetQVectorComponents(TProfile* sqvc)    {this->fQVectorComponentsGFC = sqvc;};
  TProfile* GetQVectorComponents() const       {return this->fQVectorComponentsGFC;};
//----------------------------------------------------------------------------------------------------------------
 
 private:
  AliFlowAnalysisWithCumulants(const AliFlowAnalysisWithCumulants& afawc);
  AliFlowAnalysisWithCumulants& operator=(const AliFlowAnalysisWithCumulants& afawc);
  AliFlowTrackSimple* fTrack;                                   //track
  static const Int_t fgkQmax = AliFlowCumuConstants::kQmax;     //needed for numerics
  static const Int_t fgkPmax = AliFlowCumuConstants::kPmax;     //needed for numerics  
  static const Int_t fgkQmax4 = AliFlowCumuConstants::kQmax4;   //needed for numerics
  static const Int_t fgkPmax4 = AliFlowCumuConstants::kPmax4;   //needed for numerics  
  static const Int_t fgkQmax6 = AliFlowCumuConstants::kQmax6;   //needed for numerics
  static const Int_t fgkPmax6 = AliFlowCumuConstants::kPmax6;   //needed for numerics  
  static const Int_t fgkQmax8 = AliFlowCumuConstants::kQmax8;   //needed for numerics
  static const Int_t fgkPmax8 = AliFlowCumuConstants::kPmax8;   //needed for numerics   
  static const Int_t fgkQmax16 = AliFlowCumuConstants::kQmax16; //needed for numerics
  static const Int_t fgkPmax16 = AliFlowCumuConstants::kPmax16; //needed for numerics 
  static const Int_t fgkFlow = AliFlowCumuConstants::kFlow;     //integrated flow coefficient to be calculated
  static const Int_t fgkMltpl = AliFlowCumuConstants::kMltpl;   //the multiple in p=m*n (diff. flow) 
  TList* fHistList;                                             //list to hold all output histograms

  Double_t fR0;       //needed for numerics
  Double_t fPtMax;    //maximum pt
  Double_t fPtMin;    //minimum pt
  Double_t fBinWidth; //width of pt bin (in GeV)
  Int_t fgknBins;     //number of pt bins
      
  Double_t fAvQx;  //<Q_x>
  Double_t fAvQy;  //<Q_y>
  Double_t fAvQ2x; //<(Q_x)^2>
  Double_t fAvQ2y; //<(Q_y)^2>

  TProfile*          fAvMultIntFlowGFC; //average selected multiplicity
 
  TProfile*          fQVectorComponentsGFC; //averages of Q-vector components (1st bin: <Q_x>, 2nd bin: <Q_y>, 3rd bin: <(Q_x)^2>, 4th bin: <(Q_y)^2>)
    
  TH1D*              fIntFlowResultsGFC; //integrated flow final results
  
  TH1D*              fDiffFlowResults2ndOrderGFC; //differential flow final results (2nd order estimate) 
  TH1D*              fDiffFlowResults4thOrderGFC; //differential flow final results (4th order estimate)
  TH1D*              fDiffFlowResults6thOrderGFC; //differential flow final results (6th order estimate)
  TH1D*              fDiffFlowResults8thOrderGFC; //differential flow final results (8th order estimate)
  
  AliFlowCommonHistResults*  fCommonHistsResults2nd; //final results for 2nd order int. and diff. flow stored in the common histograms 
  AliFlowCommonHistResults*  fCommonHistsResults4th; //final results for 4th order int. and diff. flow stored in the common histograms 
  AliFlowCommonHistResults*  fCommonHistsResults6th; //final results for 6th order int. and diff. flow stored in the common histograms
  AliFlowCommonHistResults*  fCommonHistsResults8th; //final results for 8th order int. and diff. flow stored in the common histograms
  
  TProfile2D*        fIntFlowGenFun;   //avarage of the generating function for integrated flow 
  
  TProfile2D*        fIntFlowGenFun4;  //avarage of the generating function for integrated flow (only for different system of Eq.)
  TProfile2D*        fIntFlowGenFun6;  //avarage of the generating function for integrated flow (only for different system of Eq.)
  TProfile2D*        fIntFlowGenFun8;  //avarage of the generating function for integrated flow (only for different system of Eq.)
  TProfile2D*        fIntFlowGenFun16; //avarage of the generating function for integrated flow (only for different system of Eq.)

  TProfile*          fAvMultIntFlow4GFC;  //average selected multiplicity (only for different system of Eq.)
  TProfile*          fAvMultIntFlow6GFC;  //average selected multiplicity (only for different system of Eq.) 
  TProfile*          fAvMultIntFlow8GFC;  //average selected multiplicity (only for different system of Eq.)
  TProfile*          fAvMultIntFlow16GFC; //average selected multiplicity (only for different system of Eq.)
                   
  TProfile3D*        fDiffFlowGenFunRe; //avarage of the generating function for differential flow (real part)
  TProfile3D*        fDiffFlowGenFunIm; //avarage of the generating function for differential flow (imaginary part)
  TProfile*          fBinNoOfParticles; //number of particles per pt bin
  
  /*
  TProfile2D *fDiffFlowGenFunRe0,*fDiffFlowGenFunRe1,*fDiffFlowGenFunRe2,*fDiffFlowGenFunRe3;//differential flow 
  TProfile2D *fDiffFlowGenFunRe4,*fDiffFlowGenFunRe5,*fDiffFlowGenFunRe6,*fDiffFlowGenFunRe7;//differential flow
  TProfile2D *fDiffFlowGenFunIm0,*fDiffFlowGenFunIm1,*fDiffFlowGenFunIm2,*fDiffFlowGenFunIm3;//differential flow
  TProfile2D *fDiffFlowGenFunIm4,*fDiffFlowGenFunIm5,*fDiffFlowGenFunIm6,*fDiffFlowGenFunIm7;//differential flow
  */
 
  AliFlowCommonHist* fCommonHists;      //common control histograms
  
  Bool_t fOtherEquations; //numerical equations for cumulants solved up to different highest order 
      
  ClassDef(AliFlowAnalysisWithCumulants, 0);
};

//================================================================================================================

#endif





