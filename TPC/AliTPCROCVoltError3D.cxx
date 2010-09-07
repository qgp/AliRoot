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

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// AliTPCROCVoltError3D class                                               //
// The class calculates the space point distortions due to residual voltage //
// errors on Read Out Chambers of the TPC in 3D.                            //
//                                                                          //
// The class allows "effective Omega Tau" corrections.                      // 
//                                                                          //
// NOTE: This class is capable of calculating z distortions due to          //
//       misalignment and the vd dependency on the residual drift field     //
//                                                                          //
// date: 08/08/2010                                                         //
// Authors: Jim Thomas, Stefan Rossegger                                    //
//                                                                          //
// Example usage :                                                          //
//  AliTPCROCVoltError3D ROCerror;                                            //
//////////////////////////////////////////////////////////////////////////////

#include "AliMagF.h"
#include "TGeoGlobalMagField.h"
#include "AliTPCcalibDB.h"
#include "AliTPCParam.h"
#include "AliLog.h"
#include "TMatrixD.h"
#include "TFile.h"

#include "TMath.h"
#include "AliTPCROC.h"
#include "AliTPCROCVoltError3D.h"

ClassImp(AliTPCROCVoltError3D)

AliTPCROCVoltError3D::AliTPCROCVoltError3D()
  : AliTPCCorrection("ROCVoltErrors","ROC z alignment Errors"),
    fC0(0.),fC1(0.),
    fROCdisplacement(kTRUE),
    fInitLookUp(kFALSE),
    fROCDataFileName("$(ALICE_ROOT)/TPC/Calib/maps/TPCROCdzSurvey.root"),  // standard file name of ROC survey
    fdzDataLinFit(0)
{
  //
  // default constructor
  //

  // Array which will contain the solution according to the setted boundary conditions
  // main input: z alignment of the Read Out chambers
  // see InitROCVoltError3D() function
  for ( Int_t k = 0 ; k < kNPhi ; k++ ) {
    fLookUpErOverEz[k]   =  new TMatrixD(kNR,kNZ);  
    fLookUpEphiOverEz[k] =  new TMatrixD(kNR,kNZ);
    fLookUpDeltaEz[k]    =  new TMatrixD(kNR,kNZ);   
  }

  SetROCDataFileName(fROCDataFileName); // initialization of fdzDataLinFit is included

}

AliTPCROCVoltError3D::~AliTPCROCVoltError3D() {
  //
  // destructor
  //
  
  for ( Int_t k = 0 ; k < kNPhi ; k++ ) {
    delete fLookUpErOverEz[k];
    delete fLookUpEphiOverEz[k];
    delete fLookUpDeltaEz[k];
  }

  delete fdzDataLinFit;
}

void AliTPCROCVoltError3D::SetDZMap(TMatrixD * matrix){
  //
  //
  //
  if (!fdzDataLinFit) fdzDataLinFit=new TMatrixD(*matrix);
  else *fdzDataLinFit = *matrix;
}


void AliTPCROCVoltError3D::Init() {
  //
  // Initialization funtion
  //
  
  AliMagF* magF= (AliMagF*)TGeoGlobalMagField::Instance()->GetField();
  if (!magF) AliError("Magneticd field - not initialized");
  Double_t bzField = magF->SolenoidField()/10.; //field in T
  AliTPCParam *param= AliTPCcalibDB::Instance()->GetParameters();
  if (!param) AliError("Parameters - not initialized");
  Double_t vdrift = param->GetDriftV()/1000000.; // [cm/us]   // From dataBase: to be updated: per second (ideally)
  Double_t ezField = 400; // [V/cm]   // to be updated: never (hopefully)
  Double_t wt = -10.0 * (bzField*10) * vdrift / ezField ; 
  // Correction Terms for effective omegaTau; obtained by a laser calibration run
  SetOmegaTauT1T2(wt,fT1,fT2);

  InitROCVoltError3D();
}

