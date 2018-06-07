/*--- This code is part of the SOPHYA library ---
  (C) Univ. Paris-Sud   (C) LAL-IN2P3/CNRS   (C) IRFU-CEA
  SOPHYA - R. Ansari (LAL)   2002
*/

#include "sopnamsp.h"
#include "tarray.h"
//G.Barrand #include "cimage.h"
#include "skymap.h"
#include "mapoperation.h"
/*G.Barrand :
#include "sambainit.h"
#include "fftwserver.h"
*/

#include <typeinfo>


//--------------------------------------------------------------------------
//  Classe pour projeter localement (dans un tableau indexe i,j) 
//  des coordonnees spheriques

class MtxLM {
public:
  MtxLM(double latdeg, double longdeg, 
	double deltheta_min, double delphi_min,
	int_4 npixtheta, int_4 npixphi);
  MtxLM(TMatrix<r_8> & mtx, bool uselat=true);

  virtual ~MtxLM();

  void Init(double latdeg, double longdeg,
	double deltheta_min, double delphi_min,
	int_4 npixtheta, int_4 npixphi);

  // Conversion d'index de pixel (ip,jt) (ip: Phi/X/Column, jt: Theta/Y/Row )
  // en Theta,Phi
  void ip_jt2tp(int_4 ip, int_4 jt, double& theta, double& phi);
  // Conversion de (Theta,Phi) en index de pixel (ip,jt) 
  // (ip: Phi/X/Column, jt: Theta/Y/Row )
  void tp2ipjt(double theta, double phi, int_4& ip, int_4& jt);

  double _latdeg, _longdeg;
  double _deltheta_min, _delphi_min;
  int_4 _npixtheta, _npixphi;
  double _theta0, _phi0;
  double _thetaC, _phiC;
  double _deltatheta, _deltaphi;
  static double _deuxpi;
};

double MtxLM::_deuxpi = 2.*M_PI;

MtxLM::MtxLM(double latdeg, double longdeg, 
	     double deltheta_min, double delphi_min,
	     int_4 npixtheta, int_4 npixphi)
{
  Init(latdeg, longdeg, deltheta_min, delphi_min, npixtheta,  npixphi );
}

//--------------------------------------------------------------------------

MtxLM::MtxLM(TMatrix<r_8> & mtx, bool uselat)
{
  double latdeg = (uselat) ? (double)mtx.Info()["Latitude_C"] : 0.;
  double longdeg = mtx.Info()["Longitude_C"];
  double deltheta_min = mtx.Info()["LatPixSize"];
  double delphi_min = mtx.Info()["LongPixSize"];
  int_4 npixtheta =  mtx.NRows();
  int_4 npixphi = mtx.NCols();
  Init(latdeg, longdeg, deltheta_min, delphi_min, npixtheta, npixphi);
}

MtxLM::~MtxLM() 
{
}

void MtxLM::Init(double latdeg, double longdeg,
	    double deltheta_min, double delphi_min,
	    int_4 npixtheta, int_4 npixphi)
{
  _latdeg = latdeg;
  _longdeg = longdeg;
  _deltheta_min = deltheta_min;
  _delphi_min = delphi_min;
  _npixtheta = npixtheta;
  _npixphi = npixphi;  
  _thetaC = (90.-latdeg)*M_PI/180.;
  _phiC = longdeg*M_PI/180.;
  _deltatheta = deltheta_min*M_PI/(180.*60.);
  _deltaphi = delphi_min*M_PI/(180.*60.)/sin(_thetaC);
  _theta0 = _thetaC-0.5*(double)_npixtheta*_deltatheta;
  if ((_thetaC + 0.5*(double)_npixtheta*_deltatheta) > M_PI-1.e-6) 
    throw RangeCheckError("MtxLM::Init() ThetaMax out of range (> M_PI-1.e-6)");
  if ((_thetaC - 0.5*(double)_npixtheta*_deltatheta) < 1.e-6) 
    throw RangeCheckError("MtxLM::Init() ThetaMin out of range (< 1.e-6)");
  
  double deltaphitot = delphi_min*M_PI/(180.*60.)/sin(_thetaC + 0.5*(double)_npixtheta*_deltatheta);
  if (deltaphitot*_npixphi > _deuxpi) 
    throw RangeCheckError("MtxLM::Init() Wrapping will occur for Phi at ThetaMax ! ");
  deltaphitot = delphi_min*M_PI/(180.*60.)/sin(_thetaC - 0.5*(double)_npixtheta*_deltatheta);
  if (deltaphitot*_npixphi > _deuxpi) 
    throw RangeCheckError("MtxLM::Init() Wrapping will occur for Phi at ThetaMin ! ");
  
  _phi0 = _phiC-0.5*(double)_npixphi*_deltaphi;
  return;
}

