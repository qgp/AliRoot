
#include <TDatabasePDG.h>
#include <TString.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TFile.h>
#include <TPRegexp.h>

#include <AliExternalTrackParam.h>
#include <AliTPCcalibDB.h>
#include <AliTPCclusterMI.h>
#include <AliTPCCorrection.h>
#include <AliTrackerBase.h>
#include <AliTrackPointArray.h>
#include <AliLog.h>
#include <AliTPCParam.h>
#include <AliTPCROC.h>
#include <TTreeStream.h>
#include <AliTPCReconstructor.h>
#include <AliTPCTransform.h>
#include <AliTPCseed.h>
#include <AliTPCtracker.h>
#include <AliTPCtrackerSector.h>
#include <AliRieman.h>

#include "AliToyMCTrack.h"
#include "AliToyMCEvent.h"

#include "AliToyMCReconstruction.h"

/*



*/

//____________________________________________________________________________________
AliToyMCReconstruction::AliToyMCReconstruction() : TObject()
, fSeedingRow(140)
, fSeedingDist(10)
, fClusterType(0)
, fCorrectionType(kNoCorrection)
, fDoTrackFit(kTRUE)
, fUseMaterial(kFALSE)
, fIdealTracking(kFALSE)
, fNmaxEvents(-1)
, fTime0(-1)
, fCreateT0seed(kFALSE)
, fStreamer(0x0)
, fInputFile(0x0)
, fTree(0x0)
, fEvent(0x0)
, fTPCParam(0x0)
, fTPCCorrection(0x0)
, fkNSectorInner(18) // hard-coded to avoid loading the parameters before
, fInnerSectorArray(0x0)
, fkNSectorOuter(18) // hard-coded to avoid loading the parameters before
, fOuterSectorArray(0x0)
, fAllClusters("AliTPCclusterMI",10000)
, fMapTrackEvent(10000)
, fMapTrackTrackInEvent(10000)
, fIsAC(kFALSE)
{
  //
  //  ctor
  //
  fTPCParam=AliTPCcalibDB::Instance()->GetParameters();

}

//____________________________________________________________________________________
AliToyMCReconstruction::~AliToyMCReconstruction()
{
  //
  //  dtor
  //

  Cleanup();
}

//____________________________________________________________________________________
void AliToyMCReconstruction::RunReco(const char* file, Int_t nmaxEv)
{
  //
  // Recostruction from associated clusters
  //

  ConnectInputFile(file, nmaxEv);
  if (!fTree) return;

  InitStreamer(".debug");
  
  gROOT->cd();

  static AliExternalTrackParam dummySeedT0;
  static AliExternalTrackParam dummySeed;
  static AliExternalTrackParam dummyTrack;

  AliExternalTrackParam t0seed;
  AliExternalTrackParam seed;
  AliExternalTrackParam track;
  AliExternalTrackParam tOrig;

  AliExternalTrackParam *dummy;
  
  Int_t maxev=fTree->GetEntries();
  if (nmaxEv>0&&nmaxEv<maxev) maxev=nmaxEv;
  
  for (Int_t iev=0; iev<maxev; ++iev){
    printf("==============  Processing Event %6d =================\n",iev);
    fTree->GetEvent(iev);
    for (Int_t itr=0; itr<fEvent->GetNumberOfTracks(); ++itr){
//       printf(" > ======  Processing Track %6d ========  \n",itr);
      const AliToyMCTrack *tr=fEvent->GetTrack(itr);
      tOrig = *tr;

      
      // set dummy 
      t0seed    = dummySeedT0;
      seed      = dummySeed;
      track     = dummyTrack;
      
      Float_t z0=fEvent->GetZ();
      Float_t t0=fEvent->GetT0();

      Float_t vDrift=GetVDrift();
      Float_t zLength=GetZLength(0);

      // crate time0 seed, steered by fCreateT0seed
//       printf("t0 seed\n");
      fTime0=-1.;
      fCreateT0seed=kTRUE;
      dummy = GetSeedFromTrack(tr);
      
      if (dummy) {
        t0seed = *dummy;
        delete dummy;

        // crate real seed using the time 0 from the first seed
        // set fCreateT0seed now to false to get the seed in z coordinates
        fTime0 = t0seed.GetZ()-zLength/vDrift;
        fCreateT0seed = kFALSE;
//         printf("seed (%.2g)\n",fTime0);
        dummy  = GetSeedFromTrack(tr);
        if (dummy) {
          seed = *dummy;
          delete dummy;

          // create fitted track
          if (fDoTrackFit){
//             printf("track\n");
            dummy = GetFittedTrackFromSeed(tr, &seed);
            track = *dummy;
            delete dummy;
          }

          // propagate seed to 0
          const Double_t kMaxSnp = 0.85;
          const Double_t kMass = TDatabasePDG::Instance()->GetParticle("pi+")->Mass();
          AliTrackerBase::PropagateTrackTo(&seed,0,kMass,5,kTRUE,kMaxSnp,0,kFALSE,fUseMaterial);
          
        }
      }

      Int_t ctype(fCorrectionType);
      
      if (fStreamer) {
        (*fStreamer) << "Tracks" <<
        "iev="         << iev             <<
        "z0="          << z0              <<
        "t0="          << t0              <<
        "fTime0="      << fTime0          <<
        "itr="         << itr             <<
        "clsType="     << fClusterType    <<
        "corrType="    << ctype           <<
        "seedRow="     << fSeedingRow     <<
        "seedDist="    << fSeedingDist    <<
        "vDrift="      << vDrift          <<
        "zLength="     << zLength         <<
        "t0seed.="     << &t0seed         <<
        "seed.="       << &seed           <<
        "track.="      << &track          <<
        "tOrig.="      << &tOrig          <<
        "\n";
      }
      
      
    }
  }

  Cleanup();
}


//____________________________________________________________________________________
void AliToyMCReconstruction::RunRecoAllClusters(const char* file, Int_t nmaxEv)
{
  //
  // Reconstruction for seed from associated clusters, but array of clusters:
  // Step 1) Filling of cluster arrays
  // Step 2) Seeding from clusters associated to tracks
  // Step 3) Free track reconstruction using all clusters
  //

  TFile f(file);
  if (!f.IsOpen() || f.IsZombie()) {
    printf("ERROR: couldn't open the file '%s'\n", file);
    return;
  }
  
 fTree=(TTree*)f.Get("toyMCtree");
  if (!fTree) {
    printf("ERROR: couldn't read the 'toyMCtree' from file '%s'\n", file);
    return;
  }

  fEvent=0x0;
  fTree->SetBranchAddress("event",&fEvent);
  
  // read spacecharge from the Userinfo ot the tree
  InitSpaceCharge();
  
  TString debugName=file;
  debugName.ReplaceAll(".root","");
  debugName.Append(Form(".%1d.%1d_%1d_%1d_%03d_%02d",
                        fUseMaterial,fIdealTracking,fClusterType,
                        Int_t(fCorrectionType),fSeedingRow,fSeedingDist));
  debugName.Append(".allClusters.debug.root");
  
  gSystem->Exec(Form("test -f %s && rm %s", debugName.Data(), debugName.Data()));
  if (!fStreamer) fStreamer=new TTreeSRedirector(debugName.Data());
  
  gROOT->cd();

  static AliExternalTrackParam dummySeedT0;
  static AliExternalTrackParam dummySeed;
  static AliExternalTrackParam dummyTrack;

  AliExternalTrackParam t0seed;
  AliExternalTrackParam seed;
  AliExternalTrackParam track;
  AliExternalTrackParam tOrig;

  AliExternalTrackParam *dummy;
  
  Int_t maxev=fTree->GetEntries();
  if (nmaxEv>0&&nmaxEv<maxev) maxev=nmaxEv;
  
  // ===========================================================================================
  // Loop 1: Fill AliTPCtrackerSector structure
  // ===========================================================================================
  FillSectorStructure(maxev);

  // settings (TODO: find the correct settings)
  AliTPCRecoParam *tpcRecoParam = new AliTPCRecoParam();
  tpcRecoParam->SetDoKinks(kFALSE);
  AliTPCcalibDB::Instance()->GetTransform()->SetCurrentRecoParam(tpcRecoParam);
  //tpcRecoParam->Print();

  // need AliTPCReconstructor for parameter settings in AliTPCtracker
  AliTPCReconstructor *tpcRec   = new AliTPCReconstructor();
  tpcRec->SetRecoParam(tpcRecoParam);


  // ===========================================================================================
  // Loop 2: Seeding from clusters associated to tracks
  // TODO: Implement tracking from given seed!
  // ===========================================================================================
  for (Int_t iev=0; iev<maxev; ++iev){
    printf("==============  Processing Event %6d =================\n",iev);
    fTree->GetEvent(iev);
    for (Int_t itr=0; itr<fEvent->GetNumberOfTracks(); ++itr){
      printf(" > ======  Processing Track %6d ========  \n",itr);
      const AliToyMCTrack *tr=fEvent->GetTrack(itr);
      tOrig = *tr;

      
      // set dummy 
      t0seed    = dummySeedT0;
      seed      = dummySeed;
      track     = dummyTrack;
      
      Float_t z0=fEvent->GetZ();
      Float_t t0=fEvent->GetT0();

      Float_t vDrift=GetVDrift();
      Float_t zLength=GetZLength(0);

      Int_t nClus = 0;

      // crate time0 seed, steered by fCreateT0seed
      printf("t0 seed\n");
      fTime0=-1.;
      fCreateT0seed=kTRUE;
      dummy = GetSeedFromTrack(tr);
      
      if (dummy) {
        t0seed = *dummy;
        delete dummy;

        // crate real seed using the time 0 from the first seed
        // set fCreateT0seed now to false to get the seed in z coordinates
        fTime0 = t0seed.GetZ()-zLength/vDrift;
        fCreateT0seed = kFALSE;
        printf("seed (%.2g)\n",fTime0);
        dummy  = GetSeedFromTrack(tr);
  	if (dummy) {
          seed = *dummy;
          delete dummy;
	  
          // create fitted track
          if (fDoTrackFit){
            printf("track\n");
            dummy = GetFittedTrackFromSeedAllClusters(tr, &seed,nClus);
            track = *dummy;
            delete dummy;
          }
	  
          // propagate seed to 0
          const Double_t kMaxSnp = 0.85;
          const Double_t kMass = TDatabasePDG::Instance()->GetParticle("pi+")->Mass();
  	  AliTrackerBase::PropagateTrackTo(&seed,0,kMass,5,kTRUE,kMaxSnp,0,kFALSE,kFALSE);
          
        }
      }

      Int_t ctype(fCorrectionType);
      
      if (fStreamer) {
        (*fStreamer) << "Tracks" <<
        "iev="         << iev             <<
        "z0="          << z0              <<
        "t0="          << t0              <<
        "fTime0="      << fTime0          <<
        "itr="         << itr             <<
        "clsType="     << fClusterType    <<
        "corrType="    << ctype           <<
        "seedRow="     << fSeedingRow     <<
        "seedDist="    << fSeedingDist    <<
        "vDrift="      << vDrift          <<
        "zLength="     << zLength         <<
        "nClus="       << nClus           <<
        "t0seed.="     << &t0seed         <<
        "seed.="       << &seed           <<
        "track.="      << &track          <<
        "tOrig.="      << &tOrig          <<
        "\n";
      }
      
      
    }
  }


  delete fStreamer;
  fStreamer=0x0;

  delete fEvent;
  fEvent = 0x0;
  
  delete fTree;
  fTree=0x0;
  f.Close();
}

