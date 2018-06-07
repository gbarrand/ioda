// G. Le Meur 04/2000

#include "sopnamsp.h"
#include "fiolocalmap.h"
#include "fioarr.h"
#include "pexceptions.h"
#include "fiondblock.h"
#include "datatype.h"
#include <typeinfo>

//*******************************************************************
// class FIO_LocalMap<T>
//  Les objets delegues pour la gestion de persistance 
//*******************************************************************

//++
template <class T>
FIO_LocalMap<T>::FIO_LocalMap()
//
//--
{
  dobj= new LocalMap<T>;
  ownobj= true;
}
//++
template <class T>
FIO_LocalMap<T>::FIO_LocalMap(string const& filename)
//
//--
{
  dobj= new LocalMap<T>;
  ownobj= true;
  Read(filename);
}

//++
template <class T>
FIO_LocalMap<T>::FIO_LocalMap(const LocalMap<T>& obj)
//
//--
{
  dobj= new LocalMap<T>(obj, true);
  ownobj= true;
}

template <class T>
FIO_LocalMap<T>::FIO_LocalMap(LocalMap<T>* obj)
{
  dobj= obj;
  ownobj= false;
}

//++
template <class T>
FIO_LocalMap<T>::~FIO_LocalMap()
//
//--
{
  if (ownobj && dobj) delete dobj;
}

//++
template <class T>
AnyDataObj* FIO_LocalMap<T>::DataObj()
//
//--
{
  return(dobj);
}

//++
template <class T>
void FIO_LocalMap<T>::SetDataObj(AnyDataObj & o)
//
//--
{
  LocalMap<T> * po = dynamic_cast< LocalMap<T> * >(&o);
  if (po == NULL) {
    char buff[160];
    sprintf(buff,"FIO_LocalMap<%s>::SetDataObj(%s) - Object type  error ! ",
	    DataTypeInfo<T>::getTypeName().c_str(), typeid(o).name());
    throw TypeMismatchExc(PExcLongMessage(buff));    
  }
  if (ownobj && dobj) delete dobj;
  dobj = po; ownobj = false;
} 

//++
template <class T>
void FIO_LocalMap<T>::ReadSelf(PInPersist& is)
//
//--
{

  if(dobj == NULL) 
    {
      dobj= new LocalMap<T>;
      ownobj= true;
    }

  // Pour savoir s'il y avait un DVList Info associe
  char strg[256];
  is.GetLine(strg, 255);
  bool hadinfo= false;
  if(strncmp(strg+strlen(strg)-7, "HasInfo", 7) == 0)  hadinfo= true;
  if(hadinfo) 
    {    // Lecture eventuelle du DVList Info
      is >> dobj->Info(); 
    }

  int_4 nSzX;
  is.GetI4(nSzX);
  //  dobj->setSize_x(nSzX);

  int_4 nSzY;
  is.GetI4(nSzY);
  //  dobj->setSize_y(nSzY);

  // int_4 nPix;
  // is.GetI4(nPix);
  //  dobj->setNbPixels(nPix);
  dobj->ReSize(nSzX, nSzY); 
  string ss("local mapping is done");
  string sso;
  is.GetStr(sso);
  if(sso == ss)
    {
      //   cout<<" ReadSelf:: local mapping"<<endl;

      double angleX, angleY;
      is.GetR8(angleX);
      is.GetR8(angleY);
      dobj->SetSize(angleX, angleY);


      int_4 x0, y0;
      double theta, phi, angle;
      is.GetR8(theta);
      is.GetR8(phi);
      is.GetI4(x0);
      is.GetI4(y0);
      is.GetR8(angle);
      dobj->SetOrigin(theta, phi, x0, y0, angle);

    }
  
// On lit le DataBlock;
      //  FIO_NDataBlock<T> fio_nd(&dobj->DataBlock());
// On lit la matrice;
  FIO_TArray<T> fio_nd(&dobj->Matrix());
  fio_nd.Read(is);
}

//++
template <class T>
void FIO_LocalMap<T>::WriteSelf(POutPersist& os) const
//
//--
{
  if(dobj == NULL) 
    {
      cout << " FIO_LocalMap::WriteSelf:: dobj= null " << endl;
      return;
    }

  char strg[256];
  int_4 nSzX= dobj->Size_x();
  int_4 nSzY= dobj->Size_y();
 
  if(dobj->ptrInfo()) 
    {
      sprintf(strg,"LocalMap: NPixX=%6d  NPixY=%9d HasInfo",nSzX,nSzY);
      os.PutLine(strg);
      os << dobj->Info();
    }
  else 
    { 
      sprintf(strg,"LocalMap: NPixX=%6d  NPixY=%9d ",nSzX,nSzY);
      os.PutLine(strg);  
    }

  os.PutI4(nSzX);
  os.PutI4(nSzY);

  if(dobj->LocalMap_isDone())
    {
      string ss("local mapping is done");
      os.PutStr(ss);

      double angleX, angleY;
      dobj->Aperture(angleX, angleY);
      os.PutR8(angleX);
      os.PutR8(angleY);


      int_4 x0, y0;
      double theta, phi, angle;
      dobj->Origin(theta, phi, x0, y0, angle);
      os.PutR8(theta);
      os.PutR8(phi);
      os.PutI4(x0);
      os.PutI4(y0);
      os.PutR8(angle);

    }
  else
    {
      string ss("no local mapping");
      os.PutStr(ss);
    }

// On ecrit le dataBlock 
      //  FIO_NDataBlock<T> fio_nd(&dobj->DataBlock());
// On ecrit la matrice 
  FIO_TArray<T> fio_nd(&dobj->Matrix());
  fio_nd.Write(os);
}

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template FIO_LocalMap<int_4>
#pragma define_template FIO_LocalMap<r_4>
#pragma define_template FIO_LocalMap<r_8>
#pragma define_template FIO_LocalMap< complex<r_4> >
#pragma define_template FIO_LocalMap< complex<r_8> >
#endif
#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
namespace SOPHYA {
template class FIO_LocalMap<int_4>;
template class FIO_LocalMap<r_4>;
template class FIO_LocalMap<r_8>;
template class FIO_LocalMap< complex<r_4> >;
template class FIO_LocalMap< complex<r_8> >;
}
#endif
