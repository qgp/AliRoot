#ifndef ALIEMCALJETFINDER_H
#define ALIEMCALJETFINDER_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */

/* $Id$ */
//*-- Author:
//*-- Andreas Morsch (CERN)

#include <TTask.h>
#include "AliEMCALJet.h"

class TClonesArray;
class TH2F;
class TH1F;
class TCanvas;
class TList;
class AliEMCALHadronCorrection;

class AliEMCALJetFinder : public TTask {
 public:
    AliEMCALJetFinder();
    AliEMCALJetFinder(const char* name, const char *title);
    virtual ~AliEMCALJetFinder();
    virtual void  Init();
    virtual void  Find(Int_t ncell, Int_t ncell_tot, Float_t etc[30000], 
		      Float_t etac[30000], Float_t phic[30000],
		      Float_t min_move, Float_t max_move, Int_t mode,
		      Float_t prec_bg, Int_t ierror);
    virtual void  Find();
    virtual void  FindTracksInJetCone();
    virtual void  Test();
    virtual void  BuildTrackFlagTable();
    virtual Int_t SetTrackFlag(Float_t radius, Int_t pdgCode, Double_t charge);
    // Geometry
    virtual void SetCellSize(Float_t eta, Float_t phi);
    // Parameters
    virtual void SetDebug(Int_t flag = 0) {fDebug = flag;}
    virtual void SetConeRadius(Float_t par);
    virtual void SetEtSeed(Float_t par);
    virtual void SetMinJetEt(Float_t par);
    virtual void SetMinCellEt(Float_t par);
    virtual void SetPtCut(Float_t par = 1.);
    virtual void SetMomentumSmearing(Bool_t flag = kFALSE) {fSmear = flag;}
    virtual void SetEfficiencySim(Bool_t flag = kFALSE)    {fEffic = flag;}
    virtual void SetSamplingFraction(Float_t par = 12.9) {fSamplingF = par;}
    virtual void SetIncludeK0andN(Bool_t flag = kFALSE) {fK0N = flag;}
    // Correction of hadronic energy
    virtual void SetHadronCorrector(AliEMCALHadronCorrection* corr)
	{fHadronCorrector = corr;}
    virtual void SetHadronCorrection(Int_t flag = 1) {fHCorrection = flag;}
    // PAI
    void SetWriteKey(Bool_t flag = kFALSE) {fWrite = flag;}
    void SetMode(Int_t mode = 0) {fMode = mode;}
    void SetMinMove(Float_t minMove = 0.05) {fMinMove = minMove;}
    void SetMaxMove(Float_t maxMove = 0.15) {fMaxMove = maxMove;}
    void SetPrecBg (Float_t precBg = 0.035) {fPrecBg = precBg;}
    void SetParametersForBgSubtraction
    (Int_t mode=0, Float_t minMove=0.05, Float_t maxMove=0.15, Float_t precBg=0.035);
    //    virtual void Print(Option_t* option="") const;    // *MENU*

    Bool_t GetWriteKey() {return fWrite;}
  //AliEMCALJet* GetJetT() {return fJetT[0];}
    AliEMCALJet* GetJetT(Int_t n = 0) {return fJetT[n];}
    virtual void DrawHistsForTuning(Int_t mode=0);           // *MENU*
    virtual void PrintParameters(Int_t mode=0);              // *MENU*
    virtual const Char_t* GetFileNameForParameters(Char_t* dir="RES/");

    // Access to Results
    virtual Int_t   Njets();
    virtual Float_t JetEnergy(Int_t);
    virtual Float_t JetPhiL(Int_t);
    virtual Float_t JetPhiW(Int_t);
    virtual Float_t JetEtaL(Int_t);  
    virtual Float_t JetEtaW(Int_t);
    TH2F*   GetLego()  {return fLego;}
    TH2F*   GetLegoB() {return fLegoB;}
    TH2F*   GetLegoEMCAL() {return fhLegoEMCAL;}
    TH2F*   GethEff() {return fhEff;}
    TH1F*   GetCellEt() {return fhCellEt;}
    TH1F*   GetCellEMCALEt() {return fhCellEMCALEt;}
    TH1F*   GetTrackPt() {return fhTrackPt;}
    TH1F*   GetTrackPtBcut() {return fhTrackPtBcut;}
    TList*  GetHistsList() {return fHistsList;}
    Int_t   GetNChTpc() {return fNChTpc;}
    void    DrawLego(Char_t *opt="lego");         // *MENU*
    void    DrawLegoEMCAL(Char_t *opt="lego");    // *MENU*
    void    DrawLegos();                          // *MENU*
    Bool_t  IsThisPartonsOrDiQuark(Int_t pdg);
    TString &GetPythiaParticleName(Int_t kf);     
    // I/O
    virtual void SetOutputFileName(char* name) {fOutFileName = name;}
    virtual void FillFromHits(Int_t flag = 0);
    virtual void FillFromHitFlaggedTracks(Int_t flag = 0);
    virtual void FillFromDigits(Int_t flag = 0);
    virtual void FillFromTracks(Int_t flag = 0, Int_t ich = 0);
    virtual void FillFromParticles();
    virtual void FillFromPartons();

