// $Id: AliEveEventManager.cxx 64557 2013-10-16 20:03:08Z hristov $
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#include "AliEveEventManager.h"
#include "AliEveEventSelector.h"
#include "AliEveMacroExecutor.h"

#include <THashList.h>
#include <TEveElement.h>
#include <TEveManager.h>
#include <TEveViewer.h>

#include <AliLog.h>
#include <AliRunLoader.h>
#include <AliRun.h>
#include <AliESDRun.h>
#include <AliESDEvent.h>
#include <AliESDfriend.h>
#include <AliAODEvent.h>

#include <AliRecoParam.h>
#include <AliCentralTrigger.h>
#include <AliCDBEntry.h>
#include <AliTriggerClass.h>
#include <AliTriggerConfiguration.h>
#include <AliTriggerCluster.h>
#include <AliDetectorRecoParam.h>

#include <AliDAQ.h>
#include <AliRawEventHeaderBase.h>
#include <AliRawReaderRoot.h>
#include <AliRawReaderFile.h>
#include <AliRawReaderDate.h>
#include <AliMagF.h>
#include <AliCDBManager.h>
#include <AliCDBStorage.h>
#include <AliGRPObject.h>
#include <AliHeader.h>
#include <AliGeomManager.h>
#include <AliGRPManager.h>
#include <AliSysInfo.h>

#include <TFile.h>
#include <TTree.h>
#include <TGeoManager.h>
#include <TGeoGlobalMagField.h>
#include <TSystem.h>
#include <TTimeStamp.h>
#include <TPRegexp.h>
#include <TError.h>
#include <TEnv.h>
#include <TString.h>
#include <TMap.h>

#ifdef ZMQ
#include <AliNetMessage.h>
#include <AliSocket.h>
#endif

//==============================================================================
//==============================================================================
// AliEveEventManager
//==============================================================================

//______________________________________________________________________________
//
// Provides interface for loading and navigating standard AliRoot data
// (AliRunLoader), ESD, AOD and RAW.
//
// ESDfriend is attached automatically, if the file is found.
//
// AODfriends are not attached automatically as there are several
// possible files involved. To have a specific AODfriend attached, call
// static method
//   AliEveEventManager::AddAODfriend("AliAOD.VertexingHF.root");
// before initializing the event-manager.
//
// Also provides interface to magnetic-field and geometry. Mostly
// intended as wrappers over standard AliRoot functionality for
// convenient use from visualizateion macros.
//
// There can be a single main event-manger, it is stored in private
// data member fgMaster and can be accessed via static member function
// GetMaster().
//
// For event overlaying and embedding one can instantiate additional
// event-managers via static method AddDependentManager(const TString& path).
// This interface is under development.

ClassImp(AliEveEventManager)

Bool_t AliEveEventManager::fgAssertRunLoader = kFALSE;
Bool_t AliEveEventManager::fgAssertESD       = kFALSE;
Bool_t AliEveEventManager::fgAssertAOD       = kFALSE;
Bool_t AliEveEventManager::fgAssertRaw       = kFALSE;

TString  AliEveEventManager::fgESDFileName("AliESDs.root");
AliEveEventManager::EVisibleESDTrees  AliEveEventManager::fgESDvisibleTrees(AliEveEventManager::kOfflineTree);
TString  AliEveEventManager::fgESDfriendsFileName("AliESDfriends.root");
TString  AliEveEventManager::fgAODFileName("AliAOD.root");
TString  AliEveEventManager::fgGAliceFileName("galice.root");
TString  AliEveEventManager::fgRawFileName("raw.root");
TString  AliEveEventManager::fgCdbUri;

TList*   AliEveEventManager::fgAODfriends = 0;

Bool_t   AliEveEventManager::fgRawFromStandardLoc = kFALSE;

Bool_t   AliEveEventManager::fgGRPLoaded    = kFALSE;
AliMagF* AliEveEventManager::fgMagField     = 0;
AliRecoParam* AliEveEventManager::fgRecoParam = 0;
Bool_t   AliEveEventManager::fgUniformField = kFALSE;

AliEveEventManager* AliEveEventManager::fgMaster  = 0;
AliEveEventManager* AliEveEventManager::fgCurrent = 0;

zmq::context_t* AliEveEventManager::fgSubContext=0;
AliSocket* AliEveEventManager::fgSubSock=0;

bool AliEveEventManager::ConnectToServer(const char* host, int port)
{

#ifdef ZMQ
	// make a zeromq socket
    fgSubContext = new zmq::context_t;
    fgSubSock = new AliSocket(&*fgSubContext, ZMQ_SUB);
    fgSubSock->Subscribe("");
    fgSubSock->Connect(Form("%s:%d", host, port) );
#else
	fgSubContext=0;
	fgSubSock =0;
#endif
	
  return true;
}

void AliEveEventManager::InitInternals()
{
    // Initialize internal members.

    static const TEveException kEH("AliEveEventManager::InitInternals ");

    if (fgCurrent != 0)
    {
        throw(kEH + "Dependent event-managers should be created via static method AddDependentManager().");
    }

    if (fgMaster == 0)
    {
        fgMaster = this;
    }

    fgCurrent = this;

    fAutoLoadTimer = new TTimer;
    fAutoLoadTimer->Connect("Timeout()", "AliEveEventManager", this, "AutoLoadNextEvent()");
    fAutoLoadTimer->Connect("Timeout()", "AliEveEventManager", this, "Timeout()");

    fExecutor = new AliEveMacroExecutor;

    fTransients = new TEveElementList("Transients", "Transient per-event elements.");
    fTransients->IncDenyDestroy();
    gEve->AddToListTree(fTransients, kFALSE);

    fTransientLists = new TEveElementList("Transient Lists", "Containers of transient elements.");
    fTransientLists->IncDenyDestroy();
    gEve->AddToListTree(fTransientLists, kFALSE);

    fPEventSelector = new AliEveEventSelector(this);

    fGlobal = new TMap; fGlobal->SetOwnerKeyValue();
}

AliEveEventManager::AliEveEventManager(const TString& name, Int_t ev) :
    TEveEventManager(name, ""),

    fEventId(-1),
    fRunLoader (0),
    fESDFile   (0), fESDTree (0), fHLTESDTree(0), fESD (0),
    fESDfriend (0), fESDfriendExists(kFALSE),
    fAODFile   (0), fAODTree (0), fAOD (0),
    fRawReader (0), fEventInfo(),
    fAutoLoad  (kFALSE), fAutoLoadTime (5),      fAutoLoadTimer(0),
    fIsOpen    (kFALSE), fHasEvent     (kFALSE), fExternalCtrl (kFALSE),
    fGlobal    (0), fGlobalReplace (kTRUE), fGlobalUpdate (kTRUE),
    fExecutor    (0), fTransients(0), fTransientLists(0),
    fPEventSelector(0),
    fSubManagers (0),
    fAutoLoadTimerRunning(kFALSE)
{
    // Constructor with event-id.

    InitInternals();

    Open();
    if (ev >= 0)
    {
        GotoEvent(ev);
    }
}

AliEveEventManager::~AliEveEventManager()
{
    // Destructor.
    fAutoLoadTimer->Stop();
    fAutoLoadTimer->Disconnect("Timeout");

    delete fSubManagers;

    if (fIsOpen)
    {
        Close();
    }

    fTransients->DecDenyDestroy();
    fTransients->Destroy();

    fTransientLists->DecDenyDestroy();
    fTransientLists->Destroy();

    //delete fExecutor;
}

/******************************************************************************/

void AliEveEventManager::SetESDFileName(const TString& esd, EVisibleESDTrees shown)
{
    fgESDvisibleTrees = shown;
    // Set file-name for opening ESD, default "AliESDs.root".
    if (esd.IsNull()) return;

    fgESDFileName = esd;
    if (esd.EndsWith(".zip")) fgESDFileName.Form("%s#AliESDs.root",esd.Data());

}

void AliEveEventManager::SetESDfriendFileName(const TString& esdf)
{
    // Set file-name for opening ESD friend, default "AliESDfriends.root".

    if (esdf.IsNull()) return;
    fgESDfriendsFileName = esdf;
    
    if (esdf.EndsWith(".zip")) fgESDfriendsFileName.Form("%s#AliESDfriends.root",esdf.Data());
}

