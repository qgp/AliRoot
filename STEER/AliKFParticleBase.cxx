//---------------------------------------------------------------------------------
// Implementation of the AliKFParticleBase class
// .
// @author  S.Gorbunov, I.Kisel
// @version 1.0
// @since   13.05.07
// 
// Class to reconstruct and store the decayed particle parameters.
// The method is described in CBM-SOFT note 2007-003, 
// ``Reconstruction of decayed particles based on the Kalman filter'', 
// http://www.gsi.de/documents/DOC-2007-May-14-1.pdf
//
// This class describes general mathematics which is used by AliKFParticle class
// 
//  -= Copyright &copy ALICE HLT Group =-
//_________________________________________________________________________________


#include "AliKFParticleBase.h"
#include "TMath.h"

ClassImp(AliKFParticleBase)


AliKFParticleBase::AliKFParticleBase() :fQ(0), fNDF(-3), fChi2(0), fSFromDecay(0), fAtProductionVertex(0), fIsLinearized(0)
{ 
  //* Constructor 

  Initialize();
}

void AliKFParticleBase::Initialize( const Double_t Param[], const Double_t Cov[], Int_t Charge, Double_t Mass )
{
  // Constructor from "cartesian" track, particle mass hypothesis should be provided
  //
  // Param[6] = { X, Y, Z, Px, Py, Pz } - position and momentum
  // Cov [21] = lower-triangular part of the covariance matrix:
  //
  //                (  0  .  .  .  .  . )
  //                (  1  2  .  .  .  . )
  //  Cov. matrix = (  3  4  5  .  .  . ) - numbering of covariance elements in Cov[]
  //                (  6  7  8  9  .  . )
  //                ( 10 11 12 13 14  . )
  //                ( 15 16 17 18 19 20 )


  for( Int_t i=0; i<6 ; i++ ) fP[i] = Param[i];
  for( Int_t i=0; i<21; i++ ) fC[i] = Cov[i];

  Double_t energy = TMath::Sqrt( Mass*Mass + fP[3]*fP[3] + fP[4]*fP[4] + fP[5]*fP[5]);
  fP[6] = energy;
  fP[7] = 0;
  fQ = Charge;
  fNDF = 0;
  fChi2 = 0;
  fAtProductionVertex = 0;
  fIsLinearized = 0;
  fSFromDecay = 0;

  Double_t energyInv = 1./energy;
  Double_t 
    h0 = fP[3]*energyInv,
    h1 = fP[4]*energyInv,
    h2 = fP[5]*energyInv;

  fC[21] = h0*fC[ 6] + h1*fC[10] + h2*fC[15];
  fC[22] = h0*fC[ 7] + h1*fC[11] + h2*fC[16];
  fC[23] = h0*fC[ 8] + h1*fC[12] + h2*fC[17];
  fC[24] = h0*fC[ 9] + h1*fC[13] + h2*fC[18];
  fC[25] = h0*fC[13] + h1*fC[14] + h2*fC[19];
  fC[26] = h0*fC[18] + h1*fC[19] + h2*fC[20];
  fC[27] = h0*h0*fC[ 9] + h1*h1*fC[14] + h2*h2*fC[20] 
    + 2*(h0*h1*fC[13] + h0*h2*fC[18] + h1*h2*fC[19] );
  for( Int_t i=28; i<36; i++ ) fC[i] = 0;
  fC[35] = 1.;
}

void AliKFParticleBase::Initialize()
{
  //* Initialise covariance matrix and set current parameters to 0.0 

  for( Int_t i=0; i<8; i++) fP[i] = 0;
  for(Int_t i=0;i<36;++i) fC[i]=0.;
  fC[0] = fC[2] = fC[5] = 100.;
  fC[35] = 1.;
  fNDF  = -3;
  fChi2 =  0.;
  fQ = 0;
  fSFromDecay = 0;
  fAtProductionVertex = 0;
  fVtxGuess[0]=fVtxGuess[1]=fVtxGuess[2]=0.;
  fIsLinearized = 0;
}

void AliKFParticleBase::SetVtxGuess( Double_t x, Double_t y, Double_t z )
{
  //* Set decay vertex parameters for linearisation 

  fVtxGuess[0] = x;
  fVtxGuess[1] = y;
  fVtxGuess[2] = z;
  fIsLinearized = 1;
}


Int_t AliKFParticleBase::GetMomentum( Double_t &P, Double_t &Error )  const 
{
  //* Calculate particle momentum

  Double_t x = fP[3];
  Double_t y = fP[4];
  Double_t z = fP[5];
  Double_t x2 = x*x;
  Double_t y2 = y*y;
  Double_t z2 = z*z;
  Double_t p2 = x2+y2+z2;
  P = TMath::Sqrt(p2);
  Error = (x2*fC[9]+y2*fC[14]+z2*fC[20] + 2*(x*y*fC[13]+x*z*fC[18]+y*z*fC[19]) );
  if( Error>0 && P>1.e-4 ){
    Error = TMath::Sqrt(Error)/P;
    return 0;
  }
  return 1;
}

Int_t AliKFParticleBase::GetMass( Double_t &M, Double_t &Error ) const 
{
  //* Calculate particle mass
  
  // s = sigma^2 of m2/2

  Double_t s = (  fP[3]*fP[3]*fC[9] + fP[4]*fP[4]*fC[14] + fP[5]*fP[5]*fC[20] 
		  + fP[6]*fP[6]*fC[27] 
		+2*( + fP[3]*fP[4]*fC[13] + fP[5]*(fP[3]*fC[18] + fP[4]*fC[19]) 
		     - fP[6]*( fP[3]*fC[24] + fP[4]*fC[25] + fP[5]*fC[26] )   )
		 ); 
  Double_t m2 = fP[6]*fP[6] - fP[3]*fP[3] - fP[4]*fP[4] - fP[5]*fP[5];
  M     = 0;
  if( m2>1.e-20 ){
    M = TMath::Sqrt(m2);
    if( s>=0 ){
      Error = TMath::Sqrt(s/m2);
      return 0;
    }
  }
  Error = 1.e20;
  return 1;
}


Int_t AliKFParticleBase::GetDecayLength( Double_t &L, Double_t &Error ) const 
{
  //* Calculate particle decay length [cm]

  Double_t x = fP[3];
  Double_t y = fP[4];
  Double_t z = fP[5];
  Double_t t = fP[7];
  Double_t x2 = x*x;
  Double_t y2 = y*y;
  Double_t z2 = z*z;
  Double_t p2 = x2+y2+z2;
  L = t*TMath::Sqrt(p2);
  if( p2>1.e-4){
    Error = p2*fC[35] + t*t/p2*(x2*fC[9]+y2*fC[14]+z2*fC[20]
				+ 2*(x*y*fC[13]+x*z*fC[18]+y*z*fC[19]) )
      + 2*t*(x*fC[31]+y*fC[32]+z*fC[33]);
    Error = TMath::Sqrt(TMath::Abs(Error));
    return 0;
  }
  Error = 1.e20;
  return 1;
}

Int_t AliKFParticleBase::GetLifeTime( Double_t &TauC, Double_t &Error ) const 
{
  //* Calculate particle decay time [s]

  Double_t m, dm;
  GetMass( m, dm );
  Double_t cTM = (-fP[3]*fC[31] - fP[4]*fC[32] - fP[5]*fC[33] + fP[6]*fC[34]);
  TauC = fP[7]*m;
  Error = m*m*fC[35] + 2*fP[7]*cTM + fP[7]*fP[7]*dm*dm;
  if( Error > 0 ){
    Error = TMath::Sqrt( Error );
    return 0;
  }
  Error = 1.e20;
  return 1;
}


void AliKFParticleBase::operator +=( const AliKFParticleBase &Daughter )
{
  //* Add daughter via operator+=

  AddDaughter( Daughter );
}
  

