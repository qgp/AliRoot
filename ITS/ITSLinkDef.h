#ifdef __CINT__
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
//#pragma link C++ enum   Cluster_t;

//#pragma link C++ global gITSdisplay;  // global used by AliITSdisplay

// Standard ITS classes 
 
#pragma link C++ class  AliITS+;
#pragma link C++ class  AliITSv1+;
#pragma link C++ class  AliITSv5+;
#pragma link C++ class  AliITSv5symm+;
#pragma link C++ class  AliITSv5asymm+;
#pragma link C++ class  AliITSvPPRcoarseasymm+;
#pragma link C++ class  AliITSvPPRcoarsesymm+;
#pragma link C++ class  AliITSvPPRasymm+;
#pragma link C++ class  AliITSvPPRsymm+;
#pragma link C++ class  AliITShit+;
#pragma link C++ class  AliITSdigit+;
#pragma link C++ class  AliITSdigitSPD+;
#pragma link C++ class  AliITSdigitSDD+;
#pragma link C++ class  AliITSdigitSSD+;
#pragma link C++ class  AliITSTransientDigit+;
#pragma link C++ class  AliITSgeom+;
#pragma link C++ class  AliITSgeomMatrix-;
#pragma link C++ class  AliITSgeomSPD+;
#pragma link C++ class  AliITSgeomSDD+;
#pragma link C++ class  AliITSgeomSSD+;
#pragma link C++ class  AliITSGeant3Geometry+;
// Standard ITS detector class initilizers
#pragma link C++ class AliITSgeomSPD300+;
#pragma link C++ class AliITSgeomSPD425Short+;
#pragma link C++ class AliITSgeomSPD425Long+;
#pragma link C++ class AliITSgeomSDD256+;
#pragma link C++ class AliITSgeomSDD300+;
#pragma link C++ class AliITSgeomSSD175+;
#pragma link C++ class AliITSgeomSSD275and75+;
#pragma link C++ class AliITSgeomSSD75and275+;

#pragma link C++ class  AliITSmodule+;
#pragma link C++ class  AliITSRecPoint+;
#pragma link C++ class  AliITSRawCluster+;
#pragma link C++ class  AliITSRawClusterSPD+;
#pragma link C++ class  AliITSRawClusterSDD+;
#pragma link C++ class  AliITSRawClusterSSD+;
#pragma link C++ class  AliITSMap+;
#pragma link C++ class  AliITSMapA1+;
#pragma link C++ class  AliITSMapA2+;
#pragma link C++ class  AliITSsegmentation+;
#pragma link C++ class  AliITSsegmentationSPD+;
#pragma link C++ class  AliITSsegmentationSDD+;
#pragma link C++ class  AliITSsegmentationSSD+;
#pragma link C++ class  AliITSresponse+;
#pragma link C++ class  AliITSresponseSPD+;
#pragma link C++ class  AliITSresponseSPDdubna+;
#pragma link C++ class  AliITSresponseSDD+;
#pragma link C++ class  AliITSresponseSSD+;
#pragma link C++ class  AliITSsimulation+;
#pragma link C++ class  AliITSsimulationSPD+;
#pragma link C++ class  AliITSsimulationSPDdubna+;
#pragma link C++ class  AliITSsimulationSDD+;
#pragma link C++ class  AliITSsimulationSSD+;
#pragma link C++ class  AliITSTableSSD+;
#pragma link C++ class  AliITSpList+;
#pragma link C++ class  AliITSpListItem+;
#pragma link C++ class  AliITSsimulationFastPoints+;
#pragma link C++ class  AliITSsimulationFastPointsV0+;
#pragma link C++ class  AliITSClusterFinder+;
#pragma link C++ class  AliITSClusterFinderSPD+;
#pragma link C++ class  AliITSClusterFinderSPDdubna+;
#pragma link C++ class  AliITSClusterFinderSDD+;
#pragma link C++ class  AliITSClusterFinderSSD+;
#pragma link C++ class  AliITSDetType+;
#pragma link C++ class  AliITSstatistics+;
#pragma link C++ class  AliITSstatistics2+;
// SDD simulation
#pragma link C++ class  AliITSRawData+;
// These streamers must be formatted according to the raw data fromat
#pragma link C++ class  AliITSInStream+;
#pragma link C++ class  AliITSOutStream+;
//
#pragma link C++ class  AliITSHNode+;
#pragma link C++ class  AliITSHTable+;
#pragma link C++ class  AliITSetfSDD+;
// SSD simulation and reconstruction
#pragma link C++ class  AliITSdcsSSD+;
#pragma link C++ class  AliITSclusterSSD+;
#pragma link C++ class  AliITSpackageSSD+;
#pragma link C++ class  AliITSPid+;
#pragma link C++ class  AliITStrackV2Pid+;
// Classes used for Tracking
#pragma link C++ class  AliITSTrackV1+;
#pragma link C++ class  AliITSRad+;
#pragma link C++ class  AliITSIOTrack+;
#pragma link C++ class  AliITSTrackerV1+;
#pragma link C++ class  AliITSgeoinfo+;
#pragma link C++ class  AliITSRiemannFit-;
// New used for Alignment studdies
//#pragma link C++ class  AliITSAlignmentTrack-;
//#pragma link C++ class  AliITSAlignmentModule-;
//#pragma link C function HitsTo;
//#pragma link C function HitsToClustAl;
//#pragma link C function FillGlobalPositions;
//#pragma link C function PlotGeomChanges;
//#pragma link C function FitAllTracks;
//#pragma link C function FitVertexAll;
//#pragma link C function OnlyOneGeometry;
//#pragma link C function deleteClustAl;
// New used for AliITSdisplay
//#pragma link C++ class  AliITSdisplay;
//#pragma link C++ class  AliITSDisplay;
//#pragma link C++ class  TInputDialog;   // MUST BE RENAMED
//#pragma link C function OpenFileDialog;
//#pragma link C function GetStringDialog;
//#pragma link C function GetIntegerDialog;
//#pragma link C function GetFloatDialog;
// This class will always be for ITS only
#pragma link C++ class  AliITSvtest+;

#pragma link C++ class AliITSclusterV2+;
#pragma link C++ class AliITStrackV2+;
#pragma link C++ class AliITStrackerV2+;
#pragma link C++ class  AliV0vertex+;
#pragma link C++ class  AliV0vertexer+;
#pragma link C++ class  AliCascadeVertex+;
#pragma link C++ class  AliCascadeVertexer+;

#pragma link C++ class  AliITSVertex+;
// Classes for neural tracking
#pragma link C++ class AliITSglobalRecPoint+;
#pragma link C++ class AliITSneuron+;
#pragma link C++ class AliITSneuralTrack+;
#pragma link C++ class AliITSneuralTracker+;
// Tasks
#pragma link C++ class AliITSreconstruction+;
#pragma link C++ class AliITSsDigitize+;
#pragma link C++ class AliITSDigitizer+;
#pragma link C++ class AliITSFDigitizer+;
#endif
