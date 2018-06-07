#include "sopnamsp.h"
#include "pexceptions.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

/* --- Nettoyage par Reza, Mars 2009 
// egcs ne semble pas connaitre set_new_handler (Reza 26/04/99)
// ca ne passe plus avec SGI-CC -LANG:std $CHECK$ Reza 14/02/2003
void PFailHandler(void)
{
  set_new_handler(NULL);
  cerr << "Allocation exception -- out of memory" << endl;
  throw(AllocationError("new"));
}
void InitFailNewHandler()
{
   set_new_handler(PFailHandler);
}
--- FIN nettoyage */


namespace SOPHYA {

/*!
  \ingroup BaseTools
  \brief Utility function for appending a file name and line number to a message

  In practice, the macro \b PExcLongMessage(a) can be used to append file name 
  and line number to the message a.
*/

string BuildLongExceptionMessage(const char * s, const char *file, int line)
{
  char buff[32];
  sprintf(buff," Line=%d", line);
  string rs=s;
  rs += " File="; rs += file; rs += buff;
  return(rs);
}

/*! 
    \class PThrowable
    \ingroup BaseTools
    \brief Base exception class in Sophya, inherits from std::exception

    This class is the ancestor class for PError and PException classes which are 
    the base classes for all exceptions used in SOPHYA. The PThrowable class inherits
    from the standard c++ exception base class, std::exception and implements the what()
    method. A message (string/char*) passed to the constructor is kept in the exception object, 
    and can be retrieved  using the \b what() method or \b Msg() method. An integer identifier 
    can also be associated with the exception objet and retrieved by \b Id().
    The message passed to the constructor is truncated to a maximum length defined by 
    SEXC_MAXMSGLEN (=160 characters).
*/

PThrowable::PThrowable(const string& m, int ident) throw() 
{
  id_ = ident;
  strncpy(msg_, m.c_str(), SEXC_MAXMSGLEN-1);
  msg_[SEXC_MAXMSGLEN-1] = '\0';
}

PThrowable::PThrowable(const char* m, int ident) throw()
{
  id_ = ident;
  if (m!=NULL) { 
    strncpy(msg_, m, SEXC_MAXMSGLEN-1);
    msg_[SEXC_MAXMSGLEN-1] = '\0';
  }
  else msg_[0] = '\0';
}

PThrowable::~PThrowable() throw()
{
}
const char* PThrowable::what() const throw()
{
  return msg_;
}

string const PThrowable::Msg() const 
{
  return (string(msg_));
}

int PThrowable::Id() const
{
  return id_;
}

} // namespace SOPHYA

