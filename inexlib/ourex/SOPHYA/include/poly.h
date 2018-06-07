// This may look like C code, but it is really -*- C++ -*-
//
// $Id: poly.h,v 1.12 2012/11/19 09:39:43 ansari Exp $
//

// Des polynomes avec des operations de bases et surtout des fits.

#ifndef POLY_SEEN
#define POLY_SEEN

#include "objfio.h"
#include <stdio.h>
#include "peida.h"
#include "tvector.h"
#include "ppersist.h"
#include "anydataobj.h"

namespace SOPHYA {

class Poly2;

//////////////////////////////////////////////////////////////////////////
//! Class of 1 dimension polynomials P(x)
class Poly : public TVector<r_8> {
  friend class ObjFileIO<Poly>;
public:
  Poly(int degre = 0);
  Poly(Poly const& a);

  //! Return degre of polynomials
  inline int Degre() const {UpdateDegIfDirty(); return deg;}

  //! Return space for a \b n degre polynomial
  /*!
    \param \b n : degre of new polynomial
    \param \b force : force re-allocation if true,\
                 if not allocation occurs only
                 if space needed is greater than the old one
  */
  inline void Realloc(int n, bool force=false)
                     {TVector<r_8>::Realloc(n+1,force);}

  //  Pour compatibilite PEIDA  - Reza 03/2000
  //! Return coefficient of degre \b i
  inline double Element(int i) const { return Elem(i,0,0,0,0); } 
  //! Return coefficient of degre \b i
  inline double & Element(int i)  { return Elem(i,0,0,0,0); } 

  //! Return coefficient of degre \b i
  inline double operator[](int i) const {return Elem(i,0,0,0,0);}
  //! Return coefficient of degre \b i
  inline double& operator[](int i) {dirty = 1; return Elem(i,0,0,0,0);}
  // Retourne le coefficient de degre i

  double operator()(double x) const;
  // Retourne la valeur prise en x.

  void   Derivate();
  // Derive le polynome en place

  void   Derivate(Poly& der) const;
  // Derive le polynome dans un autre

  int    Roots(TVector<r_8>& roots) const;
  // retourne les racines si on peut les calculer...

  int    Root1(double& r) const;
  // special degre 1

  int    Root2(double& r1, double& r2) const;
  // special degre 2

  Poly& operator = (Poly const& b);
  Poly& operator += (Poly const& b);
  Poly& operator -= (Poly const& b);

  Poly& operator *= (double a);

  Poly Mult(Poly const& b) const;

  Poly power(int n) const;
  Poly operator() (Poly const& b) const;
  Poly2 operator() (Poly2 const& b) const;

  ostream& Print(ostream& s, sa_size_t maxprt=-1, 
	         bool si=false, bool ascd=false) const ;

  double Fit(TVector<r_8> const& x, TVector<r_8> const& y, int degre);
  // Fit d'un polynome de degre donne sur les x et y.

  double Fit(TVector<r_8> const& x, TVector<r_8> const& y,
         TVector<r_8> const& erry2, int degre, TVector<r_8>& errCoef);
  // En plus, on fournit les carres des erreurs sur y et on a les erreurs
  // sur les coefficients dans un vecteur.

private:
  int dirty;
  int_4 deg;
  void UpdateDeg() const;
  void UpdateDegIfDirty() const {if (dirty) UpdateDeg();}
};

/*! \ingroup NTools \fn operator*(Poly const&,Poly const&)
  \brief perform and return P(X) = a(x) * b(x) */
inline Poly operator* (Poly const& a, Poly const& b)
  { return a.Mult(b); }

/*! \ingroup NTools \fn operator+(Poly const&,Poly const&)
  \brief perform and return P(X) = a(x) + b(x) */
inline Poly operator+ (Poly const& a, Poly const& b)
 { Poly c((a.Degre() > b.Degre())?(a.Degre()+1):(b.Degre()+1));
   c = a; c += b; return c; }

/*! \ingroup NTools \fn operator-(Poly const&,Poly const&)
  \brief perform and return P(X) = a(x) - b(x) */
inline Poly operator- (Poly const& a, Poly const& b)
  { Poly c((a.Degre() > b.Degre())?(a.Degre()+1):(b.Degre()+1));
    c = a; c -= b; return c; }

/*! \ingroup NTools \fn operator*(double,Poly const&)
  \brief perform and return P(X) = a * b(x) */
inline Poly operator* (double a, Poly const& b)
  { Poly c(b); c *= a; return c; }

/*! \ingroup NTools \fn operator<<(ostream&,const Poly&)
  \brief Print a(x) on stream \b s */
inline ostream& operator << (ostream& s, const Poly& a)
  { return a.Print(s); }

//////////////////////////////////////////////////////////////////////////
/*! \ingroup NTools \fn operator<<(POutPersist&,Poly&)
  \brief For persistance management */
inline POutPersist& operator << (POutPersist& os, Poly & obj)
  { ObjFileIO<Poly> fio(&obj);  fio.Write(os);  return(os); }
/*! \ingroup NTools \fn operator<<(PInPersist&,,Poly&)
  \brief For persistance management */
inline PInPersist& operator >> (PInPersist& is, Poly & obj)
  { ObjFileIO<Poly> fio(&obj);  fio.Read(is);  return(is); }
// Classe pour la gestion de persistance
// ObjFileIO<Poly>

//////////////////////////////////////////////////////////////////////////
int binomial(int n, int p);

//////////////////////////////////////////////////////////////////////////
//! Class of 2 dimensions polynomials P(x,y)
class Poly2 : public TVector<r_8> {
  friend class ObjFileIO<Poly2>;
public:
  Poly2(int degreX=0, int degreY=0);
  // degres alloues.
  Poly2(Poly const& polX, Poly const& polY);
  Poly2(Poly2 const& a);

