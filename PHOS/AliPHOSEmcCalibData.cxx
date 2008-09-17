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

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// class for PHOS EmCal calibration                                          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliPHOSEmcCalibData.h"

ClassImp(AliPHOSEmcCalibData)

//________________________________________________________________
  AliPHOSEmcCalibData::AliPHOSEmcCalibData(): TNamed()
{
  // Default constructor
  Reset();
}

//________________________________________________________________
AliPHOSEmcCalibData::AliPHOSEmcCalibData(const char* name)
{
  // Constructor
  TString namst = "Calib_";
  namst += name;
  SetName(namst.Data());
  SetTitle(namst.Data());
  Reset();
}

//________________________________________________________________
AliPHOSEmcCalibData::AliPHOSEmcCalibData(const AliPHOSEmcCalibData& calibda) :
  TNamed(calibda)
{
  // copy constructor
  SetName(calibda.GetName());
  SetTitle(calibda.GetName());

  for(Int_t module=0; module<5; module++) {
    for(Int_t column=0; column<56; column++) {
      for(Int_t row=0; row<64; row++) {
	fADCchannelEmc[module][column][row] = calibda.fADCchannelEmc[module][column][row];
	fADCpedestalEmc[module][column][row] = calibda.fADCpedestalEmc[module][column][row];
	fHighLowRatioEmc[module][column][row] = calibda.fHighLowRatioEmc[module][column][row];
	fTimeShiftEmc[module][column][row] = calibda.fTimeShiftEmc[module][column][row];
        fAltroOffsets[module][column][row] = calibda.fAltroOffsets[module][column][row];
      }
    }
  }
}

//________________________________________________________________
AliPHOSEmcCalibData &AliPHOSEmcCalibData::operator =(const AliPHOSEmcCalibData& calibda)
{
  // assignment operator

  if(this != &calibda) { 

    SetName(calibda.GetName());
    SetTitle(calibda.GetName());

    for(Int_t module=0; module<5; module++) {
      for(Int_t column=0; column<56; column++) {
	for(Int_t row=0; row<64; row++) {
	  fADCchannelEmc[module][column][row] = calibda.fADCchannelEmc[module][column][row];
	  fADCpedestalEmc[module][column][row] = calibda.fADCpedestalEmc[module][column][row];
	  fHighLowRatioEmc[module][column][row] = calibda.fHighLowRatioEmc[module][column][row];
	  fTimeShiftEmc[module][column][row] = calibda.fTimeShiftEmc[module][column][row];
          fAltroOffsets[module][column][row] = calibda.fAltroOffsets[module][column][row]; 
	}
      }
    }
  }

  return *this;
}

//________________________________________________________________
AliPHOSEmcCalibData::~AliPHOSEmcCalibData()
{
  // Destructor
}

//________________________________________________________________
void AliPHOSEmcCalibData::Reset()
{
  // Set all pedestals and all ADC channels to its ideal values = 5. (MeV/ADC)

  for (Int_t module=0; module<5; module++){
    for (Int_t column=0; column<56; column++){
      for (Int_t row=0; row<64; row++){
	fADCpedestalEmc[module][column][row] = 0.;
	fADCchannelEmc[module][column][row]  = 0.005;
	fHighLowRatioEmc[module][column][row] = 16. ;
	fTimeShiftEmc[module][column][row] = 0. ;
        fAltroOffsets[module][column][row] = 0 ;
      }
    }
  }

}

//________________________________________________________________
void  AliPHOSEmcCalibData::Print(Option_t *option) const
{
  // Print tables of pedestals and ADC channels

  if (strstr(option,"ped")) {
    printf("\n	----	EMC Pedestal values	----\n\n");
    for (Int_t module=0; module<5; module++){
      printf("============== Module %d\n",module+1);
      for (Int_t column=0; column<56; column++){
	for (Int_t row=0; row<64; row++){
	  printf("%4.1f",fADCpedestalEmc[module][column][row]);
	}
	printf("\n");
      }
    }
  }

  if (strstr(option,"gain")) {
    printf("\n	----	EMC ADC channel values	----\n\n");
    for (Int_t module=0; module<5; module++){
      printf("============== Module %d\n",module+1);
      for (Int_t column=0; column<56; column++){
	for (Int_t row=0; row<64; row++){
	  printf("%4.1f",fADCchannelEmc[module][column][row]);
	}
	printf("\n");
      }
    }
  }

  if (strstr(option,"hilo")) {
    printf("\n	----	EMC High/Low ratio	----\n\n");
    for (Int_t module=0; module<5; module++){
      printf("============== Module %d\n",module+1);
      for (Int_t column=0; column<56; column++){
	for (Int_t row=0; row<64; row++){
	  printf("%4.1f",fHighLowRatioEmc[module][column][row]);
	}
	printf("\n");
      }
    }
  }
  if (strstr(option,"time")) {
    printf("\n	----	EMC t0 shifts	----\n\n");
    for (Int_t module=0; module<5; module++){
      printf("============== Module %d\n",module+1);
      for (Int_t column=0; column<56; column++){
	for (Int_t row=0; row<64; row++){
	  printf("%6.3e",fTimeShiftEmc[module][column][row]);
	}
	printf("\n");
      }
    }
  }
  if (strstr(option,"altro")) {
    printf("\n  ----    EMC altro offsets   ----\n\n");
    for (Int_t module=0; module<5; module++){
      printf("============== Module %d\n",module+1);
      for (Int_t column=0; column<56; column++){
        for (Int_t row=0; row<64; row++){
          printf("%5d",fAltroOffsets[module][column][row]);
        }
        printf("\n");
      }
    }
  }
}

