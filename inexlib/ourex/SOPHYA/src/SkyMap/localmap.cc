#include "sopnamsp.h"
#include "smathconst.h"
#include <complex>
#include "fiondblock.h"

#define LOCALMAP_CC_BFILE  // avoid extern template declarations 
#include "localmap.h"

#include <string.h>
#include <iostream>
#include "timing.h"


/*!
  \class SOPHYA::LocalMap
  \ingroup SkyMap
  A local map is a 2 dimensional matrix, with ny rows and nx columns.
  The local map has a spatial origin with spherical coordinates (theta0, phi0), mapped to pixel(x0, y0). Default value of (x0, y0) is middle of the map, center of pixel(nx/2, ny/2).

 Each pixel(ip, it) is defined by its "phi-like" index ip (column 
 index in the matrix) and its "theta-like" index it (row index in 
 the matrix). Index ip is associated with x-axis in a map-coordinate 
 system ; index it is associated with y-axis in the same map-coordinate system.
  
    The map is supposed to lie in a plan tangent to the celestial sphere 
    at its origin, with spherical coordinates (theta0, phi0), whose pixel 
    numbers  are (x0,y0) in the local map as indicated above. The aperture of the map is defined by two values of angles, angleX and angleY, covered respectively by all the pixels in x direction and all the pixels in y direction.

    Each pixel has angleX/nx and angleY/ny, as angle extensions. So, in
    map-coordinate system the pixel (i,j) has following coordinates : 

    x = (i-x0)*angleX/nx
 
    y = (j-y0)*angleY/ny 
    
    (angles in radians)

    The projection (method : ProjectionToSphere() ) of the map onto a 
    sphere is made by the following procedure : 

    the sphere is supposed to have radius=1. The map is 
    considered to be tangent to the sphere, at a point with (theta0, phi0)
    spherical coodinates. A reference coordinate system (plane-coordinate
    system) , is chosen with respect to the plane of the map  with reference axes : 

    x-axis : vector tangent to a parallel at (theta0, phi0) on the sphere
    (components in "3-dim cartesian system : -sin(phi0) ; cos(phi0) ; 0)

    z-axis : vector-radius with 3-dim cartesian cordinates : 
             sin(theta0)*cos(phi0) ; sin(theta0*sin(phi0) ; cos(theta0)

    y-axis : z-axis^x-axis : tangent to the meridian at (theta0, phi0)


    note that the map-coordinate system may be rotated with respect to
    plane-coordinate system (parameter "angle" below).
    
    the projection of a map pixel is defined as the intersection of 
    the vector-radius, from sphere center to the pixel defined by
    his coordinates  in the plane-coordinate system (computed from x,y 
    above, with eventual rotation), with the sphere.

   In order to make an instance of this class  available, the user must either use the standard constructor LocalMap() (as well as the "full" constructor LocalMap() ) or use the "partial" constructor LocalMap() followed by call to SetSize() and SetOrigin() (for fixing links between the map and the celestial sphere.

Example : 

\verbatim 
  LocalMap<r_4>    lcm(300, 600, 10.,20., 90.,0.); // full cstr.
\endverbatim

defining a local map 300X600 pixels, representing a sphere portion of angular apertures 10 degres x 20 degres, with map center located on the equator, is equivalent to :

\verbatim 
  LocalMap<r_4>    lcm(300, 600); // partial cstr.
  lcm.SetSize(10., 20.);
  lcm.SetOrigin(90., 0.);
\endverbatim

as well as : 


\verbatim 
  LocalMap<r_4>    lcm(300, 600, 10.,20., 90.,0., 0, 0); // full cstr.
\endverbatim

(same map, but with origin of coordinates at pixel (0,0) instead of (150, 300) as it was implicit above) 

is equivalent to : 

\verbatim 
  LocalMap<r_4>    lcm(300, 600); 
  lcm.SetSize(10., 20.);
lcm.SetOrigin(90., 0., 0,0);
\endverbatim


*/
template<class T>
LocalMap<T>::LocalMap() :  pixels_()
{
  InitNul();
}

//! partial constructor of the local map
/*! \fn void SOPHYA::LocalMap::LocalMap(int_4 nx, int_4 ny)
  \param nx : number of pixels in x direction
  \param ny : number of pixels in y direction

must be followed by calls to the methods  SetSize() and SetOrigin() in order the object become usable

 */
