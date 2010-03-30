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
 *                                                                        *
 **************************************************************************/

/********************************************************************************
*                                                                               *
*   AliLHCData: summary of the LHC related information from LHC DIP.            *
*   Created from the TMap provided by the AliLHCReader with optional beginning  *
*                                                                               *
*   The data are (wrapped in the AliLHCDipValT):                                *
*   made of TimeStamp (double) and array of values                              *
*                                                                               *
*   Multiple entries for each type of data are possible. To obtaine number of   *
*   records (with distinct timestamp) for give type od records use:             *
*   int GetNBunchConfigMeasured(int beam) (with beam=0,1) etc.                  *
*                                                                               *
*   To get i-th entry, use brec= AliLHCDipValI* GetBunchConfigMeasured(bm,i);   *
*   Note: exact type of templated AliLHCDipValT pointer depends on the record   *
*   type, concult getters to know it.                                           *
*                                                                               *
*   Then, once the pointer is obtained, details can be accessed:                *
*   int nBunches = brec->GetSize();                                             *
*   for (int i=0;i<nBunches;i++) printf("Bunch#%d: %d\n",i,(*brec)[i]);         *
*                                                                               *
*                                                                               *
*   Author: ruben.shahoyan@cern.ch                                              *
*                                                                               *
********************************************************************************/

#include "AliLHCData.h"
#include "TMap.h"
#include "AliDCSArray.h"
#include <TString.h>
#include <TObjArray.h>

ClassImp(AliLHCData)

const Char_t* AliLHCData::fgkDCSNames[] = {
  "LHC_IntensityBeam%d_totalIntensity",
  "LHC_BeamIntensityPerBunchBeam%d_averageBeamIntensity",
  "LHC_BeamIntensityPerBunchBeam%d_Average_BunchIntensities",
  //
  "LHC_LumAverageBRANB_4%c2_acqMode",
  "LHC_LumAverageBRANB_4%c2_meanLuminosity",
  "LHC_LumAverageBRANB_4%c2_meanLuminosityError",
  "LHC_BeamLuminosityPerBunchBRANB_4%c2_Average_BunchLuminosity",
  "LHC_BeamLuminosityPerBunchBRANB_4%c2_BunchLuminosityError",
  "LHC_LumAverageBRANB_4%c2_meanCrossingAngle",
  "LHC_LumAverageBRANB_4%c2_meanCrossingAngleError",
  "LHC_CirculatingBunchConfig_Beam%d",
  "LHC_FillNumber",
  //
  "LHC_BunchLengthBeam%d_nBunches",
  "LHC_BunchLengthBeam%d_bunchesLenghts",
  "LHC_BunchLengthBeam%d_filledBuckets",
  //
  "LHC_RunControl_ActiveInjectionScheme",
  "LHC_RunControl_BetaStar",
  "LHC_RunControl_IP2_Xing_Murad",
  "LHC_RunControl_IP2_ALICE_Murad",

  "LHC_BeamSizeBeam%d_acqMode",
  "LHC_BeamSizeBeam%d_sigmaH",
  "LHC_BeamSizeBeam%d_sigmaV",
  "LHC_BeamSizeBeam%d_emittanceH",
  "LHC_BeamSizeBeam%d_emittanceV",
  "LHC_BeamSizeBeam%d_errorSigmaH",
  "LHC_BeamSizeBeam%d_errorSigmaV",
  //
  "LHC_CollimatorPos_%s_lvdt_%s"
};

const Char_t* AliLHCData::fgkDCSColNames[] = {
  "TCTVB_4L2",
  "TCTVB_4R2",
  "TCLIA_4R2"
};

const Char_t* AliLHCData::fgkDCSColJaws[] = {
  "gap_downstream","gap_upstream","left_downstream",
  "left_upstream","right_downstream","right_upstream"};

//___________________________________________________________________
AliLHCData::AliLHCData(const TMap* dcsMap, double tmin, double tmax)
  : fTMin(tmin),fTMax(tmax),fFillNumber(0),fData(0)
{
  Clear();
  SetTMin(tmin);
  SetTMin(tmax);
  FillData(dcsMap);
}

