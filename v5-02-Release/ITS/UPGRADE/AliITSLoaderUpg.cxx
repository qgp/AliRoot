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

/* $Id: AliITSLoaderUpg.cxx $ */

#include <TClonesArray.h>
#include <TClass.h>
#include <TGeoManager.h>
#include <TTree.h>

#include "AliITSdigit.h"
#include "AliITSLoaderUpg.h"
#include "AliRunLoader.h"
#include "AliObjectLoader.h"
#include "AliITSInitGeometryUpg.h"
#include "AliLog.h"

///////////////////////////////////////////////////////////////////////////
// Loader for ITS Upgrade
// it manages the I/O for:
// raw clusters, primary vertices
// V0 and cascade
// and tracks propagated to the origin
//////////////////////////////////////////////////////////////////////////
ClassImp(AliITSLoaderUpg)

/**********************************************************************/
  AliITSLoaderUpg::AliITSLoaderUpg():AliLoader(),
fGeom(0){
  // Default constructor
}
/*********************************************************************/
AliITSLoaderUpg::AliITSLoaderUpg(const Char_t *name,const Char_t *topfoldername):
AliLoader(name,topfoldername),
fGeom(0){
  //Constructor   
  TString rawContainerName = "TreeC";
    AliDataLoader* rawClustersDataLoader = new AliDataLoader(
        fDetectorName + ".RawCl.root",rawContainerName,
        "Raw Clusters");
    fDataLoaders->Add(rawClustersDataLoader);
    rawClustersDataLoader->SetEventFolder(fEventFolder);
    rawClustersDataLoader->SetFolder(GetDetectorDataFolder());

  TString  backTracksContainerName= "TreeB";
    AliDataLoader* backTracksDataLoader =  new AliDataLoader(
        fDetectorName + ".BackTracks.root",backTracksContainerName,
        "Back Propagated Tracks");
    fDataLoaders->Add(backTracksDataLoader);
    backTracksDataLoader->SetEventFolder(fEventFolder);
    backTracksDataLoader->SetFolder(GetDetectorDataFolder());
  TString verticesContainerName="Vertex"; 
    AliDataLoader* vertexDataLoader = new AliDataLoader(
        fDetectorName + ".Vertex.root",verticesContainerName,
        "Primary Vertices","O");
    fDataLoaders->Add(vertexDataLoader);
    vertexDataLoader->SetEventFolder(fEventFolder);
    vertexDataLoader->SetFolder(GetDetectorDataFolder());
 TString v0ContainerName="V0";
    AliDataLoader* v0DataLoader = new AliDataLoader(
        fDetectorName + ".V0s.root",v0ContainerName,"V0 Vertices");
    fDataLoaders->Add(v0DataLoader);
    v0DataLoader->SetEventFolder(fEventFolder);
    v0DataLoader->SetFolder(GetDetectorDataFolder());
  TString cascadeContainerName="Cascade";
    AliDataLoader* cascadeDataLoader = new AliDataLoader(
        fDetectorName + ".Cascades.root",cascadeContainerName,
        "Cascades");
    fDataLoaders->Add(cascadeDataLoader);
    cascadeDataLoader->SetEventFolder(fEventFolder);
    cascadeDataLoader->SetFolder(GetDetectorDataFolder());
}
/**********************************************************************/
AliITSLoaderUpg::AliITSLoaderUpg(const Char_t *name,TFolder *topfolder): 
  AliLoader(name,topfolder),
fGeom(0){
  //ctor  
   TString rawContainerName="TreeC";
    AliDataLoader*  rawClustersDataLoader = new AliDataLoader(
        fDetectorName + ".RawCl.root",rawContainerName,
        "Raw Clusters"); 
    fDataLoaders->Add(rawClustersDataLoader);
    rawClustersDataLoader->SetEventFolder(fEventFolder);
    rawClustersDataLoader->SetFolder(GetDetectorDataFolder());
   TString backTracksContainerName="TreeB";
    AliDataLoader*  backTracksDataLoader =  new AliDataLoader(
        fDetectorName + ".BackTracks.root",backTracksContainerName,
        "Back Propagated Tracks");
    fDataLoaders->Add(backTracksDataLoader);
    backTracksDataLoader->SetEventFolder(fEventFolder);
    backTracksDataLoader->SetFolder(GetDetectorDataFolder());
  TString verticesContainerName="Vertex";
    AliDataLoader* vertexDataLoader = new AliDataLoader(
        fDetectorName + ".Vertex.root",verticesContainerName,
        "Primary Vertices","O");
    fDataLoaders->Add(vertexDataLoader);
    vertexDataLoader->SetEventFolder(fEventFolder);
    vertexDataLoader->SetFolder(GetDetectorDataFolder());
  TString v0ContainerName="V0";
    AliDataLoader* v0DataLoader = new AliDataLoader(
        fDetectorName + ".V0.root",v0ContainerName,"V0 Vertices");
    fDataLoaders->Add(v0DataLoader);
    v0DataLoader->SetEventFolder(fEventFolder);
    v0DataLoader->SetFolder(GetDetectorDataFolder());
  TString cascadeContainerName="Cascade";
    AliDataLoader* cascadeDataLoader = new AliDataLoader(
        fDetectorName + ".Cascade.root",cascadeContainerName,
        "Cascades");
    fDataLoaders->Add(cascadeDataLoader);
    cascadeDataLoader->SetEventFolder(fEventFolder);
    cascadeDataLoader->SetFolder(GetDetectorDataFolder());
}


