#include "sopnamsp.h"
#include "rpneval.h"
#include <stdlib.h>
#include <stdio.h>
#include "strutilxx.h"
#include "srandgen.h"
#include <iostream>
#include <math.h>

namespace SOPHYA {

/*!
  \class RPNExpressionEvaluator
  \ingroup SysTools
  Arithmetic expression (double precision float) evaluator 
  in Reverse Polish Notation (RPN). This is an HP calculator 
  like syntax. Spaces are used for separating the string 
  expression into tokens. \n
  The string parsed by RPNExpressionEvaluator should be 
  formed by a set of space separated words. Each word may be
  a numerical constant or operation or function. 
  All numeriacl constants are pushed to stack top. 
  The stack is limited only
  by the available memory. The three numbers on the stack top
  are referred to as <tt> x y z </tt>. \n
  Available operations: 
  - op= + - * / % : replace (x,y) by x.op.y 
  - e pi : M_PI , M_E numerical constants
  - f= sin cos tan asin acos atan : replace x by f(x)
  - f= chs sqrt sq : (x<-f(x)) change-sign , square root and square (x<-x^2) operations 
  - f= log log10 exp : replace x by f(x) 
  - f= fabs floor ceiling : replace x by f(x) 
  - f= deg2rad rad2deg : replace x by f(x) 
  - f= rand01 randpm1 gaurand : pushes a random number on the stack top ([0 1] [-1 1] Gaussian)
  - print x<>y pop push : stack operations
  - sum product : Replace the complete stack by the sum / product of the numbers in the stack
  - mean sigma : Replace the complete stack by the mean / sigma of the numbers in the stack

  \sa CExpressionEvaluator Commander

  The following output is produced by the sample code below:
  \code
  #include "rpneval.h"
  ...
  RPNExpressionEvaluator rpn1("4 2 print + 3 * ");
  cout << "RPN1: 4 2 + 3 * -> rpn1.Value() = " << rpn1.Value() << endl;  
  RPNExpressionEvaluator rpn2("1 2 3 4 5 sum");
  cout << "RPN2: 1 2 3 4 5 sum -> rpn2.Value() = " << rpn2.Value() << endl;
  \endcode

  Output:
  \verbatim
  RPNExpressionEvaluator::PrintStack() Size()= 2
      0:  2   (x) 
      1:  4   (y) 
  RPN1: 4 2 + 3 * -> rpn1.Value() = 18
  RPN2: 1 2 3 4 5 sum -> rpn2.Value() = 15  
  \endverbatim
*/

/*!
  \brief Parses the string \b sex into words and perform the specified operations on the stack.

  Can throw  RPNExprException 
*/
RPNExpressionEvaluator::RPNExpressionEvaluator(string const & sex)
{
  vector<string> exe;
  FillVStringFrString(sex, exe, ' ');
  int rc = EvalRPNExpr(exe, 0);
  if (rc < exe.size()) { 
    string msg = "RPNExpressionEvaluator() - syntax error near ";
    msg += exe[rc];
    char buff[32];
    sprintf(buff," (word %d)",rc);
    msg += buff;
    throw RPNExprException(msg);
  }
}

/*! 
  \brief Perform the operations specified by \b on the stack, starting from element \b exe[off].  

  Can throw  RPNExprException 
*/
RPNExpressionEvaluator::RPNExpressionEvaluator(vector<string> & exe, int off)
{
  int rc = EvalRPNExpr(exe, off);
  if (rc < exe.size()) { 
    string msg = "RPNExpressionEvaluator() - syntax error near ";
    msg += exe[rc];
    char buff[32];
    sprintf(buff," (word %d)",rc);
    msg += buff;
    throw RPNExprException(msg);
  }
}

RPNExpressionEvaluator::~RPNExpressionEvaluator()
{
}

/* Operations sur le stack RPN */
/* --Methode-- */
//! Return the stack top (x) 
double RPNExpressionEvaluator::Evaluate() const
{
  double x;
  if ( CheckStack( x) ) 
    throw RPNExprException("RPNExpressionEvaluator::Evaluate() EmptyStack");
  else return x;
}
  
/* --Methode-- */
int RPNExpressionEvaluator::EvalRPNExpr(vector<string> & args, int off)
{
  
  if (args.size() <= off)  return 1;
  double x,y;
  x = y = 0.;

  for(int k=off; k<args.size(); k++) {
    // Les 4 operations de base + - * / 
    if (args[k] == "+") { 
      if ( CheckStack( x, y) ) return k;
      rpnstack_.top() = y+x;
    }
    else if (args[k] == "-") { 
      if ( CheckStack( x, y) ) return k;
      rpnstack_.top() = y-x;
    }
    else if (args[k] == "*") { 
      if ( CheckStack( x, y) ) return k;
      rpnstack_.top() = y*x;
    }
    else if (args[k] == "/") { 
      if ( CheckStack( x, y) ) return k;
      rpnstack_.top() = y/x;
    }
    else if (args[k] == "%") { 
      if ( CheckStack( x, y) ) return k;
      rpnstack_.top() = (int)y % (int)x;
    }
    // Les constantes : e , pi
    else if (args[k] == "e") {
      rpnstack_.push(M_E);
    }
    else if (args[k] == "pi") {
      rpnstack_.push(M_PI);
    }
    // Les fonctions usuelles a 1 argument f(x)
    else if (args[k] == "cos") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = cos(x);
    }
    else if (args[k] == "sin") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = sin(x);
    }
    else if (args[k] == "tan") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = tan(x);
    }
    else if (args[k] == "acos") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = acos(x);
    }
    else if (args[k] == "asin") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = asin(x);
    }
    else if (args[k] == "atan") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = atan(x);
    }
    else if (args[k] == "chs") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = -x;
    }
    else if (args[k] == "sqrt") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = sqrt(x);
    }
    else if (args[k] == "sq") {  // x^2 
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = x*x;
    }
    else if (args[k] == "log") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = log(x);
    }
    else if (args[k] == "log10") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = log10(x);
    }
    else if (args[k] == "exp") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = exp(x);
    }
    else if (args[k] == "fabs") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = fabs(x);
    }
    else if (args[k] == "floor") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = floor(x);
    }
    else if (args[k] == "ceil") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = ceil(x);
    }
    // trunc et nint vire - ca ne compile pas sous linux - Reza 01/2003
    else if (args[k] == "deg2rad") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = x*M_PI/180.;
    }
    else if (args[k] == "rad2deg") {
      if ( CheckStack( x) ) return k;
      rpnstack_.top() = x*180./M_PI;
    }
    // Les fonctions usuelles a 2 argument f(x,y)
    else if (args[k] == "pow") { 
      if ( CheckStack( x, y) ) return k;
      rpnstack_.top() = pow(y,x);
    }
    else if (args[k] == "atan2") { 
      if ( CheckStack( x, y) ) return k;
      rpnstack_.top() = atan2(x,y);
    }
    // generateur aleatoire
    else if (args[k] == "rand01") { 
      double rnd = drand01();
      rpnstack_.push(rnd);
    }
    else if (args[k] == "randpm1") { 
      double rnd = drandpm1();
      rpnstack_.push(rnd);
    }
    else if (args[k] == "gaurand") { 
      double rnd = GaussianRand(1.,0.);
      rpnstack_.push(rnd);
    }
    // Fonction a N arguments  - Somme, produit, etc ...
    else if ((args[k] == "sum") || (args[k] == "mean") || (args[k] == "sigmean") || 
	     (args[k] == "sigma") || (args[k] == "sigma2") ) {
      double sx, sx2;
      int nn = SumStack( sx, sx2);
      if (args[k] == "sum") rpnstack_.push(sx);
      else { 
	if (nn == 0) return 1;
	double fnn = nn;
	double mean = sx/fnn;
	if (args[k] == "sigma2")  rpnstack_.push(sx2/fnn-mean*mean);
	else {
	  if ((args[k] == "sigma") || (args[k] == "sigmean")) 
	    rpnstack_.push(sqrt(sx2/fnn-mean*mean));
	  if ((args[k] == "mean") || (args[k] == "sigmean"))  rpnstack_.push(mean);
	}
      }
    }
    else if (args[k] == "product") { 
      double px;
      int nn = ProductStack( px);
      if (nn == 0) return k;
      rpnstack_.push(px);
    }
    // Fonctions de manipulation de stack
    else if (args[k] == "print") { 
      PrintStack();
    }
    else if (args[k] == "x<>y") {
      if ( CheckStack( x, y) ) return k;
      rpnstack_.top() = x;  rpnstack_.push(y);
    }
    else if (args[k] == "pop") {
      rpnstack_.pop();
    }
    else if (args[k] == "push") {
      if (rpnstack_.empty()) rpnstack_.push(0.);
      else rpnstack_.push(rpnstack_.top());
    }
    // On met un nombre sur le stack 
    else {
      char * esptr;
      x = strtod(args[k].c_str(), &esptr);
      //      if (ctof(args[k].c_str(),&x) < 0) {
      if (esptr == args[k].c_str()) return k; 
      rpnstack_.push(x);
    }

  }
  return(args.size()+1);
}

