// This may look like C code, but it is really -*- C++ -*-

#ifndef PPFBINSTREAM_H_SEEN
#define PPFBINSTREAM_H_SEEN

// PPF (Portable Persistence Format) Input/Output streams
//
// E. Aubourg     CEA DAPNIA/SPP  1999
// R. Ansari      LAL IN2P3/CNRS  2000-2003


#include "machdefs.h"
#include "rawstream.h"

#include <time.h>

#include <complex>
#include <string>
#include <map>
#include <vector>



namespace SOPHYA {
  
class PPFBinaryIOStream {
public:

  enum  PPFByteOrdering  {PPS_NATIVE = -1, PPS_LITTLE_ENDIAN = 0, PPS_BIG_ENDIAN = 1};

  // Value of item identification tags in PPF binary streams   
  enum  PPFItemTag {
    PPS_NULL = 0,             // this is a null object
    PPS_STRING = 1,           // string, length (4b) + data
    PPS_OBJECT = 2,           // classId, data...
    PPS_REFERENCE = 3,        // objectId
    PPS_NAMETAG_TABLE = 4,    // Name tag table (Written at the end of file/stream) 
    PPS_EOF = 5,              // Just before tag infomation, offset to PPS_TAG
    PPS_ENDOBJECT = 6,        // marks the end of a given object information
    PPS_NAMETAG_MARK = 7,     // To have a name tag, position marker in a file
    PPS_POSTAG_MARK = 8,      // Position tag mark + 8 bytes (=stream position)
    PPS_POSTAG_TABLE = 9,     // Position tag table + 8 bytes (=stream position)
    PPS_SIMPLE = 16,          // 16 + number of bytes, up to 8 bytes
    PPS_SIMPLE_ARRAY4 = 32,   // 32 + number of bytes, up to 8 bytes, then 4 bytes of length
    PPS_SIMPLE_ARRAY8 = 48    // 48 + number of bytes, up to 8 bytes, then 8 bytes of length
  };

  // The following values are used with PPS_SIMPLE and PPS_SIMPLE_ARRAY (Using OR)
  enum   PPFItemTagDataType {  
    PPS_DATATYPE_CHAR = 0,        // 0  : DataType=character
    PPS_DATATYPE_FLOAT = 64,      // 64 : DataType=float 
    PPS_DATATYPE_COMPLEX = 65,    // 65 : DataType=complex 
    PPS_DATATYPE_INTEGER = 128,   // 128 :DataType=integer 
    PPS_DATATYPE_UNSIGNED = 192   // 192 :DataType=Unsigned integer 
  };
  
  PPFBinaryIOStream();
  virtual ~PPFBinaryIOStream();

  inline int    Version() {return version;}  // PIn/OutPersist version number
  inline time_t CreationDate() { return creationdate; }
  string        CreationDateStr(); 
  string        InfoString(); 

  inline int_8  NbPosTags() {return _nbpostag; }
  inline int_8  NbNameTags() {return tags.size(); }
  inline int_8  NbObjects() {return _nbobjs; }
  inline int_8  NbTopLevelObjects() {return _nbtlobjs; }
  inline int_8  NbReferences() {return _nbrefs; }
  inline int_4  MaxNestedObjsLevel() {return _maxnestlevel; }

  string GetTagName(int itag);  // 0..NbTags-1
  vector<string> const &  GetNameTags();  

protected:
  // La liste des NameTag ds le flot  
  map<string, int_8> tags;
  int version;     // PPersist(In/Out) version 
  time_t creationdate;   // Date de creation du fichier

