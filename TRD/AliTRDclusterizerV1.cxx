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

/*
$Log$
Revision 1.17  2002/06/12 09:54:35  cblume
Update of tracking code provided by Sergei

Revision 1.16  2002/03/25 20:01:30  cblume
Introduce parameter class

Revision 1.15  2001/11/14 12:09:11  cblume
Use correct name for digitizer

Revision 1.14  2001/11/14 10:50:45  cblume
Changes in digits IO. Add merging of summable digits

Revision 1.13  2001/05/28 17:07:58  hristov
Last minute changes; ExB correction in AliTRDclusterizerV1; taking into account of material in G10 TEC frames and material between TEC planes (C.Blume,S.Sedykh)

Revision 1.12  2001/05/21 17:42:58  hristov
Constant casted to avoid the ambiguity

Revision 1.11  2001/05/21 16:45:47  hristov
Last minute changes (C.Blume)

Revision 1.10  2001/05/07 08:06:44  cblume
Speedup of the code. Create only AliTRDcluster

Revision 1.9  2000/11/01 14:53:20  cblume
Merge with TRD-develop

Revision 1.1.4.5  2000/10/15 23:40:01  cblume
Remove AliTRDconst

Revision 1.1.4.4  2000/10/06 16:49:46  cblume
Made Getters const

Revision 1.1.4.3  2000/10/04 16:34:58  cblume
Replace include files by forward declarations

Revision 1.1.4.2  2000/09/22 14:49:49  cblume
Adapted to tracking code

Revision 1.8  2000/10/02 21:28:19  fca
Removal of useless dependecies via forward declarations

Revision 1.7  2000/06/27 13:08:50  cblume
Changed to Copy(TObject &A) to appease the HP-compiler

Revision 1.6  2000/06/09 11:10:07  cblume
Compiler warnings and coding conventions, next round

Revision 1.5  2000/06/08 18:32:58  cblume
Make code compliant to coding conventions

Revision 1.4  2000/06/07 16:27:01  cblume
Try to remove compiler warnings on Sun and HP

Revision 1.3  2000/05/08 16:17:27  cblume
Merge TRD-develop

Revision 1.1.4.1  2000/05/08 15:09:01  cblume
Introduce AliTRDdigitsManager

Revision 1.1  2000/02/28 18:58:54  cblume
Add new TRD classes

*/

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// TRD cluster finder for the slow simulator. 
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <TF1.h>
#include <TTree.h>
#include <TH1.h>
#include <TFile.h>

#include "AliRun.h"

#include "AliTRD.h"
#include "AliTRDclusterizerV1.h"
#include "AliTRDmatrix.h"
#include "AliTRDgeometry.h"
#include "AliTRDdigitizer.h"
#include "AliTRDdataArrayF.h"
#include "AliTRDdataArrayI.h"
#include "AliTRDdigitsManager.h"
#include "AliTRDparameter.h"

ClassImp(AliTRDclusterizerV1)

//_____________________________________________________________________________
AliTRDclusterizerV1::AliTRDclusterizerV1():AliTRDclusterizer()
{
  //
  // AliTRDclusterizerV1 default constructor
  //

  fDigitsManager = 0;

}

//_____________________________________________________________________________
AliTRDclusterizerV1::AliTRDclusterizerV1(const Text_t* name, const Text_t* title)
                    :AliTRDclusterizer(name,title)
{
  //
  // AliTRDclusterizerV1 default constructor
  //

  fDigitsManager = new AliTRDdigitsManager();
  fDigitsManager->CreateArrays();

}

//_____________________________________________________________________________
AliTRDclusterizerV1::AliTRDclusterizerV1(const AliTRDclusterizerV1 &c)
{
  //
  // AliTRDclusterizerV1 copy constructor
  //

  ((AliTRDclusterizerV1 &) c).Copy(*this);

}

//_____________________________________________________________________________
AliTRDclusterizerV1::~AliTRDclusterizerV1()
{
  //
  // AliTRDclusterizerV1 destructor
  //

  if (fDigitsManager) {
    delete fDigitsManager;
    fDigitsManager = NULL;
  }

}

//_____________________________________________________________________________
AliTRDclusterizerV1 &AliTRDclusterizerV1::operator=(const AliTRDclusterizerV1 &c)
{
  //
  // Assignment operator
  //

  if (this != &c) ((AliTRDclusterizerV1 &) c).Copy(*this);
  return *this;

}

