#ifndef ALIMUONDATA_H
#define ALIMUONDATA_H
//
// AliMUONData
// Class containing MUON data: hits, digits, rawclusters, globaltrigger, localtrigger, etc ...
// Gines Martinez, Subatech,  September 2003
//

#include "TNamed.h"

#include "AliLoader.h" 
#include "AliMUONConstants.h"

class TClonesArray;
class TObjArray;
class TTree;

class AliMUONRawCluster;

//__________________________________________________________________
/////////////////////////////////////////////////////////////////////
//                                                                 //
//  class AliMUONData                                              //
//                                                                 //
/////////////////////////////////////////////////////////////////////

class AliMUONData : public TNamed {
 public:
    AliMUONData();
    AliMUONData(AliLoader * loader, const char* name, const char* title);
    AliMUONData(const AliMUONData& rMUONData);
    virtual ~AliMUONData();  
    virtual void   AddDigit(Int_t id, Int_t* tracks, Int_t* charges,
			     Int_t* digits); 
    virtual void   AddHit(Int_t fIshunt, Int_t track, Int_t iChamber, 
			  Int_t idpart, Float_t X, Float_t Y, Float_t Z, 
			  Float_t tof, Float_t momentum, Float_t theta, 
			  Float_t phi, Float_t length, Float_t destep);
    virtual void   AddGlobalTrigger(Int_t *singlePlus, Int_t *singleMinus,
				    Int_t *singleUndef, Int_t *pairUnlike, 
				    Int_t *pairLike);
    virtual void   AddLocalTrigger(Int_t* ltrigger);
    virtual void   AddRawCluster(Int_t id, const AliMUONRawCluster& clust);
    TClonesArray*  Hits() {return fHits;}
    TClonesArray*  Digits(Int_t DetectionPlane, Int_t /*Cathode*/) 
      {return ( (TClonesArray*) fDigits->At(DetectionPlane) );}
    TClonesArray*  LocalTrigger() {return fLocalTrigger;}
    TClonesArray*  GlobalTrigger() {return fGlobalTrigger;}
    TClonesArray*  RawClusters(Int_t DetectionPlane)
      {return ( (TClonesArray*) fRawClusters->At(DetectionPlane) );}
    
    virtual void   MakeBranch(Option_t *opt=" ");
    virtual void   SetTreeAddress();

    virtual void   ResetHits();
    virtual void   ResetDigits();
    virtual void   ResetTrigger();
    virtual void   ResetRawClusters();
  
    TTree*         TreeH() {return fLoader->TreeH(); }
    TTree*         TreeD() {return fLoader->TreeD(); }
    TTree*         TreeR() {return fLoader->TreeR(); }
    TTree*         TreeT() {return fLoader->TreeT(); }
    TTree*         TreeP() {return fLoader->TreeP(); }

 private:  
    //descendant classes should
    //use protected interface methods to access these folders

  
 protected: 
    AliLoader*  fLoader;

    TClonesArray*   fHits;    // One event in treeH per primary track
    TObjArray*      fDigits;  // One event in treeD and one branch per detection plane
    TObjArray*      fRawClusters; //One event in TreeR/Rawcluster and one branch per tracking detection plane
    TClonesArray*   fGlobalTrigger; //! List of Global Trigger One event in TreeR/GlobalTriggerBranch
    TClonesArray*   fLocalTrigger;  //! List of Local Trigger, ONe event in TreeR/LocalTriggerBranch       
    Int_t           fNhits;
    Int_t*          fNdigits;
    Int_t*          fNrawclusters;
    Int_t           fNglobaltrigger;
    Int_t           fNlocaltrigger;

    ClassDef(AliMUONData,1)
 };
#endif
