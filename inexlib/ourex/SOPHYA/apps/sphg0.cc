
#include <iostream>
#include "spherehealpix.h"

/*G.Barrand :
#include "sopnamsp.h"
#include "sambainit.h"
#include "tod.h"
*/

int main() 
{
  double teta,phi;
  //GB SophyaInit();

  int  m=32;
  SOPHYA::SphereHEALPix<double> sph(m);
  for (int j=0;j<sph.NbPixels();j++) 
    {
      sph.PixThetaPhi(j,teta,phi);
      sph(j)= 0.2* cos(3.*teta)*sin(8*phi);
    }
  // Projection  dans une image

  cout <<  " ===== Fin de sphg0 ======== " << endl;
  return 0;
}
