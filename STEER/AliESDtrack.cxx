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
//-----------------------------------------------------------------
//           Implementation of the ESD track class
//   ESD = Event Summary Data
//   This is the class to deal with during the phisical analysis of data
//      Origin: Iouri Belikov, CERN
//      e-mail: Jouri.Belikov@cern.ch
//-----------------------------------------------------------------

#include "TMath.h"

#include "AliESDtrack.h"
#include "AliKalmanTrack.h"

ClassImp(AliESDtrack)

//_______________________________________________________________________
AliESDtrack::AliESDtrack() : 
fFlags(0),
fLabel(0),
fTrackLength(0),
fStopVertex(0),
fRalpha(0),
fRx(0),
fCalpha(0),
fCx(0),
fCchi2(1e10),
fIalpha(0),
fIx(0),
fOalpha(0),
fOx(0),
fITSchi2(0),
fITSncls(0),
fITSsignal(0),
fTPCchi2(0),
fTPCncls(0),
fTPCClusterMap(159),//number of padrows
fTPCsignal(0),
fTRDchi2(0),
fTRDncls(0),
fTRDsignal(0),
fTOFchi2(0),
fTOFindex(0),
fTOFsignal(-1),
fPHOSsignal(-1),
fEMCALsignal(-1),
fRICHsignal(-1)
{
  //
  // The default ESD constructor 
  //
  for (Int_t i=0; i<kSPECIES; i++) {
    fTrackTime[i]=0.;
    fR[i]=1.;
    fITSr[i]=1.;
    fTPCr[i]=1.;
    fTRDr[i]=1.;
    fTOFr[i]=1.;
    fRICHr[i]=1.;
  }
  
  for (Int_t i=0; i<kSPECIESN; i++) {
    fPHOSr[i]  = 1.;
    fEMCALr[i] = 1.;
  }

 
  fPHOSpos[0]=fPHOSpos[1]=fPHOSpos[2]=0.;
  fEMCALpos[0]=fEMCALpos[1]=fEMCALpos[2]=0.;
  Int_t i;
  for (i=0; i<5; i++)  { fRp[i]=0.; fCp[i]=0.; fIp[i]=0.; fOp[i]=0.;}
  for (i=0; i<15; i++) { fRc[i]=0.; fCc[i]=0.; fIc[i]=0.; fOc[i]=0.;   }
  for (i=0; i<6; i++)  { fITSindex[i]=0; }
  for (i=0; i<180; i++){ fTPCindex[i]=0; }
  for (i=0; i<90; i++) { fTRDindex[i]=0; }
  fTPCLabel = 0;
  fTRDLabel = 0;
  fITSLabel = 0;
  
}
//_______________________________________________________________________
void AliESDtrack::MakeMiniESDtrack() {
  // Reseting all the data members except
  // fFlags: Reconstruction status flags 
  // fLabel: Track label
  // fR[kSPECIES]: combined "detector response probability"
  // Running track parameters
  // fRalpha: track rotation angle
  // fRx: X-coordinate of the track reference plane 
  // fRp[5]: external track parameters  
  // fRc[15]: external cov. matrix of the track parameters

  fTrackLength = 0;
  for (Int_t i=0; i<kSPECIES; i++) fTrackTime[i] = 0;
  fStopVertex = 0;


  // Reset track parameters constrained to the primary vertex
  fCalpha = 0;
  fCx = 0;
  for (Int_t i=0; i<5; i++) fCp[i] = 0;
  for (Int_t i=0; i<15; i++) fCc[i] = 0;
  fCchi2 = 0;

  // Reset track parameters at the inner wall of the TPC
  fIalpha = 0;
  fIx = 0;
  for (Int_t i=0; i<5; i++) fIp[i] = 0;
  for (Int_t i=0; i<15; i++) fIc[i] = 0;

  // Reset track parameters at the radius of the PHOS
  fOalpha = 0;
  fOx = 0;
  for (Int_t i=0; i<5; i++) fOp[i] = 0;
  for (Int_t i=0; i<15; i++) fOc[i] = 0;

  //Reset rrack parameters at the radius of the EMCAL
  fXalpha = 0;
  fXx = 0;
  for (Int_t i=0; i<5; i++) fXp[i] = 0;
  for (Int_t i=0; i<15; i++) fXc[i] = 0;

  // ITS related track information
  fITSchi2 = 0;
  for (Int_t i=0; i<6; i++) fITSchi2MIP[i] = 0;
  fITSncls = 0;
  for (Int_t i=0; i<6; i++) fITSindex[i] = 0;
  fITSsignal = 0;
  for (Int_t i=0; i<kSPECIES ; i++) fITSr[i];
  fITSLabel = 0;
  fITSFakeRatio = 0;

  // TPC related track information
  fTPCchi2 = 0;
  fTPCncls = 0;
  for (Int_t i=0; i<180; i++) fTPCindex[i] = 0;
  fTPCClusterMap = 0;
  fTPCsignal = 0;
  for (Int_t i=0; i<kSPECIES; i++) fTPCr[i] = 0;
  fTPCLabel = 0;

  // TRD related track information
  fTRDchi2 = 0;
  fTRDncls = 0;
  for (Int_t i=0; i<90; i++) fTRDindex[i] = 0;
  fTRDsignal = 0;
  for (Int_t i=0; i<kSPECIES; i++) fTRDr[i] = 0;
  fTRDLabel = 0;

  // TOF related track information
  fTOFchi2 = 0;
  fTOFindex = 0;
  fTOFsignal = 0;
  for (Int_t i=0; i<kSPECIES; i++) fTOFr[i] = 0;

  // PHOS related track information 
  for (Int_t i=0; i<3; i++) fPHOSpos[i] = 0;
  fPHOSsignal = 0;
  fPHOStof = 0;
  for (Int_t i=0; i<kSPECIESN; i++) fPHOSr[i] = 0;

  // EMCAL related track information 
  for (Int_t i=0; i<3; i++) fEMCALpos[i] = 0;
  fEMCALsignal = 0;
  for (Int_t i=0; i<kSPECIESN; i++) fEMCALr[i] = 0;

  // HMPID related track information
  fRICHsignal = 0;
  for (Int_t i=0; i<kSPECIES; i++) fRICHr[i] = 0;

}
//_______________________________________________________________________
Double_t AliESDtrack::GetMass() const {
  // Returns the mass of the most probable particle type
  Float_t max=0.;
  Int_t k=-1;
  for (Int_t i=0; i<kSPECIES; i++) {
    if (fR[i]>max) {k=i; max=fR[i];}
  }
  if (k==0) return 0.00051;
  if (k==1) return 0.10566;
  if (k==2||k==-1) return 0.13957;
  if (k==3) return 0.49368;
  if (k==4) return 0.93827;
  Warning("GetMass()","Undefined mass !");
  return 0.13957;
}