void AliKFParticleBase::GetMeasurement( const Double_t XYZ[], Double_t m[], Double_t V[] ) const
{
  //* Get additional covariances V used during measurement

  Double_t b[3];
  GetFieldValue( XYZ, b );
  const Double_t kCLight =  0.000299792458;
  b[0]*=kCLight; b[1]*=kCLight; b[2]*=kCLight;

  Transport( GetDStoPoint(XYZ), m, V );

  Double_t d[3] = { XYZ[0]-m[0], XYZ[1]-m[1], XYZ[2]-m[2] };
  Double_t p2 = m[3]*m[3]+m[4]*m[4]+m[5]*m[5];
  Double_t sigmaS = (p2>1.e-4) ? ( .1+TMath::Sqrt( 100*(d[0]*d[0]+d[1]*d[1]+d[2]*d[2])) )/TMath::Sqrt(p2) : 1.;

  Double_t h[6];

  h[0] = m[3]*sigmaS;
  h[1] = m[4]*sigmaS;
  h[2] = m[5]*sigmaS;
  h[3] = ( h[1]*b[2]-h[2]*b[1] )*GetQ();
  h[4] = ( h[2]*b[0]-h[0]*b[2] )*GetQ();
  h[5] = ( h[0]*b[1]-h[1]*b[0] )*GetQ();
    
  //* Fit of momentum (Px,Py,Pz) to XYZ point
  if(0){      
    Double_t mVv[6] = 
      { V[ 0] + h[0]*h[0], 
	V[ 1] + h[0]*h[1], V[ 2] + h[1]*h[1],			     
	V[ 3] + h[0]*h[2], V[ 4] + h[1]*h[2], V[ 5] + h[2]*h[2] };
    
    Double_t mVvp[9]=
      { V[ 6] + h[0]*h[3], V[ 7] + h[1]*h[3], V[ 8] + h[2]*h[3],
	V[10] + h[0]*h[4], V[11] + h[1]*h[4], V[12] + h[2]*h[4],
	V[15] + h[0]*h[5], V[16] + h[1]*h[5], V[17] + h[2]*h[5] };
    
    Double_t mS[6] = 
      { mVv[2]*mVv[5] - mVv[4]*mVv[4],
	mVv[3]*mVv[4] - mVv[1]*mVv[5], mVv[0]*mVv[5] - mVv[3]*mVv[3],
	mVv[1]*mVv[4] - mVv[2]*mVv[3], mVv[1]*mVv[3] - mVv[0]*mVv[4], 
	mVv[0]*mVv[2] - mVv[1]*mVv[1] };		 
    
    Double_t s = ( mVv[0]*mS[0] + mVv[1]*mS[1] + mVv[3]*mS[3] );    
    s = ( s > 1.E-20 )  ?1./s :0;
        
    Double_t mSz[3] = { mS[0]*d[0]+mS[1]*d[1]+mS[3]*d[2],
			mS[1]*d[0]+mS[2]*d[1]+mS[4]*d[2],
			mS[3]*d[0]+mS[4]*d[1]+mS[5]*d[2] };
    
    Double_t px = m[3] + s*( mVvp[0]*mSz[0] + mVvp[1]*mSz[1] + mVvp[2]*mSz[2] );
    Double_t py = m[4] + s*( mVvp[3]*mSz[0] + mVvp[4]*mSz[1] + mVvp[5]*mSz[2] );
    Double_t pz = m[5] + s*( mVvp[6]*mSz[0] + mVvp[7]*mSz[1] + mVvp[8]*mSz[2] );

    h[0] = px*sigmaS;
    h[1] = py*sigmaS;
    h[2] = pz*sigmaS;
    h[3] = ( h[1]*b[2]-h[2]*b[1] )*GetQ();
    h[4] = ( h[2]*b[0]-h[0]*b[2] )*GetQ();
    h[5] = ( h[0]*b[1]-h[1]*b[0] )*GetQ();
  }
   
  V[ 0]+= h[0]*h[0];
  V[ 1]+= h[1]*h[0];
  V[ 2]+= h[1]*h[1];
  V[ 3]+= h[2]*h[0];
  V[ 4]+= h[2]*h[1];
  V[ 5]+= h[2]*h[2];

  V[ 6]+= h[3]*h[0];
  V[ 7]+= h[3]*h[1];
  V[ 8]+= h[3]*h[2];
  V[ 9]+= h[3]*h[3];

  V[10]+= h[4]*h[0];
  V[11]+= h[4]*h[1];
  V[12]+= h[4]*h[2];
  V[13]+= h[4]*h[3];
  V[14]+= h[4]*h[4];

  V[15]+= h[5]*h[0];
  V[16]+= h[5]*h[1];
  V[17]+= h[5]*h[2];
  V[18]+= h[5]*h[3];
  V[19]+= h[5]*h[4];
  V[20]+= h[5]*h[5];
}

  
void AliKFParticleBase::AddDaughter( const AliKFParticleBase &Daughter )
{
  //* Add daughter 

  if( fNDF<-1 ){ // first daughter -> just copy
    fNDF   = -1;
    fQ     =  Daughter.GetQ();
    for( Int_t i=0; i<7; i++) fP[i] = Daughter.fP[i];
    for( Int_t i=0; i<28; i++) fC[i] = Daughter.fC[i];
    fSFromDecay = 0;
    return;
  }

  TransportToDecayVertex();

  Double_t b[3]; 
  Int_t maxIter = 1;

  if( !fIsLinearized ){
    if( fNDF==-1 ){
      Double_t ds, ds1;
      GetDStoParticle(Daughter, ds, ds1);      
      TransportToDS( ds );
      Double_t m[8];
      Double_t mCd[36];       
      Daughter.Transport( ds1, m, mCd );    
      fVtxGuess[0] = .5*( fP[0] + m[0] );
      fVtxGuess[1] = .5*( fP[1] + m[1] );
      fVtxGuess[2] = .5*( fP[2] + m[2] );
    } else {
      fVtxGuess[0] = fP[0];
      fVtxGuess[1] = fP[1];
      fVtxGuess[2] = fP[2]; 
    }
    maxIter = 3;
  }

  for( Int_t iter=0; iter<maxIter; iter++ ){

    {
      GetFieldValue( fVtxGuess, b );
      const Double_t kCLight =  0.000299792458;
      b[0]*=kCLight; b[1]*=kCLight; b[2]*=kCLight;
    }

    Double_t *ffP = fP, *ffC = fC, tmpP[8], tmpC[36];
    if( fNDF==-1 ){      
      GetMeasurement( fVtxGuess, tmpP, tmpC );
      ffP = tmpP;
      ffC = tmpC;
    }

    Double_t m[8], mV[36];
    if( Daughter.fC[35]>0 ){
      Daughter.GetMeasurement( fVtxGuess, m, mV );
    } else {
      for( Int_t i=0; i<8; i++ ) m[i] = Daughter.fP[i];
      for( Int_t i=0; i<36; i++ ) mV[i] = Daughter.fC[i];
    }

    //*

    Double_t mS[6];
    {
      Double_t mSi[6] = { ffC[0]+mV[0], 
			  ffC[1]+mV[1], ffC[2]+mV[2], 
			  ffC[3]+mV[3], ffC[4]+mV[4], ffC[5]+mV[5] };
      
      mS[0] = mSi[2]*mSi[5] - mSi[4]*mSi[4];
      mS[1] = mSi[3]*mSi[4] - mSi[1]*mSi[5];
      mS[2] = mSi[0]*mSi[5] - mSi[3]*mSi[3];
      mS[3] = mSi[1]*mSi[4] - mSi[2]*mSi[3];
      mS[4] = mSi[1]*mSi[3] - mSi[0]*mSi[4];
      mS[5] = mSi[0]*mSi[2] - mSi[1]*mSi[1];	 
      
      Double_t s = ( mSi[0]*mS[0] + mSi[1]*mS[1] + mSi[3]*mS[3] );
      s = ( s > 1.E-20 )  ?1./s :0;	  
      mS[0]*=s;
      mS[1]*=s;
      mS[2]*=s;
      mS[3]*=s;
      mS[4]*=s;
      mS[5]*=s;
    }
    
    //* Residual (measured - estimated)
    
    Double_t zeta[3] = { m[0]-ffP[0], m[1]-ffP[1], m[2]-ffP[2] };    
    
    //* CHt = CH' - D'
    
    Double_t mCHt0[7], mCHt1[7], mCHt2[7];
    
    mCHt0[0]=ffC[ 0] ;       mCHt1[0]=ffC[ 1] ;       mCHt2[0]=ffC[ 3] ;
    mCHt0[1]=ffC[ 1] ;       mCHt1[1]=ffC[ 2] ;       mCHt2[1]=ffC[ 4] ;
    mCHt0[2]=ffC[ 3] ;       mCHt1[2]=ffC[ 4] ;       mCHt2[2]=ffC[ 5] ;
    mCHt0[3]=ffC[ 6]-mV[ 6]; mCHt1[3]=ffC[ 7]-mV[ 7]; mCHt2[3]=ffC[ 8]-mV[ 8];
    mCHt0[4]=ffC[10]-mV[10]; mCHt1[4]=ffC[11]-mV[11]; mCHt2[4]=ffC[12]-mV[12];
    mCHt0[5]=ffC[15]-mV[15]; mCHt1[5]=ffC[16]-mV[16]; mCHt2[5]=ffC[17]-mV[17];
    mCHt0[6]=ffC[21]-mV[21]; mCHt1[6]=ffC[22]-mV[22]; mCHt2[6]=ffC[23]-mV[23];
  
    //* Kalman gain K = mCH'*S
    
    Double_t k0[7], k1[7], k2[7];
    
    for(Int_t i=0;i<7;++i){
      k0[i] = mCHt0[i]*mS[0] + mCHt1[i]*mS[1] + mCHt2[i]*mS[3];
      k1[i] = mCHt0[i]*mS[1] + mCHt1[i]*mS[2] + mCHt2[i]*mS[4];
      k2[i] = mCHt0[i]*mS[3] + mCHt1[i]*mS[4] + mCHt2[i]*mS[5];
    }

   //* New estimation of the vertex position 

    if( iter<maxIter-1 ){
      for(Int_t i=0; i<3; ++i) 
	fVtxGuess[i]= ffP[i] + k0[i]*zeta[0]+k1[i]*zeta[1]+k2[i]*zeta[2];
      continue;
    }

    // last itearation -> update the particle

    //* Add the daughter momentum to the particle momentum
    
    ffP[ 3] += m[ 3];
    ffP[ 4] += m[ 4];
    ffP[ 5] += m[ 5];
    ffP[ 6] += m[ 6];
  
    ffC[ 9] += mV[ 9];
    ffC[13] += mV[13];
    ffC[14] += mV[14];
    ffC[18] += mV[18];
    ffC[19] += mV[19];
    ffC[20] += mV[20];
    ffC[24] += mV[24];
    ffC[25] += mV[25];
    ffC[26] += mV[26];
    ffC[27] += mV[27];
    
    //* New estimation of the vertex position r += K*zeta
    
    for(Int_t i=0;i<7;++i) 
      fP[i] = ffP[i] + k0[i]*zeta[0] + k1[i]*zeta[1] + k2[i]*zeta[2];
    
    //* New covariance matrix C -= K*(mCH')'
   
    for(Int_t i=0, k=0;i<7;++i){
      for(Int_t j=0;j<=i;++j,++k) 
	fC[k] = ffC[k] - (k0[i]*mCHt0[j] + k1[i]*mCHt1[j] + k2[i]*mCHt2[j] );
    }
    
    //* Calculate Chi^2 

    fNDF  += 2;
    fQ    +=  Daughter.GetQ();
    fSFromDecay = 0;    
    fChi2 += (mS[0]*zeta[0] + mS[1]*zeta[1] + mS[3]*zeta[2])*zeta[0]
      +      (mS[1]*zeta[0] + mS[2]*zeta[1] + mS[4]*zeta[2])*zeta[1]
      +      (mS[3]*zeta[0] + mS[4]*zeta[1] + mS[5]*zeta[2])*zeta[2];  
  }
}

