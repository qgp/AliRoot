#ifndef ALICALOTRACKREADER_H
#define ALICALOTRACKREADER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id:  $ */

//_________________________________________________________________________
// Base class for reading data: MonteCarlo, ESD or AOD, of PHOS EMCAL and 
// Central Barrel Tracking detectors.
// Not all MC particles/tracks/clusters are kept, some kinematical restrictions are done.
// Mother class of : AliCaloTrackESDReader: Fills ESD data in 3 TObjArrays (PHOS, EMCAL, CTS)
//                 : AliCaloTrackMCReader: Fills Kinematics data in 3 TObjArrays (PHOS, EMCAL, CTS)
//                 : AliCaloTrackAODReader: Fills AOD data in 3 TObjArrays (PHOS, EMCAL, CTS) 
//                          
// -- Author: Gustavo Conesa (INFN-LNF)

// --- ROOT system ---
#include "TObject.h" 
class TObjArray ; 
class TLorentzVector ;
#include "TString.h"
#include "TObjArray.h"
class TArrayF;  
class TTree ;

//--- ANALYSIS system ---
class AliStack ; 
class AliHeader ; 
class AliGenEventHeader ; 
class AliVEvent;
class AliAODEvent;
class AliMCEvent;
class AliFiducialCut;
class AliAODMCHeader;
#include "AliPHOSGeoUtils.h"
#include "AliEMCALGeoUtils.h"

class AliCaloTrackReader : public TObject {

 public: 
  
  AliCaloTrackReader() ; // ctor
  AliCaloTrackReader(const AliCaloTrackReader & g) ; // cpy ctor
  AliCaloTrackReader & operator = (const AliCaloTrackReader & g) ;//cpy assignment
  virtual ~AliCaloTrackReader() ;//virtual dtor

  enum inputDataType {kESD, kAOD, kMC};
  
  //Select generated events, depending on comparison of pT hard and jets.
  virtual Bool_t ComparePtHardAndJetPt() ;
  virtual Bool_t IsPtHardAndJetPtComparisonSet() const {return  fComparePtHardAndJetPt ;}
  virtual void SetPtHardAndJetPtComparison(Bool_t compare) { fComparePtHardAndJetPt = compare ;}	
  virtual Float_t GetPtHardAndJetFactor() const {return  fPtHardAndJetPtFactor ;}
  virtual void SetPtHardAndJetPtFactor(Float_t factor) { fPtHardAndJetPtFactor = factor ;}		
	
  virtual void InitParameters();
  virtual void Print(const Option_t * opt) const;

  virtual Int_t GetDebug()         const { return fDebug ; }
  virtual void  SetDebug(Int_t d)        { fDebug = d ; }
  virtual Int_t GetDataType()      const { return fDataType ; }
  virtual void  SetDataType(Int_t data ) { fDataType = data ; }

  virtual Int_t   GetEventNumber()     const {return fEventNumber ; }
  virtual TString GetCurrentFileName() const {return fCurrentFileName ; }
	
  //Minimum pt setters and getters 
  virtual Float_t  GetEMCALPtMin() const { return fEMCALPtMin  ; }
  virtual Float_t  GetPHOSPtMin()  const { return fPHOSPtMin  ; }
  virtual Float_t  GetCTSPtMin()   const { return fCTSPtMin  ; }

  virtual void SetEMCALPtMin(Float_t  pt) { fEMCALPtMin = pt ; }
  virtual void SetPHOSPtMin(Float_t  pt)  { fPHOSPtMin = pt ; }
  virtual void SetCTSPtMin(Float_t  pt)   { fCTSPtMin = pt ; }
  
  //Input setters and getters

  Bool_t IsCTSSwitchedOn()  const { return fFillCTS ; }
  void SwitchOnCTS()    {fFillCTS = kTRUE ; }
  void SwitchOffCTS()   {fFillCTS = kFALSE ; }

  Bool_t IsEMCALSwitchedOn() const { return fFillEMCAL ; }
  void SwitchOnEMCAL()  {fFillEMCAL = kTRUE ; }
  void SwitchOffEMCAL() {fFillEMCAL = kFALSE ; }

  Bool_t IsPHOSSwitchedOn()  const { return fFillPHOS ; }
  void SwitchOnPHOS()   {fFillPHOS = kTRUE ; }
  void SwitchOffPHOS()  {fFillPHOS = kFALSE ; }

  Bool_t IsEMCALCellsSwitchedOn() const { return fFillEMCALCells ; }
  void SwitchOnEMCALCells()  {fFillEMCALCells = kTRUE ; }
  void SwitchOffEMCALCells() {fFillEMCALCells = kFALSE ; }

  Bool_t IsPHOSCellsSwitchedOn()  const { return fFillPHOSCells ; }
  void SwitchOnPHOSCells()   {fFillPHOSCells = kTRUE ; }
  void SwitchOffPHOSCells()  {fFillPHOSCells = kFALSE ; }

