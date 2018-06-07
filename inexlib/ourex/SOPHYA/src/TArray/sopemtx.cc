//                         C.Magneville, R.Ansari        03/2000

#include "sopnamsp.h"
#include "machdefs.h"
#include <stdio.h>
#include <iostream>
#include <complex>
#include <math.h>
#include "sopemtx.h"
#include "smathconst.h"

////////////////////////////////////////////////////////////////
// ---------------------------------------------------------- //
//  La classe de gestion des lignes et colonnes d'une matrice //
// ---------------------------------------------------------- //
////////////////////////////////////////////////////////////////

/*! \cond
  \class SOPHYA::TMatrixRC
  \ingroup TArray
  Class for representing rows, columns and diagonal of a matrix.
  \sa TMatrix TArray
*/

/*! \endcond */

namespace SOPHYA {
template <class T>
class TMatrixRC {
public:
  //! Define type of TMatrixRC
  enum TRCKind {
    TmatrixRow=0,  //!< TMatrixRC ligne
    TmatrixCol=1,  //!< TMatrixRC column
    TmatrixDiag=2  //!< TMatrixRC diagonal
  };
  TMatrixRC();
  TMatrixRC(TMatrix<T>& m, TRCKind kind, sa_size_t index=0);
  virtual ~TMatrixRC() {}

  // Acces aux rangees et colonnes de matrices
  static TMatrixRC<T> Row(TMatrix<T> & m, sa_size_t r);
  static TMatrixRC<T> Col(TMatrix<T> & m, sa_size_t c);
  static TMatrixRC<T> Diag(TMatrix<T> & m);

  int_4 SetCol(int_4 c);
  int_4 SetRow(int_4 r);
  int_4 SetDiag();

  static sa_size_t Step(const TMatrix<T>& m, TRCKind rckind);
  static T* Org(const TMatrix<T>&, TRCKind rckind, sa_size_t ind=0);

  //! Return the kind of TMatrix (line,column,diagonal)
  TRCKind Kind() const { return kind; }
  sa_size_t NElts() const;
  T& operator()(sa_size_t i);
  T  operator()(sa_size_t i) const;


   TMatrixRC<T>& operator = (const TMatrixRC<T>& rc);


  TMatrixRC<T>& operator *= (T x);
  TMatrixRC<T>& operator /= (T x);

//   TMatrixRC<T>& operator -= (T x);
//   TMatrixRC<T>& operator += (T x);
  

  TMatrixRC<T>& LinComb(T a, T b, const TMatrixRC& rc, sa_size_t first=0);
  TMatrixRC<T>& LinComb(T b, const TMatrixRC<T>& rc, sa_size_t first=0);

  sa_size_t IMaxAbs(sa_size_t first=0);
  void Print(ostream & os) const ;

  static void Swap(TMatrixRC<T>& rc1, TMatrixRC<T>& rc2);