//____________________________________________________________________________________
void AliToyMCReconstruction::RunRecoAllClustersStandardTracking(const char* file, Int_t nmaxEv)
{
  //
  // Reconstruction for seed from associated clusters, but array of clusters
  // Step 1) Filling of cluster arrays
  // Step 2) Use the standard tracking: AliTPCtracker::Clusters2Tracks();
  //

  TFile f(file);
  if (!f.IsOpen() || f.IsZombie()) {
    printf("ERROR: couldn't open the file '%s'\n", file);
    return;
  }
  
 fTree=(TTree*)f.Get("toyMCtree");
  if (!fTree) {
    printf("ERROR: couldn't read the 'toyMCtree' from file '%s'\n", file);
    return;
  }

  fEvent=0x0;
  fTree->SetBranchAddress("event",&fEvent);
  
  // read spacecharge from the Userinfo ot the tree
  InitSpaceCharge();
  
  TString debugName=file;
  debugName.ReplaceAll(".root","");
  debugName.Append(Form(".%1d.%1d_%1d_%1d_%03d_%02d",
                        fUseMaterial,fIdealTracking,fClusterType,
                        Int_t(fCorrectionType),fSeedingRow,fSeedingDist));
  debugName.Append(".allClusters.debug.root");
  
  gSystem->Exec(Form("test -f %s && rm %s", debugName.Data(), debugName.Data()));
  if (!fStreamer) fStreamer=new TTreeSRedirector(debugName.Data());
  
  gROOT->cd();

  AliExternalTrackParam t0seed;
  AliExternalTrackParam seed;
  AliExternalTrackParam track;
  AliExternalTrackParam tOrig;
  AliToyMCTrack tOrigToy;

  AliExternalTrackParam *dummy;
  AliTPCseed            *seedBest;
  AliTPCseed            *seedTmp;
  AliTPCclusterMI       *cluster;

  Int_t maxev=fTree->GetEntries();
  if (nmaxEv>0&&nmaxEv<maxev) maxev=nmaxEv;
  

  // ===========================================================================================
  // Loop 1: Fill AliTPCtrackerSector structure
  // ===========================================================================================
  FillSectorStructure(maxev);

  // ===========================================================================================
  // Loop 2: Use the TPC tracker for seeding (MakeSeeds3) 
  // TODO: - check tracking configuration
  //       - add clusters and original tracks to output (how?)
  // ===========================================================================================

  // settings (TODO: find the correct settings)
  AliTPCRecoParam *tpcRecoParam = new AliTPCRecoParam();
  tpcRecoParam->SetDoKinks(kFALSE);
  AliTPCcalibDB::Instance()->GetTransform()->SetCurrentRecoParam(tpcRecoParam);
  //tpcRecoParam->Print();

  // need AliTPCReconstructor for parameter settings in AliTPCtracker
  AliTPCReconstructor *tpcRec   = new AliTPCReconstructor();
  tpcRec->SetRecoParam(tpcRecoParam);

  // AliTPCtracker
  AliTPCtracker *tpcTracker = new AliTPCtracker(fTPCParam);
  tpcTracker->SetDebug(10);

  // set sector arrays
  tpcTracker->SetTPCtrackerSectors(fInnerSectorArray,fOuterSectorArray);
  tpcTracker->LoadInnerSectors();
  tpcTracker->LoadOuterSectors();

  // seeding
  static TObjArray arrTracks;
  TObjArray * arr    = &arrTracks;
  TObjArray * seeds  = new TObjArray;

  // cuts for seeding 
//   Float_t cuts[4];
//   cuts[0]=0.0070;  // cuts[0]   - fP4 cut
//   cuts[1] = 1.5;   // cuts[1]   - tan(phi) cut
//   cuts[2] = 3.;    // cuts[2]   - zvertex cut
//   cuts[3] = 3.;    // cuts[3]   - fP3 cut

  // rows for seeding
  Int_t lowerRow = fSeedingRow;
  Int_t upperRow = fSeedingRow+2*fSeedingDist;
  const AliTPCROC * roc      = AliTPCROC::Instance();
  const Int_t kNRowsInnerTPC = roc->GetNRows(0); 
  const Int_t kNRowsTPC      = kNRowsInnerTPC + roc->GetNRows(36); 
  if(lowerRow < kNRowsInnerTPC){
    Printf("Seeding row requested (%d) is lower than kNRowsInnerTPC --> use %d",lowerRow,kNRowsInnerTPC);
    lowerRow = kNRowsInnerTPC;
    upperRow = lowerRow + 20;
  }
  if(upperRow >= kNRowsTPC){
    Printf("Seeding row requested (%d) is larger than kNRowsTPC --> use %d",upperRow,kNRowsTPC-1);
    upperRow = kNRowsTPC-1;
    lowerRow = upperRow-20;
  }
 
  // do the seeding
  for (Int_t sec=0;sec<fkNSectorOuter;sec++){
    //
    //tpcTracker->MakeSeeds3(arr, sec,upperRow,lowerRow,cuts,-1,1);
//     MakeSeeds(arr, sec,upperRow,lowerRow); // own function (based on TLinearFitter)
    MakeSeeds2(arr, sec,upperRow,lowerRow); // own function (based on TLinearFitter)
    //tpcTracker->SumTracks(seeds,arr);   
    //tpcTracker->SignClusters(seeds,3.0,3.0);    

  }

  Printf("After seeding we have %d tracks",seeds->GetEntriesFast());

  // Standard tracking
  tpcTracker->SetSeeds(seeds);
  tpcTracker->PropagateForward();
  Printf("After trackinging we have %d tracks",seeds->GetEntriesFast());
return;

  // Loop over all input tracks and connect to found seeds
  for (Int_t iev=0; iev<maxev; ++iev){
    printf("==============  Fill Tracks: Processing Event %6d  =================\n",iev);
    fTree->GetEvent(iev);
    for (Int_t itr=0; itr<fEvent->GetNumberOfTracks(); ++itr){
      printf(" > ======  Fill Tracks: Processing Track %6d  ========  \n",itr);
      const AliToyMCTrack *tr=fEvent->GetTrack(itr);
      tOrig = *tr;
      tOrigToy = *tr;

      Float_t z0=fEvent->GetZ();
      Float_t t0=fEvent->GetT0();
      Float_t vDrift=GetVDrift();
      Float_t zLength=GetZLength(0);

      // find the corresponding seed (and track)
      Int_t trackID            = tr->GetUniqueID();
      Int_t nClustersMC        = tr->GetNumberOfSpacePoints();      // number of findable clusters (ideal)
      if(fClusterType==1) 
	    nClustersMC        = tr->GetNumberOfDistSpacePoints();  // number of findable clusters (distorted)
//       Int_t idxSeed            = 0; // index of best seed (best is with maximum number of clusters with correct ID)
      Int_t nSeeds             = 0; // number of seeds for MC track
      Int_t nSeedClusters      = 0; // number of clusters for best seed
      Int_t nSeedClustersTmp   = 0; // number of clusters for current seed
      Int_t nSeedClustersID    = 0; // number of clusters with correct ID for best seed 
      Int_t nSeedClustersIDTmp = 0; // number of clusters with correct ID for current seed 
      for(Int_t iSeeds = 0; iSeeds < seeds->GetEntriesFast(); ++iSeeds){
	
	// set current seed and reset counters
	seedTmp = (AliTPCseed*)(seeds->At(iSeeds));
	nSeedClustersTmp   = 0;
	nSeedClustersIDTmp = 0;

	if(!seedTmp) continue;

	// loop over all rows
	for(Int_t iRow = seedTmp->GetRow(); iRow < seedTmp->GetRow() + seedTmp->GetNumberOfClustersIndices(); iRow++ ){
       
	  // get cluster and increment counters
	  cluster = seedTmp->GetClusterFast(iRow);
	  if(cluster){
	    nSeedClustersTmp++;
	    if(cluster->GetLabel(0)==trackID){
	      nSeedClustersIDTmp++;
	    }
	  }
	}
	
	// if number of corresponding clusters > 0,
	// increase nSeeds
	if(nSeedClustersTmp > 0){
	  nSeeds++;
	}

	// if number of corresponding clusters bigger than current nSeedClusters,
	// take this seed as the best one
	if(nSeedClustersIDTmp > nSeedClustersID){
//  	  idxSeed  = iSeeds;
	  seedBest = seedTmp;
	  nSeedClusters   = nSeedClustersTmp;   // number of correctly assigned clusters
	  nSeedClustersID = nSeedClustersIDTmp; // number of all clusters
	}

      }

      // cluster to track association (commented out, when used standard tracking)
      if (nSeeds>0&&nSeedClusters>0) {
       	t0seed = (AliExternalTrackParam)*seedBest;
//        	fTime0 = t0seed.GetZ()-zLength/vDrift;
        // get the refitted track from the seed
        // this will also set the fTime0 from the seed extrapolation
        dummy=GetRefittedTrack(*seedBest);
        track=*dummy;
        delete dummy;
       	//printf("seed (%.2g): %d seeds with %d clusters\n",fTime0,nSeeds,nSeedClusters);

	// 	// cluster to track association for all good seeds 
	// 	// set fCreateT0seed to true to get the seed in time coordinates
	// 	fCreateT0seed = kTRUE;
	// 	dummy = ClusterToTrackAssociation(seedBest,trackID,nClus); 
	
	//// propagate track to 0
       	//const Double_t kMaxSnp = 0.85;
       	//const Double_t kMass = TDatabasePDG::Instance()->GetParticle("pi+")->Mass();
       	//AliTrackerBase::PropagateTrackTo(&track,0,kMass,5,kTRUE,kMaxSnp,0,kFALSE,kFALSE);
	
      }
      
      Int_t ctype(fCorrectionType);
      
      if (fStreamer) {
        (*fStreamer) << "Tracks" <<
	  "iev="             << iev             <<
	  "z0="              << z0              <<
	  "t0="              << t0              <<
	  "fTime0="          << fTime0          <<
	  "itr="             << itr             <<
	  "clsType="         << fClusterType    <<
	  "corrType="        << ctype           <<
	  "seedRow="         << fSeedingRow     <<
	  "seedDist="        << fSeedingDist    <<
	  "vDrift="          << vDrift          <<
	  "zLength="         << zLength         <<
	  "nClustersMC="     << nClustersMC     <<
	  "nSeeds="          << nSeeds          <<
	  "nSeedClusters="   << nSeedClusters   <<
	  "nSeedClustersID=" << nSeedClustersID <<
	  "t0seed.="         << &t0seed         <<
	  "track.="          << &track          <<
	  "tOrig.="          << &tOrig          <<
	  "tOrigToy.="       << &tOrigToy       <<
	  "\n";
      }
    }
  }

   
  delete fStreamer;
  fStreamer=0x0;

  delete fEvent;
  fEvent = 0x0;
  
  delete fTree;
  fTree=0x0;
  f.Close();
}


