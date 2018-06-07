#ifndef RANDR48_H_SEEN
#define RANDR48_H_SEEN

// Classes DR48RandGen
// Generateur aleatoire utilisant l'aleatoire drand48()
//
// R. Ansari          (C) UPS+LAL IN2P3/CNRS  2009
// C. Magneville      (C) DAPNIA/SPP  CEA     2009


#include "machdefs.h"
#include "objfio.h"
#include "ndatablock.h"
#include "randinterf.h"


namespace SOPHYA {

// Implementation de RandomGeneratorInterface avec drand48()  
class DR48RandGen : public RandomGeneratorInterface {

 public:
  DR48RandGen();
  DR48RandGen(long int seed);
  virtual ~DR48RandGen();

  virtual void SetSeed(long int seed);
  virtual void SetSeed(uint_2 seed[3]);
  virtual void GetSeed(uint_2 seed[3]);
  virtual void AutoInit(int lp=0);
  virtual void ShowRandom();

  friend class ObjFileIO<DR48RandGen> ;

 protected:
  virtual r_8 Next();

};


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

// Version thread-safe de  DR48RandGen 
class ThSDR48RandGen : public DR48RandGen {

 public:
  ThSDR48RandGen(size_t n=1024,bool tsafe=true);
  ThSDR48RandGen(ThSDR48RandGen const & rg);
  virtual ~ThSDR48RandGen();

  void SetBuffSize(size_t n);

  virtual void SetSeed(long int seed);
  virtual void SetSeed(uint_2 seed[3]);
  virtual void GetSeed(uint_2 seed[3]);
  virtual void AutoInit(int lp=0);
  virtual void ShowRandom();

  friend class ObjFileIO<ThSDR48RandGen> ;

 protected:
  // ---- protected data members
  NDataBlock<r_8>  rseq_;  
  size_t idx_;
  bool fg_nothrsafe;  // if true --> NOT thread-safe
  // ---- protected methods 
  void GenSeq();
  virtual r_8 Next();

//! For PPF handler use 
  inline void GetSeed_P(uint_2 seed[3])  
    { return DR48RandGen::GetSeed(seed); }
//! For PPF handler use 
  inline void SetSeed_P(uint_2 seed[3])   
    { return DR48RandGen::SetSeed(seed); }
};

// Classe pour la gestion de persistance PPF :  ObjFileIO<ThSDR48RandGen>

/*! Writes the random generator object state in the POutPersist stream \b os */
inline POutPersist& operator << (POutPersist& os, ThSDR48RandGen & obj)
{ ObjFileIO<ThSDR48RandGen> fio(&obj);  fio.Write(os);  return(os); }
/*! Reads the random generator object state from the PInPersist stream \b is */
inline PInPersist& operator >> (PInPersist& is, ThSDR48RandGen & obj)
{ ObjFileIO<ThSDR48RandGen> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

// Classe pour la gestion de persistance PPF :  ObjFileIO<DR48RandGen>

/*! Writes the random generator object state in the POutPersist stream \b os */
inline POutPersist& operator << (POutPersist& os, DR48RandGen & obj)
{ ObjFileIO<DR48RandGen> fio(&obj);  fio.Write(os);  return(os); }
/*! Reads the random generator object state from the PInPersist stream \b is */
inline PInPersist& operator >> (PInPersist& is, DR48RandGen & obj)
{ ObjFileIO<DR48RandGen> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

} /* namespace SOPHYA */

#endif
