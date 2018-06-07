#ifndef lint
static const char fort_fmt_sccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif

#include <stdlib.h>

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20050813

#define YYEMPTY (-1)
#define fort_fmt_clearin    (fort_fmt_char = YYEMPTY)
#define fort_fmt_errok      (fort_fmt_errflag = 0)
#define YYRECOVERING (fort_fmt_errflag != 0)

//extern int fort_fmt_parse(exlib::fort_fmt& aParser);

static int fort_fmt_growstack(void);
#define YYPREFIX "fort_fmt_"
#line 2 "../source/fort_fmt.y"

/* this :*/
#include "fort_fmt.h"

#ifdef INLIB_MEM
#include <inlib/mem>
#endif
#include <inlib/sprintf>

#include "fort_fmt_lexer.h"

extern int fort_fmt_lexer_lex(exlib::fort_fmt_lexer&);
static int fort_fmt_parse(exlib::fort_fmt& aParser);
static void fort_fmt_error(exlib::fort_fmt&,const char*); 

/*#define LIB_DEBUG*/

#line 20 "../source/fort_fmt.y"
typedef union {
  unsigned long fInteger;
  exlib::data_desc* fDesc;
  unsigned int fType;
} YYSTYPE;
#line 45 "y.tab.c"
#define NUMBER_INTEGER 257
#define NUMBER_EXPONENT 258
#define T_X 259
#define T_I 260
#define T_F 261
#define T_E 262
#define T_G 263
#define T_A 264
#define T_L 265
#define T_SLASH 266
#define UMINUS 267
#define YYERRCODE 256
short fort_fmt_lhs[] = {                                        -1,
    0,    0,    1,    1,    2,    2,    2,    3,    3,    4,
    4,    4,    4,    5,    5,    5,    5,    5,    5,    5,
    5,
};
short fort_fmt_len[] = {                                         2,
    1,    1,    3,    4,    1,    1,    3,    1,    2,    1,
    2,    4,    5,    1,    1,    1,    1,    1,    1,    1,
    1,
};
short fort_fmt_defred[] = {                                      0,
    1,    0,    0,    0,    2,    0,    0,   14,   15,   16,
   17,   18,   19,   20,   21,    6,    0,    5,    8,    0,
    0,    9,    3,    0,    0,    4,    0,    0,    0,   13,
};
short fort_fmt_dgoto[] = {                                       4,
   16,   17,   18,   19,   20,
};
short fort_fmt_sindex[] = {                                    -21,
    0,  -39,  -40,    0,    0,  -40,  -32,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  -38,    0,    0, -252,
  -37,    0,    0,  -40,  -28,    0,  -35, -241, -237,    0,
};
short fort_fmt_rindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -31,
    0,    0,    0,    0,  -30,    0,  -24,    0,  -29,    0,
};
short fort_fmt_gindex[] = {                                      0,
   22,   -4,    0,   16,    0,
};
#define YYTABLESIZE 236
short fort_fmt_table[] = {                                       3,
    6,   21,   23,   26,   25,   24,   24,    6,   24,   10,
   11,   12,   10,   11,   12,   29,    7,   28,    3,   27,
   30,    5,   22,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    7,    0,    8,    9,
   10,   11,   12,   13,   14,   15,    8,    9,   10,   11,
   12,   13,   14,   15,    1,    2,
};
short fort_fmt_check[] = {                                      40,
   40,    6,   41,   41,  257,   44,   44,   40,   44,   41,
   41,   41,   44,   44,   44,  257,   41,   46,   40,   24,
  258,    0,    7,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  257,   -1,  259,  260,
  261,  262,  263,  264,  265,  266,  259,  260,  261,  262,
  263,  264,  265,  266,  256,  257,
};
#define YYFINAL 4
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 267
#if YYDEBUG
char *fort_fmt_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'","'*'","'+'","','","'-'","'.'","'/'",0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"NUMBER_INTEGER","NUMBER_EXPONENT","T_X","T_I","T_F","T_E","T_G","T_A","T_L",
"T_SLASH","UMINUS",
};
char *fort_fmt_rule[] = {
"$accept : result",
"result : error",
"result : data_desc_tree",
"data_desc_tree : '(' data_desc_list ')'",
"data_desc_tree : NUMBER_INTEGER '(' data_desc_list ')'",
"data_desc_list : data_desc_leaf",
"data_desc_list : data_desc_tree",
"data_desc_list : data_desc_list ',' data_desc_list",
"data_desc_leaf : data_desc_one",
"data_desc_leaf : NUMBER_INTEGER data_desc_one",
"data_desc_one : data_type",
"data_desc_one : data_type NUMBER_INTEGER",
"data_desc_one : data_type NUMBER_INTEGER '.' NUMBER_INTEGER",
"data_desc_one : data_type NUMBER_INTEGER '.' NUMBER_INTEGER NUMBER_EXPONENT",
"data_type : T_X",
"data_type : T_I",
"data_type : T_F",
"data_type : T_E",
"data_type : T_G",
"data_type : T_A",
"data_type : T_L",
"data_type : T_SLASH",
};
#endif
#if YYDEBUG
#include <stdio.h>
#endif

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH  500
#endif
#endif

