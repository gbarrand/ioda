// utilitaires de pixelisation HEALPix
#include "sopnamsp.h"
#include "HEALPixUtils.h"
#include <iostream>
#include <math.h>
//#include <complex>
#include "tvector.h"
#include "smathconst.h"
extern "C" 
{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
}
#include <algorithm> //G.Barrand : _MSC_VER : for min

using namespace SOPHYA;

//////////////////////////////////////////////////////////////////////////
//
// ------------- Classe PIXELS_XY -----------------------
//
class PIXELS_XY
{

public :

static PIXELS_XY& instance();

NDataBlock<int_4> pix2x_;
NDataBlock<int_4> pix2y_;
NDataBlock<int_4> x2pix_;
NDataBlock<int_4> y2pix_;

private :

PIXELS_XY();
void mk_pix2xy();
void mk_xy2pix();
};



//*******************************************************************
// Class PIXELS_XY 
// Construction des tableaux necessaires a la traduction des indices RING en 
// indices NESTED (ou l'inverse)
//*******************************************************************

PIXELS_XY::PIXELS_XY()
{
  pix2x_.ReSize(1024);
  pix2x_.Reset();
  pix2y_.ReSize(1024);
  pix2y_.Reset();
  x2pix_.ReSize(128);
  x2pix_.Reset();
  y2pix_.ReSize(128);
  y2pix_.Reset();
  mk_pix2xy(); 
  mk_xy2pix(); 
}

// Instance unique de la classe PIXELS_XY 
static PIXELS_XY * _singleton = NULL;

PIXELS_XY& PIXELS_XY::instance()
{
  if (_singleton == NULL) _singleton = new PIXELS_XY ;
  return (*_singleton);
}

void PIXELS_XY::mk_pix2xy() 
{
  /*
    ==================================================
    subroutine mk_pix2xy
    ==================================================
    c     constructs the array giving x and y in the face from pixel number
    c     for the nested (quad-cube like) ordering of pixels
    c
    c     the bits corresponding to x and y are interleaved in the pixel number
    c     one breaks up the pixel number by even and odd bits
    ==================================================
  */
  // tranlated from FORTRAN (Gorski) to C, by B. Revenu, revised Guy Le Meur
  //  (16/12/98)

  int kpix, jpix, IX, IY, IP, ID;
  
  for(kpix = 0; kpix < 1024; kpix++) 
    {
      jpix = kpix;
      IX = 0;
      IY = 0;
      IP = 1 ;//  ! bit position (in x and y)
      while( jpix!=0 )
	{ // ! go through all the bits
	  ID=jpix%2;//  ! bit value (in kpix), goes in ix
	  jpix = jpix/2;
	  IX = ID*IP+IX;
      
	  ID=jpix%2;//  ! bit value (in kpix), goes in iy
	  jpix = jpix/2;
	  IY = ID*IP+IY;
     
	  IP = 2*IP;//        ! next bit (in x and y)
	}
      pix2x_(kpix) = IX;//     ! in 0,31
      pix2y_(kpix) = IY;//     ! in 0,31
    }
}

void PIXELS_XY::mk_xy2pix() 
{
  /*
    =================================================
    subroutine mk_xy2pix
    =================================================
    c     sets the array giving the number of the pixel lying in (x,y)
    c     x and y are in {1,128}
    c     the pixel number is in {0,128**2-1}
    c
    c     if  i-1 = sum_p=0  b_p * 2^p
    c     then ix = sum_p=0  b_p * 4^p
    c          iy = 2*ix
    c     ix + iy in {0, 128**2 -1}
    =================================================
  */
  // tranlated from FORTRAN (Gorski) to C, by B. Revenu, revised Guy Le Meur
  //  (16/12/98)

  int K,IP,I,J,ID;
  for(I = 1; I <= 128; I++) 
    {
      J  = I-1;//            !pixel numbers
      K  = 0;//
      IP = 1;//
      truc : if( J==0 ) 
	{
	  x2pix_(I-1) = K;
	  y2pix_(I-1) = 2*K;
	}
      else 
	{
	  ID = J%2;  //RzModFloor (int)fmod(J,2);
	  J  = J/2;
	  K  = IP*ID+K;
	  IP = IP*4;
	  goto truc;
	}
    }     
}