void AliTPCROCVoltError3D::Update(const TTimeStamp &/*timeStamp*/) {
  //
  // Update function 
  //
  AliMagF* magF= (AliMagF*)TGeoGlobalMagField::Instance()->GetField();
  if (!magF) AliError("Magneticd field - not initialized");
  Double_t bzField = magF->SolenoidField()/10.; //field in T
  AliTPCParam *param= AliTPCcalibDB::Instance()->GetParameters();
  if (!param) AliError("Parameters - not initialized");
  Double_t vdrift = param->GetDriftV()/1000000.; // [cm/us]  // From dataBase: to be updated: per second (ideally)
  Double_t ezField = 400; // [V/cm]   // to be updated: never (hopefully)
  Double_t wt = -10.0 * (bzField*10) * vdrift / ezField ; 
  // Correction Terms for effective omegaTau; obtained by a laser calibration run
  SetOmegaTauT1T2(wt,fT1,fT2);

}

void  AliTPCROCVoltError3D::SetROCDataFileName(char *const fname) {
  //
  // Set / load the ROC data (linear fit of ROC misalignments)
  //

  fROCDataFileName = fname;
  
  TFile f(fROCDataFileName,"READ");
  TMatrixD *m = (TMatrixD*) f.Get("dzSurveyLinFitData");
  TMatrixD &mf = *m;

  // prepare some space

  if (fdzDataLinFit) delete fdzDataLinFit;
  fdzDataLinFit = new TMatrixD(72,3);
  TMatrixD &dataIntern = *fdzDataLinFit;
  
  for (Int_t iroc=0;iroc<72;iroc++) {
    dataIntern(iroc,0) = mf(iroc,0);  // z0 offset
    dataIntern(iroc,1) = mf(iroc,1);  // slope in x
    dataIntern(iroc,2) = mf(iroc,2);  // slope in y
  }

  f.Close();

  fInitLookUp = kFALSE;

}

void AliTPCROCVoltError3D::GetCorrection(const Float_t x[],const Short_t roc,Float_t dx[]) {
  //
  // Calculates the correction due e.g. residual voltage errors on the TPC boundaries
  //   

  if (!fInitLookUp) {
    AliInfo("Lookup table was not initialized! Perform the inizialisation now ...");
    InitROCVoltError3D();
    return;
  }

  Int_t   order     = 1 ;               // FIXME: hardcoded? Linear interpolation = 1, Quadratic = 2         

  Double_t intEr, intEphi, intDeltaEz;
  Double_t r, phi, z ;
  Int_t    sign;

  r      =  TMath::Sqrt( x[0]*x[0] + x[1]*x[1] ) ;
  phi    =  TMath::ATan2(x[1],x[0]) ;
  if ( phi < 0 ) phi += TMath::TwoPi() ;                   // Table uses phi from 0 to 2*Pi
  z      =  x[2] ;                                         // Create temporary copy of x[2]

  if ( (roc%36) < 18 ) {
    sign =  1;       // (TPC A side)
  } else {
    sign = -1;       // (TPC C side)
  }
  
  if ( sign==1  && z <  fgkZOffSet ) z =  fgkZOffSet;    // Protect against discontinuity at CE
  if ( sign==-1 && z > -fgkZOffSet ) z = -fgkZOffSet;    // Protect against discontinuity at CE
  

  if ( (sign==1 && z<0) || (sign==-1 && z>0) ) // just a consistency check
    AliError("ROC number does not correspond to z coordinate! Calculation of distortions is most likely wrong!");

  // Get the Er and Ephi field integrals plus the integral over DeltaEz 
  intEr      = Interpolate3DTable(order, r, z, phi, kNR, kNZ, kNPhi, 
				  fgkRList, fgkZList, fgkPhiList, fLookUpErOverEz  );
  intEphi    = Interpolate3DTable(order, r, z, phi, kNR, kNZ, kNPhi, 
				  fgkRList, fgkZList, fgkPhiList, fLookUpEphiOverEz);
  intDeltaEz = Interpolate3DTable(order, r, z, phi, kNR, kNZ, kNPhi, 
				  fgkRList, fgkZList, fgkPhiList, fLookUpDeltaEz   );

  //  printf("%lf %lf %lf\n",intEr,intEphi,intDeltaEz);

  // Calculate distorted position
  if ( r > 0.0 ) {
    phi =  phi + ( fC0*intEphi - fC1*intEr ) / r;      
    r   =  r   + ( fC0*intEr   + fC1*intEphi );  
  }
  
  // Calculate correction in cartesian coordinates
  dx[0] = r * TMath::Cos(phi) - x[0];
  dx[1] = r * TMath::Sin(phi) - x[1]; 
  dx[2] = intDeltaEz;  // z distortion - (internally scaled with driftvelocity dependency 
                       // on the Ez field plus the actual ROC misalignment (if set TRUE)

}

