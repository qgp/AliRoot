
// This macro transfers OCDB data of one single ("detName") or all ALICE detectors from one location to another, 
// It is possible to set new run range, path etc... 

void CDBToGrid(const char *detName="", const char* fromUri="local://$ALICE_ROOT", 
	const char* toUri="local://newOCDB"){

AliCDBManager *man = AliCDBManager::Instance();

man->SetDefaultStorage(fromUri);

AliCDBStorage *dest = man->GetStorage(toUri);

man->SetRun(0);

if(detName == ""){
// drain everything
	TList *calib = man->GetAll("*/Calib/*");
	TList *align = man->GetAll("*/Align/Data");
} else {
// drain calibration and aliignment for detector "detName"

	TString calibPath = detName;
	TString alignPath = calibPath;
	calibPath+="/Calib/*";
	alignPath+="/Align/Data";

	TList *calib = man->GetAll(calibPath);
	TList *align = man->GetAll(alignPath);
}

Int_t ok=0;
TString failed="";
AliCDBEntry *entry;
for(int i=0;i<calib->GetEntries();i++){

	entry = (AliCDBEntry*) calib->At(i);
	
	// change run range
	entry->GetId().SetRunRange(0,999999999);

	TString path=entry->GetId().GetPath();
	if (path == "ITS/Resp/RespSDD") entry->GetId().SetPath("ITS/Calib/RespSDD"); // bug in ITS/Resp/RespSDD

      	if (dest->Put(entry)) {
      		ok++;
      	} else {
      		failed += path.Data(); failed += " "; 
      	}
}
printf("************ Calib done. Stored %d objects over %d *********** \n", ok, calib->GetEntries());
printf("***** List of failed objects: %s \n", failed.Data());

 
for(int i=0;i<align->GetEntries();i++){
	entry = (AliCDBEntry*) align->At(i);
	entry->GetId().SetRunRange(0,999999999);
      	if (dest->Put(entry)) {
      		ok++;
      	} else {
      		failed += path.Data(); failed += " "; 
      	}
}
printf("************ Align done. Stored %d objects over %d *********** \n", ok, align->GetEntries());
printf("***** List of failed objects: %s \n", failed.Data());


man->Destroy();

}
