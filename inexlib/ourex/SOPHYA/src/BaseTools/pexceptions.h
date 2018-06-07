// This may look like C code, but it is really -*- C++ -*-
#ifndef PEXCEPTIONS_SEEN
#define PEXCEPTIONS_SEEN

#include "machdefs.h"
#include <exception>
#include <string>

using namespace std;

// Petit utilitaire pour accoler numero de ligne et nom de fichier aux messages
// d'exception
//! Utiliy macro to append file name and line number to a const char* string  
#define PExcLongMessage(a) BuildLongExceptionMessage(a,__FILE__,__LINE__)

#define SEXC_MAXMSGLEN 160
namespace SOPHYA {

//! Utility function for appending a file name and line number to a message
  string BuildLongExceptionMessage(const char * s, const char *file, int line);

  class PThrowable : public std::exception {
  public:
    //! Constructor with the message and error-id (optional) specification
    explicit PThrowable(const string& m, int ident=0) throw() ; 
    explicit PThrowable(const char* m, int ident=0) throw() ; 
    virtual ~PThrowable() throw();
    //! Implementation of std::exception what() method, returning the exception message
    virtual const char* what() const throw();
    //! Returns the associated message string
    virtual string const Msg() const; 
    //! Returns the associated error-id
    virtual int Id() const;
  private:
    char msg_[SEXC_MAXMSGLEN];
    int    id_;
  };

//  PThrowable
//      PError
//      PException
  
/*! \ingroup BaseTools 
    \brief A PError is a serious logic error. Usually not caught... */
  class PError : public PThrowable {
  public:
    explicit PError(const string& m, int id=0) throw() : PThrowable(m,id) {}
    explicit PError(const char* m, int id=0) throw() : PThrowable(m,id) {}
  };
  
/*! \ingroup BaseTools 
    \brief A PException is not as serious... Can be caught. */
  class PException : public PThrowable {
  public:
    explicit PException(const string& m, int id=0) throw() : PThrowable(m,id) {}
    explicit PException(const char* m, int id=0) throw() : PThrowable(m,id) {}
  };
  
//  ----   Errors ----
//  PError
//     AllocationError
//     NullPtrError
//     ForbiddenError
//     AssertionFailedError

/*! \ingroup BaseTools 
    \brief Memory allocation failure */
  class AllocationError : public PError {
  public:
    explicit AllocationError(const string& m, int id=0) throw() : PError(m,id) {}
    explicit AllocationError(const char* m, int id=0) throw() : PError(m,id) {}
  };
  
/*! \ingroup BaseTools 
    \brief Null pointer error */
  class NullPtrError : public PError {
  public:
    explicit NullPtrError(const string& m, int id=0) throw() : PError(m,id) {}
    explicit NullPtrError(const char* m, int id=0) throw() : PError(m,id) {}
  };
  
  
/*! \ingroup BaseTools 
    \brief Calling a forbidden method, trying a forbidden operation */
  class ForbiddenError : public PError {
  public:
    explicit ForbiddenError(const string& m, int id=0) throw() : PError(m,id) {}
    explicit ForbiddenError(const char* m, int id=0) throw() : PError(m,id) {}
  };

  
/*! \ingroup BaseTools 
    \brief ASSERT macro failure. The message is the assertion... */
  class AssertionFailedError : public PError {
  public:
    explicit AssertionFailedError(const string& m, int id=0) throw() : PError(m,id) {}
    explicit AssertionFailedError(const char* m, int id=0) throw() : PError(m,id) {}
  };
  
