#ifndef SPHEREECP_SEEN
#define SPHEREECP_SEEN

#include "sphericalmap.h"
#include "ndatablock.h"
#include "tvector.h"


/* Classe de carte spherique avec decoupage theta-phi 
  (projection cylindique: Equatorial Cylindrical Projection) 
  avec possibilite de couverture partielle, limitees en theta,phi
  R. Ansari  - Septembre 2004 
 */

namespace SOPHYA {
template <class T>
class FIO_SphereECP;  

template <class T> 
class SphereECP : public SphericalMap<T>
{
public :

//! return the size index value corresponding to resolution res (in radian)
static inline int_4 ResolToSizeIndex(double res)
  { return (int_4)((M_PI/res)+0.5); }
//! return the pixel resolution (in radian) for the size index \b m 
static inline double  SizeIndexToResol(int_4 m) 
  { return  (M_PI/(double)m); }

  // Constructeur par defaut
			SphereECP();
  // Constructeur , couverture complete, m tranches en theta (delta theta = M_PI), 2m en phi (delta phi = 2Pi)
			SphereECP(int m);
  // Constructeur , couverture complete, ntet tranches en theta (delta theta = M_PI), nphi en phi (delta phi = 2Pi)
			SphereECP(int ntet, int nphi);
  // Constructeur , couverture partielle, ntet tranches en theta , nphi en phi 
			SphereECP(r_8 tet1, r_8 tet2, int ntet, r_8 phi1, r_8 phi2, int nphi);
  // Constructeur de copie, share=true -> partage des pixels
			SphereECP(const SphereECP<T>& a, bool share);
  // Constructeur de copie, partage des pixels
			SphereECP(const SphereECP<T>& a);
  virtual		~SphereECP();

  // Extraction de carte partielle d'une carte ECP complete
  virtual SphereECP<T> ExtractPartial(r_8 tet1, r_8 tet2, r_8 phi1, r_8 phi2);


// Renvoie Type ECP 
  virtual string	TypeOfMap() const;  
  virtual void		SetTemp(bool temp=false) const;  // A supprimer 

// Informations sur carte partielle/complete + zone de couverture
  inline  bool		IsPartial() const { return _partial; }
  inline  double        MinTheta() const { return _theta1; }
  inline  double        MaxTheta() const { return _theta2; }
  inline  double        DeltaTheta() const { return _dtheta; }
  inline  double        MinPhi() const { return _phi1; }
  inline  double        MaxPhi() const { return _phi2; }
  inline  double        DeltaPhi() const { return _dphi; }

// Interface de Pixelmap
/*!    Return total number of pixels  */
  virtual int_4		NbPixels() const;

/* retourne la valeur du pixel d'indice k */ 
/*!    Return value of pixel with index k */
  virtual T&		PixVal(int_4 k);
  virtual T const&	PixVal(int_4 k) const;

/* Return true if teta,phi in map  */
  virtual bool		ContainsSph(double theta, double phi) const;
/* retourne l'indice du pixel a (theta,phi) */ 
/*    Return index of the pixel corresponding to direction (theta, phi). */
  virtual int_4		PixIndexSph(double theta, double phi) const;

/* retourne les coordonnees Spheriques du centre du pixel d'indice k */ 
/*!   Return (theta,phi) coordinates of middle of  pixel with  index k */
  virtual void		PixThetaPhi(int_4 k, double& theta, double& phi) const;

/*! Setting pixel values to a constant */
  virtual T		SetPixels(T v);

/* retourne/fixe l'angle Solide de Pixel   (steradians) */ 
/*!   Pixel Solid angle  (steradians)

    All the pixels have the same solid angle. The dummy argument is
   for compatibility with eventual pixelizations which would not 
    fulfil this requirement.
*/
  virtual double	PixSolAngle(int_4 dummy=0) const;

/* retourne/fixe l'angle Solide couvert par la carte   (steradians) */ 
  virtual double TotSolAngle(void) const
    {return fabs((_phi2-_phi1)*(cos(_theta2)-cos(_theta1)));}

/* retourne/fixe la valeur du parametre de decoupage m */ 
  virtual int_4		SizeIndex() const ;

// Interface de SphericalMap 
// index characterizing the size pixelization : m for SphereThetaPhi
// nside for Gorski sphere...
  virtual void		Resize(int_4 m);
  virtual uint_4	NbThetaSlices() const;

