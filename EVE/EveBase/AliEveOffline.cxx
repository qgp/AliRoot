//
//  AliEveOffline.cpp
//  xAliRoot
//
//  Created by Jeremi Niedziela on 01/06/15.
//
//

#include "AliEveOffline.h"

//#include <AliQAHistViewer.h>

#include <TString.h>
#include <TGrid.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TInterpreter.h>
#include <TMath.h>
#include <TGListTree.h>
#include <TEveVSDStructs.h>
#include <TEveManager.h>
#include <TEveTrackPropagator.h>
#include <TEnv.h>
#include <TEveWindowManager.h>
#include <TGTab.h>
#include <TTimeStamp.h>
#include <TPRegexp.h>
#include <TFolder.h>
#include <TSystemDirectory.h>
#include <TGButton.h>
#include <TGFileBrowser.h>
#include <TEveMacro.h>

#include <AliESDtrackCuts.h>
#include <AliESDEvent.h>
#include <AliESDfriend.h>
#include <AliESDtrack.h>
#include <AliESDfriendTrack.h>
#include <AliExternalTrackParam.h>

#include <AliEveTrack.h>
#include <AliEveTrackCounter.h>
#include <AliEveMagField.h>
#include <AliEveOfflineEventManager.h>
#include <AliEveEventManagerEditor.h>
#include <AliEveMultiView.h>
#include <AliEveMacroExecutor.h>
#include <AliEveMacro.h>
#include <AliEveMacroExecutorWindow.h>
#include <AliEveEventSelectorWindow.h>
#include <AliEveTrackFitter.h>
#include <AliEveGeomGentle.h>

#include <AliCDBManager.h>

#include <iostream>

using namespace std;