void AliKFParticleBase::SetProductionVertex( const AliKFParticleBase &Vtx )
{
  //* Set production vertex for the particle

  const Double_t *m = Vtx.fP, *mV = Vtx.fC;

  Bool_t noS = ( fC[35]<=0 ); // no decay length allowed

  if( noS ){ 
    TransportToDecayVertex();
    fP[7] = 0;
    fC[28] = fC[29] = fC[30] = fC[31] = fC[32] = fC[33] = fC[35] = fC[35] = 0;
  } else {
    TransportToDS( GetDStoPoint( m ) );
    fP[7] = -fSFromDecay;
    Convert(1);
  }

 
  Double_t mAi[6];
  mAi[0] = fC[4]*fC[4] - fC[2]*fC[5];
  mAi[1] = fC[1]*fC[5] - fC[3]*fC[4];
  mAi[3] = fC[2]*fC[3] - fC[1]*fC[4];
  Double_t det = 1./(fC[0]*mAi[0] + fC[1]*mAi[1] + fC[3]*mAi[3]);
  mAi[0] *= det;
  mAi[1] *= det;
  mAi[3] *= det;
  mAi[2] = ( fC[3]*fC[3] - fC[0]*fC[5] )*det;
  mAi[4] = ( fC[0]*fC[4] - fC[1]*fC[3] )*det;
  mAi[5] = ( fC[1]*fC[1] - fC[0]*fC[2] )*det;

  Double_t mB[5][3];

  mB[0][0] = fC[ 6]*mAi[0] + fC[ 7]*mAi[1] + fC[ 8]*mAi[3];
  mB[0][1] = fC[ 6]*mAi[1] + fC[ 7]*mAi[2] + fC[ 8]*mAi[4];
  mB[0][2] = fC[ 6]*mAi[3] + fC[ 7]*mAi[4] + fC[ 8]*mAi[5];

  mB[1][0] = fC[10]*mAi[0] + fC[11]*mAi[1] + fC[12]*mAi[3];
  mB[1][1] = fC[10]*mAi[1] + fC[11]*mAi[2] + fC[12]*mAi[4];
  mB[1][2] = fC[10]*mAi[3] + fC[11]*mAi[4] + fC[12]*mAi[5];

  mB[2][0] = fC[15]*mAi[0] + fC[16]*mAi[1] + fC[17]*mAi[3];
  mB[2][1] = fC[15]*mAi[1] + fC[16]*mAi[2] + fC[17]*mAi[4];
  mB[2][2] = fC[15]*mAi[3] + fC[16]*mAi[4] + fC[17]*mAi[5];

  mB[3][0] = fC[21]*mAi[0] + fC[22]*mAi[1] + fC[23]*mAi[3];
  mB[3][1] = fC[21]*mAi[1] + fC[22]*mAi[2] + fC[23]*mAi[4];
  mB[3][2] = fC[21]*mAi[3] + fC[22]*mAi[4] + fC[23]*mAi[5];

  mB[4][0] = fC[28]*mAi[0] + fC[29]*mAi[1] + fC[30]*mAi[3];
  mB[4][1] = fC[28]*mAi[1] + fC[29]*mAi[2] + fC[30]*mAi[4];
  mB[4][2] = fC[28]*mAi[3] + fC[29]*mAi[4] + fC[30]*mAi[5];

  Double_t z[3] = { m[0]-fP[0], m[1]-fP[1], m[2]-fP[2] };

  {
    Double_t mAV[6] = { fC[0]-mV[0], fC[1]-mV[1], fC[2]-mV[2], 
			fC[3]-mV[3], fC[4]-mV[4], fC[5]-mV[5] };
    Double_t mAVi[6];
    mAVi[0] = mAV[4]*mAV[4] - mAV[2]*mAV[5];
    mAVi[1] = mAV[1]*mAV[5] - mAV[3]*mAV[4];
    mAVi[2] = mAV[3]*mAV[3] - mAV[0]*mAV[5] ;
    mAVi[3] = mAV[2]*mAV[3] - mAV[1]*mAV[4];
    mAVi[4] = mAV[0]*mAV[4] - mAV[1]*mAV[3] ;
    mAVi[5] = mAV[1]*mAV[1] - mAV[0]*mAV[2] ;

    det = ( mAV[0]*mAVi[0] + mAV[1]*mAVi[1] + mAV[3]*mAVi[3] );
    if( det>1.e-8 ) det = 1./det;

    fNDF  += 2;
    fChi2 += 
      ( +(mAVi[0]*z[0] + mAVi[1]*z[1] + mAVi[3]*z[2])*z[0]
	+(mAVi[1]*z[0] + mAVi[2]*z[1] + mAVi[4]*z[2])*z[1]
	+(mAVi[3]*z[0] + mAVi[4]*z[1] + mAVi[5]*z[2])*z[2] )*det;
  }
  
  fP[0] = m[0];
  fP[1] = m[1];
  fP[2] = m[2];
  fP[3]+= mB[0][0]*z[0] + mB[0][1]*z[1] + mB[0][2]*z[2];
  fP[4]+= mB[1][0]*z[0] + mB[1][1]*z[1] + mB[1][2]*z[2];
  fP[5]+= mB[2][0]*z[0] + mB[2][1]*z[1] + mB[2][2]*z[2];
  fP[6]+= mB[3][0]*z[0] + mB[3][1]*z[1] + mB[3][2]*z[2];
  fP[7]+= mB[4][0]*z[0] + mB[4][1]*z[1] + mB[4][2]*z[2];
  
  Double_t d0, d1, d2;

  fC[0] = mV[0];
  fC[1] = mV[1];
  fC[2] = mV[2];
  fC[3] = mV[3];
  fC[4] = mV[4];
  fC[5] = mV[5];

  d0= mB[0][0]*mV[0] + mB[0][1]*mV[1] + mB[0][2]*mV[3] - fC[ 6];
  d1= mB[0][0]*mV[1] + mB[0][1]*mV[2] + mB[0][2]*mV[4] - fC[ 7];
  d2= mB[0][0]*mV[3] + mB[0][1]*mV[4] + mB[0][2]*mV[5] - fC[ 8];

  fC[ 6]+= d0;
  fC[ 7]+= d1;
  fC[ 8]+= d2;
  fC[ 9]+= d0*mB[0][0] + d1*mB[0][1] + d2*mB[0][2];

  d0= mB[1][0]*mV[0] + mB[1][1]*mV[1] + mB[1][2]*mV[3] - fC[10];
  d1= mB[1][0]*mV[1] + mB[1][1]*mV[2] + mB[1][2]*mV[4] - fC[11];
  d2= mB[1][0]*mV[3] + mB[1][1]*mV[4] + mB[1][2]*mV[5] - fC[12];

  fC[10]+= d0;
  fC[11]+= d1;
  fC[12]+= d2;
  fC[13]+= d0*mB[0][0] + d1*mB[0][1] + d2*mB[0][2];
  fC[14]+= d0*mB[1][0] + d1*mB[1][1] + d2*mB[1][2];

  d0= mB[2][0]*mV[0] + mB[2][1]*mV[1] + mB[2][2]*mV[3] - fC[15];
  d1= mB[2][0]*mV[1] + mB[2][1]*mV[2] + mB[2][2]*mV[4] - fC[16];
  d2= mB[2][0]*mV[3] + mB[2][1]*mV[4] + mB[2][2]*mV[5] - fC[17];

  fC[15]+= d0;
  fC[16]+= d1;
  fC[17]+= d2;
  fC[18]+= d0*mB[0][0] + d1*mB[0][1] + d2*mB[0][2];
  fC[19]+= d0*mB[1][0] + d1*mB[1][1] + d2*mB[1][2];
  fC[20]+= d0*mB[2][0] + d1*mB[2][1] + d2*mB[2][2];

  d0= mB[3][0]*mV[0] + mB[3][1]*mV[1] + mB[3][2]*mV[3] - fC[21];
  d1= mB[3][0]*mV[1] + mB[3][1]*mV[2] + mB[3][2]*mV[4] - fC[22];
  d2= mB[3][0]*mV[3] + mB[3][1]*mV[4] + mB[3][2]*mV[5] - fC[23];

  fC[21]+= d0;
  fC[22]+= d1;
  fC[23]+= d2;
  fC[24]+= d0*mB[0][0] + d1*mB[0][1] + d2*mB[0][2];
  fC[25]+= d0*mB[1][0] + d1*mB[1][1] + d2*mB[1][2];
  fC[26]+= d0*mB[2][0] + d1*mB[2][1] + d2*mB[2][2];
  fC[27]+= d0*mB[3][0] + d1*mB[3][1] + d2*mB[3][2];

  d0= mB[4][0]*mV[0] + mB[4][1]*mV[1] + mB[4][2]*mV[3] - fC[28];
  d1= mB[4][0]*mV[1] + mB[4][1]*mV[2] + mB[4][2]*mV[4] - fC[29];
  d2= mB[4][0]*mV[3] + mB[4][1]*mV[4] + mB[4][2]*mV[5] - fC[30];

  fC[28]+= d0;
  fC[29]+= d1;
  fC[30]+= d2;
  fC[31]+= d0*mB[0][0] + d1*mB[0][1] + d2*mB[0][2];
  fC[32]+= d0*mB[1][0] + d1*mB[1][1] + d2*mB[1][2];
  fC[33]+= d0*mB[2][0] + d1*mB[2][1] + d2*mB[2][2];
  fC[34]+= d0*mB[3][0] + d1*mB[3][1] + d2*mB[3][2];
  fC[35]+= d0*mB[4][0] + d1*mB[4][1] + d2*mB[4][2];
  
  if( noS ){ 
    fP[7] = 0;
    fC[28] = fC[29] = fC[30] = fC[31] = fC[32] = fC[33] = fC[35] = fC[35] = 0;
  } else {
    TransportToDS( fP[7] );
    Convert(0);
  }

  fSFromDecay = 0;
}




