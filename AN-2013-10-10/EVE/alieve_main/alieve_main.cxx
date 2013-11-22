// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/
#include <EveBase/AliEveConfigManager.h>

#include <TInterpreter.h>
#include <TRint.h>
#include <TROOT.h>
#include <TPRegexp.h>
#include <TSystem.h>
#include <TError.h>
#include <RVersion.h>

#include <AliLog.h>

#include <TEveUtil.h>
#include <TEveManager.h>
#include <TEveSelection.h>
#include <TEveBrowser.h>
#include <TEveViewer.h>

#include <Getline.h>

// DEV Version includes
#include <EveBase/AliEveApplication.h>
#include <EveBase/AliEveMainWindow.h>

int main(int argc, char **argv)
{

    if(argc>1)
    {
        if(strcmp(argv[1],"-dev")==0 ){

            AliEveApplication app("AliEve", &argc, argv);
            AliEveMainWindow alieve("ALICE Event Display");

            app.Connect(&alieve, "CloseWindow()", "TApplication", &app, "Terminate(=0)" );
            app.Run();

            return 0;
        }
    }


    static const TEveException kEH("alieve::main");

    if (gSystem->Getenv("ALICE_ROOT") == 0)
    {
        Error(kEH.Data(), "ALICE_ROOT is not defined, aborting.");
        gSystem->Exit(1);
    }

    TString evedir(Form("%s/EVE", gSystem->Getenv("ALICE_ROOT")));

    if (gSystem->AccessPathName(evedir) == kTRUE)
    {
        Error(kEH.Data(), "Directory $ALICE_ROOT/EVE does not exist.");
        gSystem->Exit(1);
    }

    TString macPath(gROOT->GetMacroPath());
    macPath += Form(":%s/macros", evedir.Data());
    gInterpreter->AddIncludePath(evedir);
    if (gSystem->Getenv("ALICE_ROOT") != 0)
    {
        macPath += Form(":%s/alice-macros", evedir.Data());
        gInterpreter->AddIncludePath(Form("%s/EVE", gSystem->Getenv("ALICE_ROOT")));
        gInterpreter->AddIncludePath(Form("%s/PWG0", gSystem->Getenv("ALICE_ROOT")));
        gInterpreter->AddIncludePath(Form("%s/include", gSystem->Getenv("ALICE_ROOT")));
        gInterpreter->AddIncludePath(gSystem->Getenv("ALICE_ROOT"));
    }
    {
        // TabCom fails on double-colon in macro-path.
        // I fixed this in ROOT sometime ago ... could be removed
        // when we go to 5.26.
        TPMERegexp doubleColon(":{2,}", "og");
        doubleColon.Substitute(macPath, ":");
    }
    gROOT->SetMacroPath(macPath);

    // make sure logger is instantiated
    AliLog::GetRootLogger();
    TRint  *app = new TRint("App", &argc, argv);

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,25,4) || defined XXX_LATEST_ROOT
    // Waiting for update by Pawel. Now GED in ROOT is better again :)
    // Uncomment when fixed in AliEveGedXXX.
    // TEveGListTreeEditorFrame::SetEditorClass("AliEveGedEditor");
#endif

    TEveManager::Create();
    gEve->GetDefaultViewer()->SetElementName("3D View");
    gEve->GetSelection()->SetPickToSelect(TEveSelection::kPS_PableCompound);
    gEve->GetHighlight()->SetPickToSelect(TEveSelection::kPS_PableCompound);

    gEve->RegisterGeometryAlias("Default", Form("%s/alice-data/default_geo.root", evedir.Data()));

    try {AliEveConfigManager::InitializeMaster();}
    catch (TEveException exc) {
        AliErrorGeneral("alieve_main",exc.Data());
    }

    app->Connect( "TEveBrowser", "CloseWindow()", "TRint", app, "Terminate(=0)");

    app->Run(kTRUE);

    if (gEve && gEve->GetBrowser())	gEve->GetBrowser()->UnmapWindow();

    TEveManager::Terminate();
    app->Terminate(0);

    if(gEve) {delete gEve; gEve = 0;}


    return 0;
}