void AliEveEventManager::SetAODFileName(const TString& aod)
{
    // Set file-name for opening AOD, default "AliAOD.root".

    if (aod.IsNull()) return;
    fgAODFileName = aod;
    
    if (aod.EndsWith(".zip")) fgAODFileName.Form("%s#AliAOD.root",aod.Data());
    
}

void AliEveEventManager::AddAODfriend(const TString& friendFileName)
{
    // Add new AOD friend file-name to be attached when opening AOD.
    // This should include '.root', as in 'AliAOD.VertexingHF.root'.

    if (fgAODfriends == 0)
    {
        fgAODfriends = new TList;
        fgAODfriends->SetOwner(kTRUE);
    }
    if (fgAODfriends->FindObject(friendFileName) == 0)
    {
        fgAODfriends->Add(new TObjString(friendFileName));
    }
}

void AliEveEventManager::SetRawFileName(const TString& raw)
{
    // Set file-name for opening of raw-data, default "raw.root"
    if (raw.IsNull()) return;
    
    fgRawFileName = raw;
}

void AliEveEventManager::SetCdbUri(const TString& cdb)
{
    // Set path to CDB, there is no default.

    if ( ! cdb.IsNull()) fgCdbUri = cdb;
}

void AliEveEventManager::SetGAliceFileName(const TString& galice)
{
    // Set file-name for opening gAlice, default "galice.root".

    if ( galice.IsNull()) return;
    fgGAliceFileName = galice;
    
    if (galice.EndsWith(".zip")) fgGAliceFileName.Form("%s#galice.root",galice.Data());
}

void AliEveEventManager::SetFilesPath(const TString& urlPath)
{
    TString path = urlPath;
    gSystem->ExpandPathName(path);
    if (path.IsNull() || path == ".")
    {
        path = gSystem->WorkingDirectory();
    }

    TString sep;
    if(path.EndsWith(".zip")) // if given a path to root_archive.zip
        sep= "#";
    else if(!path.EndsWith("/"))
        sep = "/";
    
    SetESDFileName( TString(Form("%s%sAliESDs.root", path.Data(), sep.Data())) );
    SetESDfriendFileName(  TString(Form("%s%sAliESDfriends.root", path.Data(), sep.Data())) );
    SetAODFileName(  TString(Form("%s%sAliAOD.root", path.Data(), sep.Data())) );
    AddAODfriend(  TString(Form("%s%sAliAOD.VertexingHF.root", path.Data(), sep.Data())) );
    SetGAliceFileName( TString(Form("%s%sgalice.root", path.Data(), sep.Data())) );
    SetRawFileName(TString(Form("%s%sraw.root", path.Data(), sep.Data())));
}

void AliEveEventManager::SetAssertElements(Bool_t assertRunloader, Bool_t assertEsd,
                                           Bool_t assertAod, Bool_t assertRaw)
{
    // Set global flags that detrmine which parts of the event-data must
    // be present when the event is opened.

    fgAssertRunLoader = assertRunloader;
    fgAssertESD = assertEsd;
    fgAssertAOD = assertAod;
    fgAssertRaw = assertRaw;
}

void AliEveEventManager::SearchRawForCentralReconstruction()
{
    // Enable searching of raw data in standard location. The path passed to
    // Open() is expected to point to a centrally reconstructed run, e.g.:
    // "alien:///alice/data/2009/LHC09c/000101134/ESDs/pass1/09000101134018.10".

    fgRawFromStandardLoc = kTRUE;
}

/******************************************************************************/

void AliEveEventManager::Open()
{
    // Open event-data from URL specified in path.
    // Attempts to create AliRunLoader() and to open ESD with ESDfriends.
    // Warning is reported if run-loader or ESD is not found.
    // Global data-members fgAssertRunLoader and fgAssertESD can be set
    // to throw exceptions instead.

    static const TEveException kEH("AliEveEventManager::Open ");

    if (fExternalCtrl)
    {
        throw (kEH + "Event-loop is under external control.");
    }
    if (fIsOpen)
    {
        throw (kEH + "Event-files already opened.");
    }

    Int_t runNo = -1;

    // Open ESD and ESDfriends
    
    if ((fESDFile = TFile::Open(fgESDFileName)))
    {
        fESD = new AliESDEvent();

        switch(fgESDvisibleTrees){
        case AliEveEventManager::kOfflineTree :
            fESDTree = readESDTree("esdTree", runNo);
            break;
        case AliEveEventManager::kHLTTree :
            fHLTESDTree = readESDTree("HLTesdTree", runNo);
            break;
        default:
            fESDTree    = readESDTree("esdTree", runNo);
            fHLTESDTree = readESDTree("HLTesdTree", runNo);
        }

        if(!fESDTree && !fHLTESDTree){
            // both ESD trees are == 0
            delete fESDFile; fESDFile = 0;
            delete fESD; fESD = 0;
        }


    }
    else // esd file not readable
    {
        Warning(kEH, "can not read ESD file '%s'.", fgESDFileName.Data());
    }
    if (fESDTree == 0 && fHLTESDTree==0)
    {
        if (fgAssertESD)
        {
            throw (kEH + "ESD not initialized. Its precence was requested.");
        } else {
            Warning(kEH, "ESD not initialized.");
        }
    }

    // Open AOD and registered friends
    if ( (fAODFile = TFile::Open(fgAODFileName)) )
    {
        fAOD = new AliAODEvent();
        fAODTree = (TTree*) fAODFile->Get("aodTree");
        if (fAODTree != 0)
        {
            // Check if AODfriends exist and attach them.
            TIter       friends(fgAODfriends);
            TObjString *name;
            while ((name = (TObjString*) friends()) != 0)
            {
                TString p(Form("%s/%s", fgAODFileName.Data(), name->GetName()));
                if (fgAODFileName.EndsWith(".zip")) p.Form("%s#%s",fgAODFileName.Data(),name->GetName());
                if (gSystem->AccessPathName(p, kReadPermission) == kFALSE)
                {
                    fAODTree->AddFriend("aodTree", name->GetName());
                }
            }

            fAOD->ReadFromTree(fAODTree);

            if (fAODTree->GetEntry(0) <= 0)
            {
                delete fAODFile; fAODFile = 0;
                delete fAOD;     fAOD     = 0;
                Warning(kEH, "failed getting the first entry from addTree.");
            }
            else
            {
                if (runNo < 0)
                    runNo = fAOD->GetRunNumber();
            }
        }
        else // aodtree == 0
        {
            delete fAODFile; fAODFile = 0;
            delete fAOD;     fAOD     = 0;
            Warning(kEH, "failed getting the aodTree.");
        }
    }
    else // aod not readable
    {
        Warning(kEH, "can not read AOD file '%s'.", fgAODFileName.Data());
    }
    if (fAODTree == 0)
    {
        if (fgAssertAOD)
        {
            throw (kEH + "AOD not initialized. Its precence was requested.");
        } else {
            Warning(kEH, "AOD not initialized.");
        }
    }

    // Open RunLoader from galice.root
    TFile *gafile = TFile::Open(fgGAliceFileName);
    if (gafile)
    {
        gafile->Close();
        delete gafile;
        fRunLoader = AliRunLoader::Open(fgGAliceFileName, GetName());
        if (fRunLoader)
        {
            TString alicePath(gSystem->DirName(fgGAliceFileName));
            alicePath.Append("/");
            fRunLoader->SetDirName(alicePath);

            if (fRunLoader->LoadgAlice() != 0)
                Warning(kEH, "failed loading gAlice via run-loader.");

            if (fRunLoader->LoadHeader() == 0)
            {
                if (runNo < 0)
                    runNo = fRunLoader->GetHeader()->GetRun();
            }
            else
            {
                Warning(kEH, "failed loading run-loader's header.");
                delete fRunLoader;
                fRunLoader = 0;
            }
        }
        else // run-loader open failed
        {
            Warning(kEH, "failed opening ALICE run-loader from '%s'.", fgGAliceFileName.Data());
        }
        
    }
    else // galice not readable
    {
        Warning(kEH, "can not read '%s'.", fgGAliceFileName.Data());
    }
    if (fRunLoader == 0)
    {
        if (fgAssertRunLoader)
            throw (kEH + "Bootstraping of run-loader failed. Its precence was requested.");
        else
            Warning(kEH, "Bootstraping of run-loader failed.");
    }

    // Open raw-data file

    TString rawPath;
    if (fgRawFromStandardLoc)
    {
        if (!fgRawFileName.BeginsWith("alien:"))
            throw kEH + "Standard raw search requested, but the directory is not in AliEn.";
        if (!fgRawFileName.Contains("/ESDs/"))
            throw kEH + "Standard raw search requested, but does not contain 'ESDs' directory.";

        TPMERegexp chunk("/([\\d\\.])+/?$");
        Int_t nm = chunk.Match(fgRawFileName);
        if (nm != 2)
            throw kEH + "Standard raw search requested, but the path does not end with chunk-id directory.";

        TPMERegexp esdstrip("/ESDs/.*");
        rawPath = fgRawFileName;
        esdstrip.Substitute(rawPath, "/raw/");
        rawPath += chunk[0];
        rawPath += ".root";

        Info(kEH, "Standard raw search requested, using the following path:\n  %s\n", rawPath.Data());
    }
    else
    {
        rawPath = fgRawFileName;
    }
    // If i use open directly, raw-reader reports an error but i have
    // no way to detect it.
    // Is this (AccessPathName check) ok for xrootd / alien? Yes, not for http.
    AliLog::EType_t oldLogLevel = (AliLog::EType_t) AliLog::GetGlobalLogLevel();
    if (fgAssertRaw == kFALSE)
    {
        AliLog::SetGlobalLogLevel(AliLog::kFatal);
    }
    if (gSystem->AccessPathName(rawPath, kReadPermission) == kFALSE)
    {
        fRawReader = AliRawReader::Create(rawPath);
    }
    else
    {
        fRawReader = AliRawReader::Create(fgRawFileName);
    }
    if (fgAssertRaw == kFALSE)
    {
        AliLog::SetGlobalLogLevel(oldLogLevel);
    }

    if (fRawReader == 0)
    {
        if (fgAssertRaw)
        {
            throw (kEH + "raw-data not initialized. Its precence was requested.");
        }
        else
        {
            Warning(kEH, "raw-data not initialized.");
        }
    }

    if (runNo < 0)
    {
        if (fRawReader)
        {
            if ( ! fRawReader->NextEvent())
            {
                throw (kEH + "can not go to first event in raw-reader to determine run-id.");
            }
            runNo = fRawReader->GetRunNumber();
            Info(kEH, "Determining run-no from raw ... run=%d.", runNo);
            fRawReader->RewindEvents();
        }
        else
        {
            fExternalCtrl = kTRUE;
             fEventId = 0;
            return;
        }
    }

    // Initialize OCDB ... only in master event-manager
		InitOCDB(runNo);

    fIsOpen = kTRUE;
}

