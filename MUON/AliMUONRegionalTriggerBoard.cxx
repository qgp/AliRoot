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

//*-- Author: Rachid Guernane (LPCCFd)

#include "AliMUONRegionalTriggerBoard.h"

#include "AliLog.h"

#include "TBits.h"

#include <Riostream.h>

ClassImp(AliMUONRegionalTriggerBoard)

//___________________________________________
AliMUONRegionalTriggerBoard::AliMUONRegionalTriggerBoard()
{
   for (Int_t i=0; i<16; i++) fLocalResponse[i] = 0;
}

//___________________________________________
AliMUONRegionalTriggerBoard::AliMUONRegionalTriggerBoard(const char *name, Int_t a) : AliMUONTriggerBoard(name, a)
{
   for (Int_t i=0; i<16; i++) fLocalResponse[i] = 0;
}

//___________________________________________
void AliMUONRegionalTriggerBoard::Response()
{
   Int_t t[16];

   for (Int_t i=0;i<16;i++) t[i] = fLocalResponse[i] & fMask[i];

   Int_t rank = 8;

   for (Int_t i=0;i<4;i++)
   {
      Int_t ip = 0;
      
      for (Int_t j=0;j<rank;j++)
      {
         UShort_t athres = Algo(t[2*j],t[2*j+1],"APT",i);

         UShort_t lthres = Algo(t[2*j],t[2*j+1],"LPT",i); lthres <<= 4;

         UShort_t hthres = Algo(t[2*j],t[2*j+1],"HPT",i); hthres <<= 8;

         t[ip] = athres | lthres | hthres;

         ip++;
      }
      
      rank /= 2; 
   }

   fResponse = t[0]; // 12-bit [H4:L4:A4]
}

//___________________________________________
UShort_t AliMUONRegionalTriggerBoard::Algo(UShort_t i, UShort_t j, char *thres, Int_t level)
{
   TBits a(12), b(12); a.Set(12,&i); b.Set(12,&j);

   TBits trg1(2), trg2(2), trg(2);

   if (!strcmp(thres,"APT"))
   {
      if (!level)
      {         
         trg1[0] = a[0]; trg1[1] = a[1]; 
         trg2[0] = b[0]; trg2[1] = b[1];
      }
      else
      {
         trg1[0] = a[2]; trg1[1] = a[3]; 
         trg2[0] = b[2]; trg2[1] = b[3];
      }
   }
   else if (!strcmp(thres,"LPT"))
   {
      if (!level)
      {         
         trg1[0] = a[2]; trg1[1] = a[3]; 
         trg2[0] = b[2]; trg2[1] = b[3];
      }
      else
      {
         trg1[0] = a[6]; trg1[1] = a[7]; 
         trg2[0] = b[6]; trg2[1] = b[7];
      }
   }
   else
   {
      if (!level)
      {         
         trg1[0] = a[4]; trg1[1] = a[5]; 
         trg2[0] = b[4]; trg2[1] = b[5];
      }
      else
      {
         trg1[0] = a[10]; trg1[1] = a[11]; 
         trg2[0] = b[10]; trg2[1] = b[11];         
      }
   }
       
   TBits trgLS1(1), trgUS1(1), trgLS2(1), trgUS2(1), trgLS(1), trgUS(1);

   if (!level) 
   {
      trgLS1[0] = trgUS1[0] = trgLS2[0] = trgUS2[0] = 0;
   }
   else
   {
      if (!strcmp(thres,"APT"))
      {
         trgLS1[0] = a[1]; trgUS1[0] = a[0]; 
         trgLS2[0] = b[1]; trgUS2[0] = b[0];
      }
      else if (!strcmp(thres,"LPT"))
      {
         trgLS1[0] = a[5]; trgUS1[0] = a[4]; 
         trgLS2[0] = b[5]; trgUS2[0] = b[4];
      }
      else
      {
         trgLS1[0] = a[9]; trgUS1[0] = a[8]; 
         trgLS2[0] = b[9]; trgUS2[0] = b[8];         
      }
   }

   trgLS[0] = ( trg1[0] & trg2[0] ) | ( trg1[1] & trg2[1] ) | trgLS1[0] | trgLS2[0];
   trgUS[0] = ( trg1[0] & trg2[1] ) | ( trg1[1] & trg2[0] ) | trgUS1[0] | trgUS2[0];
   
   trg[0] = trg1[0] | trg2[0];
   trg[1] = trg1[1] | trg2[1];
   
   TBits v(4);
   
   v[0] = trgUS[0];
   v[1] = trgLS[0];
   v[2] = trg[0];
   v[3] = trg[1];
   
   UShort_t rv = 0;
   v.Get(&rv);

   return rv;
}
//___________________________________________
void AliMUONRegionalTriggerBoard::Scan(Option_t*)
{
   for (Int_t i=0; i<16; i++) 
   {
      TBits b;
      b.Set(6,&fLocalResponse[i]);
      
      cout << "Entry " << i << " is " << b << endl;
      
   }
   
}

//___________________________________________
void AliMUONRegionalTriggerBoard::Mask(Int_t index, UShort_t mask)
{
  if ( index>=0 && index < 16 ) 
  {
    fMask[index]=mask;
  }
  else
  {
    AliError(Form("Index %d out of bounds (max %d)",index,16));
  }
}

ClassImp(AliMUONRegionalTriggerBoard)
