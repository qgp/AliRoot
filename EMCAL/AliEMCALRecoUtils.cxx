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

/* $Id: AliEMCALRecoUtils.cxx 33808 2009-07-15 09:48:08Z gconesab $ */

///////////////////////////////////////////////////////////////////////////////
//
// Class AliEMCALRecoUtils
// Some utilities to recalculate the cluster position or energy linearity
//
//
// Author:  Gustavo Conesa (LPSC- Grenoble) 
//          Track matching part: Rongrong Ma (Yale)

///////////////////////////////////////////////////////////////////////////////
// --- standard c ---

// standard C++ includes
//#include <Riostream.h>

// ROOT includes
#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoBBox.h>
#include <TH2F.h>
#include <TArrayI.h>
#include <TArrayF.h>
#include <TObjArray.h>

// STEER includes
#include "AliVCluster.h"
#include "AliVCaloCells.h"
#include "AliLog.h"
#include "AliPID.h"
#include "AliESDEvent.h"
#include "AliAODEvent.h"
#include "AliESDtrack.h"
#include "AliAODTrack.h"
#include "AliExternalTrackParam.h"
#include "AliESDfriendTrack.h"
#include "AliTrackerBase.h"

// EMCAL includes
#include "AliEMCALRecoUtils.h"
#include "AliEMCALGeometry.h"
#include "AliTrackerBase.h"
#include "AliEMCALPIDUtils.h"


ClassImp(AliEMCALRecoUtils)
  
//_____________________________________
AliEMCALRecoUtils::AliEMCALRecoUtils():
  fParticleType(0),                       fPosAlgo(0),                            fW0(0), 
  fNonLinearityFunction(0),               fNonLinearThreshold(0),
  fSmearClusterEnergy(kFALSE),            fRandom(),
  fCellsRecalibrated(kFALSE),             fRecalibration(kFALSE),                 fEMCALRecalibrationFactors(),
  fTimeRecalibration(kFALSE),             fEMCALTimeRecalibrationFactors(),       fUseRunCorrectionFactors(kFALSE),       
  fRemoveBadChannels(kFALSE),             fRecalDistToBadChannels(kFALSE),        fEMCALBadChannelMap(),
  fNCellsFromEMCALBorder(0),              fNoEMCALBorderAtEta0(kTRUE),
  fRejectExoticCluster(kFALSE),           fRejectExoticCells(kFALSE), 
  fExoticCellFraction(0),                 fExoticCellDiffTime(0),                 fExoticCellMinAmplitude(0),
  fPIDUtils(),                            fAODFilterMask(0),
  fMatchedTrackIndex(0x0),                fMatchedClusterIndex(0x0), 
  fResidualEta(0x0), fResidualPhi(0x0),   fCutEtaPhiSum(kFALSE),                  fCutEtaPhiSeparate(kFALSE), 
  fCutR(0),                               fCutEta(0),                             fCutPhi(0),
  fClusterWindow(0),                      fMass(0),                           
  fStepSurface(0),                        fStepCluster(0),
  fTrackCutsType(0),                      fCutMinTrackPt(0),                      fCutMinNClusterTPC(0), 
  fCutMinNClusterITS(0),                  fCutMaxChi2PerClusterTPC(0),            fCutMaxChi2PerClusterITS(0),
  fCutRequireTPCRefit(kFALSE),            fCutRequireITSRefit(kFALSE),            fCutAcceptKinkDaughters(kFALSE),
  fCutMaxDCAToVertexXY(0),                fCutMaxDCAToVertexZ(0),                 fCutDCAToVertex2D(kFALSE)
{
//
  // Constructor.
  // Initialize all constant values which have to be used
  // during Reco algorithm execution
  //
  
  // Init parameters
  InitParameters();
  
  //Track matching
  fMatchedTrackIndex     = new TArrayI();
  fMatchedClusterIndex   = new TArrayI();
  fResidualPhi           = new TArrayF();
  fResidualEta           = new TArrayF();
  fPIDUtils              = new AliEMCALPIDUtils();

  InitTrackCuts();
}

//______________________________________________________________________
AliEMCALRecoUtils::AliEMCALRecoUtils(const AliEMCALRecoUtils & reco) 
: TNamed(reco), 
  fParticleType(reco.fParticleType),                         fPosAlgo(reco.fPosAlgo),     fW0(reco.fW0),
  fNonLinearityFunction(reco.fNonLinearityFunction),         fNonLinearThreshold(reco.fNonLinearThreshold),
  fSmearClusterEnergy(reco.fSmearClusterEnergy),             fRandom(),
  fCellsRecalibrated(reco.fCellsRecalibrated),
  fRecalibration(reco.fRecalibration),                       fEMCALRecalibrationFactors(reco.fEMCALRecalibrationFactors),
  fTimeRecalibration(reco.fTimeRecalibration),               fEMCALTimeRecalibrationFactors(reco.fEMCALTimeRecalibrationFactors),
  fUseRunCorrectionFactors(reco.fUseRunCorrectionFactors),   
  fRemoveBadChannels(reco.fRemoveBadChannels),               fRecalDistToBadChannels(reco.fRecalDistToBadChannels),
  fEMCALBadChannelMap(reco.fEMCALBadChannelMap),
  fNCellsFromEMCALBorder(reco.fNCellsFromEMCALBorder),       fNoEMCALBorderAtEta0(reco.fNoEMCALBorderAtEta0),
  fRejectExoticCluster(reco.fRejectExoticCluster),           fRejectExoticCells(reco.fRejectExoticCells), 
  fExoticCellFraction(reco.fExoticCellFraction),             fExoticCellDiffTime(reco.fExoticCellDiffTime),               
  fExoticCellMinAmplitude(reco.fExoticCellMinAmplitude),
  fPIDUtils(reco.fPIDUtils),                                 fAODFilterMask(reco.fAODFilterMask),
  fMatchedTrackIndex(  reco.fMatchedTrackIndex?  new TArrayI(*reco.fMatchedTrackIndex):0x0),
  fMatchedClusterIndex(reco.fMatchedClusterIndex?new TArrayI(*reco.fMatchedClusterIndex):0x0),
  fResidualEta(        reco.fResidualEta?        new TArrayF(*reco.fResidualEta):0x0),
  fResidualPhi(        reco.fResidualPhi?        new TArrayF(*reco.fResidualPhi):0x0),
  fCutEtaPhiSum(reco.fCutEtaPhiSum),                         fCutEtaPhiSeparate(reco.fCutEtaPhiSeparate), 
  fCutR(reco.fCutR),        fCutEta(reco.fCutEta),           fCutPhi(reco.fCutPhi),
  fClusterWindow(reco.fClusterWindow),
  fMass(reco.fMass),        fStepSurface(reco.fStepSurface), fStepCluster(reco.fStepCluster),
  fTrackCutsType(reco.fTrackCutsType),                       fCutMinTrackPt(reco.fCutMinTrackPt), 
  fCutMinNClusterTPC(reco.fCutMinNClusterTPC),               fCutMinNClusterITS(reco.fCutMinNClusterITS), 
  fCutMaxChi2PerClusterTPC(reco.fCutMaxChi2PerClusterTPC),   fCutMaxChi2PerClusterITS(reco.fCutMaxChi2PerClusterITS),
  fCutRequireTPCRefit(reco.fCutRequireTPCRefit),             fCutRequireITSRefit(reco.fCutRequireITSRefit),
  fCutAcceptKinkDaughters(reco.fCutAcceptKinkDaughters),     fCutMaxDCAToVertexXY(reco.fCutMaxDCAToVertexXY),    
  fCutMaxDCAToVertexZ(reco.fCutMaxDCAToVertexZ),             fCutDCAToVertex2D(reco.fCutDCAToVertex2D)
{
  //Copy ctor
  
  for(Int_t i = 0; i < 15 ; i++) { fMisalRotShift[i]      = reco.fMisalRotShift[i]      ; 
                                   fMisalTransShift[i]    = reco.fMisalTransShift[i]    ; } 
  for(Int_t i = 0; i < 7  ; i++) { fNonLinearityParams[i] = reco.fNonLinearityParams[i] ; }
  for(Int_t i = 0; i < 3  ; i++) { fSmearClusterParam[i]  = reco.fSmearClusterParam[i]  ; }

}


//______________________________________________________________________
AliEMCALRecoUtils & AliEMCALRecoUtils::operator = (const AliEMCALRecoUtils & reco) 
{
  //Assignment operator
  
  if(this == &reco)return *this;
  ((TNamed *)this)->operator=(reco);

  for(Int_t i = 0; i < 15 ; i++) { fMisalTransShift[i]    = reco.fMisalTransShift[i]    ; 
                                   fMisalRotShift[i]      = reco.fMisalRotShift[i]      ; }
  for(Int_t i = 0; i < 7  ; i++) { fNonLinearityParams[i] = reco.fNonLinearityParams[i] ; }
  for(Int_t i = 0; i < 3  ; i++) { fSmearClusterParam[i]  = reco.fSmearClusterParam[i]  ; }   
  
  fParticleType              = reco.fParticleType;
  fPosAlgo                   = reco.fPosAlgo; 
  fW0                        = reco.fW0;
  
  fNonLinearityFunction      = reco.fNonLinearityFunction;
  fNonLinearThreshold        = reco.fNonLinearThreshold;
  fSmearClusterEnergy        = reco.fSmearClusterEnergy;

  fCellsRecalibrated         = reco.fCellsRecalibrated;
  fRecalibration             = reco.fRecalibration;
  fEMCALRecalibrationFactors = reco.fEMCALRecalibrationFactors;

  fTimeRecalibration             = reco.fTimeRecalibration;
  fEMCALTimeRecalibrationFactors = reco.fEMCALTimeRecalibrationFactors;

  fUseRunCorrectionFactors   = reco.fUseRunCorrectionFactors;
  
  fRemoveBadChannels         = reco.fRemoveBadChannels;
  fRecalDistToBadChannels    = reco.fRecalDistToBadChannels;
  fEMCALBadChannelMap        = reco.fEMCALBadChannelMap;
  
  fNCellsFromEMCALBorder     = reco.fNCellsFromEMCALBorder;
  fNoEMCALBorderAtEta0       = reco.fNoEMCALBorderAtEta0;
  
  fRejectExoticCluster       = reco.fRejectExoticCluster;           
  fRejectExoticCells         = reco.fRejectExoticCells; 
  fExoticCellFraction        = reco.fExoticCellFraction;
  fExoticCellDiffTime        = reco.fExoticCellDiffTime;              
  fExoticCellMinAmplitude    = reco.fExoticCellMinAmplitude;
  
  fPIDUtils                  = reco.fPIDUtils;

  fAODFilterMask             = reco.fAODFilterMask;
  
  fCutEtaPhiSum              = reco.fCutEtaPhiSum;
  fCutEtaPhiSeparate         = reco.fCutEtaPhiSeparate;
  fCutR                      = reco.fCutR;
  fCutEta                    = reco.fCutEta;
  fCutPhi                    = reco.fCutPhi;
  fClusterWindow             = reco.fClusterWindow;
  fMass                      = reco.fMass;
  fStepSurface               = reco.fStepSurface;
  fStepCluster               = reco.fStepCluster;

  fTrackCutsType             = reco.fTrackCutsType;
  fCutMinTrackPt             = reco.fCutMinTrackPt;
  fCutMinNClusterTPC         = reco.fCutMinNClusterTPC;
  fCutMinNClusterITS         = reco.fCutMinNClusterITS; 
  fCutMaxChi2PerClusterTPC   = reco.fCutMaxChi2PerClusterTPC;
  fCutMaxChi2PerClusterITS   = reco.fCutMaxChi2PerClusterITS;
  fCutRequireTPCRefit        = reco.fCutRequireTPCRefit;
  fCutRequireITSRefit        = reco.fCutRequireITSRefit;
  fCutAcceptKinkDaughters    = reco.fCutAcceptKinkDaughters;
  fCutMaxDCAToVertexXY       = reco.fCutMaxDCAToVertexXY;
  fCutMaxDCAToVertexZ        = reco.fCutMaxDCAToVertexZ;
  fCutDCAToVertex2D          = reco.fCutDCAToVertex2D;
  
  if(reco.fResidualEta)
  {
    // assign or copy construct
    if(fResidualEta)
    { 
      *fResidualEta = *reco.fResidualEta;
    }
    else 
    {
      fResidualEta = new TArrayF(*reco.fResidualEta);
    }
  }
  else
  {
    if(fResidualEta)delete fResidualEta;
    fResidualEta = 0;
  }
  
  if(reco.fResidualPhi)
  {
    // assign or copy construct
    if(fResidualPhi)
    { 
      *fResidualPhi = *reco.fResidualPhi;
    }
    else 
    {
      fResidualPhi = new TArrayF(*reco.fResidualPhi);
    }
  }
  else
  {
    if(fResidualPhi)delete fResidualPhi;
    fResidualPhi = 0;
  }
  
  if(reco.fMatchedTrackIndex)
  {
    // assign or copy construct
    if(fMatchedTrackIndex)
    { 
      *fMatchedTrackIndex = *reco.fMatchedTrackIndex;
    }
    else 
    { 
      fMatchedTrackIndex = new TArrayI(*reco.fMatchedTrackIndex);
    }
  }
  else
  {
    if(fMatchedTrackIndex)delete fMatchedTrackIndex;
    fMatchedTrackIndex = 0;
  }  
  
  if(reco.fMatchedClusterIndex)
  {
    // assign or copy construct
    if(fMatchedClusterIndex)
    { 
      *fMatchedClusterIndex = *reco.fMatchedClusterIndex;
    }
    else 
    {
      fMatchedClusterIndex = new TArrayI(*reco.fMatchedClusterIndex);
    }
  }
  else
  {
    if(fMatchedClusterIndex)delete fMatchedClusterIndex;
    fMatchedClusterIndex = 0;
  }
   
  return *this;
}


