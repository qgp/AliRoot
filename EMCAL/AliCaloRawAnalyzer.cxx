/**************************************************************************
 * This file is property of and copyright by                              *
 * the Relativistic Heavy Ion Group (RHIG), Yale University, US, 2009     *
 *                                                                        *
 * Primary Author: Per Thomas Hille <perthomas.hille@yale.edu>            *
 *                                                                        *
 * Contributors are mentioned in the code where appropriate.              *
 * Please report bugs to p.t.hille@fys.uio.no                             *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/


// Base class for extraction 
// of signal amplitude and peak position
// From CALO Calorimeter RAW data (from the RCU)
// Contains some utilities for preparing / selecting
// Signals suitable for signal extraction
// By derived classes

#include "AliLog.h"
#include "AliCaloRawAnalyzer.h"
#include "AliCaloBunchInfo.h"
#include "AliCaloFitResults.h"
#include "TMath.h"
#include <iostream>
using namespace std;

ClassImp(AliCaloRawAnalyzer)  

AliCaloRawAnalyzer::AliCaloRawAnalyzer(const char *name, const char *nameshort) :  TObject(),
										   fMinTimeIndex(-1),
										   fMaxTimeIndex(-1),
										   fFitArrayCut(5),
										   fAmpCut(4),
										   fNsampleCut(5),
										   fNsamplePed(3),
										   fIsZerosupressed( false ),
										   fVerbose( false )
{
  //Comment 
  sprintf(fName, "%s", name);
  sprintf(fNameShort, "%s", nameshort);
    
  for(int i=0; i < MAXSAMPLES; i++ )
    {
      fReversed[i] = 0;
    }
}

AliCaloRawAnalyzer::~AliCaloRawAnalyzer()
{

}


void 
AliCaloRawAnalyzer::SetTimeConstraint(const int min, const int max ) 
{
  //Require that the bin if the maximum ADC value is between min and max (timebin)
  if(  ( min > max ) || min > MAXSAMPLES  || max > MAXSAMPLES  )
    {
      AliWarning( Form( "Attempt to set Invalid time bin range (Min , Max) = (%d, %d), Ingored",  min, max ) ); 
    }
  else
    {
      fMinTimeIndex  = min;
      fMaxTimeIndex  = max;
    }
}


UShort_t 
AliCaloRawAnalyzer::Max(const UShort_t *data, const int length ) const
{
  //------------
  UShort_t tmpmax  = data[0];

  for(int i=0; i < length; i++)
    {
      if( tmpmax  <  data[i] )
	{
	  tmpmax = data[i];
	}
    }
  return tmpmax;
}


void 
AliCaloRawAnalyzer::SelectSubarray( const Double_t *fData, const int length, const short maxindex, int *const first,  int *const last ) const
{
  //Selection of subset of data from one bunch that will be used for fitting or
  //Peak finding.  Go to the left and right of index of the maximum time bin
  //Until the ADC value is less that fFitArrayCut, or derivative changes sign (data jump)
  int tmpfirst =  maxindex;
  int tmplast  =  maxindex;
  Double_t valFirst =  fData[maxindex];
  Double_t valLast  =  fData[maxindex];
  
  while( (tmpfirst >= 0) && (fData[tmpfirst] >= fFitArrayCut) &&
	 (fData[tmpfirst]<valFirst || tmpfirst==maxindex) )  
    {
      valFirst = fData[tmpfirst];
      tmpfirst -- ;
    }
  
  while( (tmplast < length) && (fData[tmplast] >= fFitArrayCut) &&
	 (fData[tmplast]<valLast || tmplast==maxindex) )  
    {
      valLast = fData[tmplast];
      tmplast ++;
    }

  *first = tmpfirst +1;
  *last =  tmplast -1;
  return;
}



Float_t 
AliCaloRawAnalyzer::ReverseAndSubtractPed( const AliCaloBunchInfo *bunch, const UInt_t /*altrocfg1*/,  const UInt_t /*altrocfg2*/, double *outarray ) const
{
  //Time sample comes in reversed order, revers them back
  //Subtract the baseline based on content of altrocfg1 and altrocfg2.
  Int_t length =  bunch->GetLength(); 
  const UShort_t *sig = bunch->GetData();

  double ped = EvaluatePedestal( sig , length);

  for( int i=0; i < length; i++ )
    {
      outarray[i] = sig[length -i -1] - ped;
    }

  return ped;
}



Float_t 
AliCaloRawAnalyzer::EvaluatePedestal(const UShort_t * const data, const int /*length*/ ) const
{
  //  double ped = 0;
  double tmp = 0;

  if( fIsZerosupressed == false ) 
    {
      for(int i=0; i < fNsamplePed; i++ )
	{
	  tmp += data[i];
	}
   }

  return  tmp/fNsamplePed;
 }


short  
AliCaloRawAnalyzer::Max( const AliCaloBunchInfo *const bunch , int *const maxindex ) const
{
  //comment
  short tmpmax = -1;
  int tmpindex = -1;
  const UShort_t *sig = bunch->GetData();

  for(int i=0; i < bunch->GetLength(); i++ )
    {
      if( sig[i] > tmpmax )
	{
	  tmpmax   =  sig[i];
	  tmpindex =  i; 
	}
    }
  
  if(maxindex != 0 )
    {
      //   *maxindex =  bunch->GetLength() -1 - tmpindex + bunch->GetStartBin(); 
       *maxindex =  bunch->GetLength() -1 - tmpindex + bunch->GetStartBin(); 
    }
  
  return  tmpmax;
}