//_______________________________________________________________________
Bool_t AliESDtrack::UpdateTrackParams(AliKalmanTrack *t, ULong_t flags) {
  //
  // This function updates track's running parameters 
  //
  SetStatus(flags);
  fLabel=t->GetLabel();

  if (t->IsStartedTimeIntegral()) {
    SetStatus(kTIME);
    Double_t times[10];t->GetIntegratedTimes(times); SetIntegratedTimes(times);
    SetIntegratedLength(t->GetIntegratedLength());
  }

  fRalpha=t->GetAlpha();
  t->GetExternalParameters(fRx,fRp);
  t->GetExternalCovariance(fRc);

  switch (flags) {
    
  case kITSin: case kITSout: case kITSrefit:
    fITSncls=t->GetNumberOfClusters();
    fITSchi2=t->GetChi2();
    for (Int_t i=0;i<fITSncls;i++) fITSindex[i]=t->GetClusterIndex(i);
    fITSsignal=t->GetPIDsignal();
    fITSLabel = t->GetLabel();
    fITSFakeRatio = t->GetFakeRatio();
    break;
    
  case kTPCin: case kTPCrefit:
    fTPCLabel = t->GetLabel();
    fIalpha=fRalpha;
    fIx=fRx;
    {
      Int_t i;
      for (i=0; i<5; i++) fIp[i]=fRp[i];
      for (i=0; i<15;i++) fIc[i]=fRc[i];
    }
  case kTPCout:
  
    fTPCncls=t->GetNumberOfClusters();
    fTPCchi2=t->GetChi2();
    
     {//prevrow must be declared in separate namespace, otherwise compiler cries:
      //"jump to case label crosses initialization of `Int_t prevrow'"
       Int_t prevrow = -1;
       //       for (Int_t i=0;i<fTPCncls;i++) 
       for (Int_t i=0;i<160;i++) 
        {
          fTPCindex[i]=t->GetClusterIndex(i);

          // Piotr's Cluster Map for HBT  
          // ### please change accordingly if cluster array is changing 
          // to "New TPC Tracking" style (with gaps in array) 
          Int_t idx = fTPCindex[i];
          Int_t sect = (idx&0xff000000)>>24;
          Int_t row = (idx&0x00ff0000)>>16;
          if (sect > 18) row +=63; //if it is outer sector, add number of inner sectors

          fTPCClusterMap.SetBitNumber(row,kTRUE);

          //Fill the gap between previous row and this row with 0 bits
          //In case  ###  pleas change it as well - just set bit 0 in case there 
          //is no associated clusters for current "i"
          if (prevrow < 0) 
           {
             prevrow = row;//if previous bit was not assigned yet == this is the first one
           }
          else
           { //we don't know the order (inner to outer or reverse)
             //just to be save in case it is going to change
             Int_t n = 0, m = 0;
             if (prevrow < row)
              {
                n = prevrow;
                m = row;
              }
             else
              {
                n = row;
                m = prevrow;
              }

             for (Int_t j = n+1; j < m; j++)
              {
                fTPCClusterMap.SetBitNumber(j,kFALSE);
              }
             prevrow = row; 
           }
          // End Of Piotr's Cluster Map for HBT
        }
     }
    fTPCsignal=t->GetPIDsignal();
    {Double_t mass=t->GetMass();    // preliminary mass setting 
    if (mass>0.5) fR[4]=1.;         //        used by
    else if (mass<0.4) fR[2]=1.;    // the ITS reconstruction
    else fR[3]=1.;}
                     //
    break;

  case kTRDout:
    { //requested by the PHOS/EMCAL  ("temporary solution")
      Double_t r=460.;
      if (t->PropagateTo(r,30.,0.)) {  
         fOalpha=t->GetAlpha();
         t->GetExternalParameters(fOx,fOp);
         t->GetExternalCovariance(fOc);
      }
      r=450.;
      if (t->PropagateTo(r,30.,0.)) {  
         fXalpha=t->GetAlpha();
         t->GetExternalParameters(fXx,fXp);
         t->GetExternalCovariance(fXc);
      }
    } // don't put break here, it goes together with the next case
  case kTRDin: case kTRDrefit:
    fTRDLabel = t->GetLabel();

    fTRDncls=t->GetNumberOfClusters();
    fTRDchi2=t->GetChi2();
    for (Int_t i=0;i<fTRDncls;i++) fTRDindex[i]=t->GetClusterIndex(i);
    fTRDsignal=t->GetPIDsignal();
    break;
  case kTRDStop:
    break;
  case kTOFin: 
    break;
  case kTOFout: 
    break;
  default: 
    Error("UpdateTrackParams()","Wrong flag !\n");
    return kFALSE;
  }

  return kTRUE;
}

