#ifndef ALIMUONDIGIT_H
#define ALIMUONDIGIT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
// Revision of includes 07/05/2004

/// \ingroup base
/// \class AliMUONDigit
/// \brief MUON digit

#include <TObject.h>

class AliMUONDigit : public TObject 
{
 public:
    AliMUONDigit();
    AliMUONDigit(const AliMUONDigit& rhs);
    /// \deprecated
    AliMUONDigit(Int_t *digits);
    /// \deprecated
    AliMUONDigit(Int_t *tracks, Int_t *charges, Int_t *digits);
    virtual ~AliMUONDigit();

    AliMUONDigit& operator=(const AliMUONDigit& rhs);
    
    virtual Bool_t IsSortable() const {return kTRUE;}
    virtual int Compare(const TObject *obj) const;

    virtual Int_t DetElemId()const     {return fDetElemId;}    
    virtual Int_t PadX() const         {return fPadX;}
    virtual Int_t PadY() const         {return fPadY;}
    virtual Int_t Cathode() const      {return fCathode;}
    
    virtual Int_t Signal() const       {return fSignal;}
    
    virtual Int_t Physics() const      {return fPhysics;}
    
    virtual Int_t Hit() const          {return fHit;}  
    
    virtual Int_t Ntracks() const { return fNtracks; }
    virtual void AddTrack(Int_t trackNumber, Int_t trackCharge);
    virtual Int_t Track(Int_t i) const;
    virtual Int_t TrackCharge(Int_t i) const;
    
    virtual Int_t ADC() const { return fADC; }
    virtual Int_t ManuId() const { return fManuId; }
    virtual Int_t ManuChannel() const { return fManuChannel; }
    virtual Bool_t IsSaturated() const;
    virtual Bool_t IsNoiseOnly() const;
    
    virtual void NoiseOnly(Bool_t value=kTRUE);
    virtual void Saturated(Bool_t saturated=kTRUE);
    virtual void SetElectronics(Int_t manuId, Int_t manuChannel);
    virtual void SetADC(Int_t adc) { fADC=adc; }
    virtual void SetDetElemId(Int_t id)    {fDetElemId = id;}
    virtual void SetPadX(Int_t pad)        {fPadX = pad;}
    virtual void SetPadY(Int_t pad)        {fPadY = pad;}
    virtual void SetSignal(Int_t q)        {fSignal = q;}
    virtual void AddSignal(Int_t q)        {fSignal += q;}
    virtual void AddPhysicsSignal(Int_t q) {fPhysics += q;}
    virtual void SetHit(Int_t n)           {fHit = n;}    
    virtual void SetCathode(Int_t c)       {fCathode = c;}
    virtual void SetPhysicsSignal(Int_t q) {fPhysics = q; }
    
    virtual void Print(Option_t* opt="") const;
    
    virtual void Copy(TObject& digit) const;
    
    /** Delete the internal track arrays (which are dynamically allocated).
      * This is to insure we can put those digits in e.g. TClonesArray
      * w/o leaking memory.
      */
    virtual void Clear(Option_t*);
    
    /// Add mask to the track numbers.
    virtual void PatchTracks(Int_t mask);
    
private:
    Int_t fDetElemId;     // Detection element ID
    Int_t fManuId;        // Id of the MANU chip.
    Int_t fManuChannel;   // Channel within the MANU chip.
    Int_t fSignal;        // Signal amplitude    
      
    Int_t fPadX;          // Pad number along x
    Int_t fPadY;          // Pad number along y
    Int_t fCathode;       // Cathode number
    Int_t fADC;           // ADC value
    UInt_t fFlags;        // Special flags (e.g. is the signal an overflow ?)
    
    Int_t fNtracks;       // MC tracks making to this digit.
    Int_t* fTcharges;     //[fNtracks] charges of MC track making this digit
    Int_t* fTracks;       //[fNtracks] primary MC tracks making this digit
    Int_t fPhysics;       // MC physics contribution to signal 
    Int_t fHit;           // MC hit number - temporary solution
  
    static const UInt_t fgkSaturatedMask = 0x1; // the mask (part of fFlags) to indicate this digit is saturated
    static const UInt_t fgkNoiseOnlyMask = 0x1000; // indicate a simulated digit due to noise only
    
    ClassDef(AliMUONDigit,4)  //Digits for MUON
};
#endif
