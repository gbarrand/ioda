// Gestion de block de donnees avec partage de references
// malheureusement tres mal concu...  C.Magneville 04/99
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA
#include "sopnamsp.h"
#include "machdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <complex>
#include "pexceptions.h"

#define NDATABLOCK_CC_BFILE  // avoid extern template declarations 
#include "ndatablock.h"

#include "thsafeop.h"  //  for ThreadSafe operations (Ref.Count/Share) 

/* ---- Pour renvoyer un identificateur unique ---- */
static uint_8 _ndrefid_ = 0; // Identificateur de NDREF cree 
uint_8 AnyDataObj::getUniqueId()
{
  _ndrefid_++;
  return ( _ndrefid_ );
}

/*!
  \class SOPHYA::NDataBlock
  \ingroup BaseTools
  Management of data blocks
*/

//////////////////////////////////
// Fonctionnement en mode debug //
//////////////////////////////////

template <class T> int    NDataBlock<T>::Debug_NDataBlock = 0;
template <class T> size_t NDataBlock<T>::NallocData       = 0;
template <class T> size_t NDataBlock<T>::NallocSRef       = 0;
template <class T> ThSafeOp* NDataBlock<T>::gThsop        = NULL;

//! Set debug (and level print) for allocation and references debug.
/*!
  \param prtlevel : activate/des-activate debug mode
                    and select print level

  \arg prtlevel <= 0 : no debug
  \arg prtlevel == 1 : debug activated, no print
  \arg prtlevel >=2  : debug activated,
       print infos in all routines that have something to do with
       allocations or des-allocation of datas or references.
 */
template <class T>
void NDataBlock<T>::SetPrintDebug(int prtdbglevel)
{
  Debug_NDataBlock = prtdbglevel;
}

//! Reset debug counter values.
/*!
  \param nallocdata : reset number of allocated data structures to \b nallocdata
  \param nallocsref : reset number of allocated references to \b nallocsref
  \warning In principle this routine should not be use (only experts)
 */
template <class T>
void NDataBlock<T>::ResetDebug(size_t nallocdata, size_t nallocsref)
{
NallocData = nallocdata;
NallocSRef = nallocsref;
}

//! Print debug current status.
/*!
  Print debug current status for number of allocated
  data structures and number of allocated references.
 */
template <class T>
void NDataBlock<T>::PrintDebug()
{
cout<<"... ... ... NallocData = "<<NallocData
    <<"  ,  NallocSRef = "<<NallocSRef
    <<" ... ... ..."<<endl;
}

///////////////////////////
// Createur, Destructeur //
///////////////////////////

//! Constructor for \b n datas. if \b zero=true, filled with zeros
template <class T>
NDataBlock<T>::NDataBlock(size_t n, bool fzero)
// Createur d'une structure de "n" donnees
  : mSz(0), mSRef(NULL), mIsTemp(false)
{
if(Debug_NDataBlock>1)
  cout<<"?_NDataBlock::NDataBlock("<<this<<",n="<<n<<")"<<endl;
if (gThsop == NULL) gThsop = new ThSafeOp;

Alloc(n, NULL, NULL, fzero);   // allocation et mise a zero
}

//! Constructor for \b n datas shared with external
/*!
  Datas are previously allocated by an other external source.
  \warning This require particular care (see Alloc)
  \sa Alloc
 */
template <class T>
NDataBlock<T>::NDataBlock(size_t n, T* data, Bridge* br)
// Createur d'une structure de "n" donnees, avec donnees preallouees.
// Attention createur TRES DANGEREUX (Voir explications dans Alloc()).
: mSz(0), mSRef(NULL), mIsTemp(false)
{
if(Debug_NDataBlock>1)
  cout<<"?_NDataBlock::NDataBlock("<<this
      <<",data="<<data<<",br="<<br<<")"<<endl;
if (gThsop == NULL) gThsop = new ThSafeOp;

Alloc(n,data,br);
}

//! Default constructor
template <class T>
NDataBlock<T>::NDataBlock()
// Createur par default
: mSz(0), mSRef(NULL), mIsTemp(false)
{
if(Debug_NDataBlock>1)
  cout<<"?_NDataBlock::NDataBlock("<<this<<") default"<<endl;
if (gThsop == NULL) gThsop = new ThSafeOp;
}

//! Copy constructor
/*!
  \warning datas are \b SHARED with \b a, but NOT other associated attributes. In particular 
  the associated physical unit, if defined, is copied, NOT shared.
 */