void AliTPCROCVoltError3D::InitROCVoltError3D() {
  //
  // Initialization of the Lookup table which contains the solutions of the 
  // Dirichlet boundary problem
  // Calculation of the single 3D-Poisson solver is done just if needed
  // (see basic lookup tables in header file)
  //

  const Int_t   order       =    1  ;  // Linear interpolation = 1, Quadratic = 2  
  const Float_t gridSizeR   =  (fgkOFCRadius-fgkIFCRadius) / (kRows-1) ;
  const Float_t gridSizeZ   =  fgkTPCZ0 / (kColumns-1) ;
  const Float_t gridSizePhi =  TMath::TwoPi() / ( 18.0 * kPhiSlicesPerSector);

  // temporary arrays to create the boundary conditions
  TMatrixD *arrayofArrayV[kPhiSlices], *arrayofCharge[kPhiSlices] ; 
  TMatrixD *arrayofEroverEz[kPhiSlices], *arrayofEphioverEz[kPhiSlices], *arrayofDeltaEz[kPhiSlices] ; 

  for ( Int_t k = 0 ; k < kPhiSlices ; k++ ) {
    arrayofArrayV[k]     =   new TMatrixD(kRows,kColumns) ;
    arrayofCharge[k]     =   new TMatrixD(kRows,kColumns) ;
    arrayofEroverEz[k]   =   new TMatrixD(kRows,kColumns) ;
    arrayofEphioverEz[k] =   new TMatrixD(kRows,kColumns) ;
    arrayofDeltaEz[k]    =   new TMatrixD(kRows,kColumns) ;
  }
  
  // list of point as used in the poisson relation and the interpolation (during sum up)
  Double_t  rlist[kRows], zedlist[kColumns] , philist[kPhiSlices];
  for ( Int_t k = 0 ; k < kPhiSlices ; k++ ) {
    philist[k] =  gridSizePhi * k;
    for ( Int_t i = 0 ; i < kRows ; i++ )    {
      rlist[i] = fgkIFCRadius + i*gridSizeR ;
      for ( Int_t j = 0 ; j < kColumns ; j++ ) { // Fill Vmatrix with Boundary Conditions
	zedlist[j]  = j * gridSizeZ ;
      }
    }
  }

  // ==========================================================================
  // Solve Poisson's equation in 3D cylindrical coordinates by relaxation technique
  // Allow for different size grid spacing in R and Z directions
  
  const Int_t   symmetry = 0;
 
  // Set bondaries and solve Poisson's equation --------------------------
  
  if ( !fInitLookUp ) {
    
    AliInfo(Form("Solving the poisson equation (~ %d sec)",2*10*(int)(kPhiSlices/10)));
    
    for ( Int_t side = 0 ; side < 2 ; side++ ) {  // Solve Poisson3D twice; once for +Z and once for -Z
      
      for ( Int_t k = 0 ; k < kPhiSlices ; k++ )  {
	TMatrixD &arrayV    =  *arrayofArrayV[k] ;
	TMatrixD &charge    =  *arrayofCharge[k] ;
	
	//Fill arrays with initial conditions.  V on the boundary and Charge in the volume.
	for ( Int_t i = 0 ; i < kRows ; i++ ) {
	  for ( Int_t j = 0 ; j < kColumns ; j++ ) {  // Fill Vmatrix with Boundary Conditions
	    arrayV(i,j) = 0.0 ; 
	    charge(i,j) = 0.0 ;

	    Float_t radius0 = rlist[i] ;
	    Float_t phi0    = gridSizePhi * k ;
	    
	    // To avoid problems at sector boundaries, use an average of +- 1 degree from actual phi location
	    if ( j == (kColumns-1) ) 
	      arrayV(i,j) = 0.5*  ( GetROCVoltOffset( side, radius0, phi0+0.02 ) + GetROCVoltOffset( side, radius0, phi0-0.02 ) ) ;

	  }
	}      
	
	for ( Int_t i = 1 ; i < kRows-1 ; i++ ) { 
	  for ( Int_t j = 1 ; j < kColumns-1 ; j++ ) {
	    charge(i,j)  =  0.0 ;
	  }
	}
      }      
      
      // Solve Poisson's equation in 3D cylindrical coordinates by relaxation technique
      // Allow for different size grid spacing in R and Z directions
      
      PoissonRelaxation3D( arrayofArrayV, arrayofCharge, 
			   arrayofEroverEz, arrayofEphioverEz, arrayofDeltaEz,
			   kRows, kColumns, kPhiSlices, gridSizePhi, kIterations, 
			   symmetry, fROCdisplacement) ;
      
      
      //Interpolate results onto a custom grid which is used just for these calculations.
      Double_t  r, phi, z ;
      for ( Int_t k = 0 ; k < kNPhi ; k++ ) {
	phi = fgkPhiList[k] ;
	
	TMatrixD &erOverEz   =  *fLookUpErOverEz[k]  ;
	TMatrixD &ephiOverEz =  *fLookUpEphiOverEz[k];
	TMatrixD &deltaEz    =  *fLookUpDeltaEz[k]   ;
	
	for ( Int_t j = 0 ; j < kNZ ; j++ ) {

	  z = TMath::Abs(fgkZList[j]) ;  // Symmetric solution in Z that depends only on ABS(Z)
  
	  if ( side == 0 &&  fgkZList[j] < 0 ) continue; // Skip rest of this loop if on the wrong side
	  if ( side == 1 &&  fgkZList[j] > 0 ) continue; // Skip rest of this loop if on the wrong side
	  
	  for ( Int_t i = 0 ; i < kNR ; i++ ) { 
	    r = fgkRList[i] ;

	    // Interpolate basicLookup tables; once for each rod, then sum the results
	    erOverEz(i,j)   = Interpolate3DTable(order, r, z, phi, kRows, kColumns, kPhiSlices, 
						 rlist, zedlist, philist, arrayofEroverEz  );
	    ephiOverEz(i,j) = Interpolate3DTable(order, r, z, phi, kRows, kColumns, kPhiSlices,
						 rlist, zedlist, philist, arrayofEphioverEz);
	    deltaEz(i,j)    = Interpolate3DTable(order, r, z, phi, kRows, kColumns, kPhiSlices,
						 rlist, zedlist, philist, arrayofDeltaEz  );

	    if (side == 1)  deltaEz(i,j) = -  deltaEz(i,j); // negative coordinate system on C side

	  } // end r loop
	}// end z loop
      }// end phi loop

      if ( side == 0 ) AliInfo(" A side done");
      if ( side == 1 ) AliInfo(" C side done");
    } // end side loop
  }
  
  // clear the temporary arrays lists
  for ( Int_t k = 0 ; k < kPhiSlices ; k++ )  {
    delete arrayofArrayV[k];
    delete arrayofCharge[k];
    delete arrayofEroverEz[k];  
    delete arrayofEphioverEz[k];
    delete arrayofDeltaEz[k];
  }
 

  fInitLookUp = kTRUE;

}


