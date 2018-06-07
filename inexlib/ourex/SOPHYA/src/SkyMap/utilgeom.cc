#include "sopnamsp.h"
#include "machdefs.h"
#include <math.h>
#include <iostream>
#include "utilgeom.h"
//++
// Titre	Some utilitary functions for geometry (utilgeom.h)...
//--
//++
int sign(double d) 
//
//--
{
  return (d >= 0) - (d < 0);
}
//++
double absd(double d) 
//
//--
{
  return sqrt(d*d);
}
//++
double mod(double d, double periode) 
//
//--
{
  if( d >= 0 ) return d-floor(d/periode)*periode;
  else return d-ceil(d/periode)*periode+periode;
}
//++
void swap(double& d1, double& d2) 
//
//--
{
  double temp;
  temp=d2;
  d2=d1;
  d1=temp;
}
//++
double min(double d1, double d2) 
//
//--
{
  if( d1 >= d2 ) return d2;
  else return d1;
}
//++
double max(double d1, double d2) 
//
//--
{
  return -min(-d1,-d2);
}
//++
int arrondi(double d) 
//
//--
{
  return (int)(((d-floor(d)) >= 0.5)*ceil(d)+((d-floor(d)) < 0.5)*floor(d));
}
//++
long rangijd(int nc, int i, int j, int d) 
//
//--
{
  if( i < j ) return 2*i*(2*nc-i-1)+4*(j-i-1)+d;
  if( i > j ) return 2*j*(2*nc-j-1)+4*(i-j-1)+d+2;
  if( i == j ) 
    {
      cout << "pas d'intersection entre le cercle " << i 
	   << " et le cercle " << j << "." << endl;
      return -1;
    }
  else return -1;
}
//++
long rangdiff(int nc, int i, int j, int d) 
//
//--
{
  if( i == j ) 
    {
      cout << "diff n'est pas defini entre le cercle " << i 
	   << " et le cercle " << j << "." << endl;
      return 0;
    }
  int indm=(int)min(i,j);
  int indM=(int)max(i,j);
  return indm*(2*nc-indm-1)+2*(indM-indm-1)+d;
}
//++
long rangik(int NtotEch, int i, int k)
//
//--
{
  return NtotEch*i+k;
}
//++
long ranghk(int NtotEch, int h, int k) 
//
//--
{
  return NtotEch*h+k;
}
//++
double scangle(double sinus, double cosinus) 
//
//--
{	// Modifie par D. Y. pour eviter des plantages aux petis angles
	double eps=1.e-10;
	if( fabs(1.-sinus*sinus-cosinus*cosinus) > eps) 
    {	   // On renormalise D.Y.
	  // cerr << "angle non valide: "<< "sinus = " << sinus << " cosinus = " << cosinus << '\n'; // BUGGG
      double norm= cosinus*cosinus+sinus*sinus;
      cosinus=cosinus/norm;
      sinus=sinus/norm;     
    }
  if( fabs(cosinus) >1. ) cosinus=1.*copysign(1.,cosinus);
  if( fabs(sinus) >1. ) sinus=1.*copysign(1.,sinus);
/*  if( fabs(sinus) == 0. ) sinus=0.;
    if( fabs(cosinus) == 0. ) cosinus=0.;
*/
  return acos(cosinus)*copysign(1,sinus);
}

