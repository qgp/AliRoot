#ifndef ALIPHOSGETTER_H
#define ALIPHOSGETTER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//_________________________________________________________________________
//  A singleton that returns various objects 
//  Should be used on the analysis stage to avoid confusing between different
//  branches of reconstruction tree: e.g. reading RecPoints and TS made from 
//  another set of RecPoints.
// 
//  The objects are retrived from folders.  
//*-- Author: Yves Schutz (SUBATECH) & Dmitri Peressounko (RRC KI & SUBATECH)
//    


// --- ROOT system ---
#include "TObject.h"  
#include "TClonesArray.h" 
// #include "TFolder.h"  
// #include "TTree.h"
// #include "TFile.h"
// class TString ;
 class TParticle ;
// class TTask ;

// --- Standard library ---

// --- AliRoot header files ---
#include "AliConfig.h" 

// #include "AliRun.h"
class AliPHOS ; 
#include "AliPHOSHit.h"   

class AliPHOSGeometry ;
#include "AliPHOSDigitizer.h"
#include "AliPHOSSDigitizer.h" 
// //class AliPHOSCalibrationDB ;
// class AliPHOSConTableDB ;
class AliPHOSBeamTestEvent ;

#include "AliPHOSLoader.h" 

class AliPHOSGetter : public TObject {
  
 public:  
  AliPHOSGetter(){    // ctor: this is a singleton, the ctor should never be called but cint needs it as public
    Fatal("ctor", "AliPHOSGetter is a singleton default ctor not callable") ;
  } 
  AliPHOSGetter(const AliPHOSGetter & obj) {
    // cpy ctor requested by Coding Convention 
    Fatal("cpy ctor", "not implemented") ;
  } 
  
  AliPHOSGetter & operator = (const AliPHOSGetter & ) {
    // assignement operator requested by coding convention, but not needed
    Fatal("operator =", "not implemented") ;
    return *this ; 
  }
  virtual ~AliPHOSGetter() ; 
  
  //=========== Instantiators ================
  static AliPHOSGetter * Instance(const char* headerFile,
				  const char* version = AliConfig::fgkDefaultEventFolderName,
				  Option_t * openingOption = "READ" ) ; 
  static AliPHOSGetter * Instance() ; 

  static void Print() ; 
  
//   //=========== General information about run ==============
  Bool_t IsLoaded(const TString tree) const { return fLoadingStatus.Contains(tree) ; } 
  void   SetLoaded(const TString tree) { fLoadingStatus += tree ; } 

  Int_t  MaxEvent() const ; 
  Int_t  EventNumber() const ; 
  Bool_t VersionExists(TString & opt) const ; 
  UShort_t EventPattern(void) const ; 
  Float_t  BeamEnergy(void) const ;
  
//   //========== PHOSGeometry and PHOS ============= 
  AliPHOS *         PHOS() const  ;  
  AliPHOSGeometry * PHOSGeometry() const ; 
  
//   //========== Methods to read something from file ==========
  void   Event(const Int_t event, const char * opt = "HSDRP") ;    
  void   Track(const Int_t itrack) ;
  
//   //-----------------now getter's data--------------------------------------
// //  AliPHOSCalibrationDB * CalibrationDB(){return  fcdb; }
// //  void ReadCalibrationDB(const char * name, const char * filename) ;

  //=========== Primaries ============
//   TTree *           TreeK(TString filename="") ; 
  TClonesArray *    Primaries(void)  ;
  TParticle * Primary(Int_t index) const ;
  Int_t       NPrimaries()const { return fNPrimaries; }
  TParticle * Secondary(const TParticle * p, const Int_t index=1) const ;  
  
//   //=========== Hits =================
//   TTree *               TreeH(TString filename="") ; 
  TClonesArray *  Hits(void)  ; 
  AliPHOSHit *    Hit(const Int_t index) { return dynamic_cast<AliPHOSHit*>(Hits()->At(index) );}
  TTree *         TreeH() const ; 
  
  //=========== SDigits ==============
  TClonesArray *      SDigits() ;  
  AliPHOSDigit *      SDigit(const Int_t index) { return static_cast<AliPHOSDigit *>(SDigits()->At(index)) ;} 
  TTree *             TreeS() const ; 
  AliPHOSSDigitizer * SDigitizer() ;  

  TString             GetSDigitsFileName() { return PhosLoader()->GetSDigitsFileName() ; }  
  Int_t               LoadSDigits(Option_t* opt="") { return PhosLoader()->LoadSDigits(opt) ; }
  Int_t               LoadSDigitizer(Option_t* opt=""){ return  PhosLoader()->LoadSDigitizer(opt) ; }
  Int_t               WriteSDigits(Option_t* opt="") { return PhosLoader()->WriteSDigits(opt) ; }
  Int_t               WriteSDigitizer(Option_t* opt=""){
    return  PhosLoader()->WriteSDigitizer(opt) ; }
  
  //========== Digits ================
  TClonesArray * Digits() ;
  AliPHOSDigit * Digit(const Int_t index) { return static_cast<AliPHOSDigit *>(Digits()->At(index)) ;} 
  TTree *        TreeD() const ; 
  AliPHOSDigitizer * Digitizer() ;
  TString             GetDigitsFileName() { return PhosLoader()->GetDigitsFileName() ; }  
  Int_t               LoadDigits(Option_t* opt="") { return PhosLoader()->LoadDigits(opt) ; }
  Int_t               LoadDigitizer(Option_t* opt=""){
    return  PhosLoader()->LoadDigitizer(opt) ; }
  Int_t               WriteDigits(Option_t* opt="") { return PhosLoader()->WriteDigits(opt) ; }
  Int_t               WriteDigitizer(Option_t* opt=""){
    return  PhosLoader()->WriteDigitizer(opt) ; }
  
