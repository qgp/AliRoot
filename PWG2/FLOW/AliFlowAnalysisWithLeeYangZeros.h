/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */


#ifndef AliFlowAnalysisWithLeeYangZeros_H
#define AliFlowAnalysisWithLeeYangZeros_H

#include "AliFlowVector.h" //needed as include

//class AliFlowTrackSimple;
class AliFlowEventSimple;
class AliFlowLYZHist1; 
class AliFlowLYZHist2;
class AliFlowCommonHist;
class AliFlowCommonHistResults;

class TH1F;
class TH1D;
class TProfile;
class TProfile2D;
class TObjArray;
class TFile;
class TComplex;
class TString;
class TList;
class Riostream;


// Description: Maker to analyze Flow by the LeeYangZeros method
//              One needs to do two runs over the data; 
//              First to calculate the integrated flow 
//              and in the second to calculate the differential flow
 
class AliFlowAnalysisWithLeeYangZeros {

 public:
 
   AliFlowAnalysisWithLeeYangZeros();              //default constructor
   virtual  ~AliFlowAnalysisWithLeeYangZeros();    //destructor
 
   Bool_t    Init();                               //defines variables and histograms
   Bool_t    Make(AliFlowEventSimple* anEvent);    //calculates variables and fills histograms
   Bool_t    Finish();                             //saves histograms

   Double_t  GetQtheta(AliFlowVector aQ, Double_t aTheta);

   void      SetFirstRun(Bool_t kt)              { this->fFirstRun = kt ; }
   Bool_t    GetFirstRun() const                 { return this->fFirstRun ; }
   void      SetUseSum(Bool_t kt)                { this->fUseSum = kt ; }
   Bool_t    GetUseSum() const                   { return this->fUseSum ; }
   void      SetDoubleLoop(Bool_t kt)            { this->fDoubleLoop = kt ; }
   Bool_t    GetDoubleLoop() const               { return this->fDoubleLoop ; }
   void      SetDebug(Bool_t kt)                 { this->fDebug = kt ; }
   Bool_t    GetDebug() const                    { return this->fDebug ; }


   // Output 
   TList*   GetHistList() const                  { return this->fHistList ; }     // Gets output histogram list

   // input for second run
   void	    SetFirstRunFileName(TString name) 	{ this->firstRunFileName = name ; } // Sets input file name
   TString  GetFirstRunFileName() const		{ return this->firstRunFileName ; } // Gets output file name
   void     SetFirstRunFile(TFile* file)        { this->firstRunFile = file ; }        // Sets first run file

 private:

   AliFlowAnalysisWithLeeYangZeros(const AliFlowAnalysisWithLeeYangZeros& aAnalysis);   // copy constructor
   AliFlowAnalysisWithLeeYangZeros& operator=(const AliFlowAnalysisWithLeeYangZeros& aAnalysis); //assignment operator

   Bool_t   MakeControlHistograms(AliFlowEventSimple* anEvent); 
   Bool_t   FillFromFlowEvent(AliFlowEventSimple* anEvent);
   Bool_t   SecondFillFromFlowEvent(AliFlowEventSimple* anEvent);

   TComplex GetGrtheta(AliFlowEventSimple* anEvent, Double_t aR, Double_t aTheta);
   TComplex GetDiffFlow(AliFlowEventSimple* anEvent, Double_t aR, Int_t theta); 
   Double_t  GetR0(TH1D* fHistGtheta);

  
   
#ifndef __CINT__
   
   TVector2*  fQsum;         // flow vector sum              
   Double_t  fQ2sum;        // flow vector sum squared                 
      
#endif /*__CINT__*/

   Int_t        fEventNumber;       // event counter
        
   Bool_t       fFirstRun ;         // flag for lyz analysis: true=first run over data, false=second run 
   Bool_t       fUseSum ;           // flag for lyz analysis: true=use sum gen.function, false=use product gen.function
   Bool_t       fDoubleLoop ;       // flag for studying non flow effects
   Bool_t       fDebug ;            // flag for lyz analysis: more print statements

   TList*       fHistList;          //list to hold all output histograms  
   TString      firstRunFileName;   //
   TFile*       firstRunFile;       //
     
     
  TProfile*    fHistProVtheta;      //
  TProfile*    fHistProVeta;        //
  TProfile*    fHistProVPt;         //
  TProfile*    fHistProR0theta;     //
  TProfile*    fHistProReDenom;     //
  TProfile*    fHistProImDenom;     //
  TProfile*    fHistProReDtheta;    //
  TProfile*    fHistProImDtheta;    //
   
   
  //class AliFlowLYZHist1 defines the histograms: fHistProGtheta, fHistProReGtheta, fHistProImGtheta
  AliFlowLYZHist1* fHist1[5];       //

  //class AliFlowLYZHist1 defines the histograms: fHistProReNumer, fHistProImNumer, fHistProReNumerPt,
  //fHistProImNumerPt, fHistProReNumer2D, fHistProImNumer2D.
  AliFlowLYZHist2* fHist2[5];       //

  AliFlowCommonHist*        fCommonHists;     //
  AliFlowCommonHistResults* fCommonHistsRes;  //
 
  ClassDef(AliFlowAnalysisWithLeeYangZeros,0)  // macro for rootcint
    };
 
     
#endif