void AliEveEventManager::InitOCDB(int runNo)
{
		static const TEveException kEH("AliEveEventManager::InitOCDB ");
    //if (this == fgMaster)
    {
        AliCDBManager* cdb = AliCDBManager::Instance();
        if (cdb->IsDefaultStorageSet() == kTRUE)
        {
            Warning(kEH, "CDB already set - using the old storage:\n  '%s'",
                    cdb->GetDefaultStorage()->GetURI().Data());
        }
        else
        {
            if (fgCdbUri.IsNull())
            {
                gEnv->SetValue("Root.Stacktrace", "no");
                Fatal("Open()", "OCDB path was not specified.");
            }

            // Handle some special cases for MC (should be in OCDBManager).
            if (fgCdbUri == "mcideal://")
                cdb->SetDefaultStorage("MC", "Ideal");
            else if (fgCdbUri == "mcresidual://")
                cdb->SetDefaultStorage("MC", "Residual");
            else if (fgCdbUri == "mcfull://")
                cdb->SetDefaultStorage("MC", "Full");
            else if (fgCdbUri == "local://") {
                fgCdbUri = Form("local://%s/OCDB", gSystem->Getenv("ALICE_ROOT"));
                cdb->SetDefaultStorage(fgCdbUri);
            } else
                cdb->SetDefaultStorage(fgCdbUri);

            cdb->SetRun(runNo);

            if (cdb->IsDefaultStorageSet() == kFALSE)
                throw kEH + "CDB initialization failed for '" + fgCdbUri + "'.";
        }

        if (fgCdbUri.BeginsWith("local://"))
        {
            TString curPath = gSystem->WorkingDirectory();
            TString grp     = "GRP/GRP/Data";
            TString grppath = curPath + "/" + grp;
            if (gSystem->AccessPathName(grppath, kReadPermission) == kFALSE)
            {
                if (cdb->GetSpecificStorage(grp))
                {
                    Warning(kEH, "Local GRP exists, but the specific storage is already set.");
                }
                else
                {
                    Info(kEH, "Setting CDB specific-storage for GRP from event directory.");
                    TString lpath("local://");
                    lpath += curPath;
                    cdb->SetSpecificStorage(grp, lpath);
                }
            }
        }
    }
}

void AliEveEventManager::SetEvent(AliRunLoader *runLoader, AliRawReader *rawReader, AliESDEvent *esd, AliESDfriend *esdf)
{
    // Set an event from an external source.
    // The method is used in the online visualisation.
    // AOD is not supported.

    static const TEveException kEH("AliEveEventManager::SetEvent ");

    if (fIsOpen)
    {
        Warning(kEH, "Event-files were open. Closing and switching to external control.");
        Close();
    }

			Info(kEH,"setting it!!! ============================");

    fRunLoader = runLoader;
    fRawReader = rawReader;
    fESD       = esd;
    fESDfriend = esdf;
    fAOD       = 0;

    fEventId++;
    fHasEvent     = kTRUE;
    fExternalCtrl = kTRUE;

    SetTitle("Online event in memory");
    SetName ("Online Event");
    ElementChanged();

    AfterNewEventLoaded();

    if (fAutoLoad) StartAutoLoadTimer();
    
}

Int_t AliEveEventManager::GetMaxEventId(Bool_t refreshESD) const
{
    // Returns maximum available event id.
    // If under external control or event is not opened -1 is returned.
    // If raw-data is the only data-source this can not be known
    // and 10,000,000 is returned.
    // If neither data-source is initialised an exception is thrown.
    // If refresh_esd is true and ESD is the primary event-data source
    // its header is re-read from disk.

    static const TEveException kEH("AliEveEventManager::GetMaxEventId ");

    if (fExternalCtrl || fIsOpen == kFALSE)
    {
        return -1;
    }

    if ((fESDTree!=0) || (fHLTESDTree!=0))
    {
        if (refreshESD)
        {
            if(fESDTree!=0) fESDTree->Refresh();
            if(fHLTESDTree!=0) fHLTESDTree->Refresh();
            fPEventSelector->Update();
        }

        Int_t maxEventId=0;
        switch(fgESDvisibleTrees){
        default:
        case AliEveEventManager::kOfflineTree :
            maxEventId = fESDTree->GetEntries() - 1;
            break;
        case AliEveEventManager::kHLTTree :
            maxEventId = fHLTESDTree->GetEntries() - 1;
            break;
       }

        return maxEventId;
    }
    else if (fAODTree)
    {
        return fAODTree->GetEntries() - 1;
    }
    else if (fRunLoader)
    {
        return fRunLoader->GetNumberOfEvents() - 1;
    }
    else if (fRawReader)
    {
        Int_t n = fRawReader->GetNumberOfEvents() - 1;
        return n > -1 ? n : 10000000;
    }
    else
    {
        throw (kEH + "neither ESD, AOD, RunLoader nor Raw loaded.");
    }
}

