#include "AliESDVZERO.h"
#include "AliLog.h"

ClassImp(AliESDVZERO)

//__________________________________________________________________________
AliESDVZERO::AliESDVZERO()
  :TObject(),
   fBBtriggerV0A(0),
   fBGtriggerV0A(0),
   fBBtriggerV0C(0),
   fBGtriggerV0C(0)
{   
   // Default constructor 
   for(Int_t j=0; j<64; j++){ 
      fMultiplicity[j] = 0;   
      fAdc[j]   = 0;   
      fTime[j]  = 0; 
      fWidth[j] = 0; 
      fBBFlag[j]= kFALSE;
      fBGFlag[j]= kFALSE;  
   }
}

//__________________________________________________________________________
AliESDVZERO::AliESDVZERO(const AliESDVZERO &o)
  :TObject(o),
   fBBtriggerV0A(o.fBBtriggerV0A),
   fBGtriggerV0A(o.fBGtriggerV0A),
   fBBtriggerV0C(o.fBBtriggerV0C),
   fBGtriggerV0C(o.fBGtriggerV0C)
{   
   // Default constructor 
   for(Int_t j=0; j<64; j++) {
       fMultiplicity[j] = o.fMultiplicity[j];
       fAdc[j]    = o.fAdc[j];
       fTime[j]   = o.fTime[j];
       fWidth[j]  = o.fWidth[j];
       fBBFlag[j] = o.fBBFlag[j];
       fBGFlag[j] = o.fBGFlag[j];
   }
}

//__________________________________________________________________________
AliESDVZERO::AliESDVZERO(UInt_t BBtriggerV0A, UInt_t BGtriggerV0A,
	      UInt_t BBtriggerV0C, UInt_t BGtriggerV0C,
	      Short_t *Multiplicity, Short_t *Adc, 
	      Short_t *Time, Short_t *Width, Bool_t *BBFlag, Bool_t *BGFlag)
  :TObject(),
   fBBtriggerV0A(BBtriggerV0A),
   fBGtriggerV0A(BGtriggerV0A),
   fBBtriggerV0C(BBtriggerV0C),
   fBGtriggerV0C(BGtriggerV0C)

{
   // Constructor
   for(Int_t j=0; j<64; j++) {
       fMultiplicity[j] = Multiplicity[j];
       fAdc[j]    = Adc[j];
       fTime[j]   = Time[j];
       fWidth[j]  = Width[j];
       fBBFlag[j] = BBFlag[j];
       fBGFlag[j] = BGFlag[j];
   }
}

//__________________________________________________________________________
AliESDVZERO& AliESDVZERO::operator=(const AliESDVZERO& o)
{

  if(this==&o) return *this;
  TObject::operator=(o);
  // Assignment operator
  fBBtriggerV0A=o.fBBtriggerV0A;
  fBGtriggerV0A=o.fBGtriggerV0A;
  fBBtriggerV0C=o.fBBtriggerV0C;
  fBGtriggerV0C=o.fBGtriggerV0C;
   for(Int_t j=0; j<64; j++) {
       fMultiplicity[j] = o.fMultiplicity[j];
       fAdc[j]    = o.fAdc[j];
       fTime[j]   = o.fTime[j];
       fWidth[j]  = o.fWidth[j];
       fBBFlag[j] = o.fBBFlag[j];
       fBGFlag[j] = o.fBGFlag[j];
   }
  return *this;
}

//__________________________________________________________________________
Short_t AliESDVZERO::GetNbPMV0A()
{
  Short_t n=0;
  for(Int_t i=32;i<64;i++) 
    if (fMultiplicity[i]>0) n++;
  return n;
}