  virtual Bool_t FillInputEvent(const Int_t iEntry, const char *currentFileName)  ;
  virtual void FillInputCTS()   {;}
  virtual void FillInputEMCAL() {;}
  virtual void FillInputPHOS()  {;}
  virtual void FillInputEMCALCells() {;}
  virtual void FillInputPHOSCells()  {;}

  virtual TObjArray* GetAODCTS()   const {return fAODCTS ;}
  virtual TObjArray* GetAODEMCAL() const {return fAODEMCAL ;}
  virtual TObjArray* GetAODPHOS()  const {return fAODPHOS ;}
  virtual TNamed* GetEMCALCells()  const {return fEMCALCells ;}
  virtual TNamed* GetPHOSCells()   const {return fPHOSCells ;}

  //Get MC  informatio
  //Kinematics and galice.root available 
  virtual AliStack*    GetStack()      const ;
  virtual AliHeader*   GetHeader()     const ;
  virtual AliGenEventHeader* GetGenEventHeader() const ;
  //Filtered kinematics in AOD	
  virtual TClonesArray*   GetAODMCParticles(Int_t input = 0) const ;
  virtual AliAODMCHeader* GetAODMCHeader(Int_t input = 0)    const ;
	
  virtual AliVEvent*   GetInputEvent()  const {return fInputEvent;}
  virtual AliAODEvent* GetOutputEvent() const {return fOutputEvent;}
  virtual AliMCEvent*  GetMC()          const {return fMC;}
  virtual void         GetVertex(Double_t *) const {;}
  virtual void         GetSecondInputAODVertex(Double_t *) const {;}
  virtual Double_t     GetBField() const { return 0.;}
	
  virtual void Init();
	
  virtual void SetInputEvent(AliVEvent* const input)  {fInputEvent  = input;}
  virtual void SetOutputEvent(AliAODEvent* const aod) {fOutputEvent = aod;}
  virtual void SetMC(AliMCEvent* const mc)            {fMC  = mc;}

  virtual void ResetLists();

  virtual AliFiducialCut * GetFiducialCut() const {return  fFiducialCut ;}
  virtual void SetFiducialCut(AliFiducialCut * const fc) { fFiducialCut = fc ;}
	
  virtual void SetInputOutputMCEvent(AliVEvent* /*esd*/, AliAODEvent* /*aod*/, AliMCEvent* /*mc*/) {;}
	
  //Methods for mixing with external input file (AOD)
  virtual TTree* GetSecondInputAODTree() const {return  fSecondInputAODTree ; } 
  //virtual void SetSecondInputAODTree(TTree * tree) {fSecondInputAODTree = tree ;
  //												  fSecondInputAODEvent->ReadFromTree(tree);}//Connect tree and AOD event.
					
  virtual AliAODEvent* GetSecondInputAODEvent() const { return fSecondInputAODEvent ; } 
	
  TString GetSecondInputFileName() const    {return fSecondInputFileName ; }
  void SetSecondInputFileName(TString name) { fSecondInputFileName = name ; }

  Int_t GetSecondInputFirstEvent() const    {return fSecondInputFirstEvent ; }
  void SetSecondInputFirstEvent(Int_t iEvent0) { fSecondInputFirstEvent = iEvent0 ; }	
	
  Int_t GetAODCTSNormalInputEntries()   {if(!fSecondInputAODTree) { fAODCTSNormalInputEntries   = fAODCTS->GetEntriesFast()  ;}
										 return fAODCTSNormalInputEntries ; }
  Int_t GetAODEMCALNormalInputEntries() {if(!fSecondInputAODTree) { fAODEMCALNormalInputEntries = fAODEMCAL->GetEntriesFast();}
										 return fAODEMCALNormalInputEntries ; }
  Int_t GetAODPHOSNormalInputEntries()  {if(!fSecondInputAODTree) { fAODPHOSNormalInputEntries  = fAODPHOS->GetEntriesFast() ;}
										 return fAODPHOSNormalInputEntries ; }
	
  ULong_t GetTrackStatus() const    {return fTrackStatus ; }
  void SetTrackStatus(ULong_t bit) { fTrackStatus = bit ; }		
	
  void SwitchOnStack()              { fReadStack          = kTRUE  ; }
  void SwitchOffStack()             { fReadStack          = kFALSE ; }
  void SwitchOnAODMCParticles()     { fReadAODMCParticles = kTRUE  ; }
  void SwitchOffAODMCParticles()    { fReadAODMCParticles = kFALSE ; }
  Bool_t ReadStack()          const { return fReadStack            ; }
  Bool_t ReadAODMCParticles() const { return fReadAODMCParticles   ; }
	
  void SwitchOnCleanStdAOD()  {fCleanOutputStdAOD = kTRUE;}
  void SwitchOffCleanStdAOD() {fCleanOutputStdAOD = kFALSE;}
	
