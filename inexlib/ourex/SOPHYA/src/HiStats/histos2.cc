#include "machdefs.h"
#include <string.h>
#include <new>
#include <stdlib.h>
#include <stdio.h>
#ifndef NO_VALUES_H
#include <values.h>
#endif

#include "histos2.h"

namespace SOPHYA {

/*!
  \class Histo2D
  \ingroup HiStats
  Classe d'histogrammes 2D
  \verbatim
    Remarque sur les indices:
     H(i,j)   -> i  = coord x (0<i<nx),     j  = coord y (0<j<ny)
     v(ii,jj) -> ii = ligne (0<i<NRows()),  jj = colonne (0<i<NCol())
     On fait une correspondance directe i<->ii et j<->jj
     ce qui, en representation classique des histos2D et des matrices
     entraine une inversion x<->y cad une symetrie / diagonale principale
     H(0,...)  represente ^         mais v(0,...) represente
                          |x.......               |xxxxxxxx|
                          |x.......               |........|
                          |x.......               |........|
                          |x.......               |........|
                          |x.......               |........|
                          --------->
                          colonne no 1            ligne no 1
  \endverbatim
*/

///////////////////////////////////////////////////////////////////

/*!
  Constructeur par defaut.
*/
Histo2D::Histo2D()
: mData(NULL), mErr2(NULL)
, nHist(0), nEntries(0)
, mNx(0), mNy(0), mNxy(0), mXmin(0), mXmax(0), mYmin(0), mYmax(0), mWBinx(0), mWBiny(0)
, mHprojx(NULL), mHprojy(NULL)
{
 for(int_4 i=0;i<3;i++) for(int_4 j=0;j<3;j++) mOver[i][j]=0.;
 mB_s.H = NULL;
}

/*!
  Createur d'un histogramme 2D ayant nxBin,nyBin bins
  entre xMin,xMax et yMin,yMax.
*/
Histo2D::Histo2D(r_8 xMin,r_8 xMax,int_4 nxBin,r_8 yMin,r_8 yMax,int_4 nyBin)
: mData(NULL), mErr2(NULL), mHprojx(NULL), mHprojy(NULL)
{
  CreateOrResize(xMin,xMax,nxBin,yMin,yMax,nyBin);
}

/*!
  Createur d'un histogramme 2D ayant nxBin,nyBin bins
  entre xMin,xMax et yMin,yMax.
*/
Histo2D::Histo2D(r_4 xMin,r_4 xMax,int_4 nxBin,r_4 yMin,r_4 yMax,int_4 nyBin)
: mData(NULL), mErr2(NULL), mHprojx(NULL), mHprojy(NULL)
{
  CreateOrResize((r_8)xMin,(r_8)xMax,nxBin,(r_8)yMin,(r_8)yMax,nyBin);
}

/*!
  Constructeur par copie.
*/
Histo2D::Histo2D(const Histo2D& h)
{
int_4 i,j;
mData = new r_8[h.mNxy];
memcpy(mData, h.mData, h.mNxy*sizeof(r_8));

mErr2 = NULL;
if(h.mErr2) {
  mErr2 = new r_8[h.mNxy];
  memcpy(mErr2, h.mErr2, h.mNxy*sizeof(r_8));
}

nHist = h.nHist; nEntries = h.nEntries;
for(i=0;i<3;i++) for(j=0;j<3;j++) mOver[i][j]=h.mOver[i][j];
mNx = h.mNx;  mNy = h.mNy;  mNxy = h.mNxy;
mXmin = h.mXmin; mXmax = h.mXmax; mYmin = h.mYmin; mYmax = h.mYmax;
mWBinx = h.mWBinx; mWBiny = h.mWBiny;
mB_s.H = NULL;

mHprojx = mHprojy = NULL;
if(h.mHprojx) {SetProjX(); *mHprojx = *(h.mHprojx);}
if(h.mHprojy) {SetProjY(); *mHprojy = *(h.mHprojy);}

int_4 nb; r_8 min,max;
nb = h.NSliX();
if(nb>0) {SetSliX(nb); for(i=0;i<NSliX();i++) *HSliX(i)=*(h.HSliX(i));}
nb = h.NSliY();
if(nb>0) {SetSliY(nb); for(i=0;i<NSliY();i++) *HSliY(i)=*(h.HSliY(i));}

nb = h.NBandX();
if(nb>0) {
  for(i=0;i<nb;i++)
    {h.GetBandX(i,min,max); SetBandX(min,max); *HBandX(i)=*(h.HBandX(i));}
  for(i=0;i<NBandX();i++) *HBandX(i) = *(h.HBandX(i));
}
nb = h.NBandY();
if(nb>0) {
  for(i=0;i<nb;i++)
    {h.GetBandY(i,min,max); SetBandY(min,max); *HBandY(i)=*(h.HBandY(i));}
  for(i=0; i<NBandY();i++) *HBandY(i) = *(h.HBandY(i));
}

}

/*!
  Destructeur.
*/
Histo2D::~Histo2D()
{
  Delete();
}

///////////////////////////////////////////////////////////////////
/*!
  allocation de la place de l'histogramme (fct privee).
*/
void Histo2D::CreateOrResize(r_8 xMin,r_8 xMax,int_4 nxBin,r_8 yMin,r_8 yMax,int_4 nyBin)
{
  int_8 n = nxBin*nyBin;
  bool samelen = (n==mNx*mNy)? true: false;
  if(mData!=NULL && !samelen) {delete[] mData; mData=NULL;}
  if(mErr2!=NULL) {delete[] mErr2; mErr2=NULL;} // On des-alloue toujours
  if(n>0 && mData==NULL) mData = new r_8[n];
  if(mData) memset(mData, 0, n*sizeof(r_8));
  mNx = nxBin; mNy = nyBin; mNxy = nxBin*nyBin;
  mXmin = xMin; mXmax = xMax; mYmin = yMin; mYmax = yMax;
  mWBinx = (nxBin>0) ? (xMax - xMin)/nxBin: 0.;
  mWBiny = (nyBin>0) ? (yMax - yMin)/nyBin: 0.;
  nHist = 0; nEntries = 0;
  for(int_4 i=0;i<3;i++) for(int_4 j=0;j<3;j++) mOver[i][j]=0.;
  mB_s.H = NULL;

  DelProjX(); DelProjY();
  DelBandX(); DelBandY();
  DelSliX(); DelSliY();
}

/*!
  Desallocation de la place de l'histogramme (fct privee).
*/
void Histo2D::Delete()
{
  if( mData != NULL ) { delete[] mData; mData = NULL;}
  if( mErr2 != NULL ) { delete[] mErr2; mErr2 = NULL;}

  DelProj();
  DelBandX(); DelBandY();
  DelSliX();  DelSliY();

  nHist = 0;
  nEntries = 0;
  mNx = 0; mNy = 0; mNxy = 0;
  mXmin = 0; mXmax = 0; mYmin = 0; mYmax = 0;
  mWBinx = 0; mWBiny = 0;
  for(int_4 i=0;i<3;i++) for(int_4 j=0;j<3;j++) mOver[i][j]=0.;
  mB_s.H = NULL;  // Ne pas faire de delete!
}

///////////////////////////////////////////////////////////////////
/*!
  Remise a zero du contenu, des erreurs et des valeurs.
*/
void Histo2D::Zero()
{
  nHist = nEntries = 0;
  for(int_4 i=0;i<3;i++) for(int_4 j=0;j<3;j++) mOver[i][j]=0.;
  memset(mData, 0, mNxy*sizeof(r_8));
  memset(mOver, 0, 9*sizeof(r_8));

  if( mErr2 != NULL ) memset(mErr2, 0, mNxy*sizeof(r_8));

  ZeroProj();

  ZeroBandX(); ZeroBandY();

  ZeroSliX(); ZeroSliY();
}

///////////////////////////////////////////////////////////////////
/*!
  Pour avoir le calcul des erreurs.
*/
void Histo2D::Errors()
{
 if(mErr2!=NULL) {delete[] mErr2; mErr2 = NULL;}
 if(mNxy <= 0) return;
 mErr2 = new r_8[mNxy];
 memset(mErr2,0,mNxy*sizeof(r_8));
}

///////////////////////////////////////////////////////////////////
/*!
 Recompute XMin (YMin) and XMax (YMax) so that
 the CENTER of the first bin is exactly XMin (YMin) and
 the CENTER of the last bin is exactly XMax (YMax).
 Remember that otherwise
 XMin (YMin) is the beginning of the first bin
 and XMax (YMax) is the end of the last bin
*/
void Histo2D::ReCenterBinX(void)
{
 if(mNx<=1) return;
 double dx = (mXmax-mXmin)/(mNx-1);
 mXmin -= dx/2.;
 mXmax += dx/2.;
 mWBinx = (mXmax-mXmin)/mNx;
}

void Histo2D::ReCenterBinY(void)
{
 if(mNy<=1) return;
 double dy = (mYmax-mYmin)/(mNy-1);
 mYmin -= dy/2.;
 mYmax += dy/2.;
 mWBiny = (mYmax-mYmin)/mNy;
}

void Histo2D::ReCenterBin(void)
{
 ReCenterBinX();
 ReCenterBinY();
}


///////////////////////////////////////////////////////////////////
/*!
  Operateur H2 = H1
*/
Histo2D& Histo2D::operator = (const Histo2D& h)
{
  if(this == &h) return *this;
  CreateOrResize(h.mXmin,h.mXmax,h.mNx,h.mYmin,h.mYmax,h.mNy);
  if(h.mErr2) Errors();
  if(mData) memcpy(mData, h.mData, mNxy*sizeof(r_8));
  if(mErr2) memcpy(mErr2, h.mErr2, mNxy*sizeof(r_8));

  for(int i=0;i<3;i++) for(int j=0;j<3;j++) mOver[i][j] = h.mOver[i][j];
  nHist = h.nHist;  nEntries = h.nEntries;  

  if(h.mHprojx) {SetProjX(); *mHprojx = *(h.mHprojx);}
  if(h.mHprojy) {SetProjY(); *mHprojy = *(h.mHprojy);}

  int_4 nb;
  nb = h.NSliX();
  if(nb>0) {
    SetSliX(nb);
    for(int i=0; i<NSliX();i++) *HSliX(i) = *(h.HSliX(i));
  }
  nb = h.NSliY();
  if(nb>0) {
    SetSliY(nb);
    for(int i=0; i<NSliY();i++) *HSliY(i) = *(h.HSliY(i));
  }

  nb = h.NBandX();
  if(nb>0) {
    for(int i=0; i<nb;i++) {
      r_8 min,max;
      h.GetBandX(i,min,max);
      SetBandX(min,max);
      *HBandX(i) = *(h.HBandX(i));
    }
    for(int i=0; i<NBandX();i++) *HBandX(i) = *(h.HBandX(i));
  }
  nb = h.NBandY();
  if(nb>0) {
    for(int i=0; i<nb;i++) {
      r_8 min,max;
      h.GetBandY(i,min,max);
      SetBandY(min,max);
      *HBandY(i) = *(h.HBandY(i));
    }
    for(int i=0; i<NBandY();i++) *HBandY(i) = *(h.HBandY(i));
  }

  return *this;
}

///////////////////////////////////////////////////////////////////
/*!
  Operateur H *= b
*/
Histo2D& Histo2D::operator *= (r_8 b)
{
int_4 i,j;
r_8 b2 = b*b;
for(i=0;i<mNxy;i++) {
  mData[i] *= b;
  if(mErr2) mErr2[i] *= b2;
}
for(i=0;i<3;i++) for(j=0;j<3;j++) mOver[i][j] *= b;
nHist *= b;

if(mHprojx) *mHprojx *= b;
if(mHprojy) *mHprojy *= b;
if(NSliX()>0) for(i=0; i<NSliX();i++) *HSliX(i) *= b;
if(NSliY()>0) for(i=0; i<NSliY();i++) *HSliY(i) *= b;
if(NBandX()>0) for(i=0; i<NBandX();i++) *HBandX(i) *= b;
if(NBandY()>0) for(i=0; i<NBandY();i++) *HBandY(i) *= b;

return *this;
}

/*!
  Operateur H /= b
*/
Histo2D& Histo2D::operator /= (r_8 b)
{
int_4 i,j;
if (b==0.) throw ParmError("Histo2D::operator / (0) ");
r_8 b2 = b*b;
for(i=0;i<mNxy;i++) {
  mData[i] /= b;
  if(mErr2) mErr2[i] /= b2;
}
for(i=0;i<3;i++) for(j=0;j<3;j++) mOver[i][j] /= b;
nHist /= b;

if(mHprojx) *mHprojx /= b;
if(mHprojy) *mHprojy /= b;
if(NSliX()>0) for(i=0; i<NSliX();i++) *HSliX(i) /= b;
if(NSliY()>0) for(i=0; i<NSliY();i++) *HSliY(i) /= b;
if(NBandX()>0) for(i=0; i<NBandX();i++) *HBandX(i) /= b;
if(NBandY()>0) for(i=0; i<NBandY();i++) *HBandY(i) /= b;

return *this;
}

/*!
  Operateur H += b
*/
Histo2D& Histo2D::operator += (r_8 b)
{
int_4 i,j;
r_8 min,max;
for(i=0;i<mNxy;i++) mData[i] += b;
for(i=0;i<3;i++) for(j=0;j<3;j++) mOver[i][j] += b;
nHist += mNxy*b;

if(mHprojx) *mHprojx += b*mNy;
if(mHprojy) *mHprojy += b*mNx;
if(NSliX()>0) for(i=0; i<NSliX();i++) *HSliX(i) += b*mNy/NSliX();
if(NSliY()>0) for(i=0; i<NSliY();i++) *HSliY(i) += b*mNx/NSliY();
if(NBandX()>0) for(i=0; i<NBandX();i++) {
  GetBandX(i,min,max);
  *HBandX(i) += b*(max-min)/(mYmax-mYmin)*mNy;
}
if(NBandY()>0) for(i=0; i<NBandY();i++) {
  GetBandY(i,min,max);
  *HBandY(i) += b*(max-min)/(mXmax-mXmin)*mNx;
}

return *this;
}

/*!
  Operateur H -= b
*/
Histo2D& Histo2D::operator -= (r_8 b)
{
int_4 i,j;
r_8 min,max;
for(i=0;i<mNxy;i++) mData[i] -= b;
for(i=0;i<3;i++) for(j=0;j<3;j++) mOver[i][j] -= b;
nHist -= mNxy*b;

if(mHprojx) *mHprojx -= b*mNy;
if(mHprojy) *mHprojy -= b*mNx;
if(NSliX()>0) for(i=0; i<NSliX();i++) *HSliX(i) -= b*mNy/NSliX();
if(NSliY()>0) for(i=0; i<NSliY();i++) *HSliY(i) -= b*mNx/NSliY();
if(NBandX()>0) for(i=0; i<NBandX();i++) {
  GetBandX(i,min,max);
  *HBandX(i) -= b*(max-min)/(mYmax-mYmin)*mNy;
}
if(NBandY()>0) for(i=0; i<NBandY();i++) {
  GetBandY(i,min,max);
  *HBandY(i) -= b*(max-min)/(mXmax-mXmin)*mNx;
}

return *this;
}

///////////////////////////////////////////////////////////////////
/*!
  Operateur H += H1
*/
Histo2D& Histo2D::operator += (const Histo2D& a)
{
int_4 i,j;
if(mNx!=a.mNx || mNy!=a.mNy) throw SzMismatchError("Histo2D::operator += ");
for(i=0;i<mNxy;i++) {
  mData[i] += a.mData[i];
  if(mErr2 && a.mErr2) mErr2[i] += a.mErr2[i];
}
for(i=0;i<3;i++) for(j=0;j<3;j++) mOver[i][j] += a.mOver[i][j];
nHist += a.nHist;
nEntries += a.nEntries;

if(mHprojx && a.mHprojx) *mHprojx += *(a.mHprojx);
if(mHprojy && a.mHprojy) *mHprojy += *(a.mHprojy);
ZeroSliX();  ZeroSliY();
ZeroBandX(); ZeroBandY();

return *this;
}

/*!
  Operateur H -= H1
*/
Histo2D& Histo2D::operator -= (const Histo2D& a)
{
int_4 i,j;
if(mNx!=a.mNx || mNy!=a.mNy) throw SzMismatchError("Histo2D::operator -= ");
for(i=0;i<mNxy;i++) {
  mData[i] -= a.mData[i];
  if(mErr2 && a.mErr2) mErr2[i] += a.mErr2[i];
}
for(i=0;i<3;i++) for(j=0;j<3;j++) mOver[i][j] += a.mOver[i][j];
nHist -= a.nHist;
nEntries += a.nEntries;

if(mHprojx && a.mHprojx) *mHprojx -= *(a.mHprojx);
if(mHprojy && a.mHprojy) *mHprojy -= *(a.mHprojy);
ZeroSliX();  ZeroSliY();
ZeroBandX(); ZeroBandY();

return *this;
}

/*!
  Operateur H *= H1
*/
Histo2D& Histo2D::operator *= (const Histo2D& a)
{
int_4 i,j;
if(mNx!=a.mNx || mNy!=a.mNy) throw SzMismatchError("Histo2D::operator *= ");
nHist = 0.;
for(i=0;i<mNxy;i++) {
  if(mErr2 && a.mErr2)
      mErr2[i] = a.mData[i]*a.mData[i]*mErr2[i] + mData[i]*mData[i]*a.mErr2[i];
  mData[i] *= a.mData[i];
  nHist += mData[i];
}
for(i=0;i<3;i++) for(j=0;j<3;j++) mOver[i][j] *= a.mOver[i][j];
nEntries += a.nEntries;

if(mHprojx && a.mHprojx) *mHprojx *= *(a.mHprojx);
if(mHprojy && a.mHprojy) *mHprojy *= *(a.mHprojy);
ZeroSliX();  ZeroSliY();
ZeroBandX(); ZeroBandY();

return *this;
}

/*!
  Operateur H /= H1
*/
Histo2D& Histo2D::operator /= (const Histo2D& a)
{
int_4 i,j;
if(mNx!=a.mNx || mNy!=a.mNy) throw SzMismatchError("Histo2D::operator /= ");
nHist = 0.;
for(i=0;i<mNxy;i++) {
  if(a.mData[i]==0.) {
    mData[i]=0.;
    if(mErr2) mErr2[i]=0.;
    continue;
  }
  if(mErr2 && a.mErr2)
      mErr2[i] = (mErr2[i] + mData[i]/a.mData[i]*mData[i]/a.mData[i]*a.mErr2[i])
                /(a.mData[i]*a.mData[i]);
  mData[i] /= a.mData[i];
  nHist += mData[i];
}
for(i=0;i<3;i++) for(j=0;j<3;j++) 
  if(a.mOver[i][j]!=0.) mOver[i][j] *= a.mOver[i][j]; else mOver[i][j] = 0.;
nEntries += a.nEntries;

if(mHprojx && a.mHprojx) *mHprojx /= *(a.mHprojx);
if(mHprojy && a.mHprojy) *mHprojy /= *(a.mHprojy);
ZeroSliX();  ZeroSliY();
ZeroBandX(); ZeroBandY();

return *this;
}

///////////////////////////////////////////////////////////////////
/*!
  Remplissage d'un tableau avec les valeurs des abscisses.
*/
void Histo2D::GetXCoor(TVector<r_8> &v) const
{
r_8 x,y;
v.Realloc(mNx);
for(int_4 i=0;i<mNx;i++) {BinLowEdge(i,0,x,y); v(i) = x;}
return;
}

/*!
  Remplissage d'un tableau avec les valeurs des ordonnees.
*/
void Histo2D::GetYCoor(TVector<r_8> &v) const
{
r_8 x,y;
v.Realloc(mNy);
for(int_4 i=0;i<mNy;i++) {BinLowEdge(0,i,x,y); v(i) = y;}
return;
}

/*!
  Remplissage d'un tableau avec les valeurs du contenu.
*/
void Histo2D::GetValue(TMatrix<r_8> &v) const
{
v.Realloc(mNx,mNy);
for(int_4 i=0;i<mNx;i++)
  for(int_4 j=0;j<mNy;j++) v(i,j) = (*this)(i,j);
return;
}

/*!
  Remplissage d'un tableau avec les valeurs du carre des erreurs.
*/
void Histo2D::GetError2(TMatrix<r_8> &v) const
{
int_4 i,j;
v.Realloc(mNx,mNy);
if(!mErr2)
  {for(i=0;i<mNx;i++) for(j=0;j<mNy;j++) v(i,j) = 0.; return;}
for(i=0;i<mNx;i++) for(j=0;j<mNy;j++) v(i,j) = Error2(i,j);
return;
}

/*!
  Remplissage d'un tableau avec les valeurs des erreurs.
*/
void Histo2D::GetError(TMatrix<r_8> &v) const
{
int_4 i,j;
v.Realloc(mNx,mNy);
if(!mErr2)
  {for(i=0;i<mNx;i++) for(j=0;j<mNy;j++) v(i,j) = 0.; return;}
for(i=0;i<mNx;i++) for(j=0;j<mNy;j++) v(i,j) = Error(i,j);
return;
}

///////////////////////////////////////////////////////////////////
/*!
  Remplissage du contenu de l'histo avec les valeurs d'un tableau.
*/
void Histo2D::PutValue(TMatrix<r_8> &v, int_4 ierr)
{
//if(v.NRows()!=(uint_4)mNx || v.NCol()!=(uint_4)mNy) throw SzMismatchError("Histo2D::PutValue()");
uint_4 nnx = (v.NRows()<(uint_4)mNx)? v.NRows(): (uint_4)mNx;
uint_4 nny = (v.NCol() <(uint_4)mNy)? v.NCol() : (uint_4)mNy;
if(nnx>0 && nny>0) for(uint_4 i=0;i<nnx;i++) for(uint_4 j=0;j<nny;j++) {
  (*this)(i,j) = v(i,j);
  if(mErr2 && ierr) Error2(i,j) = fabs(v(i,j));
}
return;
}

/*!
  Addition du contenu de l'histo avec les valeurs d'un tableau.
*/
void Histo2D::PutValueAdd(TMatrix<r_8> &v, int_4 ierr)
{
//if(v.NRows()!=(uint_4)mNx || v.NCol()!=(uint_4)mNy) throw SzMismatchError("Histo2D::PutValueAdd ");
uint_4 nnx = (v.NRows()<(uint_4)mNx)? v.NRows(): (uint_4)mNx;
uint_4 nny = (v.NCol() <(uint_4)mNy)? v.NCol() : (uint_4)mNy;
if(nnx>0 && nny>0) for(uint_4 i=0;i<nnx;i++) for(uint_4 j=0;j<nny;j++) {
  (*this)(i,j) += v(i,j);
  if(mErr2 && ierr) Error2(i,j) += fabs(v(i,j));
}
return;
}

/*!
  Remplissage des erreurs au carre de l'histo
  avec les valeurs d'un tableau.
*/
void Histo2D::PutError2(TMatrix<r_8> &v)
{
//if(v.NRows()!=(uint_4)mNx || v.NCol()!=(uint_4)mNy) throw SzMismatchError("Histo2D::PutError2 ");
uint_4 nnx = (v.NRows()<(uint_4)mNx)? v.NRows(): (uint_4)mNx;
uint_4 nny = (v.NCol() <(uint_4)mNy)? v.NCol() : (uint_4)mNy;
if(nnx>0 && nny>0) {
  if(!mErr2) Errors();
  for(uint_4 i=0;i<nnx;i++) for(uint_4 j=0;j<nny;j++) Error2(i,j) = v(i,j);
}
return;
}

/*!
  Addition des erreurs au carre de l'histo
  avec les valeurs d'un tableau.
*/
void Histo2D::PutError2Add(TMatrix<r_8> &v)
{
//if(v.NRows()!=(uint_4)mNx || v.NCol()!=(uint_4)mNy) throw SzMismatchError("Histo2D::PutError2Add ");
uint_4 nnx = (v.NRows()<(uint_4)mNx)? v.NRows(): (uint_4)mNx;
uint_4 nny = (v.NCol() <(uint_4)mNy)? v.NCol() : (uint_4)mNy;
if(nnx>0 && nny>0) {
  if(!mErr2) Errors();
  for(uint_4 i=0;i<nnx;i++) for(uint_4 j=0;j<nny;j++)
          if(v(i,j)>0.) Error2(i,j) += v(i,j);
}
return;
}

/*!
  Remplissage des erreurs de l'histo avec les valeurs d'un tableau.
*/
void Histo2D::PutError(TMatrix<r_8> &v)
{
//if(v.NRows()!=(uint_4)mNx || v.NCol()!=(uint_4)mNy) throw SzMismatchError("Histo2D::PutError ");
uint_4 nnx = (v.NRows()<(uint_4)mNx)? v.NRows(): (uint_4)mNx;
uint_4 nny = (v.NCol() <(uint_4)mNy)? v.NCol() : (uint_4)mNy;
if(nnx>0 && nny>0) {
  if(!mErr2) Errors();
  for(uint_4 i=0;i<nnx;i++) for(uint_4 j=0;j<nny;j++)
    if(v(i,j)>0.) Error2(i,j)=v(i,j)*v(i,j); else Error2(i,j)= -v(i,j)*v(i,j);
}
return;
}

///////////////////////////////////////////////////////////////////
/********* Methode *********/
/*!
  Addition du contenu de l'histo pour x,y poids w.
*/
void Histo2D::Add(r_8 x, r_8 y, r_8 w)
{
list<bande_slice>::iterator it;
int_4 i,j;
FindBin(x,y,i,j);

if( mHprojx != NULL ) mHprojx->Add(x,w);
if( mHprojy != NULL ) mHprojy->Add(y,w);

if(mLBandx.size()>0)
  for( it = mLBandx.begin(); it != mLBandx.end(); it++)
    if( (*it).min <= y && y < (*it).max ) (*it).H->Add(x,w);

if(mLBandy.size()>0)
  for( it = mLBandy.begin(); it != mLBandy.end(); it++)
    if( (*it).min <= x && x < (*it).max ) (*it).H->Add(y,w);

if(mLSlix.size()>0)
  for( it = mLSlix.begin(); it != mLSlix.end(); it++)
    if( (*it).min <= y && y < (*it).max ) (*it).H->Add(x,w);

if(mLSliy.size()>0)
  for( it = mLSliy.begin(); it != mLSliy.end(); it++)
    if( (*it).min <= x && x < (*it).max ) (*it).H->Add(y,w);

if( i<0 || i>=mNx || j<0 || j>=mNy ) {
  if(i<0) i=0; else if(i>=mNx) i=2; else i=1;
  if(j<0) j=0; else if(j>=mNy) j=2; else j=1;
  mOver[i][j] += w;
  mOver[1][1] += w;
  return;
}

mData[j*mNx+i] += w;
if(mErr2!=NULL) mErr2[j*mNx+i] += w*w;
nHist += w;
nEntries++;
}

///////////////////////////////////////////////////////////////////
/*!
  Recherche du bin du maximum dans le pave [il,ih][jl,jh].
*/
void Histo2D::IJMax(int_4& imax,int_4& jmax,int_4 il,int_4 ih,int_4 jl,int_4 jh) const
{
if(mNxy<=0) {imax = jmax = -1; return;}
if( il > ih ) { il = 0; ih = mNx-1; }
if( jl > jh ) { jl = 0; jh = mNy-1; }
if( il < 0 ) il = 0;
if( jl < 0 ) jl = 0;
if( ih >= mNx ) ih = mNx-1;
if( jh >= mNy ) jh = mNy-1;

imax = jmax = 0;
if(mNxy==1) return;

r_8 mx=(*this)(il,jl);
for (int_4 i=il; i<=ih; i++)
   for (int_4 j=jl; j<=jh; j++)
      if ((*this)(i,j)>mx) {imax = i; jmax = j; mx=(*this)(i,j);}
}

/*!
  Recherche du bin du minimum dans le pave [il,ih][jl,jh].
*/
void Histo2D::IJMin(int_4& imax,int_4& jmax,int_4 il,int_4 ih,int_4 jl,int_4 jh) const
{
if(mNxy<=0) {imax = jmax = -1; return;}
if( il > ih ) { il = 0; ih = mNx-1; }
if( jl > jh ) { jl = 0; jh = mNy-1; }
if( il < 0 ) il = 0;
if( jl < 0 ) jl = 0;
if( ih >= mNx ) ih = mNx-1;
if( jh >= mNy ) jh = mNy-1;

imax = jmax = 0;
if(mNxy==1) return;

r_8 mx=(*this)(il,jl);
for (int_4 i=il; i<=ih; i++)
   for (int_4 j=jl; j<=jh; j++)
      if ((*this)(i,j)<mx) {imax = i; jmax = j; mx=(*this)(i,j);}
}


/*!
  Recherche du maximum dans le pave [il,ih][jl,jh].
*/
r_8 Histo2D::VMax(int_4 il,int_4 ih,int_4 jl,int_4 jh) const
{
if(mNxy<=0) return 0.;
if( il > ih ) { il = 0; ih = mNx-1; }
if( jl > jh ) { jl = 0; jh = mNy-1; }
if( il < 0 ) il = 0;
if( jl < 0 ) jl = 0;
if( ih >= mNx ) ih = mNx-1;
if( jh >= mNy ) jh = mNy-1;

r_8 mx=(*this)(il,jl);
if(mNxy==1) return mx;
for (int_4 i=il; i<=ih; i++)
   for (int_4 j=jl; j<=jh; j++)
      if ((*this)(i,j)>mx) mx=(*this)(i,j);
return mx;
}

/*!
  Recherche du minimum dans le pave [il,ih][jl,jh].
*/
r_8 Histo2D::VMin(int_4 il,int_4 ih,int_4 jl,int_4 jh) const
{
if(mNxy<=0) return 0.;
if( il > ih ) { il = 0; ih = mNx-1; }
if( jl > jh ) { jl = 0; jh = mNy-1; }
if( il < 0 ) il = 0;
if( jl < 0 ) jl = 0;
if( ih >= mNx ) ih = mNx-1;
if( jh >= mNy ) jh = mNy-1;

r_8 mx=(*this)(il,jl);
if(mNxy==1) return mx;
for (int_4 i=il; i<=ih; i++)
   for (int_4 j=jl; j<=jh; j++)
      if ((*this)(i,j)<mx) mx=(*this)(i,j);
return mx;
}

///////////////////////////////////////////////////////////////////
/*!
  Renvoie les under.overflow dans les 8 quadrants.
  \verbatim
     mOver[3][3]:       20 | 21 | 22
                           |    |
                       --------------
                           |    |
                        10 | 11 | 12        11 = all overflow+underflow
                           |    |
                       --------------
                           |    |
                        00 | 01 | 02
  \endverbatim
*/
r_8 Histo2D::NOver(int_4 i,int_4 j) const
{
if( i < 0 || i>=3 || j < 0 || j>=3 ) return mOver[1][1];
return mOver[i][j];
}


///////////////////////////////////////////////////////////////////
/*!
  Retourne le nombre de bins non-nuls.
*/
int_4 Histo2D::BinNonNul() const
{
if(mNxy<=0) return -1;
int_4 non=0;
for (int_4 i=0;i<mNxy;i++) if( mData[i] != 0. ) non++;
return non;
}

/*!
  Retourne le nombre de bins avec erreurs non-nulles.
*/
int_4 Histo2D::ErrNonNul() const
{
if(mErr2==NULL) return -1;
int_4 non=0;
for (int_4 i=0;i<mNxy;i++) if( mErr2[i] != 0. ) non++;
return non;
}

///////////////////////////////////////////////////////////////////
/*!
  Idem EstimeMax(int...) mais retourne x,y.
*/
int_4 Histo2D::EstimeMax(r_8& xm,r_8& ym,int_4 SzPav
                      ,int_4 il,int_4 ih,int_4 jl,int_4 jh) const
{
int_4 im,jm;
IJMax(im,jm,il,ih,jl,jh);
return EstimeMax(im,jm,xm,ym,SzPav);
}

/*!
  Determine les abscisses et ordonnees du maximum donne par im,jm
  en moyennant dans un pave SzPav x SzPav autour du maximum.
  \verbatim
     Return:
       0 = si fit maximum reussi avec SzPav pixels
       1 = si fit maximum reussi avec moins que SzPav pixels
                                   dans au moins 1 direction
       2 = si fit maximum echoue et renvoit BinCenter()
      -1 = si echec: SzPav <= 0 ou im,jm hors limites
  \endverbatim
*/
int_4 Histo2D::EstimeMax(int_4 im,int_4 jm,r_8& xm,r_8& ym,int_4 SzPav) const
{
xm = ym = 0;
if( SzPav <= 0 ) return -1;
if( im < 0 || im >= mNx ) return -1;
if( jm < 0 || jm >= mNy ) return -1;

if( SzPav%2 == 0 ) SzPav++;
SzPav = (SzPav-1)/2;

int_4 rc = 0;
r_8 dxm = 0, dym = 0, wx = 0;
for(int_4 i=im-SzPav;i<=im+SzPav;i++) {
  if( i<0 || i>= mNx ) {rc=1; continue;}
  for(int_4 j=jm-SzPav;j<=jm+SzPav;j++) {
    if( j<0 || j>= mNy ) {rc=1; continue;}
    r_8 x,y;
    BinCenter(i,j,x,y);
    dxm += x * (*this)(i,j);
    dym += y * (*this)(i,j);
    wx  += (*this)(i,j);
  }
}

if( wx > 0. ) {
  xm = dxm/wx;
  ym = dym/wx;
  return rc;
} else {
  BinCenter(im,jm,xm,ym);
  return 2;
}

}

/*!
  Pour trouver le maximum de l'histogramme en tenant compte
  des fluctuations.
  \verbatim
    Methode:
    1-/ On recherche le bin maximum MAX de l'histogramme
    2-/ On considere que tous les pixels compris entre [MAX-Dz,MAX]
        peuvent etre des pixels maxima.
    3-/ On identifie le bin maximum en choissisant le pixel du 2-/
        tel que la somme des pixels dans un pave SzPav x SzPav soit maximale.
    INPUT:
     SzPav = taille du pave pour departager
     Dz    = tolerance pour identifier tous les pixels "maximum"
    OUTPUT:
     im,jm = pixel maximum trouve
    RETURN:
      <0 =  Echec
      >0 =  nombre de pixels possibles pour le maximum
  \endverbatim
*/
int_4 Histo2D::FindMax(int_4& im,int_4& jm,int_4 SzPav,r_8 Dz
                    ,int_4 il,int_4 ih,int_4 jl,int_4 jh) const
{
if(mNxy<=0) return -1;
if( il > ih ) { il = 0; ih = mNx-1; }
if( jl > jh ) { jl = 0; jh = mNy-1; }
if( il < 0 ) il = 0;
if( jl < 0 ) jl = 0;
if( ih >= mNx ) ih = mNx-1;
if( jh >= mNy ) jh = mNy-1;
if( SzPav < 0 ) SzPav = 0;
  else { if( SzPav%2 == 0 ) SzPav++; SzPav = (SzPav-1)/2;}
if( Dz < 0 ) Dz = 0.;
r_8 max = VMax(il,ih,jl,jh) - Dz;
int_4 nmax = 0;
r_8 sumx = -1.e20;
for(int_4 i=il;i<=ih;i++) for(int_4 j=jl;j<=jh;j++) {
  if( (*this)(i,j) < max) continue;
  nmax++;
  r_8 sum = 0.;
  for(int_4 ii=i-SzPav;ii<=i+SzPav;ii++) {
    if( ii<0 || ii >= mNx ) continue;
    for(int_4 jj=j-SzPav;jj<=j+SzPav;jj++) {
      if( jj<0 || jj >= mNy ) continue;
      sum += (*this)(ii,jj);
    }
  }
  if(nmax==1 || sum>sumx) {im=i; jm=j; sumx=sum;}
}
if( nmax <= 0 ) { IJMax(im,jm,il,ih,jl,jh); return 1;}
return nmax;
}

///////////////////////////////////////////////////////////////////
/*!
  Impression des informations sur l'histogramme.
*/
void Histo2D::Show(ostream & os) const
{
  os << "Histo2D::Show    nHist=" << nHist << " nEntries=" << nEntries;
  if(HasErrors()) os << "  Errors=Y \n"; else os << "  Errors=N\n";
  os << "mOver: [ " ;
  for(int j=2; j>=0; j--) {
    for(int i=0; i<3; i++) 
      os << mOver[j][i] << " " ;
    if (j != 0) os << "// ";
  }
  os << "]\n";
  os << "  nx=" << mNx << " xmin=" << mXmin << " xmax=" << mXmax 
     << " binx=" << mWBinx ;
  os << "  ny=" << mNy << " ymin=" << mYmin << " ymax=" << mYmax 
     << " biny=" << mWBiny << endl;
   
  //printf("mOver: [ %g %g %g // %g %g %g  // %g %g %g ]\n"
  //      ,mOver[2][0],mOver[2][1],mOver[2][2]
  //      ,mOver[1][0],mOver[1][1],mOver[1][2]
  //      ,mOver[0][0],mOver[0][1],mOver[0][2]);
  //printf("  nx=%d xmin=%g xmax=%g binx=%g  ",mNx,mXmin,mXmax,mWBinx);
  //printf("  ny=%d ymin=%g ymax=%g biny=%g\n",mNy,mYmin,mYmax,mWBiny);
}

///////////////////////////////////////////////////////////////////
/*!
  Impression de l'histogramme sur stdout entre [il,ih] et [jl,jh].
  \verbatim
    numero d'index:  00000000001111111111222222222233333
                     01234567890123456789012345678901234
    valeur entiere:  00000000001111111111222222222233333
                     12345678901234567890123456789012345
  \endverbatim
*/
void Histo2D::Print(r_8 min,r_8 max
                   ,int_4 il,int_4 ih,int_4 jl,int_4 jh) const
{
if(mNxy<=0) return;
int_4 ns = 35;
const char *s =    "+23456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

if( il > ih ) { il = 0; ih = mNx-1; }
if( jl > jh ) { jl = 0; jh = mNy-1; }
if( il < 0 ) il = 0;
if( jl < 0 ) jl = 0;
if( ih >= mNx ) ih = mNx-1;
if( jh >= mNy ) jh = mNy-1;

PrintStatus();

if( il != 0 || ih != mNx-1 || jl != 0 || jh != mNy-1 ) {
  r_8 xl,xh,yl,yh;
  BinLowEdge(il,jl,xl,yl);
  BinHighEdge(ih,jh,xh,yh);
  printf("  impression");
  printf("  en X: %d=[%d,%d] xmin=%g xmax=%g  "
        ,ih-il+1,il,ih,xl,xh);
  printf("  en Y: %d=[%d,%d] ymin=%g ymax=%g\n"
        ,jh-jl+1,jl,jh,yl,yh);
}

if(min >= max) { if(min != 0.) min = VMin(il,ih,jl,jh); else min=0.;
                 max = VMax(il,ih,jl,jh); }
if(min>max) return;
if(min==max) {min -= 1.; max += 1.;}
printf("   min=%g max=%g\n",min,max);

// imprime numero de bin en colonne
printf("\n");
if( mNx-1 >= 100 ) {
  printf("     ");
  for(int_4 i=il;i<=ih;i++) printf("%1d",(int_4) (i%1000)/100);
  printf("\n");
}
if( mNx-1 >= 10 ) {
  printf("     ");
  for(int_4 i=il;i<=ih;i++) printf("%1d",(int_4) (i%100)/10);
  printf("\n");
}
printf("     ");
for(int_4 i=il;i<=ih;i++) printf("%1d",i%10);
printf("\n");
printf("     "); {for(int_4 i=il;i<=ih;i++) printf("-"); printf("\n");}

// imprime histogramme
for(int_4 j=jh;j>=jl;j--) {
  printf("%3d: ",j);
  for(int_4 i=il;i<=ih;i++) {
    int_4 h;
    if( 1<=max-min && max-min<=35 ) h = (int_4)( (*this)(i,j) - min ) - 1;
      else h = (int_4)( ((*this)(i,j)-min)/(max-min) * ns ) - 1;
    char c;
    if(h<0 && (*this)(i,j)>min) c = '.';
      else if(h<0) c = ' ';
        else if(h>=ns) c = '*';
          else c = s[h];
    printf("%c",c);
  }
  printf("\n");
}

// imprime numero de bin en colonne
printf("     "); {for(int_4 i=il;i<=ih;i++) printf("-"); printf("\n");}
if( mNx-1 >= 100 ) {
  printf("     ");
  for(int_4 i=il;i<=ih;i++) printf("%1d",(int_4) (i%1000)/100);
  printf("\n");
}
if( mNx-1 >= 10 ) {
  printf("     ");
  for(int_4 i=il;i<=ih;i++) printf("%1d",(int_4) (i%100)/10);
  printf("\n");
}
printf("     ");
{for(int_4 i=il;i<=ih;i++) printf("%1d",i%10);}
printf("\n");

}

///////////////////////////////////////////////////////////////////
// Titre	Methodes pour gerer les projections

/*!
  Pour creer la projection X.
*/
void Histo2D::SetProjX()
{
if( mHprojx != NULL ) DelProjX();
mHprojx = new Histo(mXmin,mXmax,mNx);
if(mErr2 && mHprojx) mHprojx->Errors();
}

/*!
  Pour creer la projection Y.
*/
void Histo2D::SetProjY()
{
if( mHprojy != NULL ) DelProjY();
mHprojy = new Histo(mYmin,mYmax,mNy);
if(mErr2 && mHprojy) mHprojy->Errors();
}

/*!
  Pour creer les projections X et Y.
*/
void Histo2D::SetProj()
{
SetProjX();
SetProjY();
}

/*!
  Informations sur les projections.
*/
void Histo2D::ShowProj() const
{
if( mHprojx != NULL ) cout << ">>>> Projection X set : "<< mHprojx <<endl;
  else cout << ">>>> NO Projection X set"<<endl;
if( mHprojy != NULL ) cout << ">>>> Projection Y set : "<< mHprojy <<endl;
  else cout << ">>>> NO Projection Y set"<<endl;
}

/*!
  Destruction de l'histogramme de la projection selon X.
*/
void Histo2D::DelProjX()
{
if( mHprojx == NULL ) return;
delete mHprojx;
mHprojx = NULL;
}

/*!
  Destruction de l'histogramme de la projection selon X.
*/
void Histo2D::DelProjY()
{
if( mHprojy == NULL ) return;
delete mHprojy;
mHprojy = NULL;
}

/*!
  Destruction des histogrammes des projections selon X et Y.
*/
void Histo2D::DelProj()
{
DelProjX();
DelProjY();
}

/*!
  Remise a zero de la projection selon X.
*/
void Histo2D::ZeroProjX()
{
if( mHprojx == NULL ) return;
mHprojx->Zero();
}

/*!
  Remise a zero de la projection selon Y.
*/
void Histo2D::ZeroProjY()
{
if( mHprojy == NULL ) return;
mHprojy->Zero();
}

/*!
  Remise a zero des projections selon X et Y.
*/
void Histo2D::ZeroProj()
{
ZeroProjX();
ZeroProjY();
}

///////////////////////////////////////////////////////////////////
// Titre	Methodes pour gerer les bandes

/*!
  Pour creer une bande en X entre ybmin et ybmax.
*/
int_4 Histo2D::SetBandX(r_8 ybmin,r_8 ybmax)
{
mB_s.num = mLBandx.size();
mB_s.min = ybmin;
mB_s.max = ybmax;
mB_s.H = new Histo(mXmin,mXmax,mNx);
if(mErr2 && mB_s.H) mB_s.H->Errors();
mLBandx.push_back(mB_s);
mB_s.H = NULL;
return mLBandx.size()-1;
}

/*!
  Pour creer une bande en Y entre xbmin et xbmax.
*/
int_4 Histo2D::SetBandY(r_8 xbmin,r_8 xbmax)
{
mB_s.num = mLBandy.size();
mB_s.min = xbmin;
mB_s.max = xbmax;
mB_s.H = new Histo(mYmin,mYmax,mNy);
if(mErr2 && mB_s.H) mB_s.H->Errors();
mLBandy.push_back(mB_s);
mB_s.H = NULL;
return mLBandy.size()-1;
}

/*!
  Destruction des histogrammes des bandes selon X.
*/
void Histo2D::DelBandX()
{
if( mLBandx.size() <= 0 ) return;
for(list<bande_slice>::iterator i = mLBandx.begin(); i != mLBandx.end(); i++)
       if( (*i).H != NULL ) {delete (*i).H; (*i).H=NULL;}
mLBandx.erase(mLBandx.begin(),mLBandx.end());
}

/*!
  Destruction des histogrammes des bandes selon Y.
*/
void Histo2D::DelBandY()
{
if( mLBandy.size() <= 0 ) return;
for(list<bande_slice>::iterator i = mLBandy.begin(); i != mLBandy.end(); i++)
       if( (*i).H != NULL ) {delete (*i).H; (*i).H=NULL;}
mLBandy.erase(mLBandy.begin(),mLBandy.end());
}

/*!
  Remise a zero des bandes selon X.
*/
void Histo2D::ZeroBandX()
{
if( mLBandx.size() <= 0 ) return;
list<bande_slice>::iterator i;
for(i = mLBandx.begin(); i != mLBandx.end(); i++)
              (*i).H->Zero();
}

/*!
  Remise a zero des bandes selon Y.
*/
void Histo2D::ZeroBandY()
{
if( mLBandy.size() <= 0 ) return;
list<bande_slice>::iterator i;
for(i = mLBandy.begin(); i != mLBandy.end(); i++)
              (*i).H->Zero();
}

/*!
  Retourne un pointeur sur la bande numero `n' selon X.
*/
Histo* Histo2D::HBandX(int_4 n) const
{
if( mLBandx.size() <= 0 || n < 0 || n >= (int_4) mLBandx.size() ) return NULL;
for(list<bande_slice>::const_iterator i = mLBandx.begin(); i != mLBandx.end(); i++)
              if( (*i).num == n ) return (*i).H;
return NULL;
}

/*!
  Retourne un pointeur sur la bande numero `n' selon Y.
*/
Histo* Histo2D::HBandY(int_4 n) const
{
if( mLBandy.size() <= 0 || n < 0 || n >= (int_4) mLBandy.size() ) return NULL;
for(list<bande_slice>::const_iterator i = mLBandy.begin(); i != mLBandy.end(); i++)
              if( (*i).num == n ) return (*i).H;
return NULL;
}

/*!
  Retourne les limites de la bande numero `n' selon X.
*/
void Histo2D::GetBandX(int_4 n,r_8& ybmin,r_8& ybmax) const
{
ybmin = 0.; ybmax = 0.;
if( mLBandx.size() <= 0 || n < 0 || n >= (int_4) mLBandx.size() ) return;
for(list<bande_slice>::const_iterator i = mLBandx.begin(); i != mLBandx.end(); i++)
  if( (*i).num == n ) { ybmin = (*i).min; ybmax = (*i).max; return;}
return;
}

/*!
  Retourne les limites de la bande numero `n' selon Y.
*/
void Histo2D::GetBandY(int_4 n,r_8& xbmin,r_8& xbmax) const
{
xbmin = 0.; xbmax = 0.;
if( mLBandy.size() <= 0 || n < 0 || n >= (int_4) mLBandy.size() ) return;
for(list<bande_slice>::const_iterator i = mLBandy.begin(); i != mLBandy.end(); i++)
  if( (*i).num == n ) { xbmin = (*i).min; xbmax = (*i).max; return;}
return;
}

/*!
  Informations sur les bandes.
*/
void Histo2D::ShowBand(int_4 lp) const
{
  cout << ">>>> Nombre de bande X : " << mLBandx.size() << endl;
if( lp>0 && mLBandx.size()>0 ) {
  list<bande_slice>::const_iterator i;
  for(i = mLBandx.begin(); i != mLBandx.end(); i++) {
    cout<<"  "<<(*i).num<<" de ymin="<<(*i).min<<" a ymax="<<(*i).max;
    if(lp>1) cout << "   H=" << (*i).H;
    cout << endl;
  }
}

cout << ">>>> Nombre de bande Y : " << mLBandy.size() << endl;
if( lp>0 && mLBandy.size()>0 ) {
  list<bande_slice>::const_iterator i;
  for(i = mLBandy.begin(); i != mLBandy.end(); i++) {
    cout<<"  "<<(*i).num<<" de xmin="<<(*i).min<<" a xmax="<<(*i).max;
    if(lp>1) cout << "   H=" << (*i).H;
    cout << endl;
  }
}
}

///////////////////////////////////////////////////////////////////
// Titre  Methodes pour gerer les bandes equidistantes ou slices

/*!
  Pour creer `nsli' bandes equidistantes selon X.
*/
int_4 Histo2D::SetSliX(int_4 nsli)
{
if( nsli <= 0 ) return -1;
if( nsli >  mNy ) nsli = mNy;
if( mLSlix.size() > 0 ) DelSliX();
r_8 w = (mYmax-mYmin)/nsli;

for(int_4 i=0; i<nsli; i++ ) {
  mB_s.num = i;
  mB_s.min = mYmin + i*w;
  mB_s.max = mB_s.min + w;
  mB_s.H = new Histo(mXmin,mXmax,mNx);
  if(mErr2 && mB_s.H) mB_s.H->Errors();
  mLSlix.push_back(mB_s);
  mB_s.H = NULL;
}
return (int_4) mLSlix.size();
}

/*!
  Pour creer `nsli' bandes equidistantes selon Y.
*/
int_4 Histo2D::SetSliY(int_4 nsli)
{
if( nsli <= 0 ) return -1;
if( nsli >  mNx ) nsli = mNx;
if( mLSliy.size() > 0 ) DelSliY();
r_8 w = (mXmax-mXmin)/nsli;

for(int_4 i=0; i<nsli; i++ ) {
  mB_s.num = i;
  mB_s.min = mXmin + i*w;
  mB_s.max = mB_s.min + w;
  mB_s.H = new Histo(mYmin,mYmax,mNy);
  if(mErr2 && mB_s.H) mB_s.H->Errors();
  mLSliy.push_back(mB_s);
  mB_s.H = NULL;
}
return (int_4) mLSliy.size();
}

/*!
  Destruction des bandes equidistantes selon X.
*/
void Histo2D::DelSliX()
{
if( mLSlix.size() <= 0 ) return;
for(list<bande_slice>::iterator i = mLSlix.begin(); i != mLSlix.end(); i++)
       if( (*i).H != NULL ) {delete (*i).H; (*i).H=NULL;}
mLSlix.erase(mLSlix.begin(),mLSlix.end());
}

/*!
  Destruction des bandes equidistantes selon Y.
*/
void Histo2D::DelSliY()
{
if( mLSliy.size() <= 0 ) return;
for(list<bande_slice>::iterator i = mLSliy.begin(); i != mLSliy.end(); i++)
       if( (*i).H != NULL ) {delete (*i).H; (*i).H=NULL;}
mLSliy.erase(mLSliy.begin(),mLSliy.end());
}

/*!
  Remise a zero des bandes equidistantes selon X.
*/
void Histo2D::ZeroSliX()
{
if( mLSlix.size() <= 0 ) return;
list<bande_slice>::iterator i;
for(i = mLSlix.begin(); i != mLSlix.end(); i++)
              (*i).H->Zero();
}

/*!
  Remise a zero des bandes equidistantes selon Y.
*/
void Histo2D::ZeroSliY()
{
if( mLSliy.size() <= 0 ) return;
list<bande_slice>::iterator i;
for(i = mLSliy.begin(); i != mLSliy.end(); i++)
              (*i).H->Zero();
}

/*!
  Retourne un pointeur sur la bande equidistante numero `n'
  selon X.
*/
Histo* Histo2D::HSliX(int_4 n) const
{
if( mLSlix.size() <= 0 || n < 0 || n >= (int_4) mLSlix.size() ) return NULL;
for(list<bande_slice>::const_iterator i = mLSlix.begin(); i != mLSlix.end(); i++)
              if( (*i).num == n ) return (*i).H;
return NULL;
}

/*!
  Retourne un pointeur sur la bande equidistante numero `n'
  selon Y.
*/
Histo* Histo2D::HSliY(int_4 n) const
{
if( mLSliy.size() <= 0 || n < 0 || n >= (int_4) mLSliy.size() ) return NULL;
for(list<bande_slice>::const_iterator i = mLSliy.begin(); i != mLSliy.end(); i++)
              if( (*i).num == n ) return (*i).H;
return NULL;
}

/*!
  Retourne les limites de la bande equidistante numero `n' selon X.
*/
void Histo2D::GetSliX(int_4 n,r_8& ybmin,r_8& ybmax) const
{
ybmin = 0.; ybmax = 0.;
if( mLSlix.size() <= 0 || n < 0 || n >= (int_4) mLSlix.size() ) return;
for(list<bande_slice>::const_iterator i = mLSlix.begin(); i != mLSlix.end(); i++)
  if( (*i).num == n ) { ybmin = (*i).min; ybmax = (*i).max; return;}
return;
}

/*!
  Retourne les limites de la bande equidistante numero `n' selon Y.
*/
void Histo2D::GetSliY(int_4 n,r_8& xbmin,r_8& xbmax) const
{
xbmin = 0.; xbmax = 0.;
if( mLSliy.size() <= 0 || n < 0 || n >= (int_4) mLSliy.size() ) return;
for(list<bande_slice>::const_iterator i = mLSliy.begin(); i != mLSliy.end(); i++)
  if( (*i).num == n ) { xbmin = (*i).min; xbmax = (*i).max; return;}
return;
}

/*!
  Informations sur les bandes equidistantes.
*/
void Histo2D::ShowSli(int_4 lp) const
{
list<bande_slice>::const_iterator i;
cout << ">>>> Nombre de slice X : " << mLSlix.size() << endl;
if( lp>0 && mLSlix.size() > 0 )
  for(i = mLSlix.begin(); i != mLSlix.end(); i++) {
    cout<<"  "<<(*i).num<<" de ymin="<<(*i).min<<" a ymax="<<(*i).max;
    if(lp>1) cout << "   H=" << (*i).H;
    cout << endl;
  }

cout << ">>>> Nombre de slice Y : " << mLSliy.size() << endl;
if( lp>0 && mLSliy.size()>0 )
  for(i = mLSliy.begin(); i != mLSliy.end(); i++) {
    cout<<"  "<<(*i).num<<" de xmin="<<(*i).min<<" a xmax="<<(*i).max;
    if(lp>1) cout << "   H=" << (*i).H;
    cout << endl;
  }
}

///////////////////////////////////////////////////////////
// --------------------------------------------------------
//   Les objets delegues pour la gestion de persistance 
// --------------------------------------------------------
///////////////////////////////////////////////////////////


DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void  ObjFileIO<Histo2D>::ReadSelf(PInPersist& is)
{
char strg[256];

if(dobj==NULL) dobj=new Histo2D;
  else         dobj->Delete();

r_8 min,max;
int_4 errok, projx, projy, nslix, nsliy, nbanx, nbany;

// Lecture entete
is.GetLine(strg, 255);
is.GetLine(strg, 255);
is.GetLine(strg, 255);
is.GetLine(strg, 255);
is.GetLine(strg, 255);
is.GetLine(strg, 255);

// Lecture variables de definitions
is.Get(dobj->mNx);
is.Get(dobj->mNy);
is.Get(dobj->mNxy);
is.Get(errok);
is.Get(dobj->nEntries);
is.Get(dobj->nHist);

is.Get(dobj->mXmin);
is.Get(dobj->mXmax);
is.Get(dobj->mYmin);
is.Get(dobj->mYmax);
is.Get(dobj->mWBinx);
is.Get(dobj->mWBiny);

is.Get(&(dobj->mOver[0][0]),9);

is.Get(projx);
is.Get(projy);
is.Get(nslix);
is.Get(nsliy);
is.Get(nbanx);
is.Get(nbany);

// Lecture histo2D
dobj->mData = new r_8[dobj->mNxy];
is.GetLine(strg, 255);
{for(int_4 j=0;j<dobj->mNy;j++) is.Get(dobj->mData+j*dobj->mNx,dobj->mNx);}

// Lecture erreurs
if(errok) {
  is.GetLine(strg, 255);
  dobj->mErr2 = new r_8[dobj->mNxy];
  for(int_4 j=0;j<dobj->mNy;j++) is.Get(dobj->mErr2+j*dobj->mNx,dobj->mNx);
}

// Lecture des projections
if(projx) {
  is.GetLine(strg, 255);
  dobj->SetProjX();
  ObjFileIO<Histo> fio_h(dobj->mHprojx);
  fio_h.Read(is);
}
if(projy) {
  is.GetLine(strg, 255);
  dobj->SetProjY();
  ObjFileIO<Histo> fio_h(dobj->mHprojy);
  fio_h.Read(is);
}

// Lecture des slices
if(nslix>0) {
  is.GetLine(strg, 255);
  dobj->SetSliX(nslix);
  ASSERT (nslix==dobj->NSliX());
  for(int_4 j=0;j<dobj->NSliX();j++)
    {ObjFileIO<Histo> fio_h(dobj->HSliX(j)); fio_h.Read(is);}
}
if(nsliy>0) {
  is.GetLine(strg, 255);
  dobj->SetSliY(nsliy);
  ASSERT (nsliy==dobj->NSliY());
  for(int_4 j=0;j<dobj->NSliY();j++)
    {ObjFileIO<Histo> fio_h(dobj->HSliY(j)); fio_h.Read(is);}
}

// Lecture des bandes
if( nbanx>0 ) {
  is.GetLine(strg, 255);
  {for(int_4 j=0; j<nbanx; j++) {
    is.Get(min); is.Get(max);
    dobj->SetBandX(min,max);
  }}
  ASSERT (nbanx==dobj->NBandX());
  {for(int_4 j=0; j<dobj->NBandX(); j++) {
    ObjFileIO<Histo> fio_h(dobj->HBandX(j));
    fio_h.Read(is);
  }}
}
if( nbany>0 ) {
  is.GetLine(strg, 255);
  {for(int_4 j=0; j<nbany; j++) {
    is.Get(min); is.Get(max);
    dobj->SetBandY(min,max);
  }}
  ASSERT (nbany==dobj->NBandY());
  {for(int_4 j=0; j<dobj->NBandY(); j++) {
    ObjFileIO<Histo> fio_h(dobj->HBandY(j));
    fio_h.Read(is);
  }}
}

return;
}

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<Histo2D>::WriteSelf(POutPersist& os) const
{
if (dobj == NULL)   return;
char strg[256];

// Que faut-il ecrire?
int_4 errok = (dobj->mErr2) ? 1 : 0;
int_4 projx = (dobj->mHprojx) ? 1 : 0;
int_4 projy = (dobj->mHprojy) ? 1 : 0;
int_4 nslix = dobj->NSliX();
int_4 nsliy = dobj->NSliY();
int_4 nbanx = dobj->NBandX();
int_4 nbany = dobj->NBandY();

// Ecriture entete pour identifier facilement
sprintf(strg,"nx=%d  ny=%d  nxy=%d errok=%1d"
       ,dobj->mNx,dobj->mNy,dobj->mNxy,errok);
os.PutLine(strg);
sprintf(strg,"nHist=%g nEntries=%d",dobj->nHist,dobj->nEntries);
os.PutLine(strg);
sprintf(strg,"wbinx=%g wbiny=%g",dobj->mWBinx,dobj->mWBiny);
os.PutLine(strg);
sprintf(strg,"xmin=%g xmax=%g ymin=%g ymax=%g"
       ,dobj->mXmin,dobj->mXmax,dobj->mYmin,dobj->mYmax);
os.PutLine(strg);
sprintf(strg,"projx/y=%d %d nbandx/y=%d %d nbslix/y=%d %d"
       ,projx,projy,nbanx,nbany,nslix,nsliy);
os.PutLine(strg);
sprintf(strg,"mOver %g %g %g %g %g %g %g %g %g"
       ,dobj->mOver[0][0],dobj->mOver[0][1],dobj->mOver[0][2]
       ,dobj->mOver[1][0],dobj->mOver[1][1],dobj->mOver[1][2]
       ,dobj->mOver[2][0],dobj->mOver[2][1],dobj->mOver[2][2]);
os.PutLine(strg);

// Ecriture variables de definitions
os.Put(dobj->mNx);
os.Put(dobj->mNy);
os.Put(dobj->mNxy);
os.Put(errok);
os.Put(dobj->nEntries);
os.Put(dobj->nHist);

os.Put(dobj->mXmin);
os.Put(dobj->mXmax);
os.Put(dobj->mYmin);
os.Put(dobj->mYmax);
os.Put(dobj->mWBinx);
os.Put(dobj->mWBiny);

os.Put(&(dobj->mOver[0][0]),9);

os.Put(projx);
os.Put(projy);
os.Put(nslix);
os.Put(nsliy);
os.Put(nbanx);
os.Put(nbany);

// Ecriture histo2D
sprintf(strg,"Histo2D: Tableau des donnees %d = %d * %d"
       ,dobj->mNxy,dobj->mNx,dobj->mNy);
os.PutLine(strg);
{for(int_4 j=0;j<dobj->mNy;j++) os.Put(dobj->mData+j*dobj->mNx,dobj->mNx);}

// Ecriture erreurs
if(errok) {
  sprintf(strg,"Histo2D: Tableau des erreurs %d = %d * %d"
         ,dobj->mNxy,dobj->mNx,dobj->mNy);
  os.PutLine(strg);
  for(int_4 j=0;j<dobj->mNy;j++) os.Put(dobj->mErr2+j*dobj->mNx,dobj->mNx);
}

// Ecriture des projections
if(projx) {
  sprintf(strg,"Histo2D: Projection X");
  os.PutLine(strg);
  ObjFileIO<Histo> fio_h(dobj->mHprojx); fio_h.Write(os);
}
if(projy) {
  sprintf(strg,"Histo2D: Projection Y");
  os.PutLine(strg);
  ObjFileIO<Histo> fio_h(dobj->mHprojy); fio_h.Write(os);
}

// Ecriture des slices
if(nslix>0) {
  sprintf(strg,"Histo2D: Slices X %d",nslix);
  os.PutLine(strg);
  for(int_4 j=0;j<nslix;j++) {
    Histo* h = dobj->HSliX(j);
    ObjFileIO<Histo> fio_h(h); fio_h.Write(os);
  }
}
if(nsliy>0) {
  sprintf(strg,"Histo2D: Slices Y %d",nsliy);
  os.PutLine(strg);
  for(int_4 j=0;j<nsliy;j++) {
    Histo* h = dobj->HSliY(j);
    ObjFileIO<Histo> fio_h(h); fio_h.Write(os);
  }
}

// Ecriture des bandes
if( nbanx>0 ) {
  sprintf(strg,"Histo2D: Bandes X %d",nbanx);
  os.PutLine(strg);
  list<Histo2D::bande_slice>::const_iterator it;
  for(it = dobj->mLBandx.begin(); it != dobj->mLBandx.end(); it++) {
    r_8 min = (*it).min; r_8 max = (*it).max;
    os.Put(min); os.Put(max);
  }
  for(it = dobj->mLBandx.begin(); it != dobj->mLBandx.end(); it++) {
    Histo* h = (*it).H;
    ObjFileIO<Histo> fio_h(h); fio_h.Write(os);
  }
}
if( nbany>0 ) {
  sprintf(strg,"Histo2D: Bandes Y %d",nbany);
  os.PutLine(strg);
  list<Histo2D::bande_slice>::const_iterator it;
  for(it = dobj->mLBandy.begin(); it != dobj->mLBandy.end(); it++) {
    r_8 min = (*it).min; r_8 max = (*it).max;
    os.Put(min); os.Put(max);
  }
  for(it = dobj->mLBandy.begin(); it != dobj->mLBandy.end(); it++) {
    Histo* h = (*it).H;
    ObjFileIO<Histo> fio_h(h); fio_h.Write(os);
  }
}

return;
}


#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<Histo2D>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<Histo2D>;
#endif

} // FIN namespace SOPHYA 