//____________________________________________________________________________________
void AliToyMCReconstruction::RunFullTracking(const char* file, Int_t nmaxEv)
{
  //
  //
  //

  ConnectInputFile(file,nmaxEv);
  if (!fTree) return;
  
  InitStreamer(".fullTracking");
  
  FillSectorStructureAC();

  AliTPCReconstructor::SetStreamLevel(0);
  
  TObjArray seeds;
  seeds.SetOwner();
  const Int_t lowerRow=fSeedingRow;
  const Int_t upperRow=fSeedingRow+2*fSeedingDist;

  // seeding, currently only for outer sectors
  for (Int_t sec=0;sec<36;sec++){
    printf("Seeding in sector: %d\n",sec);
    MakeSeeds2(&seeds, sec,lowerRow,upperRow);
  }

  DumpSeedInfo(&seeds,lowerRow,upperRow);

  Cleanup();
}

//____________________________________________________________________________________
AliExternalTrackParam* AliToyMCReconstruction::GetSeedFromTrack(const AliToyMCTrack * const tr)
{
  //
  // if we don't have a valid time0 informaion (fTime0) available yet
  // assume we create a seed for the time0 estimate
  //

  // seed point informaion
  AliTrackPoint    seedPoint[3];
  const AliTPCclusterMI *seedCluster[3]={0x0,0x0,0x0};
  
  // number of clusters to loop over
  const Int_t ncls=(fClusterType==0)?tr->GetNumberOfSpacePoints():tr->GetNumberOfDistSpacePoints();
  
  UChar_t nextSeedRow=fSeedingRow;
  Int_t   nseeds=0;
  
  //assumes sorted clusters
  for (Int_t icl=0;icl<ncls;++icl) {
    const AliTPCclusterMI *cl=tr->GetSpacePoint(icl);
    if (fClusterType==1) cl=tr->GetDistortedSpacePoint(icl);
    if (!cl) continue;
    // use row in sector
    const UChar_t row=cl->GetRow() + 63*(cl->GetDetector()>35);
    // skip clusters without proper pad row
    if (row>200) continue;
    
    //check seeding row
    // if we are in the last row and still miss a seed we use the last row
    //   even if the row spacing will not be equal
    if (row>=nextSeedRow || icl==ncls-1){
      seedCluster[nseeds]=cl;
      SetTrackPointFromCluster(cl, seedPoint[nseeds]);
      ++nseeds;
      nextSeedRow+=fSeedingDist;
      
      if (nseeds==3) break;
    }
  }
  
  // check we really have 3 seeds
  if (nseeds!=3) {
    AliError(Form("Seeding failed for parameters %d, %d\n",fSeedingDist,fSeedingRow));
    return 0x0;
  }
  
  // do cluster correction for fCorrectionType:
  //   0 - no correction
  //   1 - TPC center
  //   2 - average eta
  //   3 - ideal
  // assign the cluster abs time as z component to all seeds
  for (Int_t iseed=0; iseed<3; ++iseed) {
    Float_t xyz[3]={0,0,0};
    seedPoint[iseed].GetXYZ(xyz);
    const Float_t r=TMath::Sqrt(xyz[0]*xyz[0]+xyz[1]*xyz[1]);
    
    const Int_t sector=seedCluster[iseed]->GetDetector();
    const Int_t sign=1-2*((sector/18)%2);
    
    if ( (fClusterType == 1) && (fCorrectionType != kNoCorrection) ) {
//       printf("correction type: %d\n",(Int_t)fCorrectionType);

      // the settings below are for the T0 seed
      // for known T0 the z position is already calculated in SetTrackPointFromCluster
      if ( fCreateT0seed ){
        if ( fCorrectionType == kTPCCenter  ) xyz[2] = 125.*sign;
        //!!! TODO: is this the correct association?
        if ( fCorrectionType == kAverageEta ) xyz[2] = TMath::Tan(45./2.*TMath::DegToRad())*r*sign;
      }
      
      if ( fCorrectionType == kIdeal      ) xyz[2] = seedCluster[iseed]->GetZ();
      
      //!!! TODO: to be replaced with the proper correction
      fTPCCorrection->CorrectPoint(xyz, seedCluster[iseed]->GetDetector());
    }

    // after the correction set the time bin as z-Position in case of a T0 seed
    if ( fCreateT0seed )
      xyz[2]=seedCluster[iseed]->GetTimeBin();
    
    seedPoint[iseed].SetXYZ(xyz);
  }
  
  const Double_t kMaxSnp = 0.85;
  const Double_t kMass = TDatabasePDG::Instance()->GetParticle("pi+")->Mass();
  
  AliExternalTrackParam *seed = AliTrackerBase::MakeSeed(seedPoint[0], seedPoint[1], seedPoint[2]);
  seed->ResetCovariance(10);

  if (fCreateT0seed){
    // if fTime0 < 0 we assume that we create a seed for the T0 estimate
    AliTrackerBase::PropagateTrackTo(seed,0,kMass,5,kTRUE,kMaxSnp,0,kFALSE,kFALSE);
    if (TMath::Abs(seed->GetX())>3) {
//       printf("Could not propagate track to 0, %.2f, %.2f, %.2f\n",seed->GetX(),seed->GetAlpha(),seed->Pt());
    }
  }
  
  return seed;
  
}


//____________________________________________________________________________________
void AliToyMCReconstruction::SetTrackPointFromCluster(const AliTPCclusterMI *cl, AliTrackPoint &p )
{
  //
  // make AliTrackPoint out of AliTPCclusterMI
  //
  
  if (!cl) return;
    Float_t xyz[3]={0.,0.,0.};
  //   ClusterToSpacePoint(cl,xyz);
  //   cl->GetGlobalCov(cov);
  //TODO: what to do with the covariance matrix???
  //TODO: the problem is that it is used in GetAngle in AliTrackPoint
  //TODO: which is used by AliTrackerBase::MakeSeed to get alpha correct ...
  //TODO: for the moment simply assign 1 permill squared
  // in AliTrackPoint the cov is xx, xy, xz, yy, yz, zz
  //   Float_t cov[6]={xyz[0]*xyz[0]*1e-6,xyz[0]*xyz[1]*1e-6,xyz[0]*xyz[2]*1e-6,
  //                   xyz[1]*xyz[1]*1e-6,xyz[1]*xyz[2]*1e-6,xyz[2]*xyz[2]*1e-6};
  //   cl->GetGlobalXYZ(xyz);
  //   cl->GetGlobalCov(cov);
  // voluem ID to add later ....
  //   p.SetXYZ(xyz);
  //   p.SetCov(cov);
//   AliTrackPoint *tp=const_cast<AliTPCclusterMI*>(cl)->MakePoint(p);
//   p=*tp;
//   delete tp;
  const_cast<AliTPCclusterMI*>(cl)->MakePoint(p);
  //   cl->Print();
  //   p.Print();
  p.SetVolumeID(cl->GetDetector());
  
  
  if ( !fCreateT0seed && !fIdealTracking ) {
    p.GetXYZ(xyz);
    const Int_t sector=cl->GetDetector();
    const Int_t sign=1-2*((sector/18)%2);
    const Float_t zT0=( GetZLength(sector) - (cl->GetTimeBin()-fTime0)*GetVDrift() )*sign;
//     printf(" z:  %.2f  %.2f\n",xyz[2],zT0);
    xyz[2]=zT0;
    p.SetXYZ(xyz);
  }
  
  
  //   p.Rotate(p.GetAngle()).Print();
}

//____________________________________________________________________________________
void AliToyMCReconstruction::ClusterToSpacePoint(const AliTPCclusterMI *cl, Float_t xyz[3])
{
  //
  // convert the cluster to a space point in global coordinates
  //
  if (!cl) return;
  xyz[0]=cl->GetRow();
  xyz[1]=cl->GetPad();
  xyz[2]=cl->GetTimeBin(); // this will not be correct at all
  Int_t i[3]={0,cl->GetDetector(),cl->GetRow()};
  //   printf("%.2f, %.2f, %.2f - %d, %d, %d\n",xyz[0],xyz[1],xyz[2],i[0],i[1],i[2]);
  fTPCParam->Transform8to4(xyz,i);
  //   printf("%.2f, %.2f, %.2f - %d, %d, %d\n",xyz[0],xyz[1],xyz[2],i[0],i[1],i[2]);
  fTPCParam->Transform4to3(xyz,i);
  //   printf("%.2f, %.2f, %.2f - %d, %d, %d\n",xyz[0],xyz[1],xyz[2],i[0],i[1],i[2]);
  fTPCParam->Transform2to1(xyz,i);
  //   printf("%.2f, %.2f, %.2f - %d, %d, %d\n",xyz[0],xyz[1],xyz[2],i[0],i[1],i[2]);
}

//____________________________________________________________________________________
AliExternalTrackParam* AliToyMCReconstruction::GetFittedTrackFromSeed(const AliToyMCTrack *tr, const AliExternalTrackParam *seed)
{
  //
  //
  //

  // create track
  AliExternalTrackParam *track = new AliExternalTrackParam(*seed);

  Int_t ncls=(fClusterType == 0)?tr->GetNumberOfSpacePoints():tr->GetNumberOfDistSpacePoints();

  const AliTPCROC * roc = AliTPCROC::Instance();
  
  const Double_t kRTPC0  = roc->GetPadRowRadii(0,0);
  const Double_t kRTPC1  = roc->GetPadRowRadii(36,roc->GetNRows(36)-1);
  const Double_t kMaxSnp = 0.85;
  const Double_t kMaxR   = 500.;
  const Double_t kMaxZ   = 500.;
  
  //   const Double_t kMaxZ0=220;
//   const Double_t kZcut=3;
  
  const Double_t refX = tr->GetX();
  
  const Double_t kMass = TDatabasePDG::Instance()->GetParticle("pi+")->Mass();
  
  // loop over all other points and add to the track
  for (Int_t ipoint=ncls-1; ipoint>=0; --ipoint){
    AliTrackPoint pIn;
    const AliTPCclusterMI *cl=tr->GetSpacePoint(ipoint);
    if (fClusterType == 1) cl=tr->GetDistortedSpacePoint(ipoint);
    SetTrackPointFromCluster(cl, pIn);
    if (fCorrectionType != kNoCorrection){
      Float_t xyz[3]={0,0,0};
      pIn.GetXYZ(xyz);
//       if ( fCorrectionType == kIdeal ) xyz[2] = cl->GetZ();
      fTPCCorrection->CorrectPoint(xyz, cl->GetDetector());
      pIn.SetXYZ(xyz);
    }
    // rotate the cluster to the local detector frame
    track->Rotate(((cl->GetDetector()%18)*20+10)*TMath::DegToRad());
    AliTrackPoint prot = pIn.Rotate(track->GetAlpha());   // rotate to the local frame - non distoted  point
    if (TMath::Abs(prot.GetX())<kRTPC0) continue;
    if (TMath::Abs(prot.GetX())>kRTPC1) continue;
    //
    Bool_t res=AliTrackerBase::PropagateTrackTo(track,prot.GetX(),kMass,5,kFALSE,kMaxSnp,0,kFALSE,fUseMaterial);

    if (!res) break;
    
    if (TMath::Abs(track->GetZ())>kMaxZ) break;
    if (TMath::Abs(track->GetX())>kMaxR) break;
//     if (TMath::Abs(track->GetZ())<kZcut)continue;
    //
    Double_t pointPos[2]={0,0};
    Double_t pointCov[3]={0,0,0};
    pointPos[0]=prot.GetY();//local y
    pointPos[1]=prot.GetZ();//local z
    pointCov[0]=prot.GetCov()[3];//simay^2
    pointCov[1]=prot.GetCov()[4];//sigmayz
    pointCov[2]=prot.GetCov()[5];//sigmaz^2
    
    if (!track->Update(pointPos,pointCov)) {printf("no update\n"); break;}
  }

  AliTrackerBase::PropagateTrackTo2(track,refX,kMass,5.,kTRUE,kMaxSnp,0,kFALSE,fUseMaterial);

  // rotate fittet track to the frame of the original track and propagate to same reference
  track->Rotate(tr->GetAlpha());
  
  AliTrackerBase::PropagateTrackTo2(track,refX,kMass,1.,kFALSE,kMaxSnp,0,kFALSE,fUseMaterial);
  
  return track;
}


