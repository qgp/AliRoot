// @(#)alimdc:$Name$:$Id$
// Author: Fons Rademakers  26/11/99

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// alimdc                                                               //
//                                                                      //
// Main program used to create application that reads a data stream     //
// from the DATE DAQ system and that creates a ROOT database.           //
//                                                                      //
// Written by: Fons Rademakers, 1/4/99.                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include <TROOT.h>
#include <TSystem.h>
#include <TError.h>

#ifdef USE_SMI
extern "C" {
   #include <smirtl.h>
}
#endif

#include "AliRawEvent.h"


#ifdef USE_SMI
void SMI_handle_command()
{
   // Handle SMI commands

   char action[64], param[64];
   int n_params;

   smi_get_action(action, &n_params);
   if (n_params >= 1) {
      smi_get_par_value("PARAM", param);
   } else {
      strcpy(param, "");
   }
   if (strcmp(action, "STOP") == 0) {
      gAliMDC->StopLoop();
   }
   smi_set_state("RUNNING");
}
#endif

//______________________________________________________________________________
static void Usage(const char *prognam)
{
#ifdef USE_SMI
      fprintf(stderr, "Usage: %s <sminame> <dbsize> <filter> <compmode> [date_file]\n",
              prognam);
      fprintf(stderr, " <sminame> = name used by SMI\n");
#else
      fprintf(stderr, "Usage: %s <dbsize> <filter> <compmode> [date_file]\n",
              prognam);
#endif
      fprintf(stderr, " <dbsize> = maximum raw DB size (in bytes)\n");
      fprintf(stderr, "    (precede by - to delete raw and tag databases on close)\n");
      fprintf(stderr, " <filter> = state of 3rd level filter (0 or 1)\n");
      fprintf(stderr, " <compmode> = compression level (see TFile)\n");
      fprintf(stderr, "    (precede by - to use RFIO, -0 is RFIO and 0 compression)\n");
      fprintf(stderr, "    (precede by + to use rootd, -0 is rootd and 0 compression)\n");
      fprintf(stderr, "    (precede by @ to use /dev/null as sink)\n");
#ifdef USE_EB
      fprintf(stderr, " [date_file] = optional input file (default reads from DATE EventBuffer)\n");
#else
      fprintf(stderr, " [date_file] = optional input file (default reads from pipe /tmp/alimdc.fifo)\n");
#endif
      fprintf(stderr, "    (precede with - for endless loop on same file (use SIGUSR1 to stop)\n");
}

//______________________________________________________________________________
int main(int argc, char **argv)
{
   // Convert a DATE data stream to a ROOT DB.

   // Set ROOT in batch mode
   gROOT->SetBatch();

#ifdef USE_SMI
    // Handle command line arguments
   if ((argc == 2 && (!strcmp(argv[1], "-?") || !strcmp(argv[1], "-help"))) ||
       argc > 6 || argc < 5) {
      Usage(argv[0]);
      return 1;
   }

   char smiobj[128];
   strcpy(smiobj, argv[1]);
   smi_attach(smiobj, SMI_handle_command);
   smi_volatile();
   smi_set_state("RUNNING");

   for (int i = 1; i < argc-1; i++)
      argv[i] = argv[i+1];
   argc--;

#else
   // Handle command line arguments
   if ((argc == 2 && (!strcmp(argv[1], "-?") || !strcmp(argv[1], "-help"))) ||
       argc > 5 || argc < 4) {
      Usage(argv[0]);
      return 1;
   }
#endif

   Bool_t   useRFIO = kFALSE, useROOTD = kFALSE, useDEVNULL = kFALSE;
   Bool_t   useFilter = kFALSE, useLoop = kFALSE;
   Bool_t   delFiles = kFALSE;
   Int_t    fd = -1, compress;
   Double_t maxFileSize;

   // no special arg checking so don't make errors
   if (argv[1][0] == '-') {
      delFiles = kTRUE;
      maxFileSize = atoi(argv[1]+1);
   } else
      maxFileSize = atoi(argv[1]);
   if (maxFileSize < 1000 || maxFileSize > 2.e9) {
      Error(argv[0], "unreasonable file size %f\n", maxFileSize);
      return 1;
   }

   int filter = atoi(argv[2]);
   if (filter != 0)
      useFilter = kTRUE;

   if (argv[3][0] == '-') {
      useRFIO = kTRUE;
      compress = atoi(argv[3]+1);
   } else if (argv[3][0] == '+') {
      useROOTD = kTRUE;
      compress = atoi(argv[3]+1);
   } else if (argv[3][0] == '@') {
      useDEVNULL = kTRUE;
      compress = atoi(argv[3]+1);
   } else
      compress = atoi(argv[3]);
   if (compress > 9) {
      Error(argv[0], "unreasonable compression mode %d\n", compress);
      return 1;
   }

   if (argc == 5) {
      char *file = argv[4];
      if (argv[4][0] == '-') {
         useLoop = kTRUE;
         file = argv[4]+1;
      }
      if ((fd = open(file, O_RDONLY)) == -1) {
         Error(argv[0], "cannot open input file %s", argv[4]);
         return 1;
      }
   }

   // Create MDC processor object and process input stream
   AliMDC mdcproc(fd, compress, maxFileSize, useFilter, useRFIO, useROOTD,
                  useDEVNULL, useLoop, delFiles);

#ifdef USE_DEBUG
   mdcproc.SetDebugLevel(3);
#endif

   return mdcproc.Run();
}