template <class T>
NDataBlock<T>::NDataBlock(const NDataBlock<T>& a)
// Createur par copie: partage les donnees dans tous les cas
: mSz(0), mSRef(NULL), mIsTemp(false)
{
if(Debug_NDataBlock>1)
  cout<<"?_NDataBlock::NDataBlock("<<this<<",&a="<<&a<<" a.mSz="<<a.mSz<<")"<<endl;

if(a.mSRef && a.mSz>0) Share(a);
}

//! Copy constructor with \b share option
/*!
  \warning datas are shared if \b share is \b true, cloned if not. The associated physical unit,
  if defined is copied, NOT shared in all cases.
 */
template <class T>
NDataBlock<T>::NDataBlock(const NDataBlock<T>& a,bool share)
// Createur avec choix de partager ou non selon "share"
: mSz(0), mSRef(NULL), mIsTemp(false)
{
if(Debug_NDataBlock>1)
  cout<<"?_NDataBlock::NDataBlock("<<this<<",&a="<<&a
      <<",sh=<<"<<share<<")"<<endl;

if(a.mSRef && a.mSz>0) {if(share) Share(a); else Clone(a);}
}

//! Destructor
template <class T>
NDataBlock<T>::~NDataBlock()
// Destructeur
{
if(Debug_NDataBlock>1)
  cout<<"?_NDataBlock::~NDataBlock("<<this<<")"<<endl;

Dealloc();  // ThreadSafe version of Delete()
}

////////////////////////
// Gestion de donnees //
////////////////////////

//! Clone datas from \b a
template <class T>
void NDataBlock<T>::Clone(const NDataBlock<T>& a)
// Clone: copie de donnees a partir de "a"
{
if(Debug_NDataBlock>1)
  cout<<"?_NDataBlock::Clone("<<this<<","<<&a<<") a.(mSz="
      <<a.mSz<<" mSRef="<<a.mSRef<<" IsTemp="<<a.IsTemp()
      <<"), mSz="<<mSz<<" mSRef="<<mSRef<<" IsTemp="<<mIsTemp<<endl;

//G.Barrand if(&a==NULL) throw(NullPtrError("NDataBlock::Clone  &a==NULL\n"));
if(!a.mSRef || a.mSz==0) throw(NullPtrError("NDataBlock::Clone a.mSz=0\n"));
Alloc(a.mSz, NULL, NULL, false);  // pas de mise a zero 
memcpy(Data(),a.Data(),mSz*sizeof(T));
}

//! Share datas with \b a
template <class T>
void NDataBlock<T>::Share(const NDataBlock<T>& a)
// Share: Partage les donnees avec "a"
{
if(Debug_NDataBlock>1) {
  cout<<"?_NDataBlock::Share("<<this<<","<<&a<<")";
  //G.Barrand if(&a!=NULL) 
  cout<<" a.(mSz="<<a.mSz<<" mSRef="<<a.mSRef
                   <<" IsTemp="<<a.IsTemp()<<")";
  cout<<", mSz="<<mSz<<" mSRef="<<mSRef<<" IsTemp="<<mIsTemp<<endl;
}

//G.Barrand if(&a==NULL) throw(NullPtrError("NDataBlock::Share  &a==NULL\n"));
if(!a.mSRef || a.mSz==0) throw(NullPtrError("NDataBlock::Share a.mSz=0\n"));
//--- Start of atomic (in one block) operation for thread safety 
gThsop->lock();   //  (ThreadSafe)
if(mSRef) Delete();
mSz = a.mSz; mSRef = a.mSRef; mSRef->nref++;
gThsop->unlock();   //  (ThreadSafe)
//--- End of atomic operation 

if(Debug_NDataBlock>1)
  cout<<"...?_NDataBlock::Share mSz="<<mSz<<" mSRef="<<mSRef
      <<" mSRef->nref="<<mSRef->nref<<" mSRef->data="<< mSRef->data
      <<" mSRef->bridge="<<mSRef->bridge
      <<" IsTemp="<<mIsTemp<<endl;
}

