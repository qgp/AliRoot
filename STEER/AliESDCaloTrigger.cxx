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
 

 

Author: R. GUERNANE LPSC Grenoble CNRS/IN2P3
*/

#include "AliESDCaloTrigger.h"
#include "AliLog.h"

#include "TArrayI.h"
#include "Riostream.h"
#include <cstdlib>

ClassImp(AliESDCaloTrigger)

//_______________
AliESDCaloTrigger::AliESDCaloTrigger() : TNamed(),
fNEntries(0),
fCurrent(-1),
fColumn(0x0),
fRow(0x0),
fAmplitude(0x0),
fTime(0x0),
fNL0Times(0x0),
fL0Times(new TArrayI()),
fL1TimeSum(0x0)
{
	//
	for (Int_t i = 0; i < 48; i++) for (Int_t j = 0; j < 64; j++) fTriggerBits[i][j] = 0;
}

//_______________
AliESDCaloTrigger::AliESDCaloTrigger(const AliESDCaloTrigger& src) : TNamed(src),
fNEntries(0),
fCurrent(-1),
fColumn(0x0),
fRow(0x0),
fAmplitude(0x0),
fTime(0x0),
fNL0Times(0x0),
fL0Times(new TArrayI()),
fL1TimeSum(0x0)
{
	//
	src.Copy(*this);
}

//_______________
AliESDCaloTrigger::~AliESDCaloTrigger()
{
	//
	if (fNEntries) DeAllocate();
	
	delete fL0Times; fL0Times = 0x0;
}

//_______________
void AliESDCaloTrigger::DeAllocate()
{
	//
	delete [] fColumn;    fColumn    = 0x0;
	delete [] fRow;       fRow       = 0x0;     
	delete [] fAmplitude; fAmplitude = 0x0;
	delete [] fTime;      fTime      = 0x0;   
	delete [] fNL0Times;  fNL0Times  = 0x0;
	delete [] fL1TimeSum; fL1TimeSum = 0x0;
//	delete [] fL0Times;   fL0Times   = 0x0;

	fNEntries =  0;
	fCurrent  = -1;
	
	for (Int_t i = 0; i < 48; i++) for (Int_t j = 0; j < 64; j++) fTriggerBits[i][j] = 0;

	fL0Times->Reset();
}

//_______________
AliESDCaloTrigger& AliESDCaloTrigger::operator=(const AliESDCaloTrigger& src)
{
	//
	if (this != &src) src.Copy(*this);
	
	return *this;
}

//_______________
void AliESDCaloTrigger::Copy(TObject &obj) const 
{	
	//
	TNamed::Copy(obj);
	
	AliESDCaloTrigger& dest = static_cast<AliESDCaloTrigger&>(obj);

	if (dest.fNEntries) dest.DeAllocate();
	
	dest.Allocate(fNEntries);
	
	for (Int_t i = 0; i < fNEntries; i++)
	{
		Int_t times[10];
		for (Int_t j = 0; j < 10; j++) times[j] = fL0Times->At(10 * i + j);
	  
		dest.Add(fColumn[i], fRow[i], fAmplitude[i], fTime[i], times, fNL0Times[i], fL1TimeSum[i]);
	}
	
	for (Int_t i = 0; i < 48; i++) for (Int_t j = 0; j < 64; j++) dest.fTriggerBits[i][j] = fTriggerBits[i][j];
}

//_______________
void AliESDCaloTrigger::Allocate(Int_t size)
{
	//
	if (!size) return;
	
	fNEntries = size;
	
	fColumn      = new   Int_t[fNEntries];
	fRow         = new   Int_t[fNEntries];
	fAmplitude   = new Float_t[fNEntries];
	fTime        = new Float_t[fNEntries];
	fNL0Times    = new   Int_t[fNEntries];
//	fL0Times     = new   Int_t[fNEntries * 10];
	fL1TimeSum   = new   Int_t[fNEntries];

	for (Int_t i = 0; i < fNEntries; i++) 
	{
	  fColumn[i]    = 0;
	  fRow[i]       = 0;
	  fAmplitude[i] = 0;
	  fTime[i]      = 0;
	  fNL0Times[i]  = 0;
	  fL1TimeSum[i] = 0;
	}
	
	fL0Times->Set(fNEntries * 10);
}

