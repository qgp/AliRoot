//
// Class AliRsnHistoDef
//
// Histogram definition.
// Contains required informations to create a histogram
// with fixed bin size: number of bins, minimum and maximum.
// Variable bin sizes are not considered because they are 
// not used as typical output of analysis in this package.
//

#ifndef ALIRSNHISTODEF_H
#define ALIRSNHISTODEF_H

class TH1D;

class AliRsnHistoDef : public TObject
{
  public:

    AliRsnHistoDef();
    AliRsnHistoDef(Int_t n, Double_t min, Double_t max);
    virtual ~AliRsnHistoDef() { }

    Int_t    GetNBins() const {return fNBins;}
    Double_t GetMin() const {return fMin;}
    Double_t GetMax() const {return fMax;}

    void     SetBins(Int_t n, Double_t min, Double_t max);
    TH1D*    CreateHistogram(const char *name, const char *title);
    void     CheckEdges();

  private:

    Int_t     fNBins;   // number of bins
    Double_t  fMin;     // lower edge
    Double_t  fMax;     // upper edge

    // ROOT dictionary
    ClassDef(AliRsnHistoDef, 1)
};

#endif
