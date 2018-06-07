#include "stsrand.h"
#include "thsafeop.h"
//#include "srandgen.h"
#include "fiondblock.h"
#include <string.h>
#include <math.h> 
#include <sys/time.h>

namespace SOPHYA {

/*!
   \class STSRandGen
   \ingroup BaseTools
   \brief Random number generator

   \warning This class is OBSOLETE. Use ThSDR48RandGen instead.
   This class is a thread-safe random number generator.
   Its PPF handler can be used to save the complete state of the class and the underlying
   random number generator used.

   \sa SOPHYA::ObjFileIO<STSRandGen>

   \sa frand01 drand01 frandpm1 drandpm1 
   \sa GauRnd PoissRand
   \sa Ini_Ranf_Quick Ini_Ranf Get_Ranf Auto_Ini_Ranf


*/

// Objet statique global pour gestion de lock entre threads
static ThSafeOp* ths_rand = NULL;

STSRandGen::STSRandGen(size_t n, bool tsafe) 
{
  cout << " STSRandGen::STSRandGen()/Warning : this class is OBSOLETE. Use ThSDR48RandGen !" << endl;
  cout << " STSRandGen::STSRandGen()/Warning : this class is OBSOLETE. Use ThSDR48RandGen !" << endl;

  if (ths_rand == NULL)  ths_rand = new ThSafeOp; 
  if (tsafe) {   // thread-safe
    fg_nothrsafe = false;
    if (n < 1) n = 1024;
    rseq_.ReSize(n, false);
    idx_ = n;
  }
  else {   // NOT thread-safe
    fg_nothrsafe = true;
    idx_ = 1;
  }
}

STSRandGen::STSRandGen(STSRandGen const & rg)
{
  if (ths_rand == NULL)  ths_rand = new ThSafeOp; 
  if (!rg.fg_nothrsafe) { // thread-safe
    fg_nothrsafe = false;
    rseq_.ReSize(rg.rseq_.Size(), false);
    idx_ = rseq_.Size();
  }
  else {   // NOT thread-safe
    fg_nothrsafe = true;
    idx_ = 1;
  }
}


STSRandGen::~STSRandGen(void) 
{
  // rien a faire 
}

void STSRandGen::SetBuffSize(size_t n)
// redimensionnement du buffer
{
  if(fg_nothrsafe) return;
  if (n < 1) n = 1024;
  rseq_.ReSize(n, false);
  idx_ = n;
}

void STSRandGen::AutoInit(int lp)
// Initialisation automatique (pseudo) aleatoire du generateur.
// L'initialiseur est donne par un codage du nombre de millisecondes
// ecoulees depuis le 0 heure le 1er Janvier 1970 UTC (cf gettimeofday).
// Seuls les 48 bits de poids faible sont retenus.
// Un melange des bits est ensuite effectue pour que les 3 nombres
// (unsigned short) d'initialisation ne soient pas trop semblables.
//
// Le nombre le plus grand que l'on peut mettre
// dans un entier unsigned de N bits est: 2^N-1
// 48 bits -> 2^48-1 = 281474976710655 musec = 3257.8j = 8.9y
//         -> meme initialisation tous les 8.9 ans a 1 microsec pres !
{
  // On recupere le temps ecoule depuis l'origine code en sec+musec
  struct timeval now;
  gettimeofday(&now,0);

  // Calcul du temps ecoule depuis l'origine en microsecondes
  uint_8 v = (uint_8)now.tv_sec*(uint_8)1000000 + (uint_8)now.tv_usec;
  if(lp>1) cout<<"..."<<now.tv_sec<<" sec + "<<now.tv_usec<<" musec = "<<v<<" musec"<<endl;

  // Remplissage du tableau de bits
  unsigned short b[48];
  for(int ip=0;ip<48;ip++) {b[ip] = v&1; v = (v>>1);}
  if(lp>2) {
    cout<<"...b= ";
    for(int ip=47;ip>=0;ip--) {if(ip==23) cout<<" "; cout<<b[ip];}
    cout<<endl;
  }

  // Melange des bits qui varient vite (poids faible, microsec)
  //   avec ceux variant lentement (poids fort, sec)
  // On coupe le mot en trois: bits[0-15], bits[16-31] et bits[32-47]
  // On echange 2 bits sur 3 du mot bits[0-15] dans les autres mots
  //    bit0  <-> bit0  , bit1  <-> bit17 , bit2  <-> bit34
  //    bit3  <-> bit3  , bit4  <-> bit20 , bit5  <-> bit37
  //    bit13 <-> bit13 , bit14 <-> bit30 , bit15 <-> bit47
  for(int ip=0;ip<16;ip++) {
    if(ip%3==0) continue;
    int ipd = (ip%3==1)? 16+ip : 32+ip;
    unsigned short w = b[ip];
    //cout<<"swap g["<<ip<<"]="<<b[ip]<<" <-> b["<<ipd<<"]="<<b[ipd]<<endl;
    b[ip] = b[ipd];
    b[ipd] = w;
  }
  if(lp>2) {
    cout<<"...b= ";
    for(int ip=47;ip>=0;ip--) {if(ip==23) cout<<" "; cout<<b[ip];}
    cout<<endl;
  }

  // Construction du tableau d'initialisation
  unsigned short seed_16v[3] = {0,0,0};
  for(int is=0;is<3;is++) {
    unsigned short w = 1;
    for(int ip=0;ip<16;ip++) {
      seed_16v[is] += w*b[is*16+ip];
      w *= 2;
    }
  }
  if(lp>0) cout<<"STSRandGen::AutoInit: "<<seed_16v[0]<<" "<<seed_16v[1]<<" "<<seed_16v[2]<<endl;

  // Initialise drand48()
  Init(seed_16v,lp);
}

void STSRandGen::Init(long seed_val, int lp)
{
  if (ths_rand == NULL)  ths_rand = new ThSafeOp; 
  if(lp) cout << "STSRandGen::Init(long seed=" << seed_val << ")" << endl;
  ths_rand->lock();
  srand48(seed_val);
  ths_rand->unlock();
  return;
}

void STSRandGen::Init(unsigned short seed_16v[3], int lp)
{
  if (ths_rand == NULL)  ths_rand = new ThSafeOp; 
  if(lp) cout << "STSRandGen::Init(u_short seed_16v[3]=" << seed_16v[0]
	      << "," << seed_16v[1] << "," << seed_16v[2] << ")" << endl;
  ths_rand->lock();
  Init_P(seed_16v);
  ths_rand->unlock();
}

void STSRandGen::GetSeed(unsigned short seed_16v[3], int lp)
{
  if (ths_rand == NULL)  ths_rand = new ThSafeOp; 
  ths_rand->lock();
  GetSeed_P(seed_16v);
  ths_rand->unlock();
  if(lp) cout << "STSRandGen::GetSeed(u_short seed_16v[3]=" << seed_16v[0]
	      << "," << seed_16v[1] << "," << seed_16v[2] << ")" << endl;
  return;
}

void STSRandGen::Init_P(unsigned short seed_16v[3])
{
  seed48(seed_16v);
}

void STSRandGen::GetSeed_P(unsigned short seed_16v[3])
{
  unsigned short seed[3] = {0,0,0};
  unsigned short *p;
  p = seed48(seed);
  memcpy(seed_16v,p,3*sizeof(unsigned short));
  /* on re-initialise a ce qui etait avant */
  seed48(seed_16v);
  return;
}

r_8 STSRandGen::Gaussian() 
{
  r_8 A=Next(); 
  while (A==0.) A=Next(); 
  return sqrt(-2.*log(A))*cos(2.*M_PI*Next());
}


uint_8 STSRandGen::Poisson(double mu,double mumax)
{
  double pp,ppi,x;

  if((mumax>0.)&&(mu>=mumax)) {
    pp = sqrt(mu);
    while( (x=pp*Gaussian()) < -mu );
    return (uint_8)(mu+x+0.5);
  }
  else {
    uint_8 n;
    ppi = pp = exp(-mu);
    x = Next();
    n = 0;
    while (x > ppi) {
      n++;
      pp = mu*pp/(double)n;
      ppi += pp;
    }
    return n;
  }
  return 0;  // pas necessaire ?
}

void STSRandGen::GenSeq(void)
{
  ths_rand->lock();
  for(size_t k=0; k<rseq_.Size(); k++)  rseq_(k) = drand48();
  ths_rand->unlock();
  idx_ = 0;
}

//----------------------------------------------------------
// Classe pour la gestion de persistance
// ObjFileIO<STSRandGen>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<STSRandGen>::WriteSelf(POutPersist& s) const
{
  if (dobj == NULL)
    throw NullPtrError("ObjFileIO<STSRandGen>::WriteSelf() dobj=NULL");
  ths_rand->lock();  // thread-safety
  uint_4 itab[6];
  //itab : [0]: version, [1,2,3] = srand48 state/seed  , [4,5] = reserved for future use
  itab[0] = 1;
  // On recupere et on ecrit ds le PPF l'etat du generateur aleatoire
  unsigned short seed_16v[3];
  dobj->GetSeed_P(seed_16v);
  for(int i=0; i<3; i++)  itab[i+1] = seed_16v[i];
  itab[4] = 0;
  s.Put(itab, 6);
  uint_8 sz = dobj->rseq_.Size();
  s.Put(sz);  // Taille du tableau intermediaire
  uint_8 ix = dobj->idx_;
  s.Put(ix);  // valeur de l'index 

  if (dobj->rseq_.Size() > 0)  s << dobj->rseq_;  // On ecrit le tableau (NDataBlock) si necessaire
  ths_rand->unlock(); // thread-safety
  return;
}

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void        ObjFileIO<STSRandGen>::ReadSelf(PInPersist& s)
{
  uint_4 itab[6];
  //itab : [0]: version, [1,2,3] = srand48 state/seed  , [4] = reserved for future use
  s.Get(itab, 6);
  uint_8 sz,ix;
  s.Get(sz);  // Taille du tableau intermediaire
  s.Get(ix);  // Taille du tableau intermediaire

  if (dobj == NULL) dobj = new STSRandGen(sz, (sz>0)?true:false);
  dobj->idx_ = ix;
  if (sz > 0) { 
    s >> dobj->rseq_;  // On lit le tableau (NDataBlock) si necessaire
    dobj->fg_nothrsafe = false;
  }
  else {  // Objet lu est NON thread-safe, taille_tableau rseq_ = 0
    dobj->fg_nothrsafe = true;
    if (dobj->rseq_.Size() > 0)  dobj->rseq_.Dealloc(); 
  }
  // On initialise l'etat du generateur aleatoire avec les valeurs lues
  unsigned short seed_16v[3];
  dobj->GetSeed_P(seed_16v);
  for(int i=0; i<3; i++)  seed_16v[i] = itab[i+1];
  dobj->Init(seed_16v, 0);
  return;
}

// ---------------------------------------------------------
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<STSRandGen>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<STSRandGen>;
#endif
// ---------------------------------------------------------

}  /* namespace SOPHYA */