//___________________________________________________________________
Bool_t AliLHCData::FillData(const TMap* dcsMap, double tmin, double tmax)
{
  // process DCS map and fill all fields. 
  // Accept only entries with timestamp between tmin and tmax
  //
  char buff[100],buff1[100];
  //
  SetTMin(tmin);
  SetTMax(tmax);
  //
  // -------------------------- extract Fill Number
  int iEntry;
  TObjArray* arr = GetDCSEntry(dcsMap, fgkDCSNames[kFillNum],iEntry,fTMin,fTMax);
  if (arr && iEntry>=0) SetFillNumber( ExtractInt( (AliDCSArray*)arr->At(iEntry), 0) );
  //
  for (int ibm=0;ibm<2;ibm++) {
    //
    sprintf(buff,fgkDCSNames[kBunchConf],ibm+1);         // ----- declared bunch configuration
    FillBunchConfig(dcsMap, fBunchConfDecl[ibm], buff);
    //
    sprintf(buff,fgkDCSNames[kBunchLgtFillB],ibm+1);     // ----- measured bunch configuration
    FillBunchConfig(dcsMap, fBunchConfMeas[ibm], buff);
    //
    sprintf(buff,fgkDCSNames[kBunchLgt],ibm+1);          // ----- maesured bunch lenghts
    FillBunchInfo(dcsMap, fBunchLengths[ibm],buff,ibm,kFALSE);  
    //
    sprintf(buff,fgkDCSNames[kIntBunchAv],ibm+1);        // ----- B-by-B intensities
    FillBunchInfo(dcsMap, fIntensPerBunch[ibm],buff,ibm,kTRUE);
    //
    //
    sprintf(buff,fgkDCSNames[kIntTot],ibm+1);            // ----- total intensities for beam 1 and 2
    FillScalarRecord(dcsMap, fIntensTotal[ibm], buff);
    //
    sprintf(buff,fgkDCSNames[kIntTotAv],ibm+1);          // ----- total intensities for beam 1 and 2 from B-by-B average
    FillScalarRecord(dcsMap, fIntensTotalAv[ibm], buff);
    //
    sprintf(buff,fgkDCSNames[kBeamSzEmittH],ibm+1);      // ----- H emittance for beam 1 and 2 
    FillScalarRecord(dcsMap, fEmittanceH[ibm], buff);
    //
    sprintf(buff,fgkDCSNames[kBeamSzEmittV],ibm+1);      // ----- V emittance for beam 1 and 2 
    FillScalarRecord(dcsMap, fEmittanceV[ibm], buff);
    //
    sprintf(buff ,fgkDCSNames[kBeamSzSigH],   ibm+1);    // ----- H sigmas and errors for beam 1 and 2 
    sprintf(buff1,fgkDCSNames[kBeamSzSigHErr],ibm+1);
    FillScalarRecord(dcsMap, fBeamSigmaH[ibm], buff, buff1);
    //
    sprintf(buff ,fgkDCSNames[kBeamSzSigV],   ibm+1);    // ----- V sigmas and errors for beam 1 and 2 
    sprintf(buff1,fgkDCSNames[kBeamSzSigVErr],ibm+1);
    FillScalarRecord(dcsMap, fBeamSigmaV[ibm], buff, buff1);
    //
  }
  //
  for (int ilr=0;ilr<2;ilr++) {
    //
    sprintf(buff ,fgkDCSNames[kLumBunch], ilr ? 'R':'L');       // ---- BC-by-BC luminosity at IP2 and its error
    sprintf(buff1,fgkDCSNames[kLumBunchErr], ilr ? 'R':'L');
    FillBCLuminosities(dcsMap, fLuminPerBC[ilr], buff, buff1, kTRUE);
    //
    sprintf(buff ,fgkDCSNames[kLumTot]   , ilr ? 'R':'L');       // ---- total luminosity at IP2 and its error
    sprintf(buff1,fgkDCSNames[kLumTotErr], ilr ? 'R':'L');
    FillScalarRecord(dcsMap, fLuminTotal[ilr], buff, buff1);
    //
    sprintf(buff ,fgkDCSNames[kLumAcqMode], ilr ? 'R':'L');      // ---- luminosity acquisition mode
    FillAcqMode(dcsMap, fLuminAcqMode[ilr], buff);
    //
    sprintf(buff, fgkDCSNames[kLumCrossAng]   , ilr ? 'R':'L');  //----- crossing angle at IP2 and its error
    sprintf(buff1,fgkDCSNames[kLumCrossAngErr], ilr ? 'R':'L');
    FillScalarRecord(dcsMap, fCrossAngle[ilr], buff, buff1);
    //    
  }
  //
  for (int icl=0;icl<kNCollimators;icl++) {             // ----- collimators positions
    for (int jaw=0;jaw<kNJaws;jaw++) {
      sprintf(buff,fgkDCSNames[kCollPos], fgkDCSColNames[icl],fgkDCSColJaws[jaw]);        
      FillScalarRecord(dcsMap, fCollimators[icl][jaw], buff);
    } // jaws
  } // collimators
  //
  //
  // RunControl info
  FillStringRecord(dcsMap, fRCInjScheme, fgkDCSNames[kRCInjSch]);   // ---- active injection scheme
  FillScalarRecord(dcsMap, fRCBeta, fgkDCSNames[kRCBeta]);          // ---- target beta 
  FillScalarRecord(dcsMap, fRCAngH, fgkDCSNames[kRCCrossAng]);      // ---- horisontal angle
  FillScalarRecord(dcsMap, fRCAngV,fgkDCSNames[kRCVang] );          // ---- vertical angle
  //
  return kTRUE;
}