  // Variables pour garder le compte des objets et des tags
  // Le nombre d'objets a l'ecriture est mis a jour par la classe
  // derivee POutPersist
  int_8 _nbpostag;     // Nb de tag de positionnement
  int_8 _nbobjs;       // Nb total d'objets
  int_8 _nbtlobjs;     // Nb d'objets de niveau 1 
  int_8 _nbrefs;       // Nb de references PPS_REFERENCE
  int_4 _maxnestlevel; // Niveau maximum d'objets emboites 
};
  
  
//! Input PPF (Portable Persist Format) streams 
class PPFBinaryInputStream : public virtual PPFBinaryIOStream {
public:
  PPFBinaryInputStream(RawInOutStream * is, bool ad, bool scan=false);
  PPFBinaryInputStream(string const& flnm, bool scan=true);
  virtual  ~PPFBinaryInputStream();

  //! return the file name 
  inline string FileName() { return s->getFileName(); }   // Retourne le nom de fichier
  // ! return the RawInOutStream stream 
  inline RawInOutStream& getStream()  { return (*s); }   
  
  // Gestion des tags
  bool   GotoPositionTag(int_8 pos);
  bool   GotoNameTag(string const& name);
  bool   GotoNameTagNum(int itag);  // 0..NbTags-1
  
  // Saut jusqu'au prochain objet 
  bool   SkipToNextObject();
  // Saut d'un item de base (tag+donnees correspondantes), le suivant ds le flot
  bool   SkipNextItem();
  // Lecture du tag de type next item + infos correspondantes
  // Le stream est re-positionne avant le tag 
  char   NextItemTag(short & datasz, size_t & sz);
  
  // Lecture donnees de base et tableaux de donnees de base
  // Basic data type (and corresponding arrays) reading
  void   GetByte (char& c);
  void   GetBytes(void* ptr, size_t bytes);
  void   GetR4   (r_4&);
  void   GetR4s  (r_4*, size_t);
  void   GetR8   (r_8&);
  void   GetR8s  (r_8*, size_t);
  void   GetI1   (int_1&);
  void   GetI1s  (int_1*, size_t);
  void   GetU1   (uint_1&);
  void   GetU1s  (uint_1*, size_t);
  void   GetI2   (int_2&);
  void   GetI2s  (int_2*, size_t);
  void   GetU2   (uint_2&);
  void   GetU2s  (uint_2*, size_t);
  void   GetI4   (int_4&);
  void   GetI4s  (int_4*, size_t);
  void   GetU4   (uint_4&);
  void   GetU4s  (uint_4*, size_t);
  void   GetI8   (int_8&);
  void   GetI8s  (int_8*, size_t);
  void   GetU8   (uint_8&);
  void   GetU8s  (uint_8*, size_t);
  void   GetLine (char* ptr, size_t len);
  void   GetStr  (string&);
  void   GetZ4   (complex<r_4> &);
  void   GetZ4s  (complex<r_4> *, size_t);
  void   GetZ8   (complex<r_8> &);
  void   GetZ8s  (complex<r_8> *, size_t);
#ifdef SO_LDBLE128
  void   GetR16  (r_16&);
  void   GetR16s (r_16*, size_t);
  void   GetZ16  (complex<r_16> &);
  void   GetZ16s (complex<r_16> *, size_t);
#endif
  
  inline void   Get(char&   c) {GetByte(c);}
  inline void   Get(r_4&    x) {GetR4(x);}
  inline void   Get(r_8&    x) {GetR8(x);}
  inline void   Get(uint_1& x) {GetU1(x);}
  inline void   Get(int_1&  x) {GetI1(x);}
  inline void   Get(uint_2& x) {GetU2(x);}
  inline void   Get(int_2&  x) {GetI2(x);}
  inline void   Get(uint_4& x) {GetU4(x);}
  inline void   Get(int_4&  x) {GetI4(x);}
  inline void   Get(uint_8& x) {GetU8(x);}
  inline void   Get(int_8&  x) {GetI8(x);}
  inline void   Get(complex<r_4> & x) {GetZ4(x);}
  inline void   Get(complex<r_8> & x) {GetZ8(x);}
  
