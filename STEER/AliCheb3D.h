// Author: ruben.shahoyan@cern.ch   09/09/2006

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// AliCheb3D produces the interpolation of the user 3D->NDimOut arbitrary     //
// function supplied in "void (*fcn)(float* inp,float* out)" format           //
// either in a separate macro file or as a function pointer.                  //
// Only coefficients needed to guarantee the requested precision are kept.    //
//                                                                            //
// The user-callable methods are:                                             //
// To create the interpolation use:                                           //
// AliCheb3D(const char* funName,  // name of the file with user function     //
//          or                                                                //
// AliCheb3D(void (*ptr)(float*,float*),// pointer on the  user function      //
//        Int_t     DimOut,     // dimensionality of the function's output    // 
//        Float_t  *bmin,       // lower 3D bounds of interpolation domain    // 
//        Float_t  *bmax,       // upper 3D bounds of interpolation domain    // 
//        Int_t    *npoints,    // number of points in each of 3 input        //
//                              // dimension, defining the interpolation grid //
//        Float_t   prec=1E-6); // requested max.absolute difference between  //
//                              // the interpolation and any point on grid    //
//                                                                            //
// To test obtained parameterization use the method                           //
// TH1* TestRMS(int idim,int npoints = 1000,TH1* histo=0);                    // 
// it will compare the user output of the user function and interpolation     //
// for idim-th output dimension and fill the difference in the supplied       //
// histogram. If no histogram is supplied, it will be created.                //
//                                                                            //
// To save the interpolation data:                                            //
// SaveData(const char* filename, Bool_t append )                             //
// write text file with data. If append is kTRUE and the output file already  //
// exists, data will be added in the end of the file.                         //
// Alternatively, SaveData(FILE* stream) will write the data to               //
// already existing stream.                                                   //
//                                                                            //
// To read back already stored interpolation use either the constructor       // 
// AliCheb3D(const char* inpFile);                                            //
// or the default constructor AliCheb3D() followed by                         //
// AliCheb3D::LoadData(const char* inpFile);                                  //
//                                                                            //
// To compute the interpolation use Eval(float* par,float *res) method, with  //
// par being 3D vector of arguments (inside the validity region) and res is   //
// the array of DimOut elements for the output.                               //
//                                                                            //
// If only one component (say, idim-th) of the output is needed, use faster   //
// Float_t Eval(Float_t *par,int idim) method.                                //
//                                                                            //
// void Print(option="") will print the name, the ranges of validity and      //
// the absolute precision of the parameterization. Option "l" will also print //
// the information about the number of coefficients for each output           //
// dimension.                                                                 //
//                                                                            //
// NOTE: during the evaluation no check is done for parameter vector being    //
// outside the interpolation region. If there is such a risk, use             //
// Bool_t IsInside(float *par) method. Chebyshev parameterization is not      //
// good for extrapolation!                                                    //
//                                                                            //
// For the properties of Chebyshev parameterization see:                      //
// H.Wind, CERN EP Internal Report, 81-12/Rev.                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#ifndef _ALICHEB3D_
#define _ALICHEB3D_
#include <stdio.h>
#include <TNamed.h>
#include <TMethodCall.h>
#include <TMath.h>
#include <TH1.h>
#include <TObjArray.h>
#include "AliCheb3DCalc.h"

class TString;
class TSystem;
class TRandom;