template<class T>
LocalMap<T>::LocalMap(int_4 nx, int_4 ny)  
{
  InitNul();
  ReSize(nx, ny);
  //  nSzX_ = nx;
  //  nSzY_ = ny;
  //  nPix_= nx*ny;
  //  pixels_.ReSize(ny,nx);
}

//! full constructor of the local map
/*! \fn void SOPHYA::LocalMap:::LocalMap(int_4 nx, int_4 ny, double angleX,double angleY, double theta0,double phi0,int_4 x0,int_4 y0,double angle)
  \param nx : number of pixels in x direction
  \param ny : number of pixels in y direction
  \param angleX : total angle aperture in x direction (degrees)
  \param angleY : total angle aperture in y direction (degrees)
  \param theta0,phi0 : spherical coordinates of reference point at which 
                       the map is considered to be tangent to the sphere 
  \param x0, y0 : coordinates (in pixels) of the reference point just defined
  \param angle : angle (degrees) of the rotation between x-axis of 
                 map-coordinate system) and the tangent to parallel on 
                 the sphere (default : 0.).
 */
template<class T>
LocalMap<T>::LocalMap(int_4 nx, int_4 ny, double angleX,double angleY, double theta0,double phi0,int_4 x0,int_4 y0,double angle)  
{
  InitNul();
  //  nSzX_ = nx;
  // nSzY_ = ny;
  // nPix_= nx*ny;
  // pixels_.ReSize(ny,nx);
  ReSize(nx, ny);
  SetSize(angleX, angleY);
  SetOrigin(theta0, phi0, x0, y0, angle);
}

// void SOPHYA::LocalMap::LocalMap(int_4 nx, int_4 ny, double angleX,double angleY, double theta0,double phi0, double angle)

//! standard constructor of the local map
/*! \fn void SOPHYA::LocalMap::LocalMap(int_4 nx, int_4 ny, double angleX,double angleY, double theta0,double phi0, double angle)
  \param nx : number of pixels in x direction
  \param ny : number of pixels in y direction
  \param angleX : total angle aperture in x direction (degrees)
  \param angleY : total angle aperture in y direction (degrees)
  \param theta0,phi0 : spherical coordinates of reference point at which 
                       the map is considered to be tangent to the sphere 
  \param angle : angle (degrees) of the rotation between x-axis of 
                 map-coordinate system) and the tangent to parallel on 
                 the sphere (default : 0.).
 */
template<class T>
LocalMap<T>::LocalMap(int_4 nx, int_4 ny, double angleX,double angleY, double theta0,double phi0, double angle) 
{
  InitNul();
  //  nSzX_ = nx;
  //  nSzY_ = ny;
  // nPix_= nx*ny;
  //  pixels_.ReSize(ny,nx);
  ReSize(nx, ny);
  SetSize(angleX, angleY);
  SetOrigin(theta0, phi0, angle);
}

//! copy constructor 
/*! \fn void SOPHYA::LocalMap::LocalMap(const LocalMap<T>& lm, bool share)
  \param share : if true, share data. If false copy data
 */
template<class T>
LocalMap<T>::LocalMap(const LocalMap<T>& lm, bool share)
  : pixels_(lm.pixels_, share)
{

  if(lm.mInfo_) this->mInfo_= new DVList(*lm.mInfo_);
  recopierVariablesSimples(lm);
}

//! copy constructor 
/*! \fn void SOPHYA::LocalMap::LocalMap(const LocalMap<T>& lm)
  \warning datas are \b SHARED with \b lm.
  \sa TMatrix::TMatrix(const TMatrix<T>&)
*/
template<class T>
LocalMap<T>::LocalMap(const LocalMap<T>& lm)
  : pixels_(lm.pixels_)

{

  if(lm.mInfo_) this->mInfo_= new DVList(*lm.mInfo_);
  recopierVariablesSimples(lm);
}

//! destructor
/*! \fn void SOPHYA::LocalMap::~LocalMap()
 
 */ 
template<class T>
LocalMap<T>::~LocalMap() {;}



