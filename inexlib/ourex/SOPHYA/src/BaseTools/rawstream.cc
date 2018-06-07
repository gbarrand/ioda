#include "sopnamsp.h"
#include "rawstream.h"
#include "pexceptions.h"

/*!
   \class SOPHYA::RawInOutStream
   \ingroup BaseTools
   This class defines the interface for a raw input/output stream.
   It has been designed primarily to be used by PPF streams 
   (Sophya Persistence).
*/

/* --Methode-- */
RawInOutStream::RawInOutStream()
{
}

/* --Methode-- */
RawInOutStream::~RawInOutStream()
{
}

/* --Methode-- */
bool  RawInOutStream::isSeekable() const
{
  return false;
}

/* --Methode-- */
int_8 RawInOutStream::tellg() const
{
  throw NotAvailableOperation("RawInOutStream::tellg() - Not implemented ");
}

/* --Methode-- */
RawInOutStream& RawInOutStream::seekg(int_8, int)
{ 
  throw NotAvailableOperation("RawInOutStream::seekg() - Not implemented ");
}

/* --Methode-- */
RawInOutStream& RawInOutStream::read(char* s, uint_8 n)
{
  throw NotAvailableOperation("RawInOutStream::read() - Not implemented ");
}
/* --Methode-- */
int_8 RawInOutStream::tellp() const
{
  throw NotAvailableOperation("RawInOutStream::tellp() - Not implemented ");
}

/* --Methode-- */
RawInOutStream& RawInOutStream::seekp(int_8, int)
{ 
  throw NotAvailableOperation("RawInOutStream::seekp() - Not implemented ");
}

/* --Methode-- */
RawInOutStream& RawInOutStream::write(const char* s, uint_8 n)
{
  throw NotAvailableOperation("RawInOutStream::write() - Not implemented ");
}

/* --Methode-- */
void RawInOutStream::flush()
{
  return;
}

/*!
   \class SOPHYA::RawInFileStream
   \ingroup BaseTools
   Implements the input interface (reading) of  RawInOutStream
   on a disk file using the standard C library (fopen, fread, ...)
*/

/* --Methode-- */
RawInFileStream::RawInFileStream(const char * path) 
  : RawInOutStream()
{
  fip = fopen(path,"rb");
  if (fip == NULL) { 
    string errmsg = "RawInFileStream() - fopen(rb) ERROR - File= ";
    errmsg += path;
    throw IOExc(errmsg);
  }
  _filename = path;
}

/* --Methode-- */
RawInFileStream::RawInFileStream(string const & path)
  : RawInOutStream()
{
  fip = fopen(path.c_str(),"rb");
  if (fip == NULL) { 
    string errmsg = "RawInFileStream() - fopen(rb) ERROR - File= ";
    errmsg += path;
    throw IOExc(errmsg);
  }
  _filename = path;
}

/* --Methode-- */
RawInFileStream::~RawInFileStream()
{
  fclose(fip);
}


/* --Methode-- */
bool  RawInFileStream::isSeekable() const
{
  return true;
}

/* --Methode-- */
int_8 RawInFileStream::tellg() const
{
  return ftell(fip) ;
}

/* --Methode-- */
RawInOutStream& RawInFileStream::seekg(int_8 off, int dir)
{ 
  int fsd = SEEK_SET;
  if (dir == ios::beg)  fsd = SEEK_SET;
  else if (dir == ios::end)  fsd = SEEK_END;
  else fsd = SEEK_CUR;
  fseek(fip, (long int)off, fsd);
  return (*this);
}

/* --Methode-- */
RawInOutStream& RawInFileStream::read(char* s, uint_8 n)
{
  size_t rc = fread((void *)s, 1, (size_t)n, fip);
  if (rc !=  (size_t)n) {
    string errmsg = "RawInFileStream::read() ERROR - File= ";
    errmsg += _filename;
    throw IOExc(errmsg);
  }
  return (*this);
}

/*!
   \class SOPHYA::RawOutFileStream
   \ingroup BaseTools
   Implements the output interface (writing) of  RawInOutStream
   on a disk file using the standard C library (fopen, fwrite, ...)
*/

/* --Methode-- */
RawOutFileStream::RawOutFileStream(const char * path) 
  : RawInOutStream()
{
  fip = fopen(path,"wb");
  if (fip == NULL) { 
    string errmsg = "RawOutFileStream() - fopen(wb) ERROR - File= ";
    errmsg += path;
    throw IOExc(errmsg);
  }
  _filename = path;
}

/* --Methode-- */
RawOutFileStream::RawOutFileStream(string const & path)
  : RawInOutStream()
{
  fip = fopen(path.c_str(),"wb");
  if (fip == NULL) { 
    string errmsg = "RawOutFileStream() - fopen(wb) ERROR - File= ";
    errmsg += path;
    throw IOExc(errmsg);
  }
   _filename = path;
}