//___________________________________________________________________
TObjArray* AliLHCData::GetDCSEntry(const TMap* dcsMap,const char* key,int &entry,double tmin,double tmax) const
{
  // extract array from the DCS map and find the first entry within the time limits
  entry = -1;
  TObjArray* arr = (TObjArray*)dcsMap->GetValue(key);
  if (!arr || !arr->GetEntriesFast()) { 
    AliWarning(Form("No data for %s",key)); 
    return 0;
  }
  int ntot = arr->GetEntriesFast();
  for (entry=0;entry<ntot;entry++) {
    AliDCSArray* ent = (AliDCSArray*)arr->At(entry);
    if (ent->GetTimeStamp()>=tmin && ent->GetTimeStamp()<=tmax) break;
  }
  if (entry==ntot) {
    entry = -1;
    TString str;
    str += AliLHCDipValD::TimeAsString(tmin);
    str += " : ";
    str += AliLHCDipValD::TimeAsString(tmax);
    AliWarning(Form("All entries for %s are outside the requested range:\n%s",key,str.Data()));
  }
  return arr;
}

//___________________________________________________________________
Int_t AliLHCData::TimeDifference(double v1,double v2,double tol) const
{
  // return 0 if the times are the same within the tolerance
  //        1 if v1>v2
  //       -1 if v1<v2
  v1-=v2;
  if (v1>tol)  return  1;
  if (v1<-tol) return -1;
  return 0;
}

//___________________________________________________________________
Bool_t AliLHCData::GoodPairID(int beam) const
{
  // check for correct beam identifier 
  if (beam>kBeam2||beam<0) {AliError(Form("BeamID can be 0 or 1, %d requested",beam)); return kFALSE;}
  return kTRUE;
}

//___________________________________________________________________
AliLHCDipValI* AliLHCData::GetBunchConfigMeasured(int beam,double tstamp) const
{
  // find measured bunch configuration valid for given tstamp
  if (!GoodPairID(beam)) return 0;
  return (AliLHCDipValI*)FindRecValidFor(fBunchConfMeas[beam][kStart],fBunchConfMeas[beam][kNStor],tstamp);
}

//___________________________________________________________________
AliLHCDipValI* AliLHCData::GetBunchConfigDeclared(int beam,double tstamp) const
{
  // find declared bunch configuration valid for given tstamp
  if (!GoodPairID(beam)) return 0;
  return (AliLHCDipValI*)FindRecValidFor(fBunchConfDecl[beam][kStart],fBunchConfDecl[beam][kNStor],tstamp);
}

//___________________________________________________________________
TObject* AliLHCData::FindRecValidFor(int start,int nrec, double tstamp) const
{
  // find record within this limits valid for given tstamp (i.e. the last one before or equal to tstamp)
  AliLHCDipValI *prevObj = 0;
  for (int i=0;i<nrec;i++) {
    AliLHCDipValI* curObj = (AliLHCDipValI*)fData[start+i];
    if (TimeDifference(tstamp,curObj->GetTimeStamp())>0) break;
    prevObj = curObj;
  }
  if (!prevObj && nrec>0) prevObj = (AliLHCDipValI*)fData[start]; // if no exact match, return the 1st one
  return prevObj;
}

