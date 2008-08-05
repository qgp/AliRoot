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

/* $Id: AliTRDtrackingChamber.cxx 23810 2008-02-08 09:00:27Z hristov $ */

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  Tracking in one chamber                                               //
//                                                                        //
//  Authors:                                                              //
//    Alex Bercuci <A.Bercuci@gsi.de>                                     //
//    Markus Fasel <M.Fasel@gsi.de>                                       //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include "AliTRDtrackingChamber.h"

#include "TMath.h"
#include "TMatrixTBase.h"
#include <TTreeStream.h>

#include "AliTRDReconstructor.h"
#include "AliTRDrecoParam.h"
#include "AliTRDtrackerV1.h"
#include "AliTRDgeometry.h"
#include "AliTRDpadPlane.h"
#include "AliTRDcalibDB.h"

ClassImp(AliTRDtrackingChamber)

//_______________________________________________________
AliTRDtrackingChamber::AliTRDtrackingChamber(Int_t det) :
	fDetector(det)
	,fX0(0.)
{}  

//_______________________________________________________
void AliTRDtrackingChamber::Clear(const Option_t *opt)
{
	for(Int_t itb=0; itb<kNTimeBins; itb++) fTB[itb].Clear(opt);
}

//_______________________________________________________
void AliTRDtrackingChamber::InsertCluster(AliTRDcluster *c, Int_t index)
{
	fTB[c->GetPadTime()].InsertCluster(c, index);
}

//_______________________________________________________
Bool_t AliTRDtrackingChamber::Build(AliTRDgeometry *geo)
{
// Init chamber and all time bins (AliTRDchamberTimeBin)
// Calculates radial position of the chamber based on 
// radial positions of the time bins (calibration/alignment aware)
//
	Int_t stack = geo->GetStack(fDetector);
	Int_t layer = geo->GetLayer(fDetector);
	AliTRDpadPlane *pp = geo->GetPadPlane(layer, stack);
	Double_t zl = pp->GetRow0ROC() - pp->GetRowEndROC();
	Double_t z0 = geo->GetRow0(layer, stack, 0) - zl;
	Int_t nrows = pp->GetNrows();
	
	Int_t index[50], jtb = 0;
	for(Int_t itb=0; itb<kNTimeBins; itb++){ 
		if(!fTB[itb]) continue;
		fTB[itb].SetRange(z0, zl);
		fTB[itb].SetNRows(nrows);
		fTB[itb].BuildIndices();
		index[jtb++] = itb;
	}	
	if(jtb<2) return kFALSE;
	
	
	// ESTIMATE POSITION OF PAD PLANE FOR THIS CHAMBER
	Double_t x0 = fTB[index[0]].GetX();
	Double_t x1 = fTB[index[1]].GetX();
	Double_t dx = (x0 - x1)/(index[1] - index[0]); 
	fX0 = x0 + dx*(index[0] - (Int_t)AliTRDcalibDB::Instance()->GetT0Average(fDetector));	
	return kTRUE;
}
	
//_______________________________________________________	
Int_t AliTRDtrackingChamber::GetNClusters() const
{
// Returns number of clusters in chamber
//
	Int_t n = 0;
	for(Int_t itb=0; itb<kNTimeBins; itb++){ 
		n += Int_t(fTB[itb]);
	}
	return n;	
}	

//_______________________________________________________
Double_t AliTRDtrackingChamber::GetQuality()
{
  //
  // Calculate chamber quality for seeding.
  // 
  //
  // Parameters :
  //   layers : Array of propagation layers for this plane.
  //
  // Output :
  //   plane quality factor for seeding
  // 
  // Detailed description
  //
  // The quality of the plane for seeding is higher if:
  //  1. the average timebin population is closer to an integer number
  //  2. the distribution of clusters/timebin is closer to a uniform distribution.
  //    - the slope of the first derivative of a parabolic fit is small or
  //    - the slope of a linear fit is small
  //

	Int_t ncl   = 0;
	Int_t nused = 0;
	Int_t nClLayer;
	for(int itb=0; itb<kNTimeBins; itb++){
		if(!(nClLayer = fTB[itb].GetNClusters())) continue;
		ncl += nClLayer;
		for(Int_t incl = 0; incl < nClLayer; incl++){
			if((fTB[itb].GetCluster(incl))->IsUsed()) nused++;
		}
	}
	
	// calculate the deviation of the mean number of clusters from the
	// closest integer values
	Float_t nclMed = float(ncl-nused)/AliTRDtrackerV1::GetNTimeBins();
	Int_t ncli = Int_t(nclMed);
	Float_t nclDev = TMath::Abs(nclMed - TMath::Max(ncli, 1));
	nclDev -= (nclDev>.5) && ncli ? 1. : 0.;
	return TMath::Exp(-5.*TMath::Abs(nclDev));

// 	// get slope of the derivative
// 	if(!fitter.Eval()) return quality;
// 	fitter.PrintResults(3);
// 	Double_t a = fitter.GetParameter(1);
// 
// 	printf("ncl_dev(%f)  a(%f)\n", ncl_dev, a);
// 	return quality*TMath::Exp(-a);

}