void MtxLM::ip_jt2tp(int_4 ip, int_4 jt, double& theta, double& phi)
{
  theta = jt*_deltatheta+_theta0;
  if ( (theta<0.) || (theta > M_PI) )
    throw RangeCheckError(" MtxLM::ip_jt2tp - Theta out of range !");
  double delphi = _delphi_min*M_PI/(180.*60.)/sin(theta);
  double phi0 = _phiC-0.5*(double)_npixphi*delphi;
  //  phi = ip*_deltaphi+_phi0;
  phi = ip*delphi+phi0;

  while (phi < 0.)  phi += _deuxpi;
  while (phi >=_deuxpi )  phi -= _deuxpi;

  if ( (phi<0.) || (phi > _deuxpi) ) 
    throw RangeCheckError(" MtxLM::ip_jt2tp - Phi out of range !");
  return;
}

void MtxLM::tp2ipjt(double theta, double phi, int_4& ip, int_4& jt)
{
  if ( (theta<0.) || (theta > M_PI) )
    throw RangeCheckError(" MtxLM::tp2ipjt - Theta out of range !");
  if ( (phi<0.) || (phi > _deuxpi) ) 
    throw RangeCheckError(" MtxLM::tp2ipjt - Phi out of range !");

  jt = (theta-_theta0)/_deltatheta;
  double delphi = _delphi_min*M_PI/(180.*60.)/sin(theta);
  double phi0 = _phiC-0.5*(double)_npixphi*delphi;
  if (phi0 < 0.) phi0 += 2*M_PI;
  //  ip = (phi-_phi0)/_deltaphi;
  ip = (phi-phi0)/delphi;
  if ((ip < 0) || (ip >= _npixphi) )
    throw RangeCheckError(" MtxLM::ip_jt2tp - ip out of range !");
  if ((jt < 0) || (jt >= _npixtheta) )
    throw RangeCheckError(" MtxLM::ip_jt2tp - jt out of range !");
  return;
}

//--------------------------------------------------------------------------
//     Fonction de filtrage de carte de type Matrix
//-------------------------------------------------------------------------- 

// ----- Declaration des fonctions 
template <class T>
void FilterMtxMap(TMatrix<T> & mtx, TMatrix<T> & mxout, int filw=1);
template <class T>
void Sph2LocalMap(SphericalMap<T> & in, LocalMap<T> & out);
template <class T>
void ProjectMatrix2Sph(TMatrix<T> & mtx, SphericalMap<T> & out, bool userot=false);
template <class T>
void ProjMatrix2Sph(TMatrix<T> & mtx, SphericalMap<T> & out);
template <class T>
void ProjMatrix2RotateSph(TMatrix<T> & mtx, SphericalMap<T> & out, 
			  Vector3d& omega, double phirot, bool uselat=true);
template <class T>
void Sphere2Matrix(SphericalMap<T> & in, TMatrix<T> & mtx, bool userot=false);
template <class T>
void Sph2Matrix(SphericalMap<T> & in, TMatrix<T> & mtx);
template <class T>
void RotateSph2Matrix(SphericalMap<T> & in, TMatrix<T> & mtx, 
		      Vector3d& omega,double phirot, bool uselat=true);

//---------------------------------------------------------------------
//----------------------- Corps des fonctions -------------------------
//---------------------------------------------------------------------

template <class T>
void FilterMtxMap(TMatrix<T> & mtx, TMatrix<T> & mxout, int filw)
{
  if (filw < 1) return;
  mxout = mtx;
  sa_size_t r,c;
  sa_size_t fw = 2*filw+1;
  // Filtering 
  // Filling holes in Matrix
  for(r=filw; r<mtx.NRows()-filw; r++) 
    for(c=filw; c<mtx.NCols()-filw; c++) 
      mxout(r,c) = mtx(Range(r-filw, 0, fw), Range(c-filw, 0, fw)).Sum()/fw*fw;
    
  return;
}

