#include "sopnamsp.h"
#include "smathconst.h"
#include <complex>
#include "fiondblock.h"
#include <iostream>

#define SPHERETHETAPHI_CC_BFILE  // avoid extern template declarations 
#include "spherethetaphi.h"

//----- global print level for spherical maps -----
static int_4  pixel_map_prt_lev_ = 0;
// Set global print level 
void SOPHYA::PixelMap_SetGlobalPrintLevel(int_4 lev)
{ pixel_map_prt_lev_=lev; }
// Get global print level
int_4 SOPHYA::PixelMap_GetGlobalPrintLevel()
{ return pixel_map_prt_lev_; }
//----------------------------------------------------

/*!
  \class SOPHYA::SphereThetaPhi
  \ingroup SkyMap

  \brief Spherical map with equal latitude (iso-theta) rings 


  Class implementing spherical maps, with equal latitude (iso-theta) rings 
  pixelisation scheme - with template data types (double, float, complex, ...)

    sphere splitted with respect to theta, phi : each hemisphere is 
    splitted into (m-1) parallels (equator does not enter into account).
    This operation defines m slices, each of which is splitted into 
    equidistant meridians. This splitting is realized in such a way that 
    all pixels have the same area and are as square as possible.

    One begins with the hemisphere with positive z, starting from the pole
    toward the equator. The first pixel is the polar cap ; it is circular
    and centered on theta=0. 
*/

//***************************************************************
//++ 
// Class	SphereThetaPhi
// 
//
// include      spherethetaphi.h 
//--
//++
//
// Links	Parents
//
//    SphericalMap 
//-- 

/* --Methode-- */
//++
// Titre	Constructors
//--
//++

template <class T>
SphereThetaPhi<T>::SphereThetaPhi()
  : NPhi_(), TNphi_(), Theta_(), pixels_() 

//--
{
  InitNul();
}


/* --Methode-- */

/*!  
  \brief Constructor with specification of number of slices (in a hemisphere)
  \param m is the number of slices in theta on an hemisphere (the polar cap
  forms the first slice).
*/
template <class T>
SphereThetaPhi<T>::SphereThetaPhi(int_4 m)
{
  InitNul();
  Pixelize(m);
}

//! Copy constructor (shares the pixel data if share==true) 
template <class T>
SphereThetaPhi<T>::SphereThetaPhi(const SphereThetaPhi<T>& s, bool share)
  : NPhi_(s.NPhi_, share), TNphi_(s.TNphi_, share), Theta_(s.Theta_, share),
    pixels_(s.pixels_ , share) 
{

  NTheta_= s.NTheta_;
  NPix_  = s.NPix_;
  Omega_ = s.Omega_;
  if(s.mInfo_) this->mInfo_= new DVList(*s.mInfo_);
}

//! Copy constructor (shares the pixel data)
template <class T>
SphereThetaPhi<T>::SphereThetaPhi(const SphereThetaPhi<T>& s)
  : NPhi_(s.NPhi_), TNphi_(s.TNphi_), Theta_(s.Theta_), pixels_(s.pixels_) 
{

  NTheta_= s.NTheta_;
  NPix_  = s.NPix_;
  Omega_ = s.Omega_;
  if(s.mInfo_) this->mInfo_= new DVList(*s.mInfo_);
}

//++
// Titre	Destructor
//--
//++
template <class T>
SphereThetaPhi<T>::~SphereThetaPhi()

//--
{}

template <class T>
void SphereThetaPhi<T>::InitNul()
//
{
  NTheta_= 0;
  NPix_  = 0;
//  pixels_.Reset();  Pas de reset par InitNul (en cas de share) - Reza 20/11/99 $CHECK$
}


//!   re-pixelize the sphere if (m > 0) 
template <class T>
void SphereThetaPhi<T>::Resize(int_4 m)
{
  if ((m <= 0) && (NTheta_ > 0) ) {
    if (PixelMap_GetGlobalPrintLevel()>0) 
      cout << "SphereThetaPhi<T>::Resize(m) with m<=0 - NOT resized" << endl;
    return;
  }
  InitNul();
  Pixelize(m);
}

