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
class TParticle ;
class TTree ; 
class TGraph ; 
class TF1 ; 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliConfig.h" 
#include "AliPHOSLoader.h" 
#include "AliPHOSHit.h" 
#include "AliPHOSDigit.h"
#include "AliPHOSEmcRecPoint.h" 
#include "AliPHOSCpvRecPoint.h" 
#include "AliPHOSTrackSegment.h" 
#include "AliPHOSRecParticle.h" 
#include "AliPHOSDigitizer.h"
#include "AliPHOSSDigitizer.h"
#include "AliPHOSCalibData.h"
#include "AliPHOSAlignData.h"

class AliPHOS ;  
class AliPHOSGeometry ;
class AliPHOSClusterizer ;
class AliPHOSTrackSegmentMaker ;  
class AliPHOSPID ; 
class AliPHOSBeamTestEvent ;
class AliESD ; 
class AliRawReader ;

class AliPHOSGetter : public TObject {
  
public:  
  AliPHOSGetter(){    // ctor: this is a singleton, the ctor should never be called but cint needs it as public
    Fatal("ctor", "AliPHOSGetter is a singleton default ctor not callable") ;
  } 

public:
  AliPHOSGetter(const AliPHOSGetter & obj) : TObject(obj) {
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
				  const char* version = AliConfig::GetDefaultEventFolderName(),
				  Option_t * openingOption = "READ" ) ; 
  static AliPHOSGetter * Instance() ; 
  
  void Print(const Option_t *)const{}
  static void Print() ; 
  
  //=========== General information about run ==============
  virtual Bool_t IsLoaded(TString tree) const { return fLoadingStatus.Contains(tree) ; } 
  virtual void   SetLoaded(TString tree) { fLoadingStatus += tree ; } 
  
  virtual Int_t  MaxEvent() const ; 
  virtual Int_t  EventNumber() const ; 
  virtual Bool_t VersionExists(TString & opt) const ; 
  virtual UShort_t EventPattern(void) const ; 
  virtual Float_t  BeamEnergy(void) const ;
  
  //========== PHOSGeometry and PHOS ============= 
  virtual AliPHOS *         PHOS() const  ;  
  virtual AliPHOSGeometry * PHOSGeometry() const ; 
  
  //========== Methods to read something from file ==========
  virtual void   Event(Int_t event, const char * opt = "HSDRTP") ;    
  void   Event(AliRawReader *rawReader, const char * opt = "W") ;    
  virtual void   Track(Int_t itrack) ;
 
  
  //-----------------now getter's data--------------------------------------
  AliPHOSCalibrationDB * CalibrationDB(){return  fcdb; }
  void ReadCalibrationDB(const char * /*name*/, const char * /*filename*/){ ;}
  void SetCalibrationDB(AliPHOSCalibrationDB * cdb) {fcdb = cdb ;}
  
  void SetCalibData(AliPHOSCalibData* calibda) { fgCalibData = calibda; }
  AliPHOSCalibData * CalibData();

  //=========== Primaries ============
  virtual TClonesArray *    Primaries(void) ;
  virtual TParticle * Primary(Int_t index) const ;
  virtual Int_t       NPrimaries()const { return fNPrimaries; }
  virtual TParticle * Secondary(const TParticle * p, Int_t index=1) const ;  
  
  //=========== Hits =================
  virtual TClonesArray *  Hits(void) const ; 
  virtual AliPHOSHit *    Hit(Int_t index) { return dynamic_cast<AliPHOSHit*>(Hits()->At(index) );}
  virtual TTree *         TreeH() const ; 
  
  //=========== SDigits ==============
  virtual TClonesArray *      SDigits() const ;  
  virtual AliPHOSDigit *      SDigit(Int_t index) { return static_cast<AliPHOSDigit *>(SDigits()->At(index)) ;} 
  virtual TTree *             TreeS() const ; 
  virtual AliPHOSSDigitizer * SDigitizer() ;  
  
