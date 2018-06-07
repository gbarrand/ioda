// This may look like C code, but it is really -*- C++ -*-

#ifndef RAWSTREAM_H_SEEN
#define RAWSTREAM_H_SEEN

// Classe de flot brut - pour utilisation par les streams PPersist
// R. Ansari      LAL IN2P3/CNRS  11/2003

#include "machdefs.h"
#include <stdio.h>
#include <iostream>
#include <string>


namespace SOPHYA {
class RawInOutStream {
public:
                      RawInOutStream();
  virtual             ~RawInOutStream();

  virtual bool        isSeekable() const ;
  // Interface pour lecture et positionnement en lecture
  virtual int_8       tellg() const;
  virtual RawInOutStream& seekg(int_8, int sd = ios::beg);
  inline  RawInOutStream& seekg_beg() { return seekg((int_8)0, ios::beg); }
  inline  RawInOutStream& seekg_end() { return seekg((int_8)0, ios::end); }
  virtual RawInOutStream& read(char* s, uint_8 n);
  // Interface pour ecriture et positionnement en ecriture
  virtual int_8       tellp() const;
  virtual RawInOutStream& seekp(int_8, int sd = ios::beg);
  inline  RawInOutStream& seekp_beg() { return seekp((int_8)0, ios::beg); }
  inline  RawInOutStream& seekp_end() { return seekp((int_8)0, ios::end); }
  virtual RawInOutStream& write(const char* s, uint_8 n);
  inline  std::string     getFileName() const { return _filename; }
  virtual void            flush();
protected:
  std::string _filename;

};

// Input flow on a disk file 
class RawInFileStream : public RawInOutStream {
public:
                      RawInFileStream(const char * path);
                      RawInFileStream(string const & path);
  virtual             ~RawInFileStream();

  virtual bool        isSeekable() const ;
  // Interface pour lecture et positionnement index/position lecture
  virtual int_8       tellg() const ;
  virtual RawInOutStream& seekg(int_8, int sd = ios::beg);
  virtual RawInOutStream& read(char* s, uint_8 n); 

protected:
  FILE * fip;
};

// Output flow on a disk file 
class RawOutFileStream : public RawInOutStream {
public:
                      RawOutFileStream(const char * path);
                      RawOutFileStream(string const & path);
  virtual             ~RawOutFileStream();

  virtual bool        isSeekable() const ;
  // Interface pour ecriture et positionnement index/position ecriture 
  virtual int_8       tellp() const ;
  virtual RawInOutStream& seekp(int_8 pos, int sd = ios::beg);
  virtual RawInOutStream& write(const char* s, uint_8 n);
  virtual void            flush();
protected:
  FILE * fip;
};

// Input-Output flow on a disk file 
class RawInOutFileStream : public RawInOutStream {
public:
                      RawInOutFileStream(const char * path);
                      RawInOutFileStream(string const & path);
  virtual             ~RawInOutFileStream();

  virtual bool        isSeekable() const ;
  // Interface pour lecture et positionnement index/position lecture
  virtual int_8       tellg() const ;
  virtual RawInOutStream& seekg(int_8, int sd = ios::beg);
  virtual RawInOutStream& read(char* s, uint_8 n); 
  // Interface pour ecriture et positionnement index/position ecriture 
  virtual int_8       tellp() const ;
  virtual RawInOutStream& seekp(int_8 pos, int sd = ios::beg);
  virtual RawInOutStream& write(const char* s, uint_8 n);
  virtual void            flush();

protected:
  FILE * fip_g;
  FILE * fip_p;
};


} // namespace SOPHYA

#endif