//_______________________________________________________
Bool_t AliTRDtrackingChamber::GetSeedingLayer(AliTRDchamberTimeBin *&fakeLayer, AliTRDgeometry *geo, const AliTRDReconstructor *rec)
{
  //
  // Creates a seeding layer
  //
	
	// constants
	const Int_t kMaxRows = 16;
	const Int_t kMaxCols = 144;
	const Int_t kMaxPads = 2304;
	Int_t timeBinMin = rec->GetRecoParam()->GetNumberOfPresamples();
	Int_t timeBinMax = rec->GetRecoParam()->GetNumberOfPostsamples();

	// Get the geometrical data of the chamber
	Int_t layer = geo->GetLayer(fDetector);
	Int_t stack = geo->GetStack(fDetector);
	Int_t sector= geo->GetSector(fDetector);
	AliTRDpadPlane *pp = geo->GetPadPlane(layer, stack);
	Int_t nCols = pp->GetNcols();
	Float_t ymin = TMath::Min(pp->GetCol0(), pp->GetColEnd());
	Float_t ymax = TMath::Max(pp->GetCol0(), pp->GetColEnd());
	Float_t zmin = TMath::Min(pp->GetRow0(), pp->GetRowEnd());
	Float_t zmax = TMath::Max(pp->GetRow0(), pp->GetRowEnd());
	Float_t z0 = -1., zl = -1.;
	Int_t nRows = pp->GetNrows();
	Float_t binlength = (ymax - ymin)/nCols; 
	//AliInfo(Form("ymin(%f) ymax(%f) zmin(%f) zmax(%f) nRows(%d) binlength(%f)", ymin, ymax, zmin, zmax, nRows, binlength));
	
	// Fill the histogram
	Int_t nClusters;	
	Int_t *histogram[kMaxRows];											// 2D-Histogram
	Int_t hvals[kMaxPads];	memset(hvals, 0, sizeof(Int_t)*kMaxPads);	
	Float_t *sigmas[kMaxRows];
	Float_t svals[kMaxPads];	memset(svals, 0, sizeof(Float_t)*kMaxPads);	
	AliTRDcluster *c = 0x0;
	for(Int_t irs = 0; irs < kMaxRows; irs++){
		histogram[irs] = &hvals[irs*kMaxCols];
		sigmas[irs] = &svals[irs*kMaxCols];
	}
	for(Int_t iTime = timeBinMin; iTime < kNTimeBins-timeBinMax; iTime++){
    if(!(nClusters = fTB[iTime].GetNClusters())) continue;
		z0 = fTB[iTime].GetZ0();
		zl = fTB[iTime].GetDZ0();
		for(Int_t incl = 0; incl < nClusters; incl++){
			c = fTB[iTime].GetCluster(incl);	
			histogram[c->GetPadRow()][c->GetPadCol()]++;
			sigmas[c->GetPadRow()][c->GetPadCol()] += c->GetSigmaZ2();
		}
	}
	
// Now I have everything in the histogram, do the selection
	//Int_t nPads = nCols * nRows;
	// This is what we are interested in: The center of gravity of the best candidates
	Float_t cogyvals[kMaxPads]; memset(cogyvals, 0, sizeof(Float_t)*kMaxPads);
	Float_t cogzvals[kMaxPads]; memset(cogzvals, 0, sizeof(Float_t)*kMaxPads);
	Float_t *cogy[kMaxRows];
	Float_t *cogz[kMaxRows];
	
	// Lookup-Table storing coordinates according to the bins
	Float_t yLengths[kMaxCols];
	Float_t zLengths[kMaxRows];
	for(Int_t icnt = 0; icnt < nCols; icnt++){
 		yLengths[icnt] = pp->GetColPos(nCols - 1 - icnt) + binlength/2;
	}
	for(Int_t icnt = 0; icnt < nRows; icnt++){
		zLengths[icnt] = pp->GetRowPos(icnt) - pp->GetRowSize(icnt)/2;
	}

	// A bitfield is used to mask the pads as usable
	Short_t mask[kMaxCols]; memset(mask, 0 ,sizeof(Short_t) * kMaxCols);//bool mvals[kMaxPads];
	for(UChar_t icount = 0; icount < nRows; icount++){
		cogy[icount] = &cogyvals[icount*kMaxCols];
		cogz[icount] = &cogzvals[icount*kMaxCols];
	}
	// In this array the array position of the best candidates will be stored
	Int_t   cand[AliTRDtrackerV1::kMaxTracksStack];
	Float_t sigcands[AliTRDtrackerV1::kMaxTracksStack];
 	
	// helper variables
	Int_t indices[kMaxPads]; memset(indices, -1, sizeof(Int_t)*kMaxPads);
	Int_t nCandidates = 0;
 	Float_t norm, cogv;
	// histogram filled -> Select best bins
	Int_t nPads = nCols * nRows;
	TMath::Sort(nPads, hvals, indices);			// bins storing a 0 should not matter
	// Set Threshold
	Int_t maximum = hvals[indices[0]];	// best
	Int_t threshold = Int_t(maximum * rec->GetRecoParam()->GetFindableClusters());
	Int_t col, row, lower, lower1, upper, upper1;
	for(Int_t ib = 0; ib < nPads; ib++){
		if(nCandidates >= AliTRDtrackerV1::kMaxTracksStack){
			printf("Number of seed candidates %d exceeded maximum allowed per stack %d", nCandidates, AliTRDtrackerV1::kMaxTracksStack);
			break;
		}
		// Positions
		row = indices[ib]/nCols;
		col = indices[ib]%nCols;
		// here will be the threshold condition:
		if((mask[col] & (1 << row)) != 0) continue;		// Pad is masked: continue
		if(histogram[row][col] < TMath::Max(threshold, 1)){	// of course at least one cluster is needed
			break;			// number of clusters below threshold: break;
		} 
		// passing: Mark the neighbors
		lower  = TMath::Max(col - 1, 0); upper  = TMath::Min(col + 2, nCols);
		lower1 = TMath::Max(row - 1, 0); upper1 = TMath::Min(row + 2, nCols);
		for(Int_t ic = lower; ic < upper; ++ic)
			for(Int_t ir = lower1; ir < upper1; ++ir){
				if(ic == col && ir == row) continue;
				mask[ic] |= (1 << ir);
			}
		// Storing the position in an array
		// testing for neigboring
		cogv = 0;
		norm = 0;
		lower = TMath::Max(col - 1, 0);
		upper = TMath::Min(col + 2, nCols);
		for(Int_t inb = lower; inb < upper; ++inb){
			cogv += yLengths[inb] * histogram[row][inb];
			norm += histogram[row][inb];
		}
		cogy[row][col] = cogv / norm;
		cogv = 0; norm = 0;
		lower = TMath::Max(row - 1, 0);
		upper = TMath::Min(row + 2, nRows);
		for(Int_t inb = lower; inb < upper; ++inb){
			cogv += zLengths[inb] * histogram[inb][col];
			norm += histogram[inb][col];
		}
		cogz[row][col] = Float_t(cogv) /  norm;
		// passed the filter
		cand[nCandidates] = row*nCols + col;	// store the position of a passig candidate into an Array
		sigcands[nCandidates] = sigmas[row][col] / histogram[row][col]; // never be a floating point exeption
		// Analysis output
		nCandidates++;
	}
	if(!nCandidates) return kFALSE;
	
	Float_t pos[3], sig[2];
	Short_t signal[7]; memset(&signal[0], 0, 7*sizeof(Short_t));
	
  new(fakeLayer) AliTRDchamberTimeBin(layer, stack, sector, z0, zl);
	AliTRDcluster *cluster = 0x0;
	if(nCandidates){
		UInt_t fakeIndex = 0;
		for(Int_t ican = 0; ican < nCandidates; ican++){
			row = cand[ican] / nCols;
			col = cand[ican] % nCols;
			//temporary
			Int_t n = 0; Double_t x = 0., y = 0., z = 0.;
			for(int itb=0; itb<kNTimeBins; itb++){
				if(!(nClusters = fTB[itb].GetNClusters())) continue;
				for(Int_t incl = 0; incl < nClusters; incl++){
					c = fTB[itb].GetCluster(incl);	
					if(c->GetPadRow() != row) continue;
					if(TMath::Abs(c->GetPadCol() - col) > 2) continue;
					x += c->GetX();
					y += c->GetY();
					z += c->GetZ();
					n++;
				}
			}
			pos[0] = x/n;
			pos[1] = y/n;
			pos[2] = z/n;
			sig[0] = .02;
			sig[1] = sigcands[ican];
			cluster = new AliTRDcluster(fDetector, 0., pos, sig, 0x0, 3, signal, col, row, 0, 0, 0., 0);
			fakeLayer->InsertCluster(cluster, fakeIndex++);
		}
	}
	fakeLayer->SetNRows(nRows);
	fakeLayer->SetOwner();
	fakeLayer->BuildIndices();
	//fakeLayer->PrintClusters();
	
	if(rec->GetStreamLevel(AliTRDReconstructor::kTracker) >= 3){
		//TMatrixD hist(nRows, nCols);
		//for(Int_t i = 0; i < nRows; i++)
		//	for(Int_t j = 0; j < nCols; j++)
		//		hist(i,j) = histogram[i][j];
		TTreeSRedirector &cstreamer = *AliTRDtrackerV1::DebugStreamer();
		cstreamer << "GetSeedingLayer"
		<< "layer="      << layer
		<< "ymin="       << ymin
		<< "ymax="       << ymax
		<< "zmin="       << zmin
		<< "zmax="       << zmax
		<< "L.="         << fakeLayer
		//<< "Histogram.=" << &hist
		<< "\n";
	}
	
	return kTRUE;
}

