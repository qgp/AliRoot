// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#ifndef AliEveEventManager_H
#define AliEveEventManager_H

#include <TEveEventManager.h>
#include <TQObject.h>
#include <TObjArray.h>

#include <AliEventInfo.h>

#include <AliESDEvent.h>

//#include <zmq.hpp>


class AliEveMacroExecutor;
class AliEveEventSelector; 

class AliRunLoader;
class AliESDEvent;
class AliESDfriend;
class AliAODEvent;
class AliRawReader;

class AliGRPObject;
class AliRecoParam;
class AliMagF;

class TEveElement;
class TFile;
class TTree;
class TGeoManager;
class TString;
class TMap;

//==============================================================================
//
// AliEveEventManager
//
// Interface to ALICE event-data (RunLoader, ESD), magnetic field and
// geometry.
//


class AliEveEventManager : public TEveEventManager,
        public TQObject
{
public:
    enum EVisibleESDTrees{ kOfflineTree, kHLTTree };

    static void SetESDFileName(const TString& esd, EVisibleESDTrees shown=kOfflineTree);
    static void SetESDfriendFileName(const TString& esdf);
    static void SetAODFileName(const TString& aod);
    static void AddAODfriend  (const TString& friendFileName);
    static void SetRawFileName(const TString& raw);
    static void SetCdbUri     (const TString& cdb);
    static void SetGAliceFileName(const TString& galice);

    // set Local Directory or URL where the files are located
    // it can also be a path to root_archive.zip
    // assumes the filenames of ESD, AOD, etc are standard ALICE names
    // (AliESDs.root, AliESDfriends.root, AliAOD.root, AliAODfriend.root, galice.root,raw.root)
    static void SetFilesPath(const TString& path);

    static void SetAssertElements(Bool_t assertRunloader, Bool_t assertEsd,
                                  Bool_t assertAod, Bool_t assertRaw);
    static void SearchRawForCentralReconstruction();

    AliEveEventManager(const TString& name="Event", Int_t ev=0);
    virtual ~AliEveEventManager();

    virtual void  Open();
    void          SetEvent(AliRunLoader *runLoader, AliRawReader *rawReader, AliESDEvent *esd, AliESDfriend *esdf);
    virtual Int_t GetMaxEventId(Bool_t refreshESD=kFALSE) const;
    virtual void  GotoEvent(Int_t event);
    virtual void  NextEvent();
    virtual void  PrevEvent();
    virtual void  Close();
    void Timeout(); // * SIGNAL*

    Int_t         GetEventId()         const { return fEventId; }
    AliRunLoader* GetRunLoader()       const { return fRunLoader; }
    TFile*        GetESDFile()         const { return fESDFile; }
    TTree*        GetESDTree()         const { return fESDTree; }
    TTree*        GetHLTESDTree()      const { return fHLTESDTree; }
    AliESDEvent*  GetESD()             const { return fESD;     }
    AliESDfriend* GetESDfriend()       const { return fESDfriend; }
    Bool_t        GetESDfriendExists() const { return fESDfriendExists; }
    TFile*        GetAODFile()         const { return fAODFile; }
    TTree*        GetAODTree()         const { return fAODTree; }
    AliAODEvent*  GetAOD()             const { return fAOD;     }
    AliEveEventSelector* GetEventSelector() const { return fPEventSelector; }
    TString       GetEventInfoHorizontal() const;
    TString       GetEventInfoVertical()   const;
    const AliEventInfo*	GetEventInfo();

		static bool ConnectToServer(); // connect to the events server
		//static zmq::socket_t* AssertSubscriber();

    static Int_t  CurrentEventId();

    static Bool_t HasRunLoader();
    static Bool_t HasESD();
    static Bool_t HasESDfriend();
    static Bool_t HasAOD();
    static Bool_t HasRawReader();

    static AliRunLoader* AssertRunLoader();
    static AliESDEvent*  AssertESD();
    static AliESDfriend* AssertESDfriend();
    static AliAODEvent*  AssertAOD();
    static AliRawReader* AssertRawReader();

    static AliMagF*      AssertMagField();
    static TGeoManager*  AssertGeometry();
    static AliRecoParam* AssertRecoParams();

    static AliEveEventManager* AddDependentManager(const TString& name, const TString& path);
    static AliEveEventManager* GetDependentManager(const TString& name);

    static AliEveEventManager* GetMaster();
    static AliEveEventManager* GetCurrent();

    static void                RegisterTransient    (TEveElement* element);
    static void                RegisterTransientList(TEveElement* element);


    Double_t      GetAutoLoadTime()        const { return fAutoLoadTime; }
    Bool_t        GetAutoLoad()            const { return fAutoLoad;     }
    void          SetAutoLoadTime(Float_t time);
    void          SetAutoLoad(Bool_t autoLoad);
    void          SetTrigSel(Int_t trig);
    void          AutoLoadNextEvent();

    Bool_t        AreEventFilesOpened()    const { return fIsOpen;       }
    Bool_t        IsEventAvailable()       const { return fHasEvent;     }
    Bool_t        IsUnderExternalControl() const { return fExternalCtrl; }

    Bool_t        InsertGlobal(const TString& tag, TEveElement* model);
    Bool_t        InsertGlobal(const TString& tag, TEveElement* model,
                               Bool_t replace, Bool_t update);
    TEveElement*  FindGlobal(const TString& tag);

    virtual void  AfterNewEventLoaded();
    void          NewEventDataLoaded();  // *SIGNAL*
    void          NewEventLoaded();      // *SIGNAL*

    AliEveMacroExecutor* GetExecutor() const { return fExecutor; }

protected:
    Int_t         fEventId;		// Id of current event.

    AliRunLoader* fRunLoader;		// Run loader.

    TFile        *fESDFile;		// ESD file.
    TTree        *fESDTree;		// ESD tree.
    TTree        *fHLTESDTree;	// HLT ESD tree.
    AliESDEvent  *fESD;			// ESDEvent object.
    AliESDfriend *fESDfriend;		// ESDfriend object.
    Bool_t        fESDfriendExists;	// Flag specifying if ESDfriend was found during opening of the event-data.
    TFile        *fAODFile;		// AOD file.
    TTree        *fAODTree;		// AOD tree.
    AliAODEvent  *fAOD;			// AODEvent object.

    AliRawReader *fRawReader;             // Raw-data reader.
    AliEventInfo	fEventInfo;		// Current Event Info

    Bool_t        fAutoLoad;              // Automatic loading of events (online)
    Float_t       fAutoLoadTime;          // Auto-load time in seconds
    TTimer       *fAutoLoadTimer;         // Timer for automatic event loading

    Bool_t        fIsOpen;                // Are event-files opened.
    Bool_t        fHasEvent;              // Is an event available.
    Bool_t        fExternalCtrl;          // Are we under external event-loop.

    TMap*         fGlobal;
    Bool_t        fGlobalReplace;         // Are global replace
    Bool_t        fGlobalUpdate;          // Are global updates

    AliEveMacroExecutor *fExecutor;       // Executor for std macros

    TEveElementList     *fTransients;     // Container for additional transient (per event) elements.
    TEveElementList     *fTransientLists; // Container for lists of transient (per event) elements.

    AliEveEventSelector* fPEventSelector; // Event filter

    TList        *fSubManagers;           // Dependent event-managers, used for event embedding.

    static TString  fgGAliceFileName;        // galice.root file
    static TString  fgESDFileName;        // Name by which to open ESD.
    static EVisibleESDTrees  fgESDvisibleTrees; // trees to open from ESD
    static TString  fgESDfriendsFileName;
    static TString  fgAODFileName;        // Name by which to open AOD.
    static TString  fgRawFileName;        // Name by which to open raw-data file.
    static TString  fgCdbUri;		// Global URI to CDB.
    static Bool_t   fgAssertRunLoader;	// Global flag specifying if AliRunLoader must be asserted during opening of the event-data.
    static Bool_t   fgAssertESD;		// Global flag specifying if ESDEvent must be asserted during opening of the event-data.
    static Bool_t   fgAssertAOD;		// Global flag specifying if AODEvent must be asserted during opening of the event-data.
    static Bool_t   fgAssertRaw;		// Global flag specifying if raw-data presence must be asserted during opening of the event-data.

    static TList   *fgAODfriends;         // Global list of AOD friend names to be attached during opening of the event-data (empty by default).

    static Bool_t   fgRawFromStandardLoc; // Global flag to enable looking for raw data in ../../../raw/, as it is stored for central reco.

    static Bool_t        fgGRPLoaded;     // Global run parameters loaded?
    static AliMagF      *fgMagField;      // Global pointer to magnetic field.
    static AliRecoParam* fgRecoParam;
    static Bool_t        fgUniformField;  // Track with uniform field.

		//static zmq::context_t* fgSubContext;
		//static zmq::socket_t* fgSubSock;
private:
    AliEveEventManager(const AliEveEventManager&);            // Not implemented
    AliEveEventManager& operator=(const AliEveEventManager&); // Not implemented

    void InitInternals();
    void StartAutoLoadTimer();
    void StopAutoLoadTimer();

    static Bool_t InitGRP();
    static Bool_t InitRecoParam();

    TTree* readESDTree(const char* treeName, int &run);

    Bool_t fAutoLoadTimerRunning; // State of auto-load timer.

    static AliEveEventManager* fgMaster;
    static AliEveEventManager* fgCurrent;

    ClassDef(AliEveEventManager, 0); // Interface for getting all event components in a uniform way.
};

#endif
