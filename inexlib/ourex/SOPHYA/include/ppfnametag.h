// This may look like C code, but it is really -*- C++ -*-
//  Classe PPFNameTag  - R. Ansari - Nov 2003
//  To ease manipulation of nametags in PPF streams 

#ifndef PPFNAMETAG_H_SEEN
#define PPFNAMETAG_H_SEEN

#include "ppersist.h"
/*!
   \class SOPHYA::PPFNameTag
   \ingroup BaseTools
   A simple class which can be used in conjuction with << and >> operators
   to write nametags in POutPersist streams or position an input PInPersist
   stream at a given nametag. 
   Nametags are strings which can be used to identify objects for further 
   retrieval in PPF streams.
   Pushing a PPFNameTag(tname) to an output stream \b os corresponds to 
   so.WriteNameTag(tname). Extracting a PPFNameTag(tname) from an input stream
   \b is corresponds to is.GotoNameTag(tname).
   
   \code
   {
   // Writing a name tag and an object to a stream
   NDataBlock<int_4> idb(16);
   idb = 16;
   POutPersist so("myfile.ppf");
   so << PPFNameTag("MyIDB") << idb ;
   }
   //...
   {
   // Positioning and Reading from a PPF stream
   PInPersist si("myfile.ppf");
   NDataBlock<int_4> idb;
   si >>  PPFNameTag("MyIDB") >> idb;
   }
   \endcode
*/
//----------------------------------------------------------------------
namespace SOPHYA {
//! A simple class to ease manipulation of nametags in PPF streams

class PPFNameTag {
public:
  PPFNameTag(PPFNameTag const & ptn) { _tname = ptn._tname; }
  PPFNameTag(string const & tn) { _tname = tn; }
  PPFNameTag(const char * tn) { _tname = tn; }
  ~PPFNameTag() { } 
  inline bool GotoNameTag(PInPersist& pi) const
  { return pi.GotoNameTag(_tname); }
  inline void WriteTag(POutPersist& po) const
  { po.WriteNameTag(_tname); return; }
  inline char*  operator= (char* s) { _tname = s; return s; }
  inline string const & operator= (string const & s)  { _tname = s; return s; } 
  inline operator  string() const { return _tname; }
protected:
  string _tname; 
};

inline PInPersist& operator >> (PInPersist& si, PPFNameTag const & pnt) 
{ pnt.GotoNameTag(si); return(si); }

inline POutPersist& operator << (POutPersist& so, PPFNameTag const & pnt) 
{ pnt.WriteTag(so); return(so); }

}  // namespace SOPHYA

#endif   /* PPFNAMETAG_H_SEEN */
