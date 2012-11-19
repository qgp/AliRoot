*----------------------------------------------------------------------
*
*  Filename             : HYDJET1_1.F
*
*  Author               : Igor Lokhtin  
*  Version              : HYDJET1_1.f
*  Last revision        : 27-MAR-2006 
*
*======================================================================
*
*  Description : Fast event generator for AA collisons at the LHC            
*
*  Method I.P. Lokhtin and A.M. Snigirev, Eur. Phys. J C 45 (2006) 211        
*
*======================================================================

      SUBROUTINE HYDRO(A,ifb,bmin,bmax,bfix,nh)  
      real hsin,hgauss,hftaa 
      real AW 
      real A,bmin,bmax,bfix 
      integer numjet,numpar 
      integer ifb,nh,np 
      external hsin,hgauss,hftaa,numjet,numpar,hyhard,hipsear 
      external ludata 
      common /lujets/ n,k(150000,5),p(150000,5),v(150000,5)
      common /hyjets/ nl,kl(150000,5),pl(150000,5),vl(150000,5) 
      common /hyipar/ bminh,bmaxh,AW,RA,sigin,np  
      common /hyfpar/ bgen,nbcol,npart,npyt,nhyd
      common /hyflow/ ytfl,ylfl,fpart
      common /hyjpar/ nhsel,ptmin,njet 
      save/lujets/,/hyjets/,/hyipar/,/hyfpar/,/hyflow/,/hyjpar/ 

      integer nnhyd, khyd
      real phyd, vhyd
      common /hyd/ nnhyd, khyd(150000,5),phyd(150000,5),vhyd(150000,5)
      save /hyd/

* reset lujets and hyd arrays before event generation 
      
      n=0 
      nnhyd=0 
      do ncl=1,150000
       do j=1,5
        p(ncl,j)=0.
	phyd(ncl,j)=0.
	v(ncl,j)=0. 
	vhyd(ncl,j)=0.  
        k(ncl,j)=0
	khyd(ncl,j)=0 
       enddo
      end do 

* set initial beam paramters: atomic weigth and radius in fm 
      AW=A         
      RA=1.15*AW**0.333333  
* 
      pi=3.14159
      
* generate impact parameter of A-A collision 

      if(ifb.eq.0) then 
       if(bfix.lt.0.) then    
        write(6,*) 'Impact parameter less than zero!'  
        bfix=0. 
       end if  
       if (bfix.gt.2.) then 
        write(6,*) 'Impact parameter larger than two nuclear radius!'  
        bfix=2.        
       end if 
       b1=bfix*RA
       bgen=bfix 
      else        
       if(bmin.lt.0.) then    
        write(6,*) 'Impact parameter less than zero!'  
        bmin=0. 
       end if 
       if(bmax.gt.2.) then    
        write(6,*) 'Impact parameter larger than two nuclear radius!'
        bmax=2.  
       end if             
       bminh=bmin 
       bmaxh=bmax   
       call hipsear(fmax1,xmin1) 
       fmax=fmax1 
       xmin=xmin1 
 3     bb1=xmin*rlu(0)+bminh  
       ff1=fmax*rlu(0) 
       fb=hsin(bb1) 
       if(ff1.gt.fb) goto 3    
       b1=bb1*RA       
       bgen=bb1                                                                        
      end if 

* set flow parameters 
      Tf=0.14                             ! freeze-out temperature     
      if (ylfl.lt.0.01.or.ylfl.gt.7.) ylfl=5.
      etmax=ylfl                          ! longitudinal flow rapidity 
      if (ytfl.lt.0.01.or.ytfl.gt.3.) ytfl=1.
      ytmax=ytfl                          ! transverse flow rapidity

* set inelastic NN cross section, mb 
      sigin=58.  
            
* calculate number of nucelons-participants 
      bb=bgen*RA                              ! impact parameter 
      npart=numpar(bb)                        ! Npart(b) 
      npar0=411                               ! Npart(Pb,b=0) 

* calculate number of binary NN sub-collisions 
      br=max(1.e-10,0.25*bgen*bgen)
      factor=9.*sigin*AW*AW/(80.*pi*RA*RA)      
      nbcol=int(factor*hftaa(br))              ! Nsub(b)
      nbco0=1923                               ! Nsub(Pb,b=0) 
  