AliEveOffline::AliEveOffline(const TString& path, const TString& cdbUri) :
    fCDBuri(cdbUri),
    fPath(path)
{
    //-----------------------------------------------------------------------------------------
    //  Set all preferences here
    //
    fShowHLTESDtree = kFALSE;       // show HLT ESD tree
    Bool_t showMuon = kTRUE;        // show MUON
    
    Color_t colorTRD = kGray;      // color of TRD modules
    Color_t colorMUON = kGray;     // color of MUON modules
    
    bool customPreset = true;            // should one of the following custom presets be used
    Color_t colors[9] = {kCyan,kCyan,kCyan,kCyan,kCyan,kCyan,kCyan,kCyan,kCyan};
    Width_t widths[9] = {3,3,3,3,3,3,3,3,3};
    bool dashBad = true;
    
    //    Color_t colors[9] = {kGreen,kGreen,kGreen,kGreen,kGreen,kGreen,kGreen,kGreen,kGreen}; // preset for cosmics
    
    //
    //-----------------------------------------------------------------------------------------

    
    AliEveEventManager *man = new AliEveOfflineEventManager();
    Init();
    man->Open();
    
    if (gSystem->Getenv("ALICE_ROOT") != 0)
    {
        gInterpreter->AddIncludePath(Form("%s/MUON", gSystem->Getenv("ALICE_ROOT")));
        gInterpreter->AddIncludePath(Form("%s/MUON/mapping", gSystem->Getenv("ALICE_ROOT")));
    }

    
    if (!AliCDBManager::Instance()->IsDefaultStorageSet())
    {
        gEnv->SetValue("Root.Stacktrace", "no");
        Fatal("visscan_init.C", "OCDB path MUST be specified as the first argument.");
    }
    
    AliEveEventManager::AddAODfriend("AliAOD.VertexingHF.root");

    TEveUtil::AssertMacro("VizDB_scan.C");
    
    AliEveMacroExecutor *exec = man->GetExecutor();
    TEveBrowser *browser = gEve->GetBrowser();
    browser->ShowCloseTab(kFALSE);
    
    //==============================================================================
    // Geometry, scenes, projections and viewers
    //==============================================================================
    
    AliEveMultiView *mv = new AliEveMultiView(kTRUE);
    AliEveGeomGentle *geomGentle = new AliEveGeomGentle();

    mv->SetDepth(-10);

    mv->InitGeomGentle(geomGentle->GetGeomGentle(),
                              geomGentle->GetGeomGentleRphi(),
                              geomGentle->GetGeomGentleRhoz(),
                              geomGentle->GetGeomGentleRhoz());
    
    mv->InitGeomGentleTrd(geomGentle->GetGeomGentleTRD(colorTRD));
    mv->InitGeomGentleMuon(geomGentle->GetGeomGentleMUON(true,colorMUON), kFALSE, kFALSE, kTRUE);

    mv->SetDepth(0);
    
    //==============================================================================
    // Registration of per-event macros
    //==============================================================================
    
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "SIM Track",   "kine_tracks.C", "kine_tracks", "", kFALSE));
    
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "SIM Hits ITS", "its_hits.C",    "its_hits",    "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "SIM Hits TPC", "tpc_hits.C",    "tpc_hits",    "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "SIM Hits T0",  "t0_hits.C",     "t0_hits",     "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "SIM Hits FMD", "fmd_hits.C",    "fmd_hits",    "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "SIM Hits ACORDE", "acorde_hits.C",    "acorde_hits",    "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "SIM Hits EMCAL", "emcal_hits.C",    "emcal_hits",    "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "SIM Hits TOF",  "tof_hits.C",     "tof_hits",     "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "SIM Hits TRD", "trd_hits.C",    "trd_hits",    "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "SIM Hits VZERO", "vzero_hits.C",    "vzero_hits",    "", kFALSE));
    
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "DIG ITS",     "its_digits.C",  "its_digits",  "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "DIG TPC",     "tpc_digits.C",  "tpc_digits",  "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "DIG TOF",     "tof_digits.C",  "tof_digits",  "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "DIG HMPID",   "hmpid_digits.C","hmpid_digits","", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "DIG FMD",     "fmd_digits.C",  "fmd_digits",  "", kFALSE));
    
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRawReader, "RAW ITS",     "its_raw.C",     "its_raw",     "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRawReader, "RAW TPC",     "tpc_raw.C",     "tpc_raw",     "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRawReader, "RAW TOF",     "tof_raw.C",     "tof_raw",     "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRawReader, "RAW HMPID",   "hmpid_raw.C",   "hmpid_raw",   "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRawReader, "RAW T0",      "t0_raw.C",      "t0_raw",      "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRawReader, "RAW FMD",     "fmd_raw.C",     "fmd_raw",     "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRawReader, "RAW VZERO",   "vzero_raw.C",   "vzero_raw",   "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRawReader, "RAW ACORDE",  "acorde_raw.C",  "acorde_raw",  "", kFALSE));
    /*
     exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC PVTX",             "primary_vertex.C", "primary_vertex",             "",                kTRUE));
     exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC PVTX Ellipse",     "primary_vertex.C", "primary_vertex_ellipse",     "",                kTRUE));
     exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC PVTX Box",         "primary_vertex.C", "primary_vertex_box",         "kFALSE, 3, 3, 3", kFALSE));
     exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC PVTX SPD",         "primary_vertex.C", "primary_vertex_spd",         "",                kTRUE));
     exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC PVTX Ellipse SPD", "primary_vertex.C", "primary_vertex_ellipse_spd", "",                kTRUE));
     exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC PVTX Box SPD",     "primary_vertex.C", "primary_vertex_box_spd",     "kFALSE, 3, 3, 3", kFALSE));
     exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC PVTX TPC",         "primary_vertex.C", "primary_vertex_tpc",         "",                kFALSE));
     exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC PVTX Ellipse TPC", "primary_vertex.C", "primary_vertex_ellipse_tpc", "",                kFALSE));
     exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC PVTX Box TPC",     "primary_vertex.C", "primary_vertex_box_tpc",     "kFALSE, 3, 3, 3", kFALSE));
     */
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC V0",   "esd_V0_points.C",       "esd_V0_points_onfly"));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC V0",   "esd_V0_points.C",       "esd_V0_points_offline"));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC V0",   "esd_V0.C",              "esd_V0"));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC CSCD", "esd_cascade_points.C",  "esd_cascade_points"));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC CSCD", "esd_cascade.C",         "esd_cascade"));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC KINK", "esd_kink_points.C",     "esd_kink_points"));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC KINK", "esd_kink.C",            "esd_kink"));
    
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC Tracks",              "esd_tracks.C", "esd_tracks",              "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC Tracks ITS standalone",          "esd_tracks.C", "esd_tracks_ITS_standalone",              "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC Tracks ITS",          "esd_tracks.C", "esd_tracks_ITS",              "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC Tracks TPC",           "esd_tracks.C", "esd_tracks_TPC",              "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC Tracks MI",           "esd_tracks.C", "esd_tracks_MI",           "", kFALSE));
    
    // default appearance:
    //  exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC Tracks by category",  "esd_tracks.C", "esd_tracks_by_category",  "", kTRUE));
    
    // preset for cosmics:
    //  exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC Tracks by category",  "esd_tracks.C", "esd_tracks_by_category",  "kGreen,kGreen,kGreen,kGreen,kGreen,kGreen,kGreen,kGreen,kGreen,kFALSE", kTRUE));
    
    
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC Tracks by anal cuts", "esd_tracks.C", "esd_tracks_by_anal_cuts", "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC Tracks Lego", "lego.C", "lego", "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC Tracks Beams Info", "beams_info.C", "beams_info", "", kFALSE));
    
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC Tracklets SPD", "esd_spd_tracklets.C", "esd_spd_tracklets", "", kTRUE));
    
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC ZDC",      "esd_zdc.C", "esd_zdc", "", kFALSE));
    
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "REC Clusters",     "clusters.C",     "clusters", "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "REC Clusters ITS", "its_clusters.C", "its_clusters"));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "REC Clusters TPC", "tpc_clusters.C", "tpc_clusters"));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "REC Clusters TRD", "trd_clusters.C", "trd_clusters"));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "REC Clusters TOF", "tof_clusters.C", "tof_clusters"));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "REC Clusters HMPID", "hmpid_clusters.C", "hmpid_clusters"));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "REC Clusters PHOS", "phos_clusters.C", "phos_clusters"));
    
    exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "REC Clusters TPC", "vplot_tpc.C",    "vplot_tpc", "", kFALSE));
    
    exec->AddMacro(new AliEveMacro(AliEveMacro::kAOD, "ANA HF",   "aod_HF.C",   "aod_HF",   "", kFALSE));
    exec->AddMacro(new AliEveMacro(AliEveMacro::kAOD, "ANA Jets", "jetplane.C", "jetplane", "", kFALSE));
    
    exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "DUMP VZERO",   "vzero_dump.C",   "vzero_dump",   "", kFALSE));
    
    if (showMuon)
    {
        exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "SIM TrackRef MUON", "muon_trackRefs.C", "muon_trackRefs", "kTRUE", kFALSE));
        exec->AddMacro(new AliEveMacro(AliEveMacro::kRawReader, "RAW MUON", "muon_raw.C", "muon_raw", "", kFALSE));
        exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "DIG MUON", "muon_digits.C", "muon_digits", "", kFALSE));
        exec->AddMacro(new AliEveMacro(AliEveMacro::kRunLoader, "REC Clusters MUON", "muon_clusters.C", "muon_clusters", "", kTRUE));
        exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "REC Tracks MUON", "esd_muon_tracks.C", "esd_muon_tracks", "kTRUE,kFALSE", kTRUE));
        
        exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "ESD AD", "ad_esd.C", "ad_esd", "", kTRUE));
        exec->AddMacro(new AliEveMacro(AliEveMacro::kESD, "ESD EMCal", "emcal_esdclustercells.C", "emcal_esdclustercells", "", kTRUE));
        
    }
    
    //==============================================================================
    // Additional GUI components
    //==============================================================================
    
    // Macro / data selection
    TEveWindowSlot *slot = TEveWindow::CreateWindowInTab(browser->GetTabRight());
    slot->StartEmbedding();
    AliEveMacroExecutorWindow* exewin = new AliEveMacroExecutorWindow(exec);
    slot->StopEmbedding("DataSelection");
    exewin->PopulateMacros();
    
    // Event selection tab
    slot = TEveWindow::CreateWindowInTab(browser->GetTabRight());
    slot->StartEmbedding();
    new AliEveEventSelectorWindow(gClient->GetRoot(), 600, 400, man->GetEventSelector());
    slot->StopEmbedding("Selections");
    
    // QA viewer
    /*
     slot = TEveWindow::CreateWindowInTab(browser->GetTabRight());
     slot->StartEmbedding();
     new AliQAHistViewer(gClient->GetRoot(), 600, 400, kTRUE);
     slot->StopEmbedding("QA histograms");
     
     browser->GetTabRight()->SetTab(1);
     */
    browser->StartEmbedding(TRootBrowser::kBottom);
    new AliEveEventManagerWindow(man);
    browser->StopEmbedding("EventCtrl");
    
    slot = TEveWindow::CreateWindowInTab(browser->GetTabRight());
    TEveWindowTab *store_tab = slot->MakeTab();
    store_tab->SetElementNameTitle("WindowStore",
                                   "Undocked windows whose previous container is not known\n"
                                   "are placed here when the main-frame is closed.");
    gEve->GetWindowManager()->SetDefaultContainer(store_tab);
    
    
    //==============================================================================
    // AliEve objects - global tools
    //==============================================================================
    
    AliEveTrackFitter* fitter = new AliEveTrackFitter();
    gEve->AddToListTree(fitter, 1);
    gEve->AddElement(fitter, gEve->GetEventScene());
    
    AliEveTrackCounter* g_trkcnt = new AliEveTrackCounter("Primary Counter");
    gEve->AddToListTree(g_trkcnt, kFALSE);
    
    
    //==============================================================================
    // Final stuff
    //==============================================================================
    
    // A refresh to show proper window.
    //gEve->GetViewers()->SwitchColorSet();
    browser->MoveResize(0, 0, gClient->GetDisplayWidth(),gClient->GetDisplayHeight() - 32);
    gEve->Redraw3D(kTRUE);
    gSystem->ProcessEvents();
    
    man->GotoEvent(0);
    
    gEve->EditElement(g_trkcnt);
    gEve->Redraw3D(kTRUE);
    
    //move multiview to the front
    browser->GetTabRight()->SetTab(1);
    TGLViewer *glv1 = mv->Get3DView()->GetGLViewer();
    glv1->CurrentCamera().RotateRad(-0.4, 0.6);
    gEve->FullRedraw3D();
    gSystem->ProcessEvents();
    gEve->Redraw3D(kTRUE);
    
    if(customPreset)
    {
//        man->SetESDcolors(colors);
        man->SetESDwidths(widths);
        man->SetESDdashBad(false);
    }
    man->SetESDtracksByCategory(true);
    man->SetESDtracksByType(true);
    man->SetESDdashBad(false);
    
    man->SetAutoLoad(true);// set autoload by default
}