    virtual void SaveBackgroundEvent();
    virtual void InitFromBackground();
    virtual void AddJet(const AliEMCALJet& jet);
    virtual void WriteJets();
    virtual void ResetJets();
    virtual TClonesArray* Jets() {return fJets;}
 private:
    virtual void BookLego();
    virtual void DumpLego();
    virtual void ResetMap();
    virtual Float_t PropagatePhi(Float_t pt, Float_t charge, Bool_t& curls);
    virtual void RearrangeParticlesMemory(Int_t npart);
 protected:
    Bool_t                         fWrite;           // Key for writing
    TClonesArray*                  fJets;            //! List of Jets
    TH2F*                          fLego;            //! Lego Histo
    TH2F*                          fLegoB;           //! Lego Histo Backg
    TH2F*                          fhLegoTracks;     //! Lego for Tracks
    TH2F*                          fhLegoEMCAL;      //! Lego for EMCAL itself
    TH2F*                          fhLegoHadrCorr;   //! Lego for hadron correction
    TH2F*                          fhEff;            //! Hist. for controling eff.
    TH1F*                          fhCellEt;         //! Et distr. for cells from fLego
    TH1F*                          fhCellEMCALEt;    //! Et distr. for cells from fLegoEMCAL
    TH1F*                          fhTrackPt;        //! Pt distr. for charge particles
    TH1F*                          fhTrackPtBcut;    //! Pt distr. for charge particles + cut due to magnetic field
    TH1F*                          fhChPartMultInTpc;//! Ch. part. multiplicity in TPC acceptance
    TCanvas*                       fC1;              //! first canvas for drawing
    TList*                         fHistsList;       //! List of hists - 4-mar-2002
    AliEMCALJet*                   fJetT[10];        //! Jet temporary storage
    AliEMCALHadronCorrection*      fHadronCorrector; //! Pointer to hadronic correction
    Int_t                          fHCorrection;     //  Hadron correction flag
    Int_t                          fDebug;           //! Debug flag
    Int_t                          fBackground;      //! Background flag
    Float_t                        fConeRadius;      //  Cone radius
    Float_t                        fPtCut;           //  Pt cut on charged tracks
    Float_t                        fEtSeed;          //  Min. Et for seed
    Float_t                        fMinJetEt;        //  Min Et of jet
    Float_t                        fMinCellEt;       //  Min Et in one cell
    Float_t                        fSamplingF;       //  Sampling Fraction
    Bool_t                         fSmear;           //  Flag for momentum smearing
    Bool_t                         fEffic;           //  Flag for efficiency simulation
    Bool_t                         fK0N;             //  Flag for efficiency simulation
    Int_t                          fNjets;           //! Number of Jetsp
    Float_t                        fDeta;            //! eta cell size 
    Float_t                        fDphi;            //! phi cell size
    Int_t                          fNcell;           //! number of cells
    Int_t                          fNtot;            //! total number of cells
    Int_t                          fNbinEta;         //! number of cells in eta
    Int_t                          fNbinPhi;         //! number of cells in phi
    Float_t                        fEtaMin;          //! minimum eta  
    Float_t                        fEtaMax;          //! maximum eta
    Float_t                        fPhiMin;          //! minimun phi
    Float_t                        fPhiMax;          //! maximum phi
    Float_t                        fEtCell[30000];   //! Cell Energy
    Float_t                        fEtaCell[30000];  //! Cell eta
    Float_t                        fPhiCell[30000];  //! Cell phi
    Int_t                          fNt;              //! number of tracks
    Int_t                          fNChTpc;          //! number of ch.part in TPC

    Int_t                          fNtS;             //! number of tracks selected
    Int_t*                         fTrackList;       //! List of selected tracks
    Float_t*                       fPtT;             //! Pt   of tracks 
    Float_t*                       fEtaT;            //! Eta  of tracks
    Float_t*                       fPhiT;            //! Phi  of tracks
    Int_t*                         fPdgT;            //! PDG code of tracks
 
    Int_t                          fNtB;             //! number of tracks in Bg
    Int_t*                         fTrackListB;      //! List of selected tracks in Bg
    Float_t*                       fPtB;             //! Pt   of tracks in Bg
    Float_t*                       fEtaB;            //! Eta  of tracks in Bg
    Float_t*                       fPhiB;            //! Phi  of tracks in Bg
    Int_t*                         fPdgB;            //! PDG  of tracks in Bg

    // parameter for jet_finder_ua1
    Int_t                          fMode;            // key for BG subtraction
    Float_t                        fMinMove;         // min cone move 
    Float_t                        fMaxMove;         // max cone move
    Float_t                        fPrecBg;          // max value of change for BG (in %)
    Int_t                          fError;           // error variables 

    char*                          fOutFileName;     //! Output file name
    TFile*                         fOutFile;         //! Output file
    TFile*                         fInFile;          //! Output file
    Int_t                          fEvent;           //! Processed event

    ClassDef(AliEMCALJetFinder,2)        // JetFinder for EMCAL
}
;
#endif // ALIEMCALJetFinder_H
