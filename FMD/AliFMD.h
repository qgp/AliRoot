#ifndef ALIFMD_H
#define ALIFMD_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights
 * reserved. 
 *
 * Latest changes by Christian Holm Christensen <cholm@nbi.dk>
 *
 * See cxx source for full Copyright notice                               
 */

//____________________________________________________________________
//
//  Manager class for the FMD - Base class.
//
#ifndef ALIDETECTOR_H 
# include <AliDetector.h>
#endif
#ifndef ALIFMDSUBDETECTOR_H
# include "AliFMDSubDetector.h"
#endif
#ifndef ALIFMDRING_H
# include "AliFMDRing.h"
#endif
#ifndef ROOT_TBranch
# include <TBranch.h>
#endif
#ifndef ROOT_TArrayI
# include <TArrayI.h>
#endif

//____________________________________________________________________
class AliFMD : public AliDetector 
{
public:
  AliFMD();
  AliFMD(const char *name, const char *title, bool detailed);
  AliFMD(const AliFMD& other);
  virtual ~AliFMD(); 
  AliFMD& operator=(const AliFMD& other);

  // GEometry ANd Tracking (GEANT :-)
  virtual void   CreateGeometry();
  virtual void   CreateMaterials(); 
  virtual void   Init();
  virtual void   StepManager() = 0;
  AliFMDSubDetector*    GetFMD1() const       { return fFMD1; }
  AliFMDSubDetector*    GetFMD2() const       { return fFMD2; }
  AliFMDSubDetector*    GetFMD3() const       { return fFMD3; }
  AliFMDRing*           GetInner() const      { return fInner; }
  AliFMDRing*           GetOuter() const      { return fOuter; }

  // Graphics and event display 
  virtual        void   BuildGeometry();
  virtual        void   DrawDetector();
  virtual        Int_t  DistanceToPrimitive(Int_t px, Int_t py);

  // Hit and digit management 
  virtual void          MakeBranch(Option_t *opt=" ");
  virtual void          SetHitsAddressBranch(TBranch *b);
  virtual void          SetTreeAddress();
  virtual TClonesArray* SDigits() { return fSDigits; }        
  virtual void          ResetSDigits();
  virtual void          AddHit(Int_t track, Int_t *vol, Float_t *hits);
  virtual void          AddHit(Int_t    track, 
			       UShort_t detector, 
			       Char_t   ring, 
			       UShort_t sector, 
			       UShort_t strip, 
			       Float_t  x=0,
			       Float_t  y=0, 
			       Float_t  z=0,
			       Float_t  px=0, 
			       Float_t  py=0, 
			       Float_t  pz=0,
			       Float_t  edep=0,
			       Int_t    pdg=0,
			       Float_t  t=0);
  virtual        void   AddDigit(Int_t *digits);
  virtual        void   AddDigit(UShort_t detector=0, 
				 Char_t   ring='\0', 
				 UShort_t sector=0, 
				 UShort_t strip=0, 
				 UShort_t count1=0, 
				 Short_t  count2=-1, 
				 Short_t  count3=-1);
  virtual        void   AddSDigit(Int_t *digits);
  virtual        void   AddSDigit(UShort_t detector=0, 
				  Char_t   ring='\0', 
				  UShort_t sector=0, 
				  UShort_t strip=0, 
				  Float_t  edep=0,
				  UShort_t count1=0, 
				  Short_t  count2=-1, 
				  Short_t  count3=-1);

  // Digitisation
  virtual AliDigitizer* CreateDigitizer(AliRunDigitizer* manager) const;
  virtual        void   Hits2Digits();
  virtual        void   Hits2SDigits();

  // Raw data 
  virtual        void   Digits2Raw();

  // Set various parameters 
  void     SetLegLength(Double_t     length=1);
  void     SetLegRadius(Double_t     radius=.5);
  void     SetLegOffset(Double_t     offset=.5);
  void     SetModuleSpacing(Double_t spacing=1);
  void     SetSiDensity(Float_t r=2.33)         { fSiDensity = r; }
  void     SetSiThickness(Float_t r=0.03)       { fSiThickness = r; }
  void     SetVA1MipRange(UShort_t r=20)        { fVA1MipRange = r; }
  void     SetAltroChannelSize(UShort_t s=1024) { fAltroChannelSize = s;}
  void     SetSampleRate(UShort_t r=1)          { fSampleRate = (r>2 ? 2 : r);}

