#ifndef ALIITSRESPONSE_H
#define ALIITSRESPONSE_H

#include <TObject.h>

#include "AliITSsegmentation.h"

class TF1;
class TString;
class AliITSgeom;

//----------------------------------------------
//
// ITS response virtual base class
//
class AliITSresponse : public TObject {
 public:
    // Default Constructor
    AliITSresponse();
    // Standard Constructor
    AliITSresponse(Double_t Thickness);
    // Destructor.
    virtual ~AliITSresponse() {}
    //
    // Configuration methods
    //

    // Set Electronics
    virtual void    SetElectronics(Int_t p1) {}
    // Get Electronics
    virtual Int_t Electronics()  {return 0;}

    // Set maximum Adc-count value
    virtual void    SetMaxAdc(Float_t p1) {}
    // Get maximum Adc-count value
    virtual Float_t MaxAdc()  {return 0.;}

    // Set maximum Adc-top value
    virtual void    SetDynamicRange(Float_t p1) {}
    // Get maximum Adc-top value
    virtual Float_t DynamicRange()  {return 0.0;}

    // Set Charge Loss Linear Coefficient
    virtual void    SetChargeLoss(Float_t p1) {}
    // Get Charge Loss Linear Coefficient
    virtual Float_t ChargeLoss()  {return 0.0;}

    // Diffusion coefficient
    virtual void    SetDiffCoeff(Float_t, Float_t) {}
    // Get diffusion coefficients
    virtual void    DiffCoeff(Float_t &,Float_t &) {}

    // Temperature in [degree K]
    virtual void    SetTemperature(Float_t t=300.0) {fT = t;}
    // Get temperature [degree K]
    virtual Float_t Temperature() {return fT;}
    // Type of data - real or simulated
    virtual void    SetDataType(const char *data) {}
    // Set the impurity concentrations in [#/cm^3]
    virtual void SetImpurity(Double_t n=0.0){fN = n;}
    // Returns the impurity consentration in [#/cm^3]
    virtual Double_t Impurity(){return fN;}
    // Sets the applied ratio distance/voltage [cm/volt]
    virtual void SetDistanceOverVoltage(Double_t d,Double_t v){fdv = d/v;}
    // Sets the applied ration distance/voltage [cm/volt]. Default value
    // is 300E-4cm/80 volts = 0.000375 cm/volts
    virtual void SetDistanceOverVoltage(Double_t dv=0.000375){fdv = dv;}
    // Returns the ration distance/voltage
    virtual Double_t DistanceOverVoltage(){return fdv;}
    // Get data type
    virtual const char  *DataType() const {return "";}
 
    // Set parameters options: "same" or read from "file" or "SetInvalid" or...
    virtual void   SetParamOptions(const char* opt1,const char* opt2) {}
    // Set noise parameters 
    virtual void   SetNoiseParam(Float_t, Float_t) {}
    // Number of parameters to be set
    virtual  void   SetNDetParam(Int_t) {}
    // Set detector parameters: gain, coupling ...
    virtual  void   SetDetParam(Float_t *) {}

    // Parameters options
    virtual void   ParamOptions(char *,char*) {}
    virtual Int_t  NDetParam() {return 0;}
    virtual void   GetDetParam(Float_t *) {}
    virtual void   GetNoiseParam(Float_t&, Float_t&) {}

    // Zero-suppression option - could be 1D, 2D or non-ZeroSuppressed
    virtual void   SetZeroSupp(const char* opt) {}
    // Get zero-suppression option
    virtual const char *ZeroSuppOption() const {return "";}
     // Set thresholds
    virtual void   SetThresholds(Float_t, Float_t) {}
    virtual void   Thresholds(Float_t &, Float_t &) {}
    // Set min val
    virtual void   SetMinVal(Int_t) {};
    virtual Int_t  MinVal() {return 0;};

    // Set filenames
    virtual void SetFilenames(const char *f1,const char *f2,const char *f3) {}
    // Filenames
    virtual void   Filenames(char*,char*,char*) {}