//_____________________________________
AliEMCALRecoUtils::~AliEMCALRecoUtils()
{
  //Destructor.
  
  if(fEMCALRecalibrationFactors) 
  { 
    fEMCALRecalibrationFactors->Clear();
    delete fEMCALRecalibrationFactors;
  }  
  
  if(fEMCALTimeRecalibrationFactors) 
  { 
    fEMCALTimeRecalibrationFactors->Clear();
    delete fEMCALTimeRecalibrationFactors;
  }  
  
  if(fEMCALBadChannelMap) 
  { 
    fEMCALBadChannelMap->Clear();
    delete fEMCALBadChannelMap;
  }
 
  delete fMatchedTrackIndex   ; 
  delete fMatchedClusterIndex ; 
  delete fResidualEta         ; 
  delete fResidualPhi         ; 
  delete fPIDUtils            ;

  InitTrackCuts();
}

//_______________________________________________________________________________
Bool_t AliEMCALRecoUtils::AcceptCalibrateCell(const Int_t absID, const Int_t bc,
                                              Float_t  & amp,    Double_t & time, 
                                              AliVCaloCells* cells) 
{
  // Reject cell if criteria not passed and calibrate it
  
  AliEMCALGeometry* geom = AliEMCALGeometry::GetInstance();
  
  if(absID < 0 || absID >= 24*48*geom->GetNumberOfSuperModules()) return kFALSE;
  
  Int_t imod = -1, iphi =-1, ieta=-1,iTower = -1, iIphi = -1, iIeta = -1; 
  
  if(!geom->GetCellIndex(absID,imod,iTower,iIphi,iIeta)) 
  {
    // cell absID does not exist
    amp=0; time = 1.e9;
    return kFALSE; 
  }
  
  geom->GetCellPhiEtaIndexInSModule(imod,iTower,iIphi, iIeta,iphi,ieta);  

  // Do not include bad channels found in analysis,
  if( IsBadChannelsRemovalSwitchedOn() && GetEMCALChannelStatus(imod, ieta, iphi)) 
  {
    return kFALSE;
  }
  
  //Recalibrate energy
  amp  = cells->GetCellAmplitude(absID);
  if(!fCellsRecalibrated && IsRecalibrationOn())
    amp *= GetEMCALChannelRecalibrationFactor(imod,ieta,iphi);
  
  
  // Recalibrate time
  time = cells->GetCellTime(absID);
  
  RecalibrateCellTime(absID,bc,time);
  
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliEMCALRecoUtils::CheckCellFiducialRegion(const AliEMCALGeometry* geom, 
                                                  const AliVCluster* cluster, 
                                                  AliVCaloCells* cells) 
{
  // Given the list of AbsId of the cluster, get the maximum cell and 
  // check if there are fNCellsFromBorder from the calorimeter border
  
  if(!cluster)
  {
    AliInfo("Cluster pointer null!");
    return kFALSE;
  }
  
  //If the distance to the border is 0 or negative just exit accept all clusters
  if(cells->GetType()==AliVCaloCells::kEMCALCell && fNCellsFromEMCALBorder <= 0 ) return kTRUE;
  
  Int_t absIdMax  = -1, iSM =-1, ieta = -1, iphi = -1;
  Bool_t shared = kFALSE;
  GetMaxEnergyCell(geom, cells, cluster, absIdMax,  iSM, ieta, iphi, shared);
  
  AliDebug(2,Form("Cluster Max AbsId %d, Cell Energy %2.2f, Cluster Energy %2.2f, Ncells from border %d, EMCAL eta=0 %d\n", 
                  absIdMax, cells->GetCellAmplitude(absIdMax), cluster->E(), fNCellsFromEMCALBorder, fNoEMCALBorderAtEta0));
  
  if(absIdMax==-1) return kFALSE;
  
  //Check if the cell is close to the borders:
  Bool_t okrow = kFALSE;
  Bool_t okcol = kFALSE;
  
  if(iSM < 0 || iphi < 0 || ieta < 0 ) 
  {
    AliFatal(Form("Negative value for super module: %d, or cell ieta: %d, or cell iphi: %d, check EMCAL geometry name\n",
                  iSM,ieta,iphi));
  }
  
  //Check rows/phi
  if(iSM < 10)
  {
    if(iphi >= fNCellsFromEMCALBorder && iphi < 24-fNCellsFromEMCALBorder) okrow =kTRUE; 
  }
  else if (iSM >=10 && ( ( geom->GetEMCGeometry()->GetGeoName()).Contains("12SMV1"))) 
  {
    if(iphi >= fNCellsFromEMCALBorder && iphi < 8-fNCellsFromEMCALBorder) okrow =kTRUE; //1/3 sm case
  }
  else 
  {
    if(iphi >= fNCellsFromEMCALBorder && iphi < 12-fNCellsFromEMCALBorder) okrow =kTRUE; // half SM case
  }
  
  //Check columns/eta
  if(!fNoEMCALBorderAtEta0)
  {
    if(ieta  > fNCellsFromEMCALBorder && ieta < 48-fNCellsFromEMCALBorder) okcol =kTRUE; 
  }
  else
  {
    if(iSM%2==0)
    {
      if(ieta >= fNCellsFromEMCALBorder)     okcol = kTRUE;  
    }
    else 
    {
      if(ieta <  48-fNCellsFromEMCALBorder)  okcol = kTRUE;  
    }
  }//eta 0 not checked
  
  AliDebug(2,Form("EMCAL Cluster in %d cells fiducial volume: ieta %d, iphi %d, SM %d:  column? %d, row? %d\nq",
                  fNCellsFromEMCALBorder, ieta, iphi, iSM, okcol, okrow));
  
  if (okcol && okrow) 
  {
    //printf("Accept\n");
    return kTRUE;
  }
  else  
  {
    //printf("Reject\n");
    AliDebug(2,Form("Reject cluster in border, max cell : ieta %d, iphi %d, SM %d\n",ieta, iphi, iSM));
    return kFALSE;
  }
  
}  


//_______________________________________________________________________________
Bool_t AliEMCALRecoUtils::ClusterContainsBadChannel(const AliEMCALGeometry* geom, 
                                                    const UShort_t* cellList, 
                                                    const Int_t nCells)
{
  // Check that in the cluster cells, there is no bad channel of those stored 
  // in fEMCALBadChannelMap or fPHOSBadChannelMap
  
  if(!fRemoveBadChannels)  return kFALSE;
  if(!fEMCALBadChannelMap) return kFALSE;
  
  Int_t icol = -1;
  Int_t irow = -1;
  Int_t imod = -1;
  for(Int_t iCell = 0; iCell<nCells; iCell++)
  {
    //Get the column and row
    Int_t iTower = -1, iIphi = -1, iIeta = -1; 
    geom->GetCellIndex(cellList[iCell],imod,iTower,iIphi,iIeta); 
    if(fEMCALBadChannelMap->GetEntries() <= imod) continue;
    geom->GetCellPhiEtaIndexInSModule(imod,iTower,iIphi, iIeta,irow,icol);      
    if(GetEMCALChannelStatus(imod, icol, irow))
    {
      AliDebug(2,Form("Cluster with bad channel: SM %d, col %d, row %d\n",imod, icol, irow));
      return kTRUE;
    }
    
  }// cell cluster loop
  
  return kFALSE;
}


//___________________________________________________________________________
Float_t AliEMCALRecoUtils::GetECross(const Int_t absID, const Double_t tcell,
                                     AliVCaloCells* cells, const Int_t bc)
{
  //Calculate the energy in the cross around the energy given cell
  
  AliEMCALGeometry * geom = AliEMCALGeometry::GetInstance();
  
  Int_t imod = -1, iphi =-1, ieta=-1,iTower = -1, iIphi = -1, iIeta = -1; 
  geom->GetCellIndex(absID,imod,iTower,iIphi,iIeta); 
  geom->GetCellPhiEtaIndexInSModule(imod,iTower,iIphi, iIeta,iphi,ieta);  
  
  //Get close cells index, energy and time, not in corners
  
  Int_t absID1 = -1;
  Int_t absID2 = -1;
  
  if( iphi < AliEMCALGeoParams::fgkEMCALRows-1) absID1 = geom-> GetAbsCellIdFromCellIndexes(imod, iphi+1, ieta);
  if( iphi > 0 )                                absID2 = geom-> GetAbsCellIdFromCellIndexes(imod, iphi-1, ieta);
  
  // In case of cell in eta = 0 border, depending on SM shift the cross cell index
  
  Int_t absID3 = -1;
  Int_t absID4 = -1;
  
  if     ( ieta == AliEMCALGeoParams::fgkEMCALCols-1 && !(imod%2) )
  {
    absID3 = geom-> GetAbsCellIdFromCellIndexes(imod+1, iphi, 0);
    absID4 = geom-> GetAbsCellIdFromCellIndexes(imod,   iphi, ieta-1); 
  }
  else if( ieta == 0 && imod%2 )
  {
    absID3 = geom-> GetAbsCellIdFromCellIndexes(imod,   iphi, ieta+1);
    absID4 = geom-> GetAbsCellIdFromCellIndexes(imod-1, iphi, AliEMCALGeoParams::fgkEMCALCols-1); 
  }
  else
  {
    if( ieta < AliEMCALGeoParams::fgkEMCALCols-1 ) 
      absID3 = geom-> GetAbsCellIdFromCellIndexes(imod, iphi, ieta+1);
    if( ieta > 0 )                                 
      absID4 = geom-> GetAbsCellIdFromCellIndexes(imod, iphi, ieta-1); 
  }
  
  //printf("IMOD %d, AbsId %d, a %d, b %d, c %d e %d \n",imod,absID,absID1,absID2,absID3,absID4);
  
  Float_t  ecell1  = 0, ecell2  = 0, ecell3  = 0, ecell4  = 0;
  Double_t tcell1  = 0, tcell2  = 0, tcell3  = 0, tcell4  = 0;
  Bool_t   accept1 = 0, accept2 = 0, accept3 = 0, accept4 = 0;
  
  accept1 = AcceptCalibrateCell(absID1,bc, ecell1,tcell1,cells); 
  accept2 = AcceptCalibrateCell(absID2,bc, ecell2,tcell2,cells); 
  accept3 = AcceptCalibrateCell(absID3,bc, ecell3,tcell3,cells); 
  accept4 = AcceptCalibrateCell(absID4,bc, ecell4,tcell4,cells); 
  
  /*
   printf("Cell absID %d \n",absID);
   printf("\t  accept1 %d, accept2 %d, accept3 %d, accept4 %d\n",
   accept1,accept2,accept3,accept4);
   printf("\t id %d: id1 %d, id2 %d, id3 %d, id4 %d\n",
   absID,absID1,absID2,absID3,absID4);
   printf("\t e %f: e1 %f, e2 %f, e3 %f, e4 %f\n",
   ecell,ecell1,ecell2,ecell3,ecell4);
   printf("\t t %f: t1 %f, t2 %f, t3 %f, t4 %f;\n dt1 %f, dt2 %f, dt3 %f, dt4 %f\n",
   tcell*1.e9,tcell1*1.e9,tcell2*1.e9,tcell3*1.e9,tcell4*1.e9,
   TMath::Abs(tcell-tcell1)*1.e9, TMath::Abs(tcell-tcell2)*1.e9, TMath::Abs(tcell-tcell3)*1.e9, TMath::Abs(tcell-tcell4)*1.e9);
   */
  
  if(TMath::Abs(tcell-tcell1)*1.e9 > fExoticCellDiffTime) ecell1 = 0 ;
  if(TMath::Abs(tcell-tcell2)*1.e9 > fExoticCellDiffTime) ecell2 = 0 ;
  if(TMath::Abs(tcell-tcell3)*1.e9 > fExoticCellDiffTime) ecell3 = 0 ;
  if(TMath::Abs(tcell-tcell4)*1.e9 > fExoticCellDiffTime) ecell4 = 0 ;
  
  return ecell1+ecell2+ecell3+ecell4;
  
}

//_____________________________________________________________________________________________
Bool_t AliEMCALRecoUtils::IsExoticCell(const Int_t absID, AliVCaloCells* cells, const Int_t bc)
{
  // Look to cell neighbourhood and reject if it seems exotic
  // Do before recalibrating the cells

  if(!fRejectExoticCells) return kFALSE;
  
  Float_t  ecell  = 0;
  Double_t tcell  = 0;
  Bool_t   accept = AcceptCalibrateCell(absID, bc, ecell ,tcell ,cells); 
  
  if(!accept) return kTRUE; // reject this cell
  
  if(ecell < fExoticCellMinAmplitude) return kFALSE; // do not reject low energy cells

  Float_t eCross = GetECross(absID,tcell,cells,bc);
  
  if(1-eCross/ecell > fExoticCellFraction) 
  {
    AliDebug(2,Form("AliEMCALRecoUtils::IsExoticCell() - EXOTIC CELL id %d, eCell %f, eCross %f, 1-eCross/eCell %f\n",
                    absID,ecell,eCross,1-eCross/ecell));
    return kTRUE;
  }

  return kFALSE;
}

//___________________________________________________________________
Bool_t AliEMCALRecoUtils::IsExoticCluster(const AliVCluster *cluster, 
                                          AliVCaloCells *cells, 
                                          const Int_t bc) 
{
  // Check if the cluster highest energy tower is exotic
  
  if(!cluster)
  {
    AliInfo("Cluster pointer null!");
    return kFALSE;
  }
  
  if(!fRejectExoticCluster) return kFALSE;
  
  // Get highest energy tower
  AliEMCALGeometry* geom = AliEMCALGeometry::GetInstance();
  Int_t iSupMod = -1, absId = -1, ieta = -1, iphi = -1;
  Bool_t shared = kFALSE;
  GetMaxEnergyCell(geom, cells, cluster, absId, iSupMod, ieta, iphi, shared);
  
  return IsExoticCell(absId,cells,bc);
  
}

//_______________________________________________________________________
Float_t AliEMCALRecoUtils::SmearClusterEnergy(const AliVCluster* cluster) 
{
  //In case of MC analysis, smear energy to match resolution/calibration in real data
  
  if(!cluster)
  {
    AliInfo("Cluster pointer null!");
    return 0;
  }
  
  Float_t energy    = cluster->E() ;
  Float_t rdmEnergy = energy ;
  if(fSmearClusterEnergy)
  {
    rdmEnergy = fRandom.Gaus(energy,fSmearClusterParam[0] * TMath::Sqrt(energy) +
                                    fSmearClusterParam[1] * energy +
                                    fSmearClusterParam[2] );
    AliDebug(2, Form("Energy: original %f, smeared %f\n", energy, rdmEnergy));
  }
  
  return rdmEnergy;
}

//____________________________________________________________________________
Float_t AliEMCALRecoUtils::CorrectClusterEnergyLinearity(AliVCluster* cluster)
{
  // Correct cluster energy from non linearity functions
  
  if(!cluster)
  {
    AliInfo("Cluster pointer null!");
    return 0;
  }
  
  Float_t energy = cluster->E();

  if(energy < 0.1)
  {
    // Clusters with less than 100 MeV or negative are not possible
    AliInfo(Form("Too Low Cluster energy!, E = %f < 0.1 GeV",energy));
    return 0;
  }
  
  switch (fNonLinearityFunction) 
  {
      
    case kPi0MC:
    {
      //Non-Linearity correction (from MC with function ([0]*exp(-[1]/E))+(([2]/([3]*2.*TMath::Pi())*exp(-(E-[4])^2/(2.*[3]^2)))))
      //fNonLinearityParams[0] = 1.014;
      //fNonLinearityParams[1] =-0.03329;
      //fNonLinearityParams[2] =-0.3853;
      //fNonLinearityParams[3] = 0.5423;
      //fNonLinearityParams[4] =-0.4335;
       energy *= (fNonLinearityParams[0]*exp(-fNonLinearityParams[1]/energy))+
                  ((fNonLinearityParams[2]/(fNonLinearityParams[3]*2.*TMath::Pi())*
                    exp(-(energy-fNonLinearityParams[4])*(energy-fNonLinearityParams[4])/(2.*fNonLinearityParams[3]*fNonLinearityParams[3]))));
      break;
    }
     
    case kPi0MCv2:
    {
      //Non-Linearity correction (from MC with function [0]/((x+[1])^[2]))+1;
      //fNonLinearityParams[0] = 3.11111e-02;
      //fNonLinearityParams[1] =-5.71666e-02; 
      //fNonLinearityParams[2] = 5.67995e-01;      
      
      energy *= fNonLinearityParams[0]/TMath::Power(energy+fNonLinearityParams[1],fNonLinearityParams[2])+1;
      break;
    }
    
    case kPi0MCv3:
    {
      //Same as beam test corrected, change parameters
      //fNonLinearityParams[0] =  9.81039e-01
      //fNonLinearityParams[1] =  1.13508e-01;
      //fNonLinearityParams[2] =  1.00173e+00; 
      //fNonLinearityParams[3] =  9.67998e-02;
      //fNonLinearityParams[4] =  2.19381e+02;
      //fNonLinearityParams[5] =  6.31604e+01;
      //fNonLinearityParams[6] =  1;
      energy *= fNonLinearityParams[6]/(fNonLinearityParams[0]*(1./(1.+fNonLinearityParams[1]*exp(-energy/fNonLinearityParams[2]))*1./(1.+fNonLinearityParams[3]*exp((energy-fNonLinearityParams[4])/fNonLinearityParams[5]))));
      
      break;
    }
      
      
    case kPi0GammaGamma:
    {
      //Non-Linearity correction (from Olga Data with function p0+p1*exp(-p2*E))
      //fNonLinearityParams[0] = 1.04;
      //fNonLinearityParams[1] = -0.1445;
      //fNonLinearityParams[2] = 1.046;
      energy /= (fNonLinearityParams[0]+fNonLinearityParams[1]*exp(-fNonLinearityParams[2]*energy)); //Olga function
      break;
    }
      
    case kPi0GammaConversion:
    {
      //Non-Linearity correction (Nicolas from Dimitri Data with function C*[1-a*exp(-b*E)])
      //fNonLinearityParams[0] = 0.139393/0.1349766;
      //fNonLinearityParams[1] = 0.0566186;
      //fNonLinearityParams[2] = 0.982133;
      energy /= fNonLinearityParams[0]*(1-fNonLinearityParams[1]*exp(-fNonLinearityParams[2]*energy));
      
      break;
    }
      
    case kBeamTest:
    {
      //From beam test, Alexei's results, for different ZS thresholds
      //                        th=30 MeV; th = 45 MeV; th = 75 MeV
      //fNonLinearityParams[0] = 1.007;      1.003;      1.002 
      //fNonLinearityParams[1] = 0.894;      0.719;      0.797 
      //fNonLinearityParams[2] = 0.246;      0.334;      0.358 
      //Rescale the param[0] with 1.03
      energy /= fNonLinearityParams[0]/(1+fNonLinearityParams[1]*exp(-energy/fNonLinearityParams[2]));
      
      break;
    }
      
    case kBeamTestCorrected:
    {
      //From beam test, corrected for material between beam and EMCAL
      //fNonLinearityParams[0] =  0.99078
      //fNonLinearityParams[1] =  0.161499;
      //fNonLinearityParams[2] =  0.655166; 
      //fNonLinearityParams[3] =  0.134101;
      //fNonLinearityParams[4] =  163.282;
      //fNonLinearityParams[5] =  23.6904;
      //fNonLinearityParams[6] =  0.978;
        energy *= fNonLinearityParams[6]/(fNonLinearityParams[0]*(1./(1.+fNonLinearityParams[1]*exp(-energy/fNonLinearityParams[2]))*1./(1.+fNonLinearityParams[3]*exp((energy-fNonLinearityParams[4])/fNonLinearityParams[5]))));

      break;
    }
      
    case kNoCorrection:
      AliDebug(2,"No correction on the energy\n");
      break;
      
  }

  return energy;
}

//__________________________________________________
void AliEMCALRecoUtils::InitNonLinearityParam()
{
  //Initialising Non Linearity Parameters
  
  if(fNonLinearityFunction == kPi0MC) 
  {
    fNonLinearityParams[0] = 1.014;
    fNonLinearityParams[1] = -0.03329;
    fNonLinearityParams[2] = -0.3853;
    fNonLinearityParams[3] = 0.5423;
    fNonLinearityParams[4] = -0.4335;
  }
  
  if(fNonLinearityFunction == kPi0MCv2) 
  {
    fNonLinearityParams[0] = 3.11111e-02;
    fNonLinearityParams[1] =-5.71666e-02; 
    fNonLinearityParams[2] = 5.67995e-01;      
  }
  
  if(fNonLinearityFunction == kPi0MCv3) 
  {
    fNonLinearityParams[0] =  9.81039e-01;
    fNonLinearityParams[1] =  1.13508e-01;
    fNonLinearityParams[2] =  1.00173e+00; 
    fNonLinearityParams[3] =  9.67998e-02;
    fNonLinearityParams[4] =  2.19381e+02;
    fNonLinearityParams[5] =  6.31604e+01;
    fNonLinearityParams[6] =  1;
  }
  
  if(fNonLinearityFunction == kPi0GammaGamma) 
  {
    fNonLinearityParams[0] = 1.04;
    fNonLinearityParams[1] = -0.1445;
    fNonLinearityParams[2] = 1.046;
  }  

  if(fNonLinearityFunction == kPi0GammaConversion) 
  {
    fNonLinearityParams[0] = 0.139393;
    fNonLinearityParams[1] = 0.0566186;
    fNonLinearityParams[2] = 0.982133;
  }  

  if(fNonLinearityFunction == kBeamTest) 
  {
    if(fNonLinearThreshold == 30) 
    {
      fNonLinearityParams[0] = 1.007; 
      fNonLinearityParams[1] = 0.894; 
      fNonLinearityParams[2] = 0.246; 
    }
    if(fNonLinearThreshold == 45) 
    {
      fNonLinearityParams[0] = 1.003; 
      fNonLinearityParams[1] = 0.719; 
      fNonLinearityParams[2] = 0.334; 
    }
    if(fNonLinearThreshold == 75) 
    {
      fNonLinearityParams[0] = 1.002; 
      fNonLinearityParams[1] = 0.797; 
      fNonLinearityParams[2] = 0.358; 
    }
  }

  if(fNonLinearityFunction == kBeamTestCorrected) 
  {
    fNonLinearityParams[0] =  0.99078;
    fNonLinearityParams[1] =  0.161499;
    fNonLinearityParams[2] =  0.655166; 
    fNonLinearityParams[3] =  0.134101;
    fNonLinearityParams[4] =  163.282;
    fNonLinearityParams[5] =  23.6904;
    fNonLinearityParams[6] =  0.978;
  }
}

//_________________________________________________________
Float_t  AliEMCALRecoUtils::GetDepth(const Float_t energy, 
                                     const Int_t iParticle, 
                                     const Int_t iSM) const 
{
  //Calculate shower depth for a given cluster energy and particle type

  // parameters 
  Float_t x0    = 1.31;
  Float_t ecr   = 8;
  Float_t depth = 0;
  Float_t arg   = energy*1000/ ecr; //Multiply energy by 1000 to transform to MeV
  
  switch ( iParticle )
  {
    case kPhoton:
      if (arg < 1) 
	depth = 0;
      else
	depth = x0 * (TMath::Log(arg) + 0.5); 
      break;
      
    case kElectron:
      if (arg < 1) 
	depth = 0;
      else
	depth = x0 * (TMath::Log(arg) - 0.5); 
      break;
      
    case kHadron:
      // hadron 
      // boxes anc. here
      if(gGeoManager)
      {
        gGeoManager->cd("ALIC_1/XEN1_1");
        TGeoNode        *geoXEn1    = gGeoManager->GetCurrentNode();
        TGeoNodeMatrix  *geoSM      = dynamic_cast<TGeoNodeMatrix *>(geoXEn1->GetDaughter(iSM));
        if(geoSM)
        {
          TGeoVolume      *geoSMVol   = geoSM->GetVolume(); 
          TGeoShape       *geoSMShape = geoSMVol->GetShape();
          TGeoBBox        *geoBox     = dynamic_cast<TGeoBBox *>(geoSMShape);
          if(geoBox) depth = 0.5 * geoBox->GetDX()*2 ;
          else AliFatal("Null GEANT box");
        }
        else AliFatal("NULL  GEANT node matrix");
      }
      else
      {//electron
	if (arg < 1) 
	  depth = 0;
	else
	  depth = x0 * (TMath::Log(arg) - 0.5); 
      }
        
      break;
      
    default://photon
      if (arg < 1) 
	depth = 0;
      else
	depth = x0 * (TMath::Log(arg) + 0.5);
  }  
  
  return depth;
}

//____________________________________________________________________
void AliEMCALRecoUtils::GetMaxEnergyCell(const AliEMCALGeometry *geom, 
                                         AliVCaloCells* cells, 
                                         const AliVCluster* clu, 
                                         Int_t  & absId,  
                                         Int_t  & iSupMod, 
                                         Int_t  & ieta, 
                                         Int_t  & iphi, 
                                         Bool_t & shared)
{
  //For a given CaloCluster gets the absId of the cell 
  //with maximum energy deposit.
  
  Double_t eMax        = -1.;
  Double_t eCell       = -1.;
  Float_t  fraction    = 1.;
  Float_t  recalFactor = 1.;
  Int_t    cellAbsId   = -1 ;

  Int_t iTower  = -1;
  Int_t iIphi   = -1;
  Int_t iIeta   = -1;
  Int_t iSupMod0= -1;

  if(!clu)
  {
    AliInfo("Cluster pointer null!");
    absId=-1; iSupMod0=-1, ieta = -1; iphi = -1; shared = -1;
    return;
  }
  
  for (Int_t iDig=0; iDig< clu->GetNCells(); iDig++) 
  {
    cellAbsId = clu->GetCellAbsId(iDig);
    fraction  = clu->GetCellAmplitudeFraction(iDig);
    //printf("a Cell %d, id, %d, amp %f, fraction %f\n",iDig,cellAbsId,cells->GetCellAmplitude(cellAbsId),fraction);
    if(fraction < 1e-4) fraction = 1.; // in case unfolding is off
    geom->GetCellIndex(cellAbsId,iSupMod,iTower,iIphi,iIeta); 
    geom->GetCellPhiEtaIndexInSModule(iSupMod,iTower,iIphi, iIeta,iphi,ieta);
    if     (iDig==0) 
    {
      iSupMod0=iSupMod;
    }
    else if(iSupMod0!=iSupMod) 
    {
      shared = kTRUE;
      //printf("AliEMCALRecoUtils::GetMaxEnergyCell() - SHARED CLUSTER\n");
    }
    if(!fCellsRecalibrated && IsRecalibrationOn()) 
    {
      recalFactor = GetEMCALChannelRecalibrationFactor(iSupMod,ieta,iphi);
    }
    eCell  = cells->GetCellAmplitude(cellAbsId)*fraction*recalFactor;
    //printf("b Cell %d, id, %d, amp %f, fraction %f\n",iDig,cellAbsId,eCell,fraction);
    if(eCell > eMax)  
    { 
      eMax  = eCell; 
      absId = cellAbsId;
      //printf("\t new max: cell %d, e %f, ecell %f\n",maxId, eMax,eCell);
    }
  }// cell loop
  
  //Get from the absid the supermodule, tower and eta/phi numbers
  geom->GetCellIndex(absId,iSupMod,iTower,iIphi,iIeta); 
  //Gives SuperModule and Tower numbers
  geom->GetCellPhiEtaIndexInSModule(iSupMod,iTower,
                                         iIphi, iIeta,iphi,ieta); 
  //printf("Max id %d, iSM %d, col %d, row %d\n",absId,iSupMod,ieta,iphi);
  //printf("Max end---\n");
}

//______________________________________
void AliEMCALRecoUtils::InitParameters()
{
  // Initialize data members with default values
  
  fParticleType = kPhoton;
  fPosAlgo      = kUnchanged;
  fW0           = 4.5;
  
  fNonLinearityFunction = kNoCorrection;
  fNonLinearThreshold   = 30;
  
  fExoticCellFraction     = 0.97;
  fExoticCellDiffTime     = 1e6;
  fExoticCellMinAmplitude = 0.5;
  
  fAODFilterMask = 32;
  
  fCutEtaPhiSum      = kTRUE;
  fCutEtaPhiSeparate = kFALSE;
  
  fCutR   = 0.05; 
  fCutEta = 0.025; 
  fCutPhi = 0.05;
  
  fClusterWindow = 100;
  fMass          = 0.139;
  
  fStepSurface   = 20.;                      
  fStepCluster   = 5.;
  fTrackCutsType = kLooseCut;
  
  fCutMinTrackPt     = 0;
  fCutMinNClusterTPC = -1;
  fCutMinNClusterITS = -1;
  
  fCutMaxChi2PerClusterTPC  = 1e10;
  fCutMaxChi2PerClusterITS  = 1e10;
  
  fCutRequireTPCRefit     = kFALSE;
  fCutRequireITSRefit     = kFALSE;
  fCutAcceptKinkDaughters = kFALSE;
  
  fCutMaxDCAToVertexXY = 1e10;             
  fCutMaxDCAToVertexZ  = 1e10;              
  fCutDCAToVertex2D    = kFALSE;
  
  
  //Misalignment matrices
  for(Int_t i = 0; i < 15 ; i++) 
  {
    fMisalTransShift[i] = 0.; 
    fMisalRotShift[i]   = 0.; 
  }
  
  //Non linearity
  for(Int_t i = 0; i < 7  ; i++) fNonLinearityParams[i] = 0.; 
  
  //For kBeamTestCorrected case, but default is no correction
  fNonLinearityParams[0] =  0.99078;
  fNonLinearityParams[1] =  0.161499;
  fNonLinearityParams[2] =  0.655166; 
  fNonLinearityParams[3] =  0.134101;
  fNonLinearityParams[4] =  163.282;
  fNonLinearityParams[5] =  23.6904;
  fNonLinearityParams[6] =  0.978;
  
  //For kPi0GammaGamma case
  //fNonLinearityParams[0] = 0.1457/0.1349766/1.038;
  //fNonLinearityParams[1] = -0.02024/0.1349766/1.038;
  //fNonLinearityParams[2] = 1.046;
  
  //Cluster energy smearing
  fSmearClusterEnergy   = kFALSE;
  fSmearClusterParam[0] = 0.07; // * sqrt E term
  fSmearClusterParam[1] = 0.00; // * E term
  fSmearClusterParam[2] = 0.00; // constant
}

//_____________________________________________________
void AliEMCALRecoUtils::InitEMCALRecalibrationFactors()
{
  //Init EMCAL recalibration factors
  AliDebug(2,"AliCalorimeterUtils::InitEMCALRecalibrationFactors()");
  //In order to avoid rewriting the same histograms
  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  
  fEMCALRecalibrationFactors = new TObjArray(12);
  for (int i = 0; i < 12; i++) 
    fEMCALRecalibrationFactors->Add(new TH2F(Form("EMCALRecalFactors_SM%d",i),
                                             Form("EMCALRecalFactors_SM%d",i),  48, 0, 48, 24, 0, 24));
  //Init the histograms with 1
  for (Int_t sm = 0; sm < 12; sm++) 
  {
    for (Int_t i = 0; i < 48; i++) 
    {
      for (Int_t j = 0; j < 24; j++) 
      {
        SetEMCALChannelRecalibrationFactor(sm,i,j,1.);
      }
    }
  }
  
  fEMCALRecalibrationFactors->SetOwner(kTRUE);
  fEMCALRecalibrationFactors->Compress();
  
  //In order to avoid rewriting the same histograms
  TH1::AddDirectory(oldStatus);    
}

//_________________________________________________________
void AliEMCALRecoUtils::InitEMCALTimeRecalibrationFactors()
{
  //Init EMCAL recalibration factors
  AliDebug(2,"AliCalorimeterUtils::InitEMCALRecalibrationFactors()");
  //In order to avoid rewriting the same histograms
  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  
  fEMCALTimeRecalibrationFactors = new TObjArray(4);
  for (int i = 0; i < 4; i++) 
    fEMCALTimeRecalibrationFactors->Add(new TH1F(Form("hAllTimeAvBC%d",i),
                                                 Form("hAllTimeAvBC%d",i),  
                                                 48*24*12,0.,48*24*12)          );
  //Init the histograms with 1
  for (Int_t bc = 0; bc < 4; bc++) 
  {
    for (Int_t i = 0; i < 48*24*12; i++) 
      SetEMCALChannelTimeRecalibrationFactor(bc,i,0.);
  }
  
  fEMCALTimeRecalibrationFactors->SetOwner(kTRUE);
  fEMCALTimeRecalibrationFactors->Compress();
  
  //In order to avoid rewriting the same histograms
  TH1::AddDirectory(oldStatus);    
}

//____________________________________________________
void AliEMCALRecoUtils::InitEMCALBadChannelStatusMap()
{
  //Init EMCAL bad channels map
  AliDebug(2,"AliEMCALRecoUtils::InitEMCALBadChannelStatusMap()");
  //In order to avoid rewriting the same histograms
  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  
  fEMCALBadChannelMap = new TObjArray(12);
  //TH2F * hTemp = new  TH2I("EMCALBadChannelMap","EMCAL SuperModule bad channel map", 48, 0, 48, 24, 0, 24);
  for (int i = 0; i < 12; i++) 
  {
    fEMCALBadChannelMap->Add(new TH2I(Form("EMCALBadChannelMap_Mod%d",i),Form("EMCALBadChannelMap_Mod%d",i), 48, 0, 48, 24, 0, 24));
  }
  
  fEMCALBadChannelMap->SetOwner(kTRUE);
  fEMCALBadChannelMap->Compress();
  
  //In order to avoid rewriting the same histograms
  TH1::AddDirectory(oldStatus);    
}

//____________________________________________________________________________
void AliEMCALRecoUtils::RecalibrateClusterEnergy(const AliEMCALGeometry* geom, 
                                                 AliVCluster * cluster, 
                                                 AliVCaloCells * cells, 
                                                 const Int_t bc)
{
  // Recalibrate the cluster energy and Time, considering the recalibration map 
  // and the energy of the cells and time that compose the cluster.
  // bc= bunch crossing number returned by esdevent->GetBunchCrossNumber();
  
  if(!cluster)
  {
    AliInfo("Cluster pointer null!");
    return;
  }  
  
  //Get the cluster number of cells and list of absId, check what kind of cluster do we have.
  UShort_t * index    = cluster->GetCellsAbsId() ;
  Double_t * fraction = cluster->GetCellsAmplitudeFraction() ;
  Int_t ncells = cluster->GetNCells();
  
  //Initialize some used variables
  Float_t energy = 0;
  Int_t   absId  =-1;
  Int_t   icol   =-1, irow =-1, imod=1;
  Float_t factor = 1, frac = 0;
  Int_t   absIdMax = -1;
  Float_t emax     = 0;
  
  //Loop on the cells, get the cell amplitude and recalibration factor, multiply and and to the new energy
  for(Int_t icell = 0; icell < ncells; icell++)
  {
    absId = index[icell];
    frac =  fraction[icell];
    if(frac < 1e-5) frac = 1; //in case of EMCAL, this is set as 0 since unfolding is off
    
    if(!fCellsRecalibrated && IsRecalibrationOn()) 
    {
      // Energy  
      Int_t iTower = -1, iIphi = -1, iIeta = -1; 
      geom->GetCellIndex(absId,imod,iTower,iIphi,iIeta); 
      if(fEMCALRecalibrationFactors->GetEntries() <= imod) continue;
      geom->GetCellPhiEtaIndexInSModule(imod,iTower,iIphi, iIeta,irow,icol);      
      factor = GetEMCALChannelRecalibrationFactor(imod,icol,irow);
      
      AliDebug(2,Form("AliEMCALRecoUtils::RecalibrateClusterEnergy - recalibrate cell: module %d, col %d, row %d, cell fraction %f,recalibration factor %f, cell energy %f\n",
                      imod,icol,irow,frac,factor,cells->GetCellAmplitude(absId)));
      
    } 
    
    energy += cells->GetCellAmplitude(absId)*factor*frac;
    
    if(emax < cells->GetCellAmplitude(absId)*factor*frac)
    {
      emax     = cells->GetCellAmplitude(absId)*factor*frac;
      absIdMax = absId;
    }
  }
  
  AliDebug(2,Form("AliEMCALRecoUtils::RecalibrateClusterEnergy - Energy before %f, after %f \n",cluster->E(),energy));

  cluster->SetE(energy);

  // Recalculate time of cluster
  Double_t timeorg = cluster->GetTOF();

  Double_t time = cells->GetCellTime(absIdMax);
  if(!fCellsRecalibrated && IsTimeRecalibrationOn())
    RecalibrateCellTime(absIdMax,bc,time);

  cluster->SetTOF(time);

  AliDebug(2,Form("AliEMCALRecoUtils::RecalibrateClusterEnergy - Time before %f, after %f \n",timeorg,cluster->GetTOF()));
}

//_____________________________________________________________
void AliEMCALRecoUtils::RecalibrateCells(AliVCaloCells * cells,
                                         const Int_t bc)
{
  // Recalibrate the cells time and energy, considering the recalibration map and the energy 
  // of the cells that compose the cluster.
  // bc= bunch crossing number returned by esdevent->GetBunchCrossNumber();

  if(!IsRecalibrationOn() && !IsTimeRecalibrationOn() && !IsBadChannelsRemovalSwitchedOn()) return;
  
  if(!cells)
  {
    AliInfo("Cells pointer null!");
    return;
  }  
  
  Short_t  absId  =-1;
  Bool_t   accept = kFALSE;
  Float_t  ecell  = 0;
  Double_t tcell  = 0;
  Double_t ecellin = 0;
  Double_t tcellin = 0;
  Int_t  mclabel = -1;
  Double_t efrac = 0;
  
  Int_t nEMcell  = cells->GetNumberOfCells() ;  
  for (Int_t iCell = 0; iCell < nEMcell; iCell++) 
  { 
    cells->GetCell( iCell, absId, ecellin, tcellin, mclabel, efrac );
    
    accept = AcceptCalibrateCell(absId, bc, ecell ,tcell ,cells); 
    if(!accept) 
    {
      ecell = 0;
      tcell = -1;
    }
    
    //Set new values
    cells->SetCell(iCell,absId,ecell, tcell, mclabel, efrac);
  }

  fCellsRecalibrated = kTRUE;
}

//_______________________________________________________________________________________________________
void AliEMCALRecoUtils::RecalibrateCellTime(const Int_t absId, const Int_t bc, Double_t & celltime) const
{
  // Recalibrate time of cell with absID  considering the recalibration map 
  // bc= bunch crossing number returned by esdevent->GetBunchCrossNumber();
  
  if(!fCellsRecalibrated && IsTimeRecalibrationOn() && bc >= 0)
  {
    celltime -= GetEMCALChannelTimeRecalibrationFactor(bc%4,absId)*1.e-9;    ;  
  }
}
  
//______________________________________________________________________________
void AliEMCALRecoUtils::RecalculateClusterPosition(const AliEMCALGeometry *geom, 
                                                   AliVCaloCells* cells, 
                                                   AliVCluster* clu)
{
  //For a given CaloCluster recalculates the position for a given set of misalignment shifts and puts it again in the CaloCluster.
  
  if(!clu)
  {
    AliInfo("Cluster pointer null!");
    return;
  }
    
  if     (fPosAlgo==kPosTowerGlobal) RecalculateClusterPositionFromTowerGlobal( geom, cells, clu);
  else if(fPosAlgo==kPosTowerIndex)  RecalculateClusterPositionFromTowerIndex ( geom, cells, clu);
  else   AliDebug(2,"Algorithm to recalculate position not selected, do nothing.");
}  

//_____________________________________________________________________________________________
void AliEMCALRecoUtils::RecalculateClusterPositionFromTowerGlobal(const AliEMCALGeometry *geom, 
                                                                  AliVCaloCells* cells, 
                                                                  AliVCluster* clu)
{
  // For a given CaloCluster recalculates the position for a given set of misalignment shifts and puts it again in the CaloCluster.
  // The algorithm is a copy of what is done in AliEMCALRecPoint
  
  Double_t eCell       = 0.;
  Float_t  fraction    = 1.;
  Float_t  recalFactor = 1.;
  
  Int_t    absId   = -1;
  Int_t    iTower  = -1, iIphi  = -1, iIeta  = -1;
  Int_t    iSupModMax = -1, iSM=-1, iphi   = -1, ieta   = -1;
  Float_t  weight = 0.,  totalWeight=0.;
  Float_t  newPos[3] = {0,0,0};
  Double_t pLocal[3], pGlobal[3];
  Bool_t shared = kFALSE;

  Float_t  clEnergy = clu->E(); //Energy already recalibrated previously
  if (clEnergy <= 0)
    return;
  GetMaxEnergyCell(geom, cells, clu, absId,  iSupModMax, ieta, iphi,shared);
  Double_t depth = GetDepth(clEnergy,fParticleType,iSupModMax) ;
  
  //printf("** Cluster energy %f, ncells %d, depth %f\n",clEnergy,clu->GetNCells(),depth);
  
  for (Int_t iDig=0; iDig< clu->GetNCells(); iDig++) 
  {
    absId = clu->GetCellAbsId(iDig);
    fraction  = clu->GetCellAmplitudeFraction(iDig);
    if(fraction < 1e-4) fraction = 1.; // in case unfolding is off
    
    if (!fCellsRecalibrated)
    {
      geom->GetCellIndex(absId,iSM,iTower,iIphi,iIeta); 
      geom->GetCellPhiEtaIndexInSModule(iSM,iTower,iIphi, iIeta,iphi,ieta);      
      
      if(IsRecalibrationOn()) 
      {
        recalFactor = GetEMCALChannelRecalibrationFactor(iSM,ieta,iphi);
      }
    }
    
    eCell  = cells->GetCellAmplitude(absId)*fraction*recalFactor;
    
    weight = GetCellWeight(eCell,clEnergy);
    totalWeight += weight;
    
    geom->RelPosCellInSModule(absId,depth,pLocal[0],pLocal[1],pLocal[2]);
    //printf("pLocal (%f,%f,%f), SM %d, absId %d\n",pLocal[0],pLocal[1],pLocal[2],iSupModMax,absId);
    geom->GetGlobal(pLocal,pGlobal,iSupModMax);
    //printf("pLocal (%f,%f,%f)\n",pGlobal[0],pGlobal[1],pGlobal[2]);

    for(int i=0; i<3; i++ ) newPos[i] += (weight*pGlobal[i]);
  }// cell loop
  
  if(totalWeight>0)
  {
    for(int i=0; i<3; i++ )    newPos[i] /= totalWeight;
  }
    
  //Float_t pos[]={0,0,0};
  //clu->GetPosition(pos);
  //printf("OldPos  : %2.3f,%2.3f,%2.3f\n",pos[0],pos[1],pos[2]);
  //printf("NewPos  : %2.3f,%2.3f,%2.3f\n",newPos[0],newPos[1],newPos[2]);
  
  if(iSupModMax > 1) //sector 1
  {
    newPos[0] +=fMisalTransShift[3];//-=3.093; 
    newPos[1] +=fMisalTransShift[4];//+=6.82;
    newPos[2] +=fMisalTransShift[5];//+=1.635;
    //printf("   +    : %2.3f,%2.3f,%2.3f\n",fMisalTransShift[3],fMisalTransShift[4],fMisalTransShift[5]);
  } else //sector 0
  {
    newPos[0] +=fMisalTransShift[0];//+=1.134;
    newPos[1] +=fMisalTransShift[1];//+=8.2;
    newPos[2] +=fMisalTransShift[2];//+=1.197;
    //printf("   +    : %2.3f,%2.3f,%2.3f\n",fMisalTransShift[0],fMisalTransShift[1],fMisalTransShift[2]);
  }
  //printf("NewPos : %2.3f,%2.3f,%2.3f\n",newPos[0],newPos[1],newPos[2]);

  clu->SetPosition(newPos);
}  

//____________________________________________________________________________________________
void AliEMCALRecoUtils::RecalculateClusterPositionFromTowerIndex(const AliEMCALGeometry *geom, 
                                                                 AliVCaloCells* cells, 
                                                                 AliVCluster* clu)
{
  // For a given CaloCluster recalculates the position for a given set of misalignment shifts and puts it again in the CaloCluster.
  // The algorithm works with the tower indeces, averages the indeces and from them it calculates the global position
  
  Double_t eCell       = 1.;
  Float_t  fraction    = 1.;
  Float_t  recalFactor = 1.;
  
  Int_t absId   = -1;
  Int_t iTower  = -1;
  Int_t iIphi   = -1, iIeta   = -1;
  Int_t iSupMod = -1, iSupModMax = -1;
  Int_t iphi = -1, ieta =-1;
  Bool_t shared = kFALSE;

  Float_t clEnergy = clu->E(); //Energy already recalibrated previously.
  if (clEnergy <= 0)
    return;
  GetMaxEnergyCell(geom, cells, clu, absId,  iSupModMax, ieta, iphi,shared);
  Float_t  depth = GetDepth(clEnergy,fParticleType,iSupMod) ;

  Float_t weight = 0., weightedCol = 0., weightedRow = 0., totalWeight=0.;
  Bool_t areInSameSM = kTRUE; //exclude clusters with cells in different SMs for now
  Int_t startingSM = -1;
  
  for (Int_t iDig=0; iDig< clu->GetNCells(); iDig++) 
  {
    absId = clu->GetCellAbsId(iDig);
    fraction  = clu->GetCellAmplitudeFraction(iDig);
    if(fraction < 1e-4) fraction = 1.; // in case unfolding is off

    if     (iDig==0)  startingSM = iSupMod;
    else if(iSupMod != startingSM) areInSameSM = kFALSE;

    eCell  = cells->GetCellAmplitude(absId);
    
    geom->GetCellIndex(absId,iSupMod,iTower,iIphi,iIeta); 
    geom->GetCellPhiEtaIndexInSModule(iSupMod,iTower,iIphi, iIeta,iphi,ieta);    
    
    if (!fCellsRecalibrated)
    {
      if(IsRecalibrationOn()) 
      {
        recalFactor = GetEMCALChannelRecalibrationFactor(iSupMod,ieta,iphi);
      }
    }
    
    eCell  = cells->GetCellAmplitude(absId)*fraction*recalFactor;
    
    weight = GetCellWeight(eCell,clEnergy);
    if(weight < 0) weight = 0;
    totalWeight += weight;
    weightedCol += ieta*weight;
    weightedRow += iphi*weight;
    
    //printf("Max cell? cell %d, amplitude org %f, fraction %f, recalibration %f, amplitude new %f \n",cellAbsId, cells->GetCellAmplitude(cellAbsId), fraction, recalFactor, eCell) ;
  }// cell loop
    
  Float_t xyzNew[]={0.,0.,0.};
  if(areInSameSM == kTRUE) 
  {
    //printf("In Same SM\n");
    weightedCol = weightedCol/totalWeight;
    weightedRow = weightedRow/totalWeight;
    geom->RecalculateTowerPosition(weightedRow, weightedCol, iSupModMax, depth, fMisalTransShift, fMisalRotShift, xyzNew); 
  } 
  else 
  {
    //printf("In Different SM\n");
    geom->RecalculateTowerPosition(iphi,        ieta,        iSupModMax, depth, fMisalTransShift, fMisalRotShift, xyzNew); 
  }
  
  clu->SetPosition(xyzNew);
}

//___________________________________________________________________________________________
void AliEMCALRecoUtils::RecalculateClusterDistanceToBadChannel(const AliEMCALGeometry * geom, 
                                                               AliVCaloCells* cells, 
                                                               AliVCluster * cluster)
{           
  //re-evaluate distance to bad channel with updated bad map
  
  if(!fRecalDistToBadChannels) return;
  
  if(!cluster)
  {
    AliInfo("Cluster pointer null!");
    return;
  }  
  
  //Get channels map of the supermodule where the cluster is.
  Int_t absIdMax  = -1, iSupMod =-1, icolM = -1, irowM = -1;
  Bool_t shared = kFALSE;
  GetMaxEnergyCell(geom, cells, cluster, absIdMax,  iSupMod, icolM, irowM, shared);
  TH2D* hMap  = (TH2D*)fEMCALBadChannelMap->At(iSupMod);

  Int_t dRrow, dRcol;  
  Float_t  minDist = 10000.;
  Float_t  dist    = 0.;
  
  //Loop on tower status map 
  for(Int_t irow = 0; irow < AliEMCALGeoParams::fgkEMCALRows; irow++)
  {
    for(Int_t icol = 0; icol < AliEMCALGeoParams::fgkEMCALCols; icol++)
    {
      //Check if tower is bad.
      if(hMap->GetBinContent(icol,irow)==0) continue;
      //printf("AliEMCALRecoUtils::RecalculateDistanceToBadChannels() - \n \t Bad channel in SM %d, col %d, row %d, \n \t Cluster max in col %d, row %d\n",
      //       iSupMod,icol, irow, icolM,irowM);
      
      dRrow=TMath::Abs(irowM-irow);
      dRcol=TMath::Abs(icolM-icol);
      dist=TMath::Sqrt(dRrow*dRrow+dRcol*dRcol);
      if(dist < minDist)
      {
        //printf("MIN DISTANCE TO BAD %2.2f\n",dist);
        minDist = dist;
      }
    }
  }
  
  //In case the cluster is shared by 2 SuperModules, need to check the map of the second Super Module
  if (shared) 
  {
    TH2D* hMap2 = 0;
    Int_t iSupMod2 = -1;
    
    //The only possible combinations are (0,1), (2,3) ... (8,9)
    if(iSupMod%2) iSupMod2 = iSupMod-1;
    else          iSupMod2 = iSupMod+1;
    hMap2  = (TH2D*)fEMCALBadChannelMap->At(iSupMod2);
    
    //Loop on tower status map of second super module
    for(Int_t irow = 0; irow < AliEMCALGeoParams::fgkEMCALRows; irow++)
    {
      for(Int_t icol = 0; icol < AliEMCALGeoParams::fgkEMCALCols; icol++)
      {
        //Check if tower is bad.
        if(hMap2->GetBinContent(icol,irow)==0) continue;
        //printf("AliEMCALRecoUtils::RecalculateDistanceToBadChannels(shared) - \n \t Bad channel in SM %d, col %d, row %d \n \t Cluster max in SM %d, col %d, row %d\n",
        //     iSupMod2,icol, irow,iSupMod,icolM,irowM);
        dRrow=TMath::Abs(irow-irowM);
        
        if(iSupMod%2) 
        {
          dRcol=TMath::Abs(icol-(AliEMCALGeoParams::fgkEMCALCols+icolM));
        } else 
        {
          dRcol=TMath::Abs(AliEMCALGeoParams::fgkEMCALCols+icol-icolM);
        }                    
        
        dist=TMath::Sqrt(dRrow*dRrow+dRcol*dRcol);
        if(dist < minDist) minDist = dist;        
      }
    }
  }// shared cluster in 2 SuperModules
  
  AliDebug(2,Form("Max cluster cell (SM,col,row)=(%d %d %d) - Distance to Bad Channel %2.2f",iSupMod, icolM, irowM, minDist));
  cluster->SetDistanceToBadChannel(minDist);
}

//__________________________________________________________________
void AliEMCALRecoUtils::RecalculateClusterPID(AliVCluster * cluster)
{           
  //re-evaluate identification parameters with bayesian
  
  if(!cluster)
  {
    AliInfo("Cluster pointer null!");
    return;
  }
  
  if ( cluster->GetM02() != 0)
    fPIDUtils->ComputePID(cluster->E(),cluster->GetM02());
  
  Float_t pidlist[AliPID::kSPECIESCN+1];
  for(Int_t i = 0; i < AliPID::kSPECIESCN+1; i++) pidlist[i] = fPIDUtils->GetPIDFinal(i);
        
  cluster->SetPID(pidlist);
}

//___________________________________________________________________________________________________________________
void AliEMCALRecoUtils::RecalculateClusterShowerShapeParameters(const AliEMCALGeometry * geom, 
                                                                AliVCaloCells* cells, 
                                                                AliVCluster * cluster,
                                                                Float_t & l0,   Float_t & l1,   
                                                                Float_t & disp, Float_t & dEta, Float_t & dPhi,
                                                                Float_t & sEta, Float_t & sPhi, Float_t & sEtaPhi)
{
  // Calculates new center of gravity in the local EMCAL-module coordinates 
  // and tranfers into global ALICE coordinates
  // Calculates Dispersion and main axis
  
  if(!cluster)
  {
    AliInfo("Cluster pointer null!");
    return;
  }
    
  Double_t eCell       = 0.;
  Float_t  fraction    = 1.;
  Float_t  recalFactor = 1.;

  Int_t    iSupMod = -1;
  Int_t    iTower  = -1;
  Int_t    iIphi   = -1;
  Int_t    iIeta   = -1;
  Int_t    iphi    = -1;
  Int_t    ieta    = -1;
  Double_t etai    = -1.;
  Double_t phii    = -1.;
  
  Int_t    nstat   = 0 ;
  Float_t  wtot    = 0.;
  Double_t w       = 0.;
  Double_t etaMean = 0.;
  Double_t phiMean = 0.;
    
  //Loop on cells
  for(Int_t iDigit=0; iDigit < cluster->GetNCells(); iDigit++) 
  {
    //Get from the absid the supermodule, tower and eta/phi numbers
    geom->GetCellIndex(cluster->GetCellAbsId(iDigit),iSupMod,iTower,iIphi,iIeta); 
    geom->GetCellPhiEtaIndexInSModule(iSupMod,iTower,iIphi,iIeta, iphi,ieta);        
    
    //Get the cell energy, if recalibration is on, apply factors
    fraction  = cluster->GetCellAmplitudeFraction(iDigit);
    if(fraction < 1e-4) fraction = 1.; // in case unfolding is off
    
    if (!fCellsRecalibrated)
    {
      if(IsRecalibrationOn()) 
      {
        recalFactor = GetEMCALChannelRecalibrationFactor(iSupMod,ieta,iphi);
      }
    }
    
    eCell  = cells->GetCellAmplitude(cluster->GetCellAbsId(iDigit))*fraction*recalFactor;
    
    if(cluster->E() > 0 && eCell > 0)
    {
      w  = GetCellWeight(eCell,cluster->E());
      
      etai=(Double_t)ieta;
      phii=(Double_t)iphi;  
      
      if(w > 0.0) 
      {
        wtot += w ;
        nstat++;            
        //Shower shape
        sEta     += w * etai * etai ;
        etaMean  += w * etai ;
        sPhi     += w * phii * phii ;
        phiMean  += w * phii ; 
        sEtaPhi  += w * etai * phii ; 
      }
    } 
    else
      AliError(Form("Wrong energy %f and/or amplitude %f\n", eCell, cluster->E()));
  }//cell loop
  
  //Normalize to the weight  
  if (wtot > 0) 
  {
    etaMean /= wtot ;
    phiMean /= wtot ;
  }
  else
    AliError(Form("Wrong weight %f\n", wtot));
  
  //Calculate dispersion  
  for(Int_t iDigit=0; iDigit < cluster->GetNCells(); iDigit++) 
  {
    //Get from the absid the supermodule, tower and eta/phi numbers
    geom->GetCellIndex(cluster->GetCellAbsId(iDigit),iSupMod,iTower,iIphi,iIeta); 
    geom->GetCellPhiEtaIndexInSModule(iSupMod,iTower,iIphi,iIeta, iphi,ieta);
    
    //Get the cell energy, if recalibration is on, apply factors
    fraction  = cluster->GetCellAmplitudeFraction(iDigit);
    if(fraction < 1e-4) fraction = 1.; // in case unfolding is off
    if (IsRecalibrationOn()) 
    {
      recalFactor = GetEMCALChannelRecalibrationFactor(iSupMod,ieta,iphi);
    }
    eCell  = cells->GetCellAmplitude(cluster->GetCellAbsId(iDigit))*fraction*recalFactor;
    
    if(cluster->E() > 0 && eCell > 0)
    {
      w  = GetCellWeight(eCell,cluster->E());
      
      etai=(Double_t)ieta;
      phii=(Double_t)iphi;    
      if(w > 0.0) 
      { 
        disp +=  w *((etai-etaMean)*(etai-etaMean)+(phii-phiMean)*(phii-phiMean)); 
        dEta +=  w * (etai-etaMean)*(etai-etaMean) ; 
        dPhi +=  w * (phii-phiMean)*(phii-phiMean) ; 
      }
    }
    else
      AliError(Form("Wrong energy %f and/or amplitude %f\n", eCell, cluster->E()));
  }// cell loop
  
  //Normalize to the weigth and set shower shape parameters
  if (wtot > 0 && nstat > 1) 
  {
    disp    /= wtot ;
    dEta    /= wtot ;
    dPhi    /= wtot ;
    sEta    /= wtot ;
    sPhi    /= wtot ;
    sEtaPhi /= wtot ;
    
    sEta    -= etaMean * etaMean ;
    sPhi    -= phiMean * phiMean ;
    sEtaPhi -= etaMean * phiMean ;
    
    l0 = (0.5 * (sEta + sPhi) + TMath::Sqrt( 0.25 * (sEta - sPhi) * (sEta - sPhi) + sEtaPhi * sEtaPhi ));
    l1 = (0.5 * (sEta + sPhi) - TMath::Sqrt( 0.25 * (sEta - sPhi) * (sEta - sPhi) + sEtaPhi * sEtaPhi ));
  }
  else
  {
    l0   = 0. ;
    l1   = 0. ;
    dEta = 0. ; dPhi = 0. ; disp    = 0. ;
    sEta = 0. ; sPhi = 0. ; sEtaPhi = 0. ;
  }  
  
}

//____________________________________________________________________________________________
void AliEMCALRecoUtils::RecalculateClusterShowerShapeParameters(const AliEMCALGeometry * geom, 
                                                                AliVCaloCells* cells, 
                                                                AliVCluster * cluster)
{
  // Calculates new center of gravity in the local EMCAL-module coordinates 
  // and tranfers into global ALICE coordinates
  // Calculates Dispersion and main axis and puts them into the cluster
  
  Float_t l0   = 0., l1   = 0.;
  Float_t disp = 0., dEta = 0., dPhi    = 0.; 
  Float_t sEta = 0., sPhi = 0., sEtaPhi = 0.;
  
  AliEMCALRecoUtils::RecalculateClusterShowerShapeParameters(geom,cells,cluster,l0,l1,disp,
                                                             dEta, dPhi, sEta, sPhi, sEtaPhi);
  
  cluster->SetM02(l0);
  cluster->SetM20(l1);
  if(disp > 0. ) cluster->SetDispersion(TMath::Sqrt(disp)) ;
  
} 

//____________________________________________________________________________
void AliEMCALRecoUtils::FindMatches(AliVEvent *event,
                                    TObjArray * clusterArr,  
                                    const AliEMCALGeometry *geom)
{
  //This function should be called before the cluster loop
  //Before call this function, please recalculate the cluster positions
  //Given the input event, loop over all the tracks, select the closest cluster as matched with fCutR
  //Store matched cluster indexes and residuals
  
  fMatchedTrackIndex  ->Reset();
  fMatchedClusterIndex->Reset();
  fResidualPhi->Reset();
  fResidualEta->Reset();
  
  fMatchedTrackIndex  ->Set(1000);
  fMatchedClusterIndex->Set(1000);
  fResidualPhi->Set(1000);
  fResidualEta->Set(1000);
  
  AliESDEvent* esdevent = dynamic_cast<AliESDEvent*> (event);
  AliAODEvent* aodevent = dynamic_cast<AliAODEvent*> (event);
  
  // init the magnetic field if not already on
  if(!TGeoGlobalMagField::Instance()->GetField())
  {
    AliInfo("Init the magnetic field\n");
    if     (esdevent) 
    {
      esdevent->InitMagneticField();
    }
    else if(aodevent)
    {
      Double_t curSol = 30000*aodevent->GetMagneticField()/5.00668;
      Double_t curDip = 6000 *aodevent->GetMuonMagFieldScale();
      AliMagF *field  = AliMagF::CreateFieldMap(curSol,curDip);
      TGeoGlobalMagField::Instance()->SetField(field);
    }
    else
    {
      AliInfo("Mag Field not initialized, null esd/aod evetn pointers");
    }
    
  } // Init mag field
  
  TObjArray *clusterArray = 0x0;
  if(!clusterArr)
    {
      clusterArray = new TObjArray(event->GetNumberOfCaloClusters());
      for(Int_t icl=0; icl<event->GetNumberOfCaloClusters(); icl++)
  {
    AliVCluster *cluster = (AliVCluster*) event->GetCaloCluster(icl);
    if(geom && !IsGoodCluster(cluster,geom,(AliVCaloCells*)event->GetEMCALCells())) continue;
    clusterArray->AddAt(cluster,icl);
  }
    }
  
  Int_t    matched=0;
  Double_t cv[21];
  for (Int_t i=0; i<21;i++) cv[i]=0;
  for(Int_t itr=0; itr<event->GetNumberOfTracks(); itr++)
  {
    AliExternalTrackParam *trackParam = 0;
    
    //If the input event is ESD, the starting point for extrapolation is TPCOut, if available, or TPCInner 
    AliESDtrack *esdTrack = 0;
    AliAODTrack *aodTrack = 0;
    if(esdevent)
    {
      esdTrack = esdevent->GetTrack(itr);
      if(!esdTrack) continue;
      if(!IsAccepted(esdTrack)) continue;
      if(esdTrack->Pt()<fCutMinTrackPt) continue;
      Double_t phi = esdTrack->Phi()*TMath::RadToDeg();
      if(TMath::Abs(esdTrack->Eta())>0.8 || phi <= 20 || phi >= 240 ) continue;
      trackParam =  const_cast<AliExternalTrackParam*>(esdTrack->GetInnerParam());
    }
    
    //If the input event is AOD, the starting point for extrapolation is at vertex
    //AOD tracks are selected according to its filterbit.
    else if(aodevent)
    {
      aodTrack = aodevent->GetTrack(itr);
      if(!aodTrack) continue;
      if(!aodTrack->TestFilterMask(fAODFilterMask)) continue; //Select AOD tracks that fulfill GetStandardITSTPCTrackCuts2010()
      if(aodTrack->Pt()<fCutMinTrackPt) continue;
      Double_t phi = aodTrack->Phi()*TMath::RadToDeg();
      if(TMath::Abs(aodTrack->Eta())>0.8 || phi <= 20 || phi >= 240 ) continue;
      Double_t pos[3],mom[3];
      aodTrack->GetXYZ(pos);
      aodTrack->GetPxPyPz(mom);
      AliDebug(5,Form("aod track: i=%d | pos=(%5.4f,%5.4f,%5.4f) | mom=(%5.4f,%5.4f,%5.4f) | charge=%d\n",itr,pos[0],pos[1],pos[2],mom[0],mom[1],mom[2],aodTrack->Charge()));
      trackParam= new AliExternalTrackParam(pos,mom,cv,aodTrack->Charge());
    }
    
    //Return if the input data is not "AOD" or "ESD"
    else
    {
      printf("Wrong input data type! Should be \"AOD\" or \"ESD\"\n");
      if(clusterArray)
  {
    clusterArray->Clear();
    delete clusterArray;
  }
      return;
    }
    
    if(!trackParam) continue;

    //Extrapolate the track to EMCal surface
    AliExternalTrackParam emcalParam(*trackParam);
    Float_t eta, phi;
    if(!ExtrapolateTrackToEMCalSurface(&emcalParam, 430., fMass, fStepSurface, eta, phi)) 
      {
  if(aodevent && trackParam) delete trackParam;
  continue;
      }

//    if(esdevent)
//      {
//  esdTrack->SetOuterParam(&emcalParam,AliExternalTrackParam::kMultSec);
//      }

    if(TMath::Abs(eta)>0.75 || (phi) < 70*TMath::DegToRad() || (phi) > 190*TMath::DegToRad())
      {
  if(aodevent && trackParam) delete trackParam;
  continue;
      }


    //Find matched clusters
    Int_t index = -1;
    Float_t dEta = -999, dPhi = -999;
    if(!clusterArr)
      {
  index = FindMatchedClusterInClusterArr(&emcalParam, &emcalParam, clusterArray, dEta, dPhi);  
      }
    else
      {
  index = FindMatchedClusterInClusterArr(&emcalParam, &emcalParam, clusterArr, dEta, dPhi);  
      }  
    
    if(index>-1)
    {
      fMatchedTrackIndex   ->AddAt(itr,matched);
      fMatchedClusterIndex ->AddAt(index,matched);
      fResidualEta         ->AddAt(dEta,matched);
      fResidualPhi         ->AddAt(dPhi,matched);
      matched++;
    }
    if(aodevent && trackParam) delete trackParam;
  }//track loop

  if(clusterArray)
    {
      clusterArray->Clear();
      delete clusterArray;
    }
  
  AliDebug(2,Form("Number of matched pairs = %d !\n",matched));
  
  fMatchedTrackIndex   ->Set(matched);
  fMatchedClusterIndex ->Set(matched);
  fResidualPhi         ->Set(matched);
  fResidualEta         ->Set(matched);
}

//________________________________________________________________________________
Int_t AliEMCALRecoUtils::FindMatchedClusterInEvent(const AliESDtrack *track, 
                                                   const AliVEvent *event, 
                                                   const AliEMCALGeometry *geom, 
                                                   Float_t &dEta, Float_t &dPhi)
{
  //
  // This function returns the index of matched cluster to input track
  // Returns -1 if no match is found
  Int_t index = -1;
  Double_t phiV = track->Phi()*TMath::RadToDeg();
  if(TMath::Abs(track->Eta())>0.8 || phiV <= 20 || phiV >= 240 ) return index;
  AliExternalTrackParam *trackParam = const_cast<AliExternalTrackParam*>(track->GetInnerParam());
  if(!trackParam) return index;
  AliExternalTrackParam emcalParam(*trackParam);
  Float_t eta, phi;
  if(!ExtrapolateTrackToEMCalSurface(&emcalParam, 430., fMass, fStepSurface, eta, phi)) return index;
  if(TMath::Abs(eta)>0.75 || (phi) < 70*TMath::DegToRad() || (phi) > 190*TMath::DegToRad()) return index;

  TObjArray *clusterArr = new TObjArray(event->GetNumberOfCaloClusters());

  for(Int_t icl=0; icl<event->GetNumberOfCaloClusters(); icl++)
  {
    AliVCluster *cluster = (AliVCluster*) event->GetCaloCluster(icl);
    if(geom && !IsGoodCluster(cluster,geom,(AliVCaloCells*)event->GetEMCALCells())) continue;
    clusterArr->AddAt(cluster,icl);
  }

  index = FindMatchedClusterInClusterArr(&emcalParam, &emcalParam, clusterArr, dEta, dPhi);  
  clusterArr->Clear();
  delete clusterArr;
  
  return index;
}

//_______________________________________________________________________________________________
Int_t  AliEMCALRecoUtils::FindMatchedClusterInClusterArr(const AliExternalTrackParam *emcalParam, 
                                                         AliExternalTrackParam *trkParam, 
                                                         const TObjArray * clusterArr, 
                                                         Float_t &dEta, Float_t &dPhi)
{
  // Find matched cluster in array
  
  dEta=-999, dPhi=-999;
  Float_t dRMax = fCutR, dEtaMax=fCutEta, dPhiMax=fCutPhi;
  Int_t index = -1;
  Float_t tmpEta=-999, tmpPhi=-999;

  Double_t exPos[3] = {0.,0.,0.};
  if(!emcalParam->GetXYZ(exPos)) return index;

  Float_t clsPos[3] = {0.,0.,0.};
  for(Int_t icl=0; icl<clusterArr->GetEntriesFast(); icl++)
    {
      AliVCluster *cluster = dynamic_cast<AliVCluster*> (clusterArr->At(icl)) ;
      if(!cluster || !cluster->IsEMCAL()) continue;
      cluster->GetPosition(clsPos);
      Double_t dR = TMath::Sqrt(TMath::Power(exPos[0]-clsPos[0],2)+TMath::Power(exPos[1]-clsPos[1],2)+TMath::Power(exPos[2]-clsPos[2],2));
      if(dR > fClusterWindow) continue;

      AliExternalTrackParam trkPamTmp (*trkParam);//Retrieve the starting point every time before the extrapolation
      if(!ExtrapolateTrackToCluster(&trkPamTmp, cluster, fMass, fStepCluster, tmpEta, tmpPhi)) continue;
      if(fCutEtaPhiSum)
        {
          Float_t tmpR=TMath::Sqrt(tmpEta*tmpEta + tmpPhi*tmpPhi);
          if(tmpR<dRMax)
      {
        dRMax=tmpR;
        dEtaMax=tmpEta;
        dPhiMax=tmpPhi;
        index=icl;
      }
        }
      else if(fCutEtaPhiSeparate)
        {
          if(TMath::Abs(tmpEta)<TMath::Abs(dEtaMax) && TMath::Abs(tmpPhi)<TMath::Abs(dPhiMax))
      {
        dEtaMax = tmpEta;
        dPhiMax = tmpPhi;
        index=icl;
      }
        }
      else
        {
          printf("Error: please specify your cut criteria\n");
          printf("To cut on sqrt(dEta^2+dPhi^2), use: SwitchOnCutEtaPhiSum()\n");
          printf("To cut on dEta and dPhi separately, use: SwitchOnCutEtaPhiSeparate()\n");
          return index;
        }
    }

  dEta=dEtaMax;
  dPhi=dPhiMax;

  return index;
}

//------------------------------------------------------------------------------------
Bool_t AliEMCALRecoUtils::ExtrapolateTrackToEMCalSurface(AliExternalTrackParam *trkParam, 
                                                         const Double_t emcalR,
                                                         const Double_t mass, 
                                                         const Double_t step, 
                                                         Float_t &eta, 
                                                         Float_t &phi)
{
  //Extrapolate track to EMCAL surface
  
  eta = -999, phi = -999;
  if(!trkParam) return kFALSE;
  if(!AliTrackerBase::PropagateTrackToBxByBz(trkParam, emcalR, mass, step, kTRUE, 0.8, -1)) return kFALSE;
  Double_t trkPos[3] = {0.,0.,0.};
  if(!trkParam->GetXYZ(trkPos)) return kFALSE;
  TVector3 trkPosVec(trkPos[0],trkPos[1],trkPos[2]);
  eta = trkPosVec.Eta();
  phi = trkPosVec.Phi();
  if(phi<0)
    phi += 2*TMath::Pi();

  return kTRUE;
}

//-----------------------------------------------------------------------------------
Bool_t AliEMCALRecoUtils::ExtrapolateTrackToPosition(AliExternalTrackParam *trkParam, 
                                                     const Float_t *clsPos, 
                                                     Double_t mass, 
                                                     Double_t step, 
                                                     Float_t &tmpEta, 
                                                     Float_t &tmpPhi)
{
  //
  //Return the residual by extrapolating a track param to a global position
  //
  tmpEta = -999;
  tmpPhi = -999;
  if(!trkParam) return kFALSE;
  Double_t trkPos[3] = {0.,0.,0.};
  TVector3 vec(clsPos[0],clsPos[1],clsPos[2]);
  Double_t alpha =  ((int)(vec.Phi()*TMath::RadToDeg()/20)+0.5)*20*TMath::DegToRad();
  vec.RotateZ(-alpha); //Rotate the cluster to the local extrapolation coordinate system
  if(!AliTrackerBase::PropagateTrackToBxByBz(trkParam, vec.X(), mass, step,kTRUE, 0.8, -1)) return kFALSE;
  if(!trkParam->GetXYZ(trkPos)) return kFALSE; //Get the extrapolated global position

  TVector3 clsPosVec(clsPos[0],clsPos[1],clsPos[2]);
  TVector3 trkPosVec(trkPos[0],trkPos[1],trkPos[2]);

  // track cluster matching
  tmpPhi = clsPosVec.DeltaPhi(trkPosVec);    // tmpPhi is between -pi and pi
  tmpEta = clsPosVec.Eta()-trkPosVec.Eta();

  return kTRUE;
}

//----------------------------------------------------------------------------------
Bool_t AliEMCALRecoUtils::ExtrapolateTrackToCluster(AliExternalTrackParam *trkParam, 
                                                    const AliVCluster *cluster, 
                                                    const Double_t mass, 
                                                    const Double_t step, 
                                                    Float_t &tmpEta, 
                                                    Float_t &tmpPhi)
{
  //
  //Return the residual by extrapolating a track param to a cluster
  //
  tmpEta = -999;
  tmpPhi = -999;
  if(!cluster || !trkParam) return kFALSE;

  Float_t clsPos[3] = {0.,0.,0.};
  cluster->GetPosition(clsPos);

  return ExtrapolateTrackToPosition(trkParam, clsPos, mass, step, tmpEta, tmpPhi);
}

//---------------------------------------------------------------------------------
Bool_t AliEMCALRecoUtils::ExtrapolateTrackToCluster(AliExternalTrackParam *trkParam, 
                                                    const AliVCluster *cluster, 
                                                    Float_t &tmpEta, 
                                                    Float_t &tmpPhi)
{
  //
  //Return the residual by extrapolating a track param to a clusterfStepCluster
  //

  return ExtrapolateTrackToCluster(trkParam, cluster, fMass, fStepCluster, tmpEta, tmpPhi);
}

//_______________________________________________________________________
void AliEMCALRecoUtils::GetMatchedResiduals(const Int_t clsIndex, 
                                            Float_t &dEta, Float_t &dPhi)
{
  //Given a cluster index as in AliESDEvent::GetCaloCluster(clsIndex)
  //Get the residuals dEta and dPhi for this cluster to the closest track
  //Works with ESDs and AODs

  if( FindMatchedPosForCluster(clsIndex) >= 999 )
  {
    AliDebug(2,"No matched tracks found!\n");
    dEta=999.;
    dPhi=999.;
    return;
  }
  dEta = fResidualEta->At(FindMatchedPosForCluster(clsIndex));
  dPhi = fResidualPhi->At(FindMatchedPosForCluster(clsIndex));
}

//______________________________________________________________________________________________
void AliEMCALRecoUtils::GetMatchedClusterResiduals(Int_t trkIndex, Float_t &dEta, Float_t &dPhi)
{
  //Given a track index as in AliESDEvent::GetTrack(trkIndex)
  //Get the residuals dEta and dPhi for this track to the closest cluster
  //Works with ESDs and AODs

  if( FindMatchedPosForTrack(trkIndex) >= 999 )
  {
    AliDebug(2,"No matched cluster found!\n");
    dEta=999.;
    dPhi=999.;
    return;
  }
  dEta = fResidualEta->At(FindMatchedPosForTrack(trkIndex));
  dPhi = fResidualPhi->At(FindMatchedPosForTrack(trkIndex));
}

//__________________________________________________________
Int_t AliEMCALRecoUtils::GetMatchedTrackIndex(Int_t clsIndex)
{
  //Given a cluster index as in AliESDEvent::GetCaloCluster(clsIndex)
  //Get the index of matched track to this cluster
  //Works with ESDs and AODs
  
  if(IsClusterMatched(clsIndex))
    return fMatchedTrackIndex->At(FindMatchedPosForCluster(clsIndex));
  else 
    return -1; 
}

//__________________________________________________________
Int_t AliEMCALRecoUtils::GetMatchedClusterIndex(Int_t trkIndex)
{
  //Given a track index as in AliESDEvent::GetTrack(trkIndex)
  //Get the index of matched cluster to this track
  //Works with ESDs and AODs
  
  if(IsTrackMatched(trkIndex))
    return fMatchedClusterIndex->At(FindMatchedPosForTrack(trkIndex));
  else 
    return -1; 
}

//______________________________________________________________
Bool_t AliEMCALRecoUtils::IsClusterMatched(Int_t clsIndex) const
{
  //Given a cluster index as in AliESDEvent::GetCaloCluster(clsIndex)
  //Returns if the cluster has a match
  if(FindMatchedPosForCluster(clsIndex) < 999) 
    return kTRUE;
  else
    return kFALSE;
}

//____________________________________________________________
Bool_t AliEMCALRecoUtils::IsTrackMatched(Int_t trkIndex) const 
{
  //Given a track index as in AliESDEvent::GetTrack(trkIndex)
  //Returns if the track has a match
  if(FindMatchedPosForTrack(trkIndex) < 999) 
    return kTRUE;
  else
    return kFALSE;
}

//______________________________________________________________________
UInt_t AliEMCALRecoUtils::FindMatchedPosForCluster(Int_t clsIndex) const
{
  //Given a cluster index as in AliESDEvent::GetCaloCluster(clsIndex)
  //Returns the position of the match in the fMatchedClusterIndex array
  Float_t tmpR = fCutR;
  UInt_t pos = 999;
  
  for(Int_t i=0; i<fMatchedClusterIndex->GetSize(); i++) 
  {
    if(fMatchedClusterIndex->At(i)==clsIndex) 
    {
      Float_t r = TMath::Sqrt(fResidualEta->At(i)*fResidualEta->At(i) + fResidualPhi->At(i)*fResidualPhi->At(i));
      if(r<tmpR) 
      {
        pos=i;
        tmpR=r;
        AliDebug(3,Form("Matched cluster index: index: %d, dEta: %2.4f, dPhi: %2.4f.\n",
                        fMatchedClusterIndex->At(i),fResidualEta->At(i),fResidualPhi->At(i)));
      }
    }
  }
  return pos;
}

//____________________________________________________________________
UInt_t AliEMCALRecoUtils::FindMatchedPosForTrack(Int_t trkIndex) const
{
  //Given a track index as in AliESDEvent::GetTrack(trkIndex)
  //Returns the position of the match in the fMatchedTrackIndex array
  Float_t tmpR = fCutR;
  UInt_t pos = 999;
  
  for(Int_t i=0; i<fMatchedTrackIndex->GetSize(); i++) 
  {
    if(fMatchedTrackIndex->At(i)==trkIndex) 
    {
      Float_t r = TMath::Sqrt(fResidualEta->At(i)*fResidualEta->At(i) + fResidualPhi->At(i)*fResidualPhi->At(i));
      if(r<tmpR) 
      {
        pos=i;
        tmpR=r;
        AliDebug(3,Form("Matched track index: index: %d, dEta: %2.4f, dPhi: %2.4f.\n",
                        fMatchedTrackIndex->At(i),fResidualEta->At(i),fResidualPhi->At(i)));
      }
    }
  }
  return pos;
}

//__________________________________________________________________________
Bool_t AliEMCALRecoUtils::IsGoodCluster(AliVCluster *cluster, 
                                        const AliEMCALGeometry *geom, 
                                        AliVCaloCells* cells,const Int_t bc)
{
  // check if the cluster survives some quality cut
  //
  //
  Bool_t isGood=kTRUE;

  if(!cluster || !cluster->IsEMCAL())              return kFALSE;
  
  if(ClusterContainsBadChannel(geom,cluster->GetCellsAbsId(),cluster->GetNCells())) return kFALSE;
  
  if(!CheckCellFiducialRegion(geom,cluster,cells)) return kFALSE;
  
  if(IsExoticCluster(cluster, cells,bc))           return kFALSE;

  return isGood;
}

//__________________________________________________________
Bool_t AliEMCALRecoUtils::IsAccepted(AliESDtrack *esdTrack)
{
  // Given a esd track, return whether the track survive all the cuts

  // The different quality parameter are first
  // retrieved from the track. then it is found out what cuts the
  // track did not survive and finally the cuts are imposed.

  UInt_t status = esdTrack->GetStatus();

  Int_t nClustersITS = esdTrack->GetITSclusters(0);
  Int_t nClustersTPC = esdTrack->GetTPCclusters(0);

  Float_t chi2PerClusterITS = -1;
  Float_t chi2PerClusterTPC = -1;
  if (nClustersITS!=0)
    chi2PerClusterITS = esdTrack->GetITSchi2()/Float_t(nClustersITS);
  if (nClustersTPC!=0) 
    chi2PerClusterTPC = esdTrack->GetTPCchi2()/Float_t(nClustersTPC);


  //DCA cuts
  if(fTrackCutsType==kGlobalCut)
    {
      Float_t maxDCAToVertexXYPtDep = 0.0182 + 0.0350/TMath::Power(esdTrack->Pt(),1.01); //This expression comes from AliESDtrackCuts::GetStandardITSTPCTrackCuts2010()
      //AliDebug(3,Form("Track pT = %f, DCAtoVertexXY = %f",esdTrack->Pt(),MaxDCAToVertexXYPtDep));
      SetMaxDCAToVertexXY(maxDCAToVertexXYPtDep); //Set pT dependent DCA cut to vertex in x-y plane
    }


  Float_t b[2];
  Float_t bCov[3];
  esdTrack->GetImpactParameters(b,bCov);
  if (bCov[0]<=0 || bCov[2]<=0) 
  {
    AliDebug(1, "Estimated b resolution lower or equal zero!");
    bCov[0]=0; bCov[2]=0;
  }

  Float_t dcaToVertexXY = b[0];
  Float_t dcaToVertexZ = b[1];
  Float_t dcaToVertex = -1;

  if (fCutDCAToVertex2D)
    dcaToVertex = TMath::Sqrt(dcaToVertexXY*dcaToVertexXY/fCutMaxDCAToVertexXY/fCutMaxDCAToVertexXY + dcaToVertexZ*dcaToVertexZ/fCutMaxDCAToVertexZ/fCutMaxDCAToVertexZ);
  else
    dcaToVertex = TMath::Sqrt(dcaToVertexXY*dcaToVertexXY + dcaToVertexZ*dcaToVertexZ);
    
  // cut the track?
  
  Bool_t cuts[kNCuts];
  for (Int_t i=0; i<kNCuts; i++) cuts[i]=kFALSE;
  
  // track quality cuts
  if (fCutRequireTPCRefit && (status&AliESDtrack::kTPCrefit)==0)
    cuts[0]=kTRUE;
  if (fCutRequireITSRefit && (status&AliESDtrack::kITSrefit)==0)
    cuts[1]=kTRUE;
  if (nClustersTPC<fCutMinNClusterTPC)
    cuts[2]=kTRUE;
  if (nClustersITS<fCutMinNClusterITS) 
    cuts[3]=kTRUE;
  if (chi2PerClusterTPC>fCutMaxChi2PerClusterTPC) 
    cuts[4]=kTRUE; 
  if (chi2PerClusterITS>fCutMaxChi2PerClusterITS) 
    cuts[5]=kTRUE;  
  if (!fCutAcceptKinkDaughters && esdTrack->GetKinkIndex(0)>0)
    cuts[6]=kTRUE;
  if (fCutDCAToVertex2D && dcaToVertex > 1)
    cuts[7] = kTRUE;
  if (!fCutDCAToVertex2D && TMath::Abs(dcaToVertexXY) > fCutMaxDCAToVertexXY)
    cuts[8] = kTRUE;
  if (!fCutDCAToVertex2D && TMath::Abs(dcaToVertexZ) > fCutMaxDCAToVertexZ)
    cuts[9] = kTRUE;

  if(fTrackCutsType==kGlobalCut)
    {
      //Require at least one SPD point + anything else in ITS
      if( (esdTrack->HasPointOnITSLayer(0) || esdTrack->HasPointOnITSLayer(1)) == kFALSE)
  cuts[10] = kTRUE;
    }

  Bool_t cut=kFALSE;
  for (Int_t i=0; i<kNCuts; i++)
    if (cuts[i]) { cut = kTRUE ; }

    // cut the track
  if (cut) 
    return kFALSE;
  else 
    return kTRUE;
}

//_____________________________________
void AliEMCALRecoUtils::InitTrackCuts()
{
  //Intilize the track cut criteria
  //By default these cuts are set according to AliESDtrackCuts::GetStandardTPCOnlyTrackCuts()
  //Also you can customize the cuts using the setters
  
  switch (fTrackCutsType)
  {
    case kTPCOnlyCut:
    {
      AliInfo(Form("Track cuts for matching: GetStandardTPCOnlyTrackCuts()"));
      //TPC
      SetMinNClustersTPC(70);
      SetMaxChi2PerClusterTPC(4);
      SetAcceptKinkDaughters(kFALSE);
      SetRequireTPCRefit(kFALSE);
      
      //ITS
      SetRequireITSRefit(kFALSE);
      SetMaxDCAToVertexZ(3.2);
      SetMaxDCAToVertexXY(2.4);
      SetDCAToVertex2D(kTRUE);
      
      break;
    }
      
    case kGlobalCut:
    {
      AliInfo(Form("Track cuts for matching: GetStandardITSTPCTrackCuts2010(kTURE)"));
      //TPC
      SetMinNClustersTPC(70);
      SetMaxChi2PerClusterTPC(4);
      SetAcceptKinkDaughters(kFALSE);
      SetRequireTPCRefit(kTRUE);
      
      //ITS
      SetRequireITSRefit(kTRUE);
      SetMaxDCAToVertexZ(2);
      SetMaxDCAToVertexXY();
      SetDCAToVertex2D(kFALSE);
      
      break;
    }
      
    case kLooseCut:
    {
      AliInfo(Form("Track cuts for matching: Loose cut w/o DCA cut"));
      SetMinNClustersTPC(50);
      SetAcceptKinkDaughters(kTRUE);
      
      break;
    }
  }
}


//________________________________________________________________________
void AliEMCALRecoUtils::SetClusterMatchedToTrack(const AliVEvent *event)
{
  // Checks if tracks are matched to EMC clusters and set the matched EMCAL cluster index to ESD track. 

  Int_t nTracks = event->GetNumberOfTracks();
  for (Int_t iTrack = 0; iTrack < nTracks; ++iTrack) 
  {
    AliVTrack* track = dynamic_cast<AliVTrack*>(event->GetTrack(iTrack));
    if (!track) 
    {
      AliWarning(Form("Could not receive track %d", iTrack));
      continue;
    }
    
    Int_t matchClusIndex = GetMatchedClusterIndex(iTrack);       
    track->SetEMCALcluster(matchClusIndex); //sets -1 if track not matched within residual
    /*the following can be done better if AliVTrack::SetStatus will be there. Patch pending with Andreas/Peter*/
    AliESDtrack* esdtrack = dynamic_cast<AliESDtrack*>(track);
    if (esdtrack) { 
      if(matchClusIndex != -1) 
        esdtrack->SetStatus(AliESDtrack::kEMCALmatch);
      else
        esdtrack->ResetStatus(AliESDtrack::kEMCALmatch);
    } else {
      AliAODTrack* aodtrack = dynamic_cast<AliAODTrack*>(track);
      if(matchClusIndex != -1) 
        aodtrack->SetStatus(AliESDtrack::kEMCALmatch);
      else
        aodtrack->ResetStatus(AliESDtrack::kEMCALmatch);
    }

  }
  AliDebug(2,"Track matched to closest cluster");  
}

//_________________________________________________________________________
void AliEMCALRecoUtils::SetTracksMatchedToCluster(const AliVEvent *event)
{
  // Checks if EMC clusters are matched to ESD track.
  // Adds track indexes of all the tracks matched to a cluster withing residuals in ESDCalocluster.
  
  for (Int_t iClus=0; iClus < event->GetNumberOfCaloClusters(); ++iClus) 
  {
    AliVCluster *cluster = event->GetCaloCluster(iClus);
    if (!cluster->IsEMCAL()) 
      continue;
    
    Int_t nTracks = event->GetNumberOfTracks();
    TArrayI arrayTrackMatched(nTracks);
    
    // Get the closest track matched to the cluster
    Int_t nMatched = 0;
    Int_t matchTrackIndex = GetMatchedTrackIndex(iClus);
    if (matchTrackIndex != -1) 
    {
      arrayTrackMatched[nMatched] = matchTrackIndex;
      nMatched++;
    }
    
    // Get all other tracks matched to the cluster
    for(Int_t iTrk=0; iTrk<nTracks; ++iTrk) 
    {
      AliVTrack* track = dynamic_cast<AliVTrack*>(event->GetTrack(iTrk));
      if(iTrk == matchTrackIndex) continue;
      if(track->GetEMCALcluster() == iClus)
      {
        arrayTrackMatched[nMatched] = iTrk;
        ++nMatched;
      }
    }
    
    //printf("Tender::SetTracksMatchedToCluster - cluster E %f, N matches %d, first match %d\n",cluster->E(),nMatched,arrayTrackMatched[0]);
    
    arrayTrackMatched.Set(nMatched);
    AliESDCaloCluster *esdcluster = dynamic_cast<AliESDCaloCluster*>(cluster);
    if (esdcluster) 
      esdcluster->AddTracksMatched(arrayTrackMatched);
    else if (nMatched>0) {
      AliAODCaloCluster *aodcluster = dynamic_cast<AliAODCaloCluster*>(cluster);
      if (aodcluster)
        aodcluster->AddTrackMatched(event->GetTrack(arrayTrackMatched.At(0)));
    }
    
    Float_t eta= -999, phi = -999;
    if (matchTrackIndex != -1) 
      GetMatchedResiduals(iClus, eta, phi);
    cluster->SetTrackDistance(phi, eta);
  }
  
  AliDebug(2,"Cluster matched to tracks");  
}

//___________________________________________________
void AliEMCALRecoUtils::Print(const Option_t *) const 
{
  // Print Parameters
  
  printf("AliEMCALRecoUtils Settings: \n");
  printf("Misalignment shifts\n");
  for(Int_t i=0; i<5; i++) printf("\t sector %d, traslation (x,y,z)=(%f,%f,%f), rotation (x,y,z)=(%f,%f,%f)\n",i, 
                                  fMisalTransShift[i*3],fMisalTransShift[i*3+1],fMisalTransShift[i*3+2],
                                  fMisalRotShift[i*3],  fMisalRotShift[i*3+1],  fMisalRotShift[i*3+2]   );
  printf("Non linearity function %d, parameters:\n", fNonLinearityFunction);
  for(Int_t i=0; i<6; i++) printf("param[%d]=%f\n",i, fNonLinearityParams[i]);
  
  printf("Position Recalculation option %d, Particle Type %d, fW0 %2.2f, Recalibrate Data %d \n",fPosAlgo,fParticleType,fW0, fRecalibration);

  printf("Matching criteria: ");
  if(fCutEtaPhiSum)
    {
      printf("sqrt(dEta^2+dPhi^2)<%4.3f\n",fCutR);
    }
  else if(fCutEtaPhiSeparate)
    {
      printf("dEta<%4.3f, dPhi<%4.3f\n",fCutEta,fCutPhi);
    }
  else
    {
      printf("Error\n");
      printf("please specify your cut criteria\n");
      printf("To cut on sqrt(dEta^2+dPhi^2), use: SwitchOnCutEtaPhiSum()\n");
      printf("To cut on dEta and dPhi separately, use: SwitchOnCutEtaPhiSeparate()\n");
    }

  printf("Mass hypothesis = %2.3f [GeV/c^2], extrapolation step to surface = %2.2f[cm], step to cluster = %2.2f[cm]\n",fMass,fStepSurface, fStepCluster);
  printf("Cluster selection window: dR < %2.0f\n",fClusterWindow);

  printf("Track cuts: \n");
  printf("Minimum track pT: %1.2f\n",fCutMinTrackPt);
  printf("AOD track selection mask: %d\n",fAODFilterMask);
  printf("TPCRefit = %d, ITSRefit = %d\n",fCutRequireTPCRefit,fCutRequireITSRefit);
  printf("AcceptKinks = %d\n",fCutAcceptKinkDaughters);
  printf("MinNCulsterTPC = %d, MinNClusterITS = %d\n",fCutMinNClusterTPC,fCutMinNClusterITS);
  printf("MaxChi2TPC = %2.2f, MaxChi2ITS = %2.2f\n",fCutMaxChi2PerClusterTPC,fCutMaxChi2PerClusterITS);
  printf("DCSToVertex2D = %d, MaxDCAToVertexXY = %2.2f, MaxDCAToVertexZ = %2.2f\n",fCutDCAToVertex2D,fCutMaxDCAToVertexXY,fCutMaxDCAToVertexZ);
}