//_______________________________________________________________________
void 
AliESDtrack::SetConstrainedTrackParams(AliKalmanTrack *t, Double_t chi2) {
  //
  // This function sets the constrained track parameters 
  //
  fCalpha=t->GetAlpha();
  t->GetExternalParameters(fCx,fCp);
  t->GetExternalCovariance(fCc);
  fCchi2=chi2;
}


//_______________________________________________________________________
void AliESDtrack::GetExternalParameters(Double_t &x, Double_t p[5]) const {
  //---------------------------------------------------------------------
  // This function returns external representation of the track parameters
  //---------------------------------------------------------------------
  x=fRx;
  for (Int_t i=0; i<5; i++) p[i]=fRp[i];
}
//_______________________________________________________________________
void AliESDtrack::GetExternalCovariance(Double_t cov[15]) const {
  //---------------------------------------------------------------------
  // This function returns external representation of the cov. matrix
  //---------------------------------------------------------------------
  for (Int_t i=0; i<15; i++) cov[i]=fRc[i];
}


//_______________________________________________________________________
void 
AliESDtrack::GetConstrainedExternalParameters(Double_t &x, Double_t p[5])const{
  //---------------------------------------------------------------------
  // This function returns the constrained external track parameters
  //---------------------------------------------------------------------
  x=fCx;
  for (Int_t i=0; i<5; i++) p[i]=fCp[i];
}
//_______________________________________________________________________
void 
AliESDtrack::GetConstrainedExternalCovariance(Double_t c[15]) const {
  //---------------------------------------------------------------------
  // This function returns the constrained external cov. matrix
  //---------------------------------------------------------------------
  for (Int_t i=0; i<15; i++) c[i]=fCc[i];
}