//_______________
Bool_t AliESDCaloTrigger::Add(Int_t col, Int_t row, Float_t amp, Float_t time, Int_t trgtimes[], Int_t ntrgtimes, Int_t trgts)
{
	//
	fCurrent++;
	
	   fColumn[fCurrent] = col;
	      fRow[fCurrent] = row;
	fAmplitude[fCurrent] = amp;
	     fTime[fCurrent] = time;
	 fNL0Times[fCurrent] = ntrgtimes;
	fL1TimeSum[fCurrent] = trgts;	

	if (ntrgtimes > 9) 
	{
		AliError("Should not have more than 10 L0 times");
		return kFALSE;
	}
	
	for (Int_t i = 0; i < fNL0Times[fCurrent]; i++) fL0Times->AddAt(trgtimes[i], 10 * fCurrent + i);

	return kTRUE;
}

//_______________
void AliESDCaloTrigger::SetTriggerBits(Int_t col, Int_t row, Int_t i, Int_t j)
{
	//
	if (col < 0 || col > 47 || row < 0 || row > 63) 
	{
		AliError("Bad position!");
		return;
	}

	fTriggerBits[col][row] = (fTriggerBits[col][row] | (1 << (i + 3 * j))); // L0 L1g L1j
}

//_______________
Bool_t AliESDCaloTrigger::Next()
{
	//
	if (fCurrent >= fNEntries - 1 || !fNEntries) return kFALSE;
	
	fCurrent++;
	
	return kTRUE;
}

//_______________
void AliESDCaloTrigger::GetPosition(Int_t& col, Int_t& row) const
{
	//
	if (fCurrent == -1) return;
	
	col = fColumn[fCurrent];
	row =    fRow[fCurrent];
}

//_______________
void AliESDCaloTrigger::GetAmplitude(Float_t& amp) const
{
	//
	if (fCurrent == -1) return;

	amp = fAmplitude[fCurrent];
}

//_______________
void AliESDCaloTrigger::GetTime(Float_t& time) const
{
	//
	if (fCurrent == -1) return;

	time = fTime[fCurrent];
}

//_______________
void AliESDCaloTrigger::GetL1TimeSum(Int_t& amp) const
{
	//	
	if (fCurrent == -1) return;

	amp = fL1TimeSum[fCurrent];
}

//_______________
void AliESDCaloTrigger::GetNL0Times(Int_t& ntimes) const
{
	//
	if (fCurrent == -1) return;

	ntimes = fNL0Times[fCurrent];
}

//_______________
void AliESDCaloTrigger::GetTriggerBits(Char_t& bits) const
{
	//
	if (fCurrent == -1) return;

	bits = fTriggerBits[fColumn[fCurrent]][fRow[fCurrent]];
}

//_______________
void AliESDCaloTrigger::GetL0Times(Int_t times[]) const
{
	//
	if (fCurrent == -1) return;

	for (Int_t i = 0; i < fNL0Times[fCurrent]; i++) times[i] = fL0Times->At(10 * fCurrent + i);
}

//_______________
void AliESDCaloTrigger::Print(const Option_t* /*opt*/) const
{
	//
	if (fCurrent == -1) return;

	printf("============\n");
	printf("--L0:\n");
	printf("\tPOSITION (X: %2d Y: %2d) / FITTED F-ALTRO (AMP: %4f TIME: %3f)\n", 
		   fColumn[fCurrent], fRow[fCurrent], fAmplitude[fCurrent], fTime[fCurrent]);
	printf("\t%d L0 TIMES (", fNL0Times[fCurrent]); 
	for (Int_t i = 0; i < fNL0Times[fCurrent]; i++) printf("%2d ",fL0Times->At(10 * fCurrent + i));
	printf(")\n");
	printf("--L1:\n");
	printf("\tTIME SUM: %4d\n", fL1TimeSum[fCurrent]);
	printf("\tTHRESHOLDS (GAMMA: %4d, JET: %4d)\n", fL1Threshold[0], fL1Threshold[1]);
	printf("--TRIGGER BITS: 0x%x\n", fTriggerBits[fColumn[fCurrent]][fRow[fCurrent]]);
}	
