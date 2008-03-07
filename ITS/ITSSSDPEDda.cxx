/**************************************************************************
- Contact: Oleksandr_Borysov aborysov@ts.infnf.it
- Link: /afs/infn.it/ts/user/efragiac/public/testCosm3125.001
- Run Type: 
- DA Type: LDC
- Number of events needed: >=500
- Input Files: ssdpeddaconfig, raw_data_file_on_LDC
- Output Files: ./ssddaldc_<LDCID>_<RunID>.root, FXS_name=ITSSSDda_<LDCID>_<RunID>.root 
                local files are persistent over runs: data source
- Trigger types used:
 **************************************************************************/


#include <iostream>
#include "TString.h"
#include "TFile.h"
#include "daqDA.h"
#include "AliITSHandleDaSSD.h" 
#include "TROOT.h"
#include "TPluginManager.h"

using namespace std;


int main( int argc, char** argv )
{
  AliITSHandleDaSSD  *ssddaldc;
  TString             feefname, fcdbsave;
  Int_t               status;
  Char_t             *dafname = NULL;


   gROOT->GetPluginManager()->AddHandler("TVirtualStreamerInfo",
                                        "*",
                                        "TStreamerInfo",
                                        "RIO",
                                        "TStreamerInfo()");



  /* check that we got some arguments = list of files */
  if (argc<2) {
    fprintf(stderr, "Wrong number of arguments\n");
    return -1;
  }

  char *datafilename = argv[1];

  ssddaldc = new AliITSHandleDaSSD(datafilename);
  if (ssddaldc->IsZombie()) return -1;
  if (!ssddaldc->ProcessRawData())
  {
     cerr << "Error !ssddaldc->ProcessRawData()" << endl;
     delete ssddaldc;
     return -1;
  }  
  daqDA_progressReport(90);

  if (!system(NULL)) {
    cerr << "Error: the call system(NULL) in main() returned NULL!" << endl;
    return -1;
  }
  dafname = ".";
  if (ssddaldc->SaveCalibrationSSDLDC(dafname)) {
    cout << "SSDDA data are saved in " << dafname << endl;
    status = daqDA_FES_storeFile(dafname, "CALIBRATION");
    if (status) fprintf(stderr, "Failed to export file : %d\n", status);
  } else cerr << "Error saving DA data to the file! Probably $DA_TEST_DIR defined incorrectly!" << endl;

  feefname.Form("%s/ssddaldc_%i_%i.root", ".", ssddaldc->GetLdcId(), ssddaldc->GetRunId());
  cout << "Saving feessdda data in " << feefname << endl;
  TFile *fileRun = new TFile (feefname.Data(),"RECREATE");
  if (fileRun->IsZombie()) {
    cerr << "Error open file " << feefname << endl;
    delete ssddaldc;
    delete fileRun;
    return 2;
  }  
  ssddaldc->Write();
  fileRun->Close();
  delete fileRun;

  fcdbsave.Form("ssddaldc_%i.root", ssddaldc->GetLdcId());
  status = daqDA_DB_storeFile(feefname.Data(), fcdbsave.Data());
  if (status) fprintf(stderr, "Failed to export file %s to the detector db: %d, %s \n", feefname.Data(), status, fcdbsave.Data());
  fcdbsave.Form("ssddaldc_%i_%i.root", ssddaldc->GetLdcId(), ssddaldc->GetRunId());
  status = daqDA_DB_storeFile(feefname.Data(), fcdbsave.Data());
  if (status) fprintf(stderr, "Failed to export file %s to the detector db: %d, %s \n", feefname.Data(), status, fcdbsave.Data());

  delete ssddaldc;
  daqDA_progressReport(100);
  return 0;
}