#define YYINITSTACKSIZE 500

int      fort_fmt_debug;
int      fort_fmt_nerrs;
int      fort_fmt_errflag;
int      fort_fmt_char;
short   *fort_fmt_ssp;
YYSTYPE *fort_fmt_vsp;
YYSTYPE  fort_fmt_val;
YYSTYPE  fort_fmt_lval;

/* variables for the parser stack */
static short   *fort_fmt_ss;
static short   *fort_fmt_sslim;
static YYSTYPE *fort_fmt_vs;
static int      fort_fmt_stacksize;
#line 241 "../source/fort_fmt.y"

//////////////////////////////////////////////////////////////////////////////
exlib::fort_fmt::fort_fmt(
 const std::string& aString
,std::ostream& a_out
,int aVerboseLevel
) 
:fVerboseLevel(aVerboseLevel)
,fString(aString)
,f_out(a_out)
,fLexer(a_out,aVerboseLevel)
,fSuccess(false)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
}
//////////////////////////////////////////////////////////////////////////////
exlib::fort_fmt::~fort_fmt(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  clear();
}

// private.
exlib::fort_fmt::fort_fmt(const fort_fmt& aFrom) 
:fVerboseLevel(aFrom.fVerboseLevel)
,fString(aFrom.fString)
,f_out(aFrom.f_out)
,fLexer(aFrom.f_out,fVerboseLevel)
,fSuccess(false)
{}

// private.
exlib::fort_fmt& exlib::fort_fmt::operator=(const fort_fmt&) {return *this;}

void exlib::fort_fmt::setScript(const std::string& aString) {fString = aString;}

const std::string& exlib::fort_fmt::script() const {return fString;}

bool exlib::fort_fmt::parse() {
  clear();
  if(fString.empty()) return false;
  fLexer.set(fString);
  fSuccess = true;
  fort_fmt_parse(*this);
  if(!fSuccess) clear();
  return fSuccess;
}

const exlib::data_desc& exlib::fort_fmt::result() const {return fTree;}

void exlib::fort_fmt::clear() {
  fTree.clear();

  // In case of error :
 {std::vector<data_desc*>::iterator it;
  for(it=fTmpDescs.begin();it!=fTmpDescs.end();++it) delete (*it);
  fTmpDescs.clear();}
}

bool exlib::fort_fmt::isSuccess() const {return fSuccess;}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

std::ostream& exlib::fort_fmt::out() const {return f_out;}

exlib::fort_fmt_lexer& exlib::fort_fmt::lexer() {return fLexer;}

static void fort_fmt_error(exlib::fort_fmt& aParser,const char* aString) {
  aParser.out() << "fort_fmt : " << aString << std::endl;
  aParser.fSuccess = false;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
exlib::data_desc::data_desc(
)
:fParent(0)
,fIndex(0)
,fMultiplicity(1)
,fType(0) //Unknown by default
,fWidth(0)
,fPrecision(0)
,fExponent(0)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef INLIB_MEM
  inlib::mem::increment("exlib::data_desc");
#endif
}
//////////////////////////////////////////////////////////////////////////////
exlib::data_desc::~data_desc(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  clear();
#ifdef INLIB_MEM
  inlib::mem::decrement("exlib::data_desc");
#endif
}

void exlib::data_desc::clear() {
  std::vector<data_desc*>::iterator it;
  for(it=fChildren.begin();it!=fChildren.end();++it) delete *it;
  fChildren.clear();
}