void AliEveOffline::Init()
{
    const Text_t* esdfile = 0;
    const Text_t* aodfile = 0;
    const Text_t* rawfile = 0;
    
    cout<<"Adding standard macros"<<endl;
    ImportMacros();
    TEveUtil::AssertMacro("VizDB_scan.C");
    gSystem->ProcessEvents();
    
    AliEveEventManager::SetFilesPath(fPath);
    
    if(fShowHLTESDtree){
        AliEveEventManager::SetESDFileName(esdfile, AliEveEventManager::kHLTTree);
    }
    else{
        AliEveEventManager::SetESDFileName(esdfile, AliEveEventManager::kOfflineTree);
    }
    
    AliEveEventManager::SetRawFileName(rawfile);
    AliEveEventManager::SetCdbUri(fCDBuri);
//    AliEveEventManager::SetAssertElements(0,0,0,0);
    
    // Open event
    if (fPath.BeginsWith("alien:") || !fCDBuri.BeginsWith("local:"))
    {
        if (gGrid != 0)
        {
            Info("alieve_init", "TGrid already initializied. Skiping checks and initialization.");
        }
        else
        {
            Info("alieve_init", "AliEn requested - connecting.");
            if (gSystem->Getenv("GSHELL_ROOT") == 0)
            {
                Error("alieve_init", "AliEn environment not initialized. Aborting.");
                gSystem->Exit(1);
            }
            if (TGrid::Connect("alien") == 0)
            {
                Error("alieve_init", "TGrid::Connect() failed. Aborting.");
                gSystem->Exit(1);
            }
        }
    }
    cout<<"Opening event -1 from "<<fPath.Data()<<endl;
    gEve->AddEvent(AliEveEventManager::GetMaster());
}

