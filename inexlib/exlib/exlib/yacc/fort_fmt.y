%{

// this :
#include "fort_fmt.h"

#ifdef INLIB_MEM
#include <inlib/mem>
#endif
#include <inlib/sprintf>
#include <inlib/cast>

#include "fort_fmt_lexer.h"

extern int yylex(exlib::fort_fmt_lexer&);
static int yyparse(void);
static void yyerror(exlib::fort_fmt&,const char*); 

//#define LIB_DEBUG

%}

%union {
  unsigned long fInteger;
  exlib::data_desc* fDesc;
  unsigned int fType;
}

%token <fInteger> NUMBER_INTEGER NUMBER_EXPONENT

%token T_X T_I T_F T_E T_G T_A T_L T_SLASH

%type <fDesc> data_desc_tree
%type <fDesc> data_desc_list
%type <fDesc> data_desc_leaf
%type <fDesc> data_desc_one
%type <fType> data_type

%left '-' '+'
%left '/' '*'
%nonassoc UMINUS

%%

result 
: error {
  // To reset lex and yacc in case of error.
} | data_desc_tree {
  aParser.fTree.fChildren.push_back($1);
  $1->fParent = &(aParser.fTree);
  aParser.fTmpDescs.pop_back();
};

data_desc_tree 
: '(' data_desc_list ')' {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_tree : (data_desc_list)" 
        << std::endl;
  }
  $2->fMultiplicity = 1;
  $$ = $2;
} | NUMBER_INTEGER '(' data_desc_list ')' {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_tree : integer(data_desc_list)" 
        << std::endl;
  }
  $3->fMultiplicity = $1;
  $$ = $3;
};

data_desc_list 
: data_desc_leaf {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_list : data_desc_leaf" 
        << std::endl;
  }
  exlib::data_desc* list = new exlib::data_desc();
  list->fChildren.push_back($1);
  $1->fParent = list;
  aParser.fTmpDescs.pop_back();
  aParser.fTmpDescs.push_back(list);//To cleanup in case of error.
  $$ = list;
} | data_desc_tree {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_list : data_desc_tree" 
        << std::endl;
  }
  exlib::data_desc* list = new exlib::data_desc();
  list->fChildren.push_back($1);
  $1->fParent = list;
  aParser.fTmpDescs.pop_back();
  aParser.fTmpDescs.push_back(list);//To cleanup in case of error.
  $$ = list;
} | data_desc_list ',' data_desc_list {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_list : data_desc_tree,data_desc_tree" 
        << std::endl;
  }
  exlib::data_desc* list = new exlib::data_desc();
  list->fChildren.push_back($1);
  $1->fParent = list;
  list->fChildren.push_back($3);
  $3->fParent = list;
  aParser.fTmpDescs.pop_back();
  aParser.fTmpDescs.pop_back();
  aParser.fTmpDescs.push_back(list);//To cleanup in case of error.
  $$ = list;
};

data_desc_leaf 
: data_desc_one {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_leaf : data_desc_one" 
        << std::endl;
  }
  $1->fMultiplicity = 1;
  $$ = $1;
} | NUMBER_INTEGER data_desc_one {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_leaf : integer data_desc_one" 
        << std::endl;
  }
  $2->fMultiplicity = $1;
  $$ = $2;
};

data_desc_one
: data_type {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_one : data_type" 
        << std::endl;
  }
  exlib::data_desc* leaf = new exlib::data_desc();
  aParser.fTmpDescs.push_back(leaf);//To cleanup in case of error.
  leaf->fType = $1;
  $$ = leaf;
} | data_type NUMBER_INTEGER {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_one : data_type integer" 
        << std::endl;
  }
  exlib::data_desc* leaf = new exlib::data_desc();
  aParser.fTmpDescs.push_back(leaf);//To cleanup in case of error.
  leaf->fType = $1;
  leaf->fWidth = $2;
  $$ = leaf;
} | data_type NUMBER_INTEGER '.' NUMBER_INTEGER {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_one : data_type integer.integer" 
        << std::endl;
  }
  exlib::data_desc* leaf = new exlib::data_desc();
  aParser.fTmpDescs.push_back(leaf);//To cleanup in case of error.
  leaf->fType = $1;
  leaf->fWidth = $2;
  leaf->fPrecision = $4;
  $$ = leaf;
} | data_type NUMBER_INTEGER '.' NUMBER_INTEGER NUMBER_EXPONENT {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_desc_one : data_type integer.integerEinteger" 
        << std::endl;
  }
  exlib::data_desc* leaf = new exlib::data_desc();
  aParser.fTmpDescs.push_back(leaf);//To cleanup in case of error.
  leaf->fType = $1;
  leaf->fWidth = $2;
  leaf->fPrecision = $4;
  leaf->fExponent = $5;
  $$ = leaf;
};

data_type  
: T_X { 
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_type : T_X" 
        << std::endl;
  }
  $$ = data_desc_X;
} | T_I { 
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
          << " data_type : T_I" 
          << std::endl;
  }
  $$ = data_desc_I;
} | T_F { 
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_type : T_F" 
        << std::endl;
  }
  $$ = data_desc_F;
} | T_E {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_type : T_E" 
        << std::endl;
  }
  $$ = data_desc_E;
} | T_G {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_type : T_G" 
        << std::endl;
  }
  $$ = data_desc_G;
} | T_A {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
          << " data_type : T_A" 
          << std::endl;
  }
  $$ = data_desc_A;
} | T_L {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
          << " data_type : T_L" 
          << std::endl;
  }
  $$ = data_desc_L;
} | T_SLASH {
  if(aParser.fVerboseLevel) {
    aParser.out() << "exlib::fort_fmt::parse :"
        << " data_type : T_SLASH" 
        << std::endl;
  }
  $$ = data_desc_SLASH;
}
;

%%

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
  yyparse(*this);
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

static void yyerror(exlib::fort_fmt& aParser,const char* aString) {
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
