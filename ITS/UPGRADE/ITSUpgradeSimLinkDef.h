#ifdef __CINT__
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: ITSsimLinkDef.h 36856 2009-11-16 16:17:07Z masera $ */

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
//#pragma link C++ enum   Cluster_t;

//#pragma link C++ global gITSdisplay;  // global used by AliITSdisplay

// ITS upgrade classes
//v0 part >>>. obsolete?
#pragma link C++ class  AliITSupgrade+;
#pragma link C++ class  AliITSupgradeDigitizer+;
//v0 part <<<
// 
#pragma link C++ class  AliITSU+;
#pragma link C++ class  AliITSUv11+;
#pragma link C++ class  AliITSUv11Layer+;
#pragma link C++ class  AliITSv11GeomBeamPipe+;
#pragma link C++ class  AliITSUModule+;
#pragma link C++ class  AliITSUSimuParam+;
#pragma link C++ class  AliITSUSimulation+;
#pragma link C++ class  AliITSUSimulationPix+;
#pragma link C++ class  AliITSUDigitPix+;
#pragma link C++ class  AliITSUDigitizer+;
#pragma link C++ class  AliITSUHit+;


#endif
