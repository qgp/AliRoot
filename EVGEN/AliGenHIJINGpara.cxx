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
Revision 1.21  2003/03/15 19:48:01  morsch
AliDecayerPythia replaced by AliDecayer

Revision 1.20  2003/01/28 15:29:07  morsch
Spelling in name corrected.

Revision 1.19  2003/01/14 10:50:18  alibrary
Cleanup of STEER coding conventions

Revision 1.18  2002/12/11 11:58:11  morsch
Bug in formula for pi0 energy for decay corrected.

Revision 1.17  2002/12/10 17:44:57  morsch
Correct mother child relation for pi0.

Revision 1.16  2002/11/28 11:46:15  morsch
Don't track pi0 if already decayed.

Revision 1.15  2002/11/28 11:38:53  morsch
Typo corrected.

Revision 1.14  2002/11/26 17:12:36  morsch
Decay pi0 if requested.

Revision 1.13  2002/10/14 14:55:35  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.11.4.1  2002/07/24 08:56:28  alibrary
Updating EVGEN on TVirtulaMC

Revision 1.12  2002/06/19 06:56:34  hristov
Memory leak corrected

Revision 1.11  2002/03/20 10:21:13  hristov
Set fPtMax to 15 GeV in order to avoid some numerical problems

Revision 1.10  2001/10/15 16:44:46  morsch
- Possibility for vertex distribution truncation.
- Write mc header with vertex position.

Revision 1.9  2001/07/27 17:09:36  morsch
Use local SetTrack, KeepTrack and SetHighWaterMark methods
to delegate either to local stack or to stack owned by AliRun.
(Piotr Skowronski, A.M.)

Revision 1.8  2001/07/20 11:03:58  morsch
Issue warning message if used outside allowed eta range (-8 to 8).

Revision 1.7  2001/07/17 12:41:01  morsch
- Calculation of fraction of event corresponding to selected pt-range corrected
(R. Turrisi)
- Parent weight corrected.

Revision 1.6  2001/05/16 14:57:10  alibrary
New files for folders and Stack

Revision 1.5  2000/12/21 16:24:06  morsch
Coding convention clean-up

Revision 1.4  2000/11/30 07:12:50  alibrary
Introducing new Rndm and QA classes

Revision 1.3  2000/10/02 21:28:06  fca
Removal of useless dependecies via forward declarations

Revision 1.2  2000/07/11 18:24:55  fca
Coding convention corrections + few minor bug fixes

Revision 1.1  2000/06/09 20:20:30  morsch
Same class as previously in AliSimpleGen.cxx
All coding rule violations except RS3 corrected (AM)

*/

// Parameterisation of pi and K, eta and pt distributions
// used for the ALICE TDRs.
// eta: according to HIJING (shadowing + quenching)
// pT : according to CDF measurement at 1.8 TeV
// Author: andreas.morsch@cern.ch


//Begin_Html
/*
<img src="picts/AliGeneratorClass.gif">
</pre>
<br clear=left>
<font size=+2 color=red>
<p>The responsible person for this module is
<a href="mailto:andreas.morsch@cern.ch">Andreas Morsch</a>.
</font>
<pre>
*/
//End_Html
//                                                               //
///////////////////////////////////////////////////////////////////

#include <TArrayF.h>
#include <TClonesArray.h>
#include <TDatabasePDG.h>
#include <TF1.h>
#include <TParticle.h>
#include <TPDGCode.h>

#include "AliConst.h"
#include "AliDecayer.h"
#include "AliGenEventHeader.h"
#include "AliGenHIJINGpara.h"
#include "AliRun.h"

ClassImp(AliGenHIJINGpara)

AliGenHIJINGpara::AliGenHIJINGpara(const AliGenHIJINGpara & para)
{
// copy constructor
}