//! \b Share with \b a if \b temporary, \b clone from \b a if not.
/*! \warning For most purposes, users don't have to worry with
             the "temporary" nature of a NDataBlock. That is used
             internaly to avoid memory allocation in operation
             like A = B + C + D for instance. The method is not
             protected to allow users to write complicated functions
             on NDataBlock.
  \verbatim
  ----------------------------------------------------------
  Pourquoi une complication avec la notion de "temporaire" :
  ----------------------------------------------------------
  - Le constructeur par copie partageant les donnees,
    dans une methode un { NDataBlock<T> result; ...; return result;}
    ne va pas allouer de la memoire pour retourner "result".
  - La gestion de temporaire sert quand on enchaine plusieurs
    operations sur la meme ligne, par exemple : A = B+C+D;
    Dans ce cas l'objet CD=C+D est d'abord alloue et rempli
    avec C+D, puis CD est mis a "temporaire".
    Quand on ajoute B a CD, la methode d'addition va se rendre compte
    que CD est "temporaire" et additionner B "in-place" dans CD
    sans allouer une fois de plus de la place (pas d'allocation
    de place BCD pour mettre B+CD mais une operation CD += B).
    Si la notion d'objet "temporaire" n'avait pas ete consideree
    l'addition A = B+C+D aurait alloue de la place pour "CD=C+D"
    puis pour BCD=B+CD : 2 allocations auraient ete necessaires
    contre 1 seule dans notre cas de geston de "temporaire".
  \endverbatim
*/
template <class T>
void NDataBlock<T>::CloneOrShare(const NDataBlock<T>& a)
// CloneOrShare: Share si "a" temporaire, Clone sinon.
{
if(Debug_NDataBlock>1)
  cout<<"?_NDataBlock::CloneOrShare("<<this<<","<<&a<<")"<<endl;

//G.Barrand if(&a==NULL) throw(NullPtrError("NDataBlock::CloneOrShare  &a==NULL\n"));
if(a.IsTemp()) Share(a); else Clone(a);
}

////////////////////////////////////////////////////////////
// Allocation , destruction , remplissage et reallocation //
////////////////////////////////////////////////////////////

//! Allocation management
/*!
   Allocation d'un NOUVEL espace de stoquage de "n" donnees
   \verbatim
   Si data==NULL : allocation de l'espace memoire 
      si zero == true , l'espace est remplis de zeros
      data!=NULL : partage des donnees avec l'adresse data
   Si br==NULL   : les donnees nous appartiennent
      br!=NULL   : les donnees ne nous appartiennent pas (ex: Blitz)
  
   Exemple: on veut connecter a un tableau de T*
   > float *x = new float[5]; ... remplissage de x[] ...;
   1- On veut que NDataBlock NE DESALLOUE PAS le tableau "x[]"
      a- Premiere solution
         > NDataBlock A(5,x,new Bridge);
         ......
         > delete [] x;
            - Il faut deleter x[] explicitement.
            - Le destructeur de "A" ne detruit pas x[].
            ATTENTION: Une fois x[] detruit, "A" ne peut
                       plus acceder les donnees!
            - Bridge est detruit par le destructeur de "A"
      b- Autre solution:
         > NDataBlock A(5); A.FillFrom(5,x);
         > delete [] x;
         ......
            - Il faut deleter x[] explicitement.
            - "A" possede une copie en local de x[].
            - Le destructeur de "A" ne detruit pas x[] mais la copie locale.
   2- On veut que NDataBlock desalloue le tableau
         > NDataBlock A(5,x);
            - Ne Pas Faire "delete [] x;"
            - "A" partage les donnees avec x[].
            - Le destructeur de "A" detruit x[].
  
   --- REMARQUE SUR LE DANGER DE CERTAINES SITUATIONS (CMV):
   1-/ x = new float[n1]; NDataBlock A(n2,x);
       1er danger: si n2>n1 depassement de tableaux (core dump)
       2sd danger: celui qui alloue x[] ne doit pas faire le "delete"
                   en desaccord avec toutes les regles de bonne conduite.
   2-/ float x[5]={1,2,3,4,5}; {NDataBlock A(n2,&x[0]);} cout<<x[2];
       Ici, a la sortie du bloc {}, le destructeur de "A" va detruire
       l'adresse de &x[0]: je n'ose imaginer que ca se fasse sans probleme
       et de toute facon, cout<<x[2]; va surement faire des etincelles.
   3-/ x = new float[n1]; NDataBlock A(n2,x,new Bridge);
       1er danger: si n2>n1 depassement de tableaux (core dump)
       2sd danger: si la methode bridgee (blitz?) detruit x[]
                   "A" n'a plus de donnees connectees!
   --- CONCLUSION
   Cette classe est franchement merdique.
   - On peut accepter la prise de risque liee a NDataBlock(n2,x,new Bridge);
     car je ne vois pas comment on pourrait faire autrement pour connecter
     un tableau de type blitz par exemple.
   - Par contre le createur NDataBlock(n2,x); doit etre interdit
     dans sa forme actelle car trop dangereux et il me semble inutile.
   - Dans cette nouvelle optique:
     NDataBlock(n2,x,new Bridge) et NDataBlock(n2,x) disparaissent
     On remplace par NDataBlock(n2,x) {Alloc(n2,x,new Bridge);}
        qui force le Bridge dans tout les cas puisque NDataBlock
        ne possede pas les donnees.
     Mais puis-je encore le faire vu que NDataBlock est a la base
     de TVector,TMatrix et qu'il faut donc reprendre tout le code DPC
   - Quoiqu'il arrive Alloc est une methode privee et peut donc rester
     sous sa forme actuelle.
              
   \endverbatim
 */


