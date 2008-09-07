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

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  The TRD track seed                                                    //
//                                                                        //
//  Authors:                                                              //
//    Alex Bercuci <A.Bercuci@gsi.de>                                     //
//    Markus Fasel <M.Fasel@gsi.de>                                       //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include "TMath.h"
#include "TLinearFitter.h"
#include "TClonesArray.h" // tmp
#include <TTreeStream.h>

#include "AliLog.h"
#include "AliMathBase.h"

#include "AliTRDcluster.h"
#include "AliTRDseedV1.h"
#include "AliTRDtrackV1.h"
#include "AliTRDcalibDB.h"
#include "AliTRDchamberTimeBin.h"
#include "AliTRDtrackingChamber.h"
#include "AliTRDtrackerV1.h"
#include "AliTRDReconstructor.h"
#include "AliTRDrecoParam.h"
#include "AliTRDgeometry.h"
#include "Cal/AliTRDCalPID.h"

ClassImp(AliTRDseedV1)

//____________________________________________________________________
AliTRDseedV1::AliTRDseedV1(Int_t plane) 
  :AliTRDseed()
  ,fReconstructor(0x0)
  ,fPlane(plane)
  ,fMom(0.)
  ,fSnp(0.)
  ,fTgl(0.)
  ,fdX(0.)
{
  //
  // Constructor
  //
  //printf("AliTRDseedV1::AliTRDseedV1()\n");

  for(int islice=0; islice < knSlices; islice++) fdEdx[islice] = 0.;
  for(int ispec=0; ispec<AliPID::kSPECIES; ispec++) fProb[ispec]  = -1.;
}

//____________________________________________________________________
AliTRDseedV1::AliTRDseedV1(const AliTRDseedV1 &ref)
  :AliTRDseed((AliTRDseed&)ref)
  ,fReconstructor(ref.fReconstructor)
  ,fPlane(ref.fPlane)
  ,fMom(ref.fMom)
  ,fSnp(ref.fSnp)
  ,fTgl(ref.fTgl)
  ,fdX(ref.fdX)
{
  //
  // Copy Constructor performing a deep copy
  //

  //printf("AliTRDseedV1::AliTRDseedV1(const AliTRDseedV1 &)\n");
  SetBit(kOwner, kFALSE);
  for(int islice=0; islice < knSlices; islice++) fdEdx[islice] = ref.fdEdx[islice];
  for(int ispec=0; ispec<AliPID::kSPECIES; ispec++) fProb[ispec] = ref.fProb[ispec];
}


//____________________________________________________________________
AliTRDseedV1& AliTRDseedV1::operator=(const AliTRDseedV1 &ref)
{
  //
  // Assignment Operator using the copy function
  //

  if(this != &ref){
    ref.Copy(*this);
  }
  return *this;

}

//____________________________________________________________________
AliTRDseedV1::~AliTRDseedV1()
{
  //
  // Destructor. The RecoParam object belongs to the underlying tracker.
  //

  //printf("I-AliTRDseedV1::~AliTRDseedV1() : Owner[%s]\n", IsOwner()?"YES":"NO");

  if(IsOwner()) 
    for(int itb=0; itb<knTimebins; itb++){
      if(!fClusters[itb]) continue; 
      //AliInfo(Form("deleting c %p @ %d", fClusters[itb], itb));
      delete fClusters[itb];
      fClusters[itb] = 0x0;
    }
}

//____________________________________________________________________
void AliTRDseedV1::Copy(TObject &ref) const
{
  //
  // Copy function
  //

  //AliInfo("");
  AliTRDseedV1 &target = (AliTRDseedV1 &)ref; 

  target.fPlane         = fPlane;
  target.fMom           = fMom;
  target.fSnp           = fSnp;
  target.fTgl           = fTgl;
  target.fdX            = fdX;
  target.fReconstructor = fReconstructor;
  
  for(int islice=0; islice < knSlices; islice++) target.fdEdx[islice] = fdEdx[islice];
  for(int ispec=0; ispec<AliPID::kSPECIES; ispec++) target.fProb[ispec] = fProb[ispec];
  
  AliTRDseed::Copy(target);
}