//____________________________________________________________________________________
AliExternalTrackParam* AliToyMCReconstruction::GetFittedTrackFromSeedAllClusters(const AliToyMCTrack *tr, const AliExternalTrackParam *seed, Int_t &nClus)
{
  //
  // Tracking for given seed on an array of clusters
  //

  // create track
  AliExternalTrackParam *track = new AliExternalTrackParam(*seed);
  
  const AliTPCROC * roc = AliTPCROC::Instance();
  
  const Double_t kRTPC0    = roc->GetPadRowRadii(0,0);
  const Double_t kRTPC1    = roc->GetPadRowRadii(36,roc->GetNRows(36)-1);
  const Int_t kNRowsTPC    = roc->GetNRows(0) + roc->GetNRows(36) - 1;
  const Int_t kIRowsTPC    = roc->GetNRows(0) - 1;
  const Double_t kMaxSnp   = 0.85;
  const Double_t kMaxR     = 500.;
  const Double_t kMaxZ     = 500.;
  const Double_t roady     = 100.;
  const Double_t roadz     = 100.;
  
  const Double_t refX = tr->GetX();
  
  const Double_t kMass = TDatabasePDG::Instance()->GetParticle("pi+")->Mass();
  
  Int_t  secCur   = -1;
  UInt_t indexCur = 0;
  Double_t xCur, yCur, zCur = 0.;

  Float_t vDrift = GetVDrift();

  // first propagate seed to outermost row
  AliTrackerBase::PropagateTrackTo(track,kRTPC1,kMass,5,kFALSE,kMaxSnp);

  // Loop over rows and find the cluster candidates
  for( Int_t iRow = kNRowsTPC; iRow >= 0; --iRow ){
        
    // inner or outer sector
    Bool_t bInnerSector = kTRUE;
    if(iRow > kIRowsTPC) bInnerSector = kFALSE;

    // nearest track point/cluster (to be found)
    AliTrackPoint nearestPoint;
    AliTPCclusterMI *nearestCluster = NULL;
  
    // Inner Sector
    if(bInnerSector){

      // Propagate to center of pad row and extract parameters
      AliTrackerBase::PropagateTrackTo(track,roc->GetPadRowRadii(0,iRow),kMass,5,kFALSE,kMaxSnp);
      xCur   = track->GetX();
      yCur   = track->GetY();
      zCur   = track->GetZ();
      if ( !fIdealTracking ) {
	zCur = zCur/vDrift + fTime0; // Look at time, not at z!
      }
      secCur = GetSector(track);
      
      // Find the nearest cluster (TODO: correct road settings!)
      Printf("inner tracking here: x = %.2f, y = %.2f, z = %.2f (Row %d Sector %d)",xCur,yCur,zCur,iRow,secCur);
      nearestCluster = fInnerSectorArray[secCur%fkNSectorInner][iRow].FindNearest2(yCur,zCur,roady,roadz,indexCur);
      
      // Move to next row if now cluster found
      if(!nearestCluster) continue;
      //Printf("Nearest Clusters = %d (of %d) ",indexCur,fInnerSectorArray[secCur%fkNSectorInner][iRow].GetN());
      
    }

    // Outer sector
    else{

      // Propagate to center of pad row and extract parameters
      AliTrackerBase::PropagateTrackTo(track,roc->GetPadRowRadii(36,iRow-kIRowsTPC-1),kMass,5,kFALSE,kMaxSnp);
      xCur   = track->GetX();
      yCur   = track->GetY();
      zCur   = track->GetZ();
      if ( !fIdealTracking ) {
	zCur = zCur/vDrift + fTime0; // Look at time, not at z!
      }
      secCur = GetSector(track);

      // Find the nearest cluster (TODO: correct road settings!)
      Printf("outer tracking here: x = %.2f, y = %.2f, z = %.2f (Row %d Sector %d)",xCur,yCur,zCur,iRow,secCur);
      nearestCluster = fOuterSectorArray[(secCur-fkNSectorInner*2)%fkNSectorOuter][iRow-kIRowsTPC-1].FindNearest2(yCur,zCur,roady,roadz,indexCur);

      // Move to next row if now cluster found
      if(!nearestCluster) continue;
      //Printf("Nearest Clusters = %d (of %d)",indexCur,fOuterSectorArray[(secCur-fkNSectorInner*2)%fkNSectorOuter][iRow-kIRowsTPC-1].GetN());

    }

    // create track point from cluster
    SetTrackPointFromCluster(nearestCluster,nearestPoint);
    
    //Printf("Track point = %.2f %.2f %.2f",nearestPoint.GetX(),nearestPoint.GetY(),nearestPoint.GetZ());

    // correction
    // TODO: also correction when looking for the next cluster?
    if (fCorrectionType != kNoCorrection){
      Float_t xyz[3]={0,0,0};
      nearestPoint.GetXYZ(xyz);
      fTPCCorrection->CorrectPoint(xyz, nearestCluster->GetDetector());
      nearestPoint.SetXYZ(xyz);
    }

    // rotate the cluster to the local detector frame
    track->Rotate(((nearestCluster->GetDetector()%18)*20+10)*TMath::DegToRad());
    AliTrackPoint prot = nearestPoint.Rotate(track->GetAlpha());   // rotate to the local frame - non distoted  point
    if (TMath::Abs(prot.GetX())<kRTPC0) continue;
    if (TMath::Abs(prot.GetX())>kRTPC1) continue;

    
    //Printf("Rotated Track point = %.2f %.2f %.2f",prot.GetX(),prot.GetY(),prot.GetZ());

    // update track with the nearest track point  
    Bool_t res=AliTrackerBase::PropagateTrackTo(track,prot.GetX(),kMass,5,kFALSE,kMaxSnp,0,kFALSE,fUseMaterial);

    if (!res) break;
    
    if (TMath::Abs(track->GetZ())>kMaxZ) break;
    if (TMath::Abs(track->GetX())>kMaxR) break;
    //if (TMath::Abs(track->GetZ())<kZcut)continue;

      Double_t pointPos[2]={0,0};
      Double_t pointCov[3]={0,0,0};
      pointPos[0]=prot.GetY();//local y
      pointPos[1]=prot.GetZ();//local z
      pointCov[0]=prot.GetCov()[3];//simay^2
      pointCov[1]=prot.GetCov()[4];//sigmayz
      pointCov[2]=prot.GetCov()[5];//sigmaz^2
  
      if (!track->Update(pointPos,pointCov)) {printf("no update\n");}

      ++nClus;
  }

  
  // propagation to refX
  AliTrackerBase::PropagateTrackTo(track,refX,kMass,5.,kTRUE,kMaxSnp,0,kFALSE,fUseMaterial);
  
  // rotate fittet track to the frame of the original track and propagate to same reference
  track->Rotate(tr->GetAlpha());
  
  AliTrackerBase::PropagateTrackTo(track,refX,kMass,1.,kFALSE,kMaxSnp,0,kFALSE,fUseMaterial);

  Printf("We have %d clusters in this track!",nClus);
  
  return track;
}

