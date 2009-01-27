#ifndef ALITRDPIDUTIL_H
#define ALITRDPIDUTIL_H

//////////////////////////////////////////////////////
//
// Class to calculate PID performance of the TRD
//
// Author : Alex Wilk <wilka@uni-muenster.de>
//
///////////////////////////////////////////////////////

class TH1;
class AliESDtrack;
class AliTRDpidUtil : public TObject {
public:

  enum {
    kBins = 10001
  };
  typedef enum{
     kLQ   = 0 // 2D likelihood method
    ,kNN   = 1 // Neural network method
    ,kESD  = 2 // ESD results - check offline
  } PIDmethod_t;

  AliTRDpidUtil();
  virtual ~AliTRDpidUtil(){;}

  Bool_t   CalculatePionEffi(TH1* histo1, TH1* histo2);

  static Float_t ElectronEfficiency() { return fEleEffi;};
  
  static Bool_t IsElectron(const AliESDtrack *track, PIDmethod_t method = kNN);
  static Double_t GetSystematicError(const AliESDtrack *track, PIDmethod_t method = kNN);

  Double_t GetCalcElectronEfficiency() {return fCalcEleEffi;};
  Double_t GetPionEfficiency() {return fPionEffi;};
  Double_t GetError() {return fError;};
  Double_t GetThreshold() {return fThreshold;};

  static Int_t    GetMomentumBin(Double_t p);

  static void SetElectronEfficiency(Float_t eleeffi) {fEleEffi = eleeffi;};

private:
  AliTRDpidUtil(const AliTRDpidUtil&);               // not implemented
  AliTRDpidUtil& operator=(const AliTRDpidUtil&);    // not implemented

  static Float_t fEleEffi;               // electron efficiency

  Double_t fCalcEleEffi;                 // electron efficiency after calculation
  Double_t fPionEffi;                    // pion efficiency 
  Double_t fError;                       // pion efficiency error
  Double_t fThreshold;                   // threshold for calculated electron efficiency

  ClassDef(AliTRDpidUtil, 1)  // TRD PID efficiency calculator

};

#endif