//! Clone or share the SphereThetaPhi object \b a
template<class T>
void  SphereThetaPhi<T>::CloneOrShare(const  SphereThetaPhi<T>& a)
{

  NTheta_= a.NTheta_;
  NPix_  = a.NPix_;
  Omega_ = a.Omega_;
  NPhi_.CloneOrShare(a.NPhi_);
  TNphi_.CloneOrShare(a.TNphi_);
  Theta_.CloneOrShare(a.Theta_);
  pixels_.CloneOrShare(a.pixels_);
  if (this->mInfo_) {delete this->mInfo_; this->mInfo_ = NULL;}
  if (a.mInfo_) this->mInfo_ = new DVList(*(a.mInfo_));
}
//! Share the pixel data with object \b a
template<class T>
void  SphereThetaPhi<T>::Share(const  SphereThetaPhi<T>& a)
{

  NTheta_= a.NTheta_;
  NPix_  = a.NPix_;
  Omega_ = a.Omega_;
  NPhi_.Share(a.NPhi_);
  TNphi_.Share(a.TNphi_);
  Theta_.Share(a.Theta_);
  pixels_.Share(a.pixels_);
  if (this->mInfo_) {delete this->mInfo_; this->mInfo_ = NULL;}
  if (a.mInfo_) this->mInfo_ = new DVList(*(a.mInfo_));
}

////////////////////////// methodes de copie/share
//! Perform data copy or shares the data 
template<class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::Set(const SphereThetaPhi<T>& a)
{
  if (this != &a) 
    { 
      
      
      if (a.NbPixels() < 1) 
	throw RangeCheckError("SphereThetaPhi<T>::Set(a ) - Array a not allocated ! ");
      if (NbPixels() < 1) CloneOrShare(a);
      else CopyElt(a);
      if (this->mInfo_) delete this->mInfo_;
      this->mInfo_ = NULL;
      if (a.mInfo_) this->mInfo_ = new DVList(*(a.mInfo_));
    }
  return(*this);
}

//! Perform data copy or shares the data 
template<class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::CopyElt(const SphereThetaPhi<T>& a)
{
  if (NbPixels() < 1) 
    throw RangeCheckError("SphereThetaPhi<T>::CopyElt(const SphereThetaPhi<T>& )  - Not Allocated Array ! ");
  if (NbPixels() != a.NbPixels()) 
    throw(SzMismatchError("SphereThetaPhi<T>::CopyElt(const SphereThetaPhi<T>&) SizeMismatch")) ;

  NTheta_= a.NTheta_;
  NPix_  = a.NPix_;
  Omega_ = a.Omega_;
  for (int_4 k=0; k< NPix_; k++) pixels_(k) = a.pixels_(k);
  for (size_t k=0; k< a.NPhi_.Size(); k++) NPhi_(k) = a.NPhi_(k);
  for (size_t k=0; k< a.TNphi_.Size(); k++) TNphi_(k) = a.TNphi_(k);
  for (size_t k=0; k< a.Theta_.Size(); k++) Theta_(k) = a.Theta_(k);
  return(*this);

}

/* --Methode-- */
//!    Return total number of pixels 
template <class T>
int_4 SphereThetaPhi<T>::NbPixels() const
{
  return(NPix_);
}

/* --Methode-- */
//!    Return value of pixel with index k
template <class T>
T& SphereThetaPhi<T>::PixVal(int_4 k)
{
  if((k < 0) || (k >= NPix_)) 
    {
      throw RangeCheckError("SphereThetaPhi::PIxVal Pixel index out of range");
    }
  return pixels_(k);
}

//!    Return value of pixel with index k
template <class T>
T const& SphereThetaPhi<T>::PixVal(int_4 k) const
{
  if((k < 0) || (k >= NPix_)) 
    {
       throw RangeCheckError("SphereThetaPhi::PIxVal Pixel index out of range");
    } 
  return *(pixels_.Data()+k);
}

/* --Methode-- */
//! Return true if teta,phi in map  
template <class T>
bool SphereThetaPhi<T>::ContainsSph(double /*theta*/, double /*phi*/) const
{
  return(true);
}