void AliKFParticleBase::SetMassConstraint( Double_t Mass, Double_t SigmaMass )
{  
  //* Set hard mass constraint 

  Double_t mH[8];
  mH[0] = mH[1] = mH[2] = 0.;
  mH[3] = -2*fP[3]; 
  mH[4] = -2*fP[4]; 
  mH[5] = -2*fP[5]; 
  mH[6] =  2*fP[6];
  mH[7] = 0; 
  Double_t m2 = ( fP[6]*fP[6] 
		- fP[3]*fP[3] - fP[4]*fP[4] - fP[5]*fP[5] ); 

  Double_t zeta = Mass*Mass - m2;
  for(Int_t i=0;i<8;++i) zeta -= mH[i]*(fP[i]-fP[i]);
  
  Double_t s = 4*Mass*Mass*SigmaMass*SigmaMass;
  Double_t mCHt[8];
  for (Int_t i=0;i<8;++i ){
    mCHt[i] = 0.0;
    for (Int_t j=0;j<8;++j) mCHt[i] += Cij(i,j)*mH[j];
    s += mH[i]*mCHt[i];
  }
  
  if( s<1.e-20 ) return;
  s = 1./s;
  fChi2 += zeta*zeta*s;
  fNDF  += 1;
  for( Int_t i=0, ii=0; i<8; ++i ){
    Double_t ki = mCHt[i]*s;
    fP[i]+= ki*zeta;
    for(Int_t j=0;j<=i;++j) fC[ii++] -= ki*mCHt[j];    
  }
}


void AliKFParticleBase::SetNoDecayLength()
{  
  //* Set no decay length for resonances

  TransportToDecayVertex();

  Double_t h[8];
  h[0] = h[1] = h[2] = h[3] = h[4] = h[5] = h[6] = 0;
  h[7] = 1; 

  Double_t zeta = 0 - fP[7];
  for(Int_t i=0;i<8;++i) zeta -= h[i]*(fP[i]-fP[i]);
  
  Double_t s = fC[35];   
  if( s>1.e-20 ){
    s = 1./s;
    fChi2 += zeta*zeta*s;
    fNDF  += 1;
    for( Int_t i=0, ii=0; i<7; ++i ){
      Double_t ki = fC[28+i]*s;
      fP[i]+= ki*zeta;
      for(Int_t j=0;j<=i;++j) fC[ii++] -= ki*fC[28+j];    
    }
  }
  fP[7] = 0;
  fC[28] = fC[29] = fC[30] = fC[31] = fC[32] = fC[33] = fC[35] = fC[35] = 0;
}


void AliKFParticleBase::Construct( const AliKFParticleBase* vDaughters[], Int_t NDaughters,
				   const AliKFParticleBase *Parent,  Double_t Mass         )
{ 
  //* Full reconstruction in one go

  Int_t maxIter = 1;
  bool wasLinearized = fIsLinearized;
  if( !fIsLinearized ){
    //fVtxGuess[0] = fVtxGuess[1] = fVtxGuess[2] = 0;  //!!!!
    fVtxGuess[0] = GetX();
    fVtxGuess[1] = GetY();
    fVtxGuess[2] = GetZ();
    fIsLinearized = 1;
    maxIter = 3;
  }

  for( Int_t iter=0; iter<maxIter; iter++ ){
    fAtProductionVertex = 0;
    fSFromDecay = 0;
    fP[0] = fVtxGuess[0];
    fP[1] = fVtxGuess[1];
    fP[2] = fVtxGuess[2];
    fP[3] = 0;
    fP[4] = 0;
    fP[5] = 0;
    fP[6] = 0;
    fP[7] = 0;
  
    for(Int_t i=0;i<36;++i) fC[i]=0.;

    fC[0] = fC[2] = fC[5] = 100.;
    fC[35] = 1.;
    
    fNDF  = -3;
    fChi2 =  0.;
    fQ = 0;

    for( Int_t itr =0; itr<NDaughters; itr++ ){
      AddDaughter( *vDaughters[itr] );    
    }
    if( iter<maxIter-1){
      for( Int_t i=0; i<3; i++ ) fVtxGuess[i] = fP[i];  
    }
  }
  fIsLinearized = wasLinearized;    

  if( Mass>=0 ) SetMassConstraint( Mass );
  if( Parent ) SetProductionVertex( *Parent );
}


void AliKFParticleBase::Convert( bool ToProduction )
{
  //* Tricky function - convert the particle error along its trajectory to 
  //* the value which corresponds to its production/decay vertex
  //* It is done by combination of the error of decay length with the position errors

  Double_t fld[3]; 
  {
    GetFieldValue( fP, fld );
    const Double_t kCLight =  fQ*0.000299792458;
    fld[0]*=kCLight; fld[1]*=kCLight; fld[2]*=kCLight;
  }

  Double_t h[6];
  
  h[0] = fP[3];
  h[1] = fP[4];
  h[2] = fP[5];
  if( ToProduction ){ h[0]=-h[0]; h[1]=-h[1]; h[2]=-h[2]; } 
  h[3] = h[1]*fld[2]-h[2]*fld[1];
  h[4] = h[2]*fld[0]-h[0]*fld[2];
  h[5] = h[0]*fld[1]-h[1]*fld[0];
  
  Double_t c;

  c = fC[28]+h[0]*fC[35];
  fC[ 0]+= h[0]*(c+fC[28]);
  fC[28] = c;

  fC[ 1]+= h[1]*fC[28] + h[0]*fC[29];
  c = fC[29]+h[1]*fC[35];
  fC[ 2]+= h[1]*(c+fC[29]);
  fC[29] = c;

  fC[ 3]+= h[2]*fC[28] + h[0]*fC[30];
  fC[ 4]+= h[2]*fC[29] + h[1]*fC[30];
  c = fC[30]+h[2]*fC[35];
  fC[ 5]+= h[2]*(c+fC[30]);
  fC[30] = c;

  fC[ 6]+= h[3]*fC[28] + h[0]*fC[31];
  fC[ 7]+= h[3]*fC[29] + h[1]*fC[31];
  fC[ 8]+= h[3]*fC[30] + h[2]*fC[31];
  c = fC[31]+h[3]*fC[35];
  fC[ 9]+= h[3]*(c+fC[31]);
  fC[31] = c;
  
  fC[10]+= h[4]*fC[28] + h[0]*fC[32];
  fC[11]+= h[4]*fC[29] + h[1]*fC[32];
  fC[12]+= h[4]*fC[30] + h[2]*fC[32];
  fC[13]+= h[4]*fC[31] + h[3]*fC[32];
  c = fC[32]+h[4]*fC[35];
  fC[14]+= h[4]*(c+fC[32]);
  fC[32] = c;
  
  fC[15]+= h[5]*fC[28] + h[0]*fC[33];
  fC[16]+= h[5]*fC[29] + h[1]*fC[33];
  fC[17]+= h[5]*fC[30] + h[2]*fC[33];
  fC[18]+= h[5]*fC[31] + h[3]*fC[33];
  fC[19]+= h[5]*fC[32] + h[4]*fC[33];
  c = fC[33]+h[5]*fC[35];
  fC[20]+= h[5]*(c+fC[33]);
  fC[33] = c;

  fC[21]+= h[0]*fC[34];
  fC[22]+= h[1]*fC[34];
  fC[23]+= h[2]*fC[34];
  fC[24]+= h[3]*fC[34];
  fC[25]+= h[4]*fC[34];
  fC[26]+= h[5]*fC[34];
}