  // Standard exceptions
  //
  // PException
  //   IOExc
  //     FileFormatExc
  //   SzMismatchError
  //   RangeCheckError
  //   ParmError
  //   TypeMismatchExc
  //   MathExc
  //     SingMatxExc
  //   DuplicateIdExc
  //   NotFoundExc
  //   CaughtSignalExc
  
/*! \ingroup BaseTools 
    \brief Generic IO Exception */
  class IOExc : public PException {
  public:
    explicit IOExc(const string& m, int id=0) throw() : PException(m,id) {}
    explicit IOExc(const char* m, int id=0) throw() : PException(m,id) {}
  };

/*! \ingroup BaseTools 
    \brief Bad file format */
  class FileFormatExc : public IOExc {
  public:
    explicit FileFormatExc(const string& m, int id=0) throw() : IOExc(m,id) {}
    explicit FileFormatExc(const char* m, int id=0) throw() : IOExc(m,id) {}
  };

/*! \ingroup BaseTools 
    \brief Size mismatch between objects */
  class SzMismatchError : public PException {
  public:
    explicit SzMismatchError(const string& m, int id=0) throw() : PException(m,id) {}
    explicit SzMismatchError(const char* m, int id=0) throw() : PException(m,id) {}
  };
  
/*! \ingroup BaseTools 
    \brief Out of bounds for array, matrix, etc */
  class RangeCheckError : public PException {
  public:
    explicit RangeCheckError(const string& m, int id=0) throw() : PException(m,id) {}
    explicit RangeCheckError(const char* m, int id=0) throw() : PException(m,id) {}
  };
  
/*! \ingroup BaseTools 
    \brief Invalid parameter to method/constructor... */
  class ParmError : public PException {
  public:
    explicit ParmError(const string& m, int id=0) throw() : PException(m,id) {}
    explicit ParmError(const char* m, int id=0) throw() : PException(m,id) {}
  };

/*! \ingroup BaseTools 
    \brief Calling a non available / not implemented method */
  class NotAvailableOperation : public PException {
  public:
    explicit NotAvailableOperation(const string& m, int id=0) throw() : PException(m,id) {}
    explicit NotAvailableOperation(const char* m, int id=0) throw() : PException(m,id) {}
  };

/*! \ingroup BaseTools 
    \brief Bad data type -> keep ? */
  class TypeMismatchExc : public PException {
  public:
    explicit TypeMismatchExc(const string& m, int id=0) throw() : PException(m,id) {}
    explicit TypeMismatchExc(const char* m, int id=0) throw() : PException(m,id) {}
  };

/*! \ingroup BaseTools 
    \brief Math operation exception */
  class MathExc : public PException {
  public:
    explicit MathExc(const string& m, int id=0) throw() : PException(m,id) {}
    explicit MathExc(const char* m, int id=0) throw() : PException(m,id) {}
  };
  
/*! \ingroup BaseTools 
    \brief Singular matrix  */
  class SingMatrixExc : public MathExc {
  public:
    explicit SingMatrixExc(const string& m, int id=0) throw() : MathExc(m,id) {}
    explicit SingMatrixExc(const char* m, int id=0) throw() : MathExc(m,id) {}
  };

/*! \ingroup BaseTools 
    \brief Duplicate identifier during registration */
  class DuplicateIdExc : public PException {
  public:
    explicit DuplicateIdExc(const string& m, int id=0) throw() : PException(m,id) {}
    explicit DuplicateIdExc(const char* m, int id=0) throw() : PException(m,id) {}
  };
  
/*! \ingroup BaseTools 
    \brief Not found identifier  */
  class NotFoundExc : public PException {
  public:
    explicit NotFoundExc(const string& m, int id=0) throw() : PException(m,id) {}
    explicit NotFoundExc(const char* m, int id=0) throw() : PException(m,id) {}
  };
  
/*! \ingroup BaseTools 
    \brief Generated exception when processing a signal */
  class CaughtSignalExc : public PException {
  public:
    explicit CaughtSignalExc(const string& m, int id=0) throw() : PException(m,id) {}
    explicit CaughtSignalExc(const char* m, int id=0) throw() : PException(m,id) {}
 };
    
} // namespace SOPHYA


#define ASSERT(_a_) if (!(_a_)) { \
     cerr << "Assertion failed " #_a_ " file " __FILE__ " line " << __LINE__ \
          << endl; \
		     throw(AssertionFailedError(#_a_)); }
   
#define FAILNIL(_x_) \
       {if (!(_x_)) throw(NullPtrError(#_x_))}


     
#endif
