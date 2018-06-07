#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "sopnamsp.h"
#include "cexpre.h"
#include <stack>
#include <math.h>
#include "srandgen.h"

namespace SOPHYA {

long CExprBase::totnexp_create = 0;
long CExprBase::totnexp_delete = 0;
CExprBase::CExprBase()
{
  totnexp_create++;
  // cout << "CExprBase::CExprBase()-Creation " << hex << this << dec << endl;
}

CExprBase::~CExprBase()
{
  totnexp_delete++;
  // cout << "CExprBase::~CExprBase() " << hex << this << dec << endl;
}

void CExprBase::CheckThrow(const char * msg) const
{
  string errmsg;
  bool ok = CheckE(errmsg);
  if (ok) return;
  else {
    errmsg += msg;
    throw CExprException(errmsg);
  }
}

//---------------------------------------------------------
/*! 
  \internal 
  \ingroup SysTools
  \brief Class representing numbers for expression evaluation 
  \sa CExpressionEvaluator
*/
class CE_NumberExp : public CExprBase {
public:
  CE_NumberExp(double v) { _val = v; _pval=&_val; }
  CE_NumberExp(double* pv) { _pval=pv; _val = *_pval; }
  CE_NumberExp(string const & s);
  virtual ~CE_NumberExp() {}
  virtual double Evaluate() const { return (*_pval); }  
  virtual void  Print(ostream& os) const { os << (*_pval); }
protected:
  double _val;
  double* _pval; 
};

CE_NumberExp::CE_NumberExp(string const & s)
{
  _pval=&_val; 
  size_t l = s.length();
  if (l < 1) { 
    string errmsg = "CE_NumberExp::CE_NumberExp() Empty expression";
    throw CExprException(errmsg);
  }
  if (s == "Pi") _val = M_PI;
  else if (s == "E") _val = M_E;
  else {
    string errmsg = "CE_NumberExp::CE_NumberExp() Bad numerical constant or unknown variable: ";
    errmsg += s;
    
    char* lcc;
    _val = strtod(s.c_str(), &lcc);
    if (*lcc != '\0')  throw CExprException(errmsg);
  }
}

//---------------------------------------------------------
//-----------  Expression changement de signe -------------
/*! 
  \internal 
  \ingroup SysTools
  \brief Class for handling change sign expression (-x)
  \sa CExpressionEvaluator
*/
class CE_ChsExp : public CExprBase {
public:
  explicit CE_ChsExp(CExprBase * e) { _e = e; } 
  virtual  ~CE_ChsExp() { if (_e) delete _e; } 
  virtual double Evaluate() const { return -_e->Evaluate(); }  
  virtual bool   CheckE(string& errmsg) const { if (_e) return true; else return false; }
  virtual void  Print(ostream& os) const { os << "-( "; _e->Print(os); os << " )"; }
 protected:
  CExprBase * _e;
};

//---------------------------------------------------------
/*! 
  \internal 
  \ingroup SysTools
  \brief Base class for binary operations on expressions
  \sa CExpressionEvaluator
*/
class CE_BinExp : public CExprBase {
public:
  explicit CE_BinExp(char op, int prior); 
  virtual ~CE_BinExp() { if (_e1) delete _e1; if (_e2) delete _e2; }
  inline void SetE1(CExprBase * e) { _e1 = e; } 
  inline void SetE2(CExprBase * e) { _e2 = e; }
  virtual bool CheckE(string& errmsg) const; 
  virtual void Print(ostream& os) const;
  inline char BOpC() const { return _bop; } 
  inline int  Priority() const { return _priority; } 
 protected:
  CExprBase * _e1;
  CExprBase * _e2;
  char _bop;     // Caractere identificateur d'operation (+,-,*,/ ...)
  int _priority; // Niveau de priorite: 1= +,-  2: *,/  3: ^...
};

CE_BinExp::CE_BinExp(char op, int prior)
{
  _e1 = NULL; _e2 = NULL; 
  _bop = op;
  _priority = prior;
}

bool CE_BinExp::CheckE(string& errmsg) const 
{	
  if ((_e1 == NULL) || (_e2 == NULL)) {
    errmsg += "CE_BinExp::CheckE: Binary expression-missing argument ";
    return false;
  }
  return true;
}

void CE_BinExp::Print(ostream& os) const
{ 
  os << "(" << (*_e1) << _bop << (*_e2) << ")" ; 
}

//---------------------------------------------------------
/*! 
  \internal 
  \ingroup SysTools
  \brief Addition of two CExprBase (binary operation : x+y)
  \sa CExpressionEvaluator
*/
class CE_AddExp : public CE_BinExp {
public:
  CE_AddExp() : CE_BinExp('+',1) {}
  virtual double Evaluate() const { CheckThrow("CE_AddExp::Evaluate"); return _e1->Evaluate()+_e2->Evaluate();
 }
};
/*! 
  \internal 
  \ingroup SysTools
  \brief Multiplication of two CExprBase (binary operation : x*y)
  \sa CExpressionEvaluator
*/
class CE_MulExp : public CE_BinExp {
public:
  CE_MulExp() : CE_BinExp('*',2) {}
  virtual double Evaluate() const { CheckThrow("CE_MulExp::Evaluate"); return _e1->Evaluate()*_e2->Evaluate(); }
};
/*! 
  \internal 
  \ingroup SysTools
  \brief Subtraction of two CExprBase (binary operation : x-y)
  \sa CExpressionEvaluator
*/
class CE_SubExp : public CE_BinExp {
public:
  CE_SubExp() : CE_BinExp('-',1) {}
  virtual double Evaluate() const { CheckThrow("CE_SubExp::Evaluate"); return _e1->Evaluate()-_e2->Evaluate(); }
};
/*! 
  \internal 
  \ingroup SysTools
  \brief Division of two CExprBase (binary operation : x/y)
  \sa CExpressionEvaluator
*/
class CE_DivExp : public CE_BinExp {
public:
  CE_DivExp() : CE_BinExp('/',2) {}
  virtual double Evaluate() const { CheckThrow("CE_DivExp::Evaluate"); return _e1->Evaluate()/_e2->Evaluate(); }
};

//---------------------------------------------------------
typedef double (* f0exp) ();
typedef double (* f1exp) (double x);
typedef double (* f2exp) (double x, double y);
typedef double (* f3exp) (double x, double y, double z);

//---------------------------------------------------------
#define FMXARG 3
/*! 
  \internal 
  \ingroup SysTools
  \brief Function evaluation on other expression in the form f(), f(x), f(x,y), f(x,y,z)
  \sa CExpressionEvaluator
*/
class CE_FuncExp : public CExprBase {
public:
  CE_FuncExp(string const & func);
  virtual ~CE_FuncExp(); 
  virtual double Evaluate() const ;
  inline void AddArg(CExprBase * e);
  inline int  GetMaxNArg() const { return _maxne; }
  virtual bool CheckE(string& errmsg) const; 
  virtual void Print(ostream& os) const;
protected:
  f0exp _f0; 
  f1exp _f1; 
  f2exp _f2; 
  f3exp _f3; 
  int _ne, _maxne;
  CExprBase * _e[FMXARG];
  string _fname;
};

// Les fonctions de generation de nombre aleatoire
static double _CE_rand01() { return drand01(); }
static double _CE_randpm1() { return drandpm1(); }
static double _CE_gaurand() { return GaussianRand(1.,0.); }
//---------------------------------------------------------
CE_FuncExp::CE_FuncExp(string const & func)
{
  _ne = 0; _maxne = -1;
  for(int k=0; k<FMXARG; k++)  _e[k] = NULL;
  _f0 = NULL;
  _f1 = NULL;
  _f2 = NULL;
  _f3 = NULL;
  if (func == "sin") _f1 = sin;
  else if (func == "cos") _f1 = cos;
  else if (func == "tan") _f1 = tan;
  else if (func == "asin") _f1 = asin;
  else if (func == "acos") _f1 = acos;
  else if (func == "atan") _f1 = atan;
  else if (func == "atan2") _f2 = atan2;
  else if (func == "sqrt") _f1 = sqrt;
  else if (func == "fabs") _f1 = fabs;
  else if (func == "floor") _f1 = floor;
  else if (func == "exp") _f1 = exp;
  else if (func == "log") _f1 = log;
  else if (func == "log10") _f1 = log10;
  else if (func == "pow") _f2 = pow;
  else if (func == "hypot") _f2 = hypot;
  else if (func == "sinh") _f1 = sinh;
  else if (func == "cosh") _f1 = cosh;
  else if (func == "tanh") _f1 = tanh;
// Les fonctions aleatoires declaree plus haut 
  else if (func == "rand01") _f0 = _CE_rand01;	
  else if (func == "randpm1") _f0 = _CE_randpm1;	
  else if (func == "gaurand") _f0 = _CE_gaurand;	
  else { 
    string errmsg = "CE_FuncExp::CE_FuncExp() - Uknown function " ;
    errmsg += func;
    throw CExprException(errmsg);
  } 
  if (_f0)  _maxne = 0;
  else if (_f1)  _maxne = 1;
  else if (_f2) _maxne = 2;
  else if (_f3) _maxne = 3;
  _fname = func;
}


CE_FuncExp::~CE_FuncExp()
{  
for(int k=0; k<FMXARG; k++)  if (_e[k]) delete _e[k];
}

void CE_FuncExp::AddArg(CExprBase * e)
{
  if ((e != NULL) && (_ne < _maxne)) { _e[_ne] = e; _ne++; }
  else  throw CExprException("CE_FuncExp::AddArg() e=NULL or too many arguments ");
}

double CE_FuncExp::Evaluate() const 
{
  if ((_ne != _maxne) || (_maxne < 0) ) {
    throw CExprException("CE_FuncExp::Evaluate() - Wrong argument number ");
  }
  if (_ne == 0) return _f0();
  else if (_ne == 1) return _f1(_e[0]->Evaluate());
  else if (_ne == 2) return _f2(_e[0]->Evaluate(), _e[1]->Evaluate());
  else if (_ne == 3) return _f3(_e[0]->Evaluate(), _e[1]->Evaluate(), 
				_e[2]->Evaluate());
  else return 0.;
}

bool CE_FuncExp::CheckE(string& errmsg) const
{	
  if ((_ne != _maxne) || (_maxne < 0) ) {
    char buff[128];
    sprintf(buff, "CE_FuncExp::CheckE() %s - Wrong argument number - ne=%d maxne=%d ", 
	    _fname.c_str(), _ne, _maxne);
    errmsg += buff;
    return false;
  }
  return true;
}

void CE_FuncExp::Print(ostream& os) const
{ 
  if ((_ne != _maxne) || (_maxne < 1) ) 
    os << _fname << "(ArgError)" ;
  else {
    if (_ne == 1) os << _fname << "(" << *(_e[0]) << ")";
    else if (_ne == 2) os << _fname << "(" << *(_e[0]) << "," << *(_e[1]) << ")";
    else if (_ne == 3) os << _fname << "(" << *(_e[0]) 
			  << "," << *(_e[1]) << "," << *(_e[2]) << ")";
  }
}

//---------------------------------------------------------
/*!
  \class CExpressionEvaluator
  \ingroup SysTools
  \brief Class for evaluation of arithmetic expressions with C-like syntax.

  This classes can handle the parsing of c-like arithmetic
  expressions containing numerical constants, the four 
  basic operations (addition +, subtraction -, multiplication *,
  division /) and functions.
  The numerical constants are also defined <tt> (Pi = M_PI E = M_E) </tt>.
  The following functions from the standard math library and 
  random generators are available:
  
  - sqrt fabs floor hypot
  - exp log log10 pow 
  - sin cos tan asin acos atan atan2
  - sinh cosh tanh
  - rand01() : Flat random number generator in the range [0 1]
  - randpm1() : Flat random number generator in the range [-1 1]
  - gaurand() : Gaussian random number generator (m=0, sigma=1)

  Usage example : 
  \code
  #include "cexpre.h"
  ...
  string es = "4.+3*(cos(Pi/8.)+1.5)";
  CExpressionEvaluator e(es);
  cout << " Expression: " << e << " = " << e.Evaluate() << endl;
  \endcode

  Output : \n
  <tt>  Expression: (4+(3*(cos((3.14159/8))+1.5))) = 11.2716 </tt>
*/

/*! 
  Parse the character string \c exp and builds an expression. Uses the given list of variables 
  \c varlist if a valid pointer is provided. Can throw CExprException exception.
*/
CExpressionEvaluator::CExpressionEvaluator(const char* exp, CE_VarListInterface* varlist)
{
  _exp = NULL;
  _varlist = varlist;
  size_t off=0,stop=0;
  string fname = "";
  string errmsg;
  string sexp=exp;
  _exp=  ParseString(0,fname,sexp,off,stop,errmsg);
  if (_exp == NULL)  throw CExprException(errmsg);
}

/*! 
  Parse the string \c sex and builds an expression. Uses the given list of variables 
  \c varlist if a valid pointer is provided. Can throw CExprException exception.
*/
CExpressionEvaluator::CExpressionEvaluator(string const & sex, CE_VarListInterface* varlist)
{
  _exp = NULL;
  _varlist = varlist;
  size_t off=0,stop=0;
  string fname = "";
  string errmsg;
  _exp=  ParseString(0,fname,sex,off,stop,errmsg);
  if (_exp == NULL)  throw CExprException(errmsg);
}

CExpressionEvaluator::~CExpressionEvaluator()
{
  if (_exp) delete _exp;
}

double CExpressionEvaluator::Evaluate() const
{
  if (_exp) return _exp->Evaluate();
  else return 0.;
}

void CExpressionEvaluator::Print(ostream& os) const
{
  if (_exp) _exp->Print(os);
  else os << "CExpressionEvaluator ???";
}

// cette fonction rearrange le stack des operations binaires en attente
static CExprBase* Arrange_CE_BinExpStack(stack<CE_BinExp* >& sbx, CExprBase* cex, CE_BinExp* nbx)
{
  while ( !sbx.empty() && (nbx->Priority() <= sbx.top()->Priority()) ) {
    sbx.top()->SetE2(cex);
    cex = sbx.top();  sbx.pop();
  }
  nbx->SetE1(cex);
  sbx.push(nbx);
  return NULL;
}

// cette fonction rearrange le stack des operations binaires en attente
static CExprBase* Arrange_CE_BinExpStack(stack<CE_BinExp* >& sbx, CExprBase* cex)
{
  if (sbx.empty())  return cex;
  while ( !sbx.empty() ) {
    sbx.top()->SetE2(cex);
    cex = sbx.top();  sbx.pop();
  }
  return cex;
}

CExprBase* CExpressionEvaluator::VarNameOrNumber(string const & s)
{
  if (_varlist==NULL)  return new CE_NumberExp(s); 
  double* pv=_varlist->GetVarPointer(s);
  if (pv!=NULL) return new CE_NumberExp(pv); 
  return new CE_NumberExp(s); 
}

CExprBase* CExpressionEvaluator::ParseString(int extype, string fname, string const & sex, 
					size_t off, size_t& stop, string& errmsg)
{
  size_t len = sex.length();
  if (len < 1) { 
    string errmsg = "CExpressionEvaluator::ParseString() Empty expression";
    throw CExprException(errmsg);
  }
  errmsg = "";
  CExprBase* rx = NULL;   // Expression resultat
  stack< CE_BinExp* > sbx;  // Stack des expressions binaires
  CE_FuncExp* fx = NULL;    // Expression fonction 
  CExprBase* cx = NULL;       // Element d'expression (courante)

  if (extype == 2)  fx = new CE_FuncExp(fname);

  size_t p=0, q=0;
  char lastopc=0, opc=0; // Last/current operation sign (+,-,*,/,...)
  int osn = 0;
  bool finok = false;
  bool fgcont = true;
  bool checkok = true;
  bool apperrmsg = true;
  //  cout << " DBG-ParseString off= " << off << " sex[off]= " << sex[off] 
  //	<< " extype= " << extype << endl;
  p = stop = off;  
  while ((p < len) && (fgcont) && (checkok) ) { 
    // cout << " DBG-2-ParseString p=" << p << " q=" << q << endl;

    cx = NULL;
    // ------------------------------- Bretelle ------
    // Il faut sauter les signes +/- qui se trouverait dans les
    // exposants des nombres xxxe-yy xxxe+y : 0.4e-2 ...
    // Les qques lignes sont une bretelle qui saute ces signes +/- (Reza, Aout 2004) 
    // On verifie d'abord s'il y a un nombre valide au debut de la positions courante
    char* lcc;
    size_t pfn=0,psn;
    bool numberok = false;
    strtod(sex.c_str()+p, &lcc);
    if (lcc != sex.c_str()+p) {  // Expression valide de double au debut 
      numberok = true;           // de la position courante
      pfn = lcc-sex.c_str();
    }
    // On cherche le 1er signe d'operateur - apres le nombre de debut 
    q = sex.find_first_of("+-*/(),",p);

    while ( numberok && (q < len) && (q < pfn) && (q > p) && 
            ((sex[q] == '+') || (sex[q] == '-')) ) {
      psn = q+1; 
      if (psn < len) q = sex.find_first_of("+-*/(),",psn);
      else q = len+1;
    }
    //---- Fin de la Bretelle ---- pour sauter les signes +/- de l'exposant des nombres

    if (q < len) {  // operateur trouve
      opc = sex[q];  // signe operateur courant
      switch (opc) {
        case '(' : 
	  if (q == p) { 
	    string fname = "";
	    cx = ParseString(1, fname, sex, q+1, stop, errmsg);
	  }
	  else cx = ParseString(2, sex.substr(p,q-p), sex, q+1, stop, errmsg);
	  if (!cx)  { checkok = false; p = stop+1; apperrmsg = false; }
	  else { 
	    if (osn == 1) {
              if (sex[p-osn] == '-')  cx = new CE_ChsExp(cx);
            }
	    if (osn > 1) {
	      checkok = false; p = stop+1; fgcont = false; 
	      delete cx;
	      errmsg = "CExpressionEvaluator::ParseString()/ Syntax Error - multiple chs (+/-) (AA)"; 
            }
	    else if (rx) { 
	      checkok = false; p = stop+1; fgcont = false; 
	      delete cx;
	      errmsg = "CExpressionEvaluator::ParseString()/ Syntax Error - rx&&cx (A)"; 
	    }
	    else {
	      if (stop == len-1)  { 
		rx = Arrange_CE_BinExpStack(sbx, cx);
		finok = true;
	      }
	      else rx = cx;
	      p = stop+1; osn = 0; lastopc = opc; 
	    }
	  }
	  break;

        case ')' : 
        case ',' :
	  if (extype == 0) {
	    checkok = false; p = q;  fgcont = false; 
	    errmsg = "CExpressionEvaluator::ParseString() Unexpected ) or ,";
	  }
          if ( (extype == 1) && (opc == ',')) {
	    checkok = false; p = q;  fgcont = false; 
	    errmsg = "CExpressionEvaluator::ParseString() Unexpected ,";
	  }
	  if ((q > p) && (rx)) {
	    checkok = false; p = q;  fgcont = false; 
	    errmsg = "CExpressionEvaluator::ParseString()/ Syntax Error - rx&&cx (B)"; 
	  }
	  if (q > p) cx = VarNameOrNumber(sex.substr(p-osn,q-p+osn));
	  else cx = rx;
	  
	  if (!cx && ( !fx || ( opc != ')' ) || 
                       ( (fx && (fx->GetMaxNArg() > 0) && (opc == ')' ) ) ) ) ) {
	    checkok = false; p = q;  fgcont = false; 
	    errmsg = "CExpressionEvaluator::ParseString()/ Syntax Error - farg=NULL (BB)"; 
	  }
	  else {
	    if (cx) { rx = Arrange_CE_BinExpStack(sbx, cx);
	      if (extype == 2) {
	        //	    cout << " DBG-ParseString-AddArg " << *(cx) << endl;
	        fx->AddArg(rx);  rx = NULL;
	      }
            }
	    if (opc == ')') { // Signe de fin de traitement d'une portion d'expression
	      if (extype == 2) { rx = fx; fx = NULL; }
	      stop = q;  osn = 0; lastopc = opc;
	      fgcont = false;
	      finok = true;
	    }
	    else {
	      if (q == (len-1)) finok = true;
	      p = q+1; osn = 0; lastopc = opc; 
	    }
	  }
	  break;

        case '+' :
        case '-' :
        case '*' :
        case '/' :
	  if (!( ((opc=='+')||(opc=='-')) && ( (q==off) || ( (q==p) && lastopc != '(') ) ) ) {
	    CE_BinExp* nbx;
	    if (opc == '+')  nbx = new CE_AddExp;  
	    else if (opc == '-')  nbx = new CE_SubExp;   
	    else if (opc == '*')  nbx = new CE_MulExp;   
	    else  nbx =  new CE_DivExp;  
	    if ((p == q) && (rx == NULL)) {
	      checkok = false; p = q;  osn = 0; fgcont = false; 
	      delete nbx;
	      errmsg = "CExpressionEvaluator::ParseString() Syntax Error - rx==NULL (C)";
	    }
	    else {
	      if (p == q)  cx = rx;
	      else cx = VarNameOrNumber(sex.substr(p-osn,q-p+osn));
	      rx = Arrange_CE_BinExpStack(sbx, cx, nbx);
	      p = q+1;  osn = 0; lastopc = opc;
	    }
	  }
	  else {
	  // Traitement des signes +/- qui pourrait faire partie d'une constante numerique
	    p = q+1;  osn++;
	    continue;
	  }
	  break;
        default:
	  throw CExprException("CExpressionEvaluator::ParseString() BUG-BUG-BUG");
	  break;
      }  // Fin de switch 
    }
    else {  // dernier element
      if ( (p<len) && (rx != NULL)) {
	checkok = false; p = len-1; fgcont = false; 
	errmsg = "CExpressionEvaluator::ParseString() Syntax Error - rx!=NULL at end (D)";
      }
      else { 
	if (p<len)  cx = VarNameOrNumber(sex.substr(p-osn));
	else cx = rx;
	rx = Arrange_CE_BinExpStack(sbx, cx);
	stop = p = len;  fgcont = false;      
	finok = true;  
      }
    }
  }
  //  cout << " DBG-ParseString-out stop= " << stop << " sex[stop]= " << sex[stop] << endl; 
  if (finok && checkok) { 
    if( !sbx.empty() ) { 
      checkok = false; 
      errmsg = "CExpressionEvaluator::ParseString() !sbx.empty() at the end (E)";
    }
    else  { 
      checkok = rx->CheckE(errmsg);
    }
  }
  if (!finok || !checkok || !rx) {
    char buff[128];
    if (errmsg.length() < 1) {
      errmsg = "CExpressionEvaluator::ParseString() Error - " ;
      if (!finok) errmsg += " !finok " ;
      if (!checkok) errmsg += " !checkok " ;
      if (!rx)  errmsg += " !rx " ;
      errmsg += " (F)";
    }    

    if (apperrmsg) {
      if (p<len) 
        sprintf(buff,"\n CExprError... %s <> %s ", sex.substr(0,p).c_str(), 
	        sex.substr(p).c_str());
      else  sprintf(buff,"\n CExprError... %s <<", sex.c_str());
      errmsg += buff;
    }
    if (rx) delete rx;
    rx = NULL;
    while (!sbx.empty()) {
      delete sbx.top();
      sbx.pop();
    }
  }
  if (fx) delete fx; 
  return rx;
}

}   // End of namespace SOPHYA