int_4 HEALPix::nest2ring(int_4 nside, int_4 ipnest) 
{
  /*
    ====================================================
    subroutine nest2ring(nside, ipnest, ipring)
    ====================================================
    c     conversion from NESTED to RING pixel number
    ====================================================
  */
  // tranlated from FORTRAN (Gorski) to C, by B. Revenu, revised Guy Le Meur
  //  (16/12/98)
  
    const PIXELS_XY& PXY= PIXELS_XY::instance();

  int npix, npface, face_num, ncap, n_before;
  int ipf, ip_low, ip_trunc, ip_med, ip_hi;
  int ix, iy, jrt, jr, nr, jpt, jp, kshift, nl4;
  int ns_max=8192;
  int jrll[12]={2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4};
  int jpll[12]={1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7};
  
  if(  nside<1 ||  nside>ns_max ) {
    char buff[64];
    sprintf(buff,"HEALPix::nest2ring nside(=%d) out of range ", nside);
    throw RangeCheckError(PExcLongMessage(buff));
  }
  npix = 12 *  nside* nside;
  if( ipnest<0 || ipnest>npix-1 ) {
    char buff[64];
    sprintf(buff,"HEALPix::nest2ring ipnest(=%d) out of range ", ipnest);
    throw RangeCheckError(PExcLongMessage(buff));
  }

  ncap  = 2* nside*( nside-1);// ! number of points in the North Polar cap
  nl4   = 4* nside;
  
  //c     finds the face, and the number in the face
  npface =  nside* nside;
  //cccccc      ip = ipnest - 1         ! in {0,npix-1}
  
  face_num = ipnest/npface;//  ! face number in {0,11}
  ipf =ipnest%npface;//  ! pixel number in the face {0,npface-1}
  //c     finds the x,y on the face (starting from the lowest corner)
  //c     from the pixel number
  ip_low=ipf%1024;                //   ! content of the last 10 bits
  ip_trunc =   ipf/1024;         //    ! truncation of the last 10 bits
  ip_med=ip_trunc%1024;         //     ! content of the next 10 bits
  ip_hi  =     ip_trunc/1024;//   ! content of the high weight 10 bits
  
  ix = 1024*PXY.pix2x_(ip_hi)+32*PXY.pix2x_(ip_med)+PXY.pix2x_(ip_low);
  iy = 1024*PXY.pix2y_(ip_hi)+32*PXY.pix2y_(ip_med)+PXY.pix2y_(ip_low);
  
  //c     transforms this in (horizontal, vertical) coordinates
  jrt = ix + iy;//  ! 'vertical' in {0,2*(nside-1)}
  jpt = ix - iy;//  ! 'horizontal' in {-nside+1,nside-1}
  
  //c     computes the z coordinate on the sphere
  //      jr =  jrll[face_num+1]*nside - jrt - 1;//   ! ring number in {1,4*nside-1}
  jr =  jrll[face_num]*nside - jrt - 1;
  nr = nside;//                  ! equatorial region (the most frequent)
  n_before = ncap + nl4 * (jr - nside);
  kshift=(jr - nside)%2;
  if( jr<nside ) {//then     ! north pole region
    nr = jr;
    n_before = 2 * nr * (nr - 1);
    kshift = 0;
  }
  else if( jr>3*nside ) {//then ! south pole region
    nr = nl4 - jr;
    n_before = npix - 2 * (nr + 1) * nr;
    kshift = 0;
  }
  
  //c     computes the phi coordinate on the sphere, in [0,2Pi]
  jp = (jpll[face_num]*nr + jpt + 1 + kshift)/2;//  ! 'phi' number in the ring in {1,4*nr}
  
  if( jp>nl4 ) jp = jp - nl4;
  if( jp<1 )   jp = jp + nl4;
  
  //RzDel int aux=n_before + jp - 1;
  return (n_before + jp - 1);// ! in {0, npix-1}
}