  inline void   Get(r_4*    x, size_t n) {GetR4s(x,n);}
  inline void   Get(r_8*    x, size_t n) {GetR8s(x,n);}
  inline void   Get(uint_1* x, size_t n) {GetU1s(x,n);}
  inline void   Get(int_1*  x, size_t n) {GetI1s(x,n);}
  inline void   Get(uint_2* x, size_t n) {GetU2s(x,n);}
  inline void   Get(int_2*  x, size_t n) {GetI2s(x,n);}
  inline void   Get(uint_4* x, size_t n) {GetU4s(x,n);}
  inline void   Get(int_4*  x, size_t n) {GetI4s(x,n);}
  inline void   Get(uint_8* x, size_t n) {GetU8s(x,n);}
  inline void   Get(int_8*  x, size_t n) {GetI8s(x,n);}
  inline void   Get(string& x) {GetStr(x);}
  inline void   Get(complex<r_4> * x, size_t n) { GetZ4s(x, n); }
  inline void   Get(complex<r_8> * x, size_t n) { GetZ8s(x, n); }
#ifdef SO_LDBLE128
  inline void   Get(r_16&   x) {GetR16(x);}
  inline void   Get(r_16*   x, size_t n) {GetR16s(x,n);}
  inline void   Get(complex<r_16> & x) {GetZ16(x);}
  inline void   Get(complex<r_16> * x, size_t n) { GetZ16s(x, n); }
#endif  
  // Reading a list (table) of position tags
  void    GetPosTagTable(int_8*, size_t);
  void    GetPosTagTable(vector<int_8>&);
  
  void   AnalyseTags(int lev=0);   // List (all or some) tags ...
  
protected:
  void   Init(bool scan);
  void   ReadNameTagTableV2();
  void   ReadNameTagTable();

  void   SkipItem(bool fgrdi=true, unsigned char itag=0);

  void   CheckTag   (unsigned char datasz, unsigned char datatype);
  void   CheckArrayTag(unsigned char datasz, size_t sz, unsigned char datatype);
  void   GetTypeTag (unsigned char& c);
  void   GetRawByte (char& c);
  void   GetRawUByte (unsigned char& c);
  void   GetRawBytes(void* ptr, size_t bytes);
  void   GetRawI2   (int_2&);
  void   GetRawI4   (int_4&);
  void   GetRawI8   (int_8&);
  void   GetRawU8   (uint_8&);
  
  RawInOutStream* s;
  bool _ads; // delete/close the stream at the end
  
  bool bigEndian;
  // Si on a fait une lecture non sequentielle  -> seqread = false
  bool seqread;
};

//! Output stream for PPersit objects.
class PPFBinaryOutputStream : public virtual PPFBinaryIOStream {
public:
  PPFBinaryOutputStream(RawInOutStream* os, bool ad, int endianness = PPS_NATIVE);
  PPFBinaryOutputStream(string const& flnm, int endianness = PPS_NATIVE);
  virtual ~PPFBinaryOutputStream();

  //! return the file name 
  inline string FileName() { return s->getFileName(); }   // Retourne le nom de fichier
  // ! return the RawInOutStream stream 
  inline RawInOutStream& getStream()  { return (*s); }   

  // Ecriture de tags
  int_8 WritePositionTag();
  void  WriteNameTag(string const& name);
  inline void WriteTag(string const& name) { WriteNameTag(name); }

