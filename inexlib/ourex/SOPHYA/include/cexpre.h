// This may look like C code, but it is really -*- C++ -*-
// Evaluateur d'expression C   -   R. Ansari 03/2004
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#ifndef CEXPREVAL_SEEN
#define CEXPREVAL_SEEN

#include "machdefs.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "pexceptions.h"

using namespace std;
namespace SOPHYA {

/*! 
  \ingroup SysTools
  \brief Exception class used by  CExpressionEvaluator
*/
class CExprException : public PException {
  public:
  explicit CExprException(const char * m) throw() : PException(m) {}
  explicit CExprException(const string& m) throw() : PException(m) {}
};

//--------------------------------------------------------------------
/*! 
  \ingroup SysTools
  \brief Base class for arithmetic expressions used by CExpressionEvaluator
  \sa CExpressionEvaluator
*/
class CExprBase {
public:
  explicit CExprBase(); 
  virtual ~CExprBase();
  virtual double Evaluate() const = 0;
  virtual bool   CheckE(string& errmsg) const { return true; }
  virtual void   CheckThrow(const char * msg) const;
  inline  void   CheckThrow(string const & msg) const { CheckThrow(msg.c_str()); }
  virtual void   Print(ostream& os) const = 0;
  static long    NbCreate() { return totnexp_create; }
  static long    NbDelete() { return totnexp_delete; }
protected:
  static long totnexp_create;
  static long totnexp_delete;
};


/*! 
  \ingroup SysTools
  \brief For formatted write (print) of expressions in a stream
*/
inline ostream& operator << (ostream& s, CExprBase const & ex)
  {  ex.Print(s);  return(s);  }

/*! 
  \ingroup SysTools
  \brief Interface class used by the CExpressionEvaluator class to represent a list of variables
  or named values accessible trough a pointer.

  Inherited class should define the \b GetVarPointer() method, and can optionaly redefine 
  the \b Update() method. 
  \sa CExpressionEvaluator
*/
class CE_VarListInterface {
public:
  //! To be called to update values corresponding to the pointers. Default implementation does nothing.
  virtual void Update() { return; } 
  //! Return a pointer to a double containing the value associated with \t name.  
  virtual double* GetVarPointer(std::string const& name) = 0;
};


class CE_BinExp;
class CE_FuncExp;
//---------------------------------------------------------
class CExpressionEvaluator : public CExprBase {
public:
  CExpressionEvaluator(const char* sexp, CE_VarListInterface* pvlist=NULL);
  CExpressionEvaluator(string const & sex, CE_VarListInterface* pvlist=NULL);

  virtual ~CExpressionEvaluator();
//! Evaluate the expression and returns the corresponding value.
  virtual double Evaluate() const;
//! Alias for Evaluate()
  inline double Value() const { return Evaluate(); }
//! Formatted output on a stream
  virtual void  Print(ostream& os) const;

protected:
//! Parse variable names or constants and return CE_NumberExp object.
  CExprBase* VarNameOrNumber(string const & s); 
//! Does the parsing and builds an CExprBase object.
  CExprBase* ParseString(int extype, string fname, string const & sex, 
		    size_t off, size_t& stop, string& errmsg);

  CExprBase * _exp;
  CE_VarListInterface* _varlist; 
};

}   // End of namespace SOPHYA

/* end of ifdef CEXPREVAL_SEEN */
#endif