/* --Methode-- */
//!    Return index of the pixel corresponding to direction (theta, phi).
template <class T>
int_4 SphereThetaPhi<T>::PixIndexSph(double theta, double phi) const
{
  double dphi;
  int_4 i,k;
  bool fgzn = false;

  if((theta > Pi) || (theta < 0.)) return(-1);
  if((phi < 0.) || (phi > DeuxPi)) return(-1);
  if(theta > Pi*0.5) {fgzn = true; theta = Pi-theta;}
  
  // La bande d'indice kt est limitée par les valeurs de theta contenues dans
  // Theta_[kt] et Theta_[kt+1]
  for( i=1; i< NTheta_; i++ ) 
    if( theta < Theta_(i) ) break;
  
  dphi= DeuxPi/(double)NPhi_(i-1);
  
  if (fgzn)  k= NPix_-TNphi_(i)+(int_4)(phi/dphi);
  else k= TNphi_(i-1)+(int_4)(phi/dphi); 
  return(k);
}

/* --Methode-- */
//!   Return (theta,phi) coordinates of middle of  pixel with  index k
template <class T>
void SphereThetaPhi<T>::PixThetaPhi(int_4 k,double& theta,double& phi) const
{
  int_4 i; 
  bool fgzn = false;
  
  if((k < 0) || (k >= NPix_)) {theta = -99999.; phi = -99999.; return; }
  if( k >= NPix_/2)  {fgzn = true; k = NPix_-1-k;}

  // recupère l'indice i de la tranche qui contient le pixel k
  for( i=0; i< NTheta_; i++ ) 
    if( k < TNphi_(i+1) ) break;

  // angle theta
  theta= 0.5*(Theta_(i)+Theta_(i+1));
  if (fgzn) theta= Pi-theta;
  
  // angle phi
  k -= TNphi_(i);
  phi= DeuxPi/(double)NPhi_(i)*(double)(k+.5);
  if (fgzn) phi= DeuxPi-phi;
}

//! Setting pixel values to a constant 
template <class T>
T SphereThetaPhi<T>::SetPixels(T v)
{
pixels_.Reset(v);
return(v);
}

/*!
  \brief Pixel Solid angle  (steradians)
  All the pixels have the same solid angle. The dummy argument is
  for compatibility with eventual pixelizations which would not 
  fulfil this requirement.
*/
template <class T>
double SphereThetaPhi<T>::PixSolAngle(int_4 /*dummy*/) const

{
  return Omega_;
}

/* --Methode-- */
//!   Return values of theta,phi which limit the pixel with  index k
template <class T>
void SphereThetaPhi<T>::Limits(int_4 k,double& tetMin,double& tetMax,double& phiMin,double& phiMax) 
  {
  int_4 j;
  double dphi;
  bool fgzn= false;
  
  if((k < 0) || (k >= NPix_)) {
    tetMin= -99999.; 
    phiMin= -99999.;  
    tetMax= -99999.; 
    phiMax= -99999.;  
    return;
  }
  
  // si on se trouve dans l'hémisphère Sud
  if(k >= NPix_/2) { 
    fgzn= true; 
    k= NPix_-1-k; 
  }
  
  // on recupere l'indice i de la tranche qui contient le pixel k
  int_4 i;
  for( i=0; i< NTheta_; i++ ) 
    if(k < TNphi_(i+1)) break;
  
  // valeurs limites de theta dans l'hemisphere Nord
  tetMin= Theta_(i);
  tetMax= Theta_(i+1);
  // valeurs limites de theta dans l'hemisphere Sud
  if (fgzn) {
    tetMin= Pi - Theta_(i+1);
    tetMax= Pi - Theta_(i);
  }
  
  // pixel correspondant dans l'hemisphere Nord
  if (fgzn) k= TNphi_(i+1)-k+TNphi_(i)-1;
  
  // indice j de discretisation ( phi= j*dphi )
  j= k-TNphi_(i);
  dphi= DeuxPi/(double)NPhi_(i);
  
  // valeurs limites de phi 
  phiMin= dphi*(double)(j);
  phiMax= dphi*(double)(j+1);
  return;
}

