/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors:                                                       *
 *   Artur Szostak <artursz@iafrica.com>                                  *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          * 
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

// $Id$

///
/// @file   HLTMUONLinkDef.h
/// @author Artur Szostak <artursz@iafrica.com>
/// @date   29 May 2007
/// @brief  Linkdef file for dHLT.
///
/// The linkdef file for rootcint to build a ROOT dictionary of the dimuon
/// HLT classes exposed to AliRoot.
///

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
//#pragma link off all typedefs;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;
//#pragma link C++ nestedfunction;

#pragma link C++ class AliHLTMUONAgent+;
#pragma link C++ class AliHLTMUONConstants+;
#pragma link C++ class AliHLTMUONUtils+;
#pragma link C++ class AliHLTMUONProcessor+;
#pragma link C++ class AliHLTMUONTriggerRecordsSource+;
#pragma link C++ class AliHLTMUONRecHitsSource+;
#pragma link C++ class AliHLTMUONDigitPublisherComponent+;
#pragma link C++ class AliHLTMUONTriggerReconstructorComponent+;
#pragma link C++ class AliHLTMUONHitReconstructorComponent+;
#pragma link C++ class AliHLTMUONMansoTrackerFSMComponent+;
#pragma link C++ class AliHLTMUONFullTrackerComponent+;
#pragma link C++ class AliHLTMUONDecisionComponent+;
#pragma link C++ class AliHLTMUONESDMaker+;
#pragma link C++ class AliHLTMUONRecHit+;
#pragma link C++ class AliHLTMUONRecHit::AliChannel+;
#pragma link C++ class AliHLTMUONTriggerRecord+;
#pragma link C++ class AliHLTMUONTrack+;
#pragma link C++ class AliHLTMUONMansoTrack+;
#pragma link C++ class AliHLTMUONDecision+;
#pragma link C++ class AliHLTMUONDecision::AliTrackDecision+;
#pragma link C++ class AliHLTMUONDecision::AliPairDecision+;
#pragma link C++ class AliHLTMUONEvent+;
#pragma link C++ class AliHLTMUONRootifierComponent+;
#pragma link C++ class AliHLTMUONEmptyEventFilterComponent+;
#pragma link C++ class AliHLTMUONDataCheckerComponent+;
#pragma link C++ class AliHLTMUONClusterFinderComponent+;
#pragma link C++ class AliHLTMUONRawDataHistoComponent+;
#pragma link C++ class AliHLTMUONClusterHistoComponent+;

#endif // __CINT__