* generate total multiplicity in event, np,  
* fpart - fraction of soft multiplicity proportional to # of participants, 
* fbcol=1-fpart - fraction of multiplicity proprtional to # of NN subcollisions 
      if(fpart.le.0.or.fpart.gt.1.) fpart=1. 
      fbcol=1.-fpart 
      rnp=nh*(fpart*npart+fbcol*nbcol)/(fpart*npar0+fbcol*nbco0)
      np=int(rnp) 
      sign=sqrt(rnp)
 1    if(nhsel.lt.4.and.np.gt.0) np=max(0,int(hgauss(rnp,sign))) 
      if(np.gt.150000) then 
       write(6,*) 'Warning, soft multiplicity too large!'   
       goto 1 
      end if 
      
* generate hard parton-parton scattering (Q>ptmin) 'njet' times with PYTHIA       
      if(nhsel.ne.1.and.nhsel.ne.2.and.nhsel.ne.3.and.nhsel.ne.4) 
     > nhsel=0 
      njet=0 
      if(nhsel.ne.0) then 
       if(ptmin.lt.5.or.ptmin.gt.500.) ptmin=10. 
       q=ptmin 
       njet=numjet(q) 
       call hyhard 
      end if 

      npyt=n 
      nhyd=np 
c      if(nhsel.lt.3) then 
c       nhyd=max(0,np-npyt)
c      else 
c       nhyd=0 
c       np=n
c      end if 
      if(nhyd.eq.0) goto 4 

* generate sort of hadrons (pions, kaons, nucleons) in jetset7* format 
      do ip=npyt+1,npyt+np                       !cycle on particles 
       yy=49.*rlu(0)  
       if(yy.lt.11.83333333) then
        kf=211
       elseif(yy.lt.23.66666667) then
        kf=-211
       elseif(yy.lt.35.5) then
        kf=111
       elseif(yy.lt.38.375) then
        kf=321
       elseif(yy.lt.41.25) then
        kf=-321 
       elseif(yy.lt.44.125) then
        kf=310
       elseif(yy.lt.47.) then
        kf=130  
       elseif(yy.lt.47.5) then
        kf=2212  
       elseif(yy.lt.48.) then
        kf=-2212
       elseif(yy.lt.48.5) then
        kf=2112 
       else  
        kf=-2112
       end if
       n=n+1
       k(n,1)=1
       k(n,2)=kf
       do j=3,5
        k(n,j)=0
       end do

       do j=1,5
        v(n,j)=0.
       enddo
        kfa=iabs(kf)
        p(n,5)=ulmass(kfa) 

* generate uniform distribution in system of a fluid element
 2     ep0=-1.*Tf*(log(max(1.e-10,rlu(0)))+log(max(1.e-10,rlu(0)))
     >   +log(max(1.e-10,rlu(0)))) 
       if(ep0.le.p(n,5)) go to 2 
       pp0=sqrt(abs(ep0**2-abs(p(n,5)**2))) 
       probt=pp0/ep0 
       if(rlu(0).gt.probt) go to 2 
       ctp0=2.*rlu(0)-1. 
       stp0=sqrt(abs(1.-ctp0**2)) 
       php0=2.*pi*rlu(0) 

* generate coordinates of a fluid element
c       etaf=etmax*(2.*rlu(0)-1.)         ! flat initial eta-spectrum
       etaf=hgauss(0.,etmax)              ! gaussian initial eta-spectrum
       phif=2.*pi*rlu(0) 
       rm1=sqrt(abs(RA*RA-b1*b1/4.*(sin(phif)**2)))+b1*cos(phif)/2. 
       rm2=sqrt(abs(RA*RA-b1*b1/4.*(sin(phif)**2)))-b1*cos(phif)/2. 
       RF=min(rm1,rm2) 
       rrf=RF*RF*rlu(0) 

* generate four-velocity of a fluid element
       vradf=sinh(ytmax)
       sb=RA*RA*(pi-2.*asin(b1/RA/2.))-b1*sqrt(abs(RA*RA-b1*b1/4.)) 
       reff=sqrt(sqrt(sb/pi)*RA) 
       urf=vradf*sqrt(abs(rrf))/reff          ! linear transverse profile
