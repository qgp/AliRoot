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
#include "TCallf77.h" //For the fortran calls
#include "Fdblprc.h"  //(DBLPRC) fluka common
#include "Fiounit.h"  //(IOUNIT) fluka common

#define METHODDEBUG

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
  :TVirtualMC(),fInputFileName("")
{ 
  //
  // Default constructor
  //
} 
 
//____________________________________________________________________________ 
TFluka::TFluka(const char *title)
  :TVirtualMC("TFluka",title),fInputFileName("")
{
#ifdef METHODDEBUG
  cout << "==> TFluka::TFluka(" << title << ") constructor called." << endl;
#endif
#ifdef METHODDEBUG
  cout << "<== TFluka::TFluka(" << title << ") constructor called." << endl;
#endif
}

//____________________________________________________________________________ 
void TFluka::Init() {
#ifdef METHODDEBUG
  cout << "==> TFluka::Init() called." << endl;
#endif

  cout << "\t* Changing lfdrtr = (" << (GLOBAL.lfdrtr?'T':'F')
       << ") in fluka..." << endl;
  GLOBAL.lfdrtr = true;

  cout << "\t* Opening file " << fInputFileName << endl;
  const char* fname = fInputFileName;
  fluka_openinp(lunin, PASSCHARA(fname));

  cout << "\t* Calling flukam..." << endl;
  flukam(0);

  cout << "\t* Closing file " << fInputFileName << endl;
  fluka_closeinp(lunin);

#ifdef METHODDEBUG
  cout << "<== TFluka::Init() called." << endl;
#endif
}

//____________________________________________________________________________ 
void TFluka::ProcessEvent() {
#ifdef METHODDEBUG
  cout << "==> TFluka::ProcessEvent() called." << endl;
#endif

  cout << "\t* GLOBAL.fdrtr = " << (GLOBAL.lfdrtr?'T':'F') << endl;
  cout << "\t* Calling flukam again..." << endl;
  flukam(0);

#ifdef METHODDEBUG
  cout << "<== TFluka::ProcessEvent() called." << endl;
#endif
}

//____________________________________________________________________________ 
void TFluka::ProcessRun(Int_t nevent) {
#ifdef METHODDEBUG
  cout << "==> TFluka::ProcessRun(" << nevent << ") called." 
       << endl;
#endif

  cout << "\t* fdrtr = (" << (GLOBAL.lfdrtr?'T':'F') << endl;
  cout << "\t* Calling flukam again..." << endl;
  flukam(0);

#ifdef METHODDEBUG
  cout << "<== TFluka::ProcessRun(" << nevent << ") called." 
       << endl;
#endif
}