void AliEveEventManager::GotoEvent(Int_t event)
{
    // Load data for specified event.
    // If event is out of range an exception is thrown and old state
    // is preserved.
    // After successful loading of event, the virtual function
    // AfterNewEventLoaded() is called. This executes commands that
    // were registered via TEveEventManager::AddNewEventCommand().
    //
    // If event is negative, it is subtracted from the number of
    // available events, thus passing -1 will load the last event.
    // This is not supported when raw-data is the only data-source
    // as the number of events is not known.

    static const TEveException kEH("AliEveEventManager::GotoEvent ");

    if (fAutoLoadTimerRunning)
    {
        throw (kEH + "Event auto-load timer is running.");
    }
    if (fExternalCtrl)
    {
        throw (kEH + "Event-loop is under external control.");
    }
    else if (!fIsOpen)
    {
        throw (kEH + "Event-files not opened.");
    }

    fEventInfo.Reset();

    fHasEvent = kFALSE;

    Int_t maxEvent = 0;
    if ((fESDTree!=0) || (fHLTESDTree!=0))
    {
        if(fESDTree){
            if (event >= fESDTree->GetEntries())
                fESDTree->Refresh();
            maxEvent = fESDTree->GetEntries() - 1;
            if (event < 0)
                event = fESDTree->GetEntries() + event;
        }

        if(fHLTESDTree){
            if (event >= fHLTESDTree->GetEntries())
                fHLTESDTree->Refresh();
            maxEvent = fHLTESDTree->GetEntries() - 1;
            if (event < 0)
                event = fHLTESDTree->GetEntries() + event;

        }
    }
    else if (fAODTree)
    {
        maxEvent = fAODTree->GetEntries() - 1;
        if (event < 0)
            event = fAODTree->GetEntries() + event;
    }
    else if (fRunLoader)
    {
        maxEvent = fRunLoader->GetNumberOfEvents() - 1;
        if (event < 0)
            event = fRunLoader->GetNumberOfEvents() + event;
    }
    else if (fRawReader)
    {
        maxEvent = fRawReader->GetNumberOfEvents() - 1;
        if (maxEvent < 0)
        {
            maxEvent = 10000000;
            if (event < 0) {
                Error(kEH, "current raw-data source does not support direct event access.");
                return;
            }
            Info(kEH, "number of events unknown for current raw-data source, setting max-event id to 10M.");
        }
        else
        {
            if (event < 0)
                event = fRawReader->GetNumberOfEvents() + event;
        }
    }
    else
    {
        throw (kEH + "neither RunLoader, ESD nor Raw loaded.");
    }
    if (event < 0 || event > maxEvent)
    {
        throw (kEH + Form("event %d not present, available range [%d, %d].",
                          event, 0, maxEvent));
    }

    TString sysInfoHeader;
    sysInfoHeader.Form("AliEveEventManager::GotoEvent(%d) - ", event);
    AliSysInfo::AddStamp(sysInfoHeader + "Start");

    TEveManager::TRedrawDisabler rd(gEve);
    gEve->Redraw3D(kFALSE, kTRUE); // Enforce drop of all logicals.

    // !!! MT this is somewhat brutal; at least optionally, one could be
    // a bit gentler, checking for objs owning their external refs and having
    // additinal parents.
    gEve->GetViewers()->DeleteAnnotations();
    fTransients->DestroyElements();
    for (TEveElement::List_i i = fTransientLists->BeginChildren();
         i != fTransientLists->EndChildren(); ++i)
    {
        (*i)->DestroyElements();
    }
    DestroyElements();

    AliSysInfo::AddStamp(sysInfoHeader + "PostDestroy");

    if (fESDTree) {
        if (fESDTree->GetEntry(event) <= 0)
            throw (kEH + "failed getting required event from ESD.");

        if (fESDfriendExists)
            fESD->SetESDfriend(fESDfriend);
    }

    if (fHLTESDTree) {
        if (fHLTESDTree->GetEntry(event) <= 0)
            throw (kEH + "failed getting required event from HLT ESD.");

        if (fESDfriendExists)
            fESD->SetESDfriend(fESDfriend);
    }

    if (fAODTree) {
        if (fAODTree->GetEntry(event) <= 0)
            throw (kEH + "failed getting required event from AOD.");
    }

    if (fRunLoader) {
        if (fRunLoader->GetEvent(event) != 0)
            throw (kEH + "failed getting required event.");
    }

    if (fRawReader)
    {
        // AliRawReader::GotoEvent(Int_t) works for AliRawReaderRoot/Chain.
        if (fRawReader->GotoEvent(event) == kFALSE)
        {
            // Use fallback method - iteration with NextEvent().
            Int_t rawEv = fEventId;
            if (event < rawEv)
            {
                fRawReader->RewindEvents();
                rawEv = -1;
            }

            while (rawEv < event)
            {
                if ( ! fRawReader->NextEvent())
                {
                    fRawReader->RewindEvents();
                    fEventId = -1;
                    throw (kEH + Form("Error going to next raw-event from event %d.", rawEv));
                }
                ++rawEv;
            }
            Warning(kEH, "Loaded raw-event %d with fallback method.\n", rawEv);
        }
    }

    fHasEvent = kTRUE;
    fEventId  = event;
    if (this == fgMaster)
    {
        SetName(Form("Event %d", fEventId));
        ElementChanged();
    }

    AliSysInfo::AddStamp(sysInfoHeader + "PostLoadEvent");

    AfterNewEventLoaded();

    AliSysInfo::AddStamp(sysInfoHeader + "PostUserActions");
}

void AliEveEventManager::Timeout()
{
    Emit("Timeout()");
}

void AliEveEventManager::NextEvent()
{
    // Loads next event.
    // Does magick needed for online display when under external event control.

    static const TEveException kEH("AliEveEventManager::NextEvent ");

    if (fAutoLoadTimerRunning)
    {
        throw (kEH + "Event auto-load timer is running.");
    }

    if (fExternalCtrl)
    {
        // !!! This should really go somewhere else. It is done in GotoEvent(),
        // so here we should do it in SetEvent().
          DestroyElements();
        
        		#ifdef ZMQ
        		AliNetMessage *mess;
        		fgSubSock->Recv(mess);
        
        
				    AliESDEvent* data = (AliESDEvent*)(mess->ReadObjectAny(AliESDEvent::Class()));

          	if (data){
          	    printf("Got DATA:\n");

          	    printf("Event Number in File:%d Run:%d ObjectsInList:%d\n", data->GetEventNumberInFile(),  data->GetRunNumber(), data->GetList()->GetEntries());
          	   	
          	   	TTree* tree= new TTree("esdTree", "esdTree");
          	    data->WriteToTree(tree);
          	    tree-> Fill();
          	    //tree->Write();
          	   	
          	   	printf("======================= Tree has %d entries\n", tree->GetEntries());
          	   	
          	   	AliESDEvent* event= new AliESDEvent();
								event->ReadFromTree(tree);
							
					     	tree->GetEntry(0);
          	   	
          	   	InitOCDB(event->GetRunNumber());
          	   	
          	   	printf("======================= setting event to %d\n", fEventId);
          	   	SetEvent(0,0,event,0);
        	    
		       	    delete event;
        	    	delete tree;
        	    	
          	}
          	else
          	   printf("NO DATA!!!\n");
        	          	     
        	delete data; data=0;
        	delete mess; mess=0;
        	#endif
        	
    }
    else if ((fESDTree!=0) || (fHLTESDTree!=0))
    {
        Int_t nextevent=0;
        if (fPEventSelector->FindNext(nextevent))
        {
            GotoEvent(nextevent);
        }
    }
    else if (fEventId < GetMaxEventId(kTRUE))
    {
        GotoEvent(fEventId + 1);
    }
    
     gSystem->ProcessEvents();
}

void AliEveEventManager::PrevEvent()
{
    // Loads previous event.

    static const TEveException kEH("AliEveEventManager::PrevEvent ");

    if (fAutoLoadTimerRunning)
    {
        throw (kEH + "Event auto-load timer is running.");
    }
    if (fExternalCtrl)
    {
        throw (kEH + "Event-loop is under external control.");
    }

    if ((fESDTree!=0) || (fHLTESDTree!=0))
    {
        Int_t nextevent=0;
        if (fPEventSelector->FindPrev(nextevent))
        {
            GotoEvent(nextevent);
        }
    }
    else if (fEventId > 0)
    {
        GotoEvent(fEventId - 1);
    }
}