//____________________________________________________________
Bool_t AliTRDseedV1::Init(AliTRDtrackV1 *track)
{
// Initialize this tracklet using the track information
//
// Parameters:
//   track - the TRD track used to initialize the tracklet
// 
// Detailed description
// The function sets the starting point and direction of the
// tracklet according to the information from the TRD track.
// 
// Caution
// The TRD track has to be propagated to the beginning of the
// chamber where the tracklet will be constructed
//

  Double_t y, z; 
  if(!track->GetProlongation(fX0, y, z)) return kFALSE;
  fYref[0] = y;
  fYref[1] = track->GetSnp()/(1. - track->GetSnp()*track->GetSnp());
  fZref[0] = z;
  fZref[1] = track->GetTgl();

  //printf("Tracklet ref x[%7.3f] y[%7.3f] z[%7.3f], snp[%f] tgl[%f]\n", fX0, fYref[0], fZref[0], track->GetSnp(), track->GetTgl());
  return kTRUE;
}


//____________________________________________________________________
void AliTRDseedV1::CookdEdx(Int_t nslices)
{
// Calculates average dE/dx for all slices and store them in the internal array fdEdx. 
//
// Parameters:
//  nslices : number of slices for which dE/dx should be calculated
// Output:
//  store results in the internal array fdEdx. This can be accessed with the method
//  AliTRDseedV1::GetdEdx()
//
// Detailed description
// Calculates average dE/dx for all slices. Depending on the PID methode 
// the number of slices can be 3 (LQ) or 8(NN). 
// The calculation of dQ/dl are done using the tracklet fit results (see AliTRDseedV1::GetdQdl(Int_t)) i.e.
//
// dQ/dl = qc/(dx * sqrt(1 + dy/dx^2 + dz/dx^2))
//
// The following effects are included in the calculation:
// 1. calibration values for t0 and vdrift (using x coordinate to calculate slice)
// 2. cluster sharing (optional see AliTRDrecoParam::SetClusterSharing())
// 3. cluster size
//

  Int_t nclusters[knSlices];
  for(int i=0; i<knSlices; i++){ 
    fdEdx[i]     = 0.;
    nclusters[i] = 0;
  }
  Float_t clength = (/*.5 * */AliTRDgeometry::AmThick() + AliTRDgeometry::DrThick());

  AliTRDcluster *cluster = 0x0;
  for(int ic=0; ic<AliTRDtrackerV1::GetNTimeBins(); ic++){
    if(!(cluster = fClusters[ic])) continue;
    Float_t x = cluster->GetX();
    
    // Filter clusters for dE/dx calculation
    
    // 1.consider calibration effects for slice determination
    Int_t slice; 
    if(cluster->IsInChamber()) slice = Int_t(TMath::Abs(fX0 - x) * nslices / clength);
    else slice = x < fX0 ? 0 : nslices-1;
    
    // 2. take sharing into account
    Float_t w = cluster->IsShared() ? .5 : 1.;
    
    // 3. take into account large clusters TODO
    //w *= c->GetNPads() > 3 ? .8 : 1.;
    
    //CHECK !!!
    fdEdx[slice]   += w * GetdQdl(ic); //fdQdl[ic];
    nclusters[slice]++;
  } // End of loop over clusters

  //if(fReconstructor->GetPIDMethod() == AliTRDReconstructor::kLQPID){
  if(nslices == AliTRDReconstructor::kLQslices){
  // calculate mean charge per slice (only LQ PID)
    for(int is=0; is<nslices; is++){ 
      if(nclusters[is]) fdEdx[is] /= nclusters[is];
    }
  }
}


//____________________________________________________________________
Float_t AliTRDseedV1::GetdQdl(Int_t ic) const
{
  return fClusters[ic] ? TMath::Abs(fClusters[ic]->GetQ()) /fdX / TMath::Sqrt(1. + fYfit[1]*fYfit[1] + fZref[1]*fZref[1]) : 0.;
}

