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

/* $Id$ */

////////////////////////////////////////////////
//  space frame class                            /
////////////////////////////////////////////////

#include <TSystem.h>
#include <TVirtualMC.h>

#include "AliFRAMEv1.h"
#include "AliRun.h"
 
ClassImp(AliFRAMEv1)
 
//_____________________________________________________________________________
AliFRAMEv1::AliFRAMEv1()
{
// Constructor
}

//_____________________________________________________________________________
AliFRAMEv1::AliFRAMEv1(const char *name, const char *title)
  : AliFRAME(name,title)
{
// Constructor
  if(fDebug>1) printf("%s: Create FRAMEv1 object\n",ClassName());  
  fEuclidGeometry="$(ALICE_ROOT)/Euclid/frame1099i.euc";
  fEuclidMaterial="$(ALICE_ROOT)/Euclid/frame.tme";
}

 
//___________________________________________
void AliFRAMEv1::CreateGeometry()
{
//Begin_Html
/*
<img src="picts/frame.gif">
*/
//End_Html


//Begin_Html
/*
<img src="picts/tree_frame.gif">
*/
//End_Html

  char *filetmp;
  char topvol[5];
  
//
// The Space frame
  filetmp = gSystem->ExpandPathName(fEuclidGeometry.Data());
  FILE *file = fopen(filetmp,"r");
  delete [] filetmp;
  if(file) {
    fclose(file);
    if(fDebug) printf("%s: Reading FRAME geometry\n",ClassName());
    ReadEuclid(fEuclidGeometry.Data(),topvol);
  } else 
    Fatal("CreateGeometry","The Euclid file %s does not exist!\n",
	  fEuclidGeometry.Data());
//
// --- Place the FRAME ghost volume (B010) in its mother volume (ALIC)
//    and make it invisible
// 
//  AliMatrix(idrotm[2001],90.,0.,90.,90.,180.,0.);

  gMC->Gspos(topvol,1,"ALIC",0,0,0,0,"ONLY");

  gMC->Gsatt(topvol, "SEEN", 0);
}

 
//___________________________________________
void AliFRAMEv1::CreateMaterials()
{
// Create materials and media (from Euclid file)
  char *filetmp;
  if(fDebug) printf("%s: Create FRAMEv1 materials\n",ClassName());
  filetmp = gSystem->ExpandPathName(fEuclidMaterial.Data());
  FILE *file = fopen(filetmp,"r");
  delete [] filetmp;
  if(file) {
    fclose(file);
    ReadEuclidMedia(fEuclidMaterial.Data());
  } else {
    Warning("CreateMaterials","The material file %s does not exist!\n",
	    fEuclidMaterial.Data());
    exit(1);
  }
}

//_____________________________________________________________________________
void AliFRAMEv1::Init()
{
  //
  // Initialise the module after the geometry has been defined
  //

  if(fDebug) {
    printf("%s: **************************************"
	   " FRAME "
	   "**************************************\n",ClassName());
    printf("\n%s:      Version 1 of FRAME initialised, symmetric FRAME\n\n",ClassName());
    printf("%s: **************************************"
	   " FRAME "
	   "**************************************\n",ClassName());
  }
}