//________________________________________________________________
Float_t AliPHOSEmcCalibData::GetADCchannelEmc(Int_t module, Int_t column, Int_t row) const
{
  //Return EMC calibration coefficient
  //module, column,raw should follow the internal PHOS convention:
  //module 1:5, column 1:56, row 1:64

  return fADCchannelEmc[module-1][column-1][row-1];
}

//________________________________________________________________
Float_t AliPHOSEmcCalibData::GetADCpedestalEmc(Int_t module, Int_t column, Int_t row) const
{
  //Return EMC pedestal
  //module, column,raw should follow the internal PHOS convention:
  //module 1:5, column 1:56, row 1:64

  return fADCpedestalEmc[module-1][column-1][row-1];
}

//________________________________________________________________
Float_t AliPHOSEmcCalibData::GetHighLowRatioEmc(Int_t module, Int_t column, Int_t row) const
{
  //Return EMC pedestal
  //module, column,raw should follow the internal PHOS convention:
  //module 1:5, column 1:56, row 1:64

  return fHighLowRatioEmc[module-1][column-1][row-1];
}

//________________________________________________________________
Float_t AliPHOSEmcCalibData::GetTimeShiftEmc(Int_t module, Int_t column, Int_t row) const
{
  //Return EMC pedestal
  //module, column,raw should follow the internal PHOS convention:
  //module 1:5, column 1:56, row 1:64

  return fTimeShiftEmc[module-1][column-1][row-1];
}
//________________________________________________________________
Int_t AliPHOSEmcCalibData::GetAltroOffsetEmc(Int_t module, Int_t column, Int_t row) const
{
  //Return EMC altro offsets
  //module, column,raw should follow the internal PHOS convention:
  //module 1:5, column 1:56, row 1:64
 
  return fAltroOffsets[module-1][column-1][row-1];
}
//________________________________________________________________
void AliPHOSEmcCalibData::SetADCchannelEmc(Int_t module, Int_t column, Int_t row, Float_t value)
{
  //Set EMC calibration coefficient
  //module, column,raw should follow the internal PHOS convention:
  //module 1:5, column 1:56, row 1:64

  fADCchannelEmc[module-1][column-1][row-1] = value;
}

//________________________________________________________________
void AliPHOSEmcCalibData::SetADCpedestalEmc(Int_t module, Int_t column, Int_t row, Float_t value)
{
  //Set EMC pedestal
  //module, column,raw should follow the internal PHOS convention:
  //module 1:5, column 1:56, row 1:64
  fADCpedestalEmc[module-1][column-1][row-1] = value;
}

//________________________________________________________________
void AliPHOSEmcCalibData::SetHighLowRatioEmc(Int_t module, Int_t column, Int_t row, Float_t value)
{
  //Set EMC pedestal
  //module, column,raw should follow the internal PHOS convention:
  //module 1:5, column 1:56, row 1:64
  fHighLowRatioEmc[module-1][column-1][row-1] = value;
}
//________________________________________________________________
void AliPHOSEmcCalibData::SetTimeShiftEmc(Int_t module, Int_t column, Int_t row, Float_t value)
{
  //Set EMC pedestal
  //module, column,raw should follow the internal PHOS convention:
  //module 1:5, column 1:56, row 1:64
  fTimeShiftEmc[module-1][column-1][row-1] = value;
}
//________________________________________________________________
void AliPHOSEmcCalibData::SetAltroOffsetEmc(Int_t module, Int_t column, Int_t row, Int_t value)
{
  //Set EMC pedestal
  //module, column,raw should follow the internal PHOS convention:
  //module 1:5, column 1:56, row 1:64
  fAltroOffsets[module-1][column-1][row-1] = value;
}
