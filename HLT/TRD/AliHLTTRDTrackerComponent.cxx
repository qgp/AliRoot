// $Id$

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Matthias Richter <Matthias.Richter@ift.uib.no>                *
 *          Timm Steinbeck <timm@kip.uni-heidelberg.de>                   *
 *          for The ALICE Off-line Project.                               *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file   AliHLTTRDTrackerComponent.cxx
    @author Timm Steinbeck, Matthias Richter
    @date   
    @brief  A TRDTracker processing component for the HLT. */

#if __GNUC__ >= 3
using namespace std;
#endif

#include "TFile.h"

#include "AliHLTTRDTrackerComponent.h"
#include "AliHLTTRDDefinitions.h"
#include "AliCDBManager.h"

#include "AliTRDclusterizerV1HLT.h"
#include "AliTRDReconstructor.h"
#include "AliESD.h"
#include "AliTRDtrackerHLT.h"
#include "AliTRDCalibraFillHisto.h"
#include "AliMagFMaps.h"
#include "AliTRDcluster.h"
#include "TObjArray.h"

#include <cstdlib>
#include <cerrno>
#include <string>

// this is a global object used for automatic component registration, do not use this
AliHLTTRDTrackerComponent gAliHLTTRDTrackerComponent;

ClassImp(AliHLTTRDTrackerComponent);
    
AliHLTTRDTrackerComponent::AliHLTTRDTrackerComponent()
{
  fOutputPercentage = 100; // By default we copy to the output exactly what we got as input

  fStrorageDBpath = "local://$ALICE_ROOT";
  fCDB = AliCDBManager::Instance();
  //fCDB->SetDefaultStorage(fStrorageDBpath.c_str());
  fCDB->SetRun(0);

  fGeometryFileName = getenv("ALICE_ROOT");
  fGeometryFileName += "/HLT/TRD/geometry.root";
}

AliHLTTRDTrackerComponent::~AliHLTTRDTrackerComponent()
{
}

const char* AliHLTTRDTrackerComponent::GetComponentID()
{
  return "TRDTracker"; // The ID of this component
}

void AliHLTTRDTrackerComponent::GetInputDataTypes( vector<AliHLTComponent_DataType>& list)
{
  list.clear(); // We do not have any requirements for our input data type(s).
  list.push_back( AliHLTTRDDefinitions::gkClusterDataType );
}

AliHLTComponent_DataType AliHLTTRDTrackerComponent::GetOutputDataType()
{
  return AliHLTTRDDefinitions::gkClusterDataType;
}

void AliHLTTRDTrackerComponent::GetOutputDataSize( unsigned long& constBase, double& inputMultiplier )
{
  constBase = 0;
  inputMultiplier = ((double)fOutputPercentage)/100.0;
}

// Spawn function, return new instance of this class
AliHLTComponent* AliHLTTRDTrackerComponent::Spawn()
{
  return new AliHLTTRDTrackerComponent;
};

int AliHLTTRDTrackerComponent::DoInit( int argc, const char** argv )
{
  // perform initialization. We check whether our relative output size is specified in the arguments.
  fOutputPercentage = 100;
  int i = 0;
  char* cpErr;
  while ( i < argc )
    {
      Logging( kHLTLogDebug, "HLT::TRDTracker::DoInit", "Arguments", "argv[%d] == %s", i, argv[i] );
      if ( !strcmp( argv[i], "output_percentage" ) )
	{
	  if ( i+1>=argc )
	    {
	      Logging(kHLTLogError, "HLT::TRDTracker::DoInit", "Missing Argument", "Missing output_percentage parameter");
	      return ENOTSUP;
	    }
	  Logging( kHLTLogDebug, "HLT::TRDTracker::DoInit", "Arguments", "argv[%d+1] == %s", i, argv[i+1] );
	  fOutputPercentage = strtoul( argv[i+1], &cpErr, 0 );
	  if ( *cpErr )
	    {
	      Logging(kHLTLogError, "HLT::TRDTracker::DoInit", "Wrong Argument", "Cannot convert output_percentage parameter '%s'", argv[i+1] );
	      return EINVAL;
	    }
	  Logging( kHLTLogInfo, "HLT::TRDTracker::DoInit", "Output percentage set", "Output percentage set to %lu %%", fOutputPercentage );
	  i += 2;
	  continue;
	}

      if ( strcmp( argv[i], "-cdb" ) == 0)
	{
	  if ( i+1 >= argc )
	    {
	      Logging(kHLTLogError, "HLT::TRDTracker::DoInit", "Missing Argument", "Missing -cdb argument");
	      return ENOTSUP;	      
	    }
	  fStrorageDBpath = argv[i+1];
	  Logging( kHLTLogInfo, "HLT::TRDTracker::DoInit", "DB storage set", "DB storage is %s", fStrorageDBpath.c_str() );	  
	  fCDB->SetDefaultStorage(fStrorageDBpath.c_str());
	  i += 2;
	  continue;
	}      

      if ( strcmp( argv[i], "-geometry" ) == 0)
	{
	  if ( i+1 >= argc )
	    {
	      Logging(kHLTLogError, "HLT::TRDTracker::DoInit", "Missing Argument", "Missing -geometry argument");
	      return ENOTSUP;	      
	    }
	  fGeometryFileName = argv[i+1];
	  Logging( kHLTLogInfo, "HLT::TRDTracker::DoInit", "GeomFile storage set", "GeomFile storage is %s", 
		   fGeometryFileName.c_str() );	  
	  i += 2;
	  continue;
	}      

      Logging(kHLTLogError, "HLT::TRDTracker::DoInit", "Unknown Option", "Unknown option '%s'", argv[i] );
      return EINVAL;
    }

  fClusterizer = new AliTRDclusterizerV1HLT("TRCclusterizer", "TRCclusterizer");

  //init alifield map - temporarly fixed - should come from a DB
  fField = new AliMagFMaps("Maps","Maps", 2, 1., 10., 1);
  if (fField)
    AliTracker::SetFieldMap(fField,1);
  else
    Logging(kHLTLogError, "HLT::TRDTracker::DoInit", "Field", "Unable to init the field");
    
  fGeometryFile = TFile::Open(fGeometryFileName.c_str());
  if (fGeometryFile)
    {
      fGeoManager = (TGeoManager *)fGeometryFile->Get("Geometry");
      fTracker = new AliTRDtrackerHLT(fGeometryFile);
    }
  else
    {
      Logging(kHLTLogError, "HLT::TRDTracker::DoInit", "fGeometryFile", "Unable to open file. FATAL!");
      return -1;
    }

  AliTRDCalibraFillHisto *calibra = AliTRDCalibraFillHisto::Instance();
  if (calibra == 0)
    {
      Logging(kHLTLogError, "HLT::TRDTracker::DoInit", "Calibration Histos", "::Instance failed");
      return -1;      
    }
  else
    {
      calibra->SetMITracking(kTRUE);
      calibra->Init2Dhistos();
    }

  return 0;
}