c       urf=vradf*rrf/reff**2                  !square transverse profile 
       utf=cosh(etaf)*sqrt(abs(1.+urf*urf)) 
       uzf=sinh(etaf)*sqrt(abs(1.+urf*urf)) 

* boost in the laboratory system
       uipi=pp0*(urf*stp0*cos(phif-php0)+uzf*ctp0) 
       bfac=uipi/(utf+1.)+ep0 
       p(n,1)=pp0*stp0*sin(php0)+urf*sin(phif)*bfac 
       p(n,2)=pp0*stp0*cos(php0)+urf*cos(phif)*bfac                    
       p(n,3)=pp0*ctp0+uzf*bfac
       p(n,4)=sqrt(p(n,1)**2+p(n,2)**2+p(n,3)**2+p(n,5)**2) 
            
      end do 
 
 4    continue

*      write(*,*) 'NHYD, NPYT, NTOT',nhyd,npyt,nhyd+npyt

* fill array 'hyd' 
 
      nnhyd = nhyd+npyt 
      
      do ih=1,n
       do jh=1,5
        phyd(ih,jh)=p(ih,jh)
        khyd(ih,jh)=k(ih,jh)                  
        vhyd(ih,jh)=v(ih,jh) 
       end do
      end do

      return
      end

********************************* HYHARD ***************************
      SUBROUTINE HYHARD 
*     generate 'njet' number of hard parton-parton scatterings with PYTHIA 
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      REAL ptmin,pj,vj,pl,vl,bminh,bmaxh,AW,RA,sigin,bgen 
      INTEGER PYK,PYCHGE,PYCOMP
      external pydata 
      external pyp,pyr,pyk,pyquen  
      common /pyjets/ n,npad,k(4000,5),p(4000,5),v(4000,5)
      common /lujets/ nj,kj(150000,5),pj(150000,5),vj(150000,5) 
      common /hyjets/ nl,kl(150000,5),pl(150000,5),vl(150000,5)            
      COMMON /PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON /PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON /PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      COMMON /PYSUBS/MSEL,MSELPD,MSUB(500),KFIN(2,-40:40),CKIN(200)
      common /pypars/ mstp(200),parp(200),msti(200),pari(200)
      common /hyjpar/ nhsel,ptmin,njet      
      common /hyipar/ bminh,bmaxh,AW,RA,sigin,np 
      common /hyfpar/ bgen,nbcol,npart,npyt,nhyd 
      save /pyjets/,/lujets/,/hyjets/,/pydat1/,/pydat2/,/pydat3/,
     +    /pysubs/,/hyjpar/,/hyipar/,/hyfpar/ 
      
* reset array of partons in 'hyjets'
      nl=0 
      do i=1,150000
       do j=1,5
        pl(i,j)=0.
       end do
       do j=1,5
        vl(i,j)=0.
       end do
       do j=1,5
        kl(i,j)=0
       end do
      end do 

* generate 'njet' PYTHIA events and fill arrays for partons and hadrons 
      if(njet.ge.1) then 
       mdcy(pycomp(111),1)=0               ! no pi0 decay 
       mdcy(pycomp(310),1)=0               ! no K_S0 decay 
       do ihard=1,njet       
        mstp(111)=0 
c        mstj(41)=0                         ! vacuum showering off 
        call pyevnt                        ! generate hard scattering

* generate quenched jets with PYQUEN if nhcel=2 
        if(nhsel.eq.2.or.nhsel.eq.4) then 
	 ifbp=0 
	 Ap=dble(AW)
	 bfixp=dble(RA*bgen) 
	 call pyquen(Ap,ifbp,bfixp) 
	end if 

* fill array of partons
         nl=nl+n  
	 if(nl.gt.150000-np) goto 51 
         do i=nl-n+1,nl  
	  ip=i+n-nl                             
	  do j=1,5
           pl(i,j)=p(ip,j) 
          end do
	  do j=1,5
           vl(i,j)=v(ip,j) 
          end do
          do j=1,5
           kl(i,j)=k(ip,j)
          end do
	  do j=3,5
	   kk=kl(i,j) 
           if(kk.gt.0) kl(i,j)=kk+nl-n 
          end do
         end do 
 51	 continue  
          
	 call pyexec                         ! hadronization done 
