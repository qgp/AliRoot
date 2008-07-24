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

/* $Id$ */

//*************************************************************************
// SSD geometry, based on ROOT geometrical modeler
//
// Enrico Cattaruzza                                    ecattar@ts.infn.it
//*************************************************************************
#include "TMath.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include <TGeoManager.h>
#include "TVector3.h"
#include "TGeoArb8.h"
#include "TList.h"
#include "TGeoMatrix.h"
#include "TGeoCompositeShape.h"
#include "TGeoBoolNode.h"
#include "TGeoTube.h"
#include "TGeoBBox.h"
#include "TGeoXtru.h"
#include "TGeoTorus.h"
#include "TGeoPgon.h"
#include "TGeoPcon.h"
#include "TRotation.h"
#include "AliITSv11GeometrySSD.h"
/////////////////////////////////////////////////////////////////////////////////
// Names of the Sensitive Volumes of Layer 5 and Layer 6
/////////////////////////////////////////////////////////////////////////////////
const char* AliITSv11GeometrySSD::fgSSDsensitiveVolName5 = "ITSssdSensitivL5";
const char* AliITSv11GeometrySSD::fgSSDsensitiveVolName6 = "ITSssdSensitivL6";
/////////////////////////////////////////////////////////////////////////////////
//Parameters for SSD Geometry
/////////////////////////////////////////////////////////////////////////////////
// Variable for Vertical Disalignement of Modules
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkSSDTolerance = 0.0001*fgkmm;
//const Double_t AliITSv11GeometrySSD::fgkSSDTolerance = 1.*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDModuleVerticalDisalignment = 0.*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDModuleVerticalDisalignment2 = 0.2*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// Layer5 (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkSSDLay5LadderLength      = 950.7*fgkmm;
const Int_t AliITSv11GeometrySSD::fgkSSDLay5SensorsNumber        =  22;
const Int_t AliITSv11GeometrySSD::fgkSSDLay5LadderNumber         =  34;
const Double_t AliITSv11GeometrySSD::fgkSSDLay5RadiusMin         = 378.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDLay5RadiusMax         = 384.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkLay5CenterITSPosition    = 467.85*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// Layer6 (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkSSDLay6LadderLength      = 1068.0*fgkmm;
const Int_t AliITSv11GeometrySSD::fgkSSDLay6SensorsNumber        =   25;
const Int_t AliITSv11GeometrySSD::fgkSSDLay6LadderNumber         =   38;
const Double_t AliITSv11GeometrySSD::fgkSSDLay6RadiusMin         =  428.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDLay6RadiusMax         =  434.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkLay6CenterITSPosition    = 526.50*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// SSD Chips and Hybrid (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Int_t AliITSv11GeometrySSD::fgkSSDChipNumber               =   6;
const Double_t AliITSv11GeometrySSD::fgkSSDChipLength            =  11.100*fgkmm; 
const Double_t AliITSv11GeometrySSD::fgkSSDChipWidth             =   3.850*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDChipHeight            =   0.180*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDChipSeparationLength  =   1.000*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDChipGlueLength     = fgkSSDChipLength;
const Double_t AliITSv11GeometrySSD::fgkSSDChipGlueWidth      =  fgkSSDChipWidth;
const Double_t AliITSv11GeometrySSD::fgkSSDChipGlueHeight        =   0.030*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// Stiffener (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkSSDStiffenerLength       =  73.000*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDStiffenerWidth        =   6.500*fgkmm;
//const Double_t AliITSv11GeometrySSD::fgkSSDStiffenerHeight       =   3.315;
const Double_t AliITSv11GeometrySSD::fgkSSDStiffenerHeight       =   0.315*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDStiffenerToChipDist   =   2.500*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCapacitor0603Length   = 1.600*fgkmm; 
const Double_t AliITSv11GeometrySSD::fgkSSDCapacitor0603Width    =   0.870*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCapacitor0603Height   =   0.800*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCapacitor1812Length   =   4.600*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCapacitor1812Width    =   3.400*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCapacitor1812Height   =   0.700*fgkmm; // multiplied by 0.5  
const Double_t AliITSv11GeometrySSD::fgkSSDWireLength            =  30.000*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDWireRadius            =   0.185*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDConnectorPosition[2]  = 
													   {44.32*fgkmm, 0.33*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkSSDConnectorSeparation   =	  0.44*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDConnectorLength       =	  2.16*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDConnectorWidth        =	  3.60*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDConnectorHeight       = 
													  0.25*fgkSSDStiffenerHeight;
const Double_t AliITSv11GeometrySSD::fgkSSDConnectorAlHeight     =	 0.030*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDConnectorNiHeight     =   0.002*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// Cooling Block (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkSSDCoolingBlockLength    =   3.000*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCoolingBlockWidth     =   4.000*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCoolingBlockHeight[3] =  
										 {1.950*fgkmm, 0.240*fgkmm, 0.300*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkSSDCoolingBlockHoleRadius[2] = 
													  {1.000*fgkmm, 0.120*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkSSDCoolingBlockHoleLength[2] = 
													  {1.900*fgkmm, 0.400*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkSSDCoolingBlockHoleCenter    =  
																	 1.500*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCoolingBlockHoleHeight    =  
																	 0.300*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// SSD Sensor (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const char*  AliITSv11GeometrySSD::fgkSSDSensitiveVolName       = 
														 "SSDSensorSensitiveVol";
const Double_t AliITSv11GeometrySSD::fgkSSDSensorLength          =  42.000*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDSensorHeight          =   0.300*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDSensorWidth           =  75.000*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDSensorOverlap         = 
	   											   fgkSSDSensorLength-39.1*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDSensorInsensitiveLength    = 1.*fgkmm; 
const Double_t AliITSv11GeometrySSD::fgkSSDSensorInsensitiveWidth     = 1.*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// Flex (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkSSDFlexFullLength       =  106.000*fgkmm; 
const Double_t AliITSv11GeometrySSD::fgkSSDFlexLength[4]        = 
			{0.5 * (fgkSSDStiffenerLength+fgkSSDChipNumber*fgkSSDChipLength
				 + (fgkSSDChipNumber-1)*fgkSSDChipSeparationLength),
			 0.5 * (fgkSSDStiffenerLength+fgkSSDChipNumber*fgkSSDChipLength
				 + (fgkSSDChipNumber-1)*fgkSSDChipSeparationLength)
									   - 4.000*fgkmm, 9.500*fgkmm, 10.000*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkSSDFlexWidth[2]         = 
												   {  9.340*fgkmm,  5.380*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkSSDFlexHeight[2]        =
												   {  0.030*fgkmm,  0.020*fgkmm};      
const Double_t AliITSv11GeometrySSD::fgkSSDFlexAngle            =   30.000;
const Double_t AliITSv11GeometrySSD::fgkSSDFlexHoleLength       =    1.430*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDFlexHoleWidth        =    3.000*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDEndFlexCompLength[6] = 
			 {3.30*fgkmm,4.12*fgkmm,4.22*fgkmm,1.70*fgkmm,0.75*fgkmm,7.18*fgkmm};
const Double_t AliITSv11GeometrySSD:: fgkSSDEndFlexCompWidth[3] =
										   {15.03*fgkmm,23.48*fgkmm,12.28*fgkmm};
/////////////////////////////////////////////////////////////////////////////////
// SSD Ladder Cable (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkSSDLadderCableWidth     =     23.5*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// SSD Module (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkSSDModuleStiffenerPosition[2]  = 
													 { 1.000*fgkmm, 3.900*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkSSDModuleSensorSupportDistance =  
																	45.600*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDModuleCoolingBlockToSensor  =  
																	 5.075*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// Sensor Support (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkSSDSensorSideSupportLength		       = 
																	 5.800*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDSensorSideSupportWidth          =  
																	 2.000*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDSensorSideSupportHeight[2]      =
												     { 4.620*fgkmm-fgkSSDModuleVerticalDisalignment2, 
													   5.180*fgkmm-fgkSSDModuleVerticalDisalignment2};
//const Double_t AliITSv11GeometrySSD::fgkSSDSensorSideSupportHeight[2]      =
//												     { 4.520*fgkmm, 5.130*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkSSDSensorSideSupportThickness[2]   = 
													 { 0.450*fgkmm, 0.450*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkSSDSensorSideSupportPosition       
								= 0.5 * (fgkSSDModuleSensorSupportDistance
							    +  fgkSSDSensorSideSupportThickness[0])
								-  fgkSSDSensorSideSupportLength;
const Double_t AliITSv11GeometrySSD::fgkSSDSensorCenterSupportLength	   =  
									   							    5.250*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDSensorCenterSupportWidth        =
																	1.680*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDSensorCenterSupportHeight[2]    
								  = {fgkSSDSensorSideSupportHeight[0]
								  +  fgkSSDSensorSideSupportThickness[0],
									 fgkSSDSensorSideSupportHeight[1]
								  +  fgkSSDSensorSideSupportThickness[1]};
const Double_t AliITSv11GeometrySSD::fgkSSDSensorCenterSupportThickness[2] 
								  =  {fgkSSDSensorSideSupportThickness[0],
									  fgkSSDSensorSideSupportThickness[1]};
const Double_t AliITSv11GeometrySSD::fgkSSDSensorCenterSupportPosition     = 
																   19.000*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// Chip Cables (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkSSDChipCablesLength[2]   = 
				  {73.12/fgkSSDChipNumber*fgkmm,fgkSSDChipLength+2.*0.19*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkSSDChipCablesHeight[4]   = 
				  {  0.014*fgkmm,  0.010*fgkmm, fgkSSDModuleCoolingBlockToSensor
								  -  (fgkSSDSensorSideSupportHeight[1]
								  -   fgkSSDSensorSideSupportHeight[0])
								  -   fgkSSDModuleVerticalDisalignment2	
								  -   fgkSSDCoolingBlockHoleCenter
								  -   fgkSSDStiffenerHeight
								  -   fgkSSDChipHeight-fgkSSDSensorHeight,
									  fgkSSDModuleCoolingBlockToSensor
								  -   fgkSSDModuleVerticalDisalignment2	
								  -   fgkSSDCoolingBlockHoleCenter
								  -	  fgkSSDStiffenerHeight
								  -   fgkSSDChipHeight-fgkSSDSensorHeight};
const Double_t AliITSv11GeometrySSD::fgkSSDChipCablesWidth[3]    = 
		                            { 11.000*fgkmm,  0.800*fgkmm,  0.600*fgkmm};
/////////////////////////////////////////////////////////////////////////////////
// Carbon Fiber Junction Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberJunctionLength          = 
																	3.820*fgkmm;
//const Double_t AliITSv11GeometrySSD::fgkCarbonFiberJunctionLength          = 
//																	   3.780;
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberJunctionWidth           = 
										 fgkSSDSensorLength-fgkSSDSensorOverlap;
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberJunctionEdge[2]         = 
													{  0.86*fgkmm,  0.30*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberJunctionAngle[2]        = 
																{ 30.00, 90.00};
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberJunctionToSensorSupport = 
																	 1.78*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
//Carbon Fiber Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberTriangleLength          
								   = fgkSSDModuleSensorSupportDistance
								   - 2. * fgkCarbonFiberJunctionToSensorSupport;  
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberTriangleAngle = 60.00;
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberSupportTopEdgeDist[2]   = 
												  {  0.751*fgkmm,  0.482*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberSupportEdgeLength  = 
																	1.630*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberSupportWidth =   0.950*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberSupportXAxisLength      
									= fgkCarbonFiberTriangleLength
									- 0.5*fgkCarbonFiberSupportTopEdgeDist[1]
									/ TMath::Cos(fgkCarbonFiberTriangleAngle
									* TMath::DegToRad());
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberSupportYAxisLength      
									= 0.5*(fgkCarbonFiberJunctionWidth
									- fgkCarbonFiberSupportWidth)
									- fgkCarbonFiberSupportTopEdgeDist[0]
									- fgkCarbonFiberSupportWidth;
/////////////////////////////////////////////////////////////////////////////////
// Carbon Fiber Lower Support Parameters (lengths are in mm)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberLowerSupportWidth             
																	  =  0.950*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberLowerSupportLowerLenght       
																	  =  1.600*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberLowerSupportHeight            
																	  =  0.830*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberLowerSupportVolumeSeparation  
											  = 0.5*fgkCarbonFiberSupportWidth;
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberLowerSupportTransverseWidth   
									= fgkCarbonFiberJunctionWidth
									- 2. * (fgkCarbonFiberLowerSupportWidth
									+ fgkCarbonFiberLowerSupportVolumeSeparation);
const Double_t AliITSv11GeometrySSD::fgkCarbonFiberLowerSupportVolumePosition[2] 
									= {fgkCarbonFiberLowerSupportWidth
									+  fgkCarbonFiberLowerSupportVolumeSeparation,
									   fgkCarbonFiberLowerSupportWidth
									+  fgkCarbonFiberLowerSupportVolumeSeparation							
									+  fgkCarbonFiberLowerSupportTransverseWidth};
/////////////////////////////////////////////////////////////////////////////////
// End Ladder Carbon Fiber Junction Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkEndLadderCarbonFiberLowerJunctionLength[2] = 
							   {0.5 * (fgkSSDLay5LadderLength
									-  fgkSSDLay5SensorsNumber
									*  fgkCarbonFiberJunctionWidth
									-  fgkCarbonFiberLowerSupportWidth),
								0.5 * (fgkSSDLay5LadderLength
									-  fgkSSDLay5SensorsNumber
									*  fgkCarbonFiberJunctionWidth
									+  fgkCarbonFiberLowerSupportWidth)};
const Double_t AliITSv11GeometrySSD::fgkEndLadderCarbonFiberUpperJunctionLength[2] = 
						{fgkEndLadderCarbonFiberLowerJunctionLength[0]-20.4*fgkmm,
						 fgkEndLadderCarbonFiberLowerJunctionLength[1]-20.6*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndLadderMountingBlockPosition[2] = 
						{fgkEndLadderCarbonFiberLowerJunctionLength[0]-16.50*fgkmm,
						 fgkEndLadderCarbonFiberLowerJunctionLength[1]-31.50*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkendladdercoolingsupportdistance[3] = 
											{15.0*fgkmm, 13.5*fgkmm, 14.5*fgkmm};
/////////////////////////////////////////////////////////////////////////////////
// Cooling Tube Support (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkCoolingTubeSupportRmax      =  1.45*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkCoolingTubeSupportRmin          
											  = fgkSSDCoolingBlockHoleRadius[0];
const Double_t AliITSv11GeometrySSD::fgkCoolingTubeSupportLength    =  8.55*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkCoolingTubeSupportHeight    =  0.85*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkCoolingTubeSupportWidth     =  2.00*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkCoolingTubeSupportSeparation    = 
                                        fgkSSDSensorLength-2.*fgkSSDSensorOverlap;
const Double_t AliITSv11GeometrySSD::fgkCoolingTubeSupportToCarbonFiber = 
																	  11.70*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// Cooling Tube (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkCoolingTubeRmax = fgkCoolingTubeSupportRmin;
const Double_t AliITSv11GeometrySSD::fgkCoolingTubeRmin =  0.96*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkCoolingTubeLength = 
													fgkCarbonFiberJunctionWidth;
const Double_t AliITSv11GeometrySSD::fgkCoolingTubeSeparation = 
									 fgkSSDModuleSensorSupportDistance
								  +	 fgkSSDCoolingBlockLength;
const Double_t AliITSv11GeometrySSD::fgkMountingBlockToSensorSupport = 30.7*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// SSD Mounting Block Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkSSDMountingBlockLength[3]            = 
										   { 60.0*fgkmm, 42.2*fgkmm, 34.0*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkSSDMountingBlockHeight[4]            =
							   {  4.0*fgkmm,  8.0*fgkmm,  5.0*fgkmm,  0.2*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkSSDMountingBlockWidth                =   
																	  20.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDMountingBlockHoleTrapezoidAngle   =   
																		    40.0;
const Double_t AliITSv11GeometrySSD::fgkSSDMountingBlockHoleTrapezoidHeight  = 
	           0.30*(fgkSSDMountingBlockHeight[1]-fgkSSDMountingBlockHeight[2]);
const Double_t AliITSv11GeometrySSD::fgkSSDMountingBlockHoleTrapezoidUpBasis =    
																	  2.5*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDMountingBlockHoleTubeLength[2]    = 
													  { 56.0*fgkmm, 12.0*fgkmm}; 
const Double_t AliITSv11GeometrySSD::fgkSSDMountingBlockHoleTubeWidth[2]     = 
												      {  5.0*fgkmm,  2.9*fgkmm}; 
const Double_t AliITSv11GeometrySSD::fgkSSDMountingBlockHoleRadius           = 
																	  1.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDMountingBlockScrewHoleEdge        =   
																	  6.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDMountingBlockScrewHoleHeigth      =  
																	  4.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDMountingBlockScrewHoleRadius[2]   =
							  {  1.5*fgkmm,fgkSSDMountingBlockScrewHoleEdge/6.};
/////////////////////////////////////////////////////////////////////////////////
// SSD Mounting Block Clip Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkMountingBlockClipLength        = 15.1*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkMountingBlockClipThickness     = 0.3*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkMountingBlockClibScrewRadius   = 1.6*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkMountingBlockClibScrewPosition = 4.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkMountingBlockClibWidth         = 9.0*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// SSD Mounting Block Support Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkMountingBlockSupportWidth[2] = {9.5*fgkmm,10.0*fgkmm}; 
const Double_t AliITSv11GeometrySSD::fgkMountingBlockSupportDownHeight   = 4.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkMountingBlockSupportRadius[2] = {fgkSSDLay5RadiusMin
												  -  fgkSSDMountingBlockHeight[1]
												  +  0.5*fgkCoolingTubeSupportHeight
												  +	 fgkSSDModuleCoolingBlockToSensor
												  +  fgkSSDModuleVerticalDisalignment
												  -	 fgkMountingBlockSupportDownHeight,
													 fgkSSDLay6RadiusMin
												  -  fgkSSDMountingBlockHeight[1]
												  +  0.5*fgkCoolingTubeSupportHeight
												  +	 fgkSSDModuleCoolingBlockToSensor
												  +  fgkSSDModuleVerticalDisalignment
												  -	 fgkMountingBlockSupportDownHeight}; 
const Double_t AliITSv11GeometrySSD::fgkMountingBlockSupportUpHeight[2] = {fgkSSDLay5RadiusMax
   												    -  fgkSSDMountingBlockHeight[1]
												    +  0.5*fgkCoolingTubeSupportHeight
												    +  fgkSSDModuleCoolingBlockToSensor
													+  fgkSSDModuleVerticalDisalignment
													-  fgkMountingBlockSupportRadius[0],
													   fgkSSDLay6RadiusMax
   												    -  fgkSSDMountingBlockHeight[1]
												    +  0.5*fgkCoolingTubeSupportHeight
												    +  fgkSSDModuleCoolingBlockToSensor
													+  fgkSSDModuleVerticalDisalignment
													-  fgkMountingBlockSupportRadius[1]};
const Double_t AliITSv11GeometrySSD::fgkLadderSupportHeigth = 10.0*fgkmm; // To be verified
const Double_t AliITSv11GeometrySSD::fgkLadderSupportRingLay5Position = 451.35*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkLadderSupportRingLay6Position = 510.00*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// SSD End Cap Cover Plate Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkEndCapCoverPlateSmallHoleRadius = 1.25*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapCoverPlateBigHoleRadius = 2.45*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapCoverPlateThickness = 0.5*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapCoverPlateSmallHoleSeparation[3] =
												{16.5*fgkmm,22.0*fgkmm,7.*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapCoverPlateLength[6] = 
				  {7.*fgkmm,55.*fgkmm,8.0*fgkmm,53.*fgkmm,61.0*fgkmm,25.5*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapCoverPlateWidth[3] = 
											   {68.5*fgkmm,75.5*fgkmm,6.5*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapCoverPlateScrewRadiusMin = 0.750*fgkmm;  
const Double_t AliITSv11GeometrySSD::fgkEndCapCoverPlateScrewRadiusMax = 2.*fgkmm;  
const Double_t AliITSv11GeometrySSD::fgkEndCapCoverPlateClipLength = 10.4*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapCoverPlateClipWidth = 6.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapCoverPlateDownClipLength = 5.7*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapCoverPlateDownClipWidth = 5.0*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// SSD End Cap Kapton Foil Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkEndCapKaptonFoilThickness = 0.4*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapKaptonFoilLength = 68.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapKaptonFoilWidth = 75.0*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// SSD End Cap Cooling Tube Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkEndCapCoolingTubeAxialRadius[2] =
														{10.5*fgkmm,9.25*fgkmm}; 
const Double_t AliITSv11GeometrySSD::fgkEndCapCoolingTubeRadiusMin = 1.3*fgkmm; 
const Double_t AliITSv11GeometrySSD::fgkEndCapCoolingTubeRadiusMax = 1.5*fgkmm; 
const Double_t AliITSv11GeometrySSD::fgkEndCapCoolingTubeAngle[5] =
													{182.3,177.9,84.4,70.0,35.0}; 
const Double_t AliITSv11GeometrySSD::fgkEndCapCoolingTubeLength[5] = 
									{49.5*fgkmm,41.7*fgkmm,47.6*fgkmm,5.0*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapCoolingTubeToCoverSide = 13.0*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// SSD End Cap Cover Side Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkEndCapSideCoverLength[5] = {3.5*fgkmm,
									  6.5*fgkmm,75.0*fgkmm,8.0*fgkmm,2.0*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapSideCoverWidth[7] = {30.9*fgkmm,
									  47.5*fgkmm,12.6*fgkmm,5.6*fgkmm,
									  20.0*fgkmm,7.0*fgkmm,5.9*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapSideCoverThickness = 0.4*fgkmm; 
/////////////////////////////////////////////////////////////////////////////////
// SSD End Cap Cards Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkEndCapCardElectBoardBackLength[3] = 
													   {62.0*fgkmm,21.87*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapCardElectBoardBackWidth[2] = 
													    {47.0*fgkmm,0.35*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapCardElectBoardBackThickness = 
																	  1.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapCardElectBoardLength = 61.8*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapCardElectBoardLayerWidth[2] =
													   {43.5*fgkmm, 0.70*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapCardElectBoardLayerThickness = 
																	 0.15*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapCardJMDConnectorThickness = 
																	 19.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapCardJMDConnectorLength[2] = 
														 {4.80*fgkmm,1.1*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapCardJMDConnectorWidth[2] =
														 {3.3*fgkmm,1.10*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapCardJMDConnectorToLayer = 
																	  2.1*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapCardCableConnectorLength[3] =
												{5.2*fgkmm,3.5*fgkmm,1.2*fgkmm}; 
const Double_t AliITSv11GeometrySSD::fgkEndCapCardCableConnectorWidth[2] =
														 {1.9*fgkmm,0.15*fgkmm}; 
const Double_t AliITSv11GeometrySSD::fgkEndCapCardCableConnectorThickness = 
																	   19*fgkmm; 
const Double_t AliITSv11GeometrySSD::fgkEndCapCardCableConnectorDistance = 
																	  1.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapCardCableConnectorToLayer = 
																	  3.6*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapStripConnectionLength = 
																	 61.0*fgkmm; 
const Double_t AliITSv11GeometrySSD::fgkEndCapStripConnectionThickness =
																	 5.97*fgkmm; 
const Double_t AliITSv11GeometrySSD::fgkEndCapStripConnectionWidth = 3.0*fgkmm; 
const Double_t AliITSv11GeometrySSD::fgkEndCapInterfaceCardBLength[7] = 
												{3.1*fgkmm,68.0*fgkmm,3.6*fgkmm,
									  1.9*fgkmm,2.5*fgkmm,14.2*fgkmm,1.5*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapInterfaceCardBWidth[5] = 
						  {17.0*fgkmm,10.0*fgkmm,5.9*fgkmm,6.4*fgkmm,3.9*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapInterfaceCardBThickness = 
																	  1.0*fgkmm; 
const Double_t AliITSv11GeometrySSD::fgkEndCapInterfaceElectBoardCardBThickness 
																   = 0.15*fgkmm; 
const Double_t AliITSv11GeometrySSD::fgkEndCapInterfaceCardBJMDConnectorSeparation = 
																	 20.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapStiffenerLength = 68.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapStiffenerWidth = 5.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapStiffenerThickness = 5.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapEffectiveCableRadiusMin = 1.25*fgkmm; // To Be Checked
const Double_t AliITSv11GeometrySSD::fgkEndCapEffectiveCableRadiusMax = 1.575*fgkmm; // To Be Checked
/////////////////////////////////////////////////////////////////////////////////
// SSD End Cap SupportLayer5/6 Side Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkEndCapSupportLength[2] = {70.424*fgkmm,72.919*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapSupportMiddleRadius[2] = {377.0*fgkmm,437.0*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapSupportLowRadius[2] = {375.0*fgkmm,435.0*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapSupportHighWidth = 20.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapSupportLowWidth[2] = {3.0*fgkmm,3.0*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkEndCapSupportCenterLay5ITSPosition = 625.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapSupportCenterLay5Position = 2.5*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapSupportCenterLay6ITSPosition = 635.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkEndCapSupportCenterLay6Position = 2.5*fgkmm;
/////////////////////////////////////////////////////////////////////////////////
// SSD Cone Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkSSDLowerPConeRadius = 296.5*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPConeAngle = 39.0; 
const Double_t AliITSv11GeometrySSD::fgkSSDPConeZLength[2] = {168.0*fgkmm,153.0*fgkmm};
const Double_t AliITSv11GeometrySSD::fgkSSDPConeLittleHoleRadius = 317.5*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPConeLittleHoleLength = 20.*fgkmm;	
const Double_t AliITSv11GeometrySSD::fgkSSDConeMiddleRadius = 350.*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPConeMiddleLength = 30.*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPConeMiddleWidth = 40.*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPConeUpRadius = 400.*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPConeUpMaxRadius = 459.*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPConeUpMiddleRadius = 472.5*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPConeDownRadius = 282.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPConeTrapezoidAngle = 42.0;
const Double_t AliITSv11GeometrySSD::fgkSSDPConeTrapezoidBasis = 200.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPConeExternalRadius = 492.5*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPConeRadiusWidth = 16.75*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPConeLength = 168.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCentralSupportLength = 1020.*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCentralSupportRadius = 297.5*fgkmm;  
const Double_t AliITSv11GeometrySSD::fgkSSDCentralSupportWidth = 6.28*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCentralAL3SupportLength = 60.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCentralAL3SupportWidth = 2.5*fgkSSDCentralSupportWidth;
/////////////////////////////////////////////////////////////////////////////////
// SSD Cables Parameters (lengths are in mm and angles in degrees)
/////////////////////////////////////////////////////////////////////////////////
const Double_t AliITSv11GeometrySSD::fgkSSDCablesLay5TubeRadiusMin = 11.9*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCablesLay6TubeRadiusMin = 11.9*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCablesLay5RightSideHeight = 7.*fgkmm;  // to be fixed in order to reproduce material budget
const Double_t AliITSv11GeometrySSD::fgkSSDCablesLay6RightSideHeight = 7.*fgkmm;  // to be fixed in order to reproduce material budget
const Double_t AliITSv11GeometrySSD::fgkSSDCableAngle = 22.5;
const Double_t AliITSv11GeometrySSD::fgkSSDCablesLay5RightSideWaterHeight = 2.5*fgkmm;  // to be fixed in order to reproduce material budget
const Double_t AliITSv11GeometrySSD::fgkSSDCablesPatchPanel2RB26Angle[2] = {25.0,53.2};
const Double_t AliITSv11GeometrySSD::fgkSSDCablesPatchPanel2RB24Angle[2] = {23.0,53.6};
const Double_t AliITSv11GeometrySSD::fgkSSDPatchPanel2RB26ITSDistance = 975.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPatchPanel2RB24ITSDistance = 1020.0*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPatchPanel2RB26Radius = 451.3*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPatchPanel2RB24Radius = 451.3*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDPatchPanelHeigth = 87.5*fgkmm;
const Double_t AliITSv11GeometrySSD::fgkSSDCableMaterialBudgetHeight = 20.0*fgkmm;
//const Double_t AliITSv11GeometrySSD::fgkSSDPatchPanel2RB26Radius = fgkSSDPConeExternalRadius;
//const Double_t AliITSv11GeometrySSD::fgkSSDPatchPanel2RB24Radius = fgkSSDPConeExternalRadius;
/////////////////////////////////////////////////////////////////////////////////
ClassImp(AliITSv11GeometrySSD)
/////////////////////////////////////////////////////////////////////////////////
AliITSv11GeometrySSD::AliITSv11GeometrySSD(): 
  AliITSv11Geometry(),
  fSSDChipMedium(),
  fSSDChipGlueMedium(),
  fSSDStiffenerMedium(),
  fSSDStiffenerConnectorMedium(),
  fSSDStiffener0603CapacitorMedium(),
  fSSDStiffener1812CapacitorMedium(),
  fSSDStiffenerHybridWireMedium(),
  fSSDKaptonFlexMedium(),
  fSSDAlTraceFlexMedium(),
  fSSDAlTraceLadderCableMedium(),
  fSSDKaptonLadderCableMedium(),
  fSSDKaptonChipCableMedium(),
  fSSDAlTraceChipCableMedium(),
  fSSDAlCoolBlockMedium(),
  fSSDSensorMedium(),
  fSSDSensorSupportMedium(),
  fSSDCarbonFiberMedium(),
  fSSDTubeHolderMedium(),
  fSSDCoolingTubeWater(),
  fSSDCoolingTubePhynox(),
  fSSDSupportRingAl(),
  fSSDMountingBlockMedium(),
  fSSDRohaCellCone(),
  fSSDAir(),
  fSSDCopper(),
  fCreateMaterials(kFALSE),
  fTransformationMatrices(kFALSE),
  fBasicObjects(kFALSE),
  fcarbonfiberjunction(),
  fcoolingtubesupport(),
  fhybridmatrix(),
  fssdcoolingblocksystem(),
  fcoolingblocksystematrix(),
  fssdstiffenerflex(),
  fssdendflex(),
  fendladdercoolingtubesupportmatrix(),
  fendladdermountingblock(),
  fendladdermountingblockclip(),
  fSSDSensor5(),
  fSSDSensor6(),
  fSSDLayer5(),	
  fSSDLayer6(),
  fMotherVol(),
  fLay5LadderSupportRing(),
  fLay6LadderSupportRing(),
  fgkEndCapSupportSystem(),
  fSSDCone(),
  fColorCarbonFiber(4),
  fColorRyton(5),
  fColorPhynox(14),
  fColorSilicon(3),
  fColorAl(38),
  fColorKapton(6),
  fColorPolyhamide(5),
  fColorStiffener(9),
  fColorEpoxy(30),
  fColorWater(7),
  fColorG10(41)
{
  ////////////////////////
  // Standard constructor
  ////////////////////////
}
/////////////////////////////////////////////////////////////////////////////////
AliITSv11GeometrySSD::AliITSv11GeometrySSD(const AliITSv11GeometrySSD &s):
  AliITSv11Geometry(s.GetDebug()),
  fSSDChipMedium(s.fSSDChipMedium),
  fSSDChipGlueMedium(s.fSSDChipGlueMedium),
  fSSDStiffenerMedium(s.fSSDStiffenerMedium),
  fSSDStiffenerConnectorMedium(s.fSSDStiffenerConnectorMedium),
  fSSDStiffener0603CapacitorMedium(s.fSSDStiffener0603CapacitorMedium),
  fSSDStiffener1812CapacitorMedium(s.fSSDStiffener1812CapacitorMedium),
  fSSDStiffenerHybridWireMedium(s.fSSDStiffenerHybridWireMedium),
  fSSDKaptonFlexMedium(s.fSSDKaptonFlexMedium),
  fSSDAlTraceFlexMedium(s.fSSDAlTraceFlexMedium),
  fSSDAlTraceLadderCableMedium(s.fSSDAlTraceLadderCableMedium),
  fSSDKaptonLadderCableMedium(s.fSSDKaptonLadderCableMedium),
  fSSDKaptonChipCableMedium(s.fSSDKaptonChipCableMedium),
  fSSDAlTraceChipCableMedium(s.fSSDAlTraceChipCableMedium),
  fSSDAlCoolBlockMedium(s.fSSDAlCoolBlockMedium),
  fSSDSensorMedium(s.fSSDSensorMedium),
  fSSDSensorSupportMedium(s.fSSDSensorSupportMedium),
  fSSDCarbonFiberMedium(s.fSSDCarbonFiberMedium),
  fSSDTubeHolderMedium(s.fSSDTubeHolderMedium),
  fSSDCoolingTubeWater(s.fSSDCoolingTubeWater),
  fSSDCoolingTubePhynox(s.fSSDCoolingTubePhynox),
  fSSDSupportRingAl(s.fSSDSupportRingAl),
  fSSDMountingBlockMedium(s.fSSDMountingBlockMedium),
  fSSDRohaCellCone(s.fSSDRohaCellCone),
  fSSDAir(s.fSSDAir),
  fSSDCopper(s.fSSDCopper),
  fCreateMaterials(s.fCreateMaterials),
  fTransformationMatrices(s.fTransformationMatrices),
  fBasicObjects(s.fBasicObjects),
  fcarbonfiberjunction(s.fcarbonfiberjunction),
  fcoolingtubesupport(s.fcoolingtubesupport),
  fhybridmatrix(s.fhybridmatrix),
  fssdcoolingblocksystem(s.fssdcoolingblocksystem),
  fcoolingblocksystematrix(s.fcoolingblocksystematrix),
  fssdstiffenerflex(s.fssdstiffenerflex),
  fssdendflex(s.fssdendflex),
  fendladdercoolingtubesupportmatrix(s.fendladdercoolingtubesupportmatrix),
  fendladdermountingblock(s.fendladdermountingblock),
  fendladdermountingblockclip(s.fendladdermountingblockclip),
  fSSDSensor5(s.fSSDSensor5),
  fSSDSensor6(s.fSSDSensor6),
  fSSDLayer5(s.fSSDLayer5),	
  fSSDLayer6(s.fSSDLayer6),
  fMotherVol(s.fMotherVol),
  fLay5LadderSupportRing(s.fLay5LadderSupportRing),
  fLay6LadderSupportRing(s.fLay6LadderSupportRing),
  fgkEndCapSupportSystem(s.fgkEndCapSupportSystem),
  fSSDCone(s.fSSDCone),
  fColorCarbonFiber(s.fColorCarbonFiber),
  fColorRyton(s.fColorRyton),
  fColorPhynox(s.fColorPhynox),
  fColorSilicon(s.fColorSilicon),
  fColorAl(s.fColorAl),
  fColorKapton(s.fColorKapton),
  fColorPolyhamide(s.fColorPolyhamide),
  fColorStiffener(s.fColorStiffener),
  fColorEpoxy(s.fColorEpoxy),
  fColorWater(s.fColorWater),
  fColorG10(s.fColorG10)
{
  ////////////////////////
  // Copy Constructor
  ////////////////////////
}
/////////////////////////////////////////////////////////////////////////////////
AliITSv11GeometrySSD& AliITSv11GeometrySSD::
operator=(const AliITSv11GeometrySSD &s){
  ////////////////////////
  // Assignment operator
  ////////////////////////
  this->~AliITSv11GeometrySSD();
  new(this) AliITSv11GeometrySSD(s); 
  return *this;
/*	
  if(&s == this) return *this;
  fMotherVol = s.fMotherVol;
  return *this;
 */
}
///////////////////////////////////////////////////////////////////////////////
void AliITSv11GeometrySSD::CreateTransformationMatrices(){
  ///////////////////////////////////////////////////////////////////////  
  // Method generating the trasformation matrix for the whole SSD Geometry   
  ///////////////////////////////////////////////////////////////////////  
  // Setting some variables for Carbon Fiber Supportmatrix creation
  //////////////////////////////////////////////////////////////////////
  Double_t carbonfibersupportxaxisEdgeproj = fgkCarbonFiberSupportEdgeLength
										   * CosD(fgkCarbonFiberJunctionAngle[0]);
  Double_t symmetryplaneposition = (fgkCarbonFiberSupportYAxisLength
								 +	fgkCarbonFiberSupportTopEdgeDist[0]
								 +	fgkCarbonFiberSupportWidth);
  Double_t carbonfibersupportheight = carbonfibersupportxaxisEdgeproj
									* TanD(fgkCarbonFiberJunctionAngle[0]);
  TGeoRotation* carbonfiberot[3];
  for(Int_t i=0; i<3; i++) carbonfiberot[i] = new TGeoRotation();
  carbonfiberot[0]->SetAngles(0.0,180.0,0.0);
  carbonfiberot[1]->SetAngles(90.,-fgkCarbonFiberTriangleAngle,-90.);
  carbonfiberot[2]->SetRotation((*carbonfiberot[1])*(*carbonfiberot[0]));
  Double_t transvector[3] = {fgkCarbonFiberTriangleLength
						  *  CosD(fgkCarbonFiberTriangleAngle),0.,
						  -  fgkCarbonFiberTriangleLength
						  *	 SinD(fgkCarbonFiberTriangleAngle)};
  ///////////////////////////////////////////
  //Setting Local Translations and Rotations: 
  ///////////////////////////////////////////
  TGeoCombiTrans* localcarbonfibersupportmatrix[3]; 
  localcarbonfibersupportmatrix[0] = new TGeoCombiTrans(0.0,0.0,
									 0.5*carbonfibersupportheight,NULL);	
  localcarbonfibersupportmatrix[1] = new TGeoCombiTrans(transvector[0],
									 2.*symmetryplaneposition+transvector[1],
									 transvector[2], carbonfiberot[2]);
  localcarbonfibersupportmatrix[2] = new TGeoCombiTrans(*carbonfiberot[1]);
  /////////////////////////////////////////////////////////////
  // Carbon Fiber Support Transformations
  /////////////////////////////////////////////////////////////
  const Int_t kcarbonfibersupportmatrixnumber[2] = {2,3};
  for(Int_t i=0; i<fgkcarbonfibersupportnumber; i++){ 
		fcarbonfibersupportmatrix[i] = new TGeoHMatrix();
		for(Int_t j=0; j<kcarbonfibersupportmatrixnumber[i]; j++)
			fcarbonfibersupportmatrix[i]->MultiplyLeft(localcarbonfibersupportmatrix[i==0?2*j:j]);
  }		
  /////////////////////////////////////////////////////////////
  // Carbon Fiber Junction Transformation
  /////////////////////////////////////////////////////////////
  const Int_t kcarbonfiberjunctionmatrixnumber = 2;
  TGeoCombiTrans** localcarbonfiberjunctionmatrix[fgkcarbonfiberjunctionumber];
  TGeoRotation** localcarbonfiberjunctionrot[fgkcarbonfiberjunctionumber];
  TGeoTranslation** localcarbonfiberjunctiontrans[fgkcarbonfiberjunctionumber];
  for(Int_t i=0; i<fgkcarbonfiberjunctionumber; i++){ 
	localcarbonfiberjunctionmatrix[i] = 
						new TGeoCombiTrans*[kcarbonfiberjunctionmatrixnumber];
	localcarbonfiberjunctionrot[i] = 
						new TGeoRotation*[kcarbonfiberjunctionmatrixnumber];
	localcarbonfiberjunctiontrans[i] = 
						new TGeoTranslation*[kcarbonfiberjunctionmatrixnumber];
  }
  ///////////////////////
  // Setting Translations
  ///////////////////////
  localcarbonfiberjunctiontrans[0][0] = new TGeoTranslation(0.,0.,0.);
  localcarbonfiberjunctiontrans[1][0] = 
				 new TGeoTranslation(fgkCarbonFiberTriangleLength,0.0,0.0);
  localcarbonfiberjunctiontrans[2][0] = 
				 new TGeoTranslation(fgkCarbonFiberTriangleLength
				 * TMath::Cos(fgkCarbonFiberTriangleAngle*TMath::DegToRad()),
				 fgkCarbonFiberTriangleLength
				 * TMath::Sin(fgkCarbonFiberTriangleAngle*TMath::DegToRad()),0.0);
  localcarbonfiberjunctiontrans[0][1] = 
				 new TGeoTranslation(0.0,0.5*fgkCarbonFiberJunctionWidth,0.0);
  localcarbonfiberjunctiontrans[1][1] = 
				 new TGeoTranslation(*localcarbonfiberjunctiontrans[0][1]);
  localcarbonfiberjunctiontrans[2][1] = 
				 new TGeoTranslation(*localcarbonfiberjunctiontrans[0][1]);
  ////////////////////
  // Setting Rotations
  ////////////////////
  for(Int_t i=0; i<fgkcarbonfiberjunctionumber; i++)
		for(Int_t j=0; j<kcarbonfiberjunctionmatrixnumber; j++)
			localcarbonfiberjunctionrot[i][j] = new TGeoRotation();
  for(Int_t i=0; i<fgkcarbonfiberjunctionumber; i++)
	localcarbonfiberjunctionrot[i][0]->SetAngles(120.*i,0.0,0.0);
  localcarbonfiberjunctionrot[0][1]->SetAngles(0.0,90.0,0.0);
  localcarbonfiberjunctionrot[1][1]->SetRotation(*localcarbonfiberjunctionrot[0][1]);
  localcarbonfiberjunctionrot[2][1]->SetRotation(*localcarbonfiberjunctionrot[0][1]);
  ////////////////////////////////////////
  // Setting Carbon Fiber Junction matrix 
  ////////////////////////////////////////
  for(Int_t i=0; i<fgkcarbonfiberjunctionumber; i++){
		fcarbonfiberjunctionmatrix[i] = new TGeoHMatrix();
		for(Int_t j=0; j<kcarbonfiberjunctionmatrixnumber; j++){
			localcarbonfiberjunctionmatrix[i][j] = 
			new TGeoCombiTrans(*localcarbonfiberjunctiontrans[i][j],
							   *localcarbonfiberjunctionrot[i][j]);
		    fcarbonfiberjunctionmatrix[i]->MultiplyLeft(localcarbonfiberjunctionmatrix[i][j]);
	 }
  }
  /////////////////////////////////////////////////////////////
  // Carbon Fiber Lower Support Transformations
  /////////////////////////////////////////////////////////////
  TGeoTranslation* localcarbonfiberlowersupportrans[2];
  localcarbonfiberlowersupportrans[0] = new TGeoTranslation(0.0,
									 fgkCarbonFiberLowerSupportVolumePosition[1]
								+    fgkCarbonFiberLowerSupportVolumePosition[0],
									 0.0);
  localcarbonfiberlowersupportrans[1] = new TGeoTranslation(0.0,
									 fgkCarbonFiberJunctionWidth
								-    fgkCarbonFiberLowerSupportWidth
								-    fgkCarbonFiberLowerSupportVolumePosition[0]
								-    fgkCarbonFiberLowerSupportVolumePosition[1],
								-    0.5*fgkCarbonFiberLowerSupportHeight);
   localcarbonfiberlowersupportrans[0]->Add(localcarbonfiberlowersupportrans[1]);
   fcarbonfiberlowersupportrans[0] = 
						new TGeoTranslation(*localcarbonfiberlowersupportrans[0]);
   fcarbonfiberlowersupportrans[1] = 
						new TGeoTranslation(*localcarbonfiberlowersupportrans[1]);
  /////////////////////////////////////////////////////////////
  // SSD Sensor Support Transformations
  /////////////////////////////////////////////////////////////
  const Int_t kssdsensorsupportmatrixnumber = 3;
  TGeoCombiTrans** localssdsensorsupportmatrix[fgkssdsensorsupportnumber];
  TGeoRotation** localssdsensorsupportrot[fgkssdsensorsupportnumber];
  TGeoTranslation** localssdsensorsupportrans[fgkssdsensorsupportnumber];
  for(Int_t i=0; i<fgkssdsensorsupportnumber; i++){ 
	localssdsensorsupportmatrix[i] = 
						new TGeoCombiTrans*[kssdsensorsupportmatrixnumber];
	localssdsensorsupportrot[i] = 
						new TGeoRotation*[kssdsensorsupportmatrixnumber];
	localssdsensorsupportrans[i] = 
						new TGeoTranslation*[kssdsensorsupportmatrixnumber];
  }
  ///////////////////////
  // Setting Translations
  ///////////////////////
  localssdsensorsupportrans[0][0] = new TGeoTranslation(0.0,
									  0.5*fgkSSDSensorSideSupportWidth,
									  0.0);
  localssdsensorsupportrans[1][0] = 
						 new TGeoTranslation(*localssdsensorsupportrans[0][0]);
  localssdsensorsupportrans[2][0] = 
						 new TGeoTranslation(*localssdsensorsupportrans[0][0]);
  localssdsensorsupportrans[0][1] = 
						 new TGeoTranslation(-0.5*fgkSSDSensorSideSupportWidth,
										0.5*fgkSSDSensorSideSupportThickness[0],
										0.0);
  localssdsensorsupportrans[1][1] = 
						 new TGeoTranslation(0.5*fgkSSDSensorSideSupportWidth,
									-   0.5*fgkSSDSensorSideSupportThickness[0]
								    -   fgkSSDModuleSensorSupportDistance,
										0.0);
  localssdsensorsupportrans[2][1] = 
						 new TGeoTranslation(0.5*fgkSSDSensorCenterSupportThickness[0]
									-    fgkSSDSensorCenterSupportPosition,
										 0.5*fgkSSDSensorCenterSupportWidth
									-    0.5*fgkSSDModuleSensorSupportDistance,
										 fgkSSDSensorCenterSupportThickness[0]);
  localssdsensorsupportrans[0][2] = 
						 new TGeoTranslation(fgkCarbonFiberTriangleLength
									+    fgkCarbonFiberJunctionToSensorSupport,
										 fgkCarbonFiberJunctionWidth
								    -    0.5*(fgkCarbonFiberLowerSupportWidth
									+    fgkSSDSensorCenterSupportLength
									-    fgkSSDSensorCenterSupportThickness[0])
									-    fgkSSDSensorCenterSupportPosition,
									     0.0);
  localssdsensorsupportrans[1][2] = 
						new TGeoTranslation(*localssdsensorsupportrans[0][2]);
  localssdsensorsupportrans[2][2] = 
						new TGeoTranslation(*localssdsensorsupportrans[0][2]);
  ////////////////////
  // Setting Rotations
  ////////////////////
  for(Int_t i=0; i<fgkssdsensorsupportnumber; i++)
		for(Int_t j=0; j<kssdsensorsupportmatrixnumber; j++)
			localssdsensorsupportrot[i][j] = new TGeoRotation();
  for(Int_t i=0; i<fgkssdsensorsupportnumber; i++){
	localssdsensorsupportrot[i][0]->SetAngles(0.0,90.0,0.0);
	localssdsensorsupportrot[i][2]->SetAngles(-90.0,0.0,0.0);
  }
  localssdsensorsupportrot[0][1]->SetAngles(0.0,90.0,-90.0);
  localssdsensorsupportrot[1][1]->SetAngles(180.0,90.0,-90.0);
  localssdsensorsupportrot[2][1]->SetAngles(270.0,90.0,-90.0);
  ////////////////////////////////////////
  // SSD Sensor Support matrix 
  ////////////////////////////////////////
  for(Int_t i=0; i<fgkssdsensorsupportnumber; i++){
		fssdsensorsupportmatrix[i] = new TGeoHMatrix();
		for(Int_t j=0; j<kssdsensorsupportmatrixnumber; j++){
			localssdsensorsupportmatrix[i][j] = 
			new TGeoCombiTrans(*localssdsensorsupportrans[i][j],
							   *localssdsensorsupportrot[i][j]);
		    fssdsensorsupportmatrix[i]->MultiplyLeft(localssdsensorsupportmatrix[i][j]);
	 }
  }
  /////////////////////////////////////////////////////////////
  // SSD Cooling Tube Support Transformations
  /////////////////////////////////////////////////////////////
  const Int_t kcoolingtubesupportmatrixnumber = 2;
  TGeoCombiTrans* localcoolingtubesupportmatrix[kcoolingtubesupportmatrixnumber];
  TGeoTranslation* localcoolingtubesupportrans[kcoolingtubesupportmatrixnumber];
  TGeoRotation* localcoolingtubesupportrot[kcoolingtubesupportmatrixnumber];
  Double_t phi = TMath::ASin(0.5*fgkCoolingTubeSupportHeight
													/fgkCoolingTubeSupportRmax);
  localcoolingtubesupportrans[0] = 
			new TGeoTranslation(2.*fgkCoolingTubeSupportRmax*TMath::Cos(phi)
						+  2.*(fgkCoolingTubeSupportLength
						-  fgkCoolingTubeSupportRmax*(1.+TMath::Cos(phi)))
						+  fgkCarbonFiberTriangleLength
						-  2.0*fgkCarbonFiberJunctionLength,0.0,0.0);
  localcoolingtubesupportrans[1] = 
			new TGeoTranslation(fgkCarbonFiberJunctionLength
					- (fgkCoolingTubeSupportLength-fgkCoolingTubeSupportRmax),
					- (2.0*fgkSSDSensorLength-fgkSSDSensorOverlap)+
						   fgkSSDModuleStiffenerPosition[1]+fgkSSDStiffenerWidth
                    +  0.5*fgkSSDFlexHoleLength+2.*fgkCarbonFiberJunctionWidth
                    -  0.5*(fgkCarbonFiberLowerSupportWidth
					+	   fgkSSDSensorCenterSupportLength
                    -      fgkSSDSensorCenterSupportThickness[0])
					+  0.5*fgkSSDSensorLength,
					-  0.5*fgkCoolingTubeSupportHeight);  
  for(Int_t i=0; i<kcoolingtubesupportmatrixnumber; i++)     
  localcoolingtubesupportrot[i] = new TGeoRotation();
  localcoolingtubesupportrot[0]->SetAngles(180.0,0.0,0.0);
  localcoolingtubesupportrot[1]->SetAngles(0.0,90.0,0.0);
  for(Int_t i=0; i<kcoolingtubesupportmatrixnumber; i++)
	localcoolingtubesupportmatrix[i] = 
		new TGeoCombiTrans(*localcoolingtubesupportrans[i],
						   *localcoolingtubesupportrot[i]);
  fcoolingtubesupportmatrix[0] = new TGeoHMatrix(*localcoolingtubesupportmatrix[1]);
  fcoolingtubesupportmatrix[1] = new TGeoHMatrix((*localcoolingtubesupportmatrix[1])*
								(*localcoolingtubesupportmatrix[0]));
  /////////////////////////////////////////////////////////////
  // End Ladder SSD Cooling Tube Support Transformations
  /////////////////////////////////////////////////////////////
  TGeoTranslation** localendladdercooltubetrans[2];
  localendladdercooltubetrans[0] = new TGeoTranslation*[4];
  localendladdercooltubetrans[1] = new TGeoTranslation*[2];
  for(Int_t i=0; i<4; i++) localendladdercooltubetrans[0][i] = new TGeoTranslation();
  localendladdercooltubetrans[0][0]->SetTranslation(fgkCarbonFiberJunctionLength
											-	   (fgkCoolingTubeSupportLength
											-		fgkCoolingTubeSupportRmax),
													fgkEndLadderMountingBlockPosition[0]
											-		fgkendladdercoolingsupportdistance[0]
											+   0.5*fgkCoolingTubeSupportWidth,
											-   0.5*fgkCoolingTubeSupportHeight);
  localendladdercooltubetrans[0][1]->SetTranslation(fgkCarbonFiberJunctionLength
											-	   (fgkCoolingTubeSupportLength
											-		fgkCoolingTubeSupportRmax),
													fgkEndLadderMountingBlockPosition[0]
											+		fgkendladdercoolingsupportdistance[1]
											+   0.5*fgkCoolingTubeSupportWidth,
											-   0.5*fgkCoolingTubeSupportHeight);
  localendladdercooltubetrans[0][2]->SetTranslation(2*(fgkCoolingTubeSupportLength
											-       fgkCoolingTubeSupportRmax)
											+		fgkCarbonFiberTriangleLength
											-   2.0*fgkCarbonFiberJunctionLength,
												0.0,
												0.0);
  localendladdercooltubetrans[0][3]->SetTranslation(0.0,
													fgkendladdercoolingsupportdistance[0]
											+		fgkendladdercoolingsupportdistance[1],
													0.0);
  for(Int_t i=0; i<2; i++) localendladdercooltubetrans[1][i] = new TGeoTranslation();
  localendladdercooltubetrans[1][0]->SetTranslation(fgkCoolingTubeSupportRmax
											+		fgkCarbonFiberJunctionLength
											-		fgkCoolingTubeSupportLength,
													fgkEndLadderCarbonFiberLowerJunctionLength[1]
											-	0.5*fgkCoolingTubeSupportWidth
												   -fgkendladdercoolingsupportdistance[2],
											-   0.5*fgkCoolingTubeSupportHeight);
  localendladdercooltubetrans[1][1]->SetTranslation(fgkCarbonFiberTriangleLength
											+		fgkCoolingTubeSupportLength
											-		fgkCoolingTubeSupportRmax
											-		fgkCarbonFiberJunctionLength,
													fgkEndLadderCarbonFiberLowerJunctionLength[1]
											-	0.5*fgkCoolingTubeSupportWidth
											-		fgkendladdercoolingsupportdistance[2],
											-   0.5*fgkCoolingTubeSupportHeight);
  fendladdercoolingtubesupportmatrix = new TGeoHMatrix**[kcoolingtubesupportmatrixnumber];
  fendladdercoolingtubesupportmatrix[0] = new TGeoHMatrix*[4];
  fendladdercoolingtubesupportmatrix[1] = new TGeoHMatrix*[2];
  fendladdercoolingtubesupportmatrix[0][0] = new TGeoHMatrix((*localendladdercooltubetrans[0][0])*
  (*localcoolingtubesupportrot[1]));
  fendladdercoolingtubesupportmatrix[0][1] = new TGeoHMatrix((*localendladdercooltubetrans[0][1])*
  (*localcoolingtubesupportrot[1]));
  fendladdercoolingtubesupportmatrix[0][2] = new TGeoHMatrix(*fendladdercoolingtubesupportmatrix[0][0]);
  fendladdercoolingtubesupportmatrix[0][2]->Multiply(localcoolingtubesupportrot[0]);
  fendladdercoolingtubesupportmatrix[0][2]->MultiplyLeft(localendladdercooltubetrans[0][2]);
  fendladdercoolingtubesupportmatrix[0][3] = new TGeoHMatrix(*fendladdercoolingtubesupportmatrix[0][2]);
  fendladdercoolingtubesupportmatrix[0][3]->MultiplyLeft(localendladdercooltubetrans[0][3]);

  fendladdercoolingtubesupportmatrix[1][0] =	
							new TGeoHMatrix((*localendladdercooltubetrans[1][0])
										   *(*localcoolingtubesupportrot[1]));
  fendladdercoolingtubesupportmatrix[1][1] = new TGeoHMatrix(*localcoolingtubesupportrot[1]);
  fendladdercoolingtubesupportmatrix[1][1]->Multiply(localcoolingtubesupportrot[0]);
  fendladdercoolingtubesupportmatrix[1][1]->MultiplyLeft(localendladdercooltubetrans[1][1]);
  /////////////////////////////////////////////////////////////
  // SSD Cooling Tube Transformations
  /////////////////////////////////////////////////////////////
  TGeoRotation* localcoolingtuberot = new TGeoRotation();	
  localcoolingtuberot->SetAngles(0.,90.,0.);
  TGeoTranslation** localcoolingtubetrans[4];
  TVector3** localcoolingtubevect[4];
  for(Int_t i=0; i<4; i++){
	localcoolingtubevect[i] = new TVector3*[2];
	localcoolingtubetrans[i] = new TGeoTranslation*[2];
	fcoolingtubematrix[i] = new TGeoHMatrix*[2];
  }
  localcoolingtubevect[0][0] = new TVector3(-0.5*(fgkCoolingTubeSeparation
						  -fgkCarbonFiberTriangleLength),
						  - (2.0*fgkSSDSensorLength-fgkSSDSensorOverlap)+
								 fgkSSDModuleStiffenerPosition[1]+fgkSSDStiffenerWidth
						  +	 0.5*fgkSSDFlexHoleLength+2.*fgkCarbonFiberJunctionWidth
						  -	 0.5*(fgkCarbonFiberLowerSupportWidth
						  +      fgkSSDSensorCenterSupportLength
						  -      fgkSSDSensorCenterSupportThickness[0])+
							 0.5*fgkSSDSensorLength-0.25*(fgkSSDSensorLength
						  -  2.0*fgkSSDModuleStiffenerPosition[1]
						  -	 2.0*fgkSSDCoolingBlockWidth-fgkCoolingTubeSupportWidth)
						  -  0.5*fgkCoolingTubeSupportWidth,
						  -  0.5*fgkCoolingTubeSupportHeight);	
  localcoolingtubevect[0][1] = new TVector3(localcoolingtubevect[0][0]->X(),
							localcoolingtubevect[0][0]->Y()+0.5*(fgkSSDSensorLength
						  -  2.0*fgkSSDModuleStiffenerPosition[1]
						  -	 2.0*fgkSSDCoolingBlockWidth-fgkCoolingTubeSupportWidth)
						  +  fgkCoolingTubeSupportWidth,
						  localcoolingtubevect[0][0]->Z());	
  localcoolingtubevect[1][0] = new TVector3(-localcoolingtubevect[0][0]->X()
							 +				 fgkCarbonFiberTriangleLength,
											 localcoolingtubevect[0][0]->Y(),
											 localcoolingtubevect[0][0]->Z());
  localcoolingtubevect[1][1] = new TVector3(-localcoolingtubevect[0][1]->X()
							 +				 fgkCarbonFiberTriangleLength,
											 localcoolingtubevect[0][1]->Y(),
											 localcoolingtubevect[0][1]->Z());
  localcoolingtubevect[2][0] = new TVector3(-0.5*(fgkCoolingTubeSeparation
						  -	fgkCarbonFiberTriangleLength),
						  - (2.0*fgkSSDSensorLength-fgkSSDSensorOverlap)+
								 fgkSSDModuleStiffenerPosition[1]+fgkSSDStiffenerWidth
						  +	 0.5*fgkSSDFlexHoleLength+2.*fgkCarbonFiberJunctionWidth
						  -	 0.5*(fgkCarbonFiberLowerSupportWidth
						  +      fgkSSDSensorCenterSupportLength
						  -      fgkSSDSensorCenterSupportThickness[0])
						  +  fgkSSDModuleStiffenerPosition[1]
						  -  0.5*(fgkSSDModuleStiffenerPosition[1]-fgkSSDSensorOverlap),
						  -  0.5*fgkCoolingTubeSupportHeight);	
  localcoolingtubevect[2][1] = new TVector3(-localcoolingtubevect[2][0]->X()
							 +				 fgkCarbonFiberTriangleLength,
											 localcoolingtubevect[2][0]->Y(),
											 localcoolingtubevect[2][0]->Z());	
  localcoolingtubevect[3][0] = new TVector3(-0.5*(fgkCoolingTubeSeparation
						  -	fgkCarbonFiberTriangleLength),
						  - (2.0*fgkSSDSensorLength-fgkSSDSensorOverlap)+
								 fgkSSDModuleStiffenerPosition[1]+fgkSSDStiffenerWidth
						  +	 0.5*fgkSSDFlexHoleLength+2.*fgkCarbonFiberJunctionWidth
						  -	 0.5*(fgkCarbonFiberLowerSupportWidth
						  +      fgkSSDSensorCenterSupportLength
						  -      fgkSSDSensorCenterSupportThickness[0])
						  +      fgkSSDSensorLength
						  -	 0.5*fgkSSDModuleStiffenerPosition[1],
						  -  0.5*fgkCoolingTubeSupportHeight);	
  localcoolingtubevect[3][1] = new TVector3(-localcoolingtubevect[3][0]->X()
						  + fgkCarbonFiberTriangleLength,
							localcoolingtubevect[3][0]->Y(),
						  - 0.5*fgkCoolingTubeSupportHeight);	
  for(Int_t i=0; i<4; i++) 
	for(Int_t j=0; j<2; j++){
		localcoolingtubetrans[i][j] = 
			new TGeoTranslation(localcoolingtubevect[i][j]->X(),
								localcoolingtubevect[i][j]->Y(),
								localcoolingtubevect[i][j]->Z());
		fcoolingtubematrix[i][j] = new TGeoHMatrix((*localcoolingtubetrans[i][j])
							  *					(*localcoolingtuberot));
	}
  /////////////////////////////////////////////////////////////
  // SSD End Ladder Cooling Tube Transformations
  /////////////////////////////////////////////////////////////
  TGeoRotation* localendlladdercoolingtuberot = new TGeoRotation();	
  localendlladdercoolingtuberot->SetAngles(0.,90.,0.);
  TGeoTranslation** localendlladdercoolingtubetrans[2];
  localendlladdercoolingtubetrans[0] = new TGeoTranslation*[6];
  localendlladdercoolingtubetrans[1] = new TGeoTranslation*[4];
  for(Int_t i=0; i<2; i++)	
	for(Int_t j=0; j<(i==0?6:4); j++) 	
		localendlladdercoolingtubetrans[i][j] = new TGeoTranslation();
  localendlladdercoolingtubetrans[0][0]->SetTranslation(-(fgkCoolingTubeSupportLength
									-	 fgkCoolingTubeSupportRmax)
									+	 fgkCarbonFiberJunctionLength,
									  0.5*(fgkEndLadderMountingBlockPosition[0]
						  			-    fgkendladdercoolingsupportdistance[0]),
									- 0.5*fgkCoolingTubeSupportHeight);
  localendlladdercoolingtubetrans[0][1]->SetTranslation((fgkCoolingTubeSupportLength
									-	 fgkCoolingTubeSupportRmax)
									-	 fgkCarbonFiberJunctionLength
									+    fgkCarbonFiberTriangleLength,
									  0.5*(fgkEndLadderMountingBlockPosition[0]
						  			-    fgkendladdercoolingsupportdistance[0]),
									- 0.5*fgkCoolingTubeSupportHeight);
  localendlladdercoolingtubetrans[0][2]->SetTranslation(-(fgkCoolingTubeSupportLength
									-   fgkCoolingTubeSupportRmax)
									+	fgkCarbonFiberJunctionLength,
									   fgkEndLadderMountingBlockPosition[0]
									-   fgkendladdercoolingsupportdistance[0]
						  +		   0.5*(fgkendladdercoolingsupportdistance[0]
						  +				fgkendladdercoolingsupportdistance[1]
						  +				fgkCoolingTubeSupportWidth),
									- 0.5*fgkCoolingTubeSupportHeight);	
  localendlladdercoolingtubetrans[0][3]->SetTranslation((fgkCoolingTubeSupportLength
									-	 fgkCoolingTubeSupportRmax)
									-	 fgkCarbonFiberJunctionLength
									+    fgkCarbonFiberTriangleLength,
									   fgkEndLadderMountingBlockPosition[0]
									-   fgkendladdercoolingsupportdistance[0]
						  +		   0.5*(fgkendladdercoolingsupportdistance[0]
						  +				fgkendladdercoolingsupportdistance[1]
						  +				fgkCoolingTubeSupportWidth),
									- 0.5*fgkCoolingTubeSupportHeight);	
  localendlladdercoolingtubetrans[0][4]->SetTranslation(-(fgkCoolingTubeSupportLength
									-   fgkCoolingTubeSupportRmax)
									+	fgkCarbonFiberJunctionLength,
									fgkEndLadderCarbonFiberLowerJunctionLength[0]
						  - 0.50 * (fgkEndLadderCarbonFiberLowerJunctionLength[0]
						  -			fgkEndLadderMountingBlockPosition[0]
						  -			fgkendladdercoolingsupportdistance[1]		
						  -			fgkCoolingTubeSupportWidth),
									- 0.5*fgkCoolingTubeSupportHeight);	 
  localendlladdercoolingtubetrans[0][5]->SetTranslation((fgkCoolingTubeSupportLength
									-	 fgkCoolingTubeSupportRmax)
									-	 fgkCarbonFiberJunctionLength
									+    fgkCarbonFiberTriangleLength,
									fgkEndLadderCarbonFiberLowerJunctionLength[0]
						  - 0.50 * (fgkEndLadderCarbonFiberLowerJunctionLength[0]
						  -			fgkEndLadderMountingBlockPosition[0]
						  -			fgkendladdercoolingsupportdistance[1]		
						  -			fgkCoolingTubeSupportWidth),
									- 0.5*fgkCoolingTubeSupportHeight);	 
  localendlladdercoolingtubetrans[1][0]->SetTranslation(-(fgkCoolingTubeSupportLength
									-   fgkCoolingTubeSupportRmax)
									+	fgkCarbonFiberJunctionLength,
							- 0.50 * (fgkMountingBlockToSensorSupport
							- 0.50 * (fgkSSDFlexHoleWidth-fgkSSDSensorSideSupportWidth)
							-		  fgkSSDStiffenerWidth-fgkSSDModuleStiffenerPosition[1]
							+		  fgkSSDSensorOverlap
							+		  fgkEndLadderCarbonFiberLowerJunctionLength[1]
							-		  fgkendladdercoolingsupportdistance[2]
							-		  fgkEndLadderMountingBlockPosition[1]
							-		  fgkCoolingTubeSupportWidth)
							+		  fgkEndLadderCarbonFiberLowerJunctionLength[1]
							-		  fgkendladdercoolingsupportdistance[2]
							-		  fgkCoolingTubeSupportWidth,
						  -		0.5*fgkCoolingTubeSupportHeight);	 
  localendlladdercoolingtubetrans[1][1]->SetTranslation((fgkCoolingTubeSupportLength
						  -	 fgkCoolingTubeSupportRmax)
						  -	 fgkCarbonFiberJunctionLength
						  +    fgkCarbonFiberTriangleLength,
						  - 0.50 * (fgkMountingBlockToSensorSupport
							- 0.50 * (fgkSSDFlexHoleWidth-fgkSSDSensorSideSupportWidth)
							-		  fgkSSDStiffenerWidth-fgkSSDModuleStiffenerPosition[1]
							+		  fgkSSDSensorOverlap
							+		  fgkEndLadderCarbonFiberLowerJunctionLength[1]
							-		  fgkendladdercoolingsupportdistance[2]
							-		  fgkEndLadderMountingBlockPosition[1]
							-		  fgkCoolingTubeSupportWidth)
							+		  fgkEndLadderCarbonFiberLowerJunctionLength[1]
							-		  fgkendladdercoolingsupportdistance[2]
							-		  fgkCoolingTubeSupportWidth,
						  -		0.5*fgkCoolingTubeSupportHeight);	 
  localendlladdercoolingtubetrans[1][2]->SetTranslation(-(fgkCoolingTubeSupportLength
									-   fgkCoolingTubeSupportRmax)
									+	fgkCarbonFiberJunctionLength,
										fgkEndLadderCarbonFiberLowerJunctionLength[1]
									- 0.5*fgkendladdercoolingsupportdistance[2],
									- 0.5*fgkCoolingTubeSupportHeight);	 
  localendlladdercoolingtubetrans[1][3]->SetTranslation((fgkCoolingTubeSupportLength
									-	 fgkCoolingTubeSupportRmax)
									-	 fgkCarbonFiberJunctionLength
									+    fgkCarbonFiberTriangleLength,
										fgkEndLadderCarbonFiberLowerJunctionLength[1]
									- 0.5*fgkendladdercoolingsupportdistance[2],
									- 0.5*fgkCoolingTubeSupportHeight);	 
  fendladdercoolingtubematrix[0] = new TGeoHMatrix*[6]; 
  fendladdercoolingtubematrix[1] = new TGeoHMatrix*[4]; 
  for(Int_t i=0; i<2; i++)
	for(Int_t j=0; j<(i==0?6:4); j++){
		fendladdercoolingtubematrix[i][j] = new TGeoHMatrix(*localendlladdercoolingtuberot);
		fendladdercoolingtubematrix[i][j]->MultiplyLeft(localendlladdercoolingtubetrans[i][j]);	
	}
  /////////////////////////////////////////////////////////////
  // SSD Hybrid Components Transformations
  /////////////////////////////////////////////////////////////
  const Int_t khybridmatrixnumber = 3;
  TGeoTranslation* localhybridtrans[khybridmatrixnumber];
  localhybridtrans[0] = new TGeoTranslation(0.5*fgkSSDStiffenerLength,
                                            0.5*fgkSSDStiffenerWidth,
                                            0.5*fgkSSDStiffenerHeight);
  localhybridtrans[1] = new TGeoTranslation(fgkSSDModuleStiffenerPosition[0],
                                            fgkSSDModuleStiffenerPosition[1],0.0);

  localhybridtrans[2] = new TGeoTranslation(
                      -  0.5*(fgkSSDSensorWidth-fgkCarbonFiberTriangleLength),
                      -      (2.*fgkSSDSensorLength-fgkSSDSensorOverlap)+
                              fgkSSDModuleStiffenerPosition[1]+fgkSSDStiffenerWidth
                      +		0.5*fgkSSDFlexHoleLength+2.*fgkCarbonFiberJunctionWidth
                      -		0.5*(fgkCarbonFiberLowerSupportWidth+fgkSSDSensorCenterSupportLength
                      -       fgkSSDSensorCenterSupportThickness[0]),
                      -      (fgkSSDModuleCoolingBlockToSensor+0.5*fgkCoolingTubeSupportHeight
                      -       fgkSSDSensorHeight-fgkSSDChipCablesHeight[3]-fgkSSDChipHeight
					  -       fgkSSDModuleVerticalDisalignment2)); 
  fhybridmatrix = new TGeoHMatrix();
  for(Int_t i=0; i<khybridmatrixnumber; i++) fhybridmatrix->MultiplyLeft(localhybridtrans[i]);
  /////////////////////////////////////////////////////////////
  // SSD Cooling Block Transformations
  /////////////////////////////////////////////////////////////
  const Int_t kcoolingblockmatrixnumber = 4;    
  TGeoTranslation* localcoolingblocktrans[kcoolingblockmatrixnumber];
  localcoolingblocktrans[0] = new TGeoTranslation(-2.*(fgkCoolingTubeSupportRmax
                            -  fgkCoolingTubeSupportRmin),0.0,
                               0.5*(fgkSSDCoolingBlockHoleCenter+fgkCoolingTubeRmax));
  localcoolingblocktrans[1] = new TGeoTranslation(0.5*fgkSSDStiffenerLength
                            -  0.5*fgkSSDModuleSensorSupportDistance-fgkSSDCoolingBlockLength,
                               0.0,fgkSSDStiffenerHeight);
  localcoolingblocktrans[2] = new TGeoTranslation(*localhybridtrans[1]);
  localcoolingblocktrans[3] = new TGeoTranslation(*localhybridtrans[2]);
  fcoolingblocksystematrix = new TGeoHMatrix();
  for(Int_t i=0; i<kcoolingblockmatrixnumber; i++)
      fcoolingblocksystematrix->MultiplyLeft(localcoolingblocktrans[i]);
  /////////////////////////////////////////////////////////////
  // SSD Stiffener Flex Transformations
  /////////////////////////////////////////////////////////////
  const Int_t klocalflexmatrixnumber = 4;
  TGeoCombiTrans** localflexmatrix[fgkflexnumber];
  for(Int_t i=0; i<fgkflexnumber; i++)    
      localflexmatrix[i] = new TGeoCombiTrans*[klocalflexmatrixnumber];
  for(Int_t i=0; i<fgkflexnumber; i++)
      for(Int_t j =0; j<klocalflexmatrixnumber; j++) 
            localflexmatrix[i][j] = new TGeoCombiTrans();
  Double_t ssdstiffenerseparation = fgkSSDSensorLength
								  - 2.*fgkSSDModuleStiffenerPosition[1]
								  -    fgkSSDStiffenerWidth;
  localflexmatrix[0][0]->SetTranslation(-fgkSSDFlexLength[0]
                                        +0.5*fgkSSDStiffenerLength,
                                         0.5*fgkSSDStiffenerWidth,
                                        -0.5*fgkSSDStiffenerHeight
                                        -0.5*fgkSSDFlexHeight[0]);
  localflexmatrix[1][0]->SetTranslation(-(fgkSSDStiffenerLength-fgkSSDFlexLength[0])
                                        +0.5*fgkSSDStiffenerLength,ssdstiffenerseparation
                                        -0.5*fgkSSDStiffenerWidth,
                                        -0.5*fgkSSDStiffenerHeight
                                        -0.5*fgkSSDFlexHeight[0]);
  TGeoRotation* localflexrot = new TGeoRotation();
  localflexrot->SetAngles(180.,0.,0.);    
  localflexmatrix[1][0]->SetRotation(localflexrot);
  for(Int_t i=0; i<fgkflexnumber; i++)
      for(Int_t j =1; j<klocalflexmatrixnumber; j++) 
            localflexmatrix[i][j]->SetTranslation(*localhybridtrans[j-1]);
  for(Int_t i=0; i<fgkflexnumber; i++){
      fstiffenerflexmatrix[i] = new TGeoHMatrix();
      for(Int_t j =0; j<klocalflexmatrixnumber; j++)   
            fstiffenerflexmatrix[i]->MultiplyLeft(localflexmatrix[i][j]);
  }
  /////////////////////////////////////////////////////////////
  // SSD End Flex Transformations
  /////////////////////////////////////////////////////////////
  TGeoRotation* localendflexrot = new TGeoRotation();
  localendflexrot->SetAngles(0.0,90.0,0.0);
  TGeoCombiTrans* localendflexmatrix = new TGeoCombiTrans();
  Double_t ssdflexradiusmax = (fgkSSDFlexLength[3]-fgkSSDFlexLength[2])
                            /  TMath::Tan(fgkSSDFlexAngle*TMath::DegToRad());
  Double_t ssdflexboxlength = fgkSSDFlexFullLength-2.*fgkSSDFlexAngle
                            * TMath::DegToRad()*ssdflexradiusmax
					                       - fgkSSDFlexLength[2]-TMath::Pi()
					                       * fgkSSDStiffenerHeight-fgkSSDFlexLength[0];
  Double_t trans = ssdflexboxlength*CosD(2.*fgkSSDFlexAngle)
                            + (ssdflexradiusmax-fgkSSDStiffenerHeight)*SinD(2.*fgkSSDFlexAngle)
                            +      fgkSSDFlexLength[2];
  localendflexmatrix->SetTranslation(fgkSSDFlexLength[0]-trans,
                              0.5*fgkSSDFlexWidth[0],
                              2.*fgkSSDStiffenerHeight
                            + 0.5*fgkSSDFlexHeight[0]);      
  localendflexmatrix->SetRotation(localendflexrot);
  for(Int_t i=0; i<fgkflexnumber; i++) 
      fendflexmatrix[i] = new TGeoHMatrix((*fstiffenerflexmatrix[i])*(*localendflexmatrix));
  /////////////////////////////////////////////////////////////
  // End Ladder Carbon Fiber Junction
  /////////////////////////////////////////////////////////////
  TGeoCombiTrans** localendladdercarbonfiberjunctionmatrix[fgkendlabbercarbonfiberjunctionumber];
  TGeoRotation** localendladdercarbonfiberjunctionrot[fgkendlabbercarbonfiberjunctionumber];    
  TGeoTranslation** localendladdercarbonfiberjunctiontrans[fgkendlabbercarbonfiberjunctionumber];    
  for(Int_t i=0; i<fgkendlabbercarbonfiberjunctionumber; i++){
      localendladdercarbonfiberjunctionmatrix[i] 
            = new TGeoCombiTrans*[fgkendladdercabonfiberjunctionmatrixnumber];
      localendladdercarbonfiberjunctionrot[i] 
            = new TGeoRotation*[fgkendladdercabonfiberjunctionmatrixnumber];
      localendladdercarbonfiberjunctiontrans[i] 
            = new TGeoTranslation*[fgkendladdercabonfiberjunctionmatrixnumber];
      fendladdercarbonfiberjunctionmatrix[i]
            = new TGeoHMatrix*[fgkendladdercabonfiberjunctionmatrixnumber];
  }
  for(Int_t i=0; i<fgkendlabbercarbonfiberjunctionumber; i++)    
      for(Int_t j=0; j<fgkendladdercabonfiberjunctionmatrixnumber; j++){
            localendladdercarbonfiberjunctionrot[i][j] = new TGeoRotation();
            localendladdercarbonfiberjunctiontrans[i][j] = new TGeoTranslation();
      }
  for(Int_t i=0; i<fgkendlabbercarbonfiberjunctionumber; i++)     
      for(Int_t j=0; j<fgkendladdercabonfiberjunctionmatrixnumber; j++)
          localendladdercarbonfiberjunctionrot[i][j]->SetAngles(120.*j,0.,0.);
  for(Int_t i=0; i<fgkendlabbercarbonfiberjunctionumber; i++){
      localendladdercarbonfiberjunctiontrans[i][1]->SetTranslation(fgkCarbonFiberTriangleLength,
                              0.0,0.0);
      localendladdercarbonfiberjunctiontrans[i][2]->SetTranslation(fgkCarbonFiberTriangleLength
		*                     CosD(fgkCarbonFiberTriangleAngle),fgkCarbonFiberTriangleLength
		*                     SinD(fgkCarbonFiberTriangleAngle),
                        0.5*(fgkEndLadderCarbonFiberLowerJunctionLength[i]
  -                          fgkEndLadderCarbonFiberUpperJunctionLength[i]));
  }
  TGeoCombiTrans* localendladdercarbonfiberjunctionglobalmatrix[fgkendlabbercarbonfiberjunctionumber];
  TGeoRotation* localendladdercarbonfiberjunctionglobalrot[fgkendlabbercarbonfiberjunctionumber];
  TGeoTranslation* localendladdercarbonfiberjunctionglobaltrans[fgkendlabbercarbonfiberjunctionumber];
  for(Int_t i=0; i<fgkendlabbercarbonfiberjunctionumber; i++){
      localendladdercarbonfiberjunctionglobalrot[i] = new TGeoRotation();
      localendladdercarbonfiberjunctionglobaltrans[i] = new TGeoTranslation();
      localendladdercarbonfiberjunctionglobalrot[i]->SetAngles(0.0,90.0,0.0);
      localendladdercarbonfiberjunctionglobaltrans[i]->SetTranslation(0.0,
            0.5*fgkEndLadderCarbonFiberLowerJunctionLength[i],0.0);
      localendladdercarbonfiberjunctionglobalmatrix[i] = 
            new TGeoCombiTrans(*localendladdercarbonfiberjunctionglobaltrans[i],
                               *localendladdercarbonfiberjunctionglobalrot[i]);
  }
  for(Int_t i=0; i<fgkendlabbercarbonfiberjunctionumber; i++)    
      for(Int_t j=0; j<fgkendladdercabonfiberjunctionmatrixnumber; j++){
            localendladdercarbonfiberjunctionmatrix[i][j] = 
                  new TGeoCombiTrans(*localendladdercarbonfiberjunctiontrans[i][j],
                                     *localendladdercarbonfiberjunctionrot[i][j]);
           fendladdercarbonfiberjunctionmatrix[i][j] =
            new TGeoHMatrix((*localendladdercarbonfiberjunctionglobalmatrix[i])
            *               (*localendladdercarbonfiberjunctionmatrix[i][j])); 
      }  
  /////////////////////////////////////////////////////////////
  // End Ladder Carbon Fiber Support
  /////////////////////////////////////////////////////////////
  TGeoTranslation* localendladdercarbonfibertrans[fgkendladdercarbonfibermatrixnumber];
  for(Int_t i=0; i<fgkendladdercarbonfibermatrixnumber; i++){
      localendladdercarbonfibertrans[i] = new TGeoTranslation();
      localendladdercarbonfibertrans[i]->SetTranslation(0.0,
            i==0 ? 0.0 :fgkCarbonFiberLowerSupportWidth,0.0);
      fendladdercarbonfibermatrix[i] = new TGeoHMatrix*[fgkcarbonfibersupportnumber];
  }
  for(Int_t i=0; i<fgkendladdercarbonfibermatrixnumber; i++)
      for(Int_t j=0; j<fgkcarbonfibersupportnumber; j++)
            fendladdercarbonfibermatrix[i][j] = 
            new TGeoHMatrix((*localendladdercarbonfibertrans[i])
            *(*fcarbonfibersupportmatrix[j]));
  /////////////////////////////////////////////////////////////
  // End Ladder SSD Mounting Block
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<fgkendladdermountingblocknumber; i++)
      fendladdermountingblockcombitrans[i] = new TGeoCombiTrans();
  for(Int_t i=0; i<fgkendladdermountingblocknumber; i++)
      fendladdermountingblockcombitrans[i]->SetTranslation(-  0.25*(fgkSSDMountingBlockLength[0]
                                +	 fgkSSDMountingBlockLength[1])
                                +  0.5*fgkCarbonFiberTriangleLength,
                                fgkEndLadderMountingBlockPosition[i],
                                -  fgkSSDMountingBlockHeight[1]
                                +  0.5*fgkSSDMountingBlockHeight[0]);
  TGeoRotation* endladdermountingblockrot = new TGeoRotation();
  endladdermountingblockrot->SetAngles(0.,90.,0.);
  for(Int_t i=0; i<fgkendladdermountingblocknumber; i++)
	fendladdermountingblockcombitrans[i]->SetRotation(*endladdermountingblockrot);
  /////////////////////////////////////////////////////////////
  // End Ladder SSD Mounting Block Clip Matrix 
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<fgkendladdermountingblocknumber; i++) 
	fendladdermountingblockclipmatrix[i] = new TGeoHMatrix*[2];
  
  TGeoRotation* localendladdercliprot = new TGeoRotation();
  TGeoTranslation* localendladdercliptrans = new TGeoTranslation();
  localendladdercliptrans->SetTranslation(-0.5*(fgkSSDMountingBlockLength[0]
										  -     fgkSSDMountingBlockLength[1])
										  + fgkSSDMountingBlockLength[0],0.,0.);
  localendladdercliprot->SetAngles(90.,180.,-90.);
  TGeoCombiTrans* localendladderclipcombitrans = 
			new TGeoCombiTrans(*localendladdercliptrans,*localendladdercliprot);
  for(Int_t i=0; i<fgkendladdermountingblocknumber; i++)
	for(Int_t j=0; j<2; j++){
		fendladdermountingblockclipmatrix[i][j] = 
						new TGeoHMatrix(*fendladdermountingblockcombitrans[i]);
		if(j!=0) fendladdermountingblockclipmatrix[i][j]->Multiply(localendladderclipcombitrans);
	}
  /////////////////////////////////////////////////////////////
  // End Ladder Carbon Fiber Lower Support
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<fgkendladderlowersuppnumber; i++)
      fendladderlowersupptrans[i] = 
            new TGeoTranslation(0.0,(1-i)*(fgkEndLadderMountingBlockPosition[i]
                        +  0.5*fgkSSDMountingBlockWidth),
                        -  0.5*fgkCarbonFiberLowerSupportHeight);
  fendladderlowersupptrans[2] = new TGeoTranslation(0.0,
									 fgkCarbonFiberLowerSupportVolumePosition[1]
								+    fgkCarbonFiberLowerSupportVolumePosition[0],
									 0.0);
  fendladderlowersupptrans[2]->Add(fendladderlowersupptrans[1]);
 /////////////////////////////////////////////////////////////
  // Matrix for positioning Ladder into mother volume
  /////////////////////////////////////////////////////////////
  TGeoHMatrix** ladderglobalmatrix[fgkladdernumber];
  for(Int_t i=0; i<fgkladdernumber; i++) 
	ladderglobalmatrix[i] = new TGeoHMatrix*[fgkladdernumber];
  TGeoRotation* localladdermotherrot = new TGeoRotation();
  localladdermotherrot->SetAngles(0.,90.,0.);  
  TGeoTranslation* localladdermothertrans[fgkladdernumber];
  TGeoCombiTrans* localladdermothercombitrans[fgkladdernumber];
  for(Int_t i=0; i<fgkladdernumber; i++){
	localladdermothertrans[i] = new TGeoTranslation(0.,
							  - fgkEndLadderCarbonFiberLowerJunctionLength[1]
							  + fgkEndLadderCarbonFiberLowerJunctionLength[0]
							  + (i==0?fgkSSDLay5SensorsNumber:fgkSSDLay6SensorsNumber)
							  * fgkCarbonFiberJunctionWidth,0.);
	localladdermothercombitrans[i] = new TGeoCombiTrans(*localladdermothertrans[i],
														*localladdermotherrot);
	ladderglobalmatrix[0][i] = new TGeoHMatrix(*localladdermothercombitrans[i]);
	ladderglobalmatrix[1][i] = new TGeoHMatrix(ladderglobalmatrix[0][i]->Inverse());
  }
  /////////////////////////////////////////////////////////////
  // Ladder Cables Matrices
  /////////////////////////////////////////////////////////////
  Double_t ssdflexradius = fgkSSDStiffenerHeight+2*fgkSSDFlexHeight[0]
					     + fgkSSDFlexHeight[1];  
  Double_t ssdladdercabletransx[3];
  ssdladdercabletransx[0] = (ssdflexradiusmax-fgkSSDFlexHeight[1]-ssdflexradius)
						  *   SinD(2.*fgkSSDFlexAngle)
						  *	  CosD(2.*fgkSSDFlexAngle);
  ssdladdercabletransx[1] = ((ssdflexradiusmax-fgkSSDFlexHeight[1]-ssdflexradius)
						  -     ssdladdercabletransx[0]
						  /     SinD(2.*fgkSSDFlexAngle))
						  *     CosD(fgkSSDFlexAngle);						
  ssdladdercabletransx[2] = (fgkSSDFlexFullLength-2.*fgkSSDFlexAngle
						  *	  TMath::DegToRad()*ssdflexradiusmax
						  -     fgkSSDFlexLength[2]-TMath::Pi()
						  *	  fgkSSDStiffenerHeight-fgkSSDFlexLength[0]
						  -	  fgkSSDLadderCableWidth)
						  *	  CosD(2.*fgkSSDFlexAngle);
  Double_t ssdladdercabletransz[3] = {ssdladdercabletransx[0]
						  *	TanD(2.*fgkSSDFlexAngle),
							ssdladdercabletransx[1]
						  *	TanD(fgkSSDFlexAngle),
							ssdladdercabletransx[2]
						  *	TanD(2.*fgkSSDFlexAngle)};	
  TGeoRotation* localladdercablerot[3];	
  for(Int_t i=0; i<3; i++) localladdercablerot[i] = new TGeoRotation();
  localladdercablerot[0]->SetAngles(90.,0.,0.);
  localladdercablerot[1]->SetAngles(90.,60.,-90.);
  localladdercablerot[2]->SetRotation((*localladdercablerot[1])
						 *			  (*localladdercablerot[0]));
  ////////////////////////////////////////////
  // LocalLadderCableCombiTransMatrix
  ////////////////////////////////////////////
  const Int_t klocalladdersidecablesnumber = 2;
  const Int_t klocalladdercombitransnumber = 5;
  TGeoCombiTrans** localladdercablecombitransmatrix[klocalladdersidecablesnumber];
  for(Int_t i=0; i<klocalladdersidecablesnumber; i++) 
	 localladdercablecombitransmatrix[i] = 
							   new TGeoCombiTrans*[klocalladdercombitransnumber];
  ///////////////////////////////////////////
  // Left Side Ladder Cables Transformations
  ///////////////////////////////////////////
  localladdercablecombitransmatrix[0][0]  =
						new TGeoCombiTrans(-0.5*fgkCarbonFiberTriangleLength,
						0.,0.,NULL);
  localladdercablecombitransmatrix[0][1] = 
	new TGeoCombiTrans(-0.5*(fgkSSDSensorWidth-fgkCarbonFiberTriangleLength),
					   - (2.*fgkSSDSensorLength-fgkSSDSensorOverlap)+
						 fgkSSDModuleStiffenerPosition[1]+fgkSSDStiffenerWidth
					   + 0.5*fgkSSDFlexHoleLength+2.*fgkCarbonFiberJunctionWidth
					   - 0.5*(fgkCarbonFiberLowerSupportWidth
					   + fgkSSDSensorCenterSupportLength
					   - fgkSSDSensorCenterSupportThickness[0]),
					   - (fgkSSDModuleCoolingBlockToSensor
					   + 0.5*fgkCoolingTubeSupportHeight
					   - fgkSSDSensorHeight-fgkSSDChipCablesHeight[3]
					   - fgkSSDChipHeight),NULL);
  localladdercablecombitransmatrix[0][2] = 
						new TGeoCombiTrans(fgkSSDModuleStiffenerPosition[0],
										   fgkSSDModuleStiffenerPosition[1],0.,0);
  localladdercablecombitransmatrix[0][3] = new TGeoCombiTrans(
					0.5*(fgkSSDStiffenerLength+fgkSSDChipNumber*fgkSSDChipLength
				   +(fgkSSDChipNumber-1)*fgkSSDChipSeparationLength),
				   fgkSSDSensorLength-2.*fgkSSDModuleStiffenerPosition[1]
					 - fgkSSDStiffenerWidth,- 0.5*fgkSSDChipHeight,
												new TGeoRotation("",180.,0.,0.));
  localladdercablecombitransmatrix[0][4] = 
						new TGeoCombiTrans(-ssdladdercabletransx[0]
						-     ssdladdercabletransx[1]-ssdladdercabletransx[2]
						+     fgkSSDFlexLength[0]-fgkSSDFlexLength[2],
							  0.,
							  0.5*fgkSSDFlexHeight[0]+2.*(fgkSSDFlexHeight[0]
						+	  fgkSSDFlexHeight[1])+2.*fgkSSDStiffenerHeight
						+     ssdladdercabletransz[0]-ssdladdercabletransz[1]
						+	  ssdladdercabletransz[2],localladdercablerot[2]);
  ///////////////////////////////////////////
  // Rigth Side Ladder Cables Transformations
  ///////////////////////////////////////////
  TGeoCombiTrans* localladdercablessdmodulematrix = 
	new TGeoCombiTrans(0.5*(fgkSSDStiffenerLength-fgkSSDChipNumber*fgkSSDChipLength
								  - (fgkSSDChipNumber-1)*fgkSSDChipSeparationLength),
									 fgkSSDStiffenerWidth,
								  - 0.5*fgkSSDFlexHeight[0],NULL);
  for(Int_t i=0; i<klocalladdercombitransnumber; i++)
   localladdercablecombitransmatrix[1][i] = 
			(i!=3 ? new TGeoCombiTrans(*localladdercablecombitransmatrix[0][i]):
					new TGeoCombiTrans(*localladdercablessdmodulematrix)); 	
  ///////////////////////////////////////////
  // Setting LadderCableHMatrix
  ///////////////////////////////////////////
  Int_t beamaxistrans[2][3];
  beamaxistrans[0][0] = fgkSSDLay5SensorsNumber/2; 
  beamaxistrans[0][1] = beamaxistrans[0][0]+1;
  beamaxistrans[0][2] = beamaxistrans[0][0]-1;
  beamaxistrans[1][0] = (fgkSSDLay6SensorsNumber-1)/2;
  beamaxistrans[1][1] = beamaxistrans[1][0]+1;
  beamaxistrans[1][2] = beamaxistrans[1][0];
  TGeoHMatrix** localladdercablehmatrix[fgkladdercablesnumber];
  TGeoRotation* laddercablerot = new TGeoRotation();
  TGeoTranslation* laddercabletrans = new TGeoTranslation();
  TGeoCombiTrans* laddercablecombitrans = new TGeoCombiTrans();
  Double_t* laddercabletransvector;	
  for(Int_t i=0; i<fgkladdercablesnumber; i++){ 
	localladdercablehmatrix[i] = new TGeoHMatrix*[klocalladdersidecablesnumber];
	fladdercablematrix[i] = new TGeoHMatrix*[fgkladdercablematrixnumber];
  }
  for(Int_t i=0; i<fgkladdercablesnumber; i++){
	for(Int_t j=0; j<klocalladdersidecablesnumber; j++){
		localladdercablehmatrix[i][j] = new TGeoHMatrix();
		for(Int_t k=0; k<klocalladdercombitransnumber; k++){
			localladdercablehmatrix[i][j]->MultiplyLeft(
			localladdercablecombitransmatrix[j][klocalladdercombitransnumber-k-1]);
        }
		laddercablerot->SetMatrix(localladdercablehmatrix[i][j]->GetRotationMatrix());
		laddercabletransvector = localladdercablehmatrix[i][j]->GetTranslation();
		laddercabletrans->SetTranslation(laddercabletransvector[0],
									 laddercabletransvector[1]
					+                (j==0 ? beamaxistrans[i][0] : 0.)
					*				 fgkCarbonFiberJunctionWidth,
									 laddercabletransvector[2]);
		laddercablecombitrans->SetRotation(*laddercablerot);
		laddercablecombitrans->SetTranslation(*laddercabletrans);	
		fladdercablematrix[i][j] = new TGeoHMatrix(*laddercablecombitrans);
	}
    fladdercablematrix[i][2] = 
					AddTranslationToHMatrix(fladdercablematrix[i][1],0.,
					beamaxistrans[i][1]*fgkCarbonFiberJunctionWidth,0.);
	fladdercablematrix[i][3] = 
					AddTranslationToHMatrix(fladdercablematrix[i][0],0.,
					beamaxistrans[i][2]*fgkCarbonFiberJunctionWidth,0.);
  }
  for(Int_t i=0; i<fgkladdercablesnumber; i++)
	for(Int_t j=0; j<klocalladdercombitransnumber-1; j++)
		fladdercablematrix[i][j]->MultiplyLeft(ladderglobalmatrix[1][i]);
  ///////////////////////////////////////////
  // Setting Ladder HMatrix
  ///////////////////////////////////////////
  Int_t ssdlaysensorsnumber[fgkladdernumber] = {fgkSSDLay5SensorsNumber,
												fgkSSDLay6SensorsNumber};
  for(Int_t i=0; i<fgkladdernumber; i++){
	fladdermatrix[i] = new TGeoHMatrix*[ssdlaysensorsnumber[i]];
	for(Int_t j=0; j<ssdlaysensorsnumber[i]; j++){
		fladdermatrix[i][j] = new TGeoHMatrix();
		fladdermatrix[i][j]->SetDx(-0.5*fgkCarbonFiberTriangleLength);
		fladdermatrix[i][j]->SetDy(fgkCarbonFiberJunctionWidth*j);
		fladdermatrix[i][j]->MultiplyLeft(ladderglobalmatrix[1][i]);
	}
  }
  ///////////////////////////////////////////
  // Setting SSD Sensor Matrix 
  ///////////////////////////////////////////
  TGeoCombiTrans* localssdsensorcombitrans[2];
  TGeoRotation* localssdsensorrot = new TGeoRotation();	
  localssdsensorrot->SetAngles(0.,90.,0.);	
  TGeoTranslation* localssdsensortrans[2];
  for(Int_t i=0; i<2; i++) localssdsensortrans[i] = new TGeoTranslation();
  localssdsensortrans[0]->SetTranslation(0.5*fgkCarbonFiberTriangleLength,
					  -		(2.*fgkSSDSensorLength-fgkSSDSensorOverlap)+
                              fgkSSDModuleStiffenerPosition[1]+fgkSSDStiffenerWidth
                      +		0.5*fgkSSDFlexHoleLength+2.*fgkCarbonFiberJunctionWidth
                      -		0.5*(fgkCarbonFiberLowerSupportWidth+fgkSSDSensorCenterSupportLength
                      -       fgkSSDSensorCenterSupportThickness[0])+0.5*fgkSSDSensorLength,
							0.5*fgkSSDSensorHeight-0.5*fgkCoolingTubeSupportHeight
					  -		fgkSSDModuleCoolingBlockToSensor-0.5*fgkSSDModuleVerticalDisalignment
					  +    (fgkSSDSensorSideSupportHeight[1]
					  -		fgkSSDSensorSideSupportHeight[0])
					  +     0.5*fgkSSDModuleVerticalDisalignment2);
  localssdsensortrans[1]->SetTranslation(0.5*fgkCarbonFiberTriangleLength,
					  -	   (2.*fgkSSDSensorLength-fgkSSDSensorOverlap)+
                              fgkSSDModuleStiffenerPosition[1]+fgkSSDStiffenerWidth
                      +		0.5*fgkSSDFlexHoleLength+2.*fgkCarbonFiberJunctionWidth
                      -		0.5*(fgkCarbonFiberLowerSupportWidth+fgkSSDSensorCenterSupportLength
                      -       fgkSSDSensorCenterSupportThickness[0])+0.5*fgkSSDSensorLength,
						    0.5*fgkSSDSensorHeight-0.5*fgkCoolingTubeSupportHeight
							-fgkSSDModuleCoolingBlockToSensor-0.5*fgkSSDModuleVerticalDisalignment
						+	0.5*fgkSSDModuleVerticalDisalignment2);
  for(Int_t i=0; i<2; i++) 
	localssdsensorcombitrans[i] = new TGeoCombiTrans(*localssdsensortrans[i],
													 *localssdsensorrot);	
    for(Int_t i=0; i<fgkladdernumber; i++){
	fssdsensormatrix[i] = new TGeoHMatrix*[ssdlaysensorsnumber[i]];
	for(Int_t j=0; j<ssdlaysensorsnumber[i]; j++){
		switch(i){
			case 0: //Ladder of Layer5  
			fssdsensormatrix[i][j] = new TGeoHMatrix((*fladdermatrix[i][j])
								   * ((j%2==0 ? *localssdsensorcombitrans[0] :
												*localssdsensorcombitrans[1])));
			break;
			case 1: //Ladder of Layer6 
			fssdsensormatrix[i][j] = new TGeoHMatrix((*fladdermatrix[i][j])
								   * ((j%2==0 ? *localssdsensorcombitrans[1] :
												*localssdsensorcombitrans[0])));
		break;
		}
	  }
  }	
  //////////////////////////
  // Setting SSD End Ladder  
  //////////////////////////
  for(Int_t i=0; i<2; i++) fendladdersegmentmatrix[i] = new TGeoHMatrix*[2];
  for(Int_t i=0; i<2; i++){
	fendladdersegmentmatrix[0][i] = new TGeoHMatrix();
	fendladdersegmentmatrix[0][i]->SetDx(-0.5*fgkCarbonFiberTriangleLength);
	fendladdersegmentmatrix[0][i]->SetDy(fgkCarbonFiberJunctionWidth*ssdlaysensorsnumber[i]);
	fendladdersegmentmatrix[0][i]->MultiplyLeft(ladderglobalmatrix[1][i]);
	fendladdersegmentmatrix[1][i] = new TGeoHMatrix();
	fendladdersegmentmatrix[1][i]->SetDx(-0.5*fgkCarbonFiberTriangleLength);
	fendladdersegmentmatrix[1][i]->RotateZ(180.0);
	fendladdersegmentmatrix[1][i]->MultiplyLeft(ladderglobalmatrix[1][i]);
   }
  /////////////////////////////////////////////////////
  // Setting the CombiTransformation to pass ITS center 
  /////////////////////////////////////////////////////
  Double_t itscentertransz[fgklayernumber];
  itscentertransz[0] = fgkSSDLay5LadderLength
					 - fgkLay5CenterITSPosition;
  itscentertransz[1] = fgkSSDLay6LadderLength
					 - fgkLay6CenterITSPosition;
  Double_t itssensortransy = fgkSSDModuleCoolingBlockToSensor
						   + 0.5*fgkCoolingTubeSupportHeight;
  TGeoRotation* itscenterrot[3];
  for(Int_t i=0; i<fgklayernumber; i++) itscenterrot[i] = new TGeoRotation();
  itscenterrot[0]->SetAngles(90.,180.,-90.);
  itscenterrot[1]->SetAngles(0.,90.,0.);
  itscenterrot[2] = new TGeoRotation((*itscenterrot[1])*(*itscenterrot[0]));
  TGeoCombiTrans* itscentercombitrans[fgklayernumber];
  for(Int_t i=0; i<fgklayernumber; i++) 
	itscentercombitrans[i] = new TGeoCombiTrans(0.,
							 itssensortransy,
							 fgkEndLadderCarbonFiberLowerJunctionLength[1]
						   - itscentertransz[i],itscenterrot[2]);
  TGeoRotation** locallayerrot[fgklayernumber];
  TGeoTranslation** locallayertrans[fgklayernumber];	
  TGeoCombiTrans** locallayercombitrans[fgklayernumber];
  TGeoTranslation* localbeamaxistrans[fgklayernumber];
  localbeamaxistrans[0] = new TGeoTranslation(0.,0.,0.5*fgkSSDLay5LadderLength
					 - fgkLay5CenterITSPosition);
  localbeamaxistrans[1] = new TGeoTranslation(0.,0.,0.5*fgkSSDLay6LadderLength
					 - fgkLay6CenterITSPosition);
  const Int_t kssdlayladdernumber[fgklayernumber] = 
			{fgkSSDLay5LadderNumber,fgkSSDLay6LadderNumber};
  for(Int_t i=0; i<fgklayernumber; i++){
    locallayerrot[i] = new TGeoRotation*[kssdlayladdernumber[i]];
    locallayertrans[i] = new TGeoTranslation*[kssdlayladdernumber[i]];
	locallayercombitrans[i] = new TGeoCombiTrans*[kssdlayladdernumber[i]];
	flayermatrix[i] = new TGeoHMatrix*[kssdlayladdernumber[i]];
  }
  Double_t layerladderangleposition[fgklayernumber] = 
		{360./fgkSSDLay5LadderNumber,360./fgkSSDLay6LadderNumber};
  Double_t layerradius = 0.;
  for(Int_t i=0; i<fgklayernumber; i++){	
	for(Int_t j=0; j<kssdlayladdernumber[i]; j++){
		switch(i){
			case 0: //Ladder of Layer5  
			layerradius = (j%2==0 ? fgkSSDLay5RadiusMin: fgkSSDLay5RadiusMax);
			break;
			case 1: //Ladder of Layer6 
			layerradius = (j%2==0 ? fgkSSDLay6RadiusMin: fgkSSDLay6RadiusMax);
		break;
		}
		locallayerrot[i][j] = new TGeoRotation();
		locallayertrans[i][j] = new TGeoTranslation();
		locallayerrot[i][j]->SetAngles(j*layerladderangleposition[i],0.,0.);
		locallayertrans[i][j]->SetTranslation(layerradius 
							  *	CosD(90.0+j*layerladderangleposition[i]),
							    layerradius 
							  * SinD(90.0+j*layerladderangleposition[i]),0.);
		locallayercombitrans[i][j] = new TGeoCombiTrans(*locallayertrans[i][j],
									 *locallayerrot[i][j]);
		flayermatrix[i][j] = new TGeoHMatrix((*locallayercombitrans[i][j])*(*itscentercombitrans[i]));
		flayermatrix[i][j]->Multiply(ladderglobalmatrix[0][i]);
		flayermatrix[i][j]->MultiplyLeft(localbeamaxistrans[i]);
	}
  }
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i< kcarbonfibersupportmatrixnumber[1]; i++){
	delete carbonfiberot[i];
	delete localcarbonfibersupportmatrix[i];
  }
  for(Int_t i=0; i< fgkcarbonfiberjunctionumber; i++){
     for(Int_t j=0; j< kcarbonfiberjunctionmatrixnumber; j++){
       delete localcarbonfiberjunctionmatrix[i][j];
	   delete localcarbonfiberjunctionrot[i][j];
	   delete localcarbonfiberjunctiontrans[i][j];
	   }
       delete [] localcarbonfiberjunctionmatrix[i];
       delete [] localcarbonfiberjunctionrot[i];
       delete [] localcarbonfiberjunctiontrans[i];
  }
  for(Int_t i=0; i<fgkcarbonfiberlowersupportnumber; i++) 
	   delete localcarbonfiberlowersupportrans[i];
  for(Int_t i=0; i< fgkssdsensorsupportnumber; i++){
     for(Int_t j=0; j< kssdsensorsupportmatrixnumber; j++){
       delete localssdsensorsupportmatrix[i][j];
	   delete localssdsensorsupportrot[i][j];
	   delete localssdsensorsupportrans[i][j];
	   }
       delete [] localssdsensorsupportmatrix[i];
       delete [] localssdsensorsupportrot[i];
       delete [] localssdsensorsupportrans[i];
  }
  for(Int_t i=0; i<kcoolingtubesupportmatrixnumber; i++){
	delete localcoolingtubesupportmatrix[i];
	delete localcoolingtubesupportrot[i];
	delete localcoolingtubesupportrans[i];
  }
  for(Int_t i=0; i<4; i++){
	for(Int_t j=0; j<2; j++){
		delete localcoolingtubevect[i][j];
		delete localcoolingtubetrans[i][j];
	}
	delete [] localcoolingtubevect[i];
	delete [] localcoolingtubetrans[i];
  }
 delete endladdermountingblockrot;
 for(Int_t i=0; i<khybridmatrixnumber; i++) delete localhybridtrans[i];
 for(Int_t i=0; i<kcoolingblockmatrixnumber; i++) delete localcoolingblocktrans[i];
 for(Int_t i=0; i<fgkflexnumber; i++){
      for(Int_t j=1; j<klocalflexmatrixnumber; j++) 
            delete localflexmatrix[i][j];
      delete [] localflexmatrix[i];
 }
 delete localendlladdercoolingtuberot;
 for(Int_t i=0; i<2; i++){
	for(Int_t j=0; j<(i==0?6:4); j++)
		delete localendlladdercoolingtubetrans[i][j];
	delete [] localendlladdercoolingtubetrans[i];
  }

 delete localflexrot;
 delete localendflexrot;
 delete localendflexmatrix;
 for(Int_t i=0; i<fgkladdernumber; i++){ 
	delete localladdermothertrans[i];
	delete localladdermothercombitrans[i];
  }
 delete localladdermotherrot;
 for(Int_t i=0; i<fgkendlabbercarbonfiberjunctionumber; i++){    
      for(Int_t j=0; j<fgkendladdercabonfiberjunctionmatrixnumber; j++){
            delete localendladdercarbonfiberjunctionmatrix[i][j];
            delete localendladdercarbonfiberjunctionrot[i][j];
            delete localendladdercarbonfiberjunctiontrans[i][j];
      }
      delete [] localendladdercarbonfiberjunctionmatrix[i];
      delete [] localendladdercarbonfiberjunctionrot[i];
      delete [] localendladdercarbonfiberjunctiontrans[i];
      delete localendladdercarbonfiberjunctionglobalrot[i];
      delete localendladdercarbonfiberjunctionglobaltrans[i];
      delete localendladdercarbonfiberjunctionglobalmatrix[i];
 }
  for(Int_t i=0; i<2; i++){
	for(Int_t j=0; j<(i==0?4:2); j++) delete localendladdercooltubetrans[i][j];
	delete [] localendladdercooltubetrans[i];
  }
  for(Int_t i=0; i<fgkendladdercarbonfibermatrixnumber; i++)
      delete localendladdercarbonfibertrans[i];
  for(Int_t i=0; i<3; i++) delete localladdercablerot[i];
  for(Int_t i=0; i<klocalladdersidecablesnumber; i++){
	for(Int_t j=0; j<klocalladdercombitransnumber; j++)
		delete localladdercablecombitransmatrix[i][j];
		delete []localladdercablecombitransmatrix[i];
  }
  delete localendladdercliprot;
  delete localendladdercliptrans;
  for(Int_t i=0; i<fgkladdercablesnumber; i++){
	for(Int_t j=0; j<klocalladdersidecablesnumber; j++)
		delete localladdercablehmatrix[i][j];
	delete []localladdercablehmatrix[i];
  }
  delete laddercablerot;
  delete laddercabletrans;
  delete laddercablecombitrans;
  delete localladdercablessdmodulematrix;
  delete localssdsensorrot;	
  for(Int_t i=0; i<2; i++){
	delete localssdsensortrans[i];
	delete localssdsensorcombitrans[i];
  }
  for(Int_t i=0; i<fgklayernumber; i++){
	for(Int_t j=0; j<kssdlayladdernumber[i]; j++){
		delete locallayerrot[i][j];
		delete locallayertrans[i][j];
		delete locallayercombitrans[i][j];
    }
	delete [] locallayerrot[i];
	delete [] locallayertrans[i];
	delete [] locallayercombitrans[i];
	delete localbeamaxistrans[i];
  }
  for(Int_t i=0; i<3; i++) delete itscenterrot[i];
  for(Int_t i=0; i<fgkladdernumber; i++){
	for(Int_t j=0; j<fgkladdernumber; j++)
		delete ladderglobalmatrix[i][j];
	delete [] ladderglobalmatrix[i];
  }
  /////////////////////////////////////////////////////////////
  fTransformationMatrices = kTRUE;	
}
///////////////////////////////////////////////////////////////////////////////
void AliITSv11GeometrySSD::CreateBasicObjects(){
  /////////////////////////////////////////////////////////////  
  // Method generating the Objects of SSD Geometry    
  /////////////////////////////////////////////////////////////
  // SSD Sensor
  ///////////////////////////////////
  SetSSDSensor();
  /////////////////////////////////////////////////////////////  
  // Carbon Fiber Support    
  /////////////////////////////////////////////////////////////  
  TList* carbonfibersupportlist = GetCarbonFiberSupportList();  
  for(Int_t i=0; i<fgkcarbonfibersupportnumber; i++) 
      fcarbonfibersupport[i] = (TGeoVolume*)carbonfibersupportlist->At(i);
  /////////////////////////////////////////////////////////////
  // Carbon Fiber Junction 
  /////////////////////////////////////////////////////////////
  fcarbonfiberjunction = GetCarbonFiberJunction(fgkCarbonFiberJunctionWidth-fgkSSDTolerance);
  /////////////////////////////////////////////////////////////
  // Carbon Fiber Lower Support
  /////////////////////////////////////////////////////////////
  TList* carbonfiberlowersupportlist = GetCarbonFiberLowerSupportList();
  for(Int_t i=0; i<fgkcarbonfiberlowersupportnumber; i++)
 	fcarbonfiberlowersupport[i] = (TGeoVolume*)carbonfiberlowersupportlist->At(i);
  /////////////////////////////
  // SSD Sensor Support
  /////////////////////////////
  for(Int_t i=0; i<fgkvolumekind; i++) fssdsensorsupport[i] = 
										new TGeoVolume*[fgkssdsensorsupportnumber]; 
  Double_t sidesupporthickness[2] = {fgkSSDSensorSideSupportThickness[0],
									 fgkSSDSensorSideSupportThickness[1]};
  for(Int_t i=0; i<fgkssdsensorsupportnumber-1; i++){
	fssdsensorsupport[0][i] = GetSSDSensorSupport(fgkSSDSensorSideSupportLength,
											   fgkSSDSensorSideSupportHeight[i],
											   fgkSSDSensorSideSupportWidth,
											   sidesupporthickness);  
	fssdsensorsupport[1][i] = GetSSDSensorSupport(fgkSSDSensorCenterSupportLength,
											   fgkSSDSensorCenterSupportHeight[i],
											   fgkSSDSensorCenterSupportWidth,
											   sidesupporthickness);
  }
  /////////////////////////////////////////////////////////////
  // SSD Cooling Tube Support
  /////////////////////////////////////////////////////////////
  Int_t edgesnumber = 16;
  fcoolingtubesupport = GetCoolingTubeSupport(edgesnumber);	  
  /////////////////////////////////////////////////////////////
  // SSD Hybrid
  /////////////////////////////////////////////////////////////
  TList* ssdhybridcomponentslist = GetSSDHybridParts();
  for(Int_t i=0; i<fgkhybridcompnumber; i++) 
	fssdhybridcomponent[i] = (TGeoVolume*)ssdhybridcomponentslist->At(i);
  /////////////////////////////////////////////////////////////
  // SSD Cooling Block System
  /////////////////////////////////////////////////////////////
  fssdcoolingblocksystem = GetCoolingBlockSystem();
   /////////////////////////////////////////////////////////////
  // SSD Cooling Tube
  /////////////////////////////////////////////////////////////
  TList* coolingtubelist = GetCoolingTubeList();	
  for(Int_t i=0; i<fgkcoolingtubenumber; i++)	
	fcoolingtube[i] = (TGeoVolume*)coolingtubelist->At(i);
  for(Int_t i=0; i<fgkendladdercoolingtubenumber; i++)	
	fendladdercoolingtube[i] = 
			(TGeoVolume*)coolingtubelist->At(fgkcoolingtubenumber+i);
  /////////////////////////////////////////////////////////////
  // SSD Flex  
  /////////////////////////////////////////////////////////////
  fssdstiffenerflex = GetSSDStiffenerFlex();
  fssdendflex = GetSSDEndFlex();
  ///////////////////////////////////
  // End Ladder Carbon Fiber Junction
  ///////////////////////////////////
  for(Int_t i=0; i<fgkendlabbercarbonfiberjunctionumber; i++) 
						   fendladdercarbonfiberjunction[i] = 
						   new TGeoVolume*[fgkendlabbercarbonfiberjunctionumber];
  for(Int_t i=0; i<fgkendlabbercarbonfiberjunctionumber; i++){
    fendladdercarbonfiberjunction[i][0] = 
		  GetCarbonFiberJunction(fgkEndLadderCarbonFiberLowerJunctionLength[i]-fgkSSDTolerance);
    fendladdercarbonfiberjunction[i][1] = 
		  GetCarbonFiberJunction(fgkEndLadderCarbonFiberUpperJunctionLength[i]-fgkSSDTolerance);
  }
  ///////////////////////////////////
  // End Ladder Mounting Block
  ///////////////////////////////////
  fendladdermountingblock = GetSSDMountingBlock();
  ///////////////////////////////////
  // End Ladder Mounting Block
  ///////////////////////////////////
  fendladdermountingblockclip = GetMountingBlockClip();
  ///////////////////////////////////
  // Ladder Support 
  ///////////////////////////////////
  TList* laddersupportlist = GetMountingBlockSupport(20);
  fLay5LadderSupport[0] = (TGeoVolume*)laddersupportlist->At(0);
  fLay5LadderSupport[1] = (TGeoVolume*)laddersupportlist->At(1);
  fLay6LadderSupport[0] = (TGeoVolume*)laddersupportlist->At(2);
  fLay6LadderSupport[1] = (TGeoVolume*)laddersupportlist->At(3);
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  delete carbonfibersupportlist;
  delete carbonfiberlowersupportlist;
  delete ssdhybridcomponentslist;
  delete laddersupportlist;
  /////////////////////////////////////////////////////////////
  fBasicObjects = kTRUE;
}
/////////////////////////////////////////////////////////////////////////////////
void AliITSv11GeometrySSD::SetSSDSensor(){
  ////////////////////////////////////////////////////////////////
  // Method generating SSD Sensors: it sets the private variables
  // fSSDSensor5, fSSDSensor6  
  ////////////////////////////////////////////////////////////////
  Double_t ssdsensitivelength = fgkSSDSensorLength-2.*fgkSSDSensorInsensitiveLength;
  Double_t ssdsensitivewidth  = fgkSSDSensorWidth-2.*fgkSSDSensorInsensitiveWidth;
  TGeoBBox* ssdsensorsensitiveshape = new TGeoBBox("SSDSensorSensitiveShape",
                                                0.5*ssdsensitivewidth,
                                                0.5*fgkSSDSensorHeight,
                                                0.5*ssdsensitivelength);
  TGeoVolume* ssdsensorsensitiveLay5 = 
	new TGeoVolume(fgSSDsensitiveVolName5,ssdsensorsensitiveshape,fSSDSensorMedium);
  TGeoVolume* ssdsensorsensitiveLay6 = 
	new TGeoVolume(fgSSDsensitiveVolName6,ssdsensorsensitiveshape,fSSDSensorMedium);
  ssdsensorsensitiveLay5->SetLineColor(fColorSilicon);
  ssdsensorsensitiveLay6->SetLineColor(fColorSilicon);
  TGeoBBox* ssdsensorinsensitiveshape[2];
  ssdsensorinsensitiveshape[0] = new TGeoBBox("SSDSensorInsensitiveShape1",
                                                0.5*fgkSSDSensorInsensitiveWidth,
                                                0.5*fgkSSDSensorHeight,
                                                0.5*fgkSSDSensorLength);
  ssdsensorinsensitiveshape[1] = new TGeoBBox("SSDSensorInsensitiveShape2",
                                                0.5*ssdsensitivewidth,
                                                0.5*fgkSSDSensorHeight,
                                                0.5*fgkSSDSensorInsensitiveWidth);
  const char* ssdsensorinsensitivename[2] = {"SSDSensorInsensitive1",
                                             "SSDSensorInsensitive2"};
  TGeoVolume* ssdsensorinsensitive[2];
  for(Int_t i=0; i<2; i++){ ssdsensorinsensitive[i] = 
      new TGeoVolume(ssdsensorinsensitivename[i],ssdsensorinsensitiveshape[i],
                     fSSDSensorMedium);
      ssdsensorinsensitive[i]->SetLineColor(fColorCarbonFiber);
  }
  /////////////////////////////////////////////////////////////
  // Virtual Volume containing SSD Sensor  
  /////////////////////////////////////////////////////////////
  TGeoBBox* virtualSSDSensorShape = new TGeoBBox("SSDSensorShape",
											     0.5*fgkSSDSensorWidth,
											     0.5*fgkSSDSensorHeight,
											     0.5*fgkSSDSensorLength);
  fSSDSensor5 = new TGeoVolume("ITSssdSensor5",virtualSSDSensorShape,
										 fSSDAir);	
  fSSDSensor6 = new TGeoVolume("ITSssdSensor6",virtualSSDSensorShape,
										 fSSDAir);	
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<4; i++){ 
            fSSDSensor5->AddNode(i%2==0 ? ssdsensorinsensitive[0]:
            ssdsensorinsensitive[1],i<2?1:2,
			new TGeoTranslation(
			 0.5*(1.+TMath::Power(-1.,i))*(i==0?-1.: 1.)
      *   (ssdsensorsensitiveshape->GetDX()+ssdsensorinsensitiveshape[0]->GetDX()),0.,			
			0.5*(1.-TMath::Power(-1.,i))*(i==1? 1.:-1.)
      *   (ssdsensorsensitiveshape->GetDZ()+ssdsensorinsensitiveshape[1]->GetDZ())));    
            fSSDSensor6->AddNode(i%2==0 ? ssdsensorinsensitive[0]:
            ssdsensorinsensitive[1],i<2?1:2,
			new TGeoTranslation(
			 0.5*(1.+TMath::Power(-1.,i))*(i==0?-1.: 1.)
      *   (ssdsensorsensitiveshape->GetDX()+ssdsensorinsensitiveshape[0]->GetDX()),0.,			
			0.5*(1.-TMath::Power(-1.,i))*(i==1? 1.:-1.)
      *   (ssdsensorsensitiveshape->GetDZ()+ssdsensorinsensitiveshape[1]->GetDZ())));    
  }
    fSSDSensor5->AddNode(ssdsensorsensitiveLay5,1);
    fSSDSensor6->AddNode(ssdsensorsensitiveLay6,1);
}
///////////////////////////////////////////////////////////////////////////////
TList* AliITSv11GeometrySSD::GetCarbonFiberSupportList(){
  /////////////////////////////////////////////////////////////  
  // Method generating the Carbon Fiber Support   
  /////////////////////////////////////////////////////////////  
  const Int_t kvertexnumber = 4;
  const Int_t kshapesnumber = 2;
  TVector3** vertexposition[kshapesnumber];
  for(Int_t i=0; i<kshapesnumber; i++) vertexposition[i] = new TVector3*[kvertexnumber];
  Double_t carbonfibersupportxaxisEdgeproj = 
		fgkCarbonFiberSupportEdgeLength*TMath::Cos(fgkCarbonFiberJunctionAngle[0]
	*	TMath::DegToRad());
  Double_t theta = TMath::ATan(fgkCarbonFiberSupportYAxisLength
				 /			   fgkCarbonFiberSupportXAxisLength);
  /////////////////////
  //Vertex Positioning
  ////////////////////
  vertexposition[0][0] = new TVector3();
  vertexposition[0][1] = new TVector3(fgkCarbonFiberSupportXAxisLength,
									  fgkCarbonFiberSupportYAxisLength);
  vertexposition[0][2] = new TVector3(carbonfibersupportxaxisEdgeproj,
									  carbonfibersupportxaxisEdgeproj
					   *			  TMath::Tan(theta));
  vertexposition[0][3] = new TVector3(fgkCarbonFiberSupportXAxisLength
					   -			  carbonfibersupportxaxisEdgeproj,
									  fgkCarbonFiberSupportYAxisLength
					   -			  vertexposition[0][2]->Y());
  ////////////////////////////////////////////////////
  //Setting the parameters for Isometry Transformation
  ////////////////////////////////////////////////////
  Double_t symmetryplaneposition = (fgkCarbonFiberSupportYAxisLength
								 +	fgkCarbonFiberSupportTopEdgeDist[0]
								 +	fgkCarbonFiberSupportWidth);
  Double_t* param = new Double_t[4]; 
  param[0] = 0., param[1] = 1., param[2] = 0., param[3] = -symmetryplaneposition;
  for(Int_t j=0; j<kvertexnumber; j++) vertexposition[1][j] = 
				  new TVector3((GetReflection(vertexposition[0][j],param))->X(),
							  (GetReflection(vertexposition[0][j],param))->Y());
  char* carbonfibersupportshapename[kshapesnumber] = 
						{"CarbonFiberSupportShape1","CarbonFiberSupportShape2"};
  char* carbonfibersupportname[kshapesnumber] = 
						{"CarbonFiberSupport1","CarbonFiberSupport2"};
  TGeoArb8* carbonfibersupportshape[kshapesnumber]; 
  TGeoVolume* carbonfibersupport[kshapesnumber];
  TList* carbonfibersupportlist = new TList();
  Double_t width[2] = {fgkCarbonFiberSupportWidth,fgkCarbonFiberSupportWidth};
  Double_t carbonfibersupportheight = 
	  carbonfibersupportxaxisEdgeproj*TMath::Tan(fgkCarbonFiberJunctionAngle[0]
	  *TMath::DegToRad());
  for(Int_t i = 0; i< kshapesnumber; i++){
   carbonfibersupportshape[i] = 
					GetArbShape(vertexposition[i],width,carbonfibersupportheight,
								carbonfibersupportshapename[i],i==0 ? 1: -1);
   carbonfibersupport[i] = new TGeoVolume(carbonfibersupportname[i],
						   carbonfibersupportshape[i],fSSDCarbonFiberMedium);
   carbonfibersupport[i]->SetLineColor(fColorCarbonFiber);
   carbonfibersupportlist->Add(carbonfibersupport[i]);	
   }
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i< kshapesnumber; i++){
     for(Int_t j=0; j< kvertexnumber; j++)
	   delete vertexposition[i][j];
       delete [] vertexposition[i];
  }
  delete [] param;
  /////////////////////////////////////////////////////////////
   return carbonfibersupportlist;
}
/////////////////////////////////////////////////////////////////////////////////
TGeoVolume* AliITSv11GeometrySSD::GetCarbonFiberJunction(Double_t width){
  /////////////////////////////////////////////////////////////
  // Method generating SSD Carbon Fiber Junction
  /////////////////////////////////////////////////////////////
  const Int_t kvertexnumber = 6;
  TGeoXtru* carbonfiberjunctionshape = new TGeoXtru(2);
  Double_t reflectionparam[3] = {TMath::Tan(fgkCarbonFiberJunctionAngle[0]
							  *  TMath::DegToRad()),-1.,0.};
  TVector3* vertex[kvertexnumber];
  vertex[0] = new TVector3();
  vertex[3] = new TVector3(fgkCarbonFiberJunctionEdge[0]
			*			  TMath::Cos(fgkCarbonFiberJunctionAngle[0]
			*			  TMath::DegToRad()),
						  fgkCarbonFiberJunctionEdge[0]
			*			  TMath::Sin(fgkCarbonFiberJunctionAngle[0]
			*			  TMath::DegToRad()));
  vertex[4] = new TVector3(fgkCarbonFiberJunctionLength,
						   fgkCarbonFiberJunctionEdge[1]);
  vertex[5] = new TVector3(fgkCarbonFiberJunctionLength); 
  vertex[1] = GetReflection(vertex[5],reflectionparam);	
  vertex[2] = GetReflection(vertex[4],reflectionparam);	
  Double_t xvertexpoints[6], yvertexpoints[6];
  for(Int_t i=0; i<kvertexnumber; i++) 
	  xvertexpoints[i] = vertex[i]->X(), yvertexpoints[i] = vertex[i]->Y();
  carbonfiberjunctionshape->DefinePolygon(kvertexnumber,xvertexpoints,yvertexpoints);
  carbonfiberjunctionshape->DefineSection(0,-0.5*width);
  carbonfiberjunctionshape->DefineSection(1,0.5*width);
  TGeoVolume* carbonfiberjunction = new TGeoVolume("CarbonFiberJunction",
								carbonfiberjunctionshape,fSSDCarbonFiberMedium);
  carbonfiberjunction->SetLineColor(fColorCarbonFiber);
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for (Int_t i=0; i<kvertexnumber; i++) delete vertex[i];
  ///////////////////////////////////////////////////////////// 
  return carbonfiberjunction;
}
////////////////////////////////////////////////////////////////////////////////
TList* AliITSv11GeometrySSD::GetCarbonFiberLowerSupportList(){
  /////////////////////////////////////////////////////////////
  // Method generating the Carbon Fiber Lower Support   
  /////////////////////////////////////////////////////////////  
  const Int_t kvertexnumber = 4;
  const Int_t kshapesnumber = 2;
  Double_t width[2] = {fgkCarbonFiberLowerSupportWidth,
								fgkCarbonFiberLowerSupportWidth};
  TVector3** vertexposition[kshapesnumber];
  for(Int_t i = 0; i<kshapesnumber; i++) vertexposition[i] = 
						 new TVector3*[kvertexnumber];
  //First Shape Vertex Positioning
  vertexposition[0][0] = new TVector3(fgkCarbonFiberLowerSupportLowerLenght);
  vertexposition[0][1] = new TVector3(fgkCarbonFiberTriangleLength
					   -		fgkCarbonFiberLowerSupportLowerLenght);
  vertexposition[0][2] = new TVector3();
  vertexposition[0][3] = new TVector3(fgkCarbonFiberTriangleLength);
  //Second Shape Vertex Positioning
  Double_t theta = TMath::ATan((fgkCarbonFiberLowerSupportVolumePosition[1]
				 -				fgkCarbonFiberLowerSupportVolumePosition[0])
				 /				fgkCarbonFiberTriangleLength);
  vertexposition[1][0] = new TVector3(vertexposition[0][0]->X(),
								vertexposition[0][0]->X()*TMath::Tan(theta)
				 +				fgkCarbonFiberLowerSupportVolumePosition[0]);
  vertexposition[1][1] = new TVector3(vertexposition[0][1]->X(),
								vertexposition[0][1]->X()*TMath::Tan(theta)
				 +				fgkCarbonFiberLowerSupportVolumePosition[0]);
  vertexposition[1][2] = new TVector3(0.,fgkCarbonFiberLowerSupportVolumePosition[0]);
  vertexposition[1][3] = new TVector3(fgkCarbonFiberTriangleLength,
								fgkCarbonFiberLowerSupportVolumePosition[1]);
  char* carbonfiberlowersupportshapename[kshapesnumber] = 
			  {"CarbonFiberLowerSupportShape1","CarbonFiberLowerSupportShape2"};
  char* carbonfiberlowersupportname[kshapesnumber] = 
			  {"CarbonFiberLowerSupport1","CarbonFiberLowerSupport2"};
  TGeoArb8* carbonfiberlowersupportshape[kshapesnumber];
  TGeoVolume* carbonfiberlowersupport[kshapesnumber];
  TList* carbonfiberlowersupportlist = new TList();
  for(Int_t i = 0; i< kshapesnumber; i++){ 
	carbonfiberlowersupportshape[i] = 
								GetArbShape(vertexposition[i],width,
											fgkCarbonFiberLowerSupportHeight,
											carbonfiberlowersupportshapename[i]);
    carbonfiberlowersupport[i] = new TGeoVolume(carbonfiberlowersupportname[i],
						carbonfiberlowersupportshape[i],fSSDCarbonFiberMedium);
	carbonfiberlowersupport[i]->SetLineColor(fColorCarbonFiber);
    carbonfiberlowersupportlist->Add(carbonfiberlowersupport[i]);
  }
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i< kshapesnumber; i++){
     for(Int_t j=0; j< kvertexnumber; j++)
	   delete vertexposition[i][j];
       delete [] vertexposition[i];
  }
  /////////////////////////////////////////////////////////////
  return carbonfiberlowersupportlist;
}
///////////////////////////////////////////////////////////////////////////////
TGeoVolume* AliITSv11GeometrySSD::GetSSDSensorSupport(Double_t length, Double_t height, 
								 Double_t width, Double_t* thickness)const{
  /////////////////////////////////////////////////////////////
  // Method generating the Sensor Support   
  /////////////////////////////////////////////////////////////  
	const Int_t kvertexnumber = 6;
	TGeoXtru* ssdsensorsupportshape = new TGeoXtru(2);	
    TVector3* vertexposition[kvertexnumber];
	vertexposition[0] = new TVector3();	
	vertexposition[1] = new TVector3(0.0,length);	
	vertexposition[2] = new TVector3(thickness[1],vertexposition[1]->Y());	
	vertexposition[3] = new TVector3(vertexposition[2]->X(),thickness[0]);	
	vertexposition[4] = new TVector3(height,vertexposition[3]->Y());	
	vertexposition[5] = new TVector3(vertexposition[4]->X());	
	Double_t xvertexpoints[6], yvertexpoints[6];
	for(Int_t i=0; i<kvertexnumber; i++) 
		xvertexpoints[i] = vertexposition[i]->X(), 
		yvertexpoints[i] = vertexposition[i]->Y();
    ssdsensorsupportshape->DefinePolygon(kvertexnumber,xvertexpoints,yvertexpoints);
    ssdsensorsupportshape->DefineSection(0,-0.5*width);
    ssdsensorsupportshape->DefineSection(1,0.5*width);
    TGeoVolume* ssdsensorsupport = new TGeoVolume("SSDSensorSupport",
								 ssdsensorsupportshape,fSSDSensorSupportMedium);
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
	for (Int_t i=0; i<kvertexnumber; i++)
		delete vertexposition[i];
  /////////////////////////////////////////////////////////////
    return ssdsensorsupport;
}
////////////////////////////////////////////////////////////////////////////////
TGeoVolume* AliITSv11GeometrySSD::GetCoolingTubeSupport(Int_t nedges){
  /////////////////////////////////////////////////////////////
  // Method generating the Cooling Tube Support
  /////////////////////////////////////////////////////////////
  if(nedges%2!=0) nedges--;	
  const Int_t kvertexnumber = nedges+5;
  Double_t phi = TMath::ASin(0.5*fgkCoolingTubeSupportHeight
			   /			 fgkCoolingTubeSupportRmax)*TMath::RadToDeg();
  Double_t angle = 90.+phi;
  Double_t psi = 90.-phi;
  ///////////////////////////////////////
  // Vertex Positioning for TGeoXTru
  ///////////////////////////////////////
  TVector3** vertexposition = new TVector3*[kvertexnumber];
  vertexposition[0] = new TVector3(fgkCoolingTubeSupportRmin*CosD(angle),
								   fgkCoolingTubeSupportRmin*SinD(angle));
  vertexposition[1] = new TVector3(fgkCoolingTubeSupportRmax*CosD(angle),
								   fgkCoolingTubeSupportRmax*SinD(angle));
  vertexposition[2] = new TVector3(vertexposition[1]->X(),
								   fgkCoolingTubeSupportRmax);
  vertexposition[3] = new TVector3(-vertexposition[1]->X(),
								   fgkCoolingTubeSupportRmax);
  vertexposition[4] = new TVector3(-vertexposition[1]->X(),
								    vertexposition[1]->Y());
  for(Int_t i=0; i<nedges; i++)
	vertexposition[i+5] = 
		new TVector3(fgkCoolingTubeSupportRmin*CosD(psi+i*(2.*phi/nedges)),
					 fgkCoolingTubeSupportRmin*SinD(psi+i*(2.*phi/nedges)));
  ///////////////////////////////////////////////////////////////////////
  // TGeoXTru Volume definition for Cooling Tube Support Arc Part
  ///////////////////////////////////////////////////////////////////////
  TGeoXtru* coolingtubesupportarcshape = new TGeoXtru(2);	
  Double_t* xvertexpoints = new Double_t[kvertexnumber];	
  Double_t* yvertexpoints = new Double_t[kvertexnumber];	
  for(Int_t i=0; i<kvertexnumber; i++){
	xvertexpoints[i] = vertexposition[i]->X();
	yvertexpoints[i] = vertexposition[i]->Y();
  } 
  coolingtubesupportarcshape->DefinePolygon(kvertexnumber,xvertexpoints,
											yvertexpoints);
  coolingtubesupportarcshape->DefineSection(0,-0.5*fgkCoolingTubeSupportWidth);
  coolingtubesupportarcshape->DefineSection(1,0.5*fgkCoolingTubeSupportWidth);
  TGeoVolume* coolingtubesupportarc = new TGeoVolume("CoolingTubeSupportArc",
								          coolingtubesupportarcshape,
										  fSSDTubeHolderMedium);
  coolingtubesupportarc->SetLineColor(fColorG10);
  //////////////////////////////////////////////////////////////////////////
  // TGeoTubeSeg Volume definition for Cooling Tube Support TGeoTubeSeg Part
  //////////////////////////////////////////////////////////////////////////
  TGeoTubeSeg* coolingtubesupportsegshape = 
							new TGeoTubeSeg(fgkCoolingTubeSupportRmin,
											fgkCoolingTubeSupportRmax,
											0.5*fgkCoolingTubeSupportWidth,
											phi,360-phi);
  TGeoVolume* coolingtubesupportseg = new TGeoVolume("CoolingTubeSupportSeg",
											coolingtubesupportsegshape,
											fSSDTubeHolderMedium);
  coolingtubesupportseg->SetLineColor(fColorG10);
  //////////////////////////////////////////////////////////////////////////
  // TGeoBBox Volume definition for Cooling Tube Support Box Part
  //////////////////////////////////////////////////////////////////////////
  Double_t* boxorigin = new Double_t[3];
  Double_t boxlength = fgkCoolingTubeSupportLength-2.*fgkCoolingTubeSupportRmax;
  boxorigin[0] = fgkCoolingTubeSupportRmax+0.5*boxlength, boxorigin[1] = boxorigin[2] = 0.;
  TGeoBBox* coolingtubesupportboxshape = new TGeoBBox(0.5*boxlength,
										 0.5*fgkCoolingTubeSupportHeight,
										 0.5*fgkCoolingTubeSupportWidth,boxorigin);
  TGeoVolume* coolingtubesupportbox = new TGeoVolume("CoolingTubeSupportBox",
                               coolingtubesupportboxshape,fSSDTubeHolderMedium);
  coolingtubesupportbox->SetLineColor(fColorG10);
  //////////////////////////////////////////////////////////////////////////
  // Cooling Tube for Cooling Tube Support 
  //////////////////////////////////////////////////////////////////////////
  TGeoXtru* coolingtubearcshape[2];
  coolingtubearcshape[0] = new TGeoXtru(2);	
  Double_t* xvert = new Double_t[nedges+2];
  Double_t* yvert = new Double_t[nedges+2];
  Double_t ratio = fgkCoolingTubeRmin/fgkCoolingTubeSupportRmin;
  ////////////////////////////////////////
  // Positioning the vertices for TGeoXTru
  ////////////////////////////////////////
  xvert[0] = 0., yvert[0] = 0.;
  xvert[1] = vertexposition[0]->X()*ratio,  yvert[1] = vertexposition[0]->Y()*ratio;
  for(Int_t i=0; i< nedges; i++)
		xvert[i+2] = vertexposition[kvertexnumber-i-1]->X()*ratio,
		yvert[i+2] = vertexposition[kvertexnumber-i-1]->Y()*ratio;
  ////////////////////////////////////////
  // Defining TGeoXTru PolyGone
  ////////////////////////////////////////
  coolingtubearcshape[0]->DefinePolygon(nedges+2,xvert,yvert);
  coolingtubearcshape[0]->DefineSection(0,-0.5*fgkCoolingTubeSupportWidth);
  coolingtubearcshape[0]->DefineSection(1,0.5*fgkCoolingTubeSupportWidth);
  coolingtubearcshape[1] = GetArcShape(2.*phi,fgkCoolingTubeRmin,
		fgkCoolingTubeRmax,nedges,fgkCoolingTubeSupportWidth);
  TGeoVolume* coolingtubearc[2];
  coolingtubearc[0] = new TGeoVolume("CoolingTubeWaterArcPart",
								  coolingtubearcshape[0],fSSDCoolingTubeWater);
  coolingtubearc[1] = new TGeoVolume("CoolingTubePhynoxArcPart",
								  coolingtubearcshape[1],fSSDCoolingTubePhynox);
  coolingtubearc[0]->SetLineColor(fColorWater);
  coolingtubearc[1]->SetLineColor(fColorPhynox);
  ////////////////////////////////////////////
  // Defining TGeoTubeSeg Part of Cooling Tube
  ////////////////////////////////////////////
  TGeoTubeSeg* coolingtubesegshape[2];
  coolingtubesegshape[0] = new TGeoTubeSeg(fgkCoolingTubeRmin,fgkCoolingTubeRmax,
							0.5*fgkCoolingTubeSupportWidth,phi,360-phi);
  coolingtubesegshape[1] = new TGeoTubeSeg(0.,fgkCoolingTubeRmin,
							0.5*fgkCoolingTubeSupportWidth,phi,360-phi);
  TGeoVolume* coolingtubeseg[2];
  coolingtubeseg[0] = new TGeoVolume("CoolingTubePhynoxPart",
								 coolingtubesegshape[0],fSSDCoolingTubePhynox);
  coolingtubeseg[1] = new TGeoVolume("CoolingTubeWaterPart",
								 coolingtubesegshape[1],fSSDCoolingTubeWater);
  coolingtubeseg[0]->SetLineColor(fColorPhynox);
  coolingtubeseg[1]->SetLineColor(fColorWater);
  /////////////////////////////////////////////////////////////
  // Virtual Volume containing Cooling Tube Support  
  /////////////////////////////////////////////////////////////
  TGeoXtru* virtualCoolingTubeSupportShape = new TGeoXtru(2);
  const Int_t kvirtualvertexnumber = 8;
  TVector3* virtualvertex[kvirtualvertexnumber];
   ////////////////////////////////////////
  // Positioning the vertices for TGeoXTru
  ////////////////////////////////////////
  virtualvertex[0] = new TVector3(-fgkCoolingTubeSupportRmax,-fgkCoolingTubeSupportRmax); 
  virtualvertex[1] = new TVector3(virtualvertex[0]->X(),-virtualvertex[0]->Y());
  virtualvertex[2] = new TVector3(-virtualvertex[0]->X(),virtualvertex[1]->Y());
  virtualvertex[3] = new TVector3(virtualvertex[2]->X(),0.5*fgkCoolingTubeSupportHeight);
  virtualvertex[4] = new TVector3(virtualvertex[3]->X()+boxlength,virtualvertex[3]->Y());
  virtualvertex[5] = new TVector3(virtualvertex[4]->X(),-virtualvertex[4]->Y());
  virtualvertex[6] = new TVector3(virtualvertex[3]->X(),-virtualvertex[3]->Y());
  virtualvertex[7] = new TVector3(virtualvertex[2]->X(),-virtualvertex[2]->Y());
  Double_t xmothervertex[kvirtualvertexnumber], ymothervertex[kvirtualvertexnumber];
  for(Int_t i=0; i< kvirtualvertexnumber; i++)
	xmothervertex[i] = virtualvertex[i]->X(),
	ymothervertex[i] = virtualvertex[i]->Y();
  ////////////////////////////////////////
  // Defining TGeoXTru PolyGone
  ////////////////////////////////////////
  virtualCoolingTubeSupportShape->DefinePolygon(kvirtualvertexnumber,xmothervertex,
																	 ymothervertex);
  virtualCoolingTubeSupportShape->DefineSection(0,-0.5*fgkCoolingTubeSupportWidth);
  virtualCoolingTubeSupportShape->DefineSection(1,0.5*fgkCoolingTubeSupportWidth);
  TGeoVolume* virtualcoolingtubesupport = new TGeoVolume("CoolingTubeSupport",
								 virtualCoolingTubeSupportShape,fSSDAir);
  ////////////////////////////////////////
  // Positioning Volumes in Virtual Volume
  ////////////////////////////////////////
  TGeoRotation* coolingtubesupportrot = new TGeoRotation(); 
  coolingtubesupportrot->SetAngles(-90.0,0.0,0.0);
  virtualcoolingtubesupport->AddNode(coolingtubesupportarc,1,coolingtubesupportrot);
  virtualcoolingtubesupport->AddNode(coolingtubesupportbox,1);
  virtualcoolingtubesupport->AddNode(coolingtubesupportseg,1);
  virtualcoolingtubesupport->AddNode(coolingtubearc[0],1,coolingtubesupportrot);
  virtualcoolingtubesupport->AddNode(coolingtubearc[1],1,coolingtubesupportrot);
  virtualcoolingtubesupport->AddNode(coolingtubeseg[0],1);
  virtualcoolingtubesupport->AddNode(coolingtubeseg[1],1);
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  delete [] vertexposition;
  delete xvertexpoints;
  delete yvertexpoints;
  delete xvert;
  delete yvert;
  for(Int_t i=0; i< kvirtualvertexnumber; i++)
	delete virtualvertex[i];
  /////////////////////////////////////////////////////////////
	return virtualcoolingtubesupport;
}
/////////////////////////////////////////////////////////////////////////////////
TList* AliITSv11GeometrySSD::GetSSDHybridParts(){
  /////////////////////////////////////////////////////////////
  // Method generating List containing SSD Hybrid Components   
  /////////////////////////////////////////////////////////////
  TList* ssdhybridlist = new TList();
  const Int_t kssdstiffenernumber = 2;
  Double_t ssdstiffenerseparation = fgkSSDSensorLength
								  - 2.*fgkSSDModuleStiffenerPosition[1]
								  -    fgkSSDStiffenerWidth;
  Double_t ssdchipcablesradius[kssdstiffenernumber];
  for(Int_t i=0; i<kssdstiffenernumber; i++)
	  ssdchipcablesradius[i] = 0.25*(fgkSSDChipCablesHeight[i+2]
			       -  fgkSSDChipCablesHeight[0]
			       -  fgkSSDChipCablesHeight[1]);
  /////////////////////////////////////////////////////////////
  // Mother Volumes Containers 
  /////////////////////////////////////////////////////////////
  const Int_t kmothernumber = 2;
  const Int_t kmothervertexnumber = 12;
  Double_t xmothervertex[kmothernumber][kmothervertexnumber]; 
  Double_t ymothervertex[kmothernumber][kmothervertexnumber]; 
  ///////////////////////
  // Setting the vertices 
  ///////////////////////
  xmothervertex[0][0]  = -0.5*fgkSSDStiffenerLength;
  xmothervertex[0][1]  = xmothervertex[0][0]; 
  xmothervertex[0][2]  = fgkSSDFlexLength[0]-0.5*fgkSSDStiffenerLength;
  xmothervertex[0][3]  = xmothervertex[0][2];
  xmothervertex[0][4]  = xmothervertex[0][0];
  xmothervertex[0][5]  = xmothervertex[0][4];
  xmothervertex[0][6]  = -xmothervertex[0][0];
  xmothervertex[0][7]  = xmothervertex[0][6];
  xmothervertex[0][8]  = -xmothervertex[0][2];
  xmothervertex[0][9]  = xmothervertex[0][8];
  xmothervertex[0][10] = xmothervertex[0][7];
  xmothervertex[0][11] = xmothervertex[0][10];
  for(Int_t i=0; i<kmothervertexnumber; i++) xmothervertex[1][i] = xmothervertex[0][i];
  for(Int_t i = 0; i<kmothernumber; i++){
      ymothervertex[i][0]  = -(fgkSSDChipWidth-(0.5*fgkSSDStiffenerWidth-fgkSSDStiffenerToChipDist)
                           + ssdchipcablesradius[i]+fgkSSDChipCablesWidth[1]+fgkSSDChipCablesWidth[2]);
      ymothervertex[i][1]  = ssdstiffenerseparation-0.5*fgkSSDStiffenerWidth-fgkSSDFlexWidth[0];
      ymothervertex[i][2]  = ymothervertex[i][1];
      ymothervertex[i][3]  = ssdstiffenerseparation-0.5*fgkSSDStiffenerWidth;
      ymothervertex[i][4]  = ymothervertex[i][3];
      ymothervertex[i][5]  = ymothervertex[i][4]+0.5*fgkSSDStiffenerWidth-ymothervertex[i][0];
      ymothervertex[i][6]  = ymothervertex[i][5];
      ymothervertex[i][7]  = 0.5*fgkSSDStiffenerWidth+fgkSSDFlexWidth[0];
      ymothervertex[i][8]  = ymothervertex[i][7];
      ymothervertex[i][9]  = 0.5*fgkSSDStiffenerWidth;
      ymothervertex[i][10] = ymothervertex[i][9];
      ymothervertex[i][11] = ymothervertex[i][0];
  }
  TGeoXtru* ssdhybridmothershape[kmothernumber];
//  TGeoVolume* ssdhybridmother[kmothernumber];
  TGeoVolumeAssembly* ssdhybridmother[kmothernumber];
  const char* ssdhybridmothername[kmothernumber] = {"SSDHybridMother1","SSDHybridMother2"};
  for(Int_t i=0; i<kmothernumber; i++){
      ssdhybridmothershape[i] = new TGeoXtru(2);
      ssdhybridmothershape[i]->DefinePolygon(kmothervertexnumber,xmothervertex[i],
                                          ymothervertex[i]);
      ssdhybridmothershape[i]->DefineSection(0,-0.5*fgkSSDStiffenerHeight-fgkSSDChipHeight      
                                               -fgkSSDChipCablesHeight[i+2]);
      ssdhybridmothershape[i]->DefineSection(1, 0.5*fgkSSDStiffenerHeight);
//      ssdhybridmother[i] = new TGeoVolume(ssdhybridmothername[i],ssdhybridmothershape[i],
//                                          fSSDAir);
      ssdhybridmother[i] = new TGeoVolumeAssembly(ssdhybridmothername[i]);
   }   
  /////////////////////////////////////////////////////////////
  // SSD Stiffener   
  /////////////////////////////////////////////////////////////
  TGeoBBox* ssdstiffenershape = new TGeoBBox("SSDStiffenerShape",
                                             0.5*fgkSSDStiffenerLength,
                                             0.5*(fgkSSDStiffenerWidth-fgkSSDTolerance),
                                             0.5*fgkSSDStiffenerHeight);
  TGeoVolume* ssdstiffener = new TGeoVolume("SSDStiffener",ssdstiffenershape,
                                            fSSDStiffenerMedium);  
  ssdstiffener->SetLineColor(fColorStiffener); 
  TGeoTranslation* ssdstiffenertrans[kssdstiffenernumber];
  for(Int_t i=0; i<kssdstiffenernumber; i++) 
      ssdstiffenertrans[i] = new TGeoTranslation(0.,i*ssdstiffenerseparation,0.);
  /////////////////////////////////////////////////////////////
  // SSD Chip System	
  /////////////////////////////////////////////////////////////
  TList* ssdchipsystemlist = GetSSDChipSystem(); 
  Double_t ssdchipseparation = fgkSSDSensorLength
                             - 2.*fgkSSDModuleStiffenerPosition[1]
                             - 2.*(fgkSSDStiffenerWidth-fgkSSDStiffenerToChipDist
                             - 0.5*fgkSSDChipWidth);
  Double_t ssdchipsystemlength = (fgkSSDChipNumber-1)*(fgkSSDChipLength 
			       +  fgkSSDChipSeparationLength)+fgkSSDChipCablesLength[1];
  TGeoTranslation* ssdchipsystemtrans = new TGeoTranslation(0.5*fgkSSDChipCablesLength[1]
                                      - 0.5*ssdchipsystemlength,
                                        0.5*(ssdstiffenerseparation-ssdchipseparation),
                                      - 0.5*(fgkSSDChipHeight+fgkSSDStiffenerHeight)); 	
////////////////////////////
// Capacitor 0603-2200 nF
///////////////////////////
  const Int_t knapacitor0603number = 5;
  TGeoBBox* capacitor0603shape =  new TGeoBBox("Capacitor0603Shape",
											 0.5*fgkSSDCapacitor0603Length,
											 0.5*(fgkSSDCapacitor0603Width),
											 0.5*fgkSSDCapacitor0603Height);
  TGeoVolume* capacitor0603 = new TGeoVolume("Capacitor0603",capacitor0603shape,
                                             fSSDStiffener0603CapacitorMedium); 
  capacitor0603->SetLineColor(fColorAl);
  for(Int_t i=0; i<kmothernumber; i++){
      for(Int_t j=0; j<kssdstiffenernumber; j++){
            ssdhybridmother[i]->AddNode(ssdstiffener,j+1,ssdstiffenertrans[j]);
            for(Int_t k=1; k<knapacitor0603number+1; k++){
                  ssdhybridmother[i]->AddNode(capacitor0603,knapacitor0603number*j+k,
                        new TGeoTranslation((k-3.)/6*fgkSSDStiffenerLength,
                                           j*ssdstiffenerseparation
                        +                    0.5*((j==0? 1:-1)*fgkSSDStiffenerWidth
                        +                    (j==0? -1:+1)*fgkSSDCapacitor0603Width),
                        -                    0.5*(fgkSSDStiffenerHeight+fgkSSDCapacitor0603Height)));
            }
      } 
      ssdhybridmother[i]->AddNode((TGeoVolume*)ssdchipsystemlist->At(i),i+1,ssdchipsystemtrans);
      ssdhybridlist->Add(ssdhybridmother[i]);
  }    
/////////////////////////////////////////////////////////////
// Mother Volume Containing Capacitor Part 
/////////////////////////////////////////////////////////////
  const Int_t kcapacitormothernumber = 8;
  Double_t xcapacitorvertex[kcapacitormothernumber];
  Double_t ycapacitorvertex[kcapacitormothernumber];  
  ///////////////////////
  // Setting the vertices 
  ///////////////////////
  xcapacitorvertex[0] = -fgkSSDConnectorPosition[0]+ssdstiffenershape->GetDX();    
  xcapacitorvertex[1] = xcapacitorvertex[0];   
  xcapacitorvertex[2] = 0.5*fgkSSDFlexHoleWidth;   
  xcapacitorvertex[3] = xcapacitorvertex[2];   
  xcapacitorvertex[4] = xcapacitorvertex[0];   
  xcapacitorvertex[5] = xcapacitorvertex[0];   
  xcapacitorvertex[6] = -xcapacitorvertex[0];   
  xcapacitorvertex[7] = xcapacitorvertex[6];   
  ycapacitorvertex[0] = -0.5*fgkSSDStiffenerWidth;    
  ycapacitorvertex[1] = ssdstiffenerseparation-0.5*fgkSSDStiffenerWidth-fgkSSDFlexHoleLength;   
  ycapacitorvertex[2] = ycapacitorvertex[1];   
  ycapacitorvertex[3] = ycapacitorvertex[2]+fgkSSDFlexHoleLength;   
  ycapacitorvertex[4] = ycapacitorvertex[3];   
  ycapacitorvertex[5] = ycapacitorvertex[4]+fgkSSDStiffenerWidth;   
  ycapacitorvertex[6] = ycapacitorvertex[5];   
  ycapacitorvertex[7] = ycapacitorvertex[0];   
  TGeoXtru* ssdhybridcapacitormothershape = new TGeoXtru(2);
  ssdhybridcapacitormothershape->DefinePolygon(kcapacitormothernumber,xcapacitorvertex,
                                              ycapacitorvertex);
  ssdhybridcapacitormothershape->DefineSection(0,0.5*fgkSSDStiffenerHeight);
  ssdhybridcapacitormothershape->DefineSection(1, 0.5*fgkSSDStiffenerHeight+fgkSSDCapacitor1812Height);
//  TGeoVolume* ssdhybridcapacitormother = new TGeoVolume("SSDHybridCapacitorMother",ssdhybridcapacitormothershape,
//                                          fSSDAir);
  TGeoVolumeAssembly* ssdhybridcapacitormother = new TGeoVolumeAssembly("SSDHybridCapacitorMother");
////////////////////////////
// Connector 
///////////////////////////
  const Int_t kssdconnectornumber = 2;
  TGeoBBox* ssdconnectorshape[kssdconnectornumber];
  Double_t ssdAlconnectororigin[3] = {0.0,0.0,0.5*(fgkSSDStiffenerHeight+fgkSSDConnectorAlHeight)};    
  Double_t ssdNiconnectororigin[3] = {0.0,0.0,0.5*(fgkSSDStiffenerHeight+fgkSSDConnectorNiHeight)
                                   +  fgkSSDConnectorAlHeight};  
  const char* ssdconnectorname[kssdconnectornumber] = {"SSDConnectorAl","SSDConnectorNi"};
  TGeoVolume* ssdconnector[kssdconnectornumber];
  for(Int_t i=0; i<kssdconnectornumber; i++){
      ssdconnectorshape[i] = new TGeoBBox(0.5*fgkSSDConnectorLength,
                                          0.5*fgkSSDConnectorWidth,
                                          0.5*((1-i)*fgkSSDConnectorAlHeight
                           +              i*fgkSSDConnectorNiHeight),
                             i==0? ssdAlconnectororigin : ssdNiconnectororigin);
      ssdconnector[i] = new TGeoVolume(ssdconnectorname[i],ssdconnectorshape[i],
                                       i==0 ? fSSDAlTraceFlexMedium 
                                            : fSSDStiffenerConnectorMedium);      
      ssdconnector[i]->SetLineColor(i==0 ? fColorAl : fColorPhynox);
  }
  TGeoTranslation* ssdconnectortrans[2*kssdconnectornumber];
  ssdconnectortrans[0] = new TGeoTranslation(-ssdstiffenershape->GetDX()
                       +  fgkSSDConnectorPosition[0]
                       -  fgkSSDConnectorSeparation
                       -  1.5*fgkSSDConnectorLength,
                          ssdstiffenerseparation+ssdstiffenershape->GetDY()
                       -  fgkSSDConnectorPosition[1]
                       -  ssdconnectorshape[0]->GetDY(),0.0);	
  ssdconnectortrans[1] = new TGeoTranslation(
                       -  ssdstiffenershape->GetDX()
                       +  fgkSSDConnectorPosition[0]
                       -  0.5*fgkSSDConnectorLength,
                          ssdstiffenerseparation+ssdstiffenershape->GetDY()
                       -  fgkSSDConnectorPosition[1]
                       -  ssdconnectorshape[0]->GetDY(),0.0);
  ssdconnectortrans[2] = new TGeoTranslation(+ssdstiffenershape->GetDX()
                       -  fgkSSDConnectorPosition[0]
                       +  fgkSSDConnectorSeparation
                       +  1.5*fgkSSDConnectorLength,
                          -(ssdstiffenershape->GetDY()
                       -  fgkSSDConnectorPosition[1]
                       -  ssdconnectorshape[0]->GetDY()),0.0);	
  ssdconnectortrans[3] = new TGeoTranslation(+ssdstiffenershape->GetDX()
                       -  fgkSSDConnectorPosition[0]
                       +  0.5*fgkSSDConnectorLength,
                          -(ssdstiffenershape->GetDY()
                       -  fgkSSDConnectorPosition[1]
                       -  ssdconnectorshape[0]->GetDY()),0.0);
  for(Int_t i=0; i<2*kssdconnectornumber; i++)
      for(Int_t j=0; j<kssdconnectornumber; j++)
            ssdhybridcapacitormother->AddNode(ssdconnector[j],i+1,ssdconnectortrans[i]);      
////////////////////////////
// Capacitor 1812-330 nF
/////////////////////////// 
  Double_t ssdcapacitor1812origin[3] = {0.0,0.0,0.5*(fgkSSDStiffenerHeight+fgkSSDCapacitor1812Height)};    
  TGeoBBox* capacitor1812shape =  new TGeoBBox("Capacitor1812Shape",
											 0.5*fgkSSDCapacitor1812Length,
											 0.5*fgkSSDCapacitor1812Width,
											 0.5*fgkSSDCapacitor1812Height,
            ssdcapacitor1812origin);
  TGeoVolume* capacitor1812 = new TGeoVolume("Capacitor1812",capacitor1812shape,
                                             fSSDStiffener1812CapacitorMedium); 
  capacitor1812->SetLineColor(fColorAl);
  TGeoTranslation* capacitor1812trans = new TGeoTranslation(0.0,
                                        0.5*fgkSSDStiffenerWidth+ssdstiffenerseparation
                                      - capacitor1812shape->GetDY()-fgkSSDConnectorPosition[1],0.0);
  ssdhybridcapacitormother->AddNode(capacitor1812,1,capacitor1812trans);
////////////////////////////
//Hybrid Wire
////////////////////////////
  Double_t wirex = 2.*(fgkSSDConnectorPosition[0]-0.5*fgkSSDStiffenerLength
				 - 0.5*fgkSSDConnectorLength)-fgkSSDConnectorLength
				 - fgkSSDConnectorSeparation;
  Double_t wirey = ssdstiffenerseparation+fgkSSDStiffenerWidth
				 - 2.*fgkSSDConnectorPosition[1]-fgkSSDConnectorWidth;
  Double_t ssdwireradius = TMath::Sqrt(TMath::Power(wirex,2.)
				         + TMath::Power(wirey,2));
  Double_t wireangle = TMath::ATan(wirex/wirey);
  TGeoTube *hybridwireshape = new TGeoTube("HybridWireShape", 0., 
						fgkSSDWireRadius, 0.5*ssdwireradius);
  TGeoVolume* hybridwire = new TGeoVolume("HybridWire",hybridwireshape,
                                             fSSDStiffenerHybridWireMedium); 
  hybridwire->SetLineColor(fColorPhynox);
  TGeoCombiTrans* hybridwirecombitrans[2];
  hybridwirecombitrans[0] = new TGeoCombiTrans("HybridWireCombiTrans1",
                   0.5*fgkSSDStiffenerLength-fgkSSDConnectorPosition[0]
				 + 1.5*fgkSSDConnectorLength+fgkSSDConnectorSeparation,
                   0.5*ssdwireradius-0.5*fgkSSDStiffenerWidth
				 + fgkSSDConnectorPosition[1]+0.5*fgkSSDConnectorWidth,
				   ssdstiffenershape->GetDZ()
				 + fgkSSDWireRadius+fgkSSDConnectorAlHeight+fgkSSDConnectorNiHeight,
                   new TGeoRotation("HybridWireRot1",0.,90.,0.));
  hybridwirecombitrans[1] = new TGeoCombiTrans("HybridWireCombiTrans2",
                            0.0,
                          - 0.5*fgkSSDConnectorWidth+fgkSSDWireRadius,
                            0.0,	
                            new TGeoRotation("HybridWireRot2",
                          - wireangle*TMath::RadToDeg(),0.,0.));
  TGeoHMatrix* hybridwirematrix = new TGeoHMatrix();
  hybridwirematrix->MultiplyLeft(hybridwirecombitrans[0]);
  hybridwirematrix->MultiplyLeft(hybridwirecombitrans[1]);
  ssdhybridcapacitormother->AddNode(hybridwire,1,hybridwirematrix);
  ssdhybridlist->Add(ssdhybridcapacitormother);
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  delete hybridwirecombitrans[0];
  delete hybridwirecombitrans[1];
  delete ssdchipsystemlist;
  return ssdhybridlist;
  /////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////
TGeoVolume* AliITSv11GeometrySSD::GetCoolingBlockSystem(){
  /////////////////////////////////////////////////////////////
  // SSD Cooling Block System
  /////////////////////////////////////////////////////////////
  // SSD Cooling Block and Cooling Tube Transformations
  /////////////////////////////////////////////////////////////
  TGeoRotation* localcoolingblockrot = new TGeoRotation();
  localcoolingblockrot->SetAngles(0.,90.,0.);
  TGeoCombiTrans localcoolingblockmatrix(0.,0.5*fgkSSDCoolingBlockWidth,0.,localcoolingblockrot);
  TVector3* coolingblocktransvector;
  coolingblocktransvector = new TVector3(fgkSSDModuleSensorSupportDistance
								+ fgkSSDCoolingBlockLength,
								  fgkSSDSensorLength
								- 2.*fgkSSDModuleStiffenerPosition[1]
								- fgkSSDCoolingBlockWidth);
  const Int_t kcoolingblocktransnumber = 2;
  const Int_t kcoolingblocknumber = 4;
  TGeoHMatrix* coolingblockmatrix[kcoolingblocknumber];
  TGeoHMatrix* coolingtubematrix[kcoolingblocknumber];
  TGeoRotation* localcoolingtuberot = new TGeoRotation();
  localcoolingtuberot->SetAngles(0.0,90.0,0.0);
  TGeoTranslation* localcoolingtubetrans = new TGeoTranslation();
  localcoolingtubetrans->SetTranslation(0.5*fgkSSDCoolingBlockLength,
					0.5*fgkSSDCoolingBlockWidth,
					fgkSSDCoolingBlockHoleCenter);
  TGeoCombiTrans localcoolingtubematrix (*localcoolingtubetrans,*localcoolingtuberot);
  Double_t coolingtubedistance = fgkCoolingTubeSupportRmax-fgkCoolingTubeSupportRmin;
  for(Int_t i=0; i<kcoolingblocktransnumber; i++){
    for(Int_t j=0; j<kcoolingblocktransnumber; j++){
      TGeoTranslation localcoolingblocktrans(i*coolingblocktransvector->X()+2*coolingtubedistance,
					     j*coolingblocktransvector->Y(),
					     - 0.5*(fgkSSDCoolingBlockHoleCenter
						    + fgkCoolingTubeRmax));
      coolingblockmatrix[2*i+j] = new TGeoHMatrix(localcoolingblocktrans*localcoolingblockmatrix);
      coolingtubematrix[2*i+j] = new TGeoHMatrix(localcoolingblocktrans*localcoolingtubematrix);
    }
  }
  /////////////////////////////////////////////////////////////
  // Virtual Volume containing CoolingBlock System   
  /////////////////////////////////////////////////////////////
  TGeoXtru* coolingsystemothershape = new TGeoXtru(2);
  const Int_t kmothervertexnumber = 16;  
  Double_t xmothervertex[kmothervertexnumber];
  Double_t ymothervertex[kmothervertexnumber];
  ///////////////////////
  // Setting the vertices 
  ///////////////////////fgkCoolingTubeSupportRmax
  xmothervertex[0] = 0.0,ymothervertex[0] = 0.0;
  xmothervertex[1] = xmothervertex[0], ymothervertex[1] = coolingblocktransvector->Y()
				   + fgkSSDCoolingBlockWidth;
  xmothervertex[2] = coolingblocktransvector->X()
				   + fgkSSDCoolingBlockLength
				   + 4*coolingtubedistance;
  ymothervertex[2] = ymothervertex[1];
  xmothervertex[3] = xmothervertex[2], ymothervertex[3] = ymothervertex[0];
  xmothervertex[4] = xmothervertex[3]-2.*coolingtubedistance-fgkSSDCoolingBlockLength;
  ymothervertex[4] = ymothervertex[0];
  xmothervertex[5] = xmothervertex[4], ymothervertex[5] = fgkSSDCoolingBlockWidth;
  xmothervertex[6] = xmothervertex[3]-coolingtubedistance; 
  ymothervertex[6] = ymothervertex[5]; 
  xmothervertex[7] = xmothervertex[6], ymothervertex[7] = ymothervertex[2]
				   - fgkSSDCoolingBlockWidth; 
  xmothervertex[8] = xmothervertex[5], ymothervertex[8] = ymothervertex[7];
  xmothervertex[9] = xmothervertex[8], ymothervertex[9] = ymothervertex[2]
				   - coolingtubedistance;
  xmothervertex[10] = fgkSSDCoolingBlockLength+2.*coolingtubedistance;
  ymothervertex[10] = ymothervertex[9];
  xmothervertex[11] = xmothervertex[10], ymothervertex[11] = ymothervertex[8];
  xmothervertex[12] = coolingtubedistance, ymothervertex[12] = ymothervertex[11];
  xmothervertex[13] = xmothervertex[12], ymothervertex[13] = fgkSSDCoolingBlockWidth;
  xmothervertex[14] = 2.*coolingtubedistance+fgkSSDCoolingBlockLength;
  ymothervertex[14] = ymothervertex[13];
  xmothervertex[15] = xmothervertex[14], ymothervertex[15] = ymothervertex[0];
  //////////////////////////////////////////////////////////
  coolingsystemothershape->DefinePolygon(kmothervertexnumber,
									xmothervertex,ymothervertex);
  coolingsystemothershape->DefineSection(0,-0.5*(fgkSSDCoolingBlockHoleCenter
											   + fgkCoolingTubeRmax));
  coolingsystemothershape->DefineSection(1, 0.5*(fgkSSDCoolingBlockHoleCenter
											   + fgkCoolingTubeRmax));
  TGeoVolume* coolingsystemother = new TGeoVolumeAssembly("CoolingBlockSystem");
//  TGeoVolume* coolingsystemother = new TGeoVolume("CoolingBlockSystem",
//							  coolingsystemothershape,fSSDAir);
  /////////////////////////////////////////////////////////////
  // SSD Cooling Tube Part 
  /////////////////////////////////////////////////////////////
  TGeoTube* coolingtubeshape[fgkcoolingtubenumber];
  coolingtubeshape[0] = new TGeoTube(fgkCoolingTubeRmin,fgkCoolingTubeRmax,
										 0.5*(fgkSSDCoolingBlockWidth-fgkSSDTolerance)); 
  coolingtubeshape[1] = new TGeoTube(0.0,fgkCoolingTubeRmin,
									 0.5*(fgkSSDCoolingBlockWidth-fgkSSDTolerance));
  TGeoVolume* coolingtube[fgkcoolingtubenumber];
  coolingtube[0] = new TGeoVolume("OuterCoolingTube",coolingtubeshape[0],
									fSSDCoolingTubePhynox);
  coolingtube[1] = new TGeoVolume("InnerCoolingTube",coolingtubeshape[1],
									fSSDCoolingTubeWater);
  coolingtube[0]->SetLineColor(fColorPhynox);
  coolingtube[1]->SetLineColor(fColorWater);
  TGeoVolume* ssdcoolingblock = GetSSDCoolingBlock(30);
  /////////////////////////////////////////////////////////////
  // Adding Cooling block to mother volume
  /////////////////////////////////////////////////////////////
   for(Int_t i=0; i<kcoolingblocknumber; i++){ 
	coolingsystemother->AddNode(ssdcoolingblock,i+1,coolingblockmatrix[i]);
	coolingsystemother->AddNode(coolingtube[0],i+1,coolingtubematrix[i]);
	coolingsystemother->AddNode(coolingtube[1],i+1,coolingtubematrix[i]);
  }
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
	delete coolingblocktransvector;
	delete localcoolingblockrot;
	delete localcoolingtubetrans;
	delete localcoolingtuberot;
  /////////////////////////////////////////////////////////////
  // Checking overlaps	
  /////////////////////////////////////////////////////////////
	//coolingsystemother->CheckOverlaps(0.01);
  /////////////////////////////////////////////////////////////
	return coolingsystemother;
}
/////////////////////////////////////////////////////////////////////////////////
TGeoVolume* AliITSv11GeometrySSD::GetSSDStiffenerFlex()const{
  /////////////////////////////////////////////////////////////
  // SSD Flex
  /////////////////////////////////////////////////////////////
  const Int_t kssdflexlayernumber = 2;
  TGeoXtru* ssdflexshape[kssdflexlayernumber];
  for(Int_t i=0; i<kssdflexlayernumber; i++) ssdflexshape[i] = new TGeoXtru(2);
  const Int_t kmothervertexnumber = 17; 
  Double_t xmothervertex[kmothervertexnumber];
  Double_t ymothervertex[kmothervertexnumber];
  /////////////////////////////////////////////
  // Auxiliary variables for vertex positioning
  /////////////////////////////////////////////
  const Int_t kssdflexboxnumber = 5;
  Double_t ssdflexboxlength[kssdflexboxnumber];
  ssdflexboxlength[0] = 0.5*(fgkSSDChipNumber
					  *	fgkSSDChipLength+(fgkSSDChipNumber-1)
					  *	fgkSSDChipSeparationLength
					  - fgkSSDModuleSensorSupportDistance-fgkSSDFlexHoleLength)
					  - (fgkSSDFlexLength[0]-fgkSSDFlexLength[1]);
  ssdflexboxlength[1] = fgkSSDModuleSensorSupportDistance+fgkSSDFlexHoleLength;
  ssdflexboxlength[2] = 0.5*(fgkSSDModuleSensorSupportDistance
					  -	fgkSSDFlexHoleLength-fgkSSDFlexHoleWidth); 	
  ssdflexboxlength[3] = fgkSSDFlexHoleWidth;	
  ssdflexboxlength[4] = fgkSSDFlexLength[1]-ssdflexboxlength[0]
					  -	ssdflexboxlength[1];
  Double_t ssdflexboxwidth[kssdflexboxnumber];
  ssdflexboxwidth[0] = fgkSSDFlexWidth[0];
  ssdflexboxwidth[1] = fgkSSDFlexWidth[0]-fgkSSDFlexHoleWidth;
  ssdflexboxwidth[2] = fgkSSDFlexHoleWidth;
  ssdflexboxwidth[3] = ssdflexboxwidth[2]-fgkSSDFlexHoleLength;
  ssdflexboxwidth[4] = fgkSSDFlexWidth[0];
  ///////////////////////
  // Setting the vertices 
  ///////////////////////
  xmothervertex[0]  = 0.0;
  xmothervertex[1]  = xmothervertex[0];
  xmothervertex[2]  = fgkSSDFlexLength[0]-fgkSSDFlexLength[1];
  xmothervertex[3]  = xmothervertex[2]+ssdflexboxlength[0]+ssdflexboxlength[1]
					+ ssdflexboxlength[4];
  xmothervertex[4]  = xmothervertex[3];
  xmothervertex[5]  = xmothervertex[4]-ssdflexboxlength[4];
  xmothervertex[6]  = xmothervertex[5];
  xmothervertex[7]  = xmothervertex[6]-fgkSSDFlexHoleLength;
  xmothervertex[8]  = xmothervertex[7];
  xmothervertex[9]  = xmothervertex[8]-ssdflexboxlength[2];
  xmothervertex[10] = xmothervertex[9]; 
  xmothervertex[11] = xmothervertex[10]-ssdflexboxlength[3];
  xmothervertex[12] = xmothervertex[11];
  xmothervertex[13] = xmothervertex[12]-ssdflexboxlength[2];
  xmothervertex[14] = xmothervertex[13];
  xmothervertex[15] = xmothervertex[14]-fgkSSDFlexHoleLength;
  xmothervertex[16] = xmothervertex[15];
  ymothervertex[0]  = 0.0;
  ymothervertex[1]  = fgkSSDFlexWidth[1];
  ymothervertex[2]  = fgkSSDFlexWidth[0];
  ymothervertex[3]  = ymothervertex[2];
  ymothervertex[4]  = ymothervertex[0];
  ymothervertex[5]  = ymothervertex[4];
  ymothervertex[6]  = ssdflexboxwidth[2];
  ymothervertex[7]  = ymothervertex[6];
  ymothervertex[8]  = ymothervertex[0];
  ymothervertex[9]  = ymothervertex[8];
  ymothervertex[10] = ssdflexboxwidth[2]-ssdflexboxwidth[3];
  ymothervertex[11] = ymothervertex[10];
  ymothervertex[12] = ymothervertex[0];
  ymothervertex[13] = ymothervertex[12];
  ymothervertex[14] = ymothervertex[7];
  ymothervertex[15] = ymothervertex[14];
  ymothervertex[16] = ymothervertex[0];
  /////////////////////////////////////////////////////////////
  // First Mother Volume containing SSDFlex
  /////////////////////////////////////////////////////////////
  TGeoXtru* ssdflexmothershape = new TGeoXtru(2);
  ssdflexmothershape->DefinePolygon(kmothervertexnumber,xmothervertex,
								    ymothervertex);
  ssdflexmothershape->DefineSection(0,-1.5*fgkSSDFlexHeight[0]-2.*fgkSSDFlexHeight[1]);
  ssdflexmothershape->DefineSection(1, 0.5*fgkSSDFlexHeight[0]);
  TGeoVolumeAssembly* ssdflexmother = new TGeoVolumeAssembly("SSDFlexMother");
//  TGeoVolume* ssdflexmother = new TGeoVolume("SSDFlexMother",ssdflexmothershape,
//											 fSSDAir);
  /////////////////////////////////////////////////////////////
  // SSDFlex Layer Shapes
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<kssdflexlayernumber; i++){
	ssdflexshape[i]->DefinePolygon(kmothervertexnumber,xmothervertex,
								   ymothervertex);
    ssdflexshape[i]->DefineSection(0,-0.5*fgkSSDFlexHeight[i]);
	ssdflexshape[i]->DefineSection(1, 0.5*fgkSSDFlexHeight[i]);
  }
  /////////////////////////////////////
  // Setting Layers into Mother Volume
  /////////////////////////////////////
  Int_t ssdflexcolor[kssdflexlayernumber] = {fColorAl,fColorPolyhamide};
  TGeoMedium* ssdflexmed[kssdflexlayernumber] = {fSSDAlTraceFlexMedium,
												 fSSDKaptonFlexMedium};
  const char* ssdflexname[2*kssdflexlayernumber] = {"AlFlexLay1","KaptonFlexLay1",
													"AlFlexLay2","KaptonFlexLay2"};
  TGeoVolume* ssdflex[2*kssdflexlayernumber];
  TGeoTranslation* ssdflextrans[2*kssdflexlayernumber];
  for(Int_t i=0; i<2*kssdflexlayernumber; i++){
	ssdflex[i] = new TGeoVolume(ssdflexname[i],
								i%2==0 ? ssdflexshape[0] : ssdflexshape[1],
								i%2==0 ? ssdflexmed[0]   : ssdflexmed[1]);
	ssdflex[i]->SetLineColor(i%2==0 ? ssdflexcolor[0] : ssdflexcolor[1]);
    ssdflextrans[i]  = new TGeoTranslation(0.,0.,-0.5*i*(fgkSSDFlexHeight[0]
					 +					   fgkSSDFlexHeight[1])); 
    ssdflexmother->AddNode(ssdflex[i],1,ssdflextrans[i]);
  }
  return ssdflexmother;
}
/////////////////////////////////////////////////////////////////////////////////
TGeoVolume* AliITSv11GeometrySSD::GetSSDEndFlex(){
  /////////////////////////////////////////////////////////////
  // Method generating SSD End Flex   
  /////////////////////////////////////////
  Double_t ssdflexradiusmax = (fgkSSDFlexLength[3]-fgkSSDFlexLength[2])
							/  TMath::Tan(fgkSSDFlexAngle*TMath::DegToRad());
  Double_t ssdflexboxlength = fgkSSDFlexFullLength-2.*fgkSSDFlexAngle
					        * TMath::DegToRad()*ssdflexradiusmax
					        - fgkSSDFlexLength[2]-TMath::Pi()
					        * fgkSSDStiffenerHeight-fgkSSDFlexLength[0];
  const Int_t knedges = 20;  
  const Int_t karcnumber = 2;
  TVector3* vertexposition[karcnumber*(knedges+1)];
  Double_t deltangle[karcnumber] = {2.*fgkSSDFlexAngle/knedges,180.0/knedges}; 
  Double_t angle[karcnumber] = {90.-2.*fgkSSDFlexAngle,180.0};	
  Double_t radius[karcnumber] = {ssdflexradiusmax-fgkSSDStiffenerHeight,fgkSSDStiffenerHeight};
  Double_t referenceangle[karcnumber] = {-180.0*TMath::DegToRad(),
										 - 90.0*TMath::DegToRad()};
  TVector3* referencetrans[karcnumber];
  referencetrans[0] = new TVector3(ssdflexboxlength*CosD(2.*fgkSSDFlexAngle)
					+			   radius[0]*SinD(2.*fgkSSDFlexAngle),
								   radius[0]);
  referencetrans[1] = new TVector3(referencetrans[0]->X()
					+              fgkSSDFlexLength[2],
     -              fgkSSDStiffenerHeight);
for(Int_t i=0; i<karcnumber; i++){
	for(Int_t j=0; j<knedges+1; j++){
		vertexposition[j+i*(knedges+1)] = new TVector3(radius[i]*CosD(angle[i]),
										               radius[i]*SinD(angle[i]));
		angle[i] +=  deltangle[i]*(1.0-2.0*i);
	} 	
  }
  ///////////////////////
  // Setting the vertices 
  ///////////////////////
  const Int_t kendflexlayernumber = 4;
  const Int_t kendflexvertexnumber = 4*(knedges+1)+2;
  TVector3** vertex[kendflexlayernumber];
  for(Int_t i=0; i<kendflexlayernumber; i++) 
					vertex[i] = new TVector3*[kendflexvertexnumber];
  TVector3* transvector[kendflexlayernumber+1];
  TVector3* deltatransvector = new TVector3();	
  for(Int_t i=0; i<kendflexlayernumber+1; i++) transvector[i] = new TVector3();	
  transvector[0]->SetXYZ(0.0,ssdflexboxlength*SinD(2.*fgkSSDFlexAngle)
				+		 radius[0]*(1.0-CosD(2.*fgkSSDFlexAngle)),0.0);
  for(Int_t i=1; i<kendflexlayernumber+1; i++){ 	
	deltatransvector->SetXYZ((i%2!=0?fgkSSDFlexHeight[0]:fgkSSDFlexHeight[1])
					*		  CosD(fgkSSDFlexAngle),
							  (i%2!=0?fgkSSDFlexHeight[0]:fgkSSDFlexHeight[1])
					*         SinD(fgkSSDFlexAngle),0.0);   
	*transvector[i] = *transvector[i-1]+*deltatransvector;
  }
  Double_t ratioradius[karcnumber][kendflexlayernumber+1];
  ratioradius[0][0] = 1., ratioradius[1][0] = 1.;
  for(Int_t i=0; i<karcnumber; i++){
	for(Int_t j=1; j<kendflexlayernumber+1; j++){
		ratioradius[i][j] = ratioradius[i][j-1]-TMath::Power(-1.0,i)
						  * (j%2!=0?fgkSSDFlexHeight[0]:fgkSSDFlexHeight[1])
					      /radius[i];
	}
  }
  for(Int_t i=0; i<kendflexlayernumber; i++){
	vertex[i][0] = new TVector3(transvector[i]->X(),transvector[i]->Y());
	vertex[i][1] = new TVector3(transvector[i+1]->X(),transvector[i+1]->Y());
	for(Int_t j=0; j<karcnumber*(knedges+1); j++){
		if(j<(knedges+1)){
			vertex[i][j+2] = new TVector3(vertexposition[j]->X()*ratioradius[0][i+1],
										  vertexposition[j]->Y()*ratioradius[0][i+1]);
			vertex[i][j+2]->RotateZ(referenceangle[0]);
			*vertex[i][j+2] += *referencetrans[0];
			vertex[i][4*(knedges+1)-j+1] = 
							 new TVector3(vertexposition[j]->X()*ratioradius[0][i],
										  vertexposition[j]->Y()*ratioradius[0][i]);
			vertex[i][4*(knedges+1)-j+1]->RotateZ(referenceangle[0]);
			*vertex[i][4*(knedges+1)-j+1] += *referencetrans[0];
		}
		else{
		
			vertex[i][j+2] = new TVector3(vertexposition[j]->X()*ratioradius[1][i+1],
										  vertexposition[j]->Y()*ratioradius[1][i+1]);
			vertex[i][j+2]->RotateZ(referenceangle[1]);
			*vertex[i][j+2] += *referencetrans[1];
			vertex[i][4*(knedges+1)-j+1] = 
							 new TVector3(vertexposition[j]->X()*ratioradius[1][i],
										  vertexposition[j]->Y()*ratioradius[1][i]);
			vertex[i][4*(knedges+1)-j+1]->RotateZ(referenceangle[1]);
			*vertex[i][4*(knedges+1)-j+1] += *referencetrans[1];
	   }
	}
  }
  /////////////////////////////////////////////////////////////
  // First Mother Volume containing SSDEndFlex
  /////////////////////////////////////////////////////////////
  TGeoXtru* ssdendflexmothershape = new TGeoXtru(2);
  Double_t xmothervertex[kendflexvertexnumber];
  Double_t ymothervertex[kendflexvertexnumber];
  xmothervertex[0] = vertex[0][0]->X();	
  ymothervertex[0] = vertex[0][0]->Y();
  for(Int_t i=1; i<kendflexvertexnumber; i++){
	if(i<2*(knedges+1)+2){
		xmothervertex[i] = vertex[3][i]->X();
		ymothervertex[i] = vertex[3][i]->Y();
	}
	else{
		xmothervertex[i] = vertex[0][i]->X();
		ymothervertex[i] = vertex[0][i]->Y();
	}
  }
  ssdendflexmothershape->DefinePolygon(kendflexvertexnumber,
									   xmothervertex,ymothervertex);
  ssdendflexmothershape->DefineSection(0,-0.5*fgkSSDFlexWidth[0]);
  ssdendflexmothershape->DefineSection(1, 0.5*fgkSSDFlexWidth[0]);
  TGeoVolume* ssdendflexmother = new TGeoVolume("SSDEndFlexMother",
								 ssdendflexmothershape,fSSDAir);	
  //////////////////////////////////////
  // End Flex TGeoXtru Layer Definition 
  //////////////////////////////////////
  TGeoXtru* ssdendflexshape[kendflexlayernumber];
  TGeoVolume* ssdendflex[kendflexlayernumber];
  for(Int_t i=0; i<kendflexlayernumber; i++) ssdendflexshape[i] = new TGeoXtru(2);
  Double_t xvertex[kendflexlayernumber][kendflexvertexnumber];
  Double_t yvertex[kendflexlayernumber][kendflexvertexnumber];
  Int_t ssdendflexcolor[kendflexlayernumber] = {fColorAl,fColorPolyhamide};
  TGeoMedium* ssdendflexmed[kendflexlayernumber] = {fSSDAlTraceFlexMedium,
													fSSDKaptonFlexMedium};
  const char* ssdendflexname[kendflexlayernumber] = {"AlEndFlexLay1","KaptonEndFlexLay1",
													 "AlEndFlexLay2","KaptonEndFlexLay2"};
  for(Int_t i=0; i<kendflexlayernumber; i++){
	for(Int_t j=0; j<4*(knedges+1)+2; j++){
		xvertex[i][j] = vertex[i][j]->X();
		yvertex[i][j] = vertex[i][j]->Y();
	}
  ssdendflexshape[i]->DefinePolygon(kendflexvertexnumber,xvertex[i],yvertex[i]);
  ssdendflexshape[i]->DefineSection(0,-0.5*fgkSSDFlexWidth[0]);
  ssdendflexshape[i]->DefineSection(1, 0.5*fgkSSDFlexWidth[0]);
  ssdendflex[i] = new TGeoVolume(ssdendflexname[i],ssdendflexshape[i],
								 i%2==0 ? ssdendflexmed[0] : ssdendflexmed[1]);
  ssdendflex[i]->SetLineColor(i%2==0 ? ssdendflexcolor[0] : ssdendflexcolor[1]);
  ssdendflexmother->AddNode(ssdendflex[i],1);
  }
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<karcnumber*(knedges+1); i++) delete vertexposition[i];
  for(Int_t i=0; i<karcnumber; i++) delete referencetrans[i];
  for(Int_t i=0; i<kendflexlayernumber; i++){
	for(Int_t j=0; j<kendflexvertexnumber; j++) delete vertex[i][j];
	delete [] vertex[i];
  }
  for(Int_t i=0; i<kendflexlayernumber+1; i++) delete transvector[i];	
  delete deltatransvector;
  /////////////////////////////////////////////////////////////
  //ssdendflexmother->CheckOverlaps(0.01);
  return ssdendflexmother;
}
///////////////////////////////////////////////////////////////////////////////
TGeoVolume* AliITSv11GeometrySSD::GetSSDMountingBlock(){
  /////////////////////////////////////////////////////////////
  // Method generating the Mounting Block
  /////////////////////////////////////////////////////////////  
  const Int_t kvertexnumber = 8;
  Double_t xvertex[kvertexnumber];
  Double_t yvertex[kvertexnumber];
  xvertex[0] = -0.25*(fgkSSDMountingBlockLength[0]-fgkSSDMountingBlockLength[1]); 
  xvertex[1] = xvertex[0];
  xvertex[2] = -xvertex[0];
  xvertex[3] = xvertex[2];
  xvertex[4] = xvertex[3]+0.5*(fgkSSDMountingBlockLength[1]
			 -				   fgkSSDMountingBlockLength[2]);
  xvertex[5] = xvertex[4];
  xvertex[6] = 0.5*fgkSSDMountingBlockLength[0]-xvertex[2]
			 - 0.5*fgkSSDMountingBlockScrewHoleEdge
			 -     fgkSSDMountingBlockScrewHoleRadius[0];
  xvertex[7] = xvertex[6];
  yvertex[0] = -0.5*fgkCoolingTubeSupportHeight-fgkSSDModuleCoolingBlockToSensor
			 +	fgkSSDMountingBlockHeight[1]-0.5*fgkSSDMountingBlockHeight[0]
			 -  fgkSSDModuleVerticalDisalignment;
  yvertex[1] = 0.5*fgkSSDMountingBlockHeight[0];
  yvertex[2] = yvertex[1]; 
  yvertex[3] = fgkSSDMountingBlockHeight[1]-yvertex[1];
  yvertex[4] = yvertex[3];
  yvertex[5] = yvertex[2]+fgkSSDMountingBlockHeight[2]
			 - fgkSSDMountingBlockHeight[0];
  yvertex[6] = yvertex[5];
  yvertex[7] = yvertex[0];
  ///////////////////////////////////////////////////////////////////////
  // TGeoXTru Volume definition for Mounting Block Part
  ///////////////////////////////////////////////////////////////////////
  TGeoXtru* ssdmountingblockshape = new TGeoXtru(2);
  ssdmountingblockshape->DefinePolygon(kvertexnumber,xvertex,yvertex);
  ssdmountingblockshape->DefineSection(0,-0.5*fgkSSDMountingBlockWidth);
  ssdmountingblockshape->DefineSection(1,0.5*fgkSSDMountingBlockWidth);
  TGeoVolume* ssdmountingblock = new TGeoVolume("SSDMountingBlock",
								          ssdmountingblockshape,
										  fSSDMountingBlockMedium);
  ssdmountingblock->SetLineColor(fColorG10);
  TGeoCombiTrans* mountingblockcombitrans = new TGeoCombiTrans();
  mountingblockcombitrans->SetTranslation(2.*xvertex[2]+fgkSSDMountingBlockLength[1],0.,0.);
  TGeoRotation* mountingblockrot = new TGeoRotation();
  mountingblockrot->SetAngles(90.,180.,-90.);
  mountingblockcombitrans->SetRotation(*mountingblockrot);
  /////////////////////////////////////////////////////////////
  // Generating the Mounting Block Screw Vertices 
  /////////////////////////////////////////////////////////////  
  const Int_t kscrewvertexnumber = 15;
  Double_t alpha = TMath::ACos(0.5*(fgkSSDMountingBlockHeight[1]
				 -					fgkSSDMountingBlockScrewHoleEdge)
				 /				fgkSSDMountingBlockScrewHoleRadius[0])
				 * TMath::RadToDeg();
  Double_t phi0 = 90.+alpha;
  Double_t phi = 270.-2*alpha;
  Double_t deltaphi = phi/kscrewvertexnumber;	
  TVector3* screwvertex[kscrewvertexnumber+1];
  for(Int_t i=0; i<kscrewvertexnumber+1; i++)	
	screwvertex[i] = new TVector3(fgkSSDMountingBlockScrewHoleRadius[0]
				   *CosD(phi0+i*deltaphi),
				   fgkSSDMountingBlockScrewHoleRadius[0]
				   *SinD(phi0+i*deltaphi));
  Double_t xscrewvertex[kscrewvertexnumber+6];
  Double_t yscrewvertex[kscrewvertexnumber+6];
  xscrewvertex[0] = - fgkSSDMountingBlockScrewHoleRadius[0];	
  yscrewvertex[0] = -0.5*(fgkSSDMountingBlockWidth
				  -		  fgkSSDMountingBlockScrewHoleEdge);
  xscrewvertex[1] = xscrewvertex[0];
  yscrewvertex[1] = 0.5*fgkSSDMountingBlockScrewHoleEdge;
  xscrewvertex[2] = screwvertex[0]->X();
  yscrewvertex[2] = yscrewvertex[1];
  for(Int_t i=0; i<kscrewvertexnumber+1; i++){
	xscrewvertex[i+3] = screwvertex[i]->X(); 	
	yscrewvertex[i+3] = screwvertex[i]->Y(); 	
  } 
  xscrewvertex[kscrewvertexnumber+4] = 0.5*fgkSSDMountingBlockScrewHoleEdge; 	
  yscrewvertex[kscrewvertexnumber+4] = yscrewvertex[kscrewvertexnumber+3]; 	
  xscrewvertex[kscrewvertexnumber+5] = xscrewvertex[kscrewvertexnumber+4];
  yscrewvertex[kscrewvertexnumber+5] = yscrewvertex[0];
  TGeoXtru* ssdmountingblockscrewshape = new TGeoXtru(2);
  ssdmountingblockscrewshape->DefinePolygon(kscrewvertexnumber+6,xscrewvertex,yscrewvertex);
  ssdmountingblockscrewshape->DefineSection(0,yvertex[0]);
  ssdmountingblockscrewshape->DefineSection(1,-0.5*fgkSSDMountingBlockHeight[0]
							+				   fgkSSDMountingBlockHeight[2]);
  TGeoVolume* ssdmountingblockscrew = new TGeoVolume("SSDMountingBlockScrew",
								                ssdmountingblockscrewshape,
											    fSSDMountingBlockMedium);
  ssdmountingblockscrew->SetLineColor(fColorG10);
  TGeoCombiTrans* ssdmountingblockscrewcombitrans[4];
  for(Int_t i=0; i<4; i++) ssdmountingblockscrewcombitrans[i] = new TGeoCombiTrans();
  ssdmountingblockscrewcombitrans[0]->SetTranslation(-0.5*fgkSSDMountingBlockScrewHoleEdge,
									-				 yscrewvertex[1],
													 0.5*fgkSSDMountingBlockHeight[0]
									-				 fgkSSDMountingBlockHeight[2]
									+				 0.5*(-0.5*fgkSSDMountingBlockHeight[0]
									+				 fgkSSDMountingBlockHeight[2]
									-				 yvertex[0]));
  ssdmountingblockscrewcombitrans[1]->SetTranslation(0.5*fgkSSDMountingBlockScrewHoleEdge,
													-0.5*fgkSSDMountingBlockScrewHoleEdge,
														 yscrewvertex[1]
													-0.5*(-0.5*fgkSSDMountingBlockHeight[0]
													 +fgkSSDMountingBlockHeight[2]
													 -yvertex[0]));
  ssdmountingblockscrewcombitrans[2]->SetTranslation(-0.5*fgkSSDMountingBlockScrewHoleEdge,
													  yscrewvertex[1],
									-				  0.5*fgkSSDMountingBlockHeight[0]
									+				  fgkSSDMountingBlockHeight[2]
									-				  0.5*(-0.5*fgkSSDMountingBlockHeight[0]
									+				  fgkSSDMountingBlockHeight[2]
									-				  yvertex[0]));
  ssdmountingblockscrewcombitrans[3]->SetTranslation(0.5*fgkSSDMountingBlockScrewHoleEdge,
													 yscrewvertex[1],
									-				 yscrewvertex[1]
									+				 0.5*(-0.5*fgkSSDMountingBlockHeight[0]
									+				 fgkSSDMountingBlockHeight[2]
									-				 yvertex[0]));
  TGeoRotation* ssdmountingblockscrewrot[4];
  for(Int_t i=0; i<4; i++) ssdmountingblockscrewrot[i] = new TGeoRotation();
	ssdmountingblockscrewrot[1]->SetAngles(90.,-180.,-90.);	
    ssdmountingblockscrewrot[2]->SetAngles(0.,180.,0.);	
    ssdmountingblockscrewrot[3]->SetAngles(180.,0.,0.);	
  for(Int_t i=1; i<4; i++) 
	ssdmountingblockscrewcombitrans[i]->SetRotation(*ssdmountingblockscrewrot[i]);
  TGeoRotation* ssdmountingblockglobalrot = new TGeoRotation();
  ssdmountingblockglobalrot->SetAngles(0.,90.,0.);	
  TGeoTranslation* ssdmountingblockglobaltrans = new TGeoTranslation();
  ssdmountingblockglobaltrans->SetTranslation(0.5*fgkSSDMountingBlockLength[0]
							 +				  xvertex[0],yscrewvertex[1]
							 -				  0.5*(-0.5*fgkSSDMountingBlockHeight[0]
							 +				  fgkSSDMountingBlockHeight[2]
							 -				  yvertex[0]),0.);	
  TGeoHMatrix* ssdmountingblockscrewmatrix[4];
  for(Int_t i=0; i<4; i++){
	ssdmountingblockscrewmatrix[i] = 
		new TGeoHMatrix((*ssdmountingblockglobalrot)*(*ssdmountingblockscrewcombitrans[i])); 
	ssdmountingblockscrewmatrix[i]->MultiplyLeft(ssdmountingblockglobaltrans);
  }
  ///////////////////////////////////////////////////////////////////////
  // TGeoXtru for Mother Volume 
  ///////////////////////////////////////////////////////////////////////
  const Int_t kvertexmothernumber = 12;
  Double_t xmothervertex[kvertexmothernumber];
  Double_t ymothervertex[kvertexmothernumber];
  for(Int_t i=0; i<6; i++){
	xmothervertex[i] = xvertex[i];
	ymothervertex[i] = yvertex[i];
  } 
  xmothervertex[6]  = xvertex[5]+fgkSSDMountingBlockLength[2];
  ymothervertex[6]  = ymothervertex[5];
  xmothervertex[7]  = xmothervertex[6];
  ymothervertex[7]  = ymothervertex[4];
  xmothervertex[8]  = xmothervertex[7]
					+ 0.5*(fgkSSDMountingBlockLength[1]
					-	   fgkSSDMountingBlockLength[2]);
  ymothervertex[8]  = ymothervertex[4];
  xmothervertex[9]  = xmothervertex[8];
  ymothervertex[9]  = ymothervertex[2];
  xmothervertex[10] = xvertex[0]+fgkSSDMountingBlockLength[0];
  ymothervertex[10] = ymothervertex[1];
  xmothervertex[11] = xmothervertex[10];
  ymothervertex[11] = ymothervertex[0];  
  TGeoXtru* ssdmountingblockmothershape = new TGeoXtru(2);
  ssdmountingblockmothershape->DefinePolygon(kvertexmothernumber,xmothervertex,ymothervertex);
  ssdmountingblockmothershape->DefineSection(0,-0.5*fgkSSDMountingBlockWidth);
  ssdmountingblockmothershape->DefineSection(1,0.5*fgkSSDMountingBlockWidth);
  TGeoVolume* ssdmountingblockmother = new TGeoVolume("SSDMountingBlockMother",
								          ssdmountingblockmothershape,
										  fSSDAir);
  /////////////////////////////////////////////////////////////
  // Placing the Volumes into Mother Volume 
  /////////////////////////////////////////////////////////////
  ssdmountingblockmother->AddNode(ssdmountingblock,1);
  ssdmountingblockmother->AddNode(ssdmountingblock,2,mountingblockcombitrans);
  for(Int_t i=0; i<4; i++) 
	ssdmountingblockmother->AddNode(ssdmountingblockscrew,i+1,
									ssdmountingblockscrewmatrix[i]);
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  delete mountingblockrot;
  for(Int_t i=0; i<4; i++) delete ssdmountingblockscrewrot[i];
  delete ssdmountingblockglobalrot; 
  delete ssdmountingblockglobaltrans; 
  /////////////////////////////////////////////////////////////
  return ssdmountingblockmother;
}
///////////////////////////////////////////////////////////////////////////////
 TGeoVolume* AliITSv11GeometrySSD::GetMountingBlockClip() const {
  /////////////////////////////////////////////////////////////
  // Method generating the Mounting Block Clip 
  /////////////////////////////////////////////////////////////  
  const Int_t kmothervertexnumber = 10;
  Double_t xmothervertex[kmothervertexnumber];
  Double_t ymothervertex[kmothervertexnumber];
  xmothervertex[0] = -0.25*(fgkSSDMountingBlockLength[0]-fgkSSDMountingBlockLength[1])
				   - 0.5*(fgkSSDSensorWidth-fgkSSDMountingBlockLength[0]); 
  xmothervertex[1] = xmothervertex[0];
  xmothervertex[2] = xmothervertex[0]+0.5*(fgkMountingBlockClibScrewPosition
				   - fgkMountingBlockClibScrewRadius);
  xmothervertex[3] = xmothervertex[2]; 
  xmothervertex[4] = xmothervertex[3]+2.*fgkMountingBlockClibScrewRadius; 
  xmothervertex[5] = xmothervertex[4]; 
  xmothervertex[6] = xmothervertex[0]+fgkMountingBlockClipLength; 
  xmothervertex[7] = xmothervertex[6]; 
  xmothervertex[8] = -0.25*(fgkSSDMountingBlockLength[0]-fgkSSDMountingBlockLength[1]); 
  xmothervertex[9] = xmothervertex[8]; 
  ymothervertex[0] = -0.5*fgkCoolingTubeSupportHeight-fgkSSDModuleCoolingBlockToSensor
			       + fgkSSDMountingBlockHeight[1]-0.5*fgkSSDMountingBlockHeight[0]
				   - fgkSSDModuleVerticalDisalignment;
  ymothervertex[1] = 0.5*fgkSSDMountingBlockHeight[0]+fgkMountingBlockClipThickness;
  ymothervertex[2] = ymothervertex[1];
  ymothervertex[3] = ymothervertex[2]+(fgkSSDMountingBlockHeight[1]
				   - fgkSSDMountingBlockHeight[0]-fgkMountingBlockClipThickness
				   - 0.5*fgkCoolingTubeSupportHeight-fgkCoolingTubeSupportRmax);
  ymothervertex[4] = ymothervertex[3];
  ymothervertex[5] = ymothervertex[2];
  ymothervertex[6] = ymothervertex[5];
  ymothervertex[7] = ymothervertex[6]-fgkMountingBlockClipThickness;
  ymothervertex[8] = ymothervertex[7];
  ymothervertex[9] = ymothervertex[0];
  ///////////////////////////////////////////////////////////////////////
  // TGeoXTru Volume definition for Mounting Block Clip Part
  ///////////////////////////////////////////////////////////////////////
  TGeoXtru* ssdmountingblockclipshape = new TGeoXtru(2);
  ssdmountingblockclipshape->DefinePolygon(kmothervertexnumber,xmothervertex,ymothervertex);
  ssdmountingblockclipshape->DefineSection(0,0.5*fgkSSDMountingBlockWidth-fgkMountingBlockSupportWidth[0]);
  ssdmountingblockclipshape->DefineSection(1,0.5*fgkSSDMountingBlockWidth);
  TGeoVolume* ssdmountingblockclip = new TGeoVolume("SSDMountingBlockClip",
								          ssdmountingblockclipshape,fSSDAir);
  ssdmountingblockclip->SetLineColor(4);
  ///////////////////////////////////////////////////////////////////////
  // TGeoXTru Volume definition for Clip 
  ///////////////////////////////////////////////////////////////////////
  const Int_t kclipvertexnumber = 6;
  Double_t xclipvertex[kclipvertexnumber];
  Double_t yclipvertex[kclipvertexnumber];
  xclipvertex[0] = xmothervertex[0];
  xclipvertex[1] = xclipvertex[0];
  xclipvertex[2] = xmothervertex[6];
  xclipvertex[3] = xclipvertex[2];
  xclipvertex[4] = xclipvertex[0]+fgkMountingBlockClipThickness;
  xclipvertex[5] = xclipvertex[4];
  yclipvertex[0] = ymothervertex[0];
  yclipvertex[1] = ymothervertex[1];
  yclipvertex[2] = yclipvertex[1];
  yclipvertex[3] = yclipvertex[1]-fgkMountingBlockClipThickness;
  yclipvertex[4] = yclipvertex[3];
  yclipvertex[5] = yclipvertex[0];
  TGeoXtru* clipshape = new TGeoXtru(2);
  clipshape->DefinePolygon(kclipvertexnumber,xclipvertex,yclipvertex);
  clipshape->DefineSection(0,0.5*fgkSSDMountingBlockWidth-fgkMountingBlockClibWidth);
  clipshape->DefineSection(1,0.5*fgkSSDMountingBlockWidth-fgkMountingBlockSupportWidth[0]
							 +   fgkMountingBlockClibWidth);
  TGeoVolume* clip = new TGeoVolume("SSDClip",clipshape,fSSDMountingBlockMedium);
  clip->SetLineColor(18);
  ///////////////////////////////////////////////////////////////////////
  // Ladder Support Piece  
  ///////////////////////////////////////////////////////////////////////
  const Int_t ksupportvertexnumber = 4;
  Double_t xsupportvertex[ksupportvertexnumber];
  Double_t ysupportvertex[ksupportvertexnumber];
  xsupportvertex[0] = xclipvertex[5];
  xsupportvertex[1] = xsupportvertex[0];
  xsupportvertex[2] = xmothervertex[9];
  xsupportvertex[3] = xsupportvertex[2];
  ysupportvertex[0] = yclipvertex[0];
  ysupportvertex[1] = yclipvertex[3];
  ysupportvertex[2] = ysupportvertex[1];
  ysupportvertex[3] = ysupportvertex[0];
  TGeoXtru* supportshape = new TGeoXtru(2);
  supportshape->DefinePolygon(ksupportvertexnumber,xsupportvertex,ysupportvertex);
  supportshape->DefineSection(0,0.5*fgkSSDMountingBlockWidth-fgkMountingBlockSupportWidth[0]);
  supportshape->DefineSection(1,0.5*fgkSSDMountingBlockWidth);
  TGeoVolume* support = new TGeoVolume("RingSupport",supportshape,fSSDMountingBlockMedium);
  support->SetLineColor(9);
  ///////////////////////////////////////////////////////////////////////
  // TGeoXTru Volume definition for Screw   
  ///////////////////////////////////////////////////////////////////////
  Double_t radius[2] = {fgkMountingBlockClibScrewRadius,
						0.5*fgkMountingBlockClibScrewRadius};
  Int_t edgesnumber[2] = {50,6};
  Double_t section[2] = {-0.5*(ymothervertex[3]-ymothervertex[2]),
						 +0.5*(ymothervertex[3]-ymothervertex[2])};
  TGeoShape* clipscrewshape = GetScrewShape(radius,edgesnumber,section);
  TGeoVolume* clipscrew = new TGeoVolume("ClipScrewShape",clipscrewshape,fSSDSupportRingAl);
  clipscrew->SetLineColor(12);
  TGeoRotation* screwrot = new TGeoRotation();
  screwrot->SetAngles(0.,90.,0.);
  TGeoTranslation* screwtrans = new TGeoTranslation();
  screwtrans->SetTranslation(xmothervertex[3]+fgkMountingBlockClibScrewRadius,
							 0.5*(ymothervertex[3]+ymothervertex[2]),
							 0.5*fgkSSDMountingBlockWidth+
							-0.5*fgkMountingBlockSupportWidth[0]);
  TGeoCombiTrans* screwcombitrans = new TGeoCombiTrans(*screwtrans,*screwrot);
  ///////////////////////////////////////////////////////////////////////
  // Placing the Volumes
  ///////////////////////////////////////////////////////////////////////
  ssdmountingblockclip->AddNode(clip,1);
  ssdmountingblockclip->AddNode(support,1);
  ssdmountingblockclip->AddNode(clipscrew,1,screwcombitrans);
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////  
  delete screwtrans;
  delete screwrot;
  /////////////////////////////////////////////////////////////
  return ssdmountingblockclip;
}
///////////////////////////////////////////////////////////////////////////////
TList* AliITSv11GeometrySSD::GetCoolingTubeList()const{
  /////////////////////////////////////////////////////////////
  // Method generating the Cooling Tube 
  /////////////////////////////////////////////////////////////  
   TGeoTube** coolingtubeshape[fgkcoolingtubenumber];
   for(Int_t i=0; i<fgkcoolingtubenumber; i++) coolingtubeshape[i] = 
												new	TGeoTube*[2];
   // Ladder Cooling Tubes
   coolingtubeshape[0][0] = new TGeoTube(fgkCoolingTubeRmin,fgkCoolingTubeRmax,
					  0.25 * (fgkSSDSensorLength-2.*fgkSSDModuleStiffenerPosition[1]
						  -	2.*fgkSSDCoolingBlockWidth-fgkCoolingTubeSupportWidth));
   coolingtubeshape[0][1] = new TGeoTube(0.0,fgkCoolingTubeRmin,
										 coolingtubeshape[0][0]->GetDz());
   coolingtubeshape[1][0] = new TGeoTube(fgkCoolingTubeRmin,fgkCoolingTubeRmax,
										 0.5*(fgkSSDModuleStiffenerPosition[1]
						  -					  fgkSSDSensorOverlap));
   coolingtubeshape[1][1] = new TGeoTube(0.0,fgkCoolingTubeRmin,
										 coolingtubeshape[1][0]->GetDz());
   coolingtubeshape[2][0] = new TGeoTube(fgkCoolingTubeRmin,fgkCoolingTubeRmax,
										 0.5*(fgkSSDModuleStiffenerPosition[1]));
   coolingtubeshape[2][1] = new TGeoTube(0.0,fgkCoolingTubeRmin,
										 coolingtubeshape[2][0]->GetDz());
   // End Ladder Cooling Tubes	
   TGeoTube** endladdercoolingtubeshape[fgkendladdercoolingtubenumber];
   for(Int_t i=0; i<fgkendladdercoolingtubenumber; i++) 
   endladdercoolingtubeshape[i] = new	TGeoTube*[2];
   endladdercoolingtubeshape[0][0] = new TGeoTube(fgkCoolingTubeRmin,fgkCoolingTubeRmax,
							0.50 * (fgkEndLadderMountingBlockPosition[0]
						  -			fgkendladdercoolingsupportdistance[0]-fgkSSDTolerance));
   endladdercoolingtubeshape[0][1] = new TGeoTube(0.0,fgkCoolingTubeRmin,
									endladdercoolingtubeshape[0][0]->GetDz());
   endladdercoolingtubeshape[1][0] = new TGeoTube(fgkCoolingTubeRmin,fgkCoolingTubeRmax,
							0.50 * (fgkendladdercoolingsupportdistance[0]
						  +			fgkendladdercoolingsupportdistance[1]
						  -			fgkCoolingTubeSupportWidth-fgkSSDTolerance));
   endladdercoolingtubeshape[1][1] = new TGeoTube(0.0,fgkCoolingTubeRmin,
									endladdercoolingtubeshape[1][0]->GetDz());
   endladdercoolingtubeshape[2][0] = new TGeoTube(fgkCoolingTubeRmin,fgkCoolingTubeRmax,
							0.50 * (fgkEndLadderCarbonFiberLowerJunctionLength[0]
						  -			fgkEndLadderMountingBlockPosition[0]
						  -			fgkendladdercoolingsupportdistance[1]		
						  -			fgkCoolingTubeSupportWidth-fgkSSDTolerance));
   endladdercoolingtubeshape[2][1] = new TGeoTube(0.0,fgkCoolingTubeRmin,
									endladdercoolingtubeshape[2][0]->GetDz());
   endladdercoolingtubeshape[3][0] = new TGeoTube(fgkCoolingTubeRmin,fgkCoolingTubeRmax,
							  0.50 * (fgkMountingBlockToSensorSupport
							- 0.50 * (fgkSSDFlexHoleWidth-fgkSSDSensorSideSupportWidth)
							-		  fgkSSDStiffenerWidth-fgkSSDModuleStiffenerPosition[1]
							+		  fgkSSDSensorOverlap
							+		  fgkEndLadderCarbonFiberLowerJunctionLength[1]
							-		  fgkendladdercoolingsupportdistance[2]
							-		  fgkEndLadderMountingBlockPosition[1]
							-		  fgkCoolingTubeSupportWidth-fgkSSDTolerance));
   endladdercoolingtubeshape[3][1] = new TGeoTube(0.0,fgkCoolingTubeRmin,
									endladdercoolingtubeshape[3][0]->GetDz());
   endladdercoolingtubeshape[4][0] = new TGeoTube(fgkCoolingTubeRmin,fgkCoolingTubeRmax,
							  0.50 * (fgkendladdercoolingsupportdistance[2]-fgkSSDTolerance));
   endladdercoolingtubeshape[4][1] = new TGeoTube(0.0,fgkCoolingTubeRmin,
									endladdercoolingtubeshape[4][0]->GetDz());
   // Ladder Cooling Tubes
   TGeoVolume** coolingtube[fgkcoolingtubenumber];
   for(Int_t i=0; i<fgkcoolingtubenumber; i++) coolingtube[i] = 
											 new TGeoVolume*[2];
   coolingtube[0][0] = new TGeoVolume("OuterCoolingTube1",coolingtubeshape[0][0],
									  fSSDCoolingTubePhynox);
   coolingtube[0][1] = new TGeoVolume("InnerCoolingTube1",coolingtubeshape[0][1],
									  fSSDCoolingTubeWater);
   coolingtube[1][0] = new TGeoVolume("OuterCoolingTube2",coolingtubeshape[1][0],
									  fSSDCoolingTubePhynox);
   coolingtube[1][1] = new TGeoVolume("InnerCoolingTube2",coolingtubeshape[1][1],
									  fSSDCoolingTubeWater);
   coolingtube[2][0] = new TGeoVolume("OuterCoolingTube3",coolingtubeshape[2][0],
									  fSSDCoolingTubePhynox);
   coolingtube[2][1] = new TGeoVolume("InnerCoolingTube3",coolingtubeshape[2][1],
									  fSSDCoolingTubeWater);
   for(Int_t i=0; i<fgkcoolingtubenumber; i++){
	coolingtube[i][0]->SetLineColor(fColorPhynox);
	coolingtube[i][1]->SetLineColor(fColorWater);
   }
   // End Ladder Cooling Tubes	
   TGeoVolume** endladdercoolingtube[fgkendladdercoolingtubenumber];
   for(Int_t i=0; i<fgkendladdercoolingtubenumber; i++) 
   endladdercoolingtube[i] = new TGeoVolume*[2];
   endladdercoolingtube[0][0] = new TGeoVolume("OuterEndLadderCoolingTube1",
								endladdercoolingtubeshape[0][0],
								fSSDCoolingTubePhynox);
   endladdercoolingtube[0][1] = new TGeoVolume("InnerEndlLadderCoolingTube1",
								endladdercoolingtubeshape[0][1],
								fSSDCoolingTubeWater);
   endladdercoolingtube[1][0] = new TGeoVolume("OuterEndLadderCoolingTube2",
								endladdercoolingtubeshape[1][0],
								fSSDCoolingTubePhynox);
   endladdercoolingtube[1][1] = new TGeoVolume("InnerEndlLadderCoolingTube2",
								endladdercoolingtubeshape[1][1],
								fSSDCoolingTubeWater);
   endladdercoolingtube[2][0] = new TGeoVolume("OuterEndLadderCoolingTube3",
								endladdercoolingtubeshape[2][0],
								fSSDCoolingTubePhynox);
   endladdercoolingtube[2][1] = new TGeoVolume("InnerEndlLadderCoolingTube3",
								endladdercoolingtubeshape[2][1],
								fSSDCoolingTubeWater);
   endladdercoolingtube[3][0] = new TGeoVolume("OuterEndLadderCoolingTube4",
								endladdercoolingtubeshape[3][0],
								fSSDCoolingTubePhynox);
   endladdercoolingtube[3][1] = new TGeoVolume("InnerEndlLadderCoolingTube4",
								endladdercoolingtubeshape[3][1],
								fSSDCoolingTubeWater);
   endladdercoolingtube[4][0] = new TGeoVolume("OuterEndLadderCoolingTube5",
								endladdercoolingtubeshape[4][0],
								fSSDCoolingTubePhynox);
   endladdercoolingtube[4][1] = new TGeoVolume("InnerEndlLadderCoolingTube5",
								endladdercoolingtubeshape[4][1],
								fSSDCoolingTubeWater);
   for(Int_t i=0; i<fgkendladdercoolingtubenumber; i++){
		endladdercoolingtube[i][0]->SetLineColor(fColorPhynox);
		endladdercoolingtube[i][1]->SetLineColor(fColorWater);
   }
  /////////////////////////////////////////////////////////////
  // Virtual Volume containing Cooling Tubes
  /////////////////////////////////////////////////////////////
  // Ladder Cooling Tubes
  TGeoTube* virtualcoolingtubeshape[fgkcoolingtubenumber];
  for(Int_t i=0; i<fgkcoolingtubenumber; i++)
  virtualcoolingtubeshape[i] = new TGeoTube(coolingtubeshape[i][1]->GetRmin(),
											coolingtubeshape[i][0]->GetRmax(),
											coolingtubeshape[i][0]->GetDz());
  TGeoVolume* virtualcoolingtube[fgkcoolingtubenumber];
  virtualcoolingtube[0] = new TGeoVolume("CoolingTube1",virtualcoolingtubeshape[0],
									  fSSDAir);
  virtualcoolingtube[1] = new TGeoVolume("CoolingTube2",virtualcoolingtubeshape[1],
									  fSSDAir);
  virtualcoolingtube[2] = new TGeoVolume("CoolingTube3",virtualcoolingtubeshape[2],
									  fSSDAir);
  // End Ladder Cooling Tubes
  TGeoTube* endladdervirtualcoolingtubeshape[fgkendladdercoolingtubenumber];
  for(Int_t i=0; i<fgkendladdercoolingtubenumber; i++)
  endladdervirtualcoolingtubeshape[i] = new TGeoTube(endladdercoolingtubeshape[i][1]->GetRmin(),
											endladdercoolingtubeshape[i][0]->GetRmax(),
											endladdercoolingtubeshape[i][0]->GetDz());
  TGeoVolume* endladdervirtualcoolingtube[fgkendladdercoolingtubenumber];
  endladdervirtualcoolingtube[0] = new TGeoVolume("EndLadderCoolingTube1",
									  endladdervirtualcoolingtubeshape[0],
									  fSSDAir);
  endladdervirtualcoolingtube[1] = new TGeoVolume("EndLadderCoolingTube2",
									  endladdervirtualcoolingtubeshape[1],
									  fSSDAir);
  endladdervirtualcoolingtube[2] = new TGeoVolume("EndLadderCoolingTube3",
									  endladdervirtualcoolingtubeshape[2],
									  fSSDAir);
  endladdervirtualcoolingtube[3] = new TGeoVolume("EndLadderCoolingTube4",
									  endladdervirtualcoolingtubeshape[3],
									  fSSDAir);
  endladdervirtualcoolingtube[4] = new TGeoVolume("EndLadderCoolingTube5",
									  endladdervirtualcoolingtubeshape[4],
									  fSSDAir);
  TList* coolingtubelist = new TList();
  for(Int_t i=0; i<fgkcoolingtubenumber; i++){
	virtualcoolingtube[i]->AddNode(coolingtube[i][0],1);
	virtualcoolingtube[i]->AddNode(coolingtube[i][1],1);
    coolingtubelist->Add(virtualcoolingtube[i]);
  }
	endladdervirtualcoolingtube[0]->AddNode(endladdercoolingtube[0][0],1);
	endladdervirtualcoolingtube[0]->AddNode(endladdercoolingtube[0][1],1);
    coolingtubelist->Add(endladdervirtualcoolingtube[0]);
	endladdervirtualcoolingtube[1]->AddNode(endladdercoolingtube[1][0],1);
	endladdervirtualcoolingtube[1]->AddNode(endladdercoolingtube[1][1],1);
    coolingtubelist->Add(endladdervirtualcoolingtube[1]);
	endladdervirtualcoolingtube[2]->AddNode(endladdercoolingtube[2][0],1);
	endladdervirtualcoolingtube[2]->AddNode(endladdercoolingtube[2][1],1);
    coolingtubelist->Add(endladdervirtualcoolingtube[2]);
	endladdervirtualcoolingtube[3]->AddNode(endladdercoolingtube[3][0],1);
	endladdervirtualcoolingtube[3]->AddNode(endladdercoolingtube[3][1],1);
    coolingtubelist->Add(endladdervirtualcoolingtube[3]);
	endladdervirtualcoolingtube[4]->AddNode(endladdercoolingtube[4][0],1);
	endladdervirtualcoolingtube[4]->AddNode(endladdercoolingtube[4][1],1);
    coolingtubelist->Add(endladdervirtualcoolingtube[4]);
  return coolingtubelist;
}
///////////////////////////////////////////////////////////////////////////////
TGeoVolume* AliITSv11GeometrySSD::GetSSDCoolingBlock(Int_t nedges){
  /////////////////////////////////////////////////////////////
  // Method generating SSD Cooling Block    
  /////////////////////////////////////////////////////////////
  const Int_t kvertexnumber = 8;
  ///////////////////////////////////////
  // Vertex Positioning for TGeoXTru
  ///////////////////////////////////////
  TVector3** vertexposition = new TVector3*[2*kvertexnumber+nedges+1];
  vertexposition[0] = new TVector3(0.0,0.0);
  vertexposition[1] = new TVector3(0.0,fgkSSDCoolingBlockHeight[1]);
  vertexposition[2] = new TVector3(fgkSSDCoolingBlockHoleLength[1],
					  vertexposition[1]->Y());
  vertexposition[3] = new TVector3(vertexposition[2]->X(),
					  vertexposition[2]->Y()+fgkSSDCoolingBlockHeight[2]);
  vertexposition[4] = new TVector3(vertexposition[1]->X(),vertexposition[3]->Y());
  vertexposition[5] = new TVector3(vertexposition[4]->X(),
					+ vertexposition[3]->Y()+fgkSSDCoolingBlockHoleRadius[1]);
  vertexposition[6] = new TVector3(Xfrom2Points(vertexposition[5]->X(),
					  vertexposition[5]->Y(),0.5*(fgkSSDCoolingBlockLength
					- fgkSSDCoolingBlockHoleLength[0]
					- 4.*fgkSSDCoolingBlockHoleRadius[1]),
					  fgkSSDCoolingBlockHeight[0]
					- fgkSSDCoolingBlockHoleRadius[1],
					  fgkSSDCoolingBlockHeight[0]),fgkSSDCoolingBlockHeight[0]);
  vertexposition[7] = new TVector3(0.5*(fgkSSDCoolingBlockLength
					- fgkSSDCoolingBlockHoleLength[0]),
					  vertexposition[6]->Y());
  Double_t alpha = TMath::ACos(0.5*fgkSSDCoolingBlockHoleLength[0]
			   / fgkSSDCoolingBlockHoleRadius[0])*TMath::RadToDeg();
  Double_t phi = 180.-alpha;
  Double_t psi = 180.+2.*alpha;
  Double_t deltapsi = psi/nedges;
  Double_t radius = fgkSSDCoolingBlockHoleRadius[0]/CosD(0.5*deltapsi);
  TVector3* transvector = new TVector3(0.5*fgkSSDCoolingBlockLength,
						  fgkSSDCoolingBlockHoleCenter);
  for(Int_t i=0; i<nedges+1; i++){
	vertexposition[kvertexnumber+i] = new TVector3(radius*CosD(phi+i*deltapsi),
											       radius*SinD(phi+i*deltapsi));
   *vertexposition[kvertexnumber+i] += (*transvector);
  }
  Double_t param[4] = {1.0,0.0,0.0,-0.5*fgkSSDCoolingBlockLength};  
  for(Int_t i=0; i<kvertexnumber; i++)
    vertexposition[kvertexnumber+nedges+1+i] = 
						GetReflection(vertexposition[kvertexnumber-1-i],param);
  ///////////////////////////////////////////////////////////////////////
  // TGeoXTru Volume definition for Cooling Tube Support Arc Part
  ///////////////////////////////////////////////////////////////////////
  TGeoXtru* ssdcoolingblockshape = new TGeoXtru(2);	
  Double_t* xvertexpoints = new Double_t[2*kvertexnumber+nedges+1]; 
  Double_t* yvertexpoints = new Double_t[2*kvertexnumber+nedges+1];
  for(Int_t i=0; i<2*kvertexnumber+nedges+1; i++){
	xvertexpoints[i] = vertexposition[i]->X();
	yvertexpoints[i] = vertexposition[i]->Y();
  } 
  ssdcoolingblockshape->DefinePolygon(2*kvertexnumber+nedges+1,xvertexpoints,
											yvertexpoints);
  ssdcoolingblockshape->DefineSection(0,-0.5*fgkSSDCoolingBlockWidth);
  ssdcoolingblockshape->DefineSection(1,0.5*fgkSSDCoolingBlockWidth);
  TGeoVolume* ssdcoolingblock = new TGeoVolume("SSDCoolingBlock",
								          ssdcoolingblockshape,
										  fSSDAlCoolBlockMedium);
  ssdcoolingblock->SetLineColor(fColorAl);
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  delete [] vertexposition;
  delete xvertexpoints;
  delete yvertexpoints;
  /////////////////////////////////////////////////////////////
  return ssdcoolingblock;
}
/////////////////////////////////////////////////////////////////////////////////
TGeoVolume* AliITSv11GeometrySSD::GetSSDChipCables(Double_t SSDChipCablesHeigth, Int_t nedges){
  ///////////////////////////////////////////////////////
  const Int_t kssdchipcablesnumber    = 2;
  const Int_t kssdchipcableslaynumber = 2;
  const Int_t kvertexnumber			  = 4*(nedges+1)+4;
  Int_t ssdchipcablescolor[kssdchipcableslaynumber] = {fColorAl,fColorPolyhamide};
  Double_t ssdchipcablesradius[kssdchipcableslaynumber];
  ssdchipcablesradius[0] = 0.25*(SSDChipCablesHeigth
						 -  fgkSSDChipCablesHeight[0]
						 -  fgkSSDChipCablesHeight[1]);
  ssdchipcablesradius[1] = ssdchipcablesradius[0]-fgkSSDChipCablesHeight[0];
  Double_t ssdchipcablespiecelength[kssdchipcablesnumber];
  ssdchipcablespiecelength[0] = 0.5*(fgkSSDChipCablesWidth[0]
							  - 2.*TMath::Pi()*ssdchipcablesradius[0]
							  - ssdchipcablesradius[0]
							  - fgkSSDChipCablesWidth[1]
							  - fgkSSDChipCablesWidth[2]);
  ssdchipcablespiecelength[1] = ssdchipcablespiecelength[0]
							  - 0.5*(fgkSSDModuleStiffenerPosition[1]							  
							  +      fgkSSDChipCablesHeight[1]
							  +      fgkSSDSensorHeight);
  ///////////////////////////////////////////////////////
  // Vertex Positioning for TGeoXTrue Layer 1 and Layer 2
  ///////////////////////////////////////////////////////
  TVector3** vertexposition[kssdchipcableslaynumber];
  for(Int_t i=0; i<kssdchipcableslaynumber; i++) vertexposition[i] = 
												  new TVector3*[4*(nedges+1)+4];
  Double_t ratio[4];
  ratio[0] = ssdchipcablesradius[1]/ssdchipcablesradius[0]; 
  ratio[1] = (ssdchipcablesradius[1]-fgkSSDChipCablesHeight[1])
		   /  ssdchipcablesradius[0]; 
  ratio[2] = (ssdchipcablesradius[0]+fgkSSDChipCablesHeight[0])
		   /  ssdchipcablesradius[0];
  ratio[3] = (ssdchipcablesradius[0]+fgkSSDChipCablesHeight[0]
		   +  fgkSSDChipCablesHeight[1])
		   /  ssdchipcablesradius[0];
  Double_t phi = 180.;
  Double_t deltaphi = 180./nedges;
  Double_t angle = 0.0;
  Double_t **xvertexpoints = new Double_t*[kssdchipcableslaynumber];
  Double_t **yvertexpoints = new Double_t*[kssdchipcableslaynumber];
  for(Int_t i=0; i<kssdchipcableslaynumber;i++){
	xvertexpoints[i] = new Double_t[kvertexnumber];
	yvertexpoints[i] = new Double_t[kvertexnumber];
  }  
  TVector3* vertex = new TVector3();
  TVector3* transvector[kssdchipcableslaynumber];
  transvector[0] = new TVector3(fgkSSDChipWidth,
								SSDChipCablesHeigth-ssdchipcablesradius[0]);
  transvector[1] = new TVector3();
  TGeoXtru* ssdchipcableshape[kssdchipcableslaynumber*kssdchipcableslaynumber];
  TGeoVolume* ssdchipcable[kssdchipcableslaynumber*kssdchipcableslaynumber];
  const char* ssdchipcablename[kssdchipcableslaynumber*kssdchipcableslaynumber] = 
		{"SSDChipcableAllay1Left","SSDChipcableKaptonlay2Left",
		 "SSDChipcableAllay1Right","SSDChipcableKaptonlay2Right"};
  for(Int_t k=0; k<kssdchipcablesnumber; k++){
	transvector[1]->SetX(fgkSSDChipWidth-ssdchipcablespiecelength[k]);
	transvector[1]->SetY(ssdchipcablesradius[0]
				 +		 fgkSSDChipCablesHeight[0]
				 +		 fgkSSDChipCablesHeight[1]);  
	for(Int_t i=0; i<kssdchipcableslaynumber; i++){
		vertexposition[i][0] = new TVector3(0.,SSDChipCablesHeigth
							 - fgkSSDChipCablesHeight[0]-i*fgkSSDChipCablesHeight[1]);
		vertexposition[i][1] = new TVector3(0.,SSDChipCablesHeigth
							 - i*fgkSSDChipCablesHeight[0]);
		vertexposition[i][2*(nedges+1)+2] = 
					new TVector3(fgkSSDChipWidth+ssdchipcablesradius[0]
				+				 fgkSSDChipCablesWidth[1]
				+				 fgkSSDChipCablesWidth[2],
								((1.-i)*fgkSSDChipCablesHeight[i]
				+				 fgkSSDChipCablesHeight[1]));
        vertexposition[i][2*(nedges+1)+3] = 
					new TVector3(vertexposition[i][2*(nedges+1)+2]->X(),
								 vertexposition[i][2*(nedges+1)+2]->Y()
				-				 fgkSSDChipCablesHeight[i]);
	    for(Int_t j=0; j<nedges+1; j++){ 		
		    angle = 0.5*phi+TMath::Power(-1,i+1)*j*deltaphi;
			vertex->SetX(ssdchipcablesradius[0]*CosD(angle));
			vertex->SetY(ssdchipcablesradius[0]*SinD(angle));
			vertexposition[0][(nedges+1)*i+j+2] = 
						new TVector3(*vertex+*transvector[i]);
			vertexposition[1][(nedges+1)*i+j+2] = 
						new TVector3(vertex->X()*ratio[2*i]+transvector[i]->X(),
									 vertex->Y()*ratio[2*i]+transvector[i]->Y());
			vertexposition[0][(4-i)*(nedges+1)+4-j-1] = 
						new TVector3(*vertexposition[1][(nedges+1)*i+j+2]);
			vertexposition[1][(4-i)*(nedges+1)+4-j-1] = 
						new TVector3(vertex->X()*ratio[2*i+1]
							+			 transvector[i]->X(),
									   	 vertex->Y()*ratio[2*i+1]
							+	     	 transvector[i]->Y());
		}
	}
	for(Int_t i=0; i<kssdchipcableslaynumber; i++){
		for(Int_t j=0; j<kvertexnumber; j++){ 	
			xvertexpoints[i][j] = vertexposition[i][j]->X();
			yvertexpoints[i][j] = vertexposition[i][j]->Y();
		}
		ssdchipcableshape[kssdchipcablesnumber*k+i] = new TGeoXtru(2);
		ssdchipcableshape[kssdchipcablesnumber*k+i]->DefinePolygon(kvertexnumber,
										xvertexpoints[i],yvertexpoints[i]);
		ssdchipcableshape[kssdchipcablesnumber*k+i]->DefineSection(0,-0.5*fgkSSDChipCablesLength[1]);
		ssdchipcableshape[kssdchipcablesnumber*k+i]->DefineSection(1,+0.5*fgkSSDChipCablesLength[1]);
		ssdchipcable[kssdchipcablesnumber*k+i] = 
				new TGeoVolume(ssdchipcablename[kssdchipcablesnumber*k+i],
							   ssdchipcableshape[kssdchipcablesnumber*k+i],
							  (kssdchipcablesnumber*k+i)%2==0?
							   fSSDAlTraceChipCableMedium:fSSDKaptonChipCableMedium);
		ssdchipcable[kssdchipcablesnumber*k+i]->SetLineColor(ssdchipcablescolor[i]);
	}
	for(Int_t i=0; i<kssdchipcableslaynumber; i++)
		for(Int_t j=0; j<kvertexnumber; j++) delete vertexposition[i][j];
  }
  /////////////////////////////////////////////////////////////
  // Mother Volume definition 
  /////////////////////////////////////////////////////////////
  Double_t ssdchipseparation = fgkSSDSensorLength
							 - 2.*fgkSSDModuleStiffenerPosition[1]
							 - 2.*(fgkSSDStiffenerWidth-fgkSSDStiffenerToChipDist
							 - 0.5*fgkSSDChipWidth)-fgkSSDChipWidth;
  Double_t boxorigin[3] = {-0.5*ssdchipseparation,0.,0.5*SSDChipCablesHeigth}; 
  Double_t dx = ssdchipseparation+2.*(fgkSSDChipWidth+ssdchipcablesradius[0]
							  +fgkSSDChipCablesWidth[1]
							  +fgkSSDChipCablesWidth[2]);
  Double_t dy = fgkSSDChipCablesLength[1];
  Double_t dz = SSDChipCablesHeigth;
  new TGeoBBox(0.5*dx,0.5*dy,0.5*dz,boxorigin);
  TGeoVolumeAssembly* ssdchipcablesmother = new TGeoVolumeAssembly("SSDChipCablesMother");
//  TGeoVolume* ssdchipcablesmother = new TGeoVolume("SSDChipCablesMother",
//			  ssdchipcablesmotherbox,fSSDAir);
  /////////////////////////////////////////////////////////////
  // Rotation and Translation Definition for positioning 
  /////////////////////////////////////////////////////////////
  TGeoRotation* ssdchipcablesrot[5];
  ssdchipcablesrot[0] = new TGeoRotation("",90.,180.,-90);
  ssdchipcablesrot[1] = new TGeoRotation("",0.0,90.0,0.0);
  ssdchipcablesrot[2] = new TGeoRotation((*ssdchipcablesrot[1])*(*ssdchipcablesrot[0]));
  ssdchipcablesrot[3] = new TGeoRotation("",180.,0.0,0.0);
  ssdchipcablesrot[4] = new TGeoRotation((*ssdchipcablesrot[3])*(*ssdchipcablesrot[2]));
  TGeoCombiTrans* ssdchipcablescombitrans = new TGeoCombiTrans(-ssdchipseparation,
														0.,0.,ssdchipcablesrot[2]);
  ssdchipcablesmother->AddNode(ssdchipcable[0],1,ssdchipcablesrot[4]);
  ssdchipcablesmother->AddNode(ssdchipcable[1],1,ssdchipcablesrot[4]);
  ssdchipcablesmother->AddNode(ssdchipcable[2],1,ssdchipcablescombitrans);
  ssdchipcablesmother->AddNode(ssdchipcable[3],1,ssdchipcablescombitrans);
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<kssdchipcableslaynumber;i++){
	delete [] xvertexpoints[i];
	delete [] yvertexpoints[i];
  }
  for(Int_t i=0; i<kssdchipcableslaynumber; i++) delete [] vertexposition[i];
  for(Int_t i=0; i<kssdchipcableslaynumber; i++) delete transvector[i];
  delete vertex; 
  delete ssdchipcablesrot[0];
  delete ssdchipcablesrot[1];
  delete ssdchipcablesrot[3];
  /////////////////////////////////////////////////////////////
  return ssdchipcablesmother;
}
///////////////////////////////////////////////////////////////////////////////
TList* AliITSv11GeometrySSD::GetSSDChipSystem(){
  /////////////////////////////////////////////////////////////
  // SSD Chip Assembly
  /////////////////////////////////////////////////////////////
  TGeoVolume* ssdchipassembly = GetSSDChips();
  TList* ssdchipsystemlist = new TList();
//  const Int_t knedges = 20;
  const Int_t knedges = 5;
  const Int_t kchipsystemnumber = 2;
  /////////////////////////////////////////////////////////////
  // Mother Volume containing SSDChipSystem
  /////////////////////////////////////////////////////////////
  TGeoXtru* chipsystemothershape[kchipsystemnumber];
  for(Int_t i=0; i<kchipsystemnumber; i++) chipsystemothershape[i] = new TGeoXtru(2);
  const Int_t kmothervertexnumber = 12;  
  Double_t xmothervertex[kchipsystemnumber][kmothervertexnumber];
  Double_t ymothervertex[kchipsystemnumber][kmothervertexnumber];
  Double_t ssdchipcablesradius[kchipsystemnumber];
  Double_t ssdchipseparation = fgkSSDSensorLength
			     - 2.*fgkSSDModuleStiffenerPosition[1]
			     - 2.*(fgkSSDStiffenerWidth
			     - fgkSSDStiffenerToChipDist-0.5*fgkSSDChipWidth);
  for(Int_t i=0; i<kchipsystemnumber; i++)
	ssdchipcablesradius[i] = 0.25*(fgkSSDChipCablesHeight[i+2]
						   -  fgkSSDChipCablesHeight[0]
						   -  fgkSSDChipCablesHeight[1]);
  ///////////////////////
  // Setting the vertices 
  ///////////////////////
  xmothervertex[0][0]  = -0.5*fgkSSDChipCablesLength[1];  
  xmothervertex[0][1]  = xmothervertex[0][0];  
  xmothervertex[0][2]  = (fgkSSDChipNumber-1)*(fgkSSDChipLength
					   + fgkSSDChipSeparationLength)+0.5*fgkSSDChipCablesLength[1];  
  xmothervertex[0][3]  = xmothervertex[0][2];  
  xmothervertex[0][4]  = 0.5*fgkSSDChipCablesLength[1];  
  xmothervertex[0][5]  = xmothervertex[0][4];  
  xmothervertex[0][6]  = xmothervertex[0][2]-0.5*fgkSSDChipCablesLength[1];  
  xmothervertex[0][7]  = xmothervertex[0][6]; 
  xmothervertex[0][8]  = 0.0;  
  xmothervertex[0][9]  = xmothervertex[0][8];  
  xmothervertex[0][10] = xmothervertex[0][4];  
  xmothervertex[0][11] = xmothervertex[0][10];  
  for(Int_t i=0; i<kmothervertexnumber; i++) 
	xmothervertex[1][i] = xmothervertex[0][i]; 
  for(Int_t i=0; i<kchipsystemnumber; i++){
	ymothervertex[i][0]  = -0.5*fgkSSDChipWidth-ssdchipcablesradius[i]
						 - fgkSSDChipCablesWidth[1]-fgkSSDChipCablesWidth[2];
	ymothervertex[i][1]  = ssdchipseparation-ymothervertex[i][0];
	ymothervertex[i][2]  = ymothervertex[i][1];
	ymothervertex[i][3]  = ymothervertex[i][0];
	ymothervertex[i][4]  = ymothervertex[i][0];
	ymothervertex[i][5]  = 0.5*fgkSSDChipWidth;
	ymothervertex[i][6]  = ymothervertex[i][5];
	ymothervertex[i][7]  = ssdchipseparation-0.5*fgkSSDChipWidth;
	ymothervertex[i][8]  = ymothervertex[i][7];
	ymothervertex[i][9]  = ymothervertex[i][5];
	ymothervertex[i][10] = ymothervertex[i][5];
	ymothervertex[i][11] = ymothervertex[i][4];
  }
  //////////////////////////////////////////////////////////
//  TGeoVolume* chipsystemother[kchipsystemnumber];
  TGeoVolumeAssembly* chipsystemother[kchipsystemnumber];
  const char* chipsytemothername[kchipsystemnumber] = 
					{"SSDChipSytemother1","SSDChipSytemother2"};
  for(Int_t i=0; i<kchipsystemnumber; i++){
    chipsystemothershape[i]->DefinePolygon(kmothervertexnumber,
									xmothervertex[i],ymothervertex[i]);
    chipsystemothershape[i]->DefineSection(0,-fgkSSDChipCablesHeight[i+2]
										  -0.5*fgkSSDChipHeight);
    chipsystemothershape[i]->DefineSection(1,0.5*fgkSSDChipHeight);
//    chipsystemother[i] = new TGeoVolume(chipsytemothername[i],
//							  chipsystemothershape[i],fSSDAir);
    chipsystemother[i] = new TGeoVolumeAssembly(chipsytemothername[i]);
  }
  /////////////////////////////////////////////////////////////
  // SSD Chip Cables
  /////////////////////////////////////////////////////////////
  TGeoVolume* ssdchipcables[kchipsystemnumber];
  TGeoRotation** ssdchipcablesrot[kchipsystemnumber];
  TGeoTranslation** ssdchipcablestrans[kchipsystemnumber];
  TGeoCombiTrans** ssdchipcablescombitrans[kchipsystemnumber];
  //////////////////
  for(Int_t i=0; i<kchipsystemnumber; i++){
		ssdchipcables[i] = 
		GetSSDChipCables(fgkSSDChipCablesHeight[i+2],knedges);
		ssdchipcablestrans[i] = new TGeoTranslation*[fgkSSDChipNumber];
		ssdchipcablesrot[i] = new TGeoRotation*[fgkSSDChipNumber];
		ssdchipcablescombitrans[i] = new TGeoCombiTrans*[fgkSSDChipNumber];
  }
  for(Int_t i=0; i<kchipsystemnumber; i++){
	for(Int_t j=0; j<fgkSSDChipNumber; j++){
		ssdchipcablestrans[i][j] = new TGeoTranslation();
		ssdchipcablesrot[i][j] = new TGeoRotation();
		ssdchipcablescombitrans[i][j] = new TGeoCombiTrans();
		ssdchipcablesrot[i][j]->SetAngles(-90.0,0.0,0.0);
		ssdchipcablestrans[i][j]->SetTranslation(j*(fgkSSDChipLength
						  +                fgkSSDChipSeparationLength),
											0.5*fgkSSDChipWidth,
						  -					0.5*fgkSSDChipHeight
						  -					fgkSSDChipCablesHeight[i+2]);
		ssdchipcablescombitrans[i][j]->SetRotation(*ssdchipcablesrot[i][j]);
		ssdchipcablescombitrans[i][j]->SetTranslation(*ssdchipcablestrans[i][j]);
		chipsystemother[i]->AddNode(ssdchipcables[i],j+1,ssdchipcablescombitrans[i][j]);
	}
	chipsystemother[i]->AddNode(ssdchipassembly,i+1);
	ssdchipsystemlist->Add(chipsystemother[i]);	
  }
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<kchipsystemnumber; i++){
	for(Int_t j=0; j<fgkSSDChipNumber; j++){
		delete ssdchipcablesrot[i][j];
		delete ssdchipcablestrans[i][j];
	}
	delete ssdchipcablesrot[i];
	delete ssdchipcablestrans[i];
  }
  /////////////////////////////////////////////////////////////
  return ssdchipsystemlist;
}
///////////////////////////////////////////////////////////////////////////////
TGeoVolume* AliITSv11GeometrySSD::GetSSDChips() const{
  /////////////////////////////////////////////////////////////
  // SSD Chip Assembly Generation    
  /////////////////////////////////////////////////////////////
  const Int_t kssdchiprownumber = 2;
  TGeoBBox* ssdchipcompshape[2];
  ssdchipcompshape[0] =  new TGeoBBox("SSDChipCompShape",
										0.5*fgkSSDChipLength,
										0.5*fgkSSDChipWidth,
										0.5*(fgkSSDChipHeight-fgkSSDChipGlueHeight));
  ssdchipcompshape[1] =  new TGeoBBox("SSDChipGlueCompShape",
										0.5*fgkSSDChipLength,
										0.5*fgkSSDChipWidth,
										0.5*fgkSSDChipGlueHeight);
  TGeoVolume* ssdchipcomp[2];
  ssdchipcomp[0] = new TGeoVolume("SSDChipComp",ssdchipcompshape[0],fSSDChipMedium);
  ssdchipcomp[1] = new TGeoVolume("SSDChipGlueComp",ssdchipcompshape[1],
								  fSSDChipGlueMedium);
  ssdchipcomp[0]->SetLineColor(fColorSilicon);  
  ssdchipcomp[1]->SetLineColor(fColorEpoxy);
  TGeoTranslation* ssdchipcomptrans[2];
  ssdchipcomptrans[0] = new TGeoTranslation(0.,0.,-ssdchipcompshape[1]->GetDZ());
  ssdchipcomptrans[1] = new TGeoTranslation(0.,0.,ssdchipcompshape[0]->GetDZ());
  /////////////////////////////////////////////////////////////
  // Virtual Volume containing SSDChip   
  /////////////////////////////////////////////////////////////
  TGeoBBox* ssdvirtualchipshape = new TGeoBBox("SSDChipShape",0.5*fgkSSDChipLength,
						  							         0.5*fgkSSDChipWidth,
													         0.5*fgkSSDChipHeight);
  TGeoVolume* ssdchip = new TGeoVolume("SSDChip",ssdvirtualchipshape,fSSDAir);
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<2; i++) ssdchip->AddNode(ssdchipcomp[i],1,ssdchipcomptrans[i]);
  Double_t ssdchipseparation[2] = {fgkSSDChipLength+fgkSSDChipSeparationLength,
						  fgkSSDSensorLength-2.*fgkSSDModuleStiffenerPosition[1]
				   -  2.*(fgkSSDStiffenerWidth-fgkSSDStiffenerToChipDist
				   -  0.5*fgkSSDChipWidth)};
  /////////////////////////////////////////////////////////////
  // Virtual Volume containing SSDChipAssembly   
  /////////////////////////////////////////////////////////////
  TGeoXtru* ssdchipmothershape = new TGeoXtru(2);
  const Int_t kssdmothervertexnumber = 2*fgkSSDChipNumber;  
  Double_t xmothervertex[kssdmothervertexnumber];
  Double_t ymothervertex[kssdmothervertexnumber];
  ///////////////////////
  // Setting the vertices 
  ///////////////////////
  xmothervertex[0] = -0.5*fgkSSDChipLength,ymothervertex[0] = -0.5*fgkSSDChipWidth;
  xmothervertex[1] = xmothervertex[0], ymothervertex[1] = ssdchipseparation[1]
				   - ymothervertex[0];
  xmothervertex[2] = (fgkSSDChipNumber-1)*ssdchipseparation[0]-xmothervertex[0];
  ymothervertex[2] = ymothervertex[1];
  xmothervertex[3] = xmothervertex[2], ymothervertex[3] = ymothervertex[0];
  xmothervertex[4] = ssdchipseparation[0]+xmothervertex[0];
  ymothervertex[4] = ymothervertex[0];
  xmothervertex[5] = xmothervertex[4], ymothervertex[5] = -ymothervertex[4];
  xmothervertex[6] = (fgkSSDChipNumber-1)*ssdchipseparation[0]
				   + (0.5*fgkSSDChipLength-fgkSSDChipWidth);
  ymothervertex[6] = ymothervertex[5];
  xmothervertex[7] = xmothervertex[6], ymothervertex[7] = ymothervertex[2]
				   - fgkSSDChipWidth;
  xmothervertex[8] = -0.5*fgkSSDChipLength+fgkSSDChipWidth;
  ymothervertex[8] = ymothervertex[7];
  xmothervertex[9] = -0.5*fgkSSDChipLength+fgkSSDChipWidth;
  ymothervertex[9] = ymothervertex[6];
  xmothervertex[10] = -xmothervertex[0], ymothervertex[10] = ymothervertex[9];
  xmothervertex[11] = xmothervertex[10], ymothervertex[11] = ymothervertex[0];
  //////////////////////////////////////////////////////////
  ssdchipmothershape->DefinePolygon(kssdmothervertexnumber,
									xmothervertex,ymothervertex);
  ssdchipmothershape->DefineSection(0,-0.5*fgkSSDChipHeight);
  ssdchipmothershape->DefineSection(1, 0.5*fgkSSDChipHeight);
//  TGeoVolume* ssdchipmother = new TGeoVolume("SSDChipContainer",
//							  ssdchipmothershape,fSSDAir);
  TGeoVolumeAssembly* ssdchipmother = new TGeoVolumeAssembly("SSDChipContainer");
   /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<kssdchiprownumber; i++)
    for(Int_t j=0; j<fgkSSDChipNumber; j++) 
		ssdchipmother->AddNode(ssdchip,fgkSSDChipNumber*i+j+1,
		new TGeoTranslation(j*ssdchipseparation[0],i*ssdchipseparation[1],0.));
  return ssdchipmother;
}
/////////////////////////////////////////////////////////////////////////////////
TList* AliITSv11GeometrySSD::GetLadderCableSegment(Double_t ssdendladdercablelength){
  /////////////////////////////////////////////////////////////
  // Method returning a List containing pointers to Ladder Cable Volumes    
  /////////////////////////////////////////////////////////////
  const Int_t kladdercablesegmentnumber = 2;
  /////////////////////////////////////////
  // LadderSegmentBBox Volume
  /////////////////////////////////////////
  TGeoBBox* laddercablesegmentbboxshape[kladdercablesegmentnumber];
  const char* laddercablesegmentbboxshapename[kladdercablesegmentnumber] = 
				{"LadderCableSegmentBBoxShape1","LadderCableSegmentBBoxShape2"};
  for(Int_t i=0; i<kladdercablesegmentnumber; i++) laddercablesegmentbboxshape[i] = 
						  new TGeoBBox(laddercablesegmentbboxshapename[i],
									   0.5*fgkSSDFlexWidth[0],
									   0.5*fgkSSDLadderCableWidth,
									   0.5*fgkSSDFlexHeight[i]); 
  const char* laddercablesegmentbboxname[kladdercablesegmentnumber] = 
						  {"LadderCableSegmentBBox1","LadderCableSegmentBBox2"};
  TGeoVolume* laddercablesegmentbbox[kladdercablesegmentnumber];
  for(Int_t i=0; i<kladdercablesegmentnumber; i++){ 
			laddercablesegmentbbox[i] =
						  new TGeoVolume(laddercablesegmentbboxname[i],
										 laddercablesegmentbboxshape[i],
										 (i==0?fSSDAlTraceLadderCableMedium:
            fSSDKaptonLadderCableMedium));
			laddercablesegmentbbox[i]->SetLineColor(i==0 ? fColorAl : 
														   fColorPolyhamide);
  }
  TGeoTranslation* laddercablesegmentbboxtrans[kladdercablesegmentnumber];										  
  laddercablesegmentbboxtrans[0] = 
						   new TGeoTranslation("LadderCableSegmentBBoxTrans1",
											   0.5*fgkSSDFlexWidth[0],
											   0.5*fgkSSDLadderCableWidth,
											   0.5*fgkSSDFlexHeight[0]);
  laddercablesegmentbboxtrans[1] = 
						   new TGeoTranslation("LadderCableSegmentBBoxTrans2",
											   0.5*fgkSSDFlexWidth[0],
											   0.5*fgkSSDLadderCableWidth,
											   fgkSSDFlexHeight[0]
											   +0.5*fgkSSDFlexHeight[1]);
  TGeoVolume* laddercablesegmentbboxassembly = 
						   new TGeoVolumeAssembly("LadderCableSegmentBBoxAssembly"); 
  for(Int_t i=0; i<kladdercablesegmentnumber; i++)  
		laddercablesegmentbboxassembly->AddNode(laddercablesegmentbbox[i],1,
											    laddercablesegmentbboxtrans[i]);
/////////////////////////////////////////
// LadderSegmentArb8 Volume
/////////////////////////////////////////
  const Int_t kvertexnumber = 4;
  TVector3** laddercablesegmentvertexposition[kladdercablesegmentnumber];
  for(Int_t i = 0; i<kladdercablesegmentnumber; i++) laddercablesegmentvertexposition[i] = 
												  new TVector3*[kvertexnumber];
//Shape Vertex Positioning
  for(Int_t i=0; i<kladdercablesegmentnumber; i++){
	laddercablesegmentvertexposition[i][0] = new TVector3(0.,i*fgkSSDFlexHeight[0]);
	laddercablesegmentvertexposition[i][1] = new TVector3(fgkSSDLadderCableWidth,
														  i*fgkSSDFlexHeight[0]);
	laddercablesegmentvertexposition[i][2] = new TVector3(0.,fgkSSDFlexHeight[0]
										   +			     fgkSSDFlexHeight[1]
										   +			  i*fgkSSDFlexHeight[0]);
	laddercablesegmentvertexposition[i][3] = 
						   new TVector3(laddercablesegmentvertexposition[i][1]->X(),
										laddercablesegmentvertexposition[i][2]->Y());
  }
  Double_t laddercablesegmentwidth[2][2] = {{fgkSSDFlexHeight[0],fgkSSDFlexHeight[0]},
							     		    {fgkSSDFlexHeight[1],fgkSSDFlexHeight[1]}};	
  char* laddercablesegmentarbshapename[kladdercablesegmentnumber] = 
					{"LadderCableSegmentArbShape1","LadderCableSegmentArbShape2"};
  TGeoArb8* laddercablesegmentarbshape[kladdercablesegmentnumber];
  for(Int_t i = 0; i< kladdercablesegmentnumber; i++) laddercablesegmentarbshape[i] = 
					GetArbShape(laddercablesegmentvertexposition[i],
								laddercablesegmentwidth[i],
								fgkCarbonFiberJunctionWidth-fgkSSDFlexWidth[0],
								laddercablesegmentarbshapename[i]);
  const char* laddercablesegmentarbname[kladdercablesegmentnumber] = 
						  {"LadderCableSegmentArb1","LadderCableSegmentArb2"};
  TGeoVolume* laddercablesegmentarb[kladdercablesegmentnumber];
  for(Int_t i=0; i<kladdercablesegmentnumber; i++){
			 laddercablesegmentarb[i] =
						   new TGeoVolume(laddercablesegmentarbname[i],
										  laddercablesegmentarbshape[i],
										  (i==0?fSSDAlTraceLadderCableMedium:
            fSSDKaptonLadderCableMedium)); 
			 laddercablesegmentarb[i]->SetLineColor(i==0 ? fColorAl : 
														   fColorPolyhamide);
}
  TGeoRotation* laddercablesegmentarbrot[kladdercablesegmentnumber];
  laddercablesegmentarbrot[0] = new TGeoRotation("LadderCableSegmentArbRot1",
												 90.,90,-90.);	 
  laddercablesegmentarbrot[1] = new TGeoRotation("LadderCableSegmentArbRot2",
												  0.,90.,0.);	 
  TGeoCombiTrans* laddercablesegmentarbcombitrans =  
						   new TGeoCombiTrans("LadderCableSegmentArbCombiTrans",
							   0.5*(fgkCarbonFiberJunctionWidth-fgkSSDFlexWidth[0])
							 + fgkSSDFlexWidth[0],0.,0.,
						   new TGeoRotation((*laddercablesegmentarbrot[1])
						     *(*laddercablesegmentarbrot[0])));
  TGeoVolume* laddercablesegmentarbassembly = 
						   new TGeoVolumeAssembly("LadderCableSegmentArbAssembly"); 
  for(Int_t i=0; i<kladdercablesegmentnumber; i++)
  laddercablesegmentarbassembly->AddNode(laddercablesegmentarb[i],1,
										   laddercablesegmentarbcombitrans);
/////////////////////////////////////////
// End Ladder Cable Volume
/////////////////////////////////////////
  TGeoBBox* ladderendcablesegmentbboxshape[kladdercablesegmentnumber];
  const char* ladderendcablesegmentbboxshapename[kladdercablesegmentnumber] = 
				{"LadderEndCableSegmentBBoxShape1","LadderEndCableSegmentBBoxShape2"};
  for(Int_t i=0; i<kladdercablesegmentnumber; i++) ladderendcablesegmentbboxshape[i] = 
						  new TGeoBBox(ladderendcablesegmentbboxshapename[i],
									   0.5*ssdendladdercablelength,
									   0.5*fgkSSDLadderCableWidth,
									   0.5*fgkSSDFlexHeight[i]);
  const char* ladderendcablesegmentbboxname[kladdercablesegmentnumber] = 
						  {"LadderEndCableSegmentBBox1","LadderEndCableSegmentBBox2"};
  TGeoVolume* ladderendcablesegmentbbox[kladdercablesegmentnumber];
  for(Int_t i=0; i<kladdercablesegmentnumber; i++){ 
			ladderendcablesegmentbbox[i] =
						  new TGeoVolume(ladderendcablesegmentbboxname[i],
										 ladderendcablesegmentbboxshape[i],
										 (i==0?fSSDAlTraceLadderCableMedium:
            fSSDKaptonLadderCableMedium));
			ladderendcablesegmentbbox[i]->SetLineColor(i==0 ? fColorAl : 
														   fColorPolyhamide);
  }
  TGeoTranslation* ladderendcablesegmentbboxtrans[kladdercablesegmentnumber];										  
  ladderendcablesegmentbboxtrans[0] = 
						   new TGeoTranslation("LadderEndCableSegmentBBoxTrans0",
											   0.5*ssdendladdercablelength,
											   0.5*fgkSSDLadderCableWidth,
											   0.5*fgkSSDFlexHeight[0]);
  ladderendcablesegmentbboxtrans[1] = 
						   new TGeoTranslation("LadderEndCableSegmentBBoxTrans1",
											   0.5*ssdendladdercablelength,
											   0.5*fgkSSDLadderCableWidth,
											   fgkSSDFlexHeight[0]
											   +0.5*fgkSSDFlexHeight[1]);
  TGeoVolume* ladderendcablesegmentbboxassembly = 
						   new TGeoVolumeAssembly("LadderEndCableSegmentBBoxAssembly"); 
  for(Int_t i=0; i<kladdercablesegmentnumber; i++)  
		ladderendcablesegmentbboxassembly->AddNode(ladderendcablesegmentbbox[i],1,
											    ladderendcablesegmentbboxtrans[i]);
/////////////////////////////////////////
  TList* laddercablesegmentlist = new TList();
  laddercablesegmentlist->Add(laddercablesegmentbboxassembly);
  laddercablesegmentlist->Add(laddercablesegmentarbassembly);
  laddercablesegmentlist->Add(ladderendcablesegmentbboxassembly);
  return laddercablesegmentlist;
  }
/////////////////////////////////////////////////////////////////////////////////
TGeoVolume* AliITSv11GeometrySSD::GetLadderCable(Int_t n, Double_t ssdendladdercablelength){
  /////////////////////////////////////////////////////////////
  // Method generating Ladder Cable Volumes Assemblies    
  /////////////////////////////////////////////////////////////
  TList* laddercablesegmentlist = GetLadderCableSegment(ssdendladdercablelength);
  TGeoVolume* laddercable = new TGeoVolumeAssembly("LadderCable"); 
  for(Int_t i=0; i<n; i++){
	 TGeoTranslation* laddercabletrans = new TGeoTranslation(
							i*(fgkCarbonFiberJunctionWidth),
							fgkSSDLadderCableWidth-fgkSSDFlexWidth[0],
							i*(fgkSSDFlexHeight[0]+fgkSSDFlexHeight[1]));
    laddercable->AddNode((TGeoVolume*)laddercablesegmentlist->At(0),i+1,laddercabletrans);  
	if(i!=n-1) laddercable->AddNode((TGeoVolume*)laddercablesegmentlist->At(1),i+1,laddercabletrans);  
  }
  TGeoTranslation* endladdercabletrans = new TGeoTranslation("EndLadderCableTrans",
					  (n-1)*fgkCarbonFiberJunctionWidth+fgkSSDFlexWidth[0],
								 fgkSSDLadderCableWidth-fgkSSDFlexWidth[0],
					  (n-1)*(fgkSSDFlexHeight[0]+fgkSSDFlexHeight[1]));
  laddercable->AddNode((TGeoVolume*)laddercablesegmentlist->At(2),1,endladdercabletrans);
  return laddercable;
}
/////////////////////////////////////////////////////////////////////////////////
TGeoVolume* AliITSv11GeometrySSD::GetLadderCableAssembly(Int_t n, Double_t ssdendladdercablelength){
  /////////////////////////////////////////////////////////////
  // Method generating Ladder Cable Volumes Assembly   
  /////////////////////////////////////////////////////////////
  TGeoVolume* laddercableassembly = new TGeoVolumeAssembly("LadderCableAssembly");
  char laddercabletransname[30];
  for(Int_t i=0; i<n; i++){ 
	sprintf(laddercabletransname,"LadderCableTrans%i",i+1);
    laddercableassembly->AddNode(GetLadderCable(n-i,ssdendladdercablelength),i+1,
	new TGeoTranslation(laddercabletransname,i*fgkCarbonFiberJunctionWidth,0.,0.));
  }
  return laddercableassembly;
}
/////////////////////////////////////////////////////////////////////////////////
TList* AliITSv11GeometrySSD::GetLadderCableAssemblyList(Int_t n, Double_t ssdendladdercablelength){
  /////////////////////////////////////////////////////////////
  // Method generating Ladder Cable List Assemblies  
  /////////////////////////////////////////////////////////////  
  const Int_t kladdercableassemblynumber = 2;
  TGeoVolume* laddercableassembly = GetLadderCableAssembly(n,ssdendladdercablelength);
  TGeoVolume* ladderCable[kladdercableassemblynumber];
  char laddercableassemblyname[30];
  TList* laddercableassemblylist = new TList();
  for(Int_t i=0; i<kladdercableassemblynumber; i++){ 
	sprintf(laddercableassemblyname,"LadderCableAssembly%i",i+1);
	ladderCable[i] = new TGeoVolumeAssembly(laddercableassemblyname);
	ladderCable[i]->AddNode(laddercableassembly,i+1,i==0 ? NULL :
					 new TGeoCombiTrans((n-1)
					 *	 fgkCarbonFiberJunctionWidth+fgkSSDFlexWidth[0],
					     2.*fgkSSDLadderCableWidth+0.5*fgkSSDFlexWidth[0],
											0.,new TGeoRotation("",180,0.,0.)));
	laddercableassemblylist->Add(ladderCable[i]);
}
  return laddercableassemblylist;
}
///////////////////////////////////////////////////////////////////////////////
void AliITSv11GeometrySSD::SetLadderSegment(){
  /////////////////////////////////////////////////////////////
  // Method Generating Ladder Segment Array
  /////////////////////////////////////////////////////////////
  fladdersegment[0] = new TGeoVolumeAssembly("LadderSegment1");	
  fladdersegment[1] = new TGeoVolumeAssembly("LadderSegment2");	
  if(!fCreateMaterials) CreateMaterials();
  if(!fTransformationMatrices) CreateTransformationMatrices();
  if(!fBasicObjects) CreateBasicObjects();
  for(Int_t i=0; i<fgkladdersegmentnumber; i++){
  // Placing Carbon Fiber Support	
	for(Int_t j=0; j<fgkcarbonfibersupportnumber; j++){ 
		fladdersegment[i]->AddNode(fcarbonfibersupport[0],j+1,
											fcarbonfibersupportmatrix[j]);	
		fladdersegment[i]->AddNode(fcarbonfibersupport[1],j+1,
											fcarbonfibersupportmatrix[j]);
  }
  // Placing Carbon Fiber Junction
    for(Int_t j=0; j<fgkcarbonfiberjunctionumber; j++)
        fladdersegment[i]->AddNode(fcarbonfiberjunction,j+1,
								   fcarbonfiberjunctionmatrix[j]);
  // Placing Carbon Fiber Lower Support
	for(Int_t j=0; j<fgkcarbonfiberlowersupportnumber; j++)
		fladdersegment[i]->AddNode(fcarbonfiberlowersupport[j],j+1,
			           			   fcarbonfiberlowersupportrans[j]);	
  // Placing SSD Sensor Support
    for(Int_t j=0; j<fgkssdsensorsupportnumber; j++) 
	fladdersegment[i]->AddNode(j<2 ? fssdsensorsupport[0][i] :
								     fssdsensorsupport[1][i],
							   j+1,fssdsensorsupportmatrix[j]);
  // Placing SSD Cooling Tube Support 
	for(Int_t j=0; j<fgkcoolingtubesupportnumber; j++)
		fladdersegment[i]->AddNode(fcoolingtubesupport,j+1,
								   fcoolingtubesupportmatrix[j]);
  // Placing SSD Cooling Tube  
	for(Int_t j=0; j<2; j++)
		for(Int_t k=0; k<2; k++){
		fladdersegment[i]->AddNode(fcoolingtube[0],2*j+k+1,fcoolingtubematrix[j][k]);
		fladdersegment[i]->AddNode(fcoolingtube[j+1],k+1,fcoolingtubematrix[2+j][k]);
		}
  // Placing SSD Hybrid
    switch(i){
	case 0: 
		fladdersegment[i]->AddNode(fssdhybridcomponent[0],1,fhybridmatrix);
		fladdersegment[i]->AddNode(fssdhybridcomponent[2],1,fhybridmatrix);
		break;
    case 1:
		fladdersegment[i]->AddNode(fssdhybridcomponent[1],1,fhybridmatrix);
		fladdersegment[i]->AddNode(fssdhybridcomponent[2],1,fhybridmatrix);
		break;
	}
	// Placing Cooling Block System
    fladdersegment[i]->AddNode(fssdcoolingblocksystem,1,fcoolingblocksystematrix);
	// Placing SSD Flex
	for(Int_t j=0; j<fgkflexnumber; j++){
      fladdersegment[i]->AddNode(fssdstiffenerflex,j+1,fstiffenerflexmatrix[j]);
      fladdersegment[i]->AddNode(fssdendflex,j+1,fendflexmatrix[j]);
	}
   }
}
///////////////////////////////////////////////////////////////////////////////
void AliITSv11GeometrySSD::SetEndLadderSegment(){
  /////////////////////////////////////////////////////////////
  // Method Generating End Ladder
  /////////////////////////////////////////////////////////////
  // End Ladder Carbon Fiber Junction 
  /////////////////////////////////////////////////////////////
  fendladdersegment[0] = new TGeoVolumeAssembly("EndLadder1");
  fendladdersegment[1] = new TGeoVolumeAssembly("EndLadder2");
  if(!fCreateMaterials) CreateMaterials();
  if(!fTransformationMatrices) CreateTransformationMatrices();
  if(!fBasicObjects) CreateBasicObjects();
  for(Int_t i=0; i<fgkendlabbercarbonfiberjunctionumber; i++){
	for(Int_t j=0; j<fgkendladdercabonfiberjunctionmatrixnumber; j++)
		fendladdersegment[i]->AddNode(j==2 ? 
							fendladdercarbonfiberjunction[i][1] : 
							fendladdercarbonfiberjunction[i][0],
							j+1,fendladdercarbonfiberjunctionmatrix[i][j]);
  }
  /////////////////////////////////////////////////////////////
  // End Ladder Carbon Fiber Support 
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<fgkcarbonfibersupportnumber; i++)
      for(Int_t j=0; j<fgkendladdercarbonfibermatrixnumber; j++){
		  fendladdersegment[i]->AddNode(fcarbonfibersupport[0],j+1,
                  fendladdercarbonfibermatrix[i][j]);	
          fendladdersegment[i]->AddNode(fcarbonfibersupport[1],j+1,
                  fendladdercarbonfibermatrix[i][j]);	
      }
  /////////////////////////////////////////////////////////////
  // End Ladder Mounting Block
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<fgkendladdermountingblocknumber; i++)
       fendladdersegment[i]->AddNode(fendladdermountingblock,i+1,
				     fendladdermountingblockcombitrans[i]);
  /////////////////////////////////////////////////////////////
  // End Ladder Mounting Block Clip
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<fgkendladdermountingblocknumber; i++)
	for(Int_t j=0; j<2; j++)
		fendladdersegment[i]->AddNode(fendladdermountingblockclip,j+1,
					      fendladdermountingblockclipmatrix[i][j]);
  /////////////////////////////////////////////////////////////
  // End Ladder Lower Supports
  /////////////////////////////////////////////////////////////
  fendladdersegment[0]->AddNode(fcarbonfiberlowersupport[0],1,
				fendladderlowersupptrans[0]);
  fendladdersegment[1]->AddNode(fcarbonfiberlowersupport[0],2,
				fendladderlowersupptrans[1]);
  fendladdersegment[1]->AddNode(fcarbonfiberlowersupport[0],3,
				fendladderlowersupptrans[2]);
  /////////////////////////////////////////////////////////////
  // End Ladder Cooling Tube Support
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<2; i++) 
	for(Int_t j=0; j<(i==0?4:2); j++)   
		fendladdersegment[i]->AddNode(fcoolingtubesupport,j+1,
					      fendladdercoolingtubesupportmatrix[i][j]);
  /////////////////////////////////////////////////////////////
  // End Ladder Cooling Tube Support
  /////////////////////////////////////////////////////////////
//  fendladdersegment[0]->AddNode(fendladdercoolingtube[0],1,fendladdercoolingtubematrix[0][0]);									  
//  fendladdersegment[0]->AddNode(fendladdercoolingtube[0],2,fendladdercoolingtubematrix[0][1]);									  
  fendladdersegment[0]->AddNode(fendladdercoolingtube[1],1,fendladdercoolingtubematrix[0][2]);									  
  fendladdersegment[0]->AddNode(fendladdercoolingtube[1],2,fendladdercoolingtubematrix[0][3]);									  
  fendladdersegment[0]->AddNode(fendladdercoolingtube[2],1,fendladdercoolingtubematrix[0][4]);									  
  fendladdersegment[0]->AddNode(fendladdercoolingtube[2],2,fendladdercoolingtubematrix[0][5]);									  
  fendladdersegment[1]->AddNode(fendladdercoolingtube[3],1,fendladdercoolingtubematrix[1][0]);									  
  fendladdersegment[1]->AddNode(fendladdercoolingtube[3],2,fendladdercoolingtubematrix[1][1]);									  
  fendladdersegment[1]->AddNode(fendladdercoolingtube[4],1,fendladdercoolingtubematrix[1][2]);									  
  fendladdersegment[1]->AddNode(fendladdercoolingtube[4],2,fendladdercoolingtubematrix[1][3]);				  
}
///////////////////////////////////////////////////////////////////////////////
void AliITSv11GeometrySSD::SetLadder(){
  /////////////////////////////////////////////////////////////
  // Method Generating Ladder of Layer 5 and 6
  /////////////////////////////////////////////////////////////  
  Int_t ssdlaysensorsnumber[fgkladdernumber] = {fgkSSDLay5SensorsNumber,
												fgkSSDLay6SensorsNumber};
  /////////////////////////////////////////////////////////////////////////////						
  /// Generating Ladder Mother Volume Containing Ladder 
  /////////////////////////////////////////////////////////////////////////////		 
  TGeoXtru* laddershape[fgkladdernumber];	
  for(Int_t i=0; i<fgkladdernumber; i++) laddershape[i] = new TGeoXtru(2);
  const Int_t kmothervertexnumber = 8;  
  Double_t xmothervertex[fgkladdernumber][kmothervertexnumber];
  Double_t ymothervertex[fgkladdernumber][kmothervertexnumber];
  ///////////////////////
  // Setting the vertices 
  ///////////////////////
  Double_t laddercablethickness = (fgkSSDLay6SensorsNumber+2)
								* (fgkSSDFlexHeight[0]+fgkSSDFlexHeight[1]);
  xmothervertex[0][0] = -0.5*fgkSSDSensorWidth;
  ymothervertex[0][0] = -0.5*fgkCoolingTubeSupportHeight-fgkSSDModuleCoolingBlockToSensor
					  -  fgkSSDModuleVerticalDisalignment;
  xmothervertex[0][1] = xmothervertex[0][0];
  ymothervertex[0][1] = 0.0;
  xmothervertex[0][2] = - 0.5*fgkSSDModuleSensorSupportDistance-2.*fgkCoolingTubeSupportRmax
						- laddercablethickness/SinD(2.*fgkSSDFlexAngle);
  ymothervertex[0][2] = ymothervertex[0][1];
  ymothervertex[0][3] = 0.5*fgkCarbonFiberTriangleLength*TanD(2.*fgkSSDFlexAngle);
  xmothervertex[0][3] = xmothervertex[0][2]+ymothervertex[0][3]/TanD(2.*fgkSSDFlexAngle);		
  xmothervertex[0][4] = -xmothervertex[0][3];
  ymothervertex[0][4] = ymothervertex[0][3];
  xmothervertex[0][5] = -xmothervertex[0][2];
  ymothervertex[0][5] = ymothervertex[0][2];
  xmothervertex[0][6] = -xmothervertex[0][1];
  ymothervertex[0][6] = ymothervertex[0][1];
  xmothervertex[0][7] = -xmothervertex[0][0];
  ymothervertex[0][7] = ymothervertex[0][0];
  for(Int_t i=0; i<kmothervertexnumber; i++){
	xmothervertex[1][i] = xmothervertex[0][i];
	ymothervertex[1][i] = ymothervertex[0][i];
  }
  const char* laddername[fgkladdernumber] = {"ITSssdLay5Ladd","ITSssdLay6Ladd"};
  for(Int_t i=0; i<fgkladdernumber; i++){
	laddershape[i]->DefinePolygon(kmothervertexnumber,xmothervertex[i],
								    ymothervertex[i]);
    laddershape[i]->DefineSection(0,-fgkEndLadderCarbonFiberLowerJunctionLength[1]);
    laddershape[i]->DefineSection(1,ssdlaysensorsnumber[i]*fgkCarbonFiberJunctionWidth
											+fgkEndLadderCarbonFiberLowerJunctionLength[0]);
    fladder[i] = new TGeoVolume(laddername[i],laddershape[i],fSSDAir);
	fladder[i]->SetLineColor(4);
 }
///////////////////////////////////////////////////////////////////////////
 if(!fCreateMaterials) CreateMaterials();
 if(!fTransformationMatrices) CreateTransformationMatrices();
 if(!fBasicObjects) CreateBasicObjects();
 SetLadderSegment(); 
 SetEndLadderSegment();
  for(Int_t i=0; i<fgkladdernumber; i++){
	for(Int_t j=0; j<ssdlaysensorsnumber[i]; j++){
	//////////////////////////						
	/// Placing Ladder Segment
	//////////////////////////		
		fladder[i]->AddNode(j%2==0 ? fladdersegment[i==0 ? 0 : 1] :
								     fladdersegment[i==0 ? 1 : 0],
									 ssdlaysensorsnumber[i]-j-1,fladdermatrix[i][j]);
	//////////////////////////						
	/// Placing SSD Sensor
	//////////////////////////		
        if(i==0&&ssdlaysensorsnumber[i]-j-1==13) fSSDSensor5->SetLineColor(kRed);
		fladder[i]->AddNode(i==0?fSSDSensor5:fSSDSensor6,ssdlaysensorsnumber[i]-j-1,
							fssdsensormatrix[i][j]);
	}
	///////////////////////////////						
	/// Placing End Ladder Segment
	///////////////////////////////		
    fladder[i]->AddNode(fendladdersegment[0],1,fendladdersegmentmatrix[0][i]);
	fladder[i]->AddNode(fendladdersegment[1],1,fendladdersegmentmatrix[1][i]);
   }
/////////////////////////////////////////////////////////////////////////////						
/// Placing Ladder Cables
/////////////////////////////////////////////////////////////////////////////		
  Int_t sidecablenumber[2][2];
  sidecablenumber[0][0] = fgkSSDLay5SensorsNumber/2+1; 
  sidecablenumber[0][1] = sidecablenumber[0][0]-2;
  sidecablenumber[1][0] = (fgkSSDLay6SensorsNumber-1)/2+1;
  sidecablenumber[1][1] = sidecablenumber[1][0]-1;
  Double_t carbonfibertomoduleposition[3];
  carbonfibertomoduleposition[0] = -0.5*(fgkSSDSensorWidth-fgkCarbonFiberTriangleLength);
  carbonfibertomoduleposition[1] = - (2.*fgkSSDSensorLength-fgkSSDSensorOverlap)+
			 fgkSSDModuleStiffenerPosition[1]+fgkSSDStiffenerWidth
	 +		 0.5*fgkSSDFlexHoleLength+2.*fgkCarbonFiberJunctionWidth
	 -		 0.5*(fgkCarbonFiberLowerSupportWidth+fgkSSDSensorCenterSupportLength
	 -            fgkSSDSensorCenterSupportThickness[0]);
  carbonfibertomoduleposition[2] = - (fgkSSDModuleCoolingBlockToSensor
								 +   0.5*fgkCoolingTubeSupportHeight
	 -         fgkSSDSensorHeight-fgkSSDChipCablesHeight[3]-fgkSSDChipHeight);	
  const Double_t kendladdercablecorrection = 1.72*fgkmm; //this has to be tuned
  Double_t ssdendladdercablelength[4];
  ssdendladdercablelength[0] = carbonfibertomoduleposition[1]
							 + fgkSSDSensorLength
							 - fgkSSDModuleStiffenerPosition[1]
							 - fgkSSDStiffenerWidth 
							 - fgkSSDFlexWidth[0]
							 + fgkEndLadderCarbonFiberLowerJunctionLength[1]-0.000001*kendladdercablecorrection;
  ssdendladdercablelength[1] = carbonfibertomoduleposition[1]
							 + fgkSSDModuleStiffenerPosition[1]
							 + fgkSSDStiffenerWidth
							 + fgkEndLadderCarbonFiberLowerJunctionLength[1]-0.000001*kendladdercablecorrection;
  ssdendladdercablelength[2] = ssdendladdercablelength[1]
							 - fgkEndLadderCarbonFiberLowerJunctionLength[1]
							 + fgkEndLadderCarbonFiberLowerJunctionLength[0]
							 - kendladdercablecorrection;
  ssdendladdercablelength[3] = fgkCarbonFiberJunctionWidth-(fgkSSDSensorLength
							 + carbonfibertomoduleposition[1]
							 - fgkSSDModuleStiffenerPosition[1]
							 - fgkSSDStiffenerWidth)
							 + fgkEndLadderCarbonFiberLowerJunctionLength[0]-0.000001*kendladdercablecorrection;
  TList* laddercableassemblylist[4];
  const Int_t kendladdercablesnumber = 4;
  for(Int_t i=0; i<fgkladdercablesnumber; i++)
	for(Int_t j=0; j<kendladdercablesnumber; j++){
		laddercableassemblylist[j] = 
		GetLadderCableAssemblyList(sidecablenumber[i][j<2?0:1],
								   ssdendladdercablelength[j]);
//	    fladder[i]->AddNode((TGeoVolume*)laddercableassemblylist[j]->At(j%2==0?0:1),
//									j<2?1:2,fladdercablematrix[i][j]);
  }
}
////////////////////////////////////////////////////////////////////////////////
void AliITSv11GeometrySSD::SetLayer(){
////////////////////////////////////////////////////////////////////////////////
  // Creating Ladder of Layer 5 and Layer 6
  /////////////////////////////////////////////////////////////
  if(!fCreateMaterials) CreateMaterials();
  if(!fTransformationMatrices) CreateTransformationMatrices();
  if(!fBasicObjects) CreateBasicObjects();
  SetLadder(); // Generating the ladder of Layer5 and Layer6
  const Int_t kssdlayladdernumber[fgklayernumber] = 
			{fgkSSDLay5LadderNumber,fgkSSDLay6LadderNumber};
  /////////////////////////////////////////////////////////////
  // Generating mother volumes for Layer5 and Layer6
  /////////////////////////////////////////////////////////////
  fSSDLayer5 = new TGeoVolumeAssembly("ITSssdLayer5");  
  fSSDLayer6 = new TGeoVolumeAssembly("ITSssdLayer6");  
  Int_t *ladderindex[fgklayernumber];
  Int_t index[fgklayernumber] = {8,9};
  for(Int_t i=0; i<fgklayernumber; i++) ladderindex[i] = new Int_t[kssdlayladdernumber[i]];
  for(Int_t i=0; i<fgklayernumber; i++)	
	for(Int_t j=0; j<kssdlayladdernumber[i]; j++){
		ladderindex[i][j] = ((j>=0)&&(j<=kssdlayladdernumber[i]-index[i]-1)) ? 
							  j+index[i] : j+index[i]-kssdlayladdernumber[i]; 
		i ==0 ? fSSDLayer5->AddNode(fladder[0],ladderindex[i][j],flayermatrix[i][j]) : 
		        fSSDLayer6->AddNode(fladder[1],ladderindex[i][j],flayermatrix[i][j]);
	}
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<fgklayernumber; i++) delete ladderindex[i];
}
////////////////////////////////////////////////////////////////////////////////
void AliITSv11GeometrySSD::Layer5(TGeoVolume* moth){
  /////////////////////////////////////////////////////////////
  // Insert the layer 5 in the mother volume. 
  /////////////////////////////////////////////////////////////
  if (! moth) {
    printf("Error::AliITSv11GeometrySSD: Can't insert layer5, mother is null!\n");
    return;
  };
  if(!fSSDLayer5) SetLayer();
  fMotherVol = moth;
  TGeoTranslation* centerITSlayer5trans = new TGeoTranslation(0.,0.,-0.5*fgkSSDLay5LadderLength
										+ fgkLay5CenterITSPosition);
  moth->AddNode(fSSDLayer5,1,centerITSlayer5trans);
 }
////////////////////////////////////////////////////////////////////////////////
void AliITSv11GeometrySSD::Layer6(TGeoVolume* moth){
  /////////////////////////////////////////////////////////////
  // Insert the layer 6 in the mother volume. 
  /////////////////////////////////////////////////////////////
  if (! moth) {
    printf("Error::AliITSv11GeometrySSD: Can't insert layer6, mother is null!\n");
    return;
  };
  if(!fSSDLayer6) SetLayer();
  fMotherVol = moth;
  TGeoTranslation* centerITSlayer6trans = new TGeoTranslation(0.,0.,-0.5*fgkSSDLay6LadderLength
										+ fgkLay6CenterITSPosition);
  moth->AddNode(fSSDLayer6,1,centerITSlayer6trans);
 }
 ////////////////////////////////////////////////////////////////////////////////
 TList* AliITSv11GeometrySSD::GetMountingBlockSupport(Int_t nedges){
  /////////////////////////////////////////////////////////////
  // Method generating the Arc structure of Ladder Support 
  /////////////////////////////////////////////////////////////
  const Int_t kssdlayladdernumber[fgklayernumber] = 
			{fgkSSDLay5LadderNumber,fgkSSDLay6LadderNumber};
  Double_t mountingsupportedge[fgklayernumber];
  Double_t mountingblockratio[fgklayernumber];
  Double_t theta[fgklayernumber];
  Double_t phi[fgklayernumber];
  Double_t psi0[fgklayernumber];
  Double_t deltapsi[fgklayernumber];
  TVector3* mountingsupportedgevector[fgklayernumber];
  for(Int_t i=0; i<fgklayernumber; i++){
	mountingblockratio[i] = fgkSSDMountingBlockLength[0]/fgkMountingBlockSupportRadius[i];
    mountingsupportedge[i]    = 0.5*fgkMountingBlockSupportRadius[i]
							  *(TMath::Sqrt(4.0-TMath::Power(mountingblockratio[i],2))
							  * TMath::Sin(2.0*TMath::Pi()/kssdlayladdernumber[i])
							  - mountingblockratio[i]*(1.0+TMath::Cos(2.0*TMath::Pi()
							  / kssdlayladdernumber[i])));
    theta[i] = TMath::ASin(0.5*mountingblockratio[i]+mountingsupportedge[i]/fgkMountingBlockSupportRadius[i]);
    phi[i]   = TMath::ASin(0.5*mountingblockratio[i]);
	mountingsupportedgevector[i] = new TVector3();
    mountingsupportedgevector[i]->SetX(-0.5*fgkSSDMountingBlockLength[0]);
	mountingsupportedgevector[i]->SetY(fgkMountingBlockSupportRadius[i]*TMath::Sqrt(1.
								 -TMath::Power(mountingsupportedgevector[i]->X()
								 /			   fgkMountingBlockSupportRadius[i],2)));
    psi0[i] = 0.5*TMath::Pi()-phi[i];	
    deltapsi[i] = (theta[i]+phi[i])/nedges;
  }
  TVector3** vertex[fgklayernumber];
  TList* vertexlist[fgklayernumber];
  Int_t indexedge[fgklayernumber] = {0,0};
  for(Int_t i=0; i<fgklayernumber; i++){
	vertex[i] = new TVector3*[nedges+1];
	vertexlist[i] = new TList();
  } 
  for(Int_t i=0; i<fgklayernumber; i++){
	for(Int_t j=0; j<nedges+1; j++){
		vertex[i][j] = new TVector3(fgkMountingBlockSupportRadius[i]*TMath::Cos(psi0[i]+j*deltapsi[i]),
								    fgkMountingBlockSupportRadius[i]*TMath::Sin(psi0[i]+j*deltapsi[i]));
		if(vertex[i][j]->X()>mountingsupportedgevector[i]->X()) indexedge[i]++;
		vertexlist[i]->Add(vertex[i][j]);
	}
	vertexlist[i]->AddAt(mountingsupportedgevector[i],indexedge[i]);
  }
  Double_t** xsidevertex = new Double_t*[fgklayernumber]; 
  Double_t** ysidevertex = new Double_t*[fgklayernumber]; 
  Double_t** xcentervertex = new Double_t*[fgklayernumber]; 
  Double_t** ycentervertex = new Double_t*[fgklayernumber]; 
  Double_t** xsidelowervertex = new Double_t*[fgklayernumber];
  Double_t** ysidelowervertex = new Double_t*[fgklayernumber];  
  Double_t** xcenterlowervertex = new Double_t*[fgklayernumber];
  Double_t** ycenterlowervertex = new Double_t*[fgklayernumber];  
  for(Int_t i=0; i<fgklayernumber; i++){
    xsidevertex[i] = new Double_t[vertexlist[i]->GetSize()+2];
    ysidevertex[i] = new Double_t[vertexlist[i]->GetSize()+2];
    xcentervertex[i] = new Double_t[indexedge[i]+3];
    ycentervertex[i] = new Double_t[indexedge[i]+3];
	xsidelowervertex[i] = new Double_t[vertexlist[i]->GetSize()+1];
	ysidelowervertex[i] = new Double_t[vertexlist[i]->GetSize()+1];
	xcenterlowervertex[i] = new Double_t[indexedge[i]+3];
	ycenterlowervertex[i] = new Double_t[indexedge[i]+3];
	for(Int_t j=0; j<vertexlist[i]->GetSize(); j++){
		xsidevertex[i][j!=vertexlist[i]->GetSize()-1?j+3:0] = 
										((TVector3*)vertexlist[i]->At(j))->X();
		ysidevertex[i][j!=vertexlist[i]->GetSize()-1?j+3:0] = 
										((TVector3*)vertexlist[i]->At(j))->Y();
		xsidelowervertex[i][j] = ((TVector3*)vertexlist[i]->At(vertexlist[i]->GetSize()-1-j))->X();
		ysidelowervertex[i][j] = ((TVector3*)vertexlist[i]->At(vertexlist[i]->GetSize()-1-j))->Y();
		if(j<indexedge[i]+1){
			xcentervertex[i][j!=indexedge[i]?j+3:0] = 
										((TVector3*)vertexlist[i]->At(j))->X();
			ycentervertex[i][j!=indexedge[i]?j+3:0] = 
										((TVector3*)vertexlist[i]->At(j))->Y();
			xcenterlowervertex[i][j+1] = ((TVector3*)vertexlist[i]->At(indexedge[i]-j))->X();
			ycenterlowervertex[i][j+1] = ((TVector3*)vertexlist[i]->At(indexedge[i]-j))->Y();
		}
	}
	xsidevertex[i][1] = xsidevertex[i][0]; 
	ysidevertex[i][1] = fgkMountingBlockSupportRadius[i]; 
	xsidevertex[i][2] = xsidevertex[i][3]; 
	ysidevertex[i][2] = fgkMountingBlockSupportRadius[i]; 
	xcentervertex[i][1] = xcentervertex[i][0]; 
	ycentervertex[i][1] = fgkMountingBlockSupportRadius[i]; 
	xcentervertex[i][2] = xcentervertex[i][3]; 
	ycentervertex[i][2] = fgkMountingBlockSupportRadius[i]; 
	xsidelowervertex[i][vertexlist[i]->GetSize()] = xsidelowervertex[i][vertexlist[i]->GetSize()-1];
	ysidelowervertex[i][vertexlist[i]->GetSize()] = ysidelowervertex[i][0];
	xcenterlowervertex[i][0] = xcenterlowervertex[i][1];
	ycenterlowervertex[i][0] = ysidevertex[i][0];
	xcenterlowervertex[i][indexedge[i]+2] = xsidelowervertex[i][vertexlist[i]->GetSize()];
	ycenterlowervertex[i][indexedge[i]+2] = ycenterlowervertex[i][0];
  }
  /////////////////////////////////////////////////////////////
  // Building the Arc Structure of Ladder Supports 
  /////////////////////////////////////////////////////////////
  TGeoXtru* sidemountingblocksupportshape[fgklayernumber];
  TGeoXtru* centermountingsupportshape[fgklayernumber];
  TGeoXtru* sideladdersupportpieceshape[fgklayernumber];
  TGeoXtru* centerladdersupportpieceshape[fgklayernumber];
  TGeoVolume* sidemountingblocksupport[fgklayernumber];
  TGeoVolume* centermountingblocksupport[fgklayernumber];
  TGeoVolume* sideladdersupportpiece[fgklayernumber];
  TGeoVolume* centerladdersupportpiece[fgklayernumber];
  char sidemountingblockname[40];
  char centermountingblockname[40];
  char sideladdersupportpiecename[40];
  char centerladdersupportpiecename[40];
  for(Int_t i=0; i<fgklayernumber; i++){ 
	sprintf(sidemountingblockname,"MountingBlockSupportSideLay%dArc",i+5);
	sprintf(centermountingblockname,"MountingBlockSupportCenterLay%dArc",i+5);
	sprintf(sideladdersupportpiecename,"SideLadderSupportPieceLay%d",i+5);
	sprintf(centerladdersupportpiecename,"CenterLadderSupportPieceLay%d",i+5);
	sidemountingblocksupportshape[i] = new TGeoXtru(2);
    sidemountingblocksupportshape[i]->DefinePolygon(vertexlist[i]->GetSize()+2,
												xsidevertex[i],ysidevertex[i]);
    sidemountingblocksupportshape[i]->DefineSection(0,fgkMountingBlockSupportWidth[1]
													 -fgkMountingBlockSupportWidth[0]);
    sidemountingblocksupportshape[i]->DefineSection(1,fgkMountingBlockSupportWidth[1]);
    sidemountingblocksupport[i] = new TGeoVolume(sidemountingblockname,
								          sidemountingblocksupportshape[i],
										  fSSDAlCoolBlockMedium);
	sidemountingblocksupport[i]->SetLineColor(9);
	centermountingsupportshape[i] = new TGeoXtru(2);
    centermountingsupportshape[i]->DefinePolygon(indexedge[i]+3,
												xcentervertex[i],ycentervertex[i]);
	centermountingsupportshape[i]->DefineSection(0,0.);
    centermountingsupportshape[i]->DefineSection(1,fgkMountingBlockSupportWidth[1]
												  -fgkMountingBlockSupportWidth[0]);
    centermountingblocksupport[i] = new TGeoVolume(centermountingblockname,
								          centermountingsupportshape[i],
										  fSSDAlCoolBlockMedium);
	centermountingblocksupport[i]->SetLineColor(9);
	sideladdersupportpieceshape[i] = new TGeoXtru(2);
    sideladdersupportpieceshape[i]->DefinePolygon(vertexlist[i]->GetSize()+1,
												xsidelowervertex[i],ysidelowervertex[i]);
	sideladdersupportpieceshape[i]->DefineSection(0,fgkMountingBlockSupportWidth[1]
													 -fgkMountingBlockSupportWidth[0]);
    sideladdersupportpieceshape[i]->DefineSection(1,fgkMountingBlockSupportWidth[1]);
    sideladdersupportpiece[i] = new TGeoVolume(sideladdersupportpiecename,
								          sideladdersupportpieceshape[i],
										  fSSDCarbonFiberMedium);
	sideladdersupportpiece[i]->SetLineColor(fColorAl);
	centerladdersupportpieceshape[i] = new TGeoXtru(2);
    centerladdersupportpieceshape[i]->DefinePolygon(indexedge[i]+3,
												xcenterlowervertex[i],ycenterlowervertex[i]);
	centerladdersupportpieceshape[i]->DefineSection(0,0.0);
    centerladdersupportpieceshape[i]->DefineSection(1,fgkMountingBlockSupportWidth[1]
												  -fgkMountingBlockSupportWidth[0]);
    centerladdersupportpiece[i] = new TGeoVolume(centerladdersupportpiecename,
								          centerladdersupportpieceshape[i],
										  fSSDCarbonFiberMedium);
	centerladdersupportpiece[i]->SetLineColor(fColorAl);
  }
  /////////////////////////////////////////////////////////////
  // Building the Up Structure of Ladder Supports 
  /////////////////////////////////////////////////////////////
  TGeoBBox** mountingblocksupportboxupshape[fgklayernumber];	
  for(Int_t i=0; i<fgklayernumber; i++) mountingblocksupportboxupshape[i] = new TGeoBBox*[2];
  TGeoBBox** mountingblocksupportboxdownshape[fgklayernumber];	
  for(Int_t i=0; i<fgklayernumber; i++) mountingblocksupportboxdownshape[i] = new TGeoBBox*[2];
  TGeoVolume** mountingblocksupportboxdown[fgklayernumber];
  //////////////////////////////////////////////////////////
  // Setting the volume for TGeoXtru Mounting Block Piece  
  //////////////////////////////////////////////////////////
  TGeoVolume** mountingblocksupportboxup[fgklayernumber];
  TGeoXtru* mountingblockpiecedownshape[fgklayernumber];
  TGeoVolume* mountingblockpiecedown[fgklayernumber];
  TGeoXtru* mountingblockpieceupshape[fgklayernumber];
  TGeoVolume* mountingblockpieceup[fgklayernumber];
  Double_t mountingblockpieceupxvertex[fgklayernumber][8];
  Double_t mountingblockpieceupyvertex[fgklayernumber][8];
  Double_t mountingblockpiecedownxvertex[fgklayernumber][8];
  Double_t mountingblockpiecedownyvertex[fgklayernumber][8];
  char mountingblockpiecedownname[34];
  char mountingblockpieceupname[34];
  for(Int_t i=0; i<fgklayernumber; i++){
    ///////////////////////////
    // Mounting Block Down Vertex
    ///////////////////////////
  	mountingblockpiecedownshape[i] = new TGeoXtru(2);
    sprintf(mountingblockpiecedownname,"MountingBlockPieceDownLay%d",i+5);
	mountingblockpiecedownxvertex[i][0] = -0.5*fgkSSDMountingBlockLength[0];	
	mountingblockpiecedownyvertex[i][0] = fgkMountingBlockSupportRadius[i]
										+ fgkMountingBlockSupportDownHeight;
	mountingblockpiecedownxvertex[i][1] = mountingblockpiecedownxvertex[i][0];	
	mountingblockpiecedownyvertex[i][1] = mountingblockpiecedownyvertex[i][0]
										+ fgkSSDMountingBlockHeight[1]
										- 0.5*fgkCoolingTubeSupportHeight
										- fgkSSDModuleCoolingBlockToSensor
										- 2.*fgkSSDModuleVerticalDisalignment;
	mountingblockpiecedownxvertex[i][2] = 0.5*fgkSSDMountingBlockLength[0];	
	mountingblockpiecedownyvertex[i][2] = mountingblockpiecedownyvertex[i][1];
	mountingblockpiecedownxvertex[i][3] = mountingblockpiecedownxvertex[i][2];	
	mountingblockpiecedownyvertex[i][3] = mountingblockpiecedownyvertex[i][0];
	mountingblockpiecedownxvertex[i][4] = 0.5*fgkSSDMountingBlockLength[1];
	mountingblockpiecedownyvertex[i][4] = mountingblockpiecedownyvertex[i][0];
	mountingblockpiecedownxvertex[i][5] = mountingblockpiecedownxvertex[i][4];
	mountingblockpiecedownyvertex[i][5] = mountingblockpiecedownyvertex[i][4]
										+ fgkSSDMountingBlockHeight[2]
										- fgkSSDMountingBlockHeight[0];
	mountingblockpiecedownxvertex[i][6] = -mountingblockpiecedownxvertex[i][4];
	mountingblockpiecedownyvertex[i][6] = mountingblockpiecedownyvertex[i][5];
	mountingblockpiecedownxvertex[i][7] = mountingblockpiecedownxvertex[i][6];	
	mountingblockpiecedownyvertex[i][7] = mountingblockpiecedownyvertex[i][0];
	mountingblockpiecedownshape[i]->DefinePolygon(8,mountingblockpiecedownxvertex[i],
													 mountingblockpiecedownyvertex[i]);
	mountingblockpiecedownshape[i]->DefineSection(0,0.0);
	mountingblockpiecedownshape[i]->DefineSection(1,fgkMountingBlockSupportWidth[1]);
	mountingblockpiecedown[i] = new TGeoVolume(mountingblockpiecedownname,
								 mountingblockpiecedownshape[i],fSSDMountingBlockMedium);
	mountingblockpiecedown[i]->SetLineColor(fColorG10);
    ///////////////////////////
    // Mounting Block Up Vertex
    ///////////////////////////
	mountingblockpieceupshape[i] = new TGeoXtru(2);
	sprintf(mountingblockpieceupname,"MountingBlockPieceUpLay%d",i+5);
	mountingblockpieceupxvertex[i][0] = -0.5*fgkSSDMountingBlockLength[0];	
	mountingblockpieceupyvertex[i][0] = fgkMountingBlockSupportRadius[i]
										+ fgkMountingBlockSupportUpHeight[i];
	mountingblockpieceupxvertex[i][1] = mountingblockpieceupxvertex[i][0];	
	mountingblockpieceupyvertex[i][1] = mountingblockpieceupyvertex[i][0]
										+ fgkSSDMountingBlockHeight[1]
										- 0.5*fgkCoolingTubeSupportHeight
										- fgkSSDModuleCoolingBlockToSensor
										- 2.*fgkSSDModuleVerticalDisalignment;
	mountingblockpieceupxvertex[i][2] = 0.5*fgkSSDMountingBlockLength[0];	
	mountingblockpieceupyvertex[i][2] = mountingblockpieceupyvertex[i][1];
	mountingblockpieceupxvertex[i][3] = mountingblockpieceupxvertex[i][2];	
	mountingblockpieceupyvertex[i][3] = mountingblockpieceupyvertex[i][0];
	mountingblockpieceupxvertex[i][4] = 0.5*fgkSSDMountingBlockLength[1];
	mountingblockpieceupyvertex[i][4] = mountingblockpieceupyvertex[i][0];
	mountingblockpieceupxvertex[i][5] = mountingblockpieceupxvertex[i][4];
	mountingblockpieceupyvertex[i][5] = mountingblockpieceupyvertex[i][4]
										+ fgkSSDMountingBlockHeight[2]
										- fgkSSDMountingBlockHeight[0];
	mountingblockpieceupxvertex[i][6] = -mountingblockpieceupxvertex[i][4];
	mountingblockpieceupyvertex[i][6] = mountingblockpieceupyvertex[i][5];
	mountingblockpieceupxvertex[i][7] = mountingblockpieceupxvertex[i][6];	
	mountingblockpieceupyvertex[i][7] = mountingblockpieceupyvertex[i][0];
	mountingblockpieceupshape[i]->DefinePolygon(8,mountingblockpieceupxvertex[i],
													 mountingblockpieceupyvertex[i]);
	mountingblockpieceupshape[i]->DefineSection(0,0.0);
	mountingblockpieceupshape[i]->DefineSection(1,fgkMountingBlockSupportWidth[1]);
	mountingblockpieceup[i] = new TGeoVolume(mountingblockpieceupname,
								mountingblockpieceupshape[i],fSSDMountingBlockMedium);
	mountingblockpieceup[i]->SetLineColor(fColorG10);
 }
  ///////////////////////////////////////////////////////////////////
  // Setting the volume for TGeoXtru Mounting Block Support Trapezoid  
  ///////////////////////////////////////////////////////////////////
  TGeoXtru* mountingblocksupportrapezoidownshape[fgklayernumber];
  TGeoXtru* mountingblocksupportrapezoidupshape[fgklayernumber];
  TGeoVolume* mountingblocksupportrapezoidown[fgklayernumber];
  TGeoVolume* mountingblocksupportrapezoidup[fgklayernumber];
  Double_t mountingblocksupportrapezoidownxvertex[fgklayernumber][5];
  Double_t mountingblocksupportrapezoidownyvertex[fgklayernumber][5];
  Double_t mountingblocksupportrapezoidupxvertex[fgklayernumber][5];
  Double_t mountingblocksupportrapezoidupyvertex[fgklayernumber][5];
  char mountingblocksupportrapezoidowname[40];
  char mountingblocksupportrapezoidupname[40];
  Double_t scalefactor = 3./4.;
  for(Int_t i=0; i<fgklayernumber; i++){
  ////////////////////////////////////////////
  // Mounting Block Support Down Trapezoid Vertex 
  ////////////////////////////////////////////
	mountingblocksupportrapezoidownshape[i] = new TGeoXtru(2);
	mountingblocksupportrapezoidownxvertex[i][0] = -0.5*fgkSSDMountingBlockLength[0]
												 - mountingsupportedge[i];
	mountingblocksupportrapezoidownyvertex[i][0] = mountingblockpiecedownyvertex[i][0];
	mountingblocksupportrapezoidownxvertex[i][1] = 
										mountingblocksupportrapezoidownxvertex[i][0];
	mountingblocksupportrapezoidownyvertex[i][1] = mountingblockpiecedownyvertex[i][0]
												 + scalefactor*(mountingblockpiecedownyvertex[i][1]
											     - mountingblockpiecedownyvertex[i][0]);
	mountingblocksupportrapezoidownxvertex[i][2] = -0.5*fgkSSDSensorWidth;
	mountingblocksupportrapezoidownyvertex[i][2] = mountingblockpiecedownyvertex[i][1]; 
	mountingblocksupportrapezoidownxvertex[i][3] = mountingblockpiecedownxvertex[i][0];
	mountingblocksupportrapezoidownyvertex[i][3] = mountingblocksupportrapezoidownyvertex[i][2];
	mountingblocksupportrapezoidownxvertex[i][4] = mountingblocksupportrapezoidownxvertex[i][3];
	mountingblocksupportrapezoidownyvertex[i][4] = mountingblocksupportrapezoidownyvertex[i][0];
	mountingblocksupportrapezoidownshape[i]->DefinePolygon(5,mountingblocksupportrapezoidownxvertex[i],
	 												       mountingblocksupportrapezoidownyvertex[i]);
	mountingblocksupportrapezoidownshape[i]->DefineSection(0,fgkMountingBlockSupportWidth[1]
															-fgkMountingBlockSupportWidth[0]);
	mountingblocksupportrapezoidownshape[i]->DefineSection(1,fgkMountingBlockSupportWidth[1]);
	sprintf(mountingblocksupportrapezoidowname,"MountingBlockSuppTrapezoidDownLay%d",i+5);
	mountingblocksupportrapezoidown[i] = new TGeoVolume(mountingblocksupportrapezoidowname,
								mountingblocksupportrapezoidownshape[i],fSSDCarbonFiberMedium);
	mountingblocksupportrapezoidown[i]->SetLineColor(9);
  ////////////////////////////////////////////
  // Mounting Block Support Up Trapezoid Vertex 
  ////////////////////////////////////////////
	mountingblocksupportrapezoidupshape[i] = new TGeoXtru(2);
	mountingblocksupportrapezoidupxvertex[i][0] = -0.5*fgkSSDMountingBlockLength[0]
												 - mountingsupportedge[i];
	mountingblocksupportrapezoidupyvertex[i][0] = mountingblockpieceupyvertex[i][0];
	mountingblocksupportrapezoidupxvertex[i][1] = 
										mountingblocksupportrapezoidupxvertex[i][0];
	mountingblocksupportrapezoidupyvertex[i][1] = 
											       mountingblockpieceupyvertex[i][0]
												 + scalefactor*(mountingblockpieceupyvertex[i][1]
											     - mountingblockpieceupyvertex[i][0]);
	mountingblocksupportrapezoidupxvertex[i][2] = -0.5*fgkSSDSensorWidth;
	mountingblocksupportrapezoidupyvertex[i][2] = mountingblockpieceupyvertex[i][1]; 
	mountingblocksupportrapezoidupxvertex[i][3] = mountingblockpieceupxvertex[i][0];
	mountingblocksupportrapezoidupyvertex[i][3] = mountingblocksupportrapezoidupyvertex[i][2];
	mountingblocksupportrapezoidupxvertex[i][4] = mountingblocksupportrapezoidupxvertex[i][3];
	mountingblocksupportrapezoidupyvertex[i][4] = mountingblocksupportrapezoidupyvertex[i][0];
	mountingblocksupportrapezoidupshape[i]->DefinePolygon(5,mountingblocksupportrapezoidupxvertex[i],
	 												       mountingblocksupportrapezoidupyvertex[i]);
	mountingblocksupportrapezoidupshape[i]->DefineSection(0,fgkMountingBlockSupportWidth[1]
															-fgkMountingBlockSupportWidth[0]);
	mountingblocksupportrapezoidupshape[i]->DefineSection(1,fgkMountingBlockSupportWidth[1]);
	sprintf(mountingblocksupportrapezoidupname,"MountingBlockSuppTrapezoidUpLay%d",i+5);
	mountingblocksupportrapezoidup[i] = new TGeoVolume(mountingblocksupportrapezoidupname,
								mountingblocksupportrapezoidupshape[i],fSSDCarbonFiberMedium);
	mountingblocksupportrapezoidup[i]->SetLineColor(9);
  }
  ///////////////////////////////////////////////////////////////////
  for(Int_t i=0; i<fgklayernumber; i++) mountingblocksupportboxdown[i] = new TGeoVolume*[3];
  for(Int_t i=0; i<fgklayernumber; i++) mountingblocksupportboxup[i] = new TGeoVolume*[3];
  Double_t boxoriginup[fgklayernumber][2][3];
  Double_t boxorigindown[fgklayernumber][2][3];
  char mountingblocksupportboxdownname[34];
  char mountingblocksupportboxupname[34];
  TGeoRotation* mountingblocksupportrot = new TGeoRotation();
  mountingblocksupportrot->SetAngles(90.,180.,-90);
  TGeoRotation* globalrefladdersupportrot = new TGeoRotation();
  globalrefladdersupportrot->SetAngles(0.,90.,0.);
  TGeoHMatrix* laddersupportmatrix[2];
  laddersupportmatrix[0] = new TGeoHMatrix(*globalrefladdersupportrot);
  laddersupportmatrix[1] = new TGeoHMatrix((*globalrefladdersupportrot)*(*mountingblocksupportrot));
  /////////////////////////////////////////////////////////////
  // Creating Mother Volume for Containment
  /////////////////////////////////////////////////////////////
  Double_t *xmothervertex[fgklayernumber];
  Double_t *ymothervertex[fgklayernumber];
  for(Int_t i=0; i<fgklayernumber; i++){
	xmothervertex[i] = new Double_t[8];
	ymothervertex[i] = new Double_t[8];
  }  
  TGeoXtru* downmotherladdersupportshape[fgklayernumber];
  TGeoVolume* downmotherladdersupport[fgklayernumber]; 
  TGeoXtru* upmotherladdersupportshape[fgklayernumber];
//  TGeoVolume* upmotherladdersupport[fgklayernumber]; 
  TGeoVolumeAssembly* upmotherladdersupport[fgklayernumber];   
  char upmotheladdersupportname[30];
  char downmotheladdersupportname[30];
  for(Int_t i=0; i<fgklayernumber; i++){
	xmothervertex[i][0] = -0.5*fgkSSDMountingBlockLength[0]
						    -  mountingsupportedge[i];
	ymothervertex[i][0] = -fgkMountingBlockSupportWidth[1];
	xmothervertex[i][1] = xmothervertex[i][0];
	ymothervertex[i][1] = -fgkMountingBlockSupportWidth[1]
							+ fgkMountingBlockSupportWidth[0];
	xmothervertex[i][2] = -0.5*fgkSSDMountingBlockLength[0];
	ymothervertex[i][2] = ymothervertex[i][1];
	xmothervertex[i][3] = xmothervertex[i][2];
	ymothervertex[i][3] = -ymothervertex[i][0];
	xmothervertex[i][4] = -xmothervertex[i][0];
	ymothervertex[i][4] = ymothervertex[i][3];
	xmothervertex[i][5] = xmothervertex[i][4];
	ymothervertex[i][5] = -ymothervertex[i][1];
	xmothervertex[i][6] = -xmothervertex[i][2];
	ymothervertex[i][6] = ymothervertex[i][5];
	xmothervertex[i][7] = xmothervertex[i][6];
	ymothervertex[i][7] = ymothervertex[i][0];
	sprintf(downmotheladdersupportname,"LadderSupportDownLay%d",i+5);
	sprintf(upmotheladdersupportname,"LadderSupportUpLay%d",i+5);
    downmotherladdersupportshape[i] = new TGeoXtru(2);
	downmotherladdersupportshape[i]->DefinePolygon(8,xmothervertex[i],ymothervertex[i]);
	downmotherladdersupportshape[i]->DefineSection(0,ysidevertex[i][0]);
    downmotherladdersupportshape[i]->DefineSection(1,ysidevertex[i][1]
								   +			   fgkMountingBlockSupportDownHeight
								   +			   fgkSSDMountingBlockHeight[1]
								   -			   0.5*fgkCoolingTubeSupportHeight
								   -			   fgkSSDModuleCoolingBlockToSensor
								   -			   2.*fgkSSDModuleVerticalDisalignment);
    downmotherladdersupport[i] = new TGeoVolume(downmotheladdersupportname,
								          downmotherladdersupportshape[i],fSSDAir);
    upmotherladdersupportshape[i] = new TGeoXtru(2);
	upmotherladdersupportshape[i]->DefinePolygon(8,xmothervertex[i],ymothervertex[i]);
	upmotherladdersupportshape[i]->DefineSection(0,ysidevertex[i][0]);
    upmotherladdersupportshape[i]->DefineSection(1,ysidevertex[i][1]
								   +			   fgkMountingBlockSupportUpHeight[i]
								   +			   fgkSSDMountingBlockHeight[1]
								   -			   0.5*fgkCoolingTubeSupportHeight
								   -			   fgkSSDModuleCoolingBlockToSensor
								   -               2.*fgkSSDModuleVerticalDisalignment);
	upmotherladdersupport[i] = new TGeoVolumeAssembly(upmotheladdersupportname);
//    upmotherladdersupport[i] = new TGeoVolume(upmotheladdersupportname,
//											  upmotherladdersupportshape[i],fSSDAir);
  }
  for(Int_t i=0; i<fgklayernumber; i++){
	/////////////////////////
	// Setting the box origin
	/////////////////////////
	boxorigindown[i][0][0] = -0.5*mountingsupportedge[i];
	boxorigindown[i][0][1] =  fgkMountingBlockSupportRadius[i]
						   +  0.5*fgkMountingBlockSupportDownHeight;
	boxorigindown[i][0][2] =  fgkMountingBlockSupportWidth[1]
						   -  0.5*fgkMountingBlockSupportWidth[0];
  
	boxorigindown[i][1][0] = 0.0;
	boxorigindown[i][1][1] = boxorigindown[i][0][1];
	boxorigindown[i][1][2] = 0.5*(fgkMountingBlockSupportWidth[1]
						   -	  fgkMountingBlockSupportWidth[0]);
						   
	boxoriginup[i][0][0] = -0.5*mountingsupportedge[i];
	boxoriginup[i][0][1] = fgkMountingBlockSupportRadius[i]
						 + 0.5*fgkMountingBlockSupportUpHeight[i];
	boxoriginup[i][0][2] = fgkMountingBlockSupportWidth[1]
						 - 0.5*fgkMountingBlockSupportWidth[0];
  
	boxoriginup[i][1][0] = 0.0;
	boxoriginup[i][1][1] = fgkMountingBlockSupportRadius[i]
						 + 0.5*fgkMountingBlockSupportUpHeight[i];
	boxoriginup[i][1][2] = 0.5*(fgkMountingBlockSupportWidth[1]
						 - fgkMountingBlockSupportWidth[0]);
  
	/////////////////////////
    // Setting the boxes    
	/////////////////////////
	mountingblocksupportboxdownshape[i][0] = new TGeoBBox(0.5*(mountingsupportedge[i]
										 +  fgkSSDMountingBlockLength[0]),
											0.5*fgkMountingBlockSupportDownHeight,
											0.5*fgkMountingBlockSupportWidth[0],
											boxorigindown[i][0]);
    mountingblocksupportboxdownshape[i][1] = new TGeoBBox(0.5*fgkSSDMountingBlockLength[0],
											0.5*fgkMountingBlockSupportDownHeight,
											0.5*(fgkMountingBlockSupportWidth[1]
										 -  fgkMountingBlockSupportWidth[0]),
											boxorigindown[i][1]);
											
	mountingblocksupportboxupshape[i][0] = new TGeoBBox(0.5*(mountingsupportedge[i]
										 +  fgkSSDMountingBlockLength[0]),
											0.5*fgkMountingBlockSupportUpHeight[i],
											0.5*fgkMountingBlockSupportWidth[0],
											boxoriginup[i][0]);
											
	mountingblocksupportboxupshape[i][1] = new TGeoBBox(0.5*fgkSSDMountingBlockLength[0],
											0.5*fgkMountingBlockSupportUpHeight[i],
											0.5*(fgkMountingBlockSupportWidth[1]
							             -	fgkMountingBlockSupportWidth[0]),
											boxoriginup[i][1]);
	///////////////////////////////////////
    // Adding the Volumes to Mother Volume    
	///////////////////////////////////////
	for(Int_t j=0; j<2; j++){
		sprintf(mountingblocksupportboxdownname,"MountingBlockSuppDownLay%dBox%d",i+5,j+1);
		sprintf(mountingblocksupportboxupname,"MountingBlockSuppUpLay%dBox%d",i+5,j+1);
		mountingblocksupportboxdown[i][j] = new TGeoVolume(mountingblocksupportboxdownname,
										  mountingblocksupportboxdownshape[i][j],
										  fSSDCarbonFiberMedium);
		mountingblocksupportboxup[i][j] = new TGeoVolume(mountingblocksupportboxupname,
										  mountingblocksupportboxupshape[i][j],
										  fSSDCarbonFiberMedium);
		mountingblocksupportboxdown[i][j]->SetLineColor(9);
		mountingblocksupportboxup[i][j]->SetLineColor(9);
		for(Int_t k=0; k<2; k++){
			downmotherladdersupport[i]->AddNode(mountingblocksupportboxdown[i][j],k+1,laddersupportmatrix[k]);
//			upmotherladdersupport[i]->AddNode(mountingblocksupportboxup[i][j],k+1,laddersupportmatrix[k]);
		}
	}
	for(Int_t k=0; k<2; k++){
		downmotherladdersupport[i]->AddNode(centermountingblocksupport[i],k+1,laddersupportmatrix[k]);
		downmotherladdersupport[i]->AddNode(sidemountingblocksupport[i],k+1,laddersupportmatrix[k]);
		downmotherladdersupport[i]->AddNode(sideladdersupportpiece[i],k+1,laddersupportmatrix[k]);
		downmotherladdersupport[i]->AddNode(centerladdersupportpiece[i],k+1,laddersupportmatrix[k]);
	    downmotherladdersupport[i]->AddNode(mountingblockpiecedown[i],k+1,laddersupportmatrix[k]);
		downmotherladdersupport[i]->AddNode(mountingblocksupportrapezoidown[i],k+1,laddersupportmatrix[k]);
		upmotherladdersupport[i]->AddNode(centermountingblocksupport[i],k+1,laddersupportmatrix[k]);
//		upmotherladdersupport[i]->AddNode(sidemountingblocksupport[i],k+1,laddersupportmatrix[k]);
//		upmotherladdersupport[i]->AddNode(sideladdersupportpiece[i],k+1,laddersupportmatrix[k]);
//		upmotherladdersupport[i]->AddNode(centerladdersupportpiece[i],k+1,laddersupportmatrix[k]);
//		upmotherladdersupport[i]->AddNode(mountingblockpieceup[i],k+1,laddersupportmatrix[k]);
//		upmotherladdersupport[i]->AddNode(mountingblocksupportrapezoidup[i],k+1,laddersupportmatrix[k]);
	}
  }
  TList* laddersupportlist = new TList();
  laddersupportlist->Add(downmotherladdersupport[0]); 
  laddersupportlist->Add(upmotherladdersupport[0]); 
  laddersupportlist->Add(downmotherladdersupport[1]); 
  laddersupportlist->Add(upmotherladdersupport[1]); 
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<fgklayernumber; i++){
	for(Int_t j=0; j<nedges+1; j++)
		delete vertex[i][j];
	delete mountingsupportedgevector[i];
	delete [] vertex[i];
	delete vertexlist[i];
	delete [] xsidevertex[i];
	delete [] ysidevertex[i];
	delete [] xcentervertex[i];
	delete [] ycentervertex[i];
	delete [] xsidelowervertex[i];
	delete [] ysidelowervertex[i];
	delete [] xcenterlowervertex[i];
	delete [] ycenterlowervertex[i];
  }
  delete xsidevertex;
  delete ysidevertex;
  delete xcentervertex;
  delete ycentervertex;
  delete xsidelowervertex;
  delete ysidelowervertex;
  delete xcenterlowervertex;
  delete ycenterlowervertex;
  delete globalrefladdersupportrot;
  delete mountingblocksupportrot;
  /////////////////////
  return laddersupportlist;	
}
 ////////////////////////////////////////////////////////////////////////////////
void AliITSv11GeometrySSD::SetLadderSupport(Int_t nedges){ 
//////////////////////////////////////////
// Method Generating Ladder Support Ring
//////////////////////////////////////////
  if(!fCreateMaterials) CreateMaterials();
  if(!fTransformationMatrices) CreateTransformationMatrices();
  if(!fBasicObjects) CreateBasicObjects();
  fLay5LadderSupportRing = new TGeoVolumeAssembly("Lay5LadderSupportRing");
  fLay6LadderSupportRing = new TGeoVolumeAssembly("Lay6LadderSupportRing");
    const Int_t kssdlayladdernumber[fgklayernumber] = 
			{fgkSSDLay5LadderNumber,fgkSSDLay6LadderNumber};
  Double_t mountingsupportedge[fgklayernumber];
  Double_t mountingblockratio[fgklayernumber];
  Double_t theta[fgklayernumber];
  Double_t phi[fgklayernumber];
  for(Int_t i=0; i<fgklayernumber; i++){
	mountingblockratio[i] = fgkSSDMountingBlockLength[0]/fgkMountingBlockSupportRadius[i];
    mountingsupportedge[i]    = 0.5*fgkMountingBlockSupportRadius[i]
							  *(TMath::Sqrt(4.0-TMath::Power(mountingblockratio[i],2))
							  * TMath::Sin(2.0*TMath::Pi()/kssdlayladdernumber[i])
							  - mountingblockratio[i]*(1.0+TMath::Cos(2.0*TMath::Pi()
							  / kssdlayladdernumber[i])));
    theta[i] = TMath::ASin(0.5*mountingblockratio[i]+mountingsupportedge[i]
			 / fgkMountingBlockSupportRadius[i]);
    phi[i]   = TMath::ASin(0.5*mountingblockratio[i]);
  }
  TGeoRotation* globalrot = new TGeoRotation();
  globalrot->SetAngles(0.,-90.,0.); 
  TGeoRotation** laddersupportrot[fgklayernumber];
  TGeoHMatrix**  laddersupportmatrix[fgklayernumber];
  for(Int_t i=0; i<fgklayernumber; i++){		
	laddersupportrot[i] = new TGeoRotation*[kssdlayladdernumber[i]];
	laddersupportmatrix[i] = new TGeoHMatrix*[kssdlayladdernumber[i]];
	for(Int_t j=0; j<kssdlayladdernumber[i]; j++){
		laddersupportrot[i][j] = new TGeoRotation();
		laddersupportrot[i][j]->SetAngles(j*(phi[i]+theta[i])*TMath::RadToDeg(),0.,0.);
		switch(i){
			case 0: //Ladder of Layer5  
				laddersupportmatrix[i][j] = new TGeoHMatrix((*laddersupportrot[i][j])*(*globalrot));
				fLay5LadderSupportRing->AddNode(j%2==0?fLay5LadderSupport[0]:fLay5LadderSupport[1],j+1,
									    laddersupportmatrix[i][j]); 
			break;
			case 1: //Ladder of Layer6 
				laddersupportmatrix[i][j] = new TGeoHMatrix((*laddersupportrot[i][j])*(*globalrot));
				fLay6LadderSupportRing->AddNode(j%2==0?fLay6LadderSupport[0]:fLay6LadderSupport[1],j+1,
									      laddersupportmatrix[i][j]); 
			break;
		}
    }
  }
  /////////////////////////////////////////////////////////////
  // Creating Lower Ladder Support 
  /////////////////////////////////////////////////////////////
  TVector3** ringsupportvertex[fgklayernumber]; 	
  Double_t angle = 360./nedges;
  for(Int_t i=0; i<fgklayernumber; i++){
    ringsupportvertex[i] = new TVector3*[2*kssdlayladdernumber[i]+3+nedges];	
	ringsupportvertex[i][0] = new TVector3(0.,fgkMountingBlockSupportRadius[i]
							*			   TMath::Cos(theta[i]));
	ringsupportvertex[i][1] = new TVector3(-0.5*fgkSSDMountingBlockLength[0]
							-			   mountingsupportedge[i],
										   ringsupportvertex[i][0]->Y());
	ringsupportvertex[i][2] = new TVector3(0.5*fgkSSDMountingBlockLength[0],
										   ringsupportvertex[i][1]->Y()); 									   	
    ringsupportvertex[i][2]->RotateZ(theta[i]+phi[i]);
	for(Int_t j=1; j<kssdlayladdernumber[i]; j++){
	   ringsupportvertex[i][2*j+1] = new TVector3(*ringsupportvertex[i][1]);  	
	   ringsupportvertex[i][2*j+1]->RotateZ(j*(theta[i]+phi[i]));	
	   ringsupportvertex[i][2*j+2] = new TVector3(*ringsupportvertex[i][2]);  	
	   ringsupportvertex[i][2*j+2]->RotateZ(j*(theta[i]+phi[i]));	
	}
	ringsupportvertex[i][2*kssdlayladdernumber[i]+1] = new TVector3(*ringsupportvertex[i][0]);
    for(Int_t j=0; j<nedges+1; j++){
		ringsupportvertex[i][2*kssdlayladdernumber[i]+2+j] = 
			new TVector3((ringsupportvertex[i][0]->Y()-fgkLadderSupportHeigth)*CosD(90.0-j*angle),
						 (ringsupportvertex[i][0]->Y()-fgkLadderSupportHeigth)*SinD(90.0-j*angle));
	}
  }
  Double_t **xmothervertex = new Double_t*[fgklayernumber];
  Double_t **ymothervertex = new Double_t*[fgklayernumber];
  for(Int_t i=0; i<fgklayernumber; i++){
	xmothervertex[i] = new Double_t[2*kssdlayladdernumber[i]+3+nedges];
	ymothervertex[i] = new Double_t[2*kssdlayladdernumber[i]+3+nedges];
	for(Int_t j=0; j<2*kssdlayladdernumber[i]+3+nedges; j++){
		xmothervertex[i][j] = ringsupportvertex[i][j]->X();
		ymothervertex[i][j] = ringsupportvertex[i][j]->Y();
	}
  }
////////////////////////////////////////////////////////////////////////////////
// Start Corrections 13/06/08
////////////////////////////////////////////////////////////////////////////////
  char lowerladderpconsupportname[30];
  TGeoPcon* lowerladderpconsupportshape[fgklayernumber];
  TGeoVolume* lowerladderpconsupport[fgklayernumber]; 
  Double_t lowerladderpconezsection[2] = {0.,fgkMountingBlockSupportWidth[1]};
  Double_t lowerladderpconradiusmax[fgklayernumber];
  Double_t lowerladderpconradiusmin[fgklayernumber];
  TGeoRotation* lowerladdersupportrot = new TGeoRotation();
  lowerladdersupportrot->SetAngles(90.,180.,-90);
  for(Int_t i=0; i<fgklayernumber; i++){
	lowerladderpconradiusmax[i] = fgkMountingBlockSupportRadius[i]
								*			   TMath::Cos(theta[i]);
    lowerladderpconradiusmin[i] = lowerladderpconradiusmax[i]-fgkLadderSupportHeigth;
  } 
  for(Int_t i=0; i<fgklayernumber; i++){
///////////////////////////  Modified Version ?///////////////////
    lowerladderpconsupportshape[i] = new TGeoPcon(0.,360.,2);
	for(Int_t j=0; j<2; j++) lowerladderpconsupportshape[i]->DefineSection(j,
							 lowerladderpconezsection[j],lowerladderpconradiusmin[i],
							 lowerladderpconradiusmax[i]);
	sprintf(lowerladderpconsupportname,"LowerLadderPConSupportNameLay%d",i+5);
	lowerladderpconsupport[i] = new TGeoVolume(lowerladderpconsupportname,lowerladderpconsupportshape[i],fSSDSupportRingAl);
    lowerladderpconsupport[i]->SetLineColor(fColorAl);
	(i==0 ? fLay5LadderSupportRing: fLay6LadderSupportRing)->AddNode(lowerladderpconsupport[i],1);
	(i==0 ? fLay5LadderSupportRing: fLay6LadderSupportRing)->AddNode(lowerladderpconsupport[i],2,lowerladdersupportrot);
 }
////////////////////////////////////////////////////////////////////////////////
// End Corrections 13/06/08
////////////////////////////////////////////////////////////////////////////////
  /*char lowerladdersupportname[30];
  TGeoXtru* lowerladdersupportshape[fgklayernumber];
  TGeoVolume* lowerladdersupport[fgklayernumber];
  TGeoRotation* lowerladdersupportrot = new TGeoRotation();
  lowerladdersupportrot->SetAngles(90.,180.,-90);
  for(Int_t i=0; i<fgklayernumber; i++){
	lowerladdersupportshape[i] = new TGeoXtru(2);
	lowerladdersupportshape[i]->DefinePolygon(2*kssdlayladdernumber[i]+3+nedges,
											  xmothervertex[i],ymothervertex[i]);
	lowerladdersupportshape[i]->DefineSection(0,0.);
    lowerladdersupportshape[i]->DefineSection(1,fgkMountingBlockSupportWidth[1]);
	sprintf(lowerladdersupportname,"LowerLadderSupportNameLay%d",i+5);
    lowerladdersupport[i] = new TGeoVolume(lowerladdersupportname,
							lowerladdersupportshape[i],fSSDSupportRingAl);
	lowerladdersupport[i]->SetLineColor(fColorAl);
	(i==0 ? fLay5LadderSupportRing: fLay6LadderSupportRing)->AddNode(lowerladdersupport[i],1);
	(i==0 ? fLay5LadderSupportRing: fLay6LadderSupportRing)->AddNode(lowerladdersupport[i],2,lowerladdersupportrot);
  }*/
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<fgklayernumber; i++){
	for(Int_t j=0; j<2*kssdlayladdernumber[i]+3+nedges; j++)
		delete ringsupportvertex[i][j];
	delete [] ringsupportvertex[i];
  }
  for(Int_t i=0; i<fgklayernumber; i++){
	delete [] xmothervertex[i];
	delete [] ymothervertex[i];
  }
  delete xmothervertex;
  delete ymothervertex; 
  delete globalrot;
  for(Int_t i=0; i<fgklayernumber; i++){
	for(Int_t j=0; j<kssdlayladdernumber[i]; j++)
		delete laddersupportrot[i][j];
	delete [] laddersupportrot[i];
  }
 }  
 ////////////////////////////////////////////////////////////////////////////////
 TGeoVolume* AliITSv11GeometrySSD::GetEndCapCoverPlate(){
  /////////////////////////////////////////////////////////////
  // Method generating Endcap CoverPlate
  /////////////////////////////////////////////////////////////
  // Holes Definition 
  ///////////////////
  Int_t nendcapcoverplateholedges = 30;
  const Int_t kendcapcoverplatesmallholenumber[2] = {4,9}; 
  Double_t holesection[2] = {-0.5*fgkEndCapCoverPlateThickness,
							  0.5*fgkEndCapCoverPlateThickness};
  TGeoShape* endcapcoverplatesmallholeshape = GetHoleShape(fgkEndCapCoverPlateSmallHoleRadius,
													      nendcapcoverplateholedges,holesection);
  TGeoVolume* endcapcoverplatesmallhole = new TGeoVolume("EndCapCoverPlateSmallHole",
										  endcapcoverplatesmallholeshape,fSSDAlCoolBlockMedium);
  endcapcoverplatesmallhole->SetLineColor(6);
  TGeoShape* endcapcoverplatebigholeshape = GetHoleShape(fgkEndCapCoverPlateBigHoleRadius,
													      nendcapcoverplateholedges,holesection);
  TGeoVolume* endcapcoverplatebighole = new TGeoVolume("EndCapCoverPlateBigHole",
										  endcapcoverplatebigholeshape,fSSDAlCoolBlockMedium);
  endcapcoverplatebighole->SetLineColor(6);
  //////////////////////////
  // Screw Piece Definition 
  //////////////////////////
  Double_t smallscrewangle = 360.0/nendcapcoverplateholedges;
  TGeoTube* endcapsmallscrewpieceshape = new TGeoTube(0.0,fgkEndCapCoverPlateSmallHoleRadius*
												      CosD(0.5*smallscrewangle),
												      0.5*fgkEndCapCoverPlateThickness);
  TGeoVolume* endcapsmallscrewpiece = new TGeoVolume("EndCapCoverPlateSmallScrewPiece",
												endcapsmallscrewpieceshape,
												fSSDCoolingTubePhynox);
  endcapsmallscrewpiece->SetLineColor(fColorPhynox);
  ///////////////////
  // Box Definition 
  ///////////////////
  TGeoBBox* endcapcoverplateboxshape[4];
  TGeoVolume* endcapcoverplatebox[4];
  Double_t boxorigin[5][3];
  boxorigin[0][0] = 0.;
  boxorigin[0][1] = 0.5*fgkEndCapCoverPlateSmallHoleSeparation[2];
  boxorigin[0][2] = 0.;

  boxorigin[1][0] = 0.5*fgkEndCapCoverPlateSmallHoleSeparation[0];
  boxorigin[1][1] = 4.*fgkEndCapCoverPlateSmallHoleSeparation[2];
  boxorigin[1][2] = 0.;

  boxorigin[2][0] = 1.5*fgkEndCapCoverPlateSmallHoleSeparation[0]
				  + fgkEndCapCoverPlateSmallHoleSeparation[1];
  boxorigin[2][1] = boxorigin[1][1];
  boxorigin[2][2] = 0.;

  boxorigin[3][0] = fgkEndCapCoverPlateSmallHoleSeparation[0]
				  + 0.5*fgkEndCapCoverPlateSmallHoleSeparation[1];
  boxorigin[3][1] = boxorigin[1][1];
  boxorigin[3][2] = 0.;

  endcapcoverplateboxshape[0] = new TGeoBBox(fgkEndCapCoverPlateSmallHoleRadius,
										0.5*(fgkEndCapCoverPlateSmallHoleSeparation[2]
						 -              2.*fgkEndCapCoverPlateSmallHoleRadius),
									    0.5*fgkEndCapCoverPlateThickness,boxorigin[0]);

  endcapcoverplateboxshape[1] = new TGeoBBox(0.5*(fgkEndCapCoverPlateSmallHoleSeparation[0]
							                -2.*fgkEndCapCoverPlateSmallHoleRadius),
										     4.*fgkEndCapCoverPlateSmallHoleSeparation[2]
							  +				 fgkEndCapCoverPlateSmallHoleRadius,
									         0.5*fgkEndCapCoverPlateThickness,boxorigin[1]);

  endcapcoverplateboxshape[2] = new TGeoBBox(0.5*(fgkEndCapCoverPlateSmallHoleSeparation[0]
							                -2.*fgkEndCapCoverPlateSmallHoleRadius),
										     4.*fgkEndCapCoverPlateSmallHoleSeparation[2]
							  +				 fgkEndCapCoverPlateSmallHoleRadius,
									         0.5*fgkEndCapCoverPlateThickness,boxorigin[2]);

  endcapcoverplateboxshape[3] = new TGeoBBox(0.5*(fgkEndCapCoverPlateSmallHoleSeparation[1]
							                -2.*fgkEndCapCoverPlateSmallHoleRadius),
										     4.*fgkEndCapCoverPlateSmallHoleSeparation[2]
							  +				 fgkEndCapCoverPlateSmallHoleRadius,
									         0.5*fgkEndCapCoverPlateThickness,boxorigin[3]);
  
  endcapcoverplatebox[0] = new TGeoVolume("EndCapCoverPlateBox1",endcapcoverplateboxshape[0],
									   fSSDAlCoolBlockMedium);
  endcapcoverplatebox[1] = new TGeoVolume("EndCapCoverPlateBox2",endcapcoverplateboxshape[1],
									   fSSDAlCoolBlockMedium);
  endcapcoverplatebox[2] = new TGeoVolume("EndCapCoverPlateBox3",endcapcoverplateboxshape[2],
									   fSSDAlCoolBlockMedium);
  endcapcoverplatebox[3] = new TGeoVolume("EndCapCoverPlateBox4",endcapcoverplateboxshape[3],
									   fSSDAlCoolBlockMedium);
  endcapcoverplatebox[0]->SetLineColor(6);
  endcapcoverplatebox[1]->SetLineColor(6);
  endcapcoverplatebox[2]->SetLineColor(6);
  endcapcoverplatebox[3]->SetLineColor(6);
  Double_t endcapfillingboxorigin[3] = {fgkEndCapCoverPlateSmallHoleSeparation[0],0.,0.};
  TGeoBBox* endcapfillingboxshape = new TGeoBBox(fgkEndCapCoverPlateSmallHoleRadius,
											fgkEndCapCoverPlateSmallHoleRadius,
											0.5*fgkEndCapCoverPlateThickness,
											endcapfillingboxorigin);
  TGeoVolume* endcapfillingbox = new TGeoVolume("EndCapFillingBox",endcapfillingboxshape,
									   fSSDAlCoolBlockMedium);
  endcapfillingbox->SetLineColor(6);
  ////////////////////////////
  // Contour shape Definition 
  ////////////////////////////
  const Int_t kcontourvertexnumber = 10;
  Double_t xcontourvertex[kcontourvertexnumber];
  Double_t ycontourvertex[kcontourvertexnumber];
  xcontourvertex[0] = -fgkEndCapCoverPlateLength[0];
  xcontourvertex[1] = xcontourvertex[0];
  xcontourvertex[2] = fgkEndCapCoverPlateLength[1]-xcontourvertex[0];
  xcontourvertex[3] = xcontourvertex[2];
  xcontourvertex[4] = -fgkEndCapCoverPlateSmallHoleRadius;
  xcontourvertex[5] = xcontourvertex[4];
  xcontourvertex[6] = fgkEndCapCoverPlateLength[1]+fgkEndCapCoverPlateSmallHoleRadius;
  xcontourvertex[7] = xcontourvertex[6];
  xcontourvertex[8] = xcontourvertex[4];
  xcontourvertex[9] = xcontourvertex[8];
  ycontourvertex[0] = -0.5*(fgkEndCapCoverPlateWidth[0]-fgkEndCapCoverPlateWidth[2]
					- (kendcapcoverplatesmallholenumber[1]-1)
					* fgkEndCapCoverPlateSmallHoleSeparation[2]);
  ycontourvertex[1] = (kendcapcoverplatesmallholenumber[1]-1)
					* fgkEndCapCoverPlateSmallHoleSeparation[2]-ycontourvertex[0];
  ycontourvertex[2] = ycontourvertex[1];
  ycontourvertex[3] = ycontourvertex[0];
  ycontourvertex[4] = ycontourvertex[3];
  ycontourvertex[5] = -fgkEndCapCoverPlateSmallHoleRadius;
  ycontourvertex[6] = ycontourvertex[5];
  ycontourvertex[7] = (kendcapcoverplatesmallholenumber[1]-1)
					* fgkEndCapCoverPlateSmallHoleSeparation[2]
					+ fgkEndCapCoverPlateSmallHoleRadius;
  ycontourvertex[8] = ycontourvertex[7];
  ycontourvertex[9] = ycontourvertex[0];

  Double_t xboxin, dxboxin, yboxin, dyboxin;
  Double_t xboxout, dxboxout, yboxout, dyboxout;
  Double_t coordmin, coordmax;
  coordmin = -fgkEndCapCoverPlateLength[0];
  coordmax = fgkEndCapCoverPlateLength[1]-xcontourvertex[0];
  xboxout = 0.5*(coordmin+coordmax);
  dxboxout = 0.5*(coordmax-coordmin);
  coordmin = -0.5*(fgkEndCapCoverPlateWidth[0]-fgkEndCapCoverPlateWidth[2]
					- (kendcapcoverplatesmallholenumber[1]-1)
					* fgkEndCapCoverPlateSmallHoleSeparation[2]);
  coordmax = (kendcapcoverplatesmallholenumber[1]-1)
					* fgkEndCapCoverPlateSmallHoleSeparation[2]-ycontourvertex[0];
  yboxout = 0.5*(coordmin+coordmax);
  dyboxout = 0.5*(coordmax-coordmin);
  coordmin = -fgkEndCapCoverPlateSmallHoleRadius;
  coordmax = fgkEndCapCoverPlateLength[1]+fgkEndCapCoverPlateSmallHoleRadius;
  xboxin = 0.5*(coordmin+coordmax);
  dxboxin = 0.5*(coordmax-coordmin);
  coordmin = -fgkEndCapCoverPlateSmallHoleRadius;
  coordmax = (kendcapcoverplatesmallholenumber[1]-1)
					* fgkEndCapCoverPlateSmallHoleSeparation[2]
					+ fgkEndCapCoverPlateSmallHoleRadius;
  yboxin = 0.5*(coordmin+coordmax);
  dyboxin = 0.5*(coordmax-coordmin);
  new TGeoBBox("EndCapCoverPlateContourBoxOut", dxboxout, dyboxout, 0.5*fgkEndCapCoverPlateThickness);
  TGeoTranslation *trendCapCoverPlateContourboxout = new TGeoTranslation("SSD_trEndCapCoverPlateContourBoxOut",
                                                         xboxout, yboxout, 0.);
  trendCapCoverPlateContourboxout->RegisterYourself();
  new TGeoBBox("EndCapCoverPlateContourBoxIn",  dxboxin, dyboxin, 0.5*fgkEndCapCoverPlateThickness+0.01);
  TGeoTranslation *trendCapCoverPlateContourboxin = new TGeoTranslation("SSD_trEndCapCoverPlateContourBoxIn",
                                                         xboxin, yboxin, 0.);
  trendCapCoverPlateContourboxin->RegisterYourself();
  TGeoCompositeShape *contourshape = new TGeoCompositeShape("contourShape", 
        "EndCapCoverPlateContourBoxOut:SSD_trEndCapCoverPlateContourBoxOut-EndCapCoverPlateContourBoxIn:SSD_trEndCapCoverPlateContourBoxIn");

  TGeoVolume* contour = new TGeoVolume("EndCapCoverPlateContour",contourshape,
									   fSSDAlCoolBlockMedium);
  contour->SetLineColor(6);
  /////////////////////////////
  // Hole Contour Shape Definition 
  ////////////////////////////
  coordmin = xcontourvertex[0];
  coordmax = coordmin+fgkEndCapCoverPlateLength[2];
  xboxout = 0.5*(coordmin+coordmax);
  dxboxout = 0.5*(coordmax-coordmin);
  coordmin = ycontourvertex[1];
  coordmax = ycontourvertex[1]+fgkEndCapCoverPlateWidth[2];
  yboxout = 0.5*(coordmin+coordmax);
  dyboxout = 0.5*(coordmax-coordmin);
  coordmin = xcontourvertex[0]+ 0.5*(fgkEndCapCoverPlateLength[2]
						   - 2.*fgkEndCapCoverPlateBigHoleRadius);
  coordmax = coordmin + 2.*fgkEndCapCoverPlateBigHoleRadius;
  xboxin = 0.5*(coordmin+coordmax);
  dxboxin = 0.5*(coordmax-coordmin);
  coordmin = ycontourvertex[1]+0.5*(fgkEndCapCoverPlateWidth[2]
						   - 2.*fgkEndCapCoverPlateBigHoleRadius);;
  coordmax = coordmin +2.*fgkEndCapCoverPlateBigHoleRadius;
  yboxin = 0.5*(coordmin+coordmax);
  dyboxin = 0.5*(coordmax-coordmin);
  new TGeoBBox("EndCapCoverPlateContourBoxOut1", dxboxout, dyboxout, 0.5*fgkEndCapCoverPlateThickness);
  TGeoTranslation *trendCapCoverPlateContourboxout1 = new TGeoTranslation("SSD_trEndCapCoverPlateContourBoxOut1",
                                                         xboxout, yboxout, 0.);
  trendCapCoverPlateContourboxout1->RegisterYourself();
  new TGeoBBox("EndCapCoverPlateContourBoxIn1",  dxboxin, dyboxin, 0.5*fgkEndCapCoverPlateThickness+0.01);
  TGeoTranslation *trendCapCoverPlateContourboxin1 = new TGeoTranslation("SSD_trEndCapCoverPlateContourBoxIn1",
                                                         xboxin, yboxin, 0.);
  trendCapCoverPlateContourboxin1->RegisterYourself();
  TGeoCompositeShape *contourshape1 = new TGeoCompositeShape("contourShape1", 
        "EndCapCoverPlateContourBoxOut1:SSD_trEndCapCoverPlateContourBoxOut1-EndCapCoverPlateContourBoxIn1:SSD_trEndCapCoverPlateContourBoxIn1");


  coordmin = xcontourvertex[0]+fgkEndCapCoverPlateLength[5];
  coordmax = coordmin+fgkEndCapCoverPlateLength[2];
  xboxout = 0.5*(coordmin+coordmax);
  dxboxout = 0.5*(coordmax-coordmin);
  coordmin = ycontourvertex[0]-(fgkEndCapCoverPlateWidth[1]
						   - fgkEndCapCoverPlateWidth[0]);
  coordmax = ycontourvertex[0];
  yboxout = 0.5*(coordmin+coordmax);
  dyboxout = 0.5*(coordmax-coordmin);
  coordmin = xcontourvertex[0]+fgkEndCapCoverPlateLength[5]+ 0.5*(fgkEndCapCoverPlateLength[2]
						   - 2.*fgkEndCapCoverPlateBigHoleRadius);
  coordmax = coordmin + 2.*fgkEndCapCoverPlateBigHoleRadius;
  xboxin = 0.5*(coordmin+coordmax);
  dxboxin = 0.5*(coordmax-coordmin);
  coordmin = ycontourvertex[0]-(fgkEndCapCoverPlateWidth[1]
						   - fgkEndCapCoverPlateWidth[0])+0.5*(fgkEndCapCoverPlateWidth[1]
						   - fgkEndCapCoverPlateWidth[0]
						   - 2.*fgkEndCapCoverPlateBigHoleRadius);
  coordmax = coordmin+2.*fgkEndCapCoverPlateBigHoleRadius;
  yboxin = 0.5*(coordmin+coordmax);
  dyboxin = 0.5*(coordmax-coordmin);
  new TGeoBBox("EndCapCoverPlateContourBoxOut2", dxboxout, dyboxout, 0.5*fgkEndCapCoverPlateThickness);
  TGeoTranslation *trendCapCoverPlateContourboxout2 = new TGeoTranslation("SSD_trEndCapCoverPlateContourBoxOut2",
                                                         xboxout, yboxout, 0.);
  trendCapCoverPlateContourboxout2->RegisterYourself();
  new TGeoBBox("EndCapCoverPlateContourBoxIn2",  dxboxin, dyboxin, 0.5*fgkEndCapCoverPlateThickness+0.01);
  TGeoTranslation *trendCapCoverPlateContourboxin2 = new TGeoTranslation("SSD_trEndCapCoverPlateContourBoxIn2",
                                                         xboxin, yboxin, 0.);
  trendCapCoverPlateContourboxin2->RegisterYourself();
  TGeoCompositeShape *contourshape2 = new TGeoCompositeShape("contourShape2", 
        "EndCapCoverPlateContourBoxOut2:SSD_trEndCapCoverPlateContourBoxOut2-EndCapCoverPlateContourBoxIn2:SSD_trEndCapCoverPlateContourBoxIn2");
  
//  const Int_t kholecontourvertexnumber = 10;

  Double_t xholecontourvertex[2][kcontourvertexnumber];
  Double_t yholecontourvertex[2][kcontourvertexnumber];
  xholecontourvertex[0][0] = xcontourvertex[0];
  xholecontourvertex[0][1] = xholecontourvertex[0][0];
  xholecontourvertex[0][2] = xholecontourvertex[0][1]+fgkEndCapCoverPlateLength[2];
  xholecontourvertex[0][3] = xholecontourvertex[0][2];
  xholecontourvertex[0][4] = xholecontourvertex[0][0]
						   + 0.5*(fgkEndCapCoverPlateLength[2]
						   - 2.*fgkEndCapCoverPlateBigHoleRadius);
  xholecontourvertex[0][5] = xholecontourvertex[0][4];
  xholecontourvertex[0][6] = xholecontourvertex[0][5]
						   + 2.*fgkEndCapCoverPlateBigHoleRadius;
  xholecontourvertex[0][7] = xholecontourvertex[0][6];
  xholecontourvertex[0][8] = xholecontourvertex[0][4];
  xholecontourvertex[0][9] = xholecontourvertex[0][8];
  
  yholecontourvertex[0][0] = ycontourvertex[1];
  yholecontourvertex[0][1] = yholecontourvertex[0][0]+fgkEndCapCoverPlateWidth[2];
  yholecontourvertex[0][2] = yholecontourvertex[0][1];
  yholecontourvertex[0][3] = yholecontourvertex[0][0];
  yholecontourvertex[0][4] = yholecontourvertex[0][3];
  yholecontourvertex[0][5] = yholecontourvertex[0][4]+0.5*(fgkEndCapCoverPlateWidth[2]
						   - 2.*fgkEndCapCoverPlateBigHoleRadius);
  yholecontourvertex[0][6] = yholecontourvertex[0][5];
  yholecontourvertex[0][7] = yholecontourvertex[0][6]+2.*fgkEndCapCoverPlateBigHoleRadius;
  yholecontourvertex[0][8] = yholecontourvertex[0][7];
  yholecontourvertex[0][9] = yholecontourvertex[0][0];

  xholecontourvertex[1][0] = xcontourvertex[0]+fgkEndCapCoverPlateLength[5];
  xholecontourvertex[1][1] = xholecontourvertex[1][0];
  xholecontourvertex[1][2] = xholecontourvertex[1][1]+fgkEndCapCoverPlateLength[2];
  xholecontourvertex[1][3] = xholecontourvertex[1][2];
  xholecontourvertex[1][4] = xholecontourvertex[1][0]
						   + 0.5*(fgkEndCapCoverPlateLength[2]
						   - 2.*fgkEndCapCoverPlateBigHoleRadius);
  xholecontourvertex[1][5] = xholecontourvertex[1][4];
  xholecontourvertex[1][6] = xholecontourvertex[1][5]
						   + 2.*fgkEndCapCoverPlateBigHoleRadius;
  xholecontourvertex[1][7] = xholecontourvertex[1][6];
  xholecontourvertex[1][8] = xholecontourvertex[1][4];
  xholecontourvertex[1][9] = xholecontourvertex[1][8];
  
  yholecontourvertex[1][0] = ycontourvertex[0]-(fgkEndCapCoverPlateWidth[1]
						   - fgkEndCapCoverPlateWidth[0]);
  yholecontourvertex[1][1] = ycontourvertex[0];
  yholecontourvertex[1][2] = yholecontourvertex[1][1];
  yholecontourvertex[1][3] = yholecontourvertex[1][0];
  yholecontourvertex[1][4] = yholecontourvertex[1][3];
  yholecontourvertex[1][5] = yholecontourvertex[1][4]+0.5*(fgkEndCapCoverPlateWidth[1]
						   - fgkEndCapCoverPlateWidth[0]
						   - 2.*fgkEndCapCoverPlateBigHoleRadius);
  yholecontourvertex[1][6] = yholecontourvertex[1][5];
  yholecontourvertex[1][7] = yholecontourvertex[1][6]+2.*fgkEndCapCoverPlateBigHoleRadius;
  yholecontourvertex[1][8] = yholecontourvertex[1][7];
  yholecontourvertex[1][9] = yholecontourvertex[1][0];

  TGeoVolume* holecontour[2];
  holecontour[0] = new TGeoVolume("EndCapCoverPlateContour1",contourshape1,
								  fSSDAlCoolBlockMedium);
  holecontour[0]->SetLineColor(6);
  holecontour[1] = new TGeoVolume("EndCapCoverPlateContour2",contourshape2,
								  fSSDAlCoolBlockMedium);
  holecontour[1]->SetLineColor(6);
  TGeoTranslation* holecontourtrans = new TGeoTranslation(fgkEndCapCoverPlateLength[3]
									+     fgkEndCapCoverPlateLength[2],0.,0.);
  TGeoTranslation*  bigholetrans[3];
  bigholetrans[0] = new TGeoTranslation(xholecontourvertex[0][4]+fgkEndCapCoverPlateBigHoleRadius,
										yholecontourvertex[0][7]-fgkEndCapCoverPlateBigHoleRadius,0.0);
  bigholetrans[1] = new TGeoTranslation(xholecontourvertex[0][4]+fgkEndCapCoverPlateBigHoleRadius
				  +                     fgkEndCapCoverPlateLength[4],yholecontourvertex[0][7]
				  -						fgkEndCapCoverPlateBigHoleRadius,0.0);
  bigholetrans[2] = new TGeoTranslation(xholecontourvertex[1][4]+fgkEndCapCoverPlateBigHoleRadius,
										yholecontourvertex[1][5]+fgkEndCapCoverPlateBigHoleRadius,0.0);
  /////////////////////////////////
  // Mother Volume Xtru Definition 
  /////////////////////////////////
  const Int_t kmothervertexnumber = 12;
  Double_t xmothervertex[kmothervertexnumber];  
  Double_t ymothervertex[kmothervertexnumber];  
  xmothervertex[0]  = xcontourvertex[0];
  xmothervertex[1]  = xmothervertex[0];
  xmothervertex[2]  = xmothervertex[1]+fgkEndCapCoverPlateLength[2];
  xmothervertex[3]  = xmothervertex[2];
  xmothervertex[4]  = xmothervertex[3]+fgkEndCapCoverPlateLength[3];
  xmothervertex[5]  = xmothervertex[4];
  xmothervertex[6]  = xmothervertex[5]+fgkEndCapCoverPlateLength[2];
  xmothervertex[7]  = xmothervertex[6];
  xmothervertex[8]  = xmothervertex[0]+fgkEndCapCoverPlateLength[5]
					+ fgkEndCapCoverPlateLength[2]; 
  xmothervertex[9]  = xmothervertex[8];
  xmothervertex[10] = xmothervertex[9]-fgkEndCapCoverPlateLength[2];
  xmothervertex[11] = xmothervertex[10];
  
  ymothervertex[0]  = ycontourvertex[0];
  ymothervertex[1]  = ymothervertex[0]+fgkEndCapCoverPlateWidth[0];
  ymothervertex[2]  = ymothervertex[1];
  ymothervertex[3]  = ycontourvertex[1];
  ymothervertex[4]  = ymothervertex[3];
  ymothervertex[5]  = ymothervertex[1];
  ymothervertex[6]  = ymothervertex[5];
  ymothervertex[7]  = ymothervertex[0];
  ymothervertex[8]  = ymothervertex[7];
  ymothervertex[9]  = ymothervertex[8]
				   - (fgkEndCapCoverPlateWidth[1]-fgkEndCapCoverPlateWidth[0]);
  ymothervertex[10] = ymothervertex[9];
  ymothervertex[11] = ymothervertex[8];
  TGeoXtru* mothercoverplateshape = new TGeoXtru(2);
  mothercoverplateshape->DefinePolygon(kmothervertexnumber,xmothervertex,ymothervertex);  
  mothercoverplateshape->DefineSection(0,-0.5*fgkEndCapCoverPlateThickness);
  mothercoverplateshape->DefineSection(1,+0.5*fgkEndCapCoverPlateThickness);
  TGeoVolume* mothercoverplate = new TGeoVolume("EndCapCoverPlateMother",mothercoverplateshape,fSSDAir);
  ////////////////////////////////////////
  // Adding Nodes
  ////////////////////////////////////////
//  TGeoTranslation** endcapcoverplatesmallholetrans[kendcapcoverplatesmallholenumber[0]]; 
  TGeoTranslation*** endcapcoverplatesmallholetrans;
  endcapcoverplatesmallholetrans = new TGeoTranslation**[kendcapcoverplatesmallholenumber[0]]; 
  Double_t transx[4] = {0,
						fgkEndCapCoverPlateSmallHoleSeparation[0],
						fgkEndCapCoverPlateSmallHoleSeparation[0]
					 +  fgkEndCapCoverPlateSmallHoleSeparation[1],
					 2.*fgkEndCapCoverPlateSmallHoleSeparation[0]
					 +  fgkEndCapCoverPlateSmallHoleSeparation[1]};
  Int_t index = 0;
  for(Int_t i=0; i<kendcapcoverplatesmallholenumber[0]; i++){
	endcapcoverplatesmallholetrans[i] = 
					new TGeoTranslation*[kendcapcoverplatesmallholenumber[1]];
    for(Int_t j=0; j<kendcapcoverplatesmallholenumber[1]; j++){
		index = kendcapcoverplatesmallholenumber[1]*i+j+1;
	    endcapcoverplatesmallholetrans[i][j] = 
		new TGeoTranslation(transx[i],
							j*fgkEndCapCoverPlateSmallHoleSeparation[2],0.);
	    if(index!=10){ 
			mothercoverplate->AddNode(endcapcoverplatesmallhole,
									  index,endcapcoverplatesmallholetrans[i][j]);
			mothercoverplate->AddNode(endcapsmallscrewpiece,
									  index,endcapcoverplatesmallholetrans[i][j]);
		}
		if(j<kendcapcoverplatesmallholenumber[1]-1) 
			mothercoverplate->AddNode(endcapcoverplatebox[0],
									  index,endcapcoverplatesmallholetrans[i][j]);
    }
  }
  mothercoverplate->AddNode(endcapcoverplatebox[1],1);
  mothercoverplate->AddNode(endcapcoverplatebox[2],1);
  mothercoverplate->AddNode(endcapcoverplatebox[3],1);
  mothercoverplate->AddNode(endcapfillingbox,1);
  mothercoverplate->AddNode(endcapcoverplatebighole,1,bigholetrans[0]);
  mothercoverplate->AddNode(endcapcoverplatebighole,2,bigholetrans[1]);
  mothercoverplate->AddNode(endcapcoverplatebighole,3,bigholetrans[2]);
  mothercoverplate->AddNode(holecontour[0],1);
  mothercoverplate->AddNode(holecontour[0],2,holecontourtrans);
  mothercoverplate->AddNode(holecontour[1],1);  
  mothercoverplate->AddNode(contour,1);
  /////////////////////////////////
  return mothercoverplate; 	
 }
 ////////////////////////////////////////////////////////////////////////////////
 TGeoVolume* AliITSv11GeometrySSD::GetEndCapCoolingTube(){
  /////////////////////////////////////////////////////////////
  // Getting EndCap Cooling Tube 
  /////////////////////////////////////////////////////////////
  TGeoTorus* endcapcoolingtubetorushape[5];
  TGeoVolume* endcapcoolingtubetorus[5];
  TGeoTube* endcapcoolingtubeshape[4];
  TGeoVolume* endcapcoolingtube[4];
  char endcapcoolingtubetorusname[30];
  char endcapcoolingtubename[30];
  TGeoTorus* endcapcoolingwatertubetorushape[5];
  TGeoVolume* endcapcoolingwatertubetorus[5];
  TGeoTube* endcapcoolingwatertubeshape[4];
  TGeoVolume* endcapcoolingwatertube[4];
  char endcapcoolingwatertubetorusname[30];
  char endcapcoolingwatertubename[30];
  for(Int_t i=0; i<5; i++){
	sprintf(endcapcoolingtubetorusname,"EndCapCoolingTubeTorus%d",i+1);
	sprintf(endcapcoolingtubename,"EndCapCoolingTube%d",i+1);
	sprintf(endcapcoolingwatertubetorusname,"EndCapCoolingWaterTubeTorus%d",i+1);
	sprintf(endcapcoolingwatertubename,"EndCapCoolingWaterTube%d",i+1);
	if(i==3){
		endcapcoolingtubetorushape[i] = new TGeoTorus(fgkEndCapCoolingTubeAxialRadius[0],
										fgkEndCapCoolingTubeRadiusMin,
										fgkEndCapCoolingTubeRadiusMax,
										90.0,fgkEndCapCoolingTubeAngle[3]);
		endcapcoolingwatertubetorushape[i] = new TGeoTorus(fgkEndCapCoolingTubeAxialRadius[0],
										0.,fgkEndCapCoolingTubeRadiusMin,
										90.0,fgkEndCapCoolingTubeAngle[3]);
    }
	else{
		endcapcoolingtubetorushape[i] = new TGeoTorus(i!=4?fgkEndCapCoolingTubeAxialRadius[0]
									  :fgkEndCapCoolingTubeAxialRadius[1],
									   fgkEndCapCoolingTubeRadiusMin,
									   fgkEndCapCoolingTubeRadiusMax,
								    0.,fgkEndCapCoolingTubeAngle[i]);
		endcapcoolingwatertubetorushape[i] = new TGeoTorus(i!=4?fgkEndCapCoolingTubeAxialRadius[0]
									       :fgkEndCapCoolingTubeAxialRadius[1],
										 0.,fgkEndCapCoolingTubeRadiusMin,
								         0.,fgkEndCapCoolingTubeAngle[i]);
	}
	endcapcoolingtubetorus[i] = new TGeoVolume(endcapcoolingtubetorusname,
 											   endcapcoolingtubetorushape[i],
											   fSSDCoolingTubePhynox);
	endcapcoolingwatertubetorus[i] = new TGeoVolume(endcapcoolingwatertubetorusname,
													endcapcoolingwatertubetorushape[i],
													fSSDCoolingTubeWater);
    endcapcoolingtubetorus[i]->SetLineColor(fColorPhynox);
    endcapcoolingwatertubetorus[i]->SetLineColor(fColorWater);
    if(i<4){
		endcapcoolingtubeshape[i] = new TGeoTube(fgkEndCapCoolingTubeRadiusMin,
								  fgkEndCapCoolingTubeRadiusMax,
							  0.5*fgkEndCapCoolingTubeLength[i]);
		endcapcoolingwatertubeshape[i] = new TGeoTube(0.,fgkEndCapCoolingTubeRadiusMin,
							  0.5*fgkEndCapCoolingTubeLength[i]);
        endcapcoolingtube[i] = new TGeoVolume(endcapcoolingtubename,
							 endcapcoolingtubeshape[i],fSSDCoolingTubePhynox);
        endcapcoolingwatertube[i] = new TGeoVolume(endcapcoolingwatertubename,
							 endcapcoolingwatertubeshape[i],fSSDCoolingTubeWater);
		endcapcoolingtube[i]->SetLineColor(fColorPhynox);
		endcapcoolingwatertube[i]->SetLineColor(fColorWater);
	}
  }
  TGeoVolumeAssembly* endcapcoolingtubemother = new TGeoVolumeAssembly("MotherEndCapCoolingTube");
  /////////////////////////////////////////
  // Transformation for Volume Positioning 
  /////////////////////////////////////////
  TGeoCombiTrans* coolingtubecombitrans[6];
  TGeoRotation* coolingtuberot[8];
  TGeoTranslation* coolingtubetrans[6];
  TGeoHMatrix* coolingtubematrix[4];
  TGeoCombiTrans* torustubecombitrans[4];
  TGeoRotation* torustuberot[7];
  TGeoTranslation* torustubetrans[4];
  TGeoHMatrix* torustubematrix[5];
  TGeoCombiTrans* coolingwatertubecombitrans[6];
  TGeoRotation* coolingwatertuberot[8];
  TGeoTranslation* coolingwatertubetrans[6];
  TGeoHMatrix* coolingwatertubematrix[4];
  TGeoCombiTrans* toruswatertubecombitrans[4];
  TGeoRotation* toruswatertuberot[7];
  TGeoTranslation* toruswatertubetrans[4];
  TGeoHMatrix* toruswatertubematrix[5];
  for(Int_t i=0; i<8; i++){
    if(i<6){
	 coolingtubetrans[i] = new TGeoTranslation();
	 coolingwatertubetrans[i] = new TGeoTranslation();
    }
    if(i<8){
	 coolingtuberot[i] = new TGeoRotation();
	 coolingwatertuberot[i] = new TGeoRotation();
    }
    if(i<4){
	 torustubetrans[i] = new TGeoTranslation();
	 toruswatertubetrans[i] = new TGeoTranslation();
    }
    if(i<7){
	 torustuberot[i] = new TGeoRotation();
	 toruswatertuberot[i] = new TGeoRotation();
	}
  }
  /////////////////////////////////////////
  // Transformation for Inox Volume Positioning 
  /////////////////////////////////////////
  coolingtubetrans[0]->SetTranslation(fgkEndCapCoolingTubeAxialRadius[0],
									  -endcapcoolingtubeshape[0]->GetDz(),0.);
  coolingtuberot[0]->SetAngles(0.,90.,0.);
  coolingtubecombitrans[0] = new TGeoCombiTrans(*coolingtubetrans[0],
												*coolingtuberot[0]);
												
  coolingtubetrans[1]->SetTranslation(0.,-endcapcoolingtubeshape[1]->GetDz(),0.);
  coolingtuberot[1]->SetAngles(0.,90.,0.);
  coolingtubecombitrans[1] = new TGeoCombiTrans(*coolingtubetrans[1],
												*coolingtuberot[1]);

  coolingtubetrans[2]->SetTranslation(fgkEndCapCoolingTubeAxialRadius[0]
									 *CosD(fgkEndCapCoolingTubeAngle[0]),
									  fgkEndCapCoolingTubeAxialRadius[0]
									 *SinD(fgkEndCapCoolingTubeAngle[0]),
									  0.);
  coolingtuberot[2]->SetAngles(fgkEndCapCoolingTubeAngle[0]-180.,0.,0.);
  coolingtubecombitrans[2] = new TGeoCombiTrans(*coolingtubetrans[2],
												*coolingtuberot[2]);

  coolingtubematrix[0] = new TGeoHMatrix((*coolingtubecombitrans[2])
					   *				 (*coolingtubecombitrans[1]));

  torustubetrans[0]->SetTranslation(-fgkEndCapCoolingTubeAxialRadius[0],0.,
									 endcapcoolingtubeshape[1]->GetDz());
  torustuberot[0]->SetAngles(0.,90.,0.); 
  torustubecombitrans[0] = new TGeoCombiTrans(*torustubetrans[0],*torustuberot[0]);

  torustubematrix[0] = new TGeoHMatrix((*coolingtubematrix[0])*(*torustubecombitrans[0]));

  coolingtubetrans[3]->SetTranslation(-fgkEndCapCoolingTubeAxialRadius[0],
									  -endcapcoolingtubeshape[2]->GetDz(),0.);
  coolingtuberot[3]->SetAngles(0.,90.,0.);
  coolingtubecombitrans[3] = new TGeoCombiTrans(*coolingtubetrans[3],
												*coolingtuberot[3]);
  coolingtuberot[4]->SetAngles(-180.+fgkEndCapCoolingTubeAngle[1],0.,0.);
  coolingtubematrix[1] = new TGeoHMatrix((*coolingtuberot[4])*(*coolingtubecombitrans[3]));
  coolingtubematrix[1]->MultiplyLeft(torustubematrix[0]);
  
  torustubetrans[1]->SetTranslation(-fgkEndCapCoolingTubeAxialRadius[0],0.,
									endcapcoolingtubeshape[2]->GetDz());
  torustuberot[1]->SetAngles(0.,90.,0.); 
  torustubecombitrans[1] = new TGeoCombiTrans(*torustubetrans[1],*torustuberot[1]);
  torustuberot[2]->SetAngles(180.,0.,0.); 
  torustubematrix[2] = new TGeoHMatrix((*torustuberot[2])*(*torustubecombitrans[1]));
  torustubematrix[2]->MultiplyLeft(coolingtubematrix[1]);

  torustubetrans[2]->SetTranslation(0.,fgkEndCapCoolingTubeAxialRadius[0],
									-fgkEndCapCoolingTubeAxialRadius[0]);
  torustuberot[3]->SetAngles(0.,90.,0.); 
  torustubecombitrans[2] = new TGeoCombiTrans(*torustubetrans[2],*torustuberot[3]);
  torustuberot[4]->SetAngles(fgkEndCapCoolingTubeAngle[2]-90.,0.,0.); 
  torustubematrix[3] = new TGeoHMatrix((*torustuberot[4])*(*torustubecombitrans[2]));
  torustubematrix[3]->MultiplyLeft(torustubematrix[2]);

  coolingtubetrans[4]->SetTranslation(-endcapcoolingtubeshape[3]->GetDz(),
									  fgkEndCapCoolingTubeAxialRadius[0],0.);
  coolingtuberot[5]->SetAngles(90.,90.,-90.);
  coolingtubecombitrans[4] = new TGeoCombiTrans(*coolingtubetrans[4],
												*coolingtuberot[5]);
  coolingtuberot[6]->SetAngles(fgkEndCapCoolingTubeAngle[3],0.,0.);
  coolingtubematrix[2] = new TGeoHMatrix((*coolingtuberot[6])*(*coolingtubecombitrans[4]));
  coolingtubematrix[2]->MultiplyLeft(torustubematrix[3]);
  
  torustubetrans[3]->SetTranslation(-fgkEndCapCoolingTubeAxialRadius[1],0.,
									endcapcoolingtubeshape[0]->GetDz());
  torustuberot[5]->SetAngles(0.,90.,0.); 
  torustubecombitrans[3] = new TGeoCombiTrans(*torustubetrans[3],*torustuberot[5]);
  torustuberot[6]->SetAngles(-90.,0.,0.); 
  torustubematrix[4] = new TGeoHMatrix((*torustuberot[6])*(*torustubecombitrans[3]));
  torustubematrix[4]->MultiplyLeft(coolingtubecombitrans[0]);
  
  coolingtubetrans[5]->SetTranslation(fgkEndCapCoolingTubeAxialRadius[1],
									  endcapcoolingtubeshape[3]->GetDz(),0.);
  coolingtuberot[6]->SetAngles(0.,90.,0.);
  coolingtubecombitrans[5] = new TGeoCombiTrans(*coolingtubetrans[5],
												*coolingtuberot[6]);
  coolingtuberot[7]->SetAngles(fgkEndCapCoolingTubeAngle[4],0.,0.);
  coolingtubematrix[3] = new TGeoHMatrix((*coolingtuberot[7])*(*coolingtubecombitrans[5]));
  coolingtubematrix[3]->MultiplyLeft(torustubematrix[4]);
    /////////////////////////////////////////
  // Transformation for Water Volume Positioning 
  /////////////////////////////////////////
  coolingwatertubetrans[0]->SetTranslation(fgkEndCapCoolingTubeAxialRadius[0],
									  -endcapcoolingwatertubeshape[0]->GetDz(),0.);
  coolingwatertuberot[0]->SetAngles(0.,90.,0.);
  coolingwatertubecombitrans[0] = new TGeoCombiTrans(*coolingwatertubetrans[0],
												     *coolingwatertuberot[0]);

  coolingwatertubetrans[1]->SetTranslation(0.,-endcapcoolingwatertubeshape[1]->GetDz(),0.);
  coolingwatertuberot[1]->SetAngles(0.,90.,0.);
  coolingwatertubecombitrans[1] = new TGeoCombiTrans(*coolingwatertubetrans[1],
												     *coolingwatertuberot[1]);

  coolingwatertubetrans[2]->SetTranslation(fgkEndCapCoolingTubeAxialRadius[0]
										  *CosD(fgkEndCapCoolingTubeAngle[0]),
										  fgkEndCapCoolingTubeAxialRadius[0]
										  *SinD(fgkEndCapCoolingTubeAngle[0]),
									      0.);
  coolingwatertuberot[2]->SetAngles(fgkEndCapCoolingTubeAngle[0]-180.,0.,0.);
  coolingwatertubecombitrans[2] = new TGeoCombiTrans(*coolingwatertubetrans[2],
												    *coolingwatertuberot[2]);

  coolingwatertubematrix[0] = new TGeoHMatrix((*coolingwatertubecombitrans[2])
					   *				     (*coolingwatertubecombitrans[1]));
					   
  toruswatertubetrans[0]->SetTranslation(-fgkEndCapCoolingTubeAxialRadius[0],0.,
									 endcapcoolingwatertubeshape[1]->GetDz());
  toruswatertuberot[0]->SetAngles(0.,90.,0.); 
  toruswatertubecombitrans[0] = new TGeoCombiTrans(*toruswatertubetrans[0],
												   *toruswatertuberot[0]);

  toruswatertubematrix[0] = new TGeoHMatrix((*coolingwatertubematrix[0])
						  *					(*toruswatertubecombitrans[0]));

  coolingwatertubetrans[3]->SetTranslation(-fgkEndCapCoolingTubeAxialRadius[0],
									  -endcapcoolingwatertubeshape[2]->GetDz(),0.);
  coolingwatertuberot[3]->SetAngles(0.,90.,0.);
  coolingwatertubecombitrans[3] = new TGeoCombiTrans(*coolingwatertubetrans[3],
												     *coolingwatertuberot[3]);
  coolingwatertuberot[4]->SetAngles(-180.+fgkEndCapCoolingTubeAngle[1],0.,0.);
  coolingwatertubematrix[1] = new TGeoHMatrix((*coolingwatertuberot[4])
							*				  (*coolingwatertubecombitrans[3]));
  coolingwatertubematrix[1]->MultiplyLeft(toruswatertubematrix[0]);

  toruswatertubetrans[1]->SetTranslation(-fgkEndCapCoolingTubeAxialRadius[0],0.,
									endcapcoolingwatertubeshape[2]->GetDz());
  toruswatertuberot[1]->SetAngles(0.,90.,0.); 
  toruswatertubecombitrans[1] = new TGeoCombiTrans(*toruswatertubetrans[1],
												   *toruswatertuberot[1]);
  toruswatertuberot[2]->SetAngles(180.,0.,0.); 
  toruswatertubematrix[2] = new TGeoHMatrix((*toruswatertuberot[2])
						  *                 (*toruswatertubecombitrans[1]));
  toruswatertubematrix[2]->MultiplyLeft(coolingwatertubematrix[1]);
  
  toruswatertubetrans[2]->SetTranslation(0.,fgkEndCapCoolingTubeAxialRadius[0],
										   -fgkEndCapCoolingTubeAxialRadius[0]);
  toruswatertuberot[3]->SetAngles(0.,90.,0.); 
  toruswatertubecombitrans[2] = new TGeoCombiTrans(*toruswatertubetrans[2],
												   *toruswatertuberot[3]);
  toruswatertuberot[4]->SetAngles(fgkEndCapCoolingTubeAngle[2]-90.,0.,0.); 
  toruswatertubematrix[3] = new TGeoHMatrix((*toruswatertuberot[4])
						  *					(*toruswatertubecombitrans[2]));
  toruswatertubematrix[3]->MultiplyLeft(toruswatertubematrix[2]);

  coolingwatertubetrans[4]->SetTranslation(-endcapcoolingwatertubeshape[3]->GetDz(),
									        fgkEndCapCoolingTubeAxialRadius[0],0.);
  coolingwatertuberot[5]->SetAngles(90.,90.,-90.);
  coolingwatertubecombitrans[4] = new TGeoCombiTrans(*coolingwatertubetrans[4],
												     *coolingwatertuberot[5]);
  coolingwatertuberot[6]->SetAngles(fgkEndCapCoolingTubeAngle[3],0.,0.);
  coolingwatertubematrix[2] = new TGeoHMatrix((*coolingwatertuberot[6])
							*				  (*coolingwatertubecombitrans[4]));
  coolingwatertubematrix[2]->MultiplyLeft(toruswatertubematrix[3]);
  
  toruswatertubetrans[3]->SetTranslation(-fgkEndCapCoolingTubeAxialRadius[1],0.,
									      endcapcoolingwatertubeshape[0]->GetDz());
  toruswatertuberot[5]->SetAngles(0.,90.,0.); 
  toruswatertubecombitrans[3] = new TGeoCombiTrans(*toruswatertubetrans[3],
												   *toruswatertuberot[5]);
  toruswatertuberot[6]->SetAngles(-90.,0.,0.); 
  toruswatertubematrix[4] = new TGeoHMatrix((*toruswatertuberot[6])
						  *                 (*toruswatertubecombitrans[3]));
  toruswatertubematrix[4]->MultiplyLeft(coolingwatertubecombitrans[0]);
  
  coolingwatertubetrans[5]->SetTranslation(fgkEndCapCoolingTubeAxialRadius[1],
									  endcapcoolingwatertubeshape[3]->GetDz(),0.);
  coolingwatertuberot[6]->SetAngles(0.,90.,0.);
  coolingwatertubecombitrans[5] = new TGeoCombiTrans(*coolingwatertubetrans[5],
												     *coolingwatertuberot[6]);
  coolingwatertuberot[7]->SetAngles(fgkEndCapCoolingTubeAngle[4],0.,0.);
  coolingwatertubematrix[3] = new TGeoHMatrix((*coolingwatertuberot[7])
							*				  (*coolingwatertubecombitrans[5]));
  coolingwatertubematrix[3]->MultiplyLeft(toruswatertubematrix[4]);
  /////////////////////////////////////////
  // Positioning Volumes
  /////////////////////////////////////////
  endcapcoolingtubemother->AddNode(endcapcoolingtubetorus[0],1);
  endcapcoolingtubemother->AddNode(endcapcoolingwatertubetorus[0],1);
  
  endcapcoolingtubemother->AddNode(endcapcoolingtube[0],1,coolingtubecombitrans[0]);
  endcapcoolingtubemother->AddNode(endcapcoolingwatertube[0],1,coolingwatertubecombitrans[0]);

  endcapcoolingtubemother->AddNode(endcapcoolingtube[1],1,coolingtubematrix[0]);
  endcapcoolingtubemother->AddNode(endcapcoolingwatertube[1],1,coolingwatertubematrix[0]);
 
  endcapcoolingtubemother->AddNode(endcapcoolingtubetorus[1],1,torustubematrix[0]);
  endcapcoolingtubemother->AddNode(endcapcoolingwatertubetorus[1],1,toruswatertubematrix[0]);

  endcapcoolingtubemother->AddNode(endcapcoolingtube[2],1,coolingtubematrix[1]);
  endcapcoolingtubemother->AddNode(endcapcoolingwatertube[2],1,coolingwatertubematrix[1]);

  endcapcoolingtubemother->AddNode(endcapcoolingtubetorus[2],1,torustubematrix[2]);
  endcapcoolingtubemother->AddNode(endcapcoolingwatertubetorus[2],1,toruswatertubematrix[2]);

  endcapcoolingtubemother->AddNode(endcapcoolingtubetorus[3],1,torustubematrix[3]);
  endcapcoolingtubemother->AddNode(endcapcoolingwatertubetorus[3],1,toruswatertubematrix[3]);

  endcapcoolingtubemother->AddNode(endcapcoolingtube[3],1,coolingtubematrix[2]);
  endcapcoolingtubemother->AddNode(endcapcoolingwatertube[3],1,coolingwatertubematrix[2]);
  
  endcapcoolingtubemother->AddNode(endcapcoolingtubetorus[4],1,torustubematrix[4]);
  endcapcoolingtubemother->AddNode(endcapcoolingwatertubetorus[4],1,toruswatertubematrix[4]);
 
  endcapcoolingtubemother->AddNode(endcapcoolingtube[3],2,coolingtubematrix[3]);
  endcapcoolingtubemother->AddNode(endcapcoolingwatertube[3],2,coolingwatertubematrix[3]);
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<8; i++){
    if(i<6){
	 delete coolingtubetrans[i];
	 delete coolingwatertubetrans[i];
	 if(i!=0){
	  delete coolingtubecombitrans[i];
	  delete coolingwatertubecombitrans[i];
	 }
	}
    if(i<8){
	  delete coolingtuberot[i];
	  delete coolingwatertuberot[i];
    }
    if(i<4){
		delete torustubetrans[i];
		delete toruswatertubetrans[i];
		delete torustubecombitrans[i];
		delete toruswatertubecombitrans[i];
	} 
    if(i<7){
	 delete torustuberot[i];
	 delete toruswatertuberot[i];
	}
  }
  /////////////////////////////////////////////////////////////
  return endcapcoolingtubemother;
 }
 ////////////////////////////////////////////////////////////////////////////////
 TGeoVolume* AliITSv11GeometrySSD::GetEndCapSideCover() const {
  /////////////////////////////////////////////////////////////
  // Getting EndCap Cover Side 
  /////////////////////////////////////////////////////////////
  const Int_t kendcapcoverholenumber[2] = {7,5}; 
  const Int_t kvertexnumber = 15; 
  Double_t xvertex[kvertexnumber], yvertex[kvertexnumber];
  xvertex[0]  = 0.0;
  xvertex[1]  = xvertex[0];
  xvertex[2]  = fgkEndCapSideCoverLength[0];
  xvertex[3]  = fgkEndCapSideCoverLength[1];
  xvertex[4]  = xvertex[3];
  xvertex[5]  = fgkEndCapSideCoverLength[2];
  xvertex[6]  = xvertex[5];
  xvertex[7]  = xvertex[2];
  xvertex[8]  = xvertex[7];
  xvertex[9]  = xvertex[6]-fgkEndCapSideCoverLength[0];
  xvertex[10] = xvertex[9];
  xvertex[11] = xvertex[5]-fgkEndCapSideCoverLength[0]-kendcapcoverholenumber[1]
			  * fgkEndCapSideCoverLength[3]-(kendcapcoverholenumber[1]-1)
			  * fgkEndCapSideCoverLength[4];
  xvertex[12] = xvertex[11];
  xvertex[13] = xvertex[5]-fgkEndCapSideCoverLength[0]-kendcapcoverholenumber[0]
			  * fgkEndCapSideCoverLength[3]-(kendcapcoverholenumber[0]-1)
			  * fgkEndCapSideCoverLength[4];
  xvertex[14] = xvertex[13];
  yvertex[0]  = 0.0;
  yvertex[1]  = fgkEndCapSideCoverWidth[0];
  yvertex[2]  = fgkEndCapSideCoverWidth[1]-fgkEndCapSideCoverWidth[2];
  yvertex[3]  = yvertex[2];
  yvertex[4]  = fgkEndCapSideCoverWidth[1];
  yvertex[5]  = yvertex[4];
  yvertex[6]  = yvertex[0];
  yvertex[7]  = yvertex[6];
  yvertex[8]  = fgkEndCapSideCoverWidth[6];
  yvertex[9]  = yvertex[8];
  yvertex[10] = fgkEndCapSideCoverWidth[1]-fgkEndCapSideCoverWidth[3];
  yvertex[11] = yvertex[10];
  yvertex[12] = yvertex[9]+2.*fgkEndCapSideCoverWidth[5]+fgkEndCapSideCoverLength[4]; 
  yvertex[13] = yvertex[12];
  yvertex[14] = yvertex[6];
  TGeoXtru* endcapsidecovershapeout = new TGeoXtru(2);
  endcapsidecovershapeout->SetName("endcapsidecovershapeout");
  endcapsidecovershapeout->DefinePolygon(7,xvertex,yvertex); 
  endcapsidecovershapeout->DefineSection(0,-0.5*fgkEndCapSideCoverThickness);
  endcapsidecovershapeout->DefineSection(1,0.5*fgkEndCapSideCoverThickness);
  TGeoXtru* endcapsidecovershapein = new TGeoXtru(2);
  endcapsidecovershapein->SetName("endcapsidecovershapein");
  endcapsidecovershapein->DefinePolygon(6,&xvertex[8],&yvertex[8]); 
  endcapsidecovershapein->DefineSection(0,-0.5*fgkEndCapSideCoverThickness-0.01);
  endcapsidecovershapein->DefineSection(1,0.5*fgkEndCapSideCoverThickness+0.01);


  TGeoCompositeShape* endcapsidecovershape = new TGeoCompositeShape("endcapsidecovershape", "endcapsidecovershapeout-endcapsidecovershapein");
  TGeoVolume* endcapsidecover = new TGeoVolume("EndCapSideCover",
								endcapsidecovershape,fSSDCoolingTubePhynox);
  endcapsidecover->SetLineColor(fColorPhynox);
  ////////////////////////////////////////////
  // Defininition of Mother Volume
  ////////////////////////////////////////////
  const Int_t kmothervertexnumber = 7;
  Double_t xmothervertex[kmothervertexnumber]; 
  Double_t ymothervertex[kmothervertexnumber]; 
  for(Int_t i=0; i<kmothervertexnumber; i++){
	xmothervertex[i] = xvertex[i];
	ymothervertex[i] = yvertex[i];
  }
  TGeoXtru* endcapsidecovermothershape = new TGeoXtru(2);
  endcapsidecovermothershape->DefinePolygon(kmothervertexnumber,xmothervertex,ymothervertex); 
  endcapsidecovermothershape->DefineSection(0,-0.5*fgkEndCapSideCoverThickness);
  endcapsidecovermothershape->DefineSection(1,0.5*fgkEndCapSideCoverThickness);
  TGeoVolume* endcapsidecovermother = new TGeoVolume("EndCapSideCoverMother",
								endcapsidecovermothershape,fSSDAir);
  ////////////////////////////////////////////
  endcapsidecovermother->AddNode(endcapsidecover,1);
  TGeoBBox* endcapsidecoverboxshape[4];
  endcapsidecoverboxshape[0] = new TGeoBBox(0.5*(kendcapcoverholenumber[0]*fgkEndCapSideCoverLength[3]
							 +     (kendcapcoverholenumber[0]-1)*fgkEndCapSideCoverLength[4]),
							       0.5*fgkEndCapSideCoverLength[4],
								   0.5*fgkEndCapSideCoverThickness); 
  endcapsidecoverboxshape[1] = new TGeoBBox(0.5*(kendcapcoverholenumber[1]*fgkEndCapSideCoverLength[3]
							 +     (kendcapcoverholenumber[1]-1)*fgkEndCapSideCoverLength[4]),
							       0.5*(fgkEndCapSideCoverWidth[4]-2.*fgkEndCapSideCoverWidth[5]
							 -     fgkEndCapSideCoverLength[4]),
								   0.5*fgkEndCapSideCoverThickness); 
  endcapsidecoverboxshape[2] = new TGeoBBox(endcapsidecoverboxshape[1]->GetDX(),
							       0.5*fgkEndCapSideCoverLength[4],
								   0.5*fgkEndCapSideCoverThickness); 
  endcapsidecoverboxshape[3] = new TGeoBBox(0.5*fgkEndCapSideCoverLength[4],
							       0.5*fgkEndCapSideCoverWidth[5],
								   0.5*fgkEndCapSideCoverThickness); 
  TGeoVolume* endcapsidecoverbox[4];
  endcapsidecoverbox[0] = new TGeoVolume("EndCapSideCoverBox1",endcapsidecoverboxshape[0],fSSDCoolingTubePhynox);
  endcapsidecoverbox[1] = new TGeoVolume("EndCapSideCoverBox2",endcapsidecoverboxshape[1],fSSDCoolingTubePhynox);
  endcapsidecoverbox[2] = new TGeoVolume("EndCapSideCoverBox3",endcapsidecoverboxshape[2],fSSDCoolingTubePhynox);
  endcapsidecoverbox[3] = new TGeoVolume("EndCapSideCoverBox4",endcapsidecoverboxshape[3],fSSDCoolingTubePhynox);
  for(Int_t i=0; i<4; i++)   endcapsidecoverbox[i]->SetLineColor(fColorPhynox);
//  TGeoTranslation* endcapsidecoverboxtrans[3+2*(kendcapcoverholenumber[0]-1)+2*(kendcapcoverholenumber[1]-1)];
  TGeoTranslation** endcapsidecoverboxtrans;
  endcapsidecoverboxtrans = new TGeoTranslation*[3+2*(kendcapcoverholenumber[0]-1)+2*(kendcapcoverholenumber[1]-1)];
  endcapsidecoverboxtrans[0] = new TGeoTranslation(endcapsidecoverboxshape[0]->GetDX()
							 +					   fgkEndCapSideCoverLength[0],
												   endcapsidecoverboxshape[0]->GetDY()
							 +                     yvertex[9]+fgkEndCapSideCoverWidth[5],0.);
  endcapsidecoverboxtrans[1] = new TGeoTranslation(endcapsidecoverboxshape[1]->GetDX()
							 +                     xvertex[11],
												   endcapsidecoverboxshape[1]->GetDY()
							 +                     yvertex[12],0.);
  endcapsidecoverboxtrans[2] = new TGeoTranslation(endcapsidecoverboxshape[2]->GetDX()
							 +                     xvertex[11],
												   endcapsidecoverboxshape[2]->GetDY()
							 +                     yvertex[12]
							 +					   2.*endcapsidecoverboxshape[1]->GetDY() 
							 +                     fgkEndCapSideCoverWidth[5],0.);
  endcapsidecovermother->AddNode(endcapsidecoverbox[0],1,endcapsidecoverboxtrans[0]);
  endcapsidecovermother->AddNode(endcapsidecoverbox[1],1,endcapsidecoverboxtrans[1]);
  endcapsidecovermother->AddNode(endcapsidecoverbox[2],1,endcapsidecoverboxtrans[2]);
  for(Int_t i=0; i<2; i++)
	for(Int_t j=0; j<kendcapcoverholenumber[0]-1; j++){
		endcapsidecoverboxtrans[i*(kendcapcoverholenumber[0]-1)+j+3] = 
			new TGeoTranslation(endcapsidecoverboxshape[3]->GetDX()+fgkEndCapSideCoverLength[0]
								+(j+1)*fgkEndCapSideCoverLength[3]+j*fgkEndCapSideCoverLength[4],
								endcapsidecoverboxshape[3]->GetDY()+fgkEndCapSideCoverWidth[6]
								+i*(fgkEndCapSideCoverWidth[5]+fgkEndCapSideCoverLength[4]),0.0);
		endcapsidecovermother->AddNode(endcapsidecoverbox[3],i*(kendcapcoverholenumber[0]-1)+j+1,
								endcapsidecoverboxtrans[i*(kendcapcoverholenumber[0]-1)+j+3]);
	}
  for(Int_t i=0; i<2; i++)
	for(Int_t j=0; j<kendcapcoverholenumber[1]-1; j++){
		endcapsidecoverboxtrans[2*(kendcapcoverholenumber[0]-1)+3+i*(kendcapcoverholenumber[1]-1)+j] = 
		new TGeoTranslation(endcapsidecoverboxshape[3]->GetDX()+xvertex[12]
							+(j+1)*fgkEndCapSideCoverLength[3]+j*fgkEndCapSideCoverLength[4],
							endcapsidecoverboxshape[3]->GetDY()+fgkEndCapSideCoverWidth[6]
							+fgkEndCapSideCoverWidth[4]+i*(fgkEndCapSideCoverWidth[5]
							+fgkEndCapSideCoverLength[4]),0.0);
		endcapsidecovermother->AddNode(endcapsidecoverbox[3],
								2*(kendcapcoverholenumber[0]-1)+3+i*(kendcapcoverholenumber[1]-1)+j,
								endcapsidecoverboxtrans[2*(kendcapcoverholenumber[0]-1)+3
								+i*(kendcapcoverholenumber[1]-1)+j]);
	}
	return endcapsidecovermother;
 } 
 ////////////////////////////////////////////////////////////////////////////////
 TGeoVolume** AliITSv11GeometrySSD::GetEndCapCards() const { 
 ////////////////////////////////////////////////////////////////////////////////
 // Method returning Interface Card A, Interface Card B, Supply Card 
 ////////////////////////////////////////////////////////////////////////////////
 /////////////////////
 // Supply Card
 /////////////////////
 // Electronic Board Back Al Plane
 const Int_t kelectboardbackvertexnumber = 8;
 Double_t xelectboardback[kelectboardbackvertexnumber];
 Double_t yelectboardback[kelectboardbackvertexnumber];
 xelectboardback[0] = 0.0;
 xelectboardback[1] = xelectboardback[0];
 xelectboardback[2] = fgkEndCapCardElectBoardBackLength[0];
 xelectboardback[3] = xelectboardback[2];
 xelectboardback[4] = xelectboardback[3]-fgkEndCapCardElectBoardBackLength[1];
 xelectboardback[5] = xelectboardback[4];
 xelectboardback[6] = fgkEndCapCardElectBoardBackLength[1];
 xelectboardback[7] = xelectboardback[6];
 
 yelectboardback[0] = 0.0;
 yelectboardback[1] = fgkEndCapCardElectBoardBackWidth[0];
 yelectboardback[2] = yelectboardback[1];
 yelectboardback[3] = yelectboardback[0];
 yelectboardback[4] = yelectboardback[3];
 yelectboardback[5] = yelectboardback[4]+fgkEndCapCardElectBoardBackWidth[1];
 yelectboardback[6] = yelectboardback[5];
 yelectboardback[7] = yelectboardback[4];
 TGeoXtru* electboardbackshape = new TGeoXtru(2);
 electboardbackshape->DefinePolygon(kelectboardbackvertexnumber,
									xelectboardback,yelectboardback); 
 electboardbackshape->DefineSection(0,0.0);
 electboardbackshape->DefineSection(1,fgkEndCapCardElectBoardBackThickness);
 TGeoVolume* electboardback = new TGeoVolume("EndCapCardElectBoardBackPlane",
											 electboardbackshape,fSSDSupportRingAl);
 electboardback->SetLineColor(fColorAl);
 // Electronic Board Kapton Layer
 const Int_t kelectlayervertexnumber = 8;
 Double_t xelectlayer[kelectlayervertexnumber];
 Double_t yelectlayer[kelectlayervertexnumber];
 xelectlayer[0] = fgkEndCapCardElectBoardBackLength[0]-fgkEndCapCardElectBoardLength;
 xelectlayer[1] = xelectlayer[0];
 xelectlayer[2] = fgkEndCapCardElectBoardLength;
 xelectlayer[3] = xelectlayer[2];
 for(Int_t i=4; i<kelectlayervertexnumber; i++) xelectlayer[i] = xelectboardback[i]; 
     
 yelectlayer[0] = 0.0;
 yelectlayer[1] = fgkEndCapCardElectBoardLayerWidth[0];
 yelectlayer[2] = yelectlayer[1];
 yelectlayer[3] = yelectlayer[0];
 yelectlayer[4] = yelectlayer[3];
 yelectlayer[5] = yelectlayer[4]+fgkEndCapCardElectBoardLayerWidth[1];
 yelectlayer[6] = yelectlayer[5];
 yelectlayer[7] = yelectlayer[4];
 TGeoXtru* electlayershape = new TGeoXtru(2);
 electlayershape->DefinePolygon(kelectlayervertexnumber,xelectlayer,yelectlayer); 
 electlayershape->DefineSection(0,0.0);
 electlayershape->DefineSection(1,fgkEndCapCardElectBoardLayerThickness);
 TGeoVolume* electlayer = new TGeoVolume("EndCapCardElectBoardLayer",
											 electlayershape,fSSDKaptonFlexMedium);
 electlayer->SetLineColor(fColorKapton);
 // JMD Connector Female
 const Int_t kjmdconnectorvertexnumber = 6;
 Double_t xjmdconnectorvertex[kjmdconnectorvertexnumber]; 
 Double_t yjmdconnectorvertex[kjmdconnectorvertexnumber]; 
 xjmdconnectorvertex[0] = 0.0; 
 xjmdconnectorvertex[1] = xjmdconnectorvertex[0]; 
 xjmdconnectorvertex[2] = fgkEndCapCardJMDConnectorLength[1]; 
 xjmdconnectorvertex[3] = xjmdconnectorvertex[2];  
 xjmdconnectorvertex[4] = fgkEndCapCardJMDConnectorLength[0]; 
 xjmdconnectorvertex[5] = xjmdconnectorvertex[4]; 

 yjmdconnectorvertex[0] = 0.0; 
 yjmdconnectorvertex[1] = fgkEndCapCardJMDConnectorWidth[0]; 
 yjmdconnectorvertex[2] = yjmdconnectorvertex[1]; 
 yjmdconnectorvertex[3] = yjmdconnectorvertex[2]+fgkEndCapCardJMDConnectorWidth[1]; 
 yjmdconnectorvertex[4] = yjmdconnectorvertex[3]; 
 yjmdconnectorvertex[5] = yjmdconnectorvertex[0]; 
 TGeoXtru* jmdconnectorshape = new TGeoXtru(2);
 jmdconnectorshape->DefinePolygon(kjmdconnectorvertexnumber,xjmdconnectorvertex,
								  yjmdconnectorvertex); 
 jmdconnectorshape->DefineSection(0,0.0);
 jmdconnectorshape->DefineSection(1,fgkEndCapCardJMDConnectorThickness);
 TGeoVolume* jmdconnector = new TGeoVolume("EndCapCardJMDConnector",
										   jmdconnectorshape,fSSDMountingBlockMedium);
 jmdconnector->SetLineColor(fColorG10);
 // Top Cable Connector
 const Int_t kcableconnectorvertexnumber = 8;
 Double_t xconnectorvertex[kcableconnectorvertexnumber]; 
 Double_t yconnectorvertex[kcableconnectorvertexnumber]; 
 xconnectorvertex[0] = 0.0;
 xconnectorvertex[1] = xconnectorvertex[0];
 xconnectorvertex[2] = xconnectorvertex[1]+fgkEndCapCardCableConnectorLength[1];
 xconnectorvertex[3] = xconnectorvertex[2];
 xconnectorvertex[4] = fgkEndCapCardCableConnectorLength[0]
					 - fgkEndCapCardCableConnectorLength[2];
 xconnectorvertex[5] = xconnectorvertex[4];
 xconnectorvertex[6] = fgkEndCapCardCableConnectorLength[0];
 xconnectorvertex[7] = xconnectorvertex[6];

 yconnectorvertex[0] = 0.0;
 yconnectorvertex[1] = fgkEndCapCardCableConnectorWidth[0];
 yconnectorvertex[2] = yconnectorvertex[1];
 yconnectorvertex[3] = fgkEndCapCardCableConnectorWidth[1];
 yconnectorvertex[4] = yconnectorvertex[3];
 yconnectorvertex[5] = yconnectorvertex[1];
 yconnectorvertex[6] = yconnectorvertex[5];
 yconnectorvertex[7] = yconnectorvertex[0];
 TGeoXtru* cableconnectorshape = new TGeoXtru(2);
 cableconnectorshape->DefinePolygon(kcableconnectorvertexnumber,xconnectorvertex,
								    yconnectorvertex); 
 cableconnectorshape->DefineSection(0,0.0);
 cableconnectorshape->DefineSection(1,fgkEndCapCardCableConnectorThickness);
 TGeoVolume* cableconnector = new TGeoVolume("EndCapCardTopCableConnector",
										   cableconnectorshape,fSSDMountingBlockMedium);
 cableconnector->SetLineColor(fColorG10);
 // Strip Connection
 TGeoBBox* endcapstripconnectionshape = 
								new TGeoBBox(0.5*fgkEndCapStripConnectionLength,
											 0.5*fgkEndCapStripConnectionThickness,
											 0.5*fgkEndCapStripConnectionWidth);
 TGeoVolume* endcapstripconnection = new TGeoVolume("EndCapStripConnection",
													endcapstripconnectionshape,
													fSSDSupportRingAl);
 endcapstripconnection->SetLineColor(fColorAl);
 // Interface Card B
 const Int_t kcardBvertexnumber = 12; 
 Double_t xcardBvertexnumber[kcardBvertexnumber];
 Double_t ycardBvertexnumber[kcardBvertexnumber];

 xcardBvertexnumber[0]  = 0.0;
 xcardBvertexnumber[1]  = xcardBvertexnumber[0];
 xcardBvertexnumber[2]  = xcardBvertexnumber[1]+fgkEndCapInterfaceCardBLength[0];
 xcardBvertexnumber[3]  = xcardBvertexnumber[2];
 xcardBvertexnumber[4]  = xcardBvertexnumber[1];
 xcardBvertexnumber[5]  = xcardBvertexnumber[4];
 xcardBvertexnumber[6]  = xcardBvertexnumber[5]+fgkEndCapInterfaceCardBLength[1];
 xcardBvertexnumber[7]  = xcardBvertexnumber[6];
 xcardBvertexnumber[8]  = xcardBvertexnumber[7]-fgkEndCapInterfaceCardBLength[0];
 xcardBvertexnumber[9]  = xcardBvertexnumber[8];
 xcardBvertexnumber[10] = xcardBvertexnumber[7];
 xcardBvertexnumber[11] = xcardBvertexnumber[10];
 
 ycardBvertexnumber[0]  = 0.0;
 ycardBvertexnumber[1]  = fgkEndCapInterfaceCardBWidth[0];
 ycardBvertexnumber[2]  = ycardBvertexnumber[1];
 ycardBvertexnumber[3]  = ycardBvertexnumber[2]+fgkEndCapInterfaceCardBWidth[1];
 ycardBvertexnumber[4]  = ycardBvertexnumber[3];
 ycardBvertexnumber[5]  = ycardBvertexnumber[4]+fgkEndCapInterfaceCardBWidth[2];
 ycardBvertexnumber[6]  = ycardBvertexnumber[5];
 ycardBvertexnumber[7]  = ycardBvertexnumber[4];
 ycardBvertexnumber[8]  = ycardBvertexnumber[7];
 ycardBvertexnumber[9]  = ycardBvertexnumber[1];
 ycardBvertexnumber[10] = ycardBvertexnumber[9];
 ycardBvertexnumber[11] = ycardBvertexnumber[0];

 TGeoXtru* interfacecardBshape = new TGeoXtru(2);
 interfacecardBshape->DefinePolygon(kcardBvertexnumber,xcardBvertexnumber,ycardBvertexnumber);
 interfacecardBshape->DefineSection(0,0.);
 interfacecardBshape->DefineSection(1,fgkEndCapInterfaceCardBThickness);
 TGeoVolume* interfacecardB = new TGeoVolume("EndCapInterfaceCardB",interfacecardBshape,
											 fSSDMountingBlockMedium);
 interfacecardB->SetLineColor(46);
 // Interface Card B Electronic Board
 const Int_t kelectboardcardBvertexnumber = 14; 
 Double_t xelectboardcardBvertex[kelectboardcardBvertexnumber];
 Double_t yelectboardcardBvertex[kelectboardcardBvertexnumber];

 xelectboardcardBvertex[0]  = xcardBvertexnumber[0]+fgkEndCapInterfaceCardBLength[2];
 xelectboardcardBvertex[1]  = xelectboardcardBvertex[0]; 
 xelectboardcardBvertex[2]  = xelectboardcardBvertex[1]+fgkEndCapInterfaceCardBLength[3];
 xelectboardcardBvertex[3]  = xelectboardcardBvertex[2]; 
 xelectboardcardBvertex[4]  = xelectboardcardBvertex[3]+fgkEndCapInterfaceCardBLength[4];
 xelectboardcardBvertex[5]  = xelectboardcardBvertex[4];
 xelectboardcardBvertex[6]  = xelectboardcardBvertex[5]+fgkEndCapInterfaceCardBLength[5];
 xelectboardcardBvertex[7]  = xelectboardcardBvertex[6];
 xelectboardcardBvertex[8]  = xelectboardcardBvertex[7]+8.0*fgkEndCapInterfaceCardBLength[4];
 xelectboardcardBvertex[9]  = xelectboardcardBvertex[8];
 xelectboardcardBvertex[10] = xelectboardcardBvertex[9]+fgkEndCapInterfaceCardBLength[6];
 xelectboardcardBvertex[11] = xelectboardcardBvertex[10];
 xelectboardcardBvertex[12] = xelectboardcardBvertex[11]-9.0*fgkEndCapInterfaceCardBLength[4];
 xelectboardcardBvertex[13] = xelectboardcardBvertex[12];

 yelectboardcardBvertex[0]  = ycardBvertexnumber[0]+fgkEndCapInterfaceCardBWidth[1];
 yelectboardcardBvertex[1]  = yelectboardcardBvertex[0]+fgkEndCapInterfaceCardBWidth[3];
 yelectboardcardBvertex[2]  = yelectboardcardBvertex[1];
 yelectboardcardBvertex[3]  = yelectboardcardBvertex[2]+fgkEndCapInterfaceCardBWidth[3];
 yelectboardcardBvertex[4]  = yelectboardcardBvertex[3];
 yelectboardcardBvertex[5]  = yelectboardcardBvertex[2];
 yelectboardcardBvertex[6]  = yelectboardcardBvertex[5];
 yelectboardcardBvertex[7]  = yelectboardcardBvertex[6]+fgkEndCapInterfaceCardBWidth[4];
 yelectboardcardBvertex[8]  = yelectboardcardBvertex[7];
 yelectboardcardBvertex[9]  = yelectboardcardBvertex[8]-fgkEndCapInterfaceCardBWidth[3];
 yelectboardcardBvertex[10] = yelectboardcardBvertex[9];
 yelectboardcardBvertex[11] = yelectboardcardBvertex[10]-fgkEndCapInterfaceCardBWidth[3];
 yelectboardcardBvertex[12] = yelectboardcardBvertex[11];
 yelectboardcardBvertex[13] = yelectboardcardBvertex[0];

 TGeoXtru* electboardcardBshape = new TGeoXtru(2);
 electboardcardBshape->DefinePolygon(kelectboardcardBvertexnumber,
									 xelectboardcardBvertex,yelectboardcardBvertex);
 electboardcardBshape->DefineSection(0,fgkEndCapInterfaceCardBThickness);
 electboardcardBshape->DefineSection(1,fgkEndCapInterfaceCardBThickness
									 + fgkEndCapInterfaceElectBoardCardBThickness);
 TGeoVolume* electboardcardB = new TGeoVolume("EndCapInterfaceElectBoardCardB",electboardcardBshape,
											  fSSDSupportRingAl);
 electboardcardB->SetLineColor(fColorAl);
 // Generating Stiffener 2
 TGeoBBox* endcapstiffenershape = new TGeoBBox(0.5*fgkEndCapStiffenerWidth,
											   0.5*fgkEndCapStiffenerThickness,
											   0.5*fgkEndCapStiffenerLength);
 TGeoVolume* endcapstiffener = new TGeoVolume("EndCapStiffener",endcapstiffenershape,fSSDSupportRingAl);
 endcapstiffener->SetLineColor(fColorAl);   
 // Generating Mother Interface Card B Container
 const Int_t kinterfacecardBmothervertexnumber = 10;
 Double_t xinterfacecardBmothervertex[kinterfacecardBmothervertexnumber];
 Double_t yinterfacecardBmothervertex[kinterfacecardBmothervertexnumber];

 xinterfacecardBmothervertex[0] = 0.0;
 xinterfacecardBmothervertex[1] = xinterfacecardBmothervertex[0];
 xinterfacecardBmothervertex[2] = xinterfacecardBmothervertex[1]
								+ fgkEndCapInterfaceCardBThickness;
 xinterfacecardBmothervertex[3] = xinterfacecardBmothervertex[2];
 xinterfacecardBmothervertex[4] = xinterfacecardBmothervertex[3]
								+ fgkEndCapInterfaceElectBoardCardBThickness;
 xinterfacecardBmothervertex[5] = xinterfacecardBmothervertex[4];
 xinterfacecardBmothervertex[6] = xinterfacecardBmothervertex[3];
 xinterfacecardBmothervertex[7] = xinterfacecardBmothervertex[6];
 xinterfacecardBmothervertex[8] = xinterfacecardBmothervertex[7]
								+ fgkEndCapCardJMDConnectorLength[0];
 xinterfacecardBmothervertex[9] = xinterfacecardBmothervertex[8];

 yinterfacecardBmothervertex[0] = 0.0;
 yinterfacecardBmothervertex[1] = fgkEndCapInterfaceCardBWidth[0]
								+ fgkEndCapInterfaceCardBWidth[1]
								+ fgkEndCapInterfaceCardBWidth[2];
 yinterfacecardBmothervertex[2] = yinterfacecardBmothervertex[1];
 yinterfacecardBmothervertex[3] = yelectboardcardBvertex[3];
 yinterfacecardBmothervertex[4] = yinterfacecardBmothervertex[3];
 yinterfacecardBmothervertex[5] = yelectboardcardBvertex[11];
 yinterfacecardBmothervertex[6] = yinterfacecardBmothervertex[5];
 yinterfacecardBmothervertex[7] = fgkEndCapCardElectBoardLayerWidth[1]
								+ fgkEndCapCardJMDConnectorWidth[0]
								+ fgkEndCapCardJMDConnectorWidth[1];
 yinterfacecardBmothervertex[8] = yinterfacecardBmothervertex[7];
 yinterfacecardBmothervertex[9] = yinterfacecardBmothervertex[0];
 TGeoXtru* interfacecardBmothershape = new TGeoXtru(2);
 interfacecardBmothershape->DefinePolygon(kinterfacecardBmothervertexnumber,
										  xinterfacecardBmothervertex,
										  yinterfacecardBmothervertex);
 interfacecardBmothershape->DefineSection(0,-1.e-15);
 interfacecardBmothershape->DefineSection(1,fgkEndCapInterfaceCardBLength[1]);
 TGeoVolume* interfacecardBmother = new TGeoVolume("EndCapInterfaceCardBMother",
												   interfacecardBmothershape,fSSDAir);
 electboardcardB->SetLineColor(fColorAl);
 // Positioning Volumes Mother Interface Card B Container 
 TGeoRotation* interfacecardBrot = new TGeoRotation();
 TGeoTranslation* interfacecardBtrans = new TGeoTranslation(); 
 interfacecardBrot->SetAngles(90.,-90.,-90.);
 interfacecardBtrans->SetTranslation(fgkEndCapInterfaceCardBThickness,0.,0.);
 TGeoCombiTrans* interfacecardBcombitrans = new TGeoCombiTrans(*interfacecardBtrans,*interfacecardBrot);
 TGeoRotation* electboardcardBrot = new TGeoRotation();
 TGeoTranslation* electboardcardBtrans = new TGeoTranslation(); 
 electboardcardBrot->SetAngles(90.,90.,-90.);
 electboardcardBtrans->SetTranslation(0.,0.,fgkEndCapInterfaceCardBLength[1]);
 TGeoCombiTrans* electboardcardBcombitrans = 
				  new TGeoCombiTrans(*electboardcardBtrans,*electboardcardBrot);
 interfacecardBmother->AddNode(interfacecardB,1,interfacecardBcombitrans);
 interfacecardBmother->AddNode(electboardcardB,1,electboardcardBcombitrans);
 TGeoRotation* jmdconnectorcardBrot = new TGeoRotation();
 jmdconnectorcardBrot->SetAngles(90.,180.,-90.);
 TGeoTranslation* jmdconnectorcardBtrans[3];
 TGeoCombiTrans* jmdconnectorcardBcombitrans[3];
 for(Int_t i=0; i<3; i++){
   jmdconnectorcardBtrans[i] = new TGeoTranslation(fgkEndCapInterfaceCardBThickness
							 + fgkEndCapCardJMDConnectorLength[0], 
							   fgkEndCapCardElectBoardLayerWidth[1],
							   0.5*fgkEndCapCardJMDConnectorThickness
							 + 0.5*(fgkEndCapInterfaceCardBLength[1]
							 - 2.*fgkEndCapInterfaceCardBJMDConnectorSeparation)
							 + i *fgkEndCapInterfaceCardBJMDConnectorSeparation);	 
   jmdconnectorcardBcombitrans[i] = new TGeoCombiTrans(*jmdconnectorcardBtrans[i],
													   *jmdconnectorcardBrot);
   interfacecardBmother->AddNode(jmdconnector,i+1,jmdconnectorcardBcombitrans[i]);
 }
 // Mother Supply Card Container 
 TGeoVolumeAssembly* mothersupplycard = new TGeoVolumeAssembly("EndCapCardMotherSupply");
 // Interface Card Container
 TGeoVolumeAssembly* motherinterfacecardcontainer = new TGeoVolumeAssembly("EndCapMotherInterfaceCardA");
 // Placing Volumes in Mother Supply Card Container
 // JMD Connector Positioning
 TGeoTranslation* jmdconnectortrans[2];
 for(Int_t i=0; i<2; i++) jmdconnectortrans[i] = new TGeoTranslation();
 jmdconnectortrans[0]->SetTranslation(0.,fgkEndCapCardElectBoardLayerWidth[1],
											fgkEndCapCardElectBoardBackLength[0]
					  -						fgkEndCapCardJMDConnectorThickness
					  -						fgkEndCapCardJMDConnectorToLayer);
 TGeoRotation* jmdconnectorot = new TGeoRotation();
 jmdconnectortrans[1]->SetTranslation(fgkEndCapCardElectBoardBackThickness
								 + 2.*fgkEndCapCardJMDConnectorLength[0]
								 + 2.*fgkEndCapCardElectBoardLayerThickness,
									  fgkEndCapCardElectBoardLayerWidth[1],
								      fgkEndCapCardJMDConnectorThickness
								 +    fgkEndCapCardJMDConnectorToLayer);
 jmdconnectorot->SetAngles(90.,180.,-90);
 TGeoCombiTrans* jmdconnectorcombitrans = new TGeoCombiTrans(*jmdconnectortrans[1],
										* jmdconnectorot);
 mothersupplycard->AddNode(jmdconnector,1,jmdconnectortrans[0]);
 mothersupplycard->AddNode(jmdconnector,2,jmdconnectorcombitrans);
 // Top Cable Connector Placing
 TGeoRotation* cableconnectorot[2];
 for(Int_t i=0; i<2; i++) cableconnectorot[i] = new TGeoRotation();
 TGeoTranslation* cableconnectortrans[3];
 for(Int_t i=0; i<3; i++) cableconnectortrans[i] = new TGeoTranslation();
 cableconnectorot[0]->SetAngles(90.,0.,0.); 
 cableconnectorot[1]->SetAngles(0.,-90.,0.); 
 cableconnectortrans[0]->SetTranslation(fgkEndCapCardJMDConnectorLength[0],0.,0.);
 TGeoCombiTrans* cableconnectorcombitrans = new TGeoCombiTrans(*cableconnectortrans[0],
															   *cableconnectorot[0]);
 TGeoHMatrix* cableconnectormatrix[2];
 for(Int_t i=0; i<2; i++) cableconnectormatrix[i] =
							new TGeoHMatrix((*cableconnectorot[1])
										   *(*cableconnectorcombitrans));
 cableconnectortrans[1]->SetTranslation(0.,fgkEndCapCardElectBoardLayerWidth[0]
					   -				   fgkEndCapCardCableConnectorThickness,
										fgkEndCapCardCableConnectorLength[0]
					   +				fgkEndCapCardCableConnectorToLayer);
 cableconnectortrans[2]->SetTranslation(0.,fgkEndCapCardElectBoardLayerWidth[0]
					   -                2.*fgkEndCapCardCableConnectorThickness
					   -				fgkEndCapCardCableConnectorDistance,
										fgkEndCapCardCableConnectorLength[0]
					   +				fgkEndCapCardCableConnectorToLayer);
 for(Int_t i=0; i<2; i++){
	cableconnectormatrix[i]->MultiplyLeft(cableconnectortrans[i+1]);
    mothersupplycard->AddNode(cableconnector,i+1,cableconnectormatrix[i]);
 }
 TGeoRotation* electboardbackrot = new TGeoRotation(); 
 TGeoTranslation* electboardbacktrans = new TGeoTranslation();
 electboardbackrot->SetAngles(90.,-90.,-90.);
 electboardbacktrans->SetTranslation(fgkEndCapCardElectBoardBackThickness
							+		 fgkEndCapCardJMDConnectorLength[0]
							+		 fgkEndCapCardElectBoardLayerThickness,0.,0.);
 TGeoCombiTrans* electboardbackcombitrans = new TGeoCombiTrans(*electboardbacktrans,
															   *electboardbackrot);
 mothersupplycard->AddNode(electboardback,1,electboardbackcombitrans);
 // Electronic Board Kapton Layer Positioning
 TGeoRotation* electlayerrot = new TGeoRotation();
 TGeoTranslation* electlayertrans[2];
 TGeoCombiTrans* electlayercombitrans[2];
 for(Int_t i=0; i<2; i++) electlayertrans[i] = new TGeoTranslation();
 electlayerrot->SetAngles(90.,-90.,-90.);
 electlayertrans[0]->SetTranslation(fgkEndCapCardJMDConnectorLength[0]
								 + fgkEndCapCardElectBoardLayerThickness,0.,0.);
 electlayertrans[1]->SetTranslation(fgkEndCapCardJMDConnectorLength[0]
								 + 2.*fgkEndCapCardElectBoardLayerThickness
								 + fgkEndCapCardElectBoardBackThickness,0.,0.);
 for(Int_t i=0; i<2; i++){
	electlayercombitrans[i] = new TGeoCombiTrans(*electlayertrans[i],*electlayerrot);
	mothersupplycard->AddNode(electlayer,i+1,electlayercombitrans[i]);
 }
 // Placing Volumes in Mother Interface Card Container
 motherinterfacecardcontainer->AddNode(jmdconnector,1,jmdconnectorcombitrans);
 motherinterfacecardcontainer->AddNode(electboardback,1,electboardbackcombitrans);
 for(Int_t i=0; i<2; i++){
	motherinterfacecardcontainer->AddNode(electlayer,i+1,electlayercombitrans[i]);
 }
 /////////////////////////////////////////////////////////////
 // Generation of Card Interface Container
 /////////////////////////////////////////////////////////////
 Double_t stiffenertransx = fgkEndCapKaptonFoilWidth-fgkEndCapStiffenerWidth
						  - fgkEndCapCardJMDConnectorLength[0]
						  - fgkEndCapInterfaceCardBThickness
						  - 9.*fgkEndCapStripConnectionThickness
						  - 8.*fgkEndCapCardElectBoardBackThickness;
 const Int_t kcardinterfacecontainervertexnumber = 14;
 Double_t xcardinterfacecontainervertex[kcardinterfacecontainervertexnumber];
 Double_t ycardinterfacecontainervertex[kcardinterfacecontainervertexnumber];
 xcardinterfacecontainervertex[0]  =-6.5*fgkEndCapCardElectBoardBackThickness
								   - 7.0*fgkEndCapStripConnectionThickness;
 xcardinterfacecontainervertex[1]  = xcardinterfacecontainervertex[0];
 xcardinterfacecontainervertex[2]  = xcardinterfacecontainervertex[1]
								   + fgkEndCapStripConnectionThickness
								   - fgkEndCapCardElectBoardLayerThickness
								   - fgkEndCapCardCableConnectorWidth[0];
 xcardinterfacecontainervertex[3]  = xcardinterfacecontainervertex[2];
 xcardinterfacecontainervertex[4]  = xcardinterfacecontainervertex[1];
 xcardinterfacecontainervertex[5]  = xcardinterfacecontainervertex[4];
 xcardinterfacecontainervertex[6]  = 1.5*fgkEndCapCardElectBoardBackThickness
								   + 2.0*fgkEndCapStripConnectionThickness;
 xcardinterfacecontainervertex[7]  = xcardinterfacecontainervertex[6];
 xcardinterfacecontainervertex[8]  = xcardinterfacecontainervertex[7]
								   + fgkEndCapInterfaceCardBThickness;
 xcardinterfacecontainervertex[9]  = xcardinterfacecontainervertex[8];
 xcardinterfacecontainervertex[10] = xcardinterfacecontainervertex[9]
								   + fgkEndCapInterfaceElectBoardCardBThickness;
 xcardinterfacecontainervertex[11] = xcardinterfacecontainervertex[10];
 xcardinterfacecontainervertex[12] = xcardinterfacecontainervertex[11]
                                   - fgkEndCapInterfaceElectBoardCardBThickness
								   + fgkEndCapCardJMDConnectorLength[0]
								   + stiffenertransx+fgkEndCapStiffenerWidth;
 xcardinterfacecontainervertex[13] = xcardinterfacecontainervertex[12];								   

 ycardinterfacecontainervertex[0]  = 0.;
 ycardinterfacecontainervertex[1]  = fgkEndCapCardElectBoardLayerWidth[1]
								   + fgkEndCapCardJMDConnectorWidth[0]
								   + fgkEndCapCardJMDConnectorWidth[1];
 ycardinterfacecontainervertex[2]  = ycardinterfacecontainervertex[1];
 ycardinterfacecontainervertex[3]  = fgkEndCapCardElectBoardBackWidth[0]
								   - fgkEndCapStripConnectionWidth;
 ycardinterfacecontainervertex[4]  = ycardinterfacecontainervertex[3];
 ycardinterfacecontainervertex[5]  = fgkEndCapCardElectBoardBackWidth[0];
 ycardinterfacecontainervertex[6]  = ycardinterfacecontainervertex[5];
 ycardinterfacecontainervertex[7]  = fgkEndCapInterfaceCardBWidth[0]
								   + fgkEndCapInterfaceCardBWidth[1]
								   + fgkEndCapInterfaceCardBWidth[2];
 ycardinterfacecontainervertex[8]  = ycardinterfacecontainervertex[7];
 ycardinterfacecontainervertex[9]  = yelectboardcardBvertex[3];
 ycardinterfacecontainervertex[10] = ycardinterfacecontainervertex[9];
 ycardinterfacecontainervertex[11] = ycardinterfacecontainervertex[1];
 ycardinterfacecontainervertex[12] = ycardinterfacecontainervertex[11];
 ycardinterfacecontainervertex[13] = ycardinterfacecontainervertex[0];
 
 TGeoXtru* interfacecardmothershape = new TGeoXtru(2);
 interfacecardmothershape->DefinePolygon(kcardinterfacecontainervertexnumber,
										  xcardinterfacecontainervertex,
										  ycardinterfacecontainervertex);
 interfacecardmothershape->DefineSection(0,-1.e-15-0.5*(fgkEndCapStiffenerLength
									   -    fgkEndCapCardElectBoardBackLength[0]));
 interfacecardmothershape->DefineSection(1,0.5*(fgkEndCapStiffenerLength
									   +    fgkEndCapCardElectBoardBackLength[0]));
 TGeoVolume** cardinterfacecontainer;
 cardinterfacecontainer = new TGeoVolume*[4];
 cardinterfacecontainer[0] = new TGeoVolume("EndCapCardsContainerLayer5Sx",
											interfacecardmothershape,fSSDAir); 
 cardinterfacecontainer[1] = new TGeoVolume("EndCapCardsContainerLayer5Dx",
											interfacecardmothershape,fSSDAir); 
 cardinterfacecontainer[2] = new TGeoVolume("EndCapCardsContainerLayer6Sx",
											interfacecardmothershape,fSSDAir); 
 cardinterfacecontainer[3] = new TGeoVolume("EndCapCardsContainerLayer6Dx",
											interfacecardmothershape,fSSDAir); 
 /////////////////////////////////
 // cardinterfacecontainer[0]: Card Container End Cap Layer 5 Bellegarde Side
 // cardinterfacecontainer[1]: Card Container End Cap Layer 5 Gex Side
 // cardinterfacecontainer[2]: Card Container End Cap Layer 6 Bellegarde Side
 // cardinterfacecontainer[3]: Card Container End Cap Layer 6 Gex Side
 /////////////////////////////////
 TGeoRotation* endcapstripconnectionrot[2];
 for(Int_t i=0; i<2; i++) endcapstripconnectionrot[i] = new TGeoRotation();
 endcapstripconnectionrot[0]->SetAngles(0.,90.,0.);
 endcapstripconnectionrot[1]->SetAngles(90.,90.,-90.);
 TGeoHMatrix* endcapstripconnectionmatrix = new TGeoHMatrix((*endcapstripconnectionrot[1])
									*				  (*endcapstripconnectionrot[0]));
 TGeoTranslation* endcapstripconnectiontrans = new TGeoTranslation();
 endcapstripconnectiontrans->SetTranslation(-endcapstripconnectionshape->GetDY()
											-0.5*fgkEndCapCardElectBoardBackThickness,
											 fgkEndCapCardElectBoardBackWidth[0]
											-endcapstripconnectionshape->GetDZ(),
											 0.5*fgkEndCapCardElectBoardBackLength[0]);
 endcapstripconnectionmatrix->MultiplyLeft(endcapstripconnectiontrans);
 TGeoTranslation* cardinterfacetrans[9];
 TGeoHMatrix* cardinterfacematrix[9]; 
 for(Int_t i=0; i<7; i++){ 
	cardinterfacetrans[i] = new TGeoTranslation(-i*(fgkEndCapStripConnectionThickness
						  +							fgkEndCapCardElectBoardBackThickness),
												0.0,0.0);  
	cardinterfacematrix[i] = new TGeoHMatrix((*cardinterfacetrans[i])
						   *				 (*endcapstripconnectionmatrix));
 }
 cardinterfacetrans[7] = new TGeoTranslation((fgkEndCapStripConnectionThickness
						  +						fgkEndCapCardElectBoardBackThickness),
												0.0,0.0);  
 cardinterfacematrix[7] = new TGeoHMatrix((*cardinterfacetrans[7])
						*				  (*endcapstripconnectionmatrix));
 cardinterfacetrans[8] = new TGeoTranslation(2.*(fgkEndCapStripConnectionThickness
						  +						fgkEndCapCardElectBoardBackThickness),
												0.0,0.0);  
 cardinterfacematrix[8] = new TGeoHMatrix((*cardinterfacetrans[8])
						*				  (*endcapstripconnectionmatrix));

 for(Int_t i=0; i<4; i++){
	cardinterfacecontainer[i]->AddNode(endcapstripconnection,1,
									   cardinterfacematrix[7]);				
	cardinterfacecontainer[i]->AddNode(endcapstripconnection,2,
									   cardinterfacematrix[8]);				
 }
 TGeoTranslation* mothersupplycardtrans = 
					new TGeoTranslation(-0.5*(fgkEndCapCardElectBoardBackThickness
										+ 2.*fgkEndCapCardJMDConnectorLength[0]
										+ 2.*fgkEndCapCardElectBoardLayerThickness),0.0,0.0);
 TGeoHMatrix* mothersupplycardmatrix[7];
 Int_t index[4] = {1,1,1,1};
 for(Int_t i=0; i<7; i++){
	mothersupplycardmatrix[i] = new TGeoHMatrix((*cardinterfacetrans[i])
							*				  (*mothersupplycardtrans));
	for(Int_t j=0; j<4; j++){
		switch(j){
			case 0: //Layer5 EndCap Left Side  
				cardinterfacecontainer[j]->AddNode(endcapstripconnection,i+3,
												   cardinterfacematrix[i]);				
				if(i!=0){
					cardinterfacecontainer[j]->AddNode(mothersupplycard,index[j],
													   mothersupplycardmatrix[i]);			
					index[j]++;

				}
			break;
			case 1: //Layer5 EndCap Rigth Side  
				cardinterfacecontainer[j]->AddNode(endcapstripconnection,i+3,
												   cardinterfacematrix[i]);			
				if(i>0&&i<6){
					cardinterfacecontainer[j]->AddNode(mothersupplycard,index[j],
													   mothersupplycardmatrix[i]);			
					index[j]++;
				}
			break;
			case 2: //Layer6 EndCap Left Side  
				cardinterfacecontainer[j]->AddNode(endcapstripconnection,i+3,
												   cardinterfacematrix[i]);				
				if(i!=6){
					cardinterfacecontainer[j]->AddNode(mothersupplycard,index[j],
													   mothersupplycardmatrix[i]);			
					index[j]++;
				}
			break;
			case 3: //Layer6 EndCap Right Side  
				cardinterfacecontainer[j]->AddNode(endcapstripconnection,i+3,
												   cardinterfacematrix[i]);				
				cardinterfacecontainer[j]->AddNode(mothersupplycard,index[j],
												   mothersupplycardmatrix[i]);			
				index[j]++;
			break;
		}
	}
 }
 // Positioning Interface 
 TGeoTranslation* motherinterfacecardtrans = 
		new TGeoTranslation( -fgkEndCapCardJMDConnectorLength[0]
							 +0.5*fgkEndCapCardElectBoardBackThickness
							 -fgkEndCapCardElectBoardLayerThickness
							 +fgkEndCapStripConnectionThickness,0.,0.);
 for(Int_t i=0; i<4; i++) cardinterfacecontainer[i]->AddNode(
					motherinterfacecardcontainer,i+1,motherinterfacecardtrans);
 // Positioning Interface Card B 
 TGeoTranslation* interfacecardBmothertrans = 
					new TGeoTranslation(0.5 * fgkEndCapCardElectBoardBackThickness
									        + 2.*fgkEndCapStripConnectionThickness
											+ fgkEndCapCardElectBoardBackThickness,0.,
									   -0.5 * (fgkEndCapInterfaceCardBLength[1]
											-  fgkEndCapCardElectBoardBackLength[0])); 				
 for(Int_t i=0; i<4; i++) cardinterfacecontainer[i]->AddNode(interfacecardBmother,1,
															 interfacecardBmothertrans);
 // Positioning Stiffener 
 TGeoTranslation* endcapstiffenertrans = 
						new TGeoTranslation(1.5*fgkEndCapCardElectBoardBackThickness
									   +    2.0*fgkEndCapStripConnectionThickness
									   +    fgkEndCapInterfaceCardBThickness
									   +    fgkEndCapCardJMDConnectorLength[0]
									   +    stiffenertransx
									   +    endcapstiffenershape->GetDX(),endcapstiffenershape->GetDY(),
											endcapstiffenershape->GetDZ()
									   -    0.5*(fgkEndCapStiffenerLength
									   -    fgkEndCapCardElectBoardBackLength[0]));
 for(Int_t i=0; i<4; i++) cardinterfacecontainer[i]->AddNode(endcapstiffener,1,endcapstiffenertrans);  
 /////////////////////////////////////////////////////////////
 // Deallocating memory
 /////////////////////////////////////////////////////////////
 delete interfacecardBrot;
 delete interfacecardBtrans;
 delete electboardcardBtrans;
 delete electboardcardBrot; 
 delete jmdconnectorcardBrot;
 for(Int_t i=0; i<3; i++) delete jmdconnectorcardBtrans[i];
 delete jmdconnectorot;
 delete jmdconnectortrans[1];
 for(Int_t i=0; i<2; i++) delete cableconnectorot[i];
 delete cableconnectorcombitrans;
 delete electboardbacktrans;
 delete electboardbackrot;
 delete electlayerrot;
 for(Int_t i=0; i<2; i++) delete electlayertrans[i];
 for(Int_t i=0; i<2; i++) delete endcapstripconnectionrot[i];
 delete mothersupplycardtrans;
 for(Int_t i=0; i<7; i++) delete cardinterfacetrans[i];
 /////////////////////////////////////////////////////////////
 return cardinterfacecontainer;
 }
 ////////////////////////////////////////////////////////////////////////////////
 TGeoVolume** AliITSv11GeometrySSD::GetEndCapAssembly(){ 
  /////////////////////////////////////////////////////////////
  // Method returning EndCap Mother Volume
  /////////////////////////////////////////////////////////////
  const Int_t kendcapcoverplatesmallholenumber = 9;
  Double_t endcapmotherorigin[3];
  endcapmotherorigin[0] = -fgkEndCapCoverPlateLength[0]
						+  0.5 *(fgkEndCapCoverPlateLength[3]
					    +  2.0 * fgkEndCapCoverPlateLength[2]);
  endcapmotherorigin[1] = - 0.5 * (fgkEndCapCoverPlateWidth[0]
					  -			 fgkEndCapCoverPlateWidth[2]
					  -	  (kendcapcoverplatesmallholenumber-1)
					  *	   fgkEndCapCoverPlateSmallHoleSeparation[2])
					  +  0.5*(fgkEndCapSideCoverLength[2]
					  +		  fgkEndCapCoverPlateWidth[1]
					  -       fgkEndCapCoverPlateWidth[0])
					  -      (fgkEndCapCoverPlateWidth[1]
					  -       fgkEndCapCoverPlateWidth[0]);
  endcapmotherorigin[2] = 0.5*fgkEndCapCoverPlateThickness
						+ 2.*fgkEndCapCoolingTubeRadiusMax
						- 0.5*(2.*fgkEndCapCoolingTubeRadiusMax
						+      fgkEndCapSideCoverWidth[1]
						+      fgkEndCapSideCoverThickness
						+      fgkEndCapKaptonFoilThickness);
  TGeoBBox* endcapmothershape = new TGeoBBox(0.5*(fgkEndCapCoverPlateLength[3]
							  +				 2.0* fgkEndCapCoverPlateLength[2]
							  +              2.0* fgkEndCapSideCoverThickness),
							                 0.5* (fgkEndCapSideCoverLength[2]
							  +                    fgkEndCapCoverPlateWidth[1]
							  -					   fgkEndCapCoverPlateWidth[0]),
											 0.5* (2.*fgkEndCapCoolingTubeRadiusMax
						      +					   fgkEndCapSideCoverWidth[1]
							  +					  fgkEndCapSideCoverThickness
						      +					  fgkEndCapKaptonFoilThickness),
											 endcapmotherorigin);
  TGeoVolume** endcapassembly;  
  endcapassembly = new TGeoVolume*[4];
  endcapassembly[0] = new TGeoVolume("EndCapContainerLayer5Sx",
											endcapmothershape,fSSDAir); 
  endcapassembly[1] = new TGeoVolume("EndCapContainerLayer5Dx",
											endcapmothershape,fSSDAir); 
  endcapassembly[2] = new TGeoVolume("EndCapContainerLayer6Sx",
											endcapmothershape,fSSDAir); 
  endcapassembly[3] = new TGeoVolume("EndCapContainerLayer6Dx",
											endcapmothershape,fSSDAir); 
 /////////////////////////////////
 // endcapassembly[0]:  Container End Cap Layer 5 Bellegarde Side
 // endcapassembly[1]:  Container End Cap Layer 5 Gex Side
 // endcapassembly[2]:  Container End Cap Layer 6 Bellegarde Side
 // endcapassembly[3]:  Container End Cap Layer 6 Gex Side
 /////////////////////////////////
  /////////////////////////////////////////////////////
  // Placing Endcap Cover Plate
  /////////////////////////////////////////////////////
  TGeoVolume* endcapcoverplate = GetEndCapCoverPlate();
  TGeoRotation* endcapcoverplaterot = new TGeoRotation();
  endcapcoverplaterot->SetAngles(90.0,180.0,-90.0);
  TGeoCombiTrans* endcapcoverplatecombitrans = 
						  new TGeoCombiTrans(-0.5*fgkEndCapCoverPlateLength[1],0.,0.,
											 endcapcoverplaterot);
  TGeoTranslation* endcapcoverplatetrans = 
						  new TGeoTranslation(1.5*fgkEndCapCoverPlateLength[1],0.,0.);
  TGeoHMatrix* endcapcoverplatematrix = 
						  new TGeoHMatrix((*endcapcoverplatetrans)
									  *	  (*endcapcoverplatecombitrans));
  for(Int_t i=0; i<4; i++) endcapassembly[i]->AddNode(endcapcoverplate,1,endcapcoverplatematrix);
  /////////////////////////////////////////////////////
  // Placing Endcap Side Cover
  /////////////////////////////////////////////////////
  TGeoVolume* endcapsidecover = GetEndCapSideCover();
  TGeoRotation* endcapsidecoverot[2];
  TGeoCombiTrans* endcapsidecovercombitrans[3];
  for(Int_t i=0; i<2; i++) endcapsidecoverot[i] = new TGeoRotation();
  endcapsidecoverot[0]->SetAngles(-90.,0.,0.);
  endcapsidecovercombitrans[0] = new TGeoCombiTrans(0.0,
											- 0.5*(fgkEndCapCoverPlateWidth[0]
											- fgkEndCapCoverPlateWidth[2]
										    - (kendcapcoverplatesmallholenumber-1)
											* fgkEndCapCoverPlateSmallHoleSeparation[2])
											+ 0.*fgkEndCapCoverPlateWidth[0]
											+ fgkEndCapSideCoverLength[2],
											  0.5*(fgkEndCapSideCoverThickness
											+ fgkEndCapCoverPlateThickness)
											- 0.5*fgkEndCapCoverPlateThickness,
											  endcapsidecoverot[0]);
  endcapsidecoverot[1]->SetAngles(90.,-90.,-90.); 
  endcapsidecovercombitrans[1] = new TGeoCombiTrans(-fgkEndCapCoverPlateLength[0],0.0,
													0.5*fgkEndCapCoverPlateThickness
													-fgkEndCapSideCoverWidth[1],
													endcapsidecoverot[1]);
  endcapsidecovercombitrans[2] = new TGeoCombiTrans(-fgkEndCapCoverPlateLength[0]
													+fgkEndCapCoverPlateLength[3]
													+2.*fgkEndCapCoverPlateLength[2]
													+fgkEndCapSideCoverThickness,0.0,
													0.5*fgkEndCapCoverPlateThickness
													-fgkEndCapSideCoverWidth[1],
													endcapsidecoverot[1]);
  TGeoHMatrix* endcapsidecovermatrix[2];
  for(Int_t i=0; i<2; i++){
   endcapsidecovermatrix[i] = new TGeoHMatrix((*endcapsidecovercombitrans[i+1])
							*				  (*endcapsidecovercombitrans[0]));
	for(Int_t k=0; k<4; k++) endcapassembly[k]->AddNode(endcapsidecover,i+1,
														endcapsidecovermatrix[i]);
  }
  /////////////////////////////////////////////////////
  // Placing Endcap Cooling Tube
  /////////////////////////////////////////////////////
  TGeoVolume* endcapcoolingtube = GetEndCapCoolingTube();
  TGeoRotation* endcapcoolingtuberot = new TGeoRotation();
  endcapcoolingtuberot->SetAngles(0.,180.,0.); 
  TGeoCombiTrans* endcapccolingtubecombitrans 
						= new TGeoCombiTrans(-0.5*(fgkEndCapCoolingTubeAxialRadius[0]
						+ fgkEndCapCoolingTubeAxialRadius[1])
						+ fgkEndCapCoverPlateLength[0]+fgkEndCapCoverPlateLength[1]
						- fgkEndCapCoolingTubeToCoverSide,
						  fgkEndCapCoolingTubeAxialRadius[0],fgkEndCapCoolingTubeRadiusMax
						+ 0.5*fgkEndCapCoverPlateThickness,endcapcoolingtuberot);
  for(Int_t i=0; i<4; i++) endcapassembly[i]->AddNode(endcapcoolingtube,1,
													  endcapccolingtubecombitrans);
  /////////////////////////////////////////////////////
  // Placing Screws 
  /////////////////////////////////////////////////////
  Double_t screwcoverplateradius[2] = {fgkEndCapCoverPlateScrewRadiusMax,
									   fgkEndCapCoverPlateScrewRadiusMin};
  Int_t screwcoverplatedgesnumber[2] = {20,20};
  Double_t screwcoverplatesection[2] = {0.5*fgkEndCapCoverPlateThickness,
										fgkEndCapCoverPlateThickness
									 +  fgkEndCapCoolingTubeRadiusMax};
  TGeoShape* screwcoverplateshape = GetScrewShape(screwcoverplateradius,
												 screwcoverplatedgesnumber,
												 screwcoverplatesection);
  TGeoVolume* screwcoverplate = new TGeoVolume("EndCapScrewCoverPlate",
											   screwcoverplateshape,
											   fSSDCoolingTubePhynox); 
  screwcoverplate->SetLineColor(12);
  Double_t transx[4] = {0,
						fgkEndCapCoverPlateSmallHoleSeparation[0],
						fgkEndCapCoverPlateSmallHoleSeparation[0]
					 +  fgkEndCapCoverPlateSmallHoleSeparation[1],
					 2.*fgkEndCapCoverPlateSmallHoleSeparation[0]
					 +  fgkEndCapCoverPlateSmallHoleSeparation[1]};
  const Int_t kendcapcoverplatescrewnumber[2] = {4,9}; 
//  TGeoTranslation** endcapcoverplatescrewtrans[kendcapcoverplatescrewnumber[0]]; 
  TGeoTranslation*** endcapcoverplatescrewtrans;
  endcapcoverplatescrewtrans = new TGeoTranslation**[kendcapcoverplatescrewnumber[0]]; 
  Int_t index = 0;
  for(Int_t i=0; i<kendcapcoverplatescrewnumber[0]; i++){
	endcapcoverplatescrewtrans[i] = 
					new TGeoTranslation*[kendcapcoverplatescrewnumber[1]];
    for(Int_t j=0; j<kendcapcoverplatescrewnumber[1]; j++){
		index = kendcapcoverplatescrewnumber[1]*i+j+1;
        if(index==1||index==9||index==28||index==36){
			endcapcoverplatescrewtrans[i][j] = 
				new TGeoTranslation(transx[i],
									j*fgkEndCapCoverPlateSmallHoleSeparation[2],
									fgkEndCapSideCoverThickness);
		}
		else{
			endcapcoverplatescrewtrans[i][j] = 
				new TGeoTranslation(transx[i],
									j*fgkEndCapCoverPlateSmallHoleSeparation[2],
									0.);
		}
	    if(index!=19) 
		for(Int_t k=0; k<4; k++) endcapassembly[k]->AddNode(screwcoverplate,index,
											  endcapcoverplatescrewtrans[i][j]);
	}
  }
  /////////////////////////////////////////////////////
  // Placing Cover Plate Clips 
  /////////////////////////////////////////////////////
  TGeoBBox* endcapcoverplateclipshape = new TGeoBBox(0.5*fgkEndCapCoverPlateClipLength,
													 0.5*fgkEndCapCoverPlateClipWidth,
													 0.5*fgkEndCapSideCoverThickness);
  TGeoVolume* endcapcoverplateclip = new TGeoVolume("EndCapCoverPlateUpClip",
													endcapcoverplateclipshape,
													fSSDCoolingTubePhynox);
  TGeoBBox* endcapcoverplatedownclipshape = new TGeoBBox(0.5*fgkEndCapCoverPlateDownClipLength,
													 0.5*fgkEndCapCoverPlateDownClipWidth,
													 0.5*fgkEndCapSideCoverThickness);
  TGeoVolume* endcapcoverplatedownclip = new TGeoVolume("EndCapCoverPlateDownClip",
													endcapcoverplatedownclipshape,
													fSSDCoolingTubePhynox);
  TGeoTranslation* endcapcoverplatecliptrans[4];
  endcapcoverplatecliptrans[0] = new TGeoTranslation(0.5*fgkEndCapCoverPlateClipLength
							   -                     fgkEndCapCoverPlateLength[0]
							   -                     fgkEndCapSideCoverThickness,
													 0.0,
    												 0.5*(fgkEndCapSideCoverThickness
							   +						  fgkEndCapCoverPlateThickness));
  endcapcoverplatecliptrans[1] = new TGeoTranslation(0.5*fgkEndCapCoverPlateClipLength
							   -                     fgkEndCapCoverPlateLength[0]
							   -                     fgkEndCapSideCoverThickness,
													 (kendcapcoverplatescrewnumber[1]-1)
							   *					 fgkEndCapSideCoverWidth[5],
    												 0.5*(fgkEndCapSideCoverThickness
							   +						  fgkEndCapCoverPlateThickness));
  endcapcoverplatecliptrans[2] = new TGeoTranslation(0.5*fgkEndCapCoverPlateClipLength
							   -                     fgkEndCapCoverPlateLength[0]
							   +					 fgkEndCapCoverPlateLength[1]
							   +				  2.*fgkEndCapCoverPlateLength[0]
							   -					 fgkEndCapCoverPlateClipLength
							   +				     fgkEndCapSideCoverThickness,
													 0.0,
    												 0.5*(fgkEndCapSideCoverThickness
							   +						  fgkEndCapCoverPlateThickness));
  endcapcoverplatecliptrans[3] = new TGeoTranslation(0.5*fgkEndCapCoverPlateClipLength
							   -                     fgkEndCapCoverPlateLength[0]
							   +					 fgkEndCapCoverPlateLength[1]
							   +				  2.*fgkEndCapCoverPlateLength[0]
							   -					 fgkEndCapCoverPlateClipLength
							   +				     fgkEndCapSideCoverThickness,
													 (kendcapcoverplatescrewnumber[1]-1)
							   *					 fgkEndCapSideCoverWidth[5],
    												 0.5*(fgkEndCapSideCoverThickness
							   +						  fgkEndCapCoverPlateThickness));
  endcapcoverplateclip->SetLineColor(fColorPhynox);
  endcapcoverplatedownclip->SetLineColor(fColorPhynox);  
  for(Int_t i=0; i<4; i++) 
	for(Int_t j=0; j<4; j++) endcapassembly[j]->AddNode(endcapcoverplateclip,i+1,
												   endcapcoverplatecliptrans[i]);  
  TGeoTranslation* endcapcoverplatedowncliptrans[4];
  endcapcoverplatedowncliptrans[0] = new TGeoTranslation(0.5*fgkEndCapCoverPlateDownClipLength
								   -                     fgkEndCapCoverPlateLength[0]
								   -                     fgkEndCapSideCoverThickness,
								                    0.5*(fgkEndCapCoverPlateDownClipWidth
								   - 				     fgkEndCapCoverPlateClipWidth),
													0.5*(fgkEndCapSideCoverThickness
							       + 					 fgkEndCapCoverPlateThickness)
								   -                     fgkEndCapSideCoverWidth[1]
								   -					 fgkEndCapSideCoverThickness);
  endcapcoverplatedowncliptrans[1] = new TGeoTranslation(0.5*fgkEndCapCoverPlateDownClipLength
								   -                     fgkEndCapCoverPlateLength[0]
								   -                     fgkEndCapSideCoverThickness,
								                    0.5*(fgkEndCapCoverPlateDownClipWidth
								   - 				     fgkEndCapCoverPlateClipWidth)
								   +				fgkEndCapSideCoverLength[2]
								   -				fgkEndCapCoverPlateDownClipWidth,
													0.5*(fgkEndCapSideCoverThickness
							       + 					 fgkEndCapCoverPlateThickness)
								   -                     fgkEndCapSideCoverWidth[1]
								   -					 fgkEndCapSideCoverThickness);
  endcapcoverplatedowncliptrans[2] = new TGeoTranslation(0.5*fgkEndCapCoverPlateDownClipLength
								   -                     fgkEndCapCoverPlateLength[0]
								   +                     fgkEndCapSideCoverThickness
								   +                     fgkEndCapCoverPlateLength[1]
								   +                 2.0*fgkEndCapCoverPlateLength[0]
								   -                     fgkEndCapCoverPlateDownClipLength,
								                    0.5*(fgkEndCapCoverPlateDownClipWidth
								   - 				     fgkEndCapCoverPlateClipWidth),
													0.5*(fgkEndCapSideCoverThickness
							       + 					 fgkEndCapCoverPlateThickness)
								   -                     fgkEndCapSideCoverWidth[1]
								   -					 fgkEndCapSideCoverThickness);
  endcapcoverplatedowncliptrans[3] = new TGeoTranslation(0.5*fgkEndCapCoverPlateDownClipLength
								   -                     fgkEndCapCoverPlateLength[0]
								   +                     fgkEndCapSideCoverThickness
								   +                     fgkEndCapCoverPlateLength[1]
								   +                 2.0*fgkEndCapCoverPlateLength[0]
								   -                     fgkEndCapCoverPlateDownClipLength,
								                    0.5*(fgkEndCapCoverPlateDownClipWidth
								   - 				     fgkEndCapCoverPlateClipWidth)
								   +				     fgkEndCapSideCoverLength[2]
								   -				     fgkEndCapCoverPlateDownClipWidth,
													0.5*(fgkEndCapSideCoverThickness
							       + 					 fgkEndCapCoverPlateThickness)
								   -                     fgkEndCapSideCoverWidth[1]
								   -					 fgkEndCapSideCoverThickness);
  for(Int_t i=0; i<4; i++)
	for(Int_t k=0; k<4; k++) endcapassembly[k]->AddNode(endcapcoverplatedownclip,i+1,
												   endcapcoverplatedowncliptrans[i]);
  /////////////////////////////////////////////////////
  // Placing Kapton Foil
  /////////////////////////////////////////////////////
  TGeoBBox* endcapkaptonfoilshape = new TGeoBBox(0.5*fgkEndCapKaptonFoilLength,
												 0.5*fgkEndCapKaptonFoilWidth,
												 0.5*fgkEndCapKaptonFoilThickness); 
  TGeoVolume* endcapkaptonfoil = new TGeoVolume("EndCapKaptonFoil",
												endcapkaptonfoilshape,
												fSSDKaptonFlexMedium);
  endcapkaptonfoil->SetLineColor(8);
  TGeoTranslation* endcapkaptonfoiltrans = new TGeoTranslation(0.5*fgkEndCapCoverPlateLength[1],
															   0.5*fgkEndCapKaptonFoilWidth
										 -                     0.5*fgkEndCapCoverPlateClipWidth,
															   0.5*fgkEndCapCoverPlateThickness
										 -                     0.5*fgkEndCapKaptonFoilThickness
									     -                     fgkEndCapSideCoverWidth[1]
										 -                     fgkEndCapSideCoverThickness);
  for(Int_t i=0; i<4; i++) endcapassembly[i]->AddNode(endcapkaptonfoil,1,endcapkaptonfoiltrans);
  /////////////////////////////////////////////////////////////
  // Placing Electronic Tubes
  /////////////////////////////////////////////////////////////
  Double_t endcapeffectivecableswidth[2] = {fgkEndCapKaptonFoilWidth
									     - fgkEndCapInterfaceCardBThickness
									     - 9.*fgkEndCapStripConnectionThickness
									     - 8.*fgkEndCapCardElectBoardBackThickness,
									       fgkEndCapKaptonFoilWidth
									     - fgkEndCapInterfaceCardBThickness
									     - 9.*fgkEndCapStripConnectionThickness
									     - 8.*fgkEndCapCardElectBoardBackThickness
										 - fgkEndCapInterfaceElectBoardCardBThickness};
  TGeoVolume* endcapeffectivecables[2];
  endcapeffectivecables[0] = GetEndCapEffectiveCables(fgkEndCapEffectiveCableRadiusMin,
											 fgkEndCapEffectiveCableRadiusMax,
											 endcapeffectivecableswidth[0],
											 10,"EndCapEffectiveCables1"); 
  endcapeffectivecables[1] = GetEndCapEffectiveCables(fgkEndCapEffectiveCableRadiusMin,
											 fgkEndCapEffectiveCableRadiusMax,
											 endcapeffectivecableswidth[1],
											 25,"EndCapEffectiveCables2"); 
  TGeoRotation* endcapeffectivecablesrot = new TGeoRotation();
  TGeoTranslation* endcapeffectivecablestrans[2];
  endcapeffectivecablestrans[0] = new TGeoTranslation(0.5*fgkEndCapCoverPlateLength[1],
					  -							   0.5*endcapeffectivecableswidth[0]
					  -                            0.5*(fgkEndCapCoverPlateWidth[0]
					  -								  fgkEndCapCoverPlateWidth[2]
					  -						(kendcapcoverplatesmallholenumber-1)
					  *						fgkEndCapCoverPlateSmallHoleSeparation[2])
					  +						fgkEndCapSideCoverLength[2],
					  -                     0.5*fgkEndCapCoverPlateThickness
					  -						(fgkEndCapCardElectBoardBackWidth[0]
					  -						 fgkEndCapInterfaceCardBWidth[0]
					  -						 fgkEndCapInterfaceCardBWidth[1]));
  endcapeffectivecablestrans[1] = new TGeoTranslation(0.5*fgkEndCapCoverPlateLength[1],
					  -							   0.5*endcapeffectivecableswidth[1]
					  -                            0.5*(fgkEndCapCoverPlateWidth[0]
					  -								  fgkEndCapCoverPlateWidth[2]
					  -						(kendcapcoverplatesmallholenumber-1)
					  *						fgkEndCapCoverPlateSmallHoleSeparation[2])
					  +	  				    fgkEndCapSideCoverLength[2],
					  -                     0.5*fgkEndCapCoverPlateThickness
					  -						(fgkEndCapCardElectBoardBackWidth[0]
					  -						 fgkEndCapInterfaceCardBWidth[0])
					  -                     0.5*fgkEndCapInterfaceCardBWidth[2]);
  endcapeffectivecablesrot->SetAngles(0.,90.,0.);
  TGeoCombiTrans* endcapeffectivecablescombitrans[2];
  endcapeffectivecablescombitrans[0]  = new TGeoCombiTrans(*endcapeffectivecablestrans[0],
														   *endcapeffectivecablesrot);
  endcapeffectivecablescombitrans[1]  = new TGeoCombiTrans(*endcapeffectivecablestrans[1],
														   *endcapeffectivecablesrot);
//  for(Int_t i=0; i<4; i++) endcapassembly[i]->AddNode(endcapeffectivecables[0],1,
//													  endcapeffectivecablescombitrans[0]);
  for(Int_t i=0; i<4; i++) endcapassembly[i]->AddNode(endcapeffectivecables[1],1,
													  endcapeffectivecablescombitrans[1]);
  /////////////////////////////////////////////////////////////
  // Placing End Cap Cards
  /////////////////////////////////////////////////////////////
  TGeoVolume** endcapcards = GetEndCapCards();
  TGeoRotation* endcapcardsrot[2];
  for(Int_t i=0; i<2; i++) endcapcardsrot[i] = new TGeoRotation();
  endcapcardsrot[0]->SetAngles(90.,0.,0.); 
  TGeoTranslation* endcapcardstrans[2]; 
  endcapcardstrans[0] = new TGeoTranslation(0.,0.,0.5*(fgkEndCapInterfaceCardBLength[1]
											-  fgkEndCapCardElectBoardBackLength[0]));
  TGeoCombiTrans* endcapcardscombitrans = new TGeoCombiTrans(*endcapcardstrans[0],*endcapcardsrot[0]);
  endcapcardsrot[1]->SetAngles(90.,90.,-90.); 
  TGeoHMatrix* endcapcardsmatrix[2];
  endcapcardsmatrix[0] = new TGeoHMatrix((*endcapcardsrot[1])*(*endcapcardscombitrans));
  Double_t stiffenertransx = fgkEndCapKaptonFoilWidth-fgkEndCapStiffenerWidth
						  - fgkEndCapCardJMDConnectorLength[0]
						  - fgkEndCapInterfaceCardBThickness
						  - 9.*fgkEndCapStripConnectionThickness
						  - 8.*fgkEndCapCardElectBoardBackThickness;  
  endcapcardstrans[1] = new TGeoTranslation(-0.5*fgkEndCapStiffenerLength
					  -						fgkEndCapCoverPlateLength[0]
					  + 0.5 *              (fgkEndCapCoverPlateLength[3]
					  + 2.0 *				fgkEndCapCoverPlateLength[2]),	
					  -							stiffenertransx-fgkEndCapStiffenerWidth
					  -								  fgkEndCapCardJMDConnectorLength[0]
					  -								  fgkEndCapInterfaceCardBThickness
					  -	2.0 *						  fgkEndCapStripConnectionThickness
					  - 1.5 *					      fgkEndCapInterfaceCardBThickness
					  - 0.5 *						 (fgkEndCapCoverPlateWidth[0]
					  -								  fgkEndCapCoverPlateWidth[2]
					  -						(kendcapcoverplatesmallholenumber-1)
					  *						fgkEndCapCoverPlateSmallHoleSeparation[2])
					  +                     fgkEndCapKaptonFoilWidth,
      											  0.5*fgkEndCapCoverPlateThickness
					  -							fgkEndCapSideCoverWidth[1]);
  endcapcardsmatrix[1] = new TGeoHMatrix((*endcapcardstrans[1])*(*endcapcardsmatrix[0]));
  for(Int_t i=0; i<4; i++) endcapassembly[i]->AddNode(endcapcards[i],1,endcapcardsmatrix[1]);
   /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  delete endcapcoverplaterot;
  delete endcapcoverplatecombitrans;
  delete endcapcoverplatetrans;
  for(Int_t i=0; i<3; i++){
   delete endcapsidecovercombitrans[i];
   if(i<2) delete endcapsidecoverot[i];	
  }
  for(Int_t i=0; i<2; i++) delete endcapcardsrot[i];
  for(Int_t i=0; i<2; i++) delete endcapcardstrans[i];
  delete endcapcardsmatrix[0];
  return endcapassembly;
 } 
 ////////////////////////////////////////////////////////////////////////////////
 TGeoVolume* AliITSv11GeometrySSD::GetEndCapEffectiveCables(Double_t radiusmin, 
															Double_t radiusmax, 
															Double_t width, 
															Int_t ncables,
															char* volname){
  /////////////////////////////////////////////////////////////
  // Generating EndCap High Voltage Tubes 
  /////////////////////////////////////////////////////////////
  Double_t effectiveinneradius = TMath::Sqrt(ncables)*radiusmin;
  Double_t effectiveouteradius = TMath::Sqrt(TMath::Power(effectiveinneradius,2.)
							   +             TMath::Power(radiusmax,2.)
                               -             TMath::Power(radiusmin,2.));
  TGeoTube* effectiveinnertubeshape = new TGeoTube(0.,effectiveinneradius,0.5*width);
  TGeoTube* effectiveoutertubeshape = new TGeoTube(effectiveinneradius,
												   effectiveouteradius,0.5*width);
  TGeoVolume* effectiveinnertube = new TGeoVolume("EffectiveEndCapInnerTube",
												effectiveinnertubeshape,
												fSSDStiffenerConnectorMedium);
  effectiveinnertube->SetLineColor(41);
  TGeoVolume* effectiveoutertube = new TGeoVolume("EffectiveEndCapOuterTube",
												effectiveoutertubeshape,
												fSSDKaptonChipCableMedium);
  effectiveoutertube->SetLineColor(39);
  TGeoTube* effectivemothertubeshape = new TGeoTube(0.,effectiveouteradius,0.5*width);  
  TGeoVolume* effectivemothertube = new TGeoVolume(volname,effectivemothertubeshape,fSSDAir);
  effectivemothertube->AddNode(effectiveinnertube,1);
  effectivemothertube->AddNode(effectiveoutertube,1);
  return effectivemothertube;
 } 
 ////////////////////////////////////////////////////////////////////////////////
 TGeoVolume** AliITSv11GeometrySSD::EndCapSupport(){ 
  /////////////////////////////////////////////////////////////
  // Generating EndCap Support Layer 5 and Layer 6 
  /////////////////////////////////////////////////////////////
  const Int_t knedges = 5;
  ///////////////////////////////////////////////
  // Setting the vertices for TGeoXtru Up Volume
  ///////////////////////////////////////////////
  const Int_t klayernumber = 2;
  Double_t xupvertex[klayernumber][knedges+3];
  Double_t yupvertex[klayernumber][knedges+3];
  Double_t upedgeangle[klayernumber] = {360./fgkSSDLay5LadderNumber,360./fgkSSDLay6LadderNumber};
  Double_t middledgeangle[klayernumber] = {0.0,0.0};
  Double_t middlepsi[klayernumber] = {0.0,0.0};
  for(Int_t i=0; i<klayernumber; i++){
	xupvertex[i][0] = -fgkEndCapSupportMiddleRadius[i]*SinD(0.5*upedgeangle[i]);
	xupvertex[i][1] = -0.5*fgkEndCapSupportLength[i];
	xupvertex[i][2] = -xupvertex[i][1];
	xupvertex[i][3] = -xupvertex[i][0];

	yupvertex[i][0] =  fgkEndCapSupportMiddleRadius[i]*CosD(0.5*upedgeangle[i]);
	yupvertex[i][1] =  0.5*fgkEndCapSupportLength[i]/TanD(0.5*upedgeangle[i]);
	yupvertex[i][2] =  yupvertex[i][1];
	yupvertex[i][3] =  yupvertex[i][0];
	
    middledgeangle[i] = upedgeangle[i]/knedges;
    middlepsi[i] = 90.0-0.5*upedgeangle[i];
    for(Int_t j=1; j<knedges; j++){
		xupvertex[i][j+3] = fgkEndCapSupportMiddleRadius[i]*CosD(middlepsi[i]+j*middledgeangle[i]);
		yupvertex[i][j+3] = fgkEndCapSupportMiddleRadius[i]*SinD(middlepsi[i]+j*middledgeangle[i]);
	}
  }
  ////////////////////////////////////
  // Generating Up TGeoXtru
  ////////////////////////////////////
  TGeoXtru* upendcapsupportshape[klayernumber];
  TGeoVolume* upendcapsupport[klayernumber]; 
  char upendcapsupportname[30]; 
  for(Int_t i=0; i<klayernumber; i++){
   upendcapsupportshape[i] = new TGeoXtru(2);
   sprintf(upendcapsupportname,"UpEndCapSupportPieceLayer%i",i+5);
   upendcapsupportshape[i]->DefinePolygon(knedges+3,xupvertex[i],yupvertex[i]); 
   upendcapsupportshape[i]->DefineSection(0,0.);
   upendcapsupportshape[i]->DefineSection(1,fgkEndCapSupportHighWidth);
   upendcapsupport[i] = new TGeoVolume(upendcapsupportname,upendcapsupportshape[i],
									fSSDSupportRingAl);
   upendcapsupport[i]->SetLineColor(5);
  }
  ///////////////////////////////////////////////
  // Setting the vertices for TGeoXtru Down Volume
  ///////////////////////////////////////////////
  Double_t xdownvertex[klayernumber][2*(knedges+1)];
  Double_t ydownvertex[klayernumber][2*(knedges+1)];
  for(Int_t i=0; i<klayernumber; i++){
	xdownvertex[i][0] = -fgkEndCapSupportLowRadius[i]*SinD(0.5*upedgeangle[i]);
	xdownvertex[i][1] =  xupvertex[i][0];
	ydownvertex[i][0] = fgkEndCapSupportLowRadius[i]*CosD(0.5*upedgeangle[i]);
	ydownvertex[i][1] =  yupvertex[i][0];
	for(Int_t j=0; j<knedges; j++){
		xdownvertex[i][j+2] = xupvertex[i][knedges+2-j];
		ydownvertex[i][j+2] = yupvertex[i][knedges+2-j];
	} 
	for(Int_t j=0; j<knedges; j++){
		xdownvertex[i][knedges+j+2] = fgkEndCapSupportLowRadius[i]
									* CosD(middlepsi[i]+j*middledgeangle[i]);
		ydownvertex[i][knedges+j+2] = fgkEndCapSupportLowRadius[i]
									* SinD(middlepsi[i]+j*middledgeangle[i]);
	}
  }
  ////////////////////////////////////
  // Generating Down TGeoXtru
  ////////////////////////////////////  
  TGeoXtru* downendcapsupportshape[klayernumber];
  TGeoVolume* downendcapsupport[klayernumber]; 
  char downendcapsupportname[30]; 
  for(Int_t i=0; i<klayernumber; i++){
	downendcapsupportshape[i] = new TGeoXtru(2);
    sprintf(downendcapsupportname,"DownEndCapSupportPieceLayer%i",i+5);
	downendcapsupportshape[i] = new TGeoXtru(2);
	downendcapsupportshape[i]->DefinePolygon(2*(knedges+1),xdownvertex[i],ydownvertex[i]); 
    if(i==0){
		downendcapsupportshape[i]->DefineSection(0,0.);
		downendcapsupportshape[i]->DefineSection(1,fgkEndCapSupportLowWidth[i]);
    }
	else{
		downendcapsupportshape[i]->DefineSection(0,fgkEndCapSupportHighWidth
								 -                 fgkEndCapSupportLowWidth[i]);
		downendcapsupportshape[i]->DefineSection(1,fgkEndCapSupportHighWidth);
	}
    downendcapsupport[i] = new TGeoVolume(downendcapsupportname,
								downendcapsupportshape[i],fSSDSupportRingAl);
	downendcapsupport[i]->SetLineColor(5);
  }
  ///////////////////////////////////////////////
  // Setting TGeoPgon Volume
  ///////////////////////////////////////////////
  const Int_t kssdlayladdernumber[klayernumber] = {fgkSSDLay5LadderNumber,
												   fgkSSDLay6LadderNumber};
  TGeoPgon* endcapsupportmothershape[klayernumber];
  TGeoVolume** endcapsupportmother;
  endcapsupportmother = new TGeoVolume*[klayernumber];
  char endcapsupportmothername[30];
  for(Int_t i=0; i<klayernumber; i++){
	endcapsupportmothershape[i] = new TGeoPgon(0.0,360.0,kssdlayladdernumber[i],2);
    sprintf(endcapsupportmothername,"EndCapSupportMotherLayer%i",i+5);
	endcapsupportmothershape[i]->DefineSection(0,0.,ydownvertex[i][0],yupvertex[i][1]);	
    endcapsupportmothershape[i]->DefineSection(1,fgkEndCapSupportHighWidth,
											  ydownvertex[i][0],yupvertex[i][1]);
    endcapsupportmother[i] = new TGeoVolume(endcapsupportmothername,endcapsupportmothershape[i],
											fSSDAir);	
  }
  ////////////////////////////////////
  TGeoRotation** endcapsupportrot[klayernumber];
  for(Int_t i=0; i<2; i++){
	endcapsupportrot[i] = new TGeoRotation*[kssdlayladdernumber[i]];	
	for(Int_t j=0; j<kssdlayladdernumber[i]; j++){
	   endcapsupportrot[i][j] = new TGeoRotation();
	   endcapsupportrot[i][j]->SetAngles(j*upedgeangle[i],0.,0.);
       endcapsupportmother[i]->AddNode(upendcapsupport[i],j+1,endcapsupportrot[i][j]);
       endcapsupportmother[i]->AddNode(downendcapsupport[i],j+1,endcapsupportrot[i][j]);
	}
  }
  return endcapsupportmother;
 } 
 ////////////////////////////////////////////////////////////////////////////////
 void AliITSv11GeometrySSD::SetEndCapSupportAssembly(){
  /////////////////////////////////////////////////////////////
  // Setting End Cap Support Layer 5 and 6. 
  /////////////////////////////////////////////////////////////
  const Int_t kendcapcoverplatesmallholenumber = 9;
  const Int_t klayernumber = 2;
  const Int_t kssdlayladdernumber[klayernumber] = {fgkSSDLay5LadderNumber,
												   fgkSSDLay6LadderNumber};
  Double_t upedgeangle[klayernumber] = {360.0/kssdlayladdernumber[0],
										360.0/kssdlayladdernumber[1]};
  TGeoVolume** endcapsupport = EndCapSupport();
  TGeoVolume** endcapassembly = GetEndCapAssembly();
  TGeoPgon* endcapsupportshape[klayernumber];
  Double_t* radiusmin[klayernumber];
  Double_t* radiusmax[klayernumber];
  for(Int_t i=0; i<klayernumber; i++){
    endcapsupportshape[i] = (TGeoPgon*)endcapsupport[i]->GetShape();
	radiusmin[i] = endcapsupportshape[i]->GetRmin();
	radiusmax[i] = endcapsupportshape[i]->GetRmax();
  }  
  TGeoBBox* endcapassemblyshape = (TGeoBBox*)endcapassembly[0]->GetShape();
  Double_t endcapassemblycenter[3] = {endcapassemblyshape->GetDX(),
									  endcapassemblyshape->GetDY(),
									  endcapassemblyshape->GetDZ()};
  ///////////////////////////////////////////////
  // Setting TGeoPgon Volume for Mother Container
  ///////////////////////////////////////////////
  TGeoPgon* endcapsupportsystemshape[klayernumber];
  char endcapsupportsystemothername[30];
  for(Int_t i=0; i<klayernumber; i++){
	endcapsupportsystemshape[i] = new TGeoPgon(0.0,360.0,kssdlayladdernumber[i],2);
    sprintf(endcapsupportsystemothername,"EndCapSupportSystemLayer%i",i+5);
	endcapsupportsystemshape[i]->DefineSection(0,-(fgkEndCapCoverPlateWidth[1]
											     - fgkEndCapCoverPlateWidth[0]),*radiusmin[i],
											  (*radiusmax[i]*CosD(0.5*upedgeangle[i])
											   +2.*endcapassemblycenter[2])
											   /CosD(0.5*upedgeangle[i]));	
    endcapsupportsystemshape[i]->DefineSection(1,2.*endcapassemblycenter[1]
												 -(fgkEndCapCoverPlateWidth[1]
											     - fgkEndCapCoverPlateWidth[0]),
											   *radiusmin[i],
											  (*radiusmax[i]*CosD(0.5*upedgeangle[i])
											   +2.*endcapassemblycenter[2])
											   /CosD(0.5*upedgeangle[i]));
  }
  fgkEndCapSupportSystem = new TGeoVolume*[4];
  fgkEndCapSupportSystem[0] = new TGeoVolume("EndCapSupportSystemLayer5Sx",
									  endcapsupportsystemshape[0],fSSDAir);	
  fgkEndCapSupportSystem[1] = new TGeoVolume("EndCapSupportSystemLayer5Dx",
									  endcapsupportsystemshape[0],fSSDAir);	
  fgkEndCapSupportSystem[2] = new TGeoVolume("EndCapSupportSystemLayer6Sx",
									  endcapsupportsystemshape[1],fSSDAir);	
  fgkEndCapSupportSystem[3] = new TGeoVolume("EndCapSupportSystemLayer6Dx",
									  endcapsupportsystemshape[1],fSSDAir);	
  ///////////////////////////////////////////////
  TGeoTranslation* endcapassemblytrans[klayernumber];
  for(Int_t i=0; i<klayernumber; i++)
	endcapassemblytrans[i] = new TGeoTranslation(-fgkEndCapCoverPlateLength[0]
									   -  fgkEndCapSideCoverThickness
									   +  endcapassemblycenter[0],
									   -  0.5*fgkEndCapCoverPlateThickness
									   -  2.0*fgkEndCapCoolingTubeRadiusMax
									   +  2.0*endcapassemblycenter[2]
									   +  0.5*fgkEndCapSupportLength[i]
									   /  TanD(0.5*upedgeangle[i]),
										  0.5*(fgkEndCapCoverPlateWidth[0]
									   -  fgkEndCapCoverPlateWidth[2]
									   - (kendcapcoverplatesmallholenumber-1)
									   *  fgkEndCapCoverPlateSmallHoleSeparation[2]));
  TGeoRotation** endcapassemblyrot[klayernumber];
  TGeoHMatrix** endcapassemblymatrix[klayernumber];
  for(Int_t i=0; i<klayernumber; i++){
   endcapassemblyrot[i] = new TGeoRotation*[kssdlayladdernumber[i]+2];
   endcapassemblymatrix[i] = new TGeoHMatrix*[kssdlayladdernumber[i]+2];	
   for(Int_t j=0; j<kssdlayladdernumber[i]+2; j++) endcapassemblyrot[i][j] = new TGeoRotation();
   endcapassemblyrot[i][0]->SetAngles(0.,-90.,0.);	
   endcapassemblyrot[i][1]->SetAngles(90.,180.,-90.);	
   endcapassemblymatrix[i][0] = new TGeoHMatrix((*endcapassemblyrot[i][1])*(*endcapassemblyrot[i][0]));
   endcapassemblymatrix[i][1] = new TGeoHMatrix((*endcapassemblytrans[i])*(*endcapassemblymatrix[i][0]));
   for(Int_t j=0; j<kssdlayladdernumber[i]; j++){
	endcapassemblyrot[i][j+2]->SetAngles(j*upedgeangle[i],0.,0.); 
	endcapassemblymatrix[i][j+2] = new TGeoHMatrix((*endcapassemblyrot[i][j+2])*(*endcapassemblymatrix[i][1]));
   }
  }
  TGeoTranslation* lay6endcapassemblytrans = new TGeoTranslation(0.,0.,
							fgkEndCapKaptonFoilWidth-fgkEndCapSupportHighWidth);
  for(Int_t i=0; i<2*klayernumber; i++){
	for(Int_t j=0; j<(i<2? kssdlayladdernumber[0]:kssdlayladdernumber[1]); j++){
		fgkEndCapSupportSystem[i]->AddNode(endcapassembly[i],j+1,i<2?endcapassemblymatrix[0][j+2]:
																	   endcapassemblymatrix[1][j+2]);
	}
	fgkEndCapSupportSystem[i]->AddNode(i<2?endcapsupport[0]:endcapsupport[1],1,i<2?0:lay6endcapassemblytrans);
  }
   /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<klayernumber; i++){
	for(Int_t j=0; j<kssdlayladdernumber[i]+2; j++){
		delete endcapassemblyrot[i][j];
	}
	delete endcapassemblyrot[i];
	delete endcapassemblymatrix[i][0];
	delete endcapassemblymatrix[i][1];
  }
  /////////////////////////////////////////////////////////////
  }
  void AliITSv11GeometrySSD::EndCapSupportSystemLayer5(TGeoVolume* moth){
  /////////////////////////////////////////////////////////////
  // Setting End Cap Support + End Cap Assembly of Layer 5. 
  /////////////////////////////////////////////////////////////
  if (! moth) {
    printf("Error::AliITSv11GeometrySSD: Can't insert end cap support of layer5, mother is null!\n");
    return;
  };
  if(!fgkEndCapSupportSystem) SetEndCapSupportAssembly();
  TGeoTranslation* endcapsupportsystemITSCentertrans[2];
  endcapsupportsystemITSCentertrans[0] = new TGeoTranslation(0.,0.,
												fgkEndCapSupportCenterLay5ITSPosition
									   +		fgkEndCapSupportCenterLay5Position
									   -		fgkEndCapSideCoverLength[2]);
  endcapsupportsystemITSCentertrans[1] = new TGeoTranslation(0.,0.,
												fgkEndCapSideCoverLength[2]
									   -        fgkEndCapSupportCenterLay5Position
									   -        fgkEndCapSupportCenterLay5ITSPosition);
  TGeoRotation* endcapsupportsystemrot = new TGeoRotation();
  endcapsupportsystemrot->SetAngles(90.,180.,-90.);
  TGeoCombiTrans* endcapsupportsystemITSCentercombitrans = 
	new TGeoCombiTrans(*endcapsupportsystemITSCentertrans[1],*endcapsupportsystemrot);
  moth->AddNode(fgkEndCapSupportSystem[0],1,endcapsupportsystemITSCentertrans[0]);
  moth->AddNode(fgkEndCapSupportSystem[1],1,endcapsupportsystemITSCentercombitrans);
   /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  delete endcapsupportsystemrot;
  delete endcapsupportsystemITSCentertrans[1];
 }
  /////////////////////////////////////////////////////////////
  void AliITSv11GeometrySSD::EndCapSupportSystemLayer6(TGeoVolume* moth){
  /////////////////////////////////////////////////////////////
  // Setting End Cap Support + End Cap Assembly of Layer 6. 
  /////////////////////////////////////////////////////////////
  if (! moth) {
    printf("Error::AliITSv11GeometrySSD: Can't insert end cap support of layer6, mother is null!\n");
    return;
  };
  if(!fgkEndCapSupportSystem) SetEndCapSupportAssembly();
  TGeoTranslation* endcapsupportsystemITSCentertrans[2];
  endcapsupportsystemITSCentertrans[0] = new TGeoTranslation(0.,0.,
												fgkEndCapSupportCenterLay6ITSPosition
									   +		fgkEndCapSupportCenterLay6Position
									   -		fgkEndCapSideCoverLength[2]);
  endcapsupportsystemITSCentertrans[1] = new TGeoTranslation(0.,0.,
												fgkEndCapSideCoverLength[2]
									   -        fgkEndCapSupportCenterLay6Position
									   -        fgkEndCapSupportCenterLay6ITSPosition);
  TGeoRotation* endcapsupportsystemrot = new TGeoRotation();
  endcapsupportsystemrot->SetAngles(90.,180.,-90.);
  TGeoCombiTrans* endcapsupportsystemITSCentercombitrans = 
	new TGeoCombiTrans(*endcapsupportsystemITSCentertrans[1],*endcapsupportsystemrot);
  moth->AddNode(fgkEndCapSupportSystem[2],1,endcapsupportsystemITSCentertrans[0]);
  moth->AddNode(fgkEndCapSupportSystem[3],1,endcapsupportsystemITSCentercombitrans);
   /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  delete endcapsupportsystemrot;
  delete endcapsupportsystemITSCentertrans[1];
 }
 ////////////////////////////////////////////////////////////////////////////////
 void AliITSv11GeometrySSD::LadderSupportLayer5(TGeoVolume* moth){
  /////////////////////////////////////////////////////////////
  // Setting Ladder Support of Layer 5. 
  /////////////////////////////////////////////////////////////
  if (! moth) {
    printf("Error::AliITSv11GeometrySSD: Can't insert ladder lupport of layer5, mother is null!\n");
    return;
  };
  if(!fLay5LadderSupportRing) SetLadderSupport(100);
  fMotherVol = moth;
  TGeoTranslation* centerITSRingSupportLay5trans[2];
  for(Int_t i=0; i<2; i++){
	centerITSRingSupportLay5trans[i] = 
		new TGeoTranslation(0.,0.,TMath::Power(-1.,i)*fgkLadderSupportRingLay5Position);
    moth->AddNode(fLay5LadderSupportRing,i+1,centerITSRingSupportLay5trans[i]);
  }
 }
 ////////////////////////////////////////////////////////////////////////////////
 void AliITSv11GeometrySSD::LadderSupportLayer6(TGeoVolume* moth){
  /////////////////////////////////////////////////////////////
  // Setting Ladder Support of Layer 6. 
  /////////////////////////////////////////////////////////////
  if (! moth) {
    printf("Error::AliITSv11GeometrySSD: Can't insert ladder lupport of layer6, mother is null!\n");
    return;
  };
  if(!fLay6LadderSupportRing) SetLadderSupport(100);
  fMotherVol = moth;
  TGeoTranslation* centerITSRingSupportLay6trans[2];
  for(Int_t i=0; i<2; i++){
	centerITSRingSupportLay6trans[i] = 
		new TGeoTranslation(0.,0.,TMath::Power(-1.,i)*fgkLadderSupportRingLay6Position);
    moth->AddNode(fLay6LadderSupportRing,i+1,centerITSRingSupportLay6trans[i]);
  }
 }
 ////////////////////////////////////////////////////////////////////////////////
 void AliITSv11GeometrySSD::SSDCone(TGeoVolume* moth){
  /////////////////////////////////////////////////////////////
  // Setting Ladder Support of Layer 6. 
  /////////////////////////////////////////////////////////////
  if (! moth) {
    printf("Error::AliITSv11GeometrySSD: Can't insert SSD Cone, mother is null!\n");
    return;
  };
  if(!fSSDCone) SetSSDCone();
  TGeoTranslation* ssdconetrans = new TGeoTranslation(0.,0.,0.5*fgkSSDCentralSupportLength
								+					  fgkSSDCentralAL3SupportLength);
    moth->AddNode(fSSDCone,1,ssdconetrans);
}
 ////////////////////////////////////////////////////////////////////////////////
 void AliITSv11GeometrySSD::SetSSDCone(){
  /////////////////////////////////////////////////////////////
  // Method generating SSDCone 
  /////////////////////////////////////////////////////////////
  if(!fCreateMaterials) CreateMaterials();
  fSSDCone = new TGeoVolumeAssembly("ITSssdCone");
  Double_t ssdpconesectionradiusmax[16];
  Double_t ssdpconesectionradiusmin[16];
  Double_t ssdpconezsection[16];
  TGeoPcon* ssdpconelittleholeshape[8];
  TGeoVolume* ssdpconelittlehole[8];
  ssdpconezsection[0] = (fgkSSDPConeZLength[0]-fgkSSDPConeZLength[1]);
  ssdpconesectionradiusmin[0] = fgkSSDLowerPConeRadius;
  ssdpconesectionradiusmax[0] = ssdpconezsection[0]*CosD(fgkSSDPConeAngle)
						      / SinD(fgkSSDPConeAngle)
							  + ssdpconesectionradiusmin[0];
  ssdpconesectionradiusmin[1] = fgkSSDPConeLittleHoleRadius
							  - ssdpconezsection[0]*CosD(fgkSSDPConeAngle)
							  / SinD(fgkSSDPConeAngle);
  ssdpconesectionradiusmax[1] = fgkSSDPConeLittleHoleRadius; 
  ssdpconezsection[1] = (ssdpconesectionradiusmin[1]-ssdpconesectionradiusmin[0])
					  * TanD(fgkSSDPConeAngle)+ssdpconezsection[0];
  ssdpconelittleholeshape[0] = new TGeoPcon(0.,360.,2);    
  for(Int_t i=0; i<2;i++) ssdpconelittleholeshape[0]->DefineSection(i,ssdpconezsection[i],
						  ssdpconesectionradiusmin[i],ssdpconesectionradiusmax[i]); 
  ssdpconelittlehole[0] = new TGeoVolume("SSDConeLittleHole1",ssdpconelittleholeshape[0],fSSDCarbonFiberMedium);
  ssdpconelittlehole[0]->SetLineColor(4);
  /////////////////////////////////////////////////////////////
  ssdpconezsection[2] = ssdpconezsection[1];  
  ssdpconesectionradiusmin[2] = ssdpconesectionradiusmin[1];
  ssdpconesectionradiusmax[2] = ssdpconesectionradiusmax[1];
  ssdpconesectionradiusmin[3] = fgkSSDPConeLittleHoleRadius+fgkSSDPConeLittleHoleLength
							  - ssdpconezsection[0]*CosD(fgkSSDPConeAngle)
							  / SinD(fgkSSDPConeAngle);
  ssdpconesectionradiusmax[3] = ssdpconezsection[0]*CosD(fgkSSDPConeAngle)
							  / SinD(fgkSSDPConeAngle)+ssdpconesectionradiusmin[3];
  ssdpconezsection[3] = (ssdpconesectionradiusmin[3]-ssdpconesectionradiusmin[2])
					  * TanD(fgkSSDPConeAngle)+ssdpconezsection[2];
  Double_t ssdpconelittleholeangle = fgkSSDPConeLittleHoleLength/fgkSSDPConeLittleHoleRadius
								   * TMath::RadToDeg();
  ssdpconelittleholeshape[1] = new TGeoPcon(30.+0.5*ssdpconelittleholeangle,
													  60.-ssdpconelittleholeangle,2);    
  for(Int_t i=2;i<4;i++) ssdpconelittleholeshape[1]->DefineSection(i-2,ssdpconezsection[i],
						  ssdpconesectionradiusmin[i],ssdpconesectionradiusmax[i]); 
  ssdpconelittlehole[1] = new TGeoVolume("SSDConeLittleHole2",ssdpconelittleholeshape[1],fSSDCarbonFiberMedium);
  ssdpconelittlehole[1]->SetLineColor(4);
  TGeoRotation* ssdconelittleholerot[6];
  for(Int_t i=0; i<6; i++){
	ssdconelittleholerot[i] = new TGeoRotation();
    ssdconelittleholerot[i]->SetAngles(i*60,0.,0.);
  }
  /////////////////////////////////////////////////////////////
  ssdpconezsection[4] = ssdpconezsection[3];  
  ssdpconesectionradiusmin[4] = ssdpconesectionradiusmin[3];
  ssdpconesectionradiusmax[4] = ssdpconesectionradiusmax[3];
  ssdpconesectionradiusmin[5] = fgkSSDConeMiddleRadius-ssdpconezsection[0]
							  * CosD(fgkSSDPConeAngle)
							  / SinD(fgkSSDPConeAngle);
  ssdpconesectionradiusmax[5] = fgkSSDConeMiddleRadius;
  ssdpconezsection[5] = (ssdpconesectionradiusmin[5]-ssdpconesectionradiusmin[4])
					  * TanD(fgkSSDPConeAngle)+ssdpconezsection[4];
  ssdpconelittleholeshape[2] = new TGeoPcon(0.,360.,2);
  for(Int_t i=4; i<6;i++) ssdpconelittleholeshape[2]->DefineSection(i-4,ssdpconezsection[i],
						  ssdpconesectionradiusmin[i],ssdpconesectionradiusmax[i]); 
  ssdpconelittlehole[2] = new TGeoVolume("SSDConeLittleHole3",ssdpconelittleholeshape[2],fSSDCarbonFiberMedium);
  ssdpconelittlehole[2]->SetLineColor(4);
  ///////////////////////////////////////////////////
  ssdpconezsection[6] = ssdpconezsection[5];  
  ssdpconesectionradiusmin[6] = ssdpconesectionradiusmin[5];
  ssdpconesectionradiusmax[6] = ssdpconesectionradiusmax[5];
  ssdpconesectionradiusmin[7] = fgkSSDConeMiddleRadius+fgkSSDPConeMiddleLength
							  -ssdpconezsection[0]
							  * CosD(fgkSSDPConeAngle)
							  / SinD(fgkSSDPConeAngle);
  ssdpconesectionradiusmax[7] = fgkSSDConeMiddleRadius+fgkSSDPConeMiddleLength;
  ssdpconezsection[7] = (ssdpconesectionradiusmin[7]-ssdpconesectionradiusmin[6])
					  * TanD(fgkSSDPConeAngle)+ssdpconezsection[6];
  Double_t ssdpconemiddleholeangle = fgkSSDPConeMiddleWidth/fgkSSDConeMiddleRadius
								   * TMath::RadToDeg();
  ssdpconelittleholeshape[3] = new TGeoPcon(22.5+0.5*ssdpconemiddleholeangle,
													  45.-ssdpconemiddleholeangle,2);    
  for(Int_t i=6;i<8;i++) ssdpconelittleholeshape[3]->DefineSection(i-6,ssdpconezsection[i],
						  ssdpconesectionradiusmin[i],ssdpconesectionradiusmax[i]); 
  ssdpconelittlehole[3] = new TGeoVolume("SSDConeLittleHole4",ssdpconelittleholeshape[3],fSSDCarbonFiberMedium);
  ssdpconelittlehole[3]->SetLineColor(4);
  TGeoRotation* ssdconemiddleholerot[8];
  for(Int_t i=0; i<8; i++){
	ssdconemiddleholerot[i] = new TGeoRotation();
    ssdconemiddleholerot[i]->SetAngles(i*45,0.,0.);
  }
  /////////////////////////////////////////////////////////////
  ssdpconezsection[8] = ssdpconezsection[7];  
  ssdpconesectionradiusmin[8] = ssdpconesectionradiusmin[7];
  ssdpconesectionradiusmax[8] = ssdpconesectionradiusmax[7];
  ssdpconesectionradiusmin[9] = fgkSSDPConeUpRadius-ssdpconezsection[0]
							  * CosD(fgkSSDPConeAngle)
							  / SinD(fgkSSDPConeAngle);
  ssdpconesectionradiusmax[9] = fgkSSDPConeUpRadius;
  ssdpconezsection[9] = (ssdpconesectionradiusmin[9]-ssdpconesectionradiusmin[8])
					  * TanD(fgkSSDPConeAngle)+ssdpconezsection[8];
  ssdpconelittleholeshape[4] = new TGeoPcon(0.,360.,2);
  for(Int_t i=8; i<10;i++) ssdpconelittleholeshape[4]->DefineSection(i-8,ssdpconezsection[i],
						  ssdpconesectionradiusmin[i],ssdpconesectionradiusmax[i]); 
  ssdpconelittlehole[4] = new TGeoVolume("SSDConeLittleHole5",ssdpconelittleholeshape[4],fSSDCarbonFiberMedium);
  ssdpconelittlehole[4]->SetLineColor(4);
  /////////////////////////////////////////////////////////////
  Double_t ssdconetrapezoidheight = fgkSSDPConeUpMaxRadius-fgkSSDPConeUpRadius;
  Double_t ssdconetrapezoidbasis = fgkSSDPConeTrapezoidBasis-2.0
								 * (0.5*ssdconetrapezoidheight/TanD(fgkSSDPConeTrapezoidAngle)
								 -  0.5*ssdconetrapezoidheight/(fgkSSDPConeUpMaxRadius
								 -  0.5*ssdconetrapezoidheight)*(0.5*fgkSSDPConeTrapezoidBasis
								 -  0.5*ssdconetrapezoidheight/TanD(fgkSSDPConeTrapezoidAngle)));
  Double_t ssdconetrapezoidsection = (2.0*TMath::Pi()*fgkSSDPConeUpMaxRadius-8.0*ssdconetrapezoidbasis)/8.;
  Double_t ssdpconetrapezoidsectionangle = ssdconetrapezoidsection/fgkSSDPConeUpMaxRadius
										 * TMath::RadToDeg();
  ssdpconezsection[10] = ssdpconezsection[9];
  ssdpconesectionradiusmin[10] = ssdpconesectionradiusmin[9];
  ssdpconesectionradiusmax[10] = ssdpconesectionradiusmax[9];
  ssdpconesectionradiusmin[11] = fgkSSDPConeUpMaxRadius-ssdpconezsection[0]
							  * CosD(fgkSSDPConeAngle)
							  / SinD(fgkSSDPConeAngle);
  ssdpconesectionradiusmax[11] = fgkSSDPConeUpMaxRadius;
  ssdpconezsection[11] = (ssdpconesectionradiusmin[11]-ssdpconesectionradiusmin[10])
					   * TanD(fgkSSDPConeAngle)+ssdpconezsection[10];
  ssdpconelittleholeshape[5] = new TGeoPcon(90.-0.5*ssdpconetrapezoidsectionangle,
											ssdpconetrapezoidsectionangle,2);    
  for(Int_t i=10;i<12;i++) ssdpconelittleholeshape[5]->DefineSection(i-10,ssdpconezsection[i],
						  ssdpconesectionradiusmin[i],ssdpconesectionradiusmax[i]); 
  ssdpconelittlehole[5] = new TGeoVolume("SSDConeLittleHole6",ssdpconelittleholeshape[5],fSSDCarbonFiberMedium);
  ssdpconelittlehole[5]->SetLineColor(4);
  TGeoRotation* ssdconeupradiusrot[8];
  for(Int_t i=0; i<8; i++){
	ssdconeupradiusrot[i] = new TGeoRotation();
    ssdconeupradiusrot[i]->SetAngles(i*45,0.,0.);
  }
  /////////////////////////////////////////////////////////////
  ssdpconezsection[12] = ssdpconezsection[11];
  ssdpconezsection[13] = ssdpconezsection[12]+fgkSSDPConeRadiusWidth;	
  ssdpconesectionradiusmin[12] = ssdpconesectionradiusmin[11]; 
  ssdpconesectionradiusmax[12] = fgkSSDPConeExternalRadius;
  ssdpconesectionradiusmin[13] = ssdpconesectionradiusmin[12];
  ssdpconesectionradiusmax[13] = fgkSSDPConeExternalRadius;
  ssdpconelittleholeshape[6] = new TGeoPcon(0.,360.,2);
  for(Int_t i=12; i<14;i++) ssdpconelittleholeshape[6]->DefineSection(i-12,ssdpconezsection[i],
						  ssdpconesectionradiusmin[i],ssdpconesectionradiusmax[i]); 
  ssdpconelittlehole[6] = new TGeoVolume("SSDConeLittleHole7",ssdpconelittleholeshape[6],fSSDCarbonFiberMedium);
  ssdpconelittlehole[6]->SetLineColor(4);
  /////////////////////////////////////////////////////////////
  ssdpconezsection[14] = 0.0;
  ssdpconezsection[15] = ssdpconezsection[0];
  ssdpconesectionradiusmin[14] = ssdpconesectionradiusmin[0];
  ssdpconesectionradiusmax[14] = ssdpconesectionradiusmin[14];
  ssdpconesectionradiusmin[15] = ssdpconesectionradiusmin[0];
  ssdpconesectionradiusmax[15] = ssdpconesectionradiusmax[0];
  ssdpconelittleholeshape[7] = new TGeoPcon(0.,360.,2);
  for(Int_t i=14; i<16;i++) ssdpconelittleholeshape[7]->DefineSection(i-14,ssdpconezsection[i],
						  ssdpconesectionradiusmin[i],ssdpconesectionradiusmax[i]); 
  ssdpconelittlehole[7] = new TGeoVolume("SSDConeLittleHole8",ssdpconelittleholeshape[7],fSSDCarbonFiberMedium);
  ssdpconelittlehole[7]->SetLineColor(4);
  /////////////////////////////////////////////////////////////
  TGeoTube* ssdtubeconeshape[2];
  TGeoVolume* ssdtubecone[2];
  TGeoTranslation* ssdtubeconetrans[2];
  ssdtubeconeshape[0] = new TGeoTube(fgkSSDPConeUpMiddleRadius,
									   fgkSSDPConeExternalRadius,
									   0.5*(fgkSSDPConeLength-ssdpconezsection[13]));
  ssdtubeconeshape[1] = new TGeoTube(fgkSSDPConeDownRadius,ssdpconesectionradiusmin[0],
									   0.5*ssdpconezsection[0]); 
  ssdtubecone[0] = new TGeoVolume("SSDConeTube1",ssdtubeconeshape[0],fSSDSupportRingAl);
  ssdtubecone[1] = new TGeoVolume("SSDConeTube2",ssdtubeconeshape[1],fSSDSupportRingAl);
  ssdtubeconetrans[0] = new TGeoTranslation(0.,0.,
						0.5*(fgkSSDPConeLength-ssdpconezsection[13])
					  + ssdpconezsection[13]);
  ssdtubeconetrans[1] = new TGeoTranslation(0.,0.,0.5*ssdpconezsection[0]);
  ssdtubecone[0]->SetLineColor(4);
  ssdtubecone[1]->SetLineColor(4);
  /////////////////////////////////////////////////////////////
  // Mother Volume Container
  /////////////////////////////////////////////////////////////
  Double_t ssdconemotherradiusmin[8];
  Double_t ssdconemotherradiusmax[8];
  Double_t ssdconemothersection[8]; 
  ssdconemotherradiusmin[0] = fgkSSDPConeDownRadius;
  ssdconemotherradiusmax[0] = ssdpconesectionradiusmin[0];
  ssdconemotherradiusmin[1] = fgkSSDPConeDownRadius;
  ssdconemotherradiusmax[1] = ssdpconesectionradiusmax[0];
  ssdconemotherradiusmin[2] = ssdpconesectionradiusmin[0];
  ssdconemotherradiusmax[2] = ssdpconesectionradiusmax[0];
  ssdconemotherradiusmin[3] = ssdpconesectionradiusmin[11];
  ssdconemotherradiusmax[3] = ssdpconesectionradiusmax[11];
  ssdconemotherradiusmin[4] = ssdpconesectionradiusmin[12];
  ssdconemotherradiusmax[4] = ssdpconesectionradiusmax[12];
  ssdconemotherradiusmin[5] = ssdpconesectionradiusmin[13];
  ssdconemotherradiusmax[5] = ssdpconesectionradiusmax[13];
  ssdconemotherradiusmin[6] = fgkSSDPConeUpMiddleRadius;
  ssdconemotherradiusmax[6] = fgkSSDPConeExternalRadius;
  ssdconemotherradiusmin[7] = fgkSSDPConeUpMiddleRadius;
  ssdconemotherradiusmax[7] = fgkSSDPConeExternalRadius;
  ssdconemothersection[0] = 0.0;
  ssdconemothersection[1] = ssdpconezsection[0];
  ssdconemothersection[2] = ssdpconezsection[0];
  ssdconemothersection[3] = ssdpconezsection[11];
  ssdconemothersection[4] = ssdpconezsection[11];
  ssdconemothersection[5] = ssdpconezsection[13];
  ssdconemothersection[6] = ssdpconezsection[13];
  ssdconemothersection[7] = fgkSSDPConeLength;
  TGeoPcon* ssdconemothershape = new TGeoPcon(0.,360,8);
  for(Int_t i=0; i<8; i++) ssdconemothershape->DefineSection(i,ssdconemothersection[i],
									ssdconemotherradiusmin[i],ssdconemotherradiusmax[i]);
  TGeoVolume* ssdconemother = new TGeoVolume("SSDMotherCone",ssdconemothershape,fSSDAir);
  /////////////////////////////////////////////////////////////
  //Placing the Volumes into Mother 
  /////////////////////////////////////////////////////////////
  ssdconemother->AddNode(ssdpconelittlehole[0],1);
  for(Int_t i=0; i<6; i++){
	ssdconemother->AddNode(ssdpconelittlehole[1],i+1,ssdconelittleholerot[i]);
  }
  ssdconemother->AddNode(ssdpconelittlehole[2],1);
  for(Int_t i=0; i<8; i++){
    ssdconemother->AddNode(ssdpconelittlehole[3],i+1,ssdconemiddleholerot[i]);
  }
  ssdconemother->AddNode(ssdpconelittlehole[4],1);
  for(Int_t i=0; i<8; i++){
    ssdconemother->AddNode(ssdpconelittlehole[5],i+1,ssdconeupradiusrot[i]);
  }
  ssdconemother->AddNode(ssdpconelittlehole[6],1);
  ssdconemother->AddNode(ssdpconelittlehole[7],1);
  ssdconemother->AddNode(ssdtubecone[0],1,ssdtubeconetrans[0]);
  ssdconemother->AddNode(ssdtubecone[1],1,ssdtubeconetrans[1]);
  /////////////////////////////////////////////////////////////
  // ITS General Support
  /////////////////////////////////////////////////////////////
  TGeoTube* ssdcentralsupportshape = new TGeoTube(fgkSSDCentralSupportRadius-fgkSSDCentralSupportWidth,
								fgkSSDCentralSupportRadius,0.5*fgkSSDCentralSupportLength); 
  TGeoVolume* ssdcentralsupport = new TGeoVolume("SSDCentralSupport",ssdcentralsupportshape,fSSDRohaCellCone);
  TGeoTranslation* ssdcentralsupportrans = new TGeoTranslation(0.,0.,-0.5*fgkSSDCentralSupportLength
									     - fgkSSDCentralAL3SupportLength);
  ssdcentralsupport->SetLineColor(4);
  fSSDCone->AddNode(ssdcentralsupport,1,ssdcentralsupportrans);
  TGeoTube* ssdcentralal3supportshape = new TGeoTube(fgkSSDCentralSupportRadius-fgkSSDCentralSupportWidth,
								fgkSSDCentralSupportRadius,0.25*fgkSSDCentralAL3SupportLength);
  TGeoVolume* ssdcentralal3support = new TGeoVolume("SSDCentralAl3Support",ssdcentralal3supportshape,fSSDSupportRingAl);
  TGeoTranslation* ssdcentralal3supportrans[3]; 
  ssdcentralal3supportrans[0] = new TGeoTranslation(0.,0.,-0.75*fgkSSDCentralAL3SupportLength);
  ssdcentralal3supportrans[1] = new TGeoTranslation(0.,0.,-fgkSSDCentralSupportLength
							  - 1.25*fgkSSDCentralAL3SupportLength);
  ssdcentralal3support->SetLineColor(4);
  fSSDCone->AddNode(ssdcentralal3support,1,ssdcentralal3supportrans[0]);
  fSSDCone->AddNode(ssdcentralal3support,2,ssdcentralal3supportrans[1]);
  TGeoPcon* ssdpconcentralal3shape = new TGeoPcon(0.,360.,2);
  Double_t ssdpconcentralradiusmin[2];
  Double_t ssdpconcentralradiusmax[2];
  Double_t ssdpconcentralsection[2];
  ssdpconcentralradiusmin[0] = fgkSSDCentralSupportRadius-fgkSSDCentralSupportWidth;  
  ssdpconcentralradiusmin[1] = fgkSSDCentralSupportRadius-fgkSSDCentralAL3SupportWidth;  
  ssdpconcentralradiusmax[0] = fgkSSDCentralSupportRadius;
  ssdpconcentralradiusmax[1] = fgkSSDCentralSupportRadius;
  ssdpconcentralsection[0] = -0.5*fgkSSDCentralAL3SupportLength; 
  ssdpconcentralsection[1] = 0.;
  for(Int_t i=0; i<2;i++) ssdpconcentralal3shape->DefineSection(i,ssdpconcentralsection[i],
						  ssdpconcentralradiusmin[i],ssdpconcentralradiusmax[i]); 
  TGeoVolume* ssdpconcentralal3 = new TGeoVolume("SSDPConeCentralAl3",ssdpconcentralal3shape,fSSDSupportRingAl);
  ssdpconcentralal3->SetLineColor(4);
  fSSDCone->AddNode(ssdpconcentralal3,1);
  TGeoRotation* ssdcentralal3supportrot = new TGeoRotation();
  ssdcentralal3supportrot->SetAngles(90.,180,-90.);
  ssdcentralal3supportrans[2] = new TGeoTranslation(0.,0.,-fgkSSDCentralSupportLength
								-2.*fgkSSDCentralAL3SupportLength);
  TGeoCombiTrans* ssdcentralal3supporcombitrans = new TGeoCombiTrans(*ssdcentralal3supportrans[2],
												                     *ssdcentralal3supportrot);
  fSSDCone->AddNode(ssdpconcentralal3,2,ssdcentralal3supporcombitrans);
  TGeoRotation* ssdconemotherot = new TGeoRotation();
  ssdconemotherot->SetAngles(90.,180.,-90.);
  TGeoTranslation* ssdconemothertrans = new TGeoTranslation(0.,0.,-fgkSSDCentralSupportLength
															-2.*fgkSSDCentralAL3SupportLength);
  TGeoCombiTrans* ssdconemothercombitrans = new TGeoCombiTrans(*ssdconemothertrans,*ssdconemotherot);
  fSSDCone->AddNode(ssdconemother,1);
  fSSDCone->AddNode(ssdconemother,2,ssdconemothercombitrans);
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  delete ssdcentralal3supportrot;
  delete ssdcentralal3supportrans[2];
  delete ssdconemotherot;
  delete ssdconemothertrans;
  /////////////////////////////////////////////////////////////
 }
 ////////////////////////////////////////////////////////////////////////////////
 void AliITSv11GeometrySSD::SSDCables(TGeoVolume* moth){
  /////////////////////////////////////////////////////////////
  // Setting SSD Cables
  /////////////////////////////////////////////////////////////
  if (! moth) {
    printf("Error::AliITSv11GeometrySSD: Can't insert SSD Cables, mother is null!\n");
    return;
  };
  TGeoVolume* ssdcables = SetSSDCables();
  moth->AddNode(ssdcables,1);
}
 ////////////////////////////////////////////////////////////////////////////////
 TGeoVolume* AliITSv11GeometrySSD::SetSSDCables(){
  /////////////////////////////////////////////////////////////
  // Method generating SSDCables
  /////////////////////////////////////////////////////////////
  // SSD Layer 5 Cables
  //////////////////////////////////////////////////////////////////////////////////////////////////
  TGeoVolumeAssembly* ssdcablesmother = new TGeoVolumeAssembly("SSDCables");
  Double_t ssdcablelayvertical = 0.05; // Internal variables to control overlapping with SDD cables
  Double_t ssdcablelaylateral = 0.55;   // Internal variables to control overlapping with SDD cables
  Double_t ssdcablesfactor = 0.5;     // Internal variables to control overlapping with SDD cables
  //////////////////////////////////////////////////////////////////////////////////////////////////
  Double_t ssdcableslay5rigthsideradiusmin = fgkEndCapSupportMiddleRadius[0]+fgkSSDCablesLay5TubeRadiusMin;  
  Double_t ssdcableslay5endconedistance = (ssdcableslay5rigthsideradiusmin
									    -  fgkSSDLowerPConeRadius)
									    * TanD(fgkSSDPConeAngle);
  Double_t ssdcableslay5startconedistance = fgkEndCapSupportCenterLay5ITSPosition
									      + fgkEndCapSupportCenterLay5Position
									      - 0.5*fgkSSDCentralSupportLength-fgkSSDCentralAL3SupportLength;
  Double_t ssdcablelay5rightsidelength = ssdcableslay5endconedistance
									   - ssdcableslay5startconedistance; 
  ssdcablelay5rightsidelength *= ssdcablesfactor;
  Double_t ssdcableslay5rightsideradiusmax = ssdcableslay5rigthsideradiusmin+fgkSSDCablesLay5RightSideHeight; 
  TGeoTube* ssdcablelay5rightubeshape = new TGeoTube(ssdcableslay5rigthsideradiusmin,
												ssdcableslay5rightsideradiusmax,
												0.5*ssdcablelay5rightsidelength); 
  TGeoVolume* ssdcablelay5righttube = new TGeoVolume("SSDCableLay5RightSideTube",
													 ssdcablelay5rightubeshape,
													 fSSDCopper);
  ssdcablelay5righttube->SetLineColor(9);
  TGeoTranslation* ssdcablelay5rightrans = 
					  new TGeoTranslation(0.,0.,fgkEndCapSupportCenterLay5ITSPosition
										 +		fgkEndCapSupportCenterLay5Position
										 +      0.5*ssdcablelay5rightsidelength);
  ////////////////////////////////////
  //  Double_t cablescapacity[20];
  //  cablescapacity[0] = ssdcablelay5rightubeshape->Capacity();
  ////////////////////////////////////
  ssdcablesmother->AddNode(ssdcablelay5righttube,1,ssdcablelay5rightrans);
  ////////////////////////////////////
  // TGeoPCone Volumes
  ///////////////////////////////////
  TGeoPcon* ssdcableslay5pconshape[3];
  TGeoVolume* ssdcableslay5pcon[3]; 
  ssdcableslay5pconshape[0] = new TGeoPcon(0.,360.,2);   
  Double_t ssdcableslay5pconzsection[6];
  Double_t ssdcableslay5pconrmin[6];
  Double_t ssdcableslay5pconrmax[6];
  ssdcableslay5pconrmin[0] = ssdcablelay5rightubeshape->GetRmin();
  ssdcableslay5pconrmax[0] = ssdcablelay5rightubeshape->GetRmax();
  ssdcableslay5pconrmin[1] = fgkSSDPConeUpRadius*(1.0+ssdcablelayvertical);
  ssdcableslay5pconrmax[1] = ssdcableslay5pconrmin[1]+fgkSSDCablesLay5RightSideHeight;
  ssdcableslay5pconzsection[0] = fgkEndCapSupportCenterLay5ITSPosition
							   + fgkEndCapSupportCenterLay5Position
							   + 2.*ssdcablelay5rightubeshape->GetDz();
  ssdcableslay5pconzsection[1] = 0.5*fgkSSDCentralSupportLength
							   + fgkSSDCentralAL3SupportLength
							   + (fgkSSDPConeUpRadius-fgkSSDLowerPConeRadius)
							   * TanD(fgkSSDPConeAngle);      
  for(Int_t i=0; i<2;i++) ssdcableslay5pconshape[0]->DefineSection(i,ssdcableslay5pconzsection[i],
						  ssdcableslay5pconrmin[i],ssdcableslay5pconrmax[i]); 
  ssdcableslay5pcon[0] = new TGeoVolume("SSDCableLay5RightSidePCon1",
							   ssdcableslay5pconshape[0],fSSDCopper);
  ssdcableslay5pcon[0]->SetLineColor(9);
  ssdcablesmother->AddNode(ssdcableslay5pcon[0],1);
////////////////////////////////////
//  cablescapacity[1] = ssdcableslay5pconshape[0]->Capacity();
////////////////////////////////////
  ssdcableslay5pconzsection[2] = ssdcableslay5pconzsection[1];
  ssdcableslay5pconzsection[3] = 0.5*fgkSSDCentralSupportLength
							   + fgkSSDCentralAL3SupportLength
							   + 0.25*TanD(fgkSSDPConeAngle)*(fgkSSDPConeUpMaxRadius
							   + 3.*fgkSSDPConeUpRadius-4.*fgkSSDLowerPConeRadius);
  Double_t ssdcableangle = (fgkSSDPConeTrapezoidBasis-2.*(fgkSSDPConeUpMaxRadius
					     -  fgkSSDPConeUpRadius)/TanD(fgkSSDPConeTrapezoidAngle))
						 /  fgkSSDPConeUpRadius*TMath::RadToDeg()*2;
  ssdcableslay5pconshape[1] = new TGeoPcon(90.0-fgkSSDCableAngle-0.5*ssdcableangle,
										   ssdcableangle,2);   
  ssdcableslay5pconrmin[2] = ssdcableslay5pconrmin[1];
  ssdcableslay5pconrmax[2] = ssdcableslay5pconrmax[1];
  ssdcableslay5pconrmin[3] = 0.25*(fgkSSDPConeUpMaxRadius+3.*fgkSSDPConeUpRadius
						   - 4.*fgkSSDLowerPConeRadius)+fgkSSDLowerPConeRadius;
  ssdcableslay5pconrmin[3]*=(1.0+ssdcablelayvertical);
  ssdcableslay5pconrmax[3] = ssdcableslay5pconrmin[3]+fgkSSDCablesLay5RightSideHeight;
  for(Int_t i=0; i<2;i++) ssdcableslay5pconshape[1]->DefineSection(i,ssdcableslay5pconzsection[i+2],
						  ssdcableslay5pconrmin[i+2],ssdcableslay5pconrmax[i+2]); 
  ssdcableslay5pcon[1] = new TGeoVolume("SSDCableLay5RightSidePCon2",ssdcableslay5pconshape[1],fSSDCopper);
  ssdcableslay5pcon[1]->SetLineColor(9);
  ////////////////////////////////////
  ssdcableslay5pconshape[2] = new TGeoPcon(90.0-fgkSSDCableAngle-0.5*ssdcableangle,
										   ssdcableangle,2);   
  ssdcableslay5pconrmin[4] = ssdcableslay5pconrmin[3];
  ssdcableslay5pconrmax[4] = ssdcableslay5pconrmax[3];
  ssdcableslay5pconrmin[5] = ssdcableslay5pconrmin[4];
  ssdcableslay5pconrmax[5] = ssdcableslay5pconrmax[4];
  ssdcableslay5pconzsection[4] = ssdcableslay5pconzsection[3];
  ssdcableslay5pconzsection[5] = (ssdcableslay5pconrmin[5]-fgkSSDLowerPConeRadius)
							   * TanD(fgkSSDPConeAngle)
							   + 0.5*fgkSSDCentralSupportLength
							   + fgkSSDCentralAL3SupportLength;
  ssdcableslay5pconzsection[5]-= ssdcablelaylateral;
  for(Int_t i=0; i<2;i++) ssdcableslay5pconshape[2]->DefineSection(i,ssdcableslay5pconzsection[i+4],
						  ssdcableslay5pconrmin[i+4],ssdcableslay5pconrmax[i+4]); 
  ssdcableslay5pcon[2] = new TGeoVolume("SSDCableLay5RightSidePCon3",ssdcableslay5pconshape[2],fSSDCopper);
  ssdcableslay5pcon[2]->SetLineColor(9);
////////////////////////////////////
  TGeoRotation* ssdcableslay5pconrot[4];	
  for(Int_t i=0; i<4; i++){
   ssdcableslay5pconrot[i] = new TGeoRotation();
   ssdcableslay5pconrot[i]->SetAngles(90.0*i,0.,0.);
   ssdcablesmother->AddNode(ssdcableslay5pcon[1],i+1,ssdcableslay5pconrot[i]);
   ssdcablesmother->AddNode(ssdcableslay5pcon[2],i+1,ssdcableslay5pconrot[i]);  	
  }
  ////////////////////////////////////
  //cablescapacity[2] = 4.0*ssdcableslay5pconshape[1]->Capacity();
  //cablescapacity[3] = 4.0*ssdcableslay5pconshape[2]->Capacity();
  ////////////////////////////////////
  // Positioning Left SSD Cables Part
  ////////////////////////////////////
  TGeoTranslation* ssdcablesLay5RightTubeToLeftrans = new TGeoTranslation(0.,0.,
													- 0.5*ssdcablelay5rightsidelength
													- fgkEndCapSupportCenterLay5Position
												    - fgkEndCapSupportCenterLay5ITSPosition);
  ssdcablesmother->AddNode(ssdcablelay5righttube,2,ssdcablesLay5RightTubeToLeftrans);  
  TGeoRotation* ssdcablesLay5RightPConToLeftRot = new TGeoRotation();
  ssdcablesLay5RightPConToLeftRot->SetAngles(90.,180.,-90);
  ssdcablesmother->AddNode(ssdcableslay5pcon[0],2,ssdcablesLay5RightPConToLeftRot);  
  TGeoHMatrix* ssdcablesLay5RightPConToLeftMatrix[4];  
  for(Int_t i=0; i<4; i++){ ssdcablesLay5RightPConToLeftMatrix[i] = 
	new TGeoHMatrix((*ssdcablesLay5RightPConToLeftRot)*(*ssdcableslay5pconrot[i]));
	ssdcablesmother->AddNode(ssdcableslay5pcon[1],i+5,ssdcablesLay5RightPConToLeftMatrix[i]);
    ssdcablesmother->AddNode(ssdcableslay5pcon[2],i+5,ssdcablesLay5RightPConToLeftMatrix[i]);  	
  }
  ////////////////////////////////////
  //cablescapacity[4] = ssdcablelay5rightubeshape->Capacity();
  //cablescapacity[5] = ssdcableslay5pconshape[0]->Capacity();
  //cablescapacity[6] = 4.*ssdcableslay5pconshape[1]->Capacity();
  //cablescapacity[7] = 4.*ssdcableslay5pconshape[2]->Capacity();
  /////////////////////////////////////////////////////////////
  // Water Tubes Layer 5
  /////////////////////////
  TGeoTube* ssdcablelay5rightubewatershape = new TGeoTube(ssdcableslay5rightsideradiusmax,
										     ssdcableslay5rightsideradiusmax
									       + fgkSSDCablesLay5RightSideWaterHeight,
										     0.5*ssdcablelay5rightsidelength); 
  TGeoVolume* ssdcablelay5rightwatertube = new TGeoVolume("SSDCableLay5RightSideWaterTube",
													 ssdcablelay5rightubewatershape,
													 fSSDCoolingTubeWater);
  ssdcablelay5rightwatertube->SetLineColor(7);
  ssdcablesmother->AddNode(ssdcablelay5rightwatertube,1,ssdcablelay5rightrans);
  ssdcablesmother->AddNode(ssdcablelay5rightwatertube,2,ssdcablesLay5RightTubeToLeftrans);
  ////////////////////////////////////
  // TGeoPCone Water Volumes Layer 
  ///////////////////////////////////
  TGeoPcon* ssdcableslay5pconwatershape[3];
  TGeoVolume* ssdcableslay5pconwater[3]; 
  ssdcableslay5pconwatershape[0] = new TGeoPcon(0.,360.,2);   
  Double_t ssdcableslay5pconwaterzsection[6];
  Double_t ssdcableslay5pcwateronrmin[6];
  Double_t ssdcableslay5pconwaterrmax[6];
  ssdcableslay5pcwateronrmin[0] = ssdcableslay5pconrmax[0];
  ssdcableslay5pconwaterrmax[0] = ssdcableslay5pcwateronrmin[0]
								+ fgkSSDCablesLay5RightSideWaterHeight;
  ssdcableslay5pcwateronrmin[1] = ssdcableslay5pconrmax[1];
  ssdcableslay5pconwaterrmax[1] = ssdcableslay5pcwateronrmin[1]
								+ fgkSSDCablesLay5RightSideWaterHeight;
  ssdcableslay5pconwaterzsection[0] = ssdcableslay5pconzsection[0];
  ssdcableslay5pconwaterzsection[1] = ssdcableslay5pconzsection[1];
  for(Int_t i=0; i<2;i++) ssdcableslay5pconwatershape[0]->DefineSection(i,ssdcableslay5pconwaterzsection[i],
						  ssdcableslay5pcwateronrmin[i],ssdcableslay5pconwaterrmax[i]); 
  ssdcableslay5pconwater[0] = new TGeoVolume("SSDCableLay5RightSidePConWater1",
							   ssdcableslay5pconwatershape[0],fSSDCoolingTubeWater);
  ssdcableslay5pconwater[0]->SetLineColor(7);
  ssdcablesmother->AddNode(ssdcableslay5pconwater[0],1);
  ssdcablesmother->AddNode(ssdcableslay5pconwater[0],2,ssdcablesLay5RightPConToLeftRot);
////////////////////////////////////
  ssdcableslay5pconwaterzsection[2] = ssdcableslay5pconzsection[2];
  ssdcableslay5pconwaterzsection[3] = ssdcableslay5pconzsection[3];
  ssdcableslay5pconwatershape[1] = new TGeoPcon(90.0-fgkSSDCableAngle-0.5*ssdcableangle,
												ssdcableangle,2);   
  ssdcableslay5pcwateronrmin[2] = ssdcableslay5pconrmax[1];
  ssdcableslay5pconwaterrmax[2] = ssdcableslay5pcwateronrmin[2]
								+ fgkSSDCablesLay5RightSideWaterHeight;
  ssdcableslay5pcwateronrmin[3] = ssdcableslay5pconrmax[3];
  ssdcableslay5pconwaterrmax[3] = ssdcableslay5pcwateronrmin[3]
								+ fgkSSDCablesLay5RightSideWaterHeight;
  for(Int_t i=0; i<2;i++) ssdcableslay5pconwatershape[1]->DefineSection(i,ssdcableslay5pconwaterzsection[i+2],
						  ssdcableslay5pcwateronrmin[i+2],ssdcableslay5pconwaterrmax[i+2]); 
  ssdcableslay5pconwater[1] = new TGeoVolume("SSDCableLay5RightSidePConWater2",
							   ssdcableslay5pconwatershape[1],fSSDCoolingTubeWater);
  ssdcableslay5pconwater[1]->SetLineColor(7);
////////////////////////////////////
  ssdcableslay5pconwatershape[2] = new TGeoPcon(90.0-fgkSSDCableAngle-0.5*ssdcableangle,
												ssdcableangle,2);   
  ssdcableslay5pcwateronrmin[4] = ssdcableslay5pconrmax[3];
  ssdcableslay5pconwaterrmax[4] = ssdcableslay5pcwateronrmin[4]
								+ fgkSSDCablesLay5RightSideWaterHeight;
  ssdcableslay5pcwateronrmin[5] = ssdcableslay5pconrmax[4];
  ssdcableslay5pconwaterrmax[5] = ssdcableslay5pcwateronrmin[4]
								+ fgkSSDCablesLay5RightSideWaterHeight;
  ssdcableslay5pconwaterzsection[4] = ssdcableslay5pconzsection[4];
  ssdcableslay5pconwaterzsection[5] = ssdcableslay5pconzsection[5];
  for(Int_t i=0; i<2;i++) ssdcableslay5pconwatershape[2]->DefineSection(i,ssdcableslay5pconwaterzsection[i+4],
						  ssdcableslay5pcwateronrmin[i+4],ssdcableslay5pconwaterrmax[i+4]); 
  ssdcableslay5pconwater[2] = new TGeoVolume("SSDCableLay5RightSidePConWater3",
							   ssdcableslay5pconwatershape[2],fSSDCoolingTubeWater);
  ssdcableslay5pconwater[2]->SetLineColor(7);
////////////////////////////////////
  TGeoRotation* ssdcableslay5pconwaterot[4];	
  TGeoHMatrix* ssdcablesLay5RightPConWaterToLeftMatrix[4];  
  for(Int_t i=0; i<4; i++){
   ssdcableslay5pconwaterot[i] = new TGeoRotation();
   ssdcableslay5pconwaterot[i]->SetAngles(90.0*i+45.0,0.,0.);
   ssdcablesLay5RightPConWaterToLeftMatrix[i] = 
	new TGeoHMatrix((*ssdcablesLay5RightPConToLeftRot)*(*ssdcableslay5pconwaterot[i]));
	ssdcablesmother->AddNode(ssdcableslay5pconwater[1],i+1,ssdcableslay5pconwaterot[i]);
   	ssdcablesmother->AddNode(ssdcableslay5pconwater[1],i+5,ssdcablesLay5RightPConWaterToLeftMatrix[i]);
	ssdcablesmother->AddNode(ssdcableslay5pconwater[2],i+1,ssdcableslay5pconwaterot[i]);
   	ssdcablesmother->AddNode(ssdcableslay5pconwater[2],i+5,ssdcablesLay5RightPConWaterToLeftMatrix[i]);
  }
  /////////////////////////
  // SSD Layer 6 Cables
  /////////////////////////
  Double_t ssdcableslay6rigthsideradiusmin = fgkEndCapSupportMiddleRadius[1]+fgkSSDCablesLay6TubeRadiusMin;  
  Double_t ssdcablelay6rightsidelength = 2.*ssdcablelay5rightsidelength;
  Double_t ssdcableslay6rightsideradiusmax = ssdcableslay6rigthsideradiusmin+fgkSSDCablesLay6RightSideHeight; 
  TGeoTube* ssdcablelay6rightubeshape = new TGeoTube(ssdcableslay6rigthsideradiusmin,
												ssdcableslay6rightsideradiusmax,
												0.5*ssdcablelay6rightsidelength); 
  TGeoVolume* ssdcablelay6righttube = new TGeoVolume("SSDCableLay6RightSideTube",
													 ssdcablelay6rightubeshape,
													 fSSDCopper);
  ssdcablelay6righttube->SetLineColor(9);
  TGeoTranslation* ssdcablelay6rightrans = 
					  new TGeoTranslation(0.,0.,fgkEndCapSupportCenterLay6ITSPosition
										 +		fgkEndCapSupportCenterLay6Position
										 +      0.5*ssdcablelay6rightsidelength);
  TGeoTranslation* ssdcablesLay6RightTubeToLeftrans = new TGeoTranslation(0.,0.,
													- 0.5*ssdcablelay6rightsidelength
													- fgkEndCapSupportCenterLay6Position
												    - fgkEndCapSupportCenterLay6ITSPosition);
  ssdcablesmother->AddNode(ssdcablelay6righttube,1,ssdcablelay6rightrans);
  ssdcablesmother->AddNode(ssdcablelay6righttube,2,ssdcablesLay6RightTubeToLeftrans);
  ////////////////////////////////////
  //cablescapacity[8] = 2.*ssdcablelay6rightubeshape->Capacity();
  ////////////////////////////////////
  TGeoPcon* ssdcableslay6pconshape = new TGeoPcon(90.0-fgkSSDCableAngle-0.5*ssdcableangle,
										   ssdcableangle,2);   
  TGeoVolume* ssdcableslay6pcon;
  Double_t ssdcableslay6pconrmin[2];
  Double_t ssdcableslay6pconrmax[2];
  Double_t ssdcableslay6pconzsection[2];
  ssdcableslay6pconrmin[0] = ssdcableslay6rigthsideradiusmin;
  ssdcableslay6pconrmax[0] = ssdcableslay6rightsideradiusmax;
  ssdcableslay6pconrmin[1] = ssdcableslay6pconrmin[0];
  ssdcableslay6pconrmax[1] = ssdcableslay6pconrmax[0];
  ssdcableslay6pconzsection[0] = fgkEndCapSupportCenterLay6ITSPosition
							   + fgkEndCapSupportCenterLay6Position
							   + ssdcablelay6rightsidelength;
  ssdcableslay6pconzsection[1] = ssdcableslay5pconwaterzsection[5];
  for(Int_t i=0; i<2;i++) ssdcableslay6pconshape->DefineSection(i,ssdcableslay6pconzsection[i],
						  ssdcableslay6pconrmin[i],ssdcableslay6pconrmax[i]); 
  ssdcableslay6pcon = new TGeoVolume("SSDCableLay6RightSidePCon",
							   ssdcableslay6pconshape,fSSDCopper);
  ssdcableslay6pcon->SetLineColor(9);
  for(Int_t i=0; i<4; i++){
   ssdcablesmother->AddNode(ssdcableslay6pcon,i+1,ssdcableslay5pconrot[i]);
   ssdcablesmother->AddNode(ssdcableslay6pcon,i+5,ssdcablesLay5RightPConToLeftMatrix[i]);
  }
  ////////////////////////////////////
  //cablescapacity[9] = 8.*ssdcableslay6pconshape->Capacity();
  /////////////////////////
  // Water Tubes Layer 6
  /////////////////////////
  TGeoTube* ssdcablelay6righwatertubeshape = new TGeoTube(ssdcableslay6rightsideradiusmax,
														  ssdcableslay6rightsideradiusmax
										   +			  fgkSSDCablesLay5RightSideWaterHeight,
														  0.5*ssdcablelay6rightsidelength); 
  TGeoVolume* ssdcablelay6rightwatertube = new TGeoVolume("SSDCableLay6RightSideWaterTube",
													 ssdcablelay6righwatertubeshape,
													 fSSDCoolingTubeWater);
  ssdcablelay6rightwatertube->SetLineColor(7);
  ssdcablesmother->AddNode(ssdcablelay6rightwatertube,1,ssdcablelay6rightrans);
  ssdcablesmother->AddNode(ssdcablelay6rightwatertube,2,ssdcablesLay6RightTubeToLeftrans);
  TGeoPcon* ssdcableslay6waterpconshape = new TGeoPcon(90.0-fgkSSDCableAngle-0.5*ssdcableangle,
										   ssdcableangle,2);   
  TGeoVolume* ssdcableslay6waterpcon;
  Double_t ssdcableslay6waterpconrmin[2];
  Double_t ssdcableslay6waterpconrmax[2];
  Double_t ssdcableslay6waterpconzsection[2];
  ssdcableslay6waterpconrmin[0] = ssdcableslay6rightsideradiusmax;
  ssdcableslay6waterpconrmax[0] = ssdcableslay6rightsideradiusmax
							    + fgkSSDCablesLay5RightSideWaterHeight;
  ssdcableslay6waterpconrmin[1] = ssdcableslay6waterpconrmin[0];
  ssdcableslay6waterpconrmax[1] = ssdcableslay6waterpconrmax[0];
  ssdcableslay6waterpconzsection[0] = fgkEndCapSupportCenterLay6ITSPosition
							   + fgkEndCapSupportCenterLay6Position
							   + ssdcablelay6rightsidelength;
  ssdcableslay6waterpconzsection[1] = ssdcableslay5pconwaterzsection[5];
  for(Int_t i=0; i<2;i++) ssdcableslay6waterpconshape->DefineSection(i,ssdcableslay6waterpconzsection[i],
						  ssdcableslay6waterpconrmin[i],ssdcableslay6waterpconrmax[i]); 
  ssdcableslay6waterpcon = new TGeoVolume("SSDCableLay6RightSideWaterPCon",
							   ssdcableslay6waterpconshape,fSSDCoolingTubeWater);
  ssdcableslay6waterpcon->SetLineColor(7);
  TGeoRotation* ssdcableslay6pconwaterot[4];	
  TGeoRotation* ssdcablesLay6RightPConToLeftRot = new TGeoRotation();
  ssdcablesLay6RightPConToLeftRot->SetAngles(90.,180.,-90);
  TGeoHMatrix* ssdcablesLay6RightPConToLeftMatrix[4];  
  for(Int_t i=0; i<4; i++){
   ssdcableslay6pconwaterot[i] = new TGeoRotation();
   ssdcableslay6pconwaterot[i]->SetAngles(90.0*i+45.0,0.,0.);
   ssdcablesLay6RightPConToLeftMatrix[i] = new TGeoHMatrix((*ssdcablesLay6RightPConToLeftRot)
										 * (*ssdcableslay6pconwaterot[i]));   
   ssdcablesmother->AddNode(ssdcableslay6waterpcon,i+1,ssdcableslay6pconwaterot[i]);
   ssdcablesmother->AddNode(ssdcableslay6waterpcon,i+5,ssdcablesLay6RightPConToLeftMatrix[i]);
  }
  ////////////////////////////////////////
  // From ITS Ring to Patch Panel3-RB26
  ////////////////////////////////////////
  Double_t ssdcablepatchpanel3BB26radiusmin[2];
  Double_t ssdcablepatchpanel3BB26radiusmax[2];
  Double_t ssdcablepatchpanel3RB26zsection[2];
  ssdcablepatchpanel3BB26radiusmin[0] = ssdcableslay5pconrmin[3]-0.5*fgkSSDPatchPanelHeigth+2.8;
  ssdcablepatchpanel3BB26radiusmax[0] = ssdcablepatchpanel3BB26radiusmin[0]
									  + fgkSSDCablesLay5RightSideHeight
									  + fgkSSDCablesLay6RightSideHeight+0.5*fgkSSDPatchPanelHeigth;
  ssdcablepatchpanel3BB26radiusmin[1] = fgkSSDPatchPanel2RB26Radius;
  ssdcablepatchpanel3BB26radiusmax[1] = ssdcablepatchpanel3BB26radiusmin[1]
									  + 0.*fgkSSDCablesLay5RightSideHeight
									  + 0.*fgkSSDCablesLay6RightSideHeight+0.5*fgkSSDPatchPanelHeigth;
  ssdcablepatchpanel3RB26zsection[0] = 0.5*fgkSSDCentralSupportLength
										 + fgkSSDCentralAL3SupportLength
										 + fgkSSDPConeZLength[0];
  ssdcablepatchpanel3RB26zsection[1] = fgkSSDPatchPanel2RB26ITSDistance;  
  TGeoPcon* ssdcablepatchpanel3RB26pconshape = 
								new TGeoPcon(90.0-fgkSSDCablesPatchPanel2RB26Angle[0]
												- 0.5*ssdcableangle,ssdcableangle,2);   
  for(Int_t i=0; i<2;i++) ssdcablepatchpanel3RB26pconshape->DefineSection(i,ssdcablepatchpanel3RB26zsection[i],
						  ssdcablepatchpanel3BB26radiusmin[i],ssdcablepatchpanel3BB26radiusmax[i]); 
  TGeoVolume* ssdcablepatchpanel3RB26pcon = new TGeoVolume("SSDCablePatchPanel3RB26",
												ssdcablepatchpanel3RB26pconshape,fSSDCopper);
  ssdcablepatchpanel3RB26pcon->SetLineColor(9);
  TGeoRotation* ssdcablepatchpanel3B26rot[3];
  for(Int_t i=0; i<4; i++) ssdcablepatchpanel3B26rot[i] = new TGeoRotation();
  ssdcablepatchpanel3B26rot[0]->SetAngles(0.0,0.0,0.0);
  ssdcablepatchpanel3B26rot[1]->SetAngles(fgkSSDCablesPatchPanel2RB26Angle[0]
								  +			  fgkSSDCablesPatchPanel2RB26Angle[1],0.0,0.0);
  ssdcablepatchpanel3B26rot[2]->SetAngles(180.0,0.0,0.0);
  ssdcablepatchpanel3B26rot[3]->SetAngles(180.0+fgkSSDCablesPatchPanel2RB26Angle[0]
								  +			  fgkSSDCablesPatchPanel2RB26Angle[1],0.0,0.0);
  for(Int_t i=0; i<4; i++) ssdcablesmother->AddNode(ssdcablepatchpanel3RB26pcon,i+1,ssdcablepatchpanel3B26rot[i]);
  ////////////////////////////////////
  //cablescapacity[10] = 4.*ssdcablepatchpanel3RB26pconshape->Capacity();
  ////////////////////////////////////////
  //  ITS Ring Cables RB26 Part
  ////////////////////////////////////////
  Double_t ssdcableitsring3BB26pconzsection[2];
  Double_t ssdcableitsring3BB26pconrmin[2];
  Double_t ssdcableitsring3BB26pconrmax[2];
  ssdcableitsring3BB26pconzsection[0] = 0.5*fgkSSDCentralSupportLength
									  + fgkSSDCentralAL3SupportLength
									  + (4.0/5.0)*fgkSSDPConeZLength[0];
  ssdcableitsring3BB26pconzsection[1] = ssdcablepatchpanel3RB26zsection[0];
  ssdcableitsring3BB26pconrmin[0] = fgkSSDPConeUpRadius-0.5*fgkSSDPatchPanelHeigth;
  ssdcableitsring3BB26pconrmax[0] = ssdcableitsring3BB26pconrmin[0]
								  + fgkSSDCablesLay5RightSideHeight
								  + fgkSSDCablesLay6RightSideHeight+0.5*fgkSSDPatchPanelHeigth;
  ssdcableitsring3BB26pconrmin[1] = ssdcablepatchpanel3BB26radiusmin[0];
  ssdcableitsring3BB26pconrmax[1] = ssdcablepatchpanel3BB26radiusmax[0];
  TGeoPcon* ssdcableitsring3BB26pconshape[4];
  ssdcableitsring3BB26pconshape[0] = new TGeoPcon(90.0-fgkSSDCablesPatchPanel2RB26Angle[0]
								   -              0.5*ssdcableangle,ssdcableangle
								   +				(fgkSSDCablesPatchPanel2RB26Angle[0]
								   -				 fgkSSDCableAngle),2);
  ssdcableitsring3BB26pconshape[1] = new TGeoPcon(90.0+fgkSSDCablesPatchPanel2RB26Angle[1]
								   -              0.5*ssdcableangle,ssdcableangle
								   +			  3.0*fgkSSDCableAngle
								   -			  fgkSSDCablesPatchPanel2RB26Angle[1],2);
  ssdcableitsring3BB26pconshape[2] = new TGeoPcon(270-fgkSSDCablesPatchPanel2RB26Angle[0]
								   -              0.5*ssdcableangle,ssdcableangle
								   -			  fgkSSDCableAngle
								   +			  fgkSSDCablesPatchPanel2RB26Angle[0],2);
  ssdcableitsring3BB26pconshape[3] = new TGeoPcon(270.0+fgkSSDCablesPatchPanel2RB26Angle[1]
								   -              0.5*ssdcableangle,ssdcableangle
								   +			  3.0*fgkSSDCableAngle
								   -			  fgkSSDCablesPatchPanel2RB26Angle[1],2);
  for(Int_t i=0;i<4;i++)
	for(Int_t j=0; j<2; j++) ssdcableitsring3BB26pconshape[i]->DefineSection(j,ssdcableitsring3BB26pconzsection[j],
							 ssdcableitsring3BB26pconrmin[j],
							 ssdcableitsring3BB26pconrmax[j]); 
  TGeoVolume* ssdcableitsring3BB26pcon[4];
  ssdcableitsring3BB26pcon[0] = new TGeoVolume("SSDCableITSRing3RB26Part1",
												ssdcableitsring3BB26pconshape[0],fSSDCopper);
  ssdcableitsring3BB26pcon[1] = new TGeoVolume("SSDCableITSRing3RB26Part2",
												ssdcableitsring3BB26pconshape[1],fSSDCopper);
  ssdcableitsring3BB26pcon[2] = new TGeoVolume("SSDCableITSRing3RB26Part3",
												ssdcableitsring3BB26pconshape[2],fSSDCopper);
  ssdcableitsring3BB26pcon[3] = new TGeoVolume("SSDCableITSRing3RB26Part4",
												ssdcableitsring3BB26pconshape[3],fSSDCopper);
  for(Int_t i=0;i<4;i++){
	ssdcableitsring3BB26pcon[i]->SetLineColor(9);
	ssdcablesmother->AddNode(ssdcableitsring3BB26pcon[i],1);
}
  ////////////////////////////////////
  //cablescapacity[11] = ssdcableitsring3BB26pconshape[0]->Capacity()
  //				 + ssdcableitsring3BB26pconshape[1]->Capacity() 
  //				 + ssdcableitsring3BB26pconshape[2]->Capacity() 
  //				 + ssdcableitsring3BB26pconshape[3]->Capacity(); 
  ////////////////////////////////////////
  // From ITS Ring to Patch Panel2-RB24
  ////////////////////////////////////////
  Double_t ssdcablepatchpanel3BB24radiusmin[2];
  Double_t ssdcablepatchpanel3BB24radiusmax[2];
  Double_t ssdcablepatchpanel3RB24zsection[2];
  ssdcablepatchpanel3BB24radiusmin[0] = ssdcablepatchpanel3BB26radiusmin[0];
  ssdcablepatchpanel3BB24radiusmax[0] = ssdcablepatchpanel3BB26radiusmax[0];
  ssdcablepatchpanel3BB24radiusmin[1] = fgkSSDPatchPanel2RB24Radius;
  ssdcablepatchpanel3BB24radiusmax[1] = ssdcablepatchpanel3BB24radiusmin[1]
									  + 0.*fgkSSDCablesLay5RightSideHeight
									  + 0.*fgkSSDCablesLay6RightSideHeight
									  + 0.5*fgkSSDPatchPanelHeigth;
  ssdcablepatchpanel3RB24zsection[0] = -0.5*fgkSSDCentralSupportLength
									 -  fgkSSDCentralAL3SupportLength
									 -  fgkSSDPConeZLength[0];
  ssdcablepatchpanel3RB24zsection[1] = -fgkSSDPatchPanel2RB24ITSDistance;  
  TGeoPcon* ssdcablepatchpanel3RB24pconshape = 
								new TGeoPcon(90.0-fgkSSDCablesPatchPanel2RB24Angle[1]
												- 0.5*ssdcableangle,ssdcableangle,2);   
  for(Int_t i=0; i<2;i++) ssdcablepatchpanel3RB24pconshape->DefineSection(i,ssdcablepatchpanel3RB24zsection[i],
						  ssdcablepatchpanel3BB24radiusmin[i],ssdcablepatchpanel3BB24radiusmax[i]); 
  TGeoVolume* ssdcablepatchpanel3RB24pcon = new TGeoVolume("SSDCablePatchPanel3RB24",
												ssdcablepatchpanel3RB24pconshape,
												fSSDCopper);
  ssdcablepatchpanel3RB24pcon->SetLineColor(9);
  TGeoRotation* ssdcablepatchpanel3B24rot[3];
  for(Int_t i=0; i<4; i++) ssdcablepatchpanel3B24rot[i] = new TGeoRotation();
  ssdcablepatchpanel3B24rot[0]->SetAngles(0.0,0.0,0.0);
  ssdcablepatchpanel3B24rot[1]->SetAngles(fgkSSDCablesPatchPanel2RB24Angle[0]
								  +			  fgkSSDCablesPatchPanel2RB24Angle[1],0.0,0.0);
  ssdcablepatchpanel3B24rot[2]->SetAngles(180.0,0.0,0.0);
  ssdcablepatchpanel3B24rot[3]->SetAngles(180.0+fgkSSDCablesPatchPanel2RB24Angle[0]
								  +			  fgkSSDCablesPatchPanel2RB24Angle[1],0.0,0.0);
  for(Int_t i=0; i<4; i++) ssdcablesmother->AddNode(ssdcablepatchpanel3RB24pcon,i+1,ssdcablepatchpanel3B24rot[i]);
  ////////////////////////////////////
  //cablescapacity[12] = 4.*ssdcablepatchpanel3RB24pconshape->Capacity();
  ////////////////////////////////////////
  //  ITS Ring Cables RB24 Part
  ////////////////////////////////////////
  Double_t ssdcableitsring3BB24pconzsection[2];
  Double_t ssdcableitsring3BB24pconrmin[2];
  Double_t ssdcableitsring3BB24pconrmax[2];
  ssdcableitsring3BB24pconzsection[0] = -ssdcableitsring3BB26pconzsection[0];
  ssdcableitsring3BB24pconzsection[1] = ssdcablepatchpanel3RB24zsection[0];
  ssdcableitsring3BB24pconrmin[0] = fgkSSDPConeUpRadius-0.5*fgkSSDPatchPanelHeigth;
  ssdcableitsring3BB24pconrmax[0] = ssdcableitsring3BB24pconrmin[0]
								  + fgkSSDCablesLay5RightSideHeight
								  + fgkSSDCablesLay6RightSideHeight+0.5*fgkSSDPatchPanelHeigth;
  ssdcableitsring3BB24pconrmin[1] = ssdcablepatchpanel3BB24radiusmin[0];
  ssdcableitsring3BB24pconrmax[1] = ssdcablepatchpanel3BB24radiusmax[0];
  TGeoPcon* ssdcableitsring3BB24pconshape[4];
  ssdcableitsring3BB24pconshape[0] = new TGeoPcon(fgkSSDCableAngle-0.5*ssdcableangle,ssdcableangle
								   +				(90.0-fgkSSDCablesPatchPanel2RB24Angle[1]
								   -				 fgkSSDCableAngle),2);
  ssdcableitsring3BB24pconshape[1] = new TGeoPcon(90.0+fgkSSDCableAngle-0.5*ssdcableangle,
								     ssdcableangle-fgkSSDCableAngle
								   +			  fgkSSDCablesPatchPanel2RB24Angle[0],2);
  ssdcableitsring3BB24pconshape[2] = new TGeoPcon(180.0+fgkSSDCableAngle-0.5*ssdcableangle,ssdcableangle
								   -			  fgkSSDCableAngle
								   +			  90.0-fgkSSDCablesPatchPanel2RB24Angle[1],2);
  ssdcableitsring3BB24pconshape[3] = new TGeoPcon(270.0+fgkSSDCableAngle-0.5*ssdcableangle,
												  ssdcableangle-fgkSSDCableAngle
								   +			  fgkSSDCablesPatchPanel2RB24Angle[0],2);
  for(Int_t i=0;i<4;i++)
	for(Int_t j=0; j<2; j++) ssdcableitsring3BB24pconshape[i]->DefineSection(j,ssdcableitsring3BB24pconzsection[j],
							 ssdcableitsring3BB24pconrmin[j],
							 ssdcableitsring3BB24pconrmax[j]); 
  TGeoVolume* ssdcableitsring3BB24pcon[4];
  ssdcableitsring3BB24pcon[0] = new TGeoVolume("SSDCableITSRing3RB24Part1",
												ssdcableitsring3BB24pconshape[0],fSSDCopper);
  ssdcableitsring3BB24pcon[1] = new TGeoVolume("SSDCableITSRing3RB24Part2",
												ssdcableitsring3BB24pconshape[1],fSSDCopper);
  ssdcableitsring3BB24pcon[2] = new TGeoVolume("SSDCableITSRing3RB24Part3",
												ssdcableitsring3BB24pconshape[2],fSSDCopper);
  ssdcableitsring3BB24pcon[3] = new TGeoVolume("SSDCableITSRing3RB24Part4",
												ssdcableitsring3BB24pconshape[3],fSSDCopper);
  for(Int_t i=0;i<4;i++){
	ssdcableitsring3BB24pcon[i]->SetLineColor(9);
	ssdcablesmother->AddNode(ssdcableitsring3BB24pcon[i],1);
}
  ////////////////////////////////////
  //cablescapacity[13] = ssdcableitsring3BB24pconshape[0]->Capacity()
  //					 + ssdcableitsring3BB24pconshape[1]->Capacity()
  //					 + ssdcableitsring3BB24pconshape[2]->Capacity()
  //					 + ssdcableitsring3BB24pconshape[3]->Capacity();
  ////////////////////////////////////
  // Volumes for Material Budget 
  ////////////////////////////////////
  TGeoTube* ssdcablelay6materialbudgetubeshape = new TGeoTube(ssdcableslay6rightsideradiusmax
											   +     fgkSSDCablesLay5RightSideWaterHeight,
													 ssdcableslay6rightsideradiusmax
											   +     fgkSSDCablesLay5RightSideWaterHeight
											   +	 fgkSSDCableMaterialBudgetHeight,0.5*ssdcablelay6rightsidelength); 
  TGeoVolume* ssdcablelay6materialbudgetube = new TGeoVolume("SSDCableLay6MaterialBudgetTube",
													 ssdcablelay6materialbudgetubeshape,
													 fSSDCopper);
  ssdcablelay6materialbudgetube->SetLineColor(9);
  ssdcablesmother->AddNode(ssdcablelay6materialbudgetube,1,ssdcablelay6rightrans);
  ssdcablesmother->AddNode(ssdcablelay6materialbudgetube,2,ssdcablesLay6RightTubeToLeftrans);

  TGeoPcon* ssdcablelay6materialbudgetpconshape = 
					new TGeoPcon(90.0-fgkSSDCableAngle-0.5*ssdcableangle,ssdcableangle,2); 
  TGeoVolume* ssdcablelay6materialbudgetpcon;
  Double_t ssdcablelay6materialbudgetpconrmin[2];
  Double_t ssdcablelay6materialbudgetpconrmax[2];
  Double_t ssdcablelay6materialbudgetpconzsection[2];
  ssdcablelay6materialbudgetpconrmin[0] = ssdcableslay6rightsideradiusmax
										+ fgkSSDCablesLay5RightSideWaterHeight;
  ssdcablelay6materialbudgetpconrmax[0] = ssdcablelay6materialbudgetpconrmin[0]
										+ fgkSSDCableMaterialBudgetHeight;
  ssdcablelay6materialbudgetpconrmin[1] = ssdcablelay6materialbudgetpconrmin[0];
  ssdcablelay6materialbudgetpconrmax[1] = ssdcablelay6materialbudgetpconrmax[0];
  ssdcablelay6materialbudgetpconzsection[0] = fgkEndCapSupportCenterLay6ITSPosition
											+ fgkEndCapSupportCenterLay6Position
											+ ssdcablelay6rightsidelength;
  ssdcablelay6materialbudgetpconzsection[1] = ssdcableslay5pconwaterzsection[5];
  for(Int_t i=0; i<2;i++) ssdcablelay6materialbudgetpconshape->DefineSection(i,
						  ssdcablelay6materialbudgetpconzsection[i],
						  ssdcablelay6materialbudgetpconrmin[i],
						  ssdcablelay6materialbudgetpconrmax[i]); 
  ssdcablelay6materialbudgetpcon = new TGeoVolume("SSDCableLay6MaterialBudgetPCon",
							   ssdcablelay6materialbudgetpconshape,fSSDCopper);
  ssdcablelay6materialbudgetpcon->SetLineColor(9);
  for(Int_t i=0; i<4; i++){
   ssdcablesmother->AddNode(ssdcablelay6materialbudgetpcon,i+1,ssdcableslay5pconrot[i]);
   ssdcablesmother->AddNode(ssdcablelay6materialbudgetpcon,i+5,ssdcablesLay5RightPConToLeftMatrix[i]);
  }
////////////////////////////////////
 /* cablescapacity[14] = 2.*ssdcablelay6materialbudgetubeshape->Capacity();
  cablescapacity[15] = 2.*ssdcablelay6materialbudgetpconshape->Capacity();
  Double_t ssdcablesvolume = 0.0;
  for(Int_t i=0;i<16;i++) ssdcablesvolume+=cablescapacity[i];
  std::cout << ssdcablesvolume << std::endl;*/
  return ssdcablesmother;
 }
 ////////////////////////////////////////////////////////////////////////////////
TGeoArb8* AliITSv11GeometrySSD::GetArbShape(TVector3* vertexpos[], Double_t* width, 
									Double_t height, char* shapename, Int_t isign) const{
  /////////////////////////////////////////////////////////////
  // Method generating an Arb shape 
  /////////////////////////////////////////////////////////////
  const Int_t kvertexnumber = 8;
  const Int_t ktransvectnumber = 2;
  TVector3* vertex[kvertexnumber];
  TVector3* transvector[2];
  for(Int_t i=0; i<ktransvectnumber; i++) transvector[i] = new TVector3(0.,width[i]);
  /////////////////////////////////////////////////////////////
  //Setting the vertices for TGeoArb8
  /////////////////////////////////////////////////////////////
  vertex[0] = new TVector3(*vertexpos[0]);
  vertex[1] = new TVector3(*vertexpos[1]);
  vertex[2] = new TVector3(*vertex[1]+isign*(*transvector[0]));
  vertex[3] = new TVector3(*vertex[0]+isign*(*transvector[0]));
  vertex[4] = new TVector3(*vertexpos[2]);
  vertex[5] = new TVector3(*vertexpos[3]);
  vertex[6] = new TVector3(*vertex[5]+isign*(*transvector[1]));
  vertex[7] = new TVector3(*vertex[4]+isign*(*transvector[1]));
  /////////////////////////////////////////////////////////////
  TGeoArb8* arbshape = new TGeoArb8(shapename,0.5*height);
  for(Int_t i = 0; i<kvertexnumber;i++) 
							arbshape->SetVertex(i,vertex[i]->X(),vertex[i]->Y());
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i< kvertexnumber; i++) delete vertex[i];  
  for(Int_t i=0; i< ktransvectnumber; i++) delete transvector[i];  
  /////////////////////////////////////////////////////////////
  return arbshape;
} 
///////////////////////////////////////////////////////////////////////////////
TGeoXtru* AliITSv11GeometrySSD::GetArcShape(Double_t phi, Double_t rmin, 
								Double_t rmax, Int_t nedges, Double_t height){
  /////////////////////////////////////////////////////////////
  // Method generating Arc shape 
  /////////////////////////////////////////////////////////////
	const Int_t kvertexnumber = 2*nedges+2;
	TGeoXtru* arcshape = new TGeoXtru(2);	
	TVector3** vertexposition[2];
	for(Int_t i=0; i<2; i++) vertexposition[i] = new TVector3*[nedges+1];
	Double_t angle = 0.;
    for(Int_t i=0; i<nedges+1; i++){ 
		angle = 90.+0.5*phi-i*(phi/nedges);
		vertexposition[0][i] = new TVector3(rmin*CosD(angle),rmin*SinD(angle));
		vertexposition[1][i] = new TVector3(rmax*CosD(angle),rmax*SinD(angle));
	}
	Double_t *xvertexpoints = new Double_t[kvertexnumber];
	Double_t *yvertexpoints = new Double_t[kvertexnumber];
	for(Int_t i=0; i<kvertexnumber; i++){ 
		if(i==0){ xvertexpoints[i] = vertexposition[0][i]->X(),
				  yvertexpoints[i] = vertexposition[0][i]->Y();	
		}
		else if(i>=1&&i<nedges+2)
		{
			xvertexpoints[i] = vertexposition[1][i-1]->X(); 
			yvertexpoints[i] = vertexposition[1][i-1]->Y(); 
		}
        else
		{
			xvertexpoints[i] = vertexposition[0][kvertexnumber-i]->X(); 
			yvertexpoints[i] = vertexposition[0][kvertexnumber-i]->Y(); 
		}
    }
  arcshape->DefinePolygon(kvertexnumber,xvertexpoints,yvertexpoints);
  arcshape->DefineSection(0,-0.5*height);
  arcshape->DefineSection(1,0.5*height);
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  for(Int_t i=0; i<2; i++){
	for(Int_t j=0; j<nedges+1; j++)
		delete vertexposition[i][j];
	delete [] vertexposition[i];
  }
  delete [] xvertexpoints;
  delete [] yvertexpoints;
  /////////////////////////////////////////////////////////////
	return arcshape;
}
////////////////////////////////////////////////////////////////////////////////
TGeoShape* AliITSv11GeometrySSD::GetScrewShape(Double_t* radius,Int_t* edgesnumber,Double_t* section) const {
  ///////////////////////////////////////////////////////////////////////
  // Method Generating the Screw Shape  
  // radius[0]: outer radius
  // radius[1]: inner radius
  // edgesnumber[0]: outer number of edges
  // edgesnumber[1]: inner number of edges
  // section[0]: lower section position
  // section[1]: higher section position
  ///////////////////////////////////////////////////////////////////////
  Double_t outradius = radius[0];
  Double_t inradius = radius[1];
  Int_t outvertexnumber = edgesnumber[0];
  Int_t invertexnumber = edgesnumber[1];
  Double_t* xscrewvertex = new Double_t[outvertexnumber+invertexnumber];
  Double_t* yscrewvertex = new Double_t[outvertexnumber+invertexnumber];
  for(Int_t i=0; i<outvertexnumber; i++){
	xscrewvertex[i] = outradius*CosD(90.+i*360./outvertexnumber);
	yscrewvertex[i]	= outradius*SinD(90.+i*360./outvertexnumber);
  }
  for(Int_t i=0; i<invertexnumber; i++){
	xscrewvertex[outvertexnumber+i] = inradius*CosD(90.+i*360./invertexnumber);
	yscrewvertex[outvertexnumber+i] = inradius*SinD(90.+i*360./invertexnumber);
  }
  TGeoXtru* screwshapeout = new TGeoXtru(2);
  screwshapeout->DefinePolygon(outvertexnumber,xscrewvertex,yscrewvertex);
  screwshapeout->DefineSection(0,section[0]);
  screwshapeout->DefineSection(1,section[1]);
  TGeoXtru* screwshapein = new TGeoXtru(2);
  screwshapein->DefinePolygon(invertexnumber,&xscrewvertex[outvertexnumber],&yscrewvertex[outvertexnumber]);
  screwshapein->DefineSection(0,section[0]-0.01); // make inner part bigger in Z
  screwshapein->DefineSection(1,section[1]+0.01); // safer when we subtract it
  TGeoSubtraction *snode = new TGeoSubtraction(screwshapeout, screwshapein);
  TGeoCompositeShape *screwshape = new TGeoCompositeShape("", snode);
  
  delete [] xscrewvertex;
  delete [] yscrewvertex;
  return screwshape;
}
////////////////////////////////////////////////////////////////////////////////
TGeoShape* AliITSv11GeometrySSD::GetHoleShape(Double_t radius, Int_t nedges, Double_t *section) const {
  ///////////////////////////////////////////////////////////////////////
  // Method Generating the Hole Shape  
  // radius of the Hole
  // nedges: number of edges to approximate the circle
  ///////////////////////////////////////////////////////////////////////
  Double_t* xholevertex = new Double_t[nedges];
  Double_t* yholevertex = new Double_t[nedges];
  Double_t z  = 0.5*(section[0]+section[1]);
  Double_t dz = 0.5*(section[1]-section[0]);
  TGeoTranslation *tr = 0;
  if (TMath::Abs(z) > TGeoShape::Tolerance()) {
     tr = new TGeoTranslation(0.,0.,z);
     tr->RegisterYourself();
  }   
  TGeoBBox *box = new TGeoBBox("",radius,radius,dz);
  for(Int_t i=0; i<nedges; i++){
	xholevertex[i] = radius*CosD(i*360./nedges);
	yholevertex[i] = radius*SinD(i*360./nedges);
  }
  TGeoXtru* holeshapeout = new TGeoXtru(2);
  holeshapeout->DefinePolygon(nedges,xholevertex,yholevertex);
  holeshapeout->DefineSection(0,section[0]-0.01); // make subtracted part larger in Z
  holeshapeout->DefineSection(1,section[1]+0.01);
  TGeoSubtraction *snode = new TGeoSubtraction(box,holeshapeout,tr);
  TGeoCompositeShape *holeshape = new TGeoCompositeShape("", snode);
  
  delete [] xholevertex;
  delete [] yholevertex;
  return holeshape;
}
////////////////////////////////////////////////////////////////////////////////
TVector3* AliITSv11GeometrySSD::GetReflection(TVector3* vector,Double_t* param) const{
  /////////////////////////////////////////////////////////////
  // Given an axis specified by param, it gives the reflection of the point
  // respect to the axis
  /////////////////////////////////////////////////////////////
  TVector3* n = new TVector3(param[0],param[1],param[2]);
  Double_t d = ((*vector)*(*n)+param[3])/n->Mag2();
  TVector3* reflectedvector = new TVector3(*vector-2*d*(*n));
  /////////////////////////////////////////////////////////////
  // Deallocating memory
  /////////////////////////////////////////////////////////////
  delete n;
  /////////////////////////////////////////////////////////////
  return reflectedvector;
}
////////////////////////////////////////////////////////////////////////////////
TGeoHMatrix* AliITSv11GeometrySSD::AddTranslationToHMatrix(TGeoHMatrix* ct,
                                                       Double_t dx,
                                                       Double_t dy,
                                                       Double_t dz) const{
  /////////////////////////////////////////////////////////////
  // Add a dx,dy,dz translation to the initial TGeoCombiTrans
  /////////////////////////////////////////////////////////////
  TGeoHMatrix* hmatrix = new TGeoHMatrix(*ct);
  const Double_t *vect = hmatrix->GetTranslation();
  Double_t newvect[3] = {vect[0]+dx, vect[1]+dy, vect[2]+dz};
  hmatrix->SetTranslation(newvect);
  TGeoHMatrix* matrix = new TGeoHMatrix(*hmatrix);
  delete hmatrix;
  return matrix;
}
////////////////////////////////////////////////////////////////////////////////
TGeoMedium* AliITSv11GeometrySSD::GetMedium(const char* mediumName) {
  /////////////////////////////////////////////////////////////
  // Method returning the Medium type 
  /////////////////////////////////////////////////////////////
  char ch[30];
  sprintf(ch, "ITS_%s",mediumName);
  TGeoMedium* medium =  gGeoManager->GetMedium(ch);
  if (! medium)
    printf("Error(AliITSv11GeometrySSD)::medium %s not found !\n", mediumName);
  return medium;
}
////////////////////////////////////////////////////////////////////////////////
void AliITSv11GeometrySSD::CreateMaterials(){
///////////////////////////////////
// This part has to be modified
///////////////////////////////////
  ///////////////////////////////////
  // Silicon for Sensor
  /////////////////////////////////// 
  fSSDSensorMedium = GetMedium("SI$");
  ///////////////////////////////////
  // Silicon Mixture for Sensor
  /////////////////////////////////// 
  fSSDChipMedium = GetMedium("SPD SI CHIP$");
  fSSDChipGlueMedium = GetMedium("EPOXY$");
  ///////////////////////////////////
  // Stiffener Components Materials
  /////////////////////////////////// 
  fSSDStiffenerMedium = GetMedium("SDD C AL (M55J)$");
  ///////////////////////////  
  // Stiffener Connectors 
  ///////////////////////////  
  fSSDStiffenerConnectorMedium = GetMedium("COPPER$");
  ////////////////////////////////  
  // Stiffener 0603-1812 Capacitor
  ////////////////////////////////  
  fSSDStiffener0603CapacitorMedium = GetMedium("SDD ruby sph. Al2O3$");
  fSSDStiffener1812CapacitorMedium = GetMedium("SDD ruby sph. Al2O3$");
  ///////////////////////////  
  // Stiffener Hybrid Wire 
  ///////////////////////////  
  fSSDStiffenerHybridWireMedium = GetMedium("COPPER$");
  ///////////////////////////  
  // Al for Cooling Block
  ///////////////////////////  
  fSSDAlCoolBlockMedium = GetMedium("AL$");
  //////////////////////////////////////////////////////  
  // Kapton and Al for Chip Cable Flex and Ladder Cables
  //////////////////////////////////////////////////////  
  fSSDKaptonChipCableMedium = GetMedium("KAPTONH(POLYCH2)$");
  fSSDAlTraceChipCableMedium = GetMedium("AL$");
  fSSDKaptonFlexMedium = GetMedium("KAPTONH(POLYCH2)$");
  fSSDAlTraceFlexMedium = GetMedium("AL$");
  fSSDKaptonLadderCableMedium = GetMedium("KAPTONH(POLYCH2)$");
  fSSDAlTraceLadderCableMedium = GetMedium("AL$");
  /////////////////////////////////////////////////////////////////  
  // M55J for Carbon Fiber, CarbonFiber Lower Support and Junction
  //////////////////////////////////////////////////////////////////  
  fSSDCarbonFiberMedium = GetMedium("GEN C (M55J)$");
  /////////////////////////////////////////////////////////////////  
  // G10 for Detector Leg, TubeHolder
  //////////////////////////////////////////////////////////////////  
  fSSDTubeHolderMedium = GetMedium("G10FR4$");
  fSSDSensorSupportMedium = GetMedium("G10FR4$");
  fSSDMountingBlockMedium = GetMedium("G10FR4$");
  fSSDMountingBlockMedium = GetMedium("G10FR4$");
  /////////////////////////////////////////////////////////////////  
  // Water and Phynox for Cooling Tube
  //////////////////////////////////////////////////////////////////  
  fSSDCoolingTubeWater = GetMedium("WATER$");
  fSSDCoolingTubePhynox = GetMedium("INOX$");
  /////////////////////////////////////////////////////////////////////
  // Material for Support Rings
  /////////////////////////////////////////////////////////////////////
  fSSDSupportRingAl = GetMedium("AL$");
  fSSDRohaCellCone = GetMedium("ROHACELL$");
  /////////////////////////////////////////////////////////////////////
  fSSDAir = GetMedium("SDD AIR$");
  fSSDCopper = GetMedium("COPPER$");
  fCreateMaterials = kTRUE;
}
/////////////////////////////////////////////////////////////////////