void AliEveOffline::ImportMacros()
{
    // Put macros in the list of browsables, add a macro browser to
    // top-level GUI.
    
    TString  hack = gSystem->pwd(); // Problem with TGFileBrowser cding
    
    TString macdir("$(ALICE_ROOT)/EVE/alice-macros");
    gSystem->ExpandPathName(macdir);
    
    TFolder* f = gEve->GetMacroFolder();
    void* dirhandle = gSystem->OpenDirectory(macdir.Data());
    if (dirhandle != 0)
    {
        const char* filename;
        TPMERegexp re("\\.C$");
        TObjArray names;
        while ((filename = gSystem->GetDirEntry(dirhandle)) != 0)
        {
            if (re.Match(filename))
                names.AddLast(new TObjString(filename));
        }
        names.Sort();
        
        for (Int_t ii=0; ii<names.GetEntries(); ++ii)
        {
            TObjString * si = (TObjString*) names.At(ii);
            f->Add(new TEveMacro(Form("%s/%s", macdir.Data(), (si->GetString()).Data())));
        }
    }
    gSystem->FreeDirectory(dirhandle);
    
    gROOT->GetListOfBrowsables()->Add(new TSystemDirectory(macdir.Data(), macdir.Data()));
    
    {
        TEveBrowser   *br = gEve->GetBrowser();
        TGFileBrowser *fb = 0;
        fb = br->GetFileBrowser();
        fb->GotoDir(macdir);
        {
            br->StartEmbedding(0);
            fb = br->MakeFileBrowser();
            fb->BrowseObj(f);
            fb->Show();
            br->StopEmbedding();
            br->SetTabTitle("Macros", 0);
            br->SetTab(0, 0);
        }
    }
    gSystem->cd(hack);
}