//--------------------------------------------------------------------------
// Projection depuis une carte spherique dans un local map
//-------------------------------------------------------------------------- 

template <class T>
void Sph2LocalMap(SphericalMap<T> & in, LocalMap<T> & out)
{
  out.SetPixels(0);
  int kout;
  double teta,phi;

  // Projecting to localMap
  for(kout=0; kout<out.NbPixels(); kout++) {
    out.PixThetaPhi(kout, teta, phi);
    int pixel = in.PixIndexSph(teta,phi);
    out(kout) = in.PixVal(pixel);
  }

  return;
}

//-------------------------------------------------------------------------- 
// Projection depuis une matrice ds carte spherique 
//-------------------------------------------------------------------------- 
template <class T>
void ProjectMatrix2Sph(TMatrix<T> & mtx, SphericalMap<T> & out, bool userot)
{
  if (userot) {
    double phi =  (double)mtx.Info()["Longitude_C"]+90.;
    double psi = -((double)mtx.Info()["Latitude_C"])*M_PI/180.;
    while (phi >= 360.) phi -= 360.;
    phi *= (M_PI/180.);
    Vector3d omega(M_PI/2., phi);
    ProjMatrix2RotateSph(mtx, out, omega, psi, false);
  }
  else ProjMatrix2Sph(mtx, out); 
  return;
}

//-------------------------------------------------------------------------- 
// Projection depuis une matrice ds carte spherique sans rotation explicite
//-------------------------------------------------------------------------- 
template <class T>
void ProjMatrix2Sph(TMatrix<T> & mtx, SphericalMap<T> & out)
{
  int kout;
  double teta,phi;

  // Projecting to Matrix
  sa_size_t i,j,r,c;
  MtxLM mtxlm(mtx);
  for(r=0; r<mtx.NRows(); r++) 
    for(c=0; c<mtx.NCols(); c++) {
      mtxlm.ip_jt2tp(c, r, teta, phi);
      out(teta, phi) = mtx(r,c); 
    }
  return;
}

//-------------------------------------------------------------------------- 
// Projection depuis une matrice ds carte spherique avec rotation explicite
//-------------------------------------------------------------------------- 
template <class T>
void ProjMatrix2RotateSph(TMatrix<T> & mtx, SphericalMap<T> & out, 
			  Vector3d& omega, double phirot, bool uselat)
{
  double teta,phi;

  // Projecting to Matrix
  sa_size_t r,c;
  MtxLM mtxlm(mtx, uselat);
  for(r=0; r<mtx.NRows(); r++) 
    for(c=0; c<mtx.NCols(); c++) {
      mtxlm.ip_jt2tp(c, r, teta, phi);
      Vector3d vv1(teta, phi);
      Vector3d vv2 = vv1.Rotate(omega, phirot);
      out(vv2.Theta(), vv2.Phi()) = mtx(r,c); 
    }
  return;
}
//-------------------------------------------------------------------------- 
// Projection depuis carte spherique ds une matrice 
//-------------------------------------------------------------------------- 
template <class T>
void Sphere2Matrix(SphericalMap<T> & in, TMatrix<T> & mtx, bool userot)
{
  if (userot) {
    double phi =  (double)mtx.Info()["Longitude_C"]+90.;
    double psi = -((double)mtx.Info()["Latitude_C"])*M_PI/180.;
    while (phi >= 360.) phi -= 360.;
    phi *= (M_PI/180.);
    Vector3d omega(M_PI/2., phi);
    RotateSph2Matrix(in, mtx, omega, psi, false);
  }
  else Sph2Matrix(in, mtx); 
  return;

}

