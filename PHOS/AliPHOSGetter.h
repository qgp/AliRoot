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
// #include "AliPHOSDigit.h"
// #include "AliPHOSEmcRecPoint.h"
// #include "AliPHOSCpvRecPoint.h"
// #include "AliPHOSTrackSegment.h"
// #include "AliPHOSRecParticle.h"
class AliPHOSGeometry ;
#include "AliPHOSDigitizer.h"
#include "AliPHOSSDigitizer.h" 
// class AliPHOSClusterizer ;
// class AliPHOSTrackSegmentMaker ;
// class AliPHOSPID ;
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
//   const UShort_t EventPattern(void) ;
//   const Float_t BeamEnergy(void) ;
  
//   //========== PHOSGeometry and PHOS ============= 
  AliPHOS *         PHOS() const  ;  
  AliPHOSGeometry * PHOSGeometry() const ; 
  
//   //========== Methods to read something from file ==========
  void   Event(const Int_t event, const char * opt = "HSDRP") ;    
  void   Track(const Int_t itrack) ;
  //  void   ReadTreeS() ; //Method to be used when 
//                                                 //digitizing is under the control ofAliRunDigizer, 
                                                //which opens all files etc.
//   //========== Alarms ======================
//   TFolder * Alarms() const { return dynamic_cast<TFolder*>(ReturnO("Alarms", 0)) ; }
//   const TObjArray *  Alarms(const char * name ) const { return dynamic_cast<const TObjArray*>(ReturnO("Alarms", name)) ; }
//   const TTask * QATasks(const char * name = 0) const { return ReturnT("QATasks", name) ; }
  
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
  AliPHOSSDigitizer * SDigitizer() const { 
    return dynamic_cast<AliPHOSSDigitizer *>(PhosLoader()->SDigitizer()) ;} 

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
  AliPHOSDigitizer * Digitizer() const { 
       return dynamic_cast<AliPHOSDigitizer *>(PhosLoader()->Digitizer()) ; }
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
  TTree *               TreeR() const { return PhosLoader()->TreeR() ; }
//   const AliPHOSClusterizer * Clusterizer (const char * name =0) const { 
//     return (const AliPHOSClusterizer*)(ReturnT("Clusterizer", name)) ;}
  TString               GetRecPointsFileName() { return PhosLoader()->GetRecPointsFileName() ; } 
  Int_t                 LoadRecPoints(Option_t* opt="") { return PhosLoader()->LoadRecPoints(opt) ; }
  Int_t                 LoadClusterizer(Option_t* opt=""){
    return  PhosLoader()->LoadClusterizer(opt) ; }
  Int_t                 WriteRecPoints(Option_t* opt="") { return PhosLoader()->WriteRecPoints(opt) ; }
  Int_t                 WriteClusterizer(Option_t* opt=""){
    return  PhosLoader()->WriteClusterizer(opt) ; }

//   //========== TrackSegments ==========
//   TClonesArray * TrackSegments(const char * name = 0) { 
//     return static_cast<TClonesArray*>(ReturnO("TrackSegments", name)) ;   }
//   const AliPHOSTrackSegmentMaker * TrackSegmentMaker (const char * name =0) const { 
//     return (const AliPHOSTrackSegmentMaker*)(ReturnT("TrackSegmentMaker", name)) ;   }
  
//   //========== RecParticles ===========
//   TClonesArray * RecParticles(const char * name = 0) { 
//     return static_cast<TClonesArray*>(ReturnO("RecParticles", name)) ;   }
//   const AliPHOSPID * PID(const char * name =0) const { 
//     return (const AliPHOSPID*)(ReturnT("PID", name)) ; } // here static or dynamic cast does not work ! why ? 
    
//   //-----------------Auxiliary methods: cleaners-----------------
//   void  RemoveTask(TString opt, TString name) const ;
//   void  RemoveObjects(TString opt, TString name) const ;
//   void  RemoveSDigits() const ;  

//   //----------------Auxiliary methods: miscellana----------------
//   void CloseFile() ;  
//   const TFolder * Folder(const TString what) const ;
//   const Bool_t HasFailed() const { return fFailed ; }
//   void ListBranches(Int_t event=0) const ;
//   void NewBranch(TString name, Int_t event = 0) ; 
//   Bool_t NewFile(TString name) ;
//   TFolder * SDigitsFolder() { return dynamic_cast<TFolder*>(fSDigitsFolder->FindObject("PHOS")) ; }  
  void SetDebug(Int_t level) {fgDebug = level;} // Set debug level
//   void SetRecParticlesTitle(const TString title) { fRecParticlesTitle = title ; }
  
//   //------------Auxiliary methods: Posters--------------------
//   const Bool_t PostPrimaries(void ) const ;  
//   const Bool_t PostHits(void ) const ;  
//   const Bool_t PostSDigits(      const char * name,  const char * file = 0) const ;  
//   const Bool_t PostDigits(       const char * name ) const ;  
//   const Bool_t PostRecPoints(    const char * name ) const ;  
//   const Bool_t PostTrackSegments(const char * name) const ;  
//   const Bool_t PostRecParticles( const char * name) const ;  
//   const Bool_t PostClusterizer( const char * name) const ;  
//   const Bool_t PostClusterizer(AliPHOSClusterizer * clu) const ;  
  void PostSDigitizer (AliPHOSSDigitizer * sdigitizer) const {PhosLoader()->PostSDigitizer(sdigitizer);}  