template <class T>
void NDataBlock<T>::Alloc(size_t n,T* data,Bridge* br,bool zero)
{
if(Debug_NDataBlock>1)
  cout<<"?_NDataBlock::Alloc("<<this<<","
      <<n<<","<<data<<","<<br<<") mSz="<<mSz
      <<" mSRef="<<mSRef<<" IsTemp="<<mIsTemp<<endl;

if(br && !data)
  throw(NullPtrError("NDataBlock::Alloc br!=NULL && data==NULL\n"));
if(n==0) throw(SzMismatchError("NDataBlock::Alloc n==0\n"));
//--- Start of atomic (in one block) operation for thread safety (ThreadSafe)
gThsop->lock();   //  (ThreadSafe)
if(mSRef) Delete();
mSz = n;
mSRef = new NDREF;
mSRef->nref = 1;
mSRef->dsid = AnyDataObj::getUniqueId(); 
if(data) mSRef->data = data;
else {mSRef->data = new T[n]; if (zero) memset(mSRef->data,0,n*sizeof(T));}
mSRef->bridge = br;
mSRef->unit_p = NULL;
gThsop->unlock();   //  (ThreadSafe)
//--- End of atomic operation (ThreadSafe)

if(Debug_NDataBlock>0) {
  // Meme dans le cas data!=0 et br==0 (connexion d'un tableau
  // avec destruction geree par ~NDataBlock (cas 2-) on compte
  // comme si on avait fait une allocation du tableau (ce qui a ete
  // fait au niveau du dessus!).
  if(!br) NallocData++; NallocSRef++;
  if(Debug_NDataBlock>1)
    cout<<"...?_NDataBlock::Alloc mSz="<<mSz<<" mSRef="<<mSRef
        <<" mSRef->nref="<<mSRef->nref<<" mSRef->data="<<mSRef->data
        <<" mSRef->bridge="<<mSRef->bridge
        <<" IsTemp="<<mIsTemp
        <<" Total("<<NallocData<<","<<NallocSRef<<")"<<endl;
}
}

//! Management of de-allocation (NOT thread-safe)
template <class T>
void NDataBlock<T>::Delete(void)
// Pour detruire les pointeurs en tenant compte des references
{
if(Debug_NDataBlock>1) {
  cout<<"?_NDataBlock::Delete("<<this<<") mSz="<<mSz
      <<" mSRef="<<mSRef<<" IsTemp="<<mIsTemp;
  if(mSRef)
    cout<<" mSRef->nref="<<mSRef->nref<<" mSRef->data="
        <<mSRef->data<<" mSRef->bridge="<<mSRef->bridge;
  cout<<endl;
}

if(mSRef==NULL) return;

mSRef->nref--;
if(mSRef->nref != 0) {

if(Debug_NDataBlock>1)
  cout<<"...?_NDataBlock::Delete() pas de desallocation il reste nref="
      <<mSRef->nref<<" Total("<<NallocData<<","<<NallocSRef<<")"<<endl;

  mSz = 0; mSRef=NULL;
  return;
}

if(Debug_NDataBlock>0) {
  if(!mSRef->bridge) NallocData--; NallocSRef--;
  if(Debug_NDataBlock>1)
    cout<<"...?_NDataBlock::Delete() desallocation complete il reste nref="
        <<mSRef->nref<<" Total("<<NallocData<<","<<NallocSRef<<")"<<endl;
}

// Si il y a un Bridge les donnees ne n'appartiennent pas, on detruit le Bridge
// sinon, les donnees ont ete allouees par nos soins, on libere l'espace
if(mSRef->bridge) {
  if(Debug_NDataBlock>1)
    cout<<"...?_NDataBlock::Delete() Bridge "<<mSRef->bridge<<" deleted"<<endl;
  delete mSRef->bridge;
} else {
  if(Debug_NDataBlock>1)
    cout<<"...?_NDataBlock::Delete() data "<<mSRef->data<<" deleted"<<endl;
  delete [] mSRef->data;
}
mSRef->bridge=NULL; mSRef->data=NULL;
if(mSRef->unit_p)  delete mSRef->unit_p;  // delete de l'objet Units si alloue 
delete mSRef; mSRef=NULL; mSz = 0;
}

