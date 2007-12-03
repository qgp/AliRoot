#ifndef ALIITSPLANEEFF_H
#define ALIITSPLANEEFF_H
/* Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include <TObject.h>
#include <TString.h>
#include "AliLog.h"

class AliITSsegmentation;
class TF1;
class AliITSgeom;

////////////////////////////////////////////////////
//                                                //
// ITS virtual base class for Plane Efficiency    //
//                                                //
////////////////////////////////////////////////////

/* $Id$ */

class AliITSPlaneEff : public TObject {
 public:
 
    AliITSPlaneEff();// Default constructor
    // Standard constructor
    //AliITSPlaneEff(AliITSDetTypeSim *dettyp);
    virtual ~AliITSPlaneEff(){;};
    // copy constructor. See detector specific implementation.
    AliITSPlaneEff(const AliITSPlaneEff &source);
    // Assignment operator. See detector specific implementation.
    virtual AliITSPlaneEff& operator=(const AliITSPlaneEff &source);
    // Simple way to add another class (i.e. statistics). 
    //AliITSPlaneEff& operator +=( const AliITSPlaneEff &){return *this};
    // Average Plane efficiency (including dead/noisy)
    Int_t   GetRunNumber() const {return fRunNumber;}
    void    SetRunNumber(Int_t n) {fRunNumber=n;}
    //
    Double_t PlaneEff(Int_t nfound,Int_t ntried) const;     
    Double_t ErrPlaneEff(Int_t nfound,Int_t ntried) const; 
    virtual void GetPlaneEff(Int_t nfound,Int_t ntried,Double_t &eff, Double_t &err) const
        {eff=PlaneEff(nfound,ntried); err=ErrPlaneEff(nfound,ntried); return;};
    // Plane efficiency for active  detector (excluding dead/noisy channels)
    virtual Double_t LivePlaneEff(UInt_t) const
       {AliError("This method must be implemented in a derived class"); return -1.;};
    virtual Double_t ErrLivePlaneEff(UInt_t) const
       {AliError("This method must be implemented in a derived class"); return -1.;};
    // Compute the fraction of Live detector
    virtual Double_t GetFracLive(const UInt_t) const
       {AliError("This method must be implemented in a derived class"); return -1.;}; 
    // Compute the fraction of Bad (i.e. dead + noisy) detector 
    virtual Double_t GetFracBad(const UInt_t) const
       {AliError("This method must be implemented in a derived class"); return -1.;}; 
    // Update the Counting of the plane efficiency
    virtual Bool_t UpDatePlaneEff(const Bool_t, const UInt_t) 
       {AliError("This method must be implemented in a derived class"); return kFALSE;};
    // Estimate of the number of tracks needed for measuring efficiency within RelErr
    virtual Int_t GetNTracksForGivenEff(Double_t eff, Double_t RelErr) const;
    void SetDefaultStorage(const char* uri);
    // Write into the data base 
    virtual Bool_t WriteIntoCDB() const
       {AliError("This method must be implemented in a derived class"); return kFALSE;};
    virtual Bool_t ReadFromCDB()
       {AliError("This method must be implemented in a derived class"); return kFALSE;};
    virtual Bool_t AddFromCDB()
       {AliError("This method must be implemented in a derived class"); return kFALSE;};

 protected:

    void InitCDB();
    virtual void Copy(TObject &obj) const;
    void NotImplemented(const char *method) const {if(gDebug>0)
         Warning(method,"This method is not implemented for this sub-class");}
    Int_t	fRunNumber;	//! run number (to access CDB)
    TString	fCDBUri;	//! Uri of the default CDB storage
    Bool_t	fInitCDBCalled;	//! flag to check if CDB storages are already initialized
   
 private:
    //Int_t*	fFound;		// number of associated clusters into a given block (e.g. SPD 1200 chip)
    //Int_t*	fTries;		// number of exspected  clusters into a given block (e.g. SPD 1200 chip)
    //Int_t	fRunNumber;	// run number (to access CDB)

    ClassDef(AliITSPlaneEff,1) // ITS Plane Efficiency virtual base class 
};
#endif