/*!  \fn void SOPHYA::LocalMap::ReSize(int_4 nx, int_4 ny) 

  Resize storage area for pixels 
\param nx 
\param ny  new pixel numbers
*/
template<class T>
void LocalMap<T>::ReSize(int_4 nx, int_4 ny) 
{
    // angles par pixel,  en radians 
  deltaPhi_   *=  nSzX_/nx;   
  deltaTheta_ *=  nSzY_/ny;

  nSzX_ = nx;
  nSzY_ = ny;
  nPix_= nx*ny;
  pixels_.ReSize(ny,nx);
  pixelSized_ = true;
}

////////////////////////// methodes de copie/share

template<class T>
void LocalMap<T>::CloneOrShare(const LocalMap<T>& a)
{
      recopierVariablesSimples(a);     
      pixels_.CloneOrShare(a.pixels_);
      if (this->mInfo_) {delete this->mInfo_; this->mInfo_ = NULL;}
      if (a.mInfo_) this->mInfo_ = new DVList(*(a.mInfo_));
}
template<class T>
void LocalMap<T>::Share(const LocalMap<T>& a)
{
      recopierVariablesSimples(a);     
      pixels_.Share(a.pixels_);
      if (this->mInfo_) {delete this->mInfo_; this->mInfo_ = NULL;}
      if (a.mInfo_) this->mInfo_ = new DVList(*(a.mInfo_));
}

template<class T>
LocalMap<T>& LocalMap<T>::CopyElt(const LocalMap<T>& a)
{
  if (NbPixels() < 1) 
    throw RangeCheckError("LocalMap<T>::CopyElt(const LocalMap<T>& )  - Not Allocated Array ! ");
  if (NbPixels() != a.NbPixels()) 
    throw(SzMismatchError("LocalMap<T>::CopyElt(const LocalMap<T>&) SizeMismatch")) ;
      recopierVariablesSimples(a);     
  pixels_ = a.pixels_;
  return(*this);

}

template<class T>
LocalMap<T>& LocalMap<T>::Set(const LocalMap<T>& a)
   {
  if (this != &a) 
    { 
      if (a.NbPixels() < 1) 
	throw RangeCheckError("LocalMap<T>::Set(a ) - Array a not allocated ! ");
      if (NbPixels() < 1) CloneOrShare(a);
      else CopyElt(a);
      if (this->mInfo_) delete this->mInfo_;
      this->mInfo_ = NULL;
      if (a.mInfo_) this->mInfo_ = new DVList(*(a.mInfo_));
    }
  return(*this);
   }


/*! \fn int_4 SOPHYA::LocalMap::NbPixels() const

   \return number of pixels 
*/
template<class T>
int_4 LocalMap<T>::NbPixels() const
{
  return(nPix_);
}

/*! \fn T& SOPHYA::LocalMap::PixVal(int_4 k)

   \return value of pixel with index k 
*/
template<class T>
T& LocalMap<T>::PixVal(int_4 k)
{
  if((k < 0) || (k >= nPix_)) 
    {
    throw RangeCheckError("LocalMap::PIxVal Pixel index out of range ");
    }
  //
  int_4 i,j;
  Getij(k,i,j);
  return(pixels_(j,i));

  //
}

/*! \fn T const& SOPHYA::LocalMap::PixVal(int_4 k) const
  const version of previous method 
*/
template<class T>
T const& LocalMap<T>::PixVal(int_4 k) const
{
  if((k < 0) || (k >= nPix_)) 
    {
    throw RangeCheckError("LocalMap::PIxVal Pixel index out of range ");
    } 
  //
  int_4 i,j;
  Getij(k,i,j);
  return(pixels_(j,i));
}

/*! \fn bool SOPHYA::LocalMap::ContainsSph(double theta, double phi) const
 \return true if teta,phi in map  
 <b> Not yet implemented </b>
*/
template<class T>
bool LocalMap<T>::ContainsSph(double theta, double phi) const
{
// $CHECK$  Reza 11/11/99 - A modifier
  throw NotAvailableOperation("LocalMap<T>::ContainsSph() - Not yet implemented !");
  return(true);
}