c         call pyedit(2)                      ! remove partons & leave hadrons 

* fill array of final particles
         nu=nj+n 
	 if(nu.gt.150000-np) then 
         write(6,*) 'Warning, multiplicity too large! Cut hard part.'   
	  goto 52
	 end if 
	 nj=nu  
         do i=nj-n+1,nj
	  ip=i+n-nj                            
	  do j=1,5
           pj(i,j)=p(ip,j) 
          end do
	  do j=1,5
           vj(i,j)=v(ip,j) 
          end do
          do j=1,5
           kj(i,j)=k(ip,j)
          end do
	  do j=3,5
	   kk=kj(i,j) 
           if(kk.gt.0) then 
	   kj(i,j)=kk+nj-n 
	   end if
	  end do
         end do               
      
       end do 
 52    njet=ihard-1 
      end if  
    
 
      return 
      end 
****************************** END HYHARD **************************      
      
********************************* HIPSEAR ***************************
      SUBROUTINE HIPSEAR (fmax,xmin) 
* find maximum and 'sufficient minimum' of differential inelasic AA cross 
* section as a function of impact paramater (xm, fm are outputs) 
      real hsin  
      external hsin  
      common /hyipar/ bminh,bmaxh,AW,RA,sigin,np  
      save /hyipar/ 
     
      xmin=bmaxh-bminh 
      fmax=0.
      do j=1,1000
      x=bminh+xmin*(j-1)/999.
      f=hsin(x) 
       if(f.gt.fmax) then
        fmax=f
       endif
      end do   
      
      return
      end
****************************** END HIPSEAR **************************

************************* HARINV **********************************
      SUBROUTINE HARINV(X,A,F,N,R)                                      
* gives interpolation of function F(X) with  arrays A(N) and F(N) 
      DIMENSION A(N),F(N)                                              
      IF(X.LT.A(1))GO TO 11                                            
      IF(X.GT.A(N))GO TO 4                                              
      K1=1                                                              
      K2=N                                                              
 2    K3=K2-K1                                                          
      IF(K3.LE.1)GO TO 6                                               
      K3=K1+K3/2                                                        
      IF(A(K3)-X) 7,8,9                                                 
 7    K1=K3                                                             
      GOTO2                                                            
 9    K2=K3                                                            
      GOTO2                                                             
 8    P=F(K3)                                                          
      RETURN                                                          
 3    B1=A(K1)                                                          
      B2=A(K1+1)                                                      
      B3=A(K1+2)                                                        
      B4=F(K1)                                                        
      B5=F(K1+1)                                                        
      B6=F(K1+2)                                                       
      R=B4*((X-B2)*(X-B3))/((B1-B2)*(B1-B3))+B5*((X-B1)*(X-B3))/       
     1 ((B2-B1)*(B2-B3))+B6*((X-B1)*(X-B2))/((B3-B1)*(B3-B2))           
      RETURN                                                          
 6    IF(K2.NE.N)GO TO 3                                               
      K1=N-2                                                            
      GOTO3                                                            
 4    C=ABS(X-A(N))                                                     
      IF(C.LT.0.1) GO TO 5                                           
      K1=N-2                                                           
 13   CONTINUE                                                          
C13   PRINT 41,X                                                        
C41   FORMAT(25H X IS OUT OF THE INTERVAL,3H X=,F15.9)                  
      GO TO 3                                                           
 5    R=F(N)                                                           
      RETURN                                                            
 11   C=ABS(X-A(1))                                                     
      IF(C.LT.0.1) GO TO 12                                         
      K1=1                                                             
      GOTO 13                                                           
 12   R=F(1)                                                            
      RETURN                                                            
      END                                                              
C************************** END HARINV *************************************

**************************** HSIMPA **********************************
      SUBROUTINE HSIMPA (A1,B1,H1,REPS1,AEPS1,FUNCT,X,                   
     1                     AI,AIH,AIABS)                                
