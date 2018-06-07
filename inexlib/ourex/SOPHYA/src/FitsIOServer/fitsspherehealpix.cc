#include "sopnamsp.h"
#include "pexceptions.h"
#include "fitsspherehealpix.h"
#include "tarray.h"
#include "fitstarray.h" 



///////////////////////////////////////////////////////////////////////
// ----objets delegues  pour la gestion de persistance I/O format fits--
// SphereHealpix
////////////////////////////////////////////////////////////////////

/*!
  \class SOPHYA::FITS_SphereHEALPix
  \ingroup FitsIOServer
  FITS format I/O handler for SOPHYA::SphereHEALPix objects.
*/


template <class T>
FITS_SphereHEALPix<T>::FITS_SphereHEALPix()
{
  dobj_= new SphereHEALPix<T>;
  ownobj_= true;
}
  
template <class T>
FITS_SphereHEALPix<T>::FITS_SphereHEALPix(const char inputfile[],int hdunum)
{
    dobj_= new SphereHEALPix<T>;
    ownobj_= true;
  Read(inputfile,hdunum);
}


template <class T>
FITS_SphereHEALPix<T>::FITS_SphereHEALPix(const SphereHEALPix<T>& obj)
{
  dobj_= new SphereHEALPix<T>(obj);
  ownobj_= true;
}

template <class T>
FITS_SphereHEALPix<T>::FITS_SphereHEALPix(SphereHEALPix<T>* obj)
{
  dobj_= obj;
  ownobj_= false;
}

template <class T>
FITS_SphereHEALPix<T>::~FITS_SphereHEALPix()
{
  if (ownobj_ && dobj_) delete dobj_;
}

template <class T>
AnyDataObj* FITS_SphereHEALPix<T>::DataObj()
{
  return(dobj_);
}

template <class T>
void FITS_SphereHEALPix<T>::SetDataObj(AnyDataObj & o)
{
  SphereHEALPix<T> * po = dynamic_cast< SphereHEALPix<T> * >(&o);
  if (po == NULL) return;
  if (ownobj_ && dobj_) delete dobj_;
  dobj_ = po; 
  ownobj_ = false;
} 

template <class T>
int FITS_SphereHEALPix<T>::CheckHandling(AnyDataObj & o)
{
  if ( typeid(o) == typeid( SphereHEALPix< T > ) ) return 2;
  SphereHEALPix<T> * po = dynamic_cast< SphereHEALPix<T> * >(&o);
  if (po == NULL) return 0;
  else return 1;
}

template <class T>
int FITS_SphereHEALPix<T>::CheckReadability(FitsInOutFile& is)
{
  if (is.CurrentHDUType() !=  BINARY_TBL ) return 0;
  string key;
  key = "PIXTYPE";
  if ( is.KeyValue(key) != "HEALPIX")  return 0;

  bool nosk = false;
  key = "ORDERING";
  is.KeyValue(key, nosk);
  bool nosk2 = false;
  key = "NSIDE";
  is.KeyValue(key, nosk2);
  if (nosk || nosk2) return 0;

  vector<string> colnames; 
  vector<int> coltypes;
  vector<LONGLONG> repcnt, width;
  long ncols = is.GetColInfo(colnames, coltypes, repcnt, width); 
  if (ncols < 1) return 0;
  T x = 0;
  int_4 xi4;  // pour gerer l'ambiguite cfitsio avec TINT32BIT=TLONG 
  if ((coltypes[0]==TINT32BIT)&&(FitsTypes::DataType(x)==FitsTypes::DataType(xi4)))  return 2;
  else if (coltypes[0] == FitsTypes::DataType(x)) return 2 ;
  else return 1;
  return 0;
}

template <class T>
FitsHandlerInterface* FITS_SphereHEALPix<T>::Clone()
{
  return new FITS_SphereHEALPix<T>() ; 
}