//____________________________________________________________________
Double_t* AliTRDseedV1::GetProbability()
{	
// Fill probability array for tracklet from the DB.
//
// Parameters
//
// Output
//   returns pointer to the probability array and 0x0 if missing DB access 
//
// Detailed description

  
  // retrive calibration db
  AliTRDcalibDB *calibration = AliTRDcalibDB::Instance();
  if (!calibration) {
    AliError("No access to calibration data");
    return 0x0;
  }

  if (!fReconstructor) {
    AliError("Reconstructor not set.");
    return 0x0;
  }

  // Retrieve the CDB container class with the parametric detector response
  const AliTRDCalPID *pd = calibration->GetPIDObject(fReconstructor->GetPIDMethod());
  if (!pd) {
    AliError("No access to AliTRDCalPID object");
    return 0x0;
  }
  //AliInfo(Form("Method[%d] : %s", fReconstructor->GetRecoParam() ->GetPIDMethod(), pd->IsA()->GetName()));

  // calculate tracklet length TO DO
  Float_t length = (AliTRDgeometry::AmThick() + AliTRDgeometry::DrThick());
  /// TMath::Sqrt((1.0 - fSnp[iPlane]*fSnp[iPlane]) / (1.0 + fTgl[iPlane]*fTgl[iPlane]));
  
  //calculate dE/dx
  CookdEdx(fReconstructor->GetNdEdxSlices());
  
  // Sets the a priori probabilities
  for(int ispec=0; ispec<AliPID::kSPECIES; ispec++) {
    fProb[ispec] = pd->GetProbability(ispec, fMom, &fdEdx[0], length, fPlane);	
  }

  return &fProb[0];
}

//____________________________________________________________________
Float_t AliTRDseedV1::GetQuality(Bool_t kZcorr) const
{
  //
  // Returns a quality measurement of the current seed
  //

  Float_t zcorr = kZcorr ? fTilt * (fZProb - fZref[0]) : 0.;
  return 
      .5 * TMath::Abs(18.0 - fN2)
    + 10.* TMath::Abs(fYfit[1] - fYref[1])
    + 5. * TMath::Abs(fYfit[0] - fYref[0] + zcorr)
    + 2. * TMath::Abs(fMeanz - fZref[0]) / fPadLength;
}

//____________________________________________________________________
void AliTRDseedV1::GetCovAt(Double_t /*x*/, Double_t *cov) const
{
// Computes covariance in the y-z plane at radial point x

  Int_t ic = 0; while (!fClusters[ic]) ic++; 
  AliTRDcalibDB *fCalib = AliTRDcalibDB::Instance();
  Double_t exB         = fCalib->GetOmegaTau(fCalib->GetVdriftAverage(fClusters[ic]->GetDetector()), -AliTracker::GetBz()*0.1);

  Double_t sy2    = fSigmaY2*fSigmaY2 + .2*(fYfit[1]-exB)*(fYfit[1]-exB);
  Double_t sz2    = fPadLength/12.;


  //printf("Yfit[1] %f sy20 %f SigmaY2 %f\n", fYfit[1], sy20, fSigmaY2);

  cov[0] = sy2;
  cov[1] = fTilt*(sy2-sz2);
  cov[2] = sz2;

  // insert systematic uncertainties calibration and misalignment
  Double_t sys[15];
  fReconstructor->GetRecoParam()->GetSysCovMatrix(sys);
  cov[0] += (sys[0]*sys[0]);
  cov[2] += (sys[1]*sys[1]);
}


//____________________________________________________________________
void AliTRDseedV1::SetOwner()
{
  //AliInfo(Form("own [%s] fOwner[%s]", own?"YES":"NO", fOwner?"YES":"NO"));
  
  if(TestBit(kOwner)) return;
  for(int ic=0; ic<knTimebins; ic++){
    if(!fClusters[ic]) continue;
    fClusters[ic] = new AliTRDcluster(*fClusters[ic]);
  }
  SetBit(kOwner);
}