void AliKFParticleBase::TransportToDecayVertex()
{
  //* Transport the particle to its decay vertex 

  if( fSFromDecay != 0 ) TransportToDS( -fSFromDecay );
  if( fAtProductionVertex ) Convert(0);
  fAtProductionVertex = 0;
}

void AliKFParticleBase::TransportToProductionVertex()
{
  //* Transport the particle to its production vertex 
  
  if( fSFromDecay != -fP[7] ) TransportToDS( -fSFromDecay-fP[7] );
  if( !fAtProductionVertex ) Convert( 1 );
  fAtProductionVertex = 1;
}


void AliKFParticleBase::TransportToDS( Double_t dS )
{ 
  //* Transport the particle on dS parameter (SignedPath/Momentum) 
 
  Transport( dS, fP, fC );
  fSFromDecay+= dS;
}


Double_t AliKFParticleBase::GetDStoPointLine( const Double_t xyz[] ) const 
{
  //* Get dS to a certain space point without field

  Double_t p2 = fP[3]*fP[3] + fP[4]*fP[4] + fP[5]*fP[5];  
  if( p2<1.e-4 ) p2 = 1;
  return ( fP[3]*(xyz[0]-fP[0]) + fP[4]*(xyz[1]-fP[1]) + fP[5]*(xyz[2]-fP[2]) )/p2;
}


Double_t AliKFParticleBase::GetDStoPointBz( Double_t B, const Double_t xyz[] ) 
  const
{ 
  
  //* Get dS to a certain space point for Bz field
  const Double_t kCLight = 0.000299792458;
  Double_t bq = B*fQ*kCLight;
  Double_t pt2 = fP[3]*fP[3] + fP[4]*fP[4];
  if( pt2<1.e-4 ) return 0;
  Double_t dx = xyz[0] - fP[0];
  Double_t dy = xyz[1] - fP[1]; 
  Double_t a = dx*fP[3]+dy*fP[4];
  if( TMath::Abs(bq)<1.e-8 ) return a/pt2;  
  return  TMath::ATan2( bq*a, pt2 + bq*(dy*fP[3] -dx*fP[4]) )/bq;
}


void AliKFParticleBase::GetDStoParticleBz( Double_t B, const AliKFParticleBase &p, 
					   Double_t &DS, Double_t &DS1 ) 
  const
{ 
  //* Get dS to another particle for Bz field
  Double_t px = fP[3];
  Double_t py = fP[4];
  Double_t pz = fP[5];

  Double_t px1 = p.fP[3];
  Double_t py1 = p.fP[4];
  Double_t pz1 = p.fP[5];

  const Double_t kCLight = 0.000299792458;

  Double_t bq = B*fQ*kCLight;
  Double_t bq1 = B*p.fQ*kCLight;
  Double_t s=0, ds=0, s1=0, ds1=0;
  
  if( TMath::Abs(bq)>1.e-8 || TMath::Abs(bq1)>1.e-8 ){

    Double_t dx = (p.fP[0] - fP[0]);
    Double_t dy = (p.fP[1] - fP[1]);
    Double_t d2 = (dx*dx+dy*dy);
    
    Double_t p2  = (px *px  + py *py); 
    Double_t p21 = (px1*px1 + py1*py1);
    
    Double_t a = (px*py1 - py*px1);
    Double_t b = (px*px1 + py*py1);
    
    Double_t ldx = bq*bq1*dx - bq1*py + bq*py1 ;
    Double_t ldy = bq*bq1*dy + bq1*px - bq*px1 ;
    Double_t l2 = ldx*ldx + ldy*ldy;
    
    Double_t cS = bq1*p2 + bq*bq1*(dy* px - dx* py) -  bq*b;
    Double_t cS1= bq*p21 - bq*bq1*(dy*px1 - dx*py1) - bq1*b;

    Double_t ca  = bq*bq*bq1*d2  +2*( cS + bq*bq*(py1*dx-px1*dy)) ;
    Double_t ca1 = bq*bq1*bq1*d2 +2*( cS1 - bq1*bq1*(py*dx-px*dy)) ;  
  
    Double_t sa = 4*l2*p2 - ca*ca;
    Double_t sa1 = 4*l2*p21 - ca1*ca1;

    if(sa<0) sa=0;
    if(sa1<0)sa1=0;

    if( TMath::Abs(bq)>1.e-8){
      s  = TMath::ATan2(   bq*( bq1*(dx*px +dy*py) + a ) , cS )/bq;
      ds = TMath::ATan2(TMath::Sqrt(sa),ca)/bq;
    } else {
      s = ( (dx*px + dy*py) + (py*px1-px*py1)/bq1)/p2;
      ds = s*s - (d2-2*(px1*dy-py1*dx)/bq1)/p2; 
      if( ds<0 ) ds = 0;
      ds = TMath::Sqrt(ds);   
    }
    
    if( TMath::Abs(bq1)>1.e-8){
      s1 = TMath::ATan2( -bq1*( bq*(dx*px1+dy*py1) + a), cS1 )/bq1;
      ds1 = TMath::ATan2(TMath::Sqrt(sa1),ca1)/bq1;  
    } else {
      s1 = (-(dx*px1 + dy*py1) + (py*px1-px*py1)/bq)/p21;
      ds1 = s1*s1 - (d2+2*(px*dy-py*dx)/bq)/p21; 
      if( ds1<0 ) ds1 = 0;
      ds1 = TMath::Sqrt(ds1);
    }
  }

  Double_t ss[2], ss1[2], g[2][5],g1[2][5];
  
  ss[0] = s + ds;
  ss[1] = s - ds;
  ss1[0] = s1 + ds1;
  ss1[1] = s1 - ds1;
  for( Int_t i=0; i<2; i++){
    Double_t bs = bq*ss[i];
    Double_t c = TMath::Cos(bs), s = TMath::Sin(bs);
    Double_t cB,sB;
    if( TMath::Abs(bq)>1.e-8){
      cB= (1-c)/bq;     
      sB= s/bq;  
    }else{
      sB = (1. - bs*bs/6.)*ss[i];
      cB = .5*sB*bs;
    }
    g[i][0] = fP[0] + sB*px + cB*py;
    g[i][1] = fP[1] - cB*px + sB*py;
    g[i][2] = fP[2] + ss[i]*pz;
    g[i][3] =       + c*px + s*py;
    g[i][4] =       - s*px + c*py;

    bs = bq1*ss1[i];  
    c =  TMath::Cos(bs); s = TMath::Sin(bs);
    if( TMath::Abs(bq1)>1.e-8){
      cB= (1-c)/bq1;   
      sB= s/bq1;  
    }else{
      sB = (1. - bs*bs/6.)*ss1[i];
      cB = .5*sB*bs;
    }
      
    g1[i][0] = p.fP[0] + sB*px1 + cB*py1;
    g1[i][1] = p.fP[1] - cB*px1 + sB*py1;
    g1[i][2] = p.fP[2] + ss[i]*pz1;
    g1[i][3] =         + c*px1 + s*py1;
    g1[i][4] =         - s*px1 + c*py1;
  }

  Int_t i=0, i1=0;
  
  Double_t dMin = 1.e10;
  for( Int_t j=0; j<2; j++){
    for( Int_t j1=0; j1<2; j1++){
      Double_t xx = g[j][0]-g1[j1][0];
      Double_t yy = g[j][1]-g1[j1][1];
      Double_t zz = g[j][2]-g1[j1][2];
      Double_t d = xx*xx + yy*yy + zz*zz;
      if( d<dMin ){
	dMin = d;
	i = j;
	i1 = j1;
      }
    }
  }  

  DS = ss[i];
  DS1 = ss1[i1];
  if(0){
    Double_t x= g[i][0], y= g[i][1], z= g[i][2], ppx= g[i][3], ppy= g[i][4];  
    Double_t x1=g1[i1][0], y1= g1[i1][1], z1= g1[i1][2], ppx1= g1[i1][3], ppy1= g1[i1][4];  
    Double_t dx = x1-x;
    Double_t dy = y1-y;
    Double_t dz = z1-z;
    Double_t a = ppx*ppx1 + ppy*ppy1 + pz*pz1;
    Double_t b = dx*ppx1 + dy*ppy1 + dz*pz1;
    Double_t c = dx*ppx  + dy*ppy  + dz*pz ;
    Double_t pp2 = ppx*ppx + ppy*ppy + pz*pz;
    Double_t pp21= ppx1*ppx1 + ppy1*ppy1 + pz1*pz1;
    Double_t det = pp2*pp21 - a*a;
    if( TMath::Abs(det)>1.e-8 ){
      DS+=(a*b-pp21*c)/det;
      DS1+=(a*c-pp2*b)/det;
    }
  }
}