Double_t AliESDtrack::GetP() const {
  //---------------------------------------------------------------------
  // This function returns the track momentum
  //---------------------------------------------------------------------
  if (TMath::Abs(fRp[4])<=0) return 0;
  Double_t pt=1./TMath::Abs(fRp[4]);
  return pt*TMath::Sqrt(1.+ fRp[3]*fRp[3]);
}

void AliESDtrack::GetConstrainedPxPyPz(Double_t *p) const {
  //---------------------------------------------------------------------
  // This function returns the constrained global track momentum components
  //---------------------------------------------------------------------
  if (TMath::Abs(fCp[4])<=0) {
    p[0]=p[1]=p[2]=0;
    return;
  }
  Double_t phi=TMath::ASin(fCp[2]) + fCalpha;
  Double_t pt=1./TMath::Abs(fCp[4]);
  p[0]=pt*TMath::Cos(phi); p[1]=pt*TMath::Sin(phi); p[2]=pt*fCp[3]; 
}
void AliESDtrack::GetConstrainedXYZ(Double_t *xyz) const {
  //---------------------------------------------------------------------
  // This function returns the global track position
  //---------------------------------------------------------------------
  Double_t phi=TMath::ATan2(fCp[0],fCx) + fCalpha;
  Double_t r=TMath::Sqrt(fCx*fCx + fCp[0]*fCp[0]);
  xyz[0]=r*TMath::Cos(phi); xyz[1]=r*TMath::Sin(phi); xyz[2]=fCp[1]; 
}

void AliESDtrack::GetPxPyPz(Double_t *p) const {
  //---------------------------------------------------------------------
  // This function returns the global track momentum components
  //---------------------------------------------------------------------
  if (TMath::Abs(fRp[4])<=0) {
    p[0]=p[1]=p[2]=0;
    return;
  }
  Double_t phi=TMath::ASin(fRp[2]) + fRalpha;
  Double_t pt=1./TMath::Abs(fRp[4]);
  p[0]=pt*TMath::Cos(phi); p[1]=pt*TMath::Sin(phi); p[2]=pt*fRp[3]; 
}
void AliESDtrack::GetXYZ(Double_t *xyz) const {
  //---------------------------------------------------------------------
  // This function returns the global track position
  //---------------------------------------------------------------------
  Double_t phi=TMath::ATan2(fRp[0],fRx) + fRalpha;
  Double_t r=TMath::Sqrt(fRx*fRx + fRp[0]*fRp[0]);
  xyz[0]=r*TMath::Cos(phi); xyz[1]=r*TMath::Sin(phi); xyz[2]=fRp[1]; 
}


void AliESDtrack::GetInnerPxPyPz(Double_t *p) const {
  //---------------------------------------------------------------------
  // This function returns the global track momentum components
  // af the entrance of the TPC
  //---------------------------------------------------------------------
  if (fIx==0) {p[0]=p[1]=p[2]=0.; return;}
  Double_t phi=TMath::ASin(fIp[2]) + fIalpha;
  Double_t pt=1./TMath::Abs(fIp[4]);
  p[0]=pt*TMath::Cos(phi); p[1]=pt*TMath::Sin(phi); p[2]=pt*fIp[3]; 
}

void AliESDtrack::GetInnerXYZ(Double_t *xyz) const {
  //---------------------------------------------------------------------
  // This function returns the global track position
  // af the entrance of the TPC
  //---------------------------------------------------------------------
  if (fIx==0) {xyz[0]=xyz[1]=xyz[2]=0.; return;}
  Double_t phi=TMath::ATan2(fIp[0],fIx) + fIalpha;
  Double_t r=TMath::Sqrt(fIx*fIx + fIp[0]*fIp[0]);
  xyz[0]=r*TMath::Cos(phi); xyz[1]=r*TMath::Sin(phi); xyz[2]=fIp[1]; 
}