  //! Return polynomial partial degre for X
  inline int DegX() const {UpdateDegIfDirty(); return degX;}
  //! Return polynomial partial degre for Y
  inline int DegY() const {UpdateDegIfDirty(); return degY;}
  //! Return polynomial maximum partial degre for X
  inline int MaxDegX() const {return maxDegX;}
  //! Return polynomial maximum partial degre for Y
  inline int MaxDegY() const {return maxDegY;}
  //! Return polynomial total degre for X
  inline int Deg()  const {UpdateDegIfDirty(); return deg;}
  // les degres partiels en x et y, et totaux.

  //  Pour compatibilite PEIDA  - Reza 03/2000
  //! Return coefficient of degre \b i
  inline double Element(int i) const { return Elem(i,0,0,0,0); } 
  //! Return coefficient of degre \b i
  inline double & Element(int i)  { return Elem(i,0,0,0,0); } 

  double operator()(double x, double y) const;
  // retourne la valeur en (x,y)

  //! Return index of coefficient of X^dx * Y^dy in the vector
  inline int IndCoef(int dx, int dy) const {
    if (dx>maxDegX || dy>maxDegY) throw RangeCheckError("Poly2::IndCoef()");
    return dx + (maxDegX+1)*dy;
  }
  // l'indice du coefficient dans le vecteur. Public uniquement parce
  // que ca sert a recuperer les erreurs sur les coefficients lors
  // d'un fit...

  //! Return coefficient of X^dx * Y^dy
  inline double Coef(int dx, int dy) const {
    return (dx>maxDegX || dy>maxDegY) ? 0 : Element(IndCoef(dx,dy));
  }
  //! Return coefficient of X^dx * Y^dy
  inline double& Coef(int dx, int dy) {
    if (dx>maxDegX || dy>maxDegY) throw RangeCheckError("Poly2::Coef() ");
    dirty = 1; return Element(IndCoef(dx,dy));
  }
  // retourne le coefficient de degre (dx,dy)

  double Fit(TVector<r_8> const& x, TVector<r_8> const& y, TVector<r_8> const& z,
             int degreX, int degreY);
  double Fit(TVector<r_8> const& x, TVector<r_8> const& y, TVector<r_8> const& z,
             TVector<r_8> const& errz2, int degreX, int degreY,
             TVector<r_8>& errCoef);
  // degres partiels imposes. cf Poly::Fit sinon


  double Fit(TVector<r_8> const& x, TVector<r_8> const& y, TVector<r_8> const& z,
             int degre);
  double Fit(TVector<r_8> const& x, TVector<r_8> const& y, TVector<r_8> const& z,
             TVector<r_8> const& errz2, int degre,
             TVector<r_8>& errCoef);
  // degre total impose. cf Poly::Fit sinon

  Poly2& operator = (Poly2 const& b);

  Poly2& operator += (Poly2 const& b);
  Poly2& operator -= (Poly2 const& b);

  Poly2& operator *= (double a);

  Poly2 Mult(Poly2 const& b) const;

  Poly2 power(int n) const;

  Poly2 operator() (Poly const& px, Poly const& py) const;
//  Poly2 operator() (Poly2 const& px, Poly2 const& py) const;

  void Realloc(int degreX, int degreY);

  ostream& Print(ostream& s, sa_size_t maxprt=-1, 
	         bool si=false, bool ascd=false) const ;

private:
  int dirty;
  int_4 maxDegX;
  int_4 maxDegY;
  int degX;
  int degY;
  int deg;
  void UpdateDeg() const;
  void UpdateDegIfDirty() const {if (dirty) UpdateDeg();}
};

/*! \ingroup NTools \fn operator*(Poly2 const&,Poly2 const&)
  \brief Perform P(x,y) = a(x,y) * b(x,y) */
inline Poly2 operator* (Poly2 const& a, Poly2 const& b)
  { return a.Mult(b); }

/*! \ingroup NTools \fn operator+(Poly2 const&,Poly2 const&)
  \brief Perform P(x,y) = a(x,y) + b(x,y) */
inline Poly2 operator+ (Poly2 const& a, Poly2 const& b)
  { Poly2 c(a); c += b; return c; }

/*! \ingroup NTools \fn operator-(Poly2 const&,Poly2 const&)
  \brief Perform P(x,y) = a(x,y) - b(x,y) */
inline Poly2 operator- (Poly2 const& a, Poly2 const& b)
  { Poly2 c(a); c -= b; return c; }

/*! \ingroup NTools \fn operator-(double,Poly2 const&)
  \brief Perform P(x,y) = a * b(x,y) */
inline Poly2 operator * (double a, Poly2 const& b)
  { Poly2 c(b); c *= a; return c; }

/*! \ingroup NTools \fn operator<<(ostream&,const Poly2&)
  \brief Print a(x,y) on stream \b s */
inline ostream& operator << (ostream& s, const Poly2& a)
  { return a.Print(s); }

//////////////////////////////////////////////////////////////////////////
/*! \ingroup NTools \fn operator<<(POutPersist&,Poly2&)
  \brief For persistance management */
inline POutPersist& operator << (POutPersist& os, Poly2 & obj)
  { ObjFileIO<Poly2> fio(&obj);  fio.Write(os);  return(os); }
/*! \ingroup NTools \fn operator<<(POutPersist&,Poly2&)
  \brief For persistance management */
inline PInPersist& operator >> (PInPersist& is, Poly2 & obj)
  { ObjFileIO<Poly2> fio(&obj);  fio.Read(is);  return(is); }
// Classe pour la gestion de persistance
// ObjFileIO<Poly2>


} // Fin du namespace

#endif // POLY_SEEN