//___________________________________________________________________
Int_t AliLHCData::FillScalarRecord(const TMap* dcsMap, int refs[2], const char* rec, const char* recErr)
{
  // fill record for scalar value, optionally accompanied by measurement error 
  //
  AliInfo(Form("Acquiring record: %s",rec));
  //
  TObjArray *arr,*arrE;
  Int_t nEntries,nEntriesE,iEntry,iEntryE;
  //
  refs[kStart] = fData.GetEntriesFast();
  refs[kNStor] = 0;
  //
  if ( !(arr=GetDCSEntry(dcsMap,rec,iEntry,fTMin,fTMax)) || iEntry<0 ) return -1;
  nEntries = arr->GetEntriesFast();
  //
  int dim = 1;
  if (recErr) {
    if ( !(arrE=GetDCSEntry(dcsMap,recErr,iEntryE,fTMin,fTMax)) || iEntryE<0 ) nEntriesE = -999;
    else nEntriesE = arrE->GetEntriesFast();
    dim += 1;
  }
  //
  while (iEntry<nEntries) {
    AliDCSArray *dcsVal = (AliDCSArray*) arr->At(iEntry++);
    double tstamp = dcsVal->GetTimeStamp();
    if (tstamp>fTMax) break;
    //
    AliLHCDipValF* curValF = new AliLHCDipValF(dim,tstamp);  // start new period
    (*curValF)[0] = ExtractDouble(dcsVal,0);     // value
    //
    if (recErr) {
      double errVal = -1;
      while (iEntryE<nEntriesE) {       // try to find corresponding error
	AliDCSArray *dcsValE = (AliDCSArray*) arrE->At(iEntryE);
        double tstampE = dcsValE->GetTimeStamp();
        if (tstampE>fTMax) break;
        int tdif = TimeDifference(tstamp,tstampE);
        if (!tdif) { // error matches to value
          errVal = ExtractDouble(dcsValE,0);
	  iEntryE++; 
	  break;
	}
        else if (tdif>0) iEntryE++; // error time lags behind, read the next one
        else break;                 // error time is ahead of value, no error associated
      }
      (*curValF)[dim-1] = errVal;   // error
      curValF->SetLastSpecial();    // lable the last entry as an error
    }
    //
    fData.Add(curValF);
    refs[kNStor]++;
  }
  //
  return refs[kNStor];
}

//___________________________________________________________________
Int_t AliLHCData::FillBunchConfig(const TMap* dcsMap, int refs[2],const char* rec)
{
  // fill record for bunch configuration
  //
  AliInfo(Form("Acquiring record: %s",rec));
  TObjArray *arr;
  Int_t nEntries,iEntry;
  //
  refs[kStart] = fData.GetEntriesFast();
  refs[kNStor] = 0;
  //
  if ( !(arr=GetDCSEntry(dcsMap,rec,iEntry,fTMin,fTMax)) || iEntry<0 ) return -1;
  nEntries = arr->GetEntriesFast();
  //
  AliLHCDipValI* prevRecI=0;
  while (iEntry<nEntries) {
    AliDCSArray *dcsVal = (AliDCSArray*) arr->At(iEntry++);
    double tstamp = dcsVal->GetTimeStamp();
    if (tstamp>fTMax) break;
    //
    int bucket=0, nbunch=0, ndiff=0;
    int nSlots = dcsVal->GetNEntries();     // count number of actual bunches (non-zeros)
    int* dcsArr = dcsVal->GetInt();
    while(nbunch<nSlots && (bucket=dcsArr[nbunch])) {
      if (prevRecI && prevRecI->GetSize()>nbunch && bucket!=prevRecI->GetValue(nbunch)) ndiff++;
      nbunch++;
    }
    if (!nbunch) AliWarning(Form("%s record is present but empty: no beam?",rec));
    if (prevRecI && !ndiff && nbunch==prevRecI->GetSize()) continue; // record similar to previous one
    AliLHCDipValI* curValI = new AliLHCDipValI(nbunch,tstamp);      
    for (int i=nbunch;i--;) (*curValI)[i] = dcsArr[i];
    fData.Add(curValI);
    refs[kNStor]++;
    prevRecI = curValI;
  }
  //
  return refs[kNStor];
}
 
//___________________________________________________________________
Int_t AliLHCData::FillAcqMode(const TMap* dcsMap, int refs[2],const char* rec)
{
  // fill acquisition mode
  //
  AliInfo(Form("Acquiring record: %s",rec));
  TObjArray *arr;
  Int_t nEntries,iEntry;
  //
  refs[kStart] = fData.GetEntriesFast();
  refs[kNStor] = 0;
  //
  if ( !(arr=GetDCSEntry(dcsMap,rec,iEntry,fTMin,fTMax)) || iEntry<0 ) return -1;
  nEntries = arr->GetEntriesFast();
  //
  AliLHCDipValI* prevRecI=0;
  while (iEntry<nEntries) {
    AliDCSArray *dcsVal = (AliDCSArray*) arr->At(iEntry++);
    double tstamp = dcsVal->GetTimeStamp();
    if (tstamp>fTMax) break;
    //
    int nSlots = dcsVal->GetNEntries();
    if (nSlots<1) continue;
    int acqMode = dcsVal->GetInt()[0];
    if (prevRecI && (*prevRecI)[0] == acqMode) continue; // record similar to previous one
    AliLHCDipValI* curValI = new AliLHCDipValI(1,tstamp);      
    (*curValI)[0] = acqMode;
    fData.Add(curValI);
    refs[kNStor]++;
    prevRecI = curValI;
  }
  //
  return refs[kNStor];
}
 