void AliKFParticleBase::TransportCBM( Double_t dS, 
				 Double_t P[], Double_t C[] ) const
{  
  //* Transport the particle on dS, output to P[],C[], for CBM field
 
  if( fQ==0 ){
    TransportLine( dS, P, C );
    return;
  }

  const Double_t kCLight = 0.000299792458;

  Double_t c = fQ*kCLight;

  // construct coefficients 

  Double_t 
    px   = fP[3],
    py   = fP[4],
    pz   = fP[5];
      
  Double_t sx=0, sy=0, sz=0, syy=0, syz=0, syyy=0, ssx=0, ssy=0, ssz=0, ssyy=0, ssyz=0, ssyyy=0;

  { // get field integrals

    Double_t fld[3][3];   
    Double_t p0[3], p1[3], p2[3];

    // line track approximation

    p0[0] = fP[0];
    p0[1] = fP[1];
    p0[2] = fP[2];
  
    p2[0] = fP[0] + px*dS;
    p2[1] = fP[1] + py*dS;
    p2[2] = fP[2] + pz*dS;
  
    p1[0] = 0.5*(p0[0]+p2[0]);
    p1[1] = 0.5*(p0[1]+p2[1]);
    p1[2] = 0.5*(p0[2]+p2[2]);

    // first order track approximation
    {
      GetFieldValue( p0, fld[0] );
      GetFieldValue( p1, fld[1] );
      GetFieldValue( p2, fld[2] );

      Double_t ssy1 = ( 7*fld[0][1] + 6*fld[1][1]-fld[2][1] )*c*dS*dS/96.;
      Double_t ssy2 = (   fld[0][1] + 2*fld[1][1]         )*c*dS*dS/6.;

      p1[0] -= ssy1*pz;
      p1[2] += ssy1*px;
      p2[0] -= ssy2*pz;
      p2[2] += ssy2*px;   
    }

    GetFieldValue( p0, fld[0] );
    GetFieldValue( p1, fld[1] );
    GetFieldValue( p2, fld[2] );
    
    sx = c*( fld[0][0] + 4*fld[1][0] + fld[2][0] )*dS/6.;
    sy = c*( fld[0][1] + 4*fld[1][1] + fld[2][1] )*dS/6.;
    sz = c*( fld[0][2] + 4*fld[1][2] + fld[2][2] )*dS/6.;

    ssx = c*( fld[0][0] + 2*fld[1][0])*dS*dS/6.;
    ssy = c*( fld[0][1] + 2*fld[1][1])*dS*dS/6.;
    ssz = c*( fld[0][2] + 2*fld[1][2])*dS*dS/6.;

    Double_t c2[3][3]    =   { {  5, -4, -1},{  44,  80,  -4},{ 11, 44, 5} }; // /=360.    
    Double_t cc2[3][3]    =   { { 38,  8, -4},{ 148, 208, -20},{  3, 36, 3} }; // /=2520.
    for(Int_t n=0; n<3; n++)
      for(Int_t m=0; m<3; m++) 
	{
	  syz += c2[n][m]*fld[n][1]*fld[m][2];
	  ssyz += cc2[n][m]*fld[n][1]*fld[m][2];
	}
 
    syz  *= c*c*dS*dS/360.;
    ssyz  *= c*c*dS*dS*dS/2520.;
    
    syy  = c*( fld[0][1] + 4*fld[1][1] + fld[2][1] )*dS;
    syyy = syy*syy*syy / 1296;
    syy  = syy*syy/72;

    ssyy = ( fld[0][1]*( 38*fld[0][1] + 156*fld[1][1]  -   fld[2][1] )+
	    fld[1][1]*(              208*fld[1][1]  +16*fld[2][1] )+
	    fld[2][1]*(                             3*fld[2][1] )  
	    )*dS*dS*dS*c*c/2520.;
    ssyyy = 
      (
       fld[0][1]*( fld[0][1]*( 85*fld[0][1] + 526*fld[1][1]  - 7*fld[2][1] )+
		 fld[1][1]*(             1376*fld[1][1]  +84*fld[2][1] )+
		 fld[2][1]*(                            19*fld[2][1] )  )+
       fld[1][1]*( fld[1][1]*(             1376*fld[1][1] +256*fld[2][1] )+
		 fld[2][1]*(                            62*fld[2][1] )  )+
       fld[2][1]*fld[2][1]  *(                             3*fld[2][1] )       
       )*dS*dS*dS*dS*c*c*c/90720.;    
 
  }

  Double_t mJ[8][8];
  for( Int_t i=0; i<8; i++ ) for( Int_t j=0; j<8; j++) mJ[i][j]=0;

  mJ[0][0]=1; mJ[0][1]=0; mJ[0][2]=0; mJ[0][3]=dS-ssyy;  mJ[0][4]=ssx;  mJ[0][5]=ssyyy-ssy;
  mJ[1][0]=0; mJ[1][1]=1; mJ[1][2]=0; mJ[1][3]=-ssz;     mJ[1][4]=dS;  mJ[1][5]=ssx+ssyz;
  mJ[2][0]=0; mJ[2][1]=0; mJ[2][2]=1; mJ[2][3]=ssy-ssyyy; mJ[2][4]=-ssx; mJ[2][5]=dS-ssyy;
  
  mJ[3][0]=0; mJ[3][1]=0; mJ[3][2]=0; mJ[3][3]=1-syy;   mJ[3][4]=sx;  mJ[3][5]=syyy-sy;
  mJ[4][0]=0; mJ[4][1]=0; mJ[4][2]=0; mJ[4][3]=-sz;     mJ[4][4]=1;   mJ[4][5]=sx+syz;
  mJ[5][0]=0; mJ[5][1]=0; mJ[5][2]=0; mJ[5][3]=sy-syyy; mJ[5][4]=-sx; mJ[5][5]=1-syy;
  mJ[6][6] = mJ[7][7] = 1;
  
  P[0] = fP[0] + mJ[0][3]*px + mJ[0][4]*py + mJ[0][5]*pz;
  P[1] = fP[1] + mJ[1][3]*px + mJ[1][4]*py + mJ[1][5]*pz;
  P[2] = fP[2] + mJ[2][3]*px + mJ[2][4]*py + mJ[2][5]*pz;
  P[3] =        mJ[3][3]*px + mJ[3][4]*py + mJ[3][5]*pz;
  P[4] =        mJ[4][3]*px + mJ[4][4]*py + mJ[4][5]*pz;
  P[5] =        mJ[5][3]*px + mJ[5][4]*py + mJ[5][5]*pz;
  P[6] = fP[6];
  P[7] = fP[7];

  MultQSQt( mJ[0], fC, C);

}


void AliKFParticleBase::TransportBz( Double_t B, Double_t S,
				     Double_t P[], Double_t C[] ) const 
{ 
  //* Transport the particle on dS, output to P[],C[], for Bz field
 
  const Double_t kCLight = 0.000299792458;
  B = B*fQ*kCLight;
  Double_t bs= B*S;
  Double_t s = TMath::Sin(bs), c = TMath::Cos(bs);
  Double_t sB, cB;
  if( TMath::Abs(bs)>1.e-10){
    sB= s/B;
    cB= (1-c)/B;
  }else{
    sB = (1. - bs*bs/6.)*S;
    cB = .5*sB*bs;
  }
  
  Double_t px = fP[3];
  Double_t py = fP[4];
  Double_t pz = fP[5];
  
  P[0] = fP[0] + sB*px + cB*py;
  P[1] = fP[1] - cB*px + sB*py;
  P[2] = fP[2] +  S*pz;
  P[3] =          c*px + s*py;
  P[4] =         -s*px + c*py;
  P[5] = fP[5];
  P[6] = fP[6];
  P[7] = fP[7];
 
  Double_t mJ[8][8] = { {1,0,0,   sB, cB,  0, 0, 0 },
			{0,1,0,  -cB, sB,  0, 0, 0 },
			{0,0,1,    0,  0,  S, 0, 0 },
			{0,0,0,    c,  s,  0, 0, 0 },
			{0,0,0,   -s,  c,  0, 0, 0 },
			{0,0,0,    0,  0,  1, 0, 0 },
			{0,0,0,    0,  0,  0, 1, 0 },
			{0,0,0,    0,  0,  0, 0, 1 }  };
  Double_t mA[8][8];
  for( Int_t k=0,i=0; i<8; i++)
    for( Int_t j=0; j<=i; j++, k++ ) mA[i][j] = mA[j][i] = fC[k]; 

  Double_t mJC[8][8];
  for( Int_t i=0; i<8; i++ )
    for( Int_t j=0; j<8; j++ ){
      mJC[i][j]=0;
      for( Int_t k=0; k<8; k++ ) mJC[i][j]+=mJ[i][k]*mA[k][j];
    }
  
  for( Int_t k=0,i=0; i<8; i++)
    for( Int_t j=0; j<=i; j++, k++ ){
      C[k] = 0;
      for( Int_t l=0; l<8; l++ ) C[k]+=mJC[i][l]*mJ[j][l];
    }
  
  return;
  /*
  Double_t cBC13 = cB*fC[13];
  Double_t C17 = fC[17];
  Double_t C18 = fC[18];
  fC[ 0]+=  2*(sB*fC[ 6] + cB*fC[10]) + (sB*fC[ 9] + 2*cBC13)*sB + cB*cB*fC[14];

  Double_t mJC13= fC[ 7] - cB*fC[ 9] + sB*fC[13];
  Double_t mJC14= fC[11] -    cBC13 + sB*fC[14];
 
  fC[ 1]+= -cB*fC[ 6] + sB*fC[10] +mJC13*sB +mJC14*cB;
  fC[ 2]+= -cB*fC[ 7] + sB*fC[11] -mJC13*cB +mJC14*sB;

  Double_t mJC23= fC[ 8] + S*fC[18];
  Double_t mJC24= fC[12] + S*fC[19];
  fC[ 3]+= S*fC[15] +mJC23*sB +mJC24*cB;
  fC[ 4]+= S*fC[16] -mJC23*cB +mJC24*sB;
 
  fC[15]+=  C18*sB + fC[19]*cB;
  fC[16]+= -C18*cB + fC[19]*sB;
  fC[17]+=  fC[20]*S;
  fC[18] =  C18*c + fC[19]*s;
  fC[19] = -C18*s + fC[19]*c;

  fC[ 5]+= (C17 + C17 + fC[17])*S;

  Double_t mJC33= c*fC[ 9] + s*fC[13]; Double_t mJC34= c*fC[13] + s*fC[14];
  Double_t mJC43=-s*fC[ 9] + c*fC[13]; Double_t mJC44=-s*fC[13] + c*fC[14];
  Double_t C6= fC[6], C7= fC[7], C8= fC[8]; 

  fC[ 6]= c*C6 + s*fC[10] +mJC33*sB +mJC34*cB;
  fC[ 7]= c*C7 + s*fC[11] -mJC33*cB +mJC34*sB;
  fC[ 8]= c*C8 + s*fC[12] +fC[18]*S;
  fC[ 9]= mJC33*c +mJC34*s;

  fC[10]= -s*C6 + c*fC[10] +mJC43*sB +mJC44*cB;
  fC[11]= -s*C7 + c*fC[11] -mJC43*cB +mJC44*sB;
  fC[12]= -s*C8 + c*fC[12] +fC[19]*S;
  fC[13]= mJC43*c +mJC44*s;
  fC[14]=-mJC43*s +mJC44*c;
  */
}


