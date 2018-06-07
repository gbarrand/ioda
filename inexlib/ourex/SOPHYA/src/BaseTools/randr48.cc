#include "machdefs.h"
#include <math.h> 
#include <stdlib.h>
#include <string.h>
#include "thsafeop.h"
#include "fiondblock.h"

#include "randr48.h"

namespace SOPHYA {

/*!
   \class DR48RandGen
   \ingroup BaseTools
   \brief  Implementation of the RandomGeneratorInterface class using drand48() functions 

   Its PPF handler can be used to save the complete state of the class and the underlying
   random number generator used.

   \sa SOPHYA::ObjFileIO<ThSDR48RandGen>

*/

static bool dr48_first = true;
DR48RandGen::DR48RandGen()
{
  if (dr48_first) {
    Next();  dr48_first=false;
  }
}

DR48RandGen::DR48RandGen(long int  seed)
{
  srand48(seed);
  dr48_first=false;
}

DR48RandGen::~DR48RandGen()
{
}

void DR48RandGen::ShowRandom()
{
  cout<<"RandomGenerator is DR48RandGen"<<endl;
}

/*! 
  \brief Initialize the state of the random generator object using the integer value argument

  \warning This method changes the global random generator drand48() state
*/
void DR48RandGen::SetSeed(long int  seed)
{
  srand48(seed);
}

/*! 
  \brief Initialize the state of the random generator object using the short integer array

  \warning This method changes the global random generator drand48() state
*/
void DR48RandGen::SetSeed(uint_2 seed[3])
{
  seed48(seed);
}

//! Return the random generator state 
void DR48RandGen::GetSeed(uint_2 seed[3])
{
  uint_2 *p, seed_dummy[3] = {0,0,0};
  p = seed48(seed_dummy);
  memcpy(seed,p,3*sizeof(uint_2));
  // on re-initialise a ce qui etait avant
  seed48(seed);
}

r_8 DR48RandGen::Next()
{
  return drand48();
}

/*! 
  \brief Random Initialization of the state of the random generator object using the system clock

  \warning This method changes the global random generator drand48() state
*/
void DR48RandGen::AutoInit(int lp)
{
  vector<uint_2> seed;
  GenerateSeedVector(0,seed,lp);
  uint_2 s[3] = {seed[0],seed[1],seed[2]};
  SetSeed(s);
}

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
/*!
   \class ThSDR48RandGen
   \ingroup BaseTools
   \brief Thread-safe version of DR48RandGen random number generator using drand48() functions.

   
   Several instances of this class can be used in different threads without the risk of 
   corrupting the internal state of the drand48() generator. However, in multi-thread applications,
   there is no guarantee to obtain the same sequence of numbers in each thread.  
   Its PPF handler can be used to save the complete state of the class and the underlying
   random number generator (drand48() which is used.

   \sa SOPHYA::ObjFileIO<ThSDR48RandGen>

   \code 
   // A.1- Create a thread safe generator based on drand48() 
   ThSDR48RandGen rg;
   // A.2- Auto initilize its state (using the system time) 
   rg.AutoInit();
   // A.3- compute and print a smal sequence of random numbers 
   int N = 10;
   for(int i=0; i<N; i++) 
     cout << " I=" << i << "  rand_flat01= " << rg.Flat01() << " rand.gaussian= " << rg.Gaussian() << endl;
   // A.4- Save the generator state for subsequent use
   POutPersist po("rg.ppf");
   po << rg;
   // A.5- compute and print a second sequence of random numbers 
   for(int i=0; i<N; i++) 
     cout << "++ I=" << i+N << "  rand_flat01= " << rg.Flat01() << " rand.gaussian= " << rg.Gaussian() << endl;
   
   ...  In another program :

   // B.1- Create and initialize the generator from the previously saved state
   ThSDR48RandGen rgr;
   PInPersist pin("rg.ppf");
   pin >> rgr;
   int N = 10;
   // B.2- Compute and print a sequence of random number, should be compared to the sequance A.5
   for(int i=0; i<N; i++) 
     cout << "-- I=" << i << "  rand_flat01= " << rgr.Flat01() << " rand.gaussian= " << rgr.Gaussian() << endl;
   
   
   \endcode
*/

// Objet statique global pour gestion de lock entre threads
static ThSafeOp* ths_rand = NULL;

/*! 
  \brief Constructor with optional specification of the internal buffer size and thread-safety flag 
  
  The behaviour of the base class DR48RandGen can be reproduced by specifying tsafe=false
  \param n : an internal buffer of size n is created and filled through block calls to drand48()
  \param tsafe : if false, creates a non thread-safe generator 
*/

ThSDR48RandGen::ThSDR48RandGen(size_t n, bool tsafe) 
{
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

ThSDR48RandGen::ThSDR48RandGen(ThSDR48RandGen const & rg)
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


ThSDR48RandGen::~ThSDR48RandGen(void) 
{
  // rien a faire 
}

void ThSDR48RandGen::SetBuffSize(size_t n)
// redimensionnement du buffer
{
  if(fg_nothrsafe) return;
  if (n < 1) n = 1024;
  rseq_.ReSize(n, false);
  idx_ = n;
}


void ThSDR48RandGen::ShowRandom()
{
  cout<<"RandomGenerator is ThSDR48RandGen("<<rseq_.Size()<<","<<!fg_nothrsafe<<")"<<endl;
}

/*! 
  \brief Initialize the state of the random generator object using the integer value argument

  \warning This method changes the global random generator drand48() state
*/
void ThSDR48RandGen::SetSeed(long int seed)
{
  if (ths_rand == NULL)  ths_rand = new ThSafeOp; 
  ths_rand->lock();
  DR48RandGen::SetSeed(seed);
  if(!fg_nothrsafe) idx_ = rseq_.Size();
  ths_rand->unlock();
  return;
}

/*! 
  \brief Initialize the state of the random generator object using the short integer array

  \warning This method changes the global random generator drand48() state
*/
void ThSDR48RandGen::SetSeed(uint_2 seed[3])
{
  if (ths_rand == NULL)  ths_rand = new ThSafeOp; 
  ths_rand->lock();
  DR48RandGen::SetSeed(seed);
  if(!fg_nothrsafe) idx_ = rseq_.Size();
  ths_rand->unlock();
}

//! Return the random generator state 
void ThSDR48RandGen::GetSeed(uint_2 seed[3])
{
  if (ths_rand == NULL)  ths_rand = new ThSafeOp; 
  ths_rand->lock();
  DR48RandGen::GetSeed(seed);
  ths_rand->unlock();
  return;
}


/*! 
  \brief Random Initialization of the state of the random generator object using the system clock

  \warning This method changes the global random generator drand48() state
*/
void ThSDR48RandGen::AutoInit(int lp)
{
  vector<uint_2> seed;
  GenerateSeedVector(0,seed,lp);
  uint_2 s[3] = {seed[0],seed[1],seed[2]};
  SetSeed(s);
}


void ThSDR48RandGen::GenSeq(void)
{
  ths_rand->lock();
  for(size_t k=0; k<rseq_.Size(); k++)  rseq_(k) = drand48();
  ths_rand->unlock();
  idx_ = 0;
}
 
r_8 ThSDR48RandGen::Next() 
{
  if (rseq_.Size() == 0)  return drand48(); 
  else { 
    if(idx_==rseq_.Size()) GenSeq(); 
    return(rseq_(idx_++));
  }
}


//----------------------------------------------------------
// Classe pour la gestion de persistance
// ObjFileIO<DR48RandGen>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<DR48RandGen>::WriteSelf(POutPersist& s) const
{
  if (dobj == NULL)
    throw NullPtrError("ObjFileIO<DR48RandGen>::WriteSelf() dobj=NULL");
  uint_2 seed[3];
  dobj->GetSeed(seed);
  s.Put(seed,3);
  return;
}

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<DR48RandGen>::ReadSelf(PInPersist& s)
{
  uint_2 seed[3];
  s.Get(seed,3);
  if(dobj == NULL) dobj = new DR48RandGen();
  dobj->SetSeed(seed);
  return;
}

//----------------------------------------------------------
// Classe pour la gestion de persistance
// ObjFileIO<ThSDR48RandGen>
//----------------------------------------------------------

/* --Methode-- */
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<ThSDR48RandGen>::WriteSelf(POutPersist& s) const
{
  if (dobj == NULL)
    throw NullPtrError("ObjFileIO<ThSDR48RandGen>::WriteSelf() dobj=NULL");
  ths_rand->lock();  // thread-safety
  uint_4 itab[6];
  //itab : [0]: version, [1,2,3] = srand48 state/seed  , [4,5] = reserved for future use
  itab[0] = 1;
  // On recupere et on ecrit ds le PPF l'etat du generateur aleatoire
  uint_2 seed_16v[3];
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
void ObjFileIO<ThSDR48RandGen>::ReadSelf(PInPersist& s)
{
  uint_4 itab[6];
  //itab : [0]: version, [1,2,3] = srand48 state/seed  , [4] = reserved for future use
  s.Get(itab, 6);
  uint_8 sz,ix;
  s.Get(sz);  // Taille du tableau intermediaire
  s.Get(ix);  // valeur de l'index ds le tableau intermediaire

  if (dobj == NULL) dobj = new ThSDR48RandGen(sz, (sz>0)?true:false);
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
  uint_2 seed_16v[3];
  //NON ? pourquoi faire GetSeed ? : dobj->GetSeed_P(seed_16v);
  for(int i=0; i<3; i++)  seed_16v[i] = itab[i+1];
  ths_rand->lock();  // thread-safety
  dobj->SetSeed_P(seed_16v);
  ths_rand->unlock(); // thread-safety
  return;
}

// ---------------------------------------------------------
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<DR48RandGen>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<DR48RandGen>;
#endif

// ---------------------------------------------------------
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<ThSDR48RandGen>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<ThSDR48RandGen>;
#endif
// ---------------------------------------------------------


}  /* namespace SOPHYA */
