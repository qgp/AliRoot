#include "AliITSPid.h"
#include "TMath.h"
#include "AliITSIOTrack.h"
#include <Riostream.h>
ClassImp(AliITSPid)
//
Float_t AliITSPid::qtrm(Int_t track)
{
    TVector q(*( this->GetVec(track)  ));
    Int_t ml=(Int_t)q(0);
    if(ml<1)return 0.;
    if(ml>6)ml=6;
    float vf[6];
    Int_t nl=0; for(Int_t i=0;i<ml;i++){if(q(i)>fSigmin){vf[nl]=q(i+1);nl++;}}
    if(nl==0)return 0.;
    switch(nl){
      case  1:q(6)=q(1); break;
      case  2:q(6)=(q(1)+q(2))/2.; break;
      default:
	for(int fi=0;fi<2;fi++){
	  Int_t swap;
	  do{ swap=0; float qmin=vf[fi];
	  for(int j=fi+1;j<nl;j++)
	    if(qmin>vf[j]){qmin=vf[j];vf[j]=vf[fi];vf[fi]=qmin;swap=1;};
	  }while(swap==1);
	}
        q(6)= (vf[0]+vf[1])/2.;
        break;
    }
    for(Int_t i=0;i<nl;i++){q(i+1)=vf[i];} this->SetVec(track,q);
    return (q(6));
}

Float_t AliITSPid::qtrm(Float_t qarr[6],Int_t narr)
{
  Float_t q[6],qm,qmin;
  Int_t nl,ml;
  if(narr>0&&narr<7){ml=narr;}else{return 0;};
  nl=0; for(Int_t i=0;i<ml;i++){if(qarr[i]>fSigmin){q[nl]=qarr[i];nl++;}}
  if(nl==0)return 0.;
    switch(nl){
      case  1:qm=q[0]; break;
      case  2:qm=(q[0]+q[1])/2.; break;
      default:
	Int_t swap;
	for(int fi=0;fi<2;fi++){
	  do{ swap=0; qmin=q[fi];
	  for(int j=fi+1;j<nl;j++)
	    if(qmin>q[j]){qmin=q[j];q[j]=q[fi];q[fi]=qmin;swap=1;};
	  }while(swap==1);
	}
        qm= (q[0]+q[1])/2.;
        break;
    }
    return qm;
}

Int_t	AliITSPid::wpik(Float_t pm,Float_t q)
{
  Double_t par[6];
  for(int i=0;i<6;i++){par[i]=fGGpi[i]->Eval(pm);}
  fggpi->SetParameters(par);

  for(int i=0;i<3;i++){par[i]=fGGka[i]->Eval(pm);}
  fggka->SetParameters(par);

  Float_t ppi=fggpi->Eval(q);
  Float_t pka=fggka->Eval(q);
  Float_t p=ppi+pka;
  /*
  if(!fSilent){
    fggka->Print();
    fggpi->Print();
    if(p>0)cout<<" ppi,pka="<<ppi/p<<"  "<<pka/p<<endl;
  }
  */

  if(p>0){
    ppi=ppi/p; 
    pka=pka/p;
    fWp=0.; fWpi=ppi; fWk=pka;
    if( pka>ppi){return fPcode=321;}else{return fPcode=211;}
  }else{return 0;}
}
//-----------------------------------------------------------
Int_t	AliITSPid::wpikp(Float_t pm,Float_t q)
{
  Double_t par[6];
  for(int i=0;i<6;i++){par[i]=fGGpi[i]->Eval(pm);}
  fggpi->SetParameters(par);

  for(int i=0;i<3;i++){par[i]=fGGka[i]->Eval(pm);}
  fggka->SetParameters(par);

  for(int i=0;i<3;i++){par[i]=fGGpr[i]->Eval(pm);}
  fggpr->SetParameters(par);

  Float_t p,ppi,pka,ppr;
  if( q>(fggpr->GetParameter(1)+fggpr->GetParameter(2)) )
      { p=1.0; ppr=1.0; ppi=pka=0.0;
    }else{ 
    ppi=fggpi->Eval(q);
    pka=fggka->Eval(q);
    ppr=fggpr->Eval(q);
    p=ppi+pka+ppr;
  }
  if(p>0){
    ppi=ppi/p; 
    pka=pka/p;
    ppr=ppr/p;
    fWp=ppr; fWpi=ppi; fWk=pka;
    //if(!fSilent)cout<<" ppi,pka,ppr="<<ppi<<"  "<<pka<<" "<<ppr<<endl;

   if( ppi>pka&&ppi>ppr )
           {return fPcode=211;}
   else{ if(pka>ppr){return fPcode=321;}else{return fPcode=2212;}
   }

  }else{return 0;}
}
//-----------------------------------------------------------
Int_t	AliITSPid::GetPcode(TClonesArray* rps,Float_t pm)
{
    return 0;    
}
//-----------------------------------------------------------
Int_t   AliITSPid::GetPcode(AliTPCtrack *track)
{
      Double_t xk,par[5]; track->GetExternalParameters(xk,par);
      Float_t phi=TMath::ASin(par[2]) + track->GetAlpha();
      if (phi<-TMath::Pi()) phi+=2*TMath::Pi();
      if (phi>=TMath::Pi()) phi-=2*TMath::Pi();
      Float_t lam=TMath::ATan(par[3]); 
      Float_t pt_1=TMath::Abs(par[4]);
      Float_t mom=1./(pt_1*TMath::Cos(lam));
      Float_t dedx=track->GetdEdx();
    Int_t pcode=GetPcode(dedx/40.,mom);
//    cout<<"TPCtrack dedx,mom,pcode="<<dedx<<","<<mom<<","<<pcode<<endl;
    return pcode?pcode:211;
    }