int_4 HEALPix::ring2nest(int_4 nside, int_4 ipring)  
{
  /*
    ==================================================
    subroutine ring2nest(nside, ipring, ipnest)
    ==================================================
    c     conversion from RING to NESTED pixel number
    ==================================================
  */
  // tranlated from FORTRAN (Gorski) to C, by B. Revenu, revised Guy Le Meur
  //  (16/12/98)

    const PIXELS_XY& PXY= PIXELS_XY::instance();

  double fihip, hip;
  int npix, nl2, nl4, ncap, ip, iphi, ipt, ipring1;
  int     kshift, face_num, nr;
  int irn, ire, irm, irs, irt, ifm , ifp;
  int ix, iy, ix_low, ix_hi, iy_low, iy_hi, ipf;
  int ns_max(8192);
  
  // coordinate of the lowest corner of each face
  int jrll[12]={2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4};// ! in unit of nside
  int jpll[12]={1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7};//! in unit of nside/2
  
  if( nside<1 || nside>ns_max ) {
    char buff[64];
    sprintf(buff,"HEALPix::ring2nest nside(=%d) out of range ", nside);
    throw RangeCheckError(PExcLongMessage(buff));
  }
  npix = 12 * nside*nside;
  if( ipring<0 || ipring>npix-1 ) {
    char buff[64];
    sprintf(buff,"HEALPix::ring2nest ipring(=%d) out of range ", ipring);
    throw RangeCheckError(PExcLongMessage(buff));
  }
  
  nl2 = 2*nside;
  nl4 = 4*nside;
  npix = 12*nside*nside;//      ! total number of points
  ncap = 2*nside*(nside-1);// ! points in each polar cap, =0 for nside =1
  ipring1 = ipring + 1;
  
  //c     finds the ring number, the position of the ring and the face number
  if( ipring1<=ncap ) {//then
    
    hip   = ipring1/2.;
    fihip = floor ( hip );
    irn   = (int)floor( sqrt( hip - sqrt(fihip) ) ) + 1;// ! counted from North pole
    iphi  = ipring1 - 2*irn*(irn - 1);
    
    kshift = 0;
    nr = irn   ;//               ! 1/4 of the number of points on the current ring
    face_num = (iphi-1) / irn;// ! in {0,3}
  }
  else if( ipring1<=nl2*(5*nside+1) ) {//then
    
    ip    = ipring1 - ncap - 1;
    irn   = ip / nl4 + nside; //RzModFloor (int)floor( ip / nl4 ) + nside;   ! counted from North pole
    iphi  = ip%nl4 + 1;  //RzModFloor (int)fmod(ip,nl4) + 1;
    
    kshift  = (irn+nside)%2; //RzModFloor (int)fmod(irn+nside,2);  ! 1 if irn+nside is odd, 0 otherwise
    nr = nside;
    ire =  irn - nside + 1;// ! in {1, 2*nside +1}
    irm =  nl2 + 2 - ire;
    ifm = (iphi - ire/2 + nside -1) / nside;// ! face boundary
    ifp = (iphi - irm/2 + nside -1) / nside;
    if( ifp==ifm ) {//then          ! faces 4 to 7
      face_num = ifp%4+4; // RzModFloor (int)fmod(ifp,4) + 4;
    }
    else if( ifp + 1==ifm ) {//then ! (half-)faces 0 to 3
      face_num = ifp;
    }
    else if( ifp - 1==ifm ) {//then ! (half-)faces 8 to 11
      face_num = ifp + 7;
    }
  }
  else {
    
    ip    = npix - ipring1 + 1;
    hip   = ip/2.;
    fihip = floor ( hip );
    irs   = (int)floor( sqrt( hip - sqrt(fihip) ) ) + 1;//  ! counted from South pole
    iphi  = 4*irs + 1 - (ip - 2*irs*(irs-1));
    
    kshift = 0;
    nr = irs;
    irn   = nl4 - irs;
    face_num = (iphi-1) / irs + 8;// ! in {8,11}
  }
  
  //c     finds the (x,y) on the face
  irt =   irn  - jrll[face_num]*nside + 1;//       ! in {-nside+1,0}
  ipt = 2*iphi - jpll[face_num]*nr - kshift - 1;// ! in {-nside+1,nside-1}


  if( ipt>=nl2 ) ipt = ipt - 8*nside;// ! for the face #4
  
  ix =  (ipt - irt ) / 2;
  iy = -(ipt + irt ) / 2;
  
  ix_low = ix%128; //RzModFloor (int)fmod(ix,128);
  ix_hi  = ix/128;
  iy_low = iy%128; //RzModFloor (int)fmod(iy,128); 
  iy_hi  = iy/128;
  ipf=(PXY.x2pix_(ix_hi)+PXY.y2pix_(iy_hi))*(128*128)+(PXY.x2pix_(ix_low)+PXY.y2pix_(iy_low));

  return (ipf + face_num* nside *nside);//   ! in {0, 12*nside**2 - 1}
}