//! Fill dats of this NDataBlock with the \b n datas pointed by \b data
/*!
  \warning If class empty : allocate space in memory
  \warning If class already connected : overwrite with minimum size
           (\b n or \b mSz)
 */
template <class T>
void NDataBlock<T>::FillFrom(size_t n,T* data)
// Remplissage par un tableau de donnees
// - Si classe vide : creation de l'espace memoire
// - Si classe connectee : on ecrit selon la longueur minimale
//                         (cad this->mSz ou "n")
{
if(data==NULL) throw(NullPtrError("NDataBlock::FillFrom  data==NULL\n"));
if(n==0) throw(ParmError("NDataBlock::FillFrom  n<=0\n"));
if(mSRef==NULL) Alloc(n, NULL, NULL, false);  // Pas de mise a zero
if(mSz<n) n = mSz;
memcpy(Data(),data,n*sizeof(T));
}

//! Re-allocate space for \b nnew datas
/*!
  \param nnnew : new size
  \param force : to manage the way re-allocation will be done (see after).
  \verbatim
  Re-allocation de "nnew" place memoire pour les donnees
  avec conservation des "nold" donnees precedentes si possible.
  "force" gere la re-allocation de la place memoire pour les donnees.
  Divers cas se presentent:
  a-/ *** nnew>nold force=quelconque ***
      place re-allouee, donnees [0,nold[ copiees, surplus [nold,new[ mis a zero
  b-/ *** nnew<=nold force=true ***
      place re-allouee, donnees [0,nnew[ copiees, pas de surplus
  c-/ *** nnew<=nold force=false ***
      place non re-allouee, seule la valeur de la taille est diminuee
  - On tient compte du partage des donnees dans tous les cas.
  - Si il n'y a pas de donnees connectees a la classe, on re-alloue
    dans tous les cas
  \endverbatim
 */
template <class T>
void NDataBlock<T>::Realloc(size_t nnew,bool force)
{
if(nnew==0) throw(ParmError("NDataBlock::Realloc  n<=0\n"));

// Cas sans re-allocation memoire
if(mSRef && nnew<=mSz && ! force) { mSz=nnew; return;}

// Cas avec re-allocation memoire
size_t ncop;
if(!mSRef || mSz==0) ncop=0; else if(mSz<nnew) ncop=mSz; else ncop=nnew;
T* dataloc = new T[nnew];
if(ncop>0) memcpy(dataloc,mSRef->data,ncop*sizeof(T));
if(nnew>ncop) memset(dataloc+ncop,0,(nnew-ncop)*sizeof(T));
Alloc(nnew,dataloc,NULL); //Alloc gere partage de reference et bridge
}

/*! 
  \brief Calls the protected Delete() method to set the size to zero
  This is the public - thread safe version - of the Delete() method
  The memory is freed if last referenced structure. 
*/
template <class T>
void NDataBlock<T>::Dealloc()
{
  gThsop->lock();
  Delete();
  gThsop->unlock();
}

//--- Associated Units management 
template <class T>
void NDataBlock<T>::SetUnits(const Units& un)
{
  if (!mSRef)  return;
  gThsop->lock();
  if (!mSRef->unit_p)  mSRef->unit_p=new Units(un);
  else *(mSRef->unit_p)=un;
  gThsop->unlock();
}

template <class T>
Units NDataBlock<T>::GetUnits(bool& sdone) const
{
  if (mSRef && mSRef->unit_p)  { sdone=true;  return *(mSRef->unit_p);  }
  sdone=false;
  return Units();
}

////////////////
// Impression //
////////////////

//! Give infos and print \b n datas beginning at \b i1 on stream \b os.
template <class T>
void NDataBlock<T>::Print(ostream& os,size_t i1,size_t n) const
// Impression de n elements a partir de i1
{
size_t nr = 0;
T* p = NULL; Bridge* br = NULL;
string sun;
if(mSRef) {nr = mSRef->nref; p = mSRef->data; br = mSRef->bridge;} 
if(mSRef&&mSRef->unit_p) sun=" "+mSRef->unit_p->ShortName();
os<<"NDataBlock::Print("<<this<<",Sz="<<mSz<<",IsTemp="<<mIsTemp<<")\n"
  <<"            mSRef="<<mSRef<<"(nref="<<nr<<",data="<<p<<sun
  <<",bridge="<<br<<")"<<endl;
if(i1>=mSz || n<=0 || !p) return;
size_t i2 = i1+n; if(i2>mSz) i2=mSz;
size_t im = 1; bool enl=false;
while(i1<i2) {
  enl = false;
  os<<" "<<(*this)(i1);  i1++;
  if(im==8) {os<<"\n"; im=1; enl=true;} else im++;
}
if(!enl) os<<endl;
}