//____________________________________________________________________
Bool_t	AliTRDseedV1::AttachClustersIter(AliTRDtrackingChamber *chamber, Float_t quality, Bool_t kZcorr, AliTRDcluster *c)
{
  //
  // Iterative process to register clusters to the seed.
  // In iteration 0 we try only one pad-row and if quality not
  // sufficient we try 2 pad-rows (about 5% of tracks cross 2 pad-rows)
  //
  // debug level 7
  //
  
  if(!fReconstructor->GetRecoParam() ){
    AliError("Seed can not be used without a valid RecoParam.");
    return kFALSE;
  }

  AliTRDchamberTimeBin *layer = 0x0;
  if(fReconstructor->GetStreamLevel(AliTRDReconstructor::kTracker)>=7 && c){
    TClonesArray clusters("AliTRDcluster", 24);
    clusters.SetOwner(kTRUE);
    AliTRDcluster *cc = 0x0;
    Int_t det=-1, ncl, ncls = 0;
    for (Int_t iTime = 0; iTime < AliTRDtrackerV1::GetNTimeBins(); iTime++) {
      if(!(layer = chamber->GetTB(iTime))) continue;
      if(!(ncl = Int_t(*layer))) continue;
      for(int ic=0; ic<ncl; ic++){ 
        cc = (*layer)[ic];
        det = cc->GetDetector();
        new(clusters[ncls++]) AliTRDcluster(*cc);
      }
    }
    AliInfo(Form("N clusters[%d] = %d", fPlane, ncls));
    
    Int_t ref = c ? 1 : 0;
    TTreeSRedirector &cstreamer = *AliTRDtrackerV1::DebugStreamer();
    cstreamer << "AttachClustersIter"
      << "det="        << det 
      << "ref="        << ref 
      << "clusters.="  << &clusters
      << "tracklet.="  << this
      << "cl.="        << c
      << "\n";	
  }

  Float_t  tquality;
  Double_t kroady = fReconstructor->GetRecoParam() ->GetRoad1y();
  Double_t kroadz = fPadLength * .5 + 1.;
  
  // initialize configuration parameters
  Float_t zcorr = kZcorr ? fTilt * (fZProb - fZref[0]) : 0.;
  Int_t   niter = kZcorr ? 1 : 2;
  
  Double_t yexp, zexp;
  Int_t ncl = 0;
  // start seed update
  for (Int_t iter = 0; iter < niter; iter++) {
    ncl = 0;
    for (Int_t iTime = 0; iTime < AliTRDtrackerV1::GetNTimeBins(); iTime++) {
      if(!(layer = chamber->GetTB(iTime))) continue;
      if(!Int_t(*layer)) continue;
      
      // define searching configuration
      Double_t dxlayer = layer->GetX() - fX0;
      if(c){
        zexp = c->GetZ();
        //Try 2 pad-rows in second iteration
        if (iter > 0) {
          zexp = fZref[0] + fZref[1] * dxlayer - zcorr;
          if (zexp > c->GetZ()) zexp = c->GetZ() + fPadLength*0.5;
          if (zexp < c->GetZ()) zexp = c->GetZ() - fPadLength*0.5;
        }
      } else zexp = fZref[0] + (kZcorr ? fZref[1] * dxlayer : 0.);
      yexp  = fYref[0] + fYref[1] * dxlayer - zcorr;
      
      // Get and register cluster
      Int_t    index = layer->SearchNearestCluster(yexp, zexp, kroady, kroadz);
      if (index < 0) continue;
      AliTRDcluster *cl = (*layer)[index];
      
      fIndexes[iTime]  = layer->GetGlobalIndex(index);
      fClusters[iTime] = cl;
      fY[iTime]        = cl->GetY();
      fZ[iTime]        = cl->GetZ();
      ncl++;
    }
    if(fReconstructor->GetStreamLevel(AliTRDReconstructor::kTracker)>=7) AliInfo(Form("iter = %d ncl [%d] = %d", iter, fPlane, ncl));
    
    if(ncl>1){	
      // calculate length of the time bin (calibration aware)
      Int_t irp = 0; Float_t x[2]; Int_t tb[2];
      for (Int_t iTime = 0; iTime < AliTRDtrackerV1::GetNTimeBins(); iTime++) {
        if(!fClusters[iTime]) continue;
        x[irp]  = fClusters[iTime]->GetX();
        tb[irp] = iTime;
        irp++;
        if(irp==2) break;
      } 
      fdX = (x[1] - x[0]) / (tb[0] - tb[1]);
  
      // update X0 from the clusters (calibration/alignment aware)
      for (Int_t iTime = 0; iTime < AliTRDtrackerV1::GetNTimeBins(); iTime++) {
        if(!(layer = chamber->GetTB(iTime))) continue;
        if(!layer->IsT0()) continue;
        if(fClusters[iTime]){ 
          fX0 = fClusters[iTime]->GetX();
          break;
        } else { // we have to infere the position of the anode wire from the other clusters
          for (Int_t jTime = iTime+1; jTime < AliTRDtrackerV1::GetNTimeBins(); jTime++) {
            if(!fClusters[jTime]) continue;
            fX0 = fClusters[jTime]->GetX() + fdX * (jTime - iTime);
          }
          break;
        }
      }	
      
      // update YZ reference point
      // TODO
      
      // update x reference positions (calibration/alignment aware)
      for (Int_t iTime = 0; iTime < AliTRDtrackerV1::GetNTimeBins(); iTime++) {
        if(!fClusters[iTime]) continue;
        fX[iTime] = fClusters[iTime]->GetX() - fX0;
      } 
      
      AliTRDseed::Update();
    }
    if(fReconstructor->GetStreamLevel(AliTRDReconstructor::kTracker)>=7) AliInfo(Form("iter = %d nclFit [%d] = %d", iter, fPlane, fN2));
    
    if(IsOK()){
      tquality = GetQuality(kZcorr);
      if(tquality < quality) break;
      else quality = tquality;
    }
    kroadz *= 2.;
  } // Loop: iter
  if (!IsOK()) return kFALSE;

  if(fReconstructor->GetStreamLevel(AliTRDReconstructor::kTracker)>=1) CookLabels();
  UpdateUsed();
  return kTRUE;	
}

