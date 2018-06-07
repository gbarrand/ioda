%{

#include "cexpr_eval.h"

#include "cexpr_lexer.h"

#include <inlib/vmanip>

extern int yylex(exlib::yacc::cexpr_lexer&);
static void yyerror(exlib::yacc::cexpr_eval&,const char*); 

#define NotFound (-1)

//#define DEBUG

%}

%union {
  unsigned int m_index;
  unsigned int m_integer;
  double m_number;
  inlib::valop* m_expression;
}

%token <m_index> STRG
%token <m_index> T_NAME
%token <m_integer> NUMBER_INTEGER
%token <m_number> NUMBER_REAL 

%token T_PI
%token T_TRUE T_FALSE
// for strings in XML.
%token T_AND T_OR
 //%token T_GT T_GE T_LT T_LE T_EQ T_NE

%type <m_expression> comparisons comparison expression 

%left '-' '+'
%left '/' '*'
%nonassoc UMINUS

%%

result 
: error {
  // To reset lex and yacc in case of error.
} | comparisons {
  if(aParser.find_names()||aParser.find_functions()) {
    aParser.set_result(0);
  } else {
    aParser.set_result($1);
    aParser.stack().pop_back();
  }
} | expression {
  if(aParser.find_names()||aParser.find_functions()) {
    aParser.set_result(0);
  } else {
    aParser.set_result($1);
    aParser.stack().pop_back();
  }
};

comparisons 
: '!' comparisons {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : ! cps" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::NOT,$2);
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | comparisons '&' '&' comparisons {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : cps && cps" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_AND,$1,$4);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | comparisons T_AND comparisons {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : cps && cps" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_AND,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | comparisons '|' '|' comparisons {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : cps || cps" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_OR,$1,$4);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | comparisons T_OR comparisons {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : cps || cps" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_OR,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | '(' comparisons ')'  {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : (cps)" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::ASSIGN,$2);
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | comparison {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : cp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::ASSIGN,$1);
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
};

comparison 
: expression '>' expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp > exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_GT,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | expression '>' '=' expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp >= exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_GE,$1,$4);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | expression '<' expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp < exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_LT,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | expression '<' '=' expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp <= exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_LE,$1,$4);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | expression '=' '=' expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp == exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_EQ,$1,$4);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | expression '!' '=' expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp != exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_NE,$1,$4);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
  /*
} | expression T_GT expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp > exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_GT,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | expression T_GE expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp >= exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_GE,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | expression T_LT expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp < exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_LT,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | expression T_LE expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp <= exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_LE,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | expression T_EQ expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp == exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_EQ,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | expression T_NE expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp != exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::CMP_NE,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
  */
};

expression 
: expression '+' expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp + exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::ADD,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | expression '-' expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp - exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::SUB,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | expression '*' expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp * exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::MUL,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
  /*
} | expression '*' '*' expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp ** exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::FUNC,aParser.find_function("pow"),$1,$4);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
  */
} | expression '/' expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp / exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::DIV,$1,$3);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | '-' expression %prec UMINUS { 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : - exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::MINUS,$2);
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | '(' expression ')' { 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : (exp)" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::ASSIGN,$2);
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }

} | T_NAME '(' expression ')' { 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME(1)" << std::endl;
#endif
  if(aParser.find_names()) {
    $$ = 0;
  } else if(aParser.find_functions()) {
    aParser.add_name(aParser.lexer().variable($1));
    $$ = 0;
  } else {
    std::string var = aParser.lexer().variable($1);
    $$ = new inlib::valop(inlib::valop::FUNC,aParser.find_function(var),$3);
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | T_NAME '(' expression ',' expression ')' { 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME(2)" << std::endl;
#endif
  if(aParser.find_names()) {
    $$ = 0;
  } else if(aParser.find_functions()) {
    aParser.add_name(aParser.lexer().variable($1));
    $$ = 0;
  } else {
    std::string var = aParser.lexer().variable($1);    
    $$ = new inlib::valop(inlib::valop::FUNC,aParser.find_function(var),$3,$5);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | T_NAME '(' expression ',' expression ',' expression ')' {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME(3)" << std::endl;
#endif
  if(aParser.find_names()) {
    $$ = 0;
  } else if(aParser.find_functions()) {
    aParser.add_name(aParser.lexer().variable($1));
    $$ = 0;
  } else {
    std::string var = aParser.lexer().variable($1);    
    $$ = new inlib::valop(inlib::valop::FUNC,aParser.find_function(var),$3,$5,$7);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | T_NAME '(' expression ',' expression ',' expression ',' expression ')' {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME(4)" << std::endl;
#endif
  if(aParser.find_names()) {
    $$ = 0;
  } else if(aParser.find_functions()) {
    aParser.add_name(aParser.lexer().variable($1));
    $$ = 0;
  } else {
    std::string var = aParser.lexer().variable($1);    
    $$ = new inlib::valop(inlib::valop::FUNC,aParser.find_function(var),$3,$5,$7,$9);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | T_NAME '(' expression ',' expression ',' expression ',' expression ',' expression ')' {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME(5)" << std::endl;
#endif
  if(aParser.find_names()) {
    $$ = 0;
  } else if(aParser.find_functions()) {
    aParser.add_name(aParser.lexer().variable($1));
    $$ = 0;
  } else {
    std::string var = aParser.lexer().variable($1);
    $$ = new inlib::valop(inlib::valop::FUNC,aParser.find_function(var),$3,$5,$7,$9,$11);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
} | T_NAME '(' expression ',' expression ',' expression ',' expression ',' expression ',' expression ')' {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME(6)" << std::endl;
#endif
  if(aParser.find_names()) {
    $$ = 0;
  } else if(aParser.find_functions()) {
    aParser.add_name(aParser.lexer().variable($1));
    $$ = 0;
  } else {
    std::string var = aParser.lexer().variable($1);    
    $$ = new inlib::valop(inlib::valop::FUNC,aParser.find_function(var),$3,$5,$7,$9,$11,$13);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }

} | T_PI { 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_PI" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::REAL,inlib::pi());
    aParser.stack().push_back($$);
  }
} | T_TRUE { 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_TRUE" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::BOOL_TRUE,true);
    aParser.stack().push_back($$);
  }
} | T_FALSE { 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_FALSE" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::BOOL_FALSE,false);
    aParser.stack().push_back($$);
  }
} | NUMBER_INTEGER { 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : NUMBER_INTEGER" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::UNSIGNED_INTEGER,(unsigned int)$1);
    aParser.stack().push_back($$);
  }
} | NUMBER_REAL { 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : NUMBER_REAL" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::REAL,$1);
    aParser.stack().push_back($$);
  }
} | T_NAME {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME" << std::endl;
#endif
  if(aParser.find_names()) {
    aParser.add_name(aParser.lexer().variable($1));
    $$ = 0;
  } else if(aParser.find_functions()) {
    $$ = 0;
  } else {
    std::string var = aParser.lexer().variable($1);
    unsigned int index;
    int ivar = NotFound;
    if(aParser.find_variable(var,index)) ivar = int(index);
    $$ = new inlib::valop(inlib::valop::NAME,var,ivar);
    aParser.stack().push_back($$);
  }
} | STRG {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : STRG" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop(inlib::valop::STRING,
                             aParser.lexer().variable($1));
    aParser.stack().push_back($$);
  }
};

%%

static void yyerror(exlib::yacc::cexpr_eval& aParser,const char* aString) {
  aParser.out() << "Processor : " << aString << std::endl;
  aParser.set_status(false);
}