    virtual Float_t DriftSpeed() {return 0.;}
    virtual Bool_t  OutputOption() {return kFALSE;}
    virtual Bool_t  Do10to8() {return kTRUE;}
    virtual void    GiveCompressParam(Int_t *x) {}

    //
    // Detector type response methods
    // Set number of sigmas over which cluster disintegration is performed
    virtual void    SetNSigmaIntegration(Float_t p1) {}
    // Get number of sigmas over which cluster disintegration is performed
    virtual Float_t NSigmaIntegration() {return 0.;}
    // Set number of bins for the gaussian lookup table
    virtual void    SetNLookUp(Int_t p1) {}
    // Get number of bins for the gaussian lookup table
    virtual Int_t GausNLookUp() {return 0;}
    // Get scaling factor for bin i-th from the gaussian lookup table
    virtual Float_t GausLookUp(Int_t) {return 0.;}
    // Set sigmas of the charge spread function
    virtual void    SetSigmaSpread(Float_t p1, Float_t p2) {}
    // Get sigmas for the charge spread
    virtual void    SigmaSpread(Float_t &s1, Float_t &s2) {}

    // Pulse height from scored quantity (eloss)
    virtual Float_t IntPH(Float_t eloss) {return 0.;}
    // Charge disintegration
    virtual Float_t IntXZ(AliITSsegmentation *) {return 0.;}
    // Electron mobility in Si. [cm^2/(Volt Sec)]. T in degree K, N in #/cm^3
    virtual Double_t MobilityElectronSiEmp();
    // Hole mobility in Si. [cm^2/(Volt Sec)]  T in degree K, N in #/cm^3
    virtual Double_t MobilityHoleSiEmp();
    // Einstein relation for Diffusion Coefficient of Electrons. [cm^2/sec]
    //  T in degree K, N in #/cm^3
    virtual Double_t DiffusionCoefficientElectron();
    // Einstein relation for Diffusion Coefficient of Holes. [cm^2/sec]
    //  T in [degree K], N in [#/cm^3]
    virtual Double_t DiffusionCoefficientHole();
    // Electron <speed> under an applied electric field E=Volts/cm. [cm/sec]
    // d distance-thickness in [cm], v in [volts], T in [degree K],
    // N in [#/cm^3]
    virtual Double_t SpeedElectron();
    // Holes <speed> under an applied electric field E=Volts/cm. [cm/sec]
    // d distance-thickness in [cm], v in [volts], T in [degree K],
    // N in [#/cm^3]
    virtual Double_t SpeedHole();
    // Returns the Gaussian sigma == <x^2+z^2> [cm^2] due to the defusion of
    // electrons or holes through a distance l [cm] caused by an applied
    // voltage v [volt] through a distance d [cm] in any material at a
    // temperature T [degree K].
    virtual Double_t SigmaDiffusion3D(Double_t l);
    // Returns the Gaussian sigma == <x^2 +y^2+z^2> [cm^2] due to the
    // defusion of electrons or holes through a distance l [cm] caused by an
    // applied voltage v [volt] through a distance d [cm] in any material at a
    // temperature T [degree K].
    virtual Double_t SigmaDiffusion2D(Double_t l);
    // Returns the Gaussian sigma == <x^2+z^2> [cm^2] due to the defusion of
    // electrons or holes through a distance l [cm] caused by an applied
    // voltage v [volt] through a distance d [cm] in any material at a
    // temperature T [degree K].
    virtual Double_t SigmaDiffusion1D(Double_t l);
 private:
    Double_t fdv;  // The parameter d/v where d is the disance over which the
                   // the potential v is applied d/v [cm/volts]
    Double_t fN;   // the impurity consentration of the material in #/cm^3
    Double_t fT;   // The temperature of the Si in Degree K.

    ClassDef(AliITSresponse,2) // Detector type response virtual base class 
};
#endif