//-------------------------------------------------------------------------- 
// Projection depuis carte spherique ds une matrice sans rotation explicite
//-------------------------------------------------------------------------- 
template <class T>
void Sph2Matrix(SphericalMap<T> & in, TMatrix<T> & mtx)
{
  int kout;
  double teta,phi;

  // Projecting to Matrix
  sa_size_t i,j,r,c;
  MtxLM mtxlm(mtx);
  for(r=0; r<mtx.NRows(); r++) 
    for(c=0; c<mtx.NCols(); c++) {
      mtxlm.ip_jt2tp(c, r, teta, phi);
      mtx(r, c) = in.PixVal(in.PixIndexSph(teta,phi));
    }

  // Filling holes in Matrix
  int_4 nholes = 0;
  for(r=2; r<mtx.NRows()-2; r++) 
    for(c=2; c<mtx.NCols()-2; c++) {
      if (fabs(mtx(r,c)) < 1.e-31) {
	nholes++;
	mtx(r,c) = mtx(Range(r-2, 0, 5), Range(c-2, 0, 5)).Sum()/24.;
      }
    }
  cout << " Sph2Matrix()/Info: NbHoles= " << nholes << " filled " << endl;
  return;
}

//-------------------------------------------------------------------------- 
// Projection depuis carte spherique ds une matrice, avec une rotation 
// d'angle phi autour du vecteur omega 
//-------------------------------------------------------------------------- 
template <class T>
void RotateSph2Matrix(SphericalMap<T> & in, TMatrix<T> & mtx, 
		      Vector3d& omega,double phirot, bool uselat)
{
  double teta,phi;

  // Projecting to Matrix
  sa_size_t r,c;
  MtxLM mtxlm(mtx,uselat);
  for(r=0; r<mtx.NRows(); r++) 
    for(c=0; c<mtx.NCols(); c++) {
      mtxlm.ip_jt2tp(c, r, teta, phi);
      Vector3d vv1(teta, phi);
      Vector3d vv2 = vv1.Rotate(omega, phirot);
      mtx(r, c) = in.PixVal(in.PixIndexSph(vv2.Theta(), vv2.Phi()));
    }

  // Filling holes in Matrix
  int_4 nholes = 0;
  for(r=2; r<mtx.NRows()-2; r++) 
    for(c=2; c<mtx.NCols()-2; c++) {
      if (fabs(mtx(r,c)) < 1.e-31) {
	nholes++;
	mtx(r,c) = mtx(Range(r-2, 0, 5), Range(c-2, 0, 5)).Sum()/24.;
      }
    }
  cout << " RotateSph2Matrix()/Info: NbHoles= " << nholes << " filled " << endl;
  return;
}


//-------------------------------------------------------------------------- 
//-------------------------------------------------------------------------- 

template <class T>
void CheckRotatedProjection(SphericalMap<T> & in);

//-------------------------------------------------------------------------- 
//               ************** Le main *****************
//-------------------------------------------------------------------------- 

