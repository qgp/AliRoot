//
// Class AliRsnFunctionAxis
//
// Histogram definition.
// Contains required informations to create a histogram
// with fixed bin size: number of bins, minimum and maximum.
// Variable bin sizes are not considered because they are
// not used as typical output of analysis in this package.
//

#ifndef ALIRSNFunctionAxis_H
#define ALIRSNFunctionAxis_H

class AliRsnPairDef;

class AliRsnFunctionAxis : public TObject
{
  public:

    enum EAxisType
    {
      kPairInvMass,
      kPairInvMassMC,
      kPairInvMassRes,
      kPairPt,
      kPairEta,
      kEventMult,
      kAxisTypes
    };

    enum EAxisObject
    {
      kParticle,
      kPair,
      kEvent,
      kNone
    };

    AliRsnFunctionAxis();
    AliRsnFunctionAxis(EAxisType type, Int_t n, Double_t min, Double_t max);
    AliRsnFunctionAxis(EAxisType type, Double_t min, Double_t max, Double_t step);
    virtual ~AliRsnFunctionAxis() { }

    virtual const char* GetName() const;

    Int_t       GetNBins() const {return fNBins;}
    Double_t    GetMin() const {return fMin;}
    Double_t    GetMax() const {return fMax;}
    EAxisObject GetAxisObject();

    void     SetType(EAxisType type) {fType = type;}
    void     SetBins(Int_t n, Double_t min, Double_t max);
    void     SetBins(Double_t min, Double_t max, Double_t step);

    Double_t Eval(AliRsnDaughter *daughter);
    Double_t Eval(AliRsnPairParticle *pair, AliRsnPairDef *pairDef);
    Double_t Eval(AliRsnEvent *event);

  private:

    EAxisType fType;    // binning type

    Int_t     fNBins;   // number of bins
    Double_t  fMin;     // lower edge
    Double_t  fMax;     // upper edge

    // ROOT dictionary
    ClassDef(AliRsnFunctionAxis, 1)
};

#endif
