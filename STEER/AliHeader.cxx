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
Revision 1.11  2002/10/29 14:26:49  hristov
Code clean-up (F.Carminati)

Revision 1.10  2001/10/09 18:00:35  hristov
Temporary fix to provide unique event number in the simulation (J.Chudoba)

Revision 1.9  2001/05/23 08:54:53  hristov
Typo corrected

Revision 1.8  2001/05/23 08:50:01  hristov
Weird inline removed

Revision 1.7  2001/05/16 14:57:22  alibrary
New files for folders and Stack

Revision 1.4  2000/10/02 21:28:14  fca
Removal of useless dependecies via forward declarations

Revision 1.3  2000/07/12 08:56:25  fca
Coding convention correction and warning removal

Revision 1.2  1999/09/29 09:24:29  fca
Introduction of the Copyright and cvs Log

*/

#include "AliHeader.h"
#include <stdio.h>
 
ClassImp(AliHeader)

//_______________________________________________________________________
AliHeader::AliHeader():
  fRun(0),
  fNvertex(0),
  fNprimary(0),
  fNtrack(0),
  fEvent(0),
  fEventNrInRun(0),
  fStack(0),
  fGenHeader(0)
{
  //
  // Default constructor
  //
}

//_______________________________________________________________________
AliHeader::AliHeader(const AliHeader& head):
  TObject(head),
  fRun(0),
  fNvertex(0),
  fNprimary(0),
  fNtrack(0),
  fEvent(0),
  fEventNrInRun(0),
  fStack(0),
  fGenHeader(0)
{
  //
  // Copy constructor
  //
  head.Copy(*this);
}

//_______________________________________________________________________
AliHeader::AliHeader(Int_t run, Int_t event):
  fRun(run),
  fNvertex(0),
  fNprimary(0),
  fNtrack(0),
  fEvent(event),
  fEventNrInRun(0),
  fStack(0),
  fGenHeader(0)
{
  //
  // Standard constructor
  //
}

//_______________________________________________________________________
AliHeader::AliHeader(Int_t run, Int_t event, Int_t evNumber):
  fRun(run),
  fNvertex(0),
  fNprimary(0),
  fNtrack(0),
  fEvent(event),
  fEventNrInRun(evNumber),
  fStack(0),
  fGenHeader(0)
{
  //
  // Standard constructor
  //
}

//_______________________________________________________________________
void AliHeader::Reset(Int_t run, Int_t event)
{
  //
  // Resets the header with new run and event number
  //
  fRun=run;	
  fNvertex=0;
  fNprimary=0;
  fNtrack=0;
  fEvent=event;
}

//_______________________________________________________________________
void AliHeader::Reset(Int_t run, Int_t event, Int_t evNumber)
{
  //
  // Resets the header with new run and event number
  //
  fRun=run;	
  fNvertex=0;
  fNprimary=0;
  fNtrack=0;
  fEvent=event;
  fEventNrInRun=evNumber;
}

//_______________________________________________________________________
void AliHeader::Print(const char*) const
{
  //
  // Dumps header content
  //
  printf(
"\n=========== Header for run %d Event %d = beginning ======================================\n",
  fRun,fEvent);
  printf("              Number of Vertex %d\n",fNvertex);
  printf("              Number of Primary %d\n",fNprimary);
  printf("              Number of Tracks %d\n",fNtrack);
  printf(
  "=========== Header for run %d Event %d = end ============================================\n\n",
  fRun,fEvent);

}

//_______________________________________________________________________
AliStack* AliHeader::Stack() const
{
// Return pointer to stack
    return fStack;
}

//_______________________________________________________________________
void AliHeader::SetStack(AliStack* stack)
{
// Set pointer to stack
    fStack = stack;
}

//_______________________________________________________________________
void AliHeader::SetGenEventHeader(AliGenEventHeader* header)
{
// Set pointer to header for generated event
    fGenHeader = header;
}

//_______________________________________________________________________
AliGenEventHeader*  AliHeader::GenEventHeader() const
{
// Get pointer to header for generated event
    return fGenHeader;
}

//_______________________________________________________________________
void AliHeader::Copy(AliHeader&) const
{
  Fatal("Copy","Not implemented\n");
}



