/**************************************************************************
* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
*                                                                        *
* Permission to use, copy, modify and distribute this software and its   *
* documentation strictly for non-commercial purposes is hereby granted   *
* without fee, provided that the above copyright notice appears in all   *
* copies and that both the copyright notice and this permission notice   *
* appear in the supporting documentation. The authors make no claims     *
* about the suitability of this software for any purpose. It is          *
* provided "as is" without express or implied warranty.                  *
**************************************************************************/

// $Id$

/// \ingroup graphics
/// \file mchview.cxx
/// \brief Tracker visualization program
///
/// \author Laurent Aphecetche, Subatech


#include "AliMUONMchViewApplication.h"

#include "AliCDBManager.h"
#include "AliCodeTimer.h"
#include "AliLog.h"
#include "AliMUONPainterHelper.h"
#include <Riostream.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TStyle.h>

//______________________________________________________________________________
Int_t Usage()
{
  /// Printout available options of the program
  cout << "mchview " << endl;
  cout << "  --version : shows the current version of the program" << endl;
  cout << "  --use filename.root : reuse a previously saved (from this program) root file. Several --use can be used ;-)" << endl;
  cout << "  --geometry #x#+#+# : manually specify the geometry of the window, ala X11..., e.g. --geometry 1280x900+1600+0 will" << endl;
  cout << "    get a window of size 1280x900, located at (1600,0) from the top-left of the (multihead) display " << endl;
  return -1;
}

//______________________________________________________________________________
int main(int argc, char** argv)
{
  /// Main function for the program
  TObjArray args;
  
  for ( int i = 1; i < argc; ++i ) 
  {
    args.Add(new TObjString(argv[i]));
  }
  
  Int_t nok(0);
  
  TObjArray filesToOpen;
  Bool_t isGeometryFixed(kFALSE);
  Int_t gix, giy;
  Int_t gox,goy;

  for ( Int_t i = 0; i <= args.GetLast(); ++i ) 
  {
    TString a(static_cast<TObjString*>(args.At(i))->String());
    if ( a == "--version" ) 
    {
      cout << "mchview Version " << AliMUONMchViewApplication::Version() << " ($Id$)" << endl;
      ++nok;
      return 0;
    }
    if ( a == "--use" && i < args.GetLast() )
    {
      filesToOpen.Add(args.At(i+1));
      ++i;
      nok += 2;
    }
    else if ( a == "--geometry" )
    {
      isGeometryFixed = kTRUE;
      TString g(static_cast<TObjString*>(args.At(i+1))->String());
      sscanf(g.Data(),"%dx%d+%d+%d",&gix,&giy,&gox,&goy);
      nok += 2;
      ++i;
    }
    
    else
    {
      return Usage();
    }
  }
  
  if ( nok < args.GetLast() )
  {
    return Usage();
  }
  
  AliWarningGeneral("main","FIXME ? Remove default storage and run number from here...");
  
  AliCDBManager::Instance()->SetDefaultStorage("local://$ALICE_ROOT/OCDB");
  AliCDBManager::Instance()->SetRun(0);
 
  gROOT->SetStyle("Plain");  
  gStyle->SetPalette(1);
  Int_t n = gStyle->GetNumberOfColors();
  Int_t* colors = new Int_t[n+2];
  for ( Int_t i = 1; i <= n; ++i )
  {
    colors[i] = gStyle->GetColorPalette(i-1);
  }
  colors[0] = 0;
  colors[n+1] = 1;
  gStyle->SetPalette(n+2,colors);
  delete[] colors;

  UInt_t w(0);
  UInt_t h(0);
  UInt_t ox(0);
  UInt_t oy(0);

  if ( isGeometryFixed )
  {
    w = gix;
    h = giy;
    ox = gox;
    oy = goy;
  }
  
  AliMUONMchViewApplication* theApp = new AliMUONMchViewApplication("mchview", &argc, argv, w,h,gox,goy);
   
  AliCodeTimer::Instance()->Print();

  TIter next(&filesToOpen);
  TObjString* s;
  while ( ( s = static_cast<TObjString*>(next()) ) )
  {
    theApp->Open(s->String().Data());
  }
  
  // --- Start the event loop ---
  theApp->Run(kTRUE);

  AliMUONPainterHelper::Instance()->Save();
}