void exlib::data_desc::reset(bool aDoThis){
  // Descent the tree to reset all the fIndex to 0.
  if(aDoThis) fIndex = 0;
  std::vector<data_desc*>::iterator it;
  for(it=fChildren.begin();it!=fChildren.end();++it) (*it)->reset();
}

exlib::data_desc_iterator* exlib::data_desc::createIterator() const {
  data_desc& self = const_cast<data_desc&>(*this);
  return new exlib::data_desc_iterator(self);
}

exlib::data_desc* exlib::data_desc::firstLeaf() {
  std::vector<data_desc*>::iterator it;
  for(it=fChildren.begin();it!=fChildren.end();++it) {
    exlib::data_desc* leaf = (*it)->firstLeaf();
    if(leaf) return leaf;
  }
  if(!fChildren.size()) return 0;
  return fChildren[0];
}
//////////////////////////////////////////////////////////////////////////////
exlib::data_desc* exlib::data_desc::nextChild(
 exlib::data_desc* aChild
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  unsigned int number = fChildren.size(); 
  if(!aChild && number) return fChildren[0];
  for(unsigned int index=0;index<number;index++) {
    if(aChild==fChildren[index]) {
      if((index+1)<number) {
        return fChildren[index+1];
      } else { 
        return 0;
      }
    }
  }
  return 0; //Should never pass here.
}
//////////////////////////////////////////////////////////////////////////////
void exlib::data_desc::dump(
 std::ostream& a_out
,int aIndent
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  std::string sindent(aIndent,' ');

  if(!fChildren.size()) { // Leaf :
    a_out << sindent;
    a_out << fMultiplicity;
    if(fType==data_desc_X) a_out << "X";
    else if(fType==data_desc_I) a_out << "I";
    else if(fType==data_desc_F) a_out << "F";
    else if(fType==data_desc_E) a_out << "E";
    else if(fType==data_desc_G) a_out << "G";
    else if(fType==data_desc_A) a_out << "A";
    else if(fType==data_desc_SLASH) a_out << "/";
    else a_out << "?";
    a_out << fWidth;
    a_out << ".";
    a_out << fPrecision;
    a_out << "E";
    a_out << fExponent;
    a_out << std::endl;    
  } else {

    a_out << sindent;
    a_out << fMultiplicity;
    a_out << std::endl;    

   {std::vector<data_desc*>::const_iterator it;
    for(it=fChildren.begin();it!=fChildren.end();++it) {
      (*it)->dump(a_out,aIndent+2);
    }}
  }

}