void AliEveEventManager::Close()
{
    // Close the event data-files and delete ESD, ESDfriend, run-loader
    // and raw-reader.

    static const TEveException kEH("AliEveEventManager::Close ");

    if (!fIsOpen)
    {
        throw (kEH + "Event-files not opened.");
    }

    if (fAutoLoadTimerRunning)
        StopAutoLoadTimer();

    if ((fESDTree!=0) || (fHLTESDTree!=0)) {
        delete fESD;       fESD       = 0;
        // delete fESDfriend; // friend tree is deleted with the tree
        fESDfriend = 0;
        fESDfriendExists = kFALSE;

        if(fESDTree) { delete fESDTree;   fESDTree = 0; }
        if(fHLTESDTree) { delete fHLTESDTree;   fHLTESDTree = 0; }
        delete fESDFile;   fESDFile = 0;
    }

    if (fAODTree) {
        delete fAOD;       fAOD       = 0;

        delete fAODTree;   fAODTree = 0;
        delete fAODFile;   fAODFile = 0;
    }

    if (fRunLoader) {
        delete fRunLoader; fRunLoader = 0;
    }

    if (fRawReader) {
        delete fRawReader; fRawReader = 0;
    }

    fEventId  = -1;
    fIsOpen   = kFALSE;
    fHasEvent = kFALSE;
}


//------------------------------------------------------------------------------
// Static convenience functions, mainly used from macros.
//------------------------------------------------------------------------------

Int_t AliEveEventManager::CurrentEventId()
{
    // Return current event-id.

    static const TEveException kEH("AliEveEventManager::CurrentEventId ");

    if (fgCurrent == 0 || fgCurrent->fHasEvent == kFALSE)
        throw (kEH + "ALICE event not ready.");
    return fgCurrent->GetEventId();
}

Bool_t AliEveEventManager::HasRunLoader()
{
    // Check if AliRunLoader is initialized.

    return fgCurrent && fgCurrent->fHasEvent && fgCurrent->fRunLoader;
}

Bool_t AliEveEventManager::HasESD()
{
    // Check if AliESDEvent is initialized.

    return fgCurrent && fgCurrent->fHasEvent && fgCurrent->fESD;
}

Bool_t AliEveEventManager::HasESDfriend()
{
    // Check if AliESDfriend is initialized.

    return fgCurrent && fgCurrent->fHasEvent && fgCurrent->fESDfriend;
}

Bool_t AliEveEventManager::HasAOD()
{
    // Check if AliESDEvent is initialized.

    return fgCurrent && fgCurrent->fHasEvent && fgCurrent->fAOD;
}

Bool_t AliEveEventManager::HasRawReader()
{
    // Check if raw-reader is initialized.

    return fgCurrent && fgCurrent->fHasEvent && fgCurrent->fRawReader;
}

AliRunLoader* AliEveEventManager::AssertRunLoader()
{
    // Make sure AliRunLoader is initialized and return it.
    // Throws exception in case run-loader is not available.
    // Static utility for macros.

    static const TEveException kEH("AliEveEventManager::AssertRunLoader ");

    if (fgCurrent == 0 || fgCurrent->fHasEvent == kFALSE)
        throw (kEH + "ALICE event not ready.");
    if (fgCurrent->fRunLoader == 0)
        throw (kEH + "AliRunLoader not initialised.");
    return fgCurrent->fRunLoader;
}

AliESDEvent* AliEveEventManager::AssertESD()
{
    // Make sure AliESDEvent is initialized and return it.
    // Throws exception in case ESD is not available.
    // Static utility for macros.

    static const TEveException kEH("AliEveEventManager::AssertESD ");

    if (fgCurrent == 0 || fgCurrent->fHasEvent == kFALSE)
        throw (kEH + "ALICE event not ready.");
    if (fgCurrent->fESD == 0)
        throw (kEH + "AliESD not initialised.");
    return fgCurrent->fESD;
}

AliESDfriend* AliEveEventManager::AssertESDfriend()
{
    // Make sure AliESDfriend is initialized and return it.
    // Throws exception in case ESDfriend-loader is not available.
    // Static utility for macros.

    static const TEveException kEH("AliEveEventManager::AssertESDfriend ");

    if (fgCurrent == 0 || fgCurrent->fHasEvent == kFALSE)
        throw (kEH + "ALICE event not ready.");
    if (fgCurrent->fESDfriend == 0)
        throw (kEH + "AliESDfriend not initialised.");
    return fgCurrent->fESDfriend;
}

AliAODEvent* AliEveEventManager::AssertAOD()
{
    // Make sure AliAODEvent is initialized and return it.
    // Throws exception in case AOD is not available.
    // Static utility for macros.

    static const TEveException kEH("AliEveEventManager::AssertAOD ");

    if (fgCurrent == 0 || fgCurrent->fHasEvent == kFALSE)
        throw (kEH + "ALICE event not ready.");
    if (fgCurrent->fAOD == 0)
        throw (kEH + "AliAOD not initialised.");
    return fgCurrent->fAOD;
}

AliRawReader* AliEveEventManager::AssertRawReader()
{
    // Make sure raw-reader is initialized and return it.

    static const TEveException kEH("AliEveEventManager::AssertRawReader ");

    if (fgCurrent == 0 || fgCurrent->fHasEvent == kFALSE)
        throw (kEH + "ALICE event not ready.");
    if (fgCurrent->fRawReader == 0)
        throw (kEH + "RawReader not ready.");

    return fgCurrent->fRawReader;
}

//==============================================================================

AliMagF* AliEveEventManager::AssertMagField() 	 
{ 	 
    // Make sure AliMagF is initialized and returns it.
    // Throws exception in case magnetic field is not available.
    // Static utility for macros.

    static const TEveException kEH("AliEveEventManager::AssertMagField ");

    if (fgMagField)
        return fgMagField;

    if (TGeoGlobalMagField::Instance()->GetField())
    {
        fgMagField = dynamic_cast<AliMagF*>(TGeoGlobalMagField::Instance()->GetField());
        if (fgMagField == 0)
            throw kEH + "Global field set, but it is not AliMagF.";
        return fgMagField;
    }

    if (!fgGRPLoaded)
    {
        InitGRP();
    }

    if (TGeoGlobalMagField::Instance()->GetField())
    {
        fgMagField = dynamic_cast<AliMagF*>(TGeoGlobalMagField::Instance()->GetField());
        if (fgMagField == 0)
            throw kEH + "Global field set, but it is not AliMagF.";
    }
    else
    {
        throw kEH + "Could not initialize magnetic field.";
    }

    return fgMagField;
}

TGeoManager* AliEveEventManager::AssertGeometry()
{
    // Make sure AliGeomManager is initialized and returns the
    // corresponding TGeoManger.
    // gGeoManager is set to the return value.
    // Throws exception if geometry can not be loaded or if it is not
    // available and the TGeoManager is locked.
    // Static utility for macros.

    static const TEveException kEH("AliEveEventManager::AssertGeometry ");

    if (AliGeomManager::GetGeometry() == 0)
    {
        if (TGeoManager::IsLocked())
            throw (kEH + "geometry is not loaded but TGeoManager is locked.");

        gGeoManager = 0;
        AliGeomManager::LoadGeometry();
        if ( ! AliGeomManager::GetGeometry())
        {
            throw (kEH + "can not load geometry.");
        }
        if ( ! AliGeomManager::ApplyAlignObjsFromCDB("ITS TPC TRD TOF PHOS HMPID EMCAL MUON FMD ZDC PMD T0 VZERO ACORDE"))
        {
            ::Warning(kEH, "mismatch of alignable volumes. Proceeding.");
            // throw (kEH + "could not apply align objs.");
        }
        AliGeomManager::GetGeometry()->DefaultColors();
    }

    gGeoManager = AliGeomManager::GetGeometry();
    return gGeoManager;
}

AliRecoParam* AliEveEventManager::AssertRecoParams()
{
    if(!fgRecoParam)
        InitRecoParam();

    return fgRecoParam;
}

AliSocket* AliEveEventManager::AssertSubscriber()
{
	return fgSubSock;
}