int_4 HEALPix::ang2pix_ring(int_4 nside, double theta, double phi)  
{
  /*
    ==================================================
    c     gives the pixel number ipix (RING) 
    c     corresponding to angles theta and phi
    c==================================================
  */
  // tranlated from FORTRAN (Gorski) to C, by B. Revenu, revised Guy Le Meur
  //  (16/12/98)

  int nl2, nl4, ncap, npix, jp, jm, ipix1;
  double  z, za, tt, tp, tmp;
  int ir, ip, kshift;

  double piover2(Pi/2.);
  double twopi(2.*Pi);
  double z0(2./3.);
  int ns_max(8192);

  if( nside<1 || nside>ns_max ) {
    char buff[64];
    sprintf(buff,"HEALPix::ang2pix_ring nside(=%d) out of range ", nside);
    throw RangeCheckError(PExcLongMessage(buff));
  }

  if( theta<0. || theta>Pi) {
    char buff[64];
    sprintf(buff,"HEALPix::ang2pix_ring theta(=%g) out of range ", theta);
    throw RangeCheckError(PExcLongMessage(buff));
  }

  z = cos(theta);
  za = fabs(z);
  if( phi >= twopi)  phi = phi - twopi;
  if (phi < 0.)     phi = phi + twopi;
  tt = phi / piover2;//  ! in [0,4)

  nl2 = 2*nside;
  nl4 = 4*nside;
  ncap  = nl2*(nside-1);// ! number of pixels in the north polar cap
  npix  = 12*nside*nside;

  if( za <= z0 ) {
    
    jp = (int)floor(nside*(0.5 + tt - z*0.75));// ! index of  ascending edge line 
    jm = (int)floor(nside*(0.5 + tt + z*0.75));// ! index of descending edge line

    ir = nside + 1 + jp - jm;// ! in {1,2n+1} (ring number counted from z=2/3)
    kshift = 0;
    //RzModFloor if (fmod(ir,2)==0.) kshift = 1; ! kshift=1 if ir even, 0 otherwise
    if ((ir%2)==0) kshift = 1;// ! kshift=1 if ir even, 0 otherwise
    
    //RzModFloor ip = (int)floor( ( jp+jm - nside + kshift + 1 ) / 2 ) + 1; ! in {1,4n}
    ip = ( jp+jm - nside + kshift + 1 )/2  + 1;  
    if( ip>nl4 ) ip = ip - nl4;
    
    ipix1 = ncap + nl4*(ir-1) + ip ;
  }
  else {

    tp = tt - floor(tt);//      !MOD(tt,1.d0)
    tmp = sqrt( 3.*(1. - za) );

    jp = (int)floor( nside * tp * tmp );// ! increasing edge line index
    jm = (int)floor( nside * (1. - tp) * tmp );// ! decreasing edge line index

    ir = jp + jm + 1;//        ! ring number counted from the closest pole
    ip = (int)floor( tt * ir ) + 1;// ! in {1,4*ir}
    if( ip>4*ir ) ip = ip - 4*ir;
      
    ipix1 = 2*ir*(ir-1) + ip;
      if( z<=0. ) {
	ipix1 = npix - 2*ir*(ir+1) + ip;
      }
  }
    return (ipix1 - 1);// ! in {0, npix-1}
}