std::string exlib::data_desc::cformat() const {
  data_desc& self = const_cast<data_desc&>(*this);

       if(fType==data_desc_X) {
    return "";
  } else if(fType==data_desc_I) {
    std::string cformat;
    inlib::sprintf(cformat,64,"%%%d.%dd",fWidth,fPrecision);
    return cformat;
  } else if(fType==data_desc_F) {
    // # : to force having a decimal point, even if precision is 0.
    std::string cformat;
    inlib::sprintf(cformat,64,"%%#%d.%df",fWidth,fPrecision);
    return cformat;
  } else if(fType==data_desc_E) {
    std::string cformat;
    inlib::sprintf(cformat,64,"%%%d.%dE",fWidth,fPrecision);
    return cformat;
  } else if(fType==data_desc_G) {
    std::string cformat;
    inlib::sprintf(cformat,64,"%%%d.%dG",fWidth,fPrecision);
    return cformat;
  } else if(fType==data_desc_A) {
    return "%s";
  } else if(fType==data_desc_SLASH) {
    return "";
  } else {
    return "";  
  }
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
exlib::data_desc_iterator::data_desc_iterator(
 exlib::data_desc& aTree
)
:fTree(aTree)
,fDesc(0)
,fFinished(false)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef INLIB_MEM
  inlib::mem::increment("exlib::data_desc_iterator");
#endif
  fTree.reset();
}
//////////////////////////////////////////////////////////////////////////////
exlib::data_desc_iterator::~data_desc_iterator(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
#ifdef INLIB_MEM
  inlib::mem::decrement("exlib::data_desc_iterator");
#endif
}
//////////////////////////////////////////////////////////////////////////////
void exlib::data_desc_iterator::reset(
) 
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  fTree.reset();
  fDesc = 0;
  fFinished = false;
}
//////////////////////////////////////////////////////////////////////////////
bool exlib::data_desc_iterator::finished(
) const
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  return fFinished;
}
//////////////////////////////////////////////////////////////////////////////
exlib::data_desc* exlib::data_desc_iterator::next(
)
//////////////////////////////////////////////////////////////////////////////
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  if(fFinished) return 0;
  if(!fDesc) { //Begin of iteration.
    fDesc = &fTree;
    fDesc->fIndex = 0;
  } else { // Repeat again ?
    if((fDesc->fIndex+1)<(int)fDesc->fMultiplicity) {
      fDesc->fIndex++;
      fDesc->reset(false);
      return fDesc; // The same.
    }
  }
  // New desc :
  while(true) {
    exlib::data_desc* leaf = fDesc->firstLeaf();
    if(leaf) {
      fDesc = leaf;
      fDesc->fIndex = 0;
   /*{exlib::data_desc* parent = fDesc->fParent;
      while(parent) {
        parent->fIndex = 0;
        parent = parent->fParent;          
      }}*/
      return leaf;
    }
    // Back to parent and after current fDesc :
    exlib::data_desc* parent = fDesc->fParent;
    while(1) {
      if(!parent) { //Tree finished.      
        fDesc = 0;
        fFinished = true;
        return 0;
      }
      fDesc = parent->nextChild(fDesc);
      if(fDesc) break;
      // parent children exhausted.
      // Repeat again ?
      if((parent->fIndex+1)<(int)parent->fMultiplicity) {
        parent->fIndex++;
        parent->reset(false);
        fDesc = parent;
        break;
      } else {
        fDesc = parent;
        parent = parent->fParent;          
      }
    }
  }
}
#line 535 "y.tab.c"
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int fort_fmt_growstack(void)
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = fort_fmt_stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = fort_fmt_ssp - fort_fmt_ss;
    newss = (fort_fmt_ss != 0)
          ? (short *)realloc(fort_fmt_ss, newsize * sizeof(*newss))
          : (short *)malloc(newsize * sizeof(*newss));
    if (newss == 0)
        return -1;

    fort_fmt_ss  = newss;
    fort_fmt_ssp = newss + i;
    newvs = (fort_fmt_vs != 0)
          ? (YYSTYPE *)realloc(fort_fmt_vs, newsize * sizeof(*newvs))
          : (YYSTYPE *)malloc(newsize * sizeof(*newvs));
    if (newvs == 0)
        return -1;

    fort_fmt_vs = newvs;
    fort_fmt_vsp = newvs + i;
    fort_fmt_stacksize = newsize;
    fort_fmt_sslim = fort_fmt_ss + newsize - 1;
    return 0;
}

