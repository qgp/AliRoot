#include "AliHBTCorrFitFctn.h"
//____________________________________________________________
///////////////////////////////////////////////////////////////
//                                                           //
// class AliHBTCorrFitFctn                                   //
//                                                           //
//                                                           //
///////////////////////////////////////////////////////////////

ClassImp(AliHBTCorrFitFctn)

/*****************************************************************/

AliHBTCorrFitFctn::AliHBTCorrFitFctn():
 AliHBTOnePairFctn1D(),
 fNtuple(0x0),
 fNPairsFitArea(1000000),
 fFitRangeMax(0.05),
 fNPairsNormArea(1000000),
 fNormRangeMin(0.05),
 fNormRangeMax(0.1)
{
//ctor
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("wqinvcfCorrFit","Lednicky Weught Theoretical Q_{inv} Correlation Function");
} 
/*****************************************************************/

AliHBTCorrFitFctn::AliHBTCorrFitFctn(Int_t  fit, Int_t  norm):
 AliHBTOnePairFctn1D(100,0.1,0.0),
 fNtuple(new TNtuple("pair", "pair", "px1:py1:pz1:e1:px2:py2:pz2:e2")),
 fNPairsFitArea(fit),
 fFitRangeMax(0.05),
 fNPairsNormArea(norm),
 fNormRangeMin(0.05),
 fNormRangeMax(0.1)
{
//ctor
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("wqinvcfCorrFit","Lednicky Weught Theoretical Q_{inv} Correlation Function");
} 
/*****************************************************************/

void AliHBTCorrFitFctn::ProcessSameEventParticles(AliHBTPair* /*trackpair*/)
{
 //Fills the numerator using pair from the same event
//   partpair = CheckPair(partpair);
   return;

}
/****************************************************************/

void  AliHBTCorrFitFctn::ProcessDiffEventParticles(AliHBTPair* trackpair)
{
  // Fills the denominator using mixed pairs
  trackpair = CheckPair(trackpair);
  if ( trackpair == 0x0) return;
  
  Double_t q = 2.* trackpair->GetKStar();

  Bool_t fill = kFALSE;

  if ( (q < fFitRangeMax) && (fNPairsFitArea < fNPairsFitArea))
    {
      fNPairsFitArea++;
      fill = kTRUE;
    }

  if ( (q > fNormRangeMin) && (q < fNormRangeMax) && (fNPairsNormArea < fNPairsNormArea) )
    {
      fNPairsNormArea++;
      fill = kTRUE;
    }

  if (fill)
   {  
     const AliVAODParticle& p1 = *(trackpair->Particle1());
     const AliVAODParticle& p2 = *(trackpair->Particle2());
     fNtuple->Fill(p1.Px(),p1.Py(),p1.Pz(),p1.E(),
                   p2.Px(),p2.Py(),p2.Pz(),p2.E());

     fDenominator->Fill(q);  
   }


}
/*****************************************************************/

TH1* AliHBTCorrFitFctn::GetResult()
{
//returns denominator
 return fDenominator;
}
/**************************************************************/

void AliHBTCorrFitFctn::WriteFunction()
{
  //writes a function 
  AliHBTFunction::WriteFunction();
  fNtuple->Write(0,TObject::kOverwrite);
}
