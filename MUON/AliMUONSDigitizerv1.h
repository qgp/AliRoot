#ifndef ALIMUONSDIGITIZERV1_H
#define ALIMUONSDIGITIZERV1_H

// The AliMUONSDigitizer produces
// SDigits from Hits 
// J.P Cussonneau Subatech Feb 2004

#include "AliMUONDigitizerv1.h"

class AliMUONSDigitizerv1 : public AliMUONDigitizerv1
{
public:    
	AliMUONSDigitizerv1();
	
	// Preferred constructor to call which sets the manager.
	AliMUONSDigitizerv1(AliRunDigitizer * manager);

	// The following methods are inherited from AliMUONDigitizerv1
	virtual void AddDigit(Int_t chamber, Int_t tracks[kMAXTRACKS], Int_t charges[kMAXTRACKS], Int_t digits[6]);
	virtual Int_t GetSignalFrom(AliMUONTransientDigit* td);
	virtual Bool_t InitOutputData(AliMUONLoader* muonloader);
	virtual void FillOutputData();
	virtual void CleanupOutputData(AliMUONLoader* muonloader);

	ClassDef(AliMUONSDigitizerv1, 0)
};    
#endif