Bool_t AliEveEventManager::InitRecoParam()
{
    // This is mostly a reap-off from reconstruction
    // The method accesses OCDB and retrieves all
    // the available reco-param objects from there.

    fgRecoParam = new AliRecoParam;
    const Int_t  kNDetectors = 14;

    static const TEveException kEH("AliEveEventManager::InitRecoParam");

    Bool_t isOK = kTRUE;

    if (fgRecoParam->GetDetRecoParamArray(kNDetectors)) {
        ::Info(kEH, "Using custom GRP reconstruction parameters");
    }
    else {
        ::Info(kEH, "Loading GRP reconstruction parameter objects");

        AliCDBPath path("GRP","Calib","RecoParam");
        AliCDBEntry *entry=AliCDBManager::Instance()->Get(path.GetPath());
        if(!entry){
            ::Warning(kEH, "Couldn't find GRP RecoParam entry in OCDB");
            isOK = kFALSE;
        }
        else {
            TObject *recoParamObj = entry->GetObject();
            if (dynamic_cast<TObjArray*>(recoParamObj)) {
                // GRP has a normal TobjArray of AliDetectorRecoParam objects
                // Registering them in AliRecoParam
                fgRecoParam->AddDetRecoParamArray(kNDetectors,dynamic_cast<TObjArray*>(recoParamObj));
            }
            else if (dynamic_cast<AliDetectorRecoParam*>(recoParamObj)) {
                // GRP has only onse set of reco parameters
                // Registering it in AliRecoParam
                ::Info(kEH, "Single set of GRP reconstruction parameters found");
                dynamic_cast<AliDetectorRecoParam*>(recoParamObj)->SetAsDefault();
                fgRecoParam->AddDetRecoParam(kNDetectors,dynamic_cast<AliDetectorRecoParam*>(recoParamObj));
            }
            else {
                ::Error(kEH, "No valid GRP RecoParam object found in the OCDB");
                isOK = kFALSE;
            }
            entry->SetOwner(0);
        }
    }

    const char* fgkDetectorName[kNDetectors] = {"ITS", "TPC", "TRD", "TOF", "PHOS", "HMPID", "EMCAL", "MUON", "FMD", "ZDC", "PMD", "T0", "VZERO", "ACORDE" };


    for (Int_t iDet = 0; iDet < kNDetectors; iDet++) {

        if (fgRecoParam->GetDetRecoParamArray(iDet)) {
            ::Info(kEH, "Using custom reconstruction parameters for detector %s",fgkDetectorName[iDet]);
            continue;
        }

        ::Info(kEH, "Loading reconstruction parameter objects for detector %s",fgkDetectorName[iDet]);

        AliCDBPath path(fgkDetectorName[iDet],"Calib","RecoParam");
        AliCDBEntry *entry=AliCDBManager::Instance()->Get(path.GetPath());
        if(!entry){
            ::Warning(kEH, "Couldn't find RecoParam entry in OCDB for detector %s",fgkDetectorName[iDet]);
            isOK = kFALSE;
        }
        else {
            TObject *recoParamObj = entry->GetObject();
            if (dynamic_cast<TObjArray*>(recoParamObj)) {
                // The detector has a normal TobjArray of AliDetectorRecoParam objects
                // Registering them in AliRecoParam
                fgRecoParam->AddDetRecoParamArray(iDet,dynamic_cast<TObjArray*>(recoParamObj));
            }
            else if (dynamic_cast<AliDetectorRecoParam*>(recoParamObj)) {
                // The detector has only onse set of reco parameters
                // Registering it in AliRecoParam
                ::Info(kEH, "Single set of reconstruction parameters found for detector %s",fgkDetectorName[iDet]);
                dynamic_cast<AliDetectorRecoParam*>(recoParamObj)->SetAsDefault();
                fgRecoParam->AddDetRecoParam(iDet,dynamic_cast<AliDetectorRecoParam*>(recoParamObj));
            }
            else {
                ::Error(kEH, "No valid RecoParam object found in the OCDB for detector %s",fgkDetectorName[iDet]);
                isOK = kFALSE;
            }
            entry->SetOwner(0);

        }
    }

    if(!isOK) {
        delete fgRecoParam;
        fgRecoParam = 0;
    }

    return isOK;
}

TTree *AliEveEventManager::readESDTree(const char *treeName, int &runNo)
{
    if(!fESDFile && !fESD) return 0;

    static const TEveException kEH("AliEveEventManager::readESDTree ");

    TTree* tempTree = 0;

    tempTree =(TTree*) fESDFile->Get(treeName);
    if (tempTree != 0)
    {
        TFile *esdFriendFile = TFile::Open(fgESDfriendsFileName);
        if (esdFriendFile)
        {
            if (!esdFriendFile->IsZombie())
            {
                esdFriendFile->Close();
                fESDfriendExists = kTRUE;
                tempTree->SetBranchStatus ("ESDfriend*", 1);
            }
            delete esdFriendFile;
        }

        fESD->ReadFromTree(tempTree);
        if (fESDfriendExists)
        {
            fESDfriend = (AliESDfriend*) fESD->FindListObject("AliESDfriend");
            Info(kEH, "found and attached ESD friend.");
        }
        else
        {
            Warning(kEH, "ESDfriend not found.");
        }

        if (tempTree->GetEntry(0) <= 0)
        {
            Warning(kEH, "failed getting the first entry from tree: %s", treeName);
        }
        else
        {
            if (runNo < 0)
                runNo = fESD->GetESDRun()->GetRunNumber();
        }
    }
    else // tree == 0
    {
        Warning(kEH, "failed getting the tree:%s", treeName);
    }

    return tempTree;
}


//------------------------------------------------------------------------------

AliEveEventManager* AliEveEventManager::AddDependentManager(const TString& name, const TString& path)
{
    // Create and attach a dependent event-manager.
    // It is not added into eve list tree.

    static const TEveException kEH("AliEveEventManager::AddDependentManager ");

    if (fgMaster == 0)
        throw(kEH + "Master event-manager must be instantiated first.");

    if (fgMaster->fSubManagers == 0)
    {
        fgMaster->fSubManagers = new TList;
        fgMaster->fSubManagers->SetOwner(kTRUE);
    }

    AliEveEventManager* new_mgr = 0;
    fgCurrent = 0;
    try
    {
        new_mgr = new AliEveEventManager(name, fgMaster->fEventId);
        new_mgr->SetFilesPath(path);
        fgMaster->fSubManagers->Add(new_mgr);
    }
    catch (TEveException& exc)
    {
        ::Error(kEH, "Creation of new event-manager failed: '%s'.", exc.Data());
    }
    fgCurrent = fgMaster;

    return new_mgr;
}

AliEveEventManager* AliEveEventManager::GetDependentManager(const TString& name)
{
    // Get a dependant manager by name.
    // This will not change the current manager, use helper class
    // AliEveEventManager::CurrentChanger for that.

    static const TEveException kEH("AliEveEventManager::GetDependentManager ");

    if (fgMaster == 0)
        throw(kEH + "Master event-manager must be instantiated first.");

    if (fgMaster->fSubManagers == 0)
        return 0;

    return dynamic_cast<AliEveEventManager*>(fgMaster->fSubManagers->FindObject(name));
}

AliEveEventManager* AliEveEventManager::GetMaster()
{
    // Get master event-manager.

    return fgMaster;
}

AliEveEventManager* AliEveEventManager::GetCurrent()
{
    // Get current event-manager.

    return fgCurrent;
}

void AliEveEventManager::RegisterTransient(TEveElement* element)
{
    GetCurrent()->fTransients->AddElement(element);
}

void AliEveEventManager::RegisterTransientList(TEveElement* element)
{
    GetCurrent()->fTransientLists->AddElement(element);
}

//------------------------------------------------------------------------------
// Autoloading of events
//------------------------------------------------------------------------------

void AliEveEventManager::SetAutoLoadTime(Float_t time)
{
    // Set the auto-load time in seconds

    fAutoLoadTime = time;
}