void AliESDtrack::GetInnerExternalParameters(Double_t &x, Double_t p[5]) const 
{
  //skowron
 //---------------------------------------------------------------------
  // This function returns external representation of the track parameters at Inner Layer of TPC
  //---------------------------------------------------------------------
  x=fIx;
  for (Int_t i=0; i<5; i++) p[i]=fIp[i];
}
void AliESDtrack::GetInnerExternalCovariance(Double_t cov[15]) const
{
 //skowron
 //---------------------------------------------------------------------
 // This function returns external representation of the cov. matrix at Inner Layer of TPC
 //---------------------------------------------------------------------
 for (Int_t i=0; i<15; i++) cov[i]=fIc[i];
 
}

//_______________________________________________________________________
void AliESDtrack::GetOuterPxPyPzPHOS(Double_t *p) const {
  //---------------------------------------------------------------------
  // This function returns the global track momentum components
  // af the radius of the PHOS
  //---------------------------------------------------------------------
  p[0]=p[1]=p[2]=0. ; 
  if (fOx==0) 
    return;
  Double_t phi=TMath::ASin(fOp[2]) + fOalpha;
  Double_t pt=1./TMath::Abs(fOp[4]);
  p[0]=pt*TMath::Cos(phi); 
  p[1]=pt*TMath::Sin(phi); 
  p[2]=pt*fOp[3];
} 

//_______________________________________________________________________
void AliESDtrack::GetOuterPxPyPzEMCAL(Double_t *p) const {
  //---------------------------------------------------------------------
  // This function returns the global track momentum components
  // af the radius of the EMCAL
  //---------------------------------------------------------------------
  p[0]=p[1]=p[2]=0. ; 
  if (fXx==0)
    return;
  Double_t phi=TMath::ASin(fXp[2]) + fXalpha;
  Double_t pt=1./TMath::Abs(fXp[4]);
  p[0]=pt*TMath::Cos(phi); 
  p[1]=pt*TMath::Sin(phi); 
  p[2]=pt*fXp[3];
}

//_______________________________________________________________________
void AliESDtrack::GetOuterXYZPHOS(Double_t *xyz) const {
  //---------------------------------------------------------------------
  // This function returns the global track position
  // af the radius of the PHOS
  //---------------------------------------------------------------------
  xyz[0]=xyz[1]=xyz[2]=0.;
  if (fOx==0) 
    return;
  Double_t phi=TMath::ATan2(fOp[0],fOx) + fOalpha;
  Double_t r=TMath::Sqrt(fOx*fOx + fOp[0]*fOp[0]);
  xyz[0]=r*TMath::Cos(phi); xyz[1]=r*TMath::Sin(phi); xyz[2]=fOp[1]; 
} 

//_______________________________________________________________________
void AliESDtrack::GetOuterXYZEMCAL(Double_t *xyz) const {
  //---------------------------------------------------------------------
  // This function returns the global track position
  // af the radius of the PHOS
  //---------------------------------------------------------------------
  xyz[0]=xyz[1]=xyz[2]=0.;
  if (fXx==0) 
    return;
  Double_t phi=TMath::ATan2(fXp[0],fOx) + fXalpha;
  Double_t r=TMath::Sqrt(fXx*fXx + fXp[0]*fXp[0]);
  xyz[0]=r*TMath::Cos(phi); 
  xyz[1]=r*TMath::Sin(phi); 
  xyz[2]=fXp[1]; 
} 

//_______________________________________________________________________
void AliESDtrack::GetIntegratedTimes(Double_t *times) const {
  // Returns the array with integrated times for each particle hypothesis
  for (Int_t i=0; i<kSPECIES; i++) times[i]=fTrackTime[i];
}

//_______________________________________________________________________
void AliESDtrack::SetIntegratedTimes(const Double_t *times) {
  // Sets the array with integrated times for each particle hypotesis
  for (Int_t i=0; i<kSPECIES; i++) fTrackTime[i]=times[i];
}

