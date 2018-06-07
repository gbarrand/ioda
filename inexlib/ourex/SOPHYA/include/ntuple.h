// This may look like C code, but it is really -*- C++ -*-
// Class NTuple
//  CMV+Reza     Juillet 97
//  CEA-DAPNIA      LAL-IN2P3/CNRS

#ifndef NTUPLE_H_SEEN
#define NTUPLE_H_SEEN

#include "objfio.h"

#include <iostream>
#include <string>
#include <vector>

#include "ntupintf.h"
#include "ppersist.h"
#include "dvlist.h"

namespace SOPHYA {

//  Forward class declaration for Fits handler
class FITS_NTuple;
//  Forward class declaration for Thread-safe operations
class ThSafeOp;

//! Class for creation and management of double or float data sets in a table
class NTuple : public AnyDataObj , public NTupleInterface {

public:
  NTuple();
  NTuple(int nvar, char** noms, int blk=512, bool fgdouble=true);
  NTuple(int nvar, const char** noms, int blk=512, bool fgdouble=true);
  NTuple(vector<string>& noms, int blk=512, bool fgdouble=true);
  NTuple(const NTuple& NT);
  virtual ~NTuple();

  // Activate/deactivate thread-safe Fill operation
  void SetThreadSafe(bool fg=true);
  //! Return true if Fill  operations are thread-safe
  inline bool IsThreadSafe() const { return ((mThS)?true:false); }

  NTuple& operator = (const NTuple& NT);
 
  void              Fill(r_4* x);
  void              Fill(r_8* x);
  //! Return the number of lines (rows) in the table)
  inline int_4      NEntry() const  { return(mNEnt); }
  //! Return the number of columns in the tables (number of cells in a row)  
  inline int_4      NVar() const { return(mNVar); }  
  //! Return the bloc size 
  inline int_4      BlockSize() const { return(mBlk); }  
// $CHECK$ Reza 21/10/99 Pourquoi faire BLock() ?

  //! Return the content of the cell for line (row) \b n , column \b k
  float             GetVal(int n, int k)   const;
  //! Return the content of the cell for line (row) \b and column name \b nom
  inline float	    GetVal(int n, const char* nom) const 
                            { return(GetVal(n, IndexNom(nom)) ); }
  int               IndexNom(const char* nom)  const ;
  char*             NomIndex(int k) const;
  //! Return the content of line (row) \b n as a vector of r_4 (float)
  r_4*	            GetVec(int n, r_4* ret=NULL)  const ;
  //! Return the content of line (row) \b n as a vector of r_8 (double)
  r_8*	            GetVecD(int n, r_8* ret=NULL)  const ;
  //! Return the content of column \b k in the std::vector<r_8> \b vec 
  void              GetColumn(int k, vector<r_8> v) const;

// Impression, I/O
  void              Print(int num, int nmax=1)  const ;
  void              Show(ostream& os) const;
  inline void       Show() const { Show(cout); }

  //! Return the associated DVList object 
  DVList&           Info();

// Remplissage depuis fichier ASCII 
  int		    FillFromASCIIFile(string const& fn, float defval=0.);


// Declaration de l interface NTuple
  virtual size_t        NbLines() const ;
  virtual size_t        NbColumns() const ;
  virtual r_8 *         GetLineD(size_t n) const ;
  virtual r_8		GetCell(size_t n, size_t k) const ;
  virtual r_8		GetCell(size_t n, string const & nom) const ;
  virtual void		GetMinMax(size_t k, double& min, double& max)   const ; 
  virtual void		GetMinMax(string const & nom, double& min, double& max)   const ; 
  virtual size_t        ColumnIndex(string const & nom)  const ;
  virtual string        ColumnName(size_t k) const;
  virtual string	VarList_C(const char* nomx=NULL) const ;
  virtual string        LineHeaderToString() const;
  virtual string        LineToString(size_t n) const;  

//  Pour la gestion de persistance
  friend class ObjFileIO<NTuple> ;

  // pour fichiers FITS
  friend class FITS_NTuple;

private:
  void Initialize(int nvar, int blk, bool fgdouble);
  void  Clean();

  int_4 mNVar, mNEnt, mBlk, mNBlk;
  r_4* mVar;
  r_8* mVarD;
  vector<string> mNames;

  bool mFgDouble;      // true -> les donnees sont gardees en double
  vector<r_4*> mPtr;    // Pointeur de tableau float
  vector<r_8*> mPtrD;   // Pointeur de tableau double

  DVList* mInfo;        // Infos (variables) attachees au NTuple
  
  ThSafeOp* mThS;       // != NULL -> Thread safe operations
};

/*! Prints table information on stream \b s (nt.Show(s)) */
inline ostream& operator << (ostream& s, NTuple const & nt)
  {  nt.Show(s);  return(s);  }

/*! Writes the object in the POutPersist stream \b os */
inline POutPersist& operator << (POutPersist& os, NTuple & obj)
{ ObjFileIO<NTuple> fio(&obj);  fio.Write(os);  return(os); }
/*! Reads the object from the PInPersist stream \b is */
inline PInPersist& operator >> (PInPersist& is, NTuple & obj)
{ ObjFileIO<NTuple> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

// Classe pour la gestion de persistance
// ObjFileIO<NTuple>


} // namespace SOPHYA

#endif
  
