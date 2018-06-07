%{

#include "cexpr_lexer.h"

#include "cexpr_tokens"

#include <inlib/sout>
#include <inlib/sto>
#include <inlib/mathT>

#include <cstring>

//#define DEBUG

#ifdef _MSC_VER
#include <io.h>
#define isatty _isatty
#else
#include <unistd.h> //isatty
#endif

static  int yy_get_next_buffer(exlib::yacc::cexpr_lexer&);

extern int yylex(exlib::yacc::cexpr_lexer&);

#define YY_DECL int yylex(exlib::yacc::cexpr_lexer& aLexer)

#define _UNISTD_H_
#define YY_NEVER_INTERACTIVE 1

#undef YY_INPUT
#define YY_INPUT(aBuffer,aResult,aMaxSize) (aResult=aLexer.input(aBuffer,aMaxSize)) 

#define YY_NO_INPUT

#define NotFound (-1)

%}

/* signus handled in by the parser :
number [-+]?([0-9]+|(([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))([eE][-+]?[0-9]+)?)
integer [0-9]+
*/
integer ([0-9]+)([eE][+]?[0-9]+)?
hexaint 0x[0-9a-fA-F]+
number (([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))([eE][-+]?[0-9]+)?
name   [a-zA-Z_][a-zA-Z0-9_]*
string (\"[^\"\n]*\")|('[^'\n]*')

/* should be put (if any) below the below T_FALSE.
_or_ return T_OR;
_and_ return T_AND;
_gt_ return T_GT;
_ge_ return T_GE;
_lt_ return T_LT;
_le_ return T_LE;
_eq_ return T_EQ;
_ne_ return T_NE;
*/

%%


[\t ]+ /* ignore whitespace */;

{hexaint} { 
#ifdef DEBUG
  aLexer.out() << "debug : Lexer.l : integer :" << yytext << "|" << std::endl;
#endif
  unsigned int i;
  if(sscanf(yytext,"0x%x",&i)!=1) {
    aLexer.out() << "exlib::yacc::cexpr_lexer :"
        << " problem converting " << inlib::sout(yytext) 
        << " to unsigned int."
        << std::endl;
    cexpr_eval_lval.m_integer = 0;
  } else {
    cexpr_eval_lval.m_integer = i;
  }
  return NUMBER_INTEGER;
}

{integer} { 
#ifdef DEBUG
  aLexer.out() << "debug : Lexer.l : integer :" << yytext << "|" << std::endl;
#endif
  unsigned int i,j;
  if( (sscanf(yytext,"%ue%u",&i,&j)==2)  ||
      (sscanf(yytext,"%ue+%u",&i,&j)==2) ||
      (sscanf(yytext,"%uE%u",&i,&j)==2)  ||
      (sscanf(yytext,"%uE+%u",&i,&j)==2) ){
    typedef unsigned int uint_t;
    uint_t pow_10_j;
    inlib::power<uint_t,uint_t>(10,j,pow_10_j);
    cexpr_eval_lval.m_integer = i * pow_10_j;
  } else if(sscanf(yytext,"%u",&i)==1) {
    cexpr_eval_lval.m_integer = i;
  } else {
    aLexer.out() << "exlib::yacc::cexpr_lexer :"
        << " problem converting " << inlib::sout(yytext) 
        << " to long."
        << std::endl;
    cexpr_eval_lval.m_integer = 0;
  }
  return NUMBER_INTEGER;
}

{number} {
#ifdef DEBUG
  aLexer.out() << "debug : Lexer.l : number :" << yytext << "|" << std::endl;
#endif
  if(!inlib::to<double>(yytext,cexpr_eval_lval.m_number)) {
    aLexer.out() << "exlib::yacc::cexpr_lexer :"
        << " problem converting " << inlib::sout(yytext) 
        << " to double."
        << std::endl;
    cexpr_eval_lval.m_number = 0.;
  }
  return NUMBER_REAL;
}


M_PI return T_PI;
true return T_TRUE;
false return T_FALSE;

{name} {
#ifdef DEBUG
  aLexer.out() << "debug : Lexer.l : name :" << yytext << "|" << std::endl;
#endif
  aLexer.variables().push_back(yytext);
  cexpr_eval_lval.m_index = (unsigned int)(aLexer.variables().size()-1);
  return T_NAME;
}

{string} {
#ifdef DEBUG
  aLexer.out() << "debug : Lexer.l : string :" << yytext << "|" << std::endl;
#endif
  // Remove leading and trailing double quote :
  // "abcd"
  // 012345
  std::string s = yytext;
  aLexer.variables().push_back(s.substr(1,s.length()-2)); 
  cexpr_eval_lval.m_index = (unsigned int)(aLexer.variables().size()-1);
  return STRG;
}

. return yytext[0];

%%

int yywrap(){return 1;}

inline void cexpr_lexer_dummy() { //to rm g++ "unused warning".
  cexpr_lexer_unput(0,0);
}