int_4 HEALPix::ang2pix_nest(int_4 nside, double theta, double phi) 
{
  /*
    ==================================================
    subroutine ang2pix_nest(nside, theta, phi, ipix)
    ==================================================
    c     gives the pixel number ipix (NESTED) 
    c     corresponding to angles theta and phi
    c
    c     the computation is made to the highest resolution available (nside=8192)
    c     and then degraded to that required (by integer division)
    c     this doesn't cost more, and it makes sure 
    c     that the treatement of round-off will be consistent 
    c     for every resolution
    ==================================================
  */
  // tranlated from FORTRAN (Gorski) to C, by B. Revenu, revised Guy Le Meur
  //  (16/12/98)
  
    const PIXELS_XY& PXY= PIXELS_XY::instance();

  double    z, za, z0, tt, tp, tmp;
  int face_num,jp,jm;
  int ifp, ifm;
  int  ix, iy, ix_low, ix_hi, iy_low, iy_hi, ipf, ntt;
  double piover2(Pi/2.), twopi(2.*Pi);
  int ns_max(8192);
  
  if( nside<1 || nside>ns_max ) {
    char buff[64];
    sprintf(buff,"HEALPix:::ang2pix_nest nside(=%d) out of range ", nside);
    throw RangeCheckError(PExcLongMessage(buff));
  }
  if( theta<0 || theta>Pi ) {
    char buff[64];
    sprintf(buff,"HEALPix:::ang2pix_nest theta(=%g) out of range ", theta);
    throw RangeCheckError(PExcLongMessage(buff));
  }
  z  = cos(theta);
  za = fabs(z);
  z0 = 2./3.;
  if( phi>=twopi ) phi = phi - twopi;
  if( phi<0. )    phi = phi + twopi;
  tt = phi / piover2;// ! in [0,4[
  if( za<=z0 ) { // then ! equatorial region
    
    //(the index of edge lines increase when the longitude=phi goes up)
    jp = (int)floor(ns_max*(0.5 + tt - z*0.75));// !  ascending edge line index
    jm = (int)floor(ns_max*(0.5 + tt + z*0.75));// ! descending edge line index
    
    //c        finds the face
    ifp = jp / ns_max;//  ! in {0,4}
    ifm = jm / ns_max;
    if( ifp==ifm ) face_num = (ifp%4)+4; //RzModFloor (int)fmod(ifp,4) + 4; then  ! faces 4 to 7
    else if( ifp<ifm ) face_num = ifp%4; //RzModFloor (int)fmod(ifp,4); (half-)faces 0 to 3
    else face_num = (ifm%4) + 8; //RzModFloor (int)fmod(ifm,4) + 8;! (half-)faces 8 to 11
    
    ix = jm%ns_max; //RzModFloor (int)fmod(jm, ns_max);
    iy = ns_max - (jp%ns_max) - 1;//RzModFloor  ns_max - (int)fmod(jp, ns_max) - 1;
  }
  else { //! polar region, za > 2/3
    
    ntt = (int)floor(tt);
    if( ntt>=4 ) ntt = 3;
    tp = tt - ntt;
    tmp = sqrt( 3.*(1. - za) );//  ! in ]0,1]
    
    //(the index of edge lines increase when distance from the closest pole goes up)
    jp = (int)floor(ns_max*tp*tmp); // ! line going toward the pole as phi increases
    jm = (int)floor(ns_max*(1.-tp)*tmp); // ! that one goes away of the closest pole
    jp = (int)min(ns_max-1, jp);// ! for points too close to the boundary
    jm = (int)min(ns_max-1, jm);
    
    // finds the face and pixel's (x,y)
    if( z>=0 ) {
      face_num = ntt;//  ! in {0,3}
      ix = ns_max - jm - 1;
      iy = ns_max - jp - 1;
    }
    else {
      face_num = ntt + 8;// ! in {8,11}
      ix =  jp;
      iy =  jm;
    }
  }
  
  ix_low = (ix%128); //RzModFloor (int)fmod(ix,128);
  ix_hi  =     ix/128;
  iy_low = (iy%128); //RzModFloor ((int)fmod(iy,128);
  iy_hi  =     iy/128;
  ipf= (PXY.x2pix_(ix_hi)+PXY.y2pix_(iy_hi))*(128*128)+(PXY.x2pix_(ix_low)+PXY.y2pix_(iy_low));
  //  ipf = ipf / pow(ns_max/nside,2.);//  ! in {0, nside**2 - 1}
  //  return ( ipf + face_num*pow(nside,2));//    ! in {0, 12*nside**2 - 1}
  // $CHECK$  Reza 25/10/99 , pow remplace par *
  ipf = ipf / ((ns_max/nside)*(ns_max/nside));
  return (ipf + face_num*nside*nside);
}

