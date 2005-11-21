#ifndef ALIITSSIMULATIONSPDDUBNA_H
#define ALIITSSIMULATIONSPDDUBNA_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include "TH1F.h"
#include "TObjArray.h"
#include "AliITSsimulation.h"
#include "AliITSresponseSPD.h"
#include "AliITSsegmentationSPD.h"

class AliITSmodule;

//-------------------------------------------------------------------

class AliITSsimulationSPDdubna : public AliITSsimulation {
 public:
    AliITSsimulationSPDdubna();
    //    AliITSsimulationSPDdubna(AliITSsegmentation *seg,AliITSresponse *res,
    //                         Int_t cup=1);
    AliITSsimulationSPDdubna(AliITSDetTypeSim *dettyp,Int_t cup=1);
    virtual ~AliITSsimulationSPDdubna();
    // copy constructor
    AliITSsimulationSPDdubna(const AliITSsimulationSPDdubna &source); 
     // ass. operator
    AliITSsimulationSPDdubna& operator=(const AliITSsimulationSPDdubna &s);
    virtual AliITSsimulation& operator=(const AliITSsimulation &source);
    // Initilizes the variables
    void Init();

    // General User calling routines
    // Initilize simulation for a specific event and module
    void InitSimulationModule(Int_t module, Int_t event);
    // Finish and write S Digitization
    void FinishSDigitiseModule();
    // From hits to Digits, without creating SDigits
    void DigitiseModule(AliITSmodule *mod,Int_t,Int_t);

    // More or less Internal Routines
    // Create S Digits from specific module
    void SDigitiseModule(AliITSmodule *mod, Int_t mask, Int_t event);
    // Write S Digits to the tree of SDigits.
    void WriteSDigits();
    // fill pList from hits, charge sharing, diffusion, coupling
    void HitToSDigit(AliITSmodule *mod);
    // Take pList of signals and apply noise... create Digis
    void pListToDigits();
    //
    void CreateHistograms();
    void FillHistograms(Int_t ix,Int_t iz,Double_t v=1.0);
    void ResetHistograms();
    TH1F* GetHistogram(Int_t i){return (TH1F*)(fHis->At(i));}// get histogram
    TObjArray*  GetHistArray() {return fHis;}// get hist array
    TString& GetHistName(){return fSPDname;}
    void SetHistName(TString &n){fSPDname = n;}
    //
    // For backwards compatibility
    void SDigitsToDigits(){ FinishSDigitiseModule();};
    void HitToDigit(AliITSmodule *mod){
        // Standard interface to DigitiseModule
        // Inputs:
        //    AliITSmodule *mod  Pointer to this module
        // Outputs:
        //    none.
        // Return:
        //    none.
        DigitiseModule(mod,GetModuleNumber(),0);};

 private:
    void SpreadCharge(Double_t x0,Double_t z0,Int_t ix0,Int_t iz0,
		      Double_t el,Double_t sig,Int_t t,Int_t hi);
    void UpdateMapSignal(Int_t ix,Int_t iz,Int_t trk,Int_t ht,Double_t signal){
        //  This function adds a signal to the pList from the pList class
        //  Inputs:
        //    Int_t       iz     // row number
        //    Int_t       ix     // column number
        //    Int_t       trk    // track number
        //    Int_t       ht     // hit number
        //    Double_t    signal // signal strength
        //  Outputs:
        //    none.
        //  Return:
        //    none
        GetMap()->AddSignal(iz,ix,trk,ht,GetModuleNumber(),signal);};
    void UpdateMapNoise(Int_t ix,Int_t iz,Float_t noise){
        //  This function adds noise to data in the MapA2 as well as the pList
        //  Inputs:
        //    Int_t       iz       // row number
        //    Int_t       ix       // column number
        //    Double_t    ns    // electronic noise generated by pListToDigits
        //  Outputs:
        //    none.
        //  Return:
        //    none
        GetMap()->AddNoise(iz,ix,GetModuleNumber(),noise);}
    // Get a pointer to the segmentation object
    virtual AliITSsegmentation* GetSegmentationModel(Int_t /*dt*/){return fDetType->GetSegmentationModel(0);}
    // set pointer to segmentation objec
    virtual void SetSegmentationModel(Int_t /*dt*/, AliITSsegmentation *seg){fDetType->SetSegmentationModel(0,seg);}
    // Bari-Salerno Coupling parameters
    // "New" coupling routine  Tiziano Virgili
    void SetCoupling(Int_t row,Int_t col,Int_t ntrack,Int_t idhit);
    // "Old" coupling routine  Rocco Caliandro
    void SetCouplingOld(Int_t row, Int_t col,Int_t ntrack,Int_t idhit);
    // Getters for data kept in fSegmentation and fResponse.
    // Returns the Threshold in electrons
    Double_t GetThreshold(Int_t ix,Int_t iz){
      Double_t th,sig;AliITSresponseSPD* res=(AliITSresponseSPD*)GetResponseModel(0); 
	res->Thresholds(th,sig);return th;};
    // Returns the couplings Columb and Row.
    void GetCouplings(Double_t &cc,Double_t &cr){
      AliITSresponseSPD* res = (AliITSresponseSPD*)GetResponseModel(0);
      res->GetCouplingParam(cc,cr);};
    // Returns the number of pixels in x
    Int_t GetNPixelsX(){return GetSegmentationModel(0)->Npx();};
    // Returns the number of pixels in z
    Int_t GetNPixelsZ(){return GetSegmentationModel(0)->Npz();};

    TObjArray    *fHis;          //! just in case for histogramming
    TString       fSPDname;      //! Histogram name
    Int_t         fCoupling;     // Sets the coupling to be used.
                                 // ==1 use SetCoupling, ==2 use SetCouplingOld
                                 // with diffusion turned off (by constructor)
                                 // ==3 use SetCoupling, ==4 use SetCouplingOld
                                 // with diffusion, else no coupling.
    ClassDef(AliITSsimulationSPDdubna,3)  // Simulation of SPD clusters
};
#endif 