//////////////////////////////////////////////
// Calcul de la somme / produit des donnees //
//////////////////////////////////////////////

//! Return sum of \b n datas beginning at data \b i1.
template <class T>
T NDataBlock<T>::Sum(size_t i1,size_t n) const
// Somme des elements de i1 a i1+n-1
{
if(i1>=mSz) return 0;
if(n>mSz) n = mSz; if(n==0) n = mSz-i1;
T const *p=Begin()+i1, *pe=p+n;
T val = 0;
while (p<pe) val += *p++;
return val;
}

//! Return product of \b n datas beginning at data \b i1.
template <class T>
T NDataBlock<T>::Product(size_t i1,size_t n) const
// Produit des elements de i1 a i1+n-1
{
if(i1>=mSz) return 0;
if(n>mSz) n = mSz; if(n==0) n = mSz-i1;
T const *p=Begin()+i1, *pe=p+n;
T val = 0;
while (p<pe) val *= *p++;
return val;
}

///////////////////////////////////////////////////////////////
// Surcharge de = : NDataBlock=NDataBlock; NDataBlock=<T> b; //
///////////////////////////////////////////////////////////////

//! Operator = : ND = NDa
/*! \warning Datas are copied (cloned) from \b a. */
template <class T>
NDataBlock<T>& NDataBlock<T>::operator = (const NDataBlock<T>& a)
// Affectation: partage des donnees si "a" temporaire, clone sinon.
{
if(Debug_NDataBlock>1)
  cout<<"?_NDataBlock::operator=("<<this<<","<<&a<<") a.(mSz="
      <<a.mSz<<" mSRef="<<a.mSRef<<" IsTemp="<<a.IsTemp()
      <<"), mSz="<<mSz<<" mSRef="<<mSRef<<" IsTemp="<<mIsTemp<<endl;

if(this == &a) return *this;
if(a.mSz==0)
  throw(SzMismatchError("NDataBlock::operator=A null size \n"));
if (mSz==0) { CloneOrShare(a); return *this; }
if (a.mSz != mSz)
  throw(SzMismatchError("NDataBlock::operator=A Unequal sizes \n"));
memcpy(Data(),a.Data(),mSz*sizeof(T));
return *this;
}

//! Operator = : ND = \b v (at dats set to \b v).
template <class T>
NDataBlock<T>& NDataBlock<T>::operator = (T v)
// Affectation de tous les elements a une constante "v"
{
if(Debug_NDataBlock>1)
  cout<<"?_NDataBlock::operator=("<<this<<","<<v<<")"
      <<" mSz="<<mSz<<" mSRef="<<mSRef<<" IsTemp="<<mIsTemp<<endl;

if(mSz==0) throw(SzMismatchError("NDataBlock::operator=v null size\n"));
T *p=Begin(), *pe=End(); while (p<pe) *p++ = v;
return *this;
}

//////////////////////////////////////////////////////////////
// Surcharge de +=,-=,*=,/= (INPLACE): NDataBlock += <T> b; //
//////////////////////////////////////////////////////////////

//! Add a constant : ND += b
template <class T>
NDataBlock<T>& NDataBlock<T>::operator += (T b)
{
if(mSz==0) throw(SzMismatchError("NDataBlock::operator+=v null size\n"));
T *p=Begin(), *pe=End(); while (p<pe) *p++ += b;
return *this;
}

//! Substract a constant : ND -= b
template <class T>
NDataBlock<T>& NDataBlock<T>::operator -= (T b)
{
if(mSz==0) throw(SzMismatchError("NDataBlock::operator-=v null size\n"));
T *p=Begin(), *pe=End(); while (p<pe) *p++ -= b;
return *this;
}

//! Multiply by a constant : ND *= b
template <class T>
NDataBlock<T>& NDataBlock<T>::operator *= (T b)
{
if(mSz==0) throw(SzMismatchError("NDataBlock::operator*=v null size\n"));
T *p=Begin(), *pe=End(); while (p<pe) *p++ *= b;
return *this;
}