  //! Define Absolute value for uint_1
  inline static double Abs_Value(uint_1 v) {return (double) v;}
  //! Define Absolute value for uint_2
  inline static double Abs_Value(uint_2 v) {return (double) v;}
  //! Define Absolute value for int_2
  inline static double Abs_Value(int_2 v)  {return (v>0)? (double) v: (double) -v;}
  //! Define Absolute value for int_4
  inline static double Abs_Value(int_4 v)  {return (v>0)? (double) v: (double) -v;}
  //! Define Absolute value for int_8
  inline static double Abs_Value(int_8 v)  {return (v>0)? (double) v: (double) -v;}
  //! Define Absolute value for sa_size_t
  inline static double Abs_Value(uint_4 v) {return (double) v;}
  //! Define Absolute value for uint_8
  inline static double Abs_Value(uint_8 v) {return (double) v;}
  //! Define Absolute value for r_4
  inline static double Abs_Value(r_4 v)    {return (double) fabs((double)v);}
  //! Define Absolute value for r_8
  inline static double Abs_Value(r_8 v)    {return fabs(v);}
  //! Define Absolute value for complex r_4
  inline static double Abs_Value(complex<r_4> v)
    {return std::abs(v); } 
  //! Define Absolute value for complex r_8
  inline static double Abs_Value(complex<r_8> v)
    {return std::abs(v); } 
#ifdef SO_LDBLE128
  //! Define Absolute value for r_16
  inline static double Abs_Value(r_16 v)  {return (v>0)? v: -v;}
  //! Define Absolute value for complex r_16
  inline static double Abs_Value(complex<r_16> v)
    {return std::abs(v); } 
#endif

protected:
  TMatrix<T>* matrix;  //!< pointer to the TMatrix
  T*          data;    //!< pointer to the beginnig of interesting datas
  int_4       index;   //!< index of the line/column
  sa_size_t      step;    //!< step of the line/column
  TRCKind     kind;    //!< type: line, column or diagonal
};

//! Scalar product of two TMatrixRC
/*!
  \return sum[ a(i) * b(i) ]
 */
template <class T>
inline T operator * (const TMatrixRC<T>& a, const TMatrixRC<T>& b)
  {
  if ( a.NElts() != b.NElts() )
    throw(SzMismatchError("TMatrixRC::operator * size mismatch\n"));
  if ( a.Kind() != b.Kind() )
    throw(SzMismatchError("TMatrixRC::operator * type mismatch\n"));
  T sum = 0;
  for(sa_size_t i=0; i<a.NElts(); i++) sum += a(i)*b(i);
  return sum;
  }

//! Get the step in datas for a TMatrix for type rckind
/*!
  \param rckind : line, column or diagonal
  \return step in TMatrix along TMatrixRC
 */
template <class T>
inline sa_size_t TMatrixRC<T>::Step(const TMatrix<T>& m, TRCKind rckind)
  { switch (rckind) { case TmatrixRow  : return m.Step(m.ColsKA());
                      case TmatrixCol  : return m.Step(m.RowsKA());
                      case TmatrixDiag : return (m.Step(m.RowsKA())+m.Step(m.ColsKA())); }
    return 0; }

//! Get the origin of datas.
/*!
  Get the origin of datas in the TMatrix for a TMatrixRC for type
  \b rckind and index \b index .
  \param rckind : line, column or diagonal
  \param index : index of the line or column.
  \return adress of the first element in datas.
 */
template <class T>
inline T* TMatrixRC<T>::Org(const TMatrix<T>& m, TRCKind rckind, sa_size_t index)
  { switch (rckind) { case TmatrixRow  : return const_cast<T *>(&(m(index,0)));
                      case TmatrixCol  : return const_cast<T *>(&(m(0,index)));
                      case TmatrixDiag : return const_cast<T *>(m.Data()); }
    return NULL; }

//! return number of elements for a TMatrixRC
template <class T> inline sa_size_t TMatrixRC<T>::NElts() const
  { if (!matrix) return 0;
    switch (kind) { case TmatrixRow  : return matrix->NCols();
                    case TmatrixCol  : return matrix->NRows();
                    case TmatrixDiag : return matrix->NCols(); }
    return 0; }

//! access of element \b i
template <class T>
inline T& TMatrixRC<T>::operator()(sa_size_t i) {return data[i*step];}
//! access of element \b i
template <class T>
inline T  TMatrixRC<T>::operator()(sa_size_t i) const {return data[i*step];}

////////////////////////////////////////////////////////////////
//! Typedef to simplify TMatrixRC<r_8> writing
typedef TMatrixRC<r_8> MatrixRC;


//! Default constructor
template <class T> TMatrixRC<T>::TMatrixRC()
: matrix(NULL), data(NULL), index(0), step(0)
{}

//! Constructor
/*!
  \param m : matrix
  \param rckind : select line, column or diagonal
  \param ind : number of the line or column
*/
template <class T> TMatrixRC<T>::TMatrixRC(TMatrix<T>& m,TRCKind rckind,sa_size_t ind)
: matrix(&m), data(Org(m,rckind,ind)),
  index(ind), step(Step(m,rckind)), kind(rckind)
{
if (kind == TmatrixDiag && m.NCols() != m.NRows())
  throw(SzMismatchError("TMatrixRC::TMatrixRC(...,TmatrixDiag,...) size mismatch\n"));
}

////////////////////////////////////////////////////////////////
// Acces aux rangees et colonnes de matrices

//! Return TMatrixRC for line \b r of matrix \b m
template <class T> 
TMatrixRC<T> TMatrixRC<T>::Row(TMatrix<T> & m, sa_size_t r) 
{
TMatrixRC<T> rc(m, TmatrixRow, r);
return rc;
}

//! Return TMatrixRC for column \b r of matrix \b m
template <class T> 
TMatrixRC<T> TMatrixRC<T>::Col(TMatrix<T> & m, sa_size_t c) 
{
TMatrixRC<T> rc(m, TmatrixCol, c);
return rc;
}

//! Return TMatrixRC for diagonal of matrix \b m
template <class T> 
TMatrixRC<T> TMatrixRC<T>::Diag(TMatrix<T> & m) 
{
TMatrixRC<T> rc(m, TmatrixDiag);
return rc;
}



//! Set column \b c for this TMatrixRC
template <class T> int_4 TMatrixRC<T>::SetCol(int_4 c)
{
if(!matrix) return -1;
if(c<0 || c>(int_4)matrix->NCols()) return -1;
kind = TmatrixCol;
index = c;
step = Step(*matrix, TmatrixCol);
data = Org(*matrix, TmatrixCol, c);
return c;
}

//! Set line \b r for this TMatrixRC
template <class T> int_4 TMatrixRC<T>::SetRow(int_4 r)
{
if(!matrix) return -1;
if(r<0 && r>(int_4)matrix->NRows()) return -1;
kind = TmatrixRow;
index = r;
step = Step(*matrix, TmatrixRow);
data = Org(*matrix, TmatrixRow, r);
return r;
}

//! Set line diaginal for this TMatrixRC
template <class T> int_4 TMatrixRC<T>::SetDiag()
{
if (!matrix) return -1;
if (matrix->NCols() != matrix->NRows())
  throw(SzMismatchError("TMatrixRC::SetDiag size mismatch\n"));
kind = TmatrixDiag;
index = 0;
step = Step(*matrix, TmatrixDiag);
data = Org(*matrix, TmatrixDiag);
return 0;
}

//! Operator =
template <class T> TMatrixRC<T>& TMatrixRC<T>::operator = (const TMatrixRC<T>& rc)
{
matrix = rc.matrix;
data   = rc.data;
index  = rc.index;
step   = rc.step;
kind   = rc.kind;
return *this;
}


//! Operator to multiply by constant \b x
template <class T> TMatrixRC<T>& TMatrixRC<T>::operator *= (T x)
{
for(sa_size_t i=0; i<NElts(); i++) (*this)(i) *= x;
return *this;
}

//! Operator to divide by constant \b x
template <class T> TMatrixRC<T>& TMatrixRC<T>::operator /= (T x)
{
for(sa_size_t i=0; i<NElts(); i++) (*this)(i) /= x;
return *this;
}



//! Linear combination
/*!
  Do : \f$ MRC(i) = MRC(i)*a + rc(i)*b \f$
  \return *this
 */
template <class T>
TMatrixRC<T>& TMatrixRC<T>::LinComb(T a, T b, const TMatrixRC<T>& rc, sa_size_t first)
{
if ( NElts() != rc.NElts() )
  throw(SzMismatchError("TMatrixRC::LinComb size mismatch\n"));
if ( kind != rc.kind )
  throw(SzMismatchError("TMatrixRC::LinComb type mismatch\n"));
for(sa_size_t i=first; i<NElts(); i++) (*this)(i) = (*this)(i)*a + rc(i)*b;
return *this;
}

//! Linear combination
/*!
  Do : \f$ MRC(i) = MRC(i) + rc(i)*b \f$
 */
template <class T>
TMatrixRC<T>& TMatrixRC<T>::LinComb(T b, const TMatrixRC<T>& rc, sa_size_t first)
{
if ( NElts() != rc.NElts() )
  throw(SzMismatchError("TMatrixRC::LinComb size mismatch\n"));
if ( kind != rc.kind )
  throw(SzMismatchError("TMatrixRC::LinComb type mismatch\n"));
for(sa_size_t i=first; i<NElts(); i++) (*this)(i) += rc(i)*b;
return *this;
}

//! Find maximum absolute value in TMatrixRC, search begin at \b first
template <class T> sa_size_t TMatrixRC<T>::IMaxAbs(sa_size_t first)
{
if (first>NElts())
  throw(SzMismatchError("TMatrixRC::IMaxAbs size mismatch\n"));
sa_size_t imax = first;
double vmax = Abs_Value((*this)(first));
for(sa_size_t i=first+1; i<NElts(); i++) {
  double v = Abs_Value((*this)(i));
  if(v > vmax) {vmax = v; imax = i;}
}
return imax;
}

//! Print on stream \b os
template <class T>
void TMatrixRC<T>::Print(ostream & os) const
{
  os << " TMatrixRC<T>::Print(ostream & os) " << NElts() << " Kind=" 
     << kind << " Index=" << index << " Step= " << step << endl;
  for(sa_size_t i=0; i<NElts(); i++) {
    os << (*this)(i);
    if (kind == TmatrixCol)  os << endl;
    else os << ", "; 
  }
  os << endl;
}

//! Swap two TMatrixRC of the same kind
template <class T>
void TMatrixRC<T>::Swap(TMatrixRC<T>& rc1, TMatrixRC<T>& rc2)
{
if(rc1.NElts() != rc2.NElts())
  throw(SzMismatchError("TMatrixRC::Swap size mismatch\n"));
if(rc1.kind != rc2.kind)
  throw(SzMismatchError("TMatrixRC::Swap type mismatch\n"));
if(rc1.data == rc2.data) return;
for(sa_size_t i=0; i<rc1.NElts(); i++)
  {T tmp = rc1(i); rc1(i) = rc2(i); rc2(i) = tmp;}
}


////////////////////////////////////////////////////////////////
// ---------------------------------------------------------- //
//      La classe de calcul simple sur les TMatrix            //
// ---------------------------------------------------------- //
////////////////////////////////////////////////////////////////

//**** Pour inversion
#ifndef M_LN2
#define M_LN2 0.69314718055994530942
#endif
//// CMV BUG BUG : sur OSF 5.0 DMINEXP est deconnant (~1.e+19 !!!)
#ifndef LN_MINDOUBLE
#define LN_MINDOUBLE  (M_LN2 * (DMINEXP - 1))
#endif
#ifndef LN_MAXDOUBLE
#define LN_MAXDOUBLE  (M_LN2 * DMAXEXP)
#endif

template <class T>
int SimpleMatrixOperation<T>::GausPivScaling = PIV_GLOB_SCALE;

//! Gaussian pivoting
/*!
  Do Gauss pivoting of \b a, doing the same operations on matrix \b b
  \param computedet = true : return determimant of \b a (beware of over/underfloat)
  \param computedet = false : return 1 if OK, 0 if not.
  \verbatim
  Solve linear system A(n,n) * X(n,m) = B(n,m)
  and put solution X in B for return.
  \endverbatim
  \warning If \b b is identity matrix, return inverse of \b a
  \warning matrix \b a and \b b are modified.
 */  
template <class T>
T SimpleMatrixOperation<T>::GausPiv(TMatrix<T>& a, TMatrix<T>& b,bool computedet)
// Pivot de Gauss
// * Attention: egcs impose que cette fonction soit mise dans le .cc
//              avant ::Inverse() (car Inverse() l'utilise)
// {TMatrix A(a); TMatrix B(b); return (T) TMatrix::GausPiv(A,B);}
{
sa_size_t n = a.NRows();
if(n!=b.NRows())
  throw(SzMismatchError("TMatrix::GausPiv size mismatch\n"));

T det = 1;

//////////////////
// Data scaling //
//////////////////

// Pas de normalisation des donnees
if(GausPivScaling==PIV_NO_SCALE) {
  if(computedet) det = (T) 1;
// normalisation des donnees ligne par ligne
} else if(GausPivScaling==PIV_ROW_SCALE) {
  double nrm = 0.;
  for(sa_size_t iii=0; iii<a.NRows(); iii++) {
    sa_size_t jjj; double vmax = -1.;
    for(jjj=0; jjj<a.NCols(); jjj++) {
      double v = TMatrixRC<T>::Abs_Value(a(iii,jjj));
      if(v>vmax) vmax = v;
    }
    if(vmax>0.) {
      for(jjj=0; jjj<a.NCols(); jjj++) a(iii,jjj) /= (T) vmax;
      for(jjj=0; jjj<b.NCols(); jjj++) b(iii,jjj) /= (T) vmax;
      nrm += log(vmax);
    } else return (T) 0;
  }
  if(computedet) {
    if(nrm <= LN_MINDOUBLE || nrm >= LN_MAXDOUBLE) {
      cerr<<"GausPiv_Row: normalisation failure, "
          <<"determinant has to be multiplied by exp("<<nrm<<")"<<endl;
    } else det = (T) exp(nrm);
  }
// On fait une normalisation un peu brutale globale
} else {
  double vmin=MAXDOUBLE, vmax=0;
  for(sa_size_t iii=0; iii<a.NRows(); iii++)
    for(sa_size_t jjj=0; jjj<a.NCols(); jjj++) {
      double v = TMatrixRC<T>::Abs_Value(a(iii,jjj));
      if(v>vmax) vmax = v;
      if(v<vmin && v>0.) vmin = v;
  }
  double nrm = sqrt(vmin*vmax);
  if(nrm>0.) { a /= (T) nrm; b /= (T) nrm;} else return (T) 0;
  if(computedet) {
    nrm = a.NRows() * log(nrm);
    if (nrm <= LN_MINDOUBLE || nrm >= LN_MAXDOUBLE) {
      cerr<<"GausPiv_Glob: normalisation failure, "
          <<"determinant has to be multiplied by exp("<<nrm<<")"<<endl;
    } else det = (T) exp(nrm);
  }
}

////////////////////////////////////////
// Gaussian elimination with pivoting //
////////////////////////////////////////

TMatrixRC<T> pivRowa(a,TMatrixRC<T>::TmatrixRow);
TMatrixRC<T> pivRowb(b,TMatrixRC<T>::TmatrixRow);

for(sa_size_t k=0; k<n-1; k++) {
  sa_size_t iPiv = TMatrixRC<T>::Col(a, k).IMaxAbs(k);
  if(iPiv != k) {
    TMatrixRC<T> aIPiv(TMatrixRC<T>::Row(a,iPiv));
    TMatrixRC<T> aK(TMatrixRC<T>::Row(a, k));
    TMatrixRC<T>::Swap(aIPiv,aK);
    TMatrixRC<T> bIPiv(TMatrixRC<T>::Row(b, iPiv));
    TMatrixRC<T> bK(TMatrixRC<T>::Row(b, k));
    TMatrixRC<T>::Swap(bIPiv,bK);
  }
  T pivot = a(k,k);
  if( TMatrixRC<T>::Abs_Value(pivot) < 1.e-50 ) return (T) 0;
  if(computedet) det *= pivot;
  pivRowa.SetRow(k); // to avoid constructors
  pivRowb.SetRow(k);
  for (sa_size_t i=k+1; i<n; i++) {
    T r = -a(i,k)/pivot;
    TMatrixRC<T>::Row(a, i).LinComb(r, pivRowa); // + rapide que -= r * pivRowa
    TMatrixRC<T>::Row(b, i).LinComb(r, pivRowb);
  }
}
if(computedet) det *= a(n-1, n-1);

// on remonte
for(sa_size_t kk=n-1; kk>0; kk--) {
  T pivot = a(kk,kk);
  if( TMatrixRC<T>::Abs_Value(pivot) <= 1.e-50 ) return (T) 0;
  pivRowa.SetRow(kk); // to avoid constructors
  pivRowb.SetRow(kk);
  for(sa_size_t jj=0; jj<kk; jj++) {
    T r = -a(jj,kk)/pivot;
    TMatrixRC<T>::Row(a, jj).LinComb(r, pivRowa);
    TMatrixRC<T>::Row(b, jj).LinComb(r, pivRowb);
  }
}

for(sa_size_t l=0; l<n; l++) {
  if( TMatrixRC<T>::Abs_Value(a(l,l)) <= 1.e-50 ) return (T) 0;
  TMatrixRC<T>::Row(b, l) /= a(l,l);
}

return det;
}

//! Return the inverse matrix of \b A
template <class T>
TMatrix<T> SimpleMatrixOperation<T>::Inverse(TMatrix<T> const & A) 
{
TMatrix<T> a(A,false);
TMatrix<T> b(a.NCols(),a.NRows());  b = IdentityMatrix(1.);
if(GausPiv(a,b)==(T) 0)
  throw(MathExc("TMatrix Inverse() Singular Matrix"));
return b;
}


////////////////////////////////////////////////////////////////
// ---------------------------------------------------------- //
//               La classe fit lineaire                       //
// ---------------------------------------------------------- //
////////////////////////////////////////////////////////////////

//! Creator
template <class T>
LinFitter<T>::LinFitter()
{
}

//! Destructor
template <class T>
LinFitter<T>::~LinFitter()
{
}

// fit lineaire des y(k) en tant que somme de c(i)f(i,x(k)), i=0..nf-1;
//! Linear fitting
/*!
  Linear fit of y(k) as the sum of \f$ c(i)f(i,x(k)), i=0..nf-1 \f$
  \param x : vector of X values
  \param y : vector of datas
  \param nf: number of functions
  \param f : f(i,x(k)), i=0..nf-1
  \return c : vector of solutions
  \return return chisquare
 */
template <class T>
r_8 LinFitter<T>::LinFit(const TVector<T>& x, const TVector<T>& y,
                       sa_size_t nf, T (*f)(sa_size_t,T), TVector<T>& c)
{
sa_size_t n = x.NElts();
if (n != y.NElts()) 
  throw SzMismatchError("LinFitter<T>::LinFit(x,y,nf,f,c)/Error x.NElts() <> y.Nelts() ");
 
TMatrix<T> fx(nf,n);

for(sa_size_t i=0; i<nf; i++)
  for(sa_size_t j=0; j<n; j++) fx(i,j) = f(i,x(j));

return LinFit(fx,y,c);
}

// fit lineaire des y(k) en tant que somme de c(i)f(i,x(k)), i=0..nf-1,
// la matrice fx contient les valeurs des f: fx(i,j) = f(i, x(j)).
//! Linear fitting
/*!
  Linear fit of y(k) as the sum of \f$ c(i)f(i,x(k)), i=0..nf-1 \f$.
  \param fx : matrix which contains \f$ fx(i,j) = f(i, x(j)) \f$.
  \param y : vector of datas
  \return c : vector of solutions
  \return return chisquare
 */
template <class T>
r_8 LinFitter<T>::LinFit(const TMatrix<T>& fx, const TVector<T>& y, TVector<T>& c)
{
sa_size_t n = y.NElts();
if (n != fx.NCol()) 
  throw SzMismatchError("LinFitter<T>::LinFit(fx, y, c)/Error y.NElts() <> fx.Nelts() ");

sa_size_t nf = fx.NRows();

TMatrix<T> a(nf,nf);

for(sa_size_t j=0; j<nf; j++)
  for(sa_size_t k=j; k<nf; k++)
     a(j,k) = a(k,j) = TMatrixRC<T>::Row(const_cast<TMatrix<T> &>(fx), j) 
	             * TMatrixRC<T>::Row(const_cast<TMatrix<T> &>(fx), k);

c = fx * y;

if(SimpleMatrixOperation<T>::GausPiv(a,c)==(T) 0) 
  throw SingMatrixExc("LinFitter<T>::LinFit(fx, y, c) - Non invertible matrix (by GausPiv())");

r_8 xi2 = 0., ax;
T x;
for(sa_size_t k=0; k<n; k++) {
  x = (T) 0;
  for(sa_size_t i=0; i<nf; i++) x += c(i) * fx(i,k);
  x -= y(k);
  ax = TMatrixRC<T>::Abs_Value(x);
  xi2 += ax*ax;
}
return xi2;
}

// fit lineaire des y(k) en tant que somme de c(i)f(i,x(k)), i=0..nf-1,
// errY2 contient les carres des erreurs sur les Y.
// au retour, errC contient les erreurs sur les coefs.
//! Linear fitting with errors
/*!
  Linear fit with errors of y(k) as the sum of \f$ c(i)f(i,x(k)), i=0..nf-1 \f$.
  \param x : vector of X values
  \param y : vector of datas
  \param errY2 : vector of errors square on Y
  \param nf: number of functions
  \param f : f(i,x(k)), i=0..nf-1
  \return c : vector of solutions
  \return errC : vector of errors on solutions C
  \return return chisquare
 */
template <class T>
r_8 LinFitter<T>::LinFit(const TVector<T>& x, const TVector<T>& y,
             const TVector<T>& errY2, sa_size_t nf, T (*f)(sa_size_t,T),
                                  TVector<T>& c, TVector<T>& errC)
{
sa_size_t n = x.NElts();
if (n != y.NElts()) 
  throw SzMismatchError("LinFitter<T>::LinFit(x,y,errY2,nf,f,c,errC)/Error x.NElts() <> y.Nelts() ");

TMatrix<T> fx(nf, n);
for(sa_size_t i=0; i<nf; i++)
  for(sa_size_t j=0; j<n; j++)
    fx(i,j) = f(i,x(j));

return LinFit(fx,y,errY2,c,errC);
}

// fit lineaire des y(k) en tant que somme de c(i)f(i,x(k)), i=0..nf-1,
// la matrice fx contient les valeurs des f:
// fx(i,j) = f(i, x(j)).
// errY2 contient les carres des erreurs sur les Y.
// au retour, errC contient les erreurs sur les coefs.
//! Linear fitting with errors
/*!
  \param fx : matrix which contains \f$ fx(i,j) = f(i, x(j)) \f$.
  \param y : vector of datas
  \param errY2 : vector of errors square on Y
  \return c : vector of solutions
  \return errC : vector of errors on solutions on C
  \return return chisquare
 */
template <class T>
r_8 LinFitter<T>::LinFit(const TMatrix<T>& fx, const TVector<T>& y,
           const TVector<T>& errY2,TVector<T>& c, TVector<T>& errC)
{
sa_size_t n = y.NElts();
if( n != errY2.NElts()) 
  throw SzMismatchError("LinFitter<T>::LinFit(fx,y,errY2,c,errC)/Error y.NElts() <> errY2.Nelts() ");
if( n != fx.NCol()) 
  throw SzMismatchError("LinFitter<T>::LinFit(fx,y,errY2,c,errC)/Error y.NElts() <> fx.NCols() ");
sa_size_t nf = fx.NRows();

TMatrix<T> a(nf,nf);

c.Realloc(nf);
errC.Realloc(nf);

for(sa_size_t j=0; j<nf; j++)
  for(sa_size_t k=j; k<nf; k++) {
    T x=0;
    // Matrice a inverser
    for(sa_size_t l=0; l<n; l++) x += fx(j,l)*fx(k,l)/errY2(l);
    a(j,k) = a(k,j) = x;
  }
 
TMatrix<T> d(nf,nf+1);
for(sa_size_t k=0; k<nf; k++) {
  T x = (T) 0;
  // Second membre 1ere colonne
  for(sa_size_t l=0; l<n; l++) x += fx(k,l)*y(l)/errY2(l);             
  d(k,0) = x;
  // Reste second membre = Id.
  for(sa_size_t m=1; m<=nf; m++) d(k,m) = (k==m)?1:0;
}

if(SimpleMatrixOperation<T>::GausPiv(a,d)==(T) 0) 
  throw SingMatrixExc("LinFitter<T>::LinFit(...ErrY2...) - Non invertible matrix (by GausPiv())");


for(sa_size_t l=0; l<nf; l++) {
  c(l) = d(l,0);        // Parametres = 1ere colonne
  errC(l) = d(l,l+1);   // Erreurs = diag inverse.
}

r_8 xi2 = 0., ax;
T x;
for(sa_size_t jj=0; jj<n; jj++) {
  x = (T) 0;
  for(sa_size_t ii=0; ii<nf; ii++) x += c(ii) * fx(ii,jj);
    x -= y(jj);
    ax = TMatrixRC<T>::Abs_Value(x);
    xi2 += ax*ax/TMatrixRC<T>::Abs_Value(errY2(jj));
  }
  return xi2;
}

} // Fin namespace SOPHYA 
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void _ZZ_TestTMatrixRC_YY_(TMatrix<r_8> & m)
{
  cout << " ::::: _ZZ_TestTMatrixRC_YY_ :::: M= \n" << m << endl;
  TMatrixRC<r_8> l0 = TMatrixRC<r_8>::Row(m,0);
  cout << "TMatrixRC<r_8>::Row(m,0) = \n" ;
  l0.Print(cout);
  TMatrixRC<r_8> l1 = TMatrixRC<r_8>::Row(m,1);
  cout << "TMatrixRC<r_8>::Row(m,1) = \n" ;
  l1.Print(cout);
  l0.SetRow(2);
  cout << "TMatrixRC<r_8>::l0.SetRow(2 = \n" ;
  l0.Print(cout);

  TMatrixRC<r_8> c0 = TMatrixRC<r_8>::Col(m,0);
  cout << "TMatrixRC<r_8>::Col(m,0) = \n" ;
  c0.Print(cout);
  TMatrixRC<r_8> c1 = TMatrixRC<r_8>::Col(m,1);
  cout << "TMatrixRC<r_8>::Col(m,1) = \n" ;
  c1.Print(cout);  
  c0.SetCol(2);
  cout << "TMatrixRC<r_8>::c0.SetCol(2) = \n" ;
  c0.Print(cout);
  TMatrixRC<r_8> c00 = TMatrixRC<r_8>::Col(m,0);
  TMatrixRC<r_8>::Swap(c0, c00);
  cout << " :::::  M Apres Swap = \n" << m << endl;
  
}