//___________________________________________________________________
Int_t AliLHCData::FillStringRecord(const TMap* dcsMap, int refs[2],const char* rec)
{
  // fill record with string value
  //
  AliInfo(Form("Acquiring record: %s",rec));
  TString prevRec;
  TObjArray *arr;
  Int_t nEntries,iEntry;
  //
  refs[kStart] = fData.GetEntriesFast();
  refs[kNStor] = 0;
  //
  if ( !(arr=GetDCSEntry(dcsMap,rec,iEntry,fTMin,fTMax)) || iEntry<0 ) return -1;
  nEntries = arr->GetEntriesFast();
  //
  while (iEntry<nEntries) {
    AliDCSArray *dcsVal = (AliDCSArray*) arr->At(iEntry++);
    double tstamp = dcsVal->GetTimeStamp();
    if (tstamp>fTMax) break;
    //
    TString &str = ExtractString(dcsVal);
    if (!prevRec.IsNull()) {if (str == prevRec) continue;} // skip similar record
    else prevRec = str;
    //
    AliLHCDipValC* curValS = new AliLHCDipValC(1,tstamp);      
    curValS->SetValues(str.Data(),str.Length()+1);
    //
    fData.Add(curValS);
    refs[kNStor]++;
  }
  return refs[kNStor];
}

//___________________________________________________________________
Int_t AliLHCData::FillBunchInfo(const TMap* dcsMap, int refs[2],const char* rec, int ibm, Bool_t inRealSlots)
{
  // fill bunch properties for beam ibm
  // if inRealSlots = true, then the value is taken from bunchRFbucket/10, otherwise, the value 
  // for the i-th bunch is taken from the i-th element
  //
  AliInfo(Form("Acquiring record: %s",rec));
  TObjArray *arr;
  Int_t nEntries,iEntry;
  //
  refs[kStart] = fData.GetEntriesFast();
  refs[kNStor] = 0;
  //
  if ( !(arr=GetDCSEntry(dcsMap,rec,iEntry,fTMin,fTMax)) || iEntry<0 ) return -1;
  nEntries = arr->GetEntriesFast();
  //
  while (iEntry<nEntries) {
    AliDCSArray *dcsVal = (AliDCSArray*) arr->At(iEntry++);
    double tstamp = dcsVal->GetTimeStamp();
    if (tstamp>fTMax) break;
    //
    AliLHCDipValI *bconf = GetBunchConfigMeasured(ibm,tstamp);
    if (!bconf) {
      AliWarning(Form("Mearured bunch configuration for beam %d at t=%.1f is not available, trying declared one",ibm+1,tstamp));
      bconf = GetBunchConfigDeclared(ibm,tstamp);
    }
    if (!bconf) {
      AliWarning(Form("Declared bunch configuration for beam %d at t=%.1f is not available, skip this record",ibm+1,tstamp));
      return -1;
    }
    int nSlots = dcsVal->GetNEntries();     // count number of actual bunches (non-zeros)
    int nbunch = bconf->GetSize();
    if (nbunch>nSlots) {
      AliWarning(Form("More N bunches than slots in %s at time %.1f",rec,tstamp));
      continue;
    }
    double* dcsArr = dcsVal->GetDouble();
    AliLHCDipValF* curValF = new AliLHCDipValF(nbunch,tstamp);
    for (int i=nbunch;i--;) {
      int ind = inRealSlots ? (*bconf)[i]/10 : i;
      if (ind>nSlots) {
	AliError(Form("Bunch %d refers to wrong slot %d, set to -1",i,(*bconf)[i]));
	(*curValF)[i] = -1;
      }
      else (*curValF)[i] = dcsArr[ind];
    }
    fData.Add(curValF);
    refs[kNStor]++;
  }
  return refs[kNStor];
  //
}
 