Float_t AliTPCROCVoltError3D::GetROCVoltOffset(Int_t side, Float_t r0, Float_t phi0) {
  // 
  // Returns the dz alignment data (in voltage equivalents) at 
  // the given position
  //

  Float_t xp = r0*TMath::Cos(phi0);
  Float_t yp = r0*TMath::Sin(phi0);
  
  // phi0 should be between 0 and 2pi 
  if (phi0<0) phi0+=TMath::TwoPi();
  Int_t roc = (Int_t)TMath::Floor((TMath::RadToDeg()*phi0)/20);
  if (side==1) roc+=18; // C side
  if (r0>132) roc+=36;  // OROC 
  
  // linear-plane data:  z = z0 + kx*x + ky*y
  TMatrixD &fitData = *fdzDataLinFit;
  Float_t dz = fitData(roc,0)+fitData(roc,1)*xp + fitData(roc,2)*yp; // value in cm

  // aproximated Voltage-offset-aquivalent to the z misalignment
  // (linearly scaled with the z position)
  Double_t ezField = (fgkCathodeV-fgkGG)/fgkTPCZ0; // = ALICE Electric Field (V/cm) Magnitude ~ -400 V/cm; 
  Float_t voltOff = dz*ezField;            // values in "Volt equivalents"

  return voltOff;
}

