#ifndef ALIPHOSTRACKSEGMENTMAKERV2_H
#define ALIPHOSTRACKSEGMENTMAKERV2_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
/* History of cvs commits:
 *
 * $Log$
 * Revision 1.50  2007/03/06 06:54:48  kharlov
 * DP:Calculation of cluster properties dep. on vertex added
 *
 * Revision 1.49  2007/02/01 13:59:11  hristov
 * Forward declaration
 *
 * Revision 1.48  2006/08/28 10:01:56  kharlov
 * Effective C++ warnings fixed (Timur Pocheptsov)
 *
 * Revision 1.47  2005/11/17 12:35:27  hristov
 * Use references instead of objects. Avoid to create objects when they are not really needed
 *
 * Revision 1.46  2005/05/28 14:19:05  schutz
 * Compilation warnings fixed by T.P.
 *
 */

//_________________________________________________________________________
// Implementation version 1 of algorithm class to construct PHOS track segments
// Associates EMC and CPV lusters
// Unfolds the EMC cluster   
//                  
//*-- Author: Dmitri Peressounko (RRC Ki & SUBATECH)

// --- ROOT system ---
#include <TVector3.h>

// --- Standard library ---

// --- AliRoot header files ---
#include "AliPHOSTrackSegmentMaker.h"

class AliPHOSEmcRecPoint ;
class AliPHOSCpvRecPoint ;
class AliESDtrack ;
class TClonesArray;

class  AliPHOSTrackSegmentMakerv2 : public AliPHOSTrackSegmentMaker {

public:

  AliPHOSTrackSegmentMakerv2() ;                     
  AliPHOSTrackSegmentMakerv2(const TString & alirunFileNameFile, const TString & eventFolderName = AliConfig::GetDefaultEventFolderName());
  AliPHOSTrackSegmentMakerv2(const AliPHOSTrackSegmentMakerv2 & tsm);

  virtual ~ AliPHOSTrackSegmentMakerv2() ; // dtor
  
  //  virtual char*  GetRecPointsBranch    (void)const{return (char*)fRecPointsBranchTitle.Data() ;}
  //  virtual char*  GetTrackSegmentsBranch(void)const{return (char*)fTrackSegmentsBranchTitle.Data() ;}
  virtual Int_t GetTrackSegmentsInRun()const {return fTrackSegmentsInRun ;}  

  virtual void   Exec(Option_t *option); // Does the job
          void   FillOneModule() ;       // Finds range in which RecPoints belonging current PHOS module are

          void   MakeLinks() const;      //Evaluates distances(links) between EMC and CPV
          void   MakePairs() ;           //Finds pairs(triplets) with smallest link
  virtual void   Print(const Option_t * = "") const ;
  //Switch to "on flyght" mode, without writing to TreeR and file  
  void SetWriting(Bool_t toWrite = kFALSE){fWrite = toWrite;} 
  virtual void   SetMaxTPCDistance(Float_t r){ fRtpc = r ;} //Maximal distance 
                                                               //between EMCrp and extrapolation of TPC track
  //  virtual void   SetRecPointsBranch(const char * title) { fRecPointsBranchTitle = title ;} 
  //  virtual void   SetTrackSegmentsBranch(const char * title){ fTrackSegmentsBranchTitle = title ; }
  virtual const char * Version() const { return "tsm-v2" ; }  

  AliPHOSTrackSegmentMakerv2 & operator = (const AliPHOSTrackSegmentMakerv2 & )  {
    // assignement operator requested by coding convention but not needed
    Fatal("operator =", "not implemented") ;
    return *this ; 
  }
  void Unload() ;

private:

  const TString BranchName() const ; 
  void  GetDistanceInPHOSPlane(AliPHOSEmcRecPoint * EmcClu , AliESDtrack* track,
                               Float_t &dx, Float_t &dz ) const ; // see R0
  void    Init() ;
  void    InitParameters() ;
  void    PrintTrackSegments(Option_t *option) ;
  virtual void   WriteTrackSegments() ;

private:  

  Bool_t  fDefaultInit;               //! Says if the task was created by defaut ctor (only parameters are initialized)
  Bool_t  fWrite ;                   // Write Tracks to TreeT  
 
  Int_t fNTrackSegments ; // number of track segments found 

  Float_t fRtpc ;        // Maximum distance between a EMC RecPoint and extrapolation of a TPC track   
  
  TVector3 fVtx ;        //! Vertex in current position

  TClonesArray * fLinkUpArray  ;  //!
  TList *fTPC[5];        //! lists of TPC tracks sorted over PHOS modules
  Int_t fEmcFirst;     //! Index of first EMC RecPoint belonging to currect PHOS module
  Int_t fEmcLast ;     //!
  Int_t fModule ;      //! number of module being processed
  Int_t fTrackSegmentsInRun ; //! Total number of track segments in one run

  ClassDef( AliPHOSTrackSegmentMakerv2,1)  // Implementation version 1 of algorithm class to make PHOS track segments 

 };

#endif // AliPHOSTRACKSEGMENTMAKERV2_H