  // Nb de tranches en Phi
  inline  uint_4	NbPhiSlices() const { return _pixels.SizeX(); }

  virtual r_8           ThetaOfSlice(int_4 index) const;
  virtual void		GetThetaSlice(int_4 index,r_8& theta, 
				      TVector<r_8>& phi, TVector<T>& value) const ; 
  virtual void		GetThetaSlice(int_4 sliceIndex, r_8& theta, r_8& phi0, 
				      TVector<int_4>& pixelIndices,TVector<T>& value) const ;
  virtual T*            GetThetaSliceDataPtr(int_4 index);

  // Valeur de pixel hors carte
  inline void		SetOutOfMapValue(T v) { _outofmappix = _outofmapval = v; }
  inline T		GetOutOfMapValue(T v) { return _outofmapval; }

  // Impression 
  virtual void          Show(ostream& os) const;
  inline void		Show() const { Show(cout); }
  virtual void          Print(ostream& os) const;
  inline void		print(ostream& os) const { Print(os); }
  inline void		Print() const { Print(cout); }

  // ---- les operations =, +, - , *
  // operation = (remplissage avec une valeur ou carte)
  virtual SphereECP<T>& Set(const SphereECP<T>& a);
  inline  SphereECP<T>&	operator = (const SphereECP<T>& a) { return Set(a); }
  virtual SphereECP<T>& SetCst(T x);
  inline  SphereECP<T>&	operator = (T x)   { return SetCst(x); }
  // Operation + , * avec des Constantes 
  virtual SphereECP<T>&	AddCst(T x) ;
  virtual SphereECP<T>&	MulCst(T x) ;

  inline  SphereECP<T>&  operator += (T x)            { return AddCst(x); }
  inline  SphereECP<T>&  operator -= (T x)            { return AddCst(-x); }
  inline  SphereECP<T>&  operator *= (T x)            { return MulCst(x); }

  // Acces au tableau des pixels
  inline TArray<T>&	GetPixelArray() { return _pixels; }
  inline TArray<T>	GetPixelArray() const { return _pixels; }

  //--- Changement et acces a l'unite physique associee
  virtual void SetUnits(const Units& un)    { _pixels.SetUnits(un); } 
  virtual Units GetUnits(bool& sdone) const  { return _pixels.GetUnits(sdone); } 

  //! assign a new object Id (or DataRef Id) - useful for PPF write operations
  inline void RenewObjId() { _pixels.RenewObjId(); }

  friend class  FIO_SphereECP<T>;   // Gestion de persistance PPF

protected:
  bool _partial;
  r_8 _theta1,_theta2;
  r_8 _phi1, _phi2;
  r_8 _dtheta, _dphi;
  TArray<T> _pixels;
  int_4 _outofmapidx;
  int_4 _outofmapnphi;
  int_4 _outofmapntet;
  T _outofmappix;
  T _outofmapval;
};

//--------- extern template declarations (if needed) -----------
#if defined ( NEED_EXT_DECL_TEMP ) && !defined( SPHEREECP_CC_BFILE )
extern template class SphereECP<int_4>;
extern template class SphereECP<r_4>;
extern template class SphereECP<r_8>;
extern template class SphereECP< complex<r_4> >;
extern template class SphereECP< complex<r_8> >;
#endif  // Fin de if defined ( NEED_EXT_DECL_TEMP )


}// Fin du namespace

#endif
