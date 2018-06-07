#include "machdefs.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "histos.h"
#include "perrors.h"
#include "poly.h"
#include "strutil.h"

namespace SOPHYA {

/*!
  \class Histo
  \ingroup HiStats
  Classe d'histogrammes 1D
*/

/********* Methode *********/
/*! Constructeur par defaut */
Histo::Histo()
: mData(NULL), mErr2(NULL)
,  mUnder(0.), mOver(0.), nHist(0.), nEntries(0)
,  mBins(0), mMin(0.), mMax(0.), binWidth(0.)
{
}

/********* Methode *********/
/*! Constructeur d'un histo de nBin bins allant de xMin a xMax */
Histo::Histo(r_8 xMin, r_8 xMax, int_4 nBin)
: mData(NULL), mErr2(NULL)
{
  CreateOrResize(xMin,xMax,nBin);
}

/********* Methode *********/
/*! Constructeur d'un histo de nBin bins allant de xMin a xMax */
Histo::Histo(r_4 xMin, r_4 xMax, int_4 nBin)
: mData(NULL), mErr2(NULL)
{
  CreateOrResize((r_8)xMin,(r_8)xMax,nBin);
}

/********* Methode *********/
/*! Constructeur par copie */
Histo::Histo(const Histo& H)
: mData((H.mBins>0)? new r_8[H.mBins] : NULL),
  mErr2((H.mBins>0 && H.mErr2!=NULL)? new r_8[H.mBins]: NULL),
  mUnder(H.mUnder), mOver(H.mOver), nHist(H.nHist), nEntries(H.nEntries),
  mBins(H.mBins), mMin(H.mMin), mMax(H.mMax),
  binWidth(H.binWidth)
{
  if(mBins<=0) return;
  memcpy(mData,H.mData,mBins*sizeof(r_8));
  if(H.mErr2) memcpy(mErr2, H.mErr2, mBins*sizeof(r_8));
}

/********* Methode *********/
/*! Destructeur */
Histo::~Histo()
{
  Delete();
}

/********* Methode *********/
/*! Redimensionnement d'un histo */
void Histo::ReSize(r_8 xMin, r_8 xMax, int_4 nBin)
{
  CreateOrResize(xMin,xMax,nBin);
}

/********* Methode *********/
/*! Gestion de l'allocation */
void Histo::CreateOrResize(r_8 xMin, r_8 xMax, int_4 nBin)
{
  //cout<<"Histo::CreateOrResize()"<<endl;
  bool samelen = (nBin==mBins)? true: false;
  if(mData!=NULL && !samelen) {delete[] mData; mData = NULL;}
  if(mErr2!=NULL) {delete[] mErr2; mErr2 = NULL;} // On des-alloue toujours
  if(nBin>0 && mData==NULL) mData = new r_8[nBin];
  if(mData) memset(mData,0,nBin*sizeof(r_8));
  mBins = nBin;
  mMin = xMin; mMax = xMax;
  binWidth = (nBin>0) ? (mMax-mMin)/nBin : 0.;
  mUnder = mOver = nHist = 0.;
  nEntries = 0;
}

/********* Methode *********/
/*! Gestion de la des-allocation */
void Histo::Delete()
{
 //cout<<"Histo::Delete()"<<endl;
 if(mData != NULL) {delete[] mData; mData = NULL;}
 if(mErr2 != NULL) {delete[] mErr2; mErr2 = NULL;}
 mUnder = mOver = mMin = mMax = binWidth = nHist = 0.;
 mBins = 0;
 nEntries = 0;
}

/********* Methode *********/
/*!
  Remise a zero
*/
void Histo::Zero()
{
  if(mBins<=0 || mData==NULL) return;
  memset(mData,0,mBins*sizeof(r_8));
  if(mErr2) memset(mErr2,0, mBins*sizeof(r_8));
  mUnder = mOver = nHist = 0;
  nEntries = 0;
}

/********* Methode *********/
/*!
  Pour avoir le calcul des erreurs
*/
void Histo::Errors()
{
 if(mErr2 != NULL) {delete[] mErr2; mErr2 = NULL;}
 if(mBins<=0) return;
 mErr2 = new r_8[mBins];
 memset(mErr2,0,mBins*sizeof(r_8));
}

/********* Methode *********/
/*!
 Recompute XMin and XMax so that
 the CENTER of the first bin is exactly XMin and
 the CENTER of the last bin is exactly XMax.
 Remember that otherwise
 XMin is the beginning of the first bin
 and XMax is the end of the last bin
*/
void Histo::ReCenterBin(void)
{
 if(mBins<=1) return;
 double dx = (mMax-mMin)/(mBins-1);
 mMin -= dx/2.;
 mMax += dx/2.;
 binWidth = (mMax-mMin)/mBins;
}

/********* Methode *********/
/*!
  Operateur egal Histo = Histo
*/
Histo& Histo::operator = (const Histo& h)
{
  if(this == &h) return *this;
  CreateOrResize(h.mMin,h.mMax,h.mBins);
  if(h.mErr2) Errors();
  if(mData) memcpy(mData,h.mData,mBins*sizeof(r_8));
  if(mErr2) memcpy(mErr2,h.mErr2,mBins*sizeof(r_8));
  mUnder = h.mUnder; mOver = h.mOver;
  nHist = h.nHist; nEntries = h.nEntries;
  return *this;
}

/********* Methode *********/
/*!
  Operateur de multiplication par une constante
*/
Histo& Histo::operator *= (r_8 b)
{
r_8 b2 = b*b;
for(int_4 i=0;i<mBins;i++) {
  mData[i] *= b;
  if(mErr2) mErr2[i] *= b2;
}
mUnder *= b;
mOver *= b;
nHist *= b;

return *this;
}

/*!
  Operateur de division par une constante
*/
Histo& Histo::operator /= (r_8 b)
{
if (b==0.) throw ParmError(PExcLongMessage(""));
r_8 b2 = b*b;
for(int_4 i=0;i<mBins;i++) {
  mData[i] /= b;
  if(mErr2) mErr2[i] /= b2;
}
mUnder /= b;
mOver /= b;
nHist /= b;

return *this;
}

/*!
  Operateur d'addition d'une constante
*/
Histo& Histo::operator += (r_8 b)
{
for(int_4 i=0;i<mBins;i++) mData[i] += b;
mUnder += b;
mOver += b;
nHist += mBins*b;

return *this;
}

/*!
  Operateur de soustraction d'une constante
*/
Histo& Histo::operator -= (r_8 b)
{
for(int_4 i=0;i<mBins;i++) mData[i] -= b;
mUnder -= b;
mOver -= b;
nHist -= mBins*b;

return *this;
}

/********* Methode *********/
/*!
  Operateur H += H1
*/
Histo& Histo::operator += (const Histo& a)
{
if(mBins!=a.mBins) throw SzMismatchError(PExcLongMessage(""));
for(int_4 i=0;i<mBins;i++) {
  mData[i] += a(i);
  if(mErr2 && a.mErr2) mErr2[i] += a.Error2(i);
}
mUnder += a.mUnder;
mOver += a.mOver;
nHist += a.nHist;
nEntries += a.nEntries;

return *this;
}

/*!
  Operateur H -= H1
*/
Histo& Histo::operator -= (const Histo& a)
{
if(mBins!=a.mBins) throw SzMismatchError(PExcLongMessage(""));
for(int_4 i=0;i<mBins;i++) {
  mData[i] -= a(i);
  if(mErr2 && a.mErr2) mErr2[i] += a.Error2(i);
}
mUnder -= a.mUnder;
mOver -= a.mOver;
nHist -= a.nHist;
nEntries += a.nEntries;

return *this;
}

/*!
  Operateur H *= H1
*/
Histo& Histo::operator *= (const Histo& a)
{
if(mBins!=a.mBins) throw SzMismatchError(PExcLongMessage(""));
nHist = 0.;
for(int_4 i=0;i<mBins;i++) {
  if(mErr2 && a.mErr2)
      mErr2[i] = a(i)*a(i)*mErr2[i] + mData[i]*mData[i]*a.Error2(i);
  mData[i] *= a(i);
  nHist += mData[i];
}
mUnder *= a.mUnder;
mOver *= a.mOver;
nEntries += a.nEntries;

return *this;
}

/*!
  Operateur H /= H1
*/
Histo& Histo::operator /= (const Histo& a)
{
if(mBins!=a.mBins) throw SzMismatchError(PExcLongMessage(""));
nHist = 0.;
for(int_4 i=0;i<mBins;i++) {
  if(a(i)==0.) {
    mData[i]=0.;
    if(mErr2) mErr2[i]=0.;
    continue;
  }
  if(mErr2 && a.mErr2)
      mErr2[i] = (mErr2[i] + mData[i]/a(i)*mData[i]/a(i)*a.Error2(i))
                /(a(i)*a(i));
  mData[i] /= a(i);
  nHist += mData[i];
}
if(a.mUnder!=0.) mUnder /= a.mUnder; else mUnder = 0.;
if(a.mOver!=0.)  mOver  /= a.mOver;  else mOver  = 0.;
nEntries += a.nEntries;

return *this;
}

/********* Methode *********/
/*!
  Remplissage d'un tableau avec la valeur des abscisses
*/
void Histo::GetAbsc(TVector<r_8> &v) const
{
v.Realloc(mBins);
for(int_4 i=0;i<mBins;i++) v(i) = BinLowEdge(i);
return;
}

/*!
  Remplissage d'un tableau avec la valeur du contenu
*/
void Histo::GetValue(TVector<r_8> &v) const
{
v.Realloc(mBins);
for(int_4 i=0;i<mBins;i++) v(i) = mData[i];
return;
}

/*!
  Remplissage d'un tableau avec la valeur des erreurs au carre
*/
void Histo::GetError2(TVector<r_8> &v) const
{
v.Realloc(mBins);
if(!mErr2) {for(int_4 i=0;i<mBins;i++) v(i) = 0.; return;}
for(int_4 i=0;i<mBins;i++) v(i) = mErr2[i];
return;
}

/*!
  Remplissage d'un tableau avec la valeur des erreurs
*/
void Histo::GetError(TVector<r_8> &v) const
{
v.Realloc(mBins);
if(!mErr2) {for(int_4 i=0;i<mBins;i++) v(i) = 0.; return;}
for(int_4 i=0;i<mBins;i++) v(i) = Error(i);
return;
}

/********* Methode *********/
/*!
  Remplissage du contenu de l'histo avec les valeurs d'un vecteur
*/
void Histo::PutValue(TVector<r_8> &v, int_4 ierr)
{
//if(v.NElts()<(uint_4) mBins) throw SzMismatchError(PExcLongMessage(""));
uint_4 n = (v.NElts()<(uint_4) mBins) ? v.NElts(): (uint_4) mBins;
if(n>0) for(uint_4 i=0;i<n;i++) {
  mData[i] = v(i);
  if(mErr2&&ierr) mErr2[i] = fabs(v(i));
}
return;
}

/*!
  Addition du contenu de l'histo avec les valeurs d'un vecteur
*/
void Histo::PutValueAdd(TVector<r_8> &v, int_4 ierr)
{
//if(v.NElts()<(uint_4) mBins) throw SzMismatchError(PExcLongMessage(""));
uint_4 n = (v.NElts()<(uint_4) mBins) ? v.NElts(): (uint_4) mBins;
if(n>0) for(uint_4 i=0;i<n;i++) {
  mData[i] += v(i);
  if(mErr2 && ierr) mErr2[i] += fabs(v(i));
}
return;
}

/*!
  Remplissage des erreurs au carre de l'histo avec les valeurs d'un vecteur
*/
void Histo::PutError2(TVector<r_8> &v)
{
//if(v.NElts()<(uint_4) mBins) throw SzMismatchError(PExcLongMessage(""));
uint_4 n = (v.NElts()<(uint_4) mBins) ? v.NElts(): (uint_4) mBins;
if(n>0) {
  if(!mErr2) Errors();
  for(uint_4 i=0;i<n;i++) mErr2[i] = v(i);
}
return;
}

/*!
  Addition des erreurs au carre de l'histo avec les valeurs d'un vecteur
*/
void Histo::PutError2Add(TVector<r_8> &v)
{
//if(v.NElts()<(uint_4) mBins) throw SzMismatchError(PExcLongMessage(""));
uint_4 n = (v.NElts()<(uint_4) mBins) ? v.NElts(): (uint_4) mBins;
if(n>0) {
  if(!mErr2) Errors();
  for(uint_4 i=0;i<n;i++) if(v(i)>0.) mErr2[i] += v(i);
}
return;
}

/*!
  Remplissage des erreurs de l'histo avec les valeurs d'un vecteur
*/
void Histo::PutError(TVector<r_8> &v)
{
//if(v.NElts()<(uint_4) mBins) throw SzMismatchError(PExcLongMessage(""));
uint_4 n = (v.NElts()<(uint_4) mBins) ? v.NElts(): (uint_4) mBins;
if(n>0) {
  if(!mErr2) Errors();
  for(uint_4 i=0;i<n;i++)
    if(v(i)>0.) mErr2[i]=v(i)*v(i); else mErr2[i]=-v(i)*v(i);
}
return;
}

/********* Methode *********/
/*!
  Addition du contenu de l'histo pour abscisse x poids w
*/
void Histo::Add(r_8 x, r_8 w)
{
  int_4 numBin = FindBin(x);
  if (numBin<0) mUnder += w;
  else if (numBin>=mBins) mOver += w;
  else {
    mData[numBin] += w;
    if(mErr2!=NULL) mErr2[numBin] += w*w;
    nHist += w;
    nEntries++;
  }
}

/********* Methode *********/
/*!
  Addition du contenu de l'histo bin numBin poids w
*/
void Histo::AddBin(int_4 numBin, r_8 w)
{
  if (numBin<0) mUnder += w;
  else if (numBin>=mBins) mOver += w;
  else {
    mData[numBin] += w;
    if(mErr2!=NULL) mErr2[numBin] += w*w;
    nHist += w;
    nEntries++;
  }
}

/********* Methode *********/
/*!
  Remplissage du contenu de l'histo pour abscisse x poids w
*/
void Histo::SetBin(r_8 x, r_8 w)
{
  int_4 numBin = FindBin(x);
  SetBin(numBin,w);
}

/********* Methode *********/
/*!
  Remplissage du contenu de l'histo pour numBin poids w
*/
void Histo::SetBin(int_4 numBin, r_8 w)
{
  if (numBin<0) mUnder = w;
  else if (numBin>=mBins) mOver = w;
  else {
    nHist -= mData[numBin];
    mData[numBin] = w;
    nHist += w;
  }
}

/********* Methode *********/
/*!
  Remplissage des erreurs au carre pour abscisse x
*/
void Histo::SetErr2(r_8 x, r_8 e2)
{
  int_4 numBin = FindBin(x);
  SetErr2(numBin,e2);
}

/********* Methode *********/
/*!
  Remplissage des erreurs au carre pour numBin poids
*/
void Histo::SetErr2(int_4 numBin, r_8 e2)
{
  if( mErr2==NULL) return;
  if ( numBin<0 || numBin>=mBins ) return;
  mErr2[numBin] = e2;
}

/********* Methode *********/
/*!
  Remplissage des erreurs pour abscisse x
*/
void Histo::SetErr(r_8 x, r_8 e)
{
SetErr2(x,  e*e);
}

/********* Methode *********/
/*!
  Remplissage des erreurs pour numBin
*/
void Histo::SetErr(int_4 numBin, r_8 e)
{
SetErr2(numBin,  e*e);
}

/********* Methode *********/
/*!
  Methode virtuelle de mise a jour - Ne fait rien pour Histo - Voir HProf 
*/
void Histo::UpdateHisto(bool force) const
{
  return;
}

/********* Methode *********/
/*!
  Numero du bin ayant le contenu maximum
*/
int_4 Histo::IMax() const
{
  if(mBins<=0)  return 0;
  int_4 imx=0;
  if(mBins==1) return imx;
  r_8 mx=mData[0];
  for (int_4 i=1; i<mBins; i++)
    if (mData[i]>mx) {imx = i; mx=mData[i];}
  return imx;
}

/********* Methode *********/
/*!
  Numero du bin ayant le contenu minimum
*/
int_4 Histo::IMin() const
{
  if(mBins<=0)  return 0;
  int_4 imx=0;
  if(mBins==1) return imx;
  r_8 mx=mData[0];
  for (int_4 i=1; i<mBins; i++)
    if (mData[i]<mx) {imx = i; mx=mData[i];}
  return imx;
}

/********* Methode *********/
/*!
  Valeur le contenu maximum
*/
r_8 Histo::VMax() const
{
  if(mBins<=0)  return 0.;
  r_8 mx=mData[0];
  if(mBins==1) return mx;
  for (int_4 i=1;i<mBins;i++) if(mData[i]>mx) mx=mData[i];
  return mx;
}

/********* Methode *********/
/*!
  Valeur le contenu minimum
*/
r_8 Histo::VMin() const
{
  if(mBins<=0)  return 0.;
  r_8 mx=mData[0];
  if(mBins==1) return mx;
  for (int_4 i=1;i<mBins;i++) if(mData[i]<mx) mx=mData[i];
  return mx;
}

/********* Methode *********/
/*!
  Valeur somme des valeurs des bins
*/
r_8 Histo::Sum() const
{
  if(mBins<=0)  return 0.;
  r_8 sx = 0.;
  for (int_4 i=0; i<mBins; i++) sx += mData[i];
  return sx;
}

/********* Methode *********/
/*!
  Valeur somme des carres des valeurs des bins
*/
r_8 Histo::Sum2() const
{
  if(mBins<=0)  return 0.;
  r_8 sx2 = 0.;
  for (int_4 i=0; i<mBins; i++) sx2 += mData[i]*mData[i];
  return sx2;
}

/********* Methode *********/
/*!
  Valeur moyenne
*/
r_8 Histo::Mean() const
{
  if(mBins<=0)  return 0.;
  r_8 n = 0;
  r_8 sx = 0;
  for (int_4 i=0; i<mBins; i++) {
    r_8 v = (mData[i]>=0.) ? mData[i] : -mData[i];
    n += v;
    sx += BinCenter(i)*v;
  }
  if(n>0.) return sx/n; else return mMin;
}

/********* Methode *********/
/*!
  Valeur du sigma
*/
r_8 Histo::Sigma() const
{
  if(mBins<=0)  return 0.;
  r_8 n = 0;
  r_8 sx = 0;
  r_8 sx2= 0;
  for (int_4 i=0; i<mBins; i++) {
    r_8 v = (mData[i]>=0.) ? mData[i] : -mData[i];
    n += v;
    sx += BinCenter(i)*v;
    sx2+= BinCenter(i)*BinCenter(i)*v;
  }
  // attention a l'erreur d'arrondi si un seul bin rempli!!
  if( n == 0 ) return 0.;
  sx2 = sx2/n - (sx/n)*(sx/n);
  if( sx2>0. ) return sqrt( sx2 );
      else return 0.;
}

/********* Methode *********/
/*!
  Valeur de la moyenne calculee entre les bin il et ih
*/
r_8 Histo::MeanLH(int_4 il,int_4 ih) const
{
  if(mBins<=0)  return 0.;
  if( ih < il ) { il = 0; ih = mBins-1; }
  if( il < 0 ) il = 0;
  if( ih >= mBins ) ih = mBins-1;
  r_8 n = 0;
  r_8 sx = 0;
  for (int_4 i=il; i<=ih; i++) {
    r_8 v = (mData[i]>=0.) ? mData[i] : -mData[i];
    n += v;
    sx += BinCenter(i)*v;
  }
  if(n>0.) return sx/n; else return BinLowEdge(il);
}

/********* Methode *********/
/*!
  Valeur de la moyenne calculee entre les bin il et ih
*/
r_8 Histo::SigmaLH(int_4 il,int_4 ih) const
{
  if(mBins<=0)  return 0.;
  if( ih < il ) { il = 0; ih = mBins - 1; }
  if( il < 0 ) il = 0;
  if( ih >= mBins ) ih = mBins - 1;
  r_8 n = 0;
  r_8 sx = 0;
  r_8 sx2= 0;
  for (int_4 i=il; i<=ih; i++) {
    r_8 v = (mData[i]>=0.) ? mData[i] : -mData[i];
    n += v;
    sx += BinCenter(i)*v;
    sx2+= BinCenter(i)*BinCenter(i)*v;
  }
  if( n == 0 ) return 0.;
  sx2 = sx2/n - (sx/n)*(sx/n);
  if( sx2>0. ) return sqrt( sx2 );
      else return 0.;
}

/********* Methode *********/
/*!
  Valeur de la moyenne calculee entre x0-dx et x0+dx
*/
r_8 Histo::Mean(r_8 x0, r_8 dx) const
{
  if(mBins<=0)  return 0.;
  r_8 sdata = 0;
  r_8 sx = 0;
  int_4 iMin = FindBin(x0-dx);
  if (iMin<0) iMin = 0;
  int_4 iMax = FindBin(x0+dx);
  if (iMax>mBins) iMax=mBins;
  for (int_4 i=iMin; i<iMax; i++) {
    r_8 v = (mData[i]>=0.) ? mData[i] : -mData[i];
    sx += BinCenter(i)*v;
    sdata += v;
  }
  if(sdata>0.) return sx/sdata; else return mMin;
}

/********* Methode *********/
/*!
  Valeur du sigma calcule entre x0-dx et x0+dx
*/
r_8 Histo::Sigma(r_8 x0, r_8 dx) const
{
  if(mBins<=0)  return 0.;
  r_8 sx = 0;
  r_8 sx2= 0;
  r_8 sdata = 0;
  int_4 iMin = FindBin(x0-dx);
  if (iMin<0) iMin = 0;
  int_4 iMax = FindBin(x0+dx);
  if (iMax>mBins) iMax=mBins;
  for (int_4 i=iMin; i<iMax; i++) {
    r_8 v = (mData[i]>=0.) ? mData[i] : -mData[i];
    sx += BinCenter(i)*v;
    sx2+= BinCenter(i)*BinCenter(i)*v;
    sdata += v;
  }
  if(sdata>0.) return sqrt( sx2/sdata - (sx/sdata)*(sx/sdata) );
          else return 0.;
}

/********* Methode *********/
/*!
  Valeur de la moyenne et du sigma nettoyes
*/
r_8 Histo::CleanedMean(r_8& sigma) const
{
  if(mBins<=0)  return 0.;
  if (!nHist) return 0;
  // on fait ca de facon bete, on pourra raffiner apres...
  r_8 x0 = Mean();
  r_8 s  = Sigma()+binWidth;
  
  for (int_4 i=0; i<3; i++) {
    r_8 xx0 = Mean(x0,5*s);
    s  = Sigma(x0,5*s)+binWidth;
    x0 = xx0;
  }
  sigma = s;
  return x0;
}

/********* Methode *********/
/*!
  Valeur de la moyenne nettoyee
*/
r_8 Histo::CleanedMean() const
{
  if(mBins<=0)  return 0.;
  if (!nHist) return 0;
  r_8 s=0;
  return CleanedMean(s);
}

/********* Methode *********/
/*!
  Retourne le nombre de bins non-nul
*/
int_4 Histo::BinNonNul() const
{
if(mBins<=0)  return -1;
int_4 non=0;
for (int_4 i=0;i<mBins;i++) if( mData[i] != 0. ) non++;
return non;
}

/********* Methode *********/
/*!
  Retourne le nombre de bins ayant une erreur non-nulle
*/
int_4 Histo::ErrNonNul() const
{
if(mErr2==NULL) return -1;
int_4 non=0;
for (int_4 i=0;i<mBins;i++) if( mErr2[i] != 0. ) non++;
return non;
}

/********* Methode *********/
/*!
  Renvoie le numero de bin tel que il y ait "per" pourcent entrees
  entre le bin 0 et ce bin (y compris le contenu de ce bin).
*/
int_4 Histo::BinPercent(r_8 per) const
{
if(mBins<=0)  return -1;
r_8 n = nHist*per;
r_8 s = 0.;
int_4 i;
for(i=0; i<mBins; i++ ) {
  s += mData[i];
  if( s >= n ) break;
}
if( i == mBins ) i = mBins-1;
return i;
}

/********* Methode *********/
/*!
  Renvoie les numeros de bins imin,imax (0=<i<bins) tels que:
  \verbatim
       notons I = bin contenant l'abscisse x
              N1 = nombre d'entrees entre bin 0 et I compris
              N2 = nombre d'entrees entre bin I et bins-1 compris
       imin = bin tel que nombre d'entrees entre imin et I = N1 * per
       imax = bin tel que nombre d'entrees entre I et imax = N2 * per
     Return: <0 si echec
             mMin(I-imin,imax-I) si ok
  \endverbatim
*/
int_4 Histo::BinPercent(r_8 x,r_8 per,int_4& imin,int_4& imax) const
{
if(mBins<=0)  return -3;
imin = imax = -1;
if( per <= 0. ) return -1;

int_4 I = FindBin(x);
if( I<0 || I>=mBins ) return -2;

r_8 N1 = 0.; for(int_4 i=0; i<=I;   i++) N1 += mData[i]; N1 *= per;
r_8 N2 = 0.; {for(int_4 i=I; i<mBins; i++) N2 += mData[i]; N2 *= per;}

r_8 n = 0.;
for(imin=I; imin>=0; imin--)   { n += mData[imin]; if(n>=N1) break; }
if( imin<0 ) imin = 0;
// cout<<"I="<<I<<" imin="<<imin<<" n="<<n<<" N1="<<N1<<endl;

n = 0.;
for(imax=I; imax<mBins; imax++) { n += mData[imax]; if(n>=N2) break; }
if( imax>=mBins ) imax = mBins-1;
// cout<<"I="<<I<<" imax="<<imax<<" n="<<n<<" N2="<<N2<<endl;

return ( imax-I > I-imin ) ? I-imin : imax-I ;
}

/********* Methode *********/
/*!
  Idem precedent mais renvoie xmin et xmax
*/
int_4 Histo::BinPercent(r_8 x,r_8 per,r_8& xmin,r_8& xmax) const
{
xmin = xmax = 0.;
int_4 imin,imax;
int_4 rc = BinPercent(x,per,imin,imax);
if( rc >= 0 ) { xmin = BinLowEdge(imin); xmax = BinHighEdge(imax); }
return rc;
}

/********* Methode *********/
/*!
  Remplace l'histogramme par son integrale normalise a norm:
  si norm <= 0 : pas de normalisation, integration seule
*/
void Histo::HInteg(r_8 norm)
{
if( mBins <= 0 ) return; // createur par default
if(mBins>1)
  for(int_4 i=1; i<mBins; i++) {
    mData[i] += mData[i-1];
    if(mErr2!=NULL) mErr2[i] += mErr2[i-1];
  }
if( norm <= 0. ) return;
norm /= mData[mBins-1];
for(int_4 i=0; i<mBins; i++) {
  mData[i] *= norm;
  if(mErr2!=NULL) mErr2[i] *= norm*norm;
}
}

/********* Methode *********/
/*!
  Remplace l'histogramme par sa derivee
*/
void Histo::HDeriv()
{
if( mBins <= 0 ) return; // createur par default
if( mBins <= 1 ) return;
r_8 *temp = new r_8[mBins];
memcpy(temp, mData, mBins*sizeof(r_8));
if(mBins>=3) for(int_4 i=1; i<mBins-1; i++)
              temp[i] = (mData[i+1]-mData[i-1])/(2.*binWidth);
temp[0] = (mData[1]-mData[0])/binWidth;
temp[mBins-1] = (mData[mBins-1]-mData[mBins-2])/binWidth;
memcpy(mData, temp, mBins*sizeof(r_8));
delete [] temp;
}

/********* Methode *********/
/*!
  Pour rebinner l'histogramme sur nbinew bins
*/
void Histo::HRebin(int_4 nbinew)
{
  if( mBins <= 0 ) return; // createur par default
  if( nbinew <= 0 ) return;

  // memorisation de l'histogramme original
  Histo H(*this);

  // Le nombre de bins est il plus grand ??
  if( nbinew > mBins ) {
    delete [] mData; mData = NULL;
    mData = new r_8[nbinew];
    if( mErr2 != NULL ) {
      delete [] mErr2; mErr2 = NULL;
      mErr2 = new r_8[nbinew];
    }
  }

  // remise en forme de this
  mBins = nbinew;
  this->Zero();
  binWidth = (mMax-mMin)/mBins;
  // On recopie les parametres qui n'ont pas changes
  mUnder = H.mUnder;
  mOver  = H.mOver;


  // Remplissage
  for(int_4 i=0;i<mBins;i++) {
    r_8 xmi = BinLowEdge(i);
    r_8 xma = BinHighEdge(i);
    int_4 imi =  H.FindBin(xmi);
    if( imi < 0 ) imi = 0;
    int_4 ima =  H.FindBin(xma);
    if( ima >= H.mBins ) ima = H.mBins-1;
    r_8 w = 0.;
    if( ima == imi ) {
      w = H(imi) * binWidth/H.binWidth;
    } else {
      w += H(imi) * (H.BinHighEdge(imi)-xmi)/H.binWidth;
      w += H(ima) * (xma -H.BinLowEdge(ima))/H.binWidth;
      if( ima > imi+1 )
          for(int_4 ii=imi+1;ii<ima;ii++) w += H(ii);
    }
    AddBin(i, w);
  }

}

/********* Methode *********/
/*!
  Retourne le nombre de maxima locaux, la valeur du maximum (maxi)
  et sa position (imax), ainsi que la valeur du second maximum
  local (maxn) et sa position (imaxn).
  Attention: si un maximum a la meme valeur sur plusieurs bins
  consecutifs, le bin le plus a droite est pris.
*/
int_4 Histo::MaxiLocal(r_8& maxi,int_4& imax,r_8& maxn,int_4& imaxn) const
{
if(mBins<=0)  return -1;
int_4 nml = 0;
imax = imaxn = -1;
maxi = maxn = -1.;

bool up = true;
bool down = false;
for(int_4 i=0;i<mBins;i++) {
  if( !up ) if( mData[i] > mData[i-1] ) up = true;
  if( up && !down ) {
    if( i == mBins-1 ) down=true;
      else if( mData[i] > mData[i+1] ) down=true;
  }

  if( up && down ) {
    nml++;
    up = down = false;
    if( imax < 0 ) {
      imax = i; maxi = mData[i];
    } else if( mData[i] >= maxi ) {
      imaxn = imax; maxn = maxi;
      imax = i; maxi = mData[i];
    } else {
      if( imaxn < 0 || mData[i] >= maxn ) { imaxn = i; maxn = mData[i]; }
    }
  }

}
return nml;
}

/********* Methode *********/
/*!
  Fit de la position du maximum de l'histo par un polynome
  de degre `degree' a `frac' pourcent du maximum.
  L'histo est suppose etre remplit de valeurs positives
*/
r_8 Histo::FitMax(int_4 degree, r_8 frac, int_4 debug) const
{
  if(mBins<=0)  return mMin;
  if (degree < 2) degree = 2;
  if (degree > 3) degree = 3;
  if (frac <= 0. || frac >= 1.) frac = 0.5;

  if (debug > 1)
    cout<<"Histo::FitMax : Nb Entrees histo ="<<NEntries()<<endl;

  if (NEntries() < 1) throw ParmError(PExcLongMessage(""));

  int_4 iMax = IMax();
  r_8 hmax = (*this)(iMax);
  r_8 xCenter = BinCenter(iMax);

  if(debug>1)
    cout<<"Max histo i="<<iMax<<" x="<<xCenter<<" v="<<hmax<<endl;

  /* find limits at frac*hmax */

  r_8 limit = frac*hmax;

  volatile int_4 iLow = iMax;
  while (iLow>0 && (*this)(iLow)>limit) iLow--;

  volatile int_4 iHigh = iMax;
  while (iHigh<NBins()-1 && (*this)(iHigh)>limit) iHigh++;

  int_4 nLowHigh;
  for(;;) {
    nLowHigh = 0;
    for (int_4 i=iLow; i<=iHigh; i++) if((*this)(i)>0) {
      if(!mErr2) nLowHigh++;
        else if(Error2(i)>0.) nLowHigh++;
    }
    if (debug > 1) cout <<"Limites histo "<<iLow<<" - "<<iHigh
                        <<" ("<<nLowHigh<<" non nuls)"<<endl;
    if( nLowHigh >= degree+1 ) break;
    iLow--;  iHigh++;
    if(iLow<0 && iHigh>=NBins()) {
      if (debug>1)
        cout<<"Mode histogramme = "<<xCenter
            <<"   BinCenter("<<iMax<<")"<<endl;
      return xCenter;
    }
    if(iLow  < 0       ) iLow  = 0;
    if(iHigh >= NBins()) iHigh = NBins()-1;
  }

  TVector<r_8> xFit(nLowHigh);
  TVector<r_8> yFit(nLowHigh);
  TVector<r_8> e2Fit(nLowHigh);
  TVector<r_8> errcoef(1);
  int_4 ii = 0;
  for (int_4 j=iLow; j<=iHigh; j++) {
    if ((*this)(j)>0) {
      if(!mErr2) {
        xFit(ii) = BinCenter(j)-xCenter;
        yFit(ii) = (*this)(j);
        e2Fit(ii) = yFit(ii);
        ii++;
      } else if(Error2(j)>0.) {
        xFit(ii) = BinCenter(j)-xCenter;
        yFit(ii) = (*this)(j);
        e2Fit(ii) = Error2(j);
        ii++;
      }
    }
  }
  if(debug>4) {
    int_4 k;
    for(k=0;k<nLowHigh;k++) cout<<" "<<xFit(k);  cout<<endl;
    for(k=0;k<nLowHigh;k++) cout<<" "<<yFit(k);  cout<<endl;
    for(k=0;k<nLowHigh;k++) cout<<" "<<e2Fit(k); cout<<endl;
  }
  if( ii != nLowHigh ) throw ParmError(PExcLongMessage(""));
  Poly pol(degree);
  pol.Fit(xFit, yFit, e2Fit, degree, errcoef);
  if (debug>1) cout << "resultat fit = " << pol << endl;
  pol.Derivate();

  int_4 DPolDeg = pol.Degre();
  r_8 fd = 0.;

  if (DPolDeg == 0) {
    // on est dans le cas d'un fit de droite
    if( pol[0] > 0. ) {
      // on a fitte une droite de pente >0.
      fd = xFit(nLowHigh-1) + binWidth/2. + xCenter;
    } else if( pol[0] < 0. ) {
      // on a fitte une droite de pente <0.
      fd = xFit(0) - binWidth/2. + xCenter;
    } else {
      // on a fitte une droite de pente =0. (creneau)
      fd = (xFit(0)+xFit(nLowHigh-1))/2. + xCenter;
    }
  } else if (DPolDeg == 1) {
    // on est dans le cas d'un fit de parabole
    r_8 r=0;
    if (pol.Root1(r)==0) throw ParmError(PExcLongMessage(""));
    fd = r + xCenter;
  } else if (DPolDeg == 2) {
    // on est dans le cas d'un fit de cubique
    r_8 r1=0;
    r_8 r2=0;
    if (pol.Root2(r1,r2) == 0) throw ParmError(PExcLongMessage(""));
    pol.Derivate();
    fd = (pol(r1)<0) ? r1 + xCenter : r2 + xCenter;
  } else {
    // on est dans un cas non prevu
    throw ParmError(PExcLongMessage(""));
  }

  if(fd>mMax) fd = mMax;
  if(fd<mMin) fd = mMin;

  if (debug>1)
    cout << "Mode histogramme = " << fd
         << "    (DerPol.degre =" << DPolDeg
         << " pol.coeff[0] =" << pol[0]
         << ")" << endl;

  return fd;
}


/********* Methode *********/
/*!
  Calcul de la largeur a frac pourcent du maximum
  autour du bin du maximum.
  L'histo est suppose etre remplit de valeurs positives
*/
r_8 Histo::FindWidth(r_8 frac, int_4 debug) const
{
  if(mBins<=0)  return 0;
  r_8 xmax = BinCenter( IMax() );
  return FindWidth(xmax,frac,debug);
}

/********* Methode *********/
/*!
  Calcul de la largeur a frac pourcent de la valeur du bin xmax.
  L'histo est suppose etre remplit de valeurs positives
*/
r_8 Histo::FindWidth(r_8 xmax,r_8 frac, int_4 debug) const
{
  if(mBins<=0)  return 0;
  if (frac <= 0 || frac >= 1.) frac = 0.5;

  if (debug > 1)
    cout << "Histo::FindWidth a " << frac
         << " de xmax= " << xmax
         << " , ndata= " << NData()
         << " , nent= " << NEntries()
         << " , nbin= " << NBins() << endl;

  if (NEntries() < 1) throw ParmError(PExcLongMessage(""));
  if (NBins() < 3) throw ParmError(PExcLongMessage(""));

  int_4 iMax = FindBin(xmax);
  if (iMax<0 || iMax>=NBins()) throw ParmError(PExcLongMessage(""));
  r_8 hmax = mData[iMax];
  r_8 limit = frac*hmax;
  if (debug > 1)
    cout << "  Max histo[" << iMax << "] = " << hmax
         << ", limite " << limit << endl;

  int_4 iLow = iMax;
  while (iLow>=0 && mData[iLow]>limit) iLow--;
  if( iLow < 0 ) iLow = 0;

  int_4 iHigh = iMax;
  while (iHigh<NBins() && mData[iHigh]>limit) iHigh++;
  if( iHigh >=NBins() ) iHigh = NBins()-1;

  r_8 xlow   = BinCenter(iLow);
  r_8 ylow   = mData[iLow];

  r_8 xlow1=xlow, ylow1=ylow;
  if(iLow+1<NBins()) {
    xlow1  = BinCenter(iLow+1);
    ylow1  = mData[iLow+1];
  }

  r_8 xhigh  = BinCenter(iHigh);
  r_8 yhigh  = mData[iHigh];

  r_8 xhigh1=xhigh, yhigh1=yhigh;
  if(iHigh-1>=0) {
    xhigh1 = BinCenter(iHigh-1);
    yhigh1 = mData[iHigh-1];
  }

  r_8 xlpred,xhpred,wd;

  if(ylow1>ylow) {
    xlpred = xlow + (xlow1-xlow)/(ylow1-ylow)*(limit-ylow);
    if(xlpred < xlow) xlpred = xlow;
  } else xlpred = xlow;

  if(yhigh1>yhigh) {
    xhpred = xhigh + (xhigh1-xhigh)/(yhigh1-yhigh)*(limit-yhigh);
    if(xhpred > xhigh) xhpred = xhigh;
  } else xhpred = xhigh;

  wd = xhpred - xlpred;

  if (debug > 1) {
    cout << "Limites histo: " << "  Width " << wd << endl;
    cout << "  Low: [" << iLow  << "]=" << ylow << " pred-> " << xlpred
         << " - High: [" << iHigh  << "]=" << yhigh << " pred-> " << xhpred
         << endl;
  }

  return wd;
}


/********* Methode *********/
/*!
  Cf suivant mais im est le bin du maximum de l'histo
*/
int_4 Histo::EstimeMax(r_8& xm,int_4 SzPav) const
{
int_4 im = IMax();
return EstimeMax(im,xm,SzPav);
}

/********* Methode *********/
/*!
  Determine l'abscisses du maximum donne par im
  en moyennant dans un pave SzPav  autour du maximum
  \verbatim
     Return:
        0 = si fit maximum reussi avec SzPav pixels
        1 = si fit maximum reussi avec moins que SzPav pixels
        2 = si fit maximum echoue et renvoit BinCenter()
       -1 = si echec: SzPav <= 0 ou im hors limites
  \endverbatim
*/
int_4 Histo::EstimeMax(int_4& im,r_8& xm,int_4 SzPav) const
{
if(mBins<=0)  return -1;
xm = 0;
if( SzPav <= 0 ) return -1;
if( im < 0 || im >= mBins ) return -1;

if( SzPav%2 == 0 ) SzPav++;
SzPav = (SzPav-1)/2;

int_4 rc = 0;
r_8 dxm = 0, wx = 0;
for(int_4 i=im-SzPav;i<=im+SzPav;i++) {
  if( i<0 || i>= mBins ) {rc=1; continue;}
  dxm += BinCenter(i) * (*this)(i);
  wx  += (*this)(i);
}

if( wx > 0. ) {
  xm = dxm/wx;
  return rc;
} else {
  xm = BinCenter(im);
  return 2;
}

}

/********* Methode *********/
/*!
  Determine la largeur a frac% du maximum a gauche (widthG)
  et a droite (widthD)
*/
void Histo::EstimeWidthS(r_8 frac,r_8& widthG,r_8& widthD) const
{
if(mBins<=0) {frac=0.; return;}
int_4 i;
widthG = widthD = -1.;
if( mBins<=1 || frac<=0. || frac>=1. ) return;

int_4 imax = 0;
r_8 maxi = mData[0];
for(i=1;i<mBins;i++) if(mData[i]>maxi) {imax=i; maxi=mData[i];}
r_8 xmax = BinCenter(imax);
r_8 maxf = maxi * frac;

// recherche du sigma a gauche
widthG = 0.;
for(i=imax;i>=0;i--) if( mData[i] <= maxf ) break;
if(i<0) i=0;
if(i<imax) {
  if( mData[i+1] != mData[i] ) {
    widthG = BinCenter(i) + binWidth
           * (maxf-mData[i])/(mData[i+1]-mData[i]);
    widthG = xmax - widthG;
  } else widthG = xmax - BinCenter(i);
}

// recherche du sigma a droite
widthD = 0.;
for(i=imax;i<mBins;i++) if( mData[i] <= maxf ) break;
if(i>=mBins) i=mBins-1;
if(i>imax) {
  if( mData[i] != mData[i-1] ) {
    widthD = BinCenter(i) - binWidth
           * (maxf-mData[i])/(mData[i-1]-mData[i]);
    widthD -= xmax;
  } else widthD = BinCenter(i) - xmax;
}

}

/********* Methode *********/
/*!
  Impression de l'histogramme
  \verbatim
      hdyn = nombre de colonnes pour imprimer les etoiles
          si =0 alors defaut(100),
          si <0 pas de print histo seulement infos
      hmin = minimum de la dynamique
      hmax = maximum de la dynamique
          si hmax<=hmin           : hmin=VMin() hmax=VMax()
          si hmax<=hmin et hmin=0 : hmin=0 hmax=VMax()
          sinon : hmin hmax
      pflag < 0 : on imprime les informations (nbin,min,...) sans l'histogramme
            = 0 : on imprime  "BinCenter(i) mData[i]"  (note "... ...")
            bit 0 on : (v=1): numero_bin "... ..."
            bit 1 on : (v=2): "... ..." erreur
  \endverbatim
*/

void Histo::Show(ostream & os) const
{
 os << " Histo::Show  "
      << " nHist=" << nHist << "  nEntries=" << nEntries
      << "  mUnder=" << mUnder << "  mOver=" << mOver << endl;
 os << "       mBins=" << mBins
      << "  min=" << mMin << "  mMax=" << mMax
      << "  binWidth=" << binWidth << endl;
 os << "       mean=" << Mean() << " r.m.s=" << Sigma() 
      << "  Errors="<<HasErrors()<< endl;
}

void Histo::Print(int_4 hdyn,r_8 hmin, r_8 hmax,int_4 pflag,
	          int_4 il, int_4 ih) const
{
 
 if(mBins<=0) return;
 if( il > ih ) { il = 0; ih = mBins-1; }
 if( il < 0 ) il = 0;
 if( ih >= mBins ) ih = mBins-1;

 r_8 dhmin =  hmin;
 r_8 dhmax =  hmax;
 r_8 hb,hbmn,hbmx;

 if(hdyn==0) hdyn = 100;

 Show(cout);

 if(hdyn<0 || pflag<0 ) return;

 if(dhmax<=dhmin) { if(hmin != 0.) dhmin =  VMin(); else dhmin=0.;
                  dhmax =  VMax(); }
 if(dhmin>dhmax) return;
 if(dhmin==dhmax) {dhmin -= 1.; dhmax += 1.;}
 r_8 wb = (dhmax-dhmin) /  hdyn;

 // determination de la position de la valeur zero
 int_4 i0 = (int_4)(-dhmin/wb);

 // si le zero est dans la dynamique,
 //  il doit imperativement etre une limite de bin
 if( 0 <= i0 && i0 < hdyn ) {
   hbmn = dhmin + i0*wb;
   hbmx = hbmn + wb;
   if( hbmn != 0. ) {
     hbmn *= -1.;
     if( hbmn < hbmx ) {
       // le zero est + pres du bord negatif du bin
       dhmin += hbmn;
       dhmax += hbmn;
     } else {
       // le zero est + pres du bord positif du bin
       dhmin -= hbmx;
       dhmax -= hbmx;
     }
     wb = (dhmax-dhmin) /  hdyn;
     i0 = (int_4)(-dhmin/wb);
   }
 }

 cout <<"       bin minimum="<<dhmin<<"   bin maximum="<<dhmax
      <<"   binw="<<wb<< endl;

 char* s = new char[hdyn+1];
 s[hdyn] = '\0';

 // premiere ligne
 {for(int_4 i=0;i<hdyn;i++) s[i] = '=';}
 if( 0 <= i0 && i0 < hdyn ) s[i0] = '0';
 if(pflag&1) printf("====");
 printf("======================");
 if(pflag&2 && mErr2!=NULL) printf("===========");
 printf("==%s\n",s);

 // histogramme
 {for(int_4 i=il;i<=ih;i++) {
   for(int_4 k=0;k<hdyn;k++) s[k] = ' ';
   hb = (*this)(i);

   //choix du bin (hdyn bin entre [dhmin,dhmax[
   int_4 ii = (int_4)( (hb-dhmin)/wb );
   if(ii<0) ii = 0; else if(ii>=hdyn) ii = hdyn-1;

   // limite du bin
   hbmn = dhmin + ii*wb;
   hbmx = hbmn + wb;

   // remplissage de s[] en tenant compte des courbes +/-
   if(i0<0) {
     // courbe entierement positive
     for(int_4 k=0;k<=ii;k++) s[k] = 'X';
   } else if(i0>=hdyn) {
     // courbe entierement negative
     for(int_4 k=hdyn-1;k>=ii;k--) s[k] = 'X';
   } else {
     // courbe positive et negative
     s[i0] = '|';
     if(ii>i0)        for(int_4 k=i0+1;k<=ii;k++) s[k] = 'X';
       else if(ii<i0) for(int_4 k=ii;k<i0;k++)    s[k] = 'X';
          else s[ii] = 'X';
   }

   // valeur a mettre dans le bin le plus haut/bas
   int_4 ib;
   if(hb>0.) ib = (int_4)( 10.*(hb-hbmn)/(hbmx-hbmn) );
     else if(hb<0.) ib = (int_4)( 10.*(hbmx-hb)/(hbmx-hbmn) );
       else ib = -1;
   if(ib==-1) s[ii] = '|';
     else if(ib==0) s[ii] = '.';
       else if(ib>0 && ib<10) s[ii] = (char)((int_4) '0' + ib);

   // traitement des saturations +/-
   if( hb < dhmin ) s[0] = '*';
    else if( hb > dhmax ) s[hdyn-1] = '*';

   if(pflag&1) printf("%3d ",i);
   printf("%10.4g %10.4g ",BinCenter(i),hb);
   if(pflag&2 && mErr2!=NULL) printf("%10.4g ",Error(i));
   printf("= %s\n",s);
 }}

 // derniere ligne
 for(int_4 i=0;i<hdyn;i++) s[i] = '=';
 if( 0 <= i0 && i0 < hdyn ) s[i0] = '0';
 if(pflag&1) printf("====");
 printf("======================");
 if(pflag&2 && mErr2!=NULL) printf("===========");
 printf("==%s\n",s);

 // valeur basse des bins (sur ["ndig-1" digits + signe] =  ndig char (>=3))
 const int_4 ndig = 7;
 char sn[2*ndig+10];
 hb = ( fabs(dhmin) > fabs(dhmax) ) ? fabs(dhmin) : fabs(dhmax);
 int_4 n;
 if( hb > 0. ) n = (int_4)(log10(hb*1.00000001)); else n = 1;
 r_8 scaledig = pow(10.,(r_8) ndig-2);
 r_8 expo = scaledig/pow(10.,(r_8) n);
 // cout <<"n="<<n<<" hb="<<hb<<" scaledig="<<scaledig<<" expo="<<expo<<endl;
 for(int_4 k=0;k<ndig;k++) {
   for(int_4 i=0;i<hdyn;i++) s[i] = ' ';
   {for(int_4 i=0;i<hdyn;i++) {
     n = (int_4)( (dhmin + i*wb)*expo );
     for(int_4 j=0;j<2*ndig+10;j++) sn[j] = ' ';
     sprintf(sn,"%d%c",n,'\0');
     strip(sn,'B',' ');
     // cout <<"n="<<n<<" sn=("<<sn<<")  l="<<strlen(sn)<<" k="<<k;
     if( (int_4) strlen(sn) > k ) s[i] = sn[k];
     // cout <<"  s=("<<s<<")"<<endl;
   }}
   if(pflag&1) printf("    ");
   printf("                      ");
   if(pflag&2 && mErr2!=NULL) printf("           ");
   printf("  %s\n",s);
 }
 printf("  (valeurs a multiplier par %.0e)\n",1./expo);

 delete[] s;
}


///////////////////////////////////////////////////////////
// --------------------------------------------------------
//   Les objets delegues pour la gestion de persistance 
// --------------------------------------------------------
///////////////////////////////////////////////////////////


DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<Histo>::ReadSelf(PInPersist& is)
{
char strg[256];

if(dobj==NULL) dobj=new Histo;
  else         dobj->Delete();

// Lecture entete
is.GetLine(strg, 255);
bool nentries_en_8_bytes=false;
if(strg[0]=='V' && strg[1]=='_' && strg[2]=='2') nentries_en_8_bytes=true;
is.GetLine(strg, 255);
is.GetLine(strg, 255);

// Lecture des valeurs
is.Get(dobj->mBins);
if(nentries_en_8_bytes) is.Get(dobj->nEntries);
 else {int_4 dum; is.Get(dum); dobj->nEntries=(uint_8)dum;}
int_4 errok;
is.Get(errok);

is.Get(dobj->binWidth);
is.Get(dobj->mMin);
is.Get(dobj->mMax);
is.Get(dobj->mUnder);
is.Get(dobj->mOver);

is.Get(dobj->nHist);

// Lecture des donnees Histo 1D
is.GetLine(strg, 255);
if(dobj->mBins>0) dobj->mData = new r_8[dobj->mBins];
is.Get(dobj->mData, dobj->mBins);

// Lecture des erreurs
if(errok) {
  is.GetLine(strg, 255);
  if(dobj->mBins>0) dobj->mErr2 = new r_8[dobj->mBins];
  is.Get(dobj->mErr2, dobj->mBins);
}

return;
}

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<Histo>::WriteSelf(POutPersist& os) const
{
if (dobj == NULL)   return;
char strg[256];

// Que faut-il ecrire?
int_4 errok = (dobj->mErr2) ? 1 : 0;

// Ecriture entete pour identifier facilement
// G.Barrand : cast to (unsigned long long) to remove a painfull warning.
sprintf(strg,"V_2 mBins=%6d  NEnt=%15llu  errok=%1d",dobj->mBins,(unsigned long long)dobj->nEntries,errok); //G.Barrand
os.PutLine(strg);
sprintf(strg,"binw=%g  mMin=%g mMax=%g",dobj->binWidth,dobj->mMin,dobj->mMax);
os.PutLine(strg);
sprintf(strg, "mUnder=%g mOver=%g nHist=%g",dobj->mUnder,dobj->mOver,dobj->nHist);
os.PutLine(strg);

// Ecriture des valeurs
os.Put(dobj->mBins);
os.Put(dobj->nEntries);
os.Put(errok);

os.Put(dobj->binWidth);
os.Put(dobj->mMin);
os.Put(dobj->mMax);
os.Put(dobj->mUnder);
os.Put(dobj->mOver);

os.Put(dobj->nHist);

// Ecriture des donnees Histo 1D
sprintf(strg,"Histo: Tableau des donnees %d",dobj->mBins);
os.PutLine(strg);
os.Put(dobj->mData, dobj->mBins);

// Ecriture des erreurs
if(errok) {
  sprintf(strg,"Histo: Tableau des erreurs %d",dobj->mBins);
  os.PutLine(strg);
  os.Put(dobj->mErr2, dobj->mBins);
}

return;
}

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<Histo>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<Histo>;
#endif

} // FIN namespace SOPHYA 
