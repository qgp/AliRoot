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
Revision 1.2.2.2  2002/07/26 18:34:02  alibrary
Updating VirtualMC

Revision 1.3  2002/07/26 15:32:24  hristov
stream.h doesn't exest on Sun, removed from includes

Revision 1.2  2002/07/19 11:43:10  morsch
- Write full stack.
- Use SetTrack passing energy.

Revision 1.1  2002/07/16 11:33:26  morsch
First commit.

*/



// Generator using Herwig as an external generator
// The main Herwig options are accessable for the user through this interface.
// Uses the THerwig implementation of TGenerator.

#include "AliGenHerwig.h"
#include "AliRun.h"

#include <TParticle.h>
#include "THerwig6.h"

#include "Riostream.h"

 ClassImp(AliGenHerwig)

AliGenHerwig::AliGenHerwig()
{
// Constructor
}

AliGenHerwig::AliGenHerwig(Int_t npart)
    :AliGenMC(npart)
{
    SetBeamMomenta();
    SetTarget();
    SetProjectile();
    SetStrucFunc(kGRVLO98);
    fKeep=0;
    fTrigger=0;
    fDecaysOff=1;
    fSelectAll=0;
    fFlavor=0;
    fPtHardMin=10.;
    fPtRMS=0.0;
    fEnSoft=1.0;
    fMaxPr=1;
    fMaxErrors=1000;
//  Set random number
    if (!sRandom) sRandom=fRandom;
}

AliGenHerwig::AliGenHerwig(const AliGenHerwig & Herwig)
{
// copy constructor
}


AliGenHerwig::~AliGenHerwig()
{
// Destructor
}

void AliGenHerwig::Init()
{
// Initialisation
  fTarget.Resize(8);
  fProjectile.Resize(8);
  SetMC(new THerwig6());
  fHerwig=(THerwig6*) fgMCEvGen;
  // initialize common blocks
  fHerwig->Initialize(fProjectile, fTarget, fMomentum1, fMomentum2, fProcess);
  // reset parameters according to user needs
  InitPDF();
  fHerwig->SetPTMIN(fPtHardMin);
  fHerwig->SetPTRMS(fPtRMS);
  fHerwig->SetMAXPR(fMaxPr);
  fHerwig->SetMAXER(fMaxErrors);
  fHerwig->SetENSOF(fEnSoft);
  // compute parameter dependent constants
  fHerwig->PrepareRun();
}

void AliGenHerwig::InitPDF()
{
  switch(fStrucFunc)
    {
    case kGRVLO:
      fModPDF=5;
      fAutPDF="GRV";
      break;
    case kGRVHO:
      fModPDF=6;
      fAutPDF="GRV";
      break;
    case kGRVLO98:
      fModPDF=12;
      fAutPDF="GRV";
      break;
    case kMRSDminus:
      fModPDF=31;
      fAutPDF="MRS";
      break;
    case kMRSD0:
      fModPDF=30;
      fAutPDF="MRS";
      break;
    case kMRSG:
      fModPDF=41;
      fAutPDF="MRS";
      break;
    case kMRSTcgLO:
      fModPDF=72;
      fAutPDF="MRS";
      break;
    case kCTEQ4M:
      fModPDF=34;
      fAutPDF="CTEQ";
      break;
    case kCTEQ5L:
      fModPDF=46;
      fAutPDF="CTEQ";
      break;
    default:
      cerr << "This structure function is not inplemented " << fStrucFunc << endl;
      break;
    }
  fAutPDF.Resize(20);      
  fHerwig->SetMODPDF(1,fModPDF);
  fHerwig->SetMODPDF(2,fModPDF);
  fHerwig->SetAUTPDF(1,fAutPDF);
  fHerwig->SetAUTPDF(2,fAutPDF);
}

