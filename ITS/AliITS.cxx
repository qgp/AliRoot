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


///////////////////////////////////////////////////////////////////////////////
//
//      An overview of the basic philosophy of the ITS code development
// and analysis is show in the figure below.
//Begin_Html
/*
<img src="picts/ITS/ITS_Analysis_schema.gif">
</pre>
<br clear=left>
<font size=+2 color=red>
<p>Roberto Barbera is in charge of the ITS Offline code (1999).
<a href="mailto:roberto.barbera@ct.infn.it">Roberto Barbera</a>.
</font>
<pre>
*/
//End_Html
//
//  AliITS. Inner Traking System base class.
//  This class contains the base procedures for the Inner Tracking System
//
//Begin_Html
/*
<img src="picts/ITS/AliITS_Class_Diagram.gif">
</pre>
<br clear=left>
<font size=+2 color=red>
<p>This show the class diagram of the different elements that are part of
the AliITS class.
</font>
<pre>
*/
//End_Html
//
// Version: 0
// Written by Rene Brun, Federico Carminati, and Roberto Barbera
//
// Version: 1
// Modified and documented by Bjorn S. Nilsen
// July 11 1999
//
// Version: 2
// Modified and documented by A. Bologna
// October 18 1999
//
// AliITS is the general base class for the ITS. Also see AliDetector for
// futher information.
//
///////////////////////////////////////////////////////////////////////////////

#include <Riostream.h>
#include <stdlib.h>

#include <TBranch.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TMath.h>
#include <TObjectTable.h>
#include <TROOT.h>
#include <TRandom.h>
#include <TString.h>
#include <TTree.h>
#include <TVector.h>
#include <TVirtualMC.h>

#include "AliConfig.h"
#include "AliHeader.h"
#include "AliITS.h"
#include "AliITSClusterFinderSDD.h"
#include "AliITSClusterFinderSPD.h"
#include "AliITSClusterFinderSSD.h"
#include "AliITSDetType.h"
#include "AliITSLoader.h"
#include "AliITSRawCluster.h"
#include "AliITSRecPoint.h"
#include "AliITSdigit.h"
#include "AliITSgeom.h"
#include "AliITShit.h"
#include "AliITSmodule.h"
#include "AliITSpList.h"
#include "AliITSresponseSDD.h"
#include "AliITSresponseSPD.h"
#include "AliITSresponseSSD.h"
#include "AliITSsegmentationSDD.h"
#include "AliITSsegmentationSPD.h"
#include "AliITSsegmentationSSD.h"
#include "AliITSsimulationSDD.h"
#include "AliITSsimulationSPD.h"
#include "AliITSsimulationSSD.h"

ClassImp(AliITS)