/*!  \fn int_4 SOPHYA::LocalMap::PixIndexSph(double theta,double phi) const

   angles in radians
  \return index of the pixel with spherical coordinates (theta,phi) 
*/
template<class T>
int_4 LocalMap<T>::PixIndexSph(double theta,double phi) const
{
  int_4 i,j;

  if (! LocalMap_isDone()) initializationError();
  double csTheta = cos(theta);
  double snTheta = sin(theta);
  double csPhiMPhiC = cos (phi - phiC_);
  double snPhiMPhiC = sin (phi - phiC_);
  // le point sur la sphere est note M. 
  // intersection P de OM avec le plan de la carte (tangent en C)
  double denom = snTheta*snthC_*csPhiMPhiC + csTheta*csthC_;
  if ( denom == 0.) 
    {
    }
  double lambda = 1./denom;
  // coordonnes du point d'intersection P dans le repere lie au plan

  double XP = lambda*snTheta*snPhiMPhiC;
  double YP = lambda*snTheta*csthC_*csPhiMPhiC;

  // coordonnees dans le repere lie a la carte (eventuellement tourne par
  // rapport au precedent.

  double X = XP*cosAngle_ + YP*sinAngle_;
  double Y = -XP*sinAngle_ + YP*cosAngle_;

  // en unites de pixels 

  X /= deltaPhi_;
  Y /= deltaTheta_;


  double xmin= -x0_-0.5;
  double xmax= xmin+nSzX_;
  if((X > xmax) || (X < xmin)) { 
    if (exc_outofmap_) throw RangeCheckError("LocalMap<T>::PixIndexSph() - Out of Map Theta/Phi (X) ");
    else return(-1);
  }
  double xcurrent= xmin;
  for(i = 0; i < nSzX_; i++ ) 
    {
      xcurrent += 1.;
      if( X < xcurrent ) break;
    }
  double ymin= -y0_-0.5;
  double ymax= ymin+nSzY_;
  if((Y >  ymax) || (Y < ymin)) { 
    if (exc_outofmap_) throw RangeCheckError("LocalMap<T>::PixIndexSph() - Out of Map Theta/Phi (Y) ");
    else return(-1);
  }
  double ycurrent= ymin;
  for(j = 0; j < nSzY_; j++ ) 
    {
      ycurrent += 1.;
      if( Y < ycurrent ) break;
    }
  return (j*nSzX_+i);
}

/*! \fn void SOPHYA::LocalMap::PixThetaPhi(int_4 k,double& theta,double& phi) const

   \return (theta, phi) coordinates of pixel with index k 
*/


template<class T>
void LocalMap<T>::PixThetaPhi(int_4 k,double& theta,double& phi) const
{

  int_4 i,j;
  Getij(k,i,j);

  PixThetaPhi(i,j, theta, phi);

}

/*! \fn void SOPHYA::LocalMap::PixThetaPhi(int_4 ip,int_4 it, double& theta,double& phi) const


   \return (theta, phi) coordinates of pixel of map with indices (ip,it) corresponding to x and y directions

 \param ip  phi-like index
 \param it  theta-like index
*/

template<class T>
void LocalMap<T>::PixThetaPhi(int_4 ip,int_4 it, double& theta,double& phi) const
{
  double XP, YP, ZP;
  PixToSphereC(ip,it,XP,YP,ZP);

  theta = acos(ZP);
  phi = atan2(YP, XP);
  while (phi < 0.)  phi += 2.*Pi;

}




/*! \fn T SOPHYA::LocalMap::SetPixels(T v)

Set all pixels to value v 
*/
template <class T>
T LocalMap<T>::SetPixels(T v)
{
pixels_ = v;
return(v);
}
 
/*! \fn double SOPHYA::LocalMap::PixSolAngle(int_4 k) const
   Pixel Solid angle  of pixel k (steradians). 
   For the moment, all the pixels are considered to have  the same size in (theta, phi). So the parameter k is dummy.
      
*/  
template<class T>
double LocalMap<T>::PixSolAngle(int_4 k) const
{
  double XP, YP, ZP;
  int_4 i,j;
  Getij(k,i,j);
  PixToSphereC(i,j,XP,YP,ZP);

  double theta = acos(ZP);

  

  // angle solide

  double sol= 2.* deltaPhi_*sin(theta)*sin(0.5*deltaTheta_);
  return sol;
}