//_____________________________________________________________________________
void AliTRDclusterizerV1::Copy(TObject &c)
{
  //
  // Copy function
  //

  ((AliTRDclusterizerV1 &) c).fDigitsManager = 0;

  AliTRDclusterizer::Copy(c);

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizerV1::ReadDigits()
{
  //
  // Reads the digits arrays from the input aliroot file
  //

  if (!fInputFile) {
    printf("<AliTRDclusterizerV1::ReadDigits> ");
    printf("No input file open\n");
    return kFALSE;
  }

  fDigitsManager->Open(fInputFile->GetName());

  // Read in the digit arrays
  return (fDigitsManager->ReadDigits());  

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizerV1::MakeClusters()
{
  //
  // Generates the cluster.
  //

  Int_t row, col, time;

  if (fTRD->IsVersion() != 1) {
    printf("<AliTRDclusterizerV1::MakeCluster> ");
    printf("TRD must be version 1 (slow simulator).\n");
    return kFALSE; 
  }

  // Get the geometry
  AliTRDgeometry *geo = fTRD->GetGeometry();

  // Create a default parameter class if none is defined
  if (!fPar) {
    fPar = new AliTRDparameter("TRDparameter","Standard TRD parameter");
    printf("<AliTRDclusterizerV1::MakeCluster> ");
    printf("Create the default parameter object.\n");
  }

  Float_t timeBinSize = fPar->GetTimeBinSize();
  // Half of ampl.region
  const Float_t kAmWidth = AliTRDgeometry::AmThick()/2.; 

  Float_t omegaTau = fPar->GetOmegaTau();
  if (fVerbose > 0) {
    printf("<AliTRDclusterizerV1::MakeCluster> ");
    printf("OmegaTau = %f \n",omegaTau);
    printf("<AliTRDclusterizerV1::MakeCluster> ");
    printf("Start creating clusters.\n");
  } 

  AliTRDdataArrayI *digits;
  AliTRDdataArrayI *track0;
  AliTRDdataArrayI *track1;
  AliTRDdataArrayI *track2; 

  // Threshold value for the maximum
  Int_t maxThresh = fPar->GetClusMaxThresh();   
  // Threshold value for the digit signal
  Int_t sigThresh = fPar->GetClusSigThresh();   

  // Iteration limit for unfolding procedure
  const Float_t kEpsilon = 0.01;             

  const Int_t   kNclus   = 3;  
  const Int_t   kNsig    = 5;
  const Int_t   kNtrack  = 3 * kNclus;

  Int_t   iType          = 0;
  Int_t   iUnfold        = 0;

  Float_t ratioLeft      = 1.0;
  Float_t ratioRight     = 1.0;

  Float_t padSignal[kNsig];   
  Float_t clusterSignal[kNclus];
  Float_t clusterPads[kNclus];   
  Int_t   clusterDigit[kNclus];
  Int_t   clusterTracks[kNtrack];   

  Int_t chamBeg = 0;
  Int_t chamEnd = AliTRDgeometry::Ncham();
  if (fTRD->GetSensChamber()  >= 0) {
    chamBeg = fTRD->GetSensChamber();
    chamEnd = chamBeg + 1;
  }
  Int_t planBeg = 0;
  Int_t planEnd = AliTRDgeometry::Nplan();
  if (fTRD->GetSensPlane()    >= 0) {
    planBeg = fTRD->GetSensPlane();
    planEnd = planBeg + 1;
  }
  Int_t sectBeg = 0;
  Int_t sectEnd = AliTRDgeometry::Nsect();

  // Start clustering in every chamber
  for (Int_t icham = chamBeg; icham < chamEnd; icham++) {
    for (Int_t iplan = planBeg; iplan < planEnd; iplan++) {
      for (Int_t isect = sectBeg; isect < sectEnd; isect++) {

        if (fTRD->GetSensSector() >= 0) {
          Int_t sens1 = fTRD->GetSensSector();
          Int_t sens2 = sens1 + fTRD->GetSensSectorRange();
          sens2 -= ((Int_t) (sens2 / AliTRDgeometry::Nsect())) 
                 * AliTRDgeometry::Nsect();
          if (sens1 < sens2) {
            if ((isect < sens1) || (isect >= sens2)) continue;
	  }
          else {
            if ((isect < sens1) && (isect >= sens2)) continue;
	  }
	}

        Int_t idet = geo->GetDetector(iplan,icham,isect);

        Int_t nClusters      = 0;
        Int_t nClusters2pad  = 0;
        Int_t nClusters3pad  = 0;
        Int_t nClusters4pad  = 0;
        Int_t nClusters5pad  = 0;
        Int_t nClustersLarge = 0;

        if (fVerbose > 0) {
          printf("<AliTRDclusterizerV1::MakeCluster> ");
          printf("Analyzing chamber %d, plane %d, sector %d.\n"
                ,icham,iplan,isect);
	}

        Int_t   nRowMax     = fPar->GetRowMax(iplan,icham,isect);
        Int_t   nColMax     = fPar->GetColMax(iplan);
        Int_t   nTimeBefore = fPar->GetTimeBefore();
        Int_t   nTimeTotal  = fPar->GetTimeTotal();  

        Float_t row0        = fPar->GetRow0(iplan,icham,isect);
        Float_t col0        = fPar->GetCol0(iplan);
        Float_t rowSize     = fPar->GetRowPadSize(iplan,icham,isect);
        Float_t colSize     = fPar->GetColPadSize(iplan);

        // Get the digits
        digits = fDigitsManager->GetDigits(idet);
        digits->Expand();
        track0 = fDigitsManager->GetDictionary(idet,0);
        track0->Expand();
        track1 = fDigitsManager->GetDictionary(idet,1);
        track1->Expand();
        track2 = fDigitsManager->GetDictionary(idet,2); 
        track2->Expand();

        // Loop through the chamber and find the maxima 
        for ( row = 0;  row <  nRowMax;    row++) {
          for ( col = 2;  col <  nColMax;    col++) {
            for (time = 0; time < nTimeTotal; time++) {

              Int_t signalL = TMath::Abs(digits->GetDataUnchecked(row,col  ,time));
              Int_t signalM = TMath::Abs(digits->GetDataUnchecked(row,col-1,time));
              Int_t signalR = TMath::Abs(digits->GetDataUnchecked(row,col-2,time));
 
	      // Look for the maximum
              if (signalM >= maxThresh) {
                if (((signalL >= sigThresh) &&
                     (signalL <  signalM))  ||
                    ((signalR >= sigThresh) &&
                     (signalR <  signalM))) {
                  // Maximum found, mark the position by a negative signal
                  digits->SetDataUnchecked(row,col-1,time,-signalM);
		}
	      }

            }  
          }    
        }      

        // Now check the maxima and calculate the cluster position
        for ( row = 0;  row <  nRowMax  ;  row++) {
          for (time = 0; time < nTimeTotal; time++) {
            for ( col = 1;  col <  nColMax-1;  col++) {

              // Maximum found ?             
              if (digits->GetDataUnchecked(row,col,time) < 0) {

                Int_t iPad;
                for (iPad = 0; iPad < kNclus; iPad++) {
                  Int_t iPadCol = col - 1 + iPad;
                  clusterSignal[iPad]     = TMath::Abs(digits->GetDataUnchecked(row
                                                                               ,iPadCol
                                                                               ,time));
                  clusterDigit[iPad]      = digits->GetIndexUnchecked(row,iPadCol,time);
                  clusterTracks[3*iPad  ] = track0->GetDataUnchecked(row,iPadCol,time) - 1;
		  clusterTracks[3*iPad+1] = track1->GetDataUnchecked(row,iPadCol,time) - 1;
		  clusterTracks[3*iPad+2] = track2->GetDataUnchecked(row,iPadCol,time) - 1;
                }

		// Count the number of pads in the cluster
                Int_t nPadCount = 0;
                Int_t ii        = 0;
                while (TMath::Abs(digits->GetDataUnchecked(row,col-ii  ,time))
                                                                  >= sigThresh) {
                  nPadCount++;
                  ii++;
                  if (col-ii   <        0) break;
		}
                ii = 0;
                while (TMath::Abs(digits->GetDataUnchecked(row,col+ii+1,time))
                                                                  >= sigThresh) {
                  nPadCount++;
                  ii++;
                  if (col+ii+1 >= nColMax) break;
		}

                nClusters++;
                switch (nPadCount) {
                case 2:
                  iType = 0;
                  nClusters2pad++;
                  break;
                case 3:
                  iType = 1;
                  nClusters3pad++;
                  break;
                case 4:
                  iType = 2;
                  nClusters4pad++;
                  break;
                case 5:
                  iType = 3;
                  nClusters5pad++;
                  break;
                default:
                  iType = 4;
                  nClustersLarge++;
                  break;
		};

		// Don't analyze large clusters
                //if (iType == 4) continue;

                // Look for 5 pad cluster with minimum in the middle
                Bool_t fivePadCluster = kFALSE;
                if (col < nColMax-3) {
                  if (digits->GetDataUnchecked(row,col+2,time) < 0) {
                    fivePadCluster = kTRUE;
	  	  }
                  if ((fivePadCluster) && (col < nColMax-5)) {
                    if (digits->GetDataUnchecked(row,col+4,time) >= sigThresh) {
                      fivePadCluster = kFALSE;
		    }
		  }
                  if ((fivePadCluster) && (col >         1)) {
                    if (digits->GetDataUnchecked(row,col-2,time) >= sigThresh) {
                      fivePadCluster = kFALSE;
		    }
		  }
		}

		// 5 pad cluster
                // Modify the signal of the overlapping pad for the left part 
		// of the cluster which remains from a previous unfolding
                if (iUnfold) {
                  clusterSignal[0] *= ratioLeft;
                  iType   = 3;
                  iUnfold = 0;
		}

		// Unfold the 5 pad cluster
                if (fivePadCluster) {
                  for (iPad = 0; iPad < kNsig; iPad++) {
                    padSignal[iPad] = TMath::Abs(digits->GetDataUnchecked(row
                                                                         ,col-1+iPad
                                                                         ,time));
                  }
                  // Unfold the two maxima and set the signal on 
                  // the overlapping pad to the ratio
                  ratioRight        = Unfold(kEpsilon,iplan,padSignal);
                  ratioLeft         = 1.0 - ratioRight; 
                  clusterSignal[2] *= ratioRight;
                  iType   = 3;
                  iUnfold = 1;
                }

                Float_t clusterCharge = clusterSignal[0]
                                      + clusterSignal[1]
                                      + clusterSignal[2];
                
		// The position of the cluster
                clusterPads[0] = row + 0.5;
		// Take the shift of the additional time bins into account
                clusterPads[2] = time - nTimeBefore + 0.5;

                if (fPar->LUTOn()) {

  		  // Calculate the position of the cluster by using the
		  // lookup table method
                  clusterPads[1] = col + 0.5
                                 + fPar->LUTposition(iplan,clusterSignal[0]
                                                          ,clusterSignal[1]
					                  ,clusterSignal[2]);

		}
		else {

  		  // Calculate the position of the cluster by using the
		  // center of gravity method
                  clusterPads[1] = col + 0.5 
                                 + (clusterSignal[2] - clusterSignal[0]) 
		                 / clusterCharge;

		}

                Float_t q0 = clusterSignal[0];
		Float_t q1 = clusterSignal[1];
                Float_t q2 = clusterSignal[2];
                Float_t clusterSigmaY2 = (q1*(q0+q2)+4*q0*q2) /
                                         (clusterCharge*clusterCharge);

                // Correct for ExB displacement
                if (fPar->ExBOn()) { 
                  Int_t   local_time_bin = (Int_t) clusterPads[2];
                  Float_t driftLength    = local_time_bin * timeBinSize + kAmWidth;
                  Float_t colSize        = fPar->GetColPadSize(iplan);
                  Float_t deltaY         = omegaTau*driftLength/colSize;
                  clusterPads[1]         = clusterPads[1] - deltaY;
                }
                                       
                if (fVerbose > 1) {
                  printf("-----------------------------------------------------------\n");
                  printf("Create cluster no. %d\n",nClusters);
                  printf("Position: row = %f, col = %f, time = %f\n",clusterPads[0]
			                                            ,clusterPads[1]
                                                                    ,clusterPads[2]);
                  printf("Indices: %d, %d, %d\n",clusterDigit[0]
			                        ,clusterDigit[1]
                                                ,clusterDigit[2]);
                  printf("Total charge = %f\n",clusterCharge);
                  printf("Tracks: pad0 %d, %d, %d\n",clusterTracks[0]
			                            ,clusterTracks[1]
                                                    ,clusterTracks[2]);
                  printf("        pad1 %d, %d, %d\n",clusterTracks[3]
			                            ,clusterTracks[4]
                                                    ,clusterTracks[5]);
                  printf("        pad2 %d, %d, %d\n",clusterTracks[6]
			                            ,clusterTracks[7]
                                                    ,clusterTracks[8]);
                  printf("Type = %d, Number of pads = %d\n",iType,nPadCount);
                }

		// Calculate the position and the error
                Float_t clusterPos[3];
                clusterPos[0] = clusterPads[1] * colSize + col0;
                clusterPos[1] = clusterPads[0] * rowSize + row0;
                clusterPos[2] = clusterPads[2];
                Float_t clusterSig[2];
                clusterSig[0] = (clusterSigmaY2 + 1./12.) * colSize*colSize;
                clusterSig[1] = rowSize * rowSize / 12.;

                // Add the cluster to the output array 
                fTRD->AddCluster(clusterPos
                                ,idet
                                ,clusterCharge
                                ,clusterTracks
				,clusterSig
                                ,iType);

              }
            } 
          }   
        }     

	// Compress the arrays
        digits->Compress(1,0);
        track0->Compress(1,0);
        track1->Compress(1,0);
        track2->Compress(1,0);

        // Write the cluster and reset the array
	WriteClusters(idet);
	fTRD->ResetRecPoints();

        if (fVerbose > 0) {
          printf("<AliTRDclusterizerV1::MakeCluster> ");
          printf("Found %d clusters in total.\n"
                ,nClusters);
          printf("                                    2pad:  %d\n",nClusters2pad);
          printf("                                    3pad:  %d\n",nClusters3pad);
          printf("                                    4pad:  %d\n",nClusters4pad);
          printf("                                    5pad:  %d\n",nClusters5pad);
          printf("                                    Large: %d\n",nClustersLarge);
	}

      }    
    }      
  }        

  if (fVerbose > 0) {
    printf("<AliTRDclusterizerV1::MakeCluster> ");
    printf("Done.\n");
  }

  return kTRUE;

}

//_____________________________________________________________________________
Float_t AliTRDclusterizerV1::Unfold(Float_t eps, Int_t plane, Float_t* padSignal)
{
  //
  // Method to unfold neighbouring maxima.
  // The charge ratio on the overlapping pad is calculated
  // until there is no more change within the range given by eps.
  // The resulting ratio is then returned to the calling method.
  //

  Int_t   irc               = 0;
  Int_t   itStep            = 0;      // Count iteration steps

  Float_t ratio             = 0.5;    // Start value for ratio
  Float_t prevRatio         = 0;      // Store previous ratio

  Float_t newLeftSignal[3]  = {0};    // Array to store left cluster signal
  Float_t newRightSignal[3] = {0};    // Array to store right cluster signal
  Float_t newSignal[3]      = {0};

  // Start the iteration
  while ((TMath::Abs(prevRatio - ratio) > eps) && (itStep < 10)) {

    itStep++;
    prevRatio = ratio;

    // Cluster position according to charge ratio
    Float_t maxLeft  = (ratio*padSignal[2] - padSignal[0]) 
                     / (padSignal[0] + padSignal[1] + ratio*padSignal[2]);
    Float_t maxRight = (padSignal[4] - (1-ratio)*padSignal[2]) 
                     / ((1-ratio)*padSignal[2] + padSignal[3] + padSignal[4]);

    // Set cluster charge ratio
    irc = fPar->PadResponse(1.0,maxLeft ,plane,newSignal);
    Float_t ampLeft  = padSignal[1] / newSignal[1];
    irc = fPar->PadResponse(1.0,maxRight,plane,newSignal);
    Float_t ampRight = padSignal[3] / newSignal[1];

    // Apply pad response to parameters
    irc = fPar->PadResponse(ampLeft ,maxLeft ,plane,newLeftSignal );
    irc = fPar->PadResponse(ampRight,maxRight,plane,newRightSignal);

    // Calculate new overlapping ratio
    ratio = TMath::Min((Float_t)1.0,newLeftSignal[2] / 
                          (newLeftSignal[2] + newRightSignal[0]));

  }

  return ratio;

}

