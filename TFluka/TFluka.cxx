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

/*
$Log$
Revision 1.1.2.4  2002/09/26 13:22:23  iglez2
Naive implementation of ProcessRun and ProcessEvent
Opening/Closing of input file (fInputFileName) with FORTRAN unit 5 before/after the first call to flukam inside Init()

Revision 1.1.2.3  2002/09/20 15:35:51  iglez2
Modification of LFDRTR. Value is passed to FLUKA !!!

Revision 1.1.2.2  2002/09/18 14:34:44  iglez2
Revised version with all pure virtual methods implemented

Revision 1.1.2.1  2002/07/24 08:49:41  alibrary
Adding TFluka to VirtualMC

Revision 1.1  2002/07/05 13:10:07  morsch
First commit of Fluka interface.

*/

#include <iostream.h>

#include "TFluka.h"
#include "TCallf77.h"      //For the fortran calls
#include "Fdblprc.h"       //(DBLPRC) fluka common
#include "Fiounit.h"       //(IOUNIT) fluka common
#include "AliRun.h"        //For gAlice
#include "AliGenerator.h"  //To generate the particles

// Fluka methods that may be needed.
#ifndef WIN32 
# define flukam  flukam_
# define fluka_openinp fluka_openinp_
# define fluka_closeinp fluka_closeinp_
#else 
# define flukam  FLUKAM
# define fluka_openinp FLUKA_OPENINP
# define fluka_closeinp FLUKA_CLOSEINP
#endif

extern "C" 
{
  //
  // Prototypes for FLUKA functions
  //
  void type_of_call flukam(const int&);
  void type_of_call fluka_openinp(const int&, DEFCHARA);
  void type_of_call fluka_closeinp(const int&);
}

//
// Class implementation for ROOT
//
ClassImp(TFluka)

//
// TFluka methods.
//____________________________________________________________________________ 
TFluka::TFluka()
  :TVirtualMC(),
   fVerbosityLevel(0),
   fInputFileName("")
{ 
  //
  // Default constructor
  //
} 
 
//____________________________________________________________________________ 
TFluka::TFluka(const char *title, Int_t verbosity)
  :TVirtualMC("TFluka",title),
   fVerbosityLevel(verbosity),
   fInputFileName("")
{
  if (fVerbosityLevel >=3)
    cout << "==> TFluka::TFluka(" << title << ") constructor called." << endl;

  if (fVerbosityLevel >=3)
    cout << "<== TFluka::TFluka(" << title << ") constructor called." << endl;
}

//____________________________________________________________________________ 
void TFluka::Init() {
  if (fVerbosityLevel >=3)
    cout << "==> TFluka::Init() called." << endl;

  if (fVerbosityLevel >=2)
    cout << "\t* Changing lfdrtr = (" << (GLOBAL.lfdrtr?'T':'F')
	 << ") in fluka..." << endl;
  GLOBAL.lfdrtr = true;

  if (fVerbosityLevel >=2)
    cout << "\t* Opening file " << fInputFileName << endl;
  const char* fname = fInputFileName;
  fluka_openinp(lunin, PASSCHARA(fname));

  if (fVerbosityLevel >=2)
    cout << "\t* Calling flukam..." << endl;
  flukam(0);

  if (fVerbosityLevel >=2)
    cout << "\t* Closing file " << fInputFileName << endl;
  fluka_closeinp(lunin);

  if (fVerbosityLevel >=3)
    cout << "<== TFluka::Init() called." << endl;
}

//____________________________________________________________________________ 
void TFluka::ProcessEvent() {
  if (fVerbosityLevel >=3)
    cout << "==> TFluka::ProcessEvent() called." << endl;

  if (fVerbosityLevel >=3)
    cout << "<== TFluka::ProcessEvent() called." << endl;
}

//____________________________________________________________________________ 
void TFluka::ProcessRun(Int_t nevent) {
  if (fVerbosityLevel >=3)
    cout << "==> TFluka::ProcessRun(" << nevent << ") called." 
	 << endl;

  if (fVerbosityLevel >=2) {
    cout << "\t* GLOBAL.fdrtr = " << (GLOBAL.lfdrtr?'T':'F') << endl;
    cout << "\t* Calling flukam again..." << endl;
  }
  gAlice->Generator()->Generate();
  flukam(0);

  if (fVerbosityLevel >=3)
    cout << "<== TFluka::ProcessRun(" << nevent << ") called." 
	 << endl;
}