//___________________________________________________________________
Int_t AliLHCData::FillBCLuminosities(const TMap* dcsMap, int refs[2],const char* rec, const char* recErr, Bool_t opt)
{
  // fill luminosities per bunch crossing
  //
  AliInfo(Form("Acquiring record: %s",rec));
  TObjArray *arr,*arrE;
  Int_t nEntries,nEntriesE,iEntry,iEntryE;
  //
  refs[kStart] = fData.GetEntriesFast();
  refs[kNStor] = 0;
  //
  if ( !(arr=GetDCSEntry(dcsMap,rec,iEntry,fTMin,fTMax)) || iEntry<0 ) return -1;
  nEntries = arr->GetEntriesFast();
  //
  while (iEntry<nEntries) {
    AliDCSArray *dcsVal = (AliDCSArray*) arr->At(iEntry++);
    double tstamp = dcsVal->GetTimeStamp();
    if (tstamp>fTMax) break;
    //
    AliLHCDipValI *bconf = GetBunchConfigMeasured(0,tstamp);  // luminosities are stored according to 1st beam bunches
    if (!bconf) {
      AliWarning(Form("Mearured bunch configuration for beam 1 at t=%.1f is not available, trying declared one",tstamp));
      bconf = GetBunchConfigDeclared(0,tstamp);
    }
    if (!bconf) {
      AliWarning(Form("Declared bunch configuration for beam 1 at t=%.1f is not available, skip this record",tstamp));
      return -1;
    }
    int nSlots = dcsVal->GetNEntries();     // count number of actual bunches (non-zeros)
    int nbunch = bconf->GetSize();
    double* dcsArr = dcsVal->GetDouble();
    //
    // ATTENTION: FOR THE MOMENT STORE ALL SLOTS CORRESPONDING TO FILLED BUNCHES (until the scheme is clarified)
    if (nbunch>nSlots) {
      AliWarning(Form("More N bunches than slots in %s at time %.1f",rec,tstamp));
      continue;
    }
    int dim = nbunch;
    if (recErr) {
      if ( !(arrE=GetDCSEntry(dcsMap,recErr,iEntryE,fTMin,fTMax)) || iEntryE<0 ) nEntriesE = -999;
      else nEntriesE = arrE->GetEntriesFast();
      dim += 1;
    }
    AliLHCDipValF* curValF = new AliLHCDipValF(dim,tstamp);
    for (int i=nbunch;i--;) {
      int ind = opt ? (*bconf)[i]/10 : i;
      if (ind>nSlots) {
	AliError(Form("Bunch %d refers to wrong slot %d, set to -1",i,(*bconf)[i]));
	(*curValF)[i] = -1;
      }
      else (*curValF)[i] = dcsArr[ind];
    }
    //
    if (recErr) {
      double errVal = -1;
      while (iEntryE<nEntriesE) {       // try to find corresponding error
	AliDCSArray *dcsValE = (AliDCSArray*) arrE->At(iEntryE);
	double tstamp1 = dcsValE->GetTimeStamp();
	if (tstamp1>fTMax) break;
	int tdif = TimeDifference(tstamp,tstamp1);
	if (!tdif) { // error matches to value
	  errVal = dcsValE->GetDouble()[0];
	  iEntryE++; 
	  break;
	}
	else if (tdif>0) iEntryE++; // error time lags behind, read the next one
	else break;                 // error time is ahead of value, no error associated
      }
      (*curValF)[dim-1] = errVal;   // error
      curValF->SetLastSpecial();    // lable the last entry as an error
    }
    //
    fData.Add(curValF);
    refs[kNStor]++;
  }
  return refs[kNStor];
  //
}

//___________________________________________________________________
Int_t AliLHCData::ExtractInt(AliDCSArray* dcsArray,Int_t el) const
{
  // extract integer from the dcsArray
  int val = -1;
  //
  int sz = dcsArray->GetNEntries();
  if (sz<=el) return val;
  //
  if (dcsArray->GetType()==AliDCSArray::kInt)  val = dcsArray->GetInt(el);
  else if (dcsArray->GetType()==AliDCSArray::kString) {
    TObjString *stro = dcsArray->GetStringArray(el);
    if (stro) val = stro->GetString().Atoi();
    else AliError(Form("DCSArray TObjString for element %d is missing",el));
  }
  else if (dcsArray->GetType()==AliDCSArray::kUInt) val = dcsArray->GetUInt(el);
  else AliError(Form("Integer requested from DCSArray of type %d",dcsArray->GetType()));
  return val;
}