template <class T>
void FITS_SphereHEALPix<T>::WriteToFits(FitsOutFile& os) 
{
  if(dobj_ == NULL) 
    { 
      cout << " WriteToFits:: dobj_= null " << endl;
      return;
    }

  DVList dvl( dobj_->Info() );  

  SphereCoordSys* cs= dobj_->GetCoordSys();
  string description= cs->description();
  dvl["COORDSYS"]= description;

  dvl["PIXTYPE"] = "HEALPIX";
  dvl.SetComment("PIXTYPE", "HEALPIX Pixelization");
  dvl["ORDERING"]= dobj_->TypeOfMap();
  dvl.SetComment("ORDERING", "Pixel ordering scheme, either RING or NESTED");
  
  int nSide= dobj_->SizeIndex();
  dvl["NSIDE"]= (int_4) nSide;
  dvl.SetComment("NSIDE","Resolution parameter for HEALPIX" );

  int nPix= dobj_->NbPixels();
  dvl["FIRSTPIX"]= (int_4) 0;
  dvl.SetComment("FIRSTPIX", "First pixel # (0 based)");
  dvl["LASTPIX"]= (int_4) nPix-1;
  dvl.SetComment("LASTPIX", "Last pixel # (0 based)");
  dvl["Content"]= "SphereHEALPix";
  dvl.SetComment("Content", "name of SOPHYA object");
  dvl["SOPCLSNM"]= "SOPHYA::SphereHEALPix<T>";
  dvl.SetComment("SOPCLSNM", "SOPHYA class name");
  
  // On ecrit les dataBlocks 
  vector<string> Noms;   
  Noms.push_back(dvl.GetS("Content"));
  //  string extname("SIMULATION");
  string extname = os.NextExtensionName();

  string Type;
  if (typeid(T) == typeid(r_8) ) Type+='D';
  else if (typeid(T) == typeid(r_4) )  Type+='E';
    else if (typeid(T) == typeid(int_4) )  Type+='J';
    else
      {
       	cout <<  "FITS_SphereHEALPix<T>::WriteToFits()- Unsupported data type (" 
	     <<  typeid(T).name() << ") throwing IOExc " << endl;
	throw IOExc("FITS_SphereHEALPix<T>::WriteToFits() unsupported data type");
      }
  vector<int> dummy;
  os.makeHeaderBntblOnFits(Type, Noms, nPix, 1, &dvl, extname, dummy);
  os.PutColToFits(0,  nPix, dobj_->pixels_.Data()); 
} 

template <class T> 
void FITS_SphereHEALPix<T>::ReadFromFits(FitsInFile& is)
{
  if (is.CurrentHDUType() != BINARY_TBL )
    throw FitsIOException("FITS_SphereHEALPix<T>::ReadFromFits Not a binary table HDU");

  if(dobj_ == NULL) {
    dobj_= new SphereHEALPix<T>;
    ownobj_= true;      
  }
  int nbcols, nbentries;
  //

  nbcols = is.NbColsFromFits();
  if (nbcols != 1) 
    cout << "FITS_SphereHEALPix<T>::ReadFromFits()/Warning Fits::NbCols="
	 << nbcols << " not equal 1" << endl;
  
  DVList dvl=is.DVListFromFits();
   nbentries = is.NentriesFromFits(0);
  int lastpix=dvl.GetI("LASTPIX");
  if (lastpix>0)
    {
      if (nbentries!=lastpix+1) 
	{
	  nbentries=lastpix+1;
      	}
    }
  // Let's Read the SphereCoordSys object
  int id= SphereCoordSys::NEUTRAL;
  string description= "NEUTRAL SphereCoordSystem";
  string coordsys= dvl.GetS("COORDSYS");
  // $CHECK$ - Reza 2/05/2000 - compare(size_t, size_t,...) passe pas sur g++     
  //  if(coordsys.compare(0,7,"ROTATION",0,7) == 0)  $CHECK$ 
  if(coordsys.substr(0,8) == "ROTATION")
    {
      id= SphereCoordSys::ROTATION;
      description= "ROTATION SphereCoordSystem";
    } 
  //  else if(coordsys.compare(0,4,"OTHER",0,4) == 0) $CHECK$ Reza 2/05/2000 
  else if(coordsys.substr(0,5)  == "OTHER" )
    {
      id= SphereCoordSys::OTHER;
      description= "OTHER SphereCoordSystem";
    }
  dobj_->SetCoordSys(new SphereCoordSys(id,description));   

  string ordering= dvl.GetS("ORDERING");
  if ( (ordering.substr(0,4) != "RING" ) && (ordering.substr(0,6) != "NESTED" ) )  { 
    cerr << "FITS_SphereHEALPix/Error Not supported ORDERING= " << ordering 
	 << " != RING/NESTED " << endl; 
    throw FitsIOException("FITS_SphereHEALPix/Error ordering != RING/NESTED ");
  }

  bool fgring = true;
  if (ordering.substr(0,4) != "RING" )  fgring = false;

  int nside= dvl.GetI("NSIDE");
  int nPix = 0;
  if(nside <= 0)
    {
      //      throw IOExc("FITS_SphereHEALPix:: No resol parameter nside");
      if (lastpix>0)
	{
	  nside = sqrt((double)(nbentries/12));
	}
      else
	{
	  //	  dvl.Print();
	  // int naxis2 = dvl.GetI("NAXIS2");
	  // if (naxis2 > 0 ) 
	  //  {
	  nside = sqrt((double)(nbentries/12));
	      // }
	      // else
	      //  throw IOExc("FITS_SphereHEALPix:: No NSIDE nor LASTPIX nor NAXIS2 on file");
	}
    }
  nPix = 12*nside*nside;
  if (nPix != nbentries)
    {
      cout << "WARNING: le nombre de pixels relu " << nbentries << " est incoherent avec la pixelisation Healpix qui donne nPix= " << nPix << endl; 
      }
  double Omega= 4.0*Pi/nPix;
  dobj_->setParameters(nside,nPix,Omega,fgring);
  // On lit les DataBlocks;
  //
   dobj_->pixels_.ReSize(nPix);
  is.GetSingleColumn(dobj_->pixels_.Data(),nPix);
  //

  // on effectue le decoupage en tranches 
  dobj_->SetThetaSlices();
  //
  dobj_->Info()  = is.DVListFromFits();
}