/*! \fn void SOPHYA::LocalMap::PixToSphereC(int_4 ip, int_4 it, double& XP, double& YP, double& ZP) const

projection of a pixel of map, onto the unity sphere ; result in cartesian coordinates.
*/

template<class T>
void LocalMap<T>::PixToSphereC(int_4 ip, int_4 it, double& XP, double& YP, double& ZP) const
{
  if (! LocalMap_isDone()) initializationError();
  double X= double(ip-x0_)* deltaPhi_;
  double Y= double(it-y0_)*deltaTheta_; 

    // situation dans le plan de reference tangent
  double dx= X*cosAngle_-Y*sinAngle_;
  double dy= X*sinAngle_+Y*cosAngle_;

  // Aout07 : Correction -dy change en +dy suite message S.Plaszczynski du 12/07/07 pour XP et YP (Reza)
  XP = XC_ - dx*snphC_ + dy*csthC_*csphC_;  // Aout07: -dy change en +dy (Rz/SP)
  YP = YC_ + dx*csphC_ + dy*csthC_*snphC_;  // Aout07: -dy change en +dy (Rz/SP)
  ZP = ZC_ + dy*snthC_;

  // on renormalise pour eviter les probleme de precision lors
  // de la prise ulterieure de lignes trigonometriques
  double norme = sqrt(XP*XP + YP*YP + ZP*ZP);
  XP /= norme;
  YP /= norme;
  ZP /= norme;
}

/*! \fn void SOPHYA::LocalMap::SetOrigin(double theta0,double phi0,double angle)
   set the referential of the map (angles in degrees)

    \param theta0
    \param phi0 celestial coordinates attributed to center pixel:  x0=siz_x/2,  y0=siz_y/2 
\param angle  angle between map referential and plane-coordinate system (see class description)
*/
template<class T>
void LocalMap<T>::SetOrigin(double theta0,double phi0,double angle)
{
  SetOrigin(theta0, phi0,  nSzX_/2,  nSzY_/2, angle);
}
 
/*!  \fn void SOPHYA::LocalMap::SetOrigin(double theta0,double phi0,int_4 x0,int_4 y0,double angle)
   set the referential of the map (angles in degrees)

    \param theta0
    \param phi0 celestial coordinates attributed to center pixel: (x0,y0) 
\param angle  angle between map referential and plane-coordinate system (see class description)

*/
template<class T>
void LocalMap<T>::SetOrigin(double theta0,double phi0,int_4 x0,int_4 y0,double angle)
{

  if ( theta0 < 0. || theta0 > 180. || phi0 < 0. || phi0 > 360. || angle < -90. || angle > 90.)
    {
      throw ParmError("LocalMap::SetOrigin  angle out of range"); 
    }
  SetCoorC(theta0,phi0);
  angleDegres_ = angle; 
  // en radians 
  angle_ = angle*Pi/180.;
  x0_= x0;
  y0_= y0;
  cosAngle_= cos(angle_);
  sinAngle_= sin(angle_);

   if (originSet_)
     {
       cout << " WARNING:  LocalMap<T>::SetOrigin() : reset origine of a local map which had already an origin " << endl;
     }
   originSet_ = true;
} 

template<class T>
void LocalMap<T>::SetCoorC(double theta0, double phi0)
{

  thetaDegresC_ = theta0;
  phiDegresC_ = phi0;

  // passage en radians
  thetaC_= theta0*Pi/180.;
  phiC_  = phi0*Pi/180.;
  csthC_ = cos(thetaC_);
  snthC_ = sin(thetaC_);
  csphC_ = cos(phiC_);
  snphC_ = sin(phiC_);
  XC_ = snthC_*csphC_;
  YC_ = snthC_*snphC_;
  ZC_ = csthC_;
}