//___________________________________________________________________
Double_t AliLHCData::ExtractDouble(AliDCSArray* dcsArray,Int_t el) const
{
  // extract double from the dcsArray
  double val = 0;
  //
  int sz = dcsArray->GetNEntries();
  if (sz<=el) return val;
  //
  if      (dcsArray->GetType()==AliDCSArray::kDouble) val = dcsArray->GetDouble(el);
  else if (dcsArray->GetType()==AliDCSArray::kFloat)  val = dcsArray->GetFloat(el);
  else if (dcsArray->GetType()==AliDCSArray::kString) {
    TObjString *stro = dcsArray->GetStringArray(el);
    if (stro) val = stro->GetString().Atof();
    else AliError(Form("DCSArray has TObjString for element %d is missing",el));
  }
  else if (dcsArray->GetType()==AliDCSArray::kChar)   val = dcsArray->GetChar(el);
  else if (dcsArray->GetType()==AliDCSArray::kInt)    val = dcsArray->GetInt(el);
  else if (dcsArray->GetType()==AliDCSArray::kUInt)   val = dcsArray->GetUInt(el);
  else     AliError(Form("Double requested from DCSArray of type %d",dcsArray->GetType()));
  return val;
}

//___________________________________________________________________
TString& AliLHCData::ExtractString(AliDCSArray* dcsArray) const
{
  // extract string from the dcsArray
  static TString str;
  str = "";
  //
  int sz = dcsArray->GetNEntries();
  if (dcsArray->GetType()!=AliDCSArray::kString)  {
    AliError(Form("String requested from DCSArray of type %d",dcsArray->GetType()));
    return str;
  }
  //
  for (int i=0;i<sz;i++) {
    str += dcsArray->GetStringArray(i)->GetString();
    if (i<sz-1) str += " ";
  }
  return str;
}

//___________________________________________________________________
void AliLHCData::Print(const Option_t* opt) const
{
  // print full info
  TString opts = opt;
  opts.ToLower();
  Bool_t full = kTRUE;
  if (!opts.Contains("f")) {
    printf("Use Print(\"f\") to print full info\n");
    printf("Printing short info:\n<RecordType>(number of records): <TimeStamp, value> for 1st record only\n");
    full = kFALSE;
  }
  printf("Fill#%6d Validity: %s - %s\n",fFillNumber,
	 AliLHCDipValI::TimeAsString(fTMin),AliLHCDipValI::TimeAsString(fTMax));
  //
  printf("********** SETTINGS FROM RUN CONTROL **********\n");
  //
  printf("** Injection Scheme");
  PrintAux(full,fRCInjScheme);
  //
  printf("** Beta Star");
  PrintAux(full,fRCBeta);
  //
  printf("** Horisontal Crossing Angle");
  PrintAux(full,fRCAngH);
  //
  printf("** Vertical   Crossing Angle");
  PrintAux(full,fRCAngV);
  //
  for (int ib=0;ib<2;ib++) {
    printf("** Beam%d bunch filling scheme [negative: bunch interacting at IR2!]",ib+1);
    PrintAux(full,fBunchConfDecl[ib]);
  }
  //
  printf("\n**********       MEASURED DATA       **********\n");
  //
  for (int ib=0;ib<2;ib++) {
    printf("** Beam%d bunch filling scheme [negative: bunch interacts at IR2!]",ib+1);
    PrintAux(full,fBunchConfMeas[ib]);
  } 
  //
  for (int ib=0;ib<2;ib++) {
    printf("** Beam%d total intensity",ib+1);
    PrintAux(full,fIntensTotal[ib]);
  } 
  //
  for (int ib=0;ib<2;ib++) {
    printf("** Beam%d total intensity from bunch average",ib+1);
    PrintAux(full,fIntensTotalAv[ib]);
  } 
  //
  for (int ib=0;ib<2;ib++) {
    printf("** Beam%d intensity per bunch",ib+1);
    PrintAux(full,fIntensPerBunch[ib]);
  }
  //
  for (int ib=0;ib<2;ib++) {
    printf("** Beam%d bunch lengths",ib+1);
    PrintAux(full,fBunchLengths[ib]);
  } 
  //
  for (int ib=0;ib<2;ib++) {
    printf("** Beam%d H. emittance",ib+1);
    PrintAux(full,fEmittanceH[ib]);
  }
  //
  for (int ib=0;ib<2;ib++) {
    printf("** Beam%d V. emittance",ib+1);
    PrintAux(full,fEmittanceV[ib]);
  }
  //
  for (int ib=0;ib<2;ib++) {
    printf("** Beam%d H. sigma",ib+1);
    PrintAux(full,fBeamSigmaH[ib]);
  }
  //
  for (int ib=0;ib<2;ib++) {
    printf("** Beam%d V. sigma",ib+1);
    PrintAux(full,fBeamSigmaV[ib]);
  }
  //
  for (int lr=0;lr<2;lr++) {
    printf("** Total luminosity from BRANB_4%c2",lr ? 'R':'L');
    PrintAux(full,fLuminTotal[lr]);
  } 
  //
  for (int lr=0;lr<2;lr++) {
    printf("** Luminosity acquisition mode, BRANB_4%c2",lr ? 'R':'L');
    PrintAux(full,fLuminPerBC[lr]);
  } 
  //
  for (int lr=0;lr<2;lr++) {
    printf("** Luminosity per Bunch Crossing from BRANB_4%c2",lr ? 'R':'L');
    PrintAux(full,fLuminPerBC[lr]);
  }
  //
  for (int lr=0;lr<2;lr++) {
    printf("** Crossing angle, side %c",lr ? 'R':'L');
    PrintAux(full,fCrossAngle[lr]);
  }
  //
  for (int coll=0;coll<kNCollimators;coll++)
    for (int jaw=0;jaw<kNJaws;jaw++) {
      printf("** Collimator %s:%s",fgkDCSColNames[coll],fgkDCSColJaws[jaw]);
      PrintAux(full,fCollimators[coll][jaw]);
    }
  //
}

