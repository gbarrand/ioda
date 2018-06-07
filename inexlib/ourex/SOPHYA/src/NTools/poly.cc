#include "machdefs.h"
#include "poly.h"
#include "linfit.h"
#include "fioarr.h"

namespace SOPHYA {

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/*!
  \class Poly
  \ingroup NTools
  One dimensional polynomials class.
  \warning status EXPERIMENTAL - not fully tested.
*/

//! Constructor
/*! Create a 1D polynomial of degre \b degre */
Poly::Poly(int degre)
: TVector<r_8>(degre+1), dirty(0), deg(0)
{
}

//! Constructor by copy
Poly::Poly(Poly const& a)
:TVector<r_8>(a), dirty(a.dirty), deg(a.deg)
{
}

//! update degre
/*! update degre (that could be changed after operations) */
void Poly::UpdateDeg() const
{
  int i = NElts()-1;
  while (Element(i) == 0 && i>0) i--;

  (int&) deg = i;          // bientot mutable dans ANSI C++ 
  (int&) dirty = 0;
}

//! compute value P(\b x)
double Poly::operator()(double x) const
{
  UpdateDegIfDirty();
  double res = Element(deg);
  for (int i=deg-1; i>=0; i--) {
    res *= x;
    res += Element(i);
  }
  return res;
}

//! Replace p(x) by its derivate
void Poly::Derivate()
{
  UpdateDegIfDirty();
  if (deg == 0) { Element(0) = 0; return;}
  for (int i=1; i<=deg; i++)
    Element(i-1) = Element(i)*i;
  Element(deg) = 0;
  deg--;
}


//! Return the derivate in \b der(x)
void Poly::Derivate(Poly& der) const
{
  UpdateDegIfDirty();
  der.Realloc(deg);
//  der.Zero();    // on sait que Realloc met a zero le reste.
  for (int i=1; i<=deg; i++)
    der.Element(i-1) = Element(i)*i;
}


//! Return the roots of the polynomial into \b roots
/*!
  This works until degre 2
  \return the number of roots
*/
int Poly::Roots(TVector<r_8>& roots) const
{
  UpdateDegIfDirty();

  switch (deg)
    {
    case 0 :           // degre 0
      return 0;
    case 1 :           // degre 1
      roots.Realloc(1);
      return Root1(roots(0));
    case 2 :           // degre 2
      roots.Realloc(2);
      return Root2(roots(0),roots(1));
    default :
      throw ParmError("Poly::Roots()") ;
    }
}


//! Return root \b r for a degre 1 polynomial
/*! \return return 1 if succes, 0 if not */
int Poly::Root1(double& r) const
{
  UpdateDegIfDirty();
  if (deg != 1) ParmError("Poly::Root1() deg!= 1") ;

  if (Element(1) == 0) return 0;
  r = -Element(0)/Element(1);
  return 1;
}

//! Return roots \b r1 and \b r2 for a degre 2 polynomial
/*! \return return the number of roots found */
int Poly::Root2(double& r1, double& r2) const
{
  UpdateDegIfDirty();
  if (deg != 2) throw SzMismatchError("Poly::Root2() deg != 2") ;

  double delta = Element(1)*Element(1) - 4*Element(0)*Element(2);
  if (delta < 0) return 0;
  if (delta == 0) {
    r1 = r2 = -Element(1)/2/Element(0);
    return 1;
  }
  r1 = (-Element(1) - sqrt(delta))/2/Element(0);
  r2 = (-Element(1) + sqrt(delta))/2/Element(0);
  return 2;
}

//! Operator P(x) = a(x)
Poly& Poly::operator = (Poly const& a)
{
  if (this == &a) return *this;
  TVector<r_8>::operator=(a);

  UpdateDeg();
  return *this;
}

//! Perform P(x) += b(x)
Poly& Poly::operator += (Poly const& b)
{
  UpdateDegIfDirty();
  b.UpdateDegIfDirty();

  if (b.deg > deg) Realloc(b.deg);

  int n = (deg > b.deg) ? deg : b.deg;
  for (int i=0; i<=n; i++) Element(i) += b.Element(i);

  UpdateDeg();
  return *this;
}

//! Perform P(x) -= b(x)
Poly& Poly::operator -= (Poly const& b)
{
  UpdateDegIfDirty();
  b.UpdateDegIfDirty();

  if (b.deg > deg) Realloc(b.deg);

  int n = (deg > b.deg) ? deg : b.deg;
  for (int i=0; i<=n; i++) Element(i) -= b.Element(i);

  UpdateDeg();
  return *this;
}

//! Perform P(x) *= b(x)
Poly& Poly::operator *= (double a)
{
  UpdateDegIfDirty();
  for (int i=0; i<=deg; i++) Element(i) *= a;
  return *this;
}

//! Return P(x) = *this(x) * b(x)
Poly Poly::Mult(Poly const& b) const
{
Poly c(deg + b.deg);
  UpdateDegIfDirty();
  b.UpdateDegIfDirty();

  c.deg = deg + b.deg;

  for (int i=0; i<=c.deg; i++) {
    c[i] = 0;
    int kmin = (i <= deg) ? 0 : i - deg;
    int kmax = (i <= deg) ? i : deg;
    for (int k=kmin; k<=kmax; k++)
      c[i] += (*this)[k] * b[i-k];
  }
return c;
}

//! Print on stream \b s
ostream& Poly::Print(ostream& s, sa_size_t , bool, bool ) const
{
  UpdateDegIfDirty();
  int nz=0;
  for (int i = deg; i>=0; i--) {
    if ((*this)[i] != 0) {
      nz = 1;
      if (i < deg && (*this)[i] > 0) s << "+";
      s << (*this)[i];
      if (i == 1) s << "*X ";
      if (i > 1) s << "*X^" << i << " ";
    }
  }
  if (!nz) s << " 0 ";
  return s;
}

//! Fit datas by a polynomial
/*!
  Fit y(x) by a polynimial P(x)
  \param x : x datas
  \param y : y datas
  \param degre : degre of the polynomial P(x) to be fitted
  \warning result is stored in the current object
  \return return chisquare
*/
double Poly::Fit(TVector<r_8> const& x, TVector<r_8> const& y, int degre)
{
  int n = x.NElts();
  if (n != (int)y.NElts()) throw SzMismatchError("Poly::Fit() ");

  Realloc(degre);

  TMatrix<r_8> a(degre+1, n);

  for (int c=0; c<n; c++) {
    double xpow = 1.0;
    for (int l=0; l<=degre; l++) {
      a(l,c) = xpow;
      xpow *= x(c);
    }
  }

  LinFitter<r_8> lf;
  double rc = lf.LinFit(a,y,(TVector<r_8>&)*this);
  UpdateDeg();
  return rc;
}

//! Fit datas with errors by a polynomial
/*!
  Fit y(x) by a polynimial P(x)
  \param x : x datas
  \param y : y datas
  \param erry2 : errors squared on y
  \param degre : degre of the polynomial P(x) to be fitted
  \warning result is stored in the current object
  \return \b errcoeff : errors on the coefficients
  \return return chisquare
*/
double Poly::Fit(TVector<r_8> const& x, TVector<r_8> const& y,
       TVector<r_8> const& erry2, int degre,TVector<r_8>& errCoef)
{
  int n = x.NElts();
  if (n != (int)y.NElts()) throw SzMismatchError("Poly::Fit() ");
  if (n != (int)erry2.NElts()) throw SzMismatchError("Poly::Fit() ") ;

  Realloc(degre);
  errCoef.Realloc(degre+1);

  TMatrix<r_8> a(degre+1, n);

  for (int c=0; c<n; c++) {
    double xpow = 1.0;
    for (int l=0; l<=degre; l++) {
      a(l,c) = xpow;
      xpow *= x(c);
    }
  }

  LinFitter<r_8> lf;
  double rc = lf.LinFit(a,y,erry2,(TVector<r_8>&)*this,errCoef);
  UpdateDeg();
  return rc;
}


//! Return the polynomial at power \b n : ( \f$ P(x)^n \f$ )
Poly Poly::power(int n) const // a accelerer !!!
{
  if (n < 0) throw RangeCheckError("Poly::power() n<0 ");
  if (n == 0) { Poly r(0); r[0] = 1; return r;}
  if (n == 1) { return *this; }
  return *this * power(n-1);
}

//! Substitue polynomial and return P\f$ (b(x)) \f$
Poly Poly::operator() (Poly const& b) const
{
  Poly c(b.Degre()*Degre());
  for (int i=0; i<= Degre(); i++)
    c += (*this)[i] * b.power(i);
  return c;
}


//////////////////////////////////////////////////////////////////////////
//! For persistance management
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<Poly>::ReadSelf(PInPersist& is)
{
if(dobj==NULL) dobj=new Poly;
int_4 dg;
is >> dg;
dobj->Realloc(dg,true);
is >> *((TVector<r_8> *) dobj);
dobj->UpdateDeg();
}

//! For persistance management
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<Poly>::WriteSelf(POutPersist& os) const
{
if(dobj == NULL) return;
dobj->UpdateDegIfDirty();
dobj->Realloc(dobj->deg,true);
os << dobj->deg;
os << *((TVector<r_8> *) dobj);
}

//////////////////////////////////////////////////////////////////////////
/*! \ingroup NTools
  \fn binomial(int,int)
  Return the binomial coefficient \f$ {C_n}^p \f$.
*/
int binomial(int n, int p)
{
  int c = 1;
  for (int i=n-p+1; i<=n; i++) c *= i;
  for (int j=2; j<=p; j++) c /= j;
  return c;
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/*!
  \class Poly2
  \ingroup NTools
  Two dimensional polynomials class.
  \warning status EXPERIMENTAL - not fully tested.
*/

//! Constructor of 2D polynomial of degres \b degreX \b degreY
Poly2::Poly2(int degreX, int degreY)
:TVector<r_8>((degreX+1)*(degreY+1)), dirty(0),
 maxDegX(degreX), maxDegY(degreY), degX(0), degY(0), deg(0)
{
}

//! Constructor of 2D polynomial \f$ P(x,y) = px(x) * py(y) \f$
Poly2::Poly2(Poly const& polX, Poly const& polY)
:TVector<r_8>((polX.Degre()+1)*(polY.Degre()+1)), dirty(0),
 maxDegX(polX.Degre()), maxDegY(polY.Degre()),
 degX(polX.Degre()), degY(polY.Degre()), deg(degX+degY)
{
  for (int i=0; i<=degX; i++)
    for (int j=0; j<=degY; j++)
      Coef(i,j) = polX[i]*polY[j];
}

//! Constructor by copy
Poly2::Poly2(Poly2 const& a)
:TVector<r_8>(a), dirty(a.dirty),
 maxDegX(a.maxDegX), maxDegY(a.maxDegY),
 degX(a.degX), degY(a.degY), deg(a.deg)
{
}

//! Operator P(x) = a(x)
Poly2& Poly2::operator = (Poly2 const& a)
{
  if (this == &a) return *this;
  if (maxDegX < a.DegX() || maxDegY < a.DegY())
    Realloc(a.DegX(), a.DegY());


  for (int i=0; i<= maxDegX; i++)
    for (int j=0; j<= maxDegY; j++)
      Coef(i,j) = a.Coef(i,j);

  UpdateDeg();
  return *this;
}

//! Re-allocate space for 2D polynomial with partial degres \b degreX \b degreY
void Poly2::Realloc(int degreX, int degreY)
{
  UpdateDegIfDirty();
  Poly2 tmp(*this);
  TVector<r_8>::Realloc((degreX+1)*(degreY+1));
  DataBlock().Reset();
  maxDegX = degreX;
  maxDegY = degreY;

// Attention - Reza 30/09/99
// il faut prendre le min en degre du polynome de depart et le nouveau
  int cdegx = (tmp.degX < degreX) ? tmp.degX : degreX;
  int cdegy = (tmp.degY < degreY) ? tmp.degY : degreY;
  for (int i=0; i<= cdegx; i++)
    for (int j=0; j<= cdegy; j++)
      Coef(i,j) = tmp.Coef(i,j);
}


//! update degres
/*! update degres (that could be changed after operations) */
void Poly2::UpdateDeg() const
{
  (int&)degX=(int&)degY=(int&)deg=0;

  for (int dx=0; dx<=maxDegX; dx++)
    for (int dy=0; dy<=maxDegY; dy++)
      if (Coef(dx,dy) != 0) {
        if (dx > degX) (int&)degX = dx;
        if (dy > degY) (int&)degY = dy;
        if (dx+dy > deg) (int&)deg = dx+dy;
      }

  (int&)dirty = 0;
}

//! Return P(\b x, \b y)
double Poly2::operator()(double x, double y) const
{
  UpdateDegIfDirty();
  double res = 0;
  double xPow = 1;
  for (int dx=0; dx<=maxDegX; dx++) {
    double yPow = 1;
    for (int dy=0; dy<=maxDegY; dy++) {
      res += Coef(dx,dy) * xPow * yPow;
      yPow *= y;
    }
    xPow *= x;
  }
  return res;
}

//! Fit datas by a polynomial
/*!
  Fit z(x,y) by a polynimial P(x,y)
  \param x : x datas
  \param y : y datas
  \param z : z datas
  \param degreX : partial degre on X
  \param degreY : partial degre on Y
  \warning result is stored in the current object
  \return return chisquare
*/
double Poly2::Fit(TVector<r_8> const& x, TVector<r_8> const& y,
                 TVector<r_8> const& z, int degreX, int degreY)
{
  int n = x.NElts();
  if (n != (int)y.NElts()) throw SzMismatchError("Poly2::Fit() - 1");
  if (n != (int)z.NElts()) throw SzMismatchError("Poly2::Fit() - 2");

  Realloc(degreX, degreY);

  TMatrix<r_8> a((degreX+1)*(degreY+1), n);

  for (int c=0; c<n; c++) {
    double xPow = 1.0;
    for (int dx = 0; dx <= degreX; dx++) {
      double yPow = 1.0;
      for (int dy = 0; dy <= degreY; dy++) {
        a(IndCoef(dx,dy), c) = xPow*yPow;
        yPow *= y(c);
      }
      xPow *= x(c);
    }
  }

  LinFitter<r_8> lf;
  double rc = lf.LinFit(a,z,(TVector<r_8>&)*this);
  UpdateDeg();
  return rc;
}

//! Fit datas with errors by a polynomial
/*!
  Fit z(x,y) by a polynimial P(x,y)
  \param x : x datas
  \param y : y datas
  \param z : z datas
  \param errz2 : errors squared on z
  \param degreX : partial degre on X
  \param degreY : partial degre on Y
  \warning result is stored in the current object
  \return \b errcoeff : errors on the coefficients
  \return return chisquare
*/
double Poly2::Fit(TVector<r_8> const& x, TVector<r_8> const& y, TVector<r_8> const& z,
                  TVector<r_8> const& errz2, int degreX, int degreY,
                  TVector<r_8>& errCoef)
{
  int n = x.NElts();
  if (n != (int)y.NElts()) throw SzMismatchError("Poly2::Fit() - 3");
  if (n != (int)z.NElts()) throw SzMismatchError("Poly2::Fit() - 4");
  if (n != (int)errz2.NElts()) throw SzMismatchError("Poly2::Fit() - 5");

  Realloc(degreX, degreY);
  errCoef.Realloc((degreX+1)*(degreY+1));

  TMatrix<r_8> a((degreX+1)*(degreY+1), n);

  for (int c=0; c<n; c++) {
    double xPow = 1.0;
    for (int dx = 0; dx <= degreX; dx++) {
      double yPow = 1.0;
      for (int dy = 0; dy <= degreY; dy++) {
        a(IndCoef(dx,dy), c) = xPow*yPow;
        yPow *= y(c);
      }
      xPow *= x(c);
    }
  }

  LinFitter<r_8> lf;
  double rc = lf.LinFit(a,z,errz2,(TVector<r_8>&)*this,errCoef);
  UpdateDeg();
  return rc;
}

//! Fit datas by a polynomial
/*!
  Fit z(x,y) by a polynimial P(x,y)
  \param x : x datas
  \param y : y datas
  \param z : z datas
  \param degre : total degre
  \warning result is stored in the current object
  \return return chisquare
*/
double Poly2::Fit(TVector<r_8> const& x, TVector<r_8> const& y,
                              TVector<r_8> const& z, int degre)
{
  int n = x.NElts();
  if (n != (int)y.NElts()) throw SzMismatchError("Poly2::Fit() - 6");
  if (n != (int)z.NElts()) throw SzMismatchError("Poly2::Fit() - 7");

  Realloc(degre, degre);   // certains vaudront 0, impose.

  TMatrix<r_8> a((degre+1)*(degre+2)/2, n);
  TVector<r_8> cf((degre+1)*(degre+2)/2);
#define C_INDEX(i,j) ((i) + (j)*(2*degre+3-(j))/2)

  for (int c=0; c<n; c++) {
    double xPow = 1.0;
    for (int dx = 0; dx <= degre; dx++) {
      double yPow = 1.0;
      for (int dy = 0; dy <= degre; dy++) {
        if (dy+dx <= degre)
          a(C_INDEX(dx,dy), c) = xPow*yPow;
        yPow *= y(c);
      }
      xPow *= x(c);
    }
  }

  LinFitter<r_8> lf;
  double rc = lf.LinFit(a,z,cf);

  for (int dx = 0; dx <= degre; dx++)
    for (int dy = 0; dy <= degre; dy++)
      if (dx+dy <= degre)
        Coef(dx,dy) = cf(C_INDEX(dx,dy));
      else
        Coef(dx,dy) = 0;

  UpdateDeg();
  return rc;
}

//! Fit datas with errors by a polynomial
/*!
  Fit z(x,y) by a polynimial P(x,y)
  \param x : x datas
  \param y : y datas
  \param z : z datas
  \param errz2 : errors squared on z
  \param degre : total degre
  \warning result is stored in the current object
  \return \b errcoeff : errors on the coefficients
  \return return chisquare
*/
double Poly2::Fit(TVector<r_8> const& x, TVector<r_8> const& y,
               TVector<r_8> const& z,TVector<r_8> const& errz2,
                              int degre, TVector<r_8>& errCoef)
{
  int n = x.NElts();
  if (n != (int)y.NElts()) throw SzMismatchError("Poly2::Fit() - 8");
  if (n != (int)z.NElts()) throw SzMismatchError("Poly2::Fit() - 9");
  if (n != (int)errz2.NElts()) throw SzMismatchError("Poly2::Fit() - 10");

  Realloc(degre, degre);
  errCoef.Realloc((degre+1)*(degre+1));
#define C_INDEX(i,j) ((i) + (j)*(2*degre+3-(j))/2)

  TMatrix<r_8> a((degre+1)*(degre+2)/2, n);
  TVector<r_8> cf((degre+1)*(degre+2)/2);
  TVector<r_8> ecf((degre+1)*(degre+2)/2);

  for (int c=0; c<n; c++) {
    double xPow = 1.0;
    for (int dx = 0; dx <= degre; dx++) {
      double yPow = 1.0;
      for (int dy = 0; dy <= degre; dy++) {
        if (dy+dx <= degre)
          a(C_INDEX(dx,dy), c) = xPow*yPow;
        yPow *= y(c);
      }
      xPow *= x(c);
    }
  }

  LinFitter<r_8> lf;
  double rc = lf.LinFit(a,z,errz2,cf,ecf);


  for (int dx = 0; dx <= degre; dx++)
    for (int dy = 0; dy <= degre; dy++)
      if (dx+dy <= degre) {
        Coef(dx,dy) = cf(C_INDEX(dx,dy));
        errCoef(IndCoef(dx,dy)) = ecf(C_INDEX(dx,dy));
      } else {
        Coef(dx,dy) = 0;
        errCoef(IndCoef(dx,dy)) = 0;
      }
  UpdateDeg();
  return rc;
}

//! Print on stream \b s
ostream& Poly2::Print(ostream& s, sa_size_t , bool, bool ) const
{
  UpdateDegIfDirty();
  int nz=0;
  int notfirst=0;
  for (int dx = degX; dx>=0; dx--)
    for (int dy= degY; dy>=0; dy--) {
      double c = Coef(dx,dy);
      if (c != 0) {
        nz = 1;
        if (notfirst && c > 0) {
          s << "+";
          notfirst = 1;
        }
        s << c << " ";
        if (dx == 1) s << "* X ";
        if (dx > 1) s << "* X^" << dx << " ";
        if (dy == 1) s << "* Y ";
        if (dy > 1) s << "* Y^" << dy << " ";
        s << endl;
    }
  }
  if (!nz) s << " 0 ";
  return s;
}

//! Operator: return P(x) = *this(x) + b(x)
Poly2& Poly2::operator += (Poly2 const& b)
{
  if (maxDegX < b.DegX() || maxDegY < b.DegY())
    Realloc(b.DegX(),b.DegY());

  UpdateDegIfDirty();

  int mx = b.DegX();
  int my = b.DegY();
  for (int i=0; i<= mx; i++)
    for (int j=0; j<= my; j++)
      Coef(i,j) += b.Coef(i,j);

  UpdateDeg();
  return *this;
}

//! Operator: return P(x) = *this(x) - b(x)
Poly2& Poly2::operator -= (Poly2 const& b)
{
  if (maxDegX < b.DegX() || maxDegY < b.DegY())
    Realloc(b.DegX(),b.DegY());

  UpdateDegIfDirty();

  int mx = b.DegX();
  int my = b.DegY();
  for (int i=0; i<= mx; i++)
    for (int j=0; j<= my; j++)
      Coef(i,j) -= b.Coef(i,j);

  UpdateDeg();
  return *this;
}

//! Operator: return P(x) = *this(x) * a
Poly2& Poly2::operator *= (double a)
{
  for (int_4 i=0; i<NElts(); i++) Element(i) *= a;
  return *this;
}

//! Operator: return P(x) = *this(x) * b(x)
Poly2 Poly2::Mult(Poly2 const& b) const
{
  Poly2 c(DegX() + b.DegX(), DegY() + b.DegY());
  UpdateDegIfDirty();
  b.UpdateDegIfDirty();

  for (int i=0; i<=DegX(); i++)
    for (int j=0; j<=DegY(); j++)
      for (int k=0; k<=b.DegX(); k++)
        for (int l=0; l<=b.DegY(); l++)
          c.Coef(i+k,j+l) += Coef(i,j)*b.Coef(k,l);
  return c;
}

//! Return \f$ P(x,y)^n \f$
Poly2 Poly2::power(int n) const
{
  if (n < 0) throw RangeCheckError("Poly2::power(n<0) ");
  if (n == 0) { Poly2 r(0); r.Coef(0,0) = 1; return r;}
  if (n == 1) { return *this; }
  return *this * power(n-1);
}


//! substitute and return \f$ P(a(x),b(x)) \f$
Poly2 Poly2::operator() (Poly const& a, Poly const& b) const
{
  UpdateDegIfDirty();
  Poly2 c(maxDegX*a.Degre(), maxDegY*b.Degre());

  for (int i=0; i<= degX; i++)
    for (int j=0; j<= degY; j++) {
      Poly2 d(a.power(i), b.power(j));
      c += Coef(i,j) * d;
    }

  return c;
}

//! substitute and return 2D polynomial \f$ P(a(x,y)) \f$, P is a 1D polynomial
Poly2 Poly::operator() (Poly2 const& a) const
{
  Poly2 c(a.MaxDegX()*Degre(), a.MaxDegY()*Degre());

  for (int i=0; i<= Degre(); i++)
    c += (*this)[i] * a.power(i);
  return c;
}

//////////////////////////////////////////////////////////////////////////
//! For persistance management
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<Poly2>::ReadSelf(PInPersist& is)
{
if(dobj==NULL) dobj=new Poly2;
int_4 dgx, dgy;
is >> dgx >> dgy;
dobj->Realloc(dgx,dgy);
is >> *((TVector<r_8> *) dobj);
dobj->UpdateDeg();
}

//! For persistance management
DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void ObjFileIO<Poly2>::WriteSelf(POutPersist& os) const
{
if(dobj == NULL) return;
os << dobj->maxDegX << dobj->maxDegY;
os << *((TVector<r_8> *) dobj);
}


//////////////////////////////////////////////////////////////////////////
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template ObjFileIO<Poly>
#pragma define_template ObjFileIO<Poly2>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class ObjFileIO<Poly>;
template class ObjFileIO<Poly2>;
#endif

} // FIN namespace SOPHYA 