// angles en RADIANS
template<class T>
TMatrix<double> LocalMap<T>::CalculMatricePassage()
{
  cout << " calcul matrice de passage " << endl;
  if (! LocalMap_isDone()) initializationError();
  TMatrix<double> passage(3,3);
  double cos_th_axeZ;
  double sin_th_axeZ;
  double cos_phi_axeZ;
  double sin_phi_axeZ;

  double cth,sth, cdeltaPhi, phi;

  if ( snthC_ <= 0.) // carte centree au pole 
    {
      cos_th_axeZ = 1.;
      sin_th_axeZ = 0.;
      cos_phi_axeZ = 1.;
      sin_phi_axeZ = 0.;
      
    }
  else
    {
      cth = cosAngle_*snthC_;
      double arg = 1.-cth*cth;
      if (arg <= 0. ) // carte centree sur l'equateur, sans rotation
	{
	  cos_th_axeZ = 1.;
	  sin_th_axeZ = 0.;
	  cos_phi_axeZ = csphC_;
	  sin_phi_axeZ = snphC_;
	}
      else
	{
	  sth = sqrt(arg);
	  cdeltaPhi = -csthC_*cth/(snthC_*sth);
	  if (cdeltaPhi < -1. ) cdeltaPhi = -1.;
	  if (cdeltaPhi > 1. ) cdeltaPhi = 1.;
	  phi = phiC_ + acos( cdeltaPhi );

	  cos_th_axeZ = cth;
	  sin_th_axeZ = sth;
	  cos_phi_axeZ = cos(phi);
	  sin_phi_axeZ = sin(phi);
	}
    }
  passage(0,0) = snthC_*csphC_;
  passage(1,0) = snthC_*snphC_;
  passage(2,0) = csthC_;

  passage(0,2) = sin_th_axeZ*cos_phi_axeZ;
  passage(1,2) = sin_th_axeZ*sin_phi_axeZ;
  passage(2,2) = cos_th_axeZ;

  passage(0,1) = passage(1,2)*passage(2,0) - passage(2,2)*passage(1,0);
  passage(1,1) = passage(2,2)*passage(0,0) - passage(0,2)*passage(2,0); 
  passage(2,1) = passage(0,2)*passage(1,0) - passage(1,2)*passage(0,0);

  //  passage.Print(cout);
  // cout << " fin calcul passage " << endl;

  return passage;


}

/*! \fn void SOPHYA::LocalMap::SetSize(double angleX,double angleY)

   angle range of tthe map (angles in degrees) 
\param angleX phi-like angle
\param angleX theta-like angle
*/
template<class T>
void LocalMap<T>::SetSize(double angleX,double angleY)
{
  if (!pixelSized_) initializationError();

  if ( angleX <= 0. || angleX > 180. || angleY <= 0. || angleY > 180.)
    {
      throw ParmError("LocalMap::SetSize extension angle out of range"); 
    }

  angleDegresX_ = angleX;
  angleDegresY_ = angleY;
  // angles par pixel,  en radians 
  cout << " taille x " << nSzX_ <<"  taille y " << nSzY_ << endl;
  deltaPhi_   = angleX*Pi/(180.*nSzX_);
  deltaTheta_ = angleY*Pi/(180.*nSzY_);
 
  if (angleSized_)
     {
       cout << " WARNING LocalMap::SetSize  ; angle resizing of an already sized local map " << endl;
     }
  angleSized_  = true;

}



/*! \fn void SOPHYA::LocalMap::ProjectionToSphere(SphericalMap<T>& sphere) const

Projection to a spherical map 
*/
template<class T>
void LocalMap<T>::ProjectionToSphere(SphericalMap<T>& sphere) const
{
   double theta, phi;
  int it, ip;
   for (it = 0; it < nSzY_; it++)
    {
      for (ip = 0; ip < nSzX_; ip++) 
	{
	  PixThetaPhi(ip,it, theta, phi);
	  sphere(theta,phi)= pixels_(it, ip);
	}
    }
}



// Titre	Private Methods
//++
template<class T>
void LocalMap<T>::InitNul()
//
//    set attributes to zero
//--
{ 

  pixelSized_  = false;
  angleSized_  = false;
  originSet_ = false;

  nSzX_ = 0;
  nSzY_ = 0;
  angleDegresX_ = 0.;
  angleDegresY_ = 0.;
  thetaDegresC_ = 0.;
  phiDegresC_   = 0.;
  x0_ = 0;
  y0_ = 0;
  angleDegres_ = 0.;



  nPix_ = 0;

  thetaC_ = 0.;
  phiC_   = 0.;
  csthC_  = 1.;
  snthC_  = 0.;
  csphC_  = 1.;
  snphC_  = 0.;
  XC_     = 0.;
  YC_     = 0.;
  ZC_     = 1.;

  angle_      = 0.;
  cosAngle_   = 1.;
  sinAngle_   = 0.;
  deltaPhi_   = 0.;
  deltaTheta_ =0.;
  SetThrowExceptionWhenOutofMapFlag(false); // Genere des exceptions si theta-phi out of range 
}