//___________________________________________________________________
void AliLHCData::PrintAux(Bool_t full, const Int_t refs[2]) const
{
  // aux method to print the reocrds of the same type
  int nrec = refs[kNStor];
  if (nrec<1) {
    printf(": N/A\n"); 
    return;
  }
  printf(": (%d):\t",nrec); // number of records
  if (!full) nrec = 1;
  int sz = ((AliLHCDipValI*)fData[refs[kStart]])->GetSizeTotal(); // dimension of the record
  Bool_t isStr = ((AliLHCDipValI*)fData[refs[kStart]])->IsTypeC();
  if ((!isStr && sz>2) || nrec>1) printf("\n"); // long record, open new line
  for (int i=0;i<nrec;i++) fData[refs[kStart]+i]->Print();
  //
}

//___________________________________________________________________
void AliLHCData::Clear(const Option_t *)
{
  // clear all info
  fData.Delete();
  fFillNumber = 0;
  fTMin = 0;
  fTMax = 1e10;
  for (int i=2;i--;) {
    fRCInjScheme[i] = 0;
    fRCBeta[i] = 0;
    fRCAngH[i] = 0;
    fRCAngV[i] = 0;
    //
    for (int icl=kNCollimators;icl--;) for (int jaw=kNJaws;jaw--;) fCollimators[icl][jaw][i]=0;
    //
    for (int j=2;j--;) {
      fBunchConfDecl[j][i] = 0;
      fBunchConfMeas[j][i] = 0;
      fBunchLengths[j][i] = 0;
      fIntensTotal[j][i] = 0;
      fIntensTotalAv[j][i] = 0;
      fIntensPerBunch[j][i] = 0;      
      fCrossAngle[j][i] = 0;
      fEmittanceH[j][i] = 0;
      fEmittanceV[j][i] = 0;
      fBeamSigmaH[j][i] = 0;
      fBeamSigmaV[j][i] = 0;
      fLuminTotal[j][i] = 0;
      fLuminPerBC[j][i] = 0;
      fLuminAcqMode[j][i] = 0;
    }
  }
}

//___________________________________________________________________
Int_t AliLHCData::GetNInteractingBunchesMeasured(int i) const
{
  // get number of interacting bunches at IR2
  AliLHCDipValI* rec = GetBunchConfigMeasured(kBeam1,i);
  if (!rec) {AliInfo(Form("No record %d found",i)); return -1;}
  if (!rec->IsProcessed1()) { AliInfo("Interacting bunches were not marked"); return -1;}
  int n = 0;
  for (int i=rec->GetSize();i--;) if ( (*rec)[i]<0 ) n++;
  return n;
}

//___________________________________________________________________
Int_t AliLHCData::GetNInteractingBunchesDeclared(int i) const
{
  // get number of interacting bunches at IR2
  AliLHCDipValI* rec = GetBunchConfigMeasured(kBeam1,i);
  if (!rec) {AliInfo(Form("No record %d found",i)); return -1;}
  if (!rec->IsProcessed1()) { AliInfo("Interacting bunches were not marked"); return -1; }
  int n = 0;
  for (int i=rec->GetSize();i--;) if ( (*rec)[i]<0 ) n++;
  return n;
}

//___________________________________________________________________
Int_t AliLHCData::IsPilotPresent(int i) const
{
  // check in the filling scheme is the pilot bunch is present
  AliLHCDipValC* rec = GetInjectionScheme();
  if (!rec) {AliInfo(Form("No record %d found",i)); return -1;}
  TString scheme = rec->GetValues();
  return scheme.Contains("wp",TString::kIgnoreCase);
}