bool  
AliCaloRawAnalyzer::CheckBunchEdgesForMax( const AliCaloBunchInfo *const bunch ) const
{
  // a bunch is considered invalid if the maximum is in the first or last time-bin
  short tmpmax = -1;
  int tmpindex = -1;
  const UShort_t *sig = bunch->GetData();

  for(int i=0; i < bunch->GetLength(); i++ )
    {
      if( sig[i] > tmpmax )
	{
	  tmpmax   =  sig[i];
	  tmpindex =  i; 
	}
    }
  
  bool bunchOK = true;
  if (tmpindex == 0 || tmpindex == (bunch->GetLength()-1) )
    {
      bunchOK = false;
    }
  
  return  bunchOK;
}


int 
AliCaloRawAnalyzer::SelectBunch( const vector<AliCaloBunchInfo> &bunchvector,short *const maxampbin, short *const maxamplitude ) const
{
  //We select the bunch with the highest amplitude unless any time constraints is set
  short max = -1;
  short bunchindex = -1;
  short maxall = -1;
  int indx = -1;

  for(unsigned int i=0; i < bunchvector.size(); i++ )
    {
      max = Max(  &bunchvector.at(i), &indx ); // CRAP PTH, bug fix, trouble if more than one bunches  
      if( IsInTimeRange( indx) )
	{
	  if( max > maxall )
	    {
	      maxall = max;
	      bunchindex = i;
	      *maxampbin     = indx;
	      *maxamplitude  = max;
	    }
	}
    }
 
  if (bunchindex >= 0) {
    bool bunchOK = CheckBunchEdgesForMax( &bunchvector.at(bunchindex) );
    if (! bunchOK) { 
      bunchindex = -1; 
    }
  }

  return  bunchindex;
}


bool 
AliCaloRawAnalyzer::IsInTimeRange( const int maxindex  ) const
{
  // Ckeck if the index of the max ADC vaue is consistent with trigger.
  if( ( fMinTimeIndex  < 0 && fMaxTimeIndex  < 0) ||fMaxTimeIndex  < 0 )
    {
      return true; 
    }
  return ( maxindex < fMaxTimeIndex ) && ( maxindex > fMinTimeIndex  ) ? true : false;
}


void 
AliCaloRawAnalyzer::PrintBunches( const vector<AliCaloBunchInfo> &bvctr ) const
{
  //comment
  cout << __FILE__ << __LINE__<< "*************** Printing Bunches *******************" << endl;
  cout << __FILE__ << __LINE__<< "*** There are " << bvctr.size() << ", bunches" << endl;

  for(unsigned int i=0; i < bvctr.size() ; i++ )
    {
      PrintBunch( bvctr.at(i) );
      cout << " bunch = "  <<  i  << endl;
    }
  cout << __FILE__ << __LINE__<< "*************** Done ... *******************" << endl;
}


void 
AliCaloRawAnalyzer::PrintBunch( const AliCaloBunchInfo &bunch ) const
{
  //comment
  cout << __FILE__ << ":" << __LINE__ << endl;
  cout << __FILE__ << __LINE__   << ", startimebin = " << bunch.GetStartBin() << ", length =" <<  bunch.GetLength() << endl;
  const UShort_t *sig =  bunch.GetData();  
  
  for ( Int_t j = 0; j <  bunch.GetLength();  j++) 
    {
      printf("%d\t", sig[j] );
    }
  cout << endl; 
}


AliCaloFitResults
AliCaloRawAnalyzer::Evaluate( const vector<AliCaloBunchInfo>  &/*bunchvector*/, const UInt_t /*altrocfg1*/,  const UInt_t /*altrocfg2*/)
{ // method to do the selection of what should possibly be fitted
  // not implemented for base class
  return AliCaloFitResults( 0, 0 );
}


int
AliCaloRawAnalyzer::PreFitEvaluateSamples( const vector<AliCaloBunchInfo>  &bunchvector, const UInt_t altrocfg1,  const UInt_t altrocfg2, Int_t & index, Float_t & maxf, short & maxamp, short & maxrev, Float_t & ped, int & first, int & last)
{ // method to do the selection of what should possibly be fitted
  int nsamples  = 0;
  short maxampindex = 0;
  index = SelectBunch( bunchvector,  &maxampindex,  &maxamp ); // select the bunch with the highest amplitude unless any time constraints is set

  
  if( index >= 0 && maxamp > fAmpCut) // something valid was found, and non-zero amplitude
    {
      // use more convenient numbering and possibly subtract pedestal
      ped  = ReverseAndSubtractPed( &(bunchvector.at(index)),  altrocfg1, altrocfg2, fReversed  );
      maxf = TMath::MaxElement( bunchvector.at(index).GetLength(),  fReversed );
      
      if ( maxf > fAmpCut ) // possibly significant signal
	{
	  // select array around max to possibly be used in fit
	  maxrev = maxampindex - bunchvector.at(index).GetStartBin(); 
	  SelectSubarray( fReversed,  bunchvector.at(index).GetLength(),  maxrev, &first, &last);

	  // sanity check: maximum should not be in first or last bin
	  // if we should do a fit
	  if (first!=maxrev && last!=maxrev) {
	    // calculate how many samples we have 
	    nsamples =  last - first + 1;	  
	  }
	}
    }

  return nsamples;
}

