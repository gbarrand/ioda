#include "machdefs.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "hisprof.h"
#include "perrors.h"

namespace SOPHYA {

/*!
  \class HProf
  \ingroup HiStats
  Classe de profil d'histogrammes.
*/

/********* Methode *********/
/*!
  Constructeur par defaut.
*/
HProf::HProf()
: Histo(), mSumY(NULL), mSumY2(NULL), mSumW(NULL)
, mOk(false), mYMin(1.), mYMax(-1.), mOpt(0)
{
}

/********* Methode *********/
/*!
  Constructeur. Histogramme de profil de ``nBin'' bins entre ``xMin''
  et ``xMax'' avec coupure d'acceptance sur y entre ``yMin'' et ``yMax''.
  Si yMin>=yMax alors pas de coupure d'acceptance sur y.
  Par defaut l'erreur du profil represente la dispersion dans le bin,
  SetErrOpt(1) permet de demander de calculer l'erreur sur la moyenne.
  (see HProf::SetErrOpt for detail of what is computed)
*/
HProf::HProf(r_8 xMin, r_8 xMax, int_4 nBin, r_8 yMin, r_8 yMax)
: Histo(), mSumY(NULL), mSumY2(NULL), mSumW(NULL)
{
  CreateOrResize(xMin,xMax,nBin,yMin,yMax);
}

/********* Methode *********/
/*!
  Constructeur.
*/
HProf::HProf(r_4 xMin, r_4 xMax, int_4 nBin, r_4 yMin, r_4 yMax)
: Histo(), mSumY(NULL), mSumY2(NULL), mSumW(NULL)
{
  CreateOrResize((r_8)xMin,(r_8)xMax,nBin,(r_8)yMin,(r_8)yMax);
}

/********* Methode *********/
/*!
  Constructeur par copie.
*/
HProf::HProf(const HProf& H)
: Histo(H)
, mSumY((H.mBins>0)  ? new r_8[H.mBins] : NULL)
, mSumY2((H.mBins>0) ? new r_8[H.mBins] : NULL)
, mSumW((H.mBins>0)  ? new r_8[H.mBins] : NULL)
, mOk(H.mOk), mYMin(H.mYMin), mYMax(H.mYMax), mOpt(H.mOpt)
{
  if(mBins>0) {
    memcpy(mSumY,  H.mSumY,  mBins*sizeof(r_8));
    memcpy(mSumY2, H.mSumY2, mBins*sizeof(r_8));
    memcpy(mSumW,  H.mSumW,  mBins*sizeof(r_8));
  }
  UpdateHisto();
}

/********* Methode *********/
/*!
  allocation
*/
void HProf::CreateOrResize(r_8 xMin, r_8 xMax, int_4 nBin, r_8 yMin, r_8 yMax)
{
  //cout<<"HProf::CreateOrResize()"<<endl;
  bool samelen = (nBin==mBins)? true: false;
  Histo::CreateOrResize(xMin,xMax,nBin);
  Histo::Errors();
  if(mSumW!=NULL && !samelen) { //si un est alloue alors les autres aussi
    delete[] mSumY;  mSumY = NULL;
    delete[] mSumY2; mSumY2= NULL;
    delete[] mSumW;  mSumW = NULL;
  }
  if(nBin>0 && mSumW==NULL) {
    mSumY  = new r_8[nBin];
    mSumY2 = new r_8[nBin];
    mSumW  = new r_8[nBin];
  }
  if(mSumW) {
    memset(mSumY, 0, nBin*sizeof(r_8));
    memset(mSumY2,0, nBin*sizeof(r_8));
    memset(mSumW, 0, nBin*sizeof(r_8));
  }
  mYMin = yMin; mYMax = yMax;
  mOk = false;
  mOpt = 0;
}

/********* Methode *********/
/*!
  Des-allocation
*/
void HProf::Delete()
{
  //cout<<"HProf::Delete()"<<endl;
  Histo::Delete();
  if(mSumY != NULL) {delete[] mSumY;  mSumY = NULL;}
  if(mSumY2!= NULL) {delete[] mSumY2; mSumY2 = NULL;}
  if(mSumW != NULL) {delete[] mSumW;  mSumW = NULL;}
  mOk = false;
}

/********* Methode *********/
/*!
  Destructeur
*/
HProf::~HProf()
{
  Delete();
}

/********* Methode *********/
/*!
Calcule la partie histogramme du profile si elle n'est pas a jour.
*/
void HProf::UpdateHisto(bool force) const
{
if(!mOk || force) updatehisto();
}

/********* Methode *********/
/*!
  Remise a zero
*/
void HProf::Zero()
{
  Histo::Zero();
  memset(mSumY,  0, mBins*sizeof(r_8));
  memset(mSumY2, 0, mBins*sizeof(r_8));
  memset(mSumW,  0, mBins*sizeof(r_8));
  mOk = false;
}


/********* Methode *********/
/*!
  Pour changer l'option de calcul de l'erreur du profile.
  Si ``spread'' = true alors l'erreur represente la dispersion
  des donnees dans le bin, si = false elle represente l'erreur
  sur la moyenne du bin.
  \verbatim
    - Pour le bin (j):
    H(j) = sum(y*w), E(j) = sum(y^2*w), L(j) = sum(w)
    ->  h(j) = H(j)/L(j)   moyenne dans le bin
    ->  s(j) = sqrt(E(j)/L(j) - (H(j)/L(j))^2)  dispersion dans le bin
    ->  e(j) = s(j)/sqrt(L(j)) erreur sur la moyenne dans le bin
    spread=true:  opt=0 : dispersion des donnees dans le bin = s(j)
    spread=false: opt=1 : erreur sur la moyenne du bin = e(j)
  \endverbatim
*/
void HProf::SetErrOpt(bool spread)
{
uint_2 opt = (spread) ? 0 : 1;
if(opt!=mOpt) {mOpt=opt; mOk=false;}
}

/********* Methode *********/
/*!
  Pour mettre a jour l'histogramme de profil.
  Il est important d'appeler cette methode si on veut
  utiliser les methodes de la classe Histo qui ne sont
  pas redefinies dans la classe HProf.
  En effet, pour des raisons de precision la classe
  HProf travaille avec des tableaux en double precision
  et seulement au moment ou l'on a besoin de l'histogramme
  ce dernier est rempli avec les valeurs demandees (moyenne
  et dispersion/erreur sur la moyenne).
*/
void HProf::updatehisto() const
{
r_8 m,e2;
if(mBins>0) for(int_4 i=0;i<mBins;i++) {
  if(mSumW[i]<=0.) {
    m = e2 = 0.;
  } else {
    m  = mSumY[i]/mSumW[i];
    e2 = mSumY2[i]/mSumW[i] - m*m;
    if(mOpt) e2 /= mSumW[i];
  }
  mData[i] = m;
  mErr2[i] = e2;
}
mOk = true;
// Attention, a cause de "WriteSelf const" updatehisto doit etre "const".
// Comme on veut modifier mOk, on est oblige de faire cette entourloupe:
HProf *buff = (HProf *) this;
buff->mOk = true;
}

/********* Methode *********/
/*!
  Addition au contenu de l'histo pour abscisse x de la valeur y et poids w
*/
void HProf::Add(r_8 x, r_8 y, r_8 w)
{
  if(mYMax>mYMin && (y<mYMin || mYMax<y)) return;
  int_4 numBin = FindBin(x);
  if (numBin<0) mUnder += w;
  else if (numBin>=mBins) mOver += w;
  else {
    mOk = false;
    mSumY[numBin]  += y*w;
    mSumY2[numBin] += y*y*w;
    mSumW[numBin]  += w;
    nHist += w;
    nEntries++;
  }
}

/********* Methode *********/
/*!
  Addition au contenu de l'histo bin numBin de la valeur y poids w
*/
void HProf::AddBin(int_4 numBin, r_8 y, r_8 w)
{
  if(mYMax>mYMin && (y<mYMin || mYMax<y)) return;
  if (numBin<0) mUnder += w;
  else if (numBin>=mBins) mOver += w;
  else {
    mOk = false;
    mSumY[numBin]  += y*w;
    mSumY2[numBin] += y*y*w;
    mSumW[numBin]  += w;
    nHist += w;
    nEntries++;
  }
}

/********* Methode *********/
/*!
  Operateur HProf H = H1
*/
HProf& HProf::operator = (const HProf& h)
{
 if(this == &h) return *this;
 CreateOrResize(h.mMin,h.mMax,h.mBins,h.mYMin,h.mYMax);
 if(mData) memcpy(mData,h.mData,mBins*sizeof(r_8));
 if(mErr2) memcpy(mErr2,h.mErr2,mBins*sizeof(r_8));
 mUnder = h.mUnder; mOver = h.mOver;
 nHist = h.nHist; nEntries = h.nEntries;
 if(mSumY)  memcpy(mSumY,  h.mSumY,  mBins*sizeof(r_8));
 if(mSumY2) memcpy(mSumY2, h.mSumY2, mBins*sizeof(r_8));
 if(mSumW)  memcpy(mSumW,  h.mSumW,  mBins*sizeof(r_8));
 mOk = h.mOk;
 mOpt = h.mOpt;

 return *this;
}

/********* Methode *********/
/*!
  Operateur HProf H += H1

  Attention dans cette addition il n'y a pas de gestion
  des YMin et YMax. L'addition est faite meme si les limites
  en Y de ``a'' sont differentes de celles de ``this''.
*/
HProf& HProf::operator += (const HProf& a)
{
if(mBins!=a.mBins) throw SzMismatchError(PExcLongMessage(""));
Histo *hthis = (Histo *) this;
*hthis += (Histo) a;
if(mBins>0) for(int_4 i=0;i<mBins;i++) {
  mSumY[i]  += a.mSumY[i];
  mSumY2[i] += a.mSumY2[i];
  mSumW[i]  += a.mSumW[i];
}
updatehisto();

return *this;
}

/********* Methode *********/
/*!
  Pour rebinner l'histogramme de profile sur nbinew bins
*/
void HProf::HRebin(int_4 nbinew)
{
  if( mBins <= 0 ) return; // createur par default
  if( nbinew <= 0 ) return;

  // memorisation de l'HProf original
  HProf H(*this);

  // Rebin de la partie Histo
  int_4 binold = mBins;
  Histo::HRebin(nbinew);

  // Le nombre de bins est il plus grand ??
  if( mBins > binold ) {
    delete [] mSumY;  mSumY  = new r_8[nbinew]; memset(mSumY, 0,mBins*sizeof(r_8));
    delete [] mSumY2; mSumY2 = new r_8[nbinew]; memset(mSumY2,0,mBins*sizeof(r_8));
    delete [] mSumW;  mSumW  = new r_8[nbinew]; memset(mSumW, 0,mBins*sizeof(r_8));
  }

  // Remplissage des parties propres au HPprof
  for(int_4 i=0;i<mBins;i++) {
    r_8 xmi = BinLowEdge(i);
    r_8 xma = BinHighEdge(i);
    int_4 imi =  H.FindBin(xmi);
    if( imi < 0 ) imi = 0;
    int_4 ima =  H.FindBin(xma);
    if( ima >= H.mBins ) ima = H.mBins-1;
    r_8 wY = 0., wY2 = 0., wW = 0.;
    if( ima == imi ) {
      wY  = H.mSumY[imi]  * binWidth/H.binWidth;
      wY2 = H.mSumY2[imi] * binWidth/H.binWidth;
      wW  = H.mSumW[imi]  * binWidth/H.binWidth;
    } else {
      wY  += H.mSumY[imi]  * (H.BinHighEdge(imi)-xmi)/H.binWidth;
      wY  += H.mSumY[ima]  * (xma -H.BinLowEdge(ima))/H.binWidth;
      wY2 += H.mSumY2[imi] * (H.BinHighEdge(imi)-xmi)/H.binWidth;
      wY2 += H.mSumY2[ima] * (xma -H.BinLowEdge(ima))/H.binWidth;
      wW  += H.mSumW[imi]  * (H.BinHighEdge(imi)-xmi)/H.binWidth;
      wW  += H.mSumW[ima]  * (xma -H.BinLowEdge(ima))/H.binWidth;
      if(ima>imi+1) for(int_4 ii=imi+1;ii<ima;ii++)
	{wY += H.mSumY[ii]; wY2 += H.mSumY2[ii]; wW += H.mSumW[ii];}
    }
    mSumY[i] += wY; mSumY2[i] += wY2; mSumW[i] += wW;
  }
  // On synchronise les tableaux mSum?? et l'Histogramme
  updatehisto();
}

void HProf::Show(ostream & os) const
{
  UpdateHisto();
  Histo::Show(os);
  os <<"       ymin="<<mYMin<<" ymax="<<mYMax<<"  opt="<<mOpt<< endl;
}

///////////////////////////////////////////////////////////
// --------------------------------------------------------
//   Les objets delegues pour la gestion de persistance 
// --------------------------------------------------------
///////////////////////////////////////////////////////////


DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<HProf>::ReadSelf(PInPersist& is)
{
char strg[256];

if(dobj==NULL) dobj=new HProf;
  else         dobj->Delete();

// Lecture entete
is.GetLine(strg,255);

// Lecture des valeurs
is.Get(dobj->mBins);
is.Get(dobj->mYMin);
is.Get(dobj->mYMax);
is.Get(dobj->mOpt);
dobj->mOk = true;

// Lecture des donnees propres a l'histogramme de profil.
is.GetLine(strg,255);
if(dobj->mBins>0) {
  dobj->mSumY  = new r_8[dobj->mBins];
  dobj->mSumY2 = new r_8[dobj->mBins];
  dobj->mSumW  = new r_8[dobj->mBins];
}
is.Get(dobj->mSumY,  dobj->mBins);
is.Get(dobj->mSumY2, dobj->mBins);
is.Get(dobj->mSumW,  dobj->mBins);

// Lecture de l'histogramme
is >> (Histo&)(*dobj);

return;
}

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<HProf>::WriteSelf(POutPersist& os) const
{
if (dobj == NULL)   return;
char strg[256];

dobj->UpdateHisto();

// Ecriture entete pour identifier facilement
sprintf(strg,"HProf: YMin=%f  YMax=%f  Opt=%1d",dobj->mYMin,dobj->mYMax,dobj->mOpt);
os.PutLine(strg);

// Ecriture des valeurs
os.Put(dobj->mBins);
os.Put(dobj->mYMin);
os.Put(dobj->mYMax);
os.Put(dobj->mOpt);

// Ecriture des donnees propres a l'histogramme de profil.
sprintf(strg,"HProf: SumY SumY2 SumW");
os.PutLine(strg);
os.Put(dobj->mSumY,  dobj->mBins);
os.Put(dobj->mSumY2, dobj->mBins);
os.Put(dobj->mSumW,  dobj->mBins);

// Ecriture de l'histogramme
os << (Histo&)(*dobj);

return;
}

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<HProf>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<HProf>;
#endif

} // FIN namespace SOPHYA 