  // Get various parameters
  Int_t    GetSiId()                 const { return (*fIdtmed)[kSiId]; }
  Int_t    GetAirId()                const { return (*fIdtmed)[kAirId]; }
  Int_t    GetPlasticId()            const { return (*fIdtmed)[kPlasticId]; }
  Int_t    GetPcbId()                const { return (*fIdtmed)[kPcbId]; }
  Int_t    GetAlId()                 const { return (*fIdtmed)[kAlId]; }
  Int_t    GetCarbonId()             const { return (*fIdtmed)[kCarbonId]; }
  Int_t    GetPrintboardRotationId() const { return fPrintboardRotationId; }
  Int_t    GetShortLegId()           const { return fShortLegId; }
  Int_t    GetLongLegId()            const { return fLongLegId; }
  Double_t GetLegLength()            const { return fLegLength; }
  Double_t GetLegRadius()            const { return fLegRadius; }
  Double_t GetModuleSpacing()        const { return fModuleSpacing; }  
  Float_t  GetSiDensity()            const { return fSiDensity; }
  Float_t  GetSiThickness()          const { return fSiThickness; }
  UShort_t GetVA1MipRange()          const { return fVA1MipRange; }
  UShort_t GetAltroChannelSize()     const { return fAltroChannelSize; }
  UShort_t GetSampleRate()           const { return fSampleRate; }
  Float_t  GetEdepMip()              const { 
    return fSiDeDxMip * fSiDensity * fSiThickness;
  }
  
  // Utility
  void   Browse(TBrowser* b);
  enum { 
    kBaseDDL = 0x1000 // DDL offset for the FMD
  };
  // 
  static const Char_t* fgkShortLegName;  // Format for short support legs
  static const Char_t* fgkLongLegName;   // Format for long support legs
protected:
  TClonesArray*      HitsArray();
  TClonesArray*      DigitsArray();
  TClonesArray*      SDigitsArray();

  enum {
    kSiId,                 // ID of Si medium
    kAirId,                // ID of Air medium
    kPlasticId,            // ID of Plastic medium
    kPcbId,                // ID of PCB medium
    kSiChipId,             // ID of Si Chip medium
    kAlId,                 // ID of Al medium
    kCarbonId              // ID of Carbon medium
  };
  
  AliFMDRing*        fInner;                // Inner ring structure
  AliFMDRing*        fOuter;                // Outer ring structure  
  AliFMDSubDetector* fFMD1;                 // FMD1 structure
  AliFMDSubDetector* fFMD2;                 // FMD2 structure  
  AliFMDSubDetector* fFMD3;                 // FMD3 structure
  TClonesArray*      fSDigits;              // Summable digits
  Int_t              fNsdigits;             // Number of digits  
  Int_t              fPrintboardRotationId; // ID of Rotation of print bard
  Int_t              fIdentityRotationId;   // ID of identity matrix 
  Int_t              fShortLegId;           // ID short leg volume
  Int_t              fLongLegId;            // ID long leg volume  
  Double_t           fLegLength;            // Leg length
  Double_t           fLegRadius;            // Leg radius
  Double_t           fModuleSpacing;        // Staggering offset 
  Float_t            fSiDensity;            // Density of Silicon
  Float_t            fSiThickness;          // Thickness of silicon wafers
  const Float_t      fSiDeDxMip;            // MIP dE/dx in Silicon
  UShort_t           fVA1MipRange;          // # MIPs the pre-amp can do    
  UShort_t           fAltroChannelSize;     // Largest # to store in 1 ADC ch.
  UShort_t           fSampleRate;           // Times the ALTRO samples pre-amp.

  ClassDef(AliFMD,9)     // Base class FMD entry point
};

#endif
//____________________________________________________________________
//
// Local Variables:
//   mode: C++
// End:
//
// EOF
//