//____________________________________________________________________
Bool_t	AliTRDseedV1::AttachClusters(AliTRDtrackingChamber *chamber
                                      ,Bool_t kZcorr)
{
  //
  // Projective algorithm to attach clusters to seeding tracklets
  //
  // Parameters
  //
  // Output
  //
  // Detailed description
  // 1. Collapse x coordinate for the full detector plane
  // 2. truncated mean on y (r-phi) direction
  // 3. purge clusters
  // 4. truncated mean on z direction
  // 5. purge clusters
  // 6. fit tracklet
  //	

  if(!fReconstructor->GetRecoParam() ){
    AliError("Seed can not be used without a valid RecoParam.");
    return kFALSE;
  }

  const Int_t kClusterCandidates = 2 * knTimebins;
  
  //define roads
  Double_t kroady = fReconstructor->GetRecoParam() ->GetRoad1y();
  Double_t kroadz = fPadLength * 1.5 + 1.;
  // correction to y for the tilting angle
  Float_t zcorr = kZcorr ? fTilt * (fZProb - fZref[0]) : 0.;

  // working variables
  AliTRDcluster *clusters[kClusterCandidates];
  Double_t cond[4], yexp[knTimebins], zexp[knTimebins],
    yres[kClusterCandidates], zres[kClusterCandidates];
  Int_t ncl, *index = 0x0, tboundary[knTimebins];
  
  // Do cluster projection
  AliTRDchamberTimeBin *layer = 0x0;
  Int_t nYclusters = 0; Bool_t kEXIT = kFALSE;
  for (Int_t iTime = 0; iTime < AliTRDtrackerV1::GetNTimeBins(); iTime++) {
    if(!(layer = chamber->GetTB(iTime))) continue;
    if(!Int_t(*layer)) continue;
    
    fX[iTime] = layer->GetX() - fX0;
    zexp[iTime] = fZref[0] + fZref[1] * fX[iTime];
    yexp[iTime] = fYref[0] + fYref[1] * fX[iTime] - zcorr;
    
    // build condition and process clusters
    cond[0] = yexp[iTime] - kroady; cond[1] = yexp[iTime] + kroady;
    cond[2] = zexp[iTime] - kroadz; cond[3] = zexp[iTime] + kroadz;
    layer->GetClusters(cond, index, ncl);
    for(Int_t ic = 0; ic<ncl; ic++){
      AliTRDcluster *c = layer->GetCluster(index[ic]);
      clusters[nYclusters] = c;
      yres[nYclusters++] = c->GetY() - yexp[iTime];
      if(nYclusters >= kClusterCandidates) {
        AliWarning(Form("Cluster candidates reached limit %d. Some may be lost.", kClusterCandidates));
        kEXIT = kTRUE;
        break;
      }
    }
    tboundary[iTime] = nYclusters;
    if(kEXIT) break;
  }
  
  // Evaluate truncated mean on the y direction
  Double_t mean, sigma;
  AliMathBase::EvaluateUni(nYclusters, yres, mean, sigma, Int_t(nYclusters*.8)-2);
  // purge cluster candidates
  Int_t nZclusters = 0;
  for(Int_t ic = 0; ic<nYclusters; ic++){
    if(yres[ic] - mean > 4. * sigma){
      clusters[ic] = 0x0;
      continue;
    }
    zres[nZclusters++] = clusters[ic]->GetZ() - zexp[clusters[ic]->GetLocalTimeBin()];
  }
  
  // Evaluate truncated mean on the z direction
  AliMathBase::EvaluateUni(nZclusters, zres, mean, sigma, Int_t(nZclusters*.8)-2);
  // purge cluster candidates
  for(Int_t ic = 0; ic<nZclusters; ic++){
    if(zres[ic] - mean > 4. * sigma){
      clusters[ic] = 0x0;
      continue;
    }
  }

  
  // Select only one cluster/TimeBin
  Int_t lastCluster = 0;
  fN2 = 0;
  for (Int_t iTime = 0; iTime < AliTRDtrackerV1::GetNTimeBins(); iTime++) {
    ncl = tboundary[iTime] - lastCluster;
    if(!ncl) continue;
    Int_t iptr = lastCluster;
    if(ncl > 1){
      Float_t dold = 9999.;
      for(int ic=lastCluster; ic<tboundary[iTime]; ic++){
        if(!clusters[ic]) continue;
        Float_t y = yexp[iTime] - clusters[ic]->GetY();
        Float_t z = zexp[iTime] - clusters[ic]->GetZ();
        Float_t d = y * y + z * z;
        if(d > dold) continue;
        dold = d;
        iptr = ic;
      }
    }
    fIndexes[iTime]  = chamber->GetTB(iTime)->GetGlobalIndex(iptr);
    fClusters[iTime] = clusters[iptr];
    fY[iTime]        = clusters[iptr]->GetY();
    fZ[iTime]        = clusters[iptr]->GetZ();
    lastCluster      = tboundary[iTime];
    fN2++;
  }
  
  // number of minimum numbers of clusters expected for the tracklet
  Int_t kClmin = Int_t(fReconstructor->GetRecoParam() ->GetFindableClusters()*AliTRDtrackerV1::GetNTimeBins());
  if (fN2 < kClmin){
    AliWarning(Form("Not enough clusters to fit the tracklet %d [%d].", fN2, kClmin));
    fN2 = 0;
    return kFALSE;
  }

  // update used clusters
  fNUsed = 0;
  for (Int_t iTime = 0; iTime < AliTRDtrackerV1::GetNTimeBins(); iTime++) {
    if(!fClusters[iTime]) continue;
    if((fClusters[iTime]->IsUsed())) fNUsed++;
  }

  if (fN2-fNUsed < kClmin){
    AliWarning(Form("Too many clusters already in use %d (from %d).", fNUsed, fN2));
    fN2 = 0;
    return kFALSE;
  }
  
  return kTRUE;
}