//_______________________________________________________________________
void AliESDtrack::SetITSpid(const Double_t *p) {
  // Sets values for the probability of each particle type (in ITS)
  for (Int_t i=0; i<kSPECIES; i++) fITSr[i]=p[i];
  SetStatus(AliESDtrack::kITSpid);
}

void AliESDtrack::SetITSChi2MIP(const Float_t *chi2mip){
  for (Int_t i=0; i<6; i++) fITSchi2MIP[i]=chi2mip[i];
}
//_______________________________________________________________________
void AliESDtrack::GetITSpid(Double_t *p) const {
  // Gets the probability of each particle type (in ITS)
  for (Int_t i=0; i<kSPECIES; i++) p[i]=fITSr[i];
}

//_______________________________________________________________________
Int_t AliESDtrack::GetITSclusters(UInt_t *idx) const {
  //---------------------------------------------------------------------
  // This function returns indices of the assgined ITS clusters 
  //---------------------------------------------------------------------
  for (Int_t i=0; i<fITSncls; i++) idx[i]=fITSindex[i];
  return fITSncls;
}

//_______________________________________________________________________
Int_t AliESDtrack::GetTPCclusters(Int_t *idx) const {
  //---------------------------------------------------------------------
  // This function returns indices of the assgined ITS clusters 
  //---------------------------------------------------------------------
  if (idx!=0)
    for (Int_t i=0; i<180; i++) idx[i]=fTPCindex[i];  // MI I prefer some constant
  return fTPCncls;
}

//_______________________________________________________________________
void AliESDtrack::SetTPCpid(const Double_t *p) {  
  // Sets values for the probability of each particle type (in TPC)
  for (Int_t i=0; i<kSPECIES; i++) fTPCr[i]=p[i];
  SetStatus(AliESDtrack::kTPCpid);
}

//_______________________________________________________________________
void AliESDtrack::GetTPCpid(Double_t *p) const {
  // Gets the probability of each particle type (in TPC)
  for (Int_t i=0; i<kSPECIES; i++) p[i]=fTPCr[i];
}

//_______________________________________________________________________
Int_t AliESDtrack::GetTRDclusters(UInt_t *idx) const {
  //---------------------------------------------------------------------
  // This function returns indices of the assgined TRD clusters 
  //---------------------------------------------------------------------
  if (idx!=0)
    for (Int_t i=0; i<90; i++) idx[i]=fTRDindex[i];  // MI I prefer some constant
  return fTRDncls;
}

//_______________________________________________________________________
void AliESDtrack::SetTRDpid(const Double_t *p) {  
  // Sets values for the probability of each particle type (in TRD)
  for (Int_t i=0; i<kSPECIES; i++) fTRDr[i]=p[i];
  SetStatus(AliESDtrack::kTRDpid);
}

//_______________________________________________________________________
void AliESDtrack::GetTRDpid(Double_t *p) const {
  // Gets the probability of each particle type (in TRD)
  for (Int_t i=0; i<kSPECIES; i++) p[i]=fTRDr[i];
}

//_______________________________________________________________________
void    AliESDtrack::SetTRDpid(Int_t iSpecies, Float_t p)
{
  // Sets the probability of particle type iSpecies to p (in TRD)
  fTRDr[iSpecies] = p;
}

Float_t AliESDtrack::GetTRDpid(Int_t iSpecies) const
{
  // Returns the probability of particle type iSpecies (in TRD)
  return fTRDr[iSpecies];
}

//_______________________________________________________________________
void AliESDtrack::SetTOFpid(const Double_t *p) {  
  // Sets the probability of each particle type (in TOF)
  for (Int_t i=0; i<kSPECIES; i++) fTOFr[i]=p[i];
  SetStatus(AliESDtrack::kTOFpid);
}

//_______________________________________________________________________
void AliESDtrack::GetTOFpid(Double_t *p) const {
  // Gets probabilities of each particle type (in TOF)
  for (Int_t i=0; i<kSPECIES; i++) p[i]=fTOFr[i];
}



//_______________________________________________________________________
void AliESDtrack::SetPHOSpid(const Double_t *p) {  
  // Sets the probability of each particle type (in PHOS)
  for (Int_t i=0; i<kSPECIESN; i++) fPHOSr[i]=p[i];
  SetStatus(AliESDtrack::kPHOSpid);
}