class AliCheb3D: public TNamed 
{
 public:
  AliCheb3D();
  AliCheb3D(const AliCheb3D& src);
  AliCheb3D(const char* inpFile);
  AliCheb3D(FILE* stream);
  //
#ifdef _INC_CREATION_ALICHEB3D_
  AliCheb3D(const char* funName, Int_t DimOut, Float_t  *bmin,Float_t  *bmax, Int_t *npoints, Float_t  prec=1E-6);
  AliCheb3D(void (*ptr)(float*,float*), Int_t DimOut, Float_t  *bmin,Float_t  *bmax, Int_t *npoints, Float_t  prec=1E-6);
  AliCheb3D(void (*ptr)(float*,float*), int DimOut, Float_t  *bmin,Float_t  *bmax, Int_t *npX,Int_t *npY,Int_t *npZ, Float_t prec=1E-6);
  AliCheb3D(void (*ptr)(float*,float*), int DimOut, Float_t  *bmin,Float_t  *bmax, Float_t prec=1E-6);
#endif
  //
  ~AliCheb3D()                                                                 {Clear();}
  //
  AliCheb3D&   operator=(const AliCheb3D& rhs);
  void         Eval(Float_t  *par,Float_t  *res);
  Float_t      Eval(Float_t  *par,int idim);
  //
  void         EvalDeriv(int dimd, Float_t  *par,Float_t  *res);
  void         EvalDeriv2(int dimd1, int dimd2,Float_t  *par,Float_t  *res);
  Float_t      EvalDeriv(int dimd,Float_t  *par, int idim);
  Float_t      EvalDeriv2(int dimd1,int dimd2, Float_t  *par, int idim);
  void         EvalDeriv3D(Float_t *par, Float_t dbdr[3][3]); 
  void         EvalDeriv3D2(Float_t *par, Float_t dbdrdr[3][3][3]); 
  void         Print(Option_t* opt="")                                   const;
  Bool_t       IsInside(Float_t  *par)                                   const;
  Bool_t       IsInside(Double_t  *par)                                  const;
  AliCheb3DCalc*  GetChebCalc(int i)                                     const {return (AliCheb3DCalc*)fChebCalc.UncheckedAt(i);}
  Float_t      GetBoundMin(int i)                                        const {return fBMin[i];}
  Float_t      GetBoundMax(int i)                                        const {return fBMax[i];}
  Float_t*     GetBoundMin()                                             const {return (float*)fBMin;}
  Float_t*     GetBoundMax()                                             const {return (float*)fBMax;}
  Float_t      GetPrecision()                                            const {return fPrec;}
  void         ShiftBound(int id,float dif);
  //
  void         LoadData(const char* inpFile);
  void         LoadData(FILE* stream);
  //
#ifdef _INC_CREATION_ALICHEB3D_
  int*         GetNCNeeded(float xyz[3],int DimVar, float mn,float mx, float prec);
  void         EstimateNPoints(float Prec, int gridBC[3][3]);
  void         SaveData(const char* outfile,Bool_t append=kFALSE)        const;
  void         SaveData(FILE* stream=stdout)                             const;
  //
  void         SetUsrFunction(const char* name);
  void         SetUsrFunction(void (*ptr)(float*,float*));
  void         EvalUsrFunction(Float_t  *x, Float_t  *res);
  TH1*         TestRMS(int idim,int npoints = 1000,TH1* histo=0);
  static Int_t CalcChebCoefs(Float_t  *funval,int np, Float_t  *outCoefs, Float_t  prec=-1);
#endif
  //
 protected:
  void         Clear(Option_t* option = "");
  void         SetDimOut(int d);
  void         PrepareBoundaries(Float_t  *bmin,Float_t  *bmax);
  //
#ifdef _INC_CREATION_ALICHEB3D_
  void         EvalUsrFunction();
  void         DefineGrid(Int_t* npoints);
  Int_t        ChebFit();                                                                 // fit all output dimensions
  Int_t        ChebFit(int dmOut);
#endif
  //
  Float_t      MapToInternal(Float_t  x,Int_t d) const; // map x to [-1:1]
  Float_t      MapToExternal(Float_t  x,Int_t d) const {return x/fBScale[d]+fBOffset[d];}   // map from [-1:1] to x
  //  
 protected:
  Int_t        fDimOut;            // dimension of the ouput array
  Float_t      fPrec;              // requested precision
  Float_t      fBMin[3];           // min boundaries in each dimension
  Float_t      fBMax[3];           // max boundaries in each dimension  
  Float_t      fBScale[3];         // scale for boundary mapping to [-1:1] interval
  Float_t      fBOffset[3];        // offset for boundary mapping to [-1:1] interval
  TObjArray    fChebCalc;          // Chebyshev parameterization for each output dimension
  //
  Int_t        fMaxCoefs;          //! max possible number of coefs per parameterization
  Int_t        fNPoints[3];        //! number of used points in each dimension
  Float_t      fArgsTmp[3];        //! temporary vector for coefs caluclation
  Float_t      fBuff[6];           //! buffer for coordinate transformations
  Float_t *    fResTmp;            //! temporary vector for results of user function caluclation
  Float_t *    fGrid;              //! temporary buffer for Chebyshef roots grid
  Int_t        fGridOffs[3];       //! start of grid for each dimension
  TString      fUsrFunName;        //! name of user macro containing the function of  "void (*fcn)(float*,float*)" format
  TMethodCall* fUsrMacro;          //! Pointer to MethodCall for function from user macro 
  //
  ClassDef(AliCheb3D,1)  // Chebyshev parametrization for 3D->N function
};

//__________________________________________________________________________________________
inline Bool_t  AliCheb3D::IsInside(Float_t  *par) const 
{
  // check if the point is inside of the fitted box
  const float kTol = 1.e-4; 
  for (int i=3;i--;) if (fBMin[i]-par[i]>kTol || par[i]-fBMax[i]>kTol) return kFALSE;
  //if(par[i]<fBMin[i]||par[i]>fBMax[i]) return kFALSE;
  return kTRUE;
}