int main(int narg, char* arg[])
{
  #define NLM 9
  // Tableau des theta,phi des cartes partielles
  // latitude_map : Latitude en degre
  // longitude_map : longitude en degre
  // nx_map : Nb de pixels selon la longitude (Phi)
  // ny_map : Nb de pixels selon la latitude (Theta)
  // resolx_map : Resolution cartes selon la longitude (phi)
  // resoly_map : Resolution cartes selon la latitue (theta)
  //  double latitude_map[NLM] = {0., -15., -15., 65., 65., 65., 65.};
  double latitude_map[NLM] = {0., -15., -15., -28., 50., 60., 50., 75., 75.};
  //  double thetadeg_map[NLM] = {90., 105., 105., 25., 25., 25., 25.};
  //  double thetadeg_map[NLM] = {90., 105., 105., 115., 40., 30., 40., 15., 15.};
  double thetadeg_map[NLM]; // = 90.-latitude_map[lnm]
  double longitude_map[NLM] = {95., 100., 120., 130., 82., 90., 200., 140., 200.};
  int nx_map[NLM] = {512, 512, 512, 512, 512, 512, 512, 512, 512};
  int ny_map[NLM] = {512, 512, 512, 512, 512, 512, 512, 512, 512};
  double resolx_map[NLM] = {2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5}; 
  double resoly_map[NLM] = {2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5};


  if ( (narg > 1) && (strcmp(arg[1], "-h") == 0) ) {
    cout << " Usage: sph2lm InHealPixPPFName OutPPFName [-userot/-tstrot] [Filter1/2W = 0] "
	 << endl;
    exit(0);
  }
  if (narg < 3) {
    cout << " sph2lm/Erreur argument - sph2lm -h for usage" << endl; 
    exit(0);
  }

  bool userot = false;
  bool tstrot = false;
  if (narg > 3) {
    if (strcmp(arg[3],"-userot") == 0) {
      userot = true;
      cout << " sph2lm: Explicit rotation will be used -> userot=true " << endl;
    }
    if (strcmp(arg[3],"-tstrot") == 0) {
      userot = true;
      cout << " sph2lm: Test rotated projection  -> tstrot=true / CheckRotatedProjection() " << endl;
    }
  }
  int filw = 0;
  if (narg > 4) filw = atoi(arg[4]);

  cout << "\n >>>> Starting sph2lm: arg[1]= " << arg[1] 
  << " arg[2]= " << arg[2] << " FW=" << filw << endl;

  // We handle exception at the high level
  try {
  // This macro initialize the library
  // static objects handle this - However, not all loader call
  // the constructor for static objects
  //GB SophyaInit();

  cout << " --- Reading input SphericalMap from file " << arg[1] << endl;
  PInPersist inppf(arg[1]);
  PPersist * pps = inppf.ReadObject();
  AnyDataObj * obj = pps->DataObj();
  cout << " Object type read from input PPF file : " 
       << typeid(*obj).name() << endl;
  SphereHEALPix<r_8> * ps8 = dynamic_cast< SphereHEALPix<r_8>* > (obj);
  SphereHEALPix<r_8> sphin;
  if (ps8 != NULL)  sphin.Share(*ps8);
  else {
    SphereHEALPix<r_4> * ps4 = dynamic_cast< SphereHEALPix<r_4>* > (obj);
    if (ps4 != NULL) {
      cout << " -- Converting SphereHEALPix<r_4> to SphereHEALPix<r_8> " << endl;
      sphin.Resize(ps4->SizeIndex());
      for(int kp=0; kp<sphin.NbPixels(); kp++) sphin(kp) = (*ps4)(kp);
    }
    else {
      cout << " sph2lm/Error : Unsupported input object ! " << endl;
      throw  TypeMismatchExc("sph2lm/Error wrong input object type ");
    }
  }

  //   inppf.GetObject(sphin);
  cout << " Sphere read: SizeIndex()= " << sphin.SizeIndex() 
       << " NbPixels()= " << sphin.NbPixels() << endl;

  if (tstrot) {
    CheckRotatedProjection(sphin);
    cout << " sph2lm: CheckRotatedProjection() done --> exit() " << endl;
    return(0);
  }
  cout << " --- Opening output PPF file " << arg[1] << endl;
  POutPersist outppf(arg[2]);

  //G.Barrand FFTWServer FFTServ;
  SphereHEALPix<r_8> sphck(128);
  SphereHEALPix<r_8> sphckmtx(128);

  int nlm;
  for(nlm=0; nlm<NLM; nlm++) {
    thetadeg_map[nlm] = 90.-latitude_map[nlm];
    cout << "\n ------ LocalMap No " << nlm+1 << " Latitude=" << latitude_map[nlm] 
	 << " Longitude= " << longitude_map[nlm] << endl;

    LocalMap<r_8> lm(nx_map[nlm], ny_map[nlm],
		     nx_map[nlm]*resolx_map[nlm]/60.,
		     ny_map[nlm]*resoly_map[nlm]/60.,
		     thetadeg_map[nlm], longitude_map[nlm],
		     nx_map[nlm]/2, ny_map[nlm]/2);
    TMatrix<r_8> mtx(nx_map[nlm], ny_map[nlm]);

    //    lm.SetOrigin(thetadeg_map[nlm], longitude_map[nlm]);
    //    lm.SetSize(nx_map[nlm]*resolx_map[nlm]/60.,
    //       ny_map[nlm]*resoly_map[nlm]/60.);
  
    lm.SetPixels((nlm+1)*100.);
    mtx.Info()["Latitude_C"] = latitude_map[nlm];
    mtx.Info()["Longitude_C"] = longitude_map[nlm];
    mtx.Info()["LatPixSize"] = resoly_map[nlm];
    mtx.Info()["LongPixSize"] = resolx_map[nlm];
    mtx = (nlm+1)*100.;
    
    cout << " Doing lm.Project(sphck) ... " << endl;
    lm.ProjectionToSphere(sphck);
    cout << " Doing mtx.Project(sphck) ... " << endl;
    ProjectMatrix2Sph(mtx, sphckmtx, userot);

    cout << " Doing Sph2LocalMap(sphin, lm) ... " << endl;
    Sph2LocalMap(sphin, lm);
    cout << " Doing Sphere2Matrix(sphin, lm) ... " << endl;
    Sphere2Matrix(sphin, mtx, userot);
    MuTyV num = nlm+1;
    string name = "lm" + (string)num;
    cout << " Writing local map and mtx  to OutPPF " << endl;
    outppf.PutObject(lm, name);
    name = "mtx" + (string)num;
    outppf.PutObject(mtx, name);

    /*G.Barrand :
    cout << " Calling FFTServ.FFTForward(mtx, fftmtx) " << endl;
    TMatrix< complex<r_8> > fftmtx;
    FFTServ.FFTForward(mtx, fftmtx); 

    name = "fftmtx" + (string)num;
    cout << " Writing fftmtx  to OutPPF " << endl;
    outppf.PutObject(fftmtx, name);
    */

    if (filw < 1) continue;
    /*G.Barrand :
    TMatrix<r_8> fmtx;
    cout << " Filtering mtx  FilterW= " << filw << endl;
    FilterMtxMap(mtx, fmtx, filw);
    cout << " Calling FFTServ.FFTForward(fmtx, fftfmtx) " << endl;
    TMatrix< complex<r_8> > fftfmtx;
    FFTServ.FFTForward(fmtx, fftfmtx); 

    cout << " Writing fmtx and fftfmtx  to OutPPF " << endl;
    name = "fmtx" + (string)num;
    outppf.PutObject(fmtx, name);
    name = "fftfmtx" + (string)num;
    outppf.PutObject(fftfmtx, name);    
    */
  }

  //  cout << " Doing Sph2Sph(sphin, lm) ... " << endl;
  //  Sph2Sph(sphin, lm);
  cout << "\n --- Writing sphck  and sphckmtxsphericalmap  to OutPPF " << endl;
  outppf.PutObject(sphck, "sphck");
  outppf.PutObject(sphckmtx, "sphckmtx");

  }
  catch (PThrowable & exc) {
    cerr << " Catched Exception " << (string)typeid(exc).name() 
	 << " - Msg= " << exc.Msg() << endl;
  }
  catch (...) {
    cerr << " some other exception was caught ! " << endl;
  }

  cout << " >>>> End of sph2lm  <<<< " << endl; 
}