  virtual TString             GetSDigitsFileName() const { return PhosLoader()->GetSDigitsFileName() ; }  
  virtual Int_t               LoadSDigits(Option_t* opt="") const { return PhosLoader()->LoadSDigits(opt) ; }
  virtual Int_t               LoadSDigitizer(Option_t* opt="") const { return  PhosLoader()->LoadSDigitizer(opt) ; }
  virtual Int_t               WriteSDigits(Option_t* opt="") const  { return PhosLoader()->WriteSDigits(opt) ; }
  virtual Int_t               WriteSDigitizer(Option_t* opt="") const {
    return  PhosLoader()->WriteSDigitizer(opt) ; }
  
  //========== Digits ================
  virtual TClonesArray * Digits() const ;
  virtual AliPHOSDigit * Digit(Int_t index) { return static_cast<AliPHOSDigit *>(Digits()->At(index)) ;} 
  virtual TTree *        TreeD() const ; 
  virtual AliPHOSDigitizer * Digitizer() ;
  virtual TString             GetDigitsFileName() const { return PhosLoader()->GetDigitsFileName() ; }  
  virtual Int_t               LoadDigits(Option_t* opt="") const { return PhosLoader()->LoadDigits(opt) ; }
  virtual Int_t               LoadDigitizer(Option_t* opt="") const {
    return  PhosLoader()->LoadDigitizer(opt) ; }
  virtual Int_t               WriteDigits(Option_t* opt="") const { return PhosLoader()->WriteDigits(opt) ; }
  virtual Int_t               WriteDigitizer(Option_t* opt="") const {
    return  PhosLoader()->WriteDigitizer(opt) ; }

  //Methods to distinguish raw and simulated digits
  virtual Bool_t              IsRawDigits(void) const {return fRawDigits;}
  virtual void                SetRawDigits(Bool_t isRaw = kTRUE){fRawDigits = isRaw;}
  
  //========== RecPoints =============
  virtual TObjArray *           EmcRecPoints() const;
  virtual AliPHOSEmcRecPoint *  EmcRecPoint(Int_t index) { return static_cast<AliPHOSEmcRecPoint *>(EmcRecPoints()->At(index)) ;} 
  virtual TObjArray *           CpvRecPoints() const ; 
  virtual AliPHOSCpvRecPoint *  CpvRecPoint(Int_t index) { return static_cast<AliPHOSCpvRecPoint *>(CpvRecPoints()->At(index)) ;} 
  virtual TTree *               TreeR() const ;
  virtual AliPHOSClusterizer * Clusterizer() ;
  virtual TString               GetRecPointsFileName() const { return PhosLoader()->GetRecPointsFileName() ; } 
  virtual Int_t                 LoadRecPoints(Option_t* opt="") const { return PhosLoader()->LoadRecPoints(opt) ; }
  virtual Int_t                 LoadClusterizer(Option_t* opt="") const {
    return  PhosLoader()->LoadClusterizer(opt) ; }
  virtual Int_t                 WriteRecPoints(Option_t* opt="") const { return PhosLoader()->WriteRecPoints(opt) ; }
  virtual Int_t                 WriteClusterizer(Option_t* opt="") const {
    return  PhosLoader()->WriteClusterizer(opt) ; }
  
  //========== TrackSegments   TClonesArray * TrackSegments(const char * name = 0) { 
  virtual TClonesArray *           TrackSegments() const;
  virtual AliPHOSTrackSegment *  TrackSegment(Int_t index) { return static_cast<AliPHOSTrackSegment *>(TrackSegments()->At(index)) ;} 
  virtual TTree *               TreeT() const ;
  virtual AliPHOSTrackSegmentMaker * TrackSegmentMaker() ;
  virtual TString               GetTracksFileName() const { return PhosLoader()->GetTracksFileName() ; } 
  virtual Int_t                 LoadTracks(Option_t* opt="") const { return PhosLoader()->LoadTracks(opt) ; }
  virtual Int_t                 LoadTrackSegementMaker(Option_t* opt="") const {
    return  PhosLoader()->LoadTrackSegmentMaker(opt) ; }
  virtual Int_t                 WriteTracks(Option_t* opt="") const { return PhosLoader()->WriteTracks(opt) ; }
  virtual Int_t                 WriteTrackSegmentMaker(Option_t* opt="") const {
    return  PhosLoader()->WriteTracker(opt) ; }
  
