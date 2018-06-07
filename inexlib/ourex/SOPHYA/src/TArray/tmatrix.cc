// $Id: tmatrix.cc,v 1.42 2012/11/19 09:30:32 ansari Exp $
//                         C.Magneville          04/99
#include "sopnamsp.h"
#include "machdefs.h"
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include "pexceptions.h"

#define TMATRIX_CC_BFILE  // avoid extern template declarations 
#include "tmatrix.h"

namespace SOPHYA {

/*!
  \class TMatrix
  \ingroup TArray
  
  The TMatrix class specializes the TArray class for representing
  two dimensional arrays as matrices. Matrix and vector operations,
  such as matrix multiplication or transposition is implemented.

  Sub-matrices, in particular matrix rows and columns can easily and 
  efficiently be extracted and manipulated.
  It should be noted that a significant memory overhead is associated with
  small matrices (typically less than 10x10=100 elements).However, 
  higher dimension arrays (3D for examples) can be used to represent large 
  number of small matrices or vectors.

  \warning Matrix row, column indices (r,c) correspond to array indices
  (r=jy, c=ix) for CMemoryMapping and to (r=ix, c=jy) for FortranMemoryMapping.

  \b Matrix is a typedef for double precision floating point matrix ( TMatrix<r_8> ).

  \sa SOPHYA::TArray  SOPHYA::TVector
  \sa SOPHYA::Range   SOPHYA::Sequence
  \sa SOPHYA::MathArray  SOPHYA::SimpleMatrixOperation

  The following sample code illustrates vector-matrix multiplication 
  and matrix inversion, using simple gauss inversion.
  \code
  #include "array.h"
  // ....
  int n = 5;      // Size of matrix and vectors here
  Matrix a(n,n);  
  a = RandomSequence(RandomSequence::Gaussian, 0., 2.5);
  Vector x(n);
  x = RegularSequence(1.,3.);
  Vector b = a*x;
  cout << " ----- Vector x = \n " << x << endl;
  cout << " ----- Vector b = a*x = \n " << b << endl;
  SimpleMatrixOperation<r_8> smo;
  Matrix inva = smo.Inverse(a);
  cout << " ----- Matrix Inverse(a) = \n " << inva << endl;
  cout << " ----- Matrix a*Inverse(a) = \n " << inva*a << endl;
  cout << " ----- Matrix Inverse(a)*b (=Inv(a)*a*x) = \n " << inva*b << endl;
  cout << " ----- Matrix x-Inverse(a)*b = (=0 ?)\n " << x-inva*b << endl;  
  \endcode
  
 */

////////////////////////////////////////////////////////////////
//**** Createur, Destructeur
//! Default constructor
template <class T>
TMatrix<T>::TMatrix()
// Constructeur par defaut.
  : TArray<T>()
{
  arrtype_ = 1;   // Type = Matrix
}

//! constructor of a matrix with  r lines et c columns.
/*!
  \param r : number of rows
  \param c : number of columns
  \param mm : define the memory mapping type
  \param fzero : if \b true , set matrix elements to zero 
  \sa ReSize
 */
template <class T>
TMatrix<T>::TMatrix(sa_size_t r,sa_size_t c, short mm, bool fzero)
// Construit une matrice de r lignes et c colonnes.
  :  TArray<T>() 
{
  if ( (r == 0) || (c == 0) )
    throw ParmError("TMatrix<T>::TMatrix(sa_size_t r,sa_size_t c) NRows or NCols = 0");
  arrtype_ = 1;   // Type = Matrix
  ReSize(r, c, mm, fzero);
}

//! Constructor by copy
/*!
  \warning datas are \b SHARED with \b a.
  \sa NDataBlock::NDataBlock(const NDataBlock<T>&)
*/
template <class T>
TMatrix<T>::TMatrix(const TMatrix<T>& a)
// Constructeur par copie
  : TArray<T>(a)
{
  arrtype_ = 1;   // Type = Matrix
  UpdateMemoryMapping(a, SameMemoryMapping);
}

//! Constructor by copy
/*!
  \param share : if true, share data. If false copy data
 */
template <class T>
TMatrix<T>::TMatrix(const TMatrix<T>& a, bool share)
// Constructeur par copie avec possibilite de forcer le partage ou non.
: TArray<T>(a, share)
{
  arrtype_ = 1;   // Type = Matrix
  UpdateMemoryMapping(a, SameMemoryMapping);
}

//! Constructor of a matrix from a TArray \b a
/*!
  \param a : TArray to be copied or shared
  \param share : if true, share data. If false copy data
 */
template <class T>
TMatrix<T>::TMatrix(const TArray<T>& a, bool share)
: TArray<T>(a, share)
{
  arrtype_ = 1;   // Type = Matrix
  if (a.NbDimensions() == 0)  {   // Reza-Nov 2011: we allow copy contrsuctor on non allocated arrays  
    UpdateMemoryMapping(a, SameMemoryMapping);
    return;
  }
  if (a.NbDimensions() > 2) 
    throw SzMismatchError("TMatrix<T>::TMatrix(const TArray<T>& a, ...) a.NbDimensions()>2");
  if (a.NbDimensions() == 1) {
    size_[1] = 1;
    step_[1] = size_[0]*step_[0];
    ndim_ = 2;
  }
  UpdateMemoryMapping(a, SameMemoryMapping);
}

//! Constructor of a matrix from a TArray \b a , with different data type
/*!
  Matrix size and memory layout are copied from the array \b a, or a packed matrix is created if \b pack==true.
  \param a : original array, to copy sizes and data from
  \param pack : if \b true , create a packed matrix, else same memory layout as \b a. 
*/
template <class T>
TMatrix<T>::TMatrix(const BaseArray& a, bool pack)
  : TArray<T>()
{
  // On ne peut pas passer par TArray<T>(const BaseArray&), car il faut initialiser  arrtype_ d'abord !
  arrtype_ = 1;   // Type = Matrix
  if (a.NbDimensions() == 0)  {   // Reza-Nov 2011: we allow copy contrsuctor on non allocated arrays  
    UpdateMemoryMapping(a, SameMemoryMapping);
    return;
  }
  if (a.NbDimensions() > 2) 
    throw SzMismatchError("TMatrix<T>::TMatrix((const BaseArray& a, bool) a.NbDimensions()>2");
  string exmsg = "TMatrix<T>::TMatrix(const BaseArray&,bool)";
  TArray<T>::ReSize(a,pack,false);
  UpdateMemoryMapping(a, SameMemoryMapping);
  TArray<T>::ConvertAndCopyElt(a);
}



//! Destructor
template <class T>
TMatrix<T>::~TMatrix()
{
}

//! Set matrix equal to \b a and return *this
/*!
  \warning Datas are copied (cloned) from \b a.
  \sa NDataBlock::operator=(const NDataBlock<T>&)
*/
template <class T>
TArray<T>& TMatrix<T>::Set(const TArray<T>& a)
{
  if (a.NbDimensions() > 2) 
    throw SzMismatchError("TMatrix<T>::Set(const TArray<T>& a) a.NbDimensions() > 2");
  if ((arrtype_ == 2) && (a.NbDimensions() > 1) && (a.Size(0) > 1) && (a.Size(1) > 1) )
    throw SzMismatchError("TMatrix<T>::Set(const TArray<T>& a) Size(0,1)>1 for Vector");
  TArray<T>::Set(a);
  if (NbDimensions() == 1) {
    size_[1] = 1;
    step_[1] = size_[0]*step_[0];
    ndim_ = 2;
  }
  UpdateMemoryMapping(*this, SameMemoryMapping);
  return(*this);
}

template <class T>
TArray<T>& TMatrix<T>::SetBA(const BaseArray& a)
{
  if (a.NbDimensions() > 2) 
    throw SzMismatchError("TMatrix<T>::SetBA(const BaseArray& a) a.NbDimensions() > 2");
  if ((arrtype_ == 2) && (a.NbDimensions() > 1) && (a.Size(0) > 1) && (a.Size(1) > 1) )
    throw SzMismatchError("TMatrix<T>::Set(const TArray<T>& a) Size(0,1)>1 for Vector");
  TArray<T>::SetBA(a);
  if (NbDimensions() == 1) {
    size_[1] = 1;
    step_[1] = size_[0]*step_[0];
    ndim_ = 2;
  }
  UpdateMemoryMapping(*this, SameMemoryMapping);
  return(*this);
}



//! Resize the matrix
/*!
  \param r : number of rows
  \param c : number of columns
  \param mm : define the memory mapping type
         (SameMemoryMapping,CMemoryMapping
         ,FortranMemoryMapping,DefaultMemoryMapping)
  \param fzero : if \b true , set matrix elements to zero 
 */
template <class T>
void TMatrix<T>::ReSize(sa_size_t r, sa_size_t c, short mm, bool fzero)
{
  if(r==0||c==0) 
    throw(SzMismatchError("TMatrix::ReSize r or c==0 "));
  if ((arrtype_ == 2) && (r > 1) && (c > 1))
    throw(SzMismatchError("TMatrix::ReSize r>1&&c>1 for Vector "));
  sa_size_t size[BASEARRAY_MAXNDIMS];
  for(int_4 kk=0; kk<BASEARRAY_MAXNDIMS; kk++)  size[kk] = 0;
  if (mm == SameMemoryMapping) mm = GetMemoryMapping();  
  else if ( (mm != CMemoryMapping) && (mm != FortranMemoryMapping) ) 
    mm = GetDefaultMemoryMapping();
  if (mm == CMemoryMapping) {
    size[0] = c;  size[1] = r;
  }
  else {
    size[0] = r;  size[1] = c;
  }
  TArray<T>::ReSize(2, size, 1, fzero);
  UpdateMemoryMapping(mm);
}

//! Re-allocate space for the matrix
/*!
  \param r : number of rows
  \param c : number of columns
  \param mm : define the memory mapping type
  \param force : if true re-allocation is forced, if not it occurs
          only if the required space is greater than the old one.
  \sa ReSize
 */
template <class T>
void TMatrix<T>::Realloc(sa_size_t r,sa_size_t c, short mm, bool force)
{
  if(r==0||c==0) 
    throw(SzMismatchError("TMatrix::Realloc r or c==0 "));
  if ((arrtype_ == 2) && (r > 1) && (c > 1))
    throw(SzMismatchError("TMatrix::Realloc r>1&&c>1 for Vector "));
  sa_size_t size[BASEARRAY_MAXNDIMS];
  for(int_4 kk=0; kk<BASEARRAY_MAXNDIMS; kk++)  size[kk] = 0;
  if (mm == SameMemoryMapping) mm = GetMemoryMapping();  
  else if ( (mm != CMemoryMapping) && (mm != FortranMemoryMapping) ) 
    mm = GetDefaultMemoryMapping();
  if (mm == CMemoryMapping) {
    size[0] = c;  size[1] = r;
  }
  else {
    size[0] = r;  size[1] = c;
  }
  TArray<T>::Realloc(2, size, 1, force);
  UpdateMemoryMapping(mm);
}

// $CHECK$ Reza 03/2000  Doit-on declarer cette methode const ?
//! Return a submatrix define by \b Range \b rline and \b rcol
template <class T>
TMatrix<T> TMatrix<T>::SubMatrix(Range rline, Range rcol) const
{
  Range rx=Range::first();
  Range ry=Range::first();
  short mm = GetMemoryMapping();
  if (mm == CMemoryMapping)  { rx = rcol;  ry = rline; }
  else { ry = rcol;  rx = rline; }
  TMatrix sm(SubArray(rx, ry, Range::first(), Range::first(), Range::first(), false), true);
  sm.UpdateMemoryMapping(mm);
  return(sm);
}

////////////////////////////////////////////////////////////////
// Transposition
//! Transpose matrix in place, by changing the memory mapping
template <class T>
TMatrix<T>& TMatrix<T>::TransposeSelf() 
{
  short vt = (marowi_ == veceli_) ? ColumnVector : RowVector;
  int_4 rci = macoli_;
  macoli_ = marowi_;
  marowi_ = rci;
  veceli_ = (vt ==  ColumnVector ) ?  marowi_ : macoli_;
  return(*this);
}


//! Returns the transpose of the original matrix. 
/*!
  The data is shared between the two matrices
  \return return a new matrix
 */
template <class T>
TMatrix<T> TMatrix<T>::Transpose() const
{
  TMatrix<T> tm(*this);
  tm.TransposeSelf();
  return tm;
}

//! Returns a new matrix, corresponding to the transpose of the original matrix
/*!
  \param mm : define the memory mapping type
    (SameMemoryMapping,CMemoryMapping,FortranMemoryMapping)
  \return return a new matrix
 */
template <class T>
TMatrix<T> TMatrix<T>::Transpose(short mm) const
{
  if (mm == SameMemoryMapping) mm = GetMemoryMapping();  
  TMatrix<T> tm(NCols(), NRows(), mm);
  for(sa_size_t i=0; i<NRows(); i++)
    for(sa_size_t j=0; j<NCols(); j++) 
      tm(j,i) = (*this)(i,j);
  return tm;
}

//! Rearrange data in memory according to \b mm
/*!
  \param mm : define the memory mapping type
    (SameMemoryMapping,CMemoryMapping,FortranMemoryMapping)
  \warning If identical, return a matrix that share the datas
 */
template <class T>
TMatrix<T> TMatrix<T>::Rearrange(short mm) const 
{
  if ( mm == SameMemoryMapping)  mm = GetMemoryMapping();
  else if ( (mm != CMemoryMapping) && (mm != FortranMemoryMapping) ) 
    mm = GetDefaultMemoryMapping();
  
  if  (mm == GetMemoryMapping())
    return (TMatrix<T>(*this, true));
  
  TMatrix<T> tm(NRows(), NCols(), mm);
  for(sa_size_t i=0; i<NRows(); i++)
    for(sa_size_t j=0; j<NCols(); j++) 
      tm(i,j) = (*this)(i,j);
  return tm;
}

//! Set the matrix to the identity matrix \b imx
template <class T>
TMatrix<T>& TMatrix<T>::SetIdentity(IdentityMatrix imx)
{
  if (ndim_ == 0) {
    sa_size_t sz = imx.Size();
    if (sz < 1) sz = 1;
    ReSize(sz, sz);
  }
  T diag = (T)imx.Diag();
  if (NRows() != NCols()) 
    throw SzMismatchError("TMatrix::operator= (IdentityMatrix) NRows() != NCols()") ;
  *this = (T) 0;
  for(sa_size_t i=0; i<NRows(); i++) (*this)(i,i) = diag;

  return (*this);
}



////////////////////////////////////////////////////////////////
//**** Impression
//! Return info on number of rows, column and type \b T
template <class T>
string TMatrix<T>::InfoString() const
{
  string rs = "TMatrix<";
  rs += typeid(T).name();
  char buff[64];
  sprintf(buff, ">(NRows=%ld, NCols=%ld)", (long)NRows(), (long)NCols());
  rs += buff;
  return(rs);  
}

//! Print matrix
/*!
  \param os : output stream
  \param maxprt : maximum numer of print
  \param si : if true,  display attached DvList
  \param ascd : if true, suppresses the display of line numbers,
  suitable for ascii dump format.
  \sa SetMaxPrint
 */
template <class T>
ostream& TMatrix<T>::Print(ostream& os, sa_size_t maxprt, bool si, bool ascd) const
{
  if (maxprt < 0)  maxprt = max_nprt_;
  sa_size_t npr = 0;

  // keep stream's io flags 
  // ios_base::fmtflags ioflg = os.flags(); compil pas sur OSF-cxx
  // os << right ;   compile pas sur OSF-cxx

  Show(os, si);
  if (ndim_ < 1)  return os;
  if (si) {
    bool fgdu;
    Units un = GetUnits(fgdu);
    if (fgdu) os << "Units: " << un.Name() << endl;
  }
  // Calcul de la largeur d'impression pour chaque element
  int fprtw = os.precision()+7;
  int prtw = 5;

  if ( (typeid(T) == typeid( int_4 )) || (typeid(T) == typeid( uint_4 )) ) prtw = 8;
  else if ( (typeid(T) == typeid( int_8 )) || (typeid(T) == typeid( uint_8 )) ) prtw = 11;
  else if ( typeid(T) == typeid( r_4 ) ) prtw = fprtw;
  else if ( typeid(T) == typeid( r_8 ) ) prtw = fprtw;
  else if ( typeid(T) == typeid(complex<r_4>) ) prtw = fprtw;
  else if ( typeid(T) == typeid(complex<r_8>) ) prtw = fprtw;

  sa_size_t kc,kr;  
  for(kr=0; kr<size_[marowi_]; kr++) {
    if ( (size_[marowi_] > 1) && (size_[macoli_] > 10) && !ascd) 
      os << "----- Line= " << kr << endl;
    for(kc=0; kc<size_[macoli_]; kc++) {
      if(kc > 0) os << " ";  
      os << setw(prtw) << (*this)(kr, kc);   npr++; 
      if (npr >= (sa_size_t) maxprt) {
	if (npr < totsize_)  os << "\n     .... " << endl; return os;
      }
    }
    os << endl;
  }
  os << endl;
  //compile pas sur OSF-cxx os.flags(ioflg);  // reset stream io flags
  return os;
}

//////////////////////////////////////////////////////////
/////////////// Multiplication matricielle ///////////////
//////////////////////////////////////////////////////////

//! Return the matrix product C = (*this)*B
/*!
  \param mm : define the memory mapping type for the return matrix
 */
////////////// Routine de base sans optimisation //////////////
/*
template <class T>
TMatrix<T> TMatrix<T>::Multiply(const TMatrix<T>& b, short mm) const
{
  if (NCols() != b.NRows()) 
    throw(SzMismatchError("TMatrix<T>::Multiply(b) NCols() != b.NRows() ") );
  if (mm == SameMemoryMapping) mm = GetMemoryMapping();  
  TMatrix<T> rm(NRows(), b.NCols(), mm);

  const T * pea;
  const T * peb;
  T sum;
  sa_size_t r,c,k;
  sa_size_t stepa = Step(ColsKA());
  sa_size_t stepb = b.Step(b.RowsKA());
  // Calcul de C=rm = A*B   (A=*this)
  for(r=0; r<rm.NRows(); r++)      // Boucle sur les lignes de A
    for(c=0; c<rm.NCols(); c++) {     // Boucle sur les colonnes de B
      sum = 0;
      pea = &((*this)(r,0));       // 1er element de la ligne r de A
      peb = &(b(0,c));                // 1er element de la colonne c de B
      for(k=0; k<NCols(); k++)  sum += pea[k*stepa]*peb[k*stepb];
      rm(r,c) = sum;
    }

  return rm;
}
*/

////////////// Routine optimisee //////////////
template <class T>
TMatrix<T> TMatrix<T>::Multiply(const TMatrix<T>& b, short mm) const
// Calcul de C= rm = A*B   (A=*this)
// Remember: C-like      matrices are column packed
//           Fortan-like matrices are line   packed
{
  if (NCols() != b.NRows()) 
    throw(SzMismatchError("TMatrix<T>::Multiply(b) NCols() != b.NRows() ") );

  // Commentaire: pas de difference de vitesse notable selon le mapping de la matrice produit "rm"
  if (mm == SameMemoryMapping) mm = GetMemoryMapping();
  TMatrix<T> rm(NRows(), b.NCols(), mm);

  // Les "steps" pour l'adressage des colonnes de A et des lignes de B
  sa_size_t stepa = Step(ColsKA());
  sa_size_t stepb = b.Step(b.RowsKA());

  // Taille totale des matrices A et B
  size_t totsiza = this->DataBlock().Size();
  size_t totsizb = b.DataBlock().Size();


  ///////////////////////////////////////////////////////////////////
  // On decide si on optimise ou non selon les dimensions de A et B //
  // (il semble que optimiser ou non ne degrade pas                 //
  //  beaucoup la vitesse pour les petites matrices)                //
  ////////////////////////////////////////////////////////////////////

  uint_2 popt = GetMatProdOpt();
  bool no_optim = false; // optimization demandee par default
  if( (popt&(uint_2)1) == 0 ) { // pas d'optimization explicitement demande
    no_optim = true;
  } else if( (popt&(uint_2)2) == 0 ) { // pas d'optimization forcee, la methode decide
    // On part sur une disponibilite dans le cache processeur de 100 ko
    // (A et B peuvent etre stoquees dans le cache)
    if((totsiza+totsizb)*sizeof(T)<100000) no_optim = true;
  }

  sa_size_t r,c,k;
  T sum;
  const T * pe;


  /////////////////////////////////
  // Pas d'optimisation demandee //
  /////////////////////////////////

  if( no_optim ) {
    //cout<<"no_optim("<<no_optim<<") "<<stepa<<" "<<stepb<<endl;
    const T * pea;
    const T * peb;
    for(r=0; r<rm.NRows(); r++) {     // Boucle sur les lignes de A
      for(c=0; c<rm.NCols(); c++) {   // Boucle sur les colonnes de B
        sum = 0;
        pea = &((*this)(r,0));
        peb = &(b(0,c));
	// On gagne un peu en remplacant "pea[k*stepa]" par "pea+=stepa" pour les grosses matrices
        //for(k=0; k<NCols(); k++)  sum += pea[k*stepa]*peb[k*stepb];
        for(k=0; k<NCols(); k++) {sum += (*pea)*(*peb); pea+=stepa; peb+=stepb;}
        rm(r,c) = sum;
      }
    }
    return rm;
  }


  //////////////////////////////////////////////////////////////////////////////////
  // A.col est packed  et  B.row est packed (on a interet a optimiser quand meme) //
  //////////////////////////////////////////////////////////////////////////////////

  if(stepa==1 && stepb==1) {
    //cout<<"A.col packed && B.row packed "<<stepa<<" "<<stepb<<endl;
     T * pea = new T[NCols()];
    const T * peb;
    for(r=0; r<rm.NRows(); r++) {     // Boucle sur les lignes de A
      pe = &((*this)(r,0));
      for(k=0; k<NCols(); k++) {pea[k] = *(pe++);}
      for(c=0; c<rm.NCols(); c++) {   // Boucle sur les colonnes de B
        sum = 0;
        peb = &(b(0,c));
        for(k=0; k<NCols(); k++) sum += *(peb++)*pea[k];
        rm(r,c) = sum;
      }
    }
    delete [] pea;
    return rm;
  }


  //////////////////////////////////////////////////
  // A.col est packed  et  B.row n'est pas packed //
  //////////////////////////////////////////////////

  if(stepa==1 && stepb!=1) {
    //cout<<"A.col packed && B.row not packed "<<stepa<<" "<<stepb<<endl;
    const T * pea;
    T * peb = new T[NCols()];
    for(c=0; c<rm.NCols(); c++) {     // Boucle sur les colonnes de B
      pe = &(b(0,c));
      for(k=0; k<NCols(); k++) {peb[k] = *pe; pe+=stepb;}
      for(r=0; r<rm.NRows(); r++) {   // Boucle sur les lignes de A
        sum = 0;
        pea = &((*this)(r,0));
        for(k=0; k<NCols(); k++) sum += pea[k]*peb[k];
        rm(r,c) = sum;
      }
    }
    delete [] peb;
    return rm;
  }


  //////////////////////////////////////////////////
  // A.col n'est pas packed  et  B.row est packed //
  //////////////////////////////////////////////////

  if(stepa!=1 && stepb==1) {
    //cout<<"A.col not packed && B.row packed "<<stepa<<" "<<stepb<<endl;
    T * pea = new T[NCols()];
    const T * peb;
    for(r=0; r<rm.NRows(); r++) {     // Boucle sur les lignes de A
      pe = &((*this)(r,0));
      for(k=0; k<NCols(); k++) {pea[k] = *pe; pe+=stepa;}
      for(c=0; c<rm.NCols(); c++) {   // Boucle sur les colonnes de B
        sum = 0;
        peb = &(b(0,c));
        for(k=0; k<NCols(); k++) sum += pea[k]*peb[k];
        rm(r,c) = sum;
      }
    }
    delete [] pea;
    return rm;
  }


  ////////////////////////////////////////////////////////
  // A.col n'est pas packed  et  B.row n'est pas packed //
  ////////////////////////////////////////////////////////

  //---- On demande l'optimization par copie d'une des matrices

  if( (popt&(uint_2)4) ) {
    // On copie la plus petite
    if(totsiza<totsizb) {   // on copie A
      //cout<<"A.col not packed && B.row not packed ==> copy A to optimize "<<stepa<<" "<<stepb<<endl;
      // Acopy doit etre C-like pour etre column-packed
      TMatrix<T> acopy(NRows(),NCols(),BaseArray::CMemoryMapping);
      acopy = *this;
      rm = acopy.Multiply(b,mm);
    } else {               // on copie B
      //cout<<"A.col not packed && B.row not packed ==> copy B to optimize "<<stepa<<" "<<stepb<<endl;
      // Bcopy doit etre Fortran-like pour etre column-packed
      TMatrix<T> bcopy(b.NRows(),b.NCols(),BaseArray::FortranMemoryMapping);
      bcopy = b;
      rm = Multiply(bcopy,mm);
    }
    return rm;
  }
 
  //---- stepb>stepa

  if(stepa!=1 && stepb!=1 && stepb>stepa) {
    //cout<<"A.col not packed && B.row not packed ==> optimize on B "<<stepa<<" "<<stepb<<endl;
    const T * pea;
    T * peb = new T[NCols()];
    for(c=0; c<rm.NCols(); c++) {     // Boucle sur les colonnes de B
      pe = &(b(0,c));
      for(k=0; k<NCols(); k++) {peb[k] = *pe; pe+=stepb;}
      for(r=0; r<rm.NRows(); r++) {     // Boucle sur les lignes de A
        sum = 0;
        pea = &((*this)(r,0));
        for(k=0; k<NCols(); k++) {sum += (*pea)*peb[k]; pea+=stepa;}
        rm(r,c) = sum;
      }
    }
    delete [] peb;
    return rm;
  }

  //---- stepa>=stepb

  if(stepa!=1 && stepb!=1) {
    //cout<<"A.col not packed && B.row not packed ==> optimize on A "<<stepa<<" "<<stepb<<endl;
    T * pea = new T[NCols()];
    const T * peb;
    for(r=0; r<rm.NRows(); r++) {     // Boucle sur les lignes de A
      pe = &((*this)(r,0));
      for(k=0; k<NCols(); k++) {pea[k] = *pe; pe+=stepa;}
      for(c=0; c<rm.NCols(); c++) {     // Boucle sur les colonnes de B
        sum = 0;
        peb = &(b(0,c));
        for(k=0; k<NCols(); k++) {sum += pea[k]*(*peb); peb+=stepb;}
        rm(r,c) = sum;
      }
    }
    delete [] pea;
    return rm;
  }


  //////////////////////////////////////////////////
  // Cas non prevu, on ne doit JAMAIS arriver ici //
  //////////////////////////////////////////////////
  cout<<"TMatrix<T>::Multiply(b) Optimize case not treated... Please report BUG !!! "<<endl;
  throw(SzMismatchError("TMatrix<T>::Multiply(b) Optimize case not treated... Please report BUG !!! ") );
  return rm;

}


///////////////////////////////////////////////////////////////
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template TMatrix<uint_1>
#pragma define_template TMatrix<uint_2>
#pragma define_template TMatrix<uint_4>
#pragma define_template TMatrix<uint_8>
#pragma define_template TMatrix<int_1>
#pragma define_template TMatrix<int_2>
#pragma define_template TMatrix<int_4>
#pragma define_template TMatrix<int_8>
#pragma define_template TMatrix<r_4>
#pragma define_template TMatrix<r_8> 
#pragma define_template TMatrix< complex<r_4> > 
#pragma define_template TMatrix< complex<r_8> > 
#ifdef SO_LDBLE128
#pragma define_template TMatrix<r_16> 
#pragma define_template TMatrix< complex<r_16> > 
#endif
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class TMatrix<uint_1>;
template class TMatrix<uint_2>;
template class TMatrix<uint_4>;
template class TMatrix<uint_8>;
template class TMatrix<int_1>;
template class TMatrix<int_2>;
template class TMatrix<int_4>;
template class TMatrix<int_8>;
template class TMatrix<r_4>;
template class TMatrix<r_8>;
template class TMatrix< complex<r_4> >;
template class TMatrix< complex<r_8> >;
#ifdef SO_LDBLE128
template class TMatrix<r_16>;
template class TMatrix< complex<r_16> >;
#endif
#endif

} // FIN namespace SOPHYA 