template <class T>
void FITS_SphereHEALPix<T>::Mollweide_picture_projection(const char filename[])
{
  int ni = 300;
  int nj = 600;
  TMatrix<float> map(ni, nj);

   int i,j;
   for(i=0; i < ni; i++) 
     {
       double yd = (i+0.5)/ni-0.5;
       double facteur=2.*Pi/sin(acos(yd*2));
       double theta = (0.5-yd)*Pi;
       for(j=0; j < nj; j++)  
	 {
	   double xa = (j+0.5)/nj-0.5;
	   double phi =  xa*facteur+Pi;
	   float th=float(theta);
	   float ff=float(phi);
	   if (phi<2*Pi && phi>= 0) 
	     {
	       map(i,j) = (float)dobj_->PixValSph(th, ff);
	     }
	 }
     }
   FitsOutFile  fout(filename);
   fout.firstImageOnPrimaryHeader(true);
   FITS_TArray<r_4> fta(map);
   fta.Write(fout);
   
}
template <class T>
void FITS_SphereHEALPix<T>::sinus_picture_projection(const char filename[])
{
  int ni = 300;
  int nj = 600;
  TMatrix<float> map(ni, nj);

  int i,j;
   for(i=0; i < ni; i++) 
     {
       double yd = (i+0.5)/ni-0.5;
       double theta = (0.5-yd)*Pi;
	double facteur=1./sin(theta);
       for(j=0; j < nj; j++)  
	 {
	   double xa = (j+0.5)/nj-0.5;
	   double phi = 2.*Pi*xa*facteur+Pi;
	   float th=float(theta);
	   float ff=float(phi);
	   if (phi<2*Pi && phi>= 0) 
	     {
	       map(i,j) = (float)dobj_->PixValSph(th, ff);
	     }
	 }
     }
   FitsOutFile  fout(filename);
   fout.firstImageOnPrimaryHeader(true);
   FITS_TArray<r_4> fta(map);
   fta.Write(fout);

   
}


#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template FITS_SphereHEALPix<r_8>
#pragma define_template FITS_SphereHEALPix<r_4>
#pragma define_template FITS_SphereHEALPix<int_4>
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
namespace SOPHYA {
template class FITS_SphereHEALPix<r_8>;
template class FITS_SphereHEALPix<r_4>;
template class FITS_SphereHEALPix<int_4>;
}
#endif