//_____________________________________________________________________________
static Double_t ptpi(Double_t *px, Double_t *)
{
  //
  //     PT-PARAMETERIZATION CDF, PRL 61(88) 1819
  //     POWER LAW FOR PT > 500 MEV
  //     MT SCALING BELOW (T=160 MEV)
  //
  const Double_t kp0 = 1.3;
  const Double_t kxn = 8.28;
  const Double_t kxlim=0.5;
  const Double_t kt=0.160;
  const Double_t kxmpi=0.139;
  const Double_t kb=1.;
  Double_t y, y1, xmpi2, ynorm, a;
  Double_t x=*px;
  //
  y1=TMath::Power(kp0/(kp0+kxlim),kxn);
  xmpi2=kxmpi*kxmpi;
  ynorm=kb*(TMath::Exp(-sqrt(kxlim*kxlim+xmpi2)/kt));
  a=ynorm/y1;
  if (x > kxlim)
    y=a*TMath::Power(kp0/(kp0+x),kxn);
  else
    y=kb*TMath::Exp(-sqrt(x*x+xmpi2)/kt);
  return y*x;
}

//_____________________________________________________________________________
static Double_t ptscal(Double_t pt, Int_t np)
{
    //    SCALING EN MASSE PAR RAPPORT A PTPI
    //     MASS PI,K,ETA,RHO,OMEGA,ETA',PHI
    const Double_t khm[10] = {.13957,.493,.5488,.769,.7826,.958,1.02,0,0,0};
    //     VALUE MESON/PI AT 5 GEV
    const Double_t kfmax[10]={1.,0.3,0.55,1.0,1.0,1.0,1.0,0,0,0};
    np--;
    Double_t f5=TMath::Power(((
	sqrt(100.018215)+2.)/(sqrt(100.+khm[np]*khm[np])+2.0)),12.3);
    Double_t fmax2=f5/kfmax[np];
    // PIONS
    Double_t ptpion=100.*ptpi(&pt, (Double_t*) 0);
    Double_t fmtscal=TMath::Power(((
	sqrt(pt*pt+0.018215)+2.)/ (sqrt(pt*pt+khm[np]*khm[np])+2.0)),12.3)/ 
	fmax2;
    return fmtscal*ptpion;
}

//_____________________________________________________________________________
static Double_t ptka( Double_t *px, Double_t *)
{
    //
    // pt parametrisation for k
    //
    return ptscal(*px,2);
}


//_____________________________________________________________________________
static Double_t etapic( Double_t *py, Double_t *)
{
  //
  // eta parametrisation for pi
  //
    const Double_t ka1    = 4913.;
    const Double_t ka2    = 1819.;
    const Double_t keta1  = 0.22;
    const Double_t keta2  = 3.66;
    const Double_t kdeta1 = 1.47;
    const Double_t kdeta2 = 1.51;
    Double_t y=TMath::Abs(*py);
    //
    Double_t ex1 = (y-keta1)*(y-keta1)/(2*kdeta1*kdeta1);
    Double_t ex2 = (y-keta2)*(y-keta2)/(2*kdeta2*kdeta2);
    return ka1*TMath::Exp(-ex1)+ka2*TMath::Exp(-ex2);
}

//_____________________________________________________________________________
static Double_t etakac( Double_t *py, Double_t *)
{
    //
    // eta parametrisation for ka
    //
    const Double_t ka1    = 497.6;
    const Double_t ka2    = 215.6;
    const Double_t keta1  = 0.79;
    const Double_t keta2  = 4.09;
    const Double_t kdeta1 = 1.54;
    const Double_t kdeta2 = 1.40;
    Double_t y=TMath::Abs(*py);
    //
    Double_t ex1 = (y-keta1)*(y-keta1)/(2*kdeta1*kdeta1);
    Double_t ex2 = (y-keta2)*(y-keta2)/(2*kdeta2*kdeta2);
    return ka1*TMath::Exp(-ex1)+ka2*TMath::Exp(-ex2);
}

//_____________________________________________________________________________
AliGenHIJINGpara::AliGenHIJINGpara()
  :AliGenerator()
{
    //
    // Default constructor
    //
    fPtpi    =  0;
    fPtka    =  0;
    fETApic  =  0;
    fETAkac  =  0;
    fDecayer =  0;
    fNt      = -1;
    SetCutVertexZ();
    SetPtRange();
    SetPi0Decays();
}

