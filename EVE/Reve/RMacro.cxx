// $Header$

#include "RMacro.h"

#include <TSystem.h>
#include <TROOT.h>
#include <G__ci.h>

using namespace Reve;

//______________________________________________________________________
// RMacro
//
// Sub-class of TMacro, overriding Exec to unload the previous verison
// and cleanup after the execution.

ClassImp(RMacro)

RMacro::RMacro() : TMacro() {}

RMacro::RMacro(const RMacro& m) : TMacro(m) {}

RMacro::RMacro(const char* name) :
  TMacro()
{
  if (!name) return;

  fTitle = name;

  char *dot   = (char*)strrchr(name, '.');
  char *slash = (char*)strrchr(name, '/');
  if (dot) *dot = 0;
  if (slash) fName = slash + 1;
  else       fName = name;

  ReadFile(fTitle);
}

/**************************************************************************/

#include <TTimer.h>

Long_t RMacro::Exec(const char* params, Int_t* error)
{
  Long_t retval = -1;

  if (gROOT->GetGlobalFunction(fName, 0, kTRUE) != 0)
  {
      gROOT->SetExecutingMacro(kTRUE);
      gROOT->SetExecutingMacro(kFALSE);
      retval = gROOT->ProcessLine(Form("%s()", fName.Data()), error);
  }
  else
  {
    // Copy from TMacro::Exec. Difference is that the file is really placed
    // into the /tmp.
    TString fname = "/tmp/";
    {
      //the current implementation uses a file in the current directory.
      //should be replaced by a direct execution from memory by CINT
      fname += GetName();
      fname += ".C";
      SaveSource(fname);
      //disable a possible call to gROOT->Reset from the executed script
      gROOT->SetExecutingMacro(kTRUE);
      //execute script in /tmp
      TString exec = ".x " + fname;
      TString p = params;
      if (p == "") p = fParams;
      if (p != "")
        exec += "(" + p + ")";
      retval = gROOT->ProcessLine(exec, error);
      //enable gROOT->Reset
      gROOT->SetExecutingMacro(kFALSE);
      //delete the temporary file
      gSystem->Unlink(fname);
    }
  }

  //G__unloadfile(fname);

  // In case an exception was thrown (which i do not know how to detect
  // the execution of next macros does not succeed.
  // However strange this might seem, this solves the problem.
  // TTimer::SingleShot(100, "Reve::RMacro", this, "ResetRoot()");
  //
  // 27.8.07 - ok, this does not work any more. Seems I'll have to fix
  // this real soon now.
  //
  // !!!! FIX MACRO HANDLING !!!!
  //

  return retval;
}

#include <TApplication.h>

void RMacro::ResetRoot()
{
  // printf ("RMacro::ResetRoot doing 'gROOT->Reset()'.\n");
  gROOT->GetApplication()->ProcessLine("gROOT->Reset()");
}
