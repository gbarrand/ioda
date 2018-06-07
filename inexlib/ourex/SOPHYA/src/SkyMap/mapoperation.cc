// This may look like C code, but it is really -*- C++ -*-
//--------------------------------------------------------------------------
// File and Version Information:
//        Simple Operations on Maps 
// Description:
//
// History (add to end):
//      Sophie   April, 2000  - creation
//        code from Reza
//------------------------------------------------------------------------
#include "sopnamsp.h"
#include "machdefs.h"
#include <math.h>
#include "mapoperation.h"
/*!
 * \class SOPHYA::MapOperations
 * This class is for simple operations on maps
 */

template <class T>
void MapOperations<T>::sphTosph(PixelMap<T>& in, PixelMap<T>& out)
  // Cette fonction remplit la sphere out a partir de la sphere in
  // Les spheres peuvent etre de type et de pixelisations differentes
{
  int kin,kout;
  double teta,phi;
  
  int* cnt = new int[out.NbPixels()+1];
  for(kout=0; kout<out.NbPixels(); kout++)
    { cnt[kout] = 0; out(kout) = 0.; }
  
  for(kin=0; kin<in.NbPixels(); kin++) {
    in.PixThetaPhi(kin, teta, phi);
    kout = out.PixIndexSph(teta, phi);
    if (kout < 0) continue;
    out(kout) += in(kin);
    cnt[kout] ++;
  }
  
  double moy, sig, dcn;
  moy = 0.; sig = 0.;
  for(kout=0; kout<out.NbPixels(); kout++) {
    dcn = cnt[kout];  moy += dcn;	 sig += (dcn*dcn);
    if (cnt[kout] > 0)  out(kout) /= dcn;
    else {
      out.PixThetaPhi(kout, teta, phi);
      int pixel = in.PixIndexSph(teta,phi);
      out(kout) = in.PixVal(pixel);
    }
  }
  
  moy /= out.NbPixels();
  sig = sig/out.NbPixels() - moy*moy;
  if (sig >= 0.)	sig = sqrt(sig);
  
  delete[] cnt;
  
}

template <class T>
void  MapOperations<T>::meanSig(NDataBlock<T> const & dbl, double& gmoy, double& gsig)
{
  gmoy=0.;
  gsig = 0.; 
  double valok;
  for(int k=0; k<(int)dbl.Size(); k++) {
    valok = dbl(k);
    gmoy += valok;  gsig += valok*valok; 
  }
  gmoy /= (double)dbl.Size();
  gsig = gsig/(double)dbl.Size() - gmoy*gmoy;
  if (gsig >= 0.) gsig = sqrt(gsig);
}

///////////////////////////////////////////////////////////////
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template MapOperations<r_4>
#pragma define_template MapOperations<r_8>
#endif
  
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class SOPHYA::MapOperations<r_4>;
template class SOPHYA::MapOperations<r_8>;
#endif
   