  //========== RecPoints =============
  TObjArray *           EmcRecPoints() ;
  AliPHOSEmcRecPoint *  EmcRecPoint(const Int_t index) { return static_cast<AliPHOSEmcRecPoint *>(EmcRecPoints()->At(index)) ;} 
  TObjArray *           CpvRecPoints() ; 
  AliPHOSCpvRecPoint *  CpvRecPoint(const Int_t index) { return static_cast<AliPHOSCpvRecPoint *>(CpvRecPoints()->At(index)) ;} 
  TTree *               TreeR() const ;
  AliPHOSClusterizer * Clusterizer()  ;
  TString               GetRecPointsFileName() { return PhosLoader()->GetRecPointsFileName() ; } 
  Int_t                 LoadRecPoints(Option_t* opt="") { return PhosLoader()->LoadRecPoints(opt) ; }
  Int_t                 LoadClusterizer(Option_t* opt=""){
    return  PhosLoader()->LoadClusterizer(opt) ; }
  Int_t                 WriteRecPoints(Option_t* opt="") { return PhosLoader()->WriteRecPoints(opt) ; }
  Int_t                 WriteClusterizer(Option_t* opt=""){
    return  PhosLoader()->WriteClusterizer(opt) ; }

  //========== TrackSegments   TClonesArray * TrackSegments(const char * name = 0) { 
  TClonesArray *           TrackSegments() ;
  AliPHOSTrackSegment *  TrackSegments(const Int_t index) { return static_cast<AliPHOSTrackSegment *>(TrackSegments()->At(index)) ;} 
  TTree *               TreeT() const ;
  AliPHOSTrackSegmentMaker * TrackSegmentMaker() ;
  TString               GetTracksFileName() { return PhosLoader()->GetTracksFileName() ; } 
  Int_t                 LoadTracks(Option_t* opt="") { return PhosLoader()->LoadTracks(opt) ; }
  Int_t                 LoadTrackSegementMaker(Option_t* opt=""){
    return  PhosLoader()->LoadTrackSegmentMaker(opt) ; }
  Int_t                 WriteTracks(Option_t* opt="") { return PhosLoader()->WriteTracks(opt) ; }
  Int_t                 WriteTrackSegmentMaker(Option_t* opt=""){
    return  PhosLoader()->WriteTracker(opt) ; }
  //========== RecParticles ===========

  TClonesArray *         RecParticles() ;
  AliPHOSRecParticle *   RecPaticles(const Int_t index) { return static_cast<AliPHOSRecParticle *>(RecParticles()->At(index)) ;} 
  TTree *               TreeP() const ;
  AliPHOSPID * PID() ;
  TString               GetRecParticlesFileName() { return PhosLoader()->GetRecParticlesFileName() ; } 
  Int_t                 LoadRecParticles(Option_t* opt="") { return PhosLoader()->LoadRecParticles(opt) ; }
  Int_t                 LoadPID(Option_t* opt=""){
    return  PhosLoader()->LoadPID(opt) ; }
  Int_t                 WriteRecParticles(Option_t* opt="") { return PhosLoader()->WriteRecParticles(opt) ; }
  Int_t                 WritePID(Option_t* opt=""){
    return  PhosLoader()->WritePID(opt) ; }


  void SetDebug(Int_t level) {fgDebug = level;} // Set debug level 
  void PostClusterizer(AliPHOSClusterizer * clu) 
    const{PhosLoader()->PostClusterizer(clu) ; }
  void PostPID(AliPHOSPID * pid) 
    const{PhosLoader()->PostPID(pid) ; }
  void PostTrackSegmentMaker(AliPHOSTrackSegmentMaker * tr) 
    const{PhosLoader()->PostTrackSegmentMaker(tr) ; }
  void PostSDigitizer (AliPHOSSDigitizer * sdigitizer) 
    const {PhosLoader()->PostSDigitizer(sdigitizer);}    
  void PostDigitizer (AliPHOSDigitizer * digitizer)    
    const {PhosLoader()->PostDigitizer(dynamic_cast<AliDigitizer *>(digitizer));}

  TString Version() const  { return PhosLoader()->GetTitle() ; } 
  AliPHOSLoader * PhosLoader() const { return  fgPhosLoader ; }
  
private:
  
  AliPHOSGetter(const char* headerFile,
		const char* version = AliConfig::fgkDefaultEventFolderName,
		Option_t * openingOption = "READ") ;

  Int_t ReadTreeD(void) ;
  Int_t ReadTreeH(void) ;
  Int_t ReadTreeR(void) ;
  Int_t ReadTreeT(void) ;
  Int_t ReadTreeS(void) ;
  Int_t ReadTreeP(void) ;


  void ReadPrimaries(void) ;

private:

//   static TFile * fgFile;           //! 

  AliPHOSBeamTestEvent * fBTE ;           //! Header if BeamTest Event

  static Int_t          fgDebug ;             //! Debug level

  TString           fLoadingStatus ;     //! tells which trees are loaded
  Int_t             fNPrimaries ;        //! # of primaries  
  TClonesArray *    fPrimaries ;         //! list of lists of primaries

//  AliPHOSCalibrationDB * fcdb ;       //!
   
  static AliPHOSLoader * fgPhosLoader ;
  static AliPHOSGetter * fgObjGetter; // pointer to the unique instance of the singleton 
  
  enum EDataTypes{kHits,kSDigits,kDigits,kRecPoints,kTracks,kNDataTypes};


  ClassDef(AliPHOSGetter,1)  // Algorithm class that provides methods to retrieve objects from a list knowing the index 

};

#endif // AliPHOSGETTER_H