inline void RPNExpressionEvaluator::PrintStack()
{
  if (rpnstack_.empty()) 
    cout << "RPNExpressionEvaluator::PrintStack() Empty stack " << endl;
  else {
    stack<double> s;
    s = rpnstack_;
    int k = 0;
    cout << "RPNExpressionEvaluator::PrintStack() Size()= " << s.size() << endl;
    while( !s.empty() ) {
      cout << "    " << k << ":  " << s.top() << "  ";
      if (k == 0)  cout << " (x) " << endl;
      else if (k == 1) cout << " (y) " << endl;
      else if (k == 2) cout << " (z) " << endl;
      else cout << endl;
      s.pop(); k++;
    }
  }
  
} 

int RPNExpressionEvaluator::SumStack(double& sx, double& sx2) 
{
  sx = sx2 = 0.;
  int nn = 0;
  double x = 0.;
  while( !rpnstack_.empty() ) {
    x = rpnstack_.top(); rpnstack_.pop();
    sx += x; sx2 += x*x;
    nn++;
  } 
  return(nn);  
}

int RPNExpressionEvaluator::ProductStack(double& px) 
{
  px = 1.;
  int nn = 0;
  double x = 0.;
  while( !rpnstack_.empty() ) {
    x = rpnstack_.top(); rpnstack_.pop();
    px *= x;  nn++;
  } 
  return(nn);  
}


} // End of namespace SOPHYA
