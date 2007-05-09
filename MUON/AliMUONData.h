#ifndef ALIMUONDATA_H
#define ALIMUONDATA_H
//
/*  Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
// Revision of includes 07/05/2004
//
/// \ingroup base
/// \class AliMUONData
/// \brief Class containing MUON data: hits, digits, rawclusters, globaltrigger, 
/// localtrigger, etc ...
///
//  Author: Gines Martinez, Subatech,  September 2003

#include <TNamed.h>

#include "AliLoader.h"

class AliMUONConstants;
class AliMUONRawCluster;
class AliMUONTrack;
class AliMUONTriggerTrack;
class AliMUONDigit;
class AliMUONHit;
class AliMUONLocalTrigger;
class AliMUONRegionalTrigger;
class AliMUONGlobalTrigger;

class AliRunLoader;

class TClonesArray;
class TNamed;
class TObjArray;
class TTree;
class TIterator;
class TArrayI;


//__________________________________________________________________
/////////////////////////////////////////////////////////////////////
//                                                                 //
//  class AliMUONData                                              //
//                                                                 //
/////////////////////////////////////////////////////////////////////

class AliMUONData : public TNamed 
{
  public:
  
  //  enum EChamberIteration { kAllChambers, kTrackingChambers, kTriggerChambers };
  
    AliMUONData();
    AliMUONData(AliLoader * loader, const char* name, const char* title);
    AliMUONData(const char* galiceFile);
    virtual ~AliMUONData();  
    virtual void   AddDigit(Int_t id, const AliMUONDigit& digit); // use copy constructor
    virtual void   AddSDigit(Int_t id, const AliMUONDigit& digit); // use copy constructor
    virtual void   AddHit(Int_t fIshunt, Int_t track, Int_t detElemId, 
			  Int_t idpart, Float_t X, Float_t Y, Float_t Z, 
			  Float_t tof, Float_t momentum, Float_t theta, 
			  Float_t phi, Float_t length, Float_t destep, 
			  Float_t Xref,Float_t Yref,Float_t Zref);
    
    virtual void   AddGlobalTrigger(const AliMUONGlobalTrigger& trigger); // use copy constructor

    virtual void   AddLocalTrigger(const AliMUONLocalTrigger& trigger); // use copy constructor
 
    virtual void   AddRegionalTrigger(const AliMUONRegionalTrigger& trigger); // use copy constructor

    virtual void   AddRawCluster(Int_t id, const AliMUONRawCluster& clust);
    virtual void   AddRecTrack(const AliMUONTrack& track);
    virtual void   AddRecTriggerTrack(const AliMUONTriggerTrack& triggertrack);

    TClonesArray*  Hits() {return fHits;} ///< Return hits
    TClonesArray*  Digits(Int_t DetectionPlane) const;
    TClonesArray*  SDigits(Int_t DetectionPlane) const;
    TClonesArray*  LocalTrigger() const;
    TClonesArray*  RegionalTrigger() const;
    TClonesArray*  GlobalTrigger() const;    
    TClonesArray*  RawClusters(Int_t DetectionPlane);
    
                    /// Return reconstructed tracks
    TClonesArray*  RecTracks() {return fRecTracks;} 
                   /// Return reconstructed trigger tracks
    TClonesArray*  RecTriggerTracks() {return fRecTriggerTracks;}

    void           GetTrack(Int_t it) const;
    Int_t          GetNtracks() const;
    void           GetDigits() const;
    void           GetSDigits() const;
    void           GetRawClusters() const;
    void           GetTrigger() const;
    void           GetTriggerD() const;
    void           GetRecTracks() const;
    void           GetRecTriggerTracks() const;

                   /// Return split level
    Int_t          GetSplitLevel() const {return fSplitLevel;}

    Bool_t        IsRawClusterBranchesInTree();
    Bool_t        IsDigitsBranchesInTree();
    Bool_t        IsTriggerBranchesInTree();
    Bool_t        IsTriggerBranchesInTreeD();
    Bool_t        IsTrackBranchesInTree();
    Bool_t        IsTriggerTrackBranchesInTree();

                       /// Get loader
    virtual AliLoader* GetLoader() const { return fLoader; }
                       /// Set loader
    virtual void       SetLoader(AliLoader * loader) {fLoader=loader;}    
    
    virtual void   Fill(Option_t* opt=" ");
    virtual void   MakeBranch(Option_t *opt=" ");
    virtual void   SetDataContainer(Option_t *opt=" ");
    virtual void   SetTreeAddress(Option_t *opt=" ");
    
                   /// Set split level
    void           SetSplitLevel(Int_t SplitLevel) {fSplitLevel=SplitLevel;}
    
    virtual void Print(Option_t* opt="") const;
    
    virtual void   ResetHits();
    virtual void   ResetDigits();
    virtual void   ResetSDigits();
    virtual void   ResetTrigger();
    virtual void   ResetRawClusters();
    virtual void   ResetRecTracks();
    virtual void   ResetRecTriggerTracks();
  
                   /// Return tree with hits
    TTree*         TreeH() {return fLoader->TreeH(); }
                   /// Return tree with digits
    TTree*         TreeD() {return fLoader->TreeD(); }
                   /// Return tree with summable digits
    TTree*         TreeS() {return fLoader->TreeS(); }
                   /// Return tree with raw clusters
    TTree*         TreeR() {return fLoader->TreeR(); }
                   /// Return tree with tracks
    TTree*         TreeT() {return fLoader->TreeT(); }
                   /// Return tree with particles
    TTree*         TreeP() {return fLoader->TreeP(); }

                   // Methods to dump data
    void DumpKine(Int_t event2Check=0);
    void DumpHits(Int_t event2Check=0, Option_t* opt="full");
    void DumpDigits(Int_t event2Check=0, Option_t* opt="tracks");
    void DumpSDigits(Int_t event2Check=0, Option_t* opt="tracks");
    void DumpRecPoints(Int_t event2Check=0, Option_t* opt="full");
    void DumpRecTrigger(Int_t event2Check=0, Int_t write = 0, Bool_t readFromRP = kTRUE);
    
  protected: 
    /// Not implemented
    AliMUONData(const AliMUONData& rhs);
    /// Not implemented
    AliMUONData& operator=(const AliMUONData& rhs);

    AliRunLoader*   fRunLoader; //!< Run loader pointer
    AliLoader*      fLoader;  //!< Detector Loader pointer
    TClonesArray*   fHits;    ///< One event in treeH per primary track
    TObjArray*      fDigits;  ///< One event in treeD and one branch per detection plane
    TObjArray*      fSDigits; ///< One event in treeS and one branch per detection plane
    TObjArray*      fRawClusters; ///< One event in TreeR/Rawcluster and one branch per tracking detection plane
    TClonesArray*   fGlobalTrigger; ///< List of Global Trigger One event in TreeR/GlobalTriggerBranch
    TClonesArray*   fLocalTrigger;  ///< List of Local Trigger, One event in TreeR/LocalTriggerBranch
    TClonesArray*   fRegionalTrigger;  ///< List of Regional Trigger, One event in TreeR/LocalTriggerBranch

    TClonesArray*   fRecTracks; ///< pointer to array of reconstructed tracks
    TClonesArray*   fRecTriggerTracks; ///< pointer to array of reconstructed trigger tracks

    Int_t           fNhits;   //!< Number of Hits
    Int_t*          fNdigits; //!< Number of Digits
    Int_t*          fNSdigits;//!< Number of Digits
    Int_t*          fNrawclusters;  //!< Number of Raw Clusters
    Int_t           fNglobaltrigger;//!< Number of Global trigger
    Int_t           fNlocaltrigger; //!< Number of Local trigger
    Int_t           fNregionaltrigger; //!< Number of regional trigger
    Int_t           fNrectracks;    //!< Number of reconstructed tracks
    Int_t           fNrectriggertracks; //!< Number of reconstructed tracks
    Int_t           fSplitLevel;   ///< Splitting of branches 0 no spitting (root files are smaller) 1 splitting (larger output files)

    mutable Int_t fCurrentEvent; ///< Current event we're dealing with
    
private:  

  ClassDef(AliMUONData,3) // Data accessor for MUON module
      
};
// inline functions


/// Load hits for \a i th entry in hits three
inline void AliMUONData::GetTrack(Int_t it) const  {
  if (fLoader && fLoader->TreeH())
    fLoader->TreeH()->GetEvent(it);
}

/// Load sdigits tree
inline void AliMUONData::GetSDigits() const {
  if (fLoader && fLoader->TreeS())
    fLoader->TreeS()->GetEvent(0);
}

/// Load raw clusters tree
inline void AliMUONData::GetRawClusters() const {
  if (fLoader && fLoader->TreeR())
    fLoader->TreeR()->GetEvent(0);
}

/// Load trigger tree
inline void AliMUONData::GetTrigger() const {
  if (fLoader && fLoader->TreeR())
    fLoader->TreeR()->GetEvent(0);
}

/// Load trigger D tree
inline void AliMUONData::GetTriggerD() const {
  if (fLoader && fLoader->TreeD())
    fLoader->TreeD()->GetEvent(0);
}

/// Return reconstructed tracks
inline void AliMUONData::GetRecTracks() const {
  if (fLoader && fLoader->TreeT())
    fLoader->TreeT()->GetBranch("MUONTrack")->GetEvent(0);
}

/// Return reconstructed trigger tracks
inline void AliMUONData::GetRecTriggerTracks() const {
  if (fLoader && fLoader->TreeT())
    fLoader->TreeT()->GetBranch("MUONTriggerTrack")->GetEvent(0);
}



#endif

