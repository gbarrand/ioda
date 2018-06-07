// This may look like C code, but it is really -*- C++ -*-
#ifndef SOpeMatrix_SEEN
#define SOpeMatrix_SEEN

#include "machdefs.h"
#include "tmatrix.h"
#include "tvector.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//------------------------------------------------------------//
//      La classe de calcul simple sur les TMatrix            //
//------------------------------------------------------------//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

namespace SOPHYA {

/*!
  \class SimpleMatrixOperation
  \ingroup TArray
  \brief Class for simple operation on TMatrix
  This class provides presently a basic implementation of the Gauss 
  pivoting for linear system solving and matrix inversion. 

  \warning For large or ill conditioned matrices, use the methods 
  from LinAlg module. 
  \sa SOPHYA::LapackServer  SOPHYA::TMatrix 
*/
//! Class for simple operation on TMatrix
template <class T>
class SimpleMatrixOperation {
public:
  //! To define the type of data scaling for Gauss pivoting method (type T)
  enum GausPivScal {
    PIV_NO_SCALE = 0,   //!< do not scale datas for gauss pivoting
    PIV_GLOB_SCALE = 1, //!< do a global scaling of datas for gauss pivoting (default)
    PIV_ROW_SCALE = 2   //!< do a scaling by row of datas for gauss pivoting
  };
  static inline int SetGausPivScal(int gps = PIV_GLOB_SCALE)
              { GausPivScaling = gps; return GausPivScaling; }

  static TMatrix<T> Inverse(TMatrix<T> const & A);
  static T GausPiv(TMatrix<T>& A, TMatrix<T>& B, bool computedet=false);

protected:
  static int GausPivScaling;
};

} // Fin du namespace

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//------------------------------------------------------------//
//              Resolution de systemes lineaires              //
//------------------------------------------------------------//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

namespace SOPHYA {

//------------------------------------------------------------
// Resolution du systeme A*C = B
//------------------------------------------------------------
/*! \ingroup TArray \fn LinSolveInPlace(TMatrix<T>&,TVector<T>&)
    \brief  Solve A*C = B for C in place and return 1 if OK, 0 if not.
*/
template <class T>
inline T LinSolveInPlace(TMatrix<T>& a, TVector<T>& b)
{
if(a.NCols() != b.NRows() || a.NCols() != a.NRows())
  throw(SzMismatchError("LinSolveInPlace(TMatrix<T>,TVector<T>) size mismatch"));
return SimpleMatrixOperation<T>::GausPiv(a,b);
}

//------------------------------------------------------------
// Resolution du systeme A*C = B, avec C retourne dans B
//------------------------------------------------------------
/*! \ingroup TArray 
    \fn LinSolve(const TMatrix<T>&, const TVector<T>&, TVector<T>&)
    \brief Solve A*C = B and return C. Return 1 if OK, 0 if not.
*/
template <class T>
inline T LinSolve(const TMatrix<T>& a, const TVector<T>& b, TVector<T>& c) {
  if(a.NCols()!=b.NRows() || a.NCols()!=a.NRows())
    throw(SzMismatchError("LinSolve(TMatrix<T>,TVector<T>) size mismatch"));
    c = b; TMatrix<T> a1(a);
  return SimpleMatrixOperation<T>::GausPiv(a1,c);
}

} // Fin du namespace

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//------------------------------------------------------------//
//                   Inverse d'une matrice                    //
//------------------------------------------------------------//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

namespace SOPHYA {

/*! \ingroup TArray 
    \fn Inverse(TMatrix<T> const &)
    \brief To inverse a TMatrix.
*/
template <class T>
inline TMatrix<T> Inverse(TMatrix<T> const & A)
  {return SimpleMatrixOperation<T>::Inverse(A);}

/*! \ingroup TArray 
    \fn Determinant(TMatrix<T> const &)
    \brief Return the TMatrix determinant
*/
template <class T>
inline T Determinant(TMatrix<T> const & A) {
  TMatrix<T> a(A,false);
  TMatrix<T> b(a.NCols(),a.NRows());  b = IdentityMatrix(1.);
  return SimpleMatrixOperation<T>::GausPiv(a,b,true);
}

/*! \ingroup TArray 
    \fn GausPiv(TMatrix<T> const &,TMatrix<T> &, bool)
    \brief Gauss pivoting on matrix \b A, doing the same operations
           on matrix \b B.
    \param computedet = true : return the determinant of \b A
           (beware of over/underfloat), default is false.
    \return determinant if requested, or 1 if OK.
    \sa SOPHYA::SimpleMatrixOperation::GausPiv(TMatrix<T>&,TMatrix<T>&,bool)
*/
template <class T>
inline T GausPiv(TMatrix<T> const & A,TMatrix<T> & B, bool computedet=false) {
  TMatrix<T> a(A,false);
  return SimpleMatrixOperation<T>::GausPiv(a,B,computedet);
}


} // Fin du namespace


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//------------------------------------------------------------//
//                   Linear fitting                           //
//------------------------------------------------------------//
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

namespace SOPHYA {

/*!
  \class LinFitter
  \ingroup TArray
  Class for linear fitting
  \sa TMatrix TArray
*/

//!  Class for linear fitting
template <class T>
class LinFitter {

public :

         LinFitter();
virtual	~LinFitter();

//! Linear fitting
r_8 LinFit(const TVector<T>& x, const TVector<T>& y,
	   sa_size_t nf, T (*f)(sa_size_t,T), TVector<T>& c);

//! Linear fitting
r_8 LinFit(const TMatrix<T>& fx, const TVector<T>& y, TVector<T>& c);
		      
//! Linear fitting with errors
r_8 LinFit(const TVector<T>& x, const TVector<T>& y, const TVector<T>& errY2,
           sa_size_t nf,T (*f)(sa_size_t,T), TVector<T>& c, TVector<T>& errC);

//! Linear fitting with errors
r_8 LinFit(const TMatrix<T>& fx, const TVector<T>& y,
           const TVector<T>& errY2, TVector<T>& c, TVector<T>& errC);
};

} // Fin du namespace

#endif