///////////////////////////////////////////////////////////////
#ifdef __CXX_PRAGMA_TEMPLATES__
// Instances gestion lignes/colonnes
#pragma define_template TMatrixRC<int_4>
#pragma define_template TMatrixRC<r_4>
#pragma define_template TMatrixRC<r_8>
#pragma define_template TMatrixRC< complex<r_4> >
#pragma define_template TMatrixRC< complex<r_8> >
#pragma define_template SimpleMatrixOperation<int_4>
#pragma define_template SimpleMatrixOperation<r_4>
#pragma define_template SimpleMatrixOperation<r_8>
#pragma define_template SimpleMatrixOperation< complex<r_4> >
#pragma define_template SimpleMatrixOperation< complex<r_8> >
#pragma define_template LinFitter<r_4>
#pragma define_template LinFitter<r_8>
#pragma define_template LinFitter< complex<r_4> >
#pragma define_template LinFitter< complex<r_8> >
#ifdef SO_LDBLE128
#pragma define_template TMatrixRC<r_16>
#pragma define_template TMatrixRC< complex<r_16> >
#pragma define_template SimpleMatrixOperation<r_16>
#pragma define_template SimpleMatrixOperation< complex<r_16> >
#pragma define_template LinFitter<r_16>
#pragma define_template LinFitter< complex<r_16> >
#endif
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
namespace SOPHYA {
// Instances gestion lignes/colonnes
template class TMatrixRC<int_4>;
template class TMatrixRC<r_4>;
template class TMatrixRC<r_8>;
template class TMatrixRC< complex<r_4> >;
template class TMatrixRC< complex<r_8> >;
template class SimpleMatrixOperation<int_4>;
template class SimpleMatrixOperation<r_4>;
template class SimpleMatrixOperation<r_8>;
template class SimpleMatrixOperation< complex<r_4> >;
template class SimpleMatrixOperation< complex<r_8> >;
template class LinFitter<r_4>;
template class LinFitter<r_8>;
template class LinFitter< complex<r_4> >;
template class LinFitter< complex<r_8> >;
#ifdef SO_LDBLE128
template class TMatrixRC<r_16>;
template class TMatrixRC< complex<r_16> >;
template class SimpleMatrixOperation<r_16>;
template class SimpleMatrixOperation< complex<r_16> >;
template class LinFitter<r_16>;
template class LinFitter< complex<r_16> >;
#endif
}
#endif

