#ifndef ALIMUONGLOBALTRIGGER_H
#define ALIMUONGLOBALTRIGGER_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*    */

#include <TObject.h>

class AliMUONGlobalTrigger : public TObject {
 public:
  AliMUONGlobalTrigger();
  AliMUONGlobalTrigger(Int_t *singlePlus, Int_t *singleMinus,
		       Int_t *singleUndef, Int_t *pairUnlike, Int_t *pairLike);
  virtual ~AliMUONGlobalTrigger(){;}
  Int_t SinglePlusLpt();  
  Int_t SinglePlusHpt();  
  Int_t SinglePlusApt();   
  Int_t SingleMinusLpt(); 
  Int_t SingleMinusHpt(); 
  Int_t SingleMinusApt(); 
  Int_t SingleUndefLpt(); 
  Int_t SingleUndefHpt();  
  Int_t SingleUndefApt(); 
  Int_t PairUnlikeLpt();  
  Int_t PairUnlikeHpt();  
  Int_t PairUnlikeApt();  
  Int_t PairLikeLpt();    
  Int_t PairLikeHpt();    
  Int_t PairLikeApt();    

  ClassDef(AliMUONGlobalTrigger,1)  // reconstructed Global Trigger object    
    
private:
  Int_t fSinglePlusLpt;  // Number of Single Plus Low pt 
  Int_t fSinglePlusHpt;  // Number of Single Plus High pt 
  Int_t fSinglePlusApt;  // Number of Single Plus All pt 
  Int_t fSingleMinusLpt; // Number of Single Minus Low pt
  Int_t fSingleMinusHpt; // Number of Single Minus High pt 
  Int_t fSingleMinusApt; // Number of Single Minus All pt
  Int_t fSingleUndefLpt; // Number of Single Undefined Low pt
  Int_t fSingleUndefHpt; // Number of Single Undefined High pt 
  Int_t fSingleUndefApt; // Number of Single Undefined All pt
  Int_t fPairUnlikeLpt;  // Number of Unlike sign pair Low pt
  Int_t fPairUnlikeHpt;  // Number of Unlike sign pair High pt
  Int_t fPairUnlikeApt;  // Number of Unlike sign pair All pt
  Int_t fPairLikeLpt;    // Number of Like sign pair Low pt
  Int_t fPairLikeHpt;    // Number of Like sign pair High pt
  Int_t fPairLikeApt;    // Number of Like sign pair All pt

};
#endif






