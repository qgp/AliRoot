/* $Id$ */

// This class introduces the weight's calculation 
// according to the Lednicky's algorithm.
// The detailed description of the algorithm can be found 
// in comments to fortran code:
// fsiw.f, fsiini.f  

#ifndef ALIHBTLLWEIGHTS_H
#define ALIHBTLLWEIGHTS_H

#include "AliHBTWeights.h"

class AliHBTPair;
class AliHBTLLWeights: public AliHBTWeights
 {
   public:
     AliHBTLLWeights();
     virtual ~AliHBTLLWeights(){;}
     static AliHBTLLWeights* Instance();
     
     void Set();
     
     Double_t GetWeight(AliHBTPair* partpair); //get weight calculated by Lednicky's algorithm

     void Init(); //put the initial values in fortran commons fsiini, led_bldata
     void SetTest(Bool_t rtest = kTRUE);//Sets fTest member

     void SetColoumb(Bool_t col = kTRUE);//: (ICH in fortran code) Coulomb interaction between the two particles ON (OFF)
     void SetQuantumStatistics(Bool_t qss = kTRUE);//IQS: quantum statistics for the two particles ON (OFF) //if non-identical particles automatically off
     void SetStrongInterSwitch(Bool_t sis = kTRUE);//ISI: strong interaction between the two particles ON (OFF)
     void SetColWithResidNuclSwitch(Bool_t crn = kTRUE);//I3C: Coulomb interaction with residual nucleus ON (OFF)  
     void SetLambda(Double_t la){fOneMinusLambda=1.-la;}  //lambda=haoticity
     void SetApproxModel(Int_t ap);//sets  Model of Approximation (NS in Fortran code)
     void SetRandomPosition(Bool_t rp = kTRUE); //ON=kTRUE(OFF=kFALSE)
     void SetR1dw(Double_t R);   //spherical source model radii                                                                           		                                                                                                        
     void SetParticlesTypes(Int_t pid1, Int_t pid2); //set AliRoot particles types   
     void SetNucleusCharge(Double_t ch); // not used now  (see comments in fortran code)
     void SetNucleusMass(Double_t mass); // (see comments in fortran code)
     
   protected:
     
     Bool_t fTest;           //flag indicating if parameters listed below are to be calculated automatically (0)
                             //or 
     Bool_t fColoumbSwitch;   //switches on/off Coulumb effect
     Bool_t fQuantStatSwitch; //switches on/off Quantum Statistics effect
     Bool_t fStrongInterSwitch;//Switches strong interactions TRUE=ON
     Bool_t fColWithResidNuclSwitch;//Switches couloumb interaction 
                                    //with residual nucleus TRUE=ON          
     Double_t fNuclMass;   //mass of nucleus
     Double_t fNuclCharge; //charge of nucleus
     
     Bool_t   fRandomPosition;//flag indicating if positions of particles shuold be randomized according to parameters below
     Double_t fRadius;        //radius used for randomizing particle vertex position
     
     Double_t fOneMinusLambda; //1 - intercept parameter
     
     Int_t fApproximationModel; //approximation used to calculate Bethe-Salpeter amplitude see SetApproxModel for more comments

     Int_t fPID1;  //Pdg Code of the first particle in the pair
     Int_t fPID2;  //Pdg Code of the second particle in the pair
     Double_t fSigma; //constants for spherical source model in cm
     static const Double_t fgkWcons; //constant for fm->GeV conversion 1/0.1973

     static  AliHBTLLWeights *fgLLWeights;// pointer to wrapper of Fortran Lednicky code

     static Int_t GetPairCode(Int_t pid1,Int_t pid2);
     static Int_t GetPairCode(const AliHBTPair* partpair);//calculate automatically internal code 

   private:
     AliHBTLLWeights(const AliHBTLLWeights &/*source*/);
     AliHBTLLWeights & operator=(const AliHBTLLWeights& /*source*/);
     
     ClassDef(AliHBTLLWeights,1)
 };

#endif