Double_t AliKFParticleBase::GetDistanceFromVertex( const AliKFParticleBase &Vtx ) const
{
  //* Calculate distance from vertex [cm]

  return GetDistanceFromVertex( Vtx.fP );
}

Double_t AliKFParticleBase::GetDistanceFromVertex( const Double_t vtx[] ) const
{
  //* Calculate distance from vertex [cm]

  Double_t mP[8], mC[36];  
  Transport( GetDStoPoint(vtx), mP, mC );
  Double_t d[3]={ vtx[0]-mP[0], vtx[1]-mP[1], vtx[2]-mP[2]};
  return TMath::Sqrt( d[0]*d[0]+d[1]*d[1]+d[2]*d[2] );
}

Double_t AliKFParticleBase::GetDistanceFromParticle( const AliKFParticleBase &p ) 
  const
{ 
  //* Calculate distance to other particle [cm]

  Double_t dS, dS1;
  GetDStoParticle( p, dS, dS1 );   
  Double_t mP[8], mC[36], mP1[8], mC1[36];
  Transport( dS, mP, mC ); 
  p.Transport( dS1, mP1, mC1 ); 
  Double_t dx = mP[0]-mP1[0]; 
  Double_t dy = mP[1]-mP1[1]; 
  Double_t dz = mP[2]-mP1[2]; 
  return TMath::Sqrt(dx*dx+dy*dy+dz*dz);
}

Double_t AliKFParticleBase::GetDeviationFromVertex( const AliKFParticleBase &Vtx ) const
{
  //* Calculate sqrt(Chi2/ndf) deviation from vertex

  return GetDeviationFromVertex( Vtx.fP, Vtx.fC );
}


Double_t AliKFParticleBase::GetDeviationFromVertex( const Double_t v[], const Double_t Cv[] ) const
{
  //* Calculate sqrt(Chi2/ndf) deviation from vertex
  //* v = [xyz], Cv=[Cxx,Cxy,Cyy,Cxz,Cyz,Czz]-covariance matrix

  Double_t mP[8];
  Double_t mC[36];
  
  Transport( GetDStoPoint(v), mP, mC );  

  Double_t d[3]={ v[0]-mP[0], v[1]-mP[1], v[2]-mP[2]};

  Double_t sigmaS = .1+10.*TMath::Sqrt( (d[0]*d[0]+d[1]*d[1]+d[2]*d[2])/
				 (mP[3]*mP[3]+mP[4]*mP[4]+mP[5]*mP[5])  );

   
  Double_t h[3] = { mP[3]*sigmaS, mP[4]*sigmaS, mP[5]*sigmaS };       
  
  Double_t mSi[6] = 
    { mC[0] +h[0]*h[0], 
      mC[1] +h[1]*h[0], mC[2] +h[1]*h[1], 
      mC[3] +h[2]*h[0], mC[4] +h[2]*h[1], mC[5] +h[2]*h[2] };

  if( Cv ){
    mSi[0]+=Cv[0];
    mSi[1]+=Cv[1];
    mSi[2]+=Cv[2];
    mSi[3]+=Cv[3];
    mSi[4]+=Cv[4];
    mSi[5]+=Cv[5];
  }
  
  Double_t mS[6];

  mS[0] = mSi[2]*mSi[5] - mSi[4]*mSi[4];
  mS[1] = mSi[3]*mSi[4] - mSi[1]*mSi[5];
  mS[2] = mSi[0]*mSi[5] - mSi[3]*mSi[3];
  mS[3] = mSi[1]*mSi[4] - mSi[2]*mSi[3];
  mS[4] = mSi[1]*mSi[3] - mSi[0]*mSi[4];
  mS[5] = mSi[0]*mSi[2] - mSi[1]*mSi[1];	 
      
  Double_t s = ( mSi[0]*mS[0] + mSi[1]*mS[1] + mSi[3]*mS[3] );
  s = ( s > 1.E-20 )  ?1./s :0;	  

  return TMath::Sqrt( TMath::Abs(s*( ( mS[0]*d[0] + mS[1]*d[1] + mS[3]*d[2])*d[0]
		   +(mS[1]*d[0] + mS[2]*d[1] + mS[4]*d[2])*d[1]
		   +(mS[3]*d[0] + mS[4]*d[1] + mS[5]*d[2])*d[2] ))/2);
}


Double_t AliKFParticleBase::GetDeviationFromParticle( const AliKFParticleBase &p ) 
  const
{ 
  //* Calculate sqrt(Chi2/ndf) deviation from other particle

  Double_t dS, dS1;
  GetDStoParticle( p, dS, dS1 );   
  Double_t mP1[8], mC1[36];
  p.Transport( dS1, mP1, mC1 ); 

  Double_t d[3]={ fP[0]-mP1[0], fP[1]-mP1[1], fP[2]-mP1[2]};

  Double_t sigmaS = .1+10.*TMath::Sqrt( (d[0]*d[0]+d[1]*d[1]+d[2]*d[2])/
					(mP1[3]*mP1[3]+mP1[4]*mP1[4]+mP1[5]*mP1[5])  );

  Double_t h[3] = { mP1[3]*sigmaS, mP1[4]*sigmaS, mP1[5]*sigmaS };       
  
  mC1[0] +=h[0]*h[0];
  mC1[1] +=h[1]*h[0]; 
  mC1[2] +=h[1]*h[1]; 
  mC1[3] +=h[2]*h[0]; 
  mC1[4] +=h[2]*h[1];
  mC1[5] +=h[2]*h[2];

  return GetDeviationFromVertex( mP1, mC1 )*TMath::Sqrt(2./1.);
}



