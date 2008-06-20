// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#ifndef AliEveEventManaget_H
#define AliEveEventManager_H

#include <TEveEventManager.h>

class AliRunLoader;
class AliESDEvent;
class AliESDfriend;
class AliRawReader;

class AliMagF;

class TFile;
class TTree;
class TGeoManager;

//==============================================================================
//
// AliEveEventManager
//
// Interface to ALICE event-data (RunLoader, ESD), magnetic field and
// geometry.
//


class AliEveEventManager : public TEveEventManager
{
public:
  static void SetESDFileName(const Text_t* esd);
  static void SetRawFileName(const Text_t* raw);
  static void SetCdbUri(const Text_t* cdb);
  static void SetAssertElements(Bool_t assertRunloader, Bool_t assertEsd, Bool_t assertRaw);

  AliEveEventManager();
  AliEveEventManager(TString path, Int_t ev=0);
  virtual ~AliEveEventManager();


  virtual void Open();
  void         SetEvent(AliRunLoader *runLoader, AliRawReader *rawReader, AliESDEvent *esd);
	    
  virtual void GotoEvent(Int_t event);
  virtual void NextEvent();
  virtual void PrevEvent();
  virtual void Close();

  Int_t         GetEventId()   const { return fEventId; }
  AliRunLoader* GetRunLoader() const { return fRunLoader; }
  TTree*        GetESDTree()   const { return fESDTree; }
  AliESDEvent*  GetESD()       const { return fESD; }
  AliESDfriend* GetESDfriend()       const { return fESDfriend; }
  Bool_t        GetESDfriendExists() const { return fESDfriendExists; }
  virtual const Text_t* GetTitle()   const { return fPath.Data(); }
  const char*   GetEventInfo() const;

  static AliRunLoader* AssertRunLoader();
  static AliESDEvent*  AssertESD();
  static AliESDfriend* AssertESDfriend();
  static AliRawReader* AssertRawReader();

  static AliMagF*      AssertMagField();

  static TGeoManager*  AssertGeometry();

  Bool_t        GetAutoLoad() const {return fAutoLoad;}
  Double_t      GetAutoLoadTime() const {return fAutoLoadTime;}
  void          SetAutoLoad(Bool_t autoLoad);
  void          SetAutoLoadTime(Double_t time);
  Bool_t        GetIsOnline() const {return fIsOnline;}

  void          StartStopAutoLoadTimer();

protected:
  TString       fPath;			// URL to event-data.
  Int_t         fEventId;		// Id of current event.

  AliRunLoader* fRunLoader;		// Run loader.

  TFile*        fESDFile;		// ESD file.
  TTree*        fESDTree;		// ESD tree.
  AliESDEvent*  fESD;			// ESDEvent object.
  AliESDfriend* fESDfriend;		// ESDfriend object.
  Bool_t        fESDfriendExists;	// Flag specifying if ESDfriend was found during opening of the event-data.

  AliRawReader* fRawReader;             // Raw-adata reader.

  Bool_t        fAutoLoad;              // Automatic loading of events (online)
  Double_t      fAutoLoadTime;          // Auto-load time in seconds
  TTimer       *fAutoLoadTimer;         // Timer for automatic event loading
  Bool_t        fIsOnline;              // Are we running online?

  static TString  fgESDFileName;        // Name by which to open ESD.
  static TString  fgRawFileName;        // Name by which to open raw-data file.
  static TString  fgCdbUri;		// Global URI to CDB.
  static Bool_t   fgAssertRunLoader;	// Global flag specifying if AliRunLoader must be asserted during opening of the event-data.
  static Bool_t   fgAssertESD;		// Global flag specifying if ESDEvent must be asserted during opening of the event-data.
  static Bool_t   fgAssertRaw;		// Global flag specifying if raw-data presence must be asserted during opening of the event-data.

  static AliMagF* fgMagField;		// Global pointer to magneti field.

private:
  AliEveEventManager(const AliEveEventManager&);            // Not implemented
  AliEveEventManager& operator=(const AliEveEventManager&); // Not implemented

  ClassDef(AliEveEventManager, 0); // Interface for getting all event components in a uniform way.
};

extern AliEveEventManager* gAliEveEvent;

#endif