* calculate intergal of function FUNCT(X) on the interval from A1 to B1 
      IMPLICIT REAL * 4 ( A-H,O-Z )
      IMPLICIT INTEGER(I-N)
      DIMENSION F(7), P(5)                                             
      H=SIGN ( H1, B1-A1 )                                             
      S=SIGN (1., H )                                                   
      A=A1                                                              
      B=B1                                                              
      AI=0.0                                                           
      AIH=0.0                                                           
      AIABS=0.0                                                        
      P(2)=4.                                                           
      P(4)=4.                                                           
      P(3)=2.                                                           
      P(5)=1.                                                           
      IF(B-A)1,2,1                                                      
 1    REPS=ABS(REPS1)                                                   
      AEPS=ABS(AEPS1)                                                  
      DO 3 K=1,7                                                        
 3    F(K)=10.E16                                                       
      X=A                                                              
      C=0.                                                              
      F(1)=FUNCT(X)/3.                                                  
 4    X0=X                                                              
      IF( (X0+4.*H-B)*S)5,5,6                                           
 6    H=(B-X0)/4.                                                       
      IF ( H ) 7,2,7                                                   
 7    DO 8 K=2,7                                                      
 8    F(K)=10.E16                                                       
      C=1.                                                           
 5    DI2=F (1)                                                       
      DI3=ABS( F(1) )                                                   
      DO 9 K=2,5                                                       
      X=X+H                                                           
      IF((X-B)*S)23,24,24                                              
 24   X=B                                                              
 23   IF(F(K)-10.E16)10,11,10                                          
 11   F(K)=FUNCT(X)/3.                                               
 10   DI2=DI2+P(K)*F(K)                                                 
 9    DI3=DI3+P(K)*ABS(F(K))                                            
      DI1=(F(1)+4.*F(3)+F(5))*2.*H                                      
      DI2=DI2*H                                                         
      DI3=DI3*H                                                        
      IF (REPS) 12,13,12                                               
 13   IF (AEPS) 12,14,12                                                
 12   EPS=ABS((AIABS+DI3)*REPS)                                         
      IF(EPS-AEPS)15,16,16                                              
 15   EPS=AEPS                                                          
 16   DELTA=ABS(DI2-DI1)                                               
      IF(DELTA-EPS)20,21,21                                             
 20   IF(DELTA-EPS/8.)17,14,14                                          
 17   H=2.*H                                                            
      F(1)=F(5)                                                         
      F(2)=F(6)                                                         
      F(3)=F(7)                                                         
      DO 19 K=4,7                                                       
 19   F(K)=10.E16                                                      
      GO TO 18                                                         
 14   F(1)=F(5)                                                         
      F(3)=F(6)                                                         
      F(5)=F(7)                                                         
      F(2)=10.E16                                                       
      F(4)=10.E16                                                      
      F(6)=10.E16                                                      
      F(7)=10.E16                                                      
 18   DI1=DI2+(DI2-DI1)/15.                                            
      AI=AI+DI1                                                         
      AIH=AIH+DI2                                                      
      AIABS=AIABS+DI3                                                   
      GO TO 22                                                          
 21   H=H/2.                                                            
      F(7)=F(5)                                                        
      F(6)=F(4)                                                        
      F(5)=F(3)                                                        
      F(3)=F(2)                                                         
      F(2)=10.E16                                                      
      F(4)=10.E16                                                      
      X=X0                                                            
      C=0.                                                             
      GO TO 5                                                          
 22   IF(C)2,4,2                                                      
 2    RETURN                                                        
      END                                                              
************************* END HSIMPA *******************************

**************************** HSIMPB **********************************
      SUBROUTINE HSIMPB (A1,B1,H1,REPS1,AEPS1,FUNCT,X,                   
     1                     AI,AIH,AIABS)                                