//-------------------------------------------------------------------------- 
//    Fonction pour la verification des projection avec rotation 
//-------------------------------------------------------------------------- 

template <class T>
void CheckRotatedProjection(SphericalMap<T> & in)
{

  cout << " >>> CheckRotatedProjection() : Testing rotated projection with matrices " << endl;
  SphereHEALPix<r_8> sphckrot(128);
  double latdeg = 0.;
  double longdeg = 90.;
  int nr = 512;
  int nc = 512;
  double resol = 2.5; 
 
  TMatrix<r_8> mtx(nr, nc);
  TMatrix<r_8> mtxA, mtxB, mtxC; 
  
  mtx.Info()["Latitude_C"] = latdeg;
  mtx.Info()["Longitude_C"] = longdeg;
  mtx.Info()["LatPixSize"] = resol;
  mtx.Info()["LongPixSize"] = resol;
  
  mtx = 100.;
  ProjMatrix2Sph(mtx, sphckrot);
  Sph2Matrix(in, mtx);
  mtxA = mtx;
  Vector3d omega(M_PI/2., M_PI);
  mtx = 200.;
  ProjMatrix2RotateSph(mtx, sphckrot, omega, -M_PI/2.);
  RotateSph2Matrix(in, mtx, omega, -M_PI/2.);
  mtxB = mtx;
  mtx = 300.;
  ProjMatrix2RotateSph(mtx, sphckrot, omega, -M_PI/4.);
  RotateSph2Matrix(in, mtx, omega, -M_PI/4.);
  mtxC = mtx;

  string ppfname = "rotproj.ppf";
  cout << " -- Writing mtxA,B,C & sphckrot to OutPPF " << ppfname << endl;
  POutPersist outppf(ppfname);
  
  outppf.PutObject(mtxA, "mtxA");
  outppf.PutObject(mtxB, "mtxB");
  outppf.PutObject(mtxC, "mtxC");

  outppf.PutObject(sphckrot, "sphckrot");
  
  cout << " >>> End of CheckRotatedProjection() " << endl;
  
  return;
}
