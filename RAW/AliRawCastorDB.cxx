// @(#)alimdc:$Name$:$Id$
// Author: Fons Rademakers  26/11/99

/**************************************************************************
 * Copyright(c) 1998-2003, ALICE Experiment at CERN, All rights reserved. *
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

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AliRawCastorDB                                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TSystem.h>
#include <TUrl.h>

#include "AliMDC.h"

#include "AliRawCastorDB.h"


ClassImp(AliRawCastorDB)


//______________________________________________________________________________
AliRawCastorDB::AliRawCastorDB(AliRawEvent *event,
#ifdef USE_HLT
			       AliESD *esd,
#endif
			       Double_t maxsize, Int_t compress)
   : AliRawDB(event,
#ifdef USE_HLT
	      esd,
#endif
	      maxsize, compress, kFALSE)
{
   // Create a new raw DB that will be accessed via CASTOR and rootd.

#ifndef USE_RDM
   static int init = 0;
   // Set STAGE_POOL environment variable to current host
   if (!init) {
      // THESE ENVIRONMENT SYMBOLS ARE NOW DEFINED BY THE ALICE DATE SETUP
      // THEREFORE WE SHALL NOT USE ANY HARDCODED VALUES BUT RATHER USE
      // WHATEVER HAS BEEN SET IN THE DATE SITE
      //gSystem->Setenv("STAGE_POOL", "lcg00");
      //gSystem->Setenv("STAGE_HOST", "stage013");

      // however for sanity we check if they are really set
      if (!gSystem->Getenv("STAGE_POOL"))
         Error("AliRawRFIODB", "STAGE_POOL not set");
      if (!gSystem->Getenv("STAGE_HOST"))
         Error("AliRawRFIODB", "STAGE_HOST not set");
      init = 1;
   }
#endif

   if (!Create())
      MakeZombie();
   else
      fRawDB->UseCache(50, 0x200000);  //0x100000 = 1MB)
}

//______________________________________________________________________________
const char *AliRawCastorDB::GetFileName() const
{
   // Return filename based on hostname and date and time. This will make
   // each file unique. Also the directory will be made unique for each
   // day by adding the date to the fs. Assumes there is always enough
   // space on the device.

   static TString fname;

   TString fs  = AliMDC::CastorFS();
   TString fsr = AliMDC::RFIOFS();
   TDatime dt;

   // make a new subdirectory for each day
   fs += "/adc-";
   fs += dt.GetDate();

   fsr += "/adc-";
   fsr += dt.GetDate();

   Long_t id, size, flags, time;
   if (gSystem->GetPathInfo(fsr, &id, &size, &flags, &time) == 1) {
      // directory does not exist, create it
      if (gSystem->mkdir(fsr, kTRUE) == -1) {
         Error("GetFileName", "cannot create dir %s, using %s", fsr.Data(),
               AliMDC::RFIOFS());
         fs = AliMDC::CastorFS();
      }
   }
   // FIXME: should check if fs is a directory

   TString hostname = gSystem->HostName();
   Int_t pos;
   if ((pos = hostname.Index(".")) != kNPOS)
      hostname.Remove(pos);

   fname = fs + "/" + hostname + "_";
   fname += dt.GetDate();
   fname += "_";
   fname += dt.GetTime();
   fname += ".root";

   return fname;
}

//______________________________________________________________________________
void AliRawCastorDB::Close()
{
   // Close raw CASTOR/rootd DB.

   if (!fRawDB) return;

   fRawDB->cd();

   // Write the tree.
   fTree->Write();

   // Close DB, this also deletes the fTree
   fRawDB->Close();

   if (AliMDC::DeleteFiles()) {
      TUrl u(fRawDB->GetName());
      gSystem->Exec(Form("rfrm %s", u.GetFile()));
   }

   delete fRawDB;
   fRawDB = 0;
}