//! Divide by a constant : ND /= b
template <class T>
NDataBlock<T>& NDataBlock<T>::operator /= (T b)
{
if(b==(T) 0) throw(ParmError("NDataBlock::operator/=v divide by zero\n"));
if(mSz==0) throw(SzMismatchError("NDataBlock::operator/=v null size\n"));
T *p=Begin(), *pe=End(); while (p<pe) *p++ /= b;
return *this;
}

////////////////////////////////////////////////////////////////////
// Surcharge de +=,-=,*=,/= (INPLACE): NDataBlock += NDataBlock1; //
////////////////////////////////////////////////////////////////////

//! Add a NDataBlock : ND += NDa
template <class T>
NDataBlock<T>& NDataBlock<T>::operator += (const NDataBlock<T>& a)
{
if(mSz==0 || mSz!=a.mSz)
  throw(SzMismatchError("NDataBlock::operator+=A size mismatch/null"));
T *p=Begin(), *pe=End();
T const * pa=a.Begin();
while (p<pe) *p++ += *pa++;
return *this;
}

//! Substract a NDataBlock : ND -= NDa
template <class T>
NDataBlock<T>& NDataBlock<T>::operator -= (const NDataBlock<T>& a)
{
if(mSz==0 || mSz!=a.mSz)
  throw(SzMismatchError("NDataBlock::operator-=A size mismatch/null"));
T *p=Begin(), *pe=End();
T const *pa=a.Begin();
while (p<pe) *p++ -= *pa++;
return *this;
}

//! Multiply by a NDataBlock : ND *= NDa
template <class T>
NDataBlock<T>& NDataBlock<T>::operator *= (const NDataBlock<T>& a)
{
if(mSz==0 || mSz!=a.mSz)
  throw(SzMismatchError("NDataBlock::operator*=A size mismatch/null"));
T *p=Begin(), *pe=End();
T const *pa=a.Begin();
while (p<pe) *p++ *= *pa++;
return *this;
}

//! Divide by a NDataBlock : ND /= NDa
template <class T>
NDataBlock<T>& NDataBlock<T>::operator /= (const NDataBlock<T>& a)
// Attention, aucune protection si un element de "a" est nul.
{
if(mSz==0 || mSz!=a.mSz)
  throw(SzMismatchError("NDataBlock::operator/=A size mismatch/null"));
T *p=Begin(), *pe=End();
T const *pa=a.Begin();
while (p<pe) *p++ /= *pa++; // Division par zero non protegee
return *this;
}

//////////////////////////////////////////////////////////////////
// Pour surcharge de +,-,*,/ : NDataBlock = NDataBlock1+<T>b;   //
//                             NDataBlock = <T>b+NDataBlock1;   //
// Pour la notion de "temporaire" voir blabla dans CloneOrShare //
//////////////////////////////////////////////////////////////////

//! Add a constant and return NDataBlock : NDret = ND + b
template <class T>
NDataBlock<T> NDataBlock<T>::Add(T b) const
// Pour A+b
{
NDataBlock<T> result; 
result.CloneOrShare(*this); result.SetTemp(true);
result += b;
return result;
}

//! Substract a constant and return NDataBlock : NDret = ND - b or NDret = b - ND
/*! Substract a constant or from a constant
   \param fginv==false : performs NDret = ND - b (default)
   \param fginv==true : performs NDret = b - ND
*/
template <class T>
NDataBlock<T> NDataBlock<T>::Sub(T b,bool fginv) const
// Pour A-b sauf si fginv==true b-A (- n'est pas commutatif!)
{
NDataBlock<T> result; 
result.CloneOrShare(*this); result.SetTemp(true);
if(fginv) {
  T *p=result.Begin(), *pe=result.End();
  T const *pa=this->Begin();
  while(p<pe) {*p++ = b - *pa++;}
} else result -= b;
return result;
}

//! Multiply by a constant and return NDataBlock : NDret = ND * b
template <class T>
NDataBlock<T> NDataBlock<T>::Mul(T b) const
// Pour A*b
{
NDataBlock<T> result; 
result.CloneOrShare(*this); result.SetTemp(true);
result *= b;
return result;
}

//! Divide by a constant and return NDataBlock : NDret = ND / b or NDret = b / ND
/*! Divide by a constant or from a constant
   \param fginv==false : performs NDret = ND / b (default)
   \param fginv==true : performs NDret = b / ND
*/
template <class T>
NDataBlock<T> NDataBlock<T>::Div(T b,bool fginv) const
// Pour A/b sauf si fginv==true b/A (/ n'est pas commutatif!)
{
NDataBlock<T> result; 
result.CloneOrShare(*this); result.SetTemp(true);
 if(fginv) {
  T *p=result.Begin(), *pe=result.End();
  T const *pa = this->Begin();
  while(p<pe) {*p++ = b / *pa++;} // Division par zero non protegee
} else {
  if( b == (T) 0 ) throw MathExc("NDataBlock<T>::Div(T)  - Divide by zero ! ");
  result /= b;
}
return result;
}

