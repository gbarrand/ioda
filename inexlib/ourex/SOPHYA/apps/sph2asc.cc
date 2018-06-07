
//G.Barrand : sph2asc.cc is not in SOPHYA, it came from a mail of Reza.

#include <spherehealpix.h>
#include <fitsinoutfile.h>
#include <fitsspherehealpix.h>

#include <iostream>
#include <fstream>

int main() {

//--------------------------------START-OF-sph2asc.cc-----------------------------------------
// c++ codelet to be executed by runcxx (SOPHYA) 
// runcxx -import SkyMap -import FitsIOServer -f sph2asc.cc 
//--------------------------------------------------------------------
// Make sure FitsIOServer module is initialised :
//G.Barrand : FitsIOServerInit();
// Open the FITS file
string infilename="wmap_band_imap_r9_9yr_K_v5.fits";
SOPHYA::FitsInOutFile fis(infilename, SOPHYA::FitsInOutFile::Fits_RO);
// And read the content into a HEALPix Sphere 
SOPHYA::SphereHEALPix<r_4> sph;
fis >> sph; 
// Check the size ... of the HEALPix sphere 
cout << sph; 
ofstream aof("wmap9y.asc");
// print out the theta phi pixel values 
aof << " --------------------------------------------------------- \n "
   << " k  Theta(k)   Phi(k)  Value(k)  \n" 
   << " --------------------------------------------------------- \n " << endl;
double theta, phi;
//ProgressBar pgb(sph.NbPixels());   // Warning: introduced in SOPHYA in Sep 2014
for(int_4 k=0; k<sph.NbPixels(); k++) {
 sph.PixThetaPhi(k,theta,phi);
 aof << k << " " << theta << " " << phi << " " << sph.PixVal(k) << endl;  
 //pgb.update(k);
}

 return 0;
}