#define YYABORT goto fort_fmt_abort
#define YYREJECT goto fort_fmt_abort
#define YYACCEPT goto fort_fmt_accept
#define YYERROR goto fort_fmt_errlab
int
fort_fmt_parse(exlib::fort_fmt& aParser)
{
    /*GB register*/ int fort_fmt_m, fort_fmt_n, fort_fmt_state;
#if YYDEBUG
    /*GB register*/ const char *fort_fmt_s;

    if ((fort_fmt_s = getenv("YYDEBUG")) != 0)
    {
        fort_fmt_n = *fort_fmt_s;
        if (fort_fmt_n >= '0' && fort_fmt_n <= '9')
            fort_fmt_debug = fort_fmt_n - '0';
    }
#endif

    fort_fmt_nerrs = 0;
    fort_fmt_errflag = 0;
    fort_fmt_char = YYEMPTY;

    if (fort_fmt_ss == NULL && fort_fmt_growstack()) goto fort_fmt_overflow;
    fort_fmt_ssp = fort_fmt_ss;
    fort_fmt_vsp = fort_fmt_vs;
    *fort_fmt_ssp = fort_fmt_state = 0;

fort_fmt_loop:
    if ((fort_fmt_n = fort_fmt_defred[fort_fmt_state]) != 0) goto fort_fmt_reduce;
    if (fort_fmt_char < 0)
    {
        if ((fort_fmt_char = fort_fmt_lexer_lex(aParser.lexer())) < 0) fort_fmt_char = 0;
#if YYDEBUG
        if (fort_fmt_debug)
        {
            fort_fmt_s = 0;
            if (fort_fmt_char <= YYMAXTOKEN) fort_fmt_s = fort_fmt_name[fort_fmt_char];
            if (!fort_fmt_s) fort_fmt_s = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, fort_fmt_state, fort_fmt_char, fort_fmt_s);
        }
#endif
    }
    if ((fort_fmt_n = fort_fmt_sindex[fort_fmt_state]) && (fort_fmt_n += fort_fmt_char) >= 0 &&
            fort_fmt_n <= YYTABLESIZE && fort_fmt_check[fort_fmt_n] == fort_fmt_char)
    {
#if YYDEBUG
        if (fort_fmt_debug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, fort_fmt_state, fort_fmt_table[fort_fmt_n]);
#endif
        if (fort_fmt_ssp >= fort_fmt_sslim && fort_fmt_growstack())
        {
            goto fort_fmt_overflow;
        }
        *++fort_fmt_ssp = fort_fmt_state = fort_fmt_table[fort_fmt_n];
        *++fort_fmt_vsp = fort_fmt_lval;
        fort_fmt_char = YYEMPTY;
        if (fort_fmt_errflag > 0)  --fort_fmt_errflag;
        goto fort_fmt_loop;
    }
    if ((fort_fmt_n = fort_fmt_rindex[fort_fmt_state]) && (fort_fmt_n += fort_fmt_char) >= 0 &&
            fort_fmt_n <= YYTABLESIZE && fort_fmt_check[fort_fmt_n] == fort_fmt_char)
    {
        fort_fmt_n = fort_fmt_table[fort_fmt_n];
        goto fort_fmt_reduce;
    }
    if (fort_fmt_errflag) goto fort_fmt_inrecovery;

    fort_fmt_error(aParser,"syntax error");

#ifdef lint
    goto fort_fmt_errlab;
#endif

fort_fmt_errlab:
    ++fort_fmt_nerrs;

fort_fmt_inrecovery:
    if (fort_fmt_errflag < 3)
    {
        fort_fmt_errflag = 3;
        for (;;)
        {
            if ((fort_fmt_n = fort_fmt_sindex[*fort_fmt_ssp]) && (fort_fmt_n += YYERRCODE) >= 0 &&
                    fort_fmt_n <= YYTABLESIZE && fort_fmt_check[fort_fmt_n] == YYERRCODE)
            {
#if YYDEBUG
                if (fort_fmt_debug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *fort_fmt_ssp, fort_fmt_table[fort_fmt_n]);
#endif
                if (fort_fmt_ssp >= fort_fmt_sslim && fort_fmt_growstack())
                {
                    goto fort_fmt_overflow;
                }
                *++fort_fmt_ssp = fort_fmt_state = fort_fmt_table[fort_fmt_n];
                *++fort_fmt_vsp = fort_fmt_lval;
                goto fort_fmt_loop;
            }
            else
            {
#if YYDEBUG
                if (fort_fmt_debug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *fort_fmt_ssp);
#endif
                if (fort_fmt_ssp <= fort_fmt_ss) goto fort_fmt_abort;
                --fort_fmt_ssp;
                --fort_fmt_vsp;
            }
        }
    }
    else
    {
        if (fort_fmt_char == 0) goto fort_fmt_abort;
#if YYDEBUG
        if (fort_fmt_debug)
        {
            fort_fmt_s = 0;
            if (fort_fmt_char <= YYMAXTOKEN) fort_fmt_s = fort_fmt_name[fort_fmt_char];
            if (!fort_fmt_s) fort_fmt_s = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, fort_fmt_state, fort_fmt_char, fort_fmt_s);
        }
#endif
        fort_fmt_char = YYEMPTY;
        goto fort_fmt_loop;
    }

fort_fmt_reduce:
#if YYDEBUG
    if (fort_fmt_debug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, fort_fmt_state, fort_fmt_n, fort_fmt_rule[fort_fmt_n]);
