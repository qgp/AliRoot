#ifndef ALIPHOSLOADER_H
#define ALIPHOSLOADER_H
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
#include "TClonesArray.h"
#include "TFolder.h"  
#include "TTree.h"
class TString ;
class TParticle ;
class TTask ;

// --- Standard library ---
#include <stdlib.h>
#include <Riostream.h>

// --- AliRoot header files ---

#include "AliRun.h"
#include "AliLoader.h"
#include "AliRunLoader.h"
#include "AliPHOS.h" 
#include "AliPHOSHit.h" 
#include "AliPHOSDigit.h"
#include "AliPHOSEmcRecPoint.h"
#include "AliPHOSCpvRecPoint.h"
#include "AliPHOSTrackSegment.h"
#include "AliPHOSRecParticle.h"
class AliPHOSGeometry ;
#include "AliPHOSDigitizer.h"
#include "AliPHOSSDigitizer.h"
#include "AliPHOSClusterizer.h"
#include "AliPHOSTrackSegmentMaker.h"
#include "AliPHOSPID.h"
class AliPHOSCalibrationDB ;


//

class AliPHOSLoader : public AliLoader {
  
 public:

  AliPHOSLoader();
  AliPHOSLoader(const AliPHOSLoader & obj){}
  AliPHOSLoader(const Char_t *detname,const Char_t *eventfoldername); 
  
  virtual ~AliPHOSLoader() ; 

  // assignement operator requested by coding convention, but not needed
  AliPHOSLoader & operator = (const AliPHOSLoader & ) {return *this;}

  Int_t   GetEvent();//extends the method on PHOS RecPart posting
  Int_t   SetEvent();//extends the method on PHOS RecPart posting
  
  Bool_t  BranchExists(const TString& recName);
  Int_t   LoadHits(Option_t* opt=""); //reads  from disk and sends them to folder; array as well as tree
  Int_t   LoadSDigits(Option_t* opt="");
  Int_t   LoadDigits(Option_t* opt=""); //reads Digits from disk and sends them to folder; array as well as tree
  Int_t   LoadRecPoints(Option_t* opt=""); //reads RecPoints from disk and sends them to folder; array as well as tree
  Int_t   LoadTracks(Option_t* opt="");  //reads Tracks from disk and sends them to folder; array as well as tree
  Int_t   LoadRecParticles(Option_t* opt="");
 
  void    UnloadRecParticles();
  void    UnloadTracks();
  
  Int_t   PostHits();  //Posts the 
  Int_t   PostSDigits();
  Int_t   PostDigits();
  Int_t   PostRecPoints();
  Int_t   PostTracks();
  Int_t   PostRecParticles();
  
  void    CleanFolders();//cleans all the stuff loaded by this detector + calls AliLoader::Clean

  void    CleanHits();
  void    CleanSDigits();
  void    CleanDigits();
  void    CleanRecPoints();
  void    CleanTracks();
  void    CleanRecParticles();

//up to now it is only here -> no definition about global/incremental tracking/PID
 
//   Int_t   WriteRecParticles(Option_t* opt="");//writes the reconstructed particles
//   Int_t   WritePID(Option_t* opt="");//writes the task for PID to file
//   Bool_t  PostPID  (AliPHOSPID * pid) const {return kTRUE;}
//  Bool_t  PostQA   (void) const ; //it was empty anyway
  
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

  TObject** HitsRef(){return GetDetectorDataRef(Hits());}
  TObject** SDigitsRef(){return GetDetectorDataRef(SDigits());}
  TObject** DigitsRef(){return GetDetectorDataRef(Digits());}
  TObject** EmcRecPointsRef(){return GetDetectorDataRef(EmcRecPoints());}
  TObject** CpvRecPointsRef(){return GetDetectorDataRef(CpvRecPoints());}
  TObject** TracksRef(){return GetDetectorDataRef(TrackSegments());}
  TObject** RecParticlesRef(){return GetDetectorDataRef(RecParticles());}
  TObject** AlarmsRef(){return GetDetectorDataRef(Alarms());}
  void   Track(Int_t itrack) ;

  static AliPHOSGeometry* GetPHOSGeometry();
  static AliPHOSLoader* GetPHOSLoader(const  char* eventfoldername);

  //Method to be used when digitizing under AliRunDigitizer, who opens all files etc.
  Int_t  EventNumber()       { return (Int_t) GetRunLoader()->GetEventNumber();}
  Int_t  MaxEvent()          { return (Int_t) GetRunLoader()->TreeE()->GetEntries();}