//____________________________________________________________________
Bool_t AliTRDseedV1::Fit(Bool_t tilt)
{
  //
  // Linear fit of the tracklet
  //
  // Parameters :
  //
  // Output :
  //  True if successful
  //
  // Detailed description
  // 2. Check if tracklet crosses pad row boundary
  // 1. Calculate residuals in the y (r-phi) direction
  // 3. Do a Least Square Fit to the data
  //

  const Int_t kClmin = 8;
  const Int_t kNtb = AliTRDtrackerV1::GetNTimeBins();
  AliTRDtrackerV1::AliTRDLeastSquare fitterY, fitterZ;

  // convertion factor from square to gauss distribution for sigma
  Double_t convert = 1./TMath::Sqrt(12.);

  // book cluster information
  Double_t xc[knTimebins+1], yc[knTimebins], zc[knTimebins+1], sy[knTimebins], sz[knTimebins+1];
  Int_t zRow[knTimebins];
  AliTRDcluster *c = 0x0;
  Int_t nc = 0;
  for (Int_t ic=0; ic<kNtb; ic++) {
    zRow[ic] = -1;
    xc[ic]  = -1.;
    yc[ic]  = 999.;
    zc[ic]  = 999.;
    sy[ic]  = 0.;
    sz[ic]  = 0.;
    if(!(c = fClusters[ic])) continue;
    if(!c->IsInChamber()) continue;
    Float_t w = 1.;
    if(c->GetNPads()>4) w = .5;
    if(c->GetNPads()>5) w = .2;
    zRow[nc] = c->GetPadRow();
    xc[nc]   = c->GetX() - fX0;
    yc[nc]   = c->GetY();
    zc[nc]   = c->GetZ();
    sy[nc]   = w; // all clusters have the same sigma
    sz[nc]   = fPadLength*convert;
    fitterZ.AddPoint(&xc[nc], zc[nc], sz[nc]);
    nc++;
  }
  // to few clusters
  if (nc < kClmin) return kFALSE; 
  

  Int_t zN[2*35];
  Int_t nz = AliTRDtrackerV1::Freq(nc, zRow, zN, kFALSE);
  // more than one pad row crossing
  if(nz>2) return kFALSE; 
  
  // estimate reference parameter at average x
  Double_t y0 = fYref[0];
  Double_t dydx = fYref[1]; 
  Double_t dzdx = fZref[1];
  zc[nc]  = fZref[0];

  // determine z offset of the fit
  Int_t nchanges = 0, nCross = 0;
  if(nz==2){ // tracklet is crossing pad row
    // Find the break time allowing one chage on pad-rows
    // with maximal number of accepted clusters
    Int_t padRef = zRow[0];
    for (Int_t ic=1; ic<nc; ic++) {
      if(zRow[ic] == padRef) continue;
      
      // debug
      if(zRow[ic-1] == zRow[ic]){
        printf("ERROR in pad row change!!!\n");
      }
    
      // evaluate parameters of the crossing point
      Float_t sx = (xc[ic-1] - xc[ic])*convert;
      xc[nc] = .5 * (xc[ic-1] + xc[ic]);
      zc[nc] = .5 * (zc[ic-1] + zc[ic]);
      sz[nc] = TMath::Max(dzdx * sx, .01);
      dzdx   = zc[ic-1] > zc[ic] ? 1. : -1.;
      padRef = zRow[ic];
      nCross = ic;
      nchanges++;
    }
  }

  // condition on nCross and reset nchanges TODO

  if(nchanges==1){
    if(dzdx * fZref[1] < 0.){
      AliInfo("tracklet direction does not correspond to the track direction. TODO.");
    }
    SetBit(kRowCross, kTRUE); // mark pad row crossing
    fCross[0] = xc[nc]; fCross[2] = zc[nc]; fCross[3] = sz[nc]; 
    fitterZ.AddPoint(&xc[nc], zc[nc], sz[nc]);
    fitterZ.Eval();
    dzdx = fZref[1]; // we don't trust Parameter[1] ??;
    zc[nc] = fitterZ.GetFunctionParameter(0); 
  } else if(nchanges > 1){ // debug
    AliInfo("ERROR in n changes!!!");
    return kFALSE;
  }

  
  // estimate deviation from reference direction
  dzdx *= fTilt;
  for (Int_t ic=0; ic<nc; ic++) {
    yc[ic] -= (y0 + xc[ic]*dydx);
    if(tilt) yc[ic] -= (xc[ic]*dzdx + fTilt * (zc[ic] - zc[nc])); 
    fitterY.AddPoint(&xc[ic], yc[ic], sy[ic]);
  }
  fitterY.Eval();
  fYfit[0] = y0+fitterY.GetFunctionParameter(0);
  fYfit[1] = dydx+fitterY.GetFunctionParameter(1);

  if(nchanges) fCross[1] = fYfit[0] + fCross[0] * fYfit[1];

// 	printf("\nnz = %d\n", nz);
// 	for(int ic=0; ic<35; ic++) printf("%d row[%d]\n", ic, zRow[ic]);	
// 
// 	for(int ic=0; ic<nz; ic++) printf("%d n[%d]\n", ic, zN[ic]);	

  return kTRUE;
}