/*!   \fn void SOPHYA::LocalMap::Getij(int_4 k,int_4& i,int_4& j) const

 \return 2 indices corresponding to the pixel number k 
*/
template<class T>
void LocalMap<T>::Getij(int_4 k,int_4& i,int_4& j) const
{
  i= (k+1)%nSzX_-1;
  if(i == -1) i= nSzX_-1;
  j= (k-i+2)/nSzX_;
}






template<class T>
void LocalMap<T>::print(ostream& os) const
{
  os<<" SzX= "<<nSzX_<<", SzY= "<<nSzY_<<", NPix= "<<nPix_<<endl;
      os<<" theta0= "<<thetaC_*180./Pi <<", phi0= "<<phiC_*180./Pi <<", angle= "<<angle_*180./Pi<<endl;
      os<<" x0= "<<x0_<<", y0= "<<y0_<<endl;
      os<<" cos= "<< cosAngle_ <<", & sin= "<<sinAngle_<<endl;
      os<<"  deltaPhi_= "<<  deltaPhi_ <<", deltaTheta = "<< deltaTheta_ <<endl;
      os <<endl;

  os << " contenu de pixels : ";
  for(int i=0; i < nPix_; i++)
    {
      if(i%5 == 0) os << endl;
  int_4 ik,jk;
  Getij(i,ik,jk);
      os <<  pixels_(jk,ik) <<", ";
    }
  os << endl;
}


template<class T>
void LocalMap<T>::recopierVariablesSimples(const LocalMap<T>& lm)
{

  pixelSized_ = lm.pixelSized_;
  angleSized_ = lm.angleSized_;
  originSet_  = lm.originSet_ ;


  nSzX_ = lm.nSzX_;
  nSzY_ = lm.nSzY_;
  nPix_ = lm.nPix_;
  x0_   = lm.x0_;
  y0_   = lm.y0_;

  thetaC_ = lm.thetaC_;
  phiC_   = lm.phiC_;
  csthC_  = lm.csthC_;
  snthC_  = lm.snthC_;
  csphC_  = lm.csphC_;
  snphC_  = lm.snphC_;
  XC_     = lm.XC_;
  YC_     = lm.YC_;
  ZC_     = lm.ZC_;

  angle_= lm.angle_;
  cosAngle_  = lm.cosAngle_;
  sinAngle_  = lm.sinAngle_;
  deltaPhi_  = lm. deltaPhi_;
  deltaTheta_ = lm.deltaTheta_;
}

template<class T>
void LocalMap<T>::initializationError() const
{
  cout << endl;
  if (!pixelSized_) cout << " LocalMap::initializationError() ; pixel sizing is missing " << endl;
  if (!angleSized_ ) cout << " LocalMap::initializationError() ; angle sizing is missing " << endl;
  if (!originSet_ ) cout << " LocalMap::initializationError() ; origin of map is not set " << endl;
      throw PException("LocalMap::initializationError() : bad initialization of the local map ");


}


//   ...... Operations de calcul  ......


/*!  \fn void SOPHYA::LocalMap::SetT(T a)
 Fill a LocalMap with a constant value \b a
 */
template <class T>
LocalMap<T>& LocalMap<T>::SetT(T a) 
{
  if (NbPixels() < 1) 
    throw RangeCheckError("LocalMap<T>::SetT(T )  - LocalMap not dimensionned ! ");
  pixels_ = a;
  return (*this);
}

/*!  \fn void SOPHYA::LocalMap::Add(T a)
Add a constant value \b x to a LocalMap 
 */
template <class T>
LocalMap<T>& LocalMap<T>::Add(T a)
 {
  if (NbPixels()< 1) 
    throw RangeCheckError("LocalMap<T>::Add(T )  - LocalMap not dimensionned ! ");
  pixels_ += a; 
  return (*this);
}

