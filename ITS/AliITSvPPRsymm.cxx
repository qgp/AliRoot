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
Revision 1.38.4.1  2002/06/06 14:23:58  hristov
Merged with v3-08-02

Revision 1.42  2002/10/22 14:46:01  alibrary
Introducing Riostream.h

Revision 1.41  2002/10/14 14:57:09  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.38.6.3  2002/10/14 13:14:12  hristov
Updating VirtualMC to v3-09-02

Revision 1.40  2002/10/02 17:56:35  barbera
Bug in copy 37 of volume I570 corrected (thanks to J. Belikov)

Revision 1.39  2002/04/13 22:35:52  nilsen
Now symm is derived from asymm. This minimizes duplicated code because
only the geometry is different between asymm and symm.

Revision 1.38  2001/11/28 01:35:47  nilsen
Using standard constructors instead of default constructors for Clusterfinder,
Response, and FastSimulator.

Revision 1.37  2001/10/22 11:00:54  hristov
New naming schema of the rotation matrices in BuildGeometry() to avoid redefinition in other detectors (R.Barbera)

Revision 1.36  2001/10/19 21:32:35  nilsen
Minor changes to remove compliation warning on gcc 2.92.2 compiler, and
cleanded up a little bit of code.

Revision 1.35  2001/10/19 10:16:28  barbera
A bug corrected in the definition of a TPCON

Revision 1.34  2001/10/18 12:25:07  barbera
Detailed geometry in BuildGeometry() commented out (450 MB needed to compile the file). Six cylinders put back but improved by comparison with the ITS coarse geometry

Revision 1.33  2001/10/18 03:09:21  barbera
The method BuildGeometry() has been completely rewritten. Now display.C can display the detailed ITS geometry instead of the old six dummy cylunders.

Revision 1.32  2001/10/17 04:35:32  barbera
Checks for the det and chip thickness modified in order to set the dafault values to 200 um if the user chosen values are outside the range 100-300 um

Revision 1.31  2001/10/04 22:33:39  nilsen
Fixed bugs in SetDefaults.

Revision 1.30  2001/10/01 19:34:09  nilsen
Fixed a bug in asigning detector types in SetDefaults under SSD layer 6.

Revision 1.29  2001/06/07 14:42:14  barbera
Both chip and det thicknesses set to [100,300]

Revision 1.28  2001/05/31 19:24:47  barbera
Default values of SPD detector and chip thickness set to 200 microns as defined by the Technical Board

Revision 1.27  2001/05/30 16:15:47  fca
Correct comparison wiht AliGeant3::Class() introduced. Thanks to I.Hrivnacova

Revision 1.26  2001/05/30 15:55:35  hristov
Strings compared instead of pointers

Revision 1.25  2001/05/30 14:04:31  hristov
Dynamic cast replaced (F.Carminati)

Revision 1.24  2001/05/25 15:59:59  morsch
Overlaps corrected. (R. Barbera)

Revision 1.22  2001/05/16 08:17:49  hristov
Bug fixed in the StepManager to account for the difference in the geometry tree for the ITS pixels. This fixes both the funny distribution of pixel coordinates and the missing hits/digits/points in many sectors of the ITS pixel barrel. Also included is a patch to properly get and use the detector dimensions through out the ITS code. (B.Nilsen)

Revision 1.21  2001/05/10 00:12:59  nilsen
Finished fixing up the default segmentation for the PPR geometry.

Revision 1.20  2001/05/09 01:02:22  nilsen
Finished fixing SetDefaults for the segmentation of SPD, SDD, and SSD.

Revision 1.19  2001/05/03 08:40:15  barbera
Volume ITSD slightly modified to be consistent with v5. Some improvement in the printouts. The last commit did not complete successfully.

Revision 1.17  2001/05/01 22:40:42  nilsen
Partical update of SetDefault.

Revision 1.16  2001/04/22 13:48:09  barbera
New values of media parameters and thickness of SPD end-ladder electronics as given by Fabio Formenti

Revision 1.15  2001/04/04 07:02:16  barbera
Position of the cylinders holding rails corrected

Revision 1.14  2001/03/29 22:02:30  barbera
Some changes to the services due to the new drawings from the engineers.

Revision 1.13  2001/03/29 05:28:56  barbera
Rails material changed from aluminum to carbon fiber according with the decision of the last Technical Board

Revision 1.12  2001/03/28 06:40:20  barbera
Central and services mother volumes made consistenf for detailed and coarse geometry. Switch for rails added to the coarse geometries

Revision 1.11  2001/03/23 00:12:23  nilsen
Set Reading of AliITSgeom data from Geant3 common blocks as the default and
not a .det file. Removed redundent calls to BuildGeometry.

Revision 1.10  2001/03/15 13:47:55  barbera
Some service mother volumes modified

Revision 1.9  2001/03/13 18:13:30  barbera
Some mother volumes sligthly modified to eliminate an overlap with the absorber

Revision 1.8  2001/03/13 08:36:24  hristov
fabsf replaced by TMath::Abs

Revision 1.7  2001/03/13 00:43:43  barbera
Updated version of the PPR detailed geometry with symmetric services. Of course, the central part of the detector (volume ITSD and its daughters) is the same of AliITSvPPRasymm.cxx

Revision 1.6  2001/02/13 16:53:35  nilsen
Fixed a but when trying to use GEANT4. Needed to replace
if(!((TGeant3*)gMC)) with if(!(dynamic_casst<TGeant3*>(gMC)))
because just casting gMC to be TGeant3* even when it realy is a TGeant3 pointer
did not result in a zero value. For AliITSv5asymm and AliITSv5symm, needed
to fix a bug in the initilizers and a bug in BuildGeometry. This is now done
in the same way as in AliITSv5.cxx.

Revision 1.5  2001/02/09 20:06:26  nilsen
Fixed bug in distructor. Can't distroy fixxed length arrays. Thanks Peter.

Revision 1.4  2001/02/09 00:05:31  nilsen
Added fMajor/MinorVersion variables and made other changes to better make
use of the new code changes in AliITSgeom related classes.

Revision 1.3  2001/01/30 09:23:13  hristov
Streamers removed (R.Brun)

Revision 1.2  2001/01/26 20:01:19  hristov
Major upgrade of AliRoot code

Revision 1.1.2.1  2001/01/15 13:38:32  barbera
New ITS detailed geometry to be used for the PPR

Revision 1.12  2000/12/10 16:00:44  barbera
Added last definition of special media like end-ladder boxes and cones

Revision 1.11  2000/10/30 08:02:25  barbera
PCON's changed into simpler CONS and TUBS. Services now allow for the rails to go through them.

Revision 1.3.2.7  2000/10/27 17:20:00  barbera
Position of rails w.r.t. the interaction point corrected.

Revision 1.9  2000/10/27 13:31:29  barbera
Rails between ITS and TPC added.

Revision 1.8  2000/10/27 13:03:08  barbera
Small changes in the SPD volumes and materials

Revision 1.6  2000/10/16 14:45:37  barbera
Mother volume ITSD modified to avoid some overlaps

Revision 1.5  2000/10/16 13:49:15  barbera
Services volumes slightly modified and material added following Pierluigi Barberis' information

Revision 1.4  2000/10/07 15:33:07  barbera
Small corrections to the ITSV mother volume

Revision 1.3  2000/10/07 13:06:50  barbera
Some new materials and media defined

Revision 1.2  2000/10/07 10:42:43  barbera
Mother volume ITSV corrected