//____________________________________________________________________________________
AliExternalTrackParam* AliToyMCReconstruction::ClusterToTrackAssociation(const AliTPCseed *seed, Int_t trackID, Int_t &nClus)
{
  //
  // Cluster to track association for given seed on an array of clusters
  //

  // create track
  AliExternalTrackParam *track = new AliExternalTrackParam(*seed);
  
  const AliTPCROC * roc = AliTPCROC::Instance();
  
  const Double_t kRTPC0    = roc->GetPadRowRadii(0,0);
  const Double_t kRTPC1    = roc->GetPadRowRadii(36,roc->GetNRows(36)-1);
  const Int_t kNRowsTPC    = roc->GetNRows(0) + roc->GetNRows(36) - 1;
  const Int_t kIRowsTPC    = roc->GetNRows(0) - 1;
  const Double_t kMaxSnp   = 0.85;
  const Double_t kMaxR     = 500.;
  const Double_t kMaxZ     = 500.;
  const Double_t roady     = 0.1;
  const Double_t roadz     = 0.01;
    
  const Double_t kMass = TDatabasePDG::Instance()->GetParticle("pi+")->Mass();
  
  Int_t  secCur, secOld   = -1;
  UInt_t indexCur = 0;
  Double_t xCur, yCur, zCur = 0.;

//   Float_t vDrift = GetVDrift();

  // first propagate seed to outermost row
  Bool_t res0=AliTrackerBase::PropagateTrackTo(track,kRTPC1,kMass,5,kFALSE,kMaxSnp);

  // Loop over rows and find the cluster candidates
  for( Int_t iRow = kNRowsTPC; iRow >= 0; --iRow ){
        
    // inner or outer sector
    Bool_t bInnerSector = kTRUE;
    if(iRow > kIRowsTPC) bInnerSector = kFALSE;

    // nearest track point/cluster (to be found)
    AliTrackPoint nearestPoint;
    AliTPCclusterMI *nearestCluster = NULL;
  
    // Inner Sector
    if(bInnerSector){

      // Propagate to center of pad row and extract parameters
      AliTrackerBase::PropagateTrackTo(track,roc->GetPadRowRadii(0,iRow),kMass,5,kFALSE,kMaxSnp);
      xCur   = track->GetX();
      yCur   = track->GetY();
      zCur   = track->GetZ();
      secCur = GetSector(track);
      
      // Find the nearest cluster (TODO: correct road settings!)
      //Printf("inner tracking here: x = %.2f, y = %.2f, z = %.6f (Row %d Sector %d)",xCur,yCur,zCur,iRow,secCur);
      nearestCluster = fInnerSectorArray[secCur%fkNSectorInner][iRow].FindNearest2(yCur,zCur,roady,roadz,indexCur);

      // Look again at -y if nothing found here and the sector changed with respect to the last found cluster
      // Increase also the road in this case
      if(!nearestCluster && secCur != secOld && secOld > -1){
      	//Printf("inner tracking here 2: x = %.2f, y = %.2f, z = %.6f (Row %d Sector %d)",xCur,-yCur,zCur,iRow,secCur);
      	nearestCluster = fInnerSectorArray[secCur%fkNSectorInner][iRow].FindNearest2(-yCur,zCur,roady*100,roadz,indexCur);
      }
      
      // Move to next row if now cluster found
      if(!nearestCluster) continue;
      //Printf("Nearest Clusters = %d (of %d) ",indexCur,fInnerSectorArray[secCur%fkNSectorInner][iRow].GetN());
      
    }

    // Outer sector
    else{

      // Propagate to center of pad row and extract parameters
      AliTrackerBase::PropagateTrackTo(track,roc->GetPadRowRadii(36,iRow-kIRowsTPC-1),kMass,5,kFALSE,kMaxSnp);
      xCur   = track->GetX();
      yCur   = track->GetY();
      zCur   = track->GetZ();
      secCur = GetSector(track);

      // Find the nearest cluster (TODO: correct road settings!)
      Printf("res0 = %d, outer tracking here: x = %.2f, y = %.2f, z = %.6f (Row %d Sector %d)",res0,xCur,yCur,zCur,iRow,secCur);
      nearestCluster = fOuterSectorArray[(secCur-fkNSectorInner*2)%fkNSectorOuter][iRow-kIRowsTPC-1].FindNearest2(yCur,zCur,roady,roadz,indexCur);

      // Look again at -y if nothing found here and the sector changed with respect to the last found cluster
      // Increase also the road in this case
      if(!nearestCluster && secCur != secOld && secOld > -1){
      	Printf("outer tracking here 2: x = %.2f, y = %.2f, z = %.6f (Row %d Sector %d)",xCur,-yCur,zCur,iRow,secCur);
      	nearestCluster = fOuterSectorArray[(secCur-fkNSectorInner*2)%fkNSectorOuter][iRow-kIRowsTPC-1].FindNearest2(-yCur,zCur,roady*100,roadz,indexCur);
      }


      // Move to next row if now cluster found
      if(!nearestCluster) continue;
      Printf("Nearest Clusters = %d (of %d)",indexCur,fOuterSectorArray[(secCur-fkNSectorInner*2)%fkNSectorOuter][iRow-kIRowsTPC-1].GetN());

    }

    // create track point from cluster
    SetTrackPointFromCluster(nearestCluster,nearestPoint);

    //Printf("Track point = %.2f %.2f %.2f",nearestPoint.GetX(),nearestPoint.GetY(),nearestPoint.GetZ());

    // rotate the cluster to the local detector frame
    track->Rotate(((nearestCluster->GetDetector()%18)*20+10)*TMath::DegToRad());
    AliTrackPoint prot = nearestPoint.Rotate(track->GetAlpha());   // rotate to the local frame - non distoted  point
    if (TMath::Abs(prot.GetX())<kRTPC0) continue;
    if (TMath::Abs(prot.GetX())>kRTPC1) continue;

    
    //Printf("Rotated Track point = %.2f %.2f %.2f",prot.GetX(),prot.GetY(),prot.GetZ());

    // update track with the nearest track point  
    Bool_t res=AliTrackerBase::PropagateTrackTo(track,prot.GetX(),kMass,5,kFALSE,kMaxSnp,0,kFALSE,fUseMaterial);

    if (!res) break;
    
    if (TMath::Abs(track->GetZ())>kMaxZ) break;
    if (TMath::Abs(track->GetX())>kMaxR) break;
    //if (TMath::Abs(track->GetZ())<kZcut)continue;

      Double_t pointPos[2]={0,0};
      Double_t pointCov[3]={0,0,0};
      pointPos[0]=prot.GetY();//local y
      pointPos[1]=prot.GetZ();//local z
      pointCov[0]=prot.GetCov()[3];//simay^2
      pointCov[1]=prot.GetCov()[4];//sigmayz
      pointCov[2]=prot.GetCov()[5];//sigmaz^2
  
      if (!track->Update(pointPos,pointCov)) {printf("no update\n");}
      secOld = secCur;
      
      //Printf("Cluster belongs to track = %d",nearestCluster->GetLabel(0));
      
      // only count as associate cluster if it belongs to correct track!
      if(nearestCluster->GetLabel(0) == trackID)
	++nClus;
  }

  Printf("We have %d clusters in this track!",nClus);
  
  return track;
}


//____________________________________________________________________________________
void AliToyMCReconstruction::InitSpaceCharge()
{
  //
  // Init the space charge map
  //

  TString filename="$ALICE_ROOT/TPC/Calib/maps/SC_NeCO2_eps5_50kHz_precal.root";
  if (fTree) {
    TList *l=fTree->GetUserInfo();
    for (Int_t i=0; i<l->GetEntries(); ++i) {
      TString s(l->At(i)->GetName());
      if (s.Contains("SC_")) {
        filename=s;
        break;
      }
    }
  }

  printf("Initialising the space charge map using the file: '%s'\n",filename.Data());
  TFile f(filename.Data());
  fTPCCorrection=(AliTPCCorrection*)f.Get("map");
  
  //   fTPCCorrection = new AliTPCSpaceCharge3D();
  //   fTPCCorrection->SetSCDataFileName("$ALICE_ROOT/TPC/Calib/maps/SC_NeCO2_eps10_50kHz.root");
  //   fTPCCorrection->SetOmegaTauT1T2(0.325,1,1); // Ne CO2
  // //   fTPCCorrection->SetOmegaTauT1T2(0.41,1,1.05); // Ar CO2
  //   fTPCCorrection->InitSpaceCharge3DDistortion();
  
}

//____________________________________________________________________________________
Double_t AliToyMCReconstruction::GetVDrift() const
{
  //
  //
  //
  return fTPCParam->GetDriftV();
}

//____________________________________________________________________________________
Double_t AliToyMCReconstruction::GetZLength(Int_t roc) const
{
  //
  //
  //
  if (roc<0 || roc>71) return -1;
  return fTPCParam->GetZLength(roc);
}

//____________________________________________________________________________________
TTree* AliToyMCReconstruction::ConnectTrees (const char* files) {
  TString s=gSystem->GetFromPipe(Form("ls %s",files));

  TTree *tFirst=0x0;
  TObjArray *arrFiles=s.Tokenize("\n");
  
  for (Int_t ifile=0; ifile<arrFiles->GetEntriesFast(); ++ifile){
    TString name(arrFiles->At(ifile)->GetName());
    
    TPRegexp reg(".*([0-9]_[0-9]_[0-9]_[0-9]{3}_[0-9]{2}).debug.root");
    TObjArray *arrMatch=0x0;
    arrMatch=reg.MatchS(name);
    TString matchName;
    if (arrMatch && arrMatch->At(1)) matchName=arrMatch->At(1)->GetName();
    else matchName=Form("%02d",ifile);
    delete arrMatch;
    
    if (!tFirst) {
      TFile *f=TFile::Open(name.Data());
      if (!f) continue;
      TTree *t=(TTree*)f->Get("Tracks");
      if (!t) {
        delete f;
        continue;
      }
      
      t->SetName(matchName.Data());
      tFirst=t;
    } else {
      tFirst->AddFriend(Form("t%s=Tracks",matchName.Data()), name.Data());
//       tFirst->AddFriend(Form("t%d=Tracks",ifile), name.Data());
    }
  }

  tFirst->GetListOfFriends()->Print();
  return tFirst;
}

//____________________________________________________________________________________
Int_t AliToyMCReconstruction::LoadOuterSectors() {
  //-----------------------------------------------------------------
  // This function fills outer TPC sectors with clusters.
  // Copy and paste from AliTPCtracker
  //-----------------------------------------------------------------
  Int_t nrows = fOuterSectorArray->GetNRows();
  UInt_t index=0;
  for (Int_t sec = 0;sec<fkNSectorOuter;sec++)
    for (Int_t row = 0;row<nrows;row++){
      AliTPCtrackerRow*  tpcrow = &(fOuterSectorArray[sec%fkNSectorOuter][row]);  
      Int_t sec2 = sec+2*fkNSectorInner;
      //left
      Int_t ncl = tpcrow->GetN1();
      while (ncl--) {
	AliTPCclusterMI *c= (tpcrow->GetCluster1(ncl));
	index=(((sec2<<8)+row)<<16)+ncl;
	tpcrow->InsertCluster(c,index);
      }
      //right
      ncl = tpcrow->GetN2();
      while (ncl--) {
	AliTPCclusterMI *c= (tpcrow->GetCluster2(ncl));
	index=((((sec2+fkNSectorOuter)<<8)+row)<<16)+ncl;
	tpcrow->InsertCluster(c,index);
      }
      //
      // write indexes for fast acces
      //
      for (Int_t i=0;i<510;i++)
	tpcrow->SetFastCluster(i,-1);
      for (Int_t i=0;i<tpcrow->GetN();i++){
        Int_t zi = Int_t((*tpcrow)[i]->GetZ()+255.);
	tpcrow->SetFastCluster(zi,i);  // write index
      }
      Int_t last = 0;
      for (Int_t i=0;i<510;i++){
	if (tpcrow->GetFastCluster(i)<0)
	  tpcrow->SetFastCluster(i,last);
	else
	  last = tpcrow->GetFastCluster(i);
      }
    }  
  return 0;
}


//____________________________________________________________________________________
Int_t  AliToyMCReconstruction::LoadInnerSectors() {
  //-----------------------------------------------------------------
  // This function fills inner TPC sectors with clusters.
  // Copy and paste from AliTPCtracker
  //-----------------------------------------------------------------
  Int_t nrows = fInnerSectorArray->GetNRows();
  UInt_t index=0;
  for (Int_t sec = 0;sec<fkNSectorInner;sec++)
    for (Int_t row = 0;row<nrows;row++){
      AliTPCtrackerRow*  tpcrow = &(fInnerSectorArray[sec%fkNSectorInner][row]);
      //
      //left
      Int_t ncl = tpcrow->GetN1();
      while (ncl--) {
	AliTPCclusterMI *c= (tpcrow->GetCluster1(ncl));
	index=(((sec<<8)+row)<<16)+ncl;
	tpcrow->InsertCluster(c,index);
      }
      //right
      ncl = tpcrow->GetN2();
      while (ncl--) {
	AliTPCclusterMI *c= (tpcrow->GetCluster2(ncl));
	index=((((sec+fkNSectorInner)<<8)+row)<<16)+ncl;
	tpcrow->InsertCluster(c,index);
      }
      //
      // write indexes for fast acces
      //
      for (Int_t i=0;i<510;i++)
	tpcrow->SetFastCluster(i,-1);
      for (Int_t i=0;i<tpcrow->GetN();i++){
        Int_t zi = Int_t((*tpcrow)[i]->GetZ()+255.);
	tpcrow->SetFastCluster(zi,i);  // write index
      }
      Int_t last = 0;
      for (Int_t i=0;i<510;i++){
	if (tpcrow->GetFastCluster(i)<0)
	  tpcrow->SetFastCluster(i,last);
	else
	  last = tpcrow->GetFastCluster(i);
      }

    }  
  return 0;
}

//____________________________________________________________________________________
Int_t  AliToyMCReconstruction::GetSector(AliExternalTrackParam *track) {
  //-----------------------------------------------------------------
  // This function returns the sector number for a given track
  //-----------------------------------------------------------------

  Int_t sector = -1;

  // get the sector number
  // rotate point to global system (track is already global!)
  Double_t xd[3];
  track->GetXYZ(xd);
  //track->Local2GlobalPosition(xd,track->GetAlpha());
  
  // use TPCParams to get the sector number
  Float_t xyz[3] = {xd[0],xd[1],xd[2]};
  Int_t   i[3]   = {0,0,0};
  if(fTPCParam){
    sector  = fTPCParam->Transform0to1(xyz,i);
  }
  
  return sector;
}

