#ifndef ALIPHOSTRACKSEGMENTMAKERV1_H
#define ALIPHOSTRACKSEGMENTMAKERV1_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//_________________________________________________________________________
// Implementation version 1 of algorithm class to construct PHOS track segments
// Associates EMC and PPSD clusters
// Unfolds the EMC cluster   
//                  
//*-- Author: Dmitri Peressounko (RRC Ki & SUBATECH)

// --- ROOT system ---

// --- Standard library ---

// --- AliRoot header files ---
#include "AliPHOSTrackSegmentMaker.h"

class AliPHOSEmcRecPoint ;
class AliPHOSRecPoint ;


class  AliPHOSTrackSegmentMakerv1 : public AliPHOSTrackSegmentMaker {

public:

  AliPHOSTrackSegmentMakerv1() ;                     
  AliPHOSTrackSegmentMakerv1(const char* headerFile, const char* name = "Default", const Bool_t toSplit = kFALSE) ;                     
  AliPHOSTrackSegmentMakerv1(const AliPHOSTrackSegmentMakerv1 & tsm) {
    // cpy ctor: no implementation yet
    // requested by the Coding Convention
    abort() ; 
  }
   
  virtual ~ AliPHOSTrackSegmentMakerv1() ; // dtor
  
  //  virtual char*  GetRecPointsBranch    (void)const{return (char*)fRecPointsBranchTitle.Data() ;}
  //  virtual char*  GetTrackSegmentsBranch(void)const{return (char*)fTrackSegmentsBranchTitle.Data() ;}
  virtual const Int_t GetTrackSegmentsInRun()const {return fTrackSegmentsInRun ;}  

  virtual void   Exec(Option_t * option) ;
          void   FillOneModule() ;       // Finds range in which RecPoints belonging current PHOS module are

          void   MakeLinks() const;      //Evaluates distances(links) between EMC and PPSD
          void   MakePairs() ;           //Finds pairs(triplets) with smallest link
  virtual void   Print(Option_t * option) const ;
  virtual void   SetMaxEmcCPVDistance(Float_t r){ fRcpv = r ;} //Maximal distance (in PHOS plane) 
                                                               //between EMCrp and CPVrp
  virtual void   SetMaxEmcTPCDistance(Float_t r){ fRtpc = r ;} //Maximal distance (in PHOS plane) 
                                                               //between EMCrp and extrapolation of TPC track
  //  virtual void   SetRecPointsBranch(const char * title) { fRecPointsBranchTitle = title ;} 
  //  virtual void   SetTrackSegmentsBranch(const char * title){ fTrackSegmentsBranchTitle = title ; }
  virtual const char * Version() const { return "tsm-v1" ; }  

  AliPHOSTrackSegmentMakerv1 & operator = (const AliPHOSTrackSegmentMakerv1 & )  {
    // assignement operator requested by coding convention but not needed
    abort() ;
    return *this ; 
  }


private:

  const TString BranchName() const ; 
  Float_t GetDistanceInPHOSPlane(AliPHOSEmcRecPoint * EmcClu , AliPHOSRecPoint * Ppsd , Bool_t & TooFar )const ; // see R0
  void    Init() ;
  void    InitParameters() ;
  void    PrintTrackSegments(Option_t *option) ;
  virtual void   WriteTrackSegments(Int_t event) ;

private:  

  Bool_t  fDefaultInit;               //! Says if the task was created by defaut ctor (only parameters are initialized)
  //  TString fFrom ;                     // name of Recpoints 
  //  TString fHeaderFileName ;           // name of the file which contains gAlice, Tree headers etc.
  //  TString fRecPointsBranchTitle ;     // name of the file, where RecPoints branchs are stored
  //  TString fTrackSegmentsBranchTitle ; // name of the file, where TrackSegment branchs is stored
 
  Int_t fNTrackSegments ; // number of track segments found 
  Float_t fRcpv ;        // Maximum distance between a EMC RecPoint and a CPV RecPoint   
  Float_t fRtpc ;        // Maximum distance between a EMC RecPoint and extrapolation of a TPC track   

  TClonesArray * fLinkUpArray  ;  //!
  Int_t fEmcFirst;     //! Index of first EMC RecPoint belonging to currect PHOS module
  Int_t fEmcLast ;     //!
  Int_t fCpvFirst;     //! Cpv upper layer     
  Int_t fCpvLast;      //! 
  Int_t fModule ;      //! number of module being processed
  Int_t fTrackSegmentsInRun ; //! Total number of track segments in one run

  ClassDef( AliPHOSTrackSegmentMakerv1,2)  // Implementation version 1 of algorithm class to make PHOS track segments 

};

#endif // AliPHOSTRACKSEGMENTMAKERV1_H
