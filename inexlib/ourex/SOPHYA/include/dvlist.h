// This may look like C code, but it is really -*- C++ -*-
// Classe DVList : Liste dynamique de variables (valeurs) 
// identifiees par un nom    Reza Ansari - Dec 96

#ifndef DVLIST_H_SEEN
#define DVLIST_H_SEEN

#include "objfio.h"

#include <stdio.h>
#include <iostream>

#include "mutyv.h"
#include <list>
#include <map>

namespace SOPHYA {

//  Classe liste de variables  Dynamic Variable List  

//! Dynamic Variable List class. 
class DVList : public AnyDataObj {
public:

                    DVList();
                    DVList(const DVList&);
                    DVList(const char *flnm);

  virtual           ~DVList();

  DVList&           operator= (const DVList&);

  void              Clear();
  DVList&           Merge(const DVList&);

  //! Returns the number of elements (variables) in DVList object 
  inline size_t     Size() const { return(mvlist.size()); }
  //! Returns the number of elements (variables) in DVList object 
  inline size_t     NVar() const { return(mvlist.size()); }

  int_8             GetI(string const& key, int_8 def=-1) const;
  r_8               GetD(string const& key, r_8 def=-9.e19) const;
  complex<r_8>      GetZ(string const& key, complex<r_8> def=-9.e19) const;
  string            GetS(string const& key, const char* def="") const;
  string            GetComment(string const& key) const;

  bool              DeleteKey(string const& key);
  bool              HasKey(string const& key) const;

  void              SetI(string const& key, int_8 val);
  void              SetD(string const& key, r_8 val);
  void              SetZ(string const& key, complex<r_8> val);
  void              SetS(string const& key, const char *  val);
  void              SetS(string const& key, string const& val);
  void              SetT(string const& key, TimeStamp const& val);
  void              SetComment(string const& key, string const& comm);

  MuTyV             Get(string const& key) const ;
  MuTyV&            Get(string const& key);
/*! Returns the value associated with the name \b key */
  inline MuTyV      operator()  (string const& key) const { return Get(key); }
/*! Returns the value associated with the name \b key */
  inline MuTyV      operator[]  (string const& key) const { return Get(key); }
/*! Returns the global comment string associated with the object */
  inline string     Comment() const { return(comment); }
/*! Returns the value associated with the name \b key */
  inline MuTyV&     operator()  (string const& key)  { return Get(key); }
/*! Returns the value associated with the name \b key */
  inline MuTyV&     operator[]  (string const& key)  { return Get(key); }
/*! Returns the global comment string associated with the object */
  inline string&    Comment() { return(comment); }

/*! Prints a brief description of object on \b cout */
  inline  void      Show() const { Show(cout); }
  virtual void      Show(ostream& os)  const;
/*! Prints the list of variables on \b cout */
  inline  void      Print() const  { Print(cout); }  
  virtual void      Print(ostream& os)  const;

//!  \cond  
// Chaque element dans un DVList est constitue desormais d'un MuTyV
// et d'une chaine de caracteres (commentaire) regroupe dans la structure
// dvlElement. Ces elements sont associes aux noms de variables dans un 
// map<...> ValList.            Reza 02/2000
  struct dvlElement {MuTyV elval; string elcomm; } ;
  typedef map<string, dvlElement, less<string> >  ValList; 
//!  \endcond
/*! Returns an iterator pointing on the first variable in the list */
  inline ValList::const_iterator Begin() const { return(mvlist.begin()); } 
/*! Returns the iterator end value */
  inline ValList::const_iterator End() const { return(mvlist.end()); } 
  

private:
  
  ValList mvlist;
  string comment;
};

/*! operator << overloading - Prints the list on the stream \b s */
inline ostream& operator << (ostream& s, DVList const & dvl)
  {  dvl.Print(s);  return(s);  }

/*! Writes the object in the POutPersist stream \b os */
inline POutPersist& operator << (POutPersist& os, DVList & obj)
{ ObjFileIO<DVList> fio(&obj);  fio.Write(os);  return(os); }
/*! Reads the object from the PInPersist stream \b is */
inline PInPersist& operator >> (PInPersist& is, DVList & obj)
{ ObjFileIO<DVList> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }

// Classe pour la gestion de persistance
// ObjFileIO<DVList>

} // namespace SOPHYA

#endif /* DVLIST_H__SEEN */


