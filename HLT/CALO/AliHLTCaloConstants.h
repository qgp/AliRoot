//-*- Mode: C++ -*-
// $Id: AliHLTCALOConstants.h $

//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               */

/// @file   AliHLCaloConstants.h
/// @author Svein Lindal
/// @date   
/// @brief  Class containing constants for PHOS and EMCAL
///         loaded libraries

#include "Rtypes.h"
#include "TString.h"

#ifndef ALIHLTCALOCONSTANTS_H
#define ALIHLTCALOCONSTANTS_H



class AliHLTCaloConstants
{

public:

  AliHLTCaloConstants();
  virtual ~AliHLTCaloConstants();
  
  virtual Int_t GetMAXHOSTS() const = 0; 						
  virtual Int_t GetDEFAULTEVENTPORT() const = 0; 					
  virtual Int_t GetMAXBINVALUE() const = 0; 						
  virtual Int_t GetHIGHGAIN() const = 0; 						
  virtual Int_t GetLOWGAIN() const = 0; 							
								
  virtual Int_t GetALTROMAXSAMPLES() const = 0; 						
  virtual Int_t GetALTROMAXPRESAMPLES() const = 0; 					 
								
  virtual Int_t GetNZROWSRCU() const = 0; 						
  virtual Int_t GetNXCOLUMNSRCU() const = 0; 						
  virtual Int_t GetNZROWSMOD() const = 0; 						
  virtual Int_t GetNXCOLUMNSMOD() const = 0; 						
  virtual Int_t GetNGAINS() const = 0; 				
  virtual Int_t GetNDATATYPES() const = 0; 						
								
  virtual Int_t GetPFMAXPATHLENGTH() const = 0; 						
								
  virtual Int_t GetPFDEFAULTNSAMPLES() const = 0; 					
  virtual Int_t GetPFDEFAULTSTARTINDEX() const = 0; 					 
								
  virtual Double_t GetDEFAULTTAU() const = 0; 					 
  virtual Int_t GetDEFAULTFS() const = 0; 						
								
  virtual Int_t GetMODULE0() const = 0; 							
  virtual Int_t GetMODULE1() const = 0; 							
  virtual Int_t GetMODULE2() const = 0; 							
  virtual Int_t GetMODULE3() const = 0; 							
  virtual Int_t GetMODULE4() const = 0; 							
								
  virtual Int_t GetCSPSPERFEE() const = 0; 						
  virtual Int_t GetRCU0() const = 0; 							
  virtual Int_t GetRCU1() const = 0; 							
  virtual Int_t GetRCU2() const = 0; 							
  virtual Int_t GetRCU3() const = 0; 							
								
  virtual Int_t GetZ0() const = 0; 							
  virtual Int_t GetZ1() const = 0; 							
  virtual Int_t GetX0() const = 0; 							
  virtual Int_t GetX1() const = 0; 							
								
  virtual Int_t GetNMODULES() const = 0; 						
  virtual Int_t GetNRCUS() const = 0; 							
								
  virtual Int_t GetNRCUSPERMODULE() const = 0; 						
  virtual Int_t GetNRCUSPERTOTAL() const = 0; 						
  virtual Int_t GetNFEECS() const = 0; 							
  virtual Int_t GetNALTROS() const = 0; 							
  virtual Int_t GetNALTROCHANNELS() const = 0; 						
  virtual Int_t GetNBRANCHES() const = 0; 						
								
  virtual Float_t GetCELLSTEP() const = 0; 					
  virtual Int_t GetNRCUSPERSECTOR() const = 0; 						

  
private:
  ClassDef(AliHLTCaloConstants, 1);

};
#endif