/*!  \fn void SOPHYA::LocalMap::Sub(T a,bool fginv)

Substract a constant value \b a to a LocalMap 

*/
template <class T>
LocalMap<T>& LocalMap<T>::Sub(T a,bool fginv) 
{
  if (NbPixels()< 1) 
    throw RangeCheckError("LocalMap<T>::Sub(T )  - LocalMap not dimensionned ! ");
  pixels_.Sub(a,fginv); 
  return (*this);
} 

/*! \fn void SOPHYA::LocalMap::Mul(T a)
mutiply a LocalMap by a constant value \b a 
*/
template <class T>
LocalMap<T>& LocalMap<T>::Mul(T a) 
{
  if (NbPixels()< 1) 
    throw RangeCheckError("LocalMap<T>::Mul(T )  - LocalMap not dimensionned ! ");
  pixels_ *= a; 
  return (*this);
}

/*! \fn void SOPHYA::LocalMap::Div(T a)
divide a LocalMap by a constant value \b a 
*/
template <class T>
LocalMap<T>& LocalMap<T>::Div(T a) 
{
  if (NbPixels()< 1) 
    throw RangeCheckError("LocalMap<T>::Div(T )  - LocalMap not dimensionned ! ");
  pixels_ /= a; 
  return (*this);
} 

//  >>>> Operations avec 2nd membre de type LocalMap
/*! \fn void SOPHYA::LocalMap::AddElt(const LocalMap<T>& a)
Add two LocalMap
 */
template <class T>
LocalMap<T>& LocalMap<T>::AddElt(const LocalMap<T>& a)
{
  if (nSzX_ != a.nSzX_ || nSzY_ != a.nSzY_)
    {
    throw(SzMismatchError("LocalMap<T>::AddElt(const LocalMap<T>&) SizeMismatch")) ;
    }
  pixels_ += a.pixels_;
  return (*this);
}

/*! \fn void SOPHYA::LocalMap::SubElt(const LocalMap<T>& a)
Substract two LocalMap
 */
template <class T>
LocalMap<T>& LocalMap<T>::SubElt(const LocalMap<T>& a)
{
  if (nSzX_ != a.nSzX_ || nSzY_ != a.nSzY_)
    {
    throw(SzMismatchError("LocalMap<T>::SubElt(const LocalMap<T>&) SizeMismatch")) ;
    }
  pixels_ -= a.pixels_;
  return (*this);
}

/*! \fn void SOPHYA::LocalMap::MulElt(const LocalMap<T>& a)
Multiply two LocalMap (elements by elements)
 */
template <class T>
LocalMap<T>& LocalMap<T>::MulElt(const LocalMap<T>& a)
{
  if (nSzX_ != a.nSzX_ || nSzY_ != a.nSzY_)
    {
    throw(SzMismatchError("LocalMap<T>::MulElt(const LocalMap<T>&) SizeMismatch")) ;
    }
  //  pixels_ *= a.pixels_;
  pixels_.DataBlock() *= a.pixels_.DataBlock();
  return (*this);
}

/*! \fn void SOPHYA::LocalMap::DivElt(const LocalMap<T>& a)
Divide two LocalMaps (elements by elements) - No protection for divide by 0
*/
template <class T>
LocalMap<T>& LocalMap<T>::DivElt(const LocalMap<T>& a)
{
  if (nSzX_ != a.nSzX_ || nSzY_ != a.nSzY_)
    {
    throw(SzMismatchError("LocalMap<T>::DivElt(const LocalMap<T>&) SizeMismatch")) ;
    }
  //  pixels_ /= a.pixels_;
  pixels_.DataBlock() /= a.pixels_.DataBlock();
  return (*this);
}






#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template LocalMap<int_4>
#pragma define_template LocalMap<r_8>
#pragma define_template LocalMap<r_4>
#pragma define_template LocalMap< complex<r_8> >
#pragma define_template LocalMap< complex<r_4> >
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
namespace SOPHYA {
template class LocalMap<int_4>;
template class LocalMap<r_8>;
template class LocalMap<r_4>;
template class LocalMap< complex<r_8> >;
template class LocalMap< complex<r_4> >;
}
#endif