#endif
    fort_fmt_m = fort_fmt_len[fort_fmt_n];
    fort_fmt_val = fort_fmt_vsp[1-fort_fmt_m];
    switch (fort_fmt_n)
    {
case 1:
#line 43 "../source/fort_fmt.y"
{
  /* To reset lex and yacc in case of error.*/
}
break;
case 2:
#line 45 "../source/fort_fmt.y"
{
  aParser.fTree.fChildren.push_back(fort_fmt_vsp[0].fDesc);
  fort_fmt_vsp[0].fDesc->fParent = &(aParser.fTree);
  aParser.fTmpDescs.pop_back();
}
break;
case 3:
#line 52 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_tree : (data_desc_list)" 
        << std::endl;
  }
  fort_fmt_vsp[-1].fDesc->fMultiplicity = 1;
  fort_fmt_val.fDesc = fort_fmt_vsp[-1].fDesc;
}
break;
case 4:
#line 60 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_tree : integer(data_desc_list)" 
        << std::endl;
  }
  fort_fmt_vsp[-1].fDesc->fMultiplicity = fort_fmt_vsp[-3].fInteger;
  fort_fmt_val.fDesc = fort_fmt_vsp[-1].fDesc;
}
break;
case 5:
#line 71 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_list : data_desc_leaf" 
        << std::endl;
  }
  exlib::data_desc* list = new exlib::data_desc();
  list->fChildren.push_back(fort_fmt_vsp[0].fDesc);
  fort_fmt_vsp[0].fDesc->fParent = list;
  aParser.fTmpDescs.pop_back();
  aParser.fTmpDescs.push_back(list);/*To cleanup in case of error.*/
  fort_fmt_val.fDesc = list;
}
break;
case 6:
#line 83 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_list : data_desc_tree" 
        << std::endl;
  }
  exlib::data_desc* list = new exlib::data_desc();
  list->fChildren.push_back(fort_fmt_vsp[0].fDesc);
  fort_fmt_vsp[0].fDesc->fParent = list;
  aParser.fTmpDescs.pop_back();
  aParser.fTmpDescs.push_back(list);/*To cleanup in case of error.*/
  fort_fmt_val.fDesc = list;
}
break;
case 7:
#line 95 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_list : data_desc_tree,data_desc_tree" 
        << std::endl;
  }
  exlib::data_desc* list = new exlib::data_desc();
  list->fChildren.push_back(fort_fmt_vsp[-2].fDesc);
  fort_fmt_vsp[-2].fDesc->fParent = list;
  list->fChildren.push_back(fort_fmt_vsp[0].fDesc);
  fort_fmt_vsp[0].fDesc->fParent = list;
  aParser.fTmpDescs.pop_back();
  aParser.fTmpDescs.pop_back();
  aParser.fTmpDescs.push_back(list);/*To cleanup in case of error.*/
  fort_fmt_val.fDesc = list;
}
break;
case 8:
#line 113 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_leaf : data_desc_one" 
        << std::endl;
  }
  fort_fmt_vsp[0].fDesc->fMultiplicity = 1;
  fort_fmt_val.fDesc = fort_fmt_vsp[0].fDesc;
}
break;
case 9:
#line 121 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_leaf : integer data_desc_one" 
        << std::endl;
  }
  fort_fmt_vsp[0].fDesc->fMultiplicity = fort_fmt_vsp[-1].fInteger;
  fort_fmt_val.fDesc = fort_fmt_vsp[0].fDesc;
}
break;
case 10:
#line 132 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_one : data_type" 
        << std::endl;
  }
  exlib::data_desc* leaf = new exlib::data_desc();
  aParser.fTmpDescs.push_back(leaf);/*To cleanup in case of error.*/
  leaf->fType = fort_fmt_vsp[0].fType;
  fort_fmt_val.fDesc = leaf;
}
break;
case 11:
#line 142 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_one : data_type integer" 
        << std::endl;
  }
  exlib::data_desc* leaf = new exlib::data_desc();
  aParser.fTmpDescs.push_back(leaf);/*To cleanup in case of error.*/
  leaf->fType = fort_fmt_vsp[-1].fType;
  leaf->fWidth = fort_fmt_vsp[0].fInteger;
  fort_fmt_val.fDesc = leaf;
}
break;
case 12:
#line 153 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_one : data_type integer.integer" 
        << std::endl;
  }
  exlib::data_desc* leaf = new exlib::data_desc();
  aParser.fTmpDescs.push_back(leaf);/*To cleanup in case of error.*/
  leaf->fType = fort_fmt_vsp[-3].fType;
  leaf->fWidth = fort_fmt_vsp[-2].fInteger;
  leaf->fPrecision = fort_fmt_vsp[0].fInteger;
  fort_fmt_val.fDesc = leaf;
}
break;
case 13:
#line 165 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_one : data_type integer.integerEinteger" 
        << std::endl;
  }
  exlib::data_desc* leaf = new exlib::data_desc();
  aParser.fTmpDescs.push_back(leaf);/*To cleanup in case of error.*/
  leaf->fType = fort_fmt_vsp[-4].fType;
  leaf->fWidth = fort_fmt_vsp[-3].fInteger;
  leaf->fPrecision = fort_fmt_vsp[-1].fInteger;
  leaf->fExponent = fort_fmt_vsp[0].fInteger;
  fort_fmt_val.fDesc = leaf;
}
break;
case 14:
#line 181 "../source/fort_fmt.y"
{ 
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_type : T_X" 
        << std::endl;
  }
  fort_fmt_val.fType = data_desc_X;
}
break;
case 15:
#line 188 "../source/fort_fmt.y"
{ 
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
          << " data_type : T_I" 
          << std::endl;
  }
  fort_fmt_val.fType = data_desc_I;
}
break;
case 16:
#line 195 "../source/fort_fmt.y"
{ 
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_type : T_F" 
        << std::endl;
  }
  fort_fmt_val.fType = data_desc_F;
}
break;
case 17:
#line 202 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_type : T_E" 
        << std::endl;
  }
  fort_fmt_val.fType = data_desc_E;
}
break;
case 18:
#line 209 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_type : T_G" 
        << std::endl;
  }
  fort_fmt_val.fType = data_desc_G;
}
break;
case 19:
#line 216 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
          << " data_type : T_A" 
          << std::endl;
  }
  fort_fmt_val.fType = data_desc_A;
}
break;
case 20:
#line 223 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
          << " data_type : T_L" 
          << std::endl;
  }
  fort_fmt_val.fType = data_desc_L;
}
break;
case 21:
#line 230 "../source/fort_fmt.y"
{
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_type : T_SLASH" 
        << std::endl;
  }
  fort_fmt_val.fType = data_desc_SLASH;
}
break;
#line 976 "y.tab.c"
    }
    fort_fmt_ssp -= fort_fmt_m;
    fort_fmt_state = *fort_fmt_ssp;
    fort_fmt_vsp -= fort_fmt_m;
    fort_fmt_m = fort_fmt_lhs[fort_fmt_n];
    if (fort_fmt_state == 0 && fort_fmt_m == 0)
    {
#if YYDEBUG
        if (fort_fmt_debug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        fort_fmt_state = YYFINAL;
        *++fort_fmt_ssp = YYFINAL;
        *++fort_fmt_vsp = fort_fmt_val;
        if (fort_fmt_char < 0)
        {
            if ((fort_fmt_char = fort_fmt_lexer_lex(aParser.lexer())) < 0) fort_fmt_char = 0;
#if YYDEBUG
            if (fort_fmt_debug)
            {
                fort_fmt_s = 0;
                if (fort_fmt_char <= YYMAXTOKEN) fort_fmt_s = fort_fmt_name[fort_fmt_char];
                if (!fort_fmt_s) fort_fmt_s = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, fort_fmt_char, fort_fmt_s);
            }
#endif
        }
        if (fort_fmt_char == 0) goto fort_fmt_accept;
        goto fort_fmt_loop;
    }
    if ((fort_fmt_n = fort_fmt_gindex[fort_fmt_m]) && (fort_fmt_n += fort_fmt_state) >= 0 &&
            fort_fmt_n <= YYTABLESIZE && fort_fmt_check[fort_fmt_n] == fort_fmt_state)
        fort_fmt_state = fort_fmt_table[fort_fmt_n];
    else
        fort_fmt_state = fort_fmt_dgoto[fort_fmt_m];
#if YYDEBUG
    if (fort_fmt_debug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *fort_fmt_ssp, fort_fmt_state);
#endif
    if (fort_fmt_ssp >= fort_fmt_sslim && fort_fmt_growstack())
    {
        goto fort_fmt_overflow;
    }
    *++fort_fmt_ssp = fort_fmt_state;
    *++fort_fmt_vsp = fort_fmt_val;
    goto fort_fmt_loop;

fort_fmt_overflow:
    fort_fmt_error(aParser,"yacc stack overflow");

fort_fmt_abort:
    return (1);

fort_fmt_accept:
    return (0);
}