  //========== RecParticles ===========
  virtual TClonesArray *         RecParticles() const;
  virtual AliPHOSRecParticle *   RecParticle(Int_t index) { return static_cast<AliPHOSRecParticle *>(RecParticles()->At(index)) ;} 
  virtual TTree *               TreeP() const ;
  virtual AliPHOSPID * PID() ;
  virtual TString               GetRecParticlesFileName() const { return PhosLoader()->GetRecParticlesFileName() ; } 
  virtual Int_t                 LoadRecParticles(Option_t* opt="") const { return PhosLoader()->LoadRecParticles(opt) ; }
  virtual Int_t                 LoadPID(Option_t* opt="") const {
    return  PhosLoader()->LoadPID(opt) ; }
  virtual Int_t                 WriteRecParticles(Option_t* opt="") const { return PhosLoader()->WriteRecParticles(opt) ; }
  virtual Int_t                 WritePID(Option_t* opt="") const {
    return  PhosLoader()->WritePID(opt) ; }

  //========== Raw ===========
  virtual Int_t ReadRaw(AliRawReader *rawReader) ; 

  void SetDebug(Int_t level) {fgDebug = level;} // Set debug level 
  virtual void PostClusterizer(AliPHOSClusterizer * clu) 
    const{PhosLoader()->PostClusterizer(clu) ; }
  virtual void PostPID(AliPHOSPID * pid) 
    const{PhosLoader()->PostPID(pid) ; }
  virtual void PostTrackSegmentMaker(AliPHOSTrackSegmentMaker * tr) 
    const{PhosLoader()->PostTrackSegmentMaker(tr) ; }
  virtual void PostSDigitizer (AliPHOSSDigitizer * sdigitizer) 
    const {PhosLoader()->PostSDigitizer(sdigitizer);}    
  virtual void PostDigitizer (AliPHOSDigitizer * digitizer)    
    const {PhosLoader()->PostDigitizer(digitizer);}
  
  virtual TString Version() const  { return PhosLoader()->GetTitle() ; } 
  virtual AliPHOSLoader * PhosLoader() const { return  fgPhosLoader ; }
  virtual void Reset() ;
  
  virtual AliESD * ESD() const { return fESD ; }
  
protected :
  AliPHOSGetter(Int_t /*i*/){    // special constructor for onflight 

  } 
protected:
  static AliPHOSGetter * fgObjGetter; // pointer to the unique instance of the singleton 
  
private:
  AliPHOSGetter(const char* headerFile,
		const char* version = AliConfig::GetDefaultEventFolderName(),
		Option_t * openingOption = "READ") ;
private:
  
  Int_t ReadTreeD(void) ;
  Int_t ReadTreeH(void) ;
  Int_t ReadTreeR(void) ;
  Int_t ReadTreeT(void) ;
  Int_t ReadTreeS(void) ;
  Int_t ReadTreeP(void) ;

  Int_t ReadTreeE(Int_t event) ;    
  Bool_t OpenESDFile() ;
  void ReadPrimaries(void) ;

  void FitRaw(Bool_t lowGainFlag, TGraph * gLowGain, TGraph * gHighGain, TF1* signalF, Double_t & energy, Double_t & time) const; 

  Int_t CalibrateRaw (Double_t energy, Int_t *relId);

private:
  
  AliPHOSBeamTestEvent * fBTE ;           //! Header if BeamTest Event
  static Int_t          fgDebug ;             //! Debug level
  
  TString           fLoadingStatus ;     //! tells which trees are loaded
  Int_t             fNPrimaries ;        //! # of primaries  
  TClonesArray *    fPrimaries ;         //! list of lists of primaries
  TFile *           fESDFile ;           //! ESD file
  TString           fESDFileName ;       //! ESD File Name
  AliESD *          fESD ;               //! ESD object
  TTree *           fESDTree ;           //! ESD Tree
  
  Bool_t            fRawDigits ;         //! true is raw data

  AliPHOSCalibrationDB * fcdb ;       //!
  static AliPHOSCalibData * fgCalibData;

  static AliPHOSLoader * fgPhosLoader ; // the loader for the NewIO
  
  enum EDataTypes{kHits,kSDigits,kDigits,kRecPoints,kTracks,kNDataTypes};

  
  ClassDef(AliPHOSGetter,2)  // Algorithm class that provides methods to retrieve objects from a list knowing the index 
    
    };

#endif // AliPHOSGETTER_H
