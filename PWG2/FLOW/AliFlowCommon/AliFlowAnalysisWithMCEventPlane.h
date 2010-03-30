/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */


#ifndef ALIFLOWANALYSISWITHMCEVENTPLANE_H
#define ALIFLOWANALYSISWITHMCEVENTPLANE_H

#include "TVector2.h"          //called explicitly
#include "AliFlowVector.h"
#include "TString.h"

class AliFlowTrackSimple;
class AliFlowEventSimple;
class AliFlowCommonHist;
class AliFlowCommonHistResults;

class TH1F;
class TH1D;
class TProfile;
class TProfile2D;
class TObjArray;
class TFile;
class TList;
class TComplex;
class Riostream;


// Description: Maker to analyze Flow from the generated MC reaction plane.
//              This class is used to get the real value of the flow 
//              to compare the other methods to when analysing simulated events.

 
class AliFlowAnalysisWithMCEventPlane {

 public:
 
   AliFlowAnalysisWithMCEventPlane();            //default constructor
   virtual  ~AliFlowAnalysisWithMCEventPlane();  //destructor
 
   void      WriteHistograms(TString* outputFileName);
   void      WriteHistograms(TString outputFileName);
   void      WriteHistograms(TDirectoryFile *outputFileName);
   void      Init();                                       //defines variables and histograms
   void      Make(AliFlowEventSimple* anEvent);            //calculates variables and fills histograms
   void      GetOutputHistograms(TList *outputListHistos); //get pointers to all output histograms (called before Finish()) 
   void      Finish();                                     //saves histograms
   
   void      SetDebug(Bool_t kt)          { this->fDebug = kt ; }
   Bool_t    GetDebug() const             { return this->fDebug ; }

   void      SetEventNumber(Int_t n)      { this->fEventNumber = n; }
   Int_t     GetEventNumber() const       { return this->fEventNumber; }

   // Output 
   TList*    GetHistList() const          { return this->fHistList ; }  
   AliFlowCommonHist* GetCommonHists() const  { return this->fCommonHists; }
   void      SetCommonHists(AliFlowCommonHist* aCommonHist)  
     { this->fCommonHists = aCommonHist; }
   AliFlowCommonHistResults*  GetCommonHistsRes() const { return this->fCommonHistsRes; }
   void      SetCommonHistsRes(AliFlowCommonHistResults* aCommonHistResult) 
     { this->fCommonHistsRes = aCommonHistResult; }
   
   //histograms
   TH1F*     GetHistRP()                  {return this->fHistRP; } 
   void      SetHistRP(TH1F* aHistRP)     {this->fHistRP = aHistRP; }
   
   TProfile* GetHistProIntFlow()          {return this->fHistProIntFlow; } 
   void      SetHistProIntFlow(TProfile* aHistProIntFlow) 
     {this->fHistProIntFlow = aHistProIntFlow; }
     
   TProfile2D* GetHistProDiffFlowPtEtaRP()     {return this->fHistProDiffFlowPtEtaRP; } 
   void      SetHistProDiffFlowPtEtaRP(TProfile2D* aHistProDiffFlowPtEtaRP) 
     {this->fHistProDiffFlowPtEtaRP = aHistProDiffFlowPtEtaRP; }   
   
   TProfile* GetHistProDiffFlowPtRP()     {return this->fHistProDiffFlowPtRP; } 
   void      SetHistProDiffFlowPtRP(TProfile* aHistProDiffFlowPtRP) 
     {this->fHistProDiffFlowPtRP = aHistProDiffFlowPtRP; } 
   
   TProfile* GetHistProDiffFlowEtaRP()    {return this->fHistProDiffFlowEtaRP; } 
   void      SetHistProDiffFlowEtaRP(TProfile* aHistProDiffFlowEtaRP) 
     {this->fHistProDiffFlowEtaRP = aHistProDiffFlowEtaRP; } 
     
   TProfile2D* GetHistProDiffFlowPtEtaPOI()     {return this->fHistProDiffFlowPtEtaPOI; } 
   void      SetHistProDiffFlowPtEtaPOI(TProfile2D* aHistProDiffFlowPtEtaPOI) 
     {this->fHistProDiffFlowPtEtaPOI = aHistProDiffFlowPtEtaPOI; }   
   
   TProfile* GetHistProDiffFlowPtPOI()    {return this->fHistProDiffFlowPtPOI; } 
   void      SetHistProDiffFlowPtPOI(TProfile* aHistProDiffFlowPtPOI) 
     {this->fHistProDiffFlowPtPOI = aHistProDiffFlowPtPOI; } 
   
   TProfile* GetHistProDiffFlowEtaPOI()   {return this->fHistProDiffFlowEtaPOI; } 
   void      SetHistProDiffFlowEtaPOI(TProfile* aHistProDiffFlowEtaPOI) 
     {this->fHistProDiffFlowEtaPOI = aHistProDiffFlowEtaPOI; } 
     
   TH1D* GetHistSpreadOfFlow()   {return this->fHistSpreadOfFlow; } 
   void      SetHistSpreadOfFlow(TH1D* aHistSpreadOfFlow) 
     {this->fHistSpreadOfFlow = aHistSpreadOfFlow; }    
   
   // harmonic:
   void SetHarmonic(Int_t const harmonic) {this->fHarmonic = harmonic;};
   Int_t GetHarmonic() const {return this->fHarmonic;};
 
 private:
 
   AliFlowAnalysisWithMCEventPlane(const AliFlowAnalysisWithMCEventPlane& aAnalysis);             //copy constructor
   AliFlowAnalysisWithMCEventPlane& operator=(const AliFlowAnalysisWithMCEventPlane& aAnalysis);  //assignment operator 

      
#ifndef __CINT__
   TVector2*    fQsum;              // flow vector sum
   Double_t     fQ2sum;             // flow vector sum squared
#endif /*__CINT__*/

   Int_t        fEventNumber;       // event counter
   Bool_t       fDebug ;            //! flag for lyz analysis: more print statements

   TList*       fHistList;          //list to hold all output histograms  
    
   AliFlowCommonHist* fCommonHists;              //
   AliFlowCommonHistResults* fCommonHistsRes;    //
   
   TH1F*        fHistRP;                  // reaction plane
   TProfile*    fHistProIntFlow;          // profile used to calculate the integrated flow of RP particles
   TProfile2D*  fHistProDiffFlowPtEtaRP;  // profile used to calculate the differential flow (Pt,Eta) of RP particles
   TProfile*    fHistProDiffFlowPtRP;     // profile used to calculate the differential flow (Pt) of RP particles 
   TProfile*    fHistProDiffFlowEtaRP;    // profile used to calculate the differential flow (Eta) of RP particles 
   TProfile2D*  fHistProDiffFlowPtEtaPOI; // profile used to calculate the differential flow (Pt,Eta) of POI particles
   TProfile*    fHistProDiffFlowPtPOI;    // profile used to calculate the differential flow (Pt) of POI particles 
   TProfile*    fHistProDiffFlowEtaPOI;   // profile used to calculate the differential flow (Eta) of POI particles
   TH1D*        fHistSpreadOfFlow;        // histogram filled with NONAME integrated flow calculated e-b-e    
   Int_t        fHarmonic;                // harmonic 
    
   ClassDef(AliFlowAnalysisWithMCEventPlane,1)  // Analyse particle distribution versus MC reaction plane
     };

     
#endif