int AliHLTTRDTrackerComponent::DoDeinit()
{
  delete fClusterizer;
  fClusterizer = 0;

  delete fField;

  delete fTracker;
  fTracker = 0;
  
  if (fGeometryFile)
    {
      fGeometryFile->Close();
      delete fGeometryFile;
    }

  AliTRDCalibraFillHisto *calibra = AliTRDCalibraFillHisto::Instance();
  if (calibra)
    {
      calibra->Write2d();
      calibra->Destroy();
    }

  return 0;
}

int AliHLTTRDTrackerComponent::DoEvent( const AliHLTComponentEventData & evtData,
					AliHLTComponentTriggerData & trigData )
{
  Logging( kHLTLogInfo, "HLT::TRDTracker::DoEvent", "Output percentage set", "Output percentage set to %lu %%", fOutputPercentage );
  Logging( kHLTLogInfo, "HLT::TRDTracker::DoEvent", "BLOCKS", "NofBlocks %lu", evtData.fBlockCnt );

  AliHLTUInt32_t fDblock_Specification = 0;

  AliHLTComponentBlockData *dblock = (AliHLTComponentBlockData *)GetFirstInputBlock( AliHLTTRDDefinitions::gkClusterDataType );
  if (dblock != 0)
    {
      fDblock_Specification = dblock->fSpecification;
    }
  else
    {
      Logging( kHLTLogWarning, "HLT::TRDTracker::DoEvent", "DATAIN", "First Input Block not found! 0x%x", dblock);
      return -1;
    }

  int ibForce = 0;
  TObject *tobjin = (TObject *)GetFirstInputObject( AliHLTTRDDefinitions::gkClusterDataType, "TObjArray", ibForce);
  Logging( kHLTLogInfo, "HLT::TRDTracker::DoEvent", "1stBLOCK", "Pointer = 0x%x", tobjin);

//   int iTotalClusterCounter = 0;
  while (tobjin != 0)
    {
      TObjArray *clusters = (TObjArray *)tobjin;
      if (clusters != 0)
	{
	  //put back to the clusterizers tree
	  Int_t iSuggestedDet = -1; // take the det number from the first cluster
	  Bool_t kInsert = kFALSE;
	  if (clusters->GetEntries() > 0)
	    {
	      AliTRDcluster *cl = (AliTRDcluster*)clusters->At(0);
	      iSuggestedDet = cl->GetDetector();
	      kInsert = fClusterizer->InsertClusters(clusters, iSuggestedDet);      
	      Logging( kHLTLogInfo, "HLT::TRDTracker::DoEvent", "TOTREE", "Result = %d", kInsert);	  
	    }
	    
	  Logging( kHLTLogInfo, "HLT::TRDTracker::DoEvent", "CLUSTERS", "Pointer = 0x%x", clusters);	  
// 	  //dump the clusters to the log files
// 	  for (Int_t ic = 0; ic < clusters->GetEntries(); ic++)
// 	    {
// 	      AliTRDcluster *cl = (AliTRDcluster*)clusters->At(ic);
// 	      Logging( kHLTLogInfo, "HLT::TRDTracker::DoEvent", "CLUSTER", "%d : %d : Q = %1.2f", 
// 		       iTotalClusterCounter, ic, cl->GetQ());	      
// 	      iTotalClusterCounter++;
// 	    }
	  
	  //Pass the data further...
	  //PushBack(clusters, AliHLTTRDDefinitions::gkClusterDataType, fDblock_Specification);
	}
      else
	{
	  Logging( kHLTLogError, "HLT::TRDTracker::DoEvent", "CLUSTERS", "Pointer = 0x%x", clusters);	  
	}

      tobjin = (TObject *)GetNextInputObject( ibForce );
      Logging( kHLTLogInfo, "HLT::TRDTracker::DoEvent", "nextBLOCK", "Pointer = 0x%x", tobjin);

    }

  Int_t iNclusters = fClusterizer->GetNclusters();
  Logging( kHLTLogInfo, "HLT::TRDTracker::DoEvent", "COUNT", "N of Clusters = %d", iNclusters);
  fTracker->LoadClusters(fClusterizer->GetClusterTree());

  AliTRDReconstructor::SetSeedingOn(kTRUE);

  AliESD *esd = new AliESD();

  //fTracker->MakeSeedsMI(3, 5, esd);
  fTracker->PropagateBack(esd);

  //here transport the esd tracks further
  //no receiver defined yet(!)
  delete esd;
  return 0;
}