//____________________________________________________________________________________
void  AliToyMCReconstruction::FillSectorStructure(Int_t maxev) {
  //-----------------------------------------------------------------
  // This function fills the sector structure of AliToyMCReconstruction
  //-----------------------------------------------------------------

  // cluster array of all sectors
  fInnerSectorArray = new AliTPCtrackerSector[fkNSectorInner];  
  fOuterSectorArray = new AliTPCtrackerSector[fkNSectorOuter]; 
  
  for (Int_t i=0; i<fkNSectorInner; ++i) fInnerSectorArray[i].Setup(fTPCParam,0);
  for (Int_t i=0; i<fkNSectorOuter; ++i) fOuterSectorArray[i].Setup(fTPCParam,1);
  
  Int_t count[72][96] = { {0} , {0} }; 
  
  for (Int_t iev=0; iev<maxev; ++iev){
    printf("==============  Fill Clusters: Processing Event %6d  =================\n",iev);
    fTree->GetEvent(iev);
    for (Int_t itr=0; itr<fEvent->GetNumberOfTracks(); ++itr){
      printf(" > ======  Fill Clusters: Processing Track %6d ========  \n",itr);
      const AliToyMCTrack *tr=fEvent->GetTrack(itr);

      // number of clusters to loop over
      const Int_t ncls=(fClusterType==0)?tr->GetNumberOfSpacePoints():tr->GetNumberOfDistSpacePoints();

      for(Int_t icl=0; icl<ncls; ++icl){

	AliTPCclusterMI *cl=const_cast<AliTPCclusterMI *>(tr->GetSpacePoint(icl));
	if (fClusterType==1) cl=const_cast<AliTPCclusterMI *>(tr->GetDistortedSpacePoint(icl));
	if (!cl) continue;

	Int_t sec = cl->GetDetector();
	Int_t row = cl->GetRow();

        // set Q of the cluster to 1, Q==0 does not work for the seeding
        cl->SetQ(1);
        
	// set cluster time to cluster Z (if not ideal tracking)
	if ( !fIdealTracking ) {
          // a 'valid' position in z is needed for the seeding procedure
//           cl->SetZ(cl->GetTimeBin()*GetVDrift());
          cl->SetZ(cl->GetTimeBin());
        }
	//Printf("Fill clusters (sector %d row %d): %.2f %.2f %.2f %.2f",sec,row,cl->GetX(),cl->GetY(),cl->GetZ(),cl->GetTimeBin());

	// fill arrays for inner and outer sectors (A/C side handled internally)
	if (sec<fkNSectorInner*2){
	  fInnerSectorArray[sec%fkNSectorInner].InsertCluster(cl, count[sec][row], fTPCParam);    
	}
	else{
	  fOuterSectorArray[(sec-fkNSectorInner*2)%fkNSectorOuter].InsertCluster(cl, count[sec][row], fTPCParam);
	}

	++count[sec][row];
      }
    }
  }

  // fill the arrays completely
  // LoadOuterSectors();
  // LoadInnerSectors();

  // // check the arrays
  // for (Int_t i=0; i<fkNSectorInner; ++i){
  //   for (Int_t j=0; j<fInnerSectorArray[i].GetNRows(); ++j){
  //     if(fInnerSectorArray[i][j].GetN()>0){
  // 	Printf("Inner: Sector %d Row %d : %d",i,j,fInnerSectorArray[i][j].GetN());
  //     }
  //   }
  // }
  // for (Int_t i=0; i<fkNSectorInner; ++i){
  //   for (Int_t j=0; j<fOuterSectorArray[i].GetNRows(); ++j){
  //     if(fOuterSectorArray[i][j].GetN()>0){
  // 	Printf("Outer: Sector %d Row %d : %d",i,j,fOuterSectorArray[i][j].GetN());
  //     }
  //   }
  // }
}

//____________________________________________________________________________________
void  AliToyMCReconstruction::FillSectorStructureAC() {
  //-----------------------------------------------------------------
  // This function fills the sector structure of AliToyMCReconstruction
  //-----------------------------------------------------------------

  /*
   my god is the AliTPCtrackerSector stuff complicated!!!
   Ok, so here we will not fill the fClusters1 and fClusters2 of AliTPCtrackerRow,
   using InsertCluster of AliTPCtrackerSector, but only the fClusters via InsertCluster
   of AliTPCtrackerRow itself which then will not be owner, but we create an array in
   here (fAllClusters) which owns all clusters ...
  */
  
  fIsAC=kTRUE;
  // cluster array of all sectors
  fInnerSectorArray = new AliTPCtrackerSector[2*fkNSectorInner];
  fOuterSectorArray = new AliTPCtrackerSector[2*fkNSectorOuter];
  
  for (Int_t i=0; i<2*fkNSectorInner; ++i) {
    fInnerSectorArray[i].Setup(fTPCParam,0);
  }
  
  for (Int_t i=0; i<2*fkNSectorOuter; ++i) {
    fOuterSectorArray[i].Setup(fTPCParam,1);
  }
  
  Int_t count[72][96] = { {0} , {0} };
  
  for (Int_t iev=0; iev<fNmaxEvents; ++iev){
    printf("==============  Fill Clusters: Processing Event %6d  =================\n",iev);
    fTree->GetEvent(iev);
    for (Int_t itr=0; itr<fEvent->GetNumberOfTracks(); ++itr){
//       printf(" > ======  Fill Clusters: Processing Track %6d ========  \n",itr);
      const AliToyMCTrack *tr=fEvent->GetTrack(itr);
      
      // number of clusters to loop over
      const Int_t ncls=(fClusterType==0)?tr->GetNumberOfSpacePoints():tr->GetNumberOfDistSpacePoints();

      // check if expansion of the cluster arrays is needed.
      if (fAllClusters.GetEntriesFast()+ncls>=fAllClusters.Capacity()) fAllClusters.Expand(2*fAllClusters.Capacity());
      for(Int_t icl=0; icl<ncls; ++icl){
        
        AliTPCclusterMI *cl=const_cast<AliTPCclusterMI *>(tr->GetSpacePoint(icl));
        if (fClusterType==1) cl=const_cast<AliTPCclusterMI *>(tr->GetDistortedSpacePoint(icl));
        if (!cl) continue;

        // register copy to the cluster array
        cl = new(fAllClusters[fAllClusters.GetEntriesFast()]) AliTPCclusterMI(*cl);
        
        Int_t sec = cl->GetDetector();
        Int_t row = cl->GetRow();
        
        // set Q of the cluster to 1, Q==0 does not work for the seeding
        cl->SetQ(1);
        
        // set cluster time to cluster Z (if not ideal tracking)
        if ( !fIdealTracking ) {
          // a 'valid' position in z is needed for the seeding procedure
          cl->SetZ(cl->GetTimeBin()*GetVDrift());
//           cl->SetZ(cl->GetTimeBin());
        }
        //Printf("Fill clusters (sector %d row %d): %.2f %.2f %.2f %.2f",sec,row,cl->GetX(),cl->GetY(),cl->GetZ(),cl->GetTimeBin());
        
        // fill arrays for inner and outer sectors (A/C side handled internally)
        if (sec<fkNSectorInner*2){
          fInnerSectorArray[sec][row].InsertCluster(cl, count[sec][row]);
        }
        else{
          fOuterSectorArray[(sec-fkNSectorInner*2)][row].InsertCluster(cl, count[sec][row]);
        }
        
        ++count[sec][row];
      }
    }
  }
  
}

//____________________________________________________________________________________
AliToyMCTrack *AliToyMCReconstruction::ConvertTPCSeedToToyMCTrack(const AliTPCseed &seed)
{
  //
  //
  //


  AliToyMCTrack *tToy = new AliToyMCTrack(seed);

  for (Int_t icl=0; icl<159; ++icl){
    const AliTPCclusterMI *cl=seed.GetClusterFast(icl);
    if (!cl) continue;
    if (fClusterType==0){
      tToy->AddSpacePoint(*cl);
    } else {
      tToy->AddDistortedSpacePoint(*cl);
    }
  }

  return tToy;
}

//____________________________________________________________________________________
AliExternalTrackParam* AliToyMCReconstruction::GetRefittedTrack(const AliTPCseed &seed)
{
  //
  //
  //

  AliExternalTrackParam *track=0x0;

  const Float_t vDrift=GetVDrift();
  const Float_t zLength=GetZLength(0);
  const Double_t kMaxSnp = 0.85;
  const Double_t kMass = TDatabasePDG::Instance()->GetParticle("pi+")->Mass();
  
  // for propagation use a copy
  AliExternalTrackParam t0seed(seed);
  AliTrackerBase::PropagateTrackTo(&t0seed,0,kMass,5,kTRUE,kMaxSnp,0,kFALSE,fUseMaterial);
  fTime0 = t0seed.GetZ()-zLength/vDrift;
  fCreateT0seed = kFALSE;
  
  AliToyMCTrack *toyTrack = ConvertTPCSeedToToyMCTrack(seed);
  
  fTime0 = t0seed.GetZ()-zLength/vDrift;
  fCreateT0seed = kFALSE;
  //         printf("seed (%.2g)\n",fTime0);
  AliExternalTrackParam *dummy = GetSeedFromTrack(toyTrack);
  if (dummy) {
    track = GetFittedTrackFromSeed(toyTrack, dummy);
    delete dummy;
    // propagate seed to 0
    AliTrackerBase::PropagateTrackTo(track,0,kMass,5,kTRUE,kMaxSnp,0,kFALSE,fUseMaterial);
    
  }

  return track;
}

//____________________________________________________________________________________
AliTPCclusterMI* AliToyMCReconstruction::FindMiddleCluster(const AliTPCclusterMI *clInner, const AliTPCclusterMI *clOuter,
                                                           Double_t roady, Double_t roadz,
                                                           AliRieman &seedFit)
{
  //
  //
  //

  const Int_t rocInner = clInner->GetDetector();
  const Int_t rocOuter = clOuter->GetDetector();

  if ( (rocInner%18) != (rocOuter%18) ){
    AliError("Currently only same Sector implemented");
    return 0x0;
  }

  const Int_t innerDet=clInner->GetDetector();
  const Int_t outerDet=clOuter->GetDetector();
  const Int_t globalRowInner  = clInner->GetRow() +(innerDet >35)*63;
  const Int_t globalRowOuter  = clOuter->GetRow() +(outerDet >35)*63;

  Int_t iMiddle  = (globalRowInner+globalRowOuter)/2;
  Int_t roc = innerDet;
  if (iMiddle>62){
    iMiddle-=63;
    roc=outerDet;
  }

  const AliTPCtrackerRow& krMiddle = fOuterSectorArray[roc%36][iMiddle]; // middle
  // initial guess use simple linear interpolation
  Double_t y=(clInner->GetY()+clOuter->GetY())/2;
  Double_t z=(clInner->GetZ()+clOuter->GetZ())/2;
  if (seedFit.IsValid()) {
    // update values once the fit was performed
    y=seedFit.GetYat(krMiddle.GetX());
    z=seedFit.GetZat(krMiddle.GetX());
  }

  AliTPCclusterMI *n=krMiddle.FindNearest(y,z,roady,roadz);
//   if (n)
//     printf("      Nearest cluster (%.2f, %.2f, %.2f) = m(%.2f, %.2f, %.2f : %d) i(%.2f, %.2f , %.2f : %d) o(%.2f, %.2f, %.2f : %d)\n",krMiddle.GetX(),y,z,
//          n->GetX(), n->GetY(),n->GetZ(),n->GetLabel(0),
//            clInner->GetX(), clInner->GetY(),clInner->GetZ(),clInner->GetLabel(0),
//            clOuter->GetX(), clOuter->GetY(),clOuter->GetZ(),clOuter->GetLabel(0)
//         );
  return n;
}

