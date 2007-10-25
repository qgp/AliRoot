#ifndef ROOT_TKDNodeInfo
#define ROOT_TKDNodeInfo

#ifndef ROOT_TObject
#include "TObject.h"
#endif

template <typename Value> class TVectorT;
typedef struct TVectorT<Double_t> TVectorD;
template <typename Value> class TMatrixT;
typedef class TMatrixT<Double_t> TMatrixD;
class TKDInterpolatorBase;
class TKDNodeInfo : public TObject
{
friend class TKDInterpolatorBase;
public:
	TKDNodeInfo(const Int_t ndim = 0);
	TKDNodeInfo(const TKDNodeInfo &);
	TKDNodeInfo& operator=(const TKDNodeInfo &);
	virtual  ~TKDNodeInfo();
	Double_t  CookPDF(const Double_t *point, Double_t &result, Double_t &error);
	inline Bool_t    Has(const Float_t *p);
	void      Print();
	void      Store(const TVectorD &par, const TMatrixD &cov);

protected:
	void      Build(const Int_t ndim);

public:
	Int_t     fNDim;          // 3 times data dimension
	Float_t   *fData;         //[fNDim] node's data
	Float_t   fVal[2];        // measured value for node
	TMatrixD  *fCov;          // interpolator covariance matrix
	TVectorD  *fPar;          // interpolator parameters

	ClassDef(TKDNodeInfo, 1)  // node info for interpolator
};

//_____________________________________________________________________
Bool_t TKDNodeInfo::Has(const Float_t *p)
{
	Int_t n = 0;
	Int_t ndim = fNDim/3;
	for(int id=0; id<ndim; id++) if(p[id]>=fData[ndim+2*id] && p[id]<fData[ndim+2*id+1]) n++;
	if(n==ndim) return kTRUE;
	return kFALSE;
}


#endif