//___________________________________________________________________
void AliTRDseedV1::Draw(Option_t*)
{
}

//___________________________________________________________________
void AliTRDseedV1::Print(Option_t*) const
{
  //
  // Printing the seedstatus
  //

  printf("Seed status :\n");
  printf("  fTilt      = %f\n", fTilt);
  printf("  fPadLength = %f\n", fPadLength);
  printf("  fX0        = %f\n", fX0);
  for(int ic=0; ic<AliTRDtrackerV1::GetNTimeBins(); ic++) {
          const Char_t *isUsable = fUsable[ic]?"Yes":"No";
    printf("  %d X[%f] Y[%f] Z[%f] Indexes[%d] clusters[%p] usable[%s]\n"
                , ic
                , fX[ic]
                , fY[ic]
                , fZ[ic]
                , fIndexes[ic]
                , ((void*) fClusters[ic])
                , isUsable);
        }

  printf("  fYref[0] =%f fYref[1] =%f\n", fYref[0], fYref[1]);
  printf("  fZref[0] =%f fZref[1] =%f\n", fZref[0], fZref[1]);
  printf("  fYfit[0] =%f fYfit[1] =%f\n", fYfit[0], fYfit[1]);
  printf("  fYfitR[0]=%f fYfitR[1]=%f\n", fYfitR[0], fYfitR[1]);
  printf("  fZfit[0] =%f fZfit[1] =%f\n", fZfit[0], fZfit[1]);
  printf("  fZfitR[0]=%f fZfitR[1]=%f\n", fZfitR[0], fZfitR[1]);
  printf("  fSigmaY =%f\n", fSigmaY);
  printf("  fSigmaY2=%f\n", fSigmaY2);            
  printf("  fMeanz  =%f\n", fMeanz);
  printf("  fZProb  =%f\n", fZProb);
  printf("  fLabels[0]=%d fLabels[1]=%d\n", fLabels[0], fLabels[1]);
  printf("  fN      =%d\n", fN);
  printf("  fN2     =%d (>8 isOK)\n",fN2);
  printf("  fNUsed  =%d\n", fNUsed);
  printf("  fFreq   =%d\n", fFreq);
  printf("  fNChange=%d\n",  fNChange);
  printf("  fMPads  =%f\n", fMPads);
  
  printf("  fC      =%f\n", fC);        
  printf("  fCC     =%f\n",fCC);      
  printf("  fChi2   =%f\n", fChi2);  
  printf("  fChi2Z  =%f\n", fChi2Z);
}