Revision 1.1  2000/10/06 23:09:12  barbera
New  geometry (symmetric services

Revision 1.20  2000/10/02 21:28:08  fca
Removal of useless dependecies via forward declarations

Revision 1.19  2000/07/10 16:07:19  fca
Release version of ITS code

Revision 1.14.2.2  2000/05/19 10:09:21  nilsen
fix for bug with HP and Sun unix + fix for event display in ITS-working branch

Revision 1.14.2.1  2000/03/04 23:45:19  nilsen
Fixed up the comments/documentation.

Revision 1.14  1999/11/25 06:52:56  fca
Correct value of drca

Revision 1.13.2.1  1999/11/25 06:52:21  fca
Correct value of drca

Revision 1.13  1999/10/27 11:16:26  fca
Correction of problem in geometry

Revision 1.12  1999/10/22 08:25:25  fca
remove double definition of destructors

Revision 1.11  1999/10/22 08:16:49  fca
Correct destructors, thanks to I.Hrivnacova

Revision 1.10  1999/10/06 19:56:50  fca
Add destructor

Revision 1.9  1999/10/05 08:05:09  fca
Minor corrections for uninitialised variables.

Revision 1.8  1999/09/29 09:24:20  fca
Introduction of the Copyright and cvs Log

*/

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  Inner Traking System version PPR  symmetric                             //
//  This class contains the base procedures for the Inner Tracking System   //
//                                                                          //
// Authors: R. Barbera                                                      //
// version 6.                                                               //
// Created  2000.                                                           //
//                                                                          //
//  NOTE: THIS IS THE  SYMMETRIC PPR geometry of the ITS.                   //
// THIS WILL NOT WORK                                                       //
// with the geometry or module classes or any analysis classes. You are     //
// strongly encouraged to uses AliITSv5.                                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
// See AliITSvPPRsymm::StepManager().
#include <Riostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <TMath.h>
#include <TGeometry.h>
#include <TNode.h>
#include <TTUBE.h>
#include <TTUBS.h>
#include <TPCON.h>
#include <TFile.h>    // only required for Tracking function?
#include <TCanvas.h>
#include <TObjArray.h>
#include <TLorentzVector.h>
#include <TObjString.h>
#include <TClonesArray.h>
#include <TBRIK.h>
#include <TSystem.h>

#include "AliMC.h"
#include "AliRun.h"
#include "AliMagF.h"
#include "AliConst.h"
#include "AliITSGeant3Geometry.h"
#include "AliITShit.h"
#include "AliITS.h"
#include "AliITSvPPRsymm.h"
#include "AliITSgeom.h"
#include "AliITSgeomSPD.h"
#include "AliITSgeomSDD.h"
#include "AliITSgeomSSD.h"
#include "AliITSDetType.h"
#include "AliITSresponseSPD.h"
#include "AliITSresponseSDD.h"
#include "AliITSresponseSSD.h"
#include "AliITSsegmentationSPD.h"
#include "AliITSsegmentationSDD.h"
#include "AliITSsegmentationSSD.h"
#include "AliITSsimulationSPD.h"
#include "AliITSsimulationSDD.h"
#include "AliITSsimulationSSD.h"
#include "AliITSClusterFinderSPD.h"
#include "AliITSClusterFinderSDD.h"
#include "AliITSClusterFinderSSD.h"


ClassImp(AliITSvPPRsymm)
 
//_____________________________________________________________________________
AliITSvPPRsymm::AliITSvPPRsymm() : AliITSvPPRasymm() {
////////////////////////////////////////////////////////////////////////
//    Standard default constructor for the ITS version 9.
////////////////////////////////////////////////////////////////////////
}
//_____________________________________________________________________________
AliITSvPPRsymm::AliITSvPPRsymm(const char *name, const char *title) : AliITSvPPRasymm(name, title){
////////////////////////////////////////////////////////////////////////
//    Standard constructor for the ITS version 9.
////////////////////////////////////////////////////////////////////////
}
//_____________________________________________________________________________
AliITSvPPRsymm::~AliITSvPPRsymm() {
////////////////////////////////////////////////////////////////////////
//    Standard destructor for the ITS version 9.
////////////////////////////////////////////////////////////////////////
}
//_____________________________________________________________________________
void AliITSvPPRsymm::CreateGeometry(){
////////////////////////////////////////////////////////////////////////
//    This routine defines and Creates the geometry for version 9 of the ITS.
////////////////////////////////////////////////////////////////////////
  
  //INNER RADII OF THE SILICON LAYERS 
  // Float_t rl[6]    = { 3.8095,7.,15.,24.,38.1,43.5765 };   
  //THICKNESSES OF LAYERS (in % radiation length)
  Float_t drl[6]   = { 1.03,1.03,0.94,0.95,0.91,0.87 };   
  //HALF LENGTHS OF LAYERS  
  // Float_t dzl[6]   = { 14.35,14.35,25.1,32.1,49.405,55.27 };
  //LENGTHS OF END-LADDER BOXES (ALL INCLUDED)
  // Float_t dzb[6]   = { 12.4,12.4,13.5,15.,7.5,7.5 };   
  //THICKNESSES OF END-LADDER BOXES (ALL INCLUDED)
  // Float_t drb[6]   = { rl[1]-rl[0],0.2,5.,5.,4.,4. };        

 
  Float_t dits[100], rlim, zmax;
  // Float_t zpos;
  // Float_t pcits[50]
  Float_t ztpc;
  Int_t idrotm[1999], i;
  Float_t dgh[100];


  // Define some variables for SPD

  Float_t dits1[3], di101[3], di107[3], di10b[3], di106[3];  // for layer 1 
  Float_t di103[3], di10a[3], di102[3];                      // for layer 1
  Float_t dits2[3], di1d1[3], di1d7[3], di20b[3], di1d6[3];  // for layer 2
  Float_t di1d3[3], di20a[3], di1d2[3];                      // for layer 2  
  Float_t di108[3], di104[3];                                // for both layers  

  Float_t ddet1=200.;     // total detector thickness on layer 1 (micron)
  Float_t dchip1=200.;    // total chip thickness on layer 1 (micron)
  
  Float_t ddet2=200.;     // total detector thickness on layer 2 (micron)                         
  Float_t dchip2=200.;    // total chip thickness on layer 2 (micron)
  
  Float_t dbus=300.;      // total bus thickness on both layers (micron)

  ddet1 = GetThicknessDet1();
  ddet2 = GetThicknessDet2();
  dchip1 = GetThicknessChip1();
  dchip2 = GetThicknessChip2();    

  if(ddet1 < 100. || ddet1 > 300.) {
     cout << "ITS - WARNING: the detector thickness for layer 1 is outside the range of [100,300] microns."
	  " The default value of 200 microns will be used." << endl;
	  ddet1=200.;
  }
  
  if(ddet2 < 100. || ddet2 > 300.) {
     cout << "ITS - WARNING: the detector thickness for layer 2 is outside the range of [100,300] microns."
	  " The default value of 200 microns will be used." << endl;
	  ddet2=200.;
  }
  
  if(dchip1 < 100. || dchip1 > 300.) {
     cout << "ITS - WARNING: the chip thickness for layer 1 is outside the range of [100,300] microns."
	  " The default value of 200 microns will be used." << endl;
	  dchip1=200.;
  }
  
  if(dchip2 < 100. || dchip2 > 300.) {
     cout << "ITS - WARNING: the chip thickness for layer 2 is outside the range of [100,300] microns."
	  " The default value of 200 microns will be used." << endl;
	  dchip2=200.;
  }      

  Int_t rails = 1;       // flag for rails (1 --> rails in; 0 --> rails out)
  
  Int_t fluid = 1;       // flag for the cooling fluid (1 --> water; 0 --> freon)

  rails = GetRails();

  fluid = GetCoolingFluid();

  if(rails != 0 && rails != 1) {
     cout << "ITS - WARNING: the switch for rails is not set neither to 0 (rails out) nor to 1 (rails in)." 
     " The default value of 1 (rails in) will be used." << endl;
     	
  }  
  
  if(fluid != 0 && fluid != 1) {
     cout << "ITS - WARNING: the switch for cooling fluid is not set neither to 0 (freon) nor to 1 (water)." 
     " The default value of 1 (water) will be used." << endl;  
  }       
   
  cout << "ITS: Detector thickness on layer 1 is set to " << ddet1 << " microns." << endl;
  cout << "ITS: Chip thickness on layer 1 is set to " << dchip1 << " microns." << endl;
  cout << "ITS: Detector thickness on layer 2 is set to " << ddet2 << " microns." << endl;
  cout << "ITS: Chip thickness on layer 2 is set to " << dchip2 << " microns." << endl;
  if(rails == 0 ) {
     cout << "ITS: Rails are out." << endl; 
  } else {
     cout << "ITS: Rails are in." << endl;
  }   
  if(fluid == 0 ) {
     cout << "ITS: The cooling fluid is freon." << endl; 
  } else {
     cout << "ITS: The cooling fluid is water." << endl;
  }   

  ddet1  = ddet1*0.0001/2.; // conversion from tot length in um to half in cm
  ddet2  = ddet2*0.0001/2.; // conversion from tot length in um to half in cm	
  dchip1 = dchip1*0.0001/2.;// conversion from tot length in um to half in cm	
  dchip2 = dchip2*0.0001/2.;// conversion from tot length in um to half in cm	
  dbus   = dbus*0.0001/2.;  // conversion from tot length in um to half in cm       
		
  Float_t deltax, deltay; 

  Int_t thickness = GetMinorVersion()/10;
  Int_t option    = GetMinorVersion() - 10*thickness;


  // Define some variables for SDD


  Float_t sin30, cos30;

  // SDD electronics+services main volumes
  Float_t I018dits[3], I024dits[3], I047dits[3], I048dits[3];

  // SDD detector ladder

  Float_t I302dits[3], I402dits[3], I004dits[3], I005dits[3];
  Float_t Y_SDD_sep = 0.20;
  Float_t ySDD;
  Int_t   iSDD;
  Float_t Z_SDD_lay3[6] = {18.55, 10.95, 3.70, -3.70, -11.20, -18.35};
  Float_t Z_SDD_lay4[8] = {25.75, 18.60, 11.00, 3.70, -3.70, -11.20, -18.45, -26.05};

  // ladder foot and end-ladder (frame and cooling)
  Float_t I028dits[3], I420dits[3], I421dits[3], I422dits[6], I423dits[3];
  Float_t I424dits[3], xI424, yI424;
  Float_t I425dits[3];
  Int_t    indI425;
  Float_t I029dits[4], I030dits[4], I031dits[3], I032dits[3];

  // SDD ladder frame and cooling
  Float_t SDD_CoolPipe[3] = {1.7000, -0.5500, 0.0000};
  Float_t I035dits[3], I037dits[3], I038dits[3];
  Float_t I039dits[3], xI039, yI039;
  Float_t I041dits[5];

  // SDD hybrid, chips and capacitors
  Float_t I050dits[3], xI050, yI050;
  Float_t I052dits[3], xI052, yI052;
  Float_t I042dits[3], xI042, yI042;
  Float_t xI042space = 0.17;
  Float_t I043dits[3], xI043, yI043;
  Float_t xI043space = 0.17;
  Float_t zchip, zChipSpace;
  Float_t I051dits[3], xI051, yI051, zI051, yI051space, xcap;
  Int_t     ichip, icap;

  // SDD microcables
  Float_t I044dits[4], xI044, yI044, volI044;
  Float_t xHV, yHV, zHV, xLV, yLV, zLV;
  Char_t   HVname[5], LVname[5];


  // Define media off-set
  
  Int_t *idtmed = fIdtmed->GetArray()-199;

  
  // Rotation matrices
  
  // SPD - option 'a' (this is NOT the default so leave commented)
  
  
  if (option == 1) {
  
     AliMatrix(idrotm[201],90.0,90.0,90.0,180.0,0.0,0.0);
     AliMatrix(idrotm[202],90.0,90.0,90.0,0.0,0.0,0.0);
     AliMatrix(idrotm[203],90.0,350.0,90.0,260.0,0.0,0.0);
     AliMatrix(idrotm[204],90.0,170.0,90.0,80.0,0.0,0.0);
     AliMatrix(idrotm[205],90.0,10.0,90.0,100.0,0.0,0.0);
     AliMatrix(idrotm[206],90.0,190.0,90.0,280.0,0.0,0.0);
     AliMatrix(idrotm[207],90.0,342.0,90.0,72.0,0.0,0.0);
     AliMatrix(idrotm[208],90.0,156.999893,90.0,246.999893,0.0,0.0);
     AliMatrix(idrotm[209],90.0,147.999802,90.0,237.999893,0.0,0.0);
     AliMatrix(idrotm[210],90.0,138.999802,90.0,228.999802,0.0,0.0);
     AliMatrix(idrotm[211],90.0,129.999802,90.0,219.999802,0.0,0.0);
     AliMatrix(idrotm[212],90.0,36.7896,90.0,126.789597,0.0,0.0);
     AliMatrix(idrotm[213],90.0,343.579712,90.0,73.579697,0.0,0.0);
     AliMatrix(idrotm[214],90.0,95.413696,90.0,185.413696,0.0,0.0);
     AliMatrix(idrotm[215],90.0,5.4141,90.0,95.414101,0.0,0.0);
     AliMatrix(idrotm[216],90.0,318.296906,90.0,48.296902,0.0,0.0);
     AliMatrix(idrotm[217],90.0,67.000099,90.0,157.000107,0.0,0.0);
     AliMatrix(idrotm[218],90.0,337.003998,90.0,67.003998,0.0,0.0);
     AliMatrix(idrotm[219],90.0,247.000305,90.0,337.000305,0.0,0.0);
     AliMatrix(idrotm[220],90.0,305.633514,90.0,35.633499,0.0,0.0);
     AliMatrix(idrotm[221],90.0,58.000198,90.0,148.000198,0.0,0.0);
     AliMatrix(idrotm[222],90.0,327.997101,90.0,57.997101,0.0,0.0 );
     AliMatrix(idrotm[223],90.0,237.994202,90.0,327.994202,0.0,0.0);
     AliMatrix(idrotm[224],90.0,296.627502,90.0,26.627399,0.0,0.0);
     AliMatrix(idrotm[225],90.0,48.994099,90.0,138.994095,0.0,0.0);
     AliMatrix(idrotm[226],90.0,318.990997,90.0,48.991001,0.0,0.0);
     AliMatrix(idrotm[227],90.0,228.988205,90.0,318.98819,0.0,0.0);
     AliMatrix(idrotm[228],90.0,287.621399,90.0,17.621401,0.0,0.0);
     AliMatrix(idrotm[229],90.0,39.988098,90.0,129.988098,0.0,0.0);
     AliMatrix(idrotm[230],90.0,309.984985,90.0,39.985001,0.0,0.0);
     AliMatrix(idrotm[231],90.0,327.2612,90.0,57.2612,0.0,0.0);
     AliMatrix(idrotm[232],90.0,237.261398,90.0,327.261414,0.0,0.0);
     AliMatrix(idrotm[233],90.0,252.000504,90.0,342.000488,0.0,0.0 );
     AliMatrix(idrotm[234],90.0,71.9991,90.0,161.9991,0.0,0.0);
     AliMatrix(idrotm[235],90.0,270.0,90.0,0.0,0.0,0.0);
     AliMatrix(idrotm[236],90.0,180.013702,90.0,270.013702,0.0,0.0);
     AliMatrix(idrotm[237],90.0,180.0,90.0,90.0,0.0,0.0);
     AliMatrix(idrotm[238],90.0,144.0,90.0,234.0,0.0,0.0);
     AliMatrix(idrotm[239],90.0,216.0,90.0,306.0,0.0,0.0);
     AliMatrix(idrotm[240],90.0,288.0,90.0,18.0,0.0,0.0);
     AliMatrix(idrotm[241],90.0,324.0,90.0,54.0,0.0,0.0);
     AliMatrix(idrotm[242],90.0,36.0,90.0,126.0,0.0,0.0);
     AliMatrix(idrotm[243],90.0,108.0,90.0,198.0,0.0,0.0);
     AliMatrix(idrotm[244],90.0,180.0,90.0,270.0,0.0,0.0);
     AliMatrix(idrotm[245],90.0,162.0,90.0,252.0,0.0,0.0);
     AliMatrix(idrotm[246],90.0,310.0,90.0,40.0,0.0,0.0);
     AliMatrix(idrotm[247],90.0,319.0,90.0,49.0,0.0,0.0);
     AliMatrix(idrotm[248],90.0,328.0,90.0,58.0,0.0,0.0);
     AliMatrix(idrotm[249],90.0,337.0,90.0,67.0,0.0,0.0);
     AliMatrix(idrotm[1003],90.0,73.5,90.0,163.5,0.0,0.0);
     AliMatrix(idrotm[1011],90.0,342.0,90.0,72.0,0.0,0.0);
     AliMatrix(idrotm[1039],90.0,72.0,90.0,162.0,0.0,0.0);
     AliMatrix(idrotm[1043],90.0,66.91,90.0,156.91,0.0,0.0);
     AliMatrix(idrotm[1065],90.0,144.0,90.0,234.0,0.0,0.0);
     AliMatrix(idrotm[1078],90.0,180.0,90.0,270.0,0.0,0.0);
     AliMatrix(idrotm[1088],90.0,57.41,90.0,147.41,0.0,0.0);
     AliMatrix(idrotm[1089],90.0,333.0,90.0,63.0,0.0,0.0);
     AliMatrix(idrotm[1090],90.0,351.0,90.0,81.0,0.0,0.0);
     AliMatrix(idrotm[1091],90.0,216.0,90.0,306.0,0.0,0.0);
     AliMatrix(idrotm[1092],90.0,27.0,90.0,117.0,0.0,0.0);
     AliMatrix(idrotm[1093],90.0,18.0,90.0,108.0,0.0,0.0);
     AliMatrix(idrotm[1094],90.0,9.0,90.0,99.0,0.0,0.0);
     AliMatrix(idrotm[1104],90.0,252.0,90.0,342.0,0.0,0.0);
     AliMatrix(idrotm[1106],90.0,36.0,90.0,126.0,0.0,0.0);
     AliMatrix(idrotm[1107],90.0,108.0,90.0,198.0,0.0,0.0);
     AliMatrix(idrotm[1108],90.0,324.0,90.0,54.0,180.0,0.0);
     AliMatrix(idrotm[1109],90.0,0.0,90.0,90.0,180.0,0.0);
     AliMatrix(idrotm[1110],90.0,36.0,90.0,126.0,180.0,0.0);
     AliMatrix(idrotm[1111],90.0,72.0,90.0,162.0,180.0,0.0);
     AliMatrix(idrotm[1112],90.0,108.0,90.0,198.0,180.0,0.0);
     AliMatrix(idrotm[1113],90.0,144.0,90.0,234.0,180.0,0.0);
     AliMatrix(idrotm[1114],90.0,180.0,90.0,270.0,180.0,0.0);
     AliMatrix(idrotm[1115],90.0,216.0,90.0,306.0,180.0,0.0);
     AliMatrix(idrotm[1116],90.0,252.0,90.0,342.0,180.0,0.0);
     AliMatrix(idrotm[1117],90.0,288.0,90.0,18.0,0.0,0.0);
     AliMatrix(idrotm[1118],90.0,288.0,90.0,18.0,180.0,0.0);
     AliMatrix(idrotm[1130],90.0,324.0,90.0,54.0,0.0,0.0);  

  }
  
  // SPD - option 'b' (this is the default)  
    
  if (option == 2) {

     AliMatrix(idrotm[201],90.0,0.0,90.0,90.0,0.0,0.0);
     AliMatrix(idrotm[202],90.0,90.0,90.0,0.0,0.0,0.0);
     AliMatrix(idrotm[203],90.0,350.0,90.0,260.0,0.0,0.0);
     AliMatrix(idrotm[204],90.0,170.0,90.0,80.0,0.0,0.0);
     AliMatrix(idrotm[205],90.0,10.0,90.0,100.0,0.0,0.0);
     AliMatrix(idrotm[206],90.0,190.0,90.0,280.0,0.0,0.0);
     AliMatrix(idrotm[207],90.0,342.0,90.0,72.0,0.0,0.0);
     AliMatrix(idrotm[208],90.0,156.999893,90.0,246.999893,0.0,0.0);
     AliMatrix(idrotm[209],90.0,147.999802,90.0,237.999893,0.0,0.0);
     AliMatrix(idrotm[210],90.0,138.999802,90.0,228.999802,0.0,0.0);
     AliMatrix(idrotm[211],90.0,129.999802,90.0,219.999802,0.0,0.0);
     AliMatrix(idrotm[212],90.0,36.7896,90.0,126.789597,0.0,0.0);
     AliMatrix(idrotm[213],90.0,343.579712,90.0,73.579697,0.0,0.0);
     AliMatrix(idrotm[214],90.0,95.413696,90.0,185.413696,0.0,0.0);
     AliMatrix(idrotm[215],90.0,5.4141,90.0,95.414101,0.0,0.0);
     AliMatrix(idrotm[216],90.0,318.296906,90.0,48.296902,0.0,0.0);
     AliMatrix(idrotm[217],90.0,67.000099,90.0,157.000107,0.0,0.0);
     AliMatrix(idrotm[218],90.0,337.003998,90.0,67.003998,0.0,0.0);
     AliMatrix(idrotm[219],90.0,247.000305,90.0,337.000305,0.0,0.0);
     AliMatrix(idrotm[220],90.0,305.633514,90.0,35.633499,0.0,0.0);
     AliMatrix(idrotm[221],90.0,58.000198,90.0,148.000198,0.0,0.0);
     AliMatrix(idrotm[222],90.0,327.997101,90.0,57.997101,0.0,0.0);
     AliMatrix(idrotm[223],90.0,237.994202,90.0,327.994202,0.0,0.0);
     AliMatrix(idrotm[224],90.0,296.627502,90.0,26.627399,0.0,0.0);
     AliMatrix(idrotm[225],90.0,48.994099,90.0,138.994095,0.0,0.0);
     AliMatrix(idrotm[226],90.0,318.990997,90.0,48.991001,0.0,0.0);
     AliMatrix(idrotm[227],90.0,228.988205,90.0,318.98819,0.0,0.0);
     AliMatrix(idrotm[228],90.0,287.621399,90.0,17.621401,0.0,0.0);
     AliMatrix(idrotm[229],90.0,39.988098,90.0,129.988098,0.0,0.0);
     AliMatrix(idrotm[230],90.0,309.984985,90.0,39.985001,0.0,0.0);
     AliMatrix(idrotm[231],90.0,327.2612,90.0,57.2612,0.0,0.0);
     AliMatrix(idrotm[232],90.0,237.261398,90.0,327.261414,0.0,0.0);
     AliMatrix(idrotm[233],90.0,252.000504,90.0,342.000488,0.0,0.0);
     AliMatrix(idrotm[234],90.0,71.9991,90.0,161.9991,0.0,0.0);
     AliMatrix(idrotm[235],90.0,270.0,90.0,0.0,0.0,0.0);
     AliMatrix(idrotm[236],90.0,180.013702,90.0,270.013702,0.0,0.0);
     AliMatrix(idrotm[237],90.0,90.0,90.0,180.0,0.0,0.0);
     AliMatrix(idrotm[238],90.0,180.0,90.0,270.0,0.0,0.0);
     AliMatrix(idrotm[239],90.0,162.0,90.0,252.0,0.0,0.0);
     AliMatrix(idrotm[240],90.0,310.0,90.0,40.0,0.0,0.0);
     AliMatrix(idrotm[241],90.0,319.0,90.0,49.0,0.0,0.0);
     AliMatrix(idrotm[242],90.0,328.0,90.0,58.0,0.0,0.0);
     AliMatrix(idrotm[243],90.0,337.0,90.0,67.0,0.0,0.0);
     AliMatrix(idrotm[244],90.0,216.0,90.0,306.0,0.0,0.0);
     AliMatrix(idrotm[245],90.0,36.0,90.0,126.0,0.0,0.0);
     AliMatrix(idrotm[246],90.0,108.0,90.0,198.0,0.0,0.0);
     AliMatrix(idrotm[247],90.0,144.0,90.0,234.0,0.0,0.0);
     AliMatrix(idrotm[248],90.0,288.0,90.0,18.0,0.0,0.0);
     AliMatrix(idrotm[249],90.0,324.0,90.0,54.0,0.0,0.0);  
     AliMatrix(idrotm[1003],90.0,73.5,90.0,163.5,0.0,0.0);
     AliMatrix(idrotm[1011],90.0,342.0,90.0,72.0,0.0,0.0);
     AliMatrix(idrotm[1039],90.0,72.0,90.0,162.0,0.0,0.0);
     AliMatrix(idrotm[1043],90.0,66.91,90.0,156.91,0.0,0.0);
     AliMatrix(idrotm[1065],90.0,144.0,90.0,234.0,0.0,0.0);
     AliMatrix(idrotm[1078],90.0,180.0,90.0,270.0,0.0,0.0);
     AliMatrix(idrotm[1088],90.0,57.41,90.0,147.41,0.0,0.0);
     AliMatrix(idrotm[1089],90.0,333.0,90.0,63.0,0.0,0.0);
     AliMatrix(idrotm[1090],90.0,351.0,90.0,81.0,0.0,0.0);
     AliMatrix(idrotm[1091],90.0,216.0,90.0,306.0,0.0,0.0);
     AliMatrix(idrotm[1092],90.0,27.0,90.0,117.0,0.0,0.0);
     AliMatrix(idrotm[1093],90.0,18.0,90.0,108.0,0.0,0.0);
     AliMatrix(idrotm[1094],90.0,9.0,90.0,99.0,0.0,0.0);
     AliMatrix(idrotm[1104],90.0,252.0,90.0,342.0,0.0,0.0);
     AliMatrix(idrotm[1106],90.0,36.0,90.0,126.0,0.0,0.0);
     AliMatrix(idrotm[1107],90.0,108.0,90.0,198.0,0.0,0.0);
     AliMatrix(idrotm[1108],90.0,324.0,90.0,54.0,180.0,0.0);
     AliMatrix(idrotm[1109],90.0,0.0,90.0,90.0,180.0,0.0);
     AliMatrix(idrotm[1110],90.0,36.0,90.0,126.0,180.0,0.0);
     AliMatrix(idrotm[1111],90.0,72.0,90.0,162.0,180.0,0.0);
     AliMatrix(idrotm[1112],90.0,108.0,90.0,198.0,180.0,0.0);
     AliMatrix(idrotm[1113],90.0,144.0,90.0,234.0,180.0,0.0);
     AliMatrix(idrotm[1114],90.0,180.0,90.0,270.0,180.0,0.0);
     AliMatrix(idrotm[1115],90.0,216.0,90.0,306.0,180.0,0.0);
     AliMatrix(idrotm[1116],90.0,252.0,90.0,342.0,180.0,0.0);
     AliMatrix(idrotm[1117],90.0,288.0,90.0,18.0,0.0,0.0);
     AliMatrix(idrotm[1118],90.0,288.0,90.0,18.0,180.0,0.0);
     AliMatrix(idrotm[1130],90.0,324.0,90.0,54.0,0.0,0.0);  

  }
    
  // SDD
  
  AliMatrix(idrotm[301],0.0,0.0,90.0,90.0,90.0,180.0);  
  AliMatrix(idrotm[302],0.0,0.0,90.0,90.0,90.0,0.0);
  AliMatrix(idrotm[303],180.0,0.0,90.0,90.0,90.0,0.0); 
  AliMatrix(idrotm[304],180.0,0.0,90.0,90.0,90.0,180.0); 
  AliMatrix(idrotm[305],90.0,347.14,90.0,77.14,0.0,0.0); 
  AliMatrix(idrotm[306],90.0,321.43,90.0,51.43,0.0,0.0); 
  AliMatrix(idrotm[307],90.0,295.71,90.0,25.71,0.0,0.0);
  AliMatrix(idrotm[308],90.0,244.29,90.0,334.29,0.0,0.0);
  AliMatrix(idrotm[309],90.0,218.57,90.0,308.57,0.0,0.0);
  AliMatrix(idrotm[310],90.0,167.14,90.0,257.14,0.0,0.0);
  AliMatrix(idrotm[311],90.0,141.43,90.0,231.43,0.0,0.0);  
  AliMatrix(idrotm[312],90.0,0.0,0.0,0.0,90.0,270.0);
  AliMatrix(idrotm[313],90.0,115.71,90.0,205.71,0.0,0.0); 
  AliMatrix(idrotm[314],90.0,335.45,90.0,65.45,0.0,0.0); 
  AliMatrix(idrotm[315],90.0,319.09,90.0,49.09,0.0,0.0); 
  AliMatrix(idrotm[316],90.0,302.73,90.0,32.73,0.0,0.0); 
  AliMatrix(idrotm[317],90.0,286.36,90.0,16.36,0.0,0.0);
  AliMatrix(idrotm[318],90.0,270.0,90.0,360.0,0.0,0.0);
  AliMatrix(idrotm[319],90.0,253.64,90.0,343.64,0.0,0.0);
  AliMatrix(idrotm[320],90.0,237.27,90.0,327.27,0.0,0.0);
  AliMatrix(idrotm[321],90.0,12.86,90.0,102.86,0.0,0.0);  
  AliMatrix(idrotm[322],90.0,220.91,90.0,310.91,0.0,0.0);
  AliMatrix(idrotm[323],90.0,204.55,90.0,294.55,0.0,0.0); 
  AliMatrix(idrotm[324],90.0,188.18,90.0,278.18,0.0,0.0); 
  AliMatrix(idrotm[325],90.0,171.82,90.0,261.82,0.0,0.0); 
  AliMatrix(idrotm[326],90.0,155.45,90.0,245.45,0.0,0.0); 
  AliMatrix(idrotm[327],90.0,139.09,90.0,229.09,0.0,0.0);
  AliMatrix(idrotm[328],90.0,122.73,90.0,212.73,0.0,0.0);
  AliMatrix(idrotm[329],90.0,106.36,90.0,196.36,0.0,0.0);
  AliMatrix(idrotm[330],90.0,73.64,90.0,163.64,0.0,0.0);    
  AliMatrix(idrotm[331],90.0,40.91,90.0,130.91,0.0,0.0);  
  AliMatrix(idrotm[332],90.0,24.55,90.0,114.55,0.0,0.0);
  AliMatrix(idrotm[333],90.0,38.57,90.0,128.57,0.0,0.0); 
  AliMatrix(idrotm[334],90.0,351.82,90.0,81.82,0.0,0.0); 
  AliMatrix(idrotm[335],90.0,8.18,90.0,98.18,0.0,0.0); 
  AliMatrix(idrotm[336],90.0,64.29,90.0,154.29,0.0,0.0); 
  AliMatrix(idrotm[337],111.0,300.0,21.0,300.0,90.0,30.0);
  AliMatrix(idrotm[338],69.0,240.0,159.0,240.0,90.0,150.0);
  AliMatrix(idrotm[339],111.0,240.0,21.0,240.0,90.0,150.0);
  AliMatrix(idrotm[340],69.0,300.0,159.0,300.0,90.0,30.0);  
  AliMatrix(idrotm[341],128.0,0.0,38.0,0.0,90.0,270.0);  
  AliMatrix(idrotm[342],90.0,240.0,180.0,0.0,90.0,330.);
  AliMatrix(idrotm[343],90.0,120.0,180.0,0.0,90.0,210.0); 
  AliMatrix(idrotm[344],90.0,0.0,180.0,0.0,90.0,90.0); 
  AliMatrix(idrotm[345],90.0,180.0,90.0,90.0,0.0,0.0); 
  AliMatrix(idrotm[346],90.0,300.0,90.0,30.0,0.0,0.0); 
  AliMatrix(idrotm[347],90.0,240.0,90.0,150.0,0.0,0.0);
  AliMatrix(idrotm[348],90.0,180.0,0.0,0.0,90.0,270.0);
  AliMatrix(idrotm[349],90.0,235.0,90.0,145.0,0.0,0.0);
  AliMatrix(idrotm[350],90.0,90.0,90.0,180.0,0.0,0.0);  
  AliMatrix(idrotm[351],90.0,305.0,90.0,35.0,0.0,0.0);  
  AliMatrix(idrotm[352],0.0,0.0,90.0,0.0,90.0,90.0);
  AliMatrix(idrotm[353],90.0,60.0,90.0,150.0,0.0,0.0); 
  AliMatrix(idrotm[354],90.0,120.0,90.0,30.0,0.0,0.0); 
  AliMatrix(idrotm[355],90.0,180.0,90.0,90.0,180.0,0.0); 
  AliMatrix(idrotm[356],90.0,270.0,90.0,0.0,0.0,0.0); 
  AliMatrix(idrotm[366],90.0,57.27,90.0,147.27,0.0,0.0); 
  AliMatrix(idrotm[386],90.0,192.86,90.0,282.86,0.0,0.0);  
   
  // SSD
  
  AliMatrix(idrotm[501],90.0,148.24,90.0,238.24,0.0,0.0);
  AliMatrix(idrotm[503],90.0,137.65,90.0,227.65,0.0,0.0); 
  AliMatrix(idrotm[504],90.0,127.06,90.0,217.06,0.0,0.0);  
  AliMatrix(idrotm[505],90.0,116.47,90.0,206.47,0.0,0.0);  
  AliMatrix(idrotm[506],90.0,105.88,90.0,195.88,0.0,0.0);  
  AliMatrix(idrotm[507],90.0,95.29,90.0,185.29,0.0,0.0);  
  AliMatrix(idrotm[508],90.0,84.71,90.0,174.71,0.0,0.0);
  AliMatrix(idrotm[509],90.0,74.12,90.0,164.12,0.0,0.0);
  AliMatrix(idrotm[510],90.0,63.53,90.0,153.53,0.0,0.0);  
  AliMatrix(idrotm[511],90.0,52.94,90.0,142.94,0.0,0.0);
  AliMatrix(idrotm[512],90.0,42.35,90.0,132.35,0.0,0.0);
  AliMatrix(idrotm[513],90.0,31.76,90.0,121.76,0.0,0.0); 
  AliMatrix(idrotm[514],90.0,10.59,90.0,100.59,0.0,0.0);  
  AliMatrix(idrotm[515],90.0,349.41,90.0,79.41,0.0,0.0);  
  AliMatrix(idrotm[516],90.0,338.82,90.0,68.82,0.0,0.0);  
  AliMatrix(idrotm[517],90.0,328.24,90.0,58.24,0.0,0.0);  
  AliMatrix(idrotm[518],90.0,317.65,90.0,47.65,0.0,0.0);
  AliMatrix(idrotm[519],90.0,307.06,90.0,37.06,0.0,0.0);
  AliMatrix(idrotm[520],90.0,296.47,90.0,26.47,0.0,0.0);  
  AliMatrix(idrotm[521],90.0,285.88,90.0,15.88,0.0,0.0);
  AliMatrix(idrotm[522],90.0,275.29,90.0,5.29,0.0,0.0);
  AliMatrix(idrotm[523],90.0,264.71,90.0,354.71,0.0,0.0); 
  AliMatrix(idrotm[524],90.0,254.12,90.0,344.12,0.0,0.0);  
  AliMatrix(idrotm[525],90.0,243.53,90.0,333.53,0.0,0.0);  
  AliMatrix(idrotm[526],90.0,232.94,90.0,322.94,0.0,0.0);  
  AliMatrix(idrotm[527],90.0,222.35,90.0,312.35,0.0,0.0);  
  AliMatrix(idrotm[528],90.0,211.76,90.0,301.76,0.0,0.0);
  AliMatrix(idrotm[529],90.0,190.59,90.0,280.59,0.0,0.0);
  AliMatrix(idrotm[530],90.0,169.41,90.0,259.41,0.0,0.0);  
  AliMatrix(idrotm[531],90.0,158.82,90.0,248.82,0.0,0.0);
  AliMatrix(idrotm[532],90.0,360.0,90.0,90.0,0.0,0.0);
  AliMatrix(idrotm[533],90.0,180.0,90.0,270.0,0.0,0.0); 
  AliMatrix(idrotm[534],90.0,189.47,90.0,279.47,0.0,0.0);  
  AliMatrix(idrotm[535],90.0,198.95,90.0,288.95,0.0,0.0);  
  AliMatrix(idrotm[537],90.0,217.89,90.0,307.89,0.0,0.0);  
  AliMatrix(idrotm[538],90.0,227.37,90.0,317.37,0.0,0.0);
  AliMatrix(idrotm[539],90.0,236.84,90.0,326.84,0.0,0.0);
  AliMatrix(idrotm[540],90.0,246.32,90.0,336.32,0.0,0.0);  
  AliMatrix(idrotm[541],90.0,255.79,90.0,345.79,0.0,0.0);
  AliMatrix(idrotm[542],90.0,265.26,90.0,355.26,0.0,0.0);
  AliMatrix(idrotm[543],90.0,274.74,90.0,4.74,0.0,0.0); 
  AliMatrix(idrotm[544],90.0,284.21,90.0,14.21,0.0,0.0);  
  AliMatrix(idrotm[545],90.0,293.68,90.0,23.68,0.0,0.0);  
  AliMatrix(idrotm[546],90.0,303.16,90.0,33.16,0.0,0.0);  
  AliMatrix(idrotm[547],90.0,312.63,90.0,42.63,0.0,0.0);  
  AliMatrix(idrotm[548],90.0,322.11,90.0,52.11,0.0,0.0);
  AliMatrix(idrotm[549],90.0,331.58,90.0,61.58,0.0,0.0);
  AliMatrix(idrotm[550],90.0,341.05,90.0,71.05,0.0,0.0);  
  AliMatrix(idrotm[551],90.0,350.53,90.0,80.53,0.0,0.0);
  AliMatrix(idrotm[552],90.0,9.47,90.0,99.47,0.0,0.0);
  AliMatrix(idrotm[553],90.0,18.95,90.0,108.95,0.0,0.0); 
  AliMatrix(idrotm[555],90.0,37.89,90.0,127.89,0.0,0.0);  
  AliMatrix(idrotm[556],90.0,47.37,90.0,137.37,0.0,0.0);  
  AliMatrix(idrotm[557],90.0,56.84,90.0,146.84,0.0,0.0);  
  AliMatrix(idrotm[558],90.0,66.32,90.0,156.32,0.0,0.0);
  AliMatrix(idrotm[559],90.0,75.79,90.0,165.79,0.0,0.0);
  AliMatrix(idrotm[560],90.0,85.26,90.0,175.26,0.0,0.0);  
  AliMatrix(idrotm[561],90.0,94.74,90.0,184.74,0.0,0.0);
  AliMatrix(idrotm[562],90.0,104.21,90.0,194.21,0.0,0.0);
  AliMatrix(idrotm[563],90.0,113.68,90.0,203.68,0.0,0.0); 
  AliMatrix(idrotm[564],90.0,123.16,90.0,213.16,0.0,0.0);  
  AliMatrix(idrotm[565],90.0,132.63,90.0,222.63,0.0,0.0);  
  AliMatrix(idrotm[566],90.0,142.11,90.0,232.11,0.0,0.0);  
  AliMatrix(idrotm[567],90.0,151.58,90.0,241.58,0.0,0.0);  
  AliMatrix(idrotm[568],90.0,161.05,90.0,251.05,0.0,0.0);
  AliMatrix(idrotm[569],90.0,170.53,90.0,260.53,0.0,0.0);
  AliMatrix(idrotm[570],90.0,180.0,90.0,90.0,180.0,0.0);  
  AliMatrix(idrotm[571],90.0,0.0,0.0,0.0,90.0,270.0);
  AliMatrix(idrotm[572],90.0,180.0,0.0,0.0,90.0,270.0);
  AliMatrix(idrotm[573],90.0,180.0,90.0,90.0,0.0,0.0); 
  AliMatrix(idrotm[575],90.0,120.0,180.0,0.0,90.0,210.0);  
  AliMatrix(idrotm[576],65.71,300.0,90.0,30.0,24.29,120.0);  
  AliMatrix(idrotm[577],114.29,300.0,90.0,30.0,155.71,120.0);  
  AliMatrix(idrotm[579],65.71,240.0,90.0,150.0,24.29,60.0);
  AliMatrix(idrotm[580],114.29,240.0,90.0,150.0,155.71,60.0);  
  AliMatrix(idrotm[581],90.0,240.0,180.0,0.0,90.0,330.0);
  AliMatrix(idrotm[583],90.0,0.0,180.0,0.0,90.0,90.0); 
  AliMatrix(idrotm[584],90.0,180.0,180.0,0.0,90.0,90.0);  
  AliMatrix(idrotm[586],180.0,0.0,90.0,90.0,90.0,0.0);  
  AliMatrix(idrotm[618],90.0,201.18,90.0,291.18,0.0,0.0);
  AliMatrix(idrotm[620],90.0,28.42,90.0,118.42,0.0,0.0);  
  AliMatrix(idrotm[623],90.0,208.42,90.0,298.42,0.0,0.0);
  AliMatrix(idrotm[633],132.46,0.0,90.0,90.0,42.46,360.0);
  AliMatrix(idrotm[653],90.0,21.18,90.0,111.18,0.0,0.0); 

  
  // SDD cone

  AliMatrix(idrotm[846],90.0,300.0,90.0,30.0,0.0,0.0);
  AliMatrix(idrotm[851],90.0,305.0,90.0,35.0,0.0,0.0);
  AliMatrix(idrotm[853],90.0,60.0,90.0,150.0,0.0,0.0);
  AliMatrix(idrotm[856],90.0,0.0,90.0,90.0,180.0,0.0);
  AliMatrix(idrotm[857],90.0,5.0,90.0,95.0,180.0,0.0);
  AliMatrix(idrotm[858],90.0,65.0,90.0,155.0,180.0,0.0);
  AliMatrix(idrotm[859],90.0,305.0,90.0,35.0,180.0,0.0);
  AliMatrix(idrotm[860],90.0,245.0,90.0,335.0,180.0,0.0);
  AliMatrix(idrotm[861],90.0,185.0,90.0,275.0,180.0,0.0);
  AliMatrix(idrotm[862],90.0,125.0,90.0,215.0,180.0,0.0);
  AliMatrix(idrotm[863],90.0,257.5,90.0,347.5,180.0,0.0);
  AliMatrix(idrotm[864],90.0,227.5,90.0,317.5,180.0,0.0);
  AliMatrix(idrotm[865],90.0,197.5,90.0,287.5,180.0,0.0);
  AliMatrix(idrotm[867],90.0,167.5,90.0,257.5,180.0,0.0);
  AliMatrix(idrotm[868],90.0,287.5,90.0,17.5,0.0,0.0);  
  AliMatrix(idrotm[869],90.0,137.5,90.0,227.5,180.0,0.0);
  AliMatrix(idrotm[870],90.0,107.5,90.0,197.5,180.0,0.0);
  AliMatrix(idrotm[871],90.0,77.5,90.0,167.5,180.0,0.0);
  AliMatrix(idrotm[872],90.0,47.5,90.0,137.5,180.0,0.0);
  AliMatrix(idrotm[873],90.0,17.5,90.0,107.5,180.0,0.0);
  AliMatrix(idrotm[874],90.0,347.5,90.0,77.5,180.0,0.0);
  AliMatrix(idrotm[875],90.0,317.5,90.0,47.5,180.0,0.0);
  AliMatrix(idrotm[876],90.0,287.5,90.0,17.5,180.0,0.0);
  AliMatrix(idrotm[877],90.0,185.0,90.0,275.0,0.0,0.0);
  AliMatrix(idrotm[878],90.0,180.0,90.0,270.0,0.0,0.0);  
  AliMatrix(idrotm[879],90.0,125.0,90.0,215.0,0.0,0.0);
  AliMatrix(idrotm[880],90.0,65.0,90.0,155.0,0.0,0.0);
  AliMatrix(idrotm[881],90.0,5.0,90.0,95.0,0.0,0.0);
  AliMatrix(idrotm[882],90.0,245.0,90.0,335.0,0.0,0.0);
  AliMatrix(idrotm[883],90.0,47.5,90.0,137.5,0.0,0.0);
  AliMatrix(idrotm[884],90.0,77.5,90.0,167.5,0.0,0.0);
  AliMatrix(idrotm[885],90.0,107.5,90.0,197.5,0.0,0.0);
  AliMatrix(idrotm[887],90.0,137.5,90.0,227.5,0.0,0.0);
  AliMatrix(idrotm[888],90.0,167.5,90.0,257.5,0.0,0.0);
  AliMatrix(idrotm[889],90.0,197.5,90.0,287.5,0.0,0.0);
  AliMatrix(idrotm[890],90.0,227.5,90.0,317.5,0.0,0.0);
  AliMatrix(idrotm[891],90.0,347.5,90.0,77.5,0.0,0.0);
  AliMatrix(idrotm[892],90.0,317.5,90.0,47.5,0.0,0.0);
  AliMatrix(idrotm[893],90.0,257.5,90.0,347.5,0.0,0.0);
  AliMatrix(idrotm[894],90.0,270.0,0.0,0.0,90.0,180.0);
  AliMatrix(idrotm[895],90.0,286.36,0.0,0.0,90.0,196.36);
  AliMatrix(idrotm[896],90.0,302.73,0.0,0.0,90.0,212.73);
  AliMatrix(idrotm[897],90.0,319.09,0.0,0.0,90.0,229.09);
  AliMatrix(idrotm[898],90.0,17.5,90.0,107.5,0.0,0.0);
  AliMatrix(idrotm[899],90.0,335.45,0.0,0.0,90.0,245.45);
  AliMatrix(idrotm[900],90.0,351.82,0.0,0.0,90.0,261.82);
  AliMatrix(idrotm[901],90.0,8.18,0.0,0.0,90.0,278.18);
  AliMatrix(idrotm[902],90.0,24.55,0.0,0.0,90.0,294.55);
  AliMatrix(idrotm[903],90.0,40.91,0.0,0.0,90.0,310.91);
  AliMatrix(idrotm[904],90.0,57.27,0.0,0.0,90.0,327.27);
  AliMatrix(idrotm[905],90.0,73.64,0.0,0.0,90.0,343.64);
  AliMatrix(idrotm[906],90.0,90.0,0.0,0.0,90.0,360.0);
  AliMatrix(idrotm[907],90.0,106.36,0.0,0.0,90.0,16.36);
  AliMatrix(idrotm[908],90.0,122.73,0.0,0.0,90.0,32.73);
  AliMatrix(idrotm[909],90.0,139.09,0.0,0.0,90.0,49.09);
  AliMatrix(idrotm[910],90.0,155.45,0.0,0.0,90.0,65.45);
  AliMatrix(idrotm[911],90.0,171.82,0.0,0.0,90.0,81.82);
  AliMatrix(idrotm[912],90.0,188.18,0.0,0.0,90.0,98.18);
  AliMatrix(idrotm[913],90.0,204.55,0.0,0.0,90.0,114.55);
  AliMatrix(idrotm[914],90.0,220.91,0.0,0.0,90.0,130.91);
  AliMatrix(idrotm[915],90.0,237.27,0.0,0.0,90.0,147.27);
  AliMatrix(idrotm[916],90.0,253.64,0.0,0.0,90.0,163.64);
  AliMatrix(idrotm[917],90.0,295.71,0.0,0.0,90.0,205.71);
  AliMatrix(idrotm[918],90.0,321.43,0.0,0.0,90.0,231.43);
  AliMatrix(idrotm[919],90.0,347.14,0.0,0.0,90.0,257.14);
  AliMatrix(idrotm[920],90.0,12.86,0.0,0.0,90.0,282.86);
  AliMatrix(idrotm[921],90.0,38.57,0.0,0.0,90.0,308.57);
  AliMatrix(idrotm[922],90.0,64.29,0.0,0.0,90.0,334.29);
  AliMatrix(idrotm[923],90.0,115.71,0.0,0.0,90.0,25.71);
  AliMatrix(idrotm[924],90.0,141.43,0.0,0.0,90.0,51.43);
  AliMatrix(idrotm[925],90.0,167.14,0.0,0.0,90.0,77.14);
  AliMatrix(idrotm[926],90.0,192.86,0.0,0.0,90.0,102.86);
  AliMatrix(idrotm[927],90.0,218.57,0.0,0.0,90.0,128.57);
  AliMatrix(idrotm[928],90.0,244.29,0.0,0.0,90.0,154.29);
  AliMatrix(idrotm[929],90.0,120.0,90.0,210.0,0.0,0.0);
  AliMatrix(idrotm[930],90.0,240.0,90.0,330.0,0.0,0.0);
  AliMatrix(idrotm[931],90.0,60.0,90.0,150.0,180.0,0.0);
  AliMatrix(idrotm[932],90.0,120.0,90.0,210.0,180.0,0.0);
  AliMatrix(idrotm[933],90.0,180.0,90.0,270.0,180.0,0.0);
  AliMatrix(idrotm[934],90.0,240.0,90.0,330.0,180.0,0.0);
  AliMatrix(idrotm[935],90.0,300.0,90.0,30.0,180.0,0.0);

  // SSD cone

  AliMatrix(idrotm[701],90.0,0.0,90.0,90.0,180.0,0.0);
  AliMatrix(idrotm[702],90.0,347.5,90.0,77.5,180.0,0.0);
  AliMatrix(idrotm[703],90.0,17.5,90.0,107.5,180.0,0.0);
  AliMatrix(idrotm[704],90.0,47.5,90.0,137.5,180.0,0.0);
  AliMatrix(idrotm[705],90.0,77.5,90.0,167.5,180.0,0.0);
  AliMatrix(idrotm[706],90.0,107.5,90.0,197.5,180.0,0.0);
  AliMatrix(idrotm[707],90.0,137.5,90.0,227.5,180.0,0.0);
  AliMatrix(idrotm[708],90.0,167.5,90.0,257.5,180.0,0.0);
  AliMatrix(idrotm[709],90.0,197.5,90.0,287.5,180.0,0.0);
  AliMatrix(idrotm[710],90.0,227.5,90.0,317.5,180.0,0.0);
  AliMatrix(idrotm[711],90.0,257.5,90.0,347.5,180.0,0.0);
  AliMatrix(idrotm[712],90.0,287.5,90.0,17.5,180.0,0.0);
  AliMatrix(idrotm[713],90.0,317.5,90.0,47.5,180.0,0.0);
  AliMatrix(idrotm[714],90.0,328.4,90.0,58.4,180.0,0.0);
  AliMatrix(idrotm[715],90.0,28.4,90.0,118.4,180.0,0.0);
  AliMatrix(idrotm[716],90.0,88.4,90.0,178.4,180.0,0.0);
  AliMatrix(idrotm[717],90.0,148.4,90.0,238.4,180.0,0.0);
  AliMatrix(idrotm[718],90.0,208.4,90.0,298.4,180.0,0.0);
  AliMatrix(idrotm[719],90.0,268.4,90.0,358.4,180.0,0.0);
  AliMatrix(idrotm[720],90.0,28.4,90.0,118.4,0.0,0.0);
  AliMatrix(idrotm[721],90.0,88.4,90.0,178.4,0.0,0.0);
  AliMatrix(idrotm[722],90.0,148.4,90.0,238.4,0.0,0.0);
  AliMatrix(idrotm[723],90.0,208.4,90.0,298.4,0.0,0.0);
  AliMatrix(idrotm[724],90.0,268.4,90.0,358.4,0.0,0.0);
  AliMatrix(idrotm[725],90.0,328.4,90.0,58.4,0.0,0.0);
  AliMatrix(idrotm[726],90.0,77.5,90.0,167.5,0.0,0.0);
  AliMatrix(idrotm[727],90.0,107.5,90.0,197.5,0.0,0.0);
  AliMatrix(idrotm[728],90.0,137.5,90.0,227.5,0.0,0.0);
  AliMatrix(idrotm[729],90.0,167.5,90.0,257.5,0.0,0.0);
  AliMatrix(idrotm[730],90.0,227.5,90.0,317.5,0.0,0.0);
  AliMatrix(idrotm[731],90.0,257.5,90.0,347.5,0.0,0.0);
  AliMatrix(idrotm[732],90.0,317.5,90.0,47.5,0.0,0.0);  
  AliMatrix(idrotm[733],90.0,197.5,90.0,287.5,0.0,0.0);
  AliMatrix(idrotm[734],90.0,347.5,90.0,77.5,0.0,0.0);
  AliMatrix(idrotm[735],90.0,47.5,90.0,137.5,0.0,0.0);
  AliMatrix(idrotm[768],90.0,287.5,90.0,17.5,0.0,0.0);
  AliMatrix(idrotm[798],90.0,17.5,90.0,107.5,0.0,0.0);

  // Services
    
  AliMatrix(idrotm[200], 90., 0., 90., 90., 180., 0.);  


  //     CONVERT INTO CM (RL(SI)=9.36 CM) 
  
  for (i = 0; i < 6; ++i) {
    drl[i] = drl[i] / 100. * 9.36;
  }
    
  //     FIELD CAGE HALF LENGTH 
  
  rlim  = 50.;
  zmax  = 74.;
  ztpc = 284.;
  
  // --- Define ghost volume containing the whole ITS (including services) 
  //     and fill it with air 
  
  dgh[0] = 0.;
  dgh[1] = 360.;
  dgh[2] = 16.;
  dgh[3] = -ztpc-5.-0.1;
  dgh[4] = 62.4;
  dgh[5] = 85.;
  dgh[6] = -ztpc;
  dgh[7] = 62;
  dgh[8] = 85.;
  dgh[9] = -ztpc;
  dgh[10] = 62;
  dgh[11] = 62+4.;
  dgh[12] = -97.5;
  dgh[13] = 46;
  dgh[14] = rlim+6;
  dgh[15] = -zmax;
  dgh[16] = 46;
  dgh[17] = rlim+6;
  dgh[18] = -48;
  dgh[19] = 6;
  dgh[20] = rlim+6; 
  dgh[21] = -28.6;
  dgh[22] = 6;
  dgh[23] = rlim+6;
  dgh[24] = -27.6;
  dgh[25] = 3.295;
  dgh[26] = rlim+6;
  dgh[27] = 27.6;
  dgh[28] = 3.295;
  dgh[29] = rlim+6;
  dgh[30] = 28.6;
  dgh[31] = 6;
  dgh[32] = rlim+6;
  dgh[33] = 48;
  dgh[34] = 6;
  dgh[35] = rlim+6;
  dgh[36] = zmax;
  dgh[37] = 46;
  dgh[38] = rlim+6;
  dgh[39] = 97.5;
  dgh[40] = 46;
  dgh[41] = rlim+6;
  dgh[42] = ztpc;
  dgh[43] = 62;
  dgh[44] = 62+4.;
  dgh[45] = ztpc;
  dgh[46] = 62;
  dgh[47] = 85.;
  dgh[48] = ztpc+4.+0.1;
  dgh[49] = 62.4;
  dgh[50] = 85.;
  gMC->Gsvolu("ITSV", "PCON", idtmed[205], dgh, 51);


  
  // --- Place the ghost volume in its mother volume (ALIC) and make it 
  //     invisible 
  
  gMC->Gspos("ITSV", 1, "ALIC", 0., 0., 0., 0, "ONLY");
  //gMC->Gsatt("ITSV", "SEEN", 0); 


  // --- Define ghost volume containing the six layers and fill it with air 
  
  dgh[0] = 0.;
  dgh[1] = 360.;
  dgh[2] = 8.;
  dgh[3] = -zmax;  
  dgh[4] = 46.;
  dgh[5] = rlim;
  dgh[6] = -47.5;    
  dgh[7] = 6.005;
  dgh[8] = rlim;
  dgh[9] = -28.5;    
  dgh[10] = 6.005;
  dgh[11] = rlim;  
  dgh[12] = -27.5;   
  dgh[13] = 3.3;
  dgh[14] = rlim;
  dgh[15] = 27.5;    
  dgh[16] = 3.3;
  dgh[17] = rlim;
  dgh[18] = 28.5;    
  dgh[19] = 6.005;
  dgh[20] = rlim;
  dgh[21] = 47.5;    
  dgh[22] = 6.005;
  dgh[23] = rlim;
  dgh[24] = zmax;    
  dgh[25] = 46.;
  dgh[26] = rlim;
  gMC->Gsvolu("ITSD", "PCON", idtmed[205], dgh, 27);
  
  // --- Place the ghost volume in its mother volume (ITSV) and make it 
  //     invisible 
  
  gMC->Gspos("ITSD", 1, "ITSV", 0., 0., 0., 0, "ONLY");
  //gMC->Gsatt("ITSD", "SEEN", 0);

  // --- Define SPD (option 'a') volumes ----------------------------
  
  // SPD - option 'a' 
  // (this is NOT the default)

  if (option == 1) {
  
     dits[0] = 3.7;
     dits[1] = 7.75;
     dits[2] = 26.1;
     gMC->Gsvolu("IT12", "TUBE", idtmed[254], dits, 3);   

     dits[0] = 3.7;
     dits[1] = 7.7;
     dits[2] = 24;
     dits[3] = 57;
     dits[4] = 100;
     gMC->Gsvolu("I12A", "TUBS", idtmed[254], dits, 5);    // sector

     di10a[0] = 0.843;
     di10a[1] = ddet1+dchip1+dbus+0.0025;
     di10a[2] = 19.344;
     gMC->Gsvolu("I10A", "BOX ", idtmed[254], di10a, 3);    // mother volume
                                                            // on layer 1
     di20a[0] = 0.843;
     di20a[1] = ddet2+dchip2+dbus+0.0025;
     di20a[2] = 19.344;
     gMC->Gsvolu("I20A", "BOX ", idtmed[254], di20a, 3);    // mother volume
                                                            // on layer 2
     dits[0] = 1.3673;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I123", "BOX ", idtmed[253], dits, 3);

     dits[0] = 0.06;
     dits[1] = 0.08;
     dits[2] = 24;
     dits[3] = -36.79;
     dits[4] = 21.834;
     gMC->Gsvolu("I121", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 0.1253;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I122", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.04;
     dits[1] = 0.06 ;
     dits[2] = 24;
     dits[3] = 126.79;
     dits[4] = 270;
     gMC->Gsvolu("I120", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 0.1134;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I144", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.25;
     dits[1] = 0.06;
     dits[2] = 24;
     gMC->Gsvolu("I113", "BOX ", idtmed[254], dits, 3);  

     dits[0] = 0.077;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I143", "BOX ", idtmed[253], dits, 3);   

     dits[0] = 0.04;
     dits[1] = 0.06;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 90;
     gMC->Gsvolu("I142", "TUBS", idtmed[253], dits, 5); 

     dits[0] = 0.0695;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I141", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.06;
     dits[1] = 0.08;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 108;
     gMC->Gsvolu("I140", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 0.1835;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I139", "BOX ", idtmed[253], dits, 3);

     dits[0] = 0.1894 ;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I138", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.04;
     dits[1] = 0.06;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 75.261;
     gMC->Gsvolu("I137", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 1.3401;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I136", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.05;
     dits[1] = 0.07;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 72.739;
     gMC->Gsvolu("I135", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 0.1193;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I134", "BOX ", idtmed[253], dits, 3);    

     dits[0] = 0.163;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I133", "BOX ", idtmed[253], dits, 3);   

     dits[0] = 0.04;
     dits[1] = 0.06;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 157.633;
     gMC->Gsvolu("I132", "TUBS", idtmed[253], dits, 5); 

     dits[0] = 0.2497;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I131", "BOX ", idtmed[253], dits, 3); 

     dits[0] = 0.06;
     dits[1] = 0.08;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 148.633;
     gMC->Gsvolu("I130", "TUBS", idtmed[253], dits, 5); 

     dits[0] = 0.292;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I129", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.163;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I128", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.04;
     dits[1] = 0.06;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 161.297;
     gMC->Gsvolu("I126", "TUBS", idtmed[253], dits, 5);

     dits[0] = 0.2433;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I125", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.06;
     dits[1] = 0.08;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 42.883;
     gMC->Gsvolu("I124", "TUBS", idtmed[253], dits, 5);  

     di103[0] = 0.793;
     di103[1] = ddet1+dchip1;
     di103[2] = 3.536;
     gMC->Gsvolu("I103", "BOX ", idtmed[254], di103, 3); // contains det and chip  
                                                         // layer 1
     dits[0] = 0.793;
     dits[1] = 0.475;  //0.685; 0.015
     dits[2] = 2.5;
     gMC->Gsvolu("I105", "BOX ", idtmed[290], dits, 3);  

     di104[0] = 0.843;
     di104[1] = dbus;
     di104[2] = 14.344;
     gMC->Gsvolu("I104", "BOX ", idtmed[275], di104, 3);  // bus for both layers

     di1d3[0] = 0.793;
     di1d3[1] = ddet2+dchip2;
     di1d3[2] = 3.536;
     gMC->Gsvolu("I1D3", "BOX ", idtmed[254], di1d3, 3); // contains det and chip
                                                         // layer 2
     dits[0] = 0.793;
     dits[0] = 0.06;
     dits[1] = 0.08;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 80;
     gMC->Gsvolu("I112", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 0.04;
     dits[1] = 0.06;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 80;
     gMC->Gsvolu("I111", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 0.15;
     dits[1] = 0.0146;
     dits[2] = 24;
     gMC->Gsvolu("I118", "BOX ", idtmed[273], dits, 3);  

     dits[0] = 0.1315;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I110", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.025;
     dits[1] = 0.035;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 180;
     gMC->Gsvolu("I114", "TUBS", idtmed[264], dits, 5);  

     if (fluid == 1) {
        dits[0] = 0;
        dits[1] = 0.025;
        dits[2] = 24;
        dits[3] = 0;
        dits[4] = 180;
        gMC->Gsvolu("I115", "TUBS", idtmed[211], dits, 5); // set water as cooling fluid   
     } else {
        dits[0] = 0;
        dits[1] = 0.025;
        dits[2] = 24;
        dits[3] = 0;
        dits[4] = 180;
        gMC->Gsvolu("I115", "TUBS", idtmed[212], dits, 5); // set freon as cooling fluid       
     }
     
     dits[0] = 0.063;
     dits[1] = 0.035;
     dits[2] = 24;
     gMC->Gsvolu("I116", "BOX ", idtmed[264], dits, 3); 

     di102[0] = 0.793;
     di102[1] = dchip1;
     di102[2] = 0.68;
     gMC->Gsvolu("I102", "BOX ", idtmed[201], di102, 3);   // chip layer 1
	  
     di1d2[0] = 0.793;
     di1d2[1] = dchip2;
     di1d2[2] = 0.68;
     gMC->Gsvolu("I1D2", "BOX ", idtmed[201], di1d2, 3);   // chip	layer 2

     di101[0] = 0.705;
     di101[1] = ddet1;
     di101[2] = 3.536;
     gMC->Gsvolu("I101", "BOX ", idtmed[250], di101, 3);   // contains detector 
                                                           // layer 1
     di1d1[0] = 0.705;
     di1d1[1] = ddet2;
     di1d1[2] = 3.536;
     gMC->Gsvolu("I1D1", "BOX ", idtmed[250], di1d1, 3);   // contains detector 
                                                           // layer 2
     if (fluid == 1) {
        dits[0] = 0.063;
        dits[1] = 0.025;
        dits[2] = 24;
        gMC->Gsvolu("I117", "BOX ", idtmed[211], dits, 3); // set water as cooling fuid
     } else {
        dits[0] = 0.063;
        dits[1] = 0.025;
        dits[2] = 24;
        gMC->Gsvolu("I117", "BOX ", idtmed[212], dits, 3); // set freon as cooling fluid
     }

     dits1[0] = 0.64;
     dits1[1] = ddet1;
     dits1[2] = 3.48;
     gMC->Gsvolu("ITS1", "BOX ", idtmed[200], dits1, 3);   // detector layer 1

     dits2[0] = 0.64;
     dits2[1] = ddet2;
     dits2[2] = 3.48;
     gMC->Gsvolu("ITS2", "BOX ", idtmed[200], dits2, 3);   // detector layer 2

     dits[0] = 3.701;
     dits[1] = 7.699;
     dits[2] = 4;
     dits[3] = 57.1;
     dits[4] = 99.9;  
     gMC->Gsvolu("I650", "TUBS", idtmed[254], dits, 5);  // was I150 in old geom.

     dits[0] = 0;
     dits[1] = 0.5;
     dits[2] = 1.5;
     gMC->Gsvolu("I676", "TUBE", idtmed[274], dits, 3); // was I176 in old geom.

     dits[0] = 0;
     dits[1] = 0.18;
     dits[2] = 0.8;
     gMC->Gsvolu("I673", "TUBE", idtmed[274], dits, 3); // was I173 in old geom.

     dits[0] = 0;
     dits[1] = 0.18;
     dits[2] = 3;
     gMC->Gsvolu("I671", "TUBE", idtmed[274], dits, 3); // was I171 in old geom.

     dits[0] = 0;
     dits[1] = 0.075;
     dits[2] = 0.8;
     gMC->Gsvolu("I669", "TUBE", idtmed[264], dits, 3); // was I169 in old geom.

     dits[0] = 3.5;
     dits[1] = 5.6;
     dits[2] = 0.55;
     dits[3] = 0;
     dits[4] = 38;
     gMC->Gsvolu("I667", "TUBS", idtmed[263], dits, 5); // was I167 in old geom.

     dits[0] = 6.6;
     dits[1] = 7.6;
     dits[2] = 0.5;
     dits[3] = 0;
     dits[4] = 9;
     gMC->Gsvolu("I666", "TUBS", idtmed[263], dits, 5); // was I166 in old geom.

     dits[0] = 0.26;
     dits[1] = 0.32;
     dits[2] = 0.55;
     gMC->Gsvolu("I678", "TUBE", idtmed[263], dits, 3); // was I178 in old geom.

     if (fluid == 1) {
        dits[0] = 0;
        dits[1] = 0.3;
        dits[2] = 1.5;
        gMC->Gsvolu("I677", "TUBE", idtmed[211], dits, 3); // set water as cooling fluid
	                                                   // was I177 in old geom.
     } else {
         dits[0] = 0;
        dits[1] = 0.3;
        dits[2] = 1.5;
        gMC->Gsvolu("I677", "TUBE", idtmed[212], dits, 3); // set freon as cooling fluid
	                                                   // was I177 in old geom.    
     }
     
     dits[0] = 0.07;
     dits[1] = 0.125;
     dits[2] = 0.3;
     gMC->Gsvolu("I675", "TUBE", idtmed[263], dits, 3); // was I175 in old geom.

     if (fluid == 1) {
        dits[0] = 0;
        dits[1] = 0.1;
        dits[2] = 0.8;
        gMC->Gsvolu("I674", "TUBE", idtmed[211], dits, 3); // set water as cooling fluid
	                                                   // was I174 in old geom.
     } else {
        dits[0] = 0;
        dits[1] = 0.1;
        dits[2] = 0.8;
        gMC->Gsvolu("I674", "TUBE", idtmed[212], dits, 3); // set freon as cooling fluid
	                                                   // was I174 in old geom.     
     }
     
     if (fluid == 1) {
        dits[0] = 0;
        dits[1] = 0.1;
        dits[2] = 3;
        gMC->Gsvolu("I672", "TUBE", idtmed[211], dits, 3); // set water as cooling fluid
	                                                   // was I172 in old geom.
     } else {
        dits[0] = 0;
        dits[1] = 0.1;
        dits[2] = 3;
        gMC->Gsvolu("I672", "TUBE", idtmed[212], dits, 3); // set freon as cooling fluid
	                                                   // was I172 in old geom.        
     }
     
     if (fluid == 1) {     
        dits[0] = 0;
        dits[1] = 0.0746;
        dits[2] = 0.8;
        gMC->Gsvolu("I670", "TUBE", idtmed[211], dits, 3); // set water as cooling fluid
	                                                   // was I170 in old geom.
     } else {
        dits[0] = 0;
        dits[1] = 0.0746;
        dits[2] = 0.8;
        gMC->Gsvolu("I670", "TUBE", idtmed[212], dits, 3); // set freon as cooling fluid
	                                                   // was I170 in old geom.     
     }
     
     if (fluid == 1) {     
        dits[0] = 3.7;
        dits[1] = 5.4;
        dits[2] = 0.35;
        dits[3] = 2;
        dits[4] = 36;
        gMC->Gsvolu("I668", "TUBS", idtmed[211], dits, 5); // set water as cooling fluid
	                                                   // was I168 in old geom.
     } else {
        dits[0] = 3.7;
        dits[1] = 5.4;
        dits[2] = 0.35;
        dits[3] = 2;
        dits[4] = 36;
        gMC->Gsvolu("I668", "TUBS", idtmed[212], dits, 5); // set freon as cooling fluid
	                                                   // was I168 in old geom.
     }


  }

  // --- Define SPD (option 'b') volumes ----------------------------
  
  // SPD - option 'b' 
  // (this is the default)

  if (option == 2) {
  
     dits[0] = 3.7;
     dits[1] = 7.75;
     dits[2] = 26.1;
     gMC->Gsvolu("IT12", "TUBE", idtmed[254], dits, 3);   

     dits[0] = 3.7;
     dits[1] = 7.7;
     dits[2] = 24;
     dits[3] = 57;
     dits[4] = 100;
     gMC->Gsvolu("I12B", "TUBS", idtmed[254], dits, 5);   // sector

     di10b[0] = 0.843;
     di10b[1] = ddet1+dchip1+dbus+0.0025;  
     di10b[2] = 19.344;
     gMC->Gsvolu("I10B", "BOX ", idtmed[254], di10b, 3);   // mother volume 
	                                                        // on layer 1

     di20b[0] = 0.843;
     di20b[1] = ddet2+dchip2+dbus+0.0025;   
     di20b[2] = 19.344;
     gMC->Gsvolu("I20B", "BOX ", idtmed[254], di20b, 3);   // mother volume
	                                                        // layer 2

     dits[0] = 1.3673;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I123", "BOX ", idtmed[253], dits, 3);

     dits[0] = 0.06;
     dits[1] = 0.08;
     dits[2] = 24;
     dits[3] = -36.79;
     dits[4] = 21.834;
     gMC->Gsvolu("I121", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 0.1253;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I122", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.04;
     dits[1] = 0.06 ;
     dits[2] = 24;
     dits[3] = 126.79;
     dits[4] = 270;
     gMC->Gsvolu("I120", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 0.1134;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I144", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.25;
     dits[1] = 0.06;
     dits[2] = 24;
     gMC->Gsvolu("I113", "BOX ", idtmed[254], dits, 3);  

     dits[0] = 0.077;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I143", "BOX ", idtmed[253], dits, 3);   

     dits[0] = 0.04;
     dits[1] = 0.06;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 90;
     gMC->Gsvolu("I142", "TUBS", idtmed[253], dits, 5); 

     dits[0] = 0.0695;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I141", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.06;
     dits[1] = 0.08;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 108;
     gMC->Gsvolu("I140", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 0.1835;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I139", "BOX ", idtmed[253], dits, 3);

     dits[0] = 0.1894 ;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I138", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.04;
     dits[1] = 0.06;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 75.261;
     gMC->Gsvolu("I137", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 1.3401;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I136", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.05;
     dits[1] = 0.07;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 72.739;
     gMC->Gsvolu("I135", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 0.1193;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I134", "BOX ", idtmed[253], dits, 3);    

     dits[0] = 0.163;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I133", "BOX ", idtmed[253], dits, 3);   

     dits[0] = 0.04;
     dits[1] = 0.06;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 157.633;
     gMC->Gsvolu("I132", "TUBS", idtmed[253], dits, 5); 

     dits[0] = 0.2497;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I131", "BOX ", idtmed[253], dits, 3); 

     dits[0] = 0.06;
     dits[1] = 0.08;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 148.633;
     gMC->Gsvolu("I130", "TUBS", idtmed[253], dits, 5); 

     dits[0] = 0.292;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I129", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.163;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I128", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.04;
     dits[1] = 0.06;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 161.297;
     gMC->Gsvolu("I126", "TUBS", idtmed[253], dits, 5);

     dits[0] = 0.2433;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I125", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.06;
     dits[1] = 0.08;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 42.883;
     gMC->Gsvolu("I124", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 0.793;
     dits[1] = 0.475;  //0.685; 0.015
     dits[2] = 2.5;
     gMC->Gsvolu("I105", "BOX ", idtmed[290], dits, 3);  

     di107[0] = 0.793;
     di107[1] = ddet1+dchip1;
     di107[2] = 3.536;
     gMC->Gsvolu("I107", "BOX ", idtmed[254], di107, 3); // contains det and chip   
                                                         // layer 1
     dits[0] = 0.705;
     dits[1] = 0.01;
     dits[2] = 2.5;
     gMC->Gsvolu("I109", "BOX ", idtmed[275], dits, 3);  

     di108[0] = 0.705;
     di108[1] = dbus;
     di108[2] = 14.344;
     gMC->Gsvolu("I108", "BOX ", idtmed[275], di108, 3); // bus for both layers 

     di1d7[0] = 0.7975;
     di1d7[1] = ddet2+dchip2;   
     di1d7[2] = 3.536;
     gMC->Gsvolu("I1D7", "BOX ", idtmed[254], di1d7, 3); // contains det and chip
                                                         // layer 2
     dits[0] = 0.06;
     dits[1] = 0.08;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 80;
     gMC->Gsvolu("I112", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 0.04;
     dits[1] = 0.06;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 80;
     gMC->Gsvolu("I111", "TUBS", idtmed[253], dits, 5);  

     dits[0] = 0.15;
     dits[1] = 0.0146;
     dits[2] = 24;
     gMC->Gsvolu("I118", "BOX ", idtmed[273], dits, 3);  

     dits[0] = 0.1315;
     dits[1] = 0.01;
     dits[2] = 24;
     gMC->Gsvolu("I110", "BOX ", idtmed[253], dits, 3);  

     dits[0] = 0.025;
     dits[1] = 0.035;
     dits[2] = 24;
     dits[3] = 0;
     dits[4] = 180;
     gMC->Gsvolu("I114", "TUBS", idtmed[264], dits, 5);  

     if (fluid == 1) {
        dits[0] = 0;
        dits[1] = 0.025;
        dits[2] = 24;
        dits[3] = 0;
        dits[4] = 180;
        gMC->Gsvolu("I115", "TUBS", idtmed[211], dits, 5);  // set water as cooling fluid   
     } else {
        dits[0] = 0;
        dits[1] = 0.025;
        dits[2] = 24;
        dits[3] = 0;
        dits[4] = 180;
        gMC->Gsvolu("I115", "TUBS", idtmed[212], dits, 5);  // set freon as cooling fluid
     }
     
     dits[0] = 0.063;
     dits[1] = 0.035;
     dits[2] = 24;
     gMC->Gsvolu("I116", "BOX ", idtmed[264], dits, 3); 

     di106[0] = 0.7975;
     di106[1] = dchip1;   
     di106[2] = 0.68;
     gMC->Gsvolu("I106", "BOX ", idtmed[201], di106, 3);   // chip layer 1

     di1d6[0] = 0.7975;
     di1d6[1] = dchip2;   
     di1d6[2] = 0.68;
     gMC->Gsvolu("I1D6", "BOX ", idtmed[201], di1d6, 3);   // chip layer 2

     di101[0] = 0.705;
     di101[1] = ddet1;
     di101[2] = 3.536;
     gMC->Gsvolu("I101", "BOX ", idtmed[250], di101, 3);  // contains detector  
                                                          // layer 1
     di1d1[0] = 0.705;
     di1d1[1] = ddet2;   
     di1d1[2] = 3.536;
     gMC->Gsvolu("I1D1", "BOX ", idtmed[250], di1d1, 3);  // contains detector  
                                                          // layer 2
   
     if (fluid == 1) {
        dits[0] = 0.063;
        dits[1] = 0.025;
        dits[2] = 24;
        gMC->Gsvolu("I117", "BOX ", idtmed[211], dits, 3); // set water as cooling fluid
     } else {
        dits[0] = 0.063;
        dits[1] = 0.025;
        dits[2] = 24;
        gMC->Gsvolu("I117", "BOX ", idtmed[212], dits, 3); // set freon as cooling fluid
     }

     dits1[0] = 0.64;
     dits1[1] = ddet1;
     dits1[2] = 3.48;
     gMC->Gsvolu("ITS1", "BOX ", idtmed[200], dits1, 3);   // detector layer 1

     dits2[0] = 0.64;
     dits2[1] = ddet2;  
     dits2[2] = 3.48;
     gMC->Gsvolu("ITS2", "BOX ", idtmed[200], dits2, 3);   // detector layer 2

     dits[0] = 3.701;
     dits[1] = 7.699;
     dits[2] = 4;
     dits[3] = 57.1;
     dits[4] = 99.9;  
     gMC->Gsvolu("I650", "TUBS", idtmed[254], dits, 5);  // was I150 in old geom.

     dits[0] = 0;
     dits[1] = 0.5;
     dits[2] = 1.5;
     gMC->Gsvolu("I676", "TUBE", idtmed[274], dits, 3); // was I176 in old geom.

     dits[0] = 0;
     dits[1] = 0.18;
     dits[2] = 0.8;
     gMC->Gsvolu("I673", "TUBE", idtmed[274], dits, 3); // was I173 in old geom.

     dits[0] = 0;
     dits[1] = 0.18;
     dits[2] = 3;
     gMC->Gsvolu("I671", "TUBE", idtmed[274], dits, 3); // was I171 in old geom.

     dits[0] = 0;
     dits[1] = 0.075;
     dits[2] = 0.8;
     gMC->Gsvolu("I669", "TUBE", idtmed[264], dits, 3); // was I169 in old geom.

     dits[0] = 3.5;
     dits[1] = 5.6;
     dits[2] = 0.55;
     dits[3] = 0;
     dits[4] = 38;
     gMC->Gsvolu("I667", "TUBS", idtmed[263], dits, 5); // was I167 in old geom.

     dits[0] = 6.6;
     dits[1] = 7.6;
     dits[2] = 0.5;
     dits[3] = 0;
     dits[4] = 9;
     gMC->Gsvolu("I666", "TUBS", idtmed[263], dits, 5); // was I166 in old geom.

     dits[0] = 0.26;
     dits[1] = 0.32;
     dits[2] = 0.55;
     gMC->Gsvolu("I678", "TUBE", idtmed[263], dits, 3); // was I178 in old geom.

     if (fluid == 1) {
        dits[0] = 0;
        dits[1] = 0.3;
        dits[2] = 1.5;
        gMC->Gsvolu("I677", "TUBE", idtmed[211], dits, 3); //set water as cooling fluid
	                                                   // was I177 in old geom.
     } else {
        dits[0] = 0;
        dits[1] = 0.3;
        dits[2] = 1.5;
        gMC->Gsvolu("I677", "TUBE", idtmed[212], dits, 3); //set freon as cooling fluid
	                                                   // was I177 in old geom.     
     }

     dits[0] = 0.07;
     dits[1] = 0.125;
     dits[2] = 0.3;
     gMC->Gsvolu("I675", "TUBE", idtmed[263], dits, 3); // was I175 in old geom.

     if (fluid == 1) {
        dits[0] = 0;
        dits[1] = 0.1;
        dits[2] = 0.8;
        gMC->Gsvolu("I674", "TUBE", idtmed[211], dits, 3); //set water as cooling fluid
	                                                   // was I174 in old geom.
     } else {
        dits[0] = 0;
        dits[1] = 0.1;
        dits[2] = 0.8;
        gMC->Gsvolu("I674", "TUBE", idtmed[212], dits, 3); //set freon as cooling fluid
	                                                   // was I174 in old geom.     
     }
     
     if (fluid == 1) {
        dits[0] = 0;
        dits[1] = 0.1;
        dits[2] = 3;
        gMC->Gsvolu("I672", "TUBE", idtmed[211], dits, 3); //set water as cooling fluid
	                                                   // was I172 in old geom.
     } else {
        dits[0] = 0;
        dits[1] = 0.1;
        dits[2] = 3;
        gMC->Gsvolu("I672", "TUBE", idtmed[212], dits, 3); //set freon as cooling fluid
	                                                   // was I172 in old geom.     
     }
     
     if (fluid == 1) {
        dits[0] = 0;
        dits[1] = 0.0746;
        dits[2] = 0.8;
        gMC->Gsvolu("I670", "TUBE", idtmed[211], dits, 3); //set water as cooling fluid
	                                                   // was I170 in old geom.
     } else {
        dits[0] = 0;
        dits[1] = 0.0746;
        dits[2] = 0.8;
        gMC->Gsvolu("I670", "TUBE", idtmed[212], dits, 3); //set freon as cooling fluid
	                                                   // was I170 in old geom.     
     }
     
     if (fluid == 1) {
        dits[0] = 3.7;
        dits[1] = 5.4;
        dits[2] = 0.35;
        dits[3] = 2;
        dits[4] = 36;
        gMC->Gsvolu("I668", "TUBS", idtmed[211], dits, 5); //set water as cooling fluid
	                                                   // was I168 in old geom.
     } else {
        dits[0] = 3.7;
        dits[1] = 5.4;
        dits[2] = 0.35;
        dits[3] = 2;
        dits[4] = 36;
        gMC->Gsvolu("I668", "TUBS", idtmed[212], dits, 5); //set freon as cooling fluid
	                                                   // was I168 in old geom.     
     }
     

  }

  // --- Define SDD volumes ------------------------------------------

  
  cos30 = cos(30.*3.14159/180.);
  sin30 = sin(30.*3.14159/180.);

  
  dits[0] = 0;
  dits[1] = 360;
  dits[2] = 6;
  dits[3] = -34.6; 
  dits[4] = 23.49;
  dits[5] = 28;
  dits[6] = -23.65; 
  dits[7] = 23.49;
  dits[8] = 28;
  dits[9] = -23.65;  
  dits[10] = 14.59; 
  dits[11] = 28;
  dits[12] = 23.65;   
  dits[13] = 14.59;
  dits[14] = 28;
  dits[15] = 23.65;    
  dits[16] = 23.49;
  dits[17] = 28;
  dits[18] = 34.6;  
  dits[19] = 23.49;
  dits[20] = 28;
  gMC->Gsvolu("IT34", "PCON", idtmed[209], dits, 21);  

  // block of the SDD electronics and related ladder frame 
  I018dits[0] = 3.2;
  I018dits[1] = 2;
  I018dits[2] = 3.65;
  gMC->Gsvolu("I018", "BOX ", idtmed[209], I018dits, 3);  

  // block of the SDD end ladder 
  I024dits[0] = 3.2;
  I024dits[1] = 2;
  I024dits[2] = 2.725;
  gMC->Gsvolu("I024", "BOX ", idtmed[209], I024dits, 3);  

  // ladder frame of layer 3 - F.T. March,7-2001
  I047dits[0] = I018dits[0];
  I047dits[1] = I018dits[1];
  I047dits[2] = 6*I018dits[2] + 2*I024dits[2]; 
  gMC->Gsvolu("I047", "BOX ", idtmed[209], I047dits, 3);  

  // ladder frame of layer 4 - F.T. March,7-2001
  I048dits[0] = I018dits[0];
  I048dits[1] = I018dits[1];
  I048dits[2] = 8*I018dits[2] + 2*I024dits[2]; 
  gMC->Gsvolu("I048", "BOX ", idtmed[209], I048dits, 3);  


  // global SDD volume (sensitive + insensitive) 
  I302dits[0] = 3.6250;
  I302dits[1] = 0.0150;
  I302dits[2] = 4.3794;
  gMC->Gsvolu("I302", "BOX ", idtmed[278], I302dits, 3);

  // Like for I302 - F.T. March,7-2001
  I402dits[0] = 3.6250;
  I402dits[1] = 0.0150;
  I402dits[2] = 4.3794;
  gMC->Gsvolu("I402", "BOX ", idtmed[278], I402dits, 3);  

  // SDD ladder of layer 3 - F.T. March,7-2001
  I004dits[0] = I302dits[0]+0.005;
  I004dits[1] = 2*I302dits[1]+Y_SDD_sep/2.;
  I004dits[2] = TMath::Abs(Z_SDD_lay3[0]);
  if (I004dits[2] < TMath::Abs(Z_SDD_lay3[5])) {
    I004dits[2] = TMath::Abs(Z_SDD_lay3[5]);
  }
  I004dits[2] = I004dits[2] + I302dits[2];
  gMC->Gsvolu("I004", "BOX ", idtmed[209], I004dits, 3);  

  // SDD ladder of layer 4 - F.T. March,7-2001
  I005dits[0] = I402dits[0]+0.005;
  I005dits[1] = 2*I402dits[1]+Y_SDD_sep/2.;
  I005dits[2] = TMath::Abs(Z_SDD_lay4[0]);
  if (I005dits[2] < TMath::Abs(Z_SDD_lay4[7])) {
    I005dits[2] = TMath::Abs(Z_SDD_lay4[7]);
  }
  I005dits[2] = I005dits[2] + I402dits[2];
  gMC->Gsvolu("I005", "BOX ", idtmed[209], I005dits, 3);  


  // -- block of the SDD ladder foot and end ladder

  // ladder foot mother volume
  I028dits[0] = 3.0000;
  I028dits[1] = 0.4000;
  I028dits[2] = 0.9000;
  gMC->Gsvolu("I028", "BOX ", idtmed[224], I028dits, 3);  

  // positioning-box #1 at SDD end-ladder - F.T. March,7-2001
  I420dits[0] = 0.4500;
  I420dits[1] = 0.4000;
  I420dits[2] = 0.4500;
  gMC->Gsvolu("I420", "BOX ", idtmed[264], I420dits, 3);  

  // positioning-box #2 at SDD end-ladder - F.T. March,7-2001
  I421dits[0] = 0.;
  I421dits[1] = 0.25;
  I421dits[2] = I420dits[1];
  gMC->Gsvolu("I421", "TUBE", idtmed[209], I421dits, 3);  

  // reference ruby-sphere at SDD end-ladder - F.T. March,7-2001 
  I422dits[0] = 0.0000;
  I422dits[1] = 0.2000;
  I422dits[2] = 0.0000;
  I422dits[3] = 180.00;
  I422dits[4] = 0.0000;
  I422dits[5] = 360.00;
  gMC->Gsvolu("I422", "SPHE", idtmed[277], I422dits, 6);  

  // support for ruby-sphere (I422) - F.T. March,7-2001
  I423dits[0] = 0.0000;
  I423dits[1] = 0.1000;
  I423dits[2] = (I420dits[1]-I422dits[1])/2.;
  gMC->Gsvolu("I423", "TUBE", idtmed[264], I423dits, 3);  

  // passage for HV microcables - F.T. March,7-2001
  I424dits[0] = 1.5000;
  I424dits[1] = 0.1500;
  I424dits[2] = I421dits[2];
  gMC->Gsvolu("I424", "BOX ", idtmed[209], I424dits, 3);  

  // HV microcables segment at the end ladder - F.T. March,7-2001
  I425dits[0] = 1.350000;
  I425dits[1] = 0.015250;
  I425dits[2] = I024dits[2];
  gMC->Gsvolu("I425", "BOX ", idtmed[279], I425dits, 3);  

  // lower edge of SDD ladder frame at end-ladder - part 1
  dits[0] = 0.2;
  dits[1] = 0.1815;
  dits[2] = I024dits[2];
  dits[3] = 0.015;
  gMC->Gsvolu("I025", "TRD1", idtmed[208], dits, 4);  

  // lower edge of SDD ladder frame at end-ladder - part 2
  dits[0] = 0.183;
  dits[1] = 0.165;
  dits[2] = I024dits[2];
  dits[3] = 0.015;
  gMC->Gsvolu("I026", "TRD1", idtmed[208], dits, 4);  

  // new: for the 1st top rod of the structure 
  // at the end-ladder - F.T. March,7-2001
  I029dits[0] = 0.2;
  I029dits[1] = 0.1815;
  I029dits[2] = 1.0100;
  I029dits[3] = 0.015;
  gMC->Gsvolu("I029", "TRD1", idtmed[208], I029dits, 4);  

  // new: for the 2nd top rod of the structure 
  // at the end-ladder - F.T. March,7-2001
  I030dits[0] = 0.1830;
  I030dits[1] = 0.1650;
  I030dits[2] = 1.0100;
  I030dits[3] = 0.0150;
  gMC->Gsvolu("I030", "TRD1", idtmed[208], I030dits, 4);  

  // inox cooling tubes for the end ladder - F.T. March,7-2001
  I031dits[0] = 0.093;
  I031dits[1] = 0.1;
  I031dits[2] = I024dits[2];
  gMC->Gsvolu("I031", "TUBE", idtmed[264], I031dits, 3);  

  if (fluid == 1) {
     // cooling water for the end ladder - F.T. March,7-2001
     I032dits[0] = 0;
     I032dits[1] = I031dits[0];
     I032dits[2] = I024dits[2];
     gMC->Gsvolu("I032", "TUBE", idtmed[211], I032dits, 3);  
  } else {
     // cooling freon for the end ladder - R.B. March,21-2001
     I032dits[0] = 0;
     I032dits[1] = I031dits[0];
     I032dits[2] = I024dits[2];
     gMC->Gsvolu("I032", "TUBE", idtmed[212], I032dits, 3);    
  }
  
  // -- block of the SDD ladder frame holding the electronics

  // edge of the ladder frame - part 1
  dits[0] = 0.2;
  dits[1] = 0.182;
  dits[2] = 3.65;
  dits[3] = 0.015;
  gMC->Gsvolu("I019", "TRD1", idtmed[208], dits, 4);  

  // edge of the ladder frame - part 2
  dits[0] = 0.183;
  dits[1] = 0.165;
  dits[2] = 3.65;
  dits[3] = 0.015;
  gMC->Gsvolu("I020", "TRD1", idtmed[208], dits, 4);  

  // inclined segments of the ladder frame
  dits[0] = 2.23;
  dits[1] = 2.1;
  dits[2] = 0.05;
  dits[3] = 0.03;
  gMC->Gsvolu("I021", "TRD1", idtmed[208], dits, 4);  

  // horiz.segments of the ladders, normal to ladder edges
  dits[0] = 2.1;
  dits[1] = 2;
  dits[2] = 0.06;
  dits[3] = 0.04;
  gMC->Gsvolu("I022", "TRD1", idtmed[208], dits, 4);  

  // horiz.segments of the ladders, at 45 deg. to ladder edges
  dits[0] = 2.615;
  dits[1] = 2.465;
  dits[2] = 0.06;
  dits[3] = 0.04;
  gMC->Gsvolu("I023", "TRD1", idtmed[208], dits, 4);  

  // supports of the ceramic pins holding the detectors
  dits[0] = 0.3;
  dits[1] = 0.05;
  dits[2] = 0.15;
  gMC->Gsvolu("I033", "BOX ", idtmed[208], dits, 3);  

  // ceramic pins holding the detectors
  dits[0] = 0;
  dits[1] = 0.05;
  dits[2] = 0.225;
  gMC->Gsvolu("I034", "TUBE", idtmed[277], dits, 3);  

  // holders of cooling tubes
  I035dits[0] = 0.1;
  I035dits[1] = 0.15;
  I035dits[2] = 0.2;
  gMC->Gsvolu("I035", "TUBE", idtmed[208], I035dits, 3);

  // top holders of microcables
  dits[0] = 0.2;
  dits[1] = 0.01;
  dits[2] = 0.05;
  gMC->Gsvolu("I036", "BOX ", idtmed[208], dits, 3);  

  // inox cooling tubes - F.T. March,7-2001
  I037dits[0] = 0.093;
  I037dits[1] = 0.1;
  I037dits[2] = I018dits[2];
  gMC->Gsvolu("I037", "TUBE", idtmed[264], I037dits, 3);

  if (fluid == 1) {
     // cooling water - F.T. March,7-2001
     I038dits[0] = 0;
     I038dits[1] = I037dits[0];
     I038dits[2] = I018dits[2];
     gMC->Gsvolu("I038", "TUBE", idtmed[211], I038dits, 3);  
  } else {
     // cooling freon - R.B. March,21-2001
     I038dits[0] = 0;
     I038dits[1] = I037dits[0];
     I038dits[2] = I018dits[2];
     gMC->Gsvolu("I038", "TUBE", idtmed[212], I038dits, 3);    
  }
  // -- block of the SDD electronics (heat bridge, chips, hybrid, anode microcable)

  // SDD heat bridge - F.T. March,7-2001
  I039dits[0] = 1.1000;
  I039dits[1] = 0.0087;
  I039dits[2] = 3.2500;
  gMC->Gsvolu("I039", "BOX ", idtmed[268], I039dits, 3);  

  // SDD clip part 1
  dits[0] = 0.25;
  dits[1] = 0.01;
  dits[2] = I039dits[2];
  gMC->Gsvolu("I040", "BOX ", idtmed[268], dits, 3);  

  // SDD clip part 2
  I041dits[0] = 0.1;
  I041dits[1] = 0.12;
  I041dits[2] = I039dits[2];
  I041dits[3] = 90;
  I041dits[4] = 320;
  gMC->Gsvolu("I041", "TUBS", idtmed[268], I041dits, 5);  


  // SDD PASCAL - F.T. March,7-2001
  I042dits[0] = 0.5000;
  I042dits[1] = 0.0175;
  I042dits[2] = 0.5000;
  gMC->Gsvolu("I042", "BOX ", idtmed[206], I042dits, 3);  

  // SDD AMBRA - F.T. March,7-2001
  I043dits[0] = 0.3500;
  I043dits[1] = 0.0175;
  I043dits[2] = 0.5000;
  gMC->Gsvolu("I043", "BOX ", idtmed[206], I043dits, 3);  

  // SDD capacitors - F.T. March,7-2001
  I051dits[0] = 0.1400;
  I051dits[1] = 0.0350;
  I051dits[2] = 0.0625;
  gMC->Gsvolu("I051", "BOX ", idtmed[276], I051dits, 3);  

  // SDD hybrid circuit - F.T. March,7-2001
  I052dits[0] = 1.725000;
  I052dits[1] = 0.003743;
  I052dits[2] = I039dits[2];
  gMC->Gsvolu("I052", "BOX ", idtmed[281], I052dits, 3);

  // SDD anode microcable : changed - F.T. March,7-2001
  I044dits[0] = I018dits[2];
  I044dits[1] = I039dits[2];
  I044dits[2] = 0.00084;
  I044dits[3] = (15.189149/(I044dits[0]+I044dits[1]))/2;
  gMC->Gsvolu("I044", "TRD1", idtmed[282], I044dits, 4);  
  volI044 = ((2*I044dits[0] + 2*I044dits[1]) * 2*I044dits[2])/2 * 2*I044dits[3];

  // SDD electronics box - F.T. March,7-2001
  I050dits[1] = I039dits[1]+I052dits[1]+I051dits[1]+I044dits[2];
  I050dits[0] = I018dits[1]/cos(30.*3.14159/180.)-I050dits[1]*sin(30.*3.14159/180.);
  I050dits[2] = I018dits[2];
  gMC->Gsvolu("I050", "BOX ", idtmed[209], I050dits, 3);

  // SDD sensitive volume
  dits[0] = 3.50850;
  dits[1] = 0.01499; // not 0.015 because it is included into I302 which is 0.015
  dits[2] = 3.76320;
  gMC->Gsvolu("ITS3", "BOX ", idtmed[200], dits, 3);  

  // Like for ITS3 - F.T. March,7-2001
  dits[0] = 3.50850;
  dits[1] = 0.01499; // not 0.015 because it is included into I402 which is 0.015
  dits[2] = 3.76320;
  gMC->Gsvolu("ITS4", "BOX ", idtmed[200], dits, 3);  


  // --- Define SSD volumes ------------------------------------------

    
  dits[0] = 0;
  dits[1] = 360;
  dits[2] = 6;
  dits[3] = -57.45;
  dits[4] = 43.6;
  dits[5] = 48;  
  dits[6] = -49.15; 
  dits[7] = 43.6;
  dits[8] = 48;  
  dits[9] = -49.15;  
  dits[10] = 36.9;
  dits[11] = 48;  
  dits[12] = 50.55;  
  dits[13] = 36.9;
  dits[14] = 48;  
  dits[15] = 50.55;  
  dits[16] = 43.6;
  dits[17] = 48;  
  dits[18] = 57.45;
  dits[19] = 43.6;
  dits[20] = 48;   
  gMC->Gsvolu("IT56", "PCON", idtmed[220], dits, 21);   
  
  dits[0] =  3.4;
  dits[1] = 1.955;
  dits[2] = 56.5; 
  gMC->Gsvolu("I570", "BOX ", idtmed[204], dits, 3);  
  
  dits[0] = 3.75;
  dits[1] = 0.045;
  dits[2] = 50.975;
  gMC->Gsvolu("I569", "BOX ", idtmed[204], dits, 3);  
  
  dits[0] = 3.4;
  dits[1] = 1.955;
  dits[2] = 47; 
  gMC->Gsvolu("I571", "BOX ", idtmed[204], dits, 3);  
  
  dits[0] = 3.75;
  dits[1] = 0.045;
  dits[2] = 43.3;  
  gMC->Gsvolu("I565", "BOX ", idtmed[204], dits, 3);  
  
  dits[0] = 3.4;
  dits[1] = 1.955;
  dits[2] = 3.15;
  gMC->Gsvolu("I553", "BOX ", idtmed[204], dits, 3);  
  
  dits[0] = 3.405;
  dits[1] = 1.955;
  dits[2] = 1.955;
  gMC->Gsvolu("I523", "BOX ", idtmed[204], dits, 3);  
  
  dits[0] = 3.75;
  dits[1] = 0.015;
  dits[2] = 2.1;
  gMC->Gsvolu("I566", "BOX ", idtmed[206], dits, 3); 
  
  dits[0] = 3.4;
  dits[1] = 1.955;
  dits[2] = 3.15;
  gMC->Gsvolu("I544", "BOX ", idtmed[204], dits, 3);  
  
  dits[0] = 3.41;
  dits[1] = 1.955;
  dits[2] = 1.955;
  gMC->Gsvolu("I516", "BOX ", idtmed[204], dits, 3);  
  
  dits[0] = 3.75;
  dits[1] = 0.015;
  dits[2] = 2.1;
  gMC->Gsvolu("I562", "BOX ", idtmed[206], dits, 3);   
  
  if (fluid == 1) {
     dits[0] = 0;
     dits[1] = 0.07;
     dits[2] = 3.15;
     gMC->Gsvolu("I559", "TUBE", idtmed[211], dits, 3);  // set water as cooling fluid
  } else {
     dits[0] = 0;
     dits[1] = 0.07;
     dits[2] = 3.15;
     gMC->Gsvolu("I559", "TUBE", idtmed[212], dits, 3);  // set freon as cooling fluid
  }
  
  dits[0] = 0.07;
  dits[1] = 0.1;
  dits[2] = 3.15;
  gMC->Gsvolu("I560", "TUBE", idtmed[210], dits, 3);  
  
  dits[0] = 0.225;
  dits[1] = 0.195;
  dits[2] = 3.15;
  dits[3] = 0.025;
  gMC->Gsvolu("I558", "TRD1", idtmed[203], dits, 4);  
  
  dits[0] = 0.25;
  dits[1] = 0.22;
  dits[2] = 3.15;
  dits[3] = 0.025;
  gMC->Gsvolu("I557", "TRD1", idtmed[203], dits, 4);  
  
  dits[0] = 2.17;
  dits[1] = 0.035;
  dits[2] = 0.05;
  gMC->Gsvolu("I556", "BOX ", idtmed[203], dits, 3);  
  
  dits[0] = 2 ;
  dits[1] = 0.035;
  dits[2] = 0.05;
  gMC->Gsvolu("I554", "BOX ", idtmed[203], dits, 3);  
  
  dits[0] = 2.675;
  dits[1] = 0.035;
  dits[2] = 0.05;
  gMC->Gsvolu("I555", "BOX ", idtmed[203], dits, 3); 
  
  dits[0] = 0.3;
  dits[1] = 0.15;
  dits[2] = 0.15;
  gMC->Gsvolu("I561", "BOX ", idtmed[203], dits, 3);  
  
  dits[0] = 0.025;
  dits[1] = 0.025;
  dits[2] = 0.05;
  gMC->Gsvolu("I519", "BOX ", idtmed[214], dits, 3);  
  
  dits[0] = 0.304;
  dits[1] = 0.0275;
  dits[2] = 0.432;
  gMC->Gsvolu("I521", "BOX ", idtmed[206], dits, 3);   
  
  dits[0] = 0.16;
  dits[1] = 0.08;
  dits[2] = 0.08;
  gMC->Gsvolu("I520", "BOX ", idtmed[214], dits, 3);  
  
  dits[0] = 3.4;
  dits[1] = 0.015;
  dits[2] = 0.525;
  gMC->Gsvolu("I518", "BOX ", idtmed[203], dits, 3);  
  
  dits[0] = 0.15;
  dits[1] = 0.105;
  dits[2] = 0.29;
  dits[3] = 0.08;
  gMC->Gsvolu("I522", "TRD1", idtmed[203], dits, 4);  
  
  dits[0] = 0.07;
  dits[1] = 0.1;
  dits[2] = 1.955;
  gMC->Gsvolu("I542", "TUBE", idtmed[210], dits, 3);  
  
  if (fluid == 1) {
     dits[0] = 0;
     dits[1] = 0.07;
     dits[2] = 1.955;
     gMC->Gsvolu("I541", "TUBE", idtmed[211], dits, 3);  // set water as cooling fluid 
  } else {
     dits[0] = 0;
     dits[1] = 0.07;
     dits[2] = 1.955;
     gMC->Gsvolu("I541", "TUBE", idtmed[212], dits, 3);  // set freon as cooling fluid
  }
  
  dits[0] = 0.3;
  dits[1] = 0.15;
  dits[2] = 0.15;
  gMC->Gsvolu("I543", "BOX ", idtmed[203], dits, 3);  
  
  dits[0] = 0.25;
  dits[1] = 0.22;
  dits[2] = 1.955;
  dits[3] = 0.025;
  gMC->Gsvolu("I537", "TRD1", idtmed[203], dits, 4); 
  
  dits[0] = 0.225;
  dits[1] = 0.195;
  dits[2] = 1.955;
  dits[4] = 0.025;
  gMC->Gsvolu("I538", "TRD1", idtmed[203], dits, 4);  
  
  dits[0] = 2.17;
  dits[1] = 0.035;
  dits[2] = 0.05;
  gMC->Gsvolu("I536", "BOX ", idtmed[203], dits, 3);  
  
  dits[0] = 2.675;
  dits[1] = 0.035;
  dits[2] = 0.05;
  gMC->Gsvolu("I535", "BOX ", idtmed[203], dits, 3);   
  
  dits[0] = 2;
  dits[1] = 0.035;
  dits[2] = 0.05;
  gMC->Gsvolu("I534", "BOX ", idtmed[203], dits, 3);  
  
  dits[0] = 0;
  dits[1] = 0.05;
  dits[2] = 0.17;
  gMC->Gsvolu("I540", "TUBE", idtmed[203], dits, 3);  
  
  dits[0] = 0;
  dits[1] = 0.05;
  dits[2] = 0.205;
  gMC->Gsvolu("I539", "TUBE", idtmed[203], dits, 3);  
  
  dits[0] = 3.65;
  dits[1] = 0.015;
  dits[2] = 2;
  gMC->Gsvolu("ITS6", "BOX ", idtmed[200], dits, 3);  
  
  if (fluid == 1) {
     dits[0] = 0;
     dits[1] = 0.07;
     dits[2] = 3.15;
     gMC->Gsvolu("I550", "TUBE", idtmed[211], dits, 3);  // set water as cooling fluid
  } else {
     dits[0] = 0;
     dits[1] = 0.07;
     dits[2] = 3.15;
     gMC->Gsvolu("I550", "TUBE", idtmed[212], dits, 3);  // set freon as cooling fluid
  }
  
  dits[0] = 0.07;
  dits[1] = 0.1;
  dits[2] = 3.15;
  gMC->Gsvolu("I551", "TUBE", idtmed[210], dits, 3);  
  
  dits[0] = 0.225;
  dits[1] = 0.195;
  dits[2] = 3.15;
  dits[3] = 0.025;
  gMC->Gsvolu("I549", "TRD1", idtmed[203], dits, 4); 
  
  dits[0] = 0.25;
  dits[1] = 0.22;
  dits[2] = 3.15;
  dits[3] = 0.025;
  gMC->Gsvolu("I548", "TRD1", idtmed[203], dits, 4);  
  
  dits[0] = 2.17;
  dits[1] = 0.035;
  dits[2] = 0.05;
  gMC->Gsvolu("I547", "BOX ", idtmed[203], dits, 3);  
  
  dits[0] = 2;
  dits[1] = 0.035;
  dits[2] = 0.05;
  gMC->Gsvolu("I545", "BOX ", idtmed[203], dits, 3);   
  
  dits[0] = 2.675;
  dits[1] = 0.035;
  dits[2] = 0.05;
  gMC->Gsvolu("I546", "BOX ", idtmed[203], dits, 3);  
  
  dits[0] = 0.3;
  dits[1] = 0.15;
  dits[2] = 0.15;
  gMC->Gsvolu("I552", "BOX ", idtmed[203], dits, 3);  
  
  dits[0] = 0.304;
  dits[1] = 0.0275;
  dits[2] = 0.4322;
  gMC->Gsvolu("I515", "BOX ", idtmed[206], dits, 3);  
  
  dits[0] = 0.025;
  dits[1] = 0.025;
  dits[2] = 0.05;
  gMC->Gsvolu("I513", "BOX ", idtmed[214], dits, 3);  
  
  dits[0] = 0.16;
  dits[1] = 0.08;
  dits[2] = 0.08;
  gMC->Gsvolu("I514", "BOX ", idtmed[214], dits, 3);  
  
  dits[0] = 3.4;
  dits[1] = 0.015;
  dits[2] = 0.525;
  gMC->Gsvolu("I512", "BOX ", idtmed[203], dits, 3);  
  
  dits[0] = 0.225;
  dits[1] = 0.195;
  dits[2] = 1.955;
  dits[3] = 0.025;
  gMC->Gsvolu("I528", "TRD1", idtmed[203], dits, 4); 
  
  dits[0] = 0.25;
  dits[1] = 0.22;
  dits[2] = 1.955;
  dits[3] = 0.025;
  gMC->Gsvolu("I527", "TRD1", idtmed[203], dits, 4);  
  
  dits[0] = 2.17;
  dits[1] = 0.035;
  dits[2] = 0.05;
  gMC->Gsvolu("I526", "BOX ", idtmed[203], dits, 3);  
  
  dits[0] = 2.675;
  dits[1] = 0.035;
  dits[2] = 0.05;
  gMC->Gsvolu("I525", "BOX ", idtmed[203], dits, 3);  
   
  dits[0] = 2;
  dits[1] = 0.035;
  dits[2] = 0.05;
  gMC->Gsvolu("I524", "BOX ", idtmed[203], dits, 3);  
   
  dits[0] = 0;
  dits[1] = 0.05;
  dits[2] = 0.205;
  gMC->Gsvolu("I529", "TUBE", idtmed[203], dits, 3);  
   
  dits[0] = 0;
  dits[1] = 0.05;
  dits[2] = 0.17;
  gMC->Gsvolu("I530", "TUBE", idtmed[203], dits, 3);  
   
  dits[0] = 0.15;
  dits[1] = 0.105;
  dits[2] = 0.29;
  dits[3] = 0.08;
  gMC->Gsvolu("I517", "TRD1", idtmed[203], dits, 4);  
  
  if (fluid == 1) {
     dits[0] = 0;
     dits[1] = 0.07;
     dits[2] = 1.955;
     gMC->Gsvolu("I531", "TUBE", idtmed[211], dits, 3);  // set water as cooling fluid
  } else {
     dits[0] = 0;
     dits[1] = 0.07;
     dits[2] = 1.955;
     gMC->Gsvolu("I531", "TUBE", idtmed[212], dits, 3);  // set freon as cooling fluid
  }
     
  dits[0] = 0.07;
  dits[1] = 0.1;
  dits[2] = 1.955;
  gMC->Gsvolu("I532", "TUBE", idtmed[210], dits, 3);  
 
  dits[0] = 0.3;
  dits[1] = 0.15;
  dits[2] = 0.15;
  gMC->Gsvolu("I533", "BOX ", idtmed[203], dits, 3);  
  
  dits[0] = 3.65;
  dits[1] = 0.015;
  dits[2] = 2;
  gMC->Gsvolu("ITS5", "BOX ", idtmed[200], dits, 3);  



  // --- Define volumes of shield of SPD ----------------


  dits[0] = 8.37;
  dits[1] = 9.93;
  dits[2] = 25;
  gMC->Gsvolu("IC01", "TUBE", idtmed[289], dits, 3);   

  dits[0] = 8.3;
  dits[1] = 9.995;
  dits[2] = 17.5/2.;
  gMC->Gsvolu("IC02", "TUBE", idtmed[289], dits, 3);    
  
 
  // --- Define volume of first cylinder between SPD and SDD --------------
  
  dits[0] = (21.-0.128)/2.;      
  dits[1] = 21./2.;
  dits[2] = 39.4;      
  gMC->Gsvolu("ICY1", "TUBE", idtmed[208], dits, 3);
         
  // --- Define volume of second cylinder between SDD and SSD --------------

  dits[0] = (59.5-0.128)/2.;      
  dits[1] = 59.5/2.;
  dits[2] = 56.2;      // was 57
  gMC->Gsvolu("ICY2", "TUBE", idtmed[208], dits, 3);

  // --- Define volumes of SDD cone ---------------------------------- 

  dits[0] = 0;
  dits[1] = 360;
  dits[2] = 12;
  dits[3] = -59.7;
  dits[4] = 27;
  dits[5] = 28.6;
  dits[6] = -42.7;
  dits[7] = 10;
  dits[8] = 28.6;
  dits[9] = -34.65;
  dits[10] = 10;
  dits[11] = 28.6;
  dits[12] = -34.65;
  dits[13] = 10;
  dits[14] = 23.495;
  dits[15] = -23.7;
  dits[16] = 10;
  dits[17] = 23.495;
  dits[18] = -23.7;
  dits[19] = 10;
  dits[20] = 14.595;
  dits[21] = 23.7;
  dits[22] = 10;
  dits[23] = 14.595;
  dits[24] = 23.7;
  dits[25] = 10;
  dits[26] = 23.495;
  dits[27] = 34.65;
  dits[28] = 10;
  dits[29] = 23.495;
  dits[30] = 34.65;
  dits[31] = 10;
  dits[32] = 28.6;
  dits[33] = 42.7;
  dits[34] = 10;
  dits[35] = 28.6;
  dits[36] = 59.7;
  dits[37] = 27.2637;
  dits[38] = 28.6;             
  gMC->Gsvolu("IS02", "PCON", idtmed[204], dits, 39);
  
  dits[0] = 0;
  dits[1] = 360;
  dits[2] = 6;
  dits[3] = 38.65;
  dits[4] = 10.75;    
  dits[5] = 12.25;      
  dits[6] = 40.15;
  dits[7] = 10.75;
  dits[8] = 13.96;   
  dits[9] = 40.15;
  dits[10] = 12.46;  
  dits[11] = 13.96;
  dits[12] = 55.75;
  dits[13] = 27;
  dits[14] = 28.5;
  dits[15] = 55.75;
  dits[16] = 27;
  dits[17] = 28.5;
  dits[18] = 57.25;
  dits[19] = 27;
  dits[20] = 28.5;       
  gMC->Gsvolu("I093", "PCON", idtmed[272], dits, 21);  // SDD cone

  dits[0] = 0;
  dits[1] = 50;
  dits[2] = 3;
  dits[3] = 39;  
  dits[4] = 14;
  dits[5] = 18.75;
  dits[6] = 46.7-3;
  dits[7] = 14;
  dits[8] = 18.75;
  dits[9] = 51.45-3;
  dits[10] = 18.75;
  dits[11] = 18.75;
  gMC->Gsvolu("I099", "PCON", idtmed[204], dits, 12); // SDD 3 cone hole

  dits[0] = 0;
  dits[1] = 25;
  dits[2] = 3;
  dits[3] = 49;
  dits[4] = 23.4;
  dits[5] = 26.4;
  dits[6] = 56.1-3;
  dits[7] = 23.4;
  dits[8] = 26.4;
  dits[9] = 59.1-3;
  dits[10] = 26.4;
  dits[11] = 26.4;
  gMC->Gsvolu("I200", "PCON", idtmed[204], dits, 12); // SDD 4 cone hole

  dits[0] = 10.0;
  dits[1] = 10.5;
  dits[2] = 0.25;
  gMC->Gsvolu("I090", "TUBE", idtmed[224], dits, 3);  // SDD cylinder flange

  dits[0] = 21.95;
  dits[1] = 22.95;    
  dits[2] = 1;
  gMC->Gsvolu("I098", "TUBE", idtmed[283], dits, 3);    // ladder support on layer 4

  dits[0] = 13.1;    
  dits[1] = 14.1;    
  dits[2] = 1;
  gMC->Gsvolu("I097", "TUBE", idtmed[283], dits, 3);    // ladder support on layer 3

  dits[0] = 1;
  dits[1] = 1;
  dits[2] = 7.74;
  gMC->Gsvolu("I202", "BOX ", idtmed[272], dits, 3);

  dits[0] = 1;
  dits[1] = 1;
  dits[2] = 9.14;
  gMC->Gsvolu("I203", "BOX ", idtmed[272], dits, 3);

  dits[0] = 21.95;
  dits[1] = 22.95;
  dits[2] = 1;
  gMC->Gsvolu("I095", "TUBE", idtmed[224], dits, 3);

  dits[0] = 3;
  dits[1] = 2.7;
  dits[2] = 1;
  dits[3] = 0.63;
  gMC->Gsvolu("I096", "TRD1", idtmed[264], dits, 4);

  dits[0] = 13.1;
  dits[1] = 14.1;
  dits[2] = 1;
  gMC->Gsvolu("I094", "TUBE", idtmed[224], dits, 3);
  
  
  // --- Define volumes of SSD cone ----------------------------------    
            

  dits[0] = 0;
  dits[1] = 360;
  dits[2] = 12;
  dits[3] = -zmax;
  dits[4] = 46;         
  dits[5] = 49.25;       
  dits[6] = -61.2;
  dits[7] = 28.7;
  dits[8] = 49.25;       
  dits[9] = -57.5;
  dits[10] = 28.7;
  dits[11] = 49.25;      
  dits[12] = -57.5;
  dits[13] = 28.7;
  dits[14] = 43.5;
  dits[15] = -49.2;
  dits[16] = 28.7;
  dits[17] = 43.5;
  dits[18] = -49.2;
  dits[19] = 28.7;
  dits[20] = 36.85;
  dits[21] = 50.6;
  dits[22] = 28.7;
  dits[23] = 36.85;
  dits[24] = 50.6;
  dits[25] = 28.7;
  dits[26] = 43.5;
  dits[27] = 57.5;
  dits[28] = 28.7;
  dits[29] = 43.5;
  dits[30] = 57.5;
  dits[31] = 28.7;
  dits[32] = 49.25;      
  dits[33] = 61.2;
  dits[34] = 28.7;
  dits[35] = 49.25;      
  dits[36] = zmax;
  dits[37] = 46;      
  dits[38] = 49.25;      
  gMC->Gsvolu("IS01", "PCON", idtmed[204], dits, 39);   // SSD cone mother volume
  
  dits[0] = 0;
  dits[1] = 360;
  dits[2] = 6;
  dits[3] = -zmax;  
  dits[4] = 47.75;  
  dits[5] = 49.25;  
  dits[6] = -zmax+2.;  
  dits[7] = 47.75;  
  dits[8] = 49.25;   
  dits[9] = -71.2819;
  dits[10] = 46.75;   
  dits[11] = 49.0319;
  dits[12] = -57.25;   // was 58.5 
  dits[13] = 32.9681;
  dits[14] = 34.75;
  dits[15] = -57.25;   // was 58.5   
  dits[16] = 30;
  dits[17] = 34.75;
  dits[18] = -55.75;   // was 57 
  dits[19] = 30;     
  dits[20] = 32.25;    // was 31.5 
  gMC->Gsvolu("I212", "PCON", idtmed[272], dits, 21);  // SSD cone
  
  dits[0] = 28.75;          
  dits[1] = 29.75;   
  dits[2] = 0.5;
  gMC->Gsvolu("I211", "TUBE", idtmed[224], dits, 3);   // SSD cylinder flange
  
  dits[0] = 35.8;   
  dits[1] = 36.8;   
  dits[2] = 1;
  gMC->Gsvolu("I217", "TUBE", idtmed[283], dits, 3);   // ladder support on layer 5 
  
  dits[0] = 41.4;  
  dits[1] = 42.4;  
  dits[2] = 1;
  gMC->Gsvolu("I219", "TUBE", idtmed[283], dits, 3);   // ladder support on layer 6
        
  dits[0] = 42.05+5.;       
  dits[1] = 42.55+5.;     
  dits[2] = 1.25;
  gMC->Gsvolu("I214", "TUBE", idtmed[224], dits, 3);   // layer 6 electronic support
                                                       // this will change after PPR
  dits[0] = 37.05+5.;   
  dits[1] = 37.55+5.;   
  dits[2] = 1.25;
  gMC->Gsvolu("I213", "TUBE", idtmed[224], dits, 3);   // layer 5 electronic support
                                                       // this will change after PPR
  dits[0] = 0;
  dits[1] = 25;
  dits[2] = 5;
  dits[3] = -zmax+3;
  dits[4] = 45.5;
  dits[5] = 45.5;
  dits[6] = -69.7+3;
  dits[7] = 37;
  dits[8] = 45.5;
  dits[9] = -68.5+3;
  dits[10] = 37;
  dits[11] = 45.5;
  dits[12] = -68.5+4.8;  
  dits[13] = 37;
  dits[14] = 45.5; 
  dits[15] = -63.5+4.8;
  dits[16] = 37;
  dits[17] = 45.5;   
  gMC->Gsvolu("I215", "PCON", idtmed[204], dits, 18);  // SSD cone hole 
 
  dits[0] = 0;
  dits[1] = 3.2;
  dits[2] = 9;
  dits[3] = -14;
  dits[4] = 30.5;
  dits[5] = 33.5;
  dits[6] = -9.85;
  dits[7] = 30.5;
  dits[8] = 33.5;
  dits[9] = -9.85;
  dits[10] = 30.5;
  dits[11] = 43.45;
  dits[12] = -7.85;
  dits[13] = 30.5;
  dits[14] = 43.45;
  dits[15] = -7.85;
  dits[16] = 30.5;
  dits[17] = 36.5;
  dits[18] = -7;
  dits[19] = 30.5;
  dits[20] = 36.5;
  dits[21] = -4;
  dits[22] = 33.0173;
  dits[23] = 36.5;
  dits[24] = -4;
  dits[25] = 33.0173;
  dits[26] = 36.80;
  dits[27] = -2;
  dits[28] = 34.6955;
  dits[29] = 36.80;
  gMC->Gsvolu("I216", "PCON", idtmed[272], dits, 30); // supports (1-6) of the ladders
       
       
  // --- Place SPD (option 'a') volumes into their mother volume IT12
  
  // SPD - option 'a' 
  // (this is NOT the default)

  if (option == 1) {

     gMC->Gspos("I12A",5,"IT12",0.0,0.0,0.0,idrotm[238],"MANY");
     gMC->Gspos("I12A",6,"IT12",0.0,0.0,0.0,idrotm[236],"MANY");
     gMC->Gspos("I12A",7,"IT12",0.0,0.0,0.0,idrotm[239],"MANY");
     gMC->Gspos("I12A",8,"IT12",0.0,0.0,0.0,idrotm[233],"MANY");
     gMC->Gspos("I12A",9,"IT12",0.0,0.0,0.0,idrotm[240],"MANY");
     gMC->Gspos("I12A",10,"IT12",0.0,0.0,0.0,idrotm[241],"MANY");
     gMC->Gspos("I12A",2,"IT12",0.0,0.0,0.0,idrotm[242],"MANY");
     gMC->Gspos("I12A",3,"IT12",0.0,0.0,0.0,idrotm[234],"MANY");
     gMC->Gspos("I12A",4,"IT12",0.0,0.0,0.0,idrotm[243],"MANY");
     gMC->Gspos("I12A",1,"IT12",0.0,0.0,0.0,0,"MANY");
     deltax=((ddet1-0.01/2.)+(dchip1-0.015/2.))*TMath::Cos(270.*TMath::Pi()/180.);  // see definition of idrotm[244]
	  deltay=((ddet1-0.01/2.)+(dchip1-0.015/2.))*TMath::Sin(270.*TMath::Pi()/180.);  // see definition of idrotm[244]
     gMC->Gspos("I10A",2,"I12A",0.203+deltax,3.8206+deltay,0.0,idrotm[244],"ONLY");	  
     deltax=((ddet1-0.01/2.)+(dchip1-0.015/2.))*TMath::Cos(252.*TMath::Pi()/180.);  // see definition of idrotm[245]
	  deltay=((ddet1-0.01/2.)+(dchip1-0.015/2.))*TMath::Sin(252.*TMath::Pi()/180.);  // see definition of idrotm[245]  
     gMC->Gspos("I10A",1,"I12A",1.4531+deltax,3.8152+deltay,0.0,idrotm[245],"ONLY");
     deltax=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Cos(40.*TMath::Pi()/180.);  // see definition of idrotm[246]
	  deltay=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Sin(40.*TMath::Pi()/180.);  // see definition of idrotm[246]  
     gMC->Gspos("I20A",1,"I12A",3.0174+deltax,6.5143+deltay,0.0,idrotm[246],"ONLY");
     deltax=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Cos(49.*TMath::Pi()/180.);  // see definition of idrotm[247]
	  deltay=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Sin(49.*TMath::Pi()/180.);  // see definition of idrotm[247] 
     gMC->Gspos("I20A",2,"I12A",1.9612+deltax,6.9062+deltay,0.0,idrotm[247],"ONLY");
     deltax=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Cos(58.*TMath::Pi()/180.);  // see definition of idrotm[248]
	  deltay=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Sin(58.*TMath::Pi()/180.);  // see definition of idrotm[248] 
     gMC->Gspos("I20A",3,"I12A",0.8567+deltax,7.1279+deltay,0.0,idrotm[248],"ONLY");
     deltax=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Cos(67.*TMath::Pi()/180.);  // see definition of idrotm[249]
	  deltay=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Sin(67.*TMath::Pi()/180.);  // see definition of idrotm[249] 
     gMC->Gspos("I20A",4,"I12A",-0.2689+deltax,7.1742+deltay,0.0,idrotm[249],"ONLY");
     gMC->Gspos("I123",2,"I12A",-0.2978,5.5196,0.0,idrotm[214],"ONLY");
     gMC->Gspos("I121",2,"I12A",-0.2385,4.1518,0.0,idrotm[213],"ONLY");
     gMC->Gspos("I122",2,"I12A",-0.2968,4.0207,0.0,idrotm[212],"ONLY");
     gMC->Gspos("I120",2,"I12A",-0.3672,3.9056,0.0,0,"ONLY");
     gMC->Gspos("I144",1,"I12A",-0.2538,3.8556,0.0,0,"ONLY");
     gMC->Gspos("I113",3,"I12A",0.1095,3.9056,0.0,0,"ONLY");
     gMC->Gspos("I143",1,"I12A",0.4365,3.8556,0.0,idrotm[236],"ONLY");
     gMC->Gspos("I142",1,"I12A",0.5136,3.9056,0.0,idrotm[235],"ONLY");
     gMC->Gspos("I141",1,"I12A",0.5636,3.9752,0.0,idrotm[201],"ONLY");
     gMC->Gspos("I140",1,"I12A",0.6336,4.0447,0.0,idrotm[234],"ONLY");
     gMC->Gspos("I139",1,"I12A",0.8297,4.0545,0.0,idrotm[207],"ONLY");
     gMC->Gspos("I113",5,"I12A",1.2575,3.9681,0.0,idrotm[207],"ONLY");
     gMC->Gspos("I138",1,"I12A",1.66,3.7848,0.0,idrotm[207],"ONLY");
     gMC->Gspos("I137",1,"I12A",1.8556,3.7738,0.0,idrotm[233],"ONLY");
     gMC->Gspos("I136",1,"I12A",2.6224,4.874,0.0,idrotm[232],"ONLY");
     gMC->Gspos("I135",1,"I12A",3.2967,6.0337,0.0,idrotm[231],"ONLY");
     gMC->Gspos("I134",1,"I12A",3.266,6.1636,0.0,idrotm[230],"ONLY");
     gMC->Gspos("I113",1,"I12A",2.9903,6.4144,0.0,idrotm[211],"ONLY");
     gMC->Gspos("I133",3,"I12A",2.7631,6.7627,0.0,idrotm[230],"ONLY");
     gMC->Gspos("I132",3,"I12A",2.62,6.8555,0.0,idrotm[229],"ONLY");
     gMC->Gspos("I131",3,"I12A",2.648,6.6023,0.0,idrotm[228],"ONLY");
     gMC->Gspos("I130",3,"I12A",2.6569,6.3431,0.0,idrotm[227],"ONLY");
     gMC->Gspos("I129",3,"I12A",2.3906,6.4819,0.0,idrotm[226],"ONLY");
     gMC->Gspos("I113",2,"I12A",1.9488,6.7998,0.0,idrotm[210],"ONLY");
     gMC->Gspos("I133",2,"I12A",1.6699,7.1085,0.0,idrotm[226],"ONLY");
     gMC->Gspos("I132",2,"I12A",1.5142,7.1777,0.0,idrotm[225],"ONLY");
     gMC->Gspos("I131",2,"I12A",1.5814,6.932,0.0,idrotm[224],"ONLY");
     gMC->Gspos("I130",2,"I12A",1.6308,6.6774,0.0,idrotm[223],"ONLY");
     gMC->Gspos("I129",2,"I12A",1.346,6.7728,0.0,idrotm[222],"ONLY");
     gMC->Gspos("I113",6,"I12A",0.8599,7.0176,0.0,idrotm[209],"ONLY");
     gMC->Gspos("I133",1,"I12A",0.5362,7.2789,0.0,idrotm[222],"ONLY");
     gMC->Gspos("I132",1,"I12A",0.3715,7.3228,0.0,idrotm[221],"ONLY");
     gMC->Gspos("I131",1,"I12A",0.4763,7.0907,0.0,idrotm[220],"ONLY");
     gMC->Gspos("I130",1,"I12A",0.5649,6.8469,0.0,idrotm[219],"ONLY");
     gMC->Gspos("I129",1,"I12A",0.2688,6.8966,0.0,idrotm[218],"ONLY");
     gMC->Gspos("I113",4,"I12A",-0.2497,7.0624,0.0,idrotm[208],"ONLY");
     gMC->Gspos("I128",1,"I12A",-0.6103,7.2698,0.0,idrotm[218],"ONLY");
     gMC->Gspos("I126",2,"I12A",-0.7799,7.2874,0.0,idrotm[217],"ONLY");
     gMC->Gspos("I125",2,"I12A",-0.6315,7.0883,0.0,idrotm[216],"ONLY");
     gMC->Gspos("I124",2,"I12A",-0.4965,6.8742,0.0,idrotm[215],"ONLY");
     gMC->Gspos("I103",3,"I10A",-0.05,-di10a[1]+2.*di104[1]+di103[1],-3.536,0,"ONLY");
     gMC->Gspos("I103",4,"I10A",-0.05,-di10a[1]+2.*di104[1]+di103[1],-10.708,0,"ONLY");
     gMC->Gspos("I103",1,"I10A",-0.05,-di10a[1]+2.*di104[1]+di103[1],10.708,0,"ONLY");
     gMC->Gspos("I103",2,"I10A",-0.05,-di10a[1]+2.*di104[1]+di103[1],3.536,0,"ONLY");
     gMC->Gspos("I105",1,"I10A",-0.05,0.01,-16.844,idrotm[237],"ONLY");
     gMC->Gspos("I105",2,"I10A",-0.05,0.01,16.844,0,"ONLY");
     gMC->Gspos("I104",1,"I10A",0.0,-di10a[1]+di104[1],0.0,0,"ONLY");
     gMC->Gspos("I1D3",3,"I20A",-0.05,-di20a[1]+2.*di104[1]+di1d3[1],-3.536,0,"ONLY");
     gMC->Gspos("I1D3",4,"I20A",-0.05,-di20a[1]+2.*di104[1]+di1d3[1],-10.708,0,"ONLY");
     gMC->Gspos("I1D3",1,"I20A",-0.05,-di20a[1]+2.*di104[1]+di1d3[1],10.708,0,"ONLY");
     gMC->Gspos("I1D3",2,"I20A",-0.05,-di20a[1]+2.*di104[1]+di1d3[1],3.536,0,"ONLY");
     gMC->Gspos("I105",3,"I20A",-0.05,0.01,-16.844,idrotm[237],"ONLY");
     gMC->Gspos("I105",4,"I20A",-0.05,0.01,16.844,0,"ONLY");
     gMC->Gspos("I104",2,"I20A",0.0,-di20a[1]+di104[1],0.0,0,"ONLY");
     gMC->Gspos("I112",2,"I113",0.25,0.02,0.0,idrotm[206],"ONLY");
     gMC->Gspos("I111",2,"I113",0.1318,-0.0008,0.0,idrotm[205],"ONLY");
     gMC->Gspos("I118",1,"I113",0.0,-0.0454,0.0,0,"ONLY");
     gMC->Gspos("I110",1,"I113",0.0,0.0492,0.0,0,"ONLY");
     gMC->Gspos("I114",1,"I113",0.063,0.0042,0.0,idrotm[202],"ONLY");
     gMC->Gspos("I115",1,"I113",0.063,0.0042,0.0,idrotm[202],"ONLY");
     gMC->Gspos("I115",2,"I113",-0.063,0.0042,0.0,idrotm[201],"ONLY");
     gMC->Gspos("I114",2,"I113",-0.063,0.0042,0.0,idrotm[201],"ONLY");
     gMC->Gspos("I116",1,"I113",0.0,0.0042,0.0,0,"ONLY");
     gMC->Gspos("I111",1,"I113",-0.1318,-0.0008,0.0,idrotm[204],"ONLY");
     gMC->Gspos("I112",1,"I113",-0.25,0.02,0.0,idrotm[203],"ONLY");
     gMC->Gspos("I101",1,"I103",-0.088,ddet1,0.0,0,"ONLY");
     gMC->Gspos("I102",1,"I103",0.0,-dchip1,-2.8,0,"ONLY");
     gMC->Gspos("I102",2,"I103",0.0,-dchip1,-1.4,0,"ONLY");
     gMC->Gspos("I102",3,"I103",0.0,-dchip1,0.0,0,"ONLY");
     gMC->Gspos("I102",4,"I103",0.0,-dchip1,1.4,0,"ONLY");
     gMC->Gspos("I102",5,"I103",0.0,-dchip1,2.8,0,"ONLY");
     gMC->Gspos("I1D1",1,"I1D3",-0.088,ddet2,0.0,0,"ONLY");
     gMC->Gspos("I1D2",1,"I1D3",0.0,-dchip2,-2.8,0,"ONLY");
     gMC->Gspos("I1D2",2,"I1D3",0.0,-dchip2,-1.4,0,"ONLY");
     gMC->Gspos("I1D2",3,"I1D3",0.0,-dchip2,0.0,0,"ONLY");
     gMC->Gspos("I1D2",4,"I1D3",0.0,-dchip2,1.4,0,"ONLY");
     gMC->Gspos("I1D2",5,"I1D3",0.0,-dchip2,2.8,0,"ONLY");
     gMC->Gspos("I117",1,"I116",0.0,0.0,0.0,0,"ONLY");
     gMC->Gspos("ITS1",1,"I101",0.0,0.0,0.0,0,"ONLY");
     gMC->Gspos("ITS2",1,"I1D1",0.0,0.0,0.0,0,"ONLY");
     gMC->Gspos("I650",16,"IT12",0.0,0.0,22.0,idrotm[1104],"MANY");
     gMC->Gspos("I650",20,"IT12",0.0,0.0,22.0,idrotm[1130],"MANY");
     gMC->Gspos("I650",18,"IT12",0.0,0.0,22.0,idrotm[1117],"MANY");
     gMC->Gspos("I650",1,"IT12",0.0,0.0,22.0,0,"MANY");
     gMC->Gspos("I650",4,"IT12",0.0,0.0,22.0,idrotm[1106],"MANY");
     gMC->Gspos("I650",6,"IT12",0.0,0.0,22.0,idrotm[1039],"MANY");
     gMC->Gspos("I650",8,"IT12",0.0,0.0,22.0,idrotm[1107],"MANY");
     gMC->Gspos("I650",10,"IT12",0.0,0.0,22.0,idrotm[1065],"MANY");
     gMC->Gspos("I650",12,"IT12",0.0,0.0,22.0,idrotm[1078],"MANY");
     gMC->Gspos("I650",14,"IT12",0.0,0.0,22.0,idrotm[1091],"MANY");
     gMC->Gspos("I650",19,"IT12",0.0,0.0,-22.0,idrotm[1108],"MANY");
     gMC->Gspos("I650",2,"IT12",0.0,0.0,-22.0,idrotm[1109],"MANY");
     gMC->Gspos("I650",3,"IT12",0.0,0.0,-22.0,idrotm[1110],"MANY");
     gMC->Gspos("I650",5,"IT12",0.0,0.0,-22.0,idrotm[1111],"MANY");
     gMC->Gspos("I650",7,"IT12",0.0,0.0,-22.0,idrotm[1112],"MANY");
     gMC->Gspos("I650",9,"IT12",0.0,0.0,-22.0,idrotm[1113],"MANY");
     gMC->Gspos("I650",11,"IT12",0.0,0.0,-22.0,idrotm[1114],"MANY");
     gMC->Gspos("I650",13,"IT12",0.0,0.0,-22.0,idrotm[1115],"MANY");
     gMC->Gspos("I650",15,"IT12",0.0,0.0,-22.0,idrotm[1116],"MANY");
     gMC->Gspos("I650",17,"IT12",0.0,0.0,-22.0,idrotm[1118],"MANY");
     gMC->Gspos("I666",1,"I650",0.0,0.0,0.25,idrotm[1003],"MANY");
     gMC->Gspos("I667",1,"I650",0.1102,0.9945,0.45,idrotm[1088],"ONLY");
     gMC->Gspos("I669",3,"I650",0.1883,4.0372,-3.2,0,"ONLY");
     gMC->Gspos("I671",3,"I650",0.1883,4.0372,0.6,0,"ONLY");
     gMC->Gspos("I669",2,"I650",1.3343,4.0609,-3.2,0,"ONLY");
     gMC->Gspos("I671",2,"I650",1.3343,4.0609,0.6,0,"ONLY");
     gMC->Gspos("I669",6,"I650",2.9567,6.1959,-3.2,idrotm[1089],"ONLY");
     gMC->Gspos("I671",6,"I650",2.9567,6.1959,0.6,idrotm[1089],"ONLY");
     gMC->Gspos("I669",5,"I650",1.9511,6.5822,-3.2,idrotm[1011],"ONLY");
     gMC->Gspos("I671",5,"I650",1.9511,6.5822,0.6,idrotm[1011],"ONLY");
     gMC->Gspos("I669",4,"I650",0.8974,6.8064,-3.2,idrotm[1090],"ONLY");
     gMC->Gspos("I671",4,"I650",0.8974,6.8064,0.6,idrotm[1090],"ONLY");
     gMC->Gspos("I669",1,"I650",-0.1784,6.863,-3.2,0,"ONLY");
     gMC->Gspos("I671",1,"I650",-0.1784,6.863,0.6,0,"ONLY");
     gMC->Gspos("I673",1,"I650",0.2173,4.8037,1.8,0,"ONLY");
     gMC->Gspos("I673",6,"I650",1.5093,4.5605,1.8,0,"ONLY");
     gMC->Gspos("I673",4,"I650",-0.173,6.2531,1.8,idrotm[1092],"ONLY");
     gMC->Gspos("I673",3,"I650",0.8073,6.2032,1.8,idrotm[1093],"ONLY");
     gMC->Gspos("I673",2,"I650",1.7678,6.0005,1.8,idrotm[1094],"ONLY");
     gMC->Gspos("I673",5,"I650",2.6847,5.6501,1.8,0,"ONLY");
     gMC->Gspos("I676",2,"I650",1.7618,5.2269,2.5,0,"ONLY");
     gMC->Gspos("I676",1,"I650",0.4018,5.5869,2.5,0,"ONLY");
     gMC->Gspos("I668",1,"I667",0.0,0.0,0.0,0,"ONLY");
     gMC->Gspos("I670",1,"I669",0.0,0.0,0.0,0,"ONLY");
     gMC->Gspos("I672",1,"I671",0.0,0.0,0.0,0,"ONLY");
     gMC->Gspos("I674",1,"I673",0.0,0.0,0.0,0,"MANY");
     gMC->Gspos("I675",1,"I673",0.0,0.0,-0.5,0,"ONLY");
     gMC->Gspos("I677",1,"I676",0.0,0.0,0.0,0,"MANY");
     gMC->Gspos("I678",1,"I676",0.0,0.0,-0.95,0,"ONLY");    

  }


  // --- Place SPD (option 'b') volumes into their mother volume IT12
  
  // SPD - option 'b' 
  // (this is the default)

  if (option == 2) {

     gMC->Gspos("I12B",1,"IT12",0.0,0.0,0.0,0,"MANY");
     gMC->Gspos("I12B",8,"IT12",0.0,0.0,0.0,idrotm[233],"MANY");
     gMC->Gspos("I12B",7,"IT12",0.0,0.0,0.0,idrotm[244],"MANY");
     gMC->Gspos("I12B",6,"IT12",0.0,0.0,0.0,idrotm[236],"MANY");
     gMC->Gspos("I12B",2,"IT12",0.0,0.0,0.0,idrotm[245],"MANY");
     gMC->Gspos("I12B",3,"IT12",0.0,0.0,0.0,idrotm[234],"MANY");
     gMC->Gspos("I12B",4,"IT12",0.0,0.0,0.0,idrotm[246],"MANY");
     gMC->Gspos("I12B",5,"IT12",0.0,0.0,0.0,idrotm[247],"MANY");
     gMC->Gspos("I12B",9,"IT12",0.0,0.0,0.0,idrotm[248],"MANY");
     gMC->Gspos("I12B",10,"IT12",0.0,0.0,0.0,idrotm[249],"MANY");
     deltax=((ddet1-0.01/2.)+(dchip1-0.015/2.))*TMath::Cos(270.*TMath::Pi()/180.);  // see definition of idrotm[238]
	  deltay=((ddet1-0.01/2.)+(dchip1-0.015/2.))*TMath::Sin(270.*TMath::Pi()/180.);  // see definition of idrotm[238]
     gMC->Gspos("I10B",2,"I12B",0.203+deltax,3.8206+deltay,0.0,idrotm[238],"ONLY");	  
     deltax=((ddet1-0.01/2.)+(dchip1-0.015/2.))*TMath::Cos(252.*TMath::Pi()/180.);  // see definition of idrotm[239]
	  deltay=((ddet1-0.01/2.)+(dchip1-0.015/2.))*TMath::Sin(252.*TMath::Pi()/180.);  // see definition of idrotm[239]  
     gMC->Gspos("I10B",1,"I12B",1.4531+deltax,3.8152+deltay,0.0,idrotm[239],"ONLY");
     deltax=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Cos(40.*TMath::Pi()/180.);  // see definition of idrotm[240]
	  deltay=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Sin(40.*TMath::Pi()/180.);  // see definition of idrotm[240]  
     gMC->Gspos("I20B",1,"I12B",3.0174+deltax,6.5143+deltay,0.0,idrotm[240],"ONLY");
     deltax=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Cos(49.*TMath::Pi()/180.);  // see definition of idrotm[241]
	  deltay=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Sin(49.*TMath::Pi()/180.);  // see definition of idrotm[241] 
     gMC->Gspos("I20B",2,"I12B",1.9612+deltax,6.9062+deltay,0.0,idrotm[241],"ONLY");
     deltax=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Cos(58.*TMath::Pi()/180.);  // see definition of idrotm[242]
	  deltay=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Sin(58.*TMath::Pi()/180.);  // see definition of idrotm[242] 
     gMC->Gspos("I20B",3,"I12B",0.8567+deltax,7.1279+deltay,0.0,idrotm[242],"ONLY");
     deltax=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Cos(67.*TMath::Pi()/180.);  // see definition of idrotm[243]
	  deltay=((ddet2-0.01/2.)+(dchip2-0.015/2.))*TMath::Sin(67.*TMath::Pi()/180.);  // see definition of idrotm[243] 
     gMC->Gspos("I20B",4,"I12B",-0.2689+deltax,7.1742+deltay,0.0,idrotm[243],"ONLY");
     gMC->Gspos("I123",1,"I12B",-0.2978,5.5196,0.0,idrotm[214],"ONLY");
     gMC->Gspos("I121",1,"I12B",-0.2385,4.1518,0.0,idrotm[213],"ONLY");
     gMC->Gspos("I122",1,"I12B",-0.2968,4.0207,0.0,idrotm[212],"ONLY");
     gMC->Gspos("I120",1,"I12B",-0.3672,3.9056,0.0,0,"ONLY");
     gMC->Gspos("I144",1,"I12B",-0.2538,3.8556,0.0,0,"ONLY");
     gMC->Gspos("I113",3,"I12B",0.1095,3.9056,0.0,0,"ONLY");
     gMC->Gspos("I143",1,"I12B",0.4365,3.8556,0.0,idrotm[236],"ONLY");
     gMC->Gspos("I142",1,"I12B",0.5136,3.9056,0.0,idrotm[235],"ONLY");
     gMC->Gspos("I141",1,"I12B",0.5636,3.9752,0.0,idrotm[237],"ONLY");
     gMC->Gspos("I140",1,"I12B",0.6336,4.0447,0.0,idrotm[234],"ONLY");
     gMC->Gspos("I139",1,"I12B",0.8297,4.0545,0.0,idrotm[207],"ONLY");
     gMC->Gspos("I113",5,"I12B",1.2575,3.9681,0.0,idrotm[207],"ONLY");
     gMC->Gspos("I138",1,"I12B",1.66,3.7848,0.0,idrotm[207],"ONLY");
     gMC->Gspos("I137",1,"I12B",1.8556,3.7738,0.0,idrotm[233],"ONLY");
     gMC->Gspos("I136",1,"I12B",2.6224,4.874,0.0,idrotm[232],"ONLY");
     gMC->Gspos("I135",1,"I12B",3.2967,6.0337,0.0,idrotm[231],"ONLY");
     gMC->Gspos("I134",1,"I12B",3.266,6.1636,0.0,idrotm[230],"ONLY");
     gMC->Gspos("I113",1,"I12B",2.9903,6.4144,0.0,idrotm[211],"ONLY");
     gMC->Gspos("I133",3,"I12B",2.7631,6.7627,0.0,idrotm[230],"ONLY");
     gMC->Gspos("I132",3,"I12B",2.62,6.8555,0.0,idrotm[229],"ONLY");
     gMC->Gspos("I131",3,"I12B",2.648,6.6023,0.0,idrotm[228],"ONLY");
     gMC->Gspos("I130",3,"I12B",2.6569,6.3431,0.0,idrotm[227],"ONLY");
     gMC->Gspos("I129",3,"I12B",2.3906,6.4819,0.0,idrotm[226],"ONLY");
     gMC->Gspos("I113",2,"I12B",1.9488,6.7998,0.0,idrotm[210],"ONLY");
     gMC->Gspos("I133",2,"I12B",1.6699,7.1085,0.0,idrotm[226],"ONLY");
     gMC->Gspos("I132",2,"I12B",1.5142,7.1777,0.0,idrotm[225],"ONLY");
     gMC->Gspos("I131",2,"I12B",1.5814,6.932,0.0,idrotm[224],"ONLY");
     gMC->Gspos("I130",2,"I12B",1.6308,6.6774,0.0,idrotm[223],"ONLY");
     gMC->Gspos("I129",2,"I12B",1.346,6.7728,0.0,idrotm[222],"ONLY");
     gMC->Gspos("I113",6,"I12B",0.8599,7.0176,0.0,idrotm[209],"ONLY");
     gMC->Gspos("I133",1,"I12B",0.5362,7.2789,0.0,idrotm[222],"ONLY");
     gMC->Gspos("I132",1,"I12B",0.3715,7.3228,0.0,idrotm[221],"ONLY");
     gMC->Gspos("I131",1,"I12B",0.4763,7.0907,0.0,idrotm[220],"ONLY");
     gMC->Gspos("I130",1,"I12B",0.5649,6.8469,0.0,idrotm[219],"ONLY");
     gMC->Gspos("I129",1,"I12B",0.2688,6.8966,0.0,idrotm[218],"ONLY");
     gMC->Gspos("I113",4,"I12B",-0.2497,7.0624,0.0,idrotm[208],"ONLY");
     gMC->Gspos("I128",1,"I12B",-0.6103,7.2698,0.0,idrotm[218],"ONLY");
     gMC->Gspos("I126",1,"I12B",-0.7799,7.2874,0.0,idrotm[217],"ONLY");
     gMC->Gspos("I125",1,"I12B",-0.6315,7.0883,0.0,idrotm[216],"ONLY");
     gMC->Gspos("I124",1,"I12B",-0.4965,6.8742,0.0,idrotm[215],"ONLY");
     gMC->Gspos("I105",3,"I10B",-0.05,-0.01,-16.844,idrotm[201],"ONLY");
     gMC->Gspos("I105",4,"I10B",-0.05,-0.01,16.844,0,"ONLY");
     gMC->Gspos("I107",2,"I10B",-0.0455,-di10b[1]+di107[1],3.536,0,"ONLY");
     gMC->Gspos("I107",1,"I10B",-0.0455,-di10b[1]+di107[1],10.708,0,"ONLY");
     gMC->Gspos("I107",4,"I10B",-0.0455,-di10b[1]+di107[1],-10.708,0,"ONLY");
     gMC->Gspos("I107",3,"I10B",-0.0455,-di10b[1]+di107[1],-3.536,0,"ONLY");
     gMC->Gspos("I109",1,"I10B",-0.138,0.015,-16.844,idrotm[201],"ONLY");
     gMC->Gspos("I109",2,"I10B",-0.138,0.015,16.844,0,"ONLY");
     gMC->Gspos("I108",1,"I10B",-0.138,-di10b[1]+2.*di107[1]+di108[1],0.0,0,"ONLY");
     gMC->Gspos("I105",1,"I20B",-0.05,-0.01,-16.844,idrotm[201],"ONLY");
     gMC->Gspos("I105",2,"I20B",-0.05,-0.01,16.844,0,"ONLY");
     gMC->Gspos("I1D7",2,"I20B",-0.0455,-di20b[1]+di1d7[1],3.536,0,"ONLY");
     gMC->Gspos("I1D7",1,"I20B",-0.0455,-di20b[1]+di1d7[1],10.708,0,"ONLY");
     gMC->Gspos("I1D7",4,"I20B",-0.0455,-di20b[1]+di1d7[1],-10.708,idrotm[201],"ONLY");
     gMC->Gspos("I1D7",3,"I20B",-0.0455,-di20b[1]+di1d7[1],-3.536,idrotm[201],"ONLY");
     gMC->Gspos("I109",3,"I20B",-0.138,0.015,-16.844,idrotm[201],"ONLY");
     gMC->Gspos("I109",4,"I20B",-0.138,0.015,16.844,0,"ONLY");
     gMC->Gspos("I108",2,"I20B",-0.138,-di20b[1]+2.*di1d7[1]+di108[1],0.0,0,"ONLY");
     gMC->Gspos("I112",2,"I113",0.25,0.02,0.0,idrotm[206],"ONLY");
     gMC->Gspos("I111",2,"I113",0.1318,-0.0008,0.0,idrotm[205],"ONLY");
     gMC->Gspos("I118",1,"I113",0.0,-0.0454,0.0,0,"ONLY");
     gMC->Gspos("I110",1,"I113",0.0,0.0492,0.0,0,"ONLY");
     gMC->Gspos("I114",1,"I113",0.063,0.0042,0.0,idrotm[202],"ONLY");
     gMC->Gspos("I115",1,"I113",0.063,0.0042,0.0,idrotm[202],"ONLY");
     gMC->Gspos("I115",2,"I113",-0.063,0.0042,0.0,idrotm[237],"ONLY");
     gMC->Gspos("I114",2,"I113",-0.063,0.0042,0.0,idrotm[237],"ONLY");
     gMC->Gspos("I116",1,"I113",0.0,0.0042,0.0,0,"ONLY");
     gMC->Gspos("I111",1,"I113",-0.1318,-0.0008,0.0,idrotm[204],"ONLY");
     gMC->Gspos("I112",1,"I113",-0.25,0.02,0.0,idrotm[203],"ONLY");
     gMC->Gspos("I106",1,"I107",0.0,-dchip1,-1.4,0,"ONLY");
     gMC->Gspos("I106",2,"I107",0.0,-dchip1,0.0,0,"ONLY");
     gMC->Gspos("I106",3,"I107",0.0,-dchip1,1.4,0,"ONLY");
     gMC->Gspos("I106",4,"I107",0.0,-dchip1,2.8,0,"ONLY");
     gMC->Gspos("I106",5,"I107",0.0,-dchip1,-2.8,0,"ONLY");
     gMC->Gspos("I101",1,"I107",0.0,ddet1,0.0,0,"ONLY");
     gMC->Gspos("I1D6",1,"I1D7",0.0,-dchip2,-1.4,0,"ONLY");
     gMC->Gspos("I1D6",2,"I1D7",0.0,-dchip2,0.0,0,"ONLY");
     gMC->Gspos("I1D6",3,"I1D7",0.0,-dchip2,1.4,0,"ONLY");
     gMC->Gspos("I1D6",4,"I1D7",0.0,-dchip2,2.8,0,"ONLY");
     gMC->Gspos("I1D6",5,"I1D7",0.0,-dchip2,-2.8,0,"ONLY");
     gMC->Gspos("I1D1",1,"I1D7",0.0,ddet2,0.0,0,"ONLY");
     gMC->Gspos("I117",1,"I116",0.0,0.0,0.0,0,"ONLY");
     gMC->Gspos("ITS1",1,"I101",0.0,0.0,0.0,0,"ONLY");
     gMC->Gspos("ITS2",1,"I1D1",0.0,0.0,0.0,0,"ONLY");
     gMC->Gspos("I650",16,"IT12",0.0,0.0,22.0,idrotm[1104],"MANY");
     gMC->Gspos("I650",20,"IT12",0.0,0.0,22.0,idrotm[1130],"MANY");
     gMC->Gspos("I650",18,"IT12",0.0,0.0,22.0,idrotm[1117],"MANY");
     gMC->Gspos("I650",1,"IT12",0.0,0.0,22.0,0,"MANY");
     gMC->Gspos("I650",4,"IT12",0.0,0.0,22.0,idrotm[1106],"MANY");
     gMC->Gspos("I650",6,"IT12",0.0,0.0,22.0,idrotm[1039],"MANY");
     gMC->Gspos("I650",8,"IT12",0.0,0.0,22.0,idrotm[1107],"MANY");
     gMC->Gspos("I650",10,"IT12",0.0,0.0,22.0,idrotm[1065],"MANY");
     gMC->Gspos("I650",12,"IT12",0.0,0.0,22.0,idrotm[1078],"MANY");
     gMC->Gspos("I650",14,"IT12",0.0,0.0,22.0,idrotm[1091],"MANY");
     gMC->Gspos("I650",19,"IT12",0.0,0.0,-22.0,idrotm[1108],"MANY");
     gMC->Gspos("I650",2,"IT12",0.0,0.0,-22.0,idrotm[1109],"MANY");
     gMC->Gspos("I650",3,"IT12",0.0,0.0,-22.0,idrotm[1110],"MANY");
     gMC->Gspos("I650",5,"IT12",0.0,0.0,-22.0,idrotm[1111],"MANY");
     gMC->Gspos("I650",7,"IT12",0.0,0.0,-22.0,idrotm[1112],"MANY");
     gMC->Gspos("I650",9,"IT12",0.0,0.0,-22.0,idrotm[1113],"MANY");
     gMC->Gspos("I650",11,"IT12",0.0,0.0,-22.0,idrotm[1114],"MANY");
     gMC->Gspos("I650",13,"IT12",0.0,0.0,-22.0,idrotm[1115],"MANY");
     gMC->Gspos("I650",15,"IT12",0.0,0.0,-22.0,idrotm[1116],"MANY");
     gMC->Gspos("I650",17,"IT12",0.0,0.0,-22.0,idrotm[1118],"MANY");
     gMC->Gspos("I666",1,"I650",0.0,0.0,0.25,idrotm[1003],"MANY");
     gMC->Gspos("I667",1,"I650",0.1102,0.9945,0.45,idrotm[1088],"ONLY");
     gMC->Gspos("I669",3,"I650",0.1883,4.0372,-3.2,0,"ONLY");
     gMC->Gspos("I671",3,"I650",0.1883,4.0372,0.6,0,"ONLY");
     gMC->Gspos("I669",2,"I650",1.3343,4.0609,-3.2,0,"ONLY");
     gMC->Gspos("I671",2,"I650",1.3343,4.0609,0.6,0,"ONLY");
     gMC->Gspos("I669",6,"I650",2.9567,6.1959,-3.2,idrotm[1089],"ONLY");
     gMC->Gspos("I671",6,"I650",2.9567,6.1959,0.6,idrotm[1089],"ONLY");
     gMC->Gspos("I669",5,"I650",1.9511,6.5822,-3.2,idrotm[1011],"ONLY");
     gMC->Gspos("I671",5,"I650",1.9511,6.5822,0.6,idrotm[1011],"ONLY");
     gMC->Gspos("I669",4,"I650",0.8974,6.8064,-3.2,idrotm[1090],"ONLY");
     gMC->Gspos("I671",4,"I650",0.8974,6.8064,0.6,idrotm[1090],"ONLY");
     gMC->Gspos("I669",1,"I650",-0.1784,6.863,-3.2,0,"ONLY");
     gMC->Gspos("I671",1,"I650",-0.1784,6.863,0.6,0,"ONLY");
     gMC->Gspos("I673",1,"I650",0.2173,4.8037,1.8,0,"ONLY");
     gMC->Gspos("I673",6,"I650",1.5093,4.5605,1.8,0,"ONLY");
     gMC->Gspos("I673",4,"I650",-0.173,6.2531,1.8,idrotm[1092],"ONLY");
     gMC->Gspos("I673",3,"I650",0.8073,6.2032,1.8,idrotm[1093],"ONLY");
     gMC->Gspos("I673",2,"I650",1.7678,6.0005,1.8,idrotm[1094],"ONLY");
     gMC->Gspos("I673",5,"I650",2.6847,5.6501,1.8,0,"ONLY");
     gMC->Gspos("I676",2,"I650",1.7618,5.2269,2.5,0,"ONLY");
     gMC->Gspos("I676",1,"I650",0.4018,5.5869,2.5,0,"ONLY");
     gMC->Gspos("I668",1,"I667",0.0,0.0,0.0,0,"ONLY");
     gMC->Gspos("I670",1,"I669",0.0,0.0,0.0,0,"ONLY");
     gMC->Gspos("I672",1,"I671",0.0,0.0,0.0,0,"ONLY");
     gMC->Gspos("I674",1,"I673",0.0,0.0,0.0,0,"MANY");
     gMC->Gspos("I675",1,"I673",0.0,0.0,-0.5,0,"ONLY");
     gMC->Gspos("I677",1,"I676",0.0,0.0,0.0,0,"MANY");
     gMC->Gspos("I678",1,"I676",0.0,0.0,-0.95,0,"ONLY");  

  }
    
  // --- Place SDD volumes into their mother volume IT34

  
  // -- position SDD detectors of ladder 3 / layer 3

  gMC->Gspos("ITS3", 1,"I302",  0.0,      0.0,    0.0,    0,           "ONLY");
  ySDD = Y_SDD_sep/2.+I302dits[1];
  for (iSDD=0; iSDD<6; iSDD++) {
    gMC->Gspos("I302", iSDD+1, "I004",  0.0, ySDD,  Z_SDD_lay3[iSDD], 0, "ONLY");
    ySDD = -ySDD;
  }

  gMC->Gspos("I004", 1,"IT34", -3.2777,  14.3607, 0.0,   idrotm[321],"ONLY");
  gMC->Gspos("I004", 2,"IT34", -9.5581,  11.9855, 0.0,   idrotm[333],"ONLY");
  gMC->Gspos("I004", 3,"IT34",-13.2713,   6.3911, 0.0,   idrotm[336],"ONLY");
  gMC->Gspos("I004", 4,"IT34",-15.33,     0.0,    0.0,   idrotm[350],"ONLY");
  gMC->Gspos("I004", 5,"IT34",-13.2713,  -6.3911, 0.0,   idrotm[313],"ONLY");
  gMC->Gspos("I004", 6,"IT34", -9.5581, -11.9855, 0.0,   idrotm[311],"ONLY");
  gMC->Gspos("I004", 7,"IT34", -3.2777, -14.3607, 0.0,   idrotm[310],"ONLY");
  gMC->Gspos("I004", 8,"IT34",  3.4112, -14.9456, 0.0,   idrotm[386],"ONLY");
  gMC->Gspos("I004", 9,"IT34",  9.184,  -11.5164, 0.0,   idrotm[309],"ONLY");
  gMC->Gspos("I004",10,"IT34", 13.8119,  -6.6514, 0.0,   idrotm[308],"ONLY");
  gMC->Gspos("I004",11,"IT34", 14.73,     0.0,    0.0,   idrotm[356],"ONLY");
  gMC->Gspos("I004",12,"IT34", 13.8119,   6.6514, 0.0,   idrotm[307],"ONLY");
  gMC->Gspos("I004",13,"IT34",  9.184,   11.5164, 0.0,   idrotm[306],"ONLY");
  gMC->Gspos("I004",14,"IT34",  3.4113,  14.9456, 0.0,   idrotm[305],"ONLY");


  // -- position SDD detectors of ladder 4 / layer 4

  gMC->Gspos("ITS4", 1,"I402",  0.0,      0.000,  0.0,   0,"ONLY");
  ySDD = -(Y_SDD_sep/2.+I402dits[1]);
  for (iSDD=0; iSDD<8; iSDD++) {
    gMC->Gspos("I402", iSDD+1, "I005",  0.0, ySDD,  Z_SDD_lay4[iSDD], 0, "ONLY");
    ySDD = -ySDD;
  }
  
  gMC->Gspos("I005", 1,"IT34", -3.3629,  23.3895,-0.15,  idrotm[335],"ONLY");
  gMC->Gspos("I005", 2,"IT34",-10.0447,  21.9949,-0.15,  idrotm[332],"ONLY");
  gMC->Gspos("I005", 3,"IT34",-15.4744,  17.8584,-0.15,  idrotm[331],"ONLY");
  gMC->Gspos("I005", 4,"IT34",-20.3415,  13.0727,-0.15,  idrotm[366],"ONLY");
  gMC->Gspos("I005", 5,"IT34",-22.6728,   6.6573,-0.15,  idrotm[330],"ONLY");
  gMC->Gspos("I005", 6,"IT34",-24.18,     0.0,   -0.15,  idrotm[350],"ONLY");
  gMC->Gspos("I005", 7,"IT34",-22.6728,  -6.6573,-0.15,  idrotm[329],"ONLY");
  gMC->Gspos("I005", 8,"IT34",-20.3415, -13.0727,-0.15,  idrotm[328],"ONLY");
  gMC->Gspos("I005", 9,"IT34",-15.4744, -17.8584,-0.15,  idrotm[327],"ONLY");
  gMC->Gspos("I005",10,"IT34",-10.0447, -21.9949,-0.15,  idrotm[326],"ONLY");
  gMC->Gspos("I005",11,"IT34", -3.3629, -23.3895,-0.15,  idrotm[325],"ONLY");
  gMC->Gspos("I005",12,"IT34",  3.4412, -23.9339,-0.15,  idrotm[324],"ONLY");
  gMC->Gspos("I005",13,"IT34",  9.8163, -21.4946,-0.15,  idrotm[323],"ONLY");
  gMC->Gspos("I005",14,"IT34", 15.8345, -18.274, -0.15,  idrotm[322],"ONLY");
  gMC->Gspos("I005",15,"IT34", 19.8788, -12.7753,-0.15,  idrotm[320],"ONLY");
  gMC->Gspos("I005",16,"IT34", 23.2005,  -6.8123,-0.15,  idrotm[319],"ONLY");
  gMC->Gspos("I005",17,"IT34", 23.63,     0.0,   -0.15,  idrotm[318],"ONLY");
  gMC->Gspos("I005",18,"IT34", 23.2005,   6.8123,-0.15,  idrotm[317],"ONLY");
  gMC->Gspos("I005",19,"IT34", 19.8788,  12.7753,-0.15,  idrotm[316],"ONLY");
  gMC->Gspos("I005",20,"IT34", 15.8345,  18.274, -0.15,  idrotm[315],"ONLY");
  gMC->Gspos("I005",21,"IT34",  9.8163,  21.4946,-0.15,  idrotm[314],"ONLY");
  gMC->Gspos("I005",22,"IT34",  3.4412,  23.9339,-0.15,  idrotm[334],"ONLY");


  // -- build block of the SDD ladder frame holding the electronics

  gMC->Gspos("I019", 1,"I018", -1.9,     -1.735,  0.0, idrotm[344], "ONLY");
  gMC->Gspos("I019", 2,"I018",  1.987,   -1.5843, 0.0, idrotm[343], "ONLY");
  gMC->Gspos("I019", 3,"I018", -0.087,    1.7066, 0.0, idrotm[342], "ONLY");

  gMC->Gspos("I020", 1,"I018", -1.9782,  -1.569,  0.0, idrotm[342], "ONLY");
  gMC->Gspos("I020", 2,"I018",  1.8824,  -1.735,  0.0, idrotm[344], "ONLY");
  gMC->Gspos("I020", 3,"I018",  0.0958,   1.6913, 0.0, idrotm[343], "ONLY");

  gMC->Gspos("I021", 1,"I018",  1.0761,   0.0835, 2.6008, idrotm[340], "ONLY");
  gMC->Gspos("I021", 2,"I018", -1.0761,   0.0835,-2.8008, idrotm[339], "ONLY");
  gMC->Gspos("I021", 3,"I018", -1.0761,   0.0835,-1.0492, idrotm[338], "ONLY");
  gMC->Gspos("I021", 4,"I018",  1.0761,   0.0835,-2.8008, idrotm[337], "ONLY");
  gMC->Gspos("I021", 5,"I018",  1.0761,   0.0835,-1.0492, idrotm[340], "ONLY");
  gMC->Gspos("I021", 6,"I018", -1.0761,   0.0835, 0.8492, idrotm[339], "ONLY");
  gMC->Gspos("I021", 7,"I018", -1.0761,   0.0835, 2.6008, idrotm[338], "ONLY");
  gMC->Gspos("I021", 8,"I018",  1.0761,   0.0835, 0.8492, idrotm[337], "ONLY");

  gMC->Gspos("I022", 1,"I018",  0.0,     -1.79,   3.55,   idrotm[312], "ONLY");
  gMC->Gspos("I022", 2,"I018",  0.0,     -1.79,  -0.1,    idrotm[312], "ONLY");

  gMC->Gspos("I023", 1,"I018",  0.0,     -1.79,   1.725,  idrotm[341], "ONLY");
  gMC->Gspos("I023", 2,"I018",  0.0,     -1.79,  -1.925,  idrotm[341], "ONLY");

  gMC->Gspos("I033", 1,"I018",  1.8,     -1.75,   1.35,   0,           "MANY");
  gMC->Gspos("I033", 2,"I018", -1.8,     -1.75,  -2.65,   idrotm[345], "MANY");
  gMC->Gspos("I033", 3,"I018", -1.8,     -1.75,   1.35,   idrotm[345], "MANY");
  gMC->Gspos("I033", 4,"I018",  1.8,     -1.75,  -2.65,   0,           "MANY");

  gMC->Gspos("I034", 1,"I018",  1.6,     -1.775,  1.35,   idrotm[312], "ONLY");
  gMC->Gspos("I034", 2,"I018", -1.6,     -1.775, -2.65,   idrotm[348], "ONLY");
  gMC->Gspos("I034", 3,"I018", -1.6,     -1.775,  1.35,   idrotm[348], "ONLY");
  gMC->Gspos("I034", 4,"I018",  1.6,     -1.775, -2.65,   idrotm[312], "ONLY");

  gMC->Gspos("I035", 1,"I018",  1.7,     -0.55, I018dits[2]-I035dits[2], 0, "MANY");
  gMC->Gspos("I035", 2,"I018", -1.7,     -0.55, I018dits[2]-I035dits[2], 0, "MANY");

  gMC->Gspos("I036", 1,"I018",  0.3087,   1.7191, 3.56,   idrotm[346], "ONLY");
  gMC->Gspos("I036", 2,"I018",  0.3087,   1.7191,-0.11,   idrotm[346], "ONLY");
  gMC->Gspos("I036", 3,"I018", -0.3087,   1.7191,-0.11,   idrotm[347], "ONLY");
  gMC->Gspos("I036", 4,"I018", -0.3087,   1.7191, 3.56,   idrotm[347], "ONLY");

  gMC->Gspos("I037", 1,"I018",  SDD_CoolPipe[0], SDD_CoolPipe[1], SDD_CoolPipe[2], 0 , "ONLY");
  gMC->Gspos("I037", 2,"I018", -SDD_CoolPipe[0], SDD_CoolPipe[1], SDD_CoolPipe[2], 0 , "ONLY");

  gMC->Gspos("I038", 1,"I018",  SDD_CoolPipe[0], SDD_CoolPipe[1], SDD_CoolPipe[2], 0 , "ONLY");
  gMC->Gspos("I038", 2,"I018", -SDD_CoolPipe[0], SDD_CoolPipe[1], SDD_CoolPipe[2], 0 , "ONLY");

  gMC->Gspos("I040", 1,"I018",  1.9204,  -0.7118, 0.0, idrotm[346],"ONLY");
  gMC->Gspos("I040", 2,"I018", -1.9204,  -0.7118, 0.0, idrotm[347],"ONLY");
  gMC->Gspos("I041", 1,"I018",  SDD_CoolPipe[0], SDD_CoolPipe[1], SDD_CoolPipe[2], idrotm[346], "ONLY");
  gMC->Gspos("I041", 2,"I018", -SDD_CoolPipe[0], SDD_CoolPipe[1], SDD_CoolPipe[2], idrotm[347], "ONLY");


  // -- build block of the SDD electronics (heat bridge, chips, hybrid, anode microcable)

  xI050 = SDD_CoolPipe[0]+SDD_CoolPipe[1]*sin30+I050dits[1]/cos30+I041dits[1];
  yI050 = 0;
  xI039 = -SDD_CoolPipe[1]/cos30;
  yI039 = -I050dits[1]+I039dits[1];
  gMC->Gspos("I039", 1,"I050",  xI039, yI039, 0.0, 0, "ONLY");
  xI042 = xI039+I039dits[0]-xI042space-I042dits[0];
  yI042 = yI039+I039dits[1]+I042dits[1];
  xI043 = xI039-I039dits[0]+xI043space+I043dits[0];
  yI043 = yI039+I039dits[1]+I043dits[1];
  zChipSpace = I042dits[2];
  if (zChipSpace < I043dits[2]) {
    zChipSpace = I043dits[2];
  }
  zChipSpace = zChipSpace * 2;
  yI051space = (2*I039dits[2] - 4*zChipSpace)/5;
  zchip = -I039dits[2] + yI051space + zChipSpace/2.;
  for (ichip=0; ichip<4; ichip++) { 
    gMC->Gspos("I042", ichip+1, "I050", xI042, yI042, zchip, 0, "ONLY");
    gMC->Gspos("I043", ichip+1, "I050", xI043, yI043, zchip, 0, "ONLY");
    zchip += zChipSpace + yI051space;
  }
  xcap = 2*I039dits[0]/5.;
  yI051 = yI039+I039dits[1]+I051dits[1];
  zI051 = -I039dits[2] + yI051space/3.;
  icap = 1;
  for (ichip=0; ichip<5; ichip++) { 
    xI051 = xI039-I039dits[0]+xcap;
    gMC->Gspos("I051", icap++,"I050", xI051, yI051, zI051, 0, "ONLY");
    zI051 += yI051space/3.;
    gMC->Gspos("I051", icap++,"I050", xI051, yI051, zI051, 0, "ONLY");
    xI051 += xcap;
    gMC->Gspos("I051", icap++,"I050", xI051, yI051, zI051, 0, "ONLY");
    xI051 += xcap;
    gMC->Gspos("I051", icap++,"I050", xI051, yI051, zI051, 0, "ONLY");
    xI051 += xcap;
    gMC->Gspos("I051", icap++,"I050", xI051, yI051, zI051, 0, "ONLY");
    zI051 -= yI051space/3.;
    if (ichip == 0) {
      gMC->Gspos("I051", icap++,"I050", xI051, yI051, zI051, 0, "ONLY");
    }
    zI051 += zChipSpace + yI051space;
  }
  xI052 = -I050dits[0]+I052dits[0];
  yI052 = yI051+I051dits[1]+I052dits[1];
  gMC->Gspos("I052", 1,"I050", xI052, yI052, 0.0, 0, "ONLY");
  xI044 = I050dits[0]-I044dits[3];
  yI044 = yI052+I052dits[1]+I044dits[2];
  gMC->Gspos("I044", 1,"I050", xI044, yI044, 0.0, idrotm[301], "ONLY");
  gMC->Gspos("I050", 1,"I018",  xI050,  yI050,  0.0, idrotm[346],"ONLY");
  gMC->Gspos("I050", 2,"I018", -xI050,  yI050,  0.0, idrotm[347],"ONLY");


  // -- build block of the SDD ladder frame at the end ladders

  gMC->Gspos("I021",12,"I024",  1.0761,   0.0836,-0.1242, idrotm[340], "ONLY");
  gMC->Gspos("I021",11,"I024", -1.0761,   0.0836,-0.1242, idrotm[338], "ONLY");
  gMC->Gspos("I021",13,"I024", -1.0761,   0.0836,-1.8758, idrotm[339], "ONLY");
  gMC->Gspos("I021",14,"I024",  1.0761,   0.0836,-1.8758, idrotm[337], "ONLY");

  gMC->Gspos("I022", 3,"I024",  0.0,     -1.7899, 0.825,  idrotm[312], "ONLY");

  gMC->Gspos("I023", 3,"I024",  0.0,     -1.7899,-1.0,    idrotm[341], "ONLY");

  gMC->Gspos("I025", 1,"I024", -1.9,     -1.7349, 0.0,    idrotm[344], "ONLY");
  gMC->Gspos("I025", 2,"I024",  1.987,   -1.5842, 0.0,    idrotm[343], "ONLY");

  gMC->Gspos("I026", 1,"I024", -1.9782,  -1.5689, 0.0,    idrotm[342], "ONLY");
  gMC->Gspos("I026", 2,"I024",  1.8824,  -1.7349, 0.0,    idrotm[344], "ONLY");

  gMC->Gspos("I029", 1,"I024", -0.087,    1.7067, I029dits[2]-I024dits[2], idrotm[342], "ONLY");

  gMC->Gspos("I030", 1,"I024",  0.0958,   1.6914, I030dits[2]-I024dits[2], idrotm[343], "ONLY");

  gMC->Gspos("I031", 1,"I024",  SDD_CoolPipe[0], SDD_CoolPipe[1], SDD_CoolPipe[2], 0 ,"ONLY");
  gMC->Gspos("I031", 2,"I024", -SDD_CoolPipe[0], SDD_CoolPipe[1], SDD_CoolPipe[2], 0 ,"ONLY");

  gMC->Gspos("I032", 1,"I024",  SDD_CoolPipe[0], SDD_CoolPipe[1], SDD_CoolPipe[2], 0 ,"ONLY");
  gMC->Gspos("I032", 2,"I024", -SDD_CoolPipe[0], SDD_CoolPipe[1], SDD_CoolPipe[2], 0 ,"ONLY");


  xI424 = I028dits[0]/3.;
  yI424 = -I028dits[1]+I424dits[1];
  gMC->Gspos("I422", 1,"I421", 0.0, 0.0, 0.0, 0, "ONLY");
  gMC->Gspos("I423", 1,"I421", 0.0, 0.0, I421dits[2]-I423dits[2], 0, "ONLY");
  gMC->Gspos("I421", 1,"I420", 0.0, 0.0, 0.0, idrotm[312], "ONLY");
  gMC->Gspos("I420", 1,"I028", -I028dits[0]/3., I028dits[1]-I420dits[1], 0.0, 0, "ONLY");
  gMC->Gspos("I424", 1,"I028", xI424, yI424, 0.0, 0, "ONLY");
  gMC->Gspos("I028", 1,"I024", 0.0, I028dits[1]-I024dits[1], I024dits[2]-I028dits[2], 0, "MANY");


  // -- build the SDD ladder 3

  indI425 = 1;
  gMC->Gspos("I024", 1,"I047",  0.0,      0.0,   24.625, 0,           "ONLY");  
  gMC->Gspos("I018", 1,"I047",  0.0,      0.0,    3.65,  0,           "ONLY");
  gMC->Gspos("I018", 2,"I047",  0.0,      0.0,   10.95,  0,           "ONLY");
  gMC->Gspos("I018", 3,"I047",  0.0,      0.0,   18.25,  0,           "ONLY");
  gMC->Gspos("I018", 4,"I047",  0.0,      0.0,   -3.65,  0,           "ONLY");
  gMC->Gspos("I018", 5,"I047",  0.0,      0.0,  -10.95,  0,           "ONLY");
  gMC->Gspos("I018", 6,"I047",  0.0,      0.0,  -18.25,  0,           "ONLY");
  gMC->Gspos("I024", 2,"I047",  0.0,      0.0,  -24.625, idrotm[355], "ONLY");
  HVname[0] = 'I';
  HVname[1] = '3';
  HVname[2] = '1';  
  HVname[4] = '\0';
  for (iSDD=0; iSDD<3; iSDD++) {
    HVname[3] = (Char_t)(48+iSDD+5);
    dits[0] = 1.350000;
    dits[1] = I425dits[1];
    dits[2] = (I047dits[2] - 2*I024dits[2] - Z_SDD_lay3[iSDD])/2.;
    gMC->Gsvolu(HVname, "BOX ", idtmed[279], dits, 3);
    xHV = 0.0;
    yHV = -I047dits[1] + (2*iSDD+1)*dits[1];
    zHV = I047dits[2] - 2*I024dits[2] - dits[2];
    gMC->Gspos(HVname, 1,"I047", xHV, yHV,  zHV, 0, "ONLY");
    gMC->Gspos(HVname, 2,"I047", xHV, yHV, -zHV, 0, "ONLY");
    gMC->Gspos("I425", indI425++,"I047",  xI424, yHV,   24.625, 0, "ONLY");
    gMC->Gspos("I425", indI425++,"I047", -xI424, yHV,  -24.625, 0, "ONLY");
  }
  LVname[0] = 'I';
  LVname[1] = '3';
  LVname[2] = '1';  
  LVname[4] = '\0';
  for (iSDD=0; iSDD<3; iSDD++) {
    LVname[3] = (Char_t)(48+iSDD+1);
    dits[0] = 1.350000;
    dits[1] = 0.004423;
    dits[2] = (I047dits[2] - (2*iSDD+1)*I018dits[2] - I039dits[2])/2.;
    gMC->Gsvolu(LVname, "BOX ", idtmed[280], dits, 3);
    yLV = I018dits[1] - dits[0]*cos30 - dits[1]*sin30;
    xLV = xI050 -
          fabs(yI050-yLV)*sin30/cos30 +
          (I050dits[1]+(2*iSDD+1)*dits[1])/cos30;
    zLV = I047dits[2] - dits[2];
    gMC->Gspos(LVname, 1,"I047",  xLV, yLV,  zLV, idrotm[346], "ONLY");
    gMC->Gspos(LVname, 2,"I047",  xLV, yLV, -zLV, idrotm[346], "ONLY");
    gMC->Gspos(LVname, 3,"I047", -xLV, yLV,  zLV, idrotm[347], "ONLY");
    gMC->Gspos(LVname, 4,"I047", -xLV, yLV, -zLV, idrotm[347], "ONLY");
  }


  // -- build the SDD ladder 4


  gMC->Gspos("I024", 3,"I048", -0.0001,   0.0,   31.925, 0,           "ONLY");
  gMC->Gspos("I018", 7,"I048", -0.0001,   0.0,   -3.65,  0,           "ONLY");
  gMC->Gspos("I018", 8,"I048", -0.0001,   0.0,    3.65,  0,           "ONLY");
  gMC->Gspos("I018", 9,"I048", -0.0001,   0.0,   10.95,  0,           "ONLY");
  gMC->Gspos("I018",10,"I048", -0.0001,   0.0,   18.25,  0,           "ONLY");
  gMC->Gspos("I018",11,"I048", -0.0001,   0.0,   25.55,  0,           "ONLY");
  gMC->Gspos("I018",12,"I048", -0.0001,   0.0,  -10.95,  0,           "ONLY");
  gMC->Gspos("I018",13,"I048", -0.0001,   0.0,  -18.25,  0,           "ONLY");
  gMC->Gspos("I018",14,"I048", -0.0001,   0.0,  -25.55,  0,           "ONLY");
  gMC->Gspos("I024", 4,"I048", -0.0001,   0.0,  -31.925, idrotm[355], "ONLY");
  HVname[0] = 'I';
  HVname[1] = '4';
  HVname[2] = '1';  
  HVname[4] = '\0';  
  for (iSDD=0; iSDD<4; iSDD++) {
    HVname[3] = (Char_t)(48+iSDD+5);
    dits[0] = 1.350000;
    dits[1] = I425dits[1];
    dits[2] = (I048dits[2] - 2*I024dits[2] - Z_SDD_lay4[iSDD])/2.;
    gMC->Gsvolu(HVname, "BOX ", idtmed[279], dits, 3);
    xHV = -0.0001;
    yHV = -I048dits[1] + (2*iSDD+1)*dits[1];
    zHV = I048dits[2] - 2*I024dits[2] - dits[2];
    gMC->Gspos(HVname, 1,"I048", xHV, yHV,  zHV, 0, "ONLY");
    gMC->Gspos(HVname, 2,"I048", xHV, yHV, -zHV, 0, "ONLY");
    gMC->Gspos("I425", indI425++,"I048",  xI424, yHV,   31.925, 0, "ONLY");
    gMC->Gspos("I425", indI425++,"I048", -xI424, yHV,  -31.925, 0, "ONLY");
  }
  LVname[0] = 'I';
  LVname[1] = '4';
  LVname[2] = '1';  
  LVname[4] = '\0';
  for (iSDD=0; iSDD<4; iSDD++) {
    LVname[3] = (Char_t)(48+iSDD+1);
    dits[0] = 1.350000;
    dits[1] = 0.004423;
    dits[2] = (I048dits[2] - (2*iSDD+1)*I018dits[2] - I039dits[2])/2.;
    gMC->Gsvolu(LVname, "BOX ", idtmed[280], dits, 3);
    yLV = I018dits[1] - dits[0]*cos30 - dits[1]*sin30;
    xLV = xI050 -
          fabs(yI050-yLV)*sin30/cos30 +
          (I050dits[1]+(2*iSDD+1)*dits[1])/cos30;
    zLV = I048dits[2] - dits[2];
    gMC->Gspos(LVname, 1,"I048",  xLV, yLV,  zLV, idrotm[346], "ONLY");
    gMC->Gspos(LVname, 2,"I048",  xLV, yLV, -zLV, idrotm[346], "ONLY");
    gMC->Gspos(LVname, 3,"I048", -xLV, yLV,  zLV, idrotm[347], "ONLY");
    gMC->Gspos(LVname, 4,"I048", -xLV, yLV, -zLV, idrotm[347], "ONLY");
  }


  // -- build the SDD barrel (layers 3 and 4)

  gMC->Gspos("I047", 1,"IT34", -3.7528,  16.4422, 0.0,   idrotm[321], "ONLY");
  gMC->Gspos("I047", 2,"IT34",-10.8892,  13.6547, 0.0,   idrotm[333], "ONLY");
  gMC->Gspos("I047", 3,"IT34",-15.1948,   7.3175, 0.0,   idrotm[336], "ONLY");
  gMC->Gspos("I047", 4,"IT34",-17.465,    0.0,    0.0,   idrotm[350], "ONLY");
  gMC->Gspos("I047", 5,"IT34",-15.1948,  -7.3174, 0.0,   idrotm[313], "ONLY");
  gMC->Gspos("I047", 6,"IT34",-10.8893, -13.6547, 0.0,   idrotm[311], "ONLY");
  gMC->Gspos("I047", 7,"IT34", -3.7528, -16.4422, 0.0,   idrotm[310], "ONLY");
  gMC->Gspos("I047", 8,"IT34",  3.8863, -17.0271, 0.0,   idrotm[386], "ONLY");
  gMC->Gspos("I047", 9,"IT34", 10.5152, -13.1856, 0.0,   idrotm[309], "ONLY");
  gMC->Gspos("I047",10,"IT34", 15.7354,  -7.5778, 0.0,   idrotm[308], "ONLY");
  gMC->Gspos("I047",11,"IT34", 16.865,    0.0,    0.0,   idrotm[356], "ONLY");
  gMC->Gspos("I047",12,"IT34", 15.7354,   7.5778, 0.0,   idrotm[307], "ONLY");
  gMC->Gspos("I047",13,"IT34", 10.5152,  13.1856, 0.0,   idrotm[306], "ONLY");
  gMC->Gspos("I047",14,"IT34",  3.8863,  17.0271, 0.0,   idrotm[305], "ONLY");

  gMC->Gspos("I048", 1,"IT34", -3.6667,  25.5027, 0.0,   idrotm[335], "ONLY");
  gMC->Gspos("I048", 2,"IT34",-10.9317,  23.937,  0.0,   idrotm[332], "ONLY");
  gMC->Gspos("I048", 3,"IT34",-16.8725,  19.4719, 0.0,   idrotm[331], "ONLY");
  gMC->Gspos("I048", 4,"IT34",-22.1376,  14.227,  0.0,   idrotm[366], "ONLY");
  gMC->Gspos("I048", 5,"IT34",-24.7213,   7.2588, 0.0,   idrotm[330], "ONLY");
  gMC->Gspos("I048", 6,"IT34",-26.315,    0.0,    0.0,   idrotm[350], "ONLY");
  gMC->Gspos("I048", 7,"IT34",-24.7213,  -7.2588, 0.0,   idrotm[329], "ONLY");
  gMC->Gspos("I048", 8,"IT34",-22.1376, -14.227,  0.0,   idrotm[328], "ONLY");
  gMC->Gspos("I048", 9,"IT34",-16.8725, -19.4719, 0.0,   idrotm[327], "ONLY");
  gMC->Gspos("I048",10,"IT34",-10.9316, -23.937,  0.0,   idrotm[326], "ONLY");
  gMC->Gspos("I048",11,"IT34", -3.6667, -25.5027, 0.0,   idrotm[325], "ONLY");
  gMC->Gspos("I048",12,"IT34",  3.745,  -26.0472, 0.0,   idrotm[324], "ONLY");
  gMC->Gspos("I048",13,"IT34", 10.7032, -23.4367, 0.0,   idrotm[323], "ONLY");
  gMC->Gspos("I048",14,"IT34", 17.2327, -19.8876, 0.0,   idrotm[322], "ONLY");
  gMC->Gspos("I048",15,"IT34", 21.6749, -13.9296, 0.0,   idrotm[320], "ONLY");
  gMC->Gspos("I048",16,"IT34", 25.2491,  -7.4138, 0.0,   idrotm[319], "ONLY");
  gMC->Gspos("I048",17,"IT34", 25.765,    0.0,    0.0,   idrotm[318], "ONLY");
  gMC->Gspos("I048",18,"IT34", 25.2491,   7.4138, 0.0,   idrotm[317], "ONLY");
  gMC->Gspos("I048",19,"IT34", 21.6749,  13.9296, 0.0,   idrotm[316], "ONLY");
  gMC->Gspos("I048",20,"IT34", 17.2327,  19.8876, 0.0,   idrotm[315], "ONLY");
  gMC->Gspos("I048",21,"IT34", 10.7032,  23.4367, 0.0,   idrotm[314], "ONLY");
  gMC->Gspos("I048",22,"IT34", 3.745,    26.0472, 0.0,   idrotm[334], "ONLY");

  
  // --- Place SSD volumes into their mother volume IT56  


  gMC->Gspos("I570",14,"IT56",-28.0681,-36.0619,-0.27,idrotm[566],"ONLY"); 
  gMC->Gspos("I570",15,"IT56",-21.677,-40.0556,-0.27,idrotm[567],"ONLY");
  gMC->Gspos("I570",16,"IT56",-14.838,-43.2217,-0.27,idrotm[568],"ONLY");
  gMC->Gspos("I570",17,"IT56",-7.4965,-44.9238,-0.27,idrotm[569],"ONLY");
  gMC->Gspos("I570",18,"IT56",-0.27,-45.6977,-0.27,idrotm[533],"ONLY");
  gMC->Gspos("I570",19,"IT56",7.4965,-44.9238,-0.27,idrotm[534],"ONLY");
  gMC->Gspos("I570",20,"IT56",14.838,-43.2217,-0.27,idrotm[535],"ONLY");
  gMC->Gspos("I570",21,"IT56",21.677,-40.0556,-0.27,idrotm[623],"ONLY");
  gMC->Gspos("I570",22,"IT56",28.0681,-36.0619,-0.27,idrotm[537],"ONLY");
  gMC->Gspos("I570",23,"IT56",33.5085,-30.8468,-0.27,idrotm[538],"ONLY");
  gMC->Gspos("I570",24,"IT56",38.2566,-24.9943,-0.27,idrotm[539],"ONLY");
  gMC->Gspos("I570",25,"IT56",41.7089,-18.2952,-0.27,idrotm[540],"ONLY");
  gMC->Gspos("I570",26,"IT56",44.2994,-11.2181,-0.27,idrotm[541],"ONLY");
  gMC->Gspos("I570",27,"IT56",45.3894,-3.7611,-0.27,idrotm[542],"ONLY");
  gMC->Gspos("I570",28,"IT56",45.5416,3.7737,-0.27,idrotm[543],"ONLY");
  gMC->Gspos("I570",29,"IT56",44.1513,11.1806,-0.27,idrotm[544],"ONLY");
  gMC->Gspos("I570",30,"IT56",41.8487,18.3566,-0.27,idrotm[545],"ONLY");
  gMC->Gspos("I570",31,"IT56",38.1287,24.9107,-0.27,idrotm[546],"ONLY");
  gMC->Gspos("I570",32,"IT56",33.6209,30.9502,-0.27,idrotm[547],"ONLY");
  gMC->Gspos("I570",33,"IT56",27.9743,35.9414,-0.27,idrotm[548],"ONLY");
  gMC->Gspos("I570",34,"IT56",21.7497,40.1899,-0.27,idrotm[549],"ONLY");
  gMC->Gspos("I570",35,"IT56",14.7884,43.0772,-0.27,idrotm[550],"ONLY");
  gMC->Gspos("I570",36,"IT56",7.5216,45.0744,-0.27,idrotm[551],"ONLY");
  gMC->Gspos("I570",37,"IT56",0.,45.545,-0.27,0,"ONLY");
  gMC->Gspos("I570",38,"IT56",-7.5216,45.0744,-0.27,idrotm[552],"ONLY");
  gMC->Gspos("I570",1,"IT56",-14.7884,43.0772,-0.27,idrotm[553],"ONLY");
  gMC->Gspos("I570",2,"IT56",-21.7497,40.1899,-0.27,idrotm[620],"ONLY");
  gMC->Gspos("I570",3,"IT56",-27.9743,35.9414,-0.27,idrotm[555],"ONLY");
  gMC->Gspos("I570",4,"IT56",-33.6209,30.9502,-0.27,idrotm[556],"ONLY");
  gMC->Gspos("I570",5,"IT56",-38.1287,24.9108,-0.27,idrotm[557],"ONLY");
  gMC->Gspos("I570",6,"IT56",-41.8487,18.3566,-0.27,idrotm[558],"ONLY");
  gMC->Gspos("I570",7,"IT56",-44.1513,11.1806,-0.27,idrotm[559],"ONLY");
  gMC->Gspos("I570",8,"IT56",-45.5416,3.7737,-0.27,idrotm[560],"ONLY");
  gMC->Gspos("I570",9,"IT56",-45.3894,-3.7611,-0.27,idrotm[561],"ONLY");
  gMC->Gspos("I570",10,"IT56",-44.2994,-11.2181,-0.27,idrotm[562],"ONLY");
  gMC->Gspos("I570",11,"IT56",-41.7089,-18.2952,-0.27,idrotm[563],"ONLY");
  gMC->Gspos("I570",12,"IT56",-38.2566,-24.9943,-0.27,idrotm[564],"ONLY");
  gMC->Gspos("I570",13,"IT56",-33.5086,-30.8468,-0.27,idrotm[565],"ONLY");
  gMC->Gspos("I569",8,"IT56",-43.5484,3.6085,0.0,idrotm[560],"ONLY");
  gMC->Gspos("I569",9,"IT56",-43.3963,-3.5959,0.0,idrotm[561],"ONLY");
  gMC->Gspos("I569",10,"IT56",-42.3606,-10.7271,0.0,idrotm[562],"ONLY");
  gMC->Gspos("I569",11,"IT56",-39.8773,-17.4918,0.0,idrotm[563],"ONLY");
  gMC->Gspos("I569",12,"IT56",-36.5823,-23.9004,0.0,idrotm[564],"ONLY");
  gMC->Gspos("I569",13,"IT56",-32.0371,-29.4922,0.0,idrotm[565],"ONLY");
  gMC->Gspos("I569",14,"IT56",-26.8397,-34.4836,0.0,idrotm[566],"ONLY");
  gMC->Gspos("I569",15,"IT56",-20.7251,-38.2967,0.0,idrotm[567],"ONLY");
  gMC->Gspos("I569",16,"IT56",-14.1886,-41.33,0.0,idrotm[568],"ONLY");
  gMC->Gspos("I569",17,"IT56",-7.1673,-42.9511,0.0,idrotm[569],"ONLY");
  gMC->Gspos("I569",18,"IT56",0.0,-43.6977,0.0,idrotm[533],"ONLY");
  gMC->Gspos("I569",19,"IT56",7.1673,-42.9511,0.0,idrotm[534],"ONLY");
  gMC->Gspos("I569",20,"IT56",14.1886,-41.33,0.0,idrotm[535],"ONLY");
  gMC->Gspos("I569",21,"IT56",20.7251,-38.2967,0.0,idrotm[623],"ONLY");
  gMC->Gspos("I569",22,"IT56",26.8397,-34.4836,0.0,idrotm[537],"ONLY");
  gMC->Gspos("I569",23,"IT56",32.0371,-29.4922,0.0,idrotm[538],"ONLY");
  gMC->Gspos("I569",24,"IT56",36.5822,-23.9004,0.0,idrotm[539],"ONLY");
  gMC->Gspos("I569",25,"IT56",39.8773,-17.4918,0.0,idrotm[540],"ONLY");
  gMC->Gspos("I569",26,"IT56",42.3606,-10.7272,0.0,idrotm[541],"ONLY");
  gMC->Gspos("I569",27,"IT56",43.3963,-3.5959,0.0,idrotm[542],"ONLY");
  gMC->Gspos("I569",28,"IT56",43.5484,3.6085,0.0,idrotm[543],"ONLY");
  gMC->Gspos("I569",29,"IT56",42.2125,10.6897,0.0,idrotm[544],"ONLY");
  gMC->Gspos("I569",30,"IT56",40.0172,17.5532,0.0,idrotm[545],"ONLY");
  gMC->Gspos("I569",31,"IT56",36.4544,23.8169,0.0,idrotm[546],"ONLY");
  gMC->Gspos("I569",32,"IT56",32.1494,29.5956,0.0,idrotm[547],"ONLY");
  gMC->Gspos("I569",33,"IT56",26.7459,34.3631,0.0,idrotm[548],"ONLY");
  gMC->Gspos("I569",34,"IT56",20.7978,38.431,0.0,idrotm[549],"ONLY");
  gMC->Gspos("I569",35,"IT56",14.139,41.1856,0.0,idrotm[550],"ONLY");
  gMC->Gspos("I569",36,"IT56",7.1924,43.1017,0.0,idrotm[551],"ONLY");
  gMC->Gspos("I569",37,"IT56",0.0,43.545,0.0,0,"ONLY");
  gMC->Gspos("I569",38,"IT56",-7.1924,43.1017,0.0,idrotm[552],"ONLY");
  gMC->Gspos("I569",1,"IT56",-14.139,41.1856,0.0,idrotm[553],"ONLY");
  gMC->Gspos("I569",2,"IT56",-20.7978,38.431,0.0,idrotm[620],"ONLY");
  gMC->Gspos("I569",3,"IT56",-26.7459,34.3631,0.0,idrotm[555],"ONLY");
  gMC->Gspos("I569",4,"IT56",-32.1494,29.5956,0.0,idrotm[556],"ONLY");
  gMC->Gspos("I569",5,"IT56",-36.4544,23.8169,0.0,idrotm[557],"ONLY");
  gMC->Gspos("I569",6,"IT56",-40.0172,17.5532,0.0,idrotm[558],"ONLY");
  gMC->Gspos("I569",7,"IT56",-42.2125,10.6897,0.0,idrotm[559],"ONLY");
  gMC->Gspos("I571",15,"IT56",-21.2916,-34.387,0.0,idrotm[501],"ONLY");
  gMC->Gspos("I571",14,"IT56",-27.351,-30.0026,0.0,idrotm[503],"ONLY");
  gMC->Gspos("I571",13,"IT56",-32.2758,-24.3735,0.0,idrotm[504],"ONLY");
  gMC->Gspos("I571",12,"IT56",-36.3422,-18.0963,0.0,idrotm[505],"ONLY");
  gMC->Gspos("I571",11,"IT56",-38.901,-11.0683,0.0,idrotm[506],"ONLY");
  gMC->Gspos("I571",10,"IT56",-40.4252,-3.7459,0.0,idrotm[507],"ONLY");
  gMC->Gspos("I571",9,"IT56",-40.2725,3.7318,0.0,idrotm[508],"ONLY");
  gMC->Gspos("I571",8,"IT56",-39.0486,11.1103,0.0,idrotm[509],"ONLY");
  gMC->Gspos("I571",7,"IT56",-36.2049,18.0279,0.0,idrotm[510],"ONLY");
  gMC->Gspos("I571",6,"IT56",-32.3982,24.466,0.0,idrotm[511],"ONLY");
  gMC->Gspos("I571",5,"IT56",-27.2476,29.8892,0.0,idrotm[512],"ONLY");
  gMC->Gspos("I571",4,"IT56",-21.3723,34.5175,0.0,idrotm[513],"ONLY");
  gMC->Gspos("I571",3,"IT56",-14.6104,37.7138,0.0,idrotm[653],"ONLY");
  gMC->Gspos("I571",2,"IT56",-7.4599,39.9072,0.0,idrotm[514],"ONLY");
  gMC->Gspos("I571",1,"IT56",0.0,40.445,0.0,0,"ONLY");
  gMC->Gspos("I571",34,"IT56",7.46,39.9071,0.0,idrotm[515],"ONLY");
  gMC->Gspos("I571",33,"IT56",14.6104,37.7138,0.0,idrotm[516],"ONLY");
  gMC->Gspos("I571",32,"IT56",21.3723,34.5175,0.0,idrotm[517],"ONLY");
  gMC->Gspos("I571",31,"IT56",27.2476,29.8892,0.0,idrotm[518],"ONLY");
  gMC->Gspos("I571",30,"IT56",32.3983,24.466,0.0,idrotm[519],"ONLY");
  gMC->Gspos("I571",29,"IT56",36.2049,18.0279,0.0,idrotm[520],"ONLY");
  gMC->Gspos("I571",28,"IT56",39.0486,11.1103,0.0,idrotm[521],"ONLY");
  gMC->Gspos("I571",27,"IT56",40.2725,3.7318,0.0,idrotm[522],"ONLY");
  gMC->Gspos("I571",26,"IT56",40.4252,-3.746,0.0,idrotm[523],"ONLY");
  gMC->Gspos("I571",25,"IT56",38.901,-11.0683,0.0,idrotm[524],"ONLY");
  gMC->Gspos("I571",24,"IT56",36.3422,-18.0963,0.0,idrotm[525],"ONLY");
  gMC->Gspos("I571",23,"IT56",32.2758,-24.3736,0.0,idrotm[526],"ONLY");
  gMC->Gspos("I571",22,"IT56",27.351,-30.0026,0.0,idrotm[527],"ONLY");
  gMC->Gspos("I571",21,"IT56",21.2915,-34.387,0.0,idrotm[528],"ONLY");
  gMC->Gspos("I571",20,"IT56",14.6658,-37.8569,0.0,idrotm[618],"ONLY");
  gMC->Gspos("I571",19,"IT56",7.4317,-39.7563,0.0,idrotm[529],"ONLY");
  gMC->Gspos("I571",18,"IT56",0.0,-40.5984,0.0,idrotm[533],"ONLY");
  gMC->Gspos("I571",17,"IT56",-7.4318,-39.7563,0.0,idrotm[530],"ONLY");
  gMC->Gspos("I571",16,"IT56",-14.6659,-37.8569,0.0,idrotm[531],"ONLY");
  gMC->Gspos("I565",13,"IT56",-30.6798,-23.1683,0.0,idrotm[504],"ONLY");
  gMC->Gspos("I565",12,"IT56",-34.5519,-17.2048,0.0,idrotm[505],"ONLY");
  gMC->Gspos("I565",11,"IT56",-36.9774,-10.521,0.0,idrotm[506],"ONLY");
  gMC->Gspos("I565",10,"IT56",-38.4338,-3.5614,0.0,idrotm[507],"ONLY");
  gMC->Gspos("I565",9,"IT56",-38.281,3.5473,0.0,idrotm[508],"ONLY");
  gMC->Gspos("I565",8,"IT56",-37.1249,10.563,0.0,idrotm[509],"ONLY");
  gMC->Gspos("I565",7,"IT56",-34.4146,17.1364,0.0,idrotm[510],"ONLY");
  gMC->Gspos("I565",6,"IT56",-30.8022,23.2608,0.0,idrotm[511],"ONLY");
  gMC->Gspos("I565",5,"IT56",-25.9002,28.4112,0.0,idrotm[512],"ONLY");
  gMC->Gspos("I565",4,"IT56",-20.3195,32.817,0.0,idrotm[513],"ONLY");
  gMC->Gspos("I565",3,"IT56",-13.8879,35.8489,0.0,idrotm[653],"ONLY");
  gMC->Gspos("I565",2,"IT56",-7.0924,37.9412,0.0,idrotm[514],"ONLY");
  gMC->Gspos("I565",1,"IT56",0.0,38.445,0.0,0,"ONLY");
  gMC->Gspos("I565",34,"IT56",7.0925,37.9412,0.0,idrotm[515],"ONLY");
  gMC->Gspos("I565",33,"IT56",13.888,35.8489,0.0,idrotm[516],"ONLY");
  gMC->Gspos("I565",32,"IT56",20.3195,32.817,0.0,idrotm[517],"ONLY");
  gMC->Gspos("I565",31,"IT56",25.9002,28.4112,0.0,idrotm[518],"ONLY");
  gMC->Gspos("I565",30,"IT56",30.8022,23.2607,0.0,idrotm[519],"ONLY");
  gMC->Gspos("I565",29,"IT56",34.4146,17.1364,0.0,idrotm[520],"ONLY");
  gMC->Gspos("I565",28,"IT56",37.125,10.5629,0.0,idrotm[521],"ONLY");
  gMC->Gspos("I565",27,"IT56",38.281,3.5472,0.0,idrotm[522],"ONLY");
  gMC->Gspos("I565",26,"IT56",38.4338,-3.5614,0.0,idrotm[523],"ONLY");
  gMC->Gspos("I565",25,"IT56",36.9774,-10.521,0.0,idrotm[524],"ONLY");
  gMC->Gspos("I565",24,"IT56",34.5519,-17.2048,0.0,idrotm[525],"ONLY");
  gMC->Gspos("I565",23,"IT56",30.6798,-23.1683,0.0,idrotm[526],"ONLY");
  gMC->Gspos("I565",22,"IT56",26.0036,-28.5246,0.0,idrotm[527],"ONLY");
  gMC->Gspos("I565",21,"IT56",20.2387,-32.6866,0.0,idrotm[528],"ONLY");
  gMC->Gspos("I565",20,"IT56",13.9433,-35.992,0.0,idrotm[618],"ONLY");
  gMC->Gspos("I565",19,"IT56",7.0642,-37.7904,0.0,idrotm[529],"ONLY");
  gMC->Gspos("I565",18,"IT56",0.0,-38.5984,0.0,idrotm[533],"ONLY");
  gMC->Gspos("I565",17,"IT56",-7.0643,-37.7904,0.0,idrotm[530],"ONLY");
  gMC->Gspos("I565",16,"IT56",-13.9434,-35.992,0.0,idrotm[531],"ONLY");
  gMC->Gspos("I565",15,"IT56",-20.2387,-32.6866,0.0,idrotm[501],"ONLY");
  gMC->Gspos("I565",14,"IT56",-26.0036,-28.5246,0.0,idrotm[503],"ONLY");
  gMC->Gspos("I553",1,"I570",0.005,0.0,52.8453,0,"ONLY");
  gMC->Gspos("I523",1,"I570",0.0,0.0,46.9203+0.82,0,"ONLY");
  gMC->Gspos("I523",2,"I570",0.0,0.0,43.0103+0.82,0,"ONLY");
  gMC->Gspos("I523",3,"I570",0.0,0.0,39.1003+0.82,0,"ONLY");
  gMC->Gspos("I523",4,"I570",0.0,0.0,35.1903+0.82,0,"ONLY");
  gMC->Gspos("I523",5,"I570",0.0,0.0,31.2803+0.82,0,"ONLY");
  gMC->Gspos("I523",6,"I570",0.0,0.0,27.3703+0.82,0,"ONLY");
  gMC->Gspos("I523",7,"I570",0.0,0.0,23.4603+0.82,0,"ONLY");
  gMC->Gspos("I523",8,"I570",0.0,0.0,19.5503+0.82,0,"ONLY");
  gMC->Gspos("I523",9,"I570",0.0,0.0,15.6403+0.82,0,"ONLY");
  gMC->Gspos("I523",10,"I570",0.0,0.0,11.7303+0.82,0,"ONLY");
  gMC->Gspos("I523",11,"I570",0.0,0.0,7.8203+0.82,0,"ONLY");
  gMC->Gspos("I523",12,"I570",0.0,0.0,3.9103+0.82,0,"ONLY");
  gMC->Gspos("I523",13,"I570",0.0,0.0,0.0003+0.82,0,"ONLY");
  gMC->Gspos("I523",14,"I570",0.0,0.0,-3.9097+0.82,0,"ONLY");
  gMC->Gspos("I523",15,"I570",0.0,0.0,-7.8197+0.82,0,"ONLY");
  gMC->Gspos("I523",16,"I570",0.0,0.0,-11.7297+0.82,0,"ONLY");
  gMC->Gspos("I523",17,"I570",0.0,0.0,-15.6397+0.82,0,"ONLY");
  gMC->Gspos("I523",18,"I570",0.0,0.0,-19.5497+0.82,0,"ONLY");
  gMC->Gspos("I523",19,"I570",0.0,0.0,-23.4597+0.82,0,"ONLY");
  gMC->Gspos("I523",20,"I570",0.0,0.0,-27.3697+0.82,0,"ONLY");
  gMC->Gspos("I523",21,"I570",0.0,0.0,-31.2797+0.82,0,"ONLY");
  gMC->Gspos("I523",22,"I570",0.0,0.0,-35.1897+0.82,0,"ONLY");
  gMC->Gspos("I523",23,"I570",0.0,0.0,-39.0997+0.82,0,"ONLY");
  gMC->Gspos("I523",24,"I570",0.0,0.0,-43.0097+0.82,0,"ONLY");
  gMC->Gspos("I523",25,"I570",0.0,0.0,-46.9197+0.82,0,"ONLY");
  gMC->Gspos("I553",2,"I570",-0.005,0.0,-51.2047,idrotm[570],"ONLY");
  gMC->Gspos("I566",1,"I569",0.0,-0.03,46.9203,idrotm[532],"ONLY");
  gMC->Gspos("I566",2,"I569",0.0,0.03,43.0103,0,"ONLY");
  gMC->Gspos("I566",3,"I569",0.0,-0.03,39.1003,idrotm[532],"ONLY");
  gMC->Gspos("I566",4,"I569",0.0,0.03,35.1903,0,"ONLY");
  gMC->Gspos("I566",5,"I569",0.0,-0.03,31.2803,idrotm[532],"ONLY");
  gMC->Gspos("I566",6,"I569",0.0,0.03,27.3703,0,"ONLY");
  gMC->Gspos("I566",7,"I569",0.0,-0.03,23.4603,idrotm[532],"ONLY");
  gMC->Gspos("I566",8,"I569",0.0,0.03,19.5503,0,"ONLY");
  gMC->Gspos("I566",9,"I569",0.0,-0.03,15.6403,idrotm[532],"ONLY");
  gMC->Gspos("I566",10,"I569",0.0,0.03,11.7303,0,"ONLY");
  gMC->Gspos("I566",11,"I569",0.0,-0.03,7.8203,idrotm[532],"ONLY");
  gMC->Gspos("I566",12,"I569",0.0,0.03,3.9103,0,"ONLY");
  gMC->Gspos("I566",13,"I569",0.0,-0.03,0.0003,0,"ONLY");
  gMC->Gspos("I566",14,"I569",0.0,0.03,-3.9097,0,"ONLY");
  gMC->Gspos("I566",15,"I569",0.0,-0.03,-7.8197,idrotm[532],"ONLY");
  gMC->Gspos("I566",16,"I569",0.0,0.03,-11.7297,0,"ONLY");
  gMC->Gspos("I566",17,"I569",0.0,-0.03,-15.6397,0,"ONLY");
  gMC->Gspos("I566",18,"I569",0.0,0.03,-19.5497,0,"ONLY");
  gMC->Gspos("I566",19,"I569",0.0,-0.03,-23.4597,idrotm[532],"ONLY");
  gMC->Gspos("I566",20,"I569",0.0,0.03,-27.3697,0,"ONLY");
  gMC->Gspos("I566",21,"I569",0.0,-0.03,-31.2797,idrotm[532],"ONLY");
  gMC->Gspos("I566",22,"I569",0.0,0.03,-35.1897,0,"ONLY");
  gMC->Gspos("I566",23,"I569",0.0,-0.03,-39.0997,0,"ONLY");
  gMC->Gspos("I566",24,"I569",0.0,0.03,-43.0097,0,"ONLY");
  gMC->Gspos("I566",25,"I569",0.0,-0.03,-46.9197,idrotm[532],"ONLY");
  gMC->Gspos("I544",1,"I571",0.0101,0.0,43.125,0,"ONLY");
  gMC->Gspos("I516",20,"I571",0.0001,0.0,39.1-1.08,0,"ONLY");
  gMC->Gspos("I516",19,"I571",0.0001,0.0,35.19-1.08,0,"ONLY");
  gMC->Gspos("I516",18,"I571",0.0001,0.0,31.28-1.08,0,"ONLY");
  gMC->Gspos("I516",17,"I571",0.0001,0.0,27.37-1.08,0,"ONLY");
  gMC->Gspos("I516",16,"I571",0.0001,0.0,23.46-1.08,0,"ONLY");
  gMC->Gspos("I516",15,"I571",0.0001,0.0,19.55-1.08,0,"ONLY");
  gMC->Gspos("I516",14,"I571",0.0001,0.0,15.64-1.08,0,"ONLY");
  gMC->Gspos("I516",13,"I571",0.0001,0.0,11.73-1.08,0,"ONLY");
  gMC->Gspos("I516",12,"I571",0.0001,0.0,7.82-1.08,0,"ONLY");
  gMC->Gspos("I516",11,"I571",0.0001,0.0,3.91-1.08,0,"ONLY");
  gMC->Gspos("I516",10,"I571",0.0001,0.0,0.0-1.08,0,"ONLY");
  gMC->Gspos("I516",9,"I571",0.0001,0.0,-3.91-1.08,0,"ONLY");
  gMC->Gspos("I516",8,"I571",0.0001,0.0,-7.82-1.08,0,"ONLY");
  gMC->Gspos("I516",7,"I571",0.0001,0.0,-11.73-1.08,0,"ONLY");
  gMC->Gspos("I516",6,"I571",0.0001,0.0,-15.64-1.08,0,"ONLY");
  gMC->Gspos("I516",5,"I571",0.0001,0.0,-19.55-1.08,0,"ONLY");
  gMC->Gspos("I516",4,"I571",0.0001,0.0,-23.46-1.08,0,"ONLY");
  gMC->Gspos("I516",3,"I571",0.0001,0.0,-27.37-1.08,0,"ONLY");
  gMC->Gspos("I516",2,"I571",0.0001,0.0,-31.28-1.08,0,"ONLY");
  gMC->Gspos("I516",1,"I571",0.0001,0.0,-35.19-1.08,0,"ONLY");
  gMC->Gspos("I544",2,"I571",-0.0099,0.0,-41.375,idrotm[570],"ONLY");
  gMC->Gspos("I562",1,"I565",0.0,0.03,41.1546,0,"ONLY");
  gMC->Gspos("I562",2,"I565",0.0,-0.03,37.2246,0,"ONLY");
  gMC->Gspos("I562",3,"I565",0.0,0.03,33.3146,0,"ONLY");
  gMC->Gspos("I562",4,"I565",0.0,-0.03,29.3846,0,"ONLY");
  gMC->Gspos("I562",5,"I565",0.0,0.03,25.4746,0,"ONLY");
  gMC->Gspos("I562",6,"I565",0.0,-0.03,21.5446,0,"ONLY");
  gMC->Gspos("I562",7,"I565",0.0,0.03,17.6346,0,"ONLY");
  gMC->Gspos("I562",8,"I565",0.0,-0.03,13.7046,0,"ONLY");
  gMC->Gspos("I562",9,"I565",0.0,0.03,9.7946,0,"ONLY");
  gMC->Gspos("I562",10,"I565",0.0,-0.03,5.8645,0,"ONLY");
  gMC->Gspos("I562",11,"I565",0.0,0.03,1.9546,0,"ONLY");
  gMC->Gspos("I562",12,"I565",0.0,-0.03,-1.9754,0,"ONLY");
  gMC->Gspos("I562",13,"I565",0.0,0.03,-5.8855,0,"ONLY");
  gMC->Gspos("I562",14,"I565",0.0,-0.03,-9.8154,0,"ONLY");
  gMC->Gspos("I562",15,"I565",0.0,0.03,-13.7254,0,"ONLY");
  gMC->Gspos("I562",16,"I565",0.0,-0.03,-17.6555,0,"ONLY");
  gMC->Gspos("I562",17,"I565",0.0,0.03,-21.5655,0,"ONLY");
  gMC->Gspos("I562",18,"I565",0.0,-0.03,-25.4954,0,"ONLY");
  gMC->Gspos("I562",19,"I565",0.0,0.03,-29.4054,0,"ONLY");
  gMC->Gspos("I562",20,"I565",0.0,-0.03,-33.3354,0,"ONLY");
  gMC->Gspos("I562",21,"I565",0.0,0.03,-37.2454,0,"ONLY");
  gMC->Gspos("I562",22,"I565",0.0,-0.03,-41.1554,0,"ONLY");
  gMC->Gspos("I559",1,"I553",2.25,-1.615,0.0,0,"ONLY");
  gMC->Gspos("I560",1,"I553",2.25,-1.615,0.0,0,"ONLY");
  gMC->Gspos("I560",2,"I553",-2.25,-1.615,0.0,idrotm[573],"ONLY");
  gMC->Gspos("I558",1,"I553",-1.7167,-1.52,0.0,idrotm[583],"ONLY");
  gMC->Gspos("I557",1,"I553",-1.8533,-1.341,0.0,idrotm[581],"ONLY");
  gMC->Gspos("I558",2,"I553",1.8367,-1.3122,0.0,idrotm[575],"ONLY");
  gMC->Gspos("I557",2,"I553",1.75,-1.52,0.0,idrotm[583],"ONLY");
  gMC->Gspos("I558",3,"I553",-0.12,1.6613,0.0,idrotm[581],"ONLY");
  gMC->Gspos("I557",3,"I553",0.1034,1.6901,0.0,idrotm[575],"ONLY");
  gMC->Gspos("I556",3,"I553",-1.031,0.2033,-2.203,idrotm[580],"ONLY");
  gMC->Gspos("I556",1,"I553",1.0311,0.2033,-0.287,idrotm[576],"ONLY");
  gMC->Gspos("I554",1,"I553",0.0,-1.58,0.71,0,"ONLY");
  gMC->Gspos("I555",1,"I553",-0.0072,-1.58,-1.2311,idrotm[633],"ONLY");
  gMC->Gspos("I556",2,"I553",1.0311,0.2033,-2.203,idrotm[577],"ONLY");
  gMC->Gspos("I556",4,"I553",-1.031,0.2033,-0.287,idrotm[579],"ONLY");
  gMC->Gspos("I559",2,"I553",-2.25,-1.615,0.0,idrotm[573],"ONLY");
  gMC->Gspos("I561",1,"I553",2.1,-1.615,-0.24,0,"MANY");
  gMC->Gspos("I561",2,"I553",-2.1,-1.615,-0.24,idrotm[573],"MANY");
  gMC->Gspos("I519",37,"I523",0.0001,-1.79,-0.99,idrotm[586],"ONLY");
  gMC->Gspos("I519",36,"I523",-3.2986,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",35,"I523",-3.2986,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",34,"I523",-3.2286,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",33,"I523",-3.2286,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",32,"I523",-3.1586,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",31,"I523",-3.1586,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",30,"I523",-1.3436,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",29,"I523",-1.3436,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",28,"I523",-1.2736,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",27,"I523",-1.2736,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",26,"I523",-1.2036,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",25,"I523",-1.2036,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",24,"I523",-1.0458,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",23,"I523",-1.0458,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",22,"I523",-0.9758,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",21,"I523",-0.9758,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",20,"I523",-0.9058,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",19,"I523",-0.9058,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",18,"I523",0.9092,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",17,"I523",0.9092,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",16,"I523",0.9792,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",15,"I523",0.9792,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",14,"I523",1.0492,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",13,"I523",1.0492,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",12,"I523",1.207,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",11,"I523",1.207,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",10,"I523",1.277,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",9,"I523",1.277,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",8,"I523",1.347,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",7,"I523",1.347,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",6,"I523",3.162,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I519",5,"I523",3.162,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",4,"I523",3.232,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I519",3,"I523",3.232,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I521",12,"I523",-2.8209,-1.7925,-0.982,0,"ONLY");
  gMC->Gspos("I521",11,"I523",-1.6895,-1.7925,-0.982,0,"ONLY");
  gMC->Gspos("I521",10,"I523",-0.5631,-1.7925,-0.982,0,"ONLY");
  gMC->Gspos("I521",9,"I523",0.5633,-1.7925,-0.982,0,"ONLY");
  gMC->Gspos("I521",8,"I523",1.6861,-1.7925,-0.982,0,"ONLY");
  gMC->Gspos("I521",7,"I523",2.8161,-1.7925,-0.982,0,"ONLY");
  gMC->Gspos("I519",2,"I523",3.302,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I520",3,"I523",0.0001,-1.845,-1.19,0,"ONLY");
  gMC->Gspos("I520",2,"I523",-2.2499,-1.845,-1.19,0,"ONLY");
  gMC->Gspos("I521",6,"I523",-2.8209,-1.7075,-0.982,0,"ONLY");
  gMC->Gspos("I521",5,"I523",-1.6895,-1.7075,-0.982,0,"ONLY");
  gMC->Gspos("I521",4,"I523",-0.5631,-1.7075,-0.982,0,"ONLY");
  gMC->Gspos("I521",3,"I523",0.5633,-1.7075,-0.982,0,"ONLY");
  gMC->Gspos("I521",2,"I523",1.6861,-1.7075,-0.982,0,"ONLY");
  gMC->Gspos("I518",1,"I523",0.0001,-1.75,-1.065,0,"ONLY");
  gMC->Gspos("I519",1,"I523",3.302,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I520",1,"I523",2.2501,-1.845,-1.19,0,"ONLY");
  gMC->Gspos("I521",1,"I523",2.8161,-1.7075,-0.982,0,"ONLY");
  gMC->Gspos("I522",1,"I523",2.2501,-1.655,-1.3,idrotm[583],"MANY");
  gMC->Gspos("I522",2,"I523",-2.2499,-1.655,-1.3,idrotm[583],"MANY");
  gMC->Gspos("I542",2,"I523",-2.2499,-1.615,0.0,idrotm[573],"ONLY");
  gMC->Gspos("I541",2,"I523",-2.2499,-1.615,0.0,idrotm[573],"ONLY");
  gMC->Gspos("I541",1,"I523",2.2501,-1.615,0.0,0,"ONLY");
  gMC->Gspos("I542",1,"I523",2.2501,-1.615,0.0,0,"ONLY");
  gMC->Gspos("I543",1,"I523",2.1001,-1.615,0.955,0,"MANY");
  gMC->Gspos("I543",2,"I523",-2.0999,-1.615,0.955,idrotm[573],"MANY");
  gMC->Gspos("I537",2,"I523",1.7501,-1.52,0.0,idrotm[583],"ONLY");
  gMC->Gspos("I538",2,"I523",1.8368,-1.3122,0.0,idrotm[575],"ONLY");
  gMC->Gspos("I537",3,"I523",0.1035,1.6901,0.0,idrotm[575],"ONLY");
  gMC->Gspos("I538",3,"I523",-0.1199,1.6612,0.0,idrotm[581],"ONLY");
  gMC->Gspos("I538",1,"I523",-1.7166,-1.52,0.0,idrotm[583],"ONLY");
  gMC->Gspos("I537",1,"I523",-1.8532,-1.341,0.0,idrotm[581],"ONLY");
  gMC->Gspos("I536",3,"I523",-1.031,0.2033,-1.008,idrotm[580],"ONLY");
  gMC->Gspos("I536",4,"I523",-1.031,0.2033,0.908,idrotm[579],"ONLY");
  gMC->Gspos("I535",1,"I523",-0.0072,-1.58,-0.0361,idrotm[633],"ONLY");
  gMC->Gspos("I536",2,"I523",1.0312,0.2033,-1.008,idrotm[577],"ONLY");
  gMC->Gspos("I536",1,"I523",1.0312,0.2033,0.908,idrotm[576],"ONLY");
  gMC->Gspos("I534",1,"I523",0.0001,-1.58,1.905,0,"ONLY");
  gMC->Gspos("I540",1,"I523",0.0001,-1.785,1.905,idrotm[571],"ONLY");
  gMC->Gspos("I539",1,"I523",1.8001,-1.75,-0.195,idrotm[571],"ONLY");
  gMC->Gspos("I539",2,"I523",-1.7999,-1.75,-0.195,idrotm[572],"ONLY");
  gMC->Gspos("ITS6",1,"I566",0.0,0.0,0.0,0,"ONLY");
  gMC->Gspos("I550",1,"I544",2.25,-1.615,0.0,0,"ONLY");
  gMC->Gspos("I551",1,"I544",2.25,-1.615,0.0,0,"ONLY");
  gMC->Gspos("I551",2,"I544",-2.25,-1.615,0.0,idrotm[573],"ONLY");
  gMC->Gspos("I550",2,"I544",-2.25,-1.615,0.0,idrotm[573],"ONLY");
  gMC->Gspos("I549",1,"I544",1.7167,-1.52,0.0,idrotm[583],"ONLY");
  gMC->Gspos("I548",1,"I544",1.8533,-1.341,0.0,idrotm[575],"ONLY");
  gMC->Gspos("I547",1,"I544",1.0311,0.2033,-0.287,idrotm[576],"ONLY");
  gMC->Gspos("I545",1,"I544",0.0,-1.58,0.71,0,"ONLY");
  gMC->Gspos("I547",2,"I544",1.0311,0.2033,-2.203,idrotm[577],"ONLY");
  gMC->Gspos("I546",1,"I544",-0.0073,-1.58,-1.2311,idrotm[633],"ONLY");
  gMC->Gspos("I547",4,"I544",-1.0311,0.2033,-0.287,idrotm[579],"ONLY");
  gMC->Gspos("I547",3,"I544",-1.0311,0.2033,-2.203,idrotm[580],"ONLY");
  gMC->Gspos("I548",2,"I544",-0.1033,1.6901,0.0,idrotm[581],"O]NLY");
  gMC->Gspos("I549",2,"I544",0.12,1.6613,0.0,idrotm[575],"ONLY");
  gMC->Gspos("I549",3,"I544",-1.8367,-1.3122,0.0,idrotm[581],"ONLY");
  gMC->Gspos("I548",3,"I544",-1.75,-1.52,0.0,idrotm[583],"ONLY");
  gMC->Gspos("I552",1,"I544",2.1,-1.615,-0.24,0,"MANY");
  gMC->Gspos("I552",2,"I544",-2.1,-1.615,-0.24,idrotm[573],"MANY");
  gMC->Gspos("I515",12,"I516",-1.6896,-1.7075,-0.9822,0,"ONLY");
  gMC->Gspos("I515",11,"I516",-1.6896,-1.7925,-0.9822,0,"ONLY");
  gMC->Gspos("I513",37,"I516",0.0,-1.79,-1.035,idrotm[586],"ONLY");
  gMC->Gspos("I513",1,"I516",-3.2987,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I515",1,"I516",-2.816,-1.7075,-0.9822,0,"ONLY");
  gMC->Gspos("I514",1,"I516",-2.25,-1.845,-1.19,0,"ONLY");
  gMC->Gspos("I514",2,"I516",0.0,-1.845,-1.19,0,"ONLY");
  gMC->Gspos("I514",3,"I516",2.25,-1.845,-1.19,0,"ONLY");
  gMC->Gspos("I515",2,"I516",-2.816,-1.7925,-0.9822,0,"ONLY");
  gMC->Gspos("I513",2,"I516",-3.2987,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I515",3,"I516",-0.5632,-1.7075,-0.9822,0,"ONLY");
  gMC->Gspos("I515",4,"I516",-0.5632,-1.7925,-0.9822,0,"ONLY");
  gMC->Gspos("I515",5,"I516",0.5632,-1.7925,-0.9822,0,"ONLY");
  gMC->Gspos("I515",6,"I516",0.5632,-1.7075,-0.9822,0,"ONLY");
  gMC->Gspos("I515",7,"I516",1.6896,-1.7075,-0.9822,0,"ONLY");
  gMC->Gspos("I515",8,"I516",1.6896,-1.7925,-0.9822,0,"ONLY");
  gMC->Gspos("I515",9,"I516",2.816,-1.7925,-0.9822,0,"ONLY");
  gMC->Gspos("I515",10,"I516",2.816,-1.7075,-0.9822,0,"ONLY");
  gMC->Gspos("I513",3,"I516",-3.2287,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",4,"I516",-3.2287,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",5,"I516",-3.1587,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",6,"I516",-3.1587,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",7,"I516",-1.3437,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",8,"I516",-1.3437,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",9,"I516",-1.2737,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",10,"I516",-1.2737,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",11,"I516",-1.2037,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",12,"I516",-1.2037,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",13,"I516",-1.046,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",14,"I516",-1.046,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",15,"I516",-0.976,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",16,"I516",-0.976,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",17,"I516",-0.906,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",18,"I516",-0.906,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",19,"I516",0.9091,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",20,"I516",0.9091,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",21,"I516",0.9791,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",22,"I516",0.9791,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",23,"I516",1.0491,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",24,"I516",1.0491,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",25,"I516",1.2068,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",26,"I516",1.2068,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",27,"I516",1.2768,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",28,"I516",1.2768,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",29,"I516",1.3469,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",30,"I516",1.3469,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",31,"I516",3.1619,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",32,"I516",3.1619,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",33,"I516",3.2319,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I513",34,"I516",3.2319,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",35,"I516",3.3019,-1.79,-1.2943,0,"ONLY");
  gMC->Gspos("I513",36,"I516",3.3019,-1.71,-1.2943,0,"ONLY");
  gMC->Gspos("I512",1,"I516",0.0,-1.75,-1.065,0,"ONLY");
  gMC->Gspos("I528",1,"I516",1.7167,-1.52,0.0,idrotm[583],"ONLY");
  gMC->Gspos("I527",1,"I516",1.8534,-1.341,0.0,idrotm[575],"ONLY");
  gMC->Gspos("I528",2,"I516",0.12,1.6613,0.0,idrotm[575],"ONLY");
  gMC->Gspos("I527",2,"I516",-0.1033,1.6901,0.0,idrotm[581],"ONLY");
  gMC->Gspos("I527",3,"I516",-1.75,-1.52,0.0,idrotm[583],"ONLY");
  gMC->Gspos("I528",3,"I516",-1.8367,-1.3122,0.0,idrotm[581],"ONLY");
  gMC->Gspos("I526",2,"I516",1.0311,0.2033,-1.008,idrotm[577],"ONLY");
  gMC->Gspos("I525",1,"I516",-0.0073,-1.58,-0.0361,idrotm[633],"ONLY");
  gMC->Gspos("I524",1,"I516",0.0,-1.58,1.905,0,"ONLY");
  gMC->Gspos("I526",1,"I516",1.0311,0.2033,0.908,idrotm[576],"ONLY");
  gMC->Gspos("I526",3,"I516",-1.0311,0.2033,0.908,idrotm[579],"ONLY");
  gMC->Gspos("I526",4,"I516",-1.0311,0.2033,-1.008,idrotm[580],"ONLY");
  gMC->Gspos("I529",1,"I516",1.8,-1.75,-0.195,idrotm[571],"ONLY");
  gMC->Gspos("I530",1,"I516",0.0,-1.785,1.905,idrotm[571],"ONLY");
  gMC->Gspos("I529",2,"I516",-1.8,-1.75,-0.195,idrotm[572],"ONLY");
  gMC->Gspos("I517",1,"I516",2.25,-1.655,-1.3,idrotm[583],"MANY");
  gMC->Gspos("I517",2,"I516",-2.25,-1.655,-1.3,idrotm[584],"MANY");
  gMC->Gspos("I531",2,"I516",-2.25,-1.615,0.0,idrotm[573],"ONLY");
  gMC->Gspos("I531",1,"I516",2.25,-1.615,0.0,0,"ONLY");
  gMC->Gspos("I532",1,"I516",2.25,-1.615,0.0,0,"ONLY");
  gMC->Gspos("I532",2,"I516",-2.25,-1.615,0.0,idrotm[573],"ONLY");
  gMC->Gspos("I533",1,"I516",2.1,-1.615,0.955,0,"MANY");
  gMC->Gspos("I533",2,"I516",-2.1,-1.615,0.955,idrotm[573],"MANY");
  gMC->Gspos("ITS5",1,"I562",0.0,0.0,0.0,0,"ONLY");

  
  // --- Place volumes of shield between SPD and SDD 


  gMC->Gspos("IC01",1,"ITSD",0.0,0.0,0.0,0,"ONLY");
  gMC->Gspos("IC02",1,"ITSD",0.0,0.0,25.+8.75,0,"ONLY");  
  gMC->Gspos("IC02",2,"ITSD",0.0,0.0,-25.-8.75,idrotm[200],"ONLY");  
  //gMC->Gspos("IC03",1,"ITSD",0.0,0.0,25.+17.5+7.875,0,"ONLY");  
  //gMC->Gspos("IC03",2,"ITSD",0.0,0.0,-25.-17.5-7.875,idrotm[200],"ONLY");   
  
  
  // --- Place volumes of cylinders between SPD and SDD and SDD and SSD 
  
  gMC->Gspos("ICY1",1,"IS02",0.0,0.0,0.,0,"ONLY");    
  gMC->Gspos("ICY2",1,"IS01",0.0,0.0,0.,0,"ONLY");    
  

  // --- Place volumes of SDD cone ---------------------------------- 
  
  
  gMC->Gspos("I093",1,"IS02",0.0,0.0,0.0,0,"MANY");
  gMC->Gspos("I093",2,"IS02",0.0,0.0,0.0,idrotm[856],"MANY");
  gMC->Gspos("I099",4,"IS02",0.0,0.0,0.0,idrotm[857],"MANY");
  gMC->Gspos("I099",3,"IS02",0.0,0.0,0.0,idrotm[858],"MANY");
  gMC->Gspos("I099",5,"IS02",0.0,0.0,0.0,idrotm[859],"MANY");
  gMC->Gspos("I099",6,"IS02",0.0,0.0,0.0,idrotm[860],"MANY");
  gMC->Gspos("I099",7,"IS02",0.0,0.0,0.0,idrotm[861],"MANY");
  gMC->Gspos("I099",2,"IS02",0.0,0.0,0.0,idrotm[862],"MANY");
  gMC->Gspos("I200",4,"IS02",0.0,0.0,0.0,idrotm[863],"MANY");
  gMC->Gspos("I200",3,"IS02",0.0,0.0,0.0,idrotm[864],"MANY");
  gMC->Gspos("I200",2,"IS02",0.0,0.0,0.0,idrotm[865],"MANY");
  gMC->Gspos("I200",13,"IS02",0.0,0.0,0.0,idrotm[867],"MANY");
  gMC->Gspos("I200",12,"IS02",0.0,0.0,0.0,idrotm[869],"MANY");
  gMC->Gspos("I200",11,"IS02",0.0,0.0,0.0,idrotm[870],"MANY");
  gMC->Gspos("I200",10,"IS02",0.0,0.0,0.0,idrotm[871],"MANY");
  gMC->Gspos("I200",9,"IS02",0.0,0.0,0.0,idrotm[872],"MANY");
  gMC->Gspos("I200",8,"IS02",0.0,0.0,0.0,idrotm[873],"MANY");
  gMC->Gspos("I200",7,"IS02",0.0,0.0,0.0,idrotm[874],"MANY");
  gMC->Gspos("I200",6,"IS02",0.0,0.0,0.0,idrotm[875],"MANY");
  gMC->Gspos("I200",5,"IS02",0.0,0.0,0.0,idrotm[876],"MANY");
  gMC->Gspos("I090",2,"IS02",0.0,0.0,-39.4,0,"ONLY");    
  gMC->Gspos("I090",1,"IS02",0.0,0.0,39.4,idrotm[856],"ONLY");  
  gMC->Gspos("I099",9,"IS02",0.0,0.0,0.0,idrotm[877],"ONLY");
  gMC->Gspos("I099",8,"IS02",0.0,0.0,0.0,idrotm[879],"ONLY");
  gMC->Gspos("I099",1,"IS02",0.0,0.0,0.0,idrotm[880],"ONLY");
  gMC->Gspos("I099",12,"IS02",0.0,0.0,0.0,idrotm[881],"ONLY");
  gMC->Gspos("I099",11,"IS02",0.0,0.0,0.0,idrotm[851],"ONLY");
  gMC->Gspos("I099",10,"IS02",0.0,0.0,0.0,idrotm[882],"ONLY");
  gMC->Gspos("I200",23,"IS02",0.0,0.0,0.0,idrotm[898],"ONLY");
  gMC->Gspos("I200",24,"IS02",0.0,0.0,0.0,idrotm[883],"ONLY");
  gMC->Gspos("I200",1,"IS02",0.0,0.0,0.0,idrotm[884],"ONLY");
  gMC->Gspos("I200",14,"IS02",0.0,0.0,0.0,idrotm[885],"ONLY");
  gMC->Gspos("I200",15,"IS02",0.0,0.0,0.0,idrotm[887],"ONLY");
  gMC->Gspos("I200",16,"IS02",0.0,0.0,0.0,idrotm[888],"ONLY");
  gMC->Gspos("I200",17,"IS02",0.0,0.0,0.0,idrotm[889],"ONLY");
  gMC->Gspos("I200",18,"IS02",0.0,0.0,0.0,idrotm[890],"ONLY");
  gMC->Gspos("I200",22,"IS02",0.0,0.0,0.0,idrotm[891],"ONLY");
  gMC->Gspos("I200",21,"IS02",0.0,0.0,0.0,idrotm[892],"ONLY");
  gMC->Gspos("I200",20,"IS02",0.0,0.0,0.0,idrotm[868],"ONLY");
  gMC->Gspos("I200",19,"IS02",0.0,0.0,0.0,idrotm[893],"ONLY");
  gMC->Gspos("I098",1,"IS02",0.0,0.0,33.6,0,"ONLY");    
  gMC->Gspos("I097",1,"IS02",0.0,0.0,26.6,0,"ONLY");    
  gMC->Gspos("I097",2,"IS02",0.0,0.0,-26.6,idrotm[856],"ONLY");  
  gMC->Gspos("I098",2,"IS02",0.0,0.0,-33.6,idrotm[856],"ONLY");  
  gMC->Gspos("I202",1,"IS02",12.1,0.0,33.84,0,"ONLY");
  gMC->Gspos("I202",6,"IS02",-6.05,-10.4789,33.84,idrotm[930],"ONLY");
  gMC->Gspos("I202",5,"IS02",-6.05,10.4789,33.84,idrotm[929],"ONLY");
  gMC->Gspos("I202",2,"IS02",12.1,0.0,-33.84,idrotm[856],"ONLY");
  gMC->Gspos("I202",3,"IS02",-6.05,10.4789,-33.84,idrotm[932],"ONLY");
  gMC->Gspos("I202",4,"IS02",-6.05,-10.4789,-33.84,idrotm[934],"ONLY");
  gMC->Gspos("I203",12,"IS02",21.8453,0.0,-42.24,idrotm[856],"ONLY");
  gMC->Gspos("I203",11,"IS02",10.9227,-18.9186,-42.24,idrotm[935],"ONLY");
  gMC->Gspos("I203",10,"IS02",10.9227,-18.9186,42.24,idrotm[846],"ONLY");
  gMC->Gspos("I203",9,"IS02",-10.9227,-18.9186,-42.24,idrotm[934],"ONLY");
  gMC->Gspos("I203",8,"IS02",-10.9227,-18.9186,42.24,idrotm[930],"ONLY");
  gMC->Gspos("I203",7,"IS02",-21.8453,0.0,-42.24,idrotm[933],"ONLY");
  gMC->Gspos("I203",6,"IS02",-21.8453,0.0,42.24,idrotm[878],"ONLY");
  gMC->Gspos("I203",5,"IS02",-10.9227,18.9186,-42.24,idrotm[932],"ONLY");
  gMC->Gspos("I203",4,"IS02",-10.9227,18.9186,42.24,idrotm[929],"ONLY");
  gMC->Gspos("I203",3,"IS02",10.9227,18.9186,-42.24,idrotm[931],"ONLY");
  gMC->Gspos("I203",2,"IS02",10.9227,18.9186,42.24,idrotm[853],"ONLY");
  gMC->Gspos("I203",1,"IS02",21.8453,0.0,42.24,0,"ONLY");
  gMC->Gspos("I095",1,"I098",0.0,0.0,0.0,0,"ONLY");
  gMC->Gspos("I096",23,"I098",22.77,0.0,0.0,idrotm[894],"MANY");
  gMC->Gspos("I096",14,"I098",22.3754,6.57,0.0,idrotm[895],"MANY");
  gMC->Gspos("I096",3,"I098",19.1553,12.3104,0.0,idrotm[896],"MANY");
  gMC->Gspos("I096",16,"I098",15.2714,17.6241,0.0,idrotm[897],"MANY");
  gMC->Gspos("I096",5,"I098",9.459,20.7123,0.0,idrotm[899],"MANY");
  gMC->Gspos("I096",18,"I098",3.3188,23.0826,0.0,idrotm[900],"MANY");
  gMC->Gspos("I096",7,"I098",-3.2405,22.5382,0.0,idrotm[901],"MANY");
  gMC->Gspos("I096",20,"I098",-9.6875,21.2126,0.0,idrotm[902],"MANY");
  gMC->Gspos("I096",9,"I098",-14.9112,17.2084,0.0,idrotm[903],"MANY");
  gMC->Gspos("I096",22,"I098",-19.618,12.6077,0.0,idrotm[904],"MANY");
  gMC->Gspos("I096",11,"I098",-21.8477,6.4151,0.0,idrotm[905],"MANY");
  gMC->Gspos("I096",24,"I098",-23.32,0.0,0.0,idrotm[906],"MANY");
  gMC->Gspos("I096",13,"I098",-21.8477,-6.4151,0.0,idrotm[907],"MANY");
  gMC->Gspos("I096",4,"I098",-19.618,-12.6077,0.0,idrotm[908],"MANY");
  gMC->Gspos("I096",15,"I098",-14.9112,-17.2084,0.0,idrotm[909],"MANY");
  gMC->Gspos("I096",6,"I098",-9.6875,-21.2126,0.0,idrotm[910],"MANY");
  gMC->Gspos("I096",17,"I098",-3.2405,-22.5382,0.0,idrotm[911],"MANY");
  gMC->Gspos("I096",8,"I098",3.3188,-23.0826,0.0,idrotm[912],"MANY");
  gMC->Gspos("I096",19,"I098",9.459,-20.7123,0.0,idrotm[913],"MANY");
  gMC->Gspos("I096",10,"I098",15.2714,-17.6241,0.0,idrotm[914],"MANY");
  gMC->Gspos("I096",21,"I098",19.1553,-12.3104,0.0,idrotm[915],"MANY");
  gMC->Gspos("I096",12,"I098",22.3754,-6.57,0.0,idrotm[916],"MANY");
  gMC->Gspos("I094",1,"I097",0.0,0.0,0.0,0,"ONLY");
  gMC->Gspos("I096",1,"I097",13.87,0.0,0.0,idrotm[894],"MANY");
  gMC->Gspos("I096",32,"I097",13.037,6.2783,0.0,idrotm[917],"MANY");
  gMC->Gspos("I096",25,"I097",8.6478,10.844,0.0,idrotm[918],"MANY");
  gMC->Gspos("I096",34,"I097",3.2199,14.1072,0.0,idrotm[919],"MANY");
  gMC->Gspos("I096",27,"I097",-3.0864,13.5223,0.0,idrotm[920],"MANY");
  gMC->Gspos("I096",36,"I097",-9.0219,11.3131,0.0,idrotm[921],"MANY");
  gMC->Gspos("I096",29,"I097",-12.4964,6.018,0.0,idrotm[922],"MANY");
  gMC->Gspos("I096",2,"I097",-14.47,0.0,0.0,idrotm[906],"MANY");
  gMC->Gspos("I096",31,"I097",-12.4964,-6.018,0.0,idrotm[923],"MANY");
  gMC->Gspos("I096",26,"I097",-9.0219,-11.3131,0.0,idrotm[924],"MANY");
  gMC->Gspos("I096",33,"I097",-3.0864,-13.5223,0.0,idrotm[925],"MANY");
  gMC->Gspos("I096",28,"I097",3.2199,-14.1072,0.0,idrotm[926],"MANY");
  gMC->Gspos("I096",35,"I097",8.6478,-10.844,0.0,idrotm[927],"MANY");
  gMC->Gspos("I096",30,"I097",13.037,-6.2783,0.0,idrotm[928],"MANY");
  
  
  // --- Place volumes of SSD cone ----------------------------------    

    
  gMC->Gspos("I212",2,"IS01",0.0,0.0,0.0,idrotm[701],"MANY");
  gMC->Gspos("I212",1,"IS01",0.0,0.0,0.0,0,"MANY");
  gMC->Gspos("I211",1,"IS01",0.0,0.0,-56.5,0,"ONLY");
  gMC->Gspos("I217",1,"IS01",0.0,0.0,-44.4,0,"ONLY");             // this will change after PPR to be symmetric
  gMC->Gspos("I219",1,"IS01",0.0,0.0,-50.25,0,"ONLY");            // this will change after PPR to be symmetric
  gMC->Gspos("I211",2,"IS01",0.0,0.0,56.5,idrotm[701],"ONLY");   
  gMC->Gspos("I219",2,"IS01",0.0,0.0,51.65,idrotm[701],"ONLY");   // this will change after PPR to be symmetric
  gMC->Gspos("I217",2,"IS01",0.0,0.0,45.8,idrotm[701],"ONLY");    // this will change after PPR to be symmetric
  gMC->Gspos("I214",2,"IS01",0.0,0.0,67.25,idrotm[701],"ONLY");   
  gMC->Gspos("I213",2,"IS01",0.0,0.0,62.25,idrotm[701],"ONLY");  
  gMC->Gspos("I213",1,"IS01",0.0,0.0,-62.25,0,"ONLY");             
  gMC->Gspos("I214",1,"IS01",0.0,0.0,-67.25,0,"ONLY");           
  gMC->Gspos("I215",19,"IS01",0.0,0.0,0.0,idrotm[702],"MANY");
  gMC->Gspos("I215",21,"IS01",0.0,0.0,0.0,idrotm[703],"MANY");
  gMC->Gspos("I215",23,"IS01",0.0,0.0,0.0,idrotm[704],"MANY");
  gMC->Gspos("I215",24,"IS01",0.0,0.0,0.0,idrotm[705],"MANY");
  gMC->Gspos("I215",3,"IS01",0.0,0.0,0.0,idrotm[706],"MANY");
  gMC->Gspos("I215",5,"IS01",0.0,0.0,0.0,idrotm[707],"MANY");
  gMC->Gspos("I215",7,"IS01",0.0,0.0,0.0,idrotm[708],"MANY");
  gMC->Gspos("I215",9,"IS01",0.0,0.0,0.0,idrotm[709],"MANY");
  gMC->Gspos("I215",11,"IS01",0.0,0.0,0.0,idrotm[710],"MANY");
  gMC->Gspos("I215",13,"IS01",0.0,0.0,0.0,idrotm[711],"MANY");
  gMC->Gspos("I215",15,"IS01",0.0,0.0,0.0,idrotm[712],"MANY");
  gMC->Gspos("I215",17,"IS01",0.0,0.0,0.0,idrotm[713],"MANY");
  gMC->Gspos("I216",9,"IS01",0.0,0.0,45.5,idrotm[714],"ONLY");
  gMC->Gspos("I216",11,"IS01",0.0,0.0,45.5,idrotm[715],"ONLY");
  gMC->Gspos("I216",12,"IS01",0.0,0.0,45.5,idrotm[716],"ONLY");
  gMC->Gspos("I216",3,"IS01",0.0,0.0,45.5,idrotm[717],"ONLY");
  gMC->Gspos("I216",5,"IS01",0.0,0.0,45.5,idrotm[718],"ONLY");
  gMC->Gspos("I216",7,"IS01",0.0,0.0,45.5,idrotm[719],"ONLY");
  gMC->Gspos("I216",10,"IS01",0.0,0.0,-44,idrotm[720],"ONLY");
  gMC->Gspos("I216",1,"IS01",0.0,0.0,-44,idrotm[721],"ONLY");
  gMC->Gspos("I216",2,"IS01",0.0,0.0,-44,idrotm[722],"ONLY");
  gMC->Gspos("I216",4,"IS01",0.0,0.0,-44,idrotm[723],"ONLY");
  gMC->Gspos("I216",6,"IS01",0.0,0.0,-44,idrotm[724],"ONLY");
  gMC->Gspos("I216",8,"IS01",0.0,0.0,-44,idrotm[725],"ONLY");
  gMC->Gspos("I215",1,"IS01",0.0,0.0,0.0,idrotm[726],"MANY");
  gMC->Gspos("I215",2,"IS01",0.0,0.0,0.0,idrotm[727],"MANY");
  gMC->Gspos("I215",4,"IS01",0.0,0.0,0.0,idrotm[728],"MANY");
  gMC->Gspos("I215",6,"IS01",0.0,0.0,0.0,idrotm[729],"MANY");
  gMC->Gspos("I215",8,"IS01",0.0,0.0,0.0,idrotm[733],"MANY");
  gMC->Gspos("I215",10,"IS01",0.0,0.0,0.0,idrotm[730],"MANY");
  gMC->Gspos("I215",12,"IS01",0.0,0.0,0.0,idrotm[731],"MANY");
  gMC->Gspos("I215",14,"IS01",0.0,0.0,0.0,idrotm[768],"MANY");
  gMC->Gspos("I215",16,"IS01",0.0,0.0,0.0,idrotm[732],"MANY");
  gMC->Gspos("I215",18,"IS01",0.0,0.0,0.0,idrotm[734],"MANY");
  gMC->Gspos("I215",20,"IS01",0.0,0.0,0.0,idrotm[798],"MANY");
  gMC->Gspos("I215",22,"IS01",0.0,0.0,0.0,idrotm[735],"MANY");
           
	    	    
  // --- Place subdetectors' mother volumes and supports' mother volumes
  //     into ITS mother volume ITSD
    
  gMC->Gspos("IT12",1,"ITSD",0.0,0.0,0.0,0,"ONLY");  // SPD mother volume
  gMC->Gspos("IT34",1,"ITSD",0.0,0.0,0.0,0,"ONLY");  // SDD mother volume
  gMC->Gspos("IT56",1,"ITSD",0.0,0.0,0.0,0,"ONLY");  // SSD mother volume
  gMC->Gspos("IS02",1,"ITSD",0.0,0.0,0.0,0,"ONLY");  // SDD cones/supports
  gMC->Gspos("IS01",1,"ITSD",0.0,0.0,0.0,0,"ONLY");  // SSD cones/supports
        

  // ****************************  SERVICES  *********************************

   // --- DEFINE CABLES AT THE END OF THE ITS CONES - COPPER PART
  //     UPPER PART

  dgh[0] = 46.;    
  dgh[1] = 46.+1.0;  
  dgh[2] = 9.5;
  dgh[3] = 12.;
  dgh[4] = 168.;
  
  gMC->Gsvolu("I1CU", "TUBS", idtmed[213], dgh, 5);  
  gMC->Gspos("I1CU", 1, "ITSV", 0., 0., 83.5, 0, "ONLY");
  gMC->Gspos("I1CU", 2, "ITSV", 0., 0., -83.5, idrotm[200], "ONLY");
  
  // --- DEFINE CABLES AT THE END OF THE ITS CONES - COPPER PART
  //     LOWER PART

  dgh[0] = 46.;    
  dgh[1] = 46.+1.0;  
  dgh[2] = 9.5;
  dgh[3] = 192.;
  dgh[4] = 348.;
  
  gMC->Gsvolu("I2CU", "TUBS", idtmed[213], dgh, 5);  
  gMC->Gspos("I2CU", 1, "ITSV", 0., 0., 83.5, 0, "ONLY");
  gMC->Gspos("I2CU", 2, "ITSV", 0., 0., -83.5, idrotm[200], "ONLY");


  // --- DEFINE CABLES AT THE END OF THE ITS CONES - CARBON PART
  //     UPPER PART
  
  dgh[0] = 46.+1.0;  
  dgh[1] = 46.+1.0+1.5;   
  dgh[2] = 9.5;
  dgh[3] = 12.;
  dgh[4] = 168.;
  
  gMC->Gsvolu("I1CC", "TUBS", idtmed[225], dgh, 5);  
  gMC->Gspos("I1CC", 1, "ITSV", 0., 0., 83.5, 0, "ONLY");
  gMC->Gspos("I1CC", 2, "ITSV", 0., 0., -83.5, idrotm[200], "ONLY");  
  
  // --- DEFINE CABLES AT THE END OF THE ITS CONES - CARBON PART
  //     LOWER PART
  
  dgh[0] = 46.+1.0;  
  dgh[1] = 46.+1.0+1.5;   
  dgh[2] = 9.5;
  dgh[3] = 192.;
  dgh[4] = 348.;
  
  gMC->Gsvolu("I2CC", "TUBS", idtmed[225], dgh, 5);  
  gMC->Gspos("I2CC", 1, "ITSV", 0., 0., 83.5, 0, "ONLY");
  gMC->Gspos("I2CC", 2, "ITSV", 0., 0., -83.5, idrotm[200], "ONLY");  


  // --- DEFINE PATCH PANELS AT THE END OF THE ITS CONES
  //     UPPER PART
  
  dgh[0] = 46.;  
  dgh[1] = 56.;
  dgh[2] = 2.25;
  dgh[3] = 12.;
  dgh[4] = 168.;
  
  gMC->Gsvolu("IPA1", "TUBS", idtmed[210], dgh, 5);  
  gMC->Gspos("IPA1", 1, "ITSV", 0., 0., 95.25, 0, "ONLY");  
  gMC->Gspos("IPA1", 2, "ITSV", 0., 0., -95.25, idrotm[200], "ONLY"); 
  
  // --- DEFINE PATCH PANELS AT THE END OF THE ITS CONES
  //     LOWER PART
  
  dgh[0] = 46.;  
  dgh[1] = 56.;
  dgh[2] = 2.25;
  dgh[3] = 192.;
  dgh[4] = 348.;
  
  gMC->Gsvolu("IPA2", "TUBS", idtmed[210], dgh, 5);  
  gMC->Gspos("IPA2", 1, "ITSV", 0., 0., 95.25, 0, "ONLY");  
  gMC->Gspos("IPA2", 2, "ITSV", 0., 0., -95.25, idrotm[200], "ONLY"); 
 
  
  // --- DEFINE CABLES/COOLING BELOW THE TPC - COPPER PART - UPPER PART
 
  dgh[0] = (ztpc-97.5)/2.;
  dgh[1] = 46.2;
  dgh[2] = 46.2+1.0;
  dgh[3] = 62.3;
  dgh[4] = 62.3+1.0;
  dgh[5] = 12.;    
  dgh[6] = 168.;
  gMC->Gsvolu("ICU1", "CONS", idtmed[213], dgh, 7);    
  gMC->Gspos("ICU1", 1, "ITSV", 0., 0., 97.5+dgh[0], 0, "ONLY");  
  gMC->Gspos("ICU1", 2, "ITSV", 0., 0., -(97.5+dgh[0]), idrotm[200], "ONLY");   
  
  // --- DEFINE CABLES/COOLING BELOW THE TPC - COPPER PART - LOWER PART
  
  dgh[0] = (ztpc-97.5)/2.;
  dgh[1] = 46.2;
  dgh[2] = 46.2+1.0;
  dgh[3] = 62.3;
  dgh[4] = 62.3+1.0;
  dgh[5] = 192.;    
  dgh[6] = 348.;
  gMC->Gsvolu("ICU2", "CONS", idtmed[213], dgh, 7);    
  gMC->Gspos("ICU2", 1, "ITSV", 0., 0., 97.5+dgh[0], 0, "ONLY");  
  gMC->Gspos("ICU2", 2, "ITSV", 0., 0., -(97.5+dgh[0]), idrotm[200], "ONLY");     
  
  // --- DEFINE CABLES/COOLING BELOW THE TPC - CARBON PART - UPPER PART
  
  dgh[0] = (ztpc-97.5)/2.;
  dgh[1] = 46.2+1.0;
  dgh[2] = 46.2+1.0+1.5;
  dgh[3] = 62.3+1.0;
  dgh[4] = 62.3+1.0+1.5;
  dgh[5] = 12.;    
  dgh[6] = 168.;  
  gMC->Gsvolu("ICC1", "CONS", idtmed[225], dgh, 7);    
  gMC->Gspos("ICC1", 1, "ITSV", 0., 0., 97.5+dgh[0], 0, "ONLY");  
  gMC->Gspos("ICC1", 2, "ITSV", 0., 0., -(97.5+dgh[0]), idrotm[200], "ONLY");   
  
  // --- DEFINE CABLES/COOLING BELOW THE TPC - CARBON PART - LOWER PART
  
  dgh[0] = (ztpc-97.5)/2.;
  dgh[1] = 46.2+1.0;
  dgh[2] = 46.2+1.0+1.5;
  dgh[3] = 62.3+1.0;
  dgh[4] = 62.3+1.0+1.5;
  dgh[5] = 192.;    
  dgh[6] = 348.;  
  gMC->Gsvolu("ICC2", "CONS", idtmed[225], dgh, 7);    
  gMC->Gspos("ICC2", 1, "ITSV", 0., 0., 97.5+dgh[0], 0, "ONLY");  
  gMC->Gspos("ICC2", 2, "ITSV", 0., 0., -(97.5+dgh[0]), idrotm[200], "ONLY");     
    
  // --- DEFINE CABLES/COOLING BEHIND THE TPC - COPPER PART - UPPER PART
    
  dgh[0] = 62.1;
  dgh[1] = 74.5;
  dgh[2] = 0.5;
  dgh[3] = 12.;
  dgh[4] = 168.;
  gMC->Gsvolu("ICU3", "TUBS", idtmed[213], dgh, 5);    
  gMC->Gspos("ICU3", 1, "ITSV", 0., 0., ztpc+1.5+dgh[2], 0, "ONLY");  
  gMC->Gspos("ICU3", 2, "ITSV", 0., 0., -(ztpc+1.5+dgh[2]), idrotm[200], "ONLY");      
  
  // --- DEFINE CABLES/COOLING BEHIND THE TPC - COPPER PART - LOWER PART
  
  dgh[0] = 62.1;
  dgh[1] = 74.5;
  dgh[2] = 0.5;
  dgh[3] = 192.;
  dgh[4] = 348.;
  gMC->Gsvolu("ICU4", "TUBS", idtmed[213], dgh, 5);    
  gMC->Gspos("ICU4", 1, "ITSV", 0., 0., ztpc+1.5+dgh[2], 0, "ONLY");  
  gMC->Gspos("ICU4", 2, "ITSV", 0., 0., -(ztpc+1.5+dgh[2]), idrotm[200], "ONLY");      
     
  // --- DEFINE CABLES/COOLING BEHIND THE TPC - CARBON PART - UPPER PART

  dgh[0] = 62.1;
  dgh[1] = 74.5;
  dgh[2] = 0.75;
  dgh[3] = 12.;
  dgh[4] = 168.;
  gMC->Gsvolu("ICC3", "TUBS", idtmed[225], dgh, 5);    
  gMC->Gspos("ICC3", 1, "ITSV", 0., 0., ztpc+dgh[2], 0, "ONLY");  
  gMC->Gspos("ICC3", 2, "ITSV", 0., 0., -(ztpc+dgh[2]), idrotm[200], "ONLY"); 
    
  // --- DEFINE CABLES/COOLING BEHIND THE TPC - CARBON PART - LOWER PART

  dgh[0] = 62.1;
  dgh[1] = 74.5;
  dgh[2] = 0.75;
  dgh[3] = 192.;
  dgh[4] = 348.;
  gMC->Gsvolu("ICC4", "TUBS", idtmed[225], dgh, 5);    
  gMC->Gspos("ICC4", 1, "ITSV", 0., 0., ztpc+dgh[2], 0, "ONLY");  
  gMC->Gspos("ICC4", 2, "ITSV", 0., 0., -(ztpc+dgh[2]), idrotm[200], "ONLY"); 

  // --- DEFINE HOOK TO THE TPC ON OTHER SIDE W.R.T. THE ABSORBER - UPPER PART
  
  dgh[0] = 74.5;
  dgh[1] = 79.5;
  dgh[2] = 2.5;
  dgh[3] = 12.;
  dgh[4] = 168.;
  gMC->Gsvolu("IHK1", "TUBS", idtmed[264], dgh, 5);   
  gMC->Gspos("IHK1", 1, "ITSV", 0., 0., -ztpc-dgh[2], 0, "ONLY");      
  
  // --- DEFINE HOOK TO THE TPC ON OTHER SIDE W.R.T. THE ABSORBER - LOWER PART
  
  dgh[0] = 74.5;
  dgh[1] = 79.5;
  dgh[2] = 2.5;
  dgh[3] = 192.;
  dgh[4] = 348.;
  gMC->Gsvolu("IHK2", "TUBS", idtmed[264], dgh, 5);   
  gMC->Gspos("IHK2", 1, "ITSV", 0., 0., -ztpc-dgh[2], 0, "ONLY");        
  
  // --- DEFINE RAILS BETWEEN THE ITS AND THE TPC
  
  if (rails == 1) {
  
     dgh[0] = 2.;          
     dgh[1] = 8.;           
     dgh[2] = 190.;         
     gMC->Gsvolu("IRA1", "BOX ", idtmed[268], dgh, 3);
     gMC->Gspos("IRA1", 1, "ITSV", 53.5, 0., -69.5, 0, "ONLY");   
     gMC->Gsvolu("IRA2", "BOX ", idtmed[268], dgh, 3);    
     gMC->Gspos("IRA2", 1, "ITSV", -53.5, 0., -69.5, 0, "ONLY");    

     dgh[0] = 2.-0.5;    // 0.5 was determined in such a way that the aluminum area is 20.9 cm^2      
     dgh[1] = 8.-0.5;    // 0.5 was determined in such a way that the aluminum area is 20.9 cm^2       
     dgh[2] = 190.;         
     gMC->Gsvolu("IRA3", "BOX ", idtmed[205], dgh, 3);   
     gMC->Gspos("IRA3", 1, "IRA1", 0., 0., 0., 0, "ONLY");   
     gMC->Gsvolu("IRA4", "BOX ", idtmed[205], dgh, 3);     
     gMC->Gspos("IRA4", 1, "IRA2", 0., 0., 0., 0, "ONLY");    

  }
  
  // --- DEFINE CYLINDERS HOLDING RAILS BETWEEN THE ITS AND THE TPC
  
  dgh[0] = 56.9;    
  dgh[1] = 59.;
  dgh[2] = 0.6;    
  gMC->Gsvolu("ICYL", "TUBE", idtmed[210], dgh, 3);   
  gMC->Gspos("ICYL", 1, "ALIC", 0., 0., 74.1, 0, "ONLY");       
  gMC->Gspos("ICYL", 2, "ALIC", 0., 0., -74.1, idrotm[200], "ONLY");  

  // --- DEFINE SUPPORTS FOR RAILS ATTACHED TO THE CYLINDERS

  dgh[0] = 0.;        
  dgh[1] = 3.;         
  dgh[2] = 5.;  // 5. comes from the fact that the volume has to be 567.6/2 cm^3       
  gMC->Gsvolu("ISR1", "TUBE", idtmed[284], dgh, 3);   
  gMC->Gspos("ISR1", 1, "ITSV", 53.4292, 10.7053, 79.75, 0, "ONLY");    
  gMC->Gspos("ISR1", 2, "ITSV", 53.4292, -10.7053, 79.75, 0, "ONLY");   
  gMC->Gspos("ISR1", 3, "ITSV", -53.4292, 10.7053, 79.75, 0, "ONLY"); 
  gMC->Gspos("ISR1", 4, "ITSV", -53.4292, -10.7053, 79.75, 0, "ONLY");  
  gMC->Gspos("ISR1", 5, "ITSV", 53.4292, 10.7053, -79.75, 0, "ONLY");   
  gMC->Gspos("ISR1", 6, "ITSV", 53.4292, -10.7053, -79.75, 0, "ONLY");   
  gMC->Gspos("ISR1", 7, "ITSV", -53.4292, 10.7053, -79.75, 0, "ONLY"); 
  gMC->Gspos("ISR1", 8, "ITSV", -53.4292, -10.7053, -79.75, 0, "ONLY");        
  
  // --- DEFINE SUPPORTS FOR RAILS ATTACHED TO THE ABSORBER

  dgh[0] = 5.;        
  dgh[1] = 12.;         
  dgh[2] = 5.;         
  gMC->Gsvolu("ISR2", "BOX ", idtmed[210], dgh, 3);   
  gMC->Gspos("ISR2", 1, "ALIC", 53.5, 0., 125.5, 0, "ONLY");
  gMC->Gsvolu("ISR3", "BOX ", idtmed[210], dgh, 3);   
  gMC->Gspos("ISR3", 1, "ALIC", -53.5, 0., 125.5, 0, "ONLY");  
  
  dgh[0] = 5.-2.;        
  dgh[1] = 12.-2.;         
  dgh[2] = 5.;         
  gMC->Gsvolu("ISR4", "BOX ", idtmed[205], dgh, 3);   
  gMC->Gspos("ISR4", 1, "ISR2", 0., 0., 0., 0, "ONLY");     
  gMC->Gsvolu("ISR5", "BOX ", idtmed[205], dgh, 3);   
  gMC->Gspos("ISR5", 1, "ISR3", 0., 0., 0., 0, "ONLY");
  
  // --- DEFINE SUPPORTS TO ATTACH THE ITS TO THE TPC
  
  dgh[0] = 0.;        
  dgh[1] = 5.;         
  dgh[2] = 2.;         
  gMC->Gsvolu("ISR6", "TUBE", idtmed[210], dgh, 3);   
  gMC->Gspos("ISR6", 1, "ALIC", 0., 54., 77., 0, "ONLY"); 
  gMC->Gspos("ISR6", 2, "ALIC", 0., 54., -77., 0, "ONLY"); 
  gMC->Gspos("ISR6", 3, "ALIC", 0., -54., -77., 0, "ONLY");       
  
  // --- Outputs the geometry tree in the EUCLID/CAD format 
  
  if (GetEUCLID()) {
    gMC->WriteEuclid("ITSgeometry", "ITSV", 1, 5);
  }
}