//_____________________________________________________________________________
AliGenHIJINGpara::AliGenHIJINGpara(Int_t npart)
  :AliGenerator(npart)
{
  // 
  // Standard constructor
  //
    fName="HIJINGpara";
    fTitle="HIJING Parametrisation Particle Generator";
    fPtpi    =  0;
    fPtka    =  0;
    fETApic  =  0;
    fETAkac  =  0;
    fDecayer =  0;
    fNt      = -1;
    SetCutVertexZ();
    SetPtRange();
    SetPi0Decays();
}

//_____________________________________________________________________________
AliGenHIJINGpara::~AliGenHIJINGpara()
{
  //
  // Standard destructor
  //
    delete fPtpi;
    delete fPtka;
    delete fETApic;
    delete fETAkac;
}

//_____________________________________________________________________________
void AliGenHIJINGpara::Init()
{
  //
  // Initialise the HIJING parametrisation
  //
    Float_t etaMin =-TMath::Log(TMath::Tan(
	TMath::Min((Double_t)fThetaMax/2,TMath::Pi()/2-1.e-10)));
    Float_t etaMax = -TMath::Log(TMath::Tan(
	TMath::Max((Double_t)fThetaMin/2,1.e-10)));
    fPtpi   = new TF1("ptpi",&ptpi,0,20,0);
    fPtka   = new TF1("ptka",&ptka,0,20,0);
    fETApic = new TF1("etapic",&etapic,etaMin,etaMax,0);
    fETAkac = new TF1("etakac",&etakac,etaMin,etaMax,0);

    TF1 etaPic0("etapic",&etapic,-7,7,0);
    TF1 etaKac0("etakac",&etakac,-7,7,0);

    TF1 ptPic0("ptpi",&ptpi,0.,15.,0);
    TF1 ptKac0("ptka",&ptka,0.,15.,0);

    Float_t intETApi  = etaPic0.Integral(-0.5, 0.5);
    Float_t intETAka  = etaKac0.Integral(-0.5, 0.5);
    Float_t scalePi   = 7316/(intETApi/1.5);
    Float_t scaleKa   =  684/(intETAka/2.0);

//  Fraction of events corresponding to the selected pt-range    
    Float_t intPt    = (0.877*ptPic0.Integral(0, 15)+
			0.123*ptKac0.Integral(0, 15));
    Float_t intPtSel = (0.877*ptPic0.Integral(fPtMin, fPtMax)+
			0.123*ptKac0.Integral(fPtMin, fPtMax));
    Float_t ptFrac   = intPtSel/intPt;

//  Fraction of events corresponding to the selected eta-range    
    Float_t intETASel  = (scalePi*etaPic0.Integral(etaMin, etaMax)+
			  scaleKa*etaKac0.Integral(etaMin, etaMax));
//  Fraction of events corresponding to the selected phi-range    
    Float_t phiFrac    = (fPhiMax-fPhiMin)/2/TMath::Pi();

    fParentWeight = Float_t(fNpart)/(intETASel*ptFrac*phiFrac);
    
    printf("%s: The number of particles in the selected kinematic region corresponds to %f percent of a full event\n ", 
	   ClassName(),100.*fParentWeight);

// Issue warning message if etaMin or etaMax are outside the alowed range 
// of the parametrization
    if (etaMin < -8.001 || etaMax > 8.001) {
	printf("\n \n WARNING FROM AliGenHIJINGPara !");
	printf("\n YOU ARE USING THE PARAMETERISATION OUTSIDE ");	
	printf("\n THE ALLOWED PSEUDORAPIDITY RANGE (-8. - 8.)");	    
	printf("\n YOUR LIMITS: %f %f \n \n ", etaMin, etaMax);
    }
//
//
    if (fPi0Decays && gMC)
	fDecayer = gMC->GetDecayer();
}


