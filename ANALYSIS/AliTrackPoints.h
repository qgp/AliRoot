#ifndef AliTrackPoints_H
#define AliTrackPoints_H
//_________________________________
////////////////////////////////////////////////////////////
//                                                        //
// class AliTrackPoints                                //
//                                                        //
// used by Anti-Merging cut                               //
// contains set of poits the lay on track trajectory      //
// according to reconstructed track parameters -          //
// NOT CLUSTERS POSITIONS!!!                              //
// Anti-Merging cut is applied only on tracks coming from //
// different events (that are use to fill deniminators)   //
//                                                        //
////////////////////////////////////////////////////////////
#include <TObject.h>

class AliTPCtrack;
class AliESDtrack;

class AliTrackPoints: public TObject
{
  public:
    AliTrackPoints();
    AliTrackPoints(Int_t n, AliTPCtrack* track, Float_t dr=30, Float_t r0 = 84.1); //min TPC R  = 84.1; max TPC R =  246.6cm, 
    AliTrackPoints(Int_t n, AliESDtrack* track, Float_t mf, Float_t dr=30,Float_t r0 = 84.1); //min TPC R  = 84.1; max TPC R =  246.6cm, 
    AliTrackPoints(const AliTrackPoints& in);
    
    virtual ~AliTrackPoints();
    AliTrackPoints& operator=(const AliTrackPoints& in);
    
    Double_t AvarageDistance(const AliTrackPoints& tr);
    void PositionAt(Int_t n, Float_t &x, Float_t &y, Float_t &z);
    Int_t GetDebug() const {return fgDebug;}
    void  SetDebug(Int_t deblevel){fgDebug = deblevel;} 
    static void TestTPC(Int_t entr);
    static void TestESD(Int_t entr,const char* fname = "AliESDs.root");
  protected:
    void MakePoints( Float_t dr, Float_t r0, Double_t x, Double_t* par, Double_t c, Double_t alpha);
  private:
    Int_t    fN;//number of points
    Float_t* fX;//[fN]positions at x
    Float_t* fY;//[fN]positions at y
    Float_t* fZ;//[fN] positions at z
//    Float_t* fR;//! [fN] radii
    static Int_t fgDebug;//! debug level
    ClassDef(AliTrackPoints,1)
};
#endif