void HEALPix::pix2ang_ring(int_4 nside,int_4 ipix,double& theta,double& phi)  
{
  /*
    ===================================================
    c     gives theta and phi corresponding to pixel ipix (RING) 
    c     for a parameter nside
    ===================================================
  */
  // tranlated from FORTRAN (Gorski) to C, by B. Revenu, revised Guy Le Meur
  //  (16/12/98)
  
  int nl2, nl4, npix, ncap, iring, iphi, ip, ipix1;
  double  fact1, fact2, fodd, hip, fihip;
  
  int ns_max(8192);
  
  if( nside<1 || nside>ns_max ) {
    char buff[64];
    sprintf(buff,"HEALPix::pix2ang_ring nside(=%d) out of range ", nside);
    throw RangeCheckError(PExcLongMessage(buff));
  }
  npix = 12*nside*nside;      // ! total number of points
  if( ipix<0 || ipix>npix-1 ) {
    char buff[64];
    sprintf(buff,"HEALPix::pix2ang_ring ipix(=%d) out of range ", ipix);
    throw RangeCheckError(PExcLongMessage(buff));
  }
  
  ipix1 = ipix + 1; // in {1, npix}
  nl2 = 2*nside;
  nl4 = 4*nside;
  ncap = 2*nside*(nside-1);// ! points in each polar cap, =0 for nside =1
  fact1 = 1.5*nside;
  fact2 = 3.0*nside*nside;
  
  if( ipix1 <= ncap ) {  //! North Polar cap -------------
    
    hip   = ipix1/2.;
    fihip = floor(hip);
    iring = (int)floor( sqrt( hip - sqrt(fihip) ) ) + 1;// ! counted from North pole
    iphi  = ipix1 - 2*iring*(iring - 1);
    
    theta = acos( 1. - iring*iring / fact2 );
    phi   = ((double)iphi - 0.5) * Pi/(2.*iring);
    //    cout << theta << " " << phi << endl;
  }
  else if( ipix1 <= nl2*(5*nside+1) ) {//then ! Equatorial region ------
    
    ip    = ipix1 - ncap - 1;
    iring = ip/nl4  + nside; //RzModFloor (int)floor( ip / nl4 ) + nside; ! counted from North pole
    iphi  = ip%nl4 + 1;
    
    fodd  = 0.5 * (1 + (iring+nside)%2 );//  ! 1 if iring+nside is odd, 1/2 otherwise
    theta = acos( (nl2 - iring) / fact1 );
    phi   = ((double)iphi - fodd) * Pi /(2.*nside);
  }
  else {//! South Polar cap -----------------------------------
    
    ip    = npix - ipix1 + 1;
    hip   = ip/2.;
    fihip = floor(hip);
    iring = (int)floor( sqrt( hip - sqrt(fihip) ) ) + 1;//     ! counted from South pole
    iphi  = (int)(4.*iring + 1 - (ip - 2.*iring*(iring-1)));
    
    theta = acos( -1. + iring*iring / fact2 );
    phi   = ((double)iphi - 0.5) * Pi/(2.*iring);
    //    cout << theta << " " << phi << endl;
  }
}

