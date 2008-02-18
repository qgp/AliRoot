/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

//-----------------------------------------------------------------------------
/// \class AliMUONGlobalTriggerBoard
/// Global trigger implementation:
/// - inputs are regional responses
/// - output is a 12-bit word
/// - 4 bits per trigger level
///
/// \author Rachid Guernane (LPCCFd), 
/// Corrected by Christian Finck (Subatech)
//-----------------------------------------------------------------------------

#include "AliMUONGlobalTriggerBoard.h"
#include "AliLog.h"
#include "TBits.h"

#include <Riostream.h>

/// \cond CLASSIMP
ClassImp(AliMUONGlobalTriggerBoard)
/// \endcond

//___________________________________________
AliMUONGlobalTriggerBoard::AliMUONGlobalTriggerBoard(): AliMUONTriggerBoard()
{
/// Default constructor

   for (Int_t i=0;i<16;i++) fRegionalResponse[i] = 0;
}

//___________________________________________
AliMUONGlobalTriggerBoard::AliMUONGlobalTriggerBoard(const char *name, Int_t a) : AliMUONTriggerBoard(name, a)
{
/// Standard constructor

   for (Int_t i=0;i<16;i++) fRegionalResponse[i] = 0;
}

//___________________________________________
AliMUONGlobalTriggerBoard::~AliMUONGlobalTriggerBoard()
{
/// Destructor
}

//___________________________________________
void AliMUONGlobalTriggerBoard::Mask(Int_t index, UShort_t mask)
{
  /// mask global trigger board input index with value mask
  if ( index>=0 && index < 2 ) 
  {
    fMask[index]=mask;
  }
  else
  {
    AliError(Form("Index %d out of bounds (max %d)",index,2));
  }  
}

//___________________________________________
void AliMUONGlobalTriggerBoard::Response()
{
   /// compute the global trigger board
   /// response according to the algo() method
// output from global trigger algorithm
// [+, -, US, LS] * [Hpt, Lpt]
// transformed to [usHpt, usLpt, lsHpt, lsLpt, sHpt, sLpt] according
// to Global Trigger Unit user manual

   Int_t t[16];

   for (Int_t i = 0; i < 16; ++i) 
   {
     Int_t index = i/8;
     Int_t shift = i % 8;
     UShort_t enable = !((fMask[index] >> shift) & 0x1);
     if (enable)
      t[i] = fRegionalResponse[i];
     else
       t[i] = 0;
   }
   
   
   Int_t rank = 8;

   for (Int_t i=0;i<4;i++)
   {
      Int_t ip = 0;
      
      for (Int_t j=0;j<rank;j++)
      {
         UShort_t lthres = Algo(t[2*j],t[2*j+1],"LPT");

         UShort_t hthres = Algo(t[2*j],t[2*j+1],"HPT"); hthres <<= 4;

         t[ip] = lthres | hthres;

         ip++;
      }
      
      rank /= 2; 
   }
   UChar_t sLpt, sHpt, lsLpt, lsHpt, usLpt, usHpt;
   sLpt  = ((t[0] & 0xC)  != 0);
   sHpt  = ((t[0] & 0xC0) != 0);
   //lsLpt = ((t[0] & 0x2)  != 0);
   lsLpt = ((t[0] & 0x1)  != 0);
   //lsHpt = ((t[0] & 0x20) != 0);
   lsHpt = ((t[0] & 0x10) != 0);
   //usLpt = ((t[0] & 0x1 ) != 0);
   usLpt = ((t[0] & 0x2 ) != 0);
   //usHpt = ((t[0] & 0x10) != 0);
   usHpt = ((t[0] & 0x20) != 0);

   sHpt  <<= 1;
   lsLpt <<= 2;
   lsHpt <<= 3;
   usLpt <<= 4;
   usHpt <<= 5;

   fResponse = sLpt | sHpt | lsLpt | lsHpt | usLpt |usHpt;
   

}

