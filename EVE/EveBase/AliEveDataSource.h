//
//  AliEveDataSource.h
//
//  blame: Mikolaj Krzewicki, mikolaj.krzewicki@cern.ch
//
//

#ifndef __AliEveDataSource__
#define __AliEveDataSource__

#include "TFile.h"
#include "TTree.h"
#include "AliESDEvent.h"
#include "AliESDfriend.h"
#include "AliAODEvent.h"
#include "AliRunLoader.h"
#include "AliRawReader.h"
#include "TNamed.h"

#include "TQObject.h"

struct AliEveData {
  TFile        *fESDFile;		// ESD file.
  TTree        *fESDTree;		// ESD tree.
  TTree        *fHLTESDTree;	// HLT ESD tree.
  AliESDEvent  *fESD;			// ESDEvent object.
  AliESDfriend *fESDfriend;		// ESDfriend object.
  TFile        *fAODFile;		// AOD file.
  TTree        *fAODTree;		// AOD tree.
  AliAODEvent  *fAOD;			// AODEvent object.
  AliRunLoader* fRunLoader;		// Run loader.
  AliRawReader *fRawReader;     // Raw-data reader.
    
  AliEveData()
      : fESDFile(NULL)
      , fESDTree(NULL)
      , fHLTESDTree(NULL)
      , fESD(NULL)
      , fESDfriend(NULL)
      , fAODFile(NULL)
      , fAODTree(NULL)
      , fAOD(NULL)
      , fRunLoader(NULL)
      , fRawReader(NULL)
  {}

  void Clear()
  {
    delete fESDFile; fESDFile=NULL;
    delete fESDTree; fESDTree=NULL;
    delete fHLTESDTree; fHLTESDTree=NULL;
    delete fESD; fESD=NULL;
    delete fESDfriend; fESDfriend=NULL;
    delete fAODFile; fAODFile=NULL;
    delete fAODTree; fAODTree=NULL;
    delete fAOD; fAOD=NULL;
    delete fRunLoader; fRunLoader=NULL;
    delete fRawReader; fRawReader=NULL;
  }
};

class AliEveDataSource : public TNamed
{
public:
    AliEveDataSource(bool storageManager=false);
    ~AliEveDataSource();

    virtual void SetEventFromStorageManager(AliESDEvent *event);
    virtual void Init();
    virtual void GotoEvent(Int_t event);
    virtual void NextEvent();
    virtual const AliEveData* GetData() const {return &fCurrentData;}
    virtual Int_t GetMaxEventId(Bool_t refreshESD=kFALSE) const{return -1;}
    virtual void SetSourceURL(TString url) {fSourceURL=url; Init();}
    
    void StorageManagerOk();     // *SIGNAL*
    void StorageManagerDown();   // *SIGNAL*
    
protected:
    AliEveData fCurrentData;
    TString fSourceURL;
    
private:
    ClassDef(AliEveDataSource, 0); // Interface for getting all event components in a uniform way.
};

#endif
