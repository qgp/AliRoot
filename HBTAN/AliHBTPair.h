#ifndef ALIHBTPAIR_H
#define ALIHBTPAIR_H

#include <TObject.h>


#include "AliHBTParticle.h"
//class implements pair of particles and taking care of caluclation (almost)
//all of pair properties (Qinv, InvMass,...)
//more info: http://alisoft.cern.ch/people/skowron/analyzer/index.html

class AliHBTPair: public TObject
{
 public:
   AliHBTPair(Bool_t rev = kFALSE); //contructor
   AliHBTPair(AliHBTParticle* part1, AliHBTParticle* part2, Bool_t rev = kFALSE); //contructor
   virtual ~AliHBTPair(){}
   void SetParticles(AliHBTParticle*,AliHBTParticle*); //sets particles in the pair
   AliHBTPair* GetSwapedPair() {return fSwapedPair;} //returns pair with swapped particles
   
   AliHBTParticle* Particle1() const {return fPart1;} //returns pointer to first particle
   AliHBTParticle* Particle2() const {return fPart2;} //returns pointer to decond particle
   
   void Changed();
   //Center Mass System - Longitudinally Comoving
   
   virtual Double_t GetInvMass(); //returns invariant mass of the pair
   virtual Double_t GetMt();
   virtual Double_t GetQInv(); //returns Q invariant
   virtual Double_t GetQSideCMSLC(); //returns Q Side CMS longitudionally co-moving
   virtual Double_t GetQOutCMSLC(); //returns Q out CMS longitudionally co-moving
   virtual Double_t GetQLongCMSLC(); //returns Q Long CMS longitudionally co-moving
   
   
   virtual Double_t GetKt();  //returns K transverse
   virtual Double_t GetKStar();
   
   virtual Double_t GetDeltaP(); //return difference of momenta
   virtual Double_t GetDeltaPx();
   virtual Double_t GetDeltaPy();
   virtual Double_t GetDeltaPz();
   
   virtual Double_t GetGammaToCMSLC();
 protected:
   AliHBTParticle* fPart1;  //pointer to first particle
   AliHBTParticle* fPart2;  //pointer to second particle
  
   AliHBTPair* fSwapedPair; //pointer to swapped pair
   
/************************************************************/
/************CMS (LC) Q's   *********************************/
/************************************************************/
   //Center Mass System - Longitudinally Comoving
   
   Double_t fQSideCMSLC;  //value of Q side CMS longitudially co-moving
   Bool_t   fQSideCMSLCNotCalc; //flag indicating if fQSideCMSLC is already calculated for this pair
   
   Double_t fQOutCMSLC; //value of Q out CMS longitudially co-moving
   Bool_t   fQOutCMSLCNotCalc;//flag indicating if fQOutCMSLC is already calculated for this pair
   
   Double_t fQLongCMSLC; //value of Q long CMS longitudially co-moving
   Bool_t   fQLongCMSLCNotCalc;//flag indicating if fQLongCMSLC is already calculated for this pair
/************************************************************/
/************************************************************/
   Double_t fQInv;  //half of differnece of 4-momenta
   Bool_t   fQInvNotCalc;//flag indicating if fQInv is already calculated for this pair
   
   Double_t fInvMass;  //invariant mass
   Bool_t   fInvMassNotCalc;//flag indicating if fInvMass is already calculated for this pair
   
   Double_t fKt; //K == sum vector of particle's momenta. Kt transverse component
   Bool_t   fKtNotCalc;//flag indicating if fKt is already calculated for this pair
   
   Double_t fKStar; //
   Bool_t   fKStarNotCalc;
   
   Double_t fPInv;  //invariant momentum
   
   Double_t fQSide; //Q Side
   Double_t fOut;//Q Out
   Double_t fQLong;//Q Long

   Double_t fMt;//Transverse coordinate of Inv. Mass
   Bool_t   fMtNotCalc;//flag indicating if Mt is calculated
      
   Double_t fInvMassSqr;//squre of invariant mass
   Bool_t   fMassSqrNotCalc; //
   void     CalculateInvMassSqr();
   
   Double_t fQInvL;
   Bool_t   fQInvLNotCalc;
   void     CalculateQInvL();
   
   Double_t fPxSum;
   Double_t fPySum;
   Double_t fPzSum;
   Double_t fESum;
   Bool_t   fSumsNotCalc;
   void     CalculateSums();
   
