#ifndef ALI_DETECTOR_PID_H
#define ALI_DETECTOR_PID_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//---------------------------------------------------------------//
//      Class to store raw probabilities and nsigmas             //
//        of all detectors                                       //
//                                                               //
//                                                               //
//   Origin: Jens Wiechula, Uni Tuebingen, jens.wiechula@cern.ch //
//---------------------------------------------------------------//

#include <TObject.h>
#include <TClonesArray.h>

#include "AliPID.h"
#include "AliPIDResponse.h"

class AliDetectorPID : public TObject {
public:
  AliDetectorPID();
  AliDetectorPID(const AliDetectorPID &pid);
  virtual ~AliDetectorPID();
  AliDetectorPID& operator= (const AliDetectorPID &pid);
  
  void SetRawProbability(AliPIDResponse::EDetector det, const Double_t prob[], Int_t nspecies, AliPIDResponse::EDetPidStatus status);
  void SetNumberOfSigmas(AliPIDResponse::EDetector det, const Double_t nsig[], Int_t nspecies);

  AliPIDResponse::EDetPidStatus GetRawProbability(AliPIDResponse::EDetector det, Double_t prob[], Int_t nspecies) const;
  void GetNumberOfSigmas(AliPIDResponse::EDetector det, Double_t nsig[], Int_t nspecies) const;
  
  Double_t GetRawProbability(AliPIDResponse::EDetector det, AliPID::EParticleType type) const;
  Double_t GetNumberOfSigmas(AliPIDResponse::EDetector det, AliPID::EParticleType type) const;
private:
  TClonesArray fArrNsigmas;          // array to store nsigma values of all detectors
  TClonesArray fArrRawProbabilities; // array to strore raw probabilities of all detectors

  ClassDef(AliDetectorPID,1);        //Store raw probabilities and nsigmas for all detectors
};

#endif