//------------------------------------------------------------
Int_t   AliITSPid::GetPcode(AliITSIOTrack *track)
{
    Double_t px,py,pz;
    px=track->GetPx();
    py=track->GetPy();
    pz=track->GetPz();
    Float_t mom=TMath::Sqrt(px*px+py*py+pz*pz);
//???????????????????
    // Float_t dedx=1.0;
    Float_t dedx=track->GetdEdx();
//???????????????????    
    Int_t pcode=GetPcode(dedx,mom);
//    cout<<"ITSV1 dedx,mom,pcode="<<dedx<<","<<mom<<","<<pcode<<endl;
return pcode?pcode:211;
}
//-----------------------------------------------------------
Int_t   AliITSPid::GetPcode(AliITStrackV2 *track)
{
  if(track==0)return 0;
  //      track->Propagate(track->GetAlpha(),3.,0.1/65.19*1.848,0.1*1.848);
      track->PropagateTo(3.,0.0028,65.19);
      track->PropagateToVertex();
    Double_t xk,par[5]; track->GetExternalParameters(xk,par);
    Float_t lam=TMath::ATan(par[3]);
    Float_t pt_1=TMath::Abs(par[4]);
    Float_t mom=0.;
    if( (pt_1*TMath::Cos(lam))!=0. ){ mom=1./(pt_1*TMath::Cos(lam)); }else{mom=0.;};
    Float_t dedx=track->GetdEdx();
//    cout<<"lam,pt_1,mom,dedx="<<lam<<","<<pt_1<<","<<mom<<","<<dedx<<endl;
    Int_t pcode=GetPcode(dedx,mom);
//    cout<<"ITS V2 dedx,mom,pcode="<<dedx<<","<<mom<<","<<pcode<<endl;
return pcode?pcode:211;
}
//-----------------------------------------------------------
Int_t	AliITSPid::GetPcode(Float_t q,Float_t pm)
{
    fWpi=fWk=fWp=0.;     fPcode=0;

    if ( pm<=0.400 )
	{ if( q<fCutKa->Eval(pm) )
	    {return pion();}
	else{ if( q<fCutPr->Eval(pm) )
		{return kaon();}
	    else{return proton();}
	    } 
	}
    if ( pm<=0.750 )
	if ( q>fCutPr->Eval(pm)  )
	    {return proton();} else {return wpik(pm,q);};
    if( pm<=1.10 ){ return wpikp(pm,q); }
    return fPcode;    
}
//-----------------------------------------------------------
void	AliITSPid::SetCut(Int_t n,Float_t pm,Float_t pilo,Float_t pihi,
			Float_t klo,Float_t khi,Float_t plo,Float_t phi)
{
    cut[n][0]=pm;
    cut[n][1]=pilo;
    cut[n][2]=pihi;
    cut[n][3]=klo;
    cut[n][4]=khi;
    cut[n][5]=plo;
    cut[n][6]=phi;
    return ;    
}
//------------------------------------------------------------
void AliITSPid::SetVec(Int_t ntrack,TVector info)
{
TClonesArray& arr=*trs;
    new( arr[ntrack] ) TVector(info);
}
//-----------------------------------------------------------
TVector* AliITSPid::GetVec(Int_t ntrack)
{
TClonesArray& arr=*trs;
    return (TVector*)arr[ntrack];
}
//-----------------------------------------------------------
void AliITSPid::SetEdep(Int_t track,Float_t Edep)
{
    TVector xx(0,11);
    if( ((TVector*)trs->At(track))->IsValid() )
	{TVector yy( *((TVector*)trs->At(track)) );xx=yy; }
    Int_t j=(Int_t)xx(0); if(j>4)return;
    xx(++j)=Edep;xx(0)=j;
    TClonesArray &arr=*trs;
    new(arr[track])TVector(xx);
}
//-----------------------------------------------------------
void AliITSPid::SetPmom(Int_t track,Float_t Pmom)
{
    TVector xx(0,11);
    if( ((TVector*)trs->At(track))->IsValid() )
	{TVector yy( *((TVector*)trs->At(track)) );xx=yy; }
    xx(10)=Pmom;
    TClonesArray &arr=*trs;
    new(arr[track])TVector(xx);
}
//-----------------------------------------------------------
void AliITSPid::SetPcod(Int_t track,Int_t partcode)
{
    TVector xx(0,11);
    if( ((TVector*)trs->At(track))->IsValid() )
	{TVector yy( *((TVector*)trs->At(track)) );xx=yy; }
    if(xx(11)==0)
	{xx(11)=partcode; mxtrs++;
	TClonesArray &arr=*trs;
	new(arr[track])TVector(xx);
	}
}
//-----------------------------------------------------------
void AliITSPid::Print(Int_t track)
{cout<<mxtrs<<" tracks in AliITSPid obj."<<endl;
    if( ((TVector*)trs->At(track))->IsValid() )
	{TVector xx( *((TVector*)trs->At(track)) );
	 xx.Print();
	 }
    else 
	{cout<<"No data for track "<<track<<endl;return;}
}
//-----------------------------------------------------------
void AliITSPid::Tab(void)
{
if(trs->GetEntries()==0){cout<<"No entries in TAB"<<endl;return;}
cout<<"------------------------------------------------------------------------"<<endl;
cout<<"Nq"<<"   q1  "<<"   q2  "<<"   q3  "<<"   q4  "<<"   q5   "<<
      " Qtrm    "    <<"  Wpi  "<<"  Wk   "<<"  Wp  "<<"Pmom  "<<endl;
cout<<"------------------------------------------------------------------------"<<endl;
for(Int_t i=0;i<trs->GetEntries();i++)
{
  TVector xx( *((TVector*)trs->At(i)) );     
    if( xx.IsValid() && xx(0)>0 )
	{
	    TVector xx( *((TVector*)trs->At(i)) );
	    if(xx(0)>=2)
		{
//       1)Calculate Qtrm	
		    xx(6)=(this->qtrm(i));

	    	 }else{
		     xx(6)=xx(1);
		 }
//	 2)Calculate Wpi,Wk,Wp
  	    this->GetPcode(xx(6),xx(10)/1000.);
	    xx(7)=GetWpi();
	    xx(8)=GetWk();
	    xx(9)=GetWp();
//       3)Print table
	    if(xx(0)>0){
//		    cout<<xx(0)<<" ";
		    for(Int_t j=1;j<11;j++){
		      if(i<7){ cout.width(7);cout.precision(4);cout<<xx(j);}
                      if(i>7){ cout.width(7);cout.precision(5);cout<<xx(j);}
		    }
		    cout<<endl;
		}
//	  4)Update data in TVector
	    TClonesArray &arr=*trs;
	    new(arr[i])TVector(xx);	 
	}
    else 
      {/*cout<<"No data for track "<<i<<endl;*/}
}// End loop for tracks
}
void AliITSPid::Reset(void)
{
  for(Int_t i=0;i<trs->GetEntries();i++){
    TVector xx(0,11);
    TClonesArray &arr=*trs;
    new(arr[i])TVector(xx);
  }
}
//-----------------------------------------------------------
AliITSPid::AliITSPid(Int_t ntrack)
{
  fSigmin=0.01;
    trs = new TClonesArray("TVector",ntrack);
    TClonesArray &arr=*trs;
    for(Int_t i=0;i<ntrack;i++)new(arr[i])TVector(0,11);
    mxtrs=0;
    //   
    fCutKa=new TF1("fcutka","pol4",0.05,0.4);
    Double_t ka[5]={25.616, -161.59, 408.97, -462.17, 192.86};
    fCutKa->SetParameters(ka);
    //
    fCutPr=new TF1("fcutpr","[0]/x/x+[1]",0.05,1.1);
    Double_t pr[2]={0.70675,0.4455};
    fCutPr->SetParameters(pr);
    //
    //---------- signal fit ----------
{//Pions
fGGpi[0]=new TF1("fp1pi","pol4",0.34,1.2);
  Double_t parpi_0[10]={ -1.9096471071e+03, 4.5354331545e+04, -1.1860738840e+05,
   1.1405329025e+05, -3.8289694496e+04  };
  fGGpi[0]->SetParameters(parpi_0);
fGGpi[1]=new TF1("fp2pi","[0]/x/x+[1]",0.34,1.2);
  Double_t parpi_1[10]={ 1.0791668283e-02, 9.7347716496e-01  };
  fGGpi[1]->SetParameters(parpi_1);
fGGpi[2]=new TF1("fp3pi","[0]/x/x+[1]",0.34,1.2);
  Double_t parpi_2[10]={ 5.8191602279e-04, 9.7285601334e-02  };
  fGGpi[2]->SetParameters(parpi_2);
fGGpi[3]=new TF1("fp4pi","pol4",0.34,1.2);
  Double_t parpi_3[10]={ 6.6267353195e+02, 7.1595101104e+02, -5.3095111914e+03,
   6.2900977606e+03, -2.2935862292e+03  };
  fGGpi[3]->SetParameters(parpi_3);
fGGpi[4]=new TF1("fp5pi","[0]/x/x+[1]",0.34,1.2);
  Double_t parpi_4[10]={ 9.0419011783e-03, 1.1628922525e+00  };
  fGGpi[4]->SetParameters(parpi_4);
fGGpi[5]=new TF1("fp6pi","[0]/x/x+[1]",0.34,1.2);
  Double_t parpi_5[10]={ 1.8324872519e-03, 2.1503968838e-01  };
  fGGpi[5]->SetParameters(parpi_5);
}//End Pions
{//Kaons
fGGka[0]=new TF1("fp1ka","pol4",0.24,1.2);
  Double_t parka_0[20]={
  -1.1204243395e+02,4.6716191428e+01,2.2584059281e+03,
  -3.7123338009e+03,1.6003647641e+03  };
  fGGka[0]->SetParameters(parka_0);
fGGka[1]=new TF1("fp2ka","[0]/x/x+[1]",0.24,1.2);
  Double_t parka_1[20]={
  2.5181172905e-01,8.7566001814e-01  };
  fGGka[1]->SetParameters(parka_1);
fGGka[2]=new TF1("fp3ka","pol6",0.24,1.2);
  Double_t parka_2[20]={
  8.6236021573e+00,-7.0970427531e+01,2.4846827669e+02,
  -4.6094401290e+02,4.7546751408e+02,-2.5807112462e+02,
  5.7545491696e+01  };
  fGGka[2]->SetParameters(parka_2);
}//End Kaons
{//Protons
fGGpr[0]=new TF1("fp1pr","pol4",0.4,1.2);
  Double_t parpr_0[10]={
  6.0150106543e+01,-8.8176206410e+02,3.1222644604e+03,
  -3.5269200901e+03,1.2859128345e+03  };
  fGGpr[0]->SetParameters(parpr_0);
fGGpr[1]=new TF1("fp2pr","[0]/x/x+[1]",0.4,1.2);
  Double_t parpr_1[10]={
  9.4970837607e-01,7.3573504201e-01  };
  fGGpr[1]->SetParameters(parpr_1);
fGGpr[2]=new TF1("fp3pr","[0]/x/x+[1]",0.4,1.2);
  Double_t parpr_2[10]={
  1.2498403757e-01,2.7845072306e-02  };
  fGGpr[2]->SetParameters(parpr_2);
}//End Protons
    //----------- end fit -----------

    fggpr=new TF1("ggpr","gaus",0.4,1.2);
    fggpi=new TF1("ggpi","gaus+gaus(3)",0.4,1.2);
    fggka=new TF1("ggka","gaus",0.4,1.2);

    //-------------------------------------------------
const int inf=10;
//         Ncut Pmom   pilo  pihi    klo    khi     plo    phi
//       cut[j] [0]    [1]    [2]    [3]    [4]     [5]    [6]
//----------------------------------------------------------------
    SetCut(  1, 0.12 ,  0.  ,  0.  , inf  , inf   , inf  , inf  );
    SetCut(  2, 0.20 ,  0.  ,  6.0 , 6.0  , inf   , inf  , inf  );
    SetCut(  3, 0.30 ,  0.  ,  3.5 , 3.5  , 9.0   , 9.0  , inf  );
    SetCut(  4, 0.41 ,  0.  ,  1.9 , 1.9  , 4.0   , 4.0  , inf  );
//----------------------------------------------------------------
    SetCut(  5, 0.47 , 0.935, 0.139, 1.738 , 0.498  , 3.5  , inf  );  //410-470
    SetCut(  6, 0.53 , 0.914, 0.136, 1.493 , 0.436  , 3.0  , inf  );  //470-530
//----------------------------------------------------------------    
    SetCut(  7, 0.59 , 0.895, 0.131, 1.384 , 0.290 , 2.7  , inf  );    //530-590
    SetCut(  8, 0.65 , 0.887, 0.121, 1.167 , 0.287 , 2.5  , inf  );     //590-650
    SetCut(  9, 0.73 , 0.879, 0.120, 1.153 , 0.257 , 2.0  , inf  );     //650-730
//----------------------------------------------------------------    
    SetCut( 10, 0.83 , 0.880, 0.126, 1.164 , 0.204 , 2.308 , 0.297 );       //730-830
    SetCut( 11, 0.93 , 0.918, 0.145, 1.164 , 0.204 , 2.00 , 0.168 );        //830-930
    SetCut( 12, 1.03 , 0.899, 0.128, 1.164 , 0.204  ,1.80 , 0.168);
    //------------------------ pi,K ---------------------
    aprob[0][0]=1212;     aprob[1][0]=33.;   // aprob[0][i] - const for pions,cut[i+5] 
    aprob[0][1]=1022;     aprob[1][1]=46.2 ; // aprob[1][i] -           kaons
    //---------------------------------------------------
    aprob[0][2]= 889.7;   aprob[1][2]=66.58; aprob[2][2]=14.53;
    aprob[0][3]= 686.;    aprob[1][3]=88.8;  aprob[2][3]=19.27;   
    aprob[0][4]= 697.;    aprob[1][4]=125.6; aprob[2][4]=28.67;
    //------------------------ pi,K,p -------------------
    aprob[0][5]= 633.7;   aprob[1][5]=100.1;   aprob[2][5]=37.99;   // aprob[2][i] -  protons
    aprob[0][6]= 469.5;   aprob[1][6]=20.74;   aprob[2][6]=25.43;
    aprob[0][7]= 355.;    aprob[1][7]=
                          355.*(20.74/469.5);  aprob[2][7]=34.08;
}
//End AliITSPid.cxx