   Double_t fPxDiff;
   Double_t fPyDiff;
   Double_t fPzDiff;
   Double_t fEDiff;
   Bool_t   fDiffsNotCalc;
   void     CalculateDiffs();
   
   Double_t fGammaCMSLC;
   Bool_t   fGammaCMSLCNotCalc;   
   /***************************************************/
   void CalculateBase();
   Bool_t fChanged;
   
   
 private:
 public:
  ClassDef(AliHBTPair,1)
};
/****************************************************************/
inline
void AliHBTPair::SetParticles(AliHBTParticle* p1,AliHBTParticle* p2)
{
 //sets the particle to the pair
 
 fPart1 = p1;
 fPart2 = p2;
 if (fSwapedPair) //if we have Swaped (so we are not)
   fSwapedPair->SetParticles(p2,p1); //set particles for him too
 Changed();
 //and do nothing until will be asked for
} 
/****************************************************************/

inline
void AliHBTPair::Changed()
{
 // Resel all calculations (flags)
 fChanged           = kTRUE;
 fSumsNotCalc       = kTRUE;
 fDiffsNotCalc      = kTRUE;
 fMassSqrNotCalc    = kTRUE;
 fInvMassNotCalc    = kTRUE;
 fQInvNotCalc       = kTRUE;
 fMtNotCalc         = kTRUE;
 fQSideCMSLCNotCalc = kTRUE;
 fQOutCMSLCNotCalc  = kTRUE;
 fQLongCMSLCNotCalc = kTRUE;
 fKtNotCalc         = kTRUE;
 fKStarNotCalc      = kTRUE;
 fQInvLNotCalc      = kTRUE;
 fGammaCMSLCNotCalc = kTRUE;
}
/****************************************************************/
inline 
void AliHBTPair::CalculateInvMassSqr()
 {
  if (fMassSqrNotCalc)
   {
     CalculateSums();
 
     Double_t fPart12s= (fPxSum*fPxSum) + (fPySum*fPySum) + (fPzSum*fPzSum);
 
     fInvMassSqr=fESum*fESum-fPart12s;

     fMassSqrNotCalc = kFALSE;
   }
 }
/****************************************************************/
inline 
void AliHBTPair::CalculateQInvL()
 {
 //Calculates square root of Qinv
  if (fQInvLNotCalc)
  {
   CalculateDiffs();
   fQInvL = fEDiff*fEDiff - ( fPxDiff*fPxDiff + fPyDiff*fPyDiff + fPzDiff*fPzDiff );
   fQInvLNotCalc = kFALSE;
  }
 }
/****************************************************************/ 
inline 
void AliHBTPair::CalculateSums()
 {
   if(fSumsNotCalc)
    {
     fPxSum = fPart1->Px()+fPart2->Px();
     fPySum = fPart1->Py()+fPart2->Py();
     fPzSum = fPart1->Pz()+fPart2->Pz();
     fESum  = fPart1->Energy() + fPart2->Energy();
     fSumsNotCalc = kFALSE;
    }
 }
/****************************************************************/
inline 
void AliHBTPair::CalculateDiffs()
 {
   if(fDiffsNotCalc)
    {
     fPxDiff = fPart1->Px()-fPart2->Px();
     fPyDiff = fPart1->Py()-fPart2->Py();
     fPzDiff = fPart1->Pz()-fPart2->Pz();
     fEDiff  = fPart1->Energy() - fPart2->Energy();
     fDiffsNotCalc = kFALSE;
    }
 }

/****************************************************************/
inline 
Double_t AliHBTPair::GetDeltaP() //return difference of momenta
{
 CalculateDiffs();
 return TMath::Sqrt(fPxDiff*fPxDiff + fPyDiff*fPyDiff + fPzDiff*fPzDiff);
}
/****************************************************************/
inline 
Double_t AliHBTPair::GetDeltaPx()
 {
   CalculateDiffs();
   return fPxDiff;
 }
/****************************************************************/
inline 
Double_t AliHBTPair::GetDeltaPy()
 {
   CalculateDiffs();
   return fPyDiff;
 }

/****************************************************************/
inline 
Double_t AliHBTPair::GetDeltaPz()
 {
   CalculateDiffs();
   return fPzDiff;
 }


#endif
