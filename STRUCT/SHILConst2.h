#ifndef SHILCONST2_H
#define SHILCONST2_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. */

/* $Id$ */

// angle of 2nd cone
  const Float_t thetaOpen2   = 0.83*kDegrad;
// angle of 3rd cone
  const Float_t thetaOpen3   = 0.83*kDegrad;
// angle of beam tube in second cone
  const Float_t thetaOpenB   = 0.83*kDegrad;
//  const Float_t thetaOpenB = 0.84*kDegrad;
// inner lead Cone opening angle
  const Float_t thetaOpenPb  = 0.6*kDegrad;
// Outer Pb Cone opening angle
  const Float_t thetaOpenPbO = 1.6*kDegrad;
// Start of lead cone
  const Float_t zPb = 720.;   
// y-position of trigger wall
  const Float_t zFilterIn       = 1471.;
  const Float_t zFilterOut      = zFilterIn+120.;
// end of 2-degree outer cone
//  const Float_t zConeE    = 30./TMath::Tan(accMin);
  const Float_t zConeE    = 859.;
//
//
  Float_t dTubeS = 0.1;
  const Float_t dInsuS=1.5;
  const Float_t dEnveS=0.1;
  const Float_t dProtS=0.1;
  const Float_t dFreeS=0.00;

  Float_t dVacuS=dTubeS+dInsuS+dEnveS+dProtS+dFreeS;

//
// Radii and z-positions imposed by vacuum chamber layout
//
// FIRST SECTION
// delta_R for bellows
//  const Float_t dr11=2.65;
  const Float_t dr11=4.1;
// delta_R for flange
  const Float_t dr12=0.01;
// delta_R to catch up with cone
  const Float_t dr13=2.0;

// flange length
  const Float_t dF1=2.*3.9;
// bellow length
  const Float_t dB1=18.482-dF1/2-dr11/6.;
// Flange position
  const Float_t zvac2=577.;
  const Float_t zvac1=zvac2-dF1/2-dB1-dr11/10.-dr12;
  const Float_t zvac3=zvac2+dF1/2+dB1+dr12+dr13;
//  const Float_t zvac4=648.;
  const Float_t zvac4 = 618.;
  const Float_t zvac41= 558.;
// Outer shield dimensions
  const Float_t R11=15.45;
// Steel Envelope
  const Float_t dRSteel1=2.;
  const Float_t dRSteel2=4.;
//  According to design
//  const Float_t R21=20.3;
//  to avoid overlap with 2deg line
  const Float_t R21=19.4;
//
// 2nd Section
// 
  const Float_t zvac5=zvac4+4.;
//
// 3rd Section
// 
  const Float_t zvac6=711.;
  const Float_t zvac8=1274.;
//const Float_t dr21=2.263;
  const Float_t dr21=.5;
  const Float_t dr22=1.3;
  const Float_t dr23=2.263;
  const Float_t dB2=24.118;
  const Float_t dF2=10.6;
  const Float_t zvac7=zvac8-dF2/2-dB2-dr22-dr21;
  const Float_t zvac9=zvac8+dF2/2+dB2+dr22+dr23;
//
// 4th Section
// 
  const Float_t zvac10=1466.;
  const Float_t zvac11=1800.;
  const Float_t zvac12=1900.;

  const Float_t R41=35.;
  const Float_t R42=49.9;
  const Float_t R43=110.;

//
// Vacuum System
//

// Bellow1
//
  const Float_t rB1=5.97+0.4;
  const Float_t hB1=2.25;
  const Float_t lB1=1.2;
  const Float_t eB1=0.04;
//
// Flange1
//
  const Float_t rF1=8.87+0.4;
  const Float_t dFlange=0.1;

//
// Bellow2
//
  const Float_t rB2=16.35;
  const Float_t hB2=2.25;
  const Float_t lB2=2.32;
  const Float_t eB2=0.05;
//
// Flange2
//
  const Float_t rF2=18.5;

//
// Chamber positions
//
const Float_t zch1 =   530.0 ;
const Float_t zch2 =   672.5 ;

const Float_t zch11 =  526.2 ;
const Float_t zch12 =  553.8 ;
const Float_t zch21 =  670.25;
const Float_t zch22 =  701.75;
const Float_t zch31 =  946.  ;
const Float_t zch32 = 1046.  ;
const Float_t zch41 = 1238.16;
const Float_t zch42 = 1296.16;
const Float_t zch51 = 1399.0 ;
const Float_t zch52 = 1457.0 ;
#endif