//__________________________________________________________________________________________
inline Bool_t  AliCheb3D::IsInside(Double_t  *par) const 
{
  // check if the point is inside of the fitted box
  const float kTol = 1.e-4; 
  for (int i=3;i--;) if (fBMin[i]-par[i]>kTol || par[i]-fBMax[i]>kTol) return kFALSE;
  //if(par[i]<fBMin[i]||par[i]>fBMax[i]) return kFALSE;
  return kTRUE;
}

//__________________________________________________________________________________________
inline void AliCheb3D::Eval(Float_t  *par, Float_t  *res)
{
  // evaluate Chebyshev parameterization for 3d->DimOut function
  for (int i=3;i--;) fArgsTmp[i] = MapToInternal(par[i],i);
  for (int i=fDimOut;i--;) res[i] = GetChebCalc(i)->Eval(fArgsTmp);
  //
}

//__________________________________________________________________________________________
inline Float_t AliCheb3D::Eval(Float_t  *par, int idim)
{
  // evaluate Chebyshev parameterization for idim-th output dimension of 3d->DimOut function
  for (int i=3;i--;) fArgsTmp[i] = MapToInternal(par[i],i);
  return GetChebCalc(idim)->Eval(fArgsTmp);
  //
}

//__________________________________________________________________________________________
inline void AliCheb3D::EvalDeriv3D(Float_t *par, Float_t dbdr[3][3])
{
  // return gradient matrix
  for (int i=3;i--;) fArgsTmp[i] = MapToInternal(par[i],i);
  for (int ib=3;ib--;) for (int id=3;id--;) dbdr[ib][id] = GetChebCalc(ib)->EvalDeriv(id,fArgsTmp)*fBScale[id];
}

//__________________________________________________________________________________________
inline void AliCheb3D::EvalDeriv3D2(Float_t *par, Float_t dbdrdr[3][3][3])
{
  // return gradient matrix
  for (int i=3;i--;) fArgsTmp[i] = MapToInternal(par[i],i);
  for (int ib=3;ib--;) for (int id=3;id--;)for (int id1=3;id1--;) 
    dbdrdr[ib][id][id1] = GetChebCalc(ib)->EvalDeriv2(id,id1,fArgsTmp)*fBScale[id]*fBScale[id1];
}

//__________________________________________________________________________________________
inline void AliCheb3D::EvalDeriv(int dimd,Float_t  *par, Float_t  *res)
{
  // evaluate Chebyshev parameterization derivative for 3d->DimOut function
  for (int i=3;i--;) fArgsTmp[i] = MapToInternal(par[i],i);
  for (int i=fDimOut;i--;) res[i] = GetChebCalc(i)->EvalDeriv(dimd,fArgsTmp)*fBScale[dimd];;
  //
}

//__________________________________________________________________________________________
inline void AliCheb3D::EvalDeriv2(int dimd1,int dimd2,Float_t  *par, Float_t  *res)
{
  // evaluate Chebyshev parameterization 2nd derivative over dimd1 and dimd2 dimensions for 3d->DimOut function
  for (int i=3;i--;) fArgsTmp[i] = MapToInternal(par[i],i);
  for (int i=fDimOut;i--;) res[i] = GetChebCalc(i)->EvalDeriv2(dimd1,dimd2,fArgsTmp)*fBScale[dimd1]*fBScale[dimd2];
  //
}

//__________________________________________________________________________________________
inline Float_t AliCheb3D::EvalDeriv(int dimd,Float_t  *par, int idim)
{
  // evaluate Chebyshev parameterization derivative over dimd dimention for idim-th output dimension of 3d->DimOut function
  for (int i=3;i--;) fArgsTmp[i] = MapToInternal(par[i],i);
  return GetChebCalc(idim)->EvalDeriv(dimd,fArgsTmp)*fBScale[dimd];
  //
}

//__________________________________________________________________________________________
inline Float_t AliCheb3D::EvalDeriv2(int dimd1,int dimd2,Float_t  *par, int idim)
{
  // evaluate Chebyshev parameterization 2ns derivative over dimd1 and dimd2 dimensions for idim-th output dimension of 3d->DimOut function
  for (int i=3;i--;) fArgsTmp[i] = MapToInternal(par[i],i);
  return GetChebCalc(idim)->EvalDeriv2(dimd1,dimd2,fArgsTmp)*fBScale[dimd1]*fBScale[dimd2];
  //
}

//__________________________________________________________________________________________
inline Float_t AliCheb3D::MapToInternal(Float_t  x,Int_t d) const
{
  // map x to [-1:1]
#ifdef _BRING_TO_BOUNDARY_
  float res = (x-fBOffset[d])*fBScale[d];
  if (res<-1) return -1;
  if (res> 1) return 1;
  return res;
#else
  return (x-fBOffset[d])*fBScale[d];
#endif
}

#endif
