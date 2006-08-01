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

// Event generator that using an instance of type AliGenReader
// reads particles from a file and applies cuts.
// Example: In your Config.C you can include the following lines
//  AliGenExtFile *gener = new AliGenExtFile(-1);
//  gener->SetMomentumRange(0,999);
//  gener->SetPhiRange(-180.,180.);
//  gener->SetThetaRange(0,180);
//  gener->SetYRange(-999,999);
//  AliGenReaderTreeK * reader = new AliGenReaderTreeK();
//  reader->SetFileName("myFileWithTreeK.root");
//  gener->SetReader(reader);
//  gener->Init();


#include <Riostream.h>

#include "AliGenExtFile.h"
#include "AliRunLoader.h"
#include "AliHeader.h"
#include "AliGenEventHeader.h"

#include <TParticle.h>
#include <TFile.h>
#include <TTree.h>


ClassImp(AliGenExtFile)

AliGenExtFile::AliGenExtFile()
  :AliGenMC()
{
//  Constructor
//
//  Read all particles
    fNpart  =- 1;
    fReader =  0;
}

AliGenExtFile::AliGenExtFile(Int_t npart)
    :AliGenMC(npart)
{
//  Constructor
    fName   = "ExtFile";
    fTitle  = "Primaries from ext. File";
    fReader = 0;
}

AliGenExtFile::AliGenExtFile(const AliGenExtFile & ExtFile):
    AliGenMC(ExtFile)
{
// Copy constructor
    ExtFile.Copy(*this);
}
//____________________________________________________________
AliGenExtFile::~AliGenExtFile()
{
// Destructor
    delete fReader;
}

//___________________________________________________________
void AliGenExtFile::Init()
{
// Initialize
    if (fReader) fReader->Init();
}

    
void AliGenExtFile::Generate()
{
// Generate particles

  Float_t polar[3]  = {0,0,0};
  //
  Float_t origin[3] = {0,0,0};
  Float_t p[3];
  Float_t random[6];
  Int_t i = 0, j, nt;
  //
  //
  if (fVertexSmear == kPerEvent) Vertex();

  while(1) {
    Int_t nTracks = fReader->NextEvent(); 	
    if (nTracks == 0) {
      // printf("\n No more events !!! !\n");
      Warning("AliGenExtFile::Generate","\nNo more events in external file!!!\nLast event may be empty or incomplete.\n");
      return;
    }

    // Generated event header

    AliRunLoader * inRunLoader = fReader->GetRunLoader();
    if(inRunLoader) {
      AliGenEventHeader * inHeader = inRunLoader->GetHeader()->GenEventHeader();
      if (inHeader) {
	AliGenEventHeader * header = new AliGenEventHeader(*inHeader);
	AliRunLoader::GetRunLoader()->GetHeader()->SetGenEventHeader(header);
      }
    }
    //
    // Particle selection loop
    //
    // The selection criterium for the external file generator is as follows:
    //
    // 1) All tracks are subject to the cuts defined by AliGenerator, i.e.
    //    fThetaMin, fThetaMax, fPhiMin, fPhiMax, fPMin, fPMax, fPtMin, fPtMax,
    //    fYMin, fYMax.
    //    If the particle does not satisfy these cuts, it is not put on the
    //    stack.
    // 2) If fCutOnChild and some specific child is selected (e.g. if
    //    fForceDecay==kSemiElectronic) the event is rejected if NOT EVEN ONE
    //    child falls into the child-cuts.
    TParticle* iparticle = 0x0;
    
    if(fCutOnChild) {
      // Count the selected children
      Int_t nSelected = 0;
      while ((iparticle=fReader->NextParticle()) ) {
	  Int_t kf = CheckPDGCode(iparticle->GetPdgCode());
	  kf = TMath::Abs(kf);
	  if (ChildSelected(kf) && KinematicSelection(iparticle, 1)) {
	      nSelected++;
	  }
      }
      if (!nSelected) continue;    // No particle selected:  Go to next event
      fReader->RewindEvent();
    }

    //
    // Stack filling loop
    //
    for (i = 0; i < nTracks; i++) {
	TParticle* iparticle = fReader->NextParticle();
	Bool_t selected = KinematicSelection(iparticle,0); 
	if (!selected) continue;
	p[0] = iparticle->Px();
	p[1] = iparticle->Py();
	p[2] = iparticle->Pz();
	Int_t idpart = iparticle->GetPdgCode();
	if(fVertexSmear==kPerTrack) 
	{
	    Rndm(random,6);
	    for (j = 0; j < 3; j++) {
		origin[j]=fOrigin[j]+
		    fOsigma[j]*TMath::Cos(2*random[2*j]*TMath::Pi())*
		    TMath::Sqrt(-2*TMath::Log(random[2*j+1]));
	    }
	} else {
	    origin[0] = fVertex[0] + iparticle->Vx();
	    origin[1] = fVertex[1] + iparticle->Vy();
	    origin[2] = fVertex[2] + iparticle->Vz();
	}
	
	Int_t decayed    = iparticle->GetFirstDaughter();
	Int_t doTracking = fTrackIt && (decayed < 0) && (TMath::Abs(idpart) > 10) && selected;
	Int_t parent     = iparticle->GetFirstMother();
	
	PushTrack(doTracking,parent,idpart,p,origin,polar,0,kPPrimary,nt);
	KeepTrack(nt);
    } // track loop
    
    break;
    
  } // event loop
  
  SetHighWaterMark(nt);
  CdEventFile();
}

void AliGenExtFile::CdEventFile()
{
// CD back to the event file
    (AliRunLoader::GetRunLoader())->CdGAFile();
}


AliGenExtFile& AliGenExtFile::operator=(const  AliGenExtFile& rhs)
{
// Assignment operator
    rhs.Copy(*this);
    return *this;
}
 

void AliGenExtFile::Copy(TObject&) const
{
    //
    // Copy 
    //
    Fatal("Copy","Not implemented!\n");
}