//   const Bool_t PostSDigitizer ( const char * name, const char * file ) const ;  
  void PostDigitizer (AliPHOSDigitizer * digitizer)    const {PhosLoader()->PostDigitizer(dynamic_cast<AliDigitizer *>(digitizer));}
//   const Bool_t PostDigitizer  ( const char * name) const ;  
//   const Bool_t PostTrackSegmentMaker(AliPHOSTrackSegmentMaker * tsm) const ;  
//   const Bool_t PostTrackSegmentMaker(const char * name ) const ;  
//   const Bool_t PostPID  (AliPHOSPID * pid) const ;  
//   const Bool_t PostPID  (const char * name ) const ;  
//   const Bool_t PostQA   (void) const ;

  TString Version() const  { return PhosLoader()->GetTitle() ; } 
  AliPHOSLoader * PhosLoader() const { return  fgPhosLoader ; }
  
private:
  
  AliPHOSGetter(const char* headerFile,
		const char* version = AliConfig::fgkDefaultEventFolderName,
		Option_t * openingOption = "READ") ;

//   TObject * ReturnO(TString what, TString name=0, TString file=0) const ; 
//   const TTask * ReturnT(TString what,TString name=0) const ; 
//   void DefineBranchTitles(char* branch, char* branchTitle) ;
  Int_t ReadTreeD(void) ;
  Int_t ReadTreeH(void) ;
//   Int_t ReadTreeR(const Int_t event) ;
  Int_t ReadTreeS(void) ;
//   void ReadTreeQA(void) ;
  void ReadPrimaries(void) ;
//   void CleanWhiteBoard(void) ;
//   void CloseSplitFiles(void) ;
//   void SetTitle(const char * title) ;

//   TObject** PrimariesRef(void) const ;
//   TObject** HitsRef(void) const ;
//   TObject** SDigitsRef(const char * name, const char * file = 0 ) const;
//   TObject** DigitsRef (const char * name)   const ;
//   TObject** EmcRecPointsRef (const char * name) const ;
//   TObject** CpvRecPointsRef (const char * name) const ;
//   TObject** TrackSegmentsRef(const char * name)   const ;
//   TObject** RecParticlesRef (const char * name)   const ;
//   TObject** AlarmsRef (void)   const ;

//   TObject** SDigitizerRef (const char * name) const ; 
//   TObject** DigitizerRef  (const char * name) const ; 
//   TObject** ClusterizerRef(const char * name) const ; 
//   TObject** TSMakerRef    (const char * name) const ; 
//   TObject** PIDRef        (const char * name) const ; 

private:

//   static TFile * fgFile;           //! 
//   Bool_t         fToSplit ;              //! Do we work in the split mode
  AliPHOSBeamTestEvent * fBTE ;           //! Header if BeamTest Event
//   TClonesArray *    fHits ;            //! list of hits
//   TClonesArray *    fSDigits ;         //! list of SDigits
//   TClonesArray *    fDigits ;          //! list of Digits
//   TString        fHeaderFile ;           //! File in which gAlice lives
//   TString        fBranchTitle ;          //!
//   TString        fTrackSegmentsTitle ;   //! 
//   TString        fTrackSegmentsFileName ;//! 
//   TString        fRecPointsTitle ;       //!
//   TString        fRecPointsFileName ;    //!
//   TString        fRecParticlesTitle ;    //!
//   TString        fRecParticlesFileName ; //!
//   TString        fDigitsTitle ;          //! TDirectory tempo(gDirectory) 
//   TString        fDigitsFileName ;       //! TDirectory tempo(gDirectory) 
//   TString        fSDigitsTitle ;         //!
//   Bool_t         fFailed ;            //! set if file not opend or galice not found
  static Int_t          fgDebug ;             //! Debug level
//   AliRun *       fAlice ;             //! needed to read TreeK if in an other file than fHeaderFile
  TString           fLoadingStatus ;     //! tells which trees are loaded
  Int_t             fNPrimaries ;        //! # of primaries  
  TClonesArray *    fPrimaries ;         //! list of lists of primaries
//   TFolder *      fModuleFolder ;      //!Folder that contains the modules 
//   TFolder *      fPrimariesFolder ;   //!Folder that contains the Primary Particles 
//   TFolder *      fHitsFolder ;        //!Folder that contains the Hits 
//   TFolder *      fSDigitsFolder ;     //!Folder that contains the SDigits 
//   TFolder *      fDigitsFolder ;      //!Folder that contains the Digits 
//   TFolder *      fRecoFolder ;        //!Folder that contains the reconstructed objects (RecPoints, TrackSegments, RecParticles) 
  
//   TFolder *      fQAFolder ;          //!Folder that contains the QA objects  
//   TFolder *      fTasksFolder ;       //!Folder that contains the Tasks (sdigitizer, digitizer, reconstructioner)

//  AliPHOSCalibrationDB * fcdb ;       //!
   
  static AliPHOSLoader * fgPhosLoader ;
  static AliPHOSGetter * fgObjGetter; // pointer to the unique instance of the singleton 
  
  enum EDataTypes{kHits,kSDigits,kDigits,kRecPoints,kTracks,kNDataTypes};


  ClassDef(AliPHOSGetter,1)  // Algorithm class that provides methods to retrieve objects from a list knowing the index 

};

#endif // AliPHOSGETTER_H
