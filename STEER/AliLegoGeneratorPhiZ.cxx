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
Revision 1.5.8.1  2002/06/06 14:18:33  hristov
Merged with v3-08-02

Revision 1.8  2002/10/14 14:57:32  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.5.10.1  2002/06/10 14:43:06  hristov
Merged with v3-08-02

Revision 1.7  2002/05/07 16:44:04  morsch
Correct initialization of origin[2]. (Thanks to Angela Badala)

Revision 1.6  2002/05/03 12:18:24  morsch
Print-out corrected.

Revision 1.5  2001/05/16 14:57:22  alibrary
New files for folders and Stack

Revision 1.4  2001/01/12 09:23:17  morsch
Correct order of phi and z.

Revision 1.3  2000/11/30 07:12:49  alibrary
Introducing new Rndm and QA classes

Revision 1.2  2000/10/27 11:40:01  morsch
Error in printouts corrected

Revision 1.1  2000/10/27 08:13:02  morsch
Lego generator for phi-z binning.

*/

#include "AliLegoGeneratorPhiZ.h"
#include "AliRun.h"

ClassImp(AliLegoGeneratorPhiZ)


//___________________________________________
void AliLegoGeneratorPhiZ::Generate()
{
// Create a geantino with kinematics corresponding to the current bins
// Here: Coor1 =  Z 
//       Coor2 =  phi.
   
  //
  // Rootinos are 0
   const Int_t kMpart = 0;
   Float_t orig[3], pmom[3];
   Float_t t, cosp, sinp;
   if (fCoor1Bin==-1) fCoor1Bin=fNCoor1;
   // Prepare for next step
   if(fCoor1Bin>=fNCoor1-1)
     if(fCoor2Bin>=fNCoor2-1) {
       Warning("Generate","End of Lego Generation");
       return;
     } else { 
       fCoor2Bin++;
       printf("Generating rays in Phi-bin:%d\n",fCoor2Bin);
       fCoor1Bin=0;
     } else fCoor1Bin++;
   fCurCoor1 = (fCoor1Min+(fCoor1Bin+0.5)*(fCoor1Max-fCoor1Min)/fNCoor1);
   fCurCoor2 = (fCoor2Min+(fCoor2Bin+0.5)*(fCoor2Max-fCoor2Min)/fNCoor2);

   Float_t phi   = fCurCoor2*TMath::Pi()/180.;
   
   cosp      = TMath::Cos(phi);
   sinp      = TMath::Sin(phi);
   
   pmom[0] = cosp;
   pmom[1] = sinp;
   pmom[2] = 0.;
   
   // --- Where to start
   orig[0] = orig[1] = orig[2] = 0;
   orig[2] = fCurCoor1;
   
   Float_t dalicz = 3000;
   if (fRadMin > 0) {
       t = PropagateCylinder(orig,pmom,fRadMin,dalicz);
       orig[0] += pmom[0]*t;
       orig[1] += pmom[1]*t;
       orig[2] += pmom[2]*t;
       if (TMath::Abs(orig[2]) > fZMax) return;
   }
   
   Float_t polar[3]={0.,0.,0.};
   Int_t ntr;
   gAlice->SetTrack(1, -1, kMpart, pmom, orig, polar, 0, kPPrimary, ntr);
   
}
