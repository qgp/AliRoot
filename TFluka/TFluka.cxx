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
Revision 1.1.2.1  2002/07/24 08:49:41  alibrary
Adding TFluka to VirtualMC

Revision 1.1  2002/07/05 13:10:07  morsch
First commit of Fluka interface.

*/

#include <iostream.h>

#include "TFluka.h"
#include "TCallf77.h"


#define METHODDEBUG

// Fluka methods that may be needed.
#ifndef WIN32 
# define flukam  flukam_
#else 
# define flukam  FLUKAM
#endif

extern "C" 
{
  //
  // Prototypes for FLUKA functions
  //
  void type_of_call flukam(const int&);
}

//
// Class implementation for ROOT
//
ClassImp(TFluka)

//
// TFluka methods.
//____________________________________________________________________________ 
TFluka::TFluka()
  :TVirtualMC()
{ 
  //
  // Default constructor
  //
} 
 
//____________________________________________________________________________ 
TFluka::TFluka(const char *title)
  :TVirtualMC("TFluka",title)
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
  cout << "==> TFluka::Init() constructor called." << endl;
#endif
  cout << "\t* Calling flukam..." << endl;
  flukam(0);

#ifdef METHODDEBUG
  cout << "<== TFluka::Init() constructor called." << endl;
#endif
}
