// --- ROOT system
#include <TFile.h>
#include <TTimeStamp.h>

#include "AliPMDPreprocessor.h"
#include "AliPMDCalibData.h"
#include "AliLog.h"
#include "AliShuttleInterface.h"
#include "AliCDBMetaData.h"
#include <TTimeStamp.h>
#include <TObjString.h>
#include <TTree.h>
#include <TSystem.h>


ClassImp(AliPMDPreprocessor)

//______________________________________________________________________________________________
AliPMDPreprocessor::AliPMDPreprocessor(AliShuttleInterface* shuttle) :
  AliPreprocessor("PMD", shuttle)
{
  // constructor
}

//______________________________________________________________________________________________
AliPMDPreprocessor::~AliPMDPreprocessor()
{
  // destructor
}

//______________________________________________________________________________________________
void AliPMDPreprocessor::Initialize(Int_t run, UInt_t startTime,
	UInt_t endTime)
{
  // Creates AliPMDDataDAQ object

  AliPreprocessor::Initialize(run, startTime, endTime);

	AliInfo(Form("\n\tRun %d \n\tStartTime %s \n\tEndTime %s", run,
		TTimeStamp(startTime).AsString(),
		TTimeStamp(endTime).AsString()));

 	fRun = run;
        fStartTime = startTime;
        fEndTime = endTime;

}

//______________________________________________________________________________________________
UInt_t AliPMDPreprocessor::Process(TMap* pdaqAliasMap)
{

 if(!pdaqAliasMap) return 1;
	
	AliPMDCalibData *calibda = new AliPMDCalibData();

        TList* filesources = GetFileSources(kDAQ, "PMDGAINS");

        if(!filesources) {
                Log(Form("No sources found for PMDGAINS!"));
                return 1;
        }

        AliInfo("Here's the list of sources for PMDGAINS");
        filesources->Print();

        TIter iter(filesources);
        TObjString* source;
        int i=0;
        UInt_t result = 0;
	TString filename;
        while((source=dynamic_cast<TObjString*> (iter.Next()))){
                filename = GetFile(kDAQ, "PMDGAINS", source->GetName());
                if(filename.Length() == 0) {
                        Log(Form("Error retrieving file from source %s failed!", source->GetName()));
                        delete filesources;
                        return 1;
                }

                Log(Form("File with id PMDGAINS got from %s", source->GetName()));
		Int_t DET,SM,ROW,COL;
		Float_t GAIN;
		TFile *f= new TFile(filename.Data());
		if(!f || !f->IsOpen()) 
		{
			Log(Form("Error opening file with Id PMDGAINS from source %s!", source->GetName()));
			return 1;
		} 
		TTree *tree = dynamic_cast<TTree *> (f->Get("ic"));
		if (!tree) 
		{
			Log("Could not find object \"ic\" in DAQ file!");
			return 1;
		}
		
   		tree->SetBranchAddress("DET",       &DET);
  		tree->SetBranchAddress("SM",        &SM);
  		tree->SetBranchAddress("ROW",        &ROW);
  		tree->SetBranchAddress("COL",        &COL);
  		tree->SetBranchAddress("GAIN",   &GAIN);
  		Int_t nEntries = (Int_t) tree->GetEntries();
  		for(Int_t i = 0; i < nEntries; i++)
    		{
      			tree->GetEntry(i);
//			if(DET>1 || SM>23 || ROW>95 || COL>95) {
//				printf("Error! gain[%d,%d,%d,%d] = %f\n",DET,SM,ROW,COL,GAIN);
//				continue;
 //    		 	}
			calibda->SetGainFact(DET,SM,ROW,COL,GAIN);
    		}
		f->Close();
		delete f;
  	}

  //Now we have to store the final CDB file
  AliCDBMetaData metaData;
  metaData.SetBeamPeriod(0);
  metaData.SetComment("test PMD preprocessor");

  result = Store("Calib","Data", calibda, &metaData);

 delete calibda;
 return result;
}