/* --Methode-- */
//!    Return number of theta-slices on the sphere
template <class T>
uint_4 SphereThetaPhi<T>::NbThetaSlices() const
{
  if (NTheta_<=0) 
    {
      throw PException(" sphere not pixelized, NbSlice=0 ");
    }
  return( 2*NTheta_);
}

/* --Methode-- */
//!    Return number of pixels along the phi-direction of the kt-th slice
template <class T>
int_4 SphereThetaPhi<T>::NPhi(int_4 kt) const
{
  int_4 nbpix;  
  // verification
  if((kt < 0) || (kt >= 2*NTheta_)) return(-1);
  
  // si on se trouve dans l'hemisphere Sud
  if(kt >= NTheta_) {
    kt= 2*NTheta_-1-kt; 
  }
  
  // nombre de pixels
  nbpix= NPhi_(kt);
  return(nbpix);
}


/* --Methode-- */
//!    Return  theta values which limit the slice kt
template <class T>
void SphereThetaPhi<T>::Theta(int_4 kt,double& tetMin,double& tetMax) const 
{
  bool fgzn= false;
  // verification
  if( (kt< 0) || (kt>= 2*NTheta_) ) {
    tetMin= -99999.; 
    tetMax= -99999.; 
    return;
  }

  // si on se trouve dans l'hemisphere Sud
  if( kt >= NTheta_ ) {
    fgzn= true;
    kt= 2*NTheta_-1-kt; 
  }
  
  // valeurs limites de theta dans l'hemisphere Nord
  tetMin= Theta_(kt);
  tetMax= Theta_(kt+1);
  // valeurs limites de theta dans l'hemisphere Sud
  if (fgzn) {
    tetMin= Pi - Theta_(kt+1);
    tetMax= Pi - Theta_(kt);
  }  
}

/* --Methode-- */
//!   Return values of phi which limit the jp-th pixel of the kt-th slice
template <class T>
void SphereThetaPhi<T>::Phi(int_4 kt,int_4 jp,double& phiMin,double& phiMax) const 
{
  // verification
  if((kt < 0) || (kt >= 2*NTheta_)) {
    phiMin= -99999.; 
    phiMax= -99999.; 
    return;
  }
  
  // si on se trouve dans l'hemisphere Sud
  if(kt >= NTheta_) kt= 2*NTheta_-1-kt;
  
  // verifie si la tranche kt contient au moins jp pixels
  if( (jp< 0) || (jp >= NPhi_(kt)) ) {
    phiMin= -88888.; 
    phiMax= -88888.; 
    return;
  }
  
  double dphi= DeuxPi/(double)NPhi_(kt);
  phiMin= dphi*(double)(jp);
  phiMax= dphi*(double)(jp+1);
  return;
}

/* --Methode-- */
//!    Return pixel index  with sequence index jp in the slice kt 
template <class T>
int_4 SphereThetaPhi<T>::Index(int_4 kt,int_4 jp) const
{
  int_4 k;
  bool fgzn= false;
  
  // si on se trouve dans l'hemisphere Sud
  if(kt >= NTheta_) {
    fgzn= true; 
    kt= 2*NTheta_-1-kt;
  }
  
  // si la tranche kt contient au moins i pixels
  if( (jp>=0) && (jp<NPhi_(kt)) ) 
    {
      // dans l'hemisphere Sud
      if (fgzn) k= NPix_-TNphi_(kt+1)+jp;
      // dans l'hemisphere Nord
      else k= TNphi_(kt)+jp;
    }
  else
    {
      k= 9999;
      printf("\n la tranche %d ne contient pas un pixel de rang %d",kt,jp);
    }
  return(k);
}