///////////////////////////////////////////////////////////////////////
// Pour surcharge de +,-,*,/ : NDataBlock = NDataBlock1+NDataBlock2; //
///////////////////////////////////////////////////////////////////////

//! Add a NDataBlock and return a NDataBlock: ND = NDthis + NDb
template <class T>
NDataBlock<T> NDataBlock<T>::Add(const NDataBlock<T>& b) const
// Pour A+B
{
if(mSz!=b.mSz)
  throw(SzMismatchError("NDataBlock operator C=A+B size mismatch/null\n"));
NDataBlock<T> result; result.SetTemp(true);
if(b.IsTemp()) {result.Share(b);            result += *this;}
  else         {result.CloneOrShare(*this); result += b;}
return result;
}

//! Multiply by a NDataBlock and return a NDataBlock: ND = NDthis * NDb
template <class T>
NDataBlock<T> NDataBlock<T>::Mul(const NDataBlock<T>& b) const
// Pour A*B
{
if(mSz!=b.mSz)
  throw(SzMismatchError("NDataBlock operator C=A*B size mismatch/null\n"));
NDataBlock<T> result; result.SetTemp(true);
if(b.IsTemp()) {result.Share(b);            result *= *this;}
  else         {result.CloneOrShare(*this); result *= b;}
return result;
}

//! Substract a NDataBlock and return a NDataBlock: ND = NDthis - NDb
template <class T>
NDataBlock<T> NDataBlock<T>::Sub(const NDataBlock<T>& b) const
// Pour A-B
{
if(mSz!=b.mSz)
  throw(SzMismatchError("NDataBlock operator C=A-B size mismatch/null\n"));
NDataBlock<T> result; result.SetTemp(true);
if(b.IsTemp()) {
  result.Share(b);
  T *p=result.Begin(), *pe=result.End(); T const *pa=Begin();
  while(p<pe) {*p = *pa++  - *p; p++;}
} else {result.CloneOrShare(*this); result -= b;}
return result;
}

//! Divide by a NDataBlock and return a NDataBlock: ND = NDthis / NDb
template <class T>
NDataBlock<T> NDataBlock<T>::Div(const NDataBlock<T>& b) const
// Pour A/B
{
if(mSz!=b.mSz)
  throw(SzMismatchError("NDataBlock operator C=A/B size mismatch/null\n"));
NDataBlock<T> result; result.SetTemp(true);
if(b.IsTemp()) {
  result.Share(b);
  T *p=result.Begin(), *pe=result.End(); T const *pa=Begin();
  while(p<pe) {*p = *pa++  / *p; p++;} // Division par zero non protegee
} else {result.CloneOrShare(*this); result /= b;}
return result;
}


///////////////////////////////////////////////////////////////
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template NDataBlock<uint_1>
#pragma define_template NDataBlock<uint_2>
#pragma define_template NDataBlock<int_1>
#pragma define_template NDataBlock<int_2>
#pragma define_template NDataBlock<int_4>
#pragma define_template NDataBlock<int_8>
#pragma define_template NDataBlock<uint_4>
#pragma define_template NDataBlock<uint_8>
#pragma define_template NDataBlock<r_4>
#pragma define_template NDataBlock<r_8>
#pragma define_template NDataBlock< complex<r_4> >
#pragma define_template NDataBlock< complex<r_8> >
#ifdef SO_LDBLE128
#pragma define_template NDataBlock<r_16>
#pragma define_template NDataBlock< complex<r_16> >
#endif

#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
namespace SOPHYA {
template class NDataBlock<uint_1>;
template class NDataBlock<uint_2>;
template class NDataBlock<int_1>;
template class NDataBlock<int_2>;
template class NDataBlock<int_4>;
template class NDataBlock<int_8>;
template class NDataBlock<uint_4>;
template class NDataBlock<uint_8>;
template class NDataBlock<r_4>;
template class NDataBlock<r_8>;
template class NDataBlock< complex<r_4> >;
template class NDataBlock< complex<r_8> >;
#ifdef SO_LDBLE128
template class NDataBlock<r_16>;
template class NDataBlock< complex<r_16> >;
#endif
}
#endif