* calculate intergal of function FUNCT(X) on the interval from A1 to B1 
      IMPLICIT REAL * 4 ( A-H,O-Z )
      IMPLICIT INTEGER(I-N)
      DIMENSION F(7), P(5)                                             
      H=SIGN ( H1, B1-A1 )                                             
      S=SIGN (1., H )                                                   
      A=A1                                                              
      B=B1                                                              
      AI=0.0                                                           
      AIH=0.0                                                           
      AIABS=0.0                                                        
      P(2)=4.                                                           
      P(4)=4.                                                           
      P(3)=2.                                                           
      P(5)=1.                                                           
      IF(B-A)1,2,1                                                      
 1    REPS=ABS(REPS1)                                                   
      AEPS=ABS(AEPS1)                                                  
      DO 3 K=1,7                                                        
 3    F(K)=10.E16                                                       
      X=A                                                              
      C=0.                                                              
      F(1)=FUNCT(X)/3.                                                  
 4    X0=X                                                              
      IF( (X0+4.*H-B)*S)5,5,6                                           
 6    H=(B-X0)/4.                                                       
      IF ( H ) 7,2,7                                                   
 7    DO 8 K=2,7                                                      
 8    F(K)=10.E16                                                       
      C=1.                                                           
 5    DI2=F (1)                                                       
      DI3=ABS( F(1) )                                                   
      DO 9 K=2,5                                                       
      X=X+H                                                           
      IF((X-B)*S)23,24,24                                              
 24   X=B                                                              
 23   IF(F(K)-10.E16)10,11,10                                          
 11   F(K)=FUNCT(X)/3.                                               
 10   DI2=DI2+P(K)*F(K)                                                 
 9    DI3=DI3+P(K)*ABS(F(K))                                            
      DI1=(F(1)+4.*F(3)+F(5))*2.*H                                      
      DI2=DI2*H                                                         
      DI3=DI3*H                                                        
      IF (REPS) 12,13,12                                               
 13   IF (AEPS) 12,14,12                                                
 12   EPS=ABS((AIABS+DI3)*REPS)                                         
      IF(EPS-AEPS)15,16,16                                              
 15   EPS=AEPS                                                          
 16   DELTA=ABS(DI2-DI1)                                               
      IF(DELTA-EPS)20,21,21                                             
 20   IF(DELTA-EPS/8.)17,14,14                                          
 17   H=2.*H                                                            
      F(1)=F(5)                                                         
      F(2)=F(6)                                                         
      F(3)=F(7)                                                         
      DO 19 K=4,7                                                       
 19   F(K)=10.E16                                                      
      GO TO 18                                                         
 14   F(1)=F(5)                                                         
      F(3)=F(6)                                                         
      F(5)=F(7)                                                         
      F(2)=10.E16                                                       
      F(4)=10.E16                                                      
      F(6)=10.E16                                                      
      F(7)=10.E16                                                      
 18   DI1=DI2+(DI2-DI1)/15.                                            
      AI=AI+DI1                                                         
      AIH=AIH+DI2                                                      
      AIABS=AIABS+DI3                                                   
      GO TO 22                                                          
 21   H=H/2.                                                            
      F(7)=F(5)                                                        
      F(6)=F(4)                                                        
      F(5)=F(3)                                                        
      F(3)=F(2)                                                         
      F(2)=10.E16                                                      
      F(4)=10.E16                                                      
      X=X0                                                            
      C=0.                                                             
      GO TO 5                                                          
 22   IF(C)2,4,2                                                      
 2    RETURN                                                        
      END                                                              
************************* END HSIMPB *******************************

* function to calculate differential inelastic AA cross section 
      real function hsin(x) 
      external hftaa 
      real hftaa 
      common /hyipar/ bminh,bmaxh,AW,RA,sigin,np  
      save /hyipar/ 
      sigp=sigin 
      taapb0=33.16 
      br=max(1.e-10,0.25*x*x) 
      hsin=x*(1.-exp(-1.*hftaa(br)*sigp*taapb0)) 
      return 
      end 

* set of functions to calculate number of nucleons-participants at im.par.b 
      integer function numpar(c) 
      IMPLICIT REAL * 4 (A-H,O-Z)   
      real HFUNC1
      external HFUNC1 
      common /hynup1/ bp,x    
      EPS=0.01  
      A=0. 
      B=6.28318 
      H=0.01*(B-A)    
      bp=c    
      CALL HSIMPA(A,B,H,EPS,1.E-8,HFUNC1,X,RES,AIH,AIABS)
      numpar=int(2.*RES) 
      return 
      end   