/* --Methode-- */
//!    Return indices kt (theta) and jp (phi) of  pixel with index k
template <class T>
void SphereThetaPhi<T>::ThetaPhiIndex(int_4 k,int_4& kt,int_4& jp) 
{
  bool fgzn= false;  
  // si on se trouve dans l'hemisphere Sud
  if(k >= NPix_/2) 
    { 
      fgzn= true; 
      k= NPix_-1-k; 
    }
  
  // on recupere l'indice kt de la tranche qui contient le pixel k
  int_4 i;
  for(i = 0; i < NTheta_; i++) 
    if(k < TNphi_(i+1)) break;
  
  // indice  kt de tranche
  if (fgzn) kt= 2*NTheta_-1-i;
  else kt= i;
  
  // indice jp de pixel
  if (fgzn) jp= TNphi_(i+1)-k-1;
  else jp= k-TNphi_(i);  
}
/*!
  \brief achieve the splitting into pixels 
  m has the same signification as for the constructor
  
  Each theta-slice of the north hemisphere will be spitted starting f
  from  phi=0 ...
  
  South hemisphere is scanned in the same direction according to phi
  and from equator to the pole (the pixel following the last one of
  the slice closest to the equator with z>0, is the pixel with lowest 
  phi of the slice closest of the equator with z<0).
*/
template <class T>
void  SphereThetaPhi<T>::Pixelize(int_4 m) 

//--
{
  int_4 ntotpix,j;
  
  // Decodage et controle des arguments d'appel :
  // au moins 2 et au plus 524288 bandes d'un hemisphere en theta
  if (m < 2) m = 2;
  if (m > 524288) m = 524288;
  
  // On memorise les arguments d'appel
  NTheta_ = m;  
  
  // On commence par decouper l'hemisphere z>0.
  // Creation des vecteurs contenant :
  // Les valeurs limites de theta (une valeur de plus que le nombre de bandes...)
  //  Theta_= new double[m+1];
  Theta_.ReSize(m+1);
  
  // Le nombre de pixels en phi de chacune des bandes en theta
  //  NPhi_ = new int_4[m];
  // une taille de m suffit, mais je mets m+1 pour que les 3 tableaux aient 
  // la meme taille pour une manipulation plus faciles par la librairie 
  // cfitsio -- GLM (13-04-00)
  NPhi_.ReSize(m+1);
  
  // Le nombre/Deuxpi total des pixels contenus dans les bandes de z superieur a une
  // bande donnee (mTPphi[m] contient le nombre de pixels total de l'hemisphere)
  //  TNphi_= new int_4[m+1];
    TNphi_.ReSize(m+1);
  
  // Calcul du nombre total de pixels dans chaque bande pour optimiser
  // le rapport largeur/hauteur des pixels 
  
  //calotte polaire
  TNphi_(0)= 0;
  NPhi_(0) = 1;
  
  //bandes jusqu'a l'equateur
  for(j = 1; j < m; j++) 
    {
      TNphi_(j)= TNphi_(j-1)+NPhi_(j-1);
      NPhi_(j) = (int_4)(.5+4.*(double)(m-.5)*sin(Pi*(double)j/(double)(2.*m-1.))) ;
    }
  
  // Nombre total de pixels sur l'hemisphere
  ntotpix  = TNphi_(m-1)+NPhi_(m-1);
  TNphi_(m)= ntotpix;
  // et sur la sphere entiere
  NPix_= 2*ntotpix;
  
  // Creation et initialisation du vecteur des contenus des pixels 
  pixels_.ReSize(NPix_);
  pixels_.Reset();

  // Determination des limites des bandes en theta :
  // omeg est l'angle solide couvert par chaque pixel,
  // une bande donnee kt couvre un angle solide NPhi_[kt]*omeg
  // egal a 2* Pi*(cos Theta_[kt]-cos Theta_[kt+1]). De meme, l'angle solide 
  //de la
  // calotte allant du pole a la limite haute de la bande kt vaut
  // 2* Pi*(1.-cos Theta_[kt+1])= TNphi_[kt]*omeg...
  
  double omeg2pi= 1./(double)ntotpix;
  Omega_ = omeg2pi*DeuxPi;
  
  for(j=0; j <= m; j++) 
    {
      Theta_(j)= acos(1.-(double)TNphi_(j)*omeg2pi);
    }
}

//! Return the theta angle for slice defined by \b index
template <class T>
r_8 SphereThetaPhi<T>::ThetaOfSlice(int_4 index) const
{
  if(index < 0 || index >= 2*NTheta_) 
    throw RangeCheckError("SphereThetaPhi::ThetaOfSlice() index out of range"); 
  double tet1, tet2;
  Theta(index, tet1, tet2); 
  return 0.5*(tet1+tet2);  
}

