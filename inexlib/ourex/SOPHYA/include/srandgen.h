/* fonctions pour generateurs aleatoires  cmv 23/06/94 */
/*  Copie de nbrand.h .c  .Ansari 04/2000              */
/* interface avec RandomGeneratorInterface cmv 04/2009 */

#ifndef SRANDGEN_H_SEEN
#define SRANDGEN_H_SEEN

#include "machdefs.h"
#include <stdlib.h>
#include <complex>
#include "randinterf.h"

/* Declaration en fonctions inline interface avec RandomGeneratorInterface global*/

namespace SOPHYA {


inline double drand01() 
  {return RandomGeneratorInterface::GetGlobalRandGenP()->Flat01();}
inline double drandpm1() 
  {return RandomGeneratorInterface::GetGlobalRandGenP()->Flatpm1();}

inline double NorRand() 
  {return RandomGeneratorInterface::GetGlobalRandGenP()->Gaussian();}
inline double GaussianRand() 
  {return RandomGeneratorInterface::GetGlobalRandGenP()->Gaussian();}
inline double GaussianRand(double sigma) 
  {return RandomGeneratorInterface::GetGlobalRandGenP()->Gaussian(sigma);}
inline double GaussianRand(double sigma,double mu) 
  {return RandomGeneratorInterface::GetGlobalRandGenP()->Gaussian(sigma,mu);}

  inline double GaussianTailRand(double sdev,double slim=1.) 
  {return RandomGeneratorInterface::GetGlobalRandGenP()->GaussianTail(sdev,slim);}

inline complex< r_8 > ComplexGaussianRand()
  {return RandomGeneratorInterface::GetGlobalRandGenP()->ComplexGaussian();}
inline complex< r_8 > ComplexGaussianRand(double sig)
  {return RandomGeneratorInterface::GetGlobalRandGenP()->ComplexGaussian(sig);}
inline double ModComplexGaussianRand(double sig=1.)
  {return RandomGeneratorInterface::GetGlobalRandGenP()->ModComplexGaussian(sig);}

inline double PoissonRand(double mu, double mumax=-1) 
  {return RandomGeneratorInterface::GetGlobalRandGenP()->Poisson(mu,mumax);}

inline double ExponentialRand() 
  {return RandomGeneratorInterface::GetGlobalRandGenP()->Exponential();}

inline int Gaussian2DRhoRand(double &x,double &y,double mx,double my,double sx,double sy,double ro)
  {return RandomGeneratorInterface::GetGlobalRandGenP()->Gaussian2DRho(x,y,mx,my,sx,sy,ro);}
inline void Gaussian2DAngRand(double &x,double &y,double mx,double my,double sa,double sb,double teta)
  {return RandomGeneratorInterface::GetGlobalRandGenP()->Gaussian2DAng(x,y,mx,my,sa,sb,teta);}

inline void AutoInitRand(int lp)
  {return RandomGeneratorInterface::GetGlobalRandGenP()->AutoInit(lp);}

inline void ShowRandom()
  {return RandomGeneratorInterface::GetGlobalRandGenP()->ShowRandom();}

}  /* namespace SOPHYA */

#endif
