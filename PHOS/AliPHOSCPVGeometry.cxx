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
  $Log$
  Revision 1.4  2002/10/29 17:00:45  schutz
  removed iostream

  Revision 1.3  2001/09/26 12:09:36  schutz
  A new (final?) geometry developed

  Revision 1.2  2000/11/09 15:49:04  martinez
  Coding convention rules obeyed

  Revision 1.1  2000/11/03 16:28:21  schutz
  CPV geometry class to correct previous name-convetion rule violation

*/

//_________________________________________________________________________
// Geometry class  for PHOS : CPV (Charged particle veto, IHEP version)
// Its data members provide geometry parametrization of CPV
// which can be changed in the constructor only.
// Author: Yuri Kharlov (IHEP, Protvino)
// 15 September 2000
//
// --- ROOT system ---

// --- Standard library ---

// --- AliRoot header files ---

#include "AliPHOSCPVGeometry.h"

ClassImp(AliPHOSCPVGeometry) ;

//____________________________________________________________________________
AliPHOSCPVGeometry::AliPHOSCPVGeometry()
{

  // Initializes the CPV parameters
  fNumberOfCPVLayers     = 1;
  fNumberOfCPVPadsPhi    = 128;
  fNumberOfCPVPadsZ      = 64;
  fCPVPadSizePhi         = 1.13;
  fCPVPadSizeZ           = 2.26;
  fNumberOfCPVChipsPhi   = 8;
  fNumberOfCPVChipsZ     = 8;
  fCPVGasThickness       = 1.3;
  fCPVTextoliteThickness = 0.1;
  fCPVCuNiFoilThickness  = 56e-04;;
  fCPVFrameSize[0]       = 2.5;
  fCPVFrameSize[1]       = 5.1;
  fCPVFrameSize[2]       = 2.5;
  fGassiplexChipSize[0]  = 4.2;
  fGassiplexChipSize[1]  = 0.1;
  fGassiplexChipSize[2]  = 6.0;
  fFTPosition[0]         = 0.7;
  fFTPosition[1]         = 2.2;
  fFTPosition[2]         = 3.6;
  fFTPosition[3]         = 5.1;

  fCPVActiveSize[0]      = fNumberOfCPVPadsPhi * fCPVPadSizePhi;
  fCPVActiveSize[1]      = fNumberOfCPVPadsZ   * fCPVPadSizeZ;
  fCPVBoxSize[0]         = fCPVActiveSize[0] + 2 * fCPVFrameSize[0];
  fCPVBoxSize[1]         = fCPVFrameSize[1] * fNumberOfCPVLayers + 0.1;
  fCPVBoxSize[2]         = fCPVActiveSize[1] + 2 * fCPVFrameSize[2];

}
//____________________________________________________________________________
