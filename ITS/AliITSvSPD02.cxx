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
Revision 1.3  2003/03/21 14:34:10  nilsen
Removed warning from part of code not properly implimneted yet.

Revision 1.2  2003/02/12 10:39:05  hristov
Updated AliTrackReference class (S.Radomski)

Revision 1.1  2002/12/05 20:07:25  nilsen
Adding new SPD 2002 test-beam geometry, with Config file (setup for testing,
50 pions in one event and not 50 events with one pion).

*/
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

#include "AliRun.h"
#include "AliMagF.h"
#include "AliConst.h"
#include "AliITSGeant3Geometry.h"
#include "AliTrackReference.h"
#include "AliITShit.h"
#include "AliITS.h"
#include "AliITSvSPD02.h"
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


ClassImp(AliITSvSPD02)

//______________________________________________________________________
AliITSvSPD02::AliITSvSPD02() {
    ////////////////////////////////////////////////////////////////////////
    // Standard default constructor for the ITS SPD test beam 2002 version 1.
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    A default created class.
    ////////////////////////////////////////////////////////////////////////
    Int_t i;

    fIdN          = 0;
    fIdName       = 0;
    fIdSens       = 0;
    fEuclidOut    = kFALSE; // Don't write Euclide file
    fGeomDetOut   = kFALSE; // Don't write .det file
    fGeomDetIn    = kFALSE; // Don't Read .det file
    fMajorVersion = IsVersion();
    fMinorVersion = -1;
    for(i=0;i<60;i++) fRead[i] = '\0';
    for(i=0;i<60;i++) fWrite[i] = '\0';
    for(i=0;i<60;i++) fEuclidGeomDet[i] = '\0';
}
//______________________________________________________________________
AliITSvSPD02::AliITSvSPD02(const char *title) : AliITS("ITS", title){
    ////////////////////////////////////////////////////////////////////////
    //    Standard constructor for the ITS SPD testbeam 2002 version 1.
    // Inputs:
    //    const char *title    title for this ITS geometry.
    // Outputs:
    //    none.
    // Return:
    //    A standard created class.
    ////////////////////////////////////////////////////////////////////////
    Int_t i;

    fIdN = 2;
    fIdName = new TString[fIdN];
    fIdName[0] = "IMBS";
    fIdName[1] = "ITST";
    fIdSens    = new Int_t[fIdN];
    for(i=0;i<fIdN;i++) fIdSens[i] = 0;
    fMajorVersion = IsVersion();
    fMinorVersion = 2;
    fEuclidOut    = kFALSE; // Don't write Euclide file
    fGeomDetOut   = kFALSE; // Don't write .det file
    fGeomDetIn    = kFALSE; // Don't Read .det file
    SetThicknessDet1();
    SetThicknessDet2();
    SetThicknessChip1();
    SetThicknessChip2();	 	 	 

    fEuclidGeometry="$ALICE_ROOT/ITS/ITSgeometry_vSPD022.euc";
    strncpy(fEuclidGeomDet,"$ALICE_ROOT/ITS/ITSgeometry_vSPD022.det",60);
    strncpy(fRead,fEuclidGeomDet,60);
    strncpy(fWrite,fEuclidGeomDet,60);
}
//______________________________________________________________________
AliITSvSPD02::AliITSvSPD02(const AliITSvSPD02 &source){
    ////////////////////////////////////////////////////////////////////////
    //     Copy Constructor for ITS SPD test beam 2002 version 1.
    // This class is not to be copied. Function only dummy.
    // Inputs:
    //    const AliITSvSPD02 &source   The class to be copied
    // Outputs:
    //    none.
    // Return:
    //    A warning message.
    ////////////////////////////////////////////////////////////////////////
    if(&source == this) return;
    Warning("Copy Constructor","Not allowed to copy AliITSvSPD02");
    return;
}
//______________________________________________________________________
AliITSvSPD02& AliITSvSPD02::operator=(const AliITSvSPD02 &source){
    ////////////////////////////////////////////////////////////////////////
    //    Assignment operator for the ITS SPD test beam 2002 version 1.
    // This class is not to be copied. Function only dummy.
    // Inputs:
    //    const AliITSvSPD02 &source   The class to be copied
    // Outputs:
    //    none.
    // Return:
    //    A Warning message
    ////////////////////////////////////////////////////////////////////////
    if(&source == this) return *this;
    Warning("= operator","Not allowed to copy AliITSvSPD02");
    return *this;
}
//______________________________________________________________________
AliITSvSPD02::~AliITSvSPD02() {
    ////////////////////////////////////////////////////////////////////////
    //    Standard destructor for the ITS SPD test beam 2002 version 1.
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    none.
    ////////////////////////////////////////////////////////////////////////
}
//______________________________________________________________________
void AliITSvSPD02::BuildGeometry(){
    ////////////////////////////////////////////////////////////////////////
    //    Geometry builder for the ITS SPD test beam 2002 version 1.
    //    ALIC    ALICE Mother Volume
    //     |- ITSV     ITS Mother Volume
    //         |- IDET       Detector under Test
    //         |   |- ITS0       SPD Si Chip
    //         |   |  |- ITST      SPD Sensitivve Volume
    //         |   |- IPC0 *5    Readout chip
    //         |- ITEL *4    SPD Telescope
    //             |- IMB0       SPD Si Chip
    //             |   |- IMBS     SPD Sensitive volume
    //             |- ICMB       Chip MiniBus.
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    none.
    ////////////////////////////////////////////////////////////////////////
    // Get the top alice volume.
    TNode *ALIC = gAlice->GetGeometry()->GetNode("alice");
    ALIC->cd();

    // Define ITS Mother Volume
    Float_t data[3];
    Float_t ddettest=200.0E-4,ddettelescope=300.0E-4;
    Float_t dchipMiniBus=750.0E-4,dchiptest=300.0E-4;
    //Float_t yposition= 0.0;
    TRotMatrix *r0 = new TRotMatrix("ITSidrotm0","ITSidrotm0",
				    90.0,0,0.0,0,90.0,270.0);
    data[0] = 10.0;
    data[1] = 50.0;
    data[2] = 100.0;
    TBRIK *ITSVshape = new TBRIK("ITSVshape","ITS Logical Mother Volume","Air",
				 data[0],data[1],data[2]);
    TNode *ITSV = new TNode("ITSV","ITS Mother Volume",ITSVshape,
			    0.0,0.0,0.0,0,0);
    ITSV->cd(); // set ourselve into ITSV subvolume of ALIC

    // SPD part of telescope (MiniBuS)
    data[0] = 0.705;
    data[1] = 0.5*ddettelescope;
    data[2] = 3.536;
    TBRIK *IMB0shape = new TBRIK("IMB0shape","SPD wafer","Si",
				 data[0],data[1],data[2]);
    Float_t detMiniBusX,detMiniBusY,detMiniBusZ;
    data[0] = detMiniBusX = 0.64;
    data[1] = detMiniBusY = 0.5*ddettelescope;
    data[2] = detMiniBusZ = 3.48;
    TBRIK *IMBSshape = new TBRIK("IMBSshape","SPD Sensitive volume","Si",
				 data[0],data[1],data[2]);
    Float_t chipMiniBusX,chipMiniBusY,chipMiniBusZ;
    data[0] = chipMiniBusX = 0.793;
    data[1] = chipMiniBusY = 0.5*dchipMiniBus;
    data[2] = chipMiniBusZ = 0.68;
    TBRIK *ICMBshape = new TBRIK("ICMBshape","chip Minibus","Si",
				 data[0],data[1],data[2]);
    data[0] = TMath::Max(detMiniBusX,chipMiniBusX);
    data[1] = detMiniBusY+chipMiniBusY;
    data[2] = TMath::Max(detMiniBusZ,chipMiniBusZ);
    TBRIK *ITELshape = new TBRIK("ITELshape","ITELshape","Air",
				 data[0],data[1],data[2]);

    // SPD under test
    Float_t spdX,spdY,spdZ,spdchipX,spdchipY,spdchipZ;
    data[0] = 0.705;
    data[1] = ddettest;
    data[2] = 3.536;
    TBRIK *ITS0shape = new TBRIK("ITS0shape","SPD wafer","Si",
				 data[0],data[1],data[2]); // contains detector
    data[0] = spdX = 0.64;
    data[1] = spdY = ddettest;
    data[2] = spdZ = 3.48;
    TBRIK *ITSTshape = new TBRIK("ITSTshape","SPD sensitive volume","Si",
				 data[0],data[1],data[2]);
    // ITS0 with no translation and unit rotation matrix.
    data[0] = spdchipX = 0.793;
    data[1] = spdchipY = dchiptest;
    data[2] = spdchipZ = 0.68;
    TBRIK *IPC0shape = new TBRIK("IPC0shape","Readout Chips","Si",
				 data[0],data[1],data[2]); // chip under test
    data[0] = TMath::Max(spdchipX,spdX);
    data[1] = spdY+spdchipY;
    data[2] = TMath::Max(spdchipZ,spdZ);
    TBRIK *IDETshape = new TBRIK("IDETshape","Detector Under Test","Air",
				 data[0],data[1],data[2]);
    // Place volumes in geometry
    Int_t i,j;
    char name[20],title[50];
    Double_t px=0.0,py=0.0,pz[4]={-38.0,0.0,0.0,0.0};
    pz[1] = pz[0]+2.0;
    pz[2] = pz[1]+38.0+spdY+spdchipY+34.5;
    pz[3] = pz[2]+2.0;
    TNode *ITEL[4],*ICMB[4],*IMB0[4],*IMBS[4];
    TNode *IDET = new TNode("IDET","Detector Under Test",IDETshape,
			    0.0,0.0,pz[1]+38.0,r0,0);
    IDET->cd();
    TNode *ITS0 = new TNode("ITS0","SPD Chip",ITS0shape,
			    0.0,IDETshape->GetDy()-spdY,0.0,0,0);
    TNode *IPC0[5];
    for(i=0;i<5;i++) { //place readout chips on the back of SPD chip under test
	sprintf(name,"IPC0%d",i);
	sprintf(title,"Readout chip #%d",i+1);
	j = i-2;
	IPC0[i] = new TNode(name,title,IPC0shape,
			    0.0,spdchipY-IDETshape->GetDy(),
			    j*2.0*spdchipZ+j*0.25*(spdZ-5.*spdchipZ),0,0);
    } // end for i
    ITS0->cd();
    TNode *ITST = new TNode("ITST","SPD sensitive volume",ITSTshape,
			    0.0,0.0,0.0,0,0);
    for(Int_t i=0;i<4;i++){
	ITSV->cd();
	sprintf(name,"ITEL%d",i);
	sprintf(title,"Test beam telescope element #%d",i+1);
	ITEL[i] = new TNode(name,title,ITELshape,px,py,pz[i],r0,0);
	ITEL[i]->cd();
	ICMB[i] = new TNode("ICMB","Chip MiniBus",ICMBshape,
			    0.0,-ITELshape->GetDy()+detMiniBusY,0.0,0,0);
	IMB0[i] = new TNode("IMB0","Chip MiniBus",IMB0shape,
			    0.0, ITELshape->GetDy()-detMiniBusY,0.0,0,0);
	IMB0[i]->cd();
	IMBS[i] = new TNode("IMBS","IMBS",IMBSshape,0.0,0.0,0.0,0,0);
	// place IMBS inside IMB0 with no translation and unit rotation matrix.
    } // end for i
    ALIC->cd();
    ITST->SetLineColor(kYellow);
    fNodes->Add(ITST);
    for(i=0;i<4;i++){
	IMBS[i]->SetLineColor(kGreen);
	fNodes->Add(IMBS[i]);
    } // end for i
}
//______________________________________________________________________
void AliITSvSPD02::CreateGeometry(){
    ////////////////////////////////////////////////////////////////////////
    //  This routine defines and Creates the geometry for version 1 of the ITS.
    //    ALIC    ALICE Mother Volume
    //     |- ITSV     ITS Mother Volume
    //         |- IDET       Detector under Test
    //         |   |- ITS0       SPD Si Chip
    //         |   |  |- ITST      SPD Sensitivve Volume
    //         |   |- IPC0 *5    Readout chip
    //         |- ITEL *4    SPD Telescope
    //             |- IMB0       SPD Si Chip
    //             |   |- IMBS     SPD Sensitive volume
    //             |- ICMB       Chip MiniBus.
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    none.
    ////////////////////////////////////////////////////////////////////////
    Float_t data[49];
    // Define media off-set
    Int_t *idtmed = fIdtmed->GetArray()+1; // array of media indexes
    Int_t idrotm[4]; // Array of rotation matrix indexes
    Float_t ddettest=200.0E-4,ddettelescope=300.0E-4;
    Float_t dchipMiniBus=750.0E-4,dchiptest=300.0E-4;
    Float_t yposition= 0.0;

    // Define Rotation-reflextion Matrixes needed
    // 0 is the unit matrix
    AliMatrix(idrotm[0], 90.0,0.0, 0.0,0.0, 90.0,270.0);
    data[0] = 10.0;
    data[1] = 50.0;
    data[2] = 100.0;
    gMC->Gsvolu("ITSV","BOX",idtmed[0],data,3);
    gMC->Gspos("ITSV",1,"ALIC",0.0,0.0,0.0,0,"ONLY");

    //cout << "idtmed[0]=" << idtmed[0]<<endl;
    //cout << "idtmed[1]=" << idtmed[1]<<endl;
    Float_t detMiniBusX,detMiniBusY,detMiniBusZ;
    // SPD part of telescope (MiniBuS)
    data[0] = detMiniBusX = 0.705;
    data[1] = detMiniBusY = 0.5*ddettelescope;
    data[2] = detMiniBusZ = 3.536;
    gMC->Gsvolu("IMB0", "BOX ", idtmed[1], data, 3);   // contains detector
    data[0] = 0.64;
    data[1] = 0.5*ddettelescope;
    data[2] = 3.48;
    gMC->Gsvolu("IMBS","BOX ",idtmed[1],data,3); // sensitive detecor volulme
    gMC->Gspos("IMBS",1,"IMB0",0.0,0.0,0.0,0,"ONLY"); // place IMBS inside
    // IMB0 with no translation and unit rotation matrix.
    Float_t chipMiniBusX,chipMiniBusY,chipMiniBusZ;
    data[0] = chipMiniBusX = 0.793;
    data[1] = chipMiniBusY = 0.5*dchipMiniBus;
    data[2] = chipMiniBusZ = 0.68;
    gMC->Gsvolu("ICMB","BOX ",idtmed[1],data, 3);   // chip Minibus
    data[0] = TMath::Max(detMiniBusX,chipMiniBusX);
    data[1] = detMiniBusY+chipMiniBusY;
    data[2] = TMath::Max(detMiniBusZ,chipMiniBusZ);
    gMC->Gsvolu("ITEL","BOX ",idtmed[0],data,3);
    gMC->Gspos("IMB0",1,"ITEL",0.0,data[1]-detMiniBusY,0.0,0,"ONLY");
    gMC->Gspos("ICMB",1,"ITEL",0.0,-data[1]+chipMiniBusY,0.0,0,"ONLY");

    // SPD under test
    Float_t spdX,spdY,spdZ,spdchipX,spdchipY,spdchipZ;
    data[0] = spdX = 0.705;
    data[1] = spdY = 0.5*ddettest;
    data[2] = spdZ = 3.536;
    gMC->Gsvolu("ITS0", "BOX ", idtmed[1], data, 3);   // contains detector
    data[0] = 0.64;
    data[1] = 0.5*ddettest;
    data[2] = 3.48;
    gMC->Gsvolu("ITST","BOX ",idtmed[1],data,3);// sensitive detecor volume
    gMC->Gspos("ITST",1,"ITS0",0.0,0.0,0.0,0,"ONLY"); // place ITST inside
    // ITS0 with no translation and unit rotation matrix.
    data[0] = spdchipX = 0.793;
    data[1] = spdchipY = 0.5*dchiptest;
    data[2] = spdchipZ = 0.68;
    gMC->Gsvolu("IPC0", "BOX ", idtmed[1],data,3);   // chip under test
    data[0] = TMath::Max(spdchipX,spdX);
    data[1] = spdY+spdchipY;
    data[2] = TMath::Max(spdchipZ,spdZ);
    gMC->Gsvolu("IDET","BOX ",idtmed[0],data,3);
    gMC->Gspos("ITS0",1,"IDET",0.0,data[1]-spdY,0.0,0,"ONLY");
    for(Int_t i=-2;i<3;i++) gMC->Gspos("IPC0",i+3,"IDET",0.0,-data[1]+spdchipY,
              i*2.*spdchipZ+i*0.25*(spdZ-5.*spdchipZ),0,"ONLY");

    // Positions detectors, Beam Axis Z, X to the right, Y up to the sky.
    Float_t p00X,p00Y,p00Z,p01X,p01Y,p01Z,p10X,p10Y,p10Z,p11X,p11Y,p11Z;
    p00X = 0.0;
    p00Y = 0.0;
    p00Z = -38.0;
    gMC->Gspos("ITEL",1,"ITSV",p00X,p00Y,p00Z,idrotm[0],"ONLY");
    p01X = 0.0;
    p01Y = 0.0;
    p01Z = p00Z+2.0;
    gMC->Gspos("ITEL",2,"ITSV",p01X,p01Y,p01Z,idrotm[0],"ONLY");
    Float_t pdetX,pdetY,pdetZ;
    pdetX = 0.0;
    pdetY = 0.0+yposition;
    pdetZ = p01Z+38.0;
    gMC->Gspos("IDET",1,"ITSV",pdetX,pdetY,pdetZ,idrotm[0],"ONLY");
    p10X = 0.0;
    p10Y = 0.0;
    p10Z = pdetZ + 34.5;
    gMC->Gspos("ITEL",3,"ITSV",p10X,p10Y,p10Z,idrotm[0],"ONLY");
    p11X = 0.0;
    p11Y = 0.0;
    p11Z = p10Z+2.0;
    gMC->Gspos("ITEL",4,"ITSV",p11X,p11Y,p11Z,idrotm[0],"ONLY");
}
//______________________________________________________________________
void AliITSvSPD02::CreateMaterials(){
    ////////////////////////////////////////////////////////////////////////
    //
    // Create ITS SPD test beam materials
    //     This function defines the default materials used in the Geant
    // Monte Carlo simulations for the geometries AliITSv1, AliITSv3,
    // AliITSvSPD02.
    // In general it is automatically replaced by
    // the CreatMaterials routine defined in AliITSv?. Should the function
    // CreateMaterials not exist for the geometry version you are using this
    // one is used. See the definition found in AliITSv5 or the other routine
    // for a complete definition.
    //
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    none.
    /////////////////////////////////////////////////////////////////////////
    Float_t tmaxfdSi = 0.1; // Degree
    Float_t stemaxSi = 0.0075; // cm
    Float_t deemaxSi = 0.1; // Fraction of particle's energy 0<deemax<=1
    Float_t epsilSi  = 1.0E-4;//
    Float_t stminSi  = 0.0; // cm "Default value used"

    Float_t tmaxfdAir = 0.1; // Degree
    Float_t stemaxAir = .10000E+01; // cm
    Float_t deemaxAir = 0.1; // Fraction of particle's energy 0<deemax<=1
    Float_t epsilAir  = 1.0E-4;//
    Float_t stminAir  = 0.0; // cm "Default value used"
    Int_t   ifield = gAlice->Field()->Integ();
    Float_t fieldm = gAlice->Field()->Max();

    AliMaterial(1,"AIR$",0.14610E+02,0.73000E+01,0.12050E-02,
		0.30423E+05,0.99900E+03);
    AliMedium(1,"AIR$",1,0,ifield,fieldm,tmaxfdAir,stemaxAir,deemaxAir,
	      epsilAir,stminAir);

    AliMaterial(2,"SI$",0.28086E+02,0.14000E+02,0.23300E+01,
		0.93600E+01,0.99900E+03);
    AliMedium(2,"SI$",2,0,ifield,fieldm,tmaxfdSi,stemaxSi,deemaxSi,
	      epsilSi,stminSi);
}
//______________________________________________________________________
void AliITSvSPD02::InitAliITSgeom(){
    //     Based on the geometry tree defined in Geant 3.21, this
    // routine initilizes the Class AliITSgeom from the Geant 3.21 ITS geometry
    // sturture.
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    none.
    if(strcmp(gMC->GetName(),"TGeant3")) {
	Error("InitAliITSgeom",
		"Wrong Monte Carlo. InitAliITSgeom uses TGeant3 calls");
	return;
    } // end if
    cout << "Reading Geometry transformation directly from Geant 3." << endl;
    const Int_t nlayers = 6;
    const Int_t ndeep = 5;
    Int_t itsGeomTreeNames[nlayers][ndeep],lnam[20],lnum[20];
    Int_t nlad[nlayers],ndet[nlayers];
    Double_t t[3],r[10];
    Float_t  par[20],att[20];
    Int_t    npar,natt,idshape,imat,imed;
    AliITSGeant3Geometry *ig = new AliITSGeant3Geometry();
    Int_t mod,lay,lad,det,i,j,k;
    Char_t names[nlayers][ndeep][4];
    Int_t itsGeomTreeCopys[nlayers][ndeep];
    Char_t *namesA[nlayers][ndeep] = {
     {"ALIC","ITSV","ITEL","IMB0","IMBS"}, // lay=1
     {"ALIC","ITSV","IDET","ITS0","ITST"}};// Test SPD
    Int_t itsGeomTreeCopysA[nlayers][ndeep]= {{1,1,4,1,1},// lay=1
					      {1,1,1,1,1}};//lay=2 TestSPD
    for(i=0;i<nlayers;i++)for(j=0;j<ndeep;j++){
	for(k=0;k<4;k++) names[i][j][k] = namesA[i][j][k];
	itsGeomTreeCopys[i][j] = itsGeomTreeCopysA[i][j];
    } // end for i,j
    // Sorry, but this is not very pritty code. It should be replaced
    // at some point with a version that can search through the geometry
    // tree its self.
    cout << "Reading Geometry informaton from Geant3 common blocks" << endl;
    for(i=0;i<20;i++) lnam[i] = lnum[i] = 0;
    for(i=0;i<nlayers;i++)for(j=0;j<ndeep;j++) 
	itsGeomTreeNames[i][j] = ig->StringToInt(names[i][j]);
    mod = 0;
    for(i=0;i<nlayers;i++){
	k = 1;
	for(j=0;j<ndeep;j++) if(itsGeomTreeCopys[i][j]!=0)
	    k *= TMath::Abs(itsGeomTreeCopys[i][j]);
	mod += k;
    } // end for i

    if(fITSgeom!=0) delete fITSgeom;
    nlad[0]=20;nlad[1]=40;nlad[2]=14;nlad[3]=22;nlad[4]=34;nlad[5]=38;
    ndet[0]=4;ndet[1]=4;ndet[2]=6;ndet[3]=8;ndet[4]=22;ndet[5]=25;
    fITSgeom = new AliITSgeom(0,6,nlad,ndet,mod);
    mod = -1;
    for(lay=1;lay<=nlayers;lay++){
	for(j=0;j<ndeep;j++) lnam[j] = itsGeomTreeNames[lay-1][j];
	for(j=0;j<ndeep;j++) lnum[j] = itsGeomTreeCopys[lay-1][j];
	switch (lay){
	case 1: case 2: // layers 1 and 2 are a bit special
	    lad = 0;
	    for(j=1;j<=itsGeomTreeCopys[lay-1][4];j++){
		lnum[4] = j;
		for(k=1;k<=itsGeomTreeCopys[lay-1][5];k++){
		    lad++;
		    lnum[5] = k;
		    for(det=1;det<=itsGeomTreeCopys[lay-1][6];det++){
			lnum[6] = det;
			mod++;
			ig->GetGeometry(ndeep,lnam,lnum,t,r,idshape,npar,natt,
					par,att,imat,imed);
			fITSgeom->CreatMatrix(mod,lay,lad,det,kSPD,t,r);
			if(!(fITSgeom->IsShapeDefined((Int_t)kSPD)))
                             fITSgeom->ReSetShape(kSPD,
                                         new AliITSgeomSPD425Short(npar,par));
		    } // end for det
		} // end for k
            } // end for j
	    break;
	} // end switch
    } // end for lay
    return;
}
//______________________________________________________________________
void AliITSvSPD02::Init(){
    ////////////////////////////////////////////////////////////////////////
    //     Initialise the ITS after it has been created.
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    none.
    ////////////////////////////////////////////////////////////////////////
    Int_t i;

    cout << endl;
    for(i=0;i<26;i++) cout << "*";
    cout << " ITSvSPD02" << fMinorVersion << "_Init ";
    for(i=0;i<25;i++) cout << "*";cout << endl;
//
    if(fRead[0]=='\0') strncpy(fRead,fEuclidGeomDet,60);
    if(fWrite[0]=='\0') strncpy(fWrite,fEuclidGeomDet,60);
    if(fITSgeom!=0) delete fITSgeom;
    fITSgeom = new AliITSgeom();
    if(fGeomDetIn) fITSgeom->ReadNewFile(fRead);
    if(!fGeomDetIn) this->InitAliITSgeom();
    if(fGeomDetOut) fITSgeom->WriteNewFile(fWrite);
    AliITS::Init();
//
    for(i=0;i<72;i++) cout << "*";
    cout << endl;
    fIDMother = gMC->VolId("ITSV"); // ITS Mother Volume ID.
}
//______________________________________________________________________
void AliITSvSPD02::SetDefaults(){
    // sets the default segmentation, response, digit and raw cluster classes
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    none.
    const Float_t kconv = 1.0e+04; // convert cm to microns

    Info("SetDefaults","Setting up only SPD detector");

    AliITSDetType *iDetType;
    AliITSgeomSPD  *s0;
    Int_t i;
    Float_t bx[256],bz[280];

    //SPD
    iDetType=DetType(kSPD);
    s0 = (AliITSgeomSPD*) fITSgeom->GetShape(kSPD);// Get shape info. Do it this way for now.
    AliITSresponse *resp0=new AliITSresponseSPD();
    SetResponseModel(kSPD,resp0);
    AliITSsegmentationSPD *seg0=new AliITSsegmentationSPD(fITSgeom);
    seg0->SetDetSize(s0->GetDx()*2.*kconv, // base this on AliITSgeomSPD
		     s0->GetDz()*2.*kconv, // for now.
		     s0->GetDy()*2.*kconv); // x,z,y full width in microns.
    seg0->SetNPads(256,160);// Number of Bins in x and z
    for(i=000;i<256;i++) bx[i] =  50.0; // in x all are 50 microns.
    for(i=000;i<160;i++) bz[i] = 425.0; // most are 425 microns except below
    for(i=160;i<280;i++) bz[i] =   0.0; // Outside of detector.
    bz[ 31] = bz[ 32] = 625.0; // first chip boundry
    bz[ 63] = bz[ 64] = 625.0; // first chip boundry
    bz[ 95] = bz[ 96] = 625.0; // first chip boundry
    bz[127] = bz[128] = 625.0; // first chip boundry
    bz[160] = 425.0; // Set so that there is no zero pixel size for fNz.
    seg0->SetBinSize(bx,bz); // Based on AliITSgeomSPD for now.
    SetSegmentationModel(kSPD,seg0);
    // set digit and raw cluster classes to be used
    const char *kData0=(iDetType->GetResponseModel())->DataType();
    if (strstr(kData0,"real")) iDetType->ClassNames("AliITSdigit",
						    "AliITSRawClusterSPD");
    else iDetType->ClassNames("AliITSdigitSPD","AliITSRawClusterSPD");
//    SetSimulationModel(kSPD,new AliITSsimulationSPD(seg0,resp0));
//    iDetType->ReconstructionModel(new AliITSClusterFinderSPD());

    if(kNTYPES>3){
	Warning("SetDefaults",
		"Only the four basic detector types are initialised!");
    }// end if
    return;
}
//______________________________________________________________________
void AliITSvSPD02::DrawModule(){
    ////////////////////////////////////////////////////////////////////////
    //     Draw a shaded view of the ITS SPD test beam version 1.
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    none.
    ////////////////////////////////////////////////////////////////////////
    // Set everything unseen
    gMC->Gsatt("*", "seen", -1);
    // Set ALIC mother visible
    gMC->Gsatt("ALIC","SEEN",0);
    // Set ALIC ITS visible
    gMC->Gsatt("ITSV","SEEN",0);
    // Set ALIC Telescopes visible
    gMC->Gsatt("ITEL","SEEN",0);
    // Set ALIC detetcor visible
    gMC->Gsatt("IDET","SEEN",0);
    // Set Detector chip mother visible and drawn
    gMC->Gsatt("IPC0","SEEN",1);
    // Set Detector mother visible and drawn
    gMC->Gsatt("ITS0","SEEN",1);
    // Set minibus chip mother visible and drawn
    gMC->Gsatt("ICMB","SEEN",1);
    // Set minibus mother visible and drawn
    gMC->Gsatt("IMB0","SEEN",1);
}
//______________________________________________________________________
void AliITSvSPD02::StepManager(){
    ////////////////////////////////////////////////////////////////////////
    //    Called for every step in the ITS SPD test beam, then calles the 
    // AliITShit class  creator with the information to be recoreded about
    //  that hit.
    //     The value of the macro ALIITSPRINTGEOM if set to 1 will allow the
    // printing of information to a file which can be used to create a .det
    // file read in by the routine CreateGeometry(). If set to 0 or any other
    // value except 1, the default behavior, then no such file is created nor
    // it the extra variables and the like used in the printing allocated.
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    none.
    ////////////////////////////////////////////////////////////////////////
    Int_t         copy, id;
    TLorentzVector position, momentum;
    static TLorentzVector position0;
    static Int_t stat0=0;
    if((id=gMC->CurrentVolID(copy) == fIDMother)&&
       (gMC->IsTrackEntering()||gMC->IsTrackExiting())){
	copy = fTrackReferences->GetEntriesFast();
	TClonesArray &lTR = *fTrackReferences;
	// Fill TrackReference structure with this new TrackReference.
	new(lTR[copy]) AliTrackReference(gAlice->CurrentTrack());
    } // if Outer ITS mother Volume
    if(!(this->IsActive())){
	return;
    } // end if !Active volume.
    Int_t   vol[5];
    TClonesArray &lhits = *fHits;
    //
    // Track status
    vol[3] = 0;
    vol[4] = 0;
    if(gMC->IsTrackInside())      vol[3] +=  1;
    if(gMC->IsTrackEntering())    vol[3] +=  2;
    if(gMC->IsTrackExiting())     vol[3] +=  4;
    if(gMC->IsTrackOut())         vol[3] +=  8;
    if(gMC->IsTrackDisappeared()) vol[3] += 16;
    if(gMC->IsTrackStop())        vol[3] += 32;
    if(gMC->IsTrackAlive())       vol[3] += 64;
    //
    // Fill hit structure.
    if(!(gMC->TrackCharge())) return;
    id = gMC->CurrentVolID(copy);
    if(id==fIdSens[0]){
	vol[0] = vol[1] = 1; // Layer, ladder
	id = gMC->CurrentVolOffID(2,copy);
	//detector copy in the ladder = 1<->4  (ITS1 < I101 < I103 < I10A)
	vol[2] = copy; // detector
    } else if(id == fIdSens[1]){
	vol[0] = 1; // layer
	vol[1] = 2; // ladder
	id = gMC->CurrentVolOffID(2,copy);
	//detector copy in the ladder = 1<->4  (ITS2 < I1D1 < I1D3 < I20A)
	vol[2] = copy;  // detector
    } else return; // end if
    //
    gMC->TrackPosition(position);
    gMC->TrackMomentum(momentum);
    vol[4] = stat0;
    if(gMC->IsTrackEntering()){
	position0 = position;
	stat0 = vol[3];
    } // end if IsEntering
    // Fill hit structure with this new hit only for non-entrerance hits.
    else new(lhits[fNhits++]) AliITShit(fIshunt,gAlice->CurrentTrack(),vol,
					gMC->Edep(),gMC->TrackTime(),position,
					position0,momentum);
    //
    position0 = position;
    stat0 = vol[3];

    return;
}