void AliEveEventManager::SetAutoLoad(Bool_t autoLoad)
{
    // Set the automatic event loading mode

    static const TEveException kEH("AliEveEventManager::SetAutoLoad ");

    if (fAutoLoad == autoLoad)
    {
        Warning(kEH, "Setting autoload to the same value as before - %s. Ignoring.", fAutoLoad ? "true" : "false");
        return;
    }

    fAutoLoad = autoLoad;
    if (fAutoLoad)
    {
        StartAutoLoadTimer();
    }
    else
    {
        StopAutoLoadTimer();
    }
}

void AliEveEventManager::SetTrigSel(Int_t trig)
{
    static const TEveException kEH("AliEveEventManager::SetTrigSel ");

    if (!fRawReader)
    {
        Warning(kEH, "No Raw-reader exists. Ignoring the call.");
        return;
    }
    else
    {
        ULong64_t trigMask = 0;
        if (trig >= 0) trigMask = (1ull << trig);
        Info(kEH,"Trigger selection: 0x%llx",trigMask);
        fRawReader->SelectEvents(-1,trigMask,NULL);
    }
}

void AliEveEventManager::StartAutoLoadTimer()
{
    // Start the auto-load timer.

    fAutoLoadTimer->SetTime((Long_t)(1000*fAutoLoadTime));
    fAutoLoadTimer->Reset();
    fAutoLoadTimer->TurnOn();
    fAutoLoadTimerRunning = kTRUE;
}

void AliEveEventManager::StopAutoLoadTimer()
{
    // Stop the auto-load timer.

    fAutoLoadTimerRunning = kFALSE;
    fAutoLoadTimer->TurnOff();
}

void AliEveEventManager::AutoLoadNextEvent()
{
    // Called from auto-load timer, so it has to be public.
    // Do NOT call it directly.

	  static const TEveException kEH("AliEveEventManager::AutoLoadNextEvent ");
	  
	  	Info(kEH, "called!");

    if ( ! fAutoLoadTimerRunning || ! fAutoLoadTimer->HasTimedOut())
    {
        Warning(kEH, "Called unexpectedly - ignoring the call. Should ONLY be called from an internal timer.");
        return;
    }

    StopAutoLoadTimer();
    NextEvent();
    if (fAutoLoad)
        StartAutoLoadTimer();
}

//------------------------------------------------------------------------------
// Post event-loading functions
//------------------------------------------------------------------------------

void AliEveEventManager::AfterNewEventLoaded()
{
    // Execute registered macros and commands.
    // At the end emit NewEventLoaded signal.
    //
    // Virtual from TEveEventManager.

    static const TEveException kEH("AliEveEventManager::AfterNewEventLoaded ");

  Info(kEH, "------------------!!!------------");
                      
    NewEventDataLoaded();

    if (fExecutor)
        fExecutor->ExecMacros();

    TEveEventManager::AfterNewEventLoaded();

    NewEventLoaded();

    if (this == fgMaster && fSubManagers != 0)
    {
        TIter next(fSubManagers);
        while ((fgCurrent = dynamic_cast<AliEveEventManager*>(next())) != 0)
        {
            gEve->SetCurrentEvent(fgCurrent);
            try
            {
                fgCurrent->GotoEvent(fEventId);
            }
            catch (TEveException& exc)
            {
                // !!! Should somehow tag / disable / remove it?
                Error(kEH, "Getting event %d for sub-event-manager '%s' failed: '%s'.",
                      fEventId, fgCurrent->GetName(), exc.Data());
            }
              Info(kEH, "------------------!!! while() gEve->SetCurrentEvent() ------------");
        }
        fgCurrent = fgMaster;
        Info(kEH, "------------------!!! while() gEve->SetCurrentEvent(MASTER) ------------");
        gEve->SetCurrentEvent(fgMaster);
    }
    
}

void AliEveEventManager::NewEventDataLoaded()
{
    // Emit NewEventDataLoaded signal.

    Emit("NewEventDataLoaded()");
}

void AliEveEventManager::NewEventLoaded()
{
    // Emit NewEventLoaded signal.

    Emit("NewEventLoaded()");
}


//------------------------------------------------------------------------------
// Event info dumpers
//------------------------------------------------------------------------------

const AliEventInfo* AliEveEventManager::GetEventInfo() 
{
    // Fill the event info object

    AliCentralTrigger *aCTP = NULL;
    if (fRawReader) {
        fEventInfo.SetEventType(fRawReader->GetType());

        ULong64_t mask = fRawReader->GetClassMask();
        fEventInfo.SetTriggerMask(mask);
        UInt_t clmask = fRawReader->GetDetectorPattern()[0];
        fEventInfo.SetTriggerCluster(AliDAQ::ListOfTriggeredDetectors(clmask));

        aCTP = new AliCentralTrigger();
        TString configstr("");
        if (!aCTP->LoadConfiguration(configstr)) { // Load CTP config from OCDB
            AliError("No trigger configuration found in OCDB! The trigger configuration information will not be used!");
            delete aCTP;
            return 0;
        }
        aCTP->SetClassMask(mask);
        aCTP->SetClusterMask(clmask);

        if (fRunLoader) {
            AliCentralTrigger* rlCTP = fRunLoader->GetTrigger();
            if (rlCTP) {
                rlCTP->SetClassMask(mask);
                rlCTP->SetClusterMask(clmask);
            }
        }
    }
    else {
        fEventInfo.SetEventType(AliRawEventHeaderBase::kPhysicsEvent);

        if (fRunLoader && (!fRunLoader->LoadTrigger())) {
            aCTP = fRunLoader->GetTrigger();
            fEventInfo.SetTriggerMask(aCTP->GetClassMask());
            // get inputs from actp - just get
            AliESDHeader* esdheader = fESD->GetHeader();
            esdheader->SetL0TriggerInputs(aCTP->GetL0TriggerInputs());
            esdheader->SetL1TriggerInputs(aCTP->GetL1TriggerInputs());
            esdheader->SetL2TriggerInputs(aCTP->GetL2TriggerInputs());
            fEventInfo.SetTriggerCluster(AliDAQ::ListOfTriggeredDetectors(aCTP->GetClusterMask()));
        }
        else {
            AliWarning("No trigger can be loaded! The trigger information will not be used!");
            return 0;
        }
    }

    AliTriggerConfiguration *config = aCTP->GetConfiguration();
    if (!config) {
        AliError("No trigger configuration has been found! The trigger configuration information will not be used!");
        if (fRawReader) delete aCTP;
        return 0;
    }

    TString declTriggerClasses;

    // Load trigger aliases and declare the trigger classes included in aliases
    AliCDBEntry * entry = AliCDBManager::Instance()->Get("GRP/CTP/Aliases");
    if (entry) {
        THashList * lst = dynamic_cast<THashList*>(entry->GetObject());
        if (lst) {
            lst->Sort(kSortDescending); // to avoid problems with substrings
            if (fRawReader) fRawReader->LoadTriggerAlias(lst);
            // Now declare all the triggers present in the aliases
            TIter iter(lst);
            TNamed *nmd = 0;
            while((nmd = dynamic_cast<TNamed*>(iter.Next()))){
                declTriggerClasses += " ";
                declTriggerClasses += nmd->GetName();
            }
        }
        else {
            AliError("Cannot cast the object with trigger aliases to THashList!");
        }
    }
    else {
        AliError("No OCDB entry for the trigger aliases!");
    }

    // Load trigger classes for this run
    UChar_t clustmask = 0;
    TString trclasses;
    ULong64_t trmask = fEventInfo.GetTriggerMask();
    const TObjArray& classesArray = config->GetClasses();
    Int_t nclasses = classesArray.GetEntriesFast();
    for( Int_t iclass=0; iclass < nclasses; iclass++ ) {
        AliTriggerClass* trclass = (AliTriggerClass*)classesArray.At(iclass);
        if (trclass && trclass->GetMask()>0) {
            Int_t trindex = TMath::Nint(TMath::Log2(trclass->GetMask()));
            if (fESD) fESD->SetTriggerClass(trclass->GetName(),trindex);
            if (fRawReader) fRawReader->LoadTriggerClass(trclass->GetName(),trindex);
            if (trmask & (1ull << trindex)) {
                trclasses += " ";
                trclasses += trclass->GetName();
                trclasses += " ";
                clustmask |= trclass->GetCluster()->GetClusterMask();
            }
        }
    }
    fEventInfo.SetTriggerClasses(trclasses);

    if (!aCTP->CheckTriggeredDetectors()) {
        if (fRawReader) delete aCTP;
        return 0;
    }

    if (fRawReader) delete aCTP;

    // everything went ok, return pointer
    return (&fEventInfo);
}