  const AliPHOS *         PHOS();
  const AliPHOSGeometry  *PHOSGeometry() ; 
  // Alarms
  // TFolder * Alarms() const { return (TFolder*)(ReturnO("Alarms", 0)); }
  TObjArray *  Alarms();  

  /*********************************************/
  /************    TClonesArrays     ***********/
  /*********************************************/
  /****   H i t s  ****/
  TClonesArray*  Hits(void);
  const AliPHOSHit*    Hit(Int_t index);
  void MakeHitsArray();
  /****   S D i g i t s  ****/ 
  TClonesArray*  SDigits();
  const AliPHOSDigit*  SDigit(Int_t index);
  void MakeSDigitsArray();
  /****  D i g i t s  ****/
  TClonesArray*   Digits();
  const AliPHOSDigit *  Digit(Int_t index);
  void MakeDigitsArray();
  /****  R e c P o i n t s  ****/
  TObjArray * EmcRecPoints();
  TObjArray * CpvRecPoints();
  const AliPHOSEmcRecPoint * EmcRecPoint(Int_t index) ;
  const AliPHOSCpvRecPoint * CpvRecPoint(Int_t index) ;
  void MakeRecPointsArray();
  /****   T r a c k S e g m e n t s ****/
  TClonesArray * TrackSegments();
  const AliPHOSTrackSegment * TrackSegment(Int_t index);
  void MakeTrackSegmentsArray();
  /****  R e c P a r t ic l e s   ****/
  TClonesArray * RecParticles() ;
  const AliPHOSRecParticle * RecParticle(Int_t index);
  void MakeRecParticlesArray();

  /*********************************************/
  /************    T A S K S      **************/
  /*********************************************/
  // 
  //  AliPHOSSDigitizer*  PHOSSDigitizer(TString name = AliConfig::fgkDefaultEventFolderName);
  //AliPHOSDigitizer*   PHOSDigitizer()  { return  dynamic_cast<AliPHOSDigitizer*>(Digitizer()) ;}

  AliPHOSClusterizer* Clusterizer ()  {return dynamic_cast<AliPHOSClusterizer*>(Reconstructioner()) ;}
  Int_t PostClusterizer(TTask* clust){return PostReconstructioner(clust);}
  Int_t LoadClusterizer(Option_t * opt="") {return LoadReconstructioner(opt);}
  Int_t WriteClusterizer(Option_t * opt="") {return WriteReconstructioner(opt);}

  AliPHOSPID * PID (){return dynamic_cast<AliPHOSPID*>(PIDTask()) ;}
  Int_t PostPID(TTask* pid){return PostPIDTask(pid);}
  Int_t LoadPID(Option_t * opt="") {return LoadPIDTask(opt);}
  Int_t WritePID(Option_t * opt="") {return WritePIDTask(opt);}


  AliPHOSTrackSegmentMaker * TrackSegmentMaker ()  { return dynamic_cast<AliPHOSTrackSegmentMaker *>(Tracker()) ;}
  Int_t PostTrackSegmentMaker(TTask* segmaker){return PostTracker(segmaker);}
  Int_t LoadTrackSegmentMaker(Option_t * opt="") {return LoadTracker(opt);}
  Int_t WriteTrackSegmentMaker(Option_t * opt="") {return WriteTracker(opt);}

  
  void   SetDebug(Int_t level) {fDebug = level;} // Set debug level
  void   SetBranchTitle(const TString& btitle);
  
  AliPHOSCalibrationDB * CalibrationDB(){return  fcdb; }
  void ReadCalibrationDB(const char * name, const char * filename);
  
protected:
  TString fBranchTitle;            //Title of the branch
  Bool_t  fRecParticlesLoaded;     //Flag signing if Reconstructed Particles are loaded
  Bool_t  fTracksLoaded;           //Flag signing if Tracks are loaded
  TString fRecParticlesFileOption; //Loading Option for Reconstructed Particles
  AliPHOSCalibrationDB * fcdb ;       //!

private:

  Int_t ReadHits();
  Int_t ReadDigits();
  Int_t ReadSDigits();
  Int_t ReadRecPoints();
  Int_t ReadTracks();
  Int_t ReadRecParticles();
  
  void  ReadTreeQA() ;
  Int_t  fDebug ;             // Debug level
 
 public:

  static const TString fgkHitsName;//Name for TClonesArray with hits from one event
  static const TString fgkSDigitsName;//Name for TClonesArray 
  static const TString fgkDigitsName;//Name for TClonesArray 
  static const TString fgkEmcRecPointsName;//Name for TClonesArray 
  static const TString fgkCpvRecPointsName;//Name for TClonesArray 
  static const TString fgkTracksName;//Name for TClonesArray 
  static const TString fgkRecParticlesName;//Name for TClonesArray

  static const TString fgkEmcRecPointsBranchName;//Name for branch
  static const TString fgkCpvRecPointsBranchName;//Name for branch
  static const TString fgkTrackSegmentsBranchName;//Name for branch
  static const TString fgkRecParticlesBranchName;//Name for branch
  
  ClassDef(AliPHOSLoader,2)  // Algorithm class that provides methods to retrieve objects from a list knowing the index 

};

/******************************************************************************/
/****************    I N L I N E S     ****************************************/
/******************************************************************************/

inline TClonesArray* AliPHOSLoader::Hits()  
{
 return (TClonesArray*)GetDetectorData(fgkHitsName);
}
/******************************************************************************/

inline const AliPHOSHit* AliPHOSLoader::Hit(Int_t index)  
{
  const TClonesArray* tcarr = Hits();
  if (tcarr)
    return (const AliPHOSHit*) tcarr->At(index);
  return 0x0; 
}
/******************************************************************************/

inline TClonesArray* AliPHOSLoader::SDigits()
{
   return dynamic_cast<TClonesArray*>(GetDetectorData(fgkSDigitsName));
}
/******************************************************************************/

inline const AliPHOSDigit*  AliPHOSLoader::SDigit(Int_t index)
{
  const TClonesArray* tcarr = SDigits();
  if (tcarr)
    return (const AliPHOSDigit*) tcarr->At(index);
  return 0x0; 
}
/******************************************************************************/

inline TClonesArray* AliPHOSLoader::Digits()
{
 return dynamic_cast<TClonesArray*>(GetDetectorData(fgkDigitsName));
}
/******************************************************************************/

inline const AliPHOSDigit*  AliPHOSLoader::Digit(Int_t index)
{
  const TClonesArray* tcarr = Digits();
  if (tcarr)
    return (const AliPHOSDigit*) tcarr->At(index);
  return 0x0; 
}
/******************************************************************************/

inline TObjArray * AliPHOSLoader::EmcRecPoints()
{
 return dynamic_cast<TObjArray*>(GetDetectorData(fgkEmcRecPointsName));
}
/******************************************************************************/

inline const AliPHOSEmcRecPoint * AliPHOSLoader::EmcRecPoint(Int_t index)
{
  TObjArray* tcarr = EmcRecPoints();
  if (tcarr)
    return (const AliPHOSEmcRecPoint*) tcarr->At(index);
  return 0x0; 
}
/******************************************************************************/

inline TObjArray * AliPHOSLoader::CpvRecPoints()
{
 return dynamic_cast<TObjArray*>(GetDetectorData(fgkCpvRecPointsName));
}
/******************************************************************************/

inline const AliPHOSCpvRecPoint * AliPHOSLoader::CpvRecPoint(Int_t index)
{
  TObjArray* tcarr = CpvRecPoints();
  if (tcarr)
    return (const AliPHOSCpvRecPoint*) tcarr->At(index);
  return 0x0; 
}
/******************************************************************************/

inline TClonesArray * AliPHOSLoader::TrackSegments()
{
 return dynamic_cast<TClonesArray*>(GetDetectorData(fgkTracksName));
}
/******************************************************************************/

inline const AliPHOSTrackSegment * AliPHOSLoader::TrackSegment(Int_t index)
{
  const TClonesArray* tcarr = TrackSegments();
  if (tcarr)
    return (const AliPHOSTrackSegment*) tcarr->At(index);
  return 0x0; 
}
/******************************************************************************/

inline TClonesArray * AliPHOSLoader::RecParticles() 
{
 return dynamic_cast<TClonesArray*>(GetDetectorData(fgkRecParticlesName)); 
}
/******************************************************************************/

inline const AliPHOSRecParticle* AliPHOSLoader::RecParticle(Int_t index)
{
  TClonesArray* tcarr = RecParticles();
  if (tcarr)
    return (const AliPHOSRecParticle*) tcarr->At(index);
  return 0x0;  
}
/******************************************************************************/
inline TObjArray *  AliPHOSLoader::Alarms()
{ return (TObjArray*)(GetQAFolder()->FindObject(fDetectorName));}

#endif // AliPHOSLOADER_H