/**********************************************************************/
AliITSLoaderUpg::~AliITSLoaderUpg(){
    //destructor
    UnloadRawClusters();
    AliDataLoader *dl = GetRawClLoader();
    fDataLoaders->Remove(dl);

    UnloadBackTracks();
    dl = GetBackTracksDataLoader();
    fDataLoaders->Remove(dl);

    UnloadVertices();
    dl = GetVertexDataLoader();
    fDataLoaders->Remove(dl);

    UnloadV0s();
    dl = GetV0DataLoader();
    fDataLoaders->Remove(dl);

    UnloadCascades();
    dl = GetCascadeDataLoader();
    fDataLoaders->Remove(dl);
  
    if(fGeom)delete fGeom;
    fGeom = 0;
}
/*
//----------------------------------------------------------------------
AliITS* AliITSLoaderUpg::GetITS(){
    // Returns the pointer to the ITS, kept on the file. A short cut metthod
    // Inputs:
    //    none.
    // Outputs:
    //    none.
    // Returns:
    //    Returns a pointer to the ITS, if not found returns 0.
    AliITS *its;

    if(gAlice){
        its = dynamic_cast<AliITS*> (gAlice->GetDetector(
            GetDetectorName().Data()));
        if(its) return its;
    } // end if gAlice
    AliRunLoader *rl=0;
    rl = GetRunLoader();
    if(!rl) return 0;
    AliRun *ar=0;
    ar = rl->GetAliRun();
    if(!ar) return 0;
    its = dynamic_cast<AliITS*> (ar->GetDetector(GetDetectorName().Data()));
    return its;
}
//----------------------------------------------------------------------
void AliITSLoaderUpg::SetupDigits(AliITS *its){
    // Sets up to store ITS Digits in side AliITS::fDtype TObjArray
    // Inputs:
    //    AliITS *its  Pointer to the ITS
    // Outputs:
    //    none.
    // Return:
    //    none.

    its->SetTreeAddressD(TreeD());
}
*/
//----------------------------------------------------------------------
void AliITSLoaderUpg::SetupDigits(TObjArray *digPerDet,Int_t n,
				  const Char_t **digclass){
    // Sets up digPerDet to store ITS Digits.
    // Inputs:
    //    TObjArray *digPerDet   A pointer to a TObject Array size>=3.
    //    Int_t      n           The size of the TObjArray and digclass array
    //    Char_t     **digclass  Array of digit class names
    // Outputs:
    //    TObjArray *digPerDet   Setup and linked to the tree of digits
    // Return:
    //    none.
    Int_t i,m;
    TClonesArray *cl = 0;
    TTree *td = 0;
    TBranch *br = 0;
    Char_t branch[14];
    const Char_t *det[3] = {"SPD","SDD","SSD"};

    if(!digPerDet){
        Error("SetUpDigits","TObject Array digPerDet does not exist");
        return;
    } // end if
    m = digPerDet->GetSize();
    if(m<n){
        Error("SetUpDigits","TObject Array digPerDet=%p must have a size"
              " at least that of n=%d",digPerDet,n);
    } // end if
    if(m<3){
        Error("SetUpDigits","TObject Array digPerDet=%p must have a size >2",
              digPerDet);
        return;
    } // end if
    td = TreeD();
    for(i=0;i<n;i++){
        if(digPerDet->At(i)==0){ // set up TClones Array
            digPerDet->AddAt(new TClonesArray(digclass[i],1000),i);
            if(n==3) snprintf(branch,13,"ITSDigits%s",det[i]);
            else     snprintf(branch,13,"ITSDigits%d",i+1);
            br = td->GetBranch(branch);
            br->SetAddress(&((*digPerDet)[i]));
            continue; // do next one.
        } // end if
        cl =  dynamic_cast<TClonesArray*> (digPerDet->At(i));
        if(!cl && digPerDet->At(i)!=0){  // not a TClonesArray
            Error("SetUpDigits","TObject Array digPerDet-At(%d)=%p must be "
                  "zeroed or filled with TClonesArrays",i,digPerDet);
            return;
        } // end if
        if(!(cl->GetClass()->GetBaseClass(AliITSdigit::Class()))){
            Error("SetUPDigits","TClones array at digPerDet[%d}=%p must be"
                  "derived from AliITSdigit",i,digPerDet->At(i));
        } // end if
        cl->Clear();
        if(n==3) snprintf(branch,13,"ITSDigits%s",det[i]);
        else     snprintf(branch,13,"ITSDigits%d",i+1);
        br = td->GetBranch(branch);
        br->SetAddress(&((*digPerDet)[i]));
        continue;
    } // end for i
}
//---------------------------------------------------------------------
AliITSdigit * AliITSLoaderUpg::GetDigit(TObjArray *digPerDet,Int_t module,
					Int_t digit){
    // Gets the digit for for a specific detector type and module.
    // To be used in conjustion with Setupdigits(AliITS *its).
    // Inputs:
    //   TObjArray *digPereDet    Pointer to the Array of digits
    //   Int_t      module        Module number
    //   Int_t      digit         Digit number
    // Outputs:
    //   none.
    // Return:
    //   returns the pointer to the digit. if zero then last digit for that
    //   module.

    if(digPerDet==0){
        Error("GetDigit","digPerDet=%p, module=%d, digit=%d",
              digPerDet,module,digit);
        return 0;
    } // end if
    return 0;
}
/*
//---------------------------------------------------------------------
AliITSdigit * AliITSLoaderUpg::GetDigit(AliITS *its,Int_t module,Int_t digit){
    // Gets the digit for for a specific detector type and module.
    // To be used in conjustion with Setupdigits(AliITS *its).
    // Inputs:
    //   AliITS *its    Pointer to the ITS
    //   Int_t  module  Module number
    //   Int_t digit    Digit number
    // Outputs:
    //   none.
    // Return:
    //   returns the pointer to the digit. if zero then last digit for that
    //   module.
    //AliITSDetType *idtype;
    AliITSgeom *geom = its->GetITSgeom();
    Int_t idet = geom->GetModuleType(module);
    TClonesArray *digits;
 
    its->ResetDigits();
    TreeD()->GetEvent(module);
    digits = its->DigitsAddress(idet);
    if(digit>-1 && digit<digits->GetEntriesFast()){ // if in range.
        return (AliITSdigit*) digits->At(digit);
    } // end if
    return 0;
}
*/
//----------------------------------------------------------------------
void AliITSLoaderUpg::MakeTree(Option_t *opt){
    // invokes AliLoader::MakeTree + specific ITS tree(s)
    // Valid options: H,S,D,R,T and C (C=raw clusters)
    AliLoader::MakeTree(opt);
    const char *oC = strstr(opt,"C");
    if (oC) MakeRawClustersContainer();

    const char *oB = strstr(opt,"B");
    if (oB) MakeBackTracksContainer();

    const char *oV0 = strstr(opt,"V0");
    if (oV0) MakeV0Container();

    const char *oX = strstr(opt,"X");
    if (oX) MakeCascadeContainer();
}

//----------------------------------------------------------------------
AliITSgeom* AliITSLoaderUpg::GetITSgeom(Bool_t force) {
  // retrieves the ITS geometry from file
  if(fGeom && !force)return fGeom;
  if(fGeom && force){
    delete fGeom;
    fGeom = 0;
  }
  if(!gGeoManager){
    AliError("gGeoManager is a null pointer - ITS geometry not built");
    return fGeom;
  }
  AliITSInitGeometryUpg initgeom;
  fGeom = initgeom.CreateAliITSgeom();
  AliDebug(1,"AliITSgeom object has been initialized from TGeo\n");
  AliDebug(1,Form("Geometry name: %s",(initgeom.GetGeometryName()).Data()));
  return fGeom;
}
//______________________________________________________________________
void AliITSLoaderUpg::SetITSgeom(AliITSgeom *geom){
    // Replaces the AliITSgeom object read from file with the one
    // given.
    // Inputs:
    //   AliITSgeom *geom   The AliITSgeom object to replace the one
    //                      read from the file
    // Outputs:
    //   none.
    // Return:
    //   none.

    if(fGeom==geom) return; // Same do nothing
    if(fGeom) {
	delete fGeom;
	fGeom=0;
    }// end if
    fGeom=geom;
}