void HEALPix::pix2ang_nest(int_4 nside,int_4 ipix,double& theta,double& phi)  
{
  /*
    ==================================================
    subroutine pix2ang_nest(nside, ipix, theta, phi)
    ==================================================
    c     gives theta and phi corresponding to pixel ipix (NESTED) 
    c     for a parameter nside
    ==================================================
  */
  // tranlated from FORTRAN (Gorski) to C, by B. Revenu, revised Guy Le Meur
  //  (16/12/98)

    const PIXELS_XY& PXY= PIXELS_XY::instance();
    
  int npix, npface, face_num;
  int ipf, ip_low, ip_trunc, ip_med, ip_hi;
  int ix, iy, jrt, jr, nr, jpt, jp, kshift, nl4;
  double z, fn, fact1, fact2;
  double piover2(Pi/2.);
  int ns_max(8192);
          
  // ! coordinate of the lowest corner of each face
  int jrll[12]={2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4};//! in unit of nside
  int jpll[12]={1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7};// ! in unit of nside/2
  
  if( nside<1 || nside>ns_max ) {
    char buff[64];
    sprintf(buff,"HEALPix::pix2ang_nest nside(=%d) out of range ", nside);
    throw RangeCheckError(PExcLongMessage(buff));
  }
  npix = 12 * nside*nside;
  if( ipix<0 || ipix>npix-1 ) {
    char buff[64];
    sprintf(buff,"HEALPix::pix2ang_nest ipix(=%d) out of range ", ipix);
    throw RangeCheckError(PExcLongMessage(buff));
  }
  
  fn = 1.*nside;
  fact1 = 1./(3.*fn*fn);
  fact2 = 2./(3.*fn);
  nl4   = 4*nside;

  //c     finds the face, and the number in the face
  npface = nside*nside;
  
  face_num = ipix/npface;//  ! face number in {0,11}
  ipf = ipix%npface; //RzModFloor (int)fmod(ipix,npface);  ! pixel number in the face {0,npface-1}
  
  //c     finds the x,y on the face (starting from the lowest corner)
  //c     from the pixel number
  ip_low = ipf%1024; //RzModFloor  (int)fmod(ipf,1024);      ! content of the last 10 bits
  ip_trunc =   ipf/1024 ;//       ! truncation of the last 10 bits
  ip_med = ip_trunc%1024;  //RzModFloor  (int)fmod(ip_trunc,1024);  ! content of the next 10 bits
  ip_hi  =     ip_trunc/1024   ;//! content of the high weight 10 bits
  
  ix = 1024*PXY.pix2x_(ip_hi)+32*PXY.pix2x_(ip_med)+PXY.pix2x_(ip_low);
  iy = 1024*PXY.pix2y_(ip_hi)+32*PXY.pix2y_(ip_med)+PXY.pix2y_(ip_low);
  
  //c     transforms this in (horizontal, vertical) coordinates
  jrt = ix + iy;//  ! 'vertical' in {0,2*(nside-1)}
  jpt = ix - iy;//  ! 'horizontal' in {-nside+1,nside-1}
  
  //c     computes the z coordinate on the sphere
  //      jr =  jrll[face_num+1]*nside - jrt - 1;//   ! ring number in {1,4*nside-1}
  jr =  jrll[face_num]*nside - jrt - 1;
  nr = nside;//                  ! equatorial region (the most frequent)
  z  = (2*nside-jr)*fact2;
  kshift = (jr-nside) % 2; //RzModFloor (int)fmod(jr - nside, 2);
  if( jr<nside ) { //then     ! north pole region
    nr = jr;
    z = 1. - nr*nr*fact1;
    kshift = 0;
  }
  else {
    if( jr>3*nside ) {// then ! south pole region
      nr = nl4 - jr;
      z = - 1. + nr*nr*fact1;
      kshift = 0;
    }
  }
  theta = acos(z);
  
  //c     computes the phi coordinate on the sphere, in [0,2Pi]
  //      jp = (jpll[face_num+1]*nr + jpt + 1 + kshift)/2;//  ! 'phi' number in the ring in {1,4*nr}
  jp = (jpll[face_num]*nr + jpt + 1 + kshift)/2;
  if( jp>nl4 ) jp = jp - nl4;
  if( jp<1 )   jp = jp + nl4;
  phi = (jp - (kshift+1)*0.5) * (piover2 / nr);
}

 
/*! 
  \brief return the size index value corresponding to resolution \b res  
  Computes the size index m providing a resolution better or equal the 
  specified value \b res (in radian)

  - m : size-index 
  - number of pixels for 4Pi : 12*m*m

  \sa SOPHYA::Angle
*/
int_4 HEALPix::ResolToSizeIndex(double res)
{
  int_4 m = 1;
  double cres = sqrt(M_PI/3.); // 4PI/12
  while (cres > 1.001*res) {
    m *= 2;
    cres = sqrt(4.*M_PI/(12*m*m));
  }
  return m;
}  

//! return the pixel resolution for a given size index (nside)
double HEALPix::SizeIndexToResol(int_4 m)
{
  double res = 4.*M_PI/(double)(12*m*m);
  return sqrt(res);
}  