  void SetDeltaAODFileName(TString name ) {fDeltaAODFileName = name ; }
  TString GetDeltaAODFileName() const {return fDeltaAODFileName ; }

  void SetFiredTriggerClassName(TString name ) {fFiredTriggerClassName = name ; }
  TString GetFiredTriggerClassName() const {return fFiredTriggerClassName ; }
  virtual TString GetFiredTriggerClasses() {return "";}
	
  void SetEMCALGeometryName(TString name)   { fEMCALGeoName = name ; }
  TString EMCALGeometryName() const { return fEMCALGeoName ; }
	void InitEMCALGeometry() {if (!fEMCALGeo) fEMCALGeo = new AliEMCALGeoUtils(fEMCALGeoName); }
  AliEMCALGeoUtils * GetEMCALGeometry() const {return fEMCALGeo;}
	
  void SetPHOSGeometryName(TString name)   { fPHOSGeoName = name ; }
  TString PHOSGeometryName() const { return fPHOSGeoName ; }
  void InitPHOSGeometry() {if (!fPHOSGeo) fPHOSGeo = new AliPHOSGeoUtils(fPHOSGeoName); }
  AliPHOSGeoUtils * GetPHOSGeometry() const {return fPHOSGeo;}
	
 protected:
  Int_t	           fEventNumber; // Event number
  TString          fCurrentFileName; // Current file name under analysis
  Int_t            fDataType ;   // Select MC:Kinematics, Data:ESD/AOD, MCData:Both
  Int_t            fDebug;       // Debugging level
  AliFiducialCut * fFiducialCut; // Acceptance cuts
	
  Bool_t           fComparePtHardAndJetPt;  // In MonteCarlo, jet events, reject fake events with wrong jet energy.
  Float_t          fPtHardAndJetPtFactor;   // Factor between ptHard and jet pT to reject/accept event.

  Float_t        fCTSPtMin;      // pT Threshold on charged particles 
  Float_t        fEMCALPtMin;    // pT Threshold on emcal clusters
  Float_t        fPHOSPtMin;     // pT Threshold on phos clusters

  TObjArray *    fAODCTS ;        //! temporal referenced array with tracks
  TObjArray *    fAODEMCAL ;      //! temporal referenced array with EMCAL CaloClusters
  TObjArray *    fAODPHOS ;       //! temporal referenced array with PHOS CaloClusters
  TNamed *       fEMCALCells ;    //! temporal array with EMCAL CaloCells, ESD or AOD
  TNamed *       fPHOSCells ;     //! temporal array with PHOS CaloCells, ESD or AOD

  AliVEvent   *  fInputEvent;     //! pointer to esd or aod input
  AliAODEvent *  fOutputEvent;    //! pointer to aod output
  AliMCEvent  *  fMC;             //! Monte Carlo Event Handler  

  Bool_t         fFillCTS;        // use data from CTS
  Bool_t         fFillEMCAL;      // use data from EMCAL
  Bool_t         fFillPHOS;       // use data from PHOS
  Bool_t         fFillEMCALCells; // use data from EMCAL
  Bool_t         fFillPHOSCells;  // use data from PHOS

  TTree *        fSecondInputAODTree;    // Tree with second input AOD, for mixing analysis.	
  AliAODEvent*   fSecondInputAODEvent;   //! pointer to second input AOD event.
  TString        fSecondInputFileName;   // File with AOD data to mix with normal stream of data.
  Int_t          fSecondInputFirstEvent; // First event to be considered in the mixing.
	
  Int_t          fAODCTSNormalInputEntries;   // Number of entries in CTS   in case of standard input, larger with mixing.
  Int_t          fAODEMCALNormalInputEntries; // Number of entries in EMCAL in case of standard input, larger with mixing.
  Int_t          fAODPHOSNormalInputEntries;  // Number of entries in PHOS  in case of standard input, larger with mixing.
	
  ULong_t        fTrackStatus        ; // Track selection bit, select tracks refitted in TPC, ITS ...
  Bool_t         fReadStack          ; // Access kine information from stack
  Bool_t	     fReadAODMCParticles ; // Access kine information from filtered AOD MC particles
	
  Bool_t	     fCleanOutputStdAOD;   // clean the written standard tracks and caloclusters in output AOD
  TString        fDeltaAODFileName ;   // Delta AOD file name
  TString        fFiredTriggerClassName  ;  // Name of trigger event type used to do the analysis

  TString        fEMCALGeoName;  // Name of geometry to use for EMCAL.
  TString        fPHOSGeoName;   // Name of geometry to use for PHOS.	
  AliEMCALGeoUtils * fEMCALGeo ; //! EMCAL geometry pointer
  AliPHOSGeoUtils  * fPHOSGeo  ; //! PHOS geometry pointer  

  ClassDef(AliCaloTrackReader,10)
} ;


#endif //ALICALOTRACKREADER_H



