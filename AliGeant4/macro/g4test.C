// $Id$
//
// Root macro that runs G4 test macro (alirunN.in) specified by macroName.
// Available tests:
// alirun0.in - standard AliRoot run - all detectors, AliRoot event generator
// alirun1.in - interactive particle gun, event visualization - TPC
// alirun2.in - lego run - TRD
// alirun3.in - geometry test - all detectors (excluded MANY)");
// alirun4.in - geometry browser - FRAME;

void g4test(const char* macroName)
{
  // load Geant4 libraries
  if (!gInterpreter->IsLoaded("g4libs.C")) gROOT->LoadMacro("g4libs.C");
  g4libs();
 
  // load AliRoot core libraries
  // gInterpreter->ProcessLine("steerlibs()");

  // menu
  if (!gInterpreter->IsLoaded("g4menu.C")) gROOT->LoadMacro("g4menu.C");

  CreateGeant4();

  ((TGeant4*)gMC)->ProcessGeantMacro(macroName);
} 