  void PutByte (char c);
  void PutBytes(void const* ptr, size_t bytes);
  void PutR4   (r_4);
  void PutR4s  (r_4 const*, size_t);
  void PutR8   (r_8);
  void PutR8s  (r_8 const*, size_t);
  void PutI1   (int_1);
  void PutI1s  (int_1 const*, size_t);
  void PutU1   (uint_1);
  void PutU1s  (uint_1 const*, size_t);
  void PutI2   (int_2);
  void PutI2s  (int_2 const*, size_t);
  void PutU2   (uint_2);
  void PutU2s  (uint_2 const*, size_t);
  void PutI4   (int_4);
  void PutI4s  (int_4 const*, size_t);
  void PutU4   (uint_4);
  void PutU4s  (uint_4 const*, size_t);
  void PutI8   (int_8);
  void PutI8s  (int_8 const*, size_t);
  void PutU8   (uint_8);
  void PutU8s  (uint_8 const*, size_t);
  void PutLine (char const* ptr, size_t len=0); // deprecated ?
  void PutStr  (string const&);
  void PutZ4   (complex<r_4>);
  void PutZ4s  (complex<r_4> const*, size_t);
  void PutZ8   (complex<r_8>);
  void PutZ8s  (complex<r_8> const*, size_t);
#ifdef SO_LDBLE128
  void PutR16  (r_16);
  void PutR16s (r_16 const*, size_t);
  void PutZ16  (complex<r_16>);
  void PutZ16s (complex<r_16> const*, size_t);
#endif  
  
  void   Put(char   c) {PutByte(c);}
  void   Put(r_4    x) {PutR4(x);}
  void   Put(r_8    x) {PutR8(x);}
  void   Put(complex<r_4>   x) {PutZ4(x);}
  void   Put(complex<r_8>   x) {PutZ8(x);}
  void   Put(uint_1 x) {PutU1(x);}
  void   Put(int_1  x) {PutI1(x);}
  void   Put(uint_2 x) {PutU2(x);}
  void   Put(int_2  x) {PutI2(x);}
  void   Put(uint_4 x) {PutU4(x);}
  void   Put(int_4  x) {PutI4(x);}
  void   Put(uint_8 x) {PutU8(x);}
  void   Put(int_8  x) {PutI8(x);}
  void   Put(r_4 const*    x, size_t n) {PutR4s(x,n);}
  void   Put(r_8 const*    x, size_t n) {PutR8s(x,n);}
  void   Put(complex<r_4> const*    x, size_t n) {PutZ4s(x,n);}
  void   Put(complex<r_8> const*    x, size_t n) {PutZ8s(x,n);}
#ifdef SO_LDBLE128
  void   Put(r_16   x) {PutR16(x);}
  void   Put(r_16 const*   x, size_t n) {PutR16s(x,n);}
  void   Put(complex<r_16>  x) {PutZ16(x);}
  void   Put(complex<r_16> const*   x, size_t n) {PutZ16s(x,n);}
#endif
  void   Put(uint_1 const* x, size_t n) {PutU1s(x,n);}
  void   Put(int_1 const*  x, size_t n) {PutI1s(x,n);}
  void   Put(uint_2 const* x, size_t n) {PutU2s(x,n);}
  void   Put(int_2 const*  x, size_t n) {PutI2s(x,n);}
  void   Put(uint_4 const* x, size_t n) {PutU4s(x,n);}
  void   Put(int_4 const*  x, size_t n) {PutI4s(x,n);}
  void   Put(uint_8 const* x, size_t n) {PutU8s(x,n);}
  void   Put(int_8 const*  x, size_t n) {PutI8s(x,n);}
  void   Put(string const& s) {PutStr(s);}
  
  // Writing a list of position tag table
  void   PutPosTagTable(int_8 const *, size_t);
  void   PutPosTagTable(vector<int_8> const&);
    
  
protected:
  void     Init(int endianness);
  void     WriteNameTagTable();
  void     WriteNameTagTableV2();

  void     PutArrayTag(unsigned char datasz, size_t sz, unsigned char datatype);
  void     PutRawByte (char);
  void     PutRawUByte (unsigned char);
  void     PutRawI2   (int_2);
  void     PutRawI4   (int_4);
  void     PutRawI8   (int_8);
  void     PutRawU8   (uint_8);
  void     PutRawBytes(void const* ptr, size_t bytes);

  // Attributs, variables
  RawInOutStream* s;
  bool _ads; // delete/close the stream at the end

  bool bigEndian;
};
  

} // namespace

#endif
