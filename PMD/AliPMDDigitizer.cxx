/***************************************************************************
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
//-----------------------------------------------------//
//                                                     //
//  Source File : PMDDigitizer.cxx, Version 00         //
//                                                     //
//  Date   : September 20 2002                         //
//                                                     //
//-----------------------------------------------------//

#include <Riostream.h>
#include <TBRIK.h>
#include <TNode.h>
#include <TTree.h>
#include <TGeometry.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TParticle.h>

#include "AliRun.h"
#include "AliPMD.h"
#include "AliHit.h"
#include "AliDetector.h"
#include "AliRunLoader.h"
#include "AliLoader.h"
#include "AliConfig.h"
#include "AliMagF.h"
#include "AliRunDigitizer.h"
#include "AliHeader.h"

#include "AliPMDcell.h"
#include "AliPMDsdigit.h"
#include "AliPMDdigit.h"
#include "AliPMDDigitizer.h"
#include "AliPMDClustering.h"
#include "AliMC.h"

ClassImp(AliPMDDigitizer)

AliPMDDigitizer::AliPMDDigitizer()
{
  // Default Constructor
  //
  if (!fSDigits) fSDigits = new TClonesArray("AliPMDsdigit", 1000);  
  fNsdigit = 0;
  if (!fDigits) fDigits = new TClonesArray("AliPMDdigit", 1000);  
  fNdigit = 0;

  for (Int_t i = 0; i < fgkTotUM; i++)
    {
      for (Int_t j = 0; j < fgkRow; j++)
	{
	  for (Int_t k = 0; k < fgkCol; k++)
	    {
	      fCPV[i][j][k] = 0.; 
	      fPRE[i][j][k] = 0.; 
	    }
	}
    }

  if (!fCell) fCell = new TObjArray();

  fZPos = 361.5; // in units of cm, This is the default position of PMD
}
AliPMDDigitizer::~AliPMDDigitizer()
{
  // Default Destructor
  //
  delete fSDigits;
  delete fDigits;
  delete fCell;
}
//
// Member functions
//
void AliPMDDigitizer::OpengAliceFile(Char_t *file, Option_t *option)
{
  // Loads galice.root file and corresponding header, kinematics
  // hits and sdigits or digits depending on the option
  //
  fRunLoader = AliRunLoader::Open(file,AliConfig::fgkDefaultEventFolderName,
				  "UPDATE");
  
  if (!fRunLoader)
   {
     Error("Open","Can not open session for file %s.",file);
   }
  
  fRunLoader->LoadgAlice();
  fRunLoader->LoadHeader();
  fRunLoader->LoadKinematics();

  gAlice = fRunLoader->GetAliRun();
  
  if (gAlice)
    {
      printf("<AliPMDdigitizer::Open> ");
      printf("AliRun object found on file.\n");
    }
  else
    {
      printf("<AliPMDdigitizer::Open> ");
      printf("Could not find AliRun object.\n");
    }

  fPMD  = (AliPMD*)gAlice->GetDetector("PMD");
  fPMDLoader = fRunLoader->GetLoader("PMDLoader");
  if (fPMDLoader == 0x0)
    {
      cerr<<"Hits2Digits : Can not find PMD or PMDLoader\n";
    }

  const char *cHS = strstr(option,"HS");
  const char *cHD = strstr(option,"HD");
  const char *cSD = strstr(option,"SD");

  if (cHS)
    {
      fPMDLoader->LoadHits("READ");
      fPMDLoader->LoadSDigits("recreate");
    }
  else if (cHD)
    {
      fPMDLoader->LoadHits("READ");
      fPMDLoader->LoadDigits("recreate");
    }
  else if (cSD)
    {
      fPMDLoader->LoadSDigits("READ");
      fPMDLoader->LoadDigits("recreate");
    }

}
void AliPMDDigitizer::Hits2SDigits(Int_t ievt)
{
  // This reads the PMD Hits tree and assigns the right track number
  // to a cell and stores in the summable digits tree
  //
  // cout << " -------- Beginning of Hits2SDigits ----------- " << endl;

  const Int_t kPi0 = 111;
  const Int_t kGamma = 22;
  Int_t npmd;
  Int_t trackno;
  Int_t smnumber;
  Int_t trackpid;
  Int_t mtrackno;
  Int_t mtrackpid;

  Float_t xPos, yPos, zPos;
  Int_t xpad = -1, ypad = -1;
  Float_t edep;
  Float_t vx = -999.0, vy = -999.0, vz = -999.0;


  FILE *fpw = fopen("junk_digit1.dat","w");
  
  ResetSDigit();

  printf("Event Number =  %d \n",ievt); 
  Int_t nparticles = fRunLoader->GetHeader()->GetNtrack();
  printf("Number of Particles = %d \n", nparticles);
  fRunLoader->GetEvent(ievt);
  //  fPArray = gAlice->GetMCApp()->Particles();
  // ------------------------------------------------------- //
  // Pointer to specific detector hits.
  // Get pointers to Alice detectors and Hits containers

  fTreeH = fPMDLoader->TreeH();
  
  Int_t ntracks    = (Int_t) fTreeH->GetEntries();
  printf("Number of Tracks in the TreeH = %d \n", ntracks);

  fTreeS = fPMDLoader->TreeS();
  if (fTreeS == 0x0)
    {
      fPMDLoader->MakeTree("S");
      fTreeS = fPMDLoader->TreeS();
    }
  Int_t bufsize = 16000;
  fTreeS->Branch("PMDSDigit", &fSDigits, bufsize); 
  
  if (fPMD) fHits   = fPMD->Hits();

  // Start loop on tracks in the hits containers

  for (Int_t track=0; track<ntracks;track++) 
    {
      gAlice->ResetHits();
      fTreeH->GetEvent(track);
      if (fPMD) 
	{
	  npmd = fHits->GetEntriesFast();
	  for (int ipmd = 0; ipmd < npmd; ipmd++) 
	    {
	      fPMDHit = (AliPMDhit*) fHits->UncheckedAt(ipmd);
	      trackno = fPMDHit->GetTrack();
	      //  get kinematics of the particles

	      fParticle = gAlice->GetMCApp()->Particle(trackno);
	      trackpid  = fParticle->GetPdgCode();

	      fprintf(fpw,"track =%d trackno = %d trackpid = %d\n",
		      track, trackno, trackpid);
	      Int_t igatr = -999;
	      Int_t ichtr = -999;
	      Int_t igapid = -999;
	      Int_t imo;
	      Int_t igen = 0;
	      Int_t idmo = -999;

	      TParticle*  mparticle = fParticle;
	      Int_t tracknoOld=0, trackpidOld=0, statusOld = 0;
	      if (mparticle->GetFirstMother() == -1)
		{
		  tracknoOld  = trackno;
		  trackpidOld = trackpid;
		  statusOld   = -1;
		}
	      Int_t igstatus = 0;
	      while((imo = mparticle->GetFirstMother()) >= 0)
		{
		  igen++;

		  mparticle =  gAlice->GetMCApp()->Particle(imo);
		  idmo = mparticle->GetPdgCode();
		  
		  vx = mparticle->Vx();
		  vy = mparticle->Vy();
		  vz = mparticle->Vz();
		
		  //printf("==> Mother ID %5d %5d %5d Vertex: %13.3f %13.3f %13.3f\n", igen, imo, idmo, vx, vy, vz);
		  //fprintf(fpw1,"==> Mother ID %5d %5d %5d Vertex: %13.3f %13.3f %13.3f\n", igen, imo, idmo, vx, vy, vz);
		  if ((idmo == kGamma || idmo == -11 || idmo == 11) && vx == 0. && vy == 0. && vz == 0.)
		    {
		      igatr = imo;
		      igapid = idmo;
		      igstatus = 1;
		    }
		  if(igstatus == 0)
		    {
		      if (idmo == kPi0 && vx == 0. && vy == 0. && vz == 0.)
			{
			  igatr = imo;
			  igapid = idmo;
			}
		    }
		  ichtr = imo;
		}

	      if (idmo == kPi0 && vx == 0. && vy == 0. && vz == 0.)
		{
		  mtrackno = igatr;
		  mtrackpid = igapid;
		}
	      else
		{
		  mtrackno  = ichtr;
		  mtrackpid = idmo;
		}
	      if (statusOld == -1)
		{
		  mtrackno  = tracknoOld;
		  mtrackpid = trackpidOld;
		}
	      xPos = fPMDHit->X();
	      yPos = fPMDHit->Y();
	      zPos = fPMDHit->Z();
	      edep       = fPMDHit->fEnergy;
	      Int_t vol1 = fPMDHit->fVolume[1]; // Column
	      Int_t vol2 = fPMDHit->fVolume[2]; // Row
	      Int_t vol3 = fPMDHit->fVolume[3]; // UnitModule
	      Int_t vol6 = fPMDHit->fVolume[6]; // SuperModule
	      // -----------------------------------------//
	      // For Super Module 1 & 2                   //
	      //  nrow = 96, ncol = 48                    //
	      // For Super Module 3 & 4                   //
	      //  nrow = 48, ncol = 96                    //
	      // -----------------------------------------//
	      
	      smnumber = (vol6-1)*6 + vol3;

	      if (vol6 == 1 || vol6 == 2)
		{
		  xpad = vol1;
		  ypad = vol2;
		}
	      else if (vol6 == 3 || vol6 == 4)
		{
		  xpad = vol2;
		  ypad = vol1;
		}

	      //cout << "zpos = " << zPos << " edep = " << edep << endl;

	      Float_t zposition = TMath::Abs(zPos);
	      if (zposition < fZPos)
		{
		  // CPV
		  fDetNo = 1;
		}
	      else if (zposition > fZPos)
		{
		  // PMD
		  fDetNo = 0;
		}
	      Int_t smn = smnumber - 1;
	      Int_t ixx = xpad     - 1;
	      Int_t iyy = ypad     - 1;
	      if (fDetNo == 0)
		{
		  fPRE[smn][ixx][iyy] += edep;
		  fPRECounter[smn][ixx][iyy]++;

		  fPMDcell = new AliPMDcell(mtrackno,smn,ixx,iyy,edep);

		  fCell->Add(fPMDcell);
		}
	      else if(fDetNo == 1)
		{
		  fCPV[smn][ixx][iyy] += edep;
		  fCPVTrackNo[smn][ixx][iyy] = mtrackno;
		}
	    }
	}
    } // Track Loop ended

  TrackAssignment2Cell();
  ResetCell();

  Float_t deltaE      = 0.;
  Int_t   detno       = 0;
  Int_t   trno        = -1;
  Int_t   cellno      = 0;

  for (Int_t idet = 0; idet < 2; idet++)
    {
      for (Int_t ism = 0; ism < fgkTotUM; ism++)
	{
	  for (Int_t jrow = 0; jrow < fgkRow; jrow++)
	    {
	      for (Int_t kcol = 0; kcol < fgkCol; kcol++)
		{
		  cellno = jrow*fgkCol + kcol;
		  if (idet == 0)
		    {
		      deltaE = fPRE[ism][jrow][kcol];
		      trno   = fPRETrackNo[ism][jrow][kcol];
		      detno = 0;
		    }
		  else if (idet == 1)
		    {
		      deltaE = fCPV[ism][jrow][kcol];
		      trno   = fCPVTrackNo[ism][jrow][kcol];
		      detno = 1;
		    }
		  if (deltaE > 0.)
		    {
		      AddSDigit(trno,detno,ism,cellno,deltaE);
		    }
		}
	    }
	  fTreeS->Fill();
	  ResetSDigit();
	}
    }
  fPMDLoader->WriteSDigits("OVERWRITE");
  ResetCellADC();

  //  cout << " -------- End of Hits2SDigit ----------- " << endl;
}

void AliPMDDigitizer::Hits2Digits(Int_t ievt)
{
  // This reads the PMD Hits tree and assigns the right track number
  // to a cell and stores in the digits tree
  //
  const Int_t kPi0 = 111;
  const Int_t kGamma = 22;
  Int_t npmd;
  Int_t trackno;
  Int_t smnumber;
  Int_t trackpid;
  Int_t mtrackno;
  Int_t mtrackpid;

  Float_t xPos, yPos, zPos;
  Int_t xpad = -1, ypad = -1;
  Float_t edep;
  Float_t vx = -999.0, vy = -999.0, vz = -999.0;

  ResetDigit();

  printf("Event Number =  %d \n",ievt); 

  Int_t nparticles = fRunLoader->GetHeader()->GetNtrack();
  printf("Number of Particles = %d \n", nparticles);
  fRunLoader->GetEvent(ievt);
  //  fPArray = gAlice->GetMCApp()->Particles();
  // ------------------------------------------------------- //
  // Pointer to specific detector hits.
  // Get pointers to Alice detectors and Hits containers

  fPMD  = (AliPMD*)gAlice->GetDetector("PMD");
  fPMDLoader = fRunLoader->GetLoader("PMDLoader");

  if (fPMDLoader == 0x0)
    {
      cerr<<"Hits2Digits method : Can not find PMD or PMDLoader\n";
    }
  fTreeH = fPMDLoader->TreeH();
  Int_t ntracks    = (Int_t) fTreeH->GetEntries();
  printf("Number of Tracks in the TreeH = %d \n", ntracks);
  fPMDLoader->LoadDigits("recreate");
  fTreeD = fPMDLoader->TreeD();
  if (fTreeD == 0x0)
    {
      fPMDLoader->MakeTree("D");
      fTreeD = fPMDLoader->TreeD();
    }
  Int_t bufsize = 16000;
  fTreeD->Branch("PMDDigit", &fDigits, bufsize); 
  
  if (fPMD) fHits   = fPMD->Hits();

  // Start loop on tracks in the hits containers

  for (Int_t track=0; track<ntracks;track++) 
    {
      gAlice->ResetHits();
      fTreeH->GetEvent(track);
      
      if (fPMD) 
	{
	  npmd = fHits->GetEntriesFast();
	  for (int ipmd = 0; ipmd < npmd; ipmd++) 
	    {
	      fPMDHit = (AliPMDhit*) fHits->UncheckedAt(ipmd);
	      trackno = fPMDHit->GetTrack();
	      
	      //  get kinematics of the particles
	      
	      fParticle = gAlice->GetMCApp()->Particle(trackno);
	      trackpid  = fParticle->GetPdgCode();

	      Int_t igatr = -999;
	      Int_t ichtr = -999;
	      Int_t igapid = -999;
	      Int_t imo;
	      Int_t igen = 0;
	      Int_t idmo = -999;

	      TParticle*  mparticle = fParticle;
	      Int_t tracknoOld=0, trackpidOld=0, statusOld = 0;
	      if (mparticle->GetFirstMother() == -1)
		{
		  tracknoOld  = trackno;
		  trackpidOld = trackpid;
		  statusOld   = -1;
		}

	      Int_t igstatus = 0;
	      while((imo = mparticle->GetFirstMother()) >= 0)
		{
		  igen++;

		  mparticle =  gAlice->GetMCApp()->Particle(imo);
		  idmo = mparticle->GetPdgCode();
		  
		  vx = mparticle->Vx();
		  vy = mparticle->Vy();
		  vz = mparticle->Vz();
		
		  //printf("==> Mother ID %5d %5d %5d Vertex: %13.3f %13.3f %13.3f\n", igen, imo, idmo, vx, vy, vz);
		  //fprintf(fpw1,"==> Mother ID %5d %5d %5d Vertex: %13.3f %13.3f %13.3f\n", igen, imo, idmo, vx, vy, vz);
		  if ((idmo == kGamma || idmo == -11 || idmo == 11) && vx == 0. && vy == 0. && vz == 0.)
		    {
		      igatr = imo;
		      igapid = idmo;
		      igstatus = 1;
		    }
		  if(igstatus == 0)
		    {
		      if (idmo == kPi0 && vx == 0. && vy == 0. && vz == 0.)
			{
			  igatr = imo;
			  igapid = idmo;
			}
		    }
		  ichtr = imo;
		}

	      if (idmo == kPi0 && vx == 0. && vy == 0. && vz == 0.)
		{
		  mtrackno = igatr;
		  mtrackpid = igapid;
		}
	      else
		{
		  mtrackno  = ichtr;
		  mtrackpid = idmo;
		}
	      if (statusOld == -1)
		{
		  mtrackno  = tracknoOld;
		  mtrackpid = trackpidOld;
		}
	      
	      xPos = fPMDHit->X();
	      yPos = fPMDHit->Y();
	      zPos = fPMDHit->Z();
	      edep       = fPMDHit->fEnergy;
	      Int_t vol1 = fPMDHit->fVolume[1]; // Column
	      Int_t vol2 = fPMDHit->fVolume[2]; // Row
	      Int_t vol3 = fPMDHit->fVolume[3]; // UnitModule
	      Int_t vol6 = fPMDHit->fVolume[6]; // SuperModule

	      // -----------------------------------------//
	      // For Super Module 1 & 2                   //
	      //  nrow = 96, ncol = 48                    //
	      // For Super Module 3 & 4                   //
	      //  nrow = 48, ncol = 96                    //
	      // -----------------------------------------//
	      
	      smnumber = (vol6-1)*6 + vol3;

	      if (vol6 == 1 || vol6 == 2)
		{
		  xpad = vol1;
		  ypad = vol2;
		}
	      else if (vol6 == 3 || vol6 == 4)
		{
		  xpad = vol2;
		  ypad = vol1;
		}

	      //cout << "-zpos = " << -zPos << endl;

	      Float_t zposition = TMath::Abs(zPos);

	      if (zposition < fZPos)
		{
		  // CPV
		  fDetNo = 1;
		}
	      else if (zposition > fZPos)
		{
		  // PMD
		  fDetNo = 0;
		}

	      Int_t smn = smnumber - 1;
	      Int_t ixx = xpad     - 1;
	      Int_t iyy = ypad     - 1;
	      if (fDetNo == 0)
		{
		  fPRE[smn][ixx][iyy] += edep;
		  fPRECounter[smn][ixx][iyy]++;

		  fPMDcell = new AliPMDcell(mtrackno,smn,ixx,iyy,edep);

		  fCell->Add(fPMDcell);
		}
	      else if(fDetNo == 1)
		{
		  fCPV[smn][ixx][iyy] += edep;
		  fCPVTrackNo[smn][ixx][iyy] = mtrackno;
		}
	    }
	}
    } // Track Loop ended

  TrackAssignment2Cell();
  ResetCell();

  Float_t deltaE = 0.;
  Int_t detno = 0;
  Int_t trno = 1;
  Int_t cellno;

  for (Int_t idet = 0; idet < 2; idet++)
    {
      for (Int_t ism = 0; ism < fgkTotUM; ism++)
	{
	  for (Int_t jrow = 0; jrow < fgkRow; jrow++)
	    {
	      for (Int_t kcol = 0; kcol < fgkCol; kcol++)
		{
		  cellno = jrow*fgkCol + kcol;
		  if (idet == 0)
		    {
		      deltaE = fPRE[ism][jrow][kcol];
		      trno   = fPRETrackNo[ism][jrow][kcol];
		      detno = 0;
		    }
		  else if (idet == 1)
		    {
		      deltaE = fCPV[ism][jrow][kcol];
		      trno   = fCPVTrackNo[ism][jrow][kcol];
		      detno = 1;
		    }
		  if (deltaE > 0.)
		    {
		      AddDigit(trno,detno,ism,cellno,deltaE);
		    }
		} // column loop
	    } // row    loop
	} // supermodule loop
      fTreeD->Fill();
      ResetDigit();
    } // detector loop

  fPMDLoader->WriteDigits("OVERWRITE");
  ResetCellADC();
  
  //  cout << " -------- End of Hits2Digit ----------- " << endl;
}


void AliPMDDigitizer::SDigits2Digits(Int_t ievt)
{
  // This reads the PMD sdigits tree and converts energy deposition
  // in a cell to ADC and stores in the digits tree
  //
  //  cout << " -------- Beginning of SDigits2Digit ----------- " << endl;
  fRunLoader->GetEvent(ievt);

  fTreeS = fPMDLoader->TreeS();
  AliPMDsdigit  *pmdsdigit;
  TBranch *branch = fTreeS->GetBranch("PMDSDigit");
  branch->SetAddress(&fSDigits);

  fTreeD = fPMDLoader->TreeD();
  if (fTreeD == 0x0)
    {
      fPMDLoader->MakeTree("D");
      fTreeD = fPMDLoader->TreeD();
    }
  Int_t bufsize = 16000;
  fTreeD->Branch("PMDDigit", &fDigits, bufsize); 

  Int_t   trno, det, smn;
  Int_t   cellno;
  Float_t edep, adc;

  Int_t nmodules = (Int_t) fTreeS->GetEntries();

  for (Int_t imodule = 0; imodule < nmodules; imodule++)
    {
      fTreeS->GetEntry(imodule); 
      Int_t nentries = fSDigits->GetLast();
      //cout << " nentries = " << nentries << endl;
      for (Int_t ient = 0; ient < nentries+1; ient++)
	{
	  pmdsdigit = (AliPMDsdigit*)fSDigits->UncheckedAt(ient);
	  trno   = pmdsdigit->GetTrackNumber();
	  det    = pmdsdigit->GetDetector();
	  smn    = pmdsdigit->GetSMNumber();
	  cellno = pmdsdigit->GetCellNumber();
	  edep   = pmdsdigit->GetCellEdep();

	  MeV2ADC(edep,adc);
	  AddDigit(trno,det,smn,cellno,adc);      
	}
      fTreeD->Fill();
      ResetDigit();
    }
  fPMDLoader->WriteDigits("OVERWRITE");
  //  cout << " -------- End of SDigits2Digit ----------- " << endl;
}

void AliPMDDigitizer::TrackAssignment2Cell()
{
  // 
  // This block assigns the cell id when there are
  // multiple tracks in a cell according to the
  // energy deposition
  //
  Bool_t jsort = false;

  Int_t i, j, k;

  Float_t *fracEdp;
  Float_t *trEdp;
  Int_t *status1;
  Int_t *status2;
  Int_t *trnarray;
  Int_t   ****pmdTrack;
  Float_t ****pmdEdep;

  pmdTrack = new Int_t ***[fgkTotUM];
  pmdEdep  = new Float_t ***[fgkTotUM];
  for (i=0; i<fgkTotUM; i++)
    {
      pmdTrack[i] = new Int_t **[fgkRow];
      pmdEdep[i]  = new Float_t **[fgkRow];
    }

  for (i = 0; i < fgkTotUM; i++)
    {
      for (j = 0; j < fgkRow; j++)
	{
	  pmdTrack[i][j] = new Int_t *[fgkCol];
	  pmdEdep[i][j]  = new Float_t *[fgkCol];
	}
    }
  
  for (i = 0; i < fgkTotUM; i++)
    {
      for (j = 0; j < fgkRow; j++)
	{
	  for (k = 0; k < fgkCol; k++)
	    {
	      Int_t nn = fPRECounter[i][j][k];
	      if(nn > 0)
		{
		  pmdTrack[i][j][k] = new Int_t[nn];
		  pmdEdep[i][j][k] = new Float_t[nn];
		}
	      else
		{
		  nn = 1;
		  pmdTrack[i][j][k] = new Int_t[nn];
		  pmdEdep[i][j][k] = new Float_t[nn];
		}		      
	      fPRECounter[i][j][k] = 0;
	    }
	}
    }


  Int_t nentries = fCell->GetEntries();

  Int_t   mtrackno, ism, ixp, iyp;
  Float_t edep;

  for (i = 0; i < nentries; i++)
    {
      fPMDcell = (AliPMDcell*)fCell->UncheckedAt(i);
      
      mtrackno = fPMDcell->GetTrackNumber();
      ism      = fPMDcell->GetSMNumber();
      ixp      = fPMDcell->GetX();
      iyp      = fPMDcell->GetY();
      edep     = fPMDcell->GetEdep();
      Int_t nn = fPRECounter[ism][ixp][iyp];
      //      cout << " nn = " << nn << endl;
      pmdTrack[ism][ixp][iyp][nn] = (Int_t) mtrackno;
      pmdEdep[ism][ixp][iyp][nn] = edep;
      fPRECounter[ism][ixp][iyp]++;
    }
  
  Int_t iz, il;
  Int_t im, ix, iy;
  Int_t nn;
  
  for (im=0; im<fgkTotUM; im++)
    {
      for (ix=0; ix<fgkRow; ix++)
	{
	  for (iy=0; iy<fgkCol; iy++)
	    {
	      nn = fPRECounter[im][ix][iy];
	      if (nn > 1)
		{
		  // This block handles if a cell is fired
		  // many times by many tracks
		  status1  = new Int_t[nn];
		  status2  = new Int_t[nn];
		  trnarray = new Int_t[nn];
		  for (iz = 0; iz < nn; iz++)
		    {
		      status1[iz] = pmdTrack[im][ix][iy][iz];
		    }
		  TMath::Sort(nn,status1,status2,jsort);
		  Int_t trackOld = -99999;
		  Int_t track, trCount = 0;
		  for (iz = 0; iz < nn; iz++)
		    {
		      track = status1[status2[iz]];
		      if (trackOld != track)
			{
			  trnarray[trCount] = track;
			  trCount++;
			}			      
		      trackOld = track;
		    }
		  delete status1;
		  delete status2;
		  Float_t totEdp = 0.;
		  trEdp = new Float_t[trCount];
		  fracEdp = new Float_t[trCount];
		  for (il = 0; il < trCount; il++)
		    {
		      trEdp[il] = 0.;
		      track = trnarray[il];
		      for (iz = 0; iz < nn; iz++)
			{
			  if (track == pmdTrack[im][ix][iy][iz])
			    {
			      trEdp[il] += pmdEdep[im][ix][iy][iz];
			    }
			}
		      totEdp += trEdp[il];
		    }
		  Int_t ilOld = 0;
		  Float_t fracOld = 0.;
		  
		  for (il = 0; il < trCount; il++)
		    {
		      fracEdp[il] = trEdp[il]/totEdp;
		      if (fracOld < fracEdp[il])
			{
			  fracOld = fracEdp[il];
			  ilOld = il;
			}
		    }
		  fPRETrackNo[im][ix][iy] = trnarray[ilOld];
		  delete fracEdp;
		  delete trEdp;
		  delete trnarray;
		}
	      else if (nn == 1)
		{
		  // This only handles if a cell is fired
		  // by only one track
		  
		  fPRETrackNo[im][ix][iy] = pmdTrack[im][ix][iy][0];
		  
		}
	      else if (nn ==0)
		{
		  // This is if no cell is fired
		  fPRETrackNo[im][ix][iy] = -999;
		}
	    } // end of iy
	} // end of ix
    } // end of im
  
  // Delete all the pointers
  
  for (i = 0; i < fgkTotUM; i++)
    {
      for (j = 0; j < fgkRow; j++)
	{
	  for (k = 0; k < fgkCol; k++)
	    {
	      delete [] pmdTrack[i][j][k];
	      delete [] pmdEdep[i][j][k];
	    }
	}
    }
  
  for (i = 0; i < fgkTotUM; i++)
    {
      for (j = 0; j < fgkRow; j++)
	{
	  delete [] pmdTrack[i][j];
	  delete [] pmdEdep[i][j];
	}
    }
  
  for (i = 0; i < fgkTotUM; i++)
    {
      delete [] pmdTrack[i];
      delete [] pmdEdep[i];
    }
  delete pmdTrack;
  delete pmdEdep;
  // 
  // End of the cell id assignment
  //
}


void AliPMDDigitizer::MeV2ADC(Float_t mev, Float_t & adc) const
{
  // This converts the simulated edep to ADC according to the
  // Test Beam Data
  // To be done
  //
  adc = mev*1.;
}
void AliPMDDigitizer::AddSDigit(Int_t trnumber, Int_t det, Int_t smnumber, 
  Int_t cellnumber, Float_t adc)
{
  // Add SDigit
  //
  TClonesArray &lsdigits = *fSDigits;
  AliPMDsdigit *newcell;
  newcell = new AliPMDsdigit(trnumber,det,smnumber,cellnumber,adc);
  new(lsdigits[fNsdigit++]) AliPMDsdigit(newcell);
  delete newcell;
}

void AliPMDDigitizer::AddDigit(Int_t trnumber, Int_t det, Int_t smnumber, 
  Int_t cellnumber, Float_t adc)
{
  // Add Digit
  //
  TClonesArray &ldigits = *fDigits;
  AliPMDdigit *newcell;
  newcell = new AliPMDdigit(trnumber,det,smnumber,cellnumber,adc);
  new(ldigits[fNdigit++]) AliPMDdigit(newcell);
  delete newcell;
}

void AliPMDDigitizer::SetZPosition(Float_t zpos)
{
  fZPos = zpos;
}
Float_t AliPMDDigitizer::GetZPosition() const
{
  return fZPos;
}

void AliPMDDigitizer::ResetCell()
{
  // clears the cell array and also the counter
  //  for each cell
  //
  fCell->Clear();
  for (Int_t i = 0; i < fgkTotUM; i++)
    {
      for (Int_t j = 0; j < fgkRow; j++)
	{
	  for (Int_t k = 0; k < fgkCol; k++)
	    {
	      fPRECounter[i][j][k] = 0; 
	    }
	}
    }
}
void AliPMDDigitizer::ResetSDigit()
{
  // Clears SDigits
  fNsdigit = 0;
  if (fSDigits) fSDigits->Clear();
}
void AliPMDDigitizer::ResetDigit()
{
  // Clears Digits
  fNdigit = 0;
  if (fDigits) fDigits->Clear();
}

void AliPMDDigitizer::ResetCellADC()
{
  // Clears individual cells edep
  for (Int_t i = 0; i < fgkTotUM; i++)
    {
      for (Int_t j = 0; j < fgkRow; j++)
	{
	  for (Int_t k = 0; k < fgkCol; k++)
	    {
	      fCPV[i][j][k] = 0.; 
	      fPRE[i][j][k] = 0.; 
	    }
	}
    }
}

void AliPMDDigitizer::UnLoad(Option_t *option)
{
  // Unloads all the root files
  //
  const char *cS = strstr(option,"S");
  const char *cD = strstr(option,"D");

  fRunLoader->UnloadgAlice();
  fRunLoader->UnloadHeader();
  fRunLoader->UnloadKinematics();

  if (cS)
    {
      fPMDLoader->UnloadHits();
    }
  if (cD)
    {
      fPMDLoader->UnloadHits();
      fPMDLoader->UnloadSDigits();
    }
}
