#include "AliHBTMonSplitting.h"

ClassImp(AliHBTMonSplittingQosl)

void   AliHBTMonSplittingQosl::ProcessSameEventParticles(AliHBTPair* trackpair, AliHBTPair* partpair)
{
  AliVAODParticle* p1 = partpair->Particle1();
  AliVAODParticle* p2 = partpair->Particle2();
  
  if (p1->Px() != p2->Px()) return;
  if (p1->Py() != p2->Py()) return;
  if (p1->Pz() != p2->Pz()) return;
  
  Double_t out = trackpair->GetQOutLCMS();
  Double_t side = trackpair->GetQSideLCMS();
  Double_t lon = trackpair->GetQLongLCMS();
    
  fNumerator->Fill(out,side,lon);//here we fill in q's corresponding to track pair 
                                          //weight calculated for the simulated one
}

ClassImp(AliHBTMonSplittingDptDthetaDphi)

void   AliHBTMonSplittingDptDthetaDphi::ProcessSameEventParticles(AliHBTPair* trackpair, AliHBTPair* partpair)
{
  AliVAODParticle* p1 = partpair->Particle1();
  AliVAODParticle* p2 = partpair->Particle2();
  
  if (p1->Px() != p2->Px()) return;
  if (p1->Py() != p2->Py()) return;
  if (p1->Pz() != p2->Pz()) return;
  
  Double_t dpt = trackpair->GetDeltaPt();
  Double_t dphi = trackpair->GetDeltaPhi();
  Double_t dtheta = trackpair->GetDeltaTheta();
    
  fNumerator->Fill(dpt, dphi, dtheta);//here we fill in q's corresponding to track pair 
                                          //weight calculated for the simulated one
}