//_____________________________________________________________________________
void AliGenHIJINGpara::Generate()
{
  //
  // Generate one trigger
  //

  
    const Float_t kRaKpic=0.14;
    const Float_t kBorne=1/(1+kRaKpic);
    Float_t polar[3]= {0,0,0};
    //
    const Int_t kPions[3] = {kPi0, kPiPlus, kPiMinus};
    const Int_t kKaons[4] = {kK0Long, kK0Short, kKPlus, kKMinus};
    //
    Float_t origin[3];
    Float_t pt, pl, ptot;
    Float_t phi, theta;
    Float_t p[3];
    Int_t i, part, j;
    //
    TF1 *ptf;
    TF1 *etaf;
    //
    Float_t random[6];
    //
    for (j=0;j<3;j++) origin[j]=fOrigin[j];

    if(fVertexSmear == kPerEvent) {
	Vertex();
	for (j=0; j < 3; j++) origin[j] = fVertex[j];
    } // if kPerEvent
    TArrayF eventVertex;
    eventVertex.Set(3);
    eventVertex[0] = origin[0];
    eventVertex[1] = origin[1];
    eventVertex[2] = origin[2];

    for(i=0;i<fNpart;i++) {
	while(1) {
	    Rndm(random,3);
	    if(random[0]<kBorne) {
		part=kPions[Int_t (random[1]*3)];
		ptf=fPtpi;
		etaf=fETApic;
	    } else {
		part=kKaons[Int_t (random[1]*4)];
		ptf=fPtka;
		etaf=fETAkac;
	    }
	    phi=fPhiMin+random[2]*(fPhiMax-fPhiMin);
	    theta=2*TMath::ATan(TMath::Exp(-etaf->GetRandom()));
	    if(theta<fThetaMin || theta>fThetaMax) continue;
	    pt=ptf->GetRandom();
	    pl=pt/TMath::Tan(theta);
	    ptot=TMath::Sqrt(pt*pt+pl*pl);
	    if(ptot<fPMin || ptot>fPMax) continue;
	    p[0]=pt*TMath::Cos(phi);
	    p[1]=pt*TMath::Sin(phi);
	    p[2]=pl;
	    if(fVertexSmear==kPerTrack) {
		Rndm(random,6);
		for (j=0;j<3;j++) {
		    origin[j]=fOrigin[j]+fOsigma[j]*TMath::Cos(2*random[2*j]*TMath::Pi())*
			TMath::Sqrt(-2*TMath::Log(random[2*j+1]));
		}
	    }
	    if (part == kPi0 && fPi0Decays){
//
//          Decay pi0 if requested
		SetTrack(0,-1,part,p,origin,polar,0,kPPrimary,fNt,fParentWeight);
		KeepTrack(fNt);
		DecayPi0(origin, p);
	    } else {
		SetTrack(fTrackIt,-1,part,p,origin,polar,0,kPPrimary,fNt,fParentWeight);
		KeepTrack(fNt);
	    }

	    break;
	}
	SetHighWaterMark(fNt);
    }
//

// Header
    AliGenEventHeader* header = new AliGenEventHeader("HIJINGparam");
// Event Vertex
    header->SetPrimaryVertex(eventVertex);
    gAlice->SetGenEventHeader(header); 
}

AliGenHIJINGpara& AliGenHIJINGpara::operator=(const  AliGenHIJINGpara& rhs)
{
// Assignment operator
    return *this;
}

void AliGenHIJINGpara::SetPtRange(Float_t ptmin, Float_t ptmax) {
    AliGenerator::SetPtRange(ptmin, ptmax);
}

void AliGenHIJINGpara::DecayPi0(Float_t* orig, Float_t * p) 
{
//
//    Decay the pi0
//    and put decay products on the stack
//
    static TClonesArray *particles;
    if(!particles) particles = new TClonesArray("TParticle",1000);
//    
    const Float_t kMass = TDatabasePDG::Instance()->GetParticle(kPi0)->Mass();
    Float_t       e     = TMath::Sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]+ kMass * kMass);
//
//  Decay the pi0    
    TLorentzVector pmom(p[0], p[1], p[2], e);
    fDecayer->Decay(kPi0, &pmom);
    
//
// Put decay particles on the stack
//
    Float_t polar[3] = {0., 0., 0.};
    Int_t np = fDecayer->ImportParticles(particles);
    Int_t nt;    
    for (Int_t i = 1; i < np; i++)
    {
	TParticle* iParticle =  (TParticle *) particles->At(i);
	p[0] = iParticle->Px();
	p[1] = iParticle->Py();
	p[2] = iParticle->Pz();
	Int_t part = iParticle->GetPdgCode();

	SetTrack(fTrackIt, fNt, part, p, orig, polar, 0, kPDecay, nt, fParentWeight);
	KeepTrack(nt);
    }
    fNt = nt;
}