//____________________________________________________________________________________
void AliToyMCReconstruction::AddMiddleClusters(AliTPCseed *seed,
                                               const AliTPCclusterMI *clInner, const AliTPCclusterMI *clOuter,
                                               Double_t roady, Double_t roadz,
                                               Int_t &nTotalClusters, AliRieman &seedFit)
{
  //
  // Iteratively add the middle clusters
  //

  // update rieman fit with every second point added
  AliTPCclusterMI *clMiddle=FindMiddleCluster(clInner,clOuter,roady,roadz,seedFit);

  // break iterative process
  if (!clMiddle) return;

  const Int_t globalRowInner  = clInner->GetRow() +(clInner->GetDetector() >35)*63;
  const Int_t globalRowMiddle = clMiddle->GetRow()+(clMiddle->GetDetector()>35)*63;
  const Int_t globalRowOuter  = clOuter->GetRow() +(clOuter->GetDetector() >35)*63;
  
  seed->SetClusterPointer(globalRowMiddle,clMiddle);
  ++nTotalClusters;
//   printf("    > Total clusters: %d\n",nTotalClusters);
  seedFit.AddPoint(clMiddle->GetX(), clMiddle->GetY(), clMiddle->GetZ(),
                   TMath::Sqrt(clMiddle->GetSigmaY2()), TMath::Sqrt(clMiddle->GetSigmaZ2()));

  if (!(seedFit.GetN()%3)) {
//     printf("      call update: %d (%d)\n",seedFit.GetN(),nTotalClusters);
//     printf("      Riemann results: valid=%d, Chi2=%.2f, Chi2Y=%.2f, Chi2Z=%.2f\n",
//            seedFit.IsValid(), seedFit.GetChi2(), seedFit.GetChi2Y(), seedFit.GetChi2Z());
    seedFit.Update();
  }
  if ( seedFit.IsValid() && seedFit.GetChi2()>1000 ) return;
  
  // Add middle clusters towards outer radius
  if (globalRowMiddle+1<globalRowOuter) AddMiddleClusters(seed,clMiddle,clOuter,roady,roadz,nTotalClusters,seedFit);

  // Add middle clusters towards innter radius
  if (globalRowInner+1<globalRowMiddle) AddMiddleClusters(seed,clInner,clMiddle,roady,roadz,nTotalClusters,seedFit);

  return;
}

//____________________________________________________________________________________
void AliToyMCReconstruction::MakeSeeds2(TObjArray * arr, Int_t sec, Int_t iRowInner, Int_t iRowOuter)
{
  //
  // find seeds in a sector, requires iRowInner < iRowOuter
  // iRowXXX runs from 0-159, so over IROC and OROC
  //

  if (!fIsAC) {
    AliError("This function requires the sector arrays filled for AC tracking");
    return;
  }
  
  // swap rows in case they are in the wrong order
  if (iRowInner>iRowOuter) {
    Int_t tmp=iRowInner;
    iRowInner=iRowOuter;
    iRowOuter=tmp;
  }

  if (iRowOuter>158) iRowOuter=158;
  if (iRowInner<0)   iRowInner=0;

  // only for CookLabel
  AliTPCtracker tpcTracker(fTPCParam);
  
  // Define the search roads:
  // timeRoadCombinatorics - the maximum time difference used for the
  //    combinatorics. Since we don't have a 'z-Vertex' estimate this will
  //    reduce the combinatorics significantly when iterating on one TF
  //    use a little more than one full drift length of the TPC to allow for
  //    distortions
  //
  // timeRoad - the time difference allowed when associating the cluster
  //    in the middle of the other two used for the initial search
  //
  // padRoad  - the local y difference allowed when associating the middle cluster
  
//   Double_t timeRoadCombinatorics = 270./vDrift;
//   Double_t timeRoad = 20./vDrift;
  Double_t timeRoadCombinatorics = 270.;
  Double_t timeRoad = 20.;
  Double_t  padRoad = 10.;


  // fOuterSectorArray runs from 0-95, so from iRowXXX which runs from 0-159
  //   the number of rows in the IROC has to be subtracted
  const Int_t innerRows=fInnerSectorArray->GetNRows();
  const AliTPCtrackerRow& krOuter  = fOuterSectorArray[sec][iRowOuter   - innerRows];   // down
  const AliTPCtrackerRow& krInner  = fOuterSectorArray[sec][iRowInner   - innerRows];   // up

  AliTPCseed *seed = new AliTPCseed;

  const Int_t nMaxClusters=iRowOuter-iRowInner+1;
//   Int_t nScannedClusters = 0;
  
  // loop over all points in the firstand last search row
  for (Int_t iOuter=0; iOuter < krOuter; iOuter++) {
    const AliTPCclusterMI *clOuter = krOuter[iOuter];
    if (clOuter->IsUsed()) continue;
    for (Int_t iInner=0; iInner < krInner; iInner++) {
      const AliTPCclusterMI *clInner = krInner[iInner];
      if (clInner->IsUsed()) continue;
// printf("\n\n Check combination %d (%d), %d (%d)\n",iOuter, iInner, clOuter->GetLabel(0), clInner->GetLabel(0));
      // check maximum distance for combinatorics
      if (TMath::Abs(clOuter->GetZ()-clInner->GetZ())>timeRoadCombinatorics) continue;
// printf("  Is inside one drift\n");

      // use rieman fit for seed description
      AliRieman seedFit(159);
      // add first two points
      seedFit.AddPoint(clInner->GetX(), clInner->GetY(), clInner->GetZ(),
                       TMath::Sqrt(clInner->GetSigmaY2()), TMath::Sqrt(clInner->GetSigmaZ2()));
      seedFit.AddPoint(clOuter->GetX(), clOuter->GetY(), clOuter->GetZ(),
                       TMath::Sqrt(clOuter->GetSigmaY2()), TMath::Sqrt(clOuter->GetSigmaZ2()));
      
      // Iteratively add all clusters in the respective middle
      Int_t nFoundClusters=2;
      AddMiddleClusters(seed,clInner,clOuter,padRoad,timeRoad,nFoundClusters,seedFit);
//       printf("  Clusters attached: %d\n",nFoundClusters);
      if (nFoundClusters>2) seedFit.Update();
//       printf("  Riemann results: valid=%d, Chi2=%.2f, Chi2Y=%.2f, Chi2Z=%.2f\n",
//              seedFit.IsValid(), seedFit.GetChi2(), seedFit.GetChi2Y(), seedFit.GetChi2Z());

      // check for minimum number of assigned clusters and a decent chi2
      if ( nFoundClusters<0.5*nMaxClusters || seedFit.GetChi2()>1000 ){
        seed->Reset();
        continue;
      }
//       printf("  >>> Seed accepted\n");
      // add original outer clusters
      const Int_t globalRowInner  = clInner->GetRow() +(clInner->GetDetector() >35)*63;
      const Int_t globalRowOuter  = clOuter->GetRow() +(clOuter->GetDetector() >35)*63;
      
      seed->SetClusterPointer(globalRowInner,const_cast<AliTPCclusterMI*>(clInner));
      seed->SetClusterPointer(globalRowOuter,const_cast<AliTPCclusterMI*>(clOuter));

      // set parameters retrieved from AliRieman
      Double_t params[5]={0};
      Double_t covar[15]={0};
      const Double_t alpha=TMath::DegToRad()*(clInner->GetDetector()%18*20.+10.);
      const Double_t x=clInner->GetX();
      seedFit.GetExternalParameters(x,params,covar);

      seed->Set(x,alpha,params,covar);

      // set label of the seed. At least 60% of the clusters need the correct label
      CookLabel(seed,.6);
//       printf("  - Label: %d\n",seed->GetLabel());
      // mark clusters as being used
      MarkClustersUsed(seed);
      
      arr->Add(seed);
      seed=new AliTPCseed;
    }
  }
  //delete surplus seed
  delete seed;
  seed=0x0;

}
//____________________________________________________________________________________
void AliToyMCReconstruction::MakeSeeds(TObjArray * /*arr*/, Int_t sec, Int_t iRow1, Int_t iRow2)
{
  //
  // Create seeds between i1 and i2 (stored in arr) with TLinearFitter
  //  
  // sec: sector number
  // iRow1:  upper row
  // iRow2:  lower row
  //

  // Create Fitter
  static TLinearFitter fitter(3,"pol2");

  // Get 3 padrows (iRow1,iMiddle=(iRow1+iRow2)/2,iRow2)
  Int_t iMiddle = (iRow1+iRow2)/2;
  const AliTPCtrackerRow& krd = fOuterSectorArray[sec][iRow2-fInnerSectorArray->GetNRows()];   // down
  const AliTPCtrackerRow& krm = fOuterSectorArray[sec][iMiddle-fInnerSectorArray->GetNRows()]; // middle
  const AliTPCtrackerRow& kru = fOuterSectorArray[sec][iRow1-fInnerSectorArray->GetNRows()];   // up

  // Loop over 3 cluster possibilities  
  for (Int_t iu=0; iu < kru; iu++) {
    for (Int_t im=0; im < krm; im++) {
      for (Int_t id=0; id < krd; id++) {

	// clear all points
	fitter.ClearPoints();

	// add all three points to fitter
	Double_t xy[2] = {kru[iu]->GetX(),kru[iu]->GetY()};
	Double_t z     = kru[iu]->GetZ();
	fitter.AddPoint(xy,z);

	xy[0] = krm[im]->GetX();
	xy[1] = krm[im]->GetY();
	z     = krm[im]->GetZ();
	fitter.AddPoint(xy,z);

	xy[0] = krd[id]->GetX();
	xy[1] = krd[id]->GetY();
	z     = krd[id]->GetZ();
	fitter.AddPoint(xy,z);
	
	// Evaluate and get parameters
	fitter.Eval();

	// how to get the other clusters now?
	// ... 
	
      }
    }
  }
}

//____________________________________________________________________________________
void AliToyMCReconstruction::InitStreamer(TString addName, Int_t level)
{
  //
  // initilise the debug streamer and set the logging level
  //   use a default naming convention
  //

  delete fStreamer;
  fStreamer=0x0;

  if (addName.IsNull()) addName=".dummy";
  
  if (!fTree) return;

  TString debugName=fInputFile->GetName();
  debugName.ReplaceAll(".root","");
  debugName.Append(Form(".%1d.%1d_%1d_%1d_%03d_%02d",
                        fUseMaterial,fIdealTracking,fClusterType,
                        Int_t(fCorrectionType),fSeedingRow,fSeedingDist));
  debugName.Append(addName);
  debugName.Append(".root");
  
  gSystem->Exec(Form("test -f %s && rm %s", debugName.Data(), debugName.Data()));
  fStreamer=new TTreeSRedirector(debugName.Data());
  fStreamer->SetUniqueID(level);

  gROOT->cd();
}

