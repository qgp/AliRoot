// -*- mode: c++ -*- 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights
 * reserved. 
 *
 * See cxx source for full Copyright notice                               
 */
/** @file    FMDutilLinkDef.h
    @author  Christian Holm Christensen <cholm@nbi.dk>
    @date    Mon Mar 27 14:19:41 2006
    @brief   Link specification for utility libary
*/
/* $Id$ */
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// #pragma link C++ class  std::pair<Float_t,UShort_t>;
// #pragma link C++ class  AliFMDMap<std::pair<Float_t,UShort_t> >;
// #pragma link C++ typedef  AliFMDEdepMap;
#pragma link C++ class  AliFMDInput;
#pragma link C++ class  AliFMDDisplay;
#pragma link C++ class  AliFMDPattern;
#pragma link C++ class  AliFMDFancy;
#pragma link C++ class  AliFMDCalibFaker;
#pragma link C++ class  AliFMDAlignFaker;
#pragma link C++ class  AliFMDAnaRing;
#pragma link C++ class  AliFMDAnaESD;
#pragma link C++ class  AliFMDBaseDA;
#pragma link C++ class  AliFMDPedestalDA;
#pragma link C++ class  AliFMDGainDA;


#pragma link C++ class  AliFMDSpectraDisplay;
#pragma link C++ class  AliFMDSpectraDisplay::AliFMDSpectraDisplayElement;
#pragma link C++ class  AliFMDSpectraDisplay::AliFMDSpectraDisplayTop;
#pragma link C++ class  AliFMDSpectraDisplay::AliFMDSpectraDisplayDetector;
#pragma link C++ class  AliFMDSpectraDisplay::AliFMDSpectraDisplayRing;
#pragma link C++ class  AliFMDSpectraDisplay::AliFMDSpectraDisplaySector;
#pragma link C++ class  AliFMDSpectraDisplay::AliFMDSpectraDisplayStrip;

#else
# error Not for compilation 
#endif
//
// EOF
//