TString AliEveEventManager::GetEventInfoHorizontal() const
{
    // Dumps the event-header contents in vertical formatting.

    TString rawInfo, esdInfo;

    if (!fRawReader)
    {
        rawInfo = "No raw-data event info is available!\n";
    }
    else
    {
        const UInt_t* attr = fRawReader->GetAttributes();
        TTimeStamp ts(fRawReader->GetTimestamp());
        rawInfo.Form("RAW event info: Run#: %d  Event type: %d (%s)  Period: %x  Orbit: %x  BC: %x\n"
                     "Trigger: %llx\nDetectors: %x (%s)\nAttributes:%x-%x-%x  Timestamp: %s\n",
                     fRawReader->GetRunNumber(),fRawReader->GetType(),AliRawEventHeaderBase::GetTypeName(fRawReader->GetType()),
                     fRawReader->GetPeriod(),fRawReader->GetOrbitID(),fRawReader->GetBCID(),
                     fRawReader->GetClassMask(),
                     *fRawReader->GetDetectorPattern(),AliDAQ::ListOfTriggeredDetectors(*fRawReader->GetDetectorPattern()),
                     attr[0],attr[1],attr[2], ts.AsString("s"));
    }

    if (!fESD)
    {
        esdInfo = "No ESD event info is available!";
    }
    else
    {
        TString acttrclasses   = fESD->GetESDRun()->GetActiveTriggerClasses();
        TString firedtrclasses = fESD->GetFiredTriggerClasses();
        TTimeStamp ts(fESD->GetTimeStamp());
        esdInfo.Form("ESD event info: Run#: %d  Event type: %d (%s)  Period: %x  Orbit: %x  BC: %x\n"
                     "Active trigger classes: %s\nTrigger: %llx (%s)\nEvent# in file: %d  Timestamp: %s, MagField: %.2e",
                     fESD->GetRunNumber(),
                     fESD->GetEventType(),AliRawEventHeaderBase::GetTypeName(fESD->GetEventType()),
                     fESD->GetPeriodNumber(),fESD->GetOrbitNumber(),fESD->GetBunchCrossNumber(),
                     acttrclasses.Data(),
                     fESD->GetTriggerMask(),firedtrclasses.Data(),
                     fESD->GetEventNumberInFile(), ts.AsString("s"), fESD->GetMagneticField());
    }

    return rawInfo + esdInfo;
}

TString AliEveEventManager::GetEventInfoVertical() const
{
    // Dumps the event-header contents in vertical formatting.

    TString rawInfo, esdInfo;

    if (!fRawReader)
    {
        rawInfo = "No raw-data event info is available!\n";
    }
    else
    {
        const UInt_t* attr = fRawReader->GetAttributes();
        rawInfo.Form("Raw-data event info:\nRun#: %d\nEvent type: %d (%s)\nPeriod: %x\nOrbit: %x   BC: %x\nTrigger: %llx\nDetectors: %x (%s)\nAttributes:%x-%x-%x\nTimestamp: %x\n",
                     fRawReader->GetRunNumber(),fRawReader->GetType(),AliRawEventHeaderBase::GetTypeName(fRawReader->GetType()),
                     fRawReader->GetPeriod(),fRawReader->GetOrbitID(),fRawReader->GetBCID(),
                     fRawReader->GetClassMask(),
                     *fRawReader->GetDetectorPattern(),AliDAQ::ListOfTriggeredDetectors(*fRawReader->GetDetectorPattern()),
                     attr[0],attr[1],attr[2],
                     fRawReader->GetTimestamp());
    }

    if (!fESD)
    {
        esdInfo = "No ESD event info is available!\n";
    }
    else
    {
        TString acttrclasses   = fESD->GetESDRun()->GetActiveTriggerClasses();
        TString firedtrclasses = fESD->GetFiredTriggerClasses();
        esdInfo.Form("ESD event info:\nRun#: %d\nActive trigger classes: %s\nEvent type: %d (%s)\nPeriod: %x\nOrbit: %x   BC: %x\nTrigger: %llx (%s)\nEvent# in file:%d\nTimestamp: %x\n",
                     fESD->GetRunNumber(),
                     acttrclasses.Data(),
                     fESD->GetEventType(),AliRawEventHeaderBase::GetTypeName(fESD->GetEventType()),
                     fESD->GetPeriodNumber(),fESD->GetOrbitNumber(),fESD->GetBunchCrossNumber(),
                     fESD->GetTriggerMask(),firedtrclasses.Data(),
                     fESD->GetEventNumberInFile(),
                     fESD->GetTimeStamp());
    }

    return rawInfo + "\n" + esdInfo;
}


//==============================================================================
// Reading of GRP and MagneticField.
// This is a reap-off from reconstruction ... should really be a common
// code to do this somewhere in STEER.
//==============================================================================

Bool_t AliEveEventManager::InitGRP()
{
    //------------------------------------
    // Initialization of the GRP entry
    //------------------------------------

    static const TEveException kEH("AliEveEventManager::InitGRP ");

    AliGRPManager grpMgr;
    if (!grpMgr.ReadGRPEntry()) {
        return kFALSE;
    }
    fgGRPLoaded = kTRUE;
    if (!grpMgr.SetMagField()) {
        throw kEH + "Setting of field failed!";
    }

    //*** Get the diamond profiles from OCDB
    // Eventually useful.

    /*
    entry = AliCDBManager::Instance()->Get("GRP/Calib/MeanVertexSPD");
    if (entry) {
    fDiamondProfileSPD = dynamic_cast<AliESDVertex*> (entry->GetObject());
    } else {
    ::Error(kEH, "No SPD diamond profile found in OCDB!");
    }

    entry = AliCDBManager::Instance()->Get("GRP/Calib/MeanVertex");
    if (entry) {
    fDiamondProfile = dynamic_cast<AliESDVertex*> (entry->GetObject());
    } else {
    ::Error(kEH, "No diamond profile found in OCDB!");
    }

    entry = AliCDBManager::Instance()->Get("GRP/Calib/MeanVertexTPC");
    if (entry) {
    fDiamondProfileTPC = dynamic_cast<AliESDVertex*> (entry->GetObject());
    } else {
    ::Error(kEH, "No TPC diamond profile found in OCDB!");
    }
  */

    return kTRUE;
} 

//------------------------------------
// Global variables management
//------------------------------------

Bool_t AliEveEventManager::InsertGlobal(const TString& tag, TEveElement* model)
{
    // Insert a new visualization-parameter database entry with the default
    return InsertGlobal(tag, model, fGlobalReplace, fGlobalUpdate);
}

Bool_t AliEveEventManager::InsertGlobal(const TString& tag, TEveElement* model,
                                        Bool_t replace, Bool_t update)
{
    TPair* pair = (TPair*) fGlobal->FindObject(tag);
    if (pair)
    {
        if (replace)
        {
            model->IncDenyDestroy();
            model->SetRnrChildren(kFALSE);

            TEveElement* old_model = dynamic_cast<TEveElement*>(pair->Value());
            if(!old_model) AliFatal("old_model == 0, dynamic cast failed\n");
            while (old_model->HasChildren())
            {
                TEveElement *el = old_model->FirstChild();
                el->SetVizModel(model);
                if (update)
                {
                    el->CopyVizParams(model);
                    el->PropagateVizParamsToProjecteds();
                }
            }
            old_model->DecDenyDestroy();

            pair->SetValue(dynamic_cast<TObject*>(model));
            return kTRUE;
        }
        else
        {
            return kFALSE;
        }
    }
    else
    {
        model->IncDenyDestroy();
        model->SetRnrChildren(kFALSE);
        fGlobal->Add(new TObjString(tag), dynamic_cast<TObject*>(model));
        return kTRUE;
    }
}

TEveElement* AliEveEventManager::FindGlobal(const TString& tag)
{
    return dynamic_cast<TEveElement*>(fGlobal->GetValue(tag));
}