//_______________________________________________________________________
void AliESDtrack::GetPHOSpid(Double_t *p) const {
  // Gets probabilities of each particle type (in PHOS)
  for (Int_t i=0; i<kSPECIESN; i++) p[i]=fPHOSr[i];
}

//_______________________________________________________________________
void AliESDtrack::SetEMCALpid(const Double_t *p) {  
  // Sets the probability of each particle type (in EMCAL)
  for (Int_t i=0; i<kSPECIESN; i++) fEMCALr[i]=p[i];
  SetStatus(AliESDtrack::kEMCALpid);
}

//_______________________________________________________________________
void AliESDtrack::GetEMCALpid(Double_t *p) const {
  // Gets probabilities of each particle type (in EMCAL)
  for (Int_t i=0; i<kSPECIESN; i++) p[i]=fEMCALr[i];
}

//_______________________________________________________________________
void AliESDtrack::SetRICHpid(const Double_t *p) {  
  // Sets the probability of each particle type (in RICH)
  for (Int_t i=0; i<kSPECIES; i++) fRICHr[i]=p[i];
  SetStatus(AliESDtrack::kRICHpid);
}

//_______________________________________________________________________
void AliESDtrack::GetRICHpid(Double_t *p) const {
  // Gets probabilities of each particle type (in RICH)
  for (Int_t i=0; i<kSPECIES; i++) p[i]=fRICHr[i];
}



//_______________________________________________________________________
void AliESDtrack::SetESDpid(const Double_t *p) {  
  // Sets the probability of each particle type for the ESD track
  for (Int_t i=0; i<kSPECIES; i++) fR[i]=p[i];
  SetStatus(AliESDtrack::kESDpid);
}

//_______________________________________________________________________
void AliESDtrack::GetESDpid(Double_t *p) const {
  // Gets probability of each particle type for the ESD track
  for (Int_t i=0; i<kSPECIES; i++) p[i]=fR[i];
}

//_______________________________________________________________________
void AliESDtrack::Print(Option_t *) const {
  // Prints info on the track
  
  Info("Print","Track info") ; 
  Double_t p[kSPECIESN] ; 
  Int_t index = 0 ; 
  if( IsOn(kITSpid) ){
    printf("From ITS: ") ; 
    GetITSpid(p) ; 
    for(index = 0 ; index < kSPECIES; index++) 
      printf("%f, ", p[index]) ;
    printf("\n           signal = %f\n", GetITSsignal()) ;
  } 
  if( IsOn(kTPCpid) ){
    printf("From TPC: ") ; 
    GetTPCpid(p) ; 
    for(index = 0 ; index < kSPECIES; index++) 
      printf("%f, ", p[index]) ;
    printf("\n           signal = %f\n", GetTPCsignal()) ;
  }
  if( IsOn(kTRDpid) ){
    printf("From TRD: ") ; 
    GetTRDpid(p) ; 
    for(index = 0 ; index < kSPECIES; index++) 
      printf("%f, ", p[index]) ;
    printf("\n           signal = %f\n", GetTRDsignal()) ;
  }
  if( IsOn(kTOFpid) ){
    printf("From TOF: ") ; 
    GetTOFpid(p) ; 
    for(index = 0 ; index < kSPECIES; index++) 
      printf("%f, ", p[index]) ;
    printf("\n           signal = %f\n", GetTOFsignal()) ;
  }
  if( IsOn(kRICHpid) ){
    printf("From TOF: ") ; 
    GetRICHpid(p) ; 
    for(index = 0 ; index < kSPECIES; index++) 
      printf("%f, ", p[index]) ;
    printf("\n           signal = %f\n", GetRICHsignal()) ;
  }
  if( IsOn(kPHOSpid) ){
    printf("From PHOS: ") ; 
    GetPHOSpid(p) ; 
    for(index = 0 ; index < kSPECIESN; index++) 
      printf("%f, ", p[index]) ;
    printf("\n           signal = %f\n", GetPHOSsignal()) ;
  }
  if( IsOn(kEMCALpid) ){
    printf("From EMCAL: ") ; 
    GetEMCALpid(p) ; 
    for(index = 0 ; index < kSPECIESN; index++) 
      printf("%f, ", p[index]) ;
    printf("\n           signal = %f\n", GetEMCALsignal()) ;
  }
} 