//___________________________________________
UShort_t AliMUONGlobalTriggerBoard::Algo(UShort_t i, UShort_t j, char *thres)
{
/// global trigger algorithm
///   a ,b = reg  response  =  Hpt (+|-|us|ls) |  Lpt (+|-|us|ls)  
                           
   TBits a(8), b(8); a.Set(8,&i); b.Set(8,&j);

   TBits trg1(2), trg2(2), trg(2);

   if (!strcmp(thres,"LPT"))
   {
      trg1[0] = a[2]; trg1[1] = a[3]; 
      trg2[0] = b[2]; trg2[1] = b[3];
   }
   else
   {
      trg1[0] = a[6]; trg1[1] = a[7]; 
      trg2[0] = b[6]; trg2[1] = b[7];         
   }
       
   TBits trgLS1(1), trgUS1(1), trgLS2(1), trgUS2(1), trgLS(1), trgUS(1);

   if (!strcmp(thres,"LPT"))
   {
      //trgLS1[0] = a[1]; trgUS1[0] = a[0]; 
      //trgLS2[0] = b[1]; trgUS2[0] = b[0];
      trgLS1[0] = a[0]; trgUS1[0] = a[1]; 
      trgLS2[0] = b[0]; trgUS2[0] = b[1];
   }
   else
   {
      //trgLS1[0] = a[5]; trgUS1[0] = a[4]; 
      //trgLS2[0] = b[5]; trgUS2[0] = b[4];         
      trgLS1[0] = a[4]; trgUS1[0] = a[5]; 
      trgLS2[0] = b[4]; trgUS2[0] = b[5];         
   }

   trgLS[0] = ( trg1[0] & trg2[0] ) | ( trg1[1] & trg2[1] ) | trgLS1[0] | trgLS2[0];
   trgUS[0] = ( trg1[0] & trg2[1] ) | ( trg1[1] & trg2[0] ) | trgUS1[0] | trgUS2[0];
   
   trg[0] = trg1[0] | trg2[0];
   trg[1] = trg1[1] | trg2[1];
   
   TBits v(4);
   
   //v[0] = trgUS[0];
   //v[1] = trgLS[0];
   v[0] = trgLS[0];
   v[1] = trgUS[0];
   v[2] = trg[0];
   v[3] = trg[1];

   UShort_t rv = 0;
   v.Get(&rv);
   
   return rv;
}

//___________________________________________
void AliMUONGlobalTriggerBoard::Scan(Option_t*) const
{
  /// print global trigger output 
  TBits w(8); w.Set(8,&fResponse);

// TRG[1:0]
// 00 noth
// 01 negative track
// 10 positive track
// 11 undef

   Int_t iSP[2] = {0,0}, iSM[2] = {0,0}, iSU[2] = {0,0};

   TBits a(2), n(2), p(2), u(2);
   
   UShort_t val;

   val = 1; n.Set(2,&val);
   val = 2; p.Set(2,&val);
   val = 3; u.Set(2,&val);
   
   a[0] = w[2];
   a[1] = w[3];
   
   if      (a==p) iSP[0] = 1;
   else if (a==n) iSM[0] = 1;
   else if (a==u) iSU[0] = 1;   

   a[0] = w[6];
   a[1] = w[7];

   if      (a==p) iSP[1] = 1;
   else if (a==n) iSM[1] = 1;
   else if (a==u) iSU[1] = 1;
   
   Int_t iPU[2] = {w[0],w[4]};
   Int_t iPL[2] = {w[1],w[5]};

   printf("============================================\n");
   printf(" Global Trigger output       Low pt  High pt\n");
   printf(" number of Single Plus      :\t");
   for (Int_t i=0; i<2; i++) printf("%i\t",iSP[i]);
   printf("\n");
   printf(" number of Single Minus     :\t");
   for (Int_t i=0; i<2; i++) printf("%i\t",iSM[i]);
   printf("\n");
   printf(" number of Single Undefined :\t"); 
   for (Int_t i=0; i<2; i++) printf("%i\t",iSU[i]);
   printf("\n");
   printf(" number of UnlikeSign pair  :\t"); 
   for (Int_t i=0; i<2; i++) printf("%i\t",iPU[i]);
   printf("\n");
   printf(" number of LikeSign pair    :\t");  
   for (Int_t i=0; i<2; i++) printf("%i\t",iPL[i]);
   printf("\n");
   printf("===================================================\n");
   printf("\n");
}