TH2F * AliTPCROCVoltError3D::CreateHistoOfZSurvey(Int_t side, Int_t nx, Int_t ny) {
  //
  // return a simple histogramm containing the input to the poisson solver
  // (z positions of the Read-out chambers, linearly interpolated)

  char hname[100];
  if (side==0) sprintf(hname,"survey_dz_Aside");
  if (side==1) sprintf(hname,"survey_dz_Cside");

  TH2F *h = new TH2F(hname,hname,nx,-250.,250.,ny,-250.,250.);

  for (Int_t iy=1;iy<=ny;++iy) {
    Double_t yp = h->GetYaxis()->GetBinCenter(iy);
    for (Int_t ix=1;ix<=nx;++ix) {
      Double_t xp = h->GetXaxis()->GetBinCenter(ix);
    
      Float_t r0 = TMath::Sqrt(xp*xp+yp*yp);
      Float_t phi0 = TMath::ATan2(yp,xp); 
   
      Float_t dz = GetROCVoltOffset(side,r0,phi0); // in [volt]

      Double_t ezField = (fgkCathodeV-fgkGG)/fgkTPCZ0; // = ALICE Electric Field (V/cm) Magnitude ~ -400 V/cm; 
      dz = dz/ezField;    // in [cm]

      if (85.<=r0 && r0<=245.) {
	h->SetBinContent(ix,iy,dz); 
      } else {
	h->SetBinContent(ix,iy,0.);
      }
    }
  }
  
  h->GetXaxis()->SetTitle("x [cm]");
  h->GetYaxis()->SetTitle("y [cm]");
  h->GetZaxis()->SetTitle("dz [cm]");
  h->SetStats(0);
  //  h->DrawCopy("colz");

  return h;
} 

void AliTPCROCVoltError3D::Print(const Option_t* option) const {
  //
  // Print function to check the settings of the Rod shifts and the rotated clips
  // option=="a" prints the C0 and C1 coefficents for calibration purposes
  //

  TString opt = option; opt.ToLower();
  printf("%s\n",GetTitle());
  printf(" - Voltage settings on the TPC Read-Out chambers - linearly interpolated\n");
  printf("   info: Check the following data-file for more details: %s \n",fROCDataFileName);

  if (opt.Contains("a")) { // Print all details
    printf(" - T1: %1.4f, T2: %1.4f \n",fT1,fT2);
    printf(" - C1: %1.4f, C0: %1.4f \n",fC1,fC0);
  }

  if (!fInitLookUp) AliError("Lookup table was not initialized! You should do InitROCVoltError3D() ...");

}