//____________________________________________________________________________________
void AliToyMCReconstruction::ConnectInputFile(const char* file, Int_t nmaxEv)
{
  //
  // connect the tree and event pointer from the input file
  //

  delete fInputFile;
  fInputFile=0x0;
  fEvent=0x0;
  fTree=0;
  
  fInputFile=TFile::Open(file);
  if (!fInputFile || !fInputFile->IsOpen() || fInputFile->IsZombie()) {
    delete fInputFile;
    fInputFile=0x0;
    printf("ERROR: couldn't open the file '%s'\n", file);
    return;
  }
  
  fTree=(TTree*)fInputFile->Get("toyMCtree");
  if (!fTree) {
    printf("ERROR: couldn't read the 'toyMCtree' from file '%s'\n", file);
    return;
  }
  
  fEvent=0x0;
  fTree->SetBranchAddress("event",&fEvent);

  gROOT->cd();

  fNmaxEvents=fTree->GetEntries();
  if (nmaxEv>-1) fNmaxEvents=TMath::Min(nmaxEv,fNmaxEvents);
  
  // setup space charge map from Userinfo of the tree
  InitSpaceCharge();

  // setup the track maps
  SetupTrackMaps();
}

//____________________________________________________________________________________
void AliToyMCReconstruction::Cleanup()
{
  //
  // Cleanup input data
  //
  
  if (fStreamer) delete fStreamer;
  fStreamer=0x0;
  
  delete fEvent;
  fEvent = 0x0;
  
//   delete fTree;
  fTree=0x0;
  
  delete fInputFile;
  fInputFile=0x0;
  
}

//____________________________________________________________________________________
void AliToyMCReconstruction::SetupTrackMaps()
{
  //
  //
  //

  fMapTrackEvent.Delete();
  fMapTrackTrackInEvent.Delete();

  if (!fTree) {
    AliError("Tree not connected");
    return;
  }

  Int_t nmaxEv=fTree->GetEntries();
  if (fNmaxEvents>-1) nmaxEv=fNmaxEvents;
  
  for (Int_t iev=0; iev<nmaxEv; ++iev) {
    fTree->GetEvent(iev);
    if (!fEvent) continue;

    const Int_t ntracks=fEvent->GetNumberOfTracks();
    if (fMapTrackEvent.GetSize()+ntracks>=fMapTrackEvent.Capacity()) fMapTrackEvent.Expand(2*fMapTrackEvent.Capacity());
    if (fMapTrackTrackInEvent.GetSize()+ntracks>=fMapTrackTrackInEvent.Capacity()) fMapTrackTrackInEvent.Expand(2*fMapTrackTrackInEvent.Capacity());
    
    for (Int_t itrack=0; itrack<ntracks; ++itrack){
      Int_t label=fEvent->GetTrack(itrack)->GetUniqueID();

      fMapTrackEvent.Add(label,iev);
      fMapTrackTrackInEvent.Add(label,itrack);
    }
  }
  
}

//____________________________________________________________________________________
void AliToyMCReconstruction::CookLabel(AliTPCseed *seed, Double_t fraction, Int_t info[5])
{
  //
  //
  //

  Int_t labels[159]={0};
//   Long64_t posMaxLabel=-1;
  Int_t nMaxLabel=0;  // clusters from maximum label
  Int_t nMaxLabel2=0; // clusters from second maximum
  Int_t nlabels=0;
  Int_t maxLabel=-1;  // label with most clusters
  Int_t maxLabel2=-1; // label with second most clusters
  Int_t nclusters=0;
  TExMap labelMap(159);

  for (Int_t icl=0; icl<159; ++icl) {
    const AliTPCclusterMI *cl=seed->GetClusterPointer(icl);
    if (!cl) continue;
    ++nclusters;
    
    const Int_t clLabel=cl->GetLabel(0);
    // a not assinged value returns 0, so we need to add 1 and subtract it afterwards
    Long64_t labelPos=labelMap.GetValue(clLabel);

    if (!labelPos) {
      labelPos=nlabels+1;
      labelMap.Add(clLabel,labelPos);
      ++nlabels;
    }
    --labelPos;

    const Int_t nCurrentLabel=++labels[labelPos];
    if (nCurrentLabel>nMaxLabel) {
      nMaxLabel2=nMaxLabel;
      nMaxLabel=nCurrentLabel;
//       posMaxLabel=labelPos;
      maxLabel2=maxLabel;
      maxLabel=clLabel;
    }
  }

  if (Double_t(nMaxLabel)/nclusters<fraction) maxLabel=-maxLabel;

  seed->SetLabel(maxLabel);

  if (info) {
    info[0]=nMaxLabel;
    info[1]=nMaxLabel2;
    info[2]=maxLabel2;
    info[3]=nclusters;
    info[4]=nlabels;
  }
}


//____________________________________________________________________________________
void AliToyMCReconstruction::DumpSeedInfo(TObjArray *arr, Int_t iRowInner, Int_t iRowOuter)
{

  // for debugging
  if (!fStreamer || !fTree) return;
  // swap rows in case they are in the wrong order
  if (iRowInner>iRowOuter) {
    Int_t tmp=iRowInner;
    iRowInner=iRowOuter;
    iRowOuter=tmp;
  }

  AliInfo("");
  
  const Double_t kMaxSnp = 0.85;
  const Double_t kMass = TDatabasePDG::Instance()->GetParticle("pi+")->Mass();
  Float_t vDrift=GetVDrift();
  Float_t zLength=GetZLength(0);
  
  //loop over all events and tracks and try to associate the seed to the track
  for (Int_t iseed=0; iseed<arr->GetEntriesFast(); ++iseed){
    AliTPCseed *seed = static_cast<AliTPCseed*>(arr->UncheckedAt(iseed));
    Int_t seedLabel=seed->GetLabel();

    // get original track
    Int_t iev=fMapTrackEvent.GetValue(TMath::Abs(seedLabel));
    Int_t itr=fMapTrackTrackInEvent.GetValue(TMath::Abs(seedLabel));

    fTree->GetEvent(iev);
    const AliToyMCTrack *toyTrack = fEvent->GetTrack(itr);

    AliExternalTrackParam extTrack(*toyTrack);

    //propagate to same reference frame
    AliExternalTrackParam extSeed(*seed);
    AliTrackerBase::PropagateTrackTo(&extSeed,0,kMass,5.,kTRUE,kMaxSnp,0,kFALSE,fUseMaterial);
    extSeed.Rotate(extTrack.GetAlpha());
    AliTrackerBase::PropagateTrackTo(&extSeed,0,kMass,1.,kFALSE,kMaxSnp,0,kFALSE,fUseMaterial);
    //create propagated track
    AliExternalTrackParam *extSeedRefit=GetRefittedTrack(*seed);
    AliTrackerBase::PropagateTrackTo(extSeedRefit,0,kMass,5.,kTRUE,kMaxSnp,0,kFALSE,fUseMaterial);
    extSeedRefit->Rotate(extTrack.GetAlpha());
    AliTrackerBase::PropagateTrackTo(extSeedRefit,0,kMass,1.,kFALSE,kMaxSnp,0,kFALSE,fUseMaterial);
    
    Int_t roc=-1;
    //
    //count findable and found clusters in the seed
    //
    Int_t nClustersSeedMax=iRowOuter-iRowInner+1;
    Int_t nClustersFindable=0;
    Int_t nClustersSeed=0;

    const Int_t ncls=(fClusterType==0)?toyTrack->GetNumberOfSpacePoints():toyTrack->GetNumberOfDistSpacePoints();

    Int_t rowInner=iRowInner-(iRowInner>62)*63;
    Int_t rowOuter=iRowOuter-(iRowOuter>62)*63;
    //findable
    for (Int_t icl=0; icl<ncls; ++icl) {
      const AliTPCclusterMI *cl=(fClusterType==0)?toyTrack->GetSpacePoint(icl):toyTrack->GetDistortedSpacePoint(icl);
      roc=cl->GetDetector();
      Int_t row=cl->GetRow();
      //         printf("row: %d, iRowInner: %d, iRowOuter: %d\n", row, iRowInner, iRowOuter);
      if ( (row<rowInner) || (row>rowOuter) ) continue;
                                                               ++nClustersFindable;
    }

    //found in seed
    for (Int_t icl=0; icl<159; ++icl) {
      const AliTPCclusterMI *cl=seed->GetClusterPointer(icl);
      if (!cl) continue;
                                                               const Int_t row=cl->GetRow();
      if ( (row<rowInner) || (row>rowOuter) ) continue;
                                                               ++nClustersSeed;
    }

    // convert back to time, since we made tracking in 'pseude z coordinates'
    fTime0=extSeed.GetZ()/vDrift;

    Float_t z0=fEvent->GetZ();
    Float_t t0=fEvent->GetT0();

    Int_t ctype(fCorrectionType);

    Int_t info[5]={0};
    CookLabel(seed,.6,info);

    (*fStreamer) << "Seeds" <<
    "iev="             << iev               <<
    "iseed="           << iseed             <<
    "z0="              << z0                <<
    "t0="              << t0                <<
    "fTime0="          << fTime0            <<
    "itr="             << itr               <<
    "clsType="         << fClusterType      <<
    "corrType="        << ctype             <<
    "seedRowInner="    << iRowInner         <<
    "seedRowOuter="    << iRowOuter         <<
    "vDrift="          << vDrift            <<
    "zLength="         << zLength           <<
    "track.="          << &extSeed          <<
    "track2.="         << extSeedRefit      <<
    "tOrig.="          << &extTrack         <<
    "seedLabel="       << seedLabel         <<
    "nclMax="          << nClustersSeedMax  <<
    "nclFindable="     << nClustersFindable <<
    "nclFound="        << nClustersSeed     <<
    "nMaxLabel="       << info[0]           <<
    "nMaxLabel2="      << info[1]           <<
    "maxLabel2="       << info[2]           <<
    "nclusters="       << info[3]           <<
    "nlabels="         << info[4]           <<
    "roc="             << roc               <<
    "\n";

    delete extSeedRefit;
  }
}

//____________________________________________________________________________________
void AliToyMCReconstruction::MarkClustersUsed(AliTPCseed *seed)
{
  //
  //
  //

  for (Int_t icl=0; icl<159; ++icl) {
    AliTPCclusterMI *cl=seed->GetClusterPointer(icl);
    if (cl) cl->Use();
  }
}

//____________________________________________________________________________________
void AliToyMCReconstruction::DumpTracksToTree(const char* file)
{
  //
  //
  //
  ConnectInputFile(file);
  if (!fTree) return;

  delete fStreamer;
  fStreamer=0x0;
  
  TString debugName=fInputFile->GetName();
  debugName.ReplaceAll(".root",".AllTracks.root");
  
  gSystem->Exec(Form("test -f %s && rm %s", debugName.Data(), debugName.Data()));
  fStreamer=new TTreeSRedirector(debugName.Data());
  
  for (Int_t iev=0;iev<fNmaxEvents;++iev){
    fTree->GetEvent(iev);
    for (Int_t itr=0; itr<fEvent->GetNumberOfTracks();++itr){
      AliToyMCTrack *toyTrack=const_cast<AliToyMCTrack*>(fEvent->GetTrack(itr));
      Int_t trackID=toyTrack->GetUniqueID();

      (*fStreamer) << "Tracks" <<
      "iev="  << iev <<
      "itr="  << itr <<
      "trackID=" << trackID <<
      "track.="  << toyTrack <<
      "\n";
      
    }
  }

  Cleanup();
}