/* --Methode-- */
RawOutFileStream::~RawOutFileStream()
{
  fclose(fip);
}


/* --Methode-- */
bool  RawOutFileStream::isSeekable() const
{
  return true;
}

/* --Methode-- */
int_8 RawOutFileStream::tellp() const
{
  return ftell(fip) ;
}

/* --Methode-- */
RawInOutStream& RawOutFileStream::seekp(int_8 off, int dir)
{ 
  int fsd = SEEK_SET;
  if (dir == ios::beg)  fsd = SEEK_SET;
  else if (dir == ios::end)  fsd = SEEK_END;
  else fsd = SEEK_CUR;
  fseek(fip, (long int)off, fsd);
  return (*this);
}

/* --Methode-- */
RawInOutStream& RawOutFileStream::write(const char* s, uint_8 n)
{
  size_t rc = fwrite((void *)s, 1, (size_t)n, fip);
  if (rc !=  (size_t)n) {
    string errmsg = "RawOutFileStream::write() ERROR - File= ";
    errmsg += _filename;
    throw IOExc(errmsg);
  }
  return (*this);
}

/* --Methode-- */
void RawOutFileStream::flush()
{
  fflush(fip);
}


/*!
   \class SOPHYA::RawInOutFileStream
   \ingroup BaseTools
   Implements the input interface (reading) and output interface (writing) of 
   RawInOutStream on a disk file using the standard C library (fopen, fread, ...)
*/

/* --Methode-- */
RawInOutFileStream::RawInOutFileStream(const char * path) 
  : RawInOutStream()
{
  fip_p = fopen(path,"wb");
  if (fip_g == NULL) { 
    string errmsg = "RawInOutFileStream() - fopen(wb) ERROR - File= ";
    errmsg += path;
    throw IOExc(errmsg);
  }
  fip_g = fopen(path,"rb");
  if (fip_g == NULL) { 
    string errmsg = "RawInOutFileStream() - fopen(rb) ERROR - File= ";
    errmsg += path;
    throw IOExc(errmsg);
  }
  _filename = path;
}

/* --Methode-- */
RawInOutFileStream::RawInOutFileStream(string const & path)
  : RawInOutStream()
{
  fip_g = fopen(path.c_str(),"rb");
  if (fip_g == NULL) { 
    string errmsg = "RawInOutFileStream() - fopen(rb) ERROR - File= ";
    errmsg += path;
    throw IOExc(errmsg);
  }
  _filename = path;
}

/* --Methode-- */
RawInOutFileStream::~RawInOutFileStream()
{
  fclose(fip_g);
}


/* --Methode-- */
bool  RawInOutFileStream::isSeekable() const
{
  return true;
}

/* --Methode-- */
int_8 RawInOutFileStream::tellg() const
{
  return ftell(fip_g) ;
}

/* --Methode-- */
RawInOutStream& RawInOutFileStream::seekg(int_8 off, int dir)
{ 
  int fsd = SEEK_SET;
  if (dir == ios::beg)  fsd = SEEK_SET;
  else if (dir == ios::end)  fsd = SEEK_END;
  else fsd = SEEK_CUR;
  fseek(fip_g, (long int)off, fsd);
  return (*this);
}

/* --Methode-- */
RawInOutStream& RawInOutFileStream::read(char* s, uint_8 n)
{
  size_t rc = fread((void *)s, 1, (size_t)n, fip_g);
  if (rc !=  (size_t)n) {
    string errmsg = "RawInOutFileStream::read() ERROR - File= ";
    errmsg += _filename;
    throw IOExc(errmsg);
  }
  return (*this);
}

/* --Methode-- */
int_8 RawInOutFileStream::tellp() const
{
  return ftell(fip_p) ;
}

/* --Methode-- */
RawInOutStream& RawInOutFileStream::seekp(int_8 off, int dir)
{ 
  int fsd = SEEK_SET;
  if (dir == ios::beg)  fsd = SEEK_SET;
  else if (dir == ios::end)  fsd = SEEK_END;
  else fsd = SEEK_CUR;
  fseek(fip_p, (long int)off, fsd);
  return (*this);
}

/* --Methode-- */
RawInOutStream& RawInOutFileStream::write(const char* s, uint_8 n)
{
  size_t rc = fwrite((void *)s, 1, (size_t)n, fip_p);
  if (rc !=  (size_t)n) {
    string errmsg = "RawInOutFileStream::write() ERROR - File= ";
    errmsg += _filename;
    throw IOExc(errmsg);
  }
  return (*this);
}

/* --Methode-- */
void RawInOutFileStream::flush()
{
  fflush(fip_p);
}
