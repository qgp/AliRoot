/************************************************************************* 
* Copyright(c) 1998-2048, ALICE Experiment at CERN, All rights reserved. * 
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

// Author: Dariusz Miskowiec <mailto:d.miskowiec@gsi.de> 2005

//=============================================================================
// two-particle correlation analyzer
// Loop over pairs and fill pair histograms. The first particle is always 
// taken from ev0 and the second from ev1 so for ev0=ev1 one is getting true 
// pairs, otherwise mixed ones. 
//=============================================================================

#include <TROOT.h>
#include <TMath.h>
#include <TRandom2.h>
#include "AliUnicorEvent.h"
#include "AliUnicorHN.h"
#include "AliUnicorAnalCorrel.h"

ClassImp(AliUnicorAnalCorrel)
 
//=============================================================================
AliUnicorAnalCorrel::AliUnicorAnalCorrel(const char *nam, Double_t emi, Double_t ema, 
			 Int_t pid0, Int_t pid1, Int_t frame): 
  AliUnicorAnal(nam), fPid0(pid0), fPid1(pid1), fMass0(0), fMass1(0), fFrame(frame), fPa() 
{
  // constructor
  // emi and ema define the rapidity range for histogram

  TParticlePDG *part0 = AliUnicorAnal::fgPDG.GetParticle(fPid0);
  TParticlePDG *part1 = AliUnicorAnal::fgPDG.GetParticle(fPid1);
  fMass0 = part0? part0->Mass() : 0;
  fMass1 = part1? part1->Mass() : 0;
  double pi = TMath::Pi();

  // correlation function

  //int npt = 7; double ptbins[]={0,0.1,0.2,0.3,0.4,0.5,0.7,1.0};
  //int npt = 6; double ptbins[]={0,0.1,0.25,0.35,0.55,1.0,2.0}; // like Adam, except last bin split
  //int npt = 7; double ptbins[]={0,0.1,0.25,0.40,0.55,0.7,1.0,2.0}; // like Adam in Mar-2010, + first+last
  int npt = 8; double ptbins[]={0,0.1,0.2,0.3,0.4,0.5,0.7,1.0,2.0};  // for Pb

  double qbins[200] = {0};
  //  for (int i=0;i<20;i++) qbins[i]=i*0.005;
  //  for (int i=0;i<45;i++) qbins[20+i]=0.1+i*0.02;
  for (int i=0;i<30;i++) qbins[i]=i*0.010;
  for (int i=0;i<35;i++) qbins[30+i]=0.3+i*0.02;
  for (int i=0;i<20;i++) qbins[65+i]=1.0+i*0.05;
  for (int i=0;i<11;i++) qbins[85+i]=2.0+i*0.20;

  TAxis *ax[8];
  ax[0] = new TAxis(3,-0.5,2.5);ax[0]->SetTitle("trumixrot");
  ax[1] = new TAxis(5,0,1.0);   ax[1]->SetTitle("centrality");
  ax[2] = new TAxis(3,emi,ema); ax[2]->SetTitle("y");          // pair y
  //  ax[3] = new TAxis(8,-pi,pi);  ax[3]->SetTitle("phi");      // wrt event plane
  ax[3] = new TAxis(1,-pi,pi);  ax[3]->SetTitle("phi");        // wrt event plane
  ax[4] = new TAxis(npt,ptbins);ax[4]->SetTitle("kt (GeV/c)"); // pair pt/2
  ax[5] = new TAxis(8,0,pi);    ax[5]->SetTitle("q-theta");
  ax[6] = new TAxis(16,-pi,pi); ax[6]->SetTitle("q-phi");
  ax[7] = new TAxis(95,qbins);  ax[7]->SetTitle("q (GeV/c)");
  //  ax[7] = new TAxis(700,0,3.5); ax[7]->SetTitle("q (GeV/c)");
  AliUnicorHN *pair = new AliUnicorHN("pair",8,ax);
  for (int i=0; i<8; i++) delete ax[i];
  fHistos.Add(pair);

  // correlation function bin monitor (needed to get <kt> etc.)

  ax[0] = new TAxis(5,0,1);      ax[0]->SetTitle("centrality");
  ax[1] = new TAxis(30,emi,ema); ax[1]->SetTitle("y");           // pair y
  ax[2] = new TAxis(100,0,2);    ax[2]->SetTitle("kt (GeV/c)");  // pair pt/2
  AliUnicorHN *bimo = new AliUnicorHN("bimo",3,ax);
  for (int i=0; i<3; i++) delete ax[i];
  fHistos.Add(bimo);

  // two-track resolution monitoring histogram

  ax[0] = new TAxis(3,-0.5,2.5);    ax[0]->SetTitle("trumixrot");
  ax[1] = new TAxis(2,-0.5,1.5);    ax[1]->SetTitle("cut applied");
  ax[2] = new TAxis(npt,ptbins);    ax[2]->SetTitle("(pair pt)/2 (GeV)");
  ax[3] = new TAxis(80,-0.02,0.02); ax[3]->SetTitle("dtheta");
  ax[4] = new TAxis(80,-0.04,0.04); ax[4]->SetTitle("dphi");
  AliUnicorHN *twot = new AliUnicorHN("twot",5,ax);
  for (int i=0; i<5; i++) delete ax[i];
  fHistos.Add(twot);

  gROOT->cd();
  printf("%s object named %s created\n",ClassName(),GetName());
}
//=============================================================================
void AliUnicorAnalCorrel::Process(Int_t tmr, const AliUnicorEvent * const ev0, const AliUnicorEvent * const ev1, Double_t phirot) 
{
  // process pairs from one or two (if mixing) events
  // tmr tells which histogram (bins) to fill: tru,mix,rot

  // Could be possibly accelerated by checking the "good particle" only once 
  // and caching the result. (Maybe the optimizer does it already.)

  static TRandom2 ran;
  AliUnicorHN *pair = (AliUnicorHN*) fHistos.At(0);
  AliUnicorHN *bimo = (AliUnicorHN*) fHistos.At(1);
  AliUnicorHN *twot = (AliUnicorHN*) fHistos.At(2);

  // mixing-and-rotating-proof centrality and reaction plane angle
  // (but not rotation-proof for rotation angles much different from 0 and 180)
  // true and rotated pairs are within the triangle (j<i), mixed - all
  // thus, proper rotation is either by 180, or by 170 AND 190, etc. 

  double cent = (ev0->Centrality()+ev1->Centrality())/2.0;
  double q0x=0,q0y=0,q1x=0,q1y=0;
  ev0->RP(q0x,q0y);
  ev1->RP(q1x,q1y); 
  double rpphi = atan2(q0y+q1y,q0x+q1x);

  // loop over pairs 

  for (int i=0; i<ev0->NParticles(); i++) {
    if (!ev0->ParticleGood(i,fPid0)) continue;
    for (int j=0; j<ev1->NParticles(); j++) {
      if (ev0==ev1 && j<i && fPid0==fPid1 ) continue; 
      if (ev0==ev1 && j==i) continue; // beware, not even when rotated or non-identical
      if (!ev1->ParticleGood(j,fPid1)) continue;
      fPa.Set0(fMass0,ev0->ParticleP(i),ev0->ParticleTheta(i),ev0->ParticlePhi(i));
      fPa.Set1(fMass1,ev1->ParticleP(j),ev1->ParticleTheta(j),ev1->ParticlePhi(j)+phirot);
      if (ev0==ev1 && fPid0==fPid1 && ran.Rndm()>=0.5) fPa.Swap();
      twot->Fill((double) tmr, 0.0, fPa.Pt()/2.0, fPa.DTheta(), fPa.DPhi(),1.0);
      if (!ev0->PairGood(ev0->ParticleP(i),ev0->ParticleTheta(i),ev0->ParticlePhi(i),
			 ev1->ParticleP(j),ev1->ParticleTheta(j),ev1->ParticlePhi(j)+phirot)) continue;
      twot->Fill((double) tmr, 1.0, fPa.Pt()/2.0, fPa.DTheta(), fPa.DPhi(),1.0);
      fPa.CalcLAB();
      if (fFrame == kPairFrame) fPa.CalcPairCM();
      if (fFrame == kLCMS)      fPa.CalcLcmsCM();
      if (fPa.QCM()==0) {printf("AliUnicorAnalCorrel: Q=0\n"); return;} // should not be too frequent
      double phi = TVector2::Phi_mpi_pi(fPa.Phi()-rpphi);
      double weigth = 1.0;
      //      if (tmr==0 && fPid0==fPid1) weigth = 1.0+0.6*exp(-pow(fPa.QCM()*5/0.197,2)); 
      //      if (tmr==0 && fPid0==fPid1) weigth = 1.0+0.6*exp(-fPa.QCM()*2/0.197); 
      pair->Fill((double) tmr,           // 0 for tru, 1 for mix, 2 for rot
		 cent,                   // centrality
		 fPa.Rapidity(),         // pair rapidity
		 phi,                    // pair phi wrt reaction plane
		 fPa.Pt()/2.0,           // half of pair pt
		 fPa.QCMTheta(),         // polar angle of Q
		 fPa.QCMPhiOut(),        // azimuthal angle of Q w.r.t. out
		 fPa.QCM(),              // |p2-p1| in c.m.s.
		 weigth);                // weigth
      if (tmr) continue;
      if (fPa.QCM()>0.2) continue;
      bimo->Fill(cent, fPa.Rapidity(), fPa.Pt()/2.0, 1.0);
    }
  }
}
//=============================================================================