//__________________________________________________________________________
Short_t AliESDVZERO::GetNbPMV0C()
{
  Short_t n=0;
  for(Int_t i=0;i<32;i++) 
    if (fMultiplicity[i]>0) n++;
  return n;
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetMTotV0A()
{
  Int_t n=0;
  for(Int_t i=32;i<64;i++) 
    n+= (Int_t) fMultiplicity[i];
  return n;
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetMTotV0C()
{
  Int_t n=0;
  for(Int_t i=0;i<32;i++) 
    n+= (Int_t) fMultiplicity[i];
  return n;
}

//__________________________________________________________________________
Int_t* AliESDVZERO::GetMRingV0A()
{
//  cout << "AliESDVZERO::GetMRingV0C() not supported any more" << endl;
//  cout << "use Int_t AliESDVZERO::GetMRingV0C(Int_t ring)" << endl;
  AliInfo("AliESDVZERO::GetMRingV0C() not supported any more");
  AliInfo("use Int_t AliESDVZERO::GetMRingV0C(Int_t ring)");
  return 0x0;
}

//__________________________________________________________________________
Int_t* AliESDVZERO::GetMRingV0C()
{
//  cout << "AliESDVZERO::GetMRingV0C() not supported any more" << endl;
//  cout << "use Int_t AliESDVZERO::GetMRingV0C(Int_t ring)" << endl;
  AliInfo("AliESDVZERO::GetMRingV0C() not supported any more");
  AliInfo("use Int_t AliESDVZERO::GetMRingV0C(Int_t ring)"); 
  return 0x0;
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetMRingV0A(Int_t ring)
{ 
  if (OutOfRange(ring, "AliESDVZERO:::GetMRingV0A",4)) return -1;
  Int_t n=0;

  if (ring == 0) for(Int_t i=32;i<40;i++) n += (Int_t) fMultiplicity[i];
  if (ring == 1) for(Int_t i=40;i<48;i++) n += (Int_t) fMultiplicity[i];
  if (ring == 2) for(Int_t i=48;i<56;i++) n += (Int_t) fMultiplicity[i];
  if (ring == 3) for(Int_t i=56;i<64;i++) n += (Int_t) fMultiplicity[i];
  return n ;
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetMRingV0C(Int_t ring)
{ 
  if (OutOfRange(ring, "AliESDVZERO:::GetMRingV0C",4)) return -1;
  Int_t n=0;

  if (ring == 0) for(Int_t i=0;i<8;i++)   n += (Int_t) fMultiplicity[i];
  if (ring == 1) for(Int_t i=8;i<16;i++)  n += (Int_t) fMultiplicity[i];
  if (ring == 2) for(Int_t i=16;i<24;i++) n += (Int_t) fMultiplicity[i];
  if (ring == 3) for(Int_t i=24;i<32;i++) n += (Int_t) fMultiplicity[i];
  return n ;
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetMultiplicity(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetMultiplicity:",64)) return -1;
  return fMultiplicity[i];
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetMultiplicityV0A(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetMultiplicityV0A:",32)) return -1;
  return fMultiplicity[32+i];
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetMultiplicityV0C(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetMultiplicityV0C:",32)) return -1;
  return fMultiplicity[i];
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetAdc(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetAdc:",64)) return -1;
  return fAdc[i];
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetAdcV0A(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetAdcV0A:",32)) return -1;
  return fAdc[32+i];
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetAdcV0C(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetAdcV0C:",32)) return -1;
  return fAdc[i];
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetTime(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetTime:",64)) return -1;
  return fTime[i];
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetTimeV0A(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetTimeV0A:",32)) return -1;
  return fTime[32+i];
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetTimeV0C(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetTimeV0C:",32)) return -1;
  return fTime[i];
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetWidth(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetWidth:",64)) return -1;
  return fWidth[i];
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetWidthV0A(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetWidthV0A:",32)) return -1;
  return fWidth[32+i];
}

//__________________________________________________________________________
Int_t AliESDVZERO::GetWidthV0C(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetWidthV0C:",32)) return -1;
  return fWidth[i];
}

//__________________________________________________________________________
Bool_t AliESDVZERO::BBTriggerV0A(Int_t i)
{
  if (OutOfRange(i, "AliESDVZERO:::BBTriggerV0A",32)) return kFALSE;
  UInt_t test = 1;
  return ( fBBtriggerV0A & (test << i) ? kTRUE : kFALSE );
}

//__________________________________________________________________________
Bool_t AliESDVZERO::BGTriggerV0A(Int_t i)
{
  if (OutOfRange(i, "AliESDVZERO:::BGTriggerV0A",32)) return kFALSE;
  UInt_t test = 1;
  return ( fBGtriggerV0A & (test << i) ? kTRUE : kFALSE );
}

//__________________________________________________________________________
Bool_t AliESDVZERO::BBTriggerV0C(Int_t i)
{
  if (OutOfRange(i, "AliESDVZERO:::BBTriggerV0C",32)) return kFALSE;
  UInt_t test = 1;
  return ( fBBtriggerV0C & (test << i) ? kTRUE : kFALSE );
}

//__________________________________________________________________________
Bool_t AliESDVZERO::BGTriggerV0C(Int_t i)
{
  if (OutOfRange(i, "AliESDVZERO:::BGTriggerV0C",32)) return kFALSE;
  UInt_t test = 1;
  return ( fBGtriggerV0C & (test << i) ? kTRUE : kFALSE );
}

//__________________________________________________________________________
Bool_t AliESDVZERO::GetBBFlag(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetBBFlag:",64)) return kFALSE;
  return fBBFlag[i];
}

//__________________________________________________________________________
Bool_t AliESDVZERO::GetBGFlag(Int_t i)

{
  if (OutOfRange(i, "AliESDVZERO::GetBGFlag:",64)) return kFALSE;
  return fBGFlag[i];
}

//__________________________________________________________________________
Bool_t AliESDVZERO::OutOfRange(Int_t i, const char *s, Int_t upper) const
{
  // checks if i is a valid index. s = name of calling method
  if (i > upper || i < 0) {
//    cout << s << " Index " << i << " out of range" << endl;
     AliInfo(Form("Index %d out of range",i));
     return kTRUE;
  }
  return kFALSE;
}