//______________________________________________________________________
AliITS::AliITS() : AliDetector() {
    // Default initializer for ITS
    //      The default constructor of the AliITS class. In addition to
    // creating the AliITS class it zeros the variables fIshunt (a member
    // of AliDetector class), fEuclidOut, and fIdN, and zeros the pointers
    // fITSpoints, fIdSens, and fIdName. The AliDetector default constructor
    // is also called.
    // Inputs:
    //      none.
    // Outputs:
    //      none.
    // Return:
    //      Blank ITS class.

    fIshunt     = 0;   // not zeroed in AliDetector.

    // AliITS variables.
    fEuclidOut  = 0;
    fITSgeom    = 0;
    fITSmodules = 0;
    fOpt        = "All";
//    SetDetectors(); // default to fOpt="All". This variable not written out.

    fIdN        = 0;
    fIdName     = 0;
    fIdSens     = 0;

    fNDetTypes  = kNTYPES;
    fDetTypes   = 0;

    fSDigits    = 0;
    fNSDigits   = 0;

    fNdtype     = 0;
    fDtype      = 0;

    fCtype      = 0;
    fNctype     = 0;

    fRecPoints  = 0;
    fNRecPoints = 0;

    SetMarkerColor(kRed);
}
//______________________________________________________________________
AliITS::AliITS(const char *name, const char *title):AliDetector(name,title){
    //     The standard Constructor for the ITS class. In addition to 
    // creating the AliITS class, it allocates memory for the TClonesArrays 
    // fHits, fSDigits, fDigits, fITSpoints, and the TObjArray of fCtype 
    // (clusters). It also zeros the variables
    // fIshunt (a member of AliDetector class), fEuclidOut, and fIdN, and zeros
    // the pointers fIdSens and fIdName. To help in displaying hits via the
    // ROOT macro display.C AliITS also sets the marker color to red. The
    // variables passes with this constructor, const char *name and *title,
    // are used by the constructor of AliDetector class. See AliDetector
    // class for a description of these parameters and its constructor
    // functions.
    // Inputs:
    //      const char *name      Detector name. Should always be "ITS"
    //      const char *title     Detector title.
    // Outputs:
    //      none.
    // Return:
    //      An ITS class.

    fIshunt     = 0;  // not zeroed in AliDetector
    fHits       = new TClonesArray("AliITShit", 1560);//not done in AliDetector
    gAlice->AddHitList(fHits);  // Not done in AliDetector.

    fEuclidOut  = 0;
    fITSgeom    = 0;
    fITSmodules = 0;
    SetDetectors(); // default to fOpt="All". This variable not written out.

    fIdN        = 0;
    fIdName     = 0;
    fIdSens     = 0;

    fNDetTypes  = kNTYPES;
    fDetTypes   = new TObjArray(fNDetTypes);

    fSDigits    = new TClonesArray("AliITSpListItem",1000);
    fNSDigits   = 0;

    fNdtype     = new Int_t[fNDetTypes];
    fDtype      = new TObjArray(fNDetTypes);

    fCtype      = new TObjArray(fNDetTypes);
    fNctype     = new Int_t[fNDetTypes];

    fRecPoints  = new TClonesArray("AliITSRecPoint",1000);
    fNRecPoints = 0;

    Int_t i;
    for(i=0;i<fNDetTypes;i++) {
        fDetTypes->AddAt(new AliITSDetType(),i); 
        fNdtype[i] = 0;
        fNctype[i] = 0;
    } // end for i

    SetMarkerColor(kRed);

}
//______________________________________________________________________
AliITS::~AliITS(){
    // Default destructor for ITS.
    //     The default destructor of the AliITS class. In addition to deleting
    // the AliITS class it deletes the memory pointed to by the fHits, fDigits,
    // fSDigits, fCtype, fITSmodules, fITSgeom, fRecPoints, fIdSens, fIdName, 
    // fITSpoints, fDetType and it's contents.
    // Inputs:
    //      none.
    // Outputs:
    //      none.
    // Return:
    //      none.

    if (fHits) {
      fHits->Delete();
      delete fHits;
      fHits=0;
    }
    if (fSDigits) {
      fSDigits->Delete();
      delete fSDigits;
      fSDigits=0;
    }
    if (fDigits) {
      fDigits->Delete();
      delete fDigits;
      fDigits=0;
    }
    if (fRecPoints) {
      fRecPoints->Delete();
      delete fRecPoints;
      fRecPoints=0;
    }
    delete[] fIdName;  // Array of TStrings
    delete[] fIdSens;
    if(fITSmodules) {
        this->ClearModules();
        delete fITSmodules;
	fITSmodules = 0;
    }// end if fITSmodules!=0

    if(fDtype) {
        fDtype->Delete();
        delete fDtype;
    } // end if fDtype
    delete [] fNdtype;
    if (fCtype) {
        fCtype->Delete();
        delete fCtype;
	fCtype = 0;
    } // end if fCtype
    delete [] fNctype;

    if (fDetTypes) {
        fDetTypes->Delete();
        delete fDetTypes;
	fDetTypes = 0;
    } // end if fDetTypes


    if (fITSgeom) delete fITSgeom;
}
//______________________________________________________________________
AliITS::AliITS(AliITS &source){
    // Copy constructor. This is a function which is not allowed to be
    // done to the ITS. It exits with an error.
    // Inputs:
    //      AliITS &source  An AliITS class.
    // Outputs:
    //      none.
    // Return:
    //      none.

    if(this==&source) return;
    Error("Copy constructor",
          "You are not allowed to make a copy of the AliITS");
    exit(1);
}
//______________________________________________________________________
AliITS& AliITS::operator=(AliITS &source){
    // Assignment operator. This is a function which is not allowed to be
    // done to the ITS. It exits with an error.
    // Inputs:
    //      AliITS &source  An AliITS class.
    // Outputs:
    //      none.
    // Return:
    //      none.

    if(this==&source) return *this;
    Error("operator=","You are not allowed to make a copy of the AliITS");
    exit(1);
    return *this; //fake return
}
//______________________________________________________________________
Int_t AliITS::DistancetoPrimitive(Int_t,Int_t){
    // Distance from mouse to ITS on the screen. Dummy routine
    //     A dummy routine used by the ROOT macro display.C to allow for the
    // use of the mouse (pointing device) in the macro. In general this should
    // never be called. If it is it returns the number 9999 for any value of
    // x and y.
    // Inputs:
    //      Int_t     Dummy screen coordinate.
    //      Int_t     Dummy screen coordinate.
    // Outputs:
    //      none.
    // Return:
    //      Int_t     Dummy = 9999 distance to ITS.

    return 9999;
}
//______________________________________________________________________
void AliITS::Init(){
    // Initializer ITS after it has been built
    //     This routine initializes the AliITS class. It is intended to be
    // called from the Init function in AliITSv?. Besides displaying a banner
    // indicating that it has been called it initializes the array fIdSens
    // and sets the default segmentation, response, digit and raw cluster
    // classes therefore it should be called after a call to CreateGeometry.
    // Inputs:
    //      none.
    // Outputs:
    //      none.
    // Return:
    //      none.
    Int_t i;

    SetDefaults();
    // Array of TStrings
    for(i=0;i<fIdN;i++) fIdSens[i] = gMC->VolId(fIdName[i]);
}
//______________________________________________________________________
void AliITS::SetDefaults(){
    // sets the default segmentation, response, digit and raw cluster classes.
    // Inputs:
    //      none.
    // Outputs:
    //      none.
    // Return:
    //      none.

    if(fDebug) printf("%s: SetDefaults\n",ClassName());

    AliITSDetType *iDetType;

    //SPD
    iDetType=DetType(0); 
    if (!iDetType->GetSegmentationModel()) {
        AliITSsegmentationSPD *seg0=new AliITSsegmentationSPD(fITSgeom);
        SetSegmentationModel(0,seg0); 
    } // end if
    if (!iDetType->GetResponseModel()) {
        SetResponseModel(0,new AliITSresponseSPD()); 
    } // end if
    // set digit and raw cluster classes to be used

    const char *kData0=(iDetType->GetResponseModel())->DataType();
    if (strstr(kData0,"real")) {
        iDetType->ClassNames("AliITSdigit","AliITSRawClusterSPD");
    } else iDetType->ClassNames("AliITSdigitSPD","AliITSRawClusterSPD");

    // SDD
    iDetType=DetType(1); 
    if (!iDetType->GetResponseModel()) {
        SetResponseModel(1,new AliITSresponseSDD("simulated")); 
    } // end if
    AliITSresponse *resp1=iDetType->GetResponseModel();
    if (!iDetType->GetSegmentationModel()) {
        AliITSsegmentationSDD *seg1=new AliITSsegmentationSDD(fITSgeom,resp1);
        SetSegmentationModel(1,seg1); 
    } // end if
    const char *kData1=(iDetType->GetResponseModel())->DataType();
    const char *kopt=iDetType->GetResponseModel()->ZeroSuppOption();
    if((!strstr(kopt,"2D"))&&(!strstr(kopt,"1D")) || strstr(kData1,"real") ){
        iDetType->ClassNames("AliITSdigit","AliITSRawClusterSDD");
    } else iDetType->ClassNames("AliITSdigitSDD","AliITSRawClusterSDD");

    // SSD
    iDetType=DetType(2); 
    if (!iDetType->GetSegmentationModel()) {
        AliITSsegmentationSSD *seg2=new AliITSsegmentationSSD(fITSgeom);
        SetSegmentationModel(2,seg2); 
    } // end if
    if (!iDetType->GetResponseModel()) {
        SetResponseModel(2,new AliITSresponseSSD("simulated"));
    } // end if
    const char *kData2=(iDetType->GetResponseModel())->DataType();
    if (strstr(kData2,"real")) {
        iDetType->ClassNames("AliITSdigit","AliITSRawClusterSSD");
    } else iDetType->ClassNames("AliITSdigitSSD","AliITSRawClusterSSD");

    if (kNTYPES>3) {
        Warning("SetDefaults",
                "Only the three basic detector types are initialized!");
    }  // end if
}
//______________________________________________________________________
void AliITS::SetDefaultSimulation(){
    // sets the default simulation.
    // Inputs:
    //      none.
    // Outputs:
    //      none.
    // Return:
    //      none.

    AliITSDetType *iDetType;
    AliITSsimulation *sim;
    iDetType=DetType(0);
    sim = iDetType->GetSimulationModel();
    if (!sim) {
        AliITSsegmentation *seg0=
            (AliITSsegmentation*)iDetType->GetSegmentationModel();
        AliITSresponse *res0 = (AliITSresponse*)iDetType->GetResponseModel();
        AliITSsimulationSPD *sim0=new AliITSsimulationSPD(seg0,res0);
        SetSimulationModel(0,sim0);
    }else{ // simulation exists, make sure it is set up properly.
        ((AliITSsimulationSPD*)sim)->Init(
            (AliITSsegmentationSPD*) iDetType->GetSegmentationModel(),
            (AliITSresponseSPD*) iDetType->GetResponseModel());
//        if(sim->GetResponseModel()==0) sim->SetResponseModel(
//            (AliITSresponse*)iDetType->GetResponseModel());
//        if(sim->GetSegmentationModel()==0) sim->SetSegmentationModel(
//            (AliITSsegmentation*)iDetType->GetSegmentationModel());
    } // end if
    iDetType=DetType(1);
    sim = iDetType->GetSimulationModel();
    if (!sim) {
        AliITSsegmentation *seg1=
            (AliITSsegmentation*)iDetType->GetSegmentationModel();
        AliITSresponse *res1 = (AliITSresponse*)iDetType->GetResponseModel();
        AliITSsimulationSDD *sim1=new AliITSsimulationSDD(seg1,res1);
        SetSimulationModel(1,sim1);
    }else{ // simulation exists, make sure it is set up properly.
        ((AliITSsimulationSDD*)sim)->Init(
            (AliITSsegmentationSDD*) iDetType->GetSegmentationModel(),
            (AliITSresponseSDD*) iDetType->GetResponseModel());
//        if(sim->GetResponseModel()==0) sim->SetResponseModel(
//            (AliITSresponse*)iDetType->GetResponseModel());
//        if(sim->GetSegmentationModel()==0) sim->SetSegmentationModel(
//            (AliITSsegmentation*)iDetType->GetSegmentationModel());
    } //end if
    iDetType=DetType(2);
    sim = iDetType->GetSimulationModel();
    if (!sim) {
        AliITSsegmentation *seg2=
            (AliITSsegmentation*)iDetType->GetSegmentationModel();
        AliITSresponse *res2 = (AliITSresponse*)iDetType->GetResponseModel();
        AliITSsimulationSSD *sim2=new AliITSsimulationSSD(seg2,res2);
        SetSimulationModel(2,sim2);
    }else{ // simulation exists, make sure it is set up properly.
        ((AliITSsimulationSSD*)sim)->Init(
            (AliITSsegmentationSSD*) iDetType->GetSegmentationModel(),
            (AliITSresponseSSD*) iDetType->GetResponseModel());
//        if(sim->GetResponseModel()==0) sim->SetResponseModel(
//            (AliITSresponse*)iDetType->GetResponseModel());
//        if(sim->GetSegmentationModel()==0) sim->SetSegmentationModel(
//            (AliITSsegmentation*)iDetType->GetSegmentationModel());
    } // end if
}
//______________________________________________________________________
void AliITS::SetDefaultClusterFinders(){
    // Sets the default cluster finders. Used in finding RecPoints.
    // Inputs:
    //      none.
    // Outputs:
    //      none.
    // Return:
    //      none.

    MakeTreeC();
    AliITSDetType *iDetType;

    // SPD
    iDetType=DetType(0);
    if (!iDetType->GetReconstructionModel()) {
        AliITSsegmentation *seg0 =
            (AliITSsegmentation*)iDetType->GetSegmentationModel();
        TClonesArray *dig0=DigitsAddress(0);
        TClonesArray *recp0=ClustersAddress(0);
        AliITSClusterFinderSPD *rec0 = new AliITSClusterFinderSPD(seg0,dig0,
                                                                  recp0);
        SetReconstructionModel(0,rec0);
    } // end if

    // SDD
    iDetType=DetType(1);
    if (!iDetType->GetReconstructionModel()) {
        AliITSsegmentation *seg1 =
            (AliITSsegmentation*)iDetType->GetSegmentationModel();
        AliITSresponse *res1 = (AliITSresponse*)iDetType->GetResponseModel();
        TClonesArray *dig1=DigitsAddress(1);
        TClonesArray *recp1=ClustersAddress(1);
        AliITSClusterFinderSDD *rec1 =
            new AliITSClusterFinderSDD(seg1,res1,dig1,recp1);
      SetReconstructionModel(1,rec1);
    } // end if

    // SSD
    iDetType=DetType(2);
    if (!iDetType->GetReconstructionModel()) {
        AliITSsegmentation *seg2=
            (AliITSsegmentation*)iDetType->GetSegmentationModel();
        TClonesArray *dig2=DigitsAddress(2);
        AliITSClusterFinderSSD *rec2= new AliITSClusterFinderSSD(seg2,dig2);
        SetReconstructionModel(2,rec2);
    } // end if
}
//______________________________________________________________________
void AliITS::MakeBranch(Option_t* option){
    // Creates Tree branches for the ITS.
    // Inputs:
    //      Option_t *option    String of Tree types S,D, and/or R.
    //      const char *file    String of the file name where these branches
    //                          are to be stored. If blank then these branches
    //                          are written to the same tree as the Hits were
    //                          read from.
    // Outputs:
    //      none.
    // Return:
    //      none.
    Bool_t cH = (strstr(option,"H")!=0);
    Bool_t cS = (strstr(option,"S")!=0);
    Bool_t cD = (strstr(option,"D")!=0);
    Bool_t cR = (strstr(option,"R")!=0);
    Bool_t cRF = (strstr(option,"RF")!=0);
    
    if(cRF)cR = kFALSE;
    if(cH && (fHits == 0x0)) fHits  = new TClonesArray("AliITShit", 1560);
    
    AliDetector::MakeBranch(option);

    if(cS) MakeBranchS(0);
    if(cD) MakeBranchD(0);
    if(cR) MakeBranchR(0);
    if(cRF) MakeBranchRF(0);
}
//______________________________________________________________________
void AliITS::SetTreeAddress(){
    // Set branch address for the Trees.
    // Inputs:
    //      none.
    // Outputs:
    //      none.
    // Return:
    //      none.
    TTree *treeS = fLoader->TreeS();
    TTree *treeD = fLoader->TreeD();
    TTree *treeR = fLoader->TreeR();
    if (fLoader->TreeH() && (fHits == 0x0)) fHits = new TClonesArray("AliITShit", 1560);
      
    AliDetector::SetTreeAddress();

    SetTreeAddressS(treeS);
    SetTreeAddressD(treeD);
    SetTreeAddressR(treeR);
}
//______________________________________________________________________
AliITSDetType* AliITS::DetType(Int_t id){
    // Return pointer to id detector type.
    // Inputs:
    //      Int_t id   detector id number.
    // Outputs:
    //      none.
    // Return:
    //      returned, a pointer to a AliITSDetType.

    return ((AliITSDetType*) fDetTypes->At(id));
}
//______________________________________________________________________
void AliITS::SetResponseModel(Int_t id, AliITSresponse *response){
    // Set the response model for the id detector type.
    // Inputs:
    //      Int_t id        detector id number.
    //      AliITSresponse* a pointer containing an instance of AliITSresponse
    //                      to be stored/owned b y AliITSDetType.
    // Outputs:
    //      none.
    // Return:
    //      none.

    ((AliITSDetType*) fDetTypes->At(id))->ResponseModel(response);
}
//______________________________________________________________________
void AliITS::SetSegmentationModel(Int_t id, AliITSsegmentation *seg){
    // Set the segmentation model for the id detector type.
    // Inputs:
    //      Int_t id            detector id number.
    //      AliITSsegmentation* a pointer containing an instance of 
    //                          AliITSsegmentation to be stored/owned b y 
    //                          AliITSDetType.
    // Outputs:
    //      none.
    // Return:
    //      none.

    ((AliITSDetType*) fDetTypes->At(id))->SegmentationModel(seg);
}
//______________________________________________________________________
void AliITS::SetSimulationModel(Int_t id, AliITSsimulation *sim){
    // Set the simulation model for the id detector type.
    // Inputs:
    //      Int_t id        detector id number.
    //      AliITSresponse* a pointer containing an instance of AliITSresponse
    //                      to be stored/owned b y AliITSDetType.
    // Outputs:
    //      none.
    // Return:
    //      none.

   ((AliITSDetType*) fDetTypes->At(id))->SimulationModel(sim);

}
//______________________________________________________________________
void AliITS::SetReconstructionModel(Int_t id, AliITSClusterFinder *reconst){
    // Set the cluster finder model for the id detector type.
    // Inputs:
    //      Int_t id             detector id number.
    //      AliITSClusterFinder* a pointer containing an instance of 
    //                           AliITSClusterFinder to be stored/owned b y 
    //                           AliITSDetType.
    // Outputs:
    //      none.
    // Return:
    //      none.

    ((AliITSDetType*) fDetTypes->At(id))->ReconstructionModel(reconst);
}
//______________________________________________________________________
void AliITS::SetClasses(Int_t id, const char *digit, const char *cluster){
    // Set the digit and cluster classes name to be used for the id detector
    // type.
    // Inputs:
    //      Int_t id            detector id number.
    //      const char *digit   Digit class name for detector id.
    //      const char *cluster Cluster class name for detector id.
    // Outputs:
    //      none.
    // Return:
    //      none.

    ((AliITSDetType*) fDetTypes->At(id))->ClassNames(digit,cluster);
}
//______________________________________________________________________
void AliITS::AddHit(Int_t track, Int_t *vol, Float_t *hits){
    // Add an ITS hit
    //     The function to add information to the AliITShit class. See the
    // AliITShit class for a full description. This function allocates the
    // necessary new space for the hit information and passes the variable
    // track, and the pointers *vol and *hits to the AliITShit constructor
    // function.
    // Inputs:
    //      Int_t   track   Track number which produced this hit.
    //      Int_t   *vol    Array of Integer Hit information. See AliITShit.h
    //      Float_t *hits   Array of Floating Hit information.  see AliITShit.h
    // Outputs:
    //      none.
    // Return:
    //      none.

    TClonesArray &lhits = *fHits;
    new(lhits[fNhits++]) AliITShit(fIshunt,track,vol,hits);
}
//______________________________________________________________________
void AliITS::InitModules(Int_t size,Int_t &nmodules){
    // Initialize the modules array.
    // Inputs:
    //      Int_t size  Size of array of the number of modules to be
    //                  created. If size <=0 then the number of modules
    //                  is gotten from AliITSgeom class kept in fITSgeom.
    // Outputs:
    //      Int_t &nmodules The number of modules existing.
    // Return:
    //      none.

    if(fITSmodules){ 
        fITSmodules->Delete();
        delete fITSmodules;
    } // end fir fITSmoudles

    Int_t nl,indexMAX,index;

    if(size<=0){ // default to using data stored in AliITSgeom
        if(fITSgeom==0) {
            Error("InitModules","fITSgeom not defined");
            return;
        } // end if fITSgeom==0
        nl = fITSgeom->GetNlayers();
        indexMAX = fITSgeom->GetModuleIndex(nl,fITSgeom->GetNladders(nl),
                                            fITSgeom->GetNdetectors(nl))+1;
        nmodules = indexMAX;
        fITSmodules = new TObjArray(indexMAX);
        for(index=0;index<indexMAX;index++){
            fITSmodules->AddAt( new AliITSmodule(index),index);
        } // end for index
    }else{
        fITSmodules = new TObjArray(size);
        for(index=0;index<size;index++) {
            fITSmodules->AddAt( new AliITSmodule(index),index);
        } // end for index

        nmodules = size;
    } // end i size<=0
}
//______________________________________________________________________
void AliITS::FillModules(Int_t evnt,Int_t bgrev,Int_t nmodules,
                         Option_t *option,Text_t *filename){
    // fill the modules with the sorted by module hits; add hits from
    // background if option=Add.
    // Inputs:
    //      Int_t evnt       Event to be processed.
    //      Int_t bgrev      Background Hit tree number.
    //      Int_t nmodules   Not used.
    //      Option_t *option String indicating if merging hits or not. To
    //                       merge hits set equal to "Add". Otherwise no
    //                       background hits are considered.
    //      Test_t *filename File name containing the background hits..
    // Outputs:
    //      none.
    // Return:
    //      none.
    static TTree *trH1;                 //Tree with background hits
    static Bool_t first=kTRUE;
    static TFile *file;
    const char *addBgr = strstr(option,"Add");

    if (addBgr ) {
        if(first) {
            file=new TFile(filename);
        } // end if first
        first=kFALSE;
        file->cd();
        file->ls();
        // Get Hits Tree header from file
        if(trH1) delete trH1;
        trH1=0;

        char treeName[20];
        sprintf(treeName,"TreeH%d",bgrev);
        trH1 = (TTree*)gDirectory->Get(treeName);
        if (!trH1) {
            Error("FillModules","cannot find Hits Tree for event:%d",bgrev);
        } // end if !trH1
        // Set branch addresses
    } // end if addBgr

    FillModules(fLoader->TreeH(),0); // fill from this file's tree.
    
    if (addBgr ) {
        FillModules(trH1,10000000); // Default mask 10M.
        TTree *fAli=fLoader->GetRunLoader()->TreeK();
        TFile *fileAli=0;
        if (fAli) fileAli =fAli->GetCurrentFile();
        fileAli->cd();
    } // end if add
}
//______________________________________________________________________
void AliITS::FillModules(TTree *treeH, Int_t mask) {
    // fill the modules with the sorted by module hits; 
    // can be called many times to do a merging
    // Inputs:
    //      TTree *treeH  The tree containing the hits to be copied into
    //                    the modules.
    //      Int_t mask    The track number mask to indecate which file
    //                    this hits came from.
    // Outputs:
    //      none.
    // Return:
    //      none.

    if (treeH == 0x0)
     {
       Error("FillModules","Tree is NULL");
     }
    Int_t lay,lad,det,index;
    AliITShit *itsHit=0;
    AliITSmodule *mod=0;
    char branchname[20];
    sprintf(branchname,"%s",GetName());
    TBranch *branch = treeH->GetBranch(branchname);
    if (!branch) {
        Error("FillModules","%s branch in TreeH not found",branchname);
        return;
    } // end if !branch
    branch->SetAddress(&fHits);
    Int_t nTracks =(Int_t) treeH->GetEntries();
    Int_t iPrimTrack,h;
    for(iPrimTrack=0; iPrimTrack<nTracks; iPrimTrack++){
        ResetHits();
        Int_t nBytes = treeH->GetEvent(iPrimTrack);
        if (nBytes <= 0) continue;
        Int_t nHits = fHits->GetEntriesFast();
        for(h=0; h<nHits; h++){
            itsHit = (AliITShit *)fHits->UncheckedAt(h);
            itsHit->GetDetectorID(lay,lad,det);
            if (fITSgeom) {
                index = fITSgeom->GetModuleIndex(lay,lad,det);
            } else {
                index=det-1; // This should not be used.
            } // end if [You must have fITSgeom for this to work!]
            mod = GetModule(index);
            itsHit->SetTrack(itsHit->GetTrack()+mask); // Set track mask.
            mod->AddHit(itsHit,iPrimTrack,h);
        } // end loop over hits 
    } // end loop over tracks
}
//______________________________________________________________________
void AliITS::ClearModules(){
    // Clear the modules TObjArray.
    // Inputs:
    //      none.
    // Outputs:
    //      none.

    if(fITSmodules) fITSmodules->Delete();
}
//______________________________________________________________________
void AliITS::MakeBranchS(const char *fl){
    // Creates Tree Branch for the ITS summable digits.
    // Inputs:
    //      cont char *fl  File name where SDigits branch is to be written
    //                     to. If blank it write the SDigits to the same
    //                     file in which the Hits were found.
    // Outputs:
    //      none.
    // Return:
    //      none.
    Int_t buffersize = 4000;
    char branchname[30];

    // only one branch for SDigits.
    sprintf(branchname,"%s",GetName());
    

    if(fSDigits && fLoader->TreeS()){
        if (fSDigits == 0x0)  fSDigits  = new TClonesArray("AliITSpListItem",1000);
        MakeBranchInTree(fLoader->TreeS(),branchname,&fSDigits,buffersize,fl);
    } // end if
}
//______________________________________________________________________
void AliITS::SetTreeAddressS(TTree *treeS){
    // Set branch address for the ITS summable digits Trees.
    // Inputs:
    //      TTree *treeS   Tree containing the SDigits.
    // Outputs:
    //      none.
    // Return:
    //      none.
    char branchname[30];

    if(!treeS) return;
    if (fSDigits == 0x0)  fSDigits = new TClonesArray("AliITSpListItem",1000);
    TBranch *branch;
    sprintf(branchname,"%s",GetName());
    branch = treeS->GetBranch(branchname);
    if (branch) branch->SetAddress(&fSDigits);
}
//______________________________________________________________________
void AliITS::MakeBranchInTreeD(TTree *treeD,const char *file){
    // Creates Tree branches for the ITS.
    // Inputs:
    //      TTree     *treeD Pointer to the Digits Tree.
    //      cont char *file  File name where Digits branch is to be written
    //                       to. If blank it write the SDigits to the same
    //                       file in which the Hits were found.
    // Outputs:
    //      none.
    // Return:
    //      none.
    Int_t buffersize = 4000;
    char branchname[30];

    sprintf(branchname,"%s",GetName());
    // one branch for digits per type of detector
    const char *det[3] = {"SPD","SDD","SSD"};
    char digclass[40];
    char clclass[40];
    Int_t i;
    for (i=0; i<kNTYPES ;i++) {
        DetType(i)->GetClassNames(digclass,clclass);
        // digits
        if (fDtype == 0x0) fDtype = new TObjArray(fNDetTypes);
        if(!(fDtype->At(i))) fDtype->AddAt(new TClonesArray(digclass,1000),i);
        else ResetDigits(i);
    } // end for i
    for (i=0; i<kNTYPES ;i++) {
        if (kNTYPES==3) sprintf(branchname,"%sDigits%s",GetName(),det[i]);
        else  sprintf(branchname,"%sDigits%d",GetName(),i+1);      
        if (fDtype && treeD) {
            MakeBranchInTree(treeD, branchname, &((*fDtype)[i]),buffersize,file);
        } // end if
    } // end for i
}
//______________________________________________________________________
void AliITS::SetTreeAddressD(TTree *treeD){
    // Set branch address for the Trees.
    // Inputs:
    //      TTree *treeD   Tree containing the Digits.
    // Outputs:
    //      none.
    // Return:
    //      none.
    char branchname[30];
    const char *det[3] = {"SPD","SDD","SSD"};
    TBranch *branch;
    char digclass[40];
    char clclass[40];
    Int_t i;

    if(!treeD) return;
    for (i=0; i<kNTYPES; i++) {
        DetType(i)->GetClassNames(digclass,clclass);
        // digits
        if (fDtype == 0x0) fDtype = new TObjArray(fNDetTypes);
        if(!(fDtype->At(i))) fDtype->AddAt(new TClonesArray(digclass,1000),i);
        else ResetDigits(i);
        if (kNTYPES==3) sprintf(branchname,"%sDigits%s",GetName(),det[i]);
        else  sprintf(branchname,"%sDigits%d",GetName(),i+1);
        if (fDtype) {
            branch = treeD->GetBranch(branchname);
            if (branch) branch->SetAddress(&((*fDtype)[i]));
        } // end if fDtype
    } // end for i
}
//______________________________________________________________________
void AliITS::Hits2SDigits(){
    // Standard Hits to summable Digits function.
    // Inputs:
    //      none.
    // Outputs:
    //      none.

//    return; // Using Hits in place of the larger sDigits.
    AliRunLoader* rl = fLoader->GetRunLoader(); 
    AliHeader *header=rl->GetHeader(); // Get event number from this file.
    if (header == 0x0)
     {
       rl->LoadHeader();
       header=rl->GetHeader();
       if (header == 0x0) return;
     }
    // Do the Hits to Digits operation. Use Standard input values.
    // Event number from file, no background hit merging , use size from
    // AliITSgeom class, option="All", input from this file only.
    HitsToSDigits(header->GetEvent(),0,-1," ",fOpt," ");
    
}
//______________________________________________________________________
void AliITS::Hits2PreDigits(){
    // Standard Hits to summable Digits function.
    // Inputs:
    //      none.
    // Outputs:
    //      none.

    AliHeader *header=fLoader->GetRunLoader()->GetHeader(); // Get event number from this file.
    // Do the Hits to Digits operation. Use Standard input values.
    // Event number from file, no background hit merging , use size from
    // AliITSgeom class, option="All", input from this file only.
    HitsToPreDigits(header->GetEvent(),0,-1," ",fOpt," ");
}
//______________________________________________________________________
void AliITS::SDigitsToDigits(Option_t *opt){
    // Standard Summable digits to Digits function.
    // Inputs:
    //      none.
    // Outputs:
    //      none.
    char name[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    if(!GetITSgeom()) return; // need transformations to do digitization.
    AliITSgeom *geom = GetITSgeom();

    const char *all = strstr(opt,"All");
    const char *det[3] = {strstr(opt,"SPD"),strstr(opt,"SDD"),
                          strstr(opt,"SSD")};
    if( !det[0] && !det[1] && !det[2] ) all = "All";
    else all = 0;
    static Bool_t setDef=kTRUE;
    if (setDef) SetDefaultSimulation();
    setDef=kFALSE;

    AliITSsimulation *sim      = 0;
    AliITSDetType    *iDetType = 0;
    TTree            *trees    = fLoader->TreeS();
    if( !(trees && this->GetSDigits()) ){
        Error("SDigits2Digits","Error: No trees or SDigits. Returning.");
        return;
    } // end if
    sprintf( name, "%s", this->GetName() );
    TBranch *brchSDigits = trees->GetBranch( name );
    
    Int_t id,module;
    for(module=0;module<geom->GetIndexMax();module++){
        id       = geom->GetModuleType(module);
        if (!all && !det[id]) continue;
        iDetType = DetType(id);
        sim      = (AliITSsimulation*)iDetType->GetSimulationModel();
        if (!sim) {
            Error("SDigit2Digits",
                  "The simulation class was not instanciated!");
            exit(1);
        } // end if !sim
        sim->InitSimulationModule(module,gAlice->GetEvNumber());
//
        // add summable digits to module
        this->GetSDigits()->Clear();
        brchSDigits->GetEvent(module);
        sim->AddSDigitsToModule(GetSDigits(),0);
//
        // Digitise current module sum(SDigits)->Digits
        sim->FinishSDigitiseModule();

        // fills all branches - wasted disk space
        fLoader->TreeD()->Fill();
        this->ResetDigits();
    } // end for module

    fLoader->TreeD()->GetEntries();

    fLoader->TreeD()->AutoSave();
    // reset tree
    fLoader->TreeD()->Reset();
    
}
//______________________________________________________________________
void AliITS::Hits2Digits(){
    // Standard Hits to Digits function.
    // Inputs:
    //      none.
    // Outputs:
    //      none.

    AliHeader *header=fLoader->GetRunLoader()->GetHeader(); // Get event number from this file.
    // Do the Hits to Digits operation. Use Standard input values.
    // Event number from file, no background hit merging , use size from
    // AliITSgeom class, option="All", input from this file only.
    HitsToDigits(header->GetEvent(),0,-1," ",fOpt," ");
}
//______________________________________________________________________
void AliITS::HitsToSDigits(Int_t evNumber,Int_t bgrev,Int_t size,
                          Option_t *option, Option_t *opt,Text_t *filename){
    // keep galice.root for signal and name differently the file for 
    // background when add! otherwise the track info for signal will be lost !
    // the condition below will disappear when the geom class will be
    // initialized for all versions - for the moment it is only for v5 !
    // 7 is the SDD beam test version. Dummy routine. Hits are ITS's Summable
    // Digits.
    // Inputs:
    //      Int_t evnt       Event to be processed.
    //      Int_t bgrev      Background Hit tree number.
    //      Int_t nmodules   Not used.
    //      Option_t *option String indicating if merging hits or not. To
    //                       merge hits set equal to "Add". Otherwise no
    //                       background hits are considered.
    //      Test_t *filename File name containing the background hits..
    // Outputs:
    //      none.
    // Return:
    //      none.
//    return; // using Hits instead of the larger sdigits.

    HitsToPreDigits(evNumber,bgrev,size,option,opt,filename);
}
//______________________________________________________________________
void AliITS::HitsToPreDigits(Int_t evNumber,Int_t bgrev,Int_t size,
                          Option_t *option, Option_t *opt,Text_t *filename){
    //   Keep galice.root for signal and name differently the file for 
    // background when add! otherwise the track info for signal will be lost !
    // the condition below will disappear when the geom class will be
    // initialized for all versions - for the moment it is only for v5 !
    // 7 is the SDD beam test version.
    // Inputs:
    //      Int_t evnt       Event to be processed.
    //      Int_t bgrev      Background Hit tree number.
    //      Int_t nmodules   Not used.
    //      Option_t *option String indicating if merging hits or not. To
    //                       merge hits set equal to "Add". Otherwise no
    //                       background hits are considered.
    //      Test_t *filename File name containing the background hits..
    // Outputs:
    //      none.
    // Return:
    //      none.

    if(!GetITSgeom()) return; // need transformations to do digitization.
    AliITSgeom *geom = GetITSgeom();

    const char *all = strstr(opt,"All");
    const char *det[3] = {strstr(opt,"SPD"),strstr(opt,"SDD"),
                          strstr(opt,"SSD")};
    static Bool_t setDef=kTRUE;
    if (setDef) SetDefaultSimulation();
    setDef=kFALSE;

    Int_t nmodules;
    InitModules(size,nmodules);
    FillModules(evNumber,bgrev,nmodules,option,filename);

    AliITSsimulation *sim      = 0;
    AliITSDetType    *iDetType = 0;
    AliITSmodule     *mod      = 0;
    Int_t id,module;
    for(module=0;module<geom->GetIndexMax();module++){
        id       = geom->GetModuleType(module);
        if (!all && !det[id]) continue;
        iDetType = DetType(id);
        sim      = (AliITSsimulation*)iDetType->GetSimulationModel();
        if (!sim) {
            Error("HitsToSDigits",
                  "The simulation class was not instanciated!");
            exit(1);
        } // end if !sim
        mod      = (AliITSmodule *)fITSmodules->At(module);
        sim->SDigitiseModule(mod,module,evNumber);
        // fills all branches - wasted disk space
        fLoader->TreeS()->Fill(); 
        ResetSDigits();
    } // end for module

    ClearModules();

    fLoader->TreeS()->GetEntries();
    fLoader->TreeS()->AutoSave();
    fLoader->WriteSDigits("OVERWRITE");
    // reset tree
    fLoader->TreeS()->Reset();
}
//______________________________________________________________________
void AliITS::HitsToDigits(Int_t evNumber,Int_t bgrev,Int_t size,
                          Option_t *option, Option_t *opt,Text_t *filename){
    //   Keep galice.root for signal and name differently the file for 
    // background when add! otherwise the track info for signal will be lost !
    // the condition below will disappear when the geom class will be
    // initialized for all versions - for the moment it is only for v5 !
    // 7 is the SDD beam test version.
    // Inputs:
    //      Int_t evnt       Event to be processed.
    //      Int_t bgrev      Background Hit tree number.
    //      Int_t nmodules   Not used.
    //      Option_t *option String indicating if merging hits or not. To
    //                       merge hits set equal to "Add". Otherwise no
    //                       background hits are considered.
    //      Test_t *filename File name containing the background hits..
    // Outputs:
    //      none.
    // Return:
    //      none.

    if(!GetITSgeom()) return; // need transformations to do digitization.
    AliITSgeom *geom = GetITSgeom();

    const char *all = strstr(opt,"All");
    const char *det[3] = {strstr(opt,"SPD"),strstr(opt,"SDD"),
                          strstr(opt,"SSD")};
    static Bool_t setDef=kTRUE;
    if (setDef) SetDefaultSimulation();
    setDef=kFALSE;

    Int_t nmodules;
    InitModules(size,nmodules);
    FillModules(evNumber,bgrev,nmodules,option,filename);

    AliITSsimulation *sim      = 0;
    AliITSDetType    *iDetType = 0;
    AliITSmodule     *mod      = 0;
    Int_t id,module;
    for(module=0;module<geom->GetIndexMax();module++){
        id       = geom->GetModuleType(module);
        if (!all && !det[id]) continue;
        iDetType = DetType(id);
        sim      = (AliITSsimulation*)iDetType->GetSimulationModel();
        if (!sim) {
            Error("HitsToDigits",
                  "The simulation class was not instanciated!");
            exit(1);
        } // end if !sim
        mod      = (AliITSmodule *)fITSmodules->At(module);
        sim->DigitiseModule(mod,module,evNumber);
        // fills all branches - wasted disk space
        fLoader->TreeD()->Fill(); 
        ResetDigits();
    } // end for module

    ClearModules();

    fLoader->TreeD()->GetEntries();
    fLoader->TreeD()->AutoSave();
    // reset tree
    fLoader->TreeD()->Reset();
}
//______________________________________________________________________
void AliITS::ResetSDigits(){
    // Reset the Summable Digits array.
    // Inputs:
    //      none.
    // Outputs:
    //      none.

    if (fSDigits) fSDigits->Clear();
    fNSDigits = 0;
}
//______________________________________________________________________
void AliITS::ResetDigits(){
    // Reset number of digits and the digits array for the ITS detector.
    // Inputs:
    //      none.
    // Outputs:
    //      none.

    if (!fDtype) return;

    Int_t i;
    for (i=0;i<kNTYPES;i++ ) {
        if (fDtype->At(i))    ((TClonesArray*)fDtype->At(i))->Clear();
        if (fNdtype)  fNdtype[i]=0;
    } // end for i
}
//______________________________________________________________________
void AliITS::ResetDigits(Int_t i){
    // Reset number of digits and the digits array for this branch.
    // Inputs:
    //      none.
    // Outputs:
    //      none.

    if (fDtype->At(i))    ((TClonesArray*)fDtype->At(i))->Clear();
    if (fNdtype)  fNdtype[i]=0;
}
//______________________________________________________________________
void AliITS::AddSumDigit(AliITSpListItem &sdig){
    // Adds the a module full of summable digits to the summable digits tree.
    // Inputs:
    //      AliITSpListItem &sdig   SDigit to be added to SDigits tree.
    // Outputs:
    //      none.
    // Return:
    //      none.

    TClonesArray &lsdig = *fSDigits;
    new(lsdig[fNSDigits++]) AliITSpListItem(sdig);
}
//______________________________________________________________________
void AliITS::AddRealDigit(Int_t id, Int_t *digits){
    //   Add a real digit - as coming from data.
    // Inputs:
    //      Int_t id        Detector type number.
    //      Int_t *digits   Integer array containing the digits info. See 
    //                      AliITSdigit.h
    // Outputs:
    //      none.
    // Return:
    //      none.

    TClonesArray &ldigits = *((TClonesArray*)fDtype->At(id));
    new(ldigits[fNdtype[id]++]) AliITSdigit(digits);
}
//______________________________________________________________________
void AliITS::AddSimDigit(Int_t id, AliITSdigit *d){
    //    Add a simulated digit.
    // Inputs:
    //      Int_t id        Detector type number.
    //      AliITSdigit *d  Digit to be added to the Digits Tree. See 
    //                      AliITSdigit.h
    // Outputs:
    //      none.
    // Return:
    //      none.

    TClonesArray &ldigits = *((TClonesArray*)fDtype->At(id));

    switch(id){
    case 0:
        new(ldigits[fNdtype[id]++]) AliITSdigitSPD(*((AliITSdigitSPD*)d));
        break;
    case 1:
        new(ldigits[fNdtype[id]++]) AliITSdigitSDD(*((AliITSdigitSDD*)d));
        break;
    case 2:
        new(ldigits[fNdtype[id]++]) AliITSdigitSSD(*((AliITSdigitSSD*)d));
        break;
    } // end switch id
}
//______________________________________________________________________
void AliITS::AddSimDigit(Int_t id,Float_t phys,Int_t *digits,Int_t *tracks,
                         Int_t *hits,Float_t *charges){
    //   Add a simulated digit to the list.
    // Inputs:
    //      Int_t id        Detector type number.
    //      Float_t phys    Physics indicator. See AliITSdigits.h
    //      Int_t *digits   Integer array containing the digits info. See 
    //                      AliITSdigit.h
    //      Int_t *tracks   Integer array [AliITSdigitS?D::GetNTracks()] 
    //                      containing the track numbers that contributed to
    //                      this digit.
    //      Int_t *hits     Integer array [AliITSdigitS?D::GetNTracks()]
    //                      containing the hit numbers, from AliITSmodule, that
    //                      contributed to this digit.
    //      Float_t *charge Floating point array of the signals contributed
    //                      to this digit by each track.
    // Outputs:
    //      none.
    // Return:
    //      none.

    TClonesArray &ldigits = *((TClonesArray*)fDtype->At(id));
    switch(id){
    case 0:
        new(ldigits[fNdtype[id]++]) AliITSdigitSPD(digits,tracks,hits);
        break;
    case 1:
        new(ldigits[fNdtype[id]++]) AliITSdigitSDD(phys,digits,tracks,
                                                   hits,charges);
        break;
    case 2:
        new(ldigits[fNdtype[id]++]) AliITSdigitSSD(digits,tracks,hits);
        break;
    } // end switch id
}
//______________________________________________________________________
void AliITS::MakeTreeC(Option_t *option){
  //   Create a separate tree to store the clusters.
  // Inputs:
  //      Option_t *option  string which must contain "C" otherwise
  //                        no Cluster Tree is created.
  // Outputs:
  //      none.
  // Return:
  //      none.

  AliITSLoader *pITSLoader = (AliITSLoader*)fLoader;    
    
  if (pITSLoader == 0x0) {
    Error("MakeTreeC","fLoader == 0x0");
    return;
  }
  if (pITSLoader->TreeC() == 0x0) pITSLoader->MakeTree("C");
  MakeBranchC();
}

void AliITS::MakeBranchC()
{
//Makes barnches in treeC
  AliITSLoader *pITSLoader = (AliITSLoader*)fLoader;    
  if (pITSLoader == 0x0) 
   {
    Error("MakeTreeC","fLoader == 0x0");
    return;
   }
  TTree * TC = pITSLoader->TreeC();
  if (TC == 0x0)
   {
     Error("MakeTreeC","Can not get TreeC from Loader");
     return;
   }

  Int_t buffersize = 4000;
  char branchname[30];
  const char *det[3] = {"SPD","SDD","SSD"};
  char digclass[40];
  char clclass[40];

    // one branch for Clusters per type of detector
  Int_t i;   
  for (i=0; i<kNTYPES ;i++) 
    {
        AliITSDetType *iDetType=DetType(i); 
        iDetType->GetClassNames(digclass,clclass);
        // clusters
        if (fCtype == 0x0) fCtype  = new TObjArray(fNDetTypes);
        if(!ClustersAddress(i))
         {
          fCtype->AddAt(new TClonesArray(clclass,1000),i);
         }
        if (kNTYPES==3) sprintf(branchname,"%sClusters%s",GetName(),det[i]);
        else  sprintf(branchname,"%sClusters%d",GetName(),i+1);
        if (fCtype  && TC) 
         {
           if (TC->GetBranch(branchname))
            {
              Warning("MakeBranchC","Branch %s alread exists in TreeC",branchname);
            }
           else
            {
              Info("MakeBranchC","Creating branch %s in TreeC",branchname);
              TC->Branch(branchname,&((*fCtype)[i]), buffersize);
            }
         } // end if fCtype && TC
  } // end for i
}

//______________________________________________________________________
void AliITS::GetTreeC(Int_t event){
  //    Get the clusters tree for this event and set the branch address.
  // Inputs:
  //      Int_t event    Event number for the cluster tree.
  // Outputs:
  //      none.
  // Return:
  //      none.
  char branchname[30];
  const char *det[3] = {"SPD","SDD","SSD"};

  AliITSLoader *pITSLoader = (AliITSLoader*)fLoader;
  TTree * TC = pITSLoader->TreeC();

  ResetClusters();
  if (TC) {
    pITSLoader->CleanRawClusters();
  } // end if TreeC()


  TBranch *branch;

  if (TC) {
    Int_t i;
        char digclass[40];
        char clclass[40];
        for (i=0; i<kNTYPES; i++) {
      AliITSDetType *iDetType=DetType(i); 
      iDetType->GetClassNames(digclass,clclass);
      // clusters
      if (fCtype == 0x0) fCtype  = new TObjArray(fNDetTypes);
      if(!fCtype->At(i)) fCtype->AddAt(new TClonesArray(clclass,1000),i);
      if(kNTYPES==3) sprintf(branchname,"%sClusters%s",GetName(),det[i]);
      else  sprintf(branchname,"%sClusters%d",GetName(),i+1);
      if (fCtype) {
                branch = TC->GetBranch(branchname);
        if (branch) branch->SetAddress(&((*fCtype)[i]));
      } // end if fCtype
        } // end for i
  } else {
        Error("GetTreeC","cannot find Clusters Tree for event:%d",event);
  } // end if TC
}
//______________________________________________________________________
void AliITS::AddCluster(Int_t id, AliITSRawCluster *c){
    //   Add a cluster to the list.
    // Inputs:
    //      Int_t id             Detector type number.
    //      AliITSRawCluster *c  Cluster class to be added to the tree of
    //                           clusters.
    // Outputs:
    //      none.
    // Return:
    //      none.

    TClonesArray &lc = *((TClonesArray*)fCtype->At(id));

    switch(id){
    case 0:
        new(lc[fNctype[id]++]) AliITSRawClusterSPD(*((AliITSRawClusterSPD*)c));
        break;
    case 1:
        new(lc[fNctype[id]++]) AliITSRawClusterSDD(*((AliITSRawClusterSDD*)c));
        break;
    case 2:
        new(lc[fNctype[id]++]) AliITSRawClusterSSD(*((AliITSRawClusterSSD*)c));
        break;
    } // end switch id
}
//______________________________________________________________________
void AliITS::ResetClusters(){
    // Reset number of clusters and the clusters array for ITS.
    // Inputs:
    //      none.
    // Outputs:
    //      none.

    Int_t i;
    for (i=0;i<kNTYPES;i++ ) ResetClusters(i);
}
//______________________________________________________________________
void AliITS::ResetClusters(Int_t i){
    //    Reset number of clusters and the clusters array for this branch.
    // Inputs:
    //      Int_t i        Detector type number.
    // Outputs:
    //      none.
    // Return:
    //      none.

    if (fCtype->At(i))    ((TClonesArray*)fCtype->At(i))->Clear();
    if (fNctype)  fNctype[i]=0;
}
//______________________________________________________________________
void AliITS::MakeBranchR(const char *file, Option_t *opt){
    // Creates Tree branches for the ITS Reconstructed points.
    // Inputs:
    //      cont char *file  File name where RecPoints branch is to be written
    //                       to. If blank it write the SDigits to the same
    //                       file in which the Hits were found.
    // Outputs:
    //      none.
    // Return:
    //      none.
    Int_t buffsz = 4000;
    char branchname[30];

    // only one branch for rec points for all detector types
    Bool_t oFast= (strstr(opt,"Fast")!=0);
    if(oFast){
      sprintf(branchname,"%sRecPointsF",GetName());
    } else {
      sprintf(branchname,"%sRecPoints",GetName());
    }
    
    

    if (fRecPoints && fLoader->TreeR()) {
        if (fRecPoints == 0x0) fRecPoints = new TClonesArray("AliITSRecPoint",1000);
        MakeBranchInTree(fLoader->TreeR(),branchname,&fRecPoints,buffsz,file);
    } // end if
}
//______________________________________________________________________
void AliITS::SetTreeAddressR(TTree *treeR){
    // Set branch address for the Reconstructed points Trees.
    // Inputs:
    //      TTree *treeR   Tree containing the RecPoints.
    // Outputs:
    //      none.
    // Return:
    //      none.
    char branchname[30];

    if(!treeR) return;
    if (fRecPoints == 0x0) fRecPoints = new TClonesArray("AliITSRecPoint",1000);
    TBranch *branch;
    sprintf(branchname,"%sRecPoints",GetName());
    branch = treeR->GetBranch(branchname);
    if (branch) {
      branch->SetAddress(&fRecPoints);
    }
    else {
      sprintf(branchname,"%sRecPointsF",GetName());
      branch = treeR->GetBranch(branchname);
      if (branch) {
        branch->SetAddress(&fRecPoints);
      }
    }
}
//______________________________________________________________________
void AliITS::AddRecPoint(const AliITSRecPoint &r){
    // Add a reconstructed space point to the list
    // Inputs:
    //      const AliITSRecPoint &r RecPoint class to be added to the tree
    //                              of reconstructed points TreeR.
    // Outputs:
    //      none.
    // Return:
    //      none.

    TClonesArray &lrecp = *fRecPoints;
    new(lrecp[fNRecPoints++]) AliITSRecPoint(r);
}
//______________________________________________________________________
void AliITS::HitsToFastRecPoints(Int_t evNumber,Int_t bgrev,Int_t size,
                                  Option_t *opt0,Option_t *opt1,Text_t *flnm){
    // keep galice.root for signal and name differently the file for 
    // background when add! otherwise the track info for signal will be lost !
    // the condition below will disappear when the geom class will be
    // initialized for all versions - for the moment it is only for v5 !
    // Inputs:
    //      Int_t evnt       Event to be processed.
    //      Int_t bgrev      Background Hit tree number.
    //      Int_t size       Size used by InitModules. See InitModules.
    //      Option_t *opt0   Option passed to FillModules. See FillModules.
    //      Option_t *opt1   String indicating if merging hits or not. To
    //                       merge hits set equal to "Add". Otherwise no
    //                       background hits are considered.
    //      Test_t *flnm     File name containing the background hits..
    // Outputs:
    //      none.
    // Return:
    //      none.

    if(!GetITSgeom()) return;
    AliITSgeom *geom = GetITSgeom();

    const char *all = strstr(opt1,"All");
    const char *det[3] ={strstr(opt1,"SPD"),strstr(opt1,"SDD"),
                         strstr(opt1,"SSD")};
    Int_t nmodules;
    InitModules(size,nmodules);
    FillModules(evNumber,bgrev,nmodules,opt0,flnm);

    AliITSsimulation *sim      = 0;
    AliITSDetType    *iDetType = 0;
    AliITSmodule     *mod      = 0;
    Int_t id,module;

    //m.b. : this change is nothing but a nice way to make sure
    //the CPU goes up !
    
    cout<<"HitsToFastRecPoints: N mod = "<<geom->GetIndexMax()<<endl;
    
    for(module=0;module<geom->GetIndexMax();module++)
     {
        id       = geom->GetModuleType(module);
        if (!all && !det[id]) continue;
        iDetType = DetType(id);
        sim      = (AliITSsimulation*)iDetType->GetSimulationModel();
        if (!sim) 
         {
           Error("HitsToFastPoints","The simulation class was not instanciated!");
           exit(1);
         } // end if !sim
        mod      = (AliITSmodule *)fITSmodules->At(module);
        sim->CreateFastRecPoints(mod,module,gRandom);
        cout<<module<<"\r";fflush(0);
        //gAlice->TreeR()->Fill(); 
        TBranch *br=fLoader->TreeR()->GetBranch("ITSRecPointsF");
        br->Fill();
        ResetRecPoints();
    } // end for module

    ClearModules();
    
    fLoader->WriteRecPoints("OVERWRITE");
}
//______________________________________________________________________
void AliITS::Digits2Reco(){
    // Find clusters and reconstruct space points.
    // Inputs:
    //      none.
    // Outputs:
    //      none.

    AliHeader *header=fLoader->GetRunLoader()->GetHeader();
    // to Digits to RecPoints for event in file, all digits in file, and
    // all ITS detectors.
    DigitsToRecPoints(header->GetEvent(),0,fOpt);
}
//______________________________________________________________________
void AliITS::DigitsToRecPoints(Int_t evNumber,Int_t lastentry,Option_t *opt){
  // cluster finding and reconstruction of space points
  // the condition below will disappear when the geom class will be
  // initialized for all versions - for the moment it is only for v5 !
  // 7 is the SDD beam test version
  // Inputs:
  //      Int_t evNumber   Event number to be processed.
  //      Int_t lastentry  Offset for module when not all of the modules
  //                       are processed.
  //      Option_t *opt    String indicating which ITS sub-detectors should
  //                       be processed. If ="All" then all of the ITS
  //                       sub detectors are processed.
  // Outputs:
  //      none.
  // Return:
  //      none.

  if(!GetITSgeom()) return;
  AliITSgeom *geom = GetITSgeom();
    
  const char *all = strstr(opt,"All");
  const char *det[3] = {strstr(opt,"SPD"),strstr(opt,"SDD"),
                        strstr(opt,"SSD")};
  static Bool_t setRec=kTRUE;
  if (setRec) SetDefaultClusterFinders();
  setRec=kFALSE;

  AliITSLoader *pITSloader = (AliITSLoader*)fLoader;
  TTree *treeC=pITSloader->TreeC();
  AliITSClusterFinder *rec     = 0;
  AliITSDetType      *iDetType = 0;
  Int_t id,module,first=0;
  for(module=0;module<geom->GetIndexMax();module++){
        id       = geom->GetModuleType(module);
    if (!all && !det[id]) continue;
        if(det[id]) first = geom->GetStartDet(id);
        iDetType = DetType(id);
        rec = (AliITSClusterFinder*)iDetType->GetReconstructionModel();
    TClonesArray *itsDigits  = this->DigitsAddress(id);
        if (!rec) {
      Error("DigitsToRecPoints",
            "The reconstruction class was not instanciated!");
      exit(1);
        } // end if !rec
        this->ResetDigits();
    TTree *TD = pITSloader->TreeD();
        if (all) {
      TD->GetEvent(lastentry+module);
    }
    else {
      TD->GetEvent(lastentry+(module-first));
    }
        Int_t ndigits = itsDigits->GetEntriesFast();
        if (ndigits) rec->FindRawClusters(module);
    pITSloader->TreeR()->Fill(); 
        ResetRecPoints();
        treeC->Fill();
    ResetClusters();
  } // end for module


  pITSloader->WriteRecPoints("OVERWRITE");
  pITSloader->WriteRawClusters("OVERWRITE");
}
//______________________________________________________________________
void AliITS::ResetRecPoints(){
    // Reset number of rec points and the rec points array.
    // Inputs:
    //      none.
    // Outputs:
    //      none.

    if (fRecPoints) fRecPoints->Clear();
    fNRecPoints = 0;
}
//______________________________________________________________________
AliLoader* AliITS::MakeLoader(const char* topfoldername)
{ 
  //builds ITSgetter (AliLoader type)
  //if detector wants to use castomized getter, it must overload this method

  cout<<"AliITS::MakeLoader: Creating standard getter for detector "<<GetName()
      <<". top folder is "<<topfoldername<<endl;
     
  fLoader = new AliITSLoader(GetName(),topfoldername);
  return fLoader;
 
}