//! Return true : All theta slices have a symmetric slice at Pi-Theta in SphereThetaPhi
template <class T>
bool SphereThetaPhi<T>::HasSymThetaSlice() const 
{
  return true;
}
//! Return the slice index for the symmetric slice at theta=Pi-ThetaOfSlice(idx) 
template <class T>
int_4 SphereThetaPhi<T>::GetSymThetaSliceIndex(int_4 idx) const
{
  if(idx < 0 || idx >= (int_4)NbThetaSlices()) 
    throw RangeCheckError("SphereThetaPhi::GetSymThetaSliceIndex index out of range"); 
  return (NbThetaSlices()-1-idx);
}

/*!    
  \brief return a Theta slice information
  For a theta-slice with index 'index', return : 
  the corresponding "theta" 
  a vector containing the phi's of the pixels of the slice
  a vector containing the corresponding values of pixels 
*/
template <class T>
void SphereThetaPhi<T>::GetThetaSlice(int_4 index,r_8& theta, TVector<r_8>& phi, TVector<T>& value) const 

{

  if(index < 0 || index >= (int_4)NbThetaSlices()) 
    throw RangeCheckError("SphereThetaPhi::GetThetaSlice(idx...) index out of range"); 
    

  int_4 iring= Index(index,0);
  int_4 lring  = NPhi(index);

  phi.ReSize(lring);
  value.ReSize(lring);
  double Te= 0.;
  double Fi= 0.;
  PixThetaPhi(iring,Te,Fi);
  double DFi = DeuxPi/(double)NPhi(index);
  for(int_4 kk = 0; kk < lring; kk++)   {
    value(kk)= pixels_(kk+iring);
    phi(kk)= Fi;
    Fi += DFi;
  }
  theta= Te;
}

/*
  \brief return information on a theta slice 
  For a theta-slice with index 'index', return : 
  the corresponding "theta" 
  the corresponding "phi" for first pixel of the slice 
  a vector containing the indices of the pixels of the slice
  (equally distributed in phi)
  a vector containing the corresponding values of pixels 
*/
template <class T>
void SphereThetaPhi<T>::GetThetaSlice(int_4 index,r_8& theta, r_8& phi0,TVector<int_4>& pixelIndices, TVector<T>& value) const 

{

  if(index < 0 || index >= (int_4)NbThetaSlices()) 
    throw RangeCheckError("SphereThetaPhi::GetThetaSlice(idx...)  idx out of range"); 

  int_4 iring= Index(index,0);
  int_4 lring  = NPhi(index);

  pixelIndices.ReSize(lring);
  value.ReSize(lring);
  for(int_4 kk = 0; kk < lring; kk++)  {
    pixelIndices(kk)=kk+iring ;
    value(kk)= pixels_(kk+iring);
  }
  PixThetaPhi(iring,theta,phi0);
}

//! return a pointer to the specified slice pixel data
template <class T>
T* SphereThetaPhi<T>::GetThetaSliceDataPtr(int_4 index)
{
  if(index < 0 || index >= (int_4)NbThetaSlices()) 
    throw RangeCheckError("SphereThetaPhi::GetThetaSliceDataPtr(idx)  idx out of range"); 
  return pixels_.Begin()+Index(index,0);
}


template <class T>
void SphereThetaPhi<T>::print(ostream& os) const
{
  this->Show(os);
  os << "SphereThetaPhi<T> NTheta_= " << NTheta_ << " NPix_    = " << NPix_ 
     << " Omega_  =  " << Omega_   << endl;
  if(this->mInfo_) os << "  DVList Info= " << *(this->mInfo_) << endl;

  os << "... NPhi_ Values : ";
  int_4 i;
  for(i=0; i < NTheta_; i++)
    {
      if(i%5 == 0) os << endl;
      os << NPhi_(i) <<", ";
    }
  os << endl;

  os << "... Theta_ Values : ";
  for(i=0; i < NTheta_+1; i++)
    {
      if(i%5 == 0) os << endl;
      os << Theta_(i) <<", ";
    }
  os << endl;

  os << "... TNphi_ Values : ";
  for(i=0; i < NTheta_+1; i++)
    {
      if(i%5 == 0) os << endl;
      os << TNphi_(i) <<", ";
    }
  os << endl;

  os << "... Pixel Values : ";
  for(i=0; i < NPix_; i++)
    {
      if(i%5 == 0) os << endl;
      os <<  pixels_(i) <<", ";
    }
  os << endl;
}