void AliKFParticleBase::SubtractFromVertex( Double_t v[], Double_t Cv[], 
				     Double_t &vChi2, Int_t vNDF ) const 
{    
  //* Subtract the particle from the vertex  
  
  Double_t fld[3];  
  {
    GetFieldValue( v, fld );
    const Double_t kCLight =  0.000299792458;
    fld[0]*=kCLight; fld[1]*=kCLight; fld[2]*=kCLight;
  }

  Double_t m[8];
  Double_t mCm[36];
    
  Transport( GetDStoPoint(v), m, mCm );

  Double_t d[3] = { v[0]-m[0], v[1]-m[1], v[2]-m[2] };
  Double_t sigmaS = .1+10.*TMath::Sqrt( (d[0]*d[0]+d[1]*d[1]+d[2]*d[2])/
					(m[3]*m[3]+m[4]*m[4]+m[5]*m[5])  );
  
  Double_t h[6];

  h[0] = m[3]*sigmaS;
  h[1] = m[4]*sigmaS;
  h[2] = m[5]*sigmaS; 
  h[3] = ( h[1]*fld[2]-h[2]*fld[1] )*GetQ();
  h[4] = ( h[2]*fld[0]-h[0]*fld[2] )*GetQ();
  h[5] = ( h[0]*fld[1]-h[1]*fld[0] )*GetQ();
    
  //* Fit of daughter momentum (Px,Py,Pz) to fVtxGuess vertex
  {
    Double_t zeta[3] = { v[0]-m[0], v[1]-m[1], v[2]-m[2] };
    
    Double_t mVv[6] = 
      { mCm[ 0] + h[0]*h[0], 
	mCm[ 1] + h[1]*h[0], mCm[ 2] + h[1]*h[1],			     
	mCm[ 3] + h[2]*h[0], mCm[ 4] + h[2]*h[1], mCm[ 5] + h[2]*h[2] };
    
    Double_t mVvp[9]=
      { mCm[ 6] + h[0]*h[3], mCm[ 7] + h[1]*h[3], mCm[ 8] + h[2]*h[3],
	mCm[10] + h[0]*h[4], mCm[11] + h[1]*h[4], mCm[12] + h[2]*h[4],
	mCm[15] + h[0]*h[5], mCm[16] + h[1]*h[5], mCm[17] + h[2]*h[5] };
      
    Double_t mS[6] = 
      { mVv[2]*mVv[5] - mVv[4]*mVv[4],
	mVv[3]*mVv[4] - mVv[1]*mVv[5], mVv[0]*mVv[5] - mVv[3]*mVv[3],
	mVv[1]*mVv[4] - mVv[2]*mVv[3], mVv[1]*mVv[3] - mVv[0]*mVv[4], 
	mVv[0]*mVv[2] - mVv[1]*mVv[1] };		 
      
    Double_t s = ( mVv[0]*mS[0] + mVv[1]*mS[1] + mVv[3]*mS[3] );
    s = ( s > 1.E-20 )  ?1./s :0;
    
    mS[0]*=s; mS[1]*=s; mS[2]*=s; mS[3]*=s; mS[4]*=s; mS[5]*=s;
      
    Double_t mSz[3] = { (mS[0]*zeta[0]+mS[1]*zeta[1]+mS[3]*zeta[2]),
		       (mS[1]*zeta[0]+mS[2]*zeta[1]+mS[4]*zeta[2]),
		       (mS[3]*zeta[0]+mS[4]*zeta[1]+mS[5]*zeta[2]) };
    
    Double_t px = m[3] + mVvp[0]*mSz[0] + mVvp[1]*mSz[1] + mVvp[2]*mSz[2];
    Double_t py = m[4] + mVvp[3]*mSz[0] + mVvp[4]*mSz[1] + mVvp[5]*mSz[2];
    Double_t pz = m[5] + mVvp[6]*mSz[0] + mVvp[7]*mSz[1] + mVvp[8]*mSz[2];
    
    h[0] = px*sigmaS;
    h[1] = py*sigmaS;
    h[2] = pz*sigmaS; 
    h[3] = ( h[1]*fld[2]-h[2]*fld[1] )*GetQ();
    h[4] = ( h[2]*fld[0]-h[0]*fld[2] )*GetQ();
    h[5] = ( h[0]*fld[1]-h[1]*fld[0] )*GetQ();
  }
    
  Double_t mV[6];
    
  mV[ 0] = mCm[ 0] + h[0]*h[0];
  mV[ 1] = mCm[ 1] + h[1]*h[0];
  mV[ 2] = mCm[ 2] + h[1]*h[1];
  mV[ 3] = mCm[ 3] + h[2]*h[0];
  mV[ 4] = mCm[ 4] + h[2]*h[1];
  mV[ 5] = mCm[ 5] + h[2]*h[2];
     
  //* 
	    
  Double_t mS[6];
  {
    Double_t mSi[6] = { mV[0]-Cv[0], 
		       mV[1]-Cv[1], mV[2]-Cv[2], 
		       mV[3]-Cv[3], mV[4]-Cv[4], mV[5]-Cv[5] };
    
    mS[0] = mSi[2]*mSi[5] - mSi[4]*mSi[4];
    mS[1] = mSi[3]*mSi[4] - mSi[1]*mSi[5];
    mS[2] = mSi[0]*mSi[5] - mSi[3]*mSi[3];
    mS[3] = mSi[1]*mSi[4] - mSi[2]*mSi[3];
    mS[4] = mSi[1]*mSi[3] - mSi[0]*mSi[4];
    mS[5] = mSi[0]*mSi[2] - mSi[1]*mSi[1];	 
    
    Double_t s = ( mSi[0]*mS[0] + mSi[1]*mS[1] + mSi[3]*mS[3] );
    s = ( s > 1.E-20 )  ?1./s :0;	  
    mS[0]*=s;
    mS[1]*=s;
    mS[2]*=s;
    mS[3]*=s;
    mS[4]*=s;
    mS[5]*=s;
  }
    
  //* Residual (measured - estimated)
    
  Double_t zeta[3] = { m[0]-v[0], m[1]-v[1], m[2]-v[2] };
        
  //* mCHt = mCH' - D'
    
  Double_t mCHt0[3], mCHt1[3], mCHt2[3];
    
  mCHt0[0]=Cv[ 0] ;      mCHt1[0]=Cv[ 1] ;      mCHt2[0]=Cv[ 3] ;
  mCHt0[1]=Cv[ 1] ;      mCHt1[1]=Cv[ 2] ;      mCHt2[1]=Cv[ 4] ;
  mCHt0[2]=Cv[ 3] ;      mCHt1[2]=Cv[ 4] ;      mCHt2[2]=Cv[ 5] ;
  
  //* Kalman gain K = mCH'*S
    
  Double_t k0[3], k1[3], k2[3];
    
  for(Int_t i=0;i<3;++i){
    k0[i] = mCHt0[i]*mS[0] + mCHt1[i]*mS[1] + mCHt2[i]*mS[3];
    k1[i] = mCHt0[i]*mS[1] + mCHt1[i]*mS[2] + mCHt2[i]*mS[4];
    k2[i] = mCHt0[i]*mS[3] + mCHt1[i]*mS[4] + mCHt2[i]*mS[5];
  }
    
  //* New estimation of the vertex position r += K*zeta
    
  for(Int_t i=0;i<3;++i) 
    v[i] -= k0[i]*zeta[0] + k1[i]*zeta[1] + k2[i]*zeta[2];
    
  //* New covariance matrix C -= K*(mCH')'
    
  for(Int_t i=0, k=0;i<3;++i){
    for(Int_t j=0;j<=i;++j,++k) 
      Cv[k] += k0[i]*mCHt0[j] + k1[i]*mCHt1[j] + k2[i]*mCHt2[j];
  }
    
  //* Calculate Chi^2 
  
  vNDF  -= 2;
  vChi2 -= (mS[0]*zeta[0] + mS[1]*zeta[1] + mS[3]*zeta[2])*zeta[0]
    +      (mS[1]*zeta[0] + mS[2]*zeta[1] + mS[4]*zeta[2])*zeta[1]
    +      (mS[3]*zeta[0] + mS[4]*zeta[1] + mS[5]*zeta[2])*zeta[2];  
}



void AliKFParticleBase::TransportLine( Double_t dS, 
				       Double_t P[], Double_t C[] ) const 
{
  //* Transport the particle as a straight line

  P[0] = fP[0] + dS*fP[3];
  P[1] = fP[1] + dS*fP[4];
  P[2] = fP[2] + dS*fP[5];
  P[3] = fP[3];
  P[4] = fP[4];
  P[5] = fP[5];
  P[6] = fP[6];
  P[7] = fP[7];
 
  Double_t c6  = fC[ 6] + dS*fC[ 9];
  Double_t c11 = fC[11] + dS*fC[14];
  Double_t c17 = fC[17] + dS*fC[20];
  Double_t sc13 = dS*fC[13];
  Double_t sc18 = dS*fC[18];
  Double_t sc19 = dS*fC[19];

  C[ 0] = fC[ 0] + dS*( fC[ 6] + c6  );
  C[ 2] = fC[ 2] + dS*( fC[11] + c11 );
  C[ 5] = fC[ 5] + dS*( fC[17] + c17 );

  C[ 7] = fC[ 7] + sc13;
  C[ 8] = fC[ 8] + sc18;
  C[ 9] = fC[ 9];

  C[12] = fC[12] + sc19;

  C[ 1] = fC[ 1] + dS*( fC[10] + C[ 7] );
  C[ 3] = fC[ 3] + dS*( fC[15] + C[ 8] );
  C[ 4] = fC[ 4] + dS*( fC[16] + C[12] ); 
  C[ 6] = c6;

  C[10] = fC[10] + sc13;
  C[11] = c11;

  C[13] = fC[13];
  C[14] = fC[14];
  C[15] = fC[15] + sc18;
  C[16] = fC[16] + sc19;
  C[17] = c17;
  
  C[18] = fC[18];
  C[19] = fC[19];
  C[20] = fC[20];
  C[21] = fC[21] + dS*fC[24];
  C[22] = fC[22] + dS*fC[25];
  C[23] = fC[23] + dS*fC[26];

  C[24] = fC[24];
  C[25] = fC[25];
  C[26] = fC[26];
  C[27] = fC[27];
  C[28] = fC[28] + dS*fC[31];
  C[29] = fC[29] + dS*fC[32];
  C[30] = fC[30] + dS*fC[33];

  C[31] = fC[31];
  C[32] = fC[32];
  C[33] = fC[33];
  C[34] = fC[34];
  C[35] = fC[35]; 
}


void AliKFParticleBase::MultQSQt( const Double_t Q[], const Double_t S[], Double_t SOut[] )
{
  //* Matrix multiplication Q*S*Q^T, Q - square matrix, S - symmetric

  const Int_t kN= 8;
  Double_t mA[kN*kN];
  
  for( Int_t i=0, ij=0; i<kN; i++ ){
    for( Int_t j=0; j<kN; j++, ++ij ){
      mA[ij] = 0 ;
      for( Int_t k=0; k<kN; ++k ) mA[ij]+= S[( k<=i ) ? i*(i+1)/2+k :k*(k+1)/2+i] * Q[ j*kN+k];
    }
  }
    
  for( Int_t i=0; i<kN; i++ ){
    for( Int_t j=0; j<=i; j++ ){
      Int_t ij = ( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i;
      SOut[ij] = 0 ;
      for( Int_t k=0; k<kN; k++ )  SOut[ij] += Q[ i*kN+k ] * mA[ k*kN+j ];
    }
  }
}


// 72-charachters line to define the printer border
//3456789012345678901234567890123456789012345678901234567890123456789012