*
      real function HFUNC1(x) 
      IMPLICIT REAL * 4 (A-H,O-Z) 
      real HFUNC2 
      external HFUNC2 
      common /hyipar/ bminh,bmaxh,AW,RA,sigin,np
      common /hynup1/ bp,xx
      save /hyipar/  
      xx=x 
      b2=0.5*bp 
      r1=abs(sqrt(abs(RA*RA-(b2*sin(xx))**2))+b2*cos(xx))
      r2=abs(sqrt(abs(RA*RA-(b2*sin(xx))**2))-b2*cos(xx))
      EPS=0.01 
      A=0. 
      B=min(r1,r2)
      H=0.01*(B-A)    
      CALL HSIMPB(A,B,H,EPS,1.E-8,HFUNC2,Y,RES,AIH,AIABS)
      HFUNC1=RES 
      return 
      end   
*      
      real function HFUNC2(y) 
      IMPLICIT REAL * 4 (A-H,O-Z) 
      real hythik 
      external hythik 
      common /hyipar/ bminh,bmaxh,AW,RA,sigin,np 
      common /hynup1/ bp,x 
      save /hyipar/ 
      r1=sqrt(abs(y*y+bp*bp/4.+y*bp*cos(x))) 
      r2=sqrt(abs(y*y+bp*bp/4.-y*bp*cos(x)))
      s=1.-exp(-0.1*sigin*hythik(r2))
      HFUNC2=y*hythik(r2)*s 
      return 
      end   
 
* to calculate nuclear thikness function 
       real function hythik(r)   
       IMPLICIT REAL * 4 (A-H,O-Z) 
       common /hyipar/ bminh,bmaxh,AW,RA,sigin,np
       save /hyipar/ 
       hythik=0.477465*AW*sqrt(abs(RA*RA-r*r))/(RA**3)
       return
       end

* to calculate nuclear overlap function  
      real function hftaa(br)  
      common /hyipar/ bminh,bmaxh,AW,RA,sigin,np  
      save /hyipar/  
      sbr=sqrt(abs(1.-br)) 
      taa=1.-br*(1.+(1.-0.25*br)*log(1./br)+2.*(1.-0.25*br)*
     + (log(1.+sbr)-sbr/(1.+sbr))-0.5*br*(1.-br)/((1.+sbr)**2))    
      hftaa=taa*((AW/207)**1.33333333) 
      return 
      end   

* function to calculate number of hard parton-parton scatterings with 
* Q>x at sqrt{s}=5.5 TeV   
      integer function numjet(x)      
      common /hyipar/ bminh,bmaxh,AW,RA,sigin,np
      common /hyfpar/ bgen,nbcol,npart,npyt,nhyd 
      save /hyipar/,/hyfpar/ 
      dimension ptj(30),sgj(30) 
      data ptj/5.,6.,7.,8.,9.,10.,12.,14.,16.,18.,20.,23.,26.,
     + 30.,35.,40.,50.,60.,70.,80.,90.,100.,120.,150.,200.,
     + 250.,300.,400.,450.,500./  
      data sgj/31.,16.8,9.5,6.1,4.1,2.7,1.4,0.8,0.46,0.3,0.2,
     + 0.11,6.8e-2,3.9e-2,2.06e-2,1.16e-2,4.46e-3,2.e-3,1.e-3,
     + 5.3e-4,3.1e-4,1.9e-4,7.7e-5,2.6e-5,5.9e-6,1.6e-6,5.9e-7,
     + 1.e-7,4.8e-8,2.3e-8/
      pt=x 
      i=0 
 31   i=i+1        
      if(i.le.30) then 
       dx=abs(ptj(i)-pt)  
       if(dx.le.0.1) then 
        sjet=sgj(i) 
	goto 32 
       else
        goto 31 
       end if 
      else 
       call harinv(pt,ptj,sgj,30,sjet) 
      end if 
 32   pjet=sjet/sigin  
      ijet=0 
      do i=1,nbcol 
       if(rlu(0).lt.pjet) ijet=ijet+1 
      end do 
      numjet=ijet        
      return 
      end   

* function to generate gauss distribution 
      real function hgauss(x0,sig)
 41   u1=rlu(0)
      u2=rlu(0)
      v1=2.*u1-1.
      v2=2.*u2-1.
      s=v1**2+v2**2
      if(s.gt.1) go to 41
      hgauss=v1*sqrt(-2.*log(s)/s)*sig+x0
      return
      end


**************************************************************************        