//   ...... Operations de calcul  ......


//! Fill a SphereThetaPhi with a constant value \b a
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::SetT(T a) 
{
  if (NbPixels() < 1) 
    throw RangeCheckError("SphereThetaPhi<T>::SetT(T )  - SphereThetaPhi not dimensionned ! ");
  pixels_ = a;
  return (*this);
}

/*! Add a constant value \b x to a SphereThetaPhi */
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::Add(T a)
 {
  if (NbPixels()< 1) 
    throw RangeCheckError("SphereThetaPhi<T>::Add(T )  - SphereThetaPhi not dimensionned ! ");
  pixels_ += a; 
  return (*this);
}

/*! Substract a constant value \b a to a SphereThetaPhi */
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::Sub(T a,bool fginv) 
{
  if (NbPixels()< 1) 
    throw RangeCheckError("SphereThetaPhi<T>::Sub(T )  - SphereThetaPhi not dimensionned ! ");
  pixels_.Sub(a,fginv); 
  return (*this);
} 

/*! multiply a SphereThetaPhi by a constant value \b a */
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::Mul(T a) 
{
  if (NbPixels()< 1) 
    throw RangeCheckError("SphereThetaPhi<T>::Mul(T )  - SphereThetaPhi not dimensionned ! ");
  pixels_ *= a; 
  return (*this);
}

/*! divide a SphereThetaPhi by a constant value \b a */
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::Div(T a) 
{
  if (NbPixels()< 1) 
    throw RangeCheckError("SphereThetaPhi<T>::Div(T )  - SphereThetaPhi not dimensionned ! ");
  pixels_ /= a; 
  return (*this);
} 

//  >>>> Operations avec 2nd membre de type SphereThetaPhi
//! Add two SphereThetaPhi

template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::AddElt(const SphereThetaPhi<T>& a)
{
  if (NbPixels()!= a.NbPixels())
    {
    throw(SzMismatchError("SphereThetaPhi<T>::AddElt(const SphereThetaPhi<T>&) SizeMismatch")) ;
    }
  pixels_ += a.pixels_;
  return (*this);
}

//! Substract two SphereThetaPhi
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::SubElt(const SphereThetaPhi<T>& a)
{
  if (NbPixels()!= a.NbPixels())
    {
    throw(SzMismatchError("SphereThetaPhi<T>::SubElt(const SphereThetaPhi<T>&) SizeMismatch")) ;
    }
  pixels_ -= a.pixels_;
  return (*this);
}

//! Multiply two SphereThetaPhi (elements by elements)
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::MulElt(const SphereThetaPhi<T>& a)
{
  if (NbPixels()!= a.NbPixels())
    {
    throw(SzMismatchError("SphereThetaPhi<T>::MulElt(const SphereThetaPhi<T>&) SizeMismatch")) ;
    }
  pixels_ *= a.pixels_;
  return (*this);
}

//! Divide two SphereThetaPhi (elements by elements) - No protection for divide by 0
template <class T>
SphereThetaPhi<T>& SphereThetaPhi<T>::DivElt(const SphereThetaPhi<T>& a)
{
  if (NbPixels()!= a.NbPixels())
    {
    throw(SzMismatchError("SphereThetaPhi<T>::DivElt(const SphereThetaPhi<T>&) SizeMismatch")) ;
    }
  pixels_ /= a.pixels_;
  return (*this);
}



#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template SphereThetaPhi<int_4>
#pragma define_template SphereThetaPhi<r_8>
#pragma define_template SphereThetaPhi<r_4>
#pragma define_template SphereThetaPhi< complex<r_4> >
#pragma define_template SphereThetaPhi< complex<r_8> >
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
namespace SOPHYA {
template class SphereThetaPhi<int_4>;
template class SphereThetaPhi<r_8>;
template class SphereThetaPhi<r_4>;
template class SphereThetaPhi< complex<r_4> >;
template class SphereThetaPhi< complex<r_8> >;
}
#endif