void AliGenHerwig::Generate()
{
  // Generate one event

  Float_t polar[3] =   {0,0,0};
  Float_t origin[3]=   {0,0,0};
  Float_t origin0[3]=  {0,0,0};
  Float_t p[4], random[6];
  
  static TClonesArray *particles;
  //  converts from mm/c to s
  const Float_t kconv=0.001/2.999792458e8;
  //
  Int_t nt=0;
  Int_t jev=0;
  Int_t j, kf, ks, imo;
  kf=0;
  
  if(!particles) particles=new TClonesArray("TParticle",10000);
  
  fTrials=0;
  for (j=0;j<3;j++) origin0[j]=fOrigin[j];
  if(fVertexSmear==kPerEvent) {
    Rndm(random,6);
    for (j=0;j<3;j++) {
      origin0[j]+=fOsigma[j]*TMath::Cos(2*random[2*j]*TMath::Pi())*
	TMath::Sqrt(-2*TMath::Log(random[2*j+1]));
    }
  }
  
  while(1)
    {
	fHerwig->GenerateEvent();
	fTrials++;
	fHerwig->ImportParticles(particles,"All");
	Int_t np = particles->GetEntriesFast()-1;
	if (np == 0 ) continue;
	
	Int_t nc=0;
	
	Int_t * newPos = new Int_t[np];
	for (Int_t i = 0; i<np; i++) *(newPos+i)=-1;
	
	for (Int_t i = 0; i<np; i++) {
	    TParticle *  iparticle       = (TParticle *) particles->At(i);
	    imo = iparticle->GetFirstMother();
	    kf        = iparticle->GetPdgCode();
	    ks        = iparticle->GetStatusCode();
	    if (ks != 3 && 
		KinematicSelection(iparticle,0)) 
	    {
		nc++;
		p[0]=iparticle->Px();
		p[1]=iparticle->Py();
		p[2]=iparticle->Pz();
		p[3]=iparticle->Energy();
		origin[0]=origin0[0]+iparticle->Vx()/10;
		origin[1]=origin0[1]+iparticle->Vy()/10;
		origin[2]=origin0[2]+iparticle->Vz()/10;
		Float_t tof = kconv*iparticle->T();
		Int_t   iparent = (imo > -1) ? newPos[imo] : -1;
		Int_t   trackIt = (ks == 1) && fTrackIt;
		gAlice->SetTrack(trackIt, iparent, kf,
				 p[0], p[1], p[2], p[3],
				 origin[0], origin[1], origin[2], 
				 tof,
				 polar[0], polar[1], polar[2],
				 kPPrimary, nt, 1., ks);
		KeepTrack(nt);
		newPos[i]=nt;
	    } // end of if: selection of particle
	} // end of for: particle loop
	if (newPos) delete[] newPos;
	printf("\n I've put %i particles on the stack \n",nc);
	//      MakeHeader();
	printf("nc: %d %d\n", nc, fNpart);
	
	if (nc > 0) {
	    jev+=nc;
	    if (jev >= fNpart || fNpart == -1) {
		fKineBias=Float_t(fNpart)/Float_t(fTrials);
		printf("\n Trials: %i %i %i\n",fTrials, fNpart, jev);
		break;
	    }
	}
    }
  SetHighWaterMark(nt);
//  adjust weight due to kinematic selection
  AdjustWeights();
//  get cross-section
  fXsection=fHerwig->GetAVWGT();
}

void AliGenHerwig::AdjustWeights()
{
// Adjust the weights after generation of all events
    TParticle *part;
    Int_t ntrack=gAlice->GetNtrack();
    for (Int_t i=0; i<ntrack; i++) {
        part= gAlice->Particle(i);
        part->SetWeight(part->GetWeight()*fKineBias);
    }
}
 

void AliGenHerwig::KeepFullEvent()
{
    fKeep=1;
}

Bool_t AliGenHerwig::DaughtersSelection(TParticle* iparticle, TClonesArray* particles)
{
//
// Looks recursively if one of the daughters has been selected
//
//    printf("\n Consider daughters %d:",iparticle->GetPdgCode());
    Int_t imin=-1;
    Int_t imax=-1;
    Int_t i;
    Bool_t hasDaughters= (iparticle->GetFirstDaughter() >=0);
    Bool_t selected=kFALSE;
    if (hasDaughters) {
	imin=iparticle->GetFirstDaughter();
	imax=iparticle->GetLastDaughter();       
	for (i=imin; i<= imax; i++){
	    TParticle *  jparticle       = (TParticle *) particles->At(i);	
	    Int_t ip=jparticle->GetPdgCode();
	    if (KinematicSelection(jparticle,0)&&SelectFlavor(ip)) {
		selected=kTRUE; break;
	    }
	    if (DaughtersSelection(jparticle, particles)) {selected=kTRUE; break; }
	}
    } else {
	return kFALSE;
    }

    return selected;
}


Bool_t AliGenHerwig::SelectFlavor(Int_t pid)
{
// Select flavor of particle
// 0: all
// 4: charm and beauty
// 5: beauty
    if (fFlavor == 0) return kTRUE;
    
    Int_t ifl=TMath::Abs(pid/100);
    if (ifl > 10) ifl/=10;
    return (fFlavor == ifl);
}

Bool_t AliGenHerwig::Stable(TParticle*  particle)
{
// Return true for a stable particle
//
    Int_t kf = TMath::Abs(particle->GetPdgCode());
    
    if ( (particle->GetFirstDaughter() < 0 ) || (kf == 1000*fFlavor+122))
	 
    {
	return kTRUE;
    } else {
	return kFALSE;
    }
}

void AliGenHerwig::FinishRun()
{
  fHerwig->Hwefin();
}


AliGenHerwig& AliGenHerwig::operator=(const  AliGenHerwig& rhs)
{
// Assignment operator
    return *this;
}


extern "C" {
  Double_t hwr_() {return sRandom->Rndm();}
}
