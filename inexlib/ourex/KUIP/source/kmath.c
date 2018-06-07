#ifndef lint
static const char kmath_sccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif

#include <stdlib.h>
#include <string.h>

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20090221

#define YYEMPTY        (-1)
#define kmath_clearin      (kmath_char = YYEMPTY)
#define kmath_errok        (kmath_errflag = 0)
#define YYRECOVERING() (kmath_errflag != 0)

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
#ifdef YYPARSE_PARAM_TYPE
#define YYPARSE_DECL() kmathparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
#else
#define YYPARSE_DECL() kmathparse(void *YYPARSE_PARAM)
#endif
#else
#define YYPARSE_DECL() kmathparse(void)
#endif /* YYPARSE_PARAM */

extern int YYPARSE_DECL();

static int kmath_growstack(void);
#define YYPREFIX "kmath_"
#line 40 "../source/kmath.yacc"

#include "kuip/kuip.h"
#include "kuip/kfor.h"
#include "kuip/kcom.h"
#include "kuip/kmenu.h"
#include "kuip/kflag.h"
#include "kuip/khash.h"
#include "kuip/kalias.h"
#include "kuip/klink.h"
#include "kuip/kmacro.h"
#include "kuip/kstrtod.h" /*G.Barrand*/

enum {
  IFUN_x,
  IFUN_xx,
  IFUN_Cx,
  IFUN_CC,
  IFUN_CCxx,
  IFUN_CCx3,
  IFUN_xCx,
  IFUN_xCC,
  IFUN_xCx8
};

typedef struct _user_fundef {
  char     *face_name;          /* function name, e.g. HEXIST */
  int       face_tok;
  char      face_type[16];
  int       face_const[16];
  char     *face_sval[16];
  double    face_dval[16];
  INT_FUNCPTR func;
  char     *call_name;
  int       call_tok;
  char      call_type[16];
  int       call_index[16];
  int       call_const[16];
  char     *call_sval[16];
  double    call_dval[16];
  char     *synopsis;
  char     *guidance;
} user_fundef;

static user_fundef user_functions[100]; /* should be plenty */


static char  *pline;
static int    kline;
static char  *result;
static int    iresult;
static double dresult;
static int    rsigma;
static int    wanted;

/*
 * The nonum flag is set in some mid-rule actions that kmathlex should
 * classify a numeric constant as STRING.
 */
static int    nonum;

static void  call_user_function( const char*, char**, double*, int*, int );
static int   count_words( const char*, const char* );
static char* extract_word( const char*, const char*, int );
static char* join_words( const char*, const char*, int, int );
static char* shell_line( const char*, int, const char* );
static char* zero_pad( char*, int );

extern int   kmathparse(void);

#ifdef ABS
#undef ABS
#endif

#line 115 "../source/kmath.yacc"
typedef union {
  double dval;
  char  *sval;
} YYSTYPE;
#line 112 "y.tab.c"
#define NUMBER 257
#define STRING 258
#define VNAME 259
#define S_USER 260
#define S_USER_S 261
#define S_USER_X 262
#define S_USER_X_S 263
#define X_USER_S 264
#define X_USER_X 265
#define X_USER_X_S 266
#define ABS 267
#define INT 268
#define MOD 269
#define ANAM 270
#define AVAL 271
#define QUOTE 272
#define UNQUOTE 273
#define ENV 274
#define FEXIST 275
#define FORMAT 276
#define INDEX 277
#define LEN 278
#define LOWER 279
#define UPPER 280
#define SUBSTRING 281
#define WORD 282
#define WORDS 283
#define SHELL 284
#define DEFINED 285
#define INLINE 286
#define iQUEST 287
#define SIGMA 288
#define RSIGMA 289
#define VEXIST 290
#define VDIM 291
#define VLEN 292
#define WANT_ANY 293
#define WANT_BOOL 294
#define WANT_MATH 295
#define LT 296
#define EQ 297
#define GT 298
#define PLUS 299
#define MINUS 300
#define STAR 301
#define SLASH 302
#define LPAR 303
#define RPAR 304
#define COMMA 305
#define OR 306
#define AND 307
#define NOT 308
#define LE 309
#define NE 310
#define GE 311
#define UPLUS 312
#define UMINUS 313
#define CONCAT 314
#define YYERRCODE 256
static const short kmath_lhs[] = {                           -1,
    0,    0,    0,    0,    0,    2,    2,    2,    2,    2,
    2,    2,    2,    2,    2,    2,    2,   10,    2,   11,
    2,    2,    2,   12,    2,   13,    2,    2,    2,    2,
    2,    2,    2,    2,   14,    2,    2,   15,    2,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,   16,
    1,    1,   17,    1,    1,    4,   18,    4,   19,    4,
    4,   20,    4,    4,   21,    4,    4,    4,   22,    4,
   23,    4,    4,   24,    4,   25,    4,    4,    4,    4,
   26,    4,   27,    4,   28,    4,    4,    4,    4,    4,
    4,   29,    5,   30,    7,   31,    6,   32,    3,   33,
    8,   34,    9,
};
static const short kmath_len[] = {                            2,
    2,    2,    2,    2,    2,    1,    3,    2,    2,    3,
    3,    3,    3,    3,    3,    3,    5,    0,    4,    0,
    6,    2,    4,    0,    4,    0,    4,    2,    4,    2,
    4,    3,    5,    7,    0,    4,    3,    0,    6,    3,
    2,    3,    3,    3,    3,    3,    3,    3,    3,    0,
    4,    3,    0,    4,    3,    1,    0,    4,    0,    4,
    2,    0,    4,    3,    0,    6,    3,    3,    0,    4,
    0,    4,    5,    0,    4,    0,    4,    2,    4,    6,
    0,    9,    0,    4,    0,    4,    3,    5,    2,    4,
    4,    0,    4,    0,    3,    0,    3,    0,    3,    0,
    3,    0,    3,
};
static const short kmath_defred[] = {                         0,
    0,    0,    0,    0,    6,   56,    0,    0,   62,    0,
    0,   35,    0,    0,    0,    0,    0,    0,    0,   74,
   76,   69,   24,    0,   20,   18,   83,   85,   92,   96,
   94,   98,   71,    0,   26,  100,  102,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   61,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    8,    9,    0,    0,    0,
    0,    0,   59,   89,    0,   57,    0,   78,    0,   22,
    0,   28,    0,   30,    0,    0,    0,   41,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   50,   53,   32,
    0,    0,    0,   64,   65,    0,   37,   38,   13,   14,
    0,   67,   68,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   15,    0,    0,
    0,    7,    0,    0,   12,   16,    0,    0,    0,    0,
   87,    0,    0,    0,    0,    0,   40,    0,   42,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   63,    0,   36,    0,    0,   75,   77,   70,   25,    0,
    0,   19,   84,   86,   93,   72,   27,   60,   91,   90,
   58,    0,   79,    0,   23,   29,   31,    0,    0,   33,
    0,    0,    0,   17,   73,    0,   88,    0,    0,   66,
   39,   21,   80,   81,   34,    0,    0,   82,
};
static const short kmath_dgoto[] = {                          4,
   51,  122,   42,   53,   44,   45,   46,   47,   48,   74,
   73,   71,   83,   60,  184,  178,  179,  160,  157,   57,
  182,   70,   81,   68,   69,  226,   75,   76,   77,   79,
   78,   80,   84,   85,
};
static const short kmath_sindex[] = {                       -53,
  -12,  -64, -161,    0,    0,    0, -161, -273,    0, -161,
 -161,    0, -161, -161, -161, -161, -161, -161, -161,    0,
    0,    0,    0, -161,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -161,    0,    0,    0, -161, -161, -161,
 -295, -172, -275, -161, -164, -150, -147, -141,  -64,  -64,
  -77, -149, -282, -156, -133,    0,  -12,  138, -126,  -12,
  144,  -68,  150,  156,  -16,  195,  205,  -12,  -12,  -12,
  -12,  131,  -12,  -12,  -12,  -12,  -12,  -12,  -12,  -12,
  -12,  211,  -12,  -12,  -12,    0,    0,  217, -161, -161,
 -161, -161,    0,    0,  -12,    0, -113,    0, -161,    0,
  -12,    0, -161,    0, -161, -122, -288,    0, -149,  -64,
  -64, -161,  -12, -161, -161,  -12, -161,    0,    0,    0,
 -161, -295, -280,    0,    0, -279,    0,    0,    0,    0,
 -161,    0,    0, -274, -268, -266, -262,  -12, -287, -260,
 -254, -233, -178, -275, -275, -275, -219,    0, -212, -275,
 -275,    0,  -25,  -25,    0,    0,  -12, -213, -205,  -12,
    0, -161,   93, -194,  223,  229,    0, -264,    0, -156,
 -295, -275, -156, -156, -295, -275, -156,  -12,  -12,  100,
    0,  -12,    0,  -12,  236,    0,    0,    0,    0, -193,
  -12,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  242,    0, -161,    0,    0,    0, -275, -275,    0,
 -161, -191, -186,    0,    0, -180,    0,  124,  248,    0,
    0,    0,    0,    0,    0,  -12, -179,    0,
};
static const short kmath_rindex[] = {                         0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   47,    0,   51,    0,    0,    0,    0,    0,    0,    0,
   55,   58,    0,   62,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  -23,  -11,   -9,    0,    0,    0,   -2,
    9,    0,    1,   20,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   17,    0,   29,
   33,   37,   41,   45,   49,   53,   57,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   61,   65,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
};
static const short kmath_gindex[] = {                         0,
  -47,  372,    0,   -1,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,
};
#define YYTABLESIZE 583
static const short kmath_table[] = {                         43,
   10,  106,  108,   89,   90,   91,   92,  112,  113,  114,
   89,   90,   91,   92,  118,  152,   43,  191,   93,   11,
  115,  116,  117,  181,  183,   93,   96,  119,   44,  186,
   56,   96,   48,   96,   96,  187,   52,  188,   96,   96,
   45,  189,  111,  192,   46,   96,    4,   96,   49,  193,
    5,   96,   55,   96,    1,  123,   47,    2,  126,   96,
   51,    3,  168,  169,   54,    0,  134,  135,  136,  137,
  194,  139,  140,  141,  142,  143,  144,  145,  146,  147,
   96,  149,  150,  151,  196,   89,   90,   91,   92,    0,
  199,  197,    0,  159,   96,    5,    0,    7,  200,  164,
   93,   96,   12,   13,   14,   15,   16,   17,   96,  205,
  215,  172,  220,   23,  176,   25,   26,  221,    0,   96,
   96,   31,   96,  222,  228,   34,  195,   96,   35,   36,
   37,   94,   95,   96,   96,   96,  190,   38,   39,   98,
   99,   40,   89,   90,   91,   92,  112,  113,  114,   89,
   90,   91,   92,  100,  101,  198,  102,  103,  201,  115,
  116,  117,  104,  105,   93,   89,   90,   91,   92,    0,
  120,  121,   89,   90,   91,   92,  208,  209,  125,    0,
  212,  167,  213,  110,  111,   89,   90,   91,   92,  216,
  161,  162,    5,    6,    7,    8,    9,   10,   11,   12,
   13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
   23,   24,   25,   26,   27,   28,   29,   30,   31,   32,
   33,    0,   34,    0,  227,   35,   36,   37,  110,  111,
   89,   90,   91,   92,   38,   39,  128,    0,   49,    1,
    2,    3,    0,   50,    5,    6,    7,    8,    9,   10,
   11,   12,   13,   14,   15,   16,   17,   18,   19,   20,
   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,
   31,   32,   33,    0,   34,   91,   92,   35,   36,   37,
   97,   97,   89,   90,   91,   92,   38,   39,  131,    0,
   40,    0,   95,   95,   99,   99,   10,   10,   10,   10,
   10,  101,  101,    0,   10,   10,   10,   10,    0,   10,
   10,   10,  103,  103,   10,   11,   11,   11,   11,   11,
   43,    0,   43,   11,   11,   11,   11,    0,   11,   11,
   11,    0,   44,   11,   44,   44,   48,    0,   48,   48,
   52,    0,   52,   52,   45,    0,   45,   45,   46,    0,
   46,   46,   49,    0,   49,   49,   55,    0,   55,   55,
   47,    0,   47,   47,   51,    0,   51,   51,   54,    0,
   54,   54,   41,   52,   54,    0,    0,    0,   55,    0,
    0,   58,   59,    0,   61,   62,   63,   64,   65,   66,
   67,   89,   90,   91,   92,   72,  203,  204,   89,   90,
   91,   92,    0,  210,  211,   82,    0,    0,    0,   86,
   87,   88,    0,    0,    0,   97,    0,    0,    0,    0,
  107,  109,   89,   90,   91,   92,    0,  223,  224,   89,
   90,   91,   92,    0,    0,  138,   89,   90,   91,   92,
    0,  124,   89,   90,   91,   92,    0,  127,   89,   90,
   91,   92,    0,  129,   89,   90,   91,   92,    0,  130,
  153,  154,  155,  156,    0,    0,  158,    0,    0,    0,
  163,    0,    0,    0,  165,    0,  166,    0,    0,    0,
    0,  109,  109,  170,  171,  173,  174,  175,  177,    0,
    0,    0,  180,   89,   90,   91,   92,    0,  132,    0,
    0,    0,  185,   89,   90,   91,   92,    0,  133,   89,
   90,   91,   92,    0,  148,   89,   90,   91,   92,    0,
  152,   89,   90,   91,   92,    0,  206,   89,   90,   91,
   92,    0,  207,  202,   89,   90,   91,   92,    0,  214,
   89,   90,   91,   92,    0,  217,   89,   90,   91,   92,
    0,  225,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  218,    0,    0,    0,    0,
    0,    0,  219,
};
static const short kmath_check[] = {                          1,
    0,   49,   50,  299,  300,  301,  302,  296,  297,  298,
  299,  300,  301,  302,  297,  304,    0,  305,  314,    0,
  309,  310,  311,  304,  304,  314,  314,  310,    0,  304,
  304,  314,    0,  314,  314,  304,    0,  304,  314,  314,
    0,  304,  307,  304,    0,  314,    0,  314,    0,  304,
    0,  314,    0,  314,    0,   57,    0,    0,   60,  314,
    0,    0,  110,  111,    0,   -1,   68,   69,   70,   71,
  304,   73,   74,   75,   76,   77,   78,   79,   80,   81,
  314,   83,   84,   85,  304,  299,  300,  301,  302,   -1,
  304,  304,   -1,   95,  314,  257,   -1,  259,  304,  101,
  314,  314,  264,  265,  266,  267,  268,  269,  314,  304,
  304,  113,  304,  275,  116,  277,  278,  304,   -1,  314,
  314,  283,  314,  304,  304,  287,  305,  314,  290,  291,
  292,  304,  305,  314,  314,  314,  138,  299,  300,  304,
  305,  303,  299,  300,  301,  302,  296,  297,  298,  299,
  300,  301,  302,  304,  305,  157,  304,  305,  160,  309,
  310,  311,  304,  305,  314,  299,  300,  301,  302,   -1,
  304,  305,  299,  300,  301,  302,  178,  179,  305,   -1,
  182,  304,  184,  306,  307,  299,  300,  301,  302,  191,
  304,  305,  257,  258,  259,  260,  261,  262,  263,  264,
  265,  266,  267,  268,  269,  270,  271,  272,  273,  274,
  275,  276,  277,  278,  279,  280,  281,  282,  283,  284,
  285,   -1,  287,   -1,  226,  290,  291,  292,  306,  307,
  299,  300,  301,  302,  299,  300,  305,   -1,  303,  293,
  294,  295,   -1,  308,  257,  258,  259,  260,  261,  262,
  263,  264,  265,  266,  267,  268,  269,  270,  271,  272,
  273,  274,  275,  276,  277,  278,  279,  280,  281,  282,
  283,  284,  285,   -1,  287,  301,  302,  290,  291,  292,
  304,  305,  299,  300,  301,  302,  299,  300,  305,   -1,
  303,   -1,  304,  305,  304,  305,  296,  297,  298,  299,
  300,  304,  305,   -1,  304,  305,  306,  307,   -1,  309,
  310,  311,  304,  305,  314,  296,  297,  298,  299,  300,
  304,   -1,  306,  304,  305,  306,  307,   -1,  309,  310,
  311,   -1,  304,  314,  306,  307,  304,   -1,  306,  307,
  304,   -1,  306,  307,  304,   -1,  306,  307,  304,   -1,
  306,  307,  304,   -1,  306,  307,  304,   -1,  306,  307,
  304,   -1,  306,  307,  304,   -1,  306,  307,  304,   -1,
  306,  307,    1,    2,    3,   -1,   -1,   -1,    7,   -1,
   -1,   10,   11,   -1,   13,   14,   15,   16,   17,   18,
   19,  299,  300,  301,  302,   24,  304,  305,  299,  300,
  301,  302,   -1,  304,  305,   34,   -1,   -1,   -1,   38,
   39,   40,   -1,   -1,   -1,   44,   -1,   -1,   -1,   -1,
   49,   50,  299,  300,  301,  302,   -1,  304,  305,  299,
  300,  301,  302,   -1,   -1,  305,  299,  300,  301,  302,
   -1,  304,  299,  300,  301,  302,   -1,  304,  299,  300,
  301,  302,   -1,  304,  299,  300,  301,  302,   -1,  304,
   89,   90,   91,   92,   -1,   -1,   95,   -1,   -1,   -1,
   99,   -1,   -1,   -1,  103,   -1,  105,   -1,   -1,   -1,
   -1,  110,  111,  112,  113,  114,  115,  116,  117,   -1,
   -1,   -1,  121,  299,  300,  301,  302,   -1,  304,   -1,
   -1,   -1,  131,  299,  300,  301,  302,   -1,  304,  299,
  300,  301,  302,   -1,  304,  299,  300,  301,  302,   -1,
  304,  299,  300,  301,  302,   -1,  304,  299,  300,  301,
  302,   -1,  304,  162,  299,  300,  301,  302,   -1,  304,
  299,  300,  301,  302,   -1,  304,  299,  300,  301,  302,
   -1,  304,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  204,   -1,   -1,   -1,   -1,
   -1,   -1,  211,
};
#define YYFINAL 4
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 314
#if YYDEBUG
static const char *kmath_name[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'","'*'","'+'","','","'-'",0,"'/'",0,0,0,0,0,0,0,0,0,0,0,0,
"'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,"NUMBER","STRING","VNAME","S_USER","S_USER_S","S_USER_X","S_USER_X_S",
"X_USER_S","X_USER_X","X_USER_X_S","ABS","INT","MOD","ANAM","AVAL","QUOTE",
"UNQUOTE","ENV","FEXIST","FORMAT","INDEX","LEN","LOWER","UPPER","SUBSTRING",
"WORD","WORDS","SHELL","DEFINED","INLINE","iQUEST","SIGMA","RSIGMA","VEXIST",
"VDIM","VLEN","WANT_ANY","WANT_BOOL","WANT_MATH","LT","EQ","GT","PLUS","MINUS",
"STAR","SLASH","LPAR","RPAR","COMMA","OR","AND","NOT","LE","NE","GE","UPLUS",
"UMINUS","CONCAT",
};
static const char *kmath_rule[] = {
"$accept : Line",
"Line : WANT_BOOL Bool",
"Line : WANT_BOOL Expr",
"Line : WANT_MATH Expr",
"Line : WANT_ANY Expr",
"Line : WANT_ANY String",
"Expr : NUMBER",
"Expr : LPAR Expr RPAR",
"Expr : PLUS Expr",
"Expr : MINUS Expr",
"Expr : Expr PLUS Expr",
"Expr : Expr MINUS Expr",
"Expr : Expr STAR Expr",
"Expr : ABS Expr RPAR",
"Expr : INT Expr RPAR",
"Expr : iQUEST Expr RPAR",
"Expr : Expr SLASH Expr",
"Expr : MOD Expr COMMA Expr RPAR",
"$$1 :",
"Expr : LEN $$1 String RPAR",
"$$2 :",
"Expr : INDEX $$2 String COMMA String RPAR",
"Expr : Words RPAR",
"Expr : Words COMMA String RPAR",
"$$3 :",
"Expr : FEXIST $$3 String RPAR",
"$$4 :",
"Expr : VEXIST $$4 String RPAR",
"Expr : Vdim RPAR",
"Expr : Vdim COMMA Expr RPAR",
"Expr : Vlen RPAR",
"Expr : Vlen COMMA Expr RPAR",
"Expr : VNAME Expr RPAR",
"Expr : VNAME Expr COMMA Expr RPAR",
"Expr : VNAME Expr COMMA Expr COMMA Expr RPAR",
"$$5 :",
"Expr : X_USER_S $$5 String RPAR",
"Expr : X_USER_X Expr RPAR",
"$$6 :",
"Expr : X_USER_X_S Expr COMMA $$6 String RPAR",
"Bool : LPAR Bool RPAR",
"Bool : NOT Bool",
"Bool : Bool AND Bool",
"Bool : Bool OR Bool",
"Bool : Expr LT Expr",
"Bool : Expr GT Expr",
"Bool : Expr LE Expr",
"Bool : Expr GE Expr",
"Bool : Expr EQ Expr",
"Bool : Expr NE Expr",
"$$7 :",
"Bool : String EQ $$7 String",
"Bool : Expr EQ String",
"$$8 :",
"Bool : String NE $$8 String",
"Bool : Expr NE String",
"String : STRING",
"$$9 :",
"String : String CONCAT $$9 String",
"$$10 :",
"String : Expr CONCAT $$10 String",
"String : S_USER RPAR",
"$$11 :",
"String : S_USER_S $$11 String RPAR",
"String : S_USER_X Expr RPAR",
"$$12 :",
"String : S_USER_X_S Expr COMMA $$12 String RPAR",
"String : ANAM Expr RPAR",
"String : AVAL Expr RPAR",
"$$13 :",
"String : ENV $$13 String RPAR",
"$$14 :",
"String : DEFINED $$14 String RPAR",
"String : FORMAT Expr COMMA String RPAR",
"$$15 :",
"String : QUOTE $$15 String RPAR",
"$$16 :",
"String : UNQUOTE $$16 String RPAR",
"String : Word RPAR",
"String : Word COMMA Expr RPAR",
"String : Word COMMA Expr COMMA Expr RPAR",
"$$17 :",
"String : Word COMMA Expr COMMA Expr COMMA $$17 String RPAR",
"$$18 :",
"String : LOWER $$18 String RPAR",
"$$19 :",
"String : UPPER $$19 String RPAR",
"String : Substring Expr RPAR",
"String : Substring Expr COMMA Expr RPAR",
"String : Shell RPAR",
"String : Shell COMMA String RPAR",
"String : Shell COMMA Expr RPAR",
"$$20 :",
"Substring : SUBSTRING $$20 String COMMA",
"$$21 :",
"Words : WORDS $$21 String",
"$$22 :",
"Word : WORD $$22 String",
"$$23 :",
"Shell : SHELL $$23 String",
"$$24 :",
"Vdim : VDIM $$24 String",
"$$25 :",
"Vlen : VLEN $$25 String",

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

int      kmath_debug;
int      kmath_nerrs;
int      kmath_errflag;
int      kmath_char;
short   *kmath_ssp;
YYSTYPE *kmath_vsp;
YYSTYPE  kmath_val;
YYSTYPE  kmathlval;

/* variables for the parser stack */
static short   *kmath_ss;
static short   *kmath_sslim;
static YYSTYPE *kmath_vs;
static unsigned kmath_stacksize;
#line 856 "../source/kmath.yacc"


/*
 * execute a shell command and return the n'th line of its output
 * if n<=0 return complete output with newlines replaced by blanks
 */
static
char *shell_line( const char *cmd,
                 int n,
                 const char *sep )
{
#ifdef UNIX
  FILE *stream;
  char *shell_cmd;
  int status;

#ifdef MSDOS
  shell_cmd = str3dup( cmd, " >", kc_flags.appl_file );
#else
  /* protect against noclobber option */
  shell_cmd = str5dup( "cp /dev/null ", kc_flags.appl_file, " ; rm ",
                      kc_flags.appl_file, " ; ( " );
  shell_cmd = mstr3cat( shell_cmd, cmd, " ) > ", kc_flags.appl_file );
#endif

  status = shsystem( kc_value.set_host_shell, shell_cmd );
  free( shell_cmd );
  if( status == -1 ) {
    perror( "$SHELL" );
  }
  else if( (stream = fopen( kc_flags.appl_file, "r" )) != NULL ) {
    char line[1024];
    char *value = NULL;

    if( sep == NULL ) {
      while( --n > 0 ) {
        if(fgets( line, (sizeof line), stream )) {} /*G.Barrand*/
      }
      if( fgets( line, (sizeof line), stream ) != NULL ) {
        int len = strlen( line );
        line[len-1] = '\0';
        value = strdup( line );
      }
    }
    else {
      while( fgets( line, (sizeof line), stream ) != NULL ) {
        int len = strlen( line );
        line[len-1] = '\0';
        if( value == NULL ) {
          value = strdup( line );
        }
        else {
          value = mstr2cat( value, sep, line );
        }
      }
    }
    fclose( stream );
    remove( kc_flags.appl_file );
    return value;
  }
#endif

  return strdup( "" );
}


static
int count_words( const char *line,
                const char *sep )
{
  int n = 0;
  while( 1 ) {
    char *p = extract_word( line, sep, n + 1 );
    if( *p != '\0' ) {
      free( p );
      n++;
    }
    else {
      free( p );
      break;
    }
  }
  return n;
}


static
char *extract_word( const char *line,
                   const char *sep,
                   int k )
{
  const char *p = line;
  int n = 0;
  if( *sep != '\0' ) {
    while( k-- > 0 ) {
      p += n;
      while( *p != '\0' && strchr( sep, *p ) != NULL )
        p++;
      n = 0;
      while( strchr( sep, p[n] ) == NULL )
        n++;
    }
  }
  return kuip_strndup( p, n );
}


static
char *join_words( const char *line,
                 const char *sep,
                 int k,
                 int n )
{
  char *words = strdup( "" );
  while( n-- > 0 ) {
    char *p = extract_word( line, sep, k++ );
    if( p[0] != '\0' ) {
      if( words[0] != '\0' )
        words = mstrcat( words, sep );
      words = mstrcat( words, p );
      free( p );
    }
    else {
      free( p );
      break;
    }
  }
  return words;
}


/*
 * VAX C printf() does not understand "%5.3d" to do zero padding
 * and we have to do it by hand.
 */
static
char* zero_pad( char *format,
               int m )
{
  int n = strlen( format );
  int minus = 0;

  while( --n >= 0 && --m >= 0 ) {
    if( format[n] == '-' ) {
      minus = 1;
      format[n] = '0';
    }
    else if( format[n] == ' ' )
      format[n] = '0';
  }

  if( minus ) {
    if( n >= 0 )
      format[n] = '-';
    else {
      char *p = str2dup( "-", format );
      free( format );
      return p;
    }
  }
  return format;
}


/*
 * enter a user define system function
 * chname is of the form "<T>=<name>(<T><T>...)"
 */
void F77_ENTRY_C4E(Kufdef,chsyn,chguid,chface,chcall,fun)
/* { */
  char *face = fstrdup( chface, len_chface );
  char *call = fstrdup( chcall, len_chcall );
  int i;

  for( i = 0; i < sizeof(user_functions) / sizeof(user_fundef) - 1; i++ ) {

    if( user_functions[i].face_tok == 0 ) {
      char face_class[16];
      char call_class[16];
      int argc;
      char *p;
      int n;

      user_functions[i].face_name = face + 2;
      user_functions[i].face_type[0] = face[0];
      user_functions[i].face_sval[0] = NULL;
      p = strchr( face, ')' );
      *p = ',';
      p = strchr( face, '(' );
      *p++ = '\0';

      if( *p == ',' ) {
        n = 1;                  /* no arguments */
      }
      else {
        for( n = 1; *p != '\0'; n++ ) {
          char *argv = NULL;

          user_functions[i].face_type[n] = *p;
          if( p[1] == '=' ) {
            char *s = strchr( p, ',' );
            *s = '\0';
            argv = p + 2;
            p = s;
          }
          else {
            p += 2;
          }
          user_functions[i].face_sval[n] = argv;
        }
      }
      user_functions[i].face_type[n] = '\0';
      argc = n;

      for( n = 0; n < argc; n++ ) {
        char *argv = user_functions[i].face_sval[n];

        if( strchr( "stuS", user_functions[i].face_type[n] ) != NULL ) {
          face_class[n] = 'S';
          if( argv != NULL )
            user_functions[i].face_const[n] = 1;
        }
        else {
          face_class[n] = 'X';
          if( argv != NULL ) {
            user_functions[i].face_const[n] = 2;
            user_functions[i].face_dval[n] = atof( argv );
          }
        }
      }
      face_class[n] = '\0';

      if( strcmp( face_class, "S" ) == 0 ) {
        user_functions[i].face_tok = S_USER;
      }
      else if( strcmp( face_class, "SS" ) == 0 ) {
        user_functions[i].face_tok = S_USER_S;
      }
      else if( strcmp( face_class, "SX" ) == 0 ) {
        user_functions[i].face_tok = S_USER_X;
      }
      else if( strcmp( face_class, "SXS" ) == 0 ) {
        user_functions[i].face_tok = S_USER_X_S;
      }
      else if( strcmp( face_class, "XS" ) == 0 ) {
        user_functions[i].face_tok = X_USER_S;
      }
      else if( strcmp( face_class, "XX" ) == 0 ) {
        user_functions[i].face_tok = X_USER_X;
      }
      else if( strcmp( face_class, "XXS" ) == 0 ) {
        user_functions[i].face_tok = X_USER_X_S;
      }
      else {
        kprintf( " *** KUFDEF: %s face class %s not implemented\n",
               user_functions[i].face_name, face_class );
        return;
      }

      user_functions[i].func = (INT_FUNCPTR)fun;
      user_functions[i].call_name = call + 2;
      user_functions[i].call_type[0] = call[0];
      user_functions[i].call_sval[0] = NULL;

      p = strchr( call, ')' );
      *p = ',';
      p = strchr( call, '(' );
      *p++ = '\0';
      for( n = 1; *p != '\0'; n++ ) {
        char *argv = NULL;

        user_functions[i].call_type[n] = *p;
        if( isdigit( p[1] ) ) {
          user_functions[i].call_index[n] = p[1] - '0';
          p++;
        }

        if( p[1] == '=' ) {
          char *s = strchr( p, ',' );
          *s = '\0';
          argv = p + 2;
          p = s;
        }
        else {
          p += 2;
        }
        user_functions[i].call_sval[n] = argv;
      }
      user_functions[i].call_type[n] = '\0';
      argc = n;

      for( n = 0; n < argc; n++ ) {
        char *argv = user_functions[i].call_sval[n];

        if( strchr( "stuC$", user_functions[i].call_type[n] ) != NULL ) {
          call_class[n] = 'S';
          if( argv != NULL )
            user_functions[i].call_const[n] = 1;
        }
        else {
          call_class[n] = 'X';
          if( argv != NULL ) {
            user_functions[i].call_const[n] = 2;
            user_functions[i].call_dval[n] = atof( argv );
          }
        }
      }
      call_class[n] = '\0';

      if( strcmp( call_class, "XX" ) == 0 ) {
        user_functions[i].call_tok = IFUN_x;
      }
      else if( strcmp( call_class, "XXX" ) == 0 ) {
        user_functions[i].call_tok = IFUN_xx;
      }
      else if( strcmp( call_class, "XSX" ) == 0 ) {
        user_functions[i].call_tok = IFUN_Cx;
      }
      else if( strcmp( call_class, "XSS" ) == 0 ) {
        user_functions[i].call_tok = IFUN_CC;
      }
      else if( strcmp( call_class, "XSSXX" ) == 0 ) {
        user_functions[i].call_tok = IFUN_CCxx;
      }
      else if( strcmp( call_class, "XSSXXX" ) == 0 ) {
        user_functions[i].call_tok = IFUN_CCx3;
      }
      else if( strcmp( call_class, "XXSX" ) == 0 ) {
        user_functions[i].call_tok = IFUN_xCx;
      }
      else if( strcmp( call_class, "XXSS" ) == 0 ) {
        user_functions[i].call_tok = IFUN_xCC;
      }
      else if( strcmp( call_class, "XXSXXXXXXXX" ) == 0 ) {
        user_functions[i].call_tok = IFUN_xCx8;
      }
      else {
        kprintf( " *** KUFDEF: %s call class %s not implemented\n",
               user_functions[i].call_name, call_class );
        return;
      }
      user_functions[i].synopsis = fstrdup( chsyn, len_chsyn );
      user_functions[i].guidance = fstrdup( chguid, len_chguid );
      return;
    }
  }
  kprintf( " *** KUFDEF: too many definitions --- %s ignored\n", face );
}


/*
 * User help routine for /KUIP/FUNCTIONS
 */
char **kxfunclist()
{
  char **text = (char**)malloc( sizeof(char*) );
  int k = 0;
  int n;

  for( n = 0; user_functions[n].face_tok != 0; n++ ) {  /*G.Barrand : strange loop ?*/

    if( user_functions[n].synopsis[0] != '\0'
       || user_functions[n].guidance[0] != '\0' ) {
      int len = strlen( user_functions[n].synopsis );

      text[k] = str3dup( " ", user_functions[n].synopsis, "  " );
      text[k] = mstrccat( text[k], '.', 28 - len );
      text[k] = mstr2cat( text[k], "  ", user_functions[n].guidance );
      text = (char**)realloc( (char*)text, (k+2) * sizeof( char* ) );
      k++;
    }
  }
  text[k] = NULL;

  return text;
}


void kxdelfuncs() /*G.Barrand*/
{
  int number,n;
  number = sizeof(user_functions) / sizeof(user_fundef);
  for(n=0;n< number;n++) {
    if( user_functions[n].face_tok == 0 ) continue;
    free(user_functions[n].face_name-2);
    free(user_functions[n].call_name-2);
    free(user_functions[n].synopsis);
    free(user_functions[n].guidance);
  }
}


static
void call_user_function( const char *name,
                        char **slist,
                        double *dlist,
                        int *which,
                        int npar )
{
  user_fundef *ufun = NULL;
  char   *sval[16];
  double  dval[16];
  EQUIV_INT_REAL xval[16][10];
  char      cval[16][256];
  char *p;
  char c;
  int i;
  int j;

  for( i = 0; user_functions[i].face_tok != 0; i++ ) {

    if( ku_strcasecmp( user_functions[i].face_name, name ) == 0 ) {

      ufun = &user_functions[i];
      for( j = 1; j <= npar; j++ ) {
        if( ufun->face_const[j] == 1
           && ku_strcasecmp( ufun->face_sval[j], slist[j] ) != 0 ) {
          ufun = NULL;
          break;
        }
        if( ufun->face_const[j] == 2
           && ufun->face_dval[j] != dlist[j] ) {
          ufun = NULL;
          break;
        }
      }

      if( ufun != NULL )
        break;
    }
  }

  if( ufun == NULL ) {
    char *comma = "";
    kprintf( " *** Invalid argument in function %s(", name );
    for( i = 1; i <= npar; i++ ) {
      if( which[i] == 1 )
        kprintf( "%s%s", comma, slist[i] );
      else
        kprintf( "%s%s", comma, strfromd( dlist[i], 0 ) );
      comma = ",";
    }
    kprintf( ")\n" );

    if( which[0] == 1 )
      slist[0] = str2dup( name, "(?)" );
    else
      dlist[0] = 1.2345e6;
    return;
  }

  for( i = 1; ufun->call_type[i] != '\0'; i++ ) {
    sval[i] = ufun->call_sval[i];
    dval[i] = ufun->call_dval[i];
  }

  for( i = 1; (c = ufun->face_type[i]) != '\0'; i++ ) {

    if( (p = strchr( ufun->call_type, c )) != NULL ) {
      int n = p - ufun->call_type;

      sval[n] = slist[i];
      dval[n] = dlist[i];
    }
  }

  for( i = 1; (c = ufun->call_type[i]) != '\0'; i++ ) {

    if( strchr( "stuC$", c ) != NULL ) {
      char *s = (sval[i] != NULL) ? sval[i] : "";
      if( c == '$' )
        s = ufun->call_name;
      else if( c == 'u' )
        strupper( s );
      fstrset( cval[i], 256, s );
    }
    else if( strchr( "ijkI", c ) != NULL ) {
      xval[i][0].i = dval[i];
    }
    else if( strchr( "lmnL", c ) != NULL ) {
      xval[i][0].l = ku_true( dval[i] != 0 );
    }
    else if( strchr( "xyzR", c ) != NULL ) {
      xval[i][0].r = dval[i];
    }
  }

  switch( ufun->call_tok ) {

  case IFUN_x:
    F77_IFUN_x(xval[0][0].i,ufun->func,xval[1]);
    break;

  case IFUN_xx:
    F77_IFUN_xx(xval[0][0].i,ufun->func,xval[1],xval[2]);
    break;

  case IFUN_Cx:
    F77_IFUN_Cx(xval[0][0].i,ufun->func,cval[1],256,xval[2]);
    break;

  case IFUN_CC:
    F77_IFUN_CC(xval[0][0].i,ufun->func,cval[1],256,cval[2],256);
    break;

  case IFUN_CCxx:
    F77_IFUN_CCxx(xval[0][0].i,ufun->func,cval[1],256,cval[2],256,
                  xval[3],xval[4]);
    break;

  case IFUN_CCx3:
    F77_IFUN_CCx3(xval[0][0].i,ufun->func,cval[1],256,cval[2],256,
                  xval[3],xval[4],xval[5]);
    break;

  case IFUN_xCx:
    F77_IFUN_xCx(xval[0][0].i,ufun->func,xval[1],cval[2],256,xval[3]);
    break;

  case IFUN_xCC:
    F77_IFUN_xCC(xval[0][0].i,ufun->func,xval[1],cval[2],256,cval[3],256);
    break;

  case IFUN_xCx8:
    F77_IFUN_xCx8(xval[0][0].i,ufun->func,xval[1],cval[2],256,
                  xval[3],xval[4],xval[5],xval[6],
                  xval[7],xval[8],xval[9],xval[10]);
    break;
  }

  c = ufun->face_type[0];
  p = strchr( ufun->call_type, c );
  i = p - ufun->call_type;
  j = ufun->call_index[i];

  if( strchr( "stu", c ) != NULL ) {
    slist[0] = fstrdup( cval[i], 256 );
  }
  else if( strchr( "ijk", c ) != NULL ) {
    dlist[0] = xval[i][j].i;
  }
  else if( strchr( "lmn", c ) != NULL ) {
    dlist[0] = (xval[i][j].l != ku_true( 0 ));
  }
  else if( strchr( "xyz", c ) != NULL ) {
    dlist[0] = xval[i][j].r;
  }
  else if( strchr( "def", c ) != NULL ) {
    DBLPREC dbl;
    memcpy( (void*)&dbl, (void*)xval[i], sizeof(DBLPREC) );
    dlist[0] = dbl;
  }
}


/*
 * Replace $EXEC functions which we can't leave to the non-reentrent parser.
 */
char *repl_exec( const char *line )
{
  int quoted = 0;
  const char *p;

  for( p = line; *p != '\0'; p++ ) {
    int n;

    if( *p == '\'' ) {
      quoted = !quoted;
    }
    else if( !quoted && ku_strncasecmp( p, "$EXEC(", 6 ) == 0
            && (n = match_paren( p )) > 0 ) {

      char *args = clean_word( kuip_strndup( p + 6, n - 7 ) );
      char *cmd = str2dup( "EXEC ", args );
      char *new_line = kuip_strndup( line, p - line );
      char *old_value = strdup(khash_lookup( kc_alias.var_table, "@", NULL ) );
      char *value;

      ku_exel( cmd );
      free( args );
      free( cmd );

      value = quote_string( strdup( khash_lookup( kc_alias.var_table,
                                                "@", NULL ) ), 0 );
      store_variable( NULL, "@", old_value );
      free( old_value );

      new_line = mstr2cat( new_line, value, p + n );
      free( value );

      return new_line;
    }
  }
  return NULL;
}


char *subst_exec( const char *line )
{
  char *ldup = strdup( line );
  char *p;

  while( (p = repl_exec( ldup )) != NULL ) {
    free( ldup );
    ldup = p;
  }
  return ldup;
}


/*
 * Evaluate an expression and return the result as an allocated string.
 * If the expression cannot be parsed a NULL string is returned.
 */
char *ku_eval( const char *line )
{
  char *val = NULL;

  /* pline must be the first of these static assignments !!! */
  pline = subst_exec( line );
  kline = 0;
  nonum = 0;
  rsigma = 0;
  wanted = WANT_ANY;

  if( kmathparse() == 0 )
    val = result;

  free( pline );
  return val;
}


/*
 * Evaluate numerical expressions
 */
int ku_math( const char *line, double *dval )
{
  int status = -1;

  /* pline must be the first of these static assignments !!! */
  pline = subst_exec( line );
  kline = 0;
  nonum = 0;
  rsigma = 0;
  wanted = WANT_MATH;

  if( kmathparse() == 0 ) {
    status = 0;
    *dval = dresult;
  }

  free( pline );
  return status;
}


/*
 * Evaluate boolean expressions
 * Return -1 if invalid expression
 */
int ku_bool( const char *line )
{
  int _bool = -1;

  /* pline must be the first of these static assignments !!! */
  pline = subst_exec( line );
  kline = 0;
  nonum = 0;
  rsigma = 0;
  wanted = WANT_BOOL;

  if( kmathparse() == 0 )
    _bool = iresult;

  free( pline );
  return _bool;
}


/*
 * Evaluate expression.
 * For invalid expressions variable, alias, and system function substitution.
 */
char *ku_expr( const char *line )
{
  char *value = ku_eval( line );

  if( value == NULL ) {
    char *p;
    value = strdup( line );

    kc_alias.substitutions = 100; /* protect against recursive aliases */

    if( (p = subst_var_alias( value )) != NULL ) {
      free( value );
      value = p;
    }

    if( (p = subst_arg_alias( value )) != NULL ) {
      free( value );
      value = p;
    }

    if( (p = subst_sysfun( value, 1 )) != NULL ) {
      free( value );
      value = p;
    }
  }
  return value;
}


static
int kmathlex()
{
  static struct tokens {
    char *tok;
    int   val;
  } op[] = {
    { "//",      CONCAT},
    { ".NOT.",   NOT   },
    { ".AND.",   AND   },
    { ".OR.",    OR    },
    { ".LE.",    LE    },  { "<=", LE  },
    { ".GE.",    GE    },  { ">=", GE  },
    { ".NE.",    NE    },  { "<>", NE  },
    { ".EQ.",    EQ    },
    { ".LT.",    LT    },
    { ".GT.",    GT    }
  }, fun[] = {
    { "ABS",        ABS        },
    { "INT",        INT        },
    { "MOD",        MOD        },
    { "$ANAM",      ANAM       },
    { "$AVAL",      AVAL       },
    { "$DEFINED",   DEFINED    },
    { "$ENV",       ENV        },
    { "$FEXIST",    FEXIST     },
    { "$FORMAT",    FORMAT     },
    { "$INDEX",     INDEX      },
    { "$INLINE",    INLINE     },
    { "$IQUEST",    iQUEST     },
    { "$LEN",       LEN        },
    { "$LOWER",     LOWER      },
    { "$QUOTE",     QUOTE      },
    { "$RSIGMA",    RSIGMA     },
    { "$SHELL",     SHELL      },
    { "$SIGMA",     SIGMA      },
    { "$SUBSTRING", SUBSTRING  },
    { "$UNQUOTE",   UNQUOTE    },
    { "$UPPER",     UPPER      },
    { "$VDIM",      VDIM       },
    { "$VEXIST",    VEXIST     },
    { "$VLEN",      VLEN       },
    { "$WORD",      WORD       },
    { "$WORDS",     WORDS      },
  }, single[] = {
     { "<",      LT    },
     { "=",      EQ    },
     { ">",      GT    },
     { "+",      PLUS  },
     { "-",      MINUS },
     { "*",      STAR  },
     { "/",      SLASH },
     { "(",      LPAR  },
     { ")",      RPAR  },
     { ",",      COMMA }
  };
  char *str;
  char *p;
  int c;
  int i;
  int n;

  if( wanted ) {
    int w  = wanted;
    wanted = 0;
    return w;
  }

  while( (c = pline[kline]) == ' ' || c == '\t' )
    kline++;                    /* ignore spaces */

  if( c == '\0' )
    return 0;

  if( c == '\'' ) {             /* quoted string */
    int quoted = 1;
    str = NULL;

    for( n = kline + 1; pline[n] != '\0'; n++ ) {
      if( pline[n] == '\'' ) {
        quoted = !quoted;
        if( !quoted && pline[n+1] != '\'' ) {
          str = kuip_strndup( &pline[kline+1], n - kline - 1 );
          break;
        }
      }
    }
    if( str != NULL ) {         /* remove quotes */
      char *s;
      for( p = str; (s = strstr( p, "''" )) != NULL; p = s + 1 )
        kuip_strcpy( s, s + 1 ); /*GB*/
      kmathlval.sval = str;
      kline = n + 1;
      return STRING;
    }
  }

  /* check for macro variable */
  if( c == '[' ) {
    /*
     * Isolate the variable name and classify the translation
     * as number or string.
     */
    int paren = 1;

    for( p = pline + kline + 1; *p != '\0'; p++ ) {

      if( *p == '[' ) {
        paren++;
      }
      else if( *p == ']' ) {

        if( --paren == 0 ) {
          int len = p - &pline[kline] + 1;
          char *name = kuip_strndup( &pline[kline], len );
          char *value = var_value( name );
          free( name );

          kline += len;

          /* e.g. for $UPPER([s]) do not try classify [s] as a number */
          if( !nonum ) {
            char *tail;
            kmathlval.dval = fstrtod( value, &tail );
            if( *tail == '\0' ) {
              free( value );
              return NUMBER;
            }
            /* allow "[vname]([i])" */
            if( ku_vtype( value ) != 0 ) {
              /*
               * test if name is followed by '('
               * otherwise it must be a single element vector for which we
               * return the immediate value
               */
              p = &pline[kline];
              while( *p == ' ' )
                p++;
              if( *p == '(' ) {
                kline = p - pline + 1;
                kmathlval.sval = value;
                return VNAME;
              }
              else if( ku_vvalue( value, &kmathlval.dval ) == 0 ) {
                kline = p - pline;
                free( value );
                return NUMBER;
              }
            }
          }
          kmathlval.sval = value;
          return STRING;
        }
      }
    }
  }

  /* numbers */
  if( !nonum && ( isdigit( c )
                 || ( c == '.' && isdigit( pline[kline+1] ) ) ) ) {
    char *tail;
    str = pline + kline;
    kmathlval.dval = kstrtod( str, &tail ); /*G.Barrand*/
    if( tail != str ) {
      kline += tail - str;
      return NUMBER;
    }
  }

  /* multiple character operators */
  for( i = 0; i < (sizeof op) / sizeof( struct tokens ); i++ ) {
    n = strlen( op[i].tok );

    if( ku_strncasecmp( &pline[kline], op[i].tok, n ) == 0 ) {
      kline += n;
      return op[i].val;
    }
  }

  /* single character operators */
  for( i = 0; i < (sizeof single) / sizeof( struct tokens ); i++ ) {
    if( single[i].tok[0] == c ) {
      kline++;
      return single[i].val;
    }
  }

  /* alias names */
  n = len_alias( pline + kline, (kline > 0) ? pline[kline-1] : '\0' );
  if( n > 0 ) {
    char *name = kuip_strndup( &pline[kline], n );
    if( (p = subst_arg_alias( name )) != NULL ) {
      free( name );
      kline += n;
      if( !nonum ) {
        char *tail;
        kmathlval.dval = fstrtod( p, &tail );
        if( *tail == '\0' ) {
          free( p );
          return NUMBER;
        }
      }
      kmathlval.sval = p;
      return STRING;
    }
    free( name );
  }

  /* vector names */
  if( !nonum ) {
    n = len_vector( pline + kline );
    if( n > 0 ) {
      char *name = kuip_strndup( &pline[kline], n );
      if( ku_vtype( name ) != 0 ) {
        /*
         * test if name is followed by '('
         * otherwise it must be a single element vector for which we
         * return the immediate value
         */
        p = &pline[kline+n];
        while( *p == ' ' )
          p++;
        if( *p == '(' ) {
          kline = p - pline + 1;
          kmathlval.sval = name;
          return VNAME;
        }
        else if( ku_vvalue( name, &kmathlval.dval ) == 0 ) {
          kline = p - pline;
          free( name );
          return NUMBER;
        }
      }
      free( name );
    }
  }

  /* user defined functions */
  for( i = 0; user_functions[i].face_tok != 0; i++ ) {
    n = strlen( user_functions[i].face_name );

    if( ku_strncasecmp( pline + kline, user_functions[i].face_name, n ) == 0 ) {

      while( pline[kline+n] == ' ' )
        n++;

      if( pline[kline+n] == '(' ) {
        kline += n + 1;
        kmathlval.sval = user_functions[i].face_name;
        return user_functions[i].face_tok;
      }
    }
  }

  /* system functions with arguments */
  for( i = 0; i < (sizeof fun) / sizeof( struct tokens ); i++ ) {
    int n = strlen( fun[i].tok );

    if( ku_strncasecmp( &pline[kline], fun[i].tok, n ) == 0 ) {
      int k = kline + n;

      while( pline[k] == ' ' || pline[k] == '\t' )
        k++;

      if( pline[k] == '(' ) {

        if( fun[i].val == SIGMA || fun[i].val == RSIGMA
           || fun[i].val == INLINE ) {
          /*
           * SIGMA expressions do not follow our syntax rules.
           * We isolate the SIGMA expression by looking for the matching ")".
           * Before calling SIGMA we have to substitute macro variables and
           * system functions without arguments.  We cannot substitute
           * system functions with arguments because this is done by the
           * expression parser which is not reentrant.
           * The result is returned as a vector name.
           *
           * INLINE allows textual substitution in an expression.
           * The argument is alias and variable expanded and replaced in the
           * expression line.
           */
          int len = match_paren( pline + k );

          if( len > 0 ) {
            char *expr = kuip_strndup( pline + k + 1, len - 2 );

            if( (p = subst_arg_alias( expr )) != NULL ) {
              free( expr );
              expr = p;
            }
            if( (p = subst_var_alias( expr )) != NULL ) {
              free( expr );
              expr = p;
            }
            if( (p = subst_sysfun( expr, 0 )) != NULL ) {
              free( expr );
              expr = p;
            }

            if( fun[i].val == INLINE ) {
              p = kuip_strndup( pline, kline );
              p = mstrcat( p, expr );
              free( expr );
              p = mstrcat( p, pline + k + len );
              free( pline );
              pline = p;
              return kmathlex();
            }
            else if( kjmpaddr.user_sigma_F != NULL ) {
              char vname[16]; /* temporary sigma vector */
              int status;
              sprintf( vname, "?SIG%d", ++kc_flags.temp_vectors );
              p = str3dup( vname, "=", expr );
              free( expr );
	      status = (*kjmpaddr.user_sigma_F)(p,kjmpaddr.user_sigma_F_data);
        /*GB  fstrset( Sikuip.CHSIGM, (sizeof Sikuip.CHSIGM), p ); */
              free( p );

        /*GB  (*kjmpaddr.user_sigma_F)(); */
        /*GB  if( IQUEST(1) != 0 ) { */
              if( status != 0 ) {
                kc_flags.temp_vectors--;
              }
              else {
                rsigma = (fun[i].val == RSIGMA);
                kline = k + len;
                if( ku_vvalue( vname, &kmathlval.dval ) == 0 )
                  return NUMBER;
                kmathlval.sval = strdup( vname );
                return STRING;
              }
            }
          }
        }
        else {
          kline = k + 1;
          return fun[i].val;
        }
      }
    }
  }

  /* system functions without arguments */
  if( c == '$' ) {
    if( (n = len_sysfun( pline + kline + 1 )) > 0 ) {
      char *name = kuip_strndup( pline + kline, n + 1 );
      char *value = repl_sysfun( name, 0 );
      free( name );
      if( value != NULL ) {
        kline += n + 1;
        if( !nonum ) {
          char *tail;
          kmathlval.dval = fstrtod( value, &tail );
          if( *tail == '\0' ) {
            free( value );
            return NUMBER;
          }
        }
        kmathlval.sval = value;
        return STRING;
      }
    }
  }

  /* classify other stuff as unquoted string */
  n = 1;
  while( strchr( " =,)[", pline[kline+n] ) == NULL )
    n++;

  str = strupper( kuip_strndup( pline + kline, n ) );

  /* check that there is no operator before the end of the string */
  for( i = 0; i < (sizeof op) / sizeof( struct tokens ); i++ ) {
    if( (p = strstr( str, op[i].tok )) != NULL ) {
      *p = '\0';
    }
  }
  n = strlen( str );
  free( str );

  kmathlval.sval = kuip_strndup( &pline[kline], n );
  kline += n;
  return STRING;
}


static
void kmatherror(const char* mess)
{
  /* ignore */
}
#line 1663 "y.tab.c"
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int kmath_growstack(void)
{
    int i;
    unsigned newsize;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = kmath_stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = kmath_ssp - kmath_ss;
    newss = (kmath_ss != 0)
          ? (short *)realloc(kmath_ss, newsize * sizeof(*newss))
          : (short *)malloc(newsize * sizeof(*newss));
    if (newss == 0)
        return -1;

    kmath_ss  = newss;
    kmath_ssp = newss + i;
    newvs = (kmath_vs != 0)
          ? (YYSTYPE *)realloc(kmath_vs, newsize * sizeof(*newvs))
          : (YYSTYPE *)malloc(newsize * sizeof(*newvs));
    if (newvs == 0)
        return -1;

    kmath_vs = newvs;
    kmath_vsp = newvs + i;
    kmath_stacksize = newsize;
    kmath_sslim = kmath_ss + newsize - 1;
    return 0;
}

#define YYABORT  goto kmath_abort
#define YYREJECT goto kmath_abort
#define YYACCEPT goto kmath_accept
#define YYERROR  goto kmath_errlab

int
YYPARSE_DECL()
{
    int kmath_m, kmath_n, kmath_state;
#if YYDEBUG
    const char *kmath_s;

    if ((kmath_s = getenv("YYDEBUG")) != 0)
    {
        kmath_n = *kmath_s;
        if (kmath_n >= '0' && kmath_n <= '9')
            kmath_debug = kmath_n - '0';
    }
#endif

    kmath_nerrs = 0;
    kmath_errflag = 0;
    kmath_char = YYEMPTY;
    kmath_state = 0;

    if (kmath_ss == NULL && kmath_growstack()) goto kmath_overflow;
    kmath_ssp = kmath_ss;
    kmath_vsp = kmath_vs;
    kmath_state = 0;
    *kmath_ssp = 0;

kmath_loop:
    if ((kmath_n = kmath_defred[kmath_state]) != 0) goto kmath_reduce;
    if (kmath_char < 0)
    {
        if ((kmath_char = kmathlex()) < 0) kmath_char = 0;
#if YYDEBUG
        if (kmath_debug)
        {
            kmath_s = 0;
            if (kmath_char <= YYMAXTOKEN) kmath_s = kmath_name[kmath_char];
            if (!kmath_s) kmath_s = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, kmath_state, kmath_char, kmath_s);
        }
#endif
    }
    if ((kmath_n = kmath_sindex[kmath_state]) && (kmath_n += kmath_char) >= 0 &&
            kmath_n <= YYTABLESIZE && kmath_check[kmath_n] == kmath_char)
    {
#if YYDEBUG
        if (kmath_debug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, kmath_state, kmath_table[kmath_n]);
#endif
        if (kmath_ssp >= kmath_sslim && kmath_growstack())
        {
            goto kmath_overflow;
        }
        kmath_state = kmath_table[kmath_n];
        *++kmath_ssp = kmath_table[kmath_n];
        *++kmath_vsp = kmathlval;
        kmath_char = YYEMPTY;
        if (kmath_errflag > 0)  --kmath_errflag;
        goto kmath_loop;
    }
    if ((kmath_n = kmath_rindex[kmath_state]) && (kmath_n += kmath_char) >= 0 &&
            kmath_n <= YYTABLESIZE && kmath_check[kmath_n] == kmath_char)
    {
        kmath_n = kmath_table[kmath_n];
        goto kmath_reduce;
    }
    if (kmath_errflag) goto kmath_inrecovery;

    kmatherror("syntax error");

    goto kmath_errlab;

kmath_errlab:
    ++kmath_nerrs;

kmath_inrecovery:
    if (kmath_errflag < 3)
    {
        kmath_errflag = 3;
        for (;;)
        {
            if ((kmath_n = kmath_sindex[*kmath_ssp]) && (kmath_n += YYERRCODE) >= 0 &&
                    kmath_n <= YYTABLESIZE && kmath_check[kmath_n] == YYERRCODE)
            {
#if YYDEBUG
                if (kmath_debug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *kmath_ssp, kmath_table[kmath_n]);
#endif
                if (kmath_ssp >= kmath_sslim && kmath_growstack())
                {
                    goto kmath_overflow;
                }
                kmath_state = kmath_table[kmath_n];
                *++kmath_ssp = kmath_table[kmath_n];
                *++kmath_vsp = kmathlval;
                goto kmath_loop;
            }
            else
            {
#if YYDEBUG
                if (kmath_debug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *kmath_ssp);
#endif
                if (kmath_ssp <= kmath_ss) goto kmath_abort;
                --kmath_ssp;
                --kmath_vsp;
            }
        }
    }
    else
    {
        if (kmath_char == 0) goto kmath_abort;
#if YYDEBUG
        if (kmath_debug)
        {
            kmath_s = 0;
            if (kmath_char <= YYMAXTOKEN) kmath_s = kmath_name[kmath_char];
            if (!kmath_s) kmath_s = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, kmath_state, kmath_char, kmath_s);
        }
#endif
        kmath_char = YYEMPTY;
        goto kmath_loop;
    }

kmath_reduce:
#if YYDEBUG
    if (kmath_debug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, kmath_state, kmath_n, kmath_rule[kmath_n]);
#endif
    kmath_m = kmath_len[kmath_n];
    if (kmath_m)
        kmath_val = kmath_vsp[1-kmath_m];
    else
        memset(&kmath_val, 0, sizeof kmath_val);
    switch (kmath_n)
    {
case 1:
#line 207 "../source/kmath.yacc"
	{
          iresult = kmath_vsp[0].dval;         /* 0 or 1 */
        }
break;
case 2:
#line 212 "../source/kmath.yacc"
	{
          iresult = (kmath_vsp[0].dval != 0);
        }
break;
case 3:
#line 217 "../source/kmath.yacc"
	{
          dresult = kmath_vsp[0].dval;
        }
break;
case 4:
#line 222 "../source/kmath.yacc"
	{
          int maxint = 0x7FFFFFFF;
          if( kmath_vsp[0].dval <= maxint && kmath_vsp[0].dval >= -maxint && (int)kmath_vsp[0].dval == kmath_vsp[0].dval ) {
            result = malloc( 16 );
            sprintf( result, "%d", (int)kmath_vsp[0].dval );
          }
          else {
            result = strdup( strfromd( kmath_vsp[0].dval, 0 ) );
          }
          if( rsigma && strpbrk( result, ".eE" ) == NULL ) {
            /* append a dot to make an integer look like a float */
            result = mstrcat( result, "." );
          }
        }
break;
case 5:
#line 238 "../source/kmath.yacc"
	{
          result = kmath_vsp[0].sval;
        }
break;
case 6:
#line 244 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[0].dval;       }
break;
case 7:
#line 245 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[-1].dval;       }
break;
case 8:
#line 246 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[0].dval;      }
break;
case 9:
#line 247 "../source/kmath.yacc"
	{ kmath_val.dval = -kmath_vsp[0].dval;      }
break;
case 10:
#line 248 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[-2].dval + kmath_vsp[0].dval;  }
break;
case 11:
#line 249 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[-2].dval - kmath_vsp[0].dval;  }
break;
case 12:
#line 250 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[-2].dval * kmath_vsp[0].dval;  }
break;
case 13:
#line 251 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[-1].dval < 0 ? -kmath_vsp[-1].dval : kmath_vsp[-1].dval; }
break;
case 14:
#line 252 "../source/kmath.yacc"
	{ kmath_val.dval = (int)kmath_vsp[-1].dval;  }
break;
case 15:
#line 253 "../source/kmath.yacc"
	{ kmath_val.dval = IQUEST((int)kmath_vsp[-1].dval); }
break;
case 16:
#line 256 "../source/kmath.yacc"
	{
          if( kmath_vsp[0].dval != 0 )
            kmath_val.dval = kmath_vsp[-2].dval / kmath_vsp[0].dval;
          else
            YYABORT;
        }
break;
case 17:
#line 263 "../source/kmath.yacc"
	{
          if( kmath_vsp[-1].dval != 0 )
/*            $$ = $2 / $4 - (int)($2 / $4); */
	    kmath_val.dval = (int) kmath_vsp[-3].dval % (int) kmath_vsp[-1].dval;
          else
            YYABORT;
        }
break;
case 18:
#line 272 "../source/kmath.yacc"
	{nonum=1;}
break;
case 19:
#line 273 "../source/kmath.yacc"
	{
          kmath_val.dval = strlen( kmath_vsp[-1].sval );
          free( kmath_vsp[-1].sval );
          nonum = 0;
        }
break;
case 20:
#line 279 "../source/kmath.yacc"
	{nonum=1;}
break;
case 21:
#line 280 "../source/kmath.yacc"
	{
          char *p = strstr( kmath_vsp[-3].sval, kmath_vsp[-1].sval );
          if( p == NULL )
            kmath_val.dval = 0;
          else
            kmath_val.dval = p - kmath_vsp[-3].sval + 1;
          free( kmath_vsp[-3].sval );
          free( kmath_vsp[-1].sval );
          nonum = 0;
        }
break;
case 22:
#line 292 "../source/kmath.yacc"
	{
          kmath_val.dval = count_words( kmath_vsp[-1].sval, " " );
          nonum = 0;
        }
break;
case 23:
#line 298 "../source/kmath.yacc"
	{
          kmath_val.dval = count_words( kmath_vsp[-3].sval, kmath_vsp[-1].sval );
          free( kmath_vsp[-1].sval );
          nonum = 0;
        }
break;
case 24:
#line 304 "../source/kmath.yacc"
	{nonum=1;}
break;
case 25:
#line 305 "../source/kmath.yacc"
	{
          FILE *stream = fopen( kmath_vsp[-1].sval, "r" );
          if( stream == NULL ) {
            kmath_val.dval = 0;
          }
          else {
            kmath_val.dval = 1;
            fclose( stream );
          }
          free( kmath_vsp[-1].sval );
          nonum = 0;
        }
break;
case 26:
#line 318 "../source/kmath.yacc"
	{nonum=1;}
break;
case 27:
#line 319 "../source/kmath.yacc"
	{
          kmath_val.dval = ku_vtype( kmath_vsp[-1].sval );
          free( kmath_vsp[-1].sval );
          nonum = 0;
        }
break;
case 28:
#line 326 "../source/kmath.yacc"
	{
          if( ku_vtype( kmath_vsp[-1].sval ) != 0 )
            kmath_val.dval = IQUEST( 31 );
          else
            kmath_val.dval = 0;
          free( kmath_vsp[-1].sval );
        }
break;
case 29:
#line 334 "../source/kmath.yacc"
	{
          int n = kmath_vsp[-1].dval;
          if( n >= 1 && n <= 3 && ku_vtype( kmath_vsp[-3].sval ) != 0 )
            kmath_val.dval = IQUEST(30+n);
          else
            kmath_val.dval = 0;
          free( kmath_vsp[-3].sval );
        }
break;
case 30:
#line 344 "../source/kmath.yacc"
	{
          kprintf("KUIP : kmath.yacc : Vlen RPAR : dummy\n");
          /*GB
          if( ku_vtype( $1 ) != 0 ) {
            int k = IQUEST(31);
            if( IQUEST(32) == 1 && IQUEST(33) == 1 ) {
              int low = ku_vqaddr( $1 );
              while( k > 0 && IQ(low+k-1) == 0 )
                k--;
            }
            $$ = k;
          }
          else */
            kmath_val.dval = 0;
          free( kmath_vsp[-1].sval );
        }
break;
case 31:
#line 361 "../source/kmath.yacc"
	{
          kprintf("KUIP : kmath.yacc : Vlen COMMA Expr RPAR : dummy\n");
          /*GB
          int n = $3;
          if( n >= 1 && n <= 3 && ku_vtype( $1 ) != 0 ) {
            int k = IQUEST(30+n);
            if( n == 1 && IQUEST(32) == 1 && IQUEST(33) == 1 ) {
              int low = ku_vqaddr( $1 );
              while( k > 0 && IQ(low+k-1) == 0 )
                k--;
            }
            $$ = k;
          }
          else GB*/
            kmath_val.dval = 0;
          free( kmath_vsp[-3].sval );
        }
break;
case 32:
#line 380 "../source/kmath.yacc"
	{
          int n = strlen( kmath_vsp[-2].sval );
          kmath_vsp[-2].sval = realloc( kmath_vsp[-2].sval, n + 16 );
          sprintf( &kmath_vsp[-2].sval[n], "(%d)", (int)kmath_vsp[-1].dval );
          n = ku_vvalue( kmath_vsp[-2].sval, &kmath_val.dval );
          free( kmath_vsp[-2].sval );
          if( n != 0 )
            YYABORT;
        }
break;
case 33:
#line 390 "../source/kmath.yacc"
	{
          int n = strlen( kmath_vsp[-4].sval );
          kmath_vsp[-4].sval = realloc( kmath_vsp[-4].sval, n + 32 );
          sprintf( &kmath_vsp[-4].sval[n], "(%d,%d)", (int)kmath_vsp[-3].dval, (int)kmath_vsp[-1].dval );
          n = ku_vvalue( kmath_vsp[-4].sval, &kmath_val.dval );
          free( kmath_vsp[-4].sval );
          if( n != 0 )
            YYABORT;
        }
break;
case 34:
#line 400 "../source/kmath.yacc"
	{
          int n = strlen( kmath_vsp[-6].sval );
          kmath_vsp[-6].sval = realloc( kmath_vsp[-6].sval, n + 48 );
          sprintf( &kmath_vsp[-6].sval[n], "(%d,%d,%d)", (int)kmath_vsp[-5].dval, (int)kmath_vsp[-3].dval, (int)kmath_vsp[-1].dval );
          n = ku_vvalue( kmath_vsp[-6].sval, &kmath_val.dval );
          free( kmath_vsp[-6].sval );
          if( n != 0 )
            YYABORT;
        }
break;
case 35:
#line 410 "../source/kmath.yacc"
	{nonum=1;}
break;
case 36:
#line 411 "../source/kmath.yacc"
	{
          char  *slist[2];
          double dlist[2];
          int    which[2];
          which[0] = 2;
          slist[1] = kmath_vsp[-1].sval;
          which[1] = 1;
          call_user_function( kmath_vsp[-3].sval, slist, dlist, which, 1 );
          kmath_val.dval = dlist[0];
          nonum = 0;
        }
break;
case 37:
#line 424 "../source/kmath.yacc"
	{
          char  *slist[2];
          double dlist[2];
          int    which[2];
          which[0] = 2;
          dlist[1] = kmath_vsp[-1].dval;
          which[1] = 2;
          call_user_function( kmath_vsp[-2].sval, slist, dlist, which, 1 );
          kmath_val.dval = dlist[0];
        }
break;
case 38:
#line 435 "../source/kmath.yacc"
	{nonum=1;}
break;
case 39:
#line 436 "../source/kmath.yacc"
	{
          char  *slist[3];
          double dlist[3];
          int    which[3];
          which[0] = 2;
          dlist[1] = kmath_vsp[-4].dval;
          which[1] = 2;
          slist[2] = kmath_vsp[-1].sval;
          which[2] = 1;
          call_user_function( kmath_vsp[-5].sval, slist, dlist, which, 2 );
          kmath_val.dval = dlist[0];
          nonum = 0;
        }
break;
case 40:
#line 453 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[-1].dval;       }
break;
case 41:
#line 454 "../source/kmath.yacc"
	{ kmath_val.dval = ! kmath_vsp[0].dval; }
break;
case 42:
#line 455 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[-2].dval && kmath_vsp[0].dval; }
break;
case 43:
#line 456 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[-2].dval || kmath_vsp[0].dval; }
break;
case 44:
#line 457 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[-2].dval <  kmath_vsp[0].dval; }
break;
case 45:
#line 458 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[-2].dval >  kmath_vsp[0].dval; }
break;
case 46:
#line 459 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[-2].dval <= kmath_vsp[0].dval; }
break;
case 47:
#line 460 "../source/kmath.yacc"
	{ kmath_val.dval = kmath_vsp[-2].dval >= kmath_vsp[0].dval; }
break;
case 48:
#line 466 "../source/kmath.yacc"
	{
          char *s1 = strdup( strfromd( kmath_vsp[-2].dval, 0 ) );
          kmath_val.dval = (strcmp( s1, strfromd( kmath_vsp[0].dval, 0 ) ) == 0);
          free( s1 );
        }
break;
case 49:
#line 472 "../source/kmath.yacc"
	{
          char *s1 = strdup( strfromd( kmath_vsp[-2].dval, 0 ) );
          kmath_val.dval = (strcmp( s1, strfromd( kmath_vsp[0].dval, 0 ) ) != 0);
          free( s1 );
        }
break;
case 50:
#line 482 "../source/kmath.yacc"
	{nonum=1;}
break;
case 51:
#line 483 "../source/kmath.yacc"
	{
          kmath_val.dval = (strcmp( kmath_vsp[-3].sval, kmath_vsp[0].sval ) == 0);
          free( kmath_vsp[-3].sval );
          free( kmath_vsp[0].sval );
          nonum = 0;
        }
break;
case 52:
#line 490 "../source/kmath.yacc"
	{
          kmath_val.dval = (strcmp( strfromd( kmath_vsp[-2].dval,0 ), kmath_vsp[0].sval ) == 0);
          free( kmath_vsp[0].sval );
        }
break;
case 53:
#line 495 "../source/kmath.yacc"
	{nonum=1;}
break;
case 54:
#line 496 "../source/kmath.yacc"
	{
          kmath_val.dval = (strcmp( kmath_vsp[-3].sval, kmath_vsp[0].sval ) != 0);
          free( kmath_vsp[-3].sval );
          free( kmath_vsp[0].sval );
          nonum = 0;
        }
break;
case 55:
#line 503 "../source/kmath.yacc"
	{
          kmath_val.dval = (strcmp( strfromd( kmath_vsp[-2].dval,0 ), kmath_vsp[0].sval ) != 0);
          free( kmath_vsp[0].sval );
        }
break;
case 56:
#line 509 "../source/kmath.yacc"
	{ kmath_val.sval = kmath_vsp[0].sval; }
break;
case 57:
#line 511 "../source/kmath.yacc"
	{nonum=1;}
break;
case 58:
#line 512 "../source/kmath.yacc"
	{
          kmath_val.sval = mstrcat( kmath_vsp[-3].sval, kmath_vsp[0].sval );
          free( kmath_vsp[0].sval );
          nonum = 0;
        }
break;
case 59:
#line 517 "../source/kmath.yacc"
	{nonum=1;}
break;
case 60:
#line 518 "../source/kmath.yacc"
	{
          kmath_val.sval = strdup( strfromd( kmath_vsp[-3].dval, 0 ) );
          kmath_val.sval = mstrcat( kmath_val.sval, kmath_vsp[0].sval);
          free( kmath_vsp[0].sval );
          nonum = 0;
        }
break;
case 61:
#line 526 "../source/kmath.yacc"
	{
          char  *slist[1];
          double dlist[1];
          int    which[1];
          which[0] = 1;
          call_user_function( kmath_vsp[-1].sval, slist, dlist, which, 0 );
          kmath_val.sval = slist[0];
        }
break;
case 62:
#line 535 "../source/kmath.yacc"
	{nonum=1;}
break;
case 63:
#line 536 "../source/kmath.yacc"
	{
          char  *slist[2];
          double dlist[2];
          int    which[2];
          which[0] = 1;
          slist[1] = kmath_vsp[-1].sval;
          which[1] = 1;
          call_user_function( kmath_vsp[-3].sval, slist, dlist, which, 1 );
          kmath_val.sval = slist[0];
          nonum = 0;
        }
break;
case 64:
#line 549 "../source/kmath.yacc"
	{
          char  *slist[2];
          double dlist[2];
          int    which[2];
          which[0] = 1;
          dlist[1] = kmath_vsp[-1].dval;
          which[1] = 2;
          call_user_function( kmath_vsp[-2].sval, slist, dlist, which, 1 );
          kmath_val.sval = slist[0];
        }
break;
case 65:
#line 560 "../source/kmath.yacc"
	{nonum=1;}
break;
case 66:
#line 561 "../source/kmath.yacc"
	{
          char  *slist[3];
          double dlist[3];
          int    which[3];
          which[0] = 1;
          dlist[1] = kmath_vsp[-4].dval;
          which[1] = 2;
          slist[2] = kmath_vsp[-1].sval;
          which[2] = 1;
          call_user_function( kmath_vsp[-5].sval, slist, dlist, which, 2 );
          kmath_val.sval = slist[0];
          nonum = 0;
        }
break;
case 67:
#line 576 "../source/kmath.yacc"
	{
          int n = kmath_vsp[-1].dval;
          if( n > 0 && n <= khash_entries( kc_alias.arg_table ) ) {
            HashArray *alias = khash_array( kc_alias.arg_table );
            kmath_val.sval = strdup( alias[n-1].name );
            free( (char*)alias );
          }
          else
            YYABORT;
        }
break;
case 68:
#line 588 "../source/kmath.yacc"
	{
          int n = kmath_vsp[-1].dval;
          if( n > 0 && n <= khash_entries( kc_alias.arg_table ) ) {
            HashArray *alias = khash_array( kc_alias.arg_table );
            kmath_val.sval = strdup( (char*)alias[n-1].value );
            free( (char*)alias );
          }
          else
            YYABORT;
        }
break;
case 69:
#line 599 "../source/kmath.yacc"
	{nonum=1;}
break;
case 70:
#line 600 "../source/kmath.yacc"
	{
          if( (kmath_val.sval = getenv( kmath_vsp[-1].sval )) == NULL )
            kmath_val.sval = "";
          free( kmath_vsp[-1].sval );
          kmath_val.sval = strdup( kmath_val.sval );
          nonum = 0;
        }
break;
case 71:
#line 608 "../source/kmath.yacc"
	{nonum=1;}
break;
case 72:
#line 609 "../source/kmath.yacc"
	{
          int n = khash_entries( kc_alias.var_table );
          kmath_val.sval = strdup( "" );
          if( n > 0 ) {
            HashArray *values = khash_array( kc_alias.var_table );
            int i;
            for( i = 0; i < n; i++ ) {
              char *name = values[i].name;

              if( ku_match( name, kmath_vsp[-1].sval, 1 ) ) {
                if( kmath_val.sval[0] != '\0' )
                  kmath_val.sval = mstrcat( kmath_val.sval, " " );
                kmath_val.sval = mstrcat( kmath_val.sval, name );
              }
            }
            free( (char*)values );
          }
          free( kmath_vsp[-1].sval );
          nonum = 0;
        }
break;
case 73:
#line 632 "../source/kmath.yacc"
	{
          char format[16];
          char *dot;
          int n = fstrtoi( kmath_vsp[-1].sval + 1, &dot );
          int m = 0;

          if( *dot == '.' ) {
            char *tail;
            m = fstrtoi( dot + 1, &tail );
          }

          kmath_val.sval = realloc( kmath_vsp[-1].sval, 256 ); /* some safety for too small n */

          switch( kmath_val.sval[0] ) {

          case 'I': case 'i':
            sprintf( format, "%%%dd", n );
            sprintf( kmath_val.sval, format, (int)kmath_vsp[-3].dval );
            if( m > 0 )
              kmath_val.sval = zero_pad( kmath_val.sval, m );
            break;

          case 'Z': case 'z':     /* hexadecimal */
            sprintf( format, "%%%dX", n );
            sprintf( kmath_val.sval, format, (int)kmath_vsp[-3].dval );
            if( m > 0 )
              kmath_val.sval = zero_pad( kmath_val.sval, m );
            break;

          case 'F': case 'f':
            sprintf( format, "%%%d.%df", n, m );
            sprintf( kmath_val.sval, format, kmath_vsp[-3].dval );
            /* force a decimal point */
            if( strchr( kmath_val.sval, '.' ) == NULL )
              kmath_val.sval = mstrcat( kmath_val.sval, "." );
            break;

          case 'E': case 'e':
            sprintf( format, "%%%d.%de", n, m );
            sprintf( kmath_val.sval, format, kmath_vsp[-3].dval );
            strupper( kmath_val.sval );
            break;

          case 'G': case 'g':
            sprintf( format, "%%%d.%dg", n, m );
            sprintf( kmath_val.sval, format, kmath_vsp[-3].dval );
            strupper( kmath_val.sval );
            break;

          default:                /* error */
            free( kmath_val.sval );
            YYABORT;
            break;
          }
        }
break;
case 74:
#line 689 "../source/kmath.yacc"
	{nonum=1;}
break;
case 75:
#line 690 "../source/kmath.yacc"
	{
          kmath_val.sval = quote_string( kmath_vsp[-1].sval, 1 );
          nonum = 0;
        }
break;
case 76:
#line 695 "../source/kmath.yacc"
	{nonum=1;}
break;
case 77:
#line 696 "../source/kmath.yacc"
	{
          /* int n = strlen( $$ ); - NC - */
          int n = strlen( kmath_vsp[-1].sval );
          kmath_val.sval = kmath_vsp[-1].sval;
          if( n >= 2 && kmath_val.sval[0] == '\'' && kmath_val.sval[n-1] == '\'' ) {
            char *p = kmath_val.sval;
            kmath_val.sval[n-1] = '\0';
            kuip_strcpy( kmath_val.sval, kmath_val.sval + 1 ); /*GB*/
            while( (p = strstr( p, "''" )) != NULL ) {
              kuip_strcpy( p, p + 1 ); /*GB*/
              p++;
            }
          }
          nonum = 0;
        }
break;
case 78:
#line 713 "../source/kmath.yacc"
	{
          kmath_val.sval = extract_word( kmath_vsp[-1].sval, " ", 1 );
          free( kmath_vsp[-1].sval );
          nonum = 0;
        }
break;
case 79:
#line 719 "../source/kmath.yacc"
	{
          kmath_val.sval = extract_word( kmath_vsp[-3].sval, " ", (int)kmath_vsp[-1].dval );
          free( kmath_vsp[-3].sval );
        }
break;
case 80:
#line 724 "../source/kmath.yacc"
	{
          char *sep = " ";
          kmath_val.sval = join_words( kmath_vsp[-5].sval, sep, (int)kmath_vsp[-3].dval, (int)kmath_vsp[-1].dval );
          free( kmath_vsp[-5].sval );
        }
break;
case 81:
#line 729 "../source/kmath.yacc"
	{nonum=1;}
break;
case 82:
#line 730 "../source/kmath.yacc"
	{
          kmath_val.sval = join_words( kmath_vsp[-8].sval, kmath_vsp[-1].sval, (int)kmath_vsp[-6].dval, (int)kmath_vsp[-4].dval );
          free( kmath_vsp[-8].sval );
          free( kmath_vsp[-1].sval );
          nonum = 0;
        }
break;
case 83:
#line 737 "../source/kmath.yacc"
	{nonum=1;}
break;
case 84:
#line 738 "../source/kmath.yacc"
	{
          kmath_val.sval = strlower( kmath_vsp[-1].sval );
          nonum = 0;
        }
break;
case 85:
#line 743 "../source/kmath.yacc"
	{nonum=1;}
break;
case 86:
#line 744 "../source/kmath.yacc"
	{
          kmath_val.sval = strupper( kmath_vsp[-1].sval );
          nonum = 0;
        }
break;
case 87:
#line 751 "../source/kmath.yacc"
	{
          int len = strlen( kmath_vsp[-2].sval );
          int k = kmath_vsp[-1].dval;
          kmath_val.sval = kmath_vsp[-2].sval;
          if( k <= 0 )
            k += len + 1;
          if( k > len )
            kmath_val.sval[0] = '\0';
          else if( k < 1 )
            YYABORT;
          else
            kuip_strcpy( kmath_val.sval, kmath_val.sval + k - 1 ); /*GB*/
        }
break;
case 88:
#line 767 "../source/kmath.yacc"
	{
          int len = strlen( kmath_vsp[-4].sval );
          int k = kmath_vsp[-3].dval;
          int n = kmath_vsp[-1].dval;
          kmath_val.sval = kmath_vsp[-4].sval;
          if( k <= 0 )
            k += len + 1;
          if( k > len )
            kmath_val.sval[0] = '\0';
          else if( k < 1 )
            YYABORT;
          else {
            kuip_strcpy( kmath_val.sval, kmath_val.sval + k - 1 ); /*GB*/
            if( k + n <= len )
              kmath_val.sval[n] = '\0';
          }
        }
break;
case 89:
#line 787 "../source/kmath.yacc"
	{
          kmath_val.sval = shell_line( kmath_vsp[-1].sval, 0, " " );
          free( kmath_vsp[-1].sval );
        }
break;
case 90:
#line 794 "../source/kmath.yacc"
	{
          kmath_val.sval = shell_line( kmath_vsp[-3].sval, 0, kmath_vsp[-1].sval );
          free( kmath_vsp[-3].sval );
          free( kmath_vsp[-1].sval );
        }
break;
case 91:
#line 802 "../source/kmath.yacc"
	{
          int n = kmath_vsp[-1].dval;
          kmath_val.sval = shell_line( kmath_vsp[-3].sval, n, NULL );
          free( kmath_vsp[-3].sval );
        }
break;
case 92:
#line 815 "../source/kmath.yacc"
	{nonum=1;}
break;
case 93:
#line 816 "../source/kmath.yacc"
	{
          kmath_val.sval = kmath_vsp[-1].sval;
          nonum = 0;
        }
break;
case 94:
#line 821 "../source/kmath.yacc"
	{nonum=1;}
break;
case 95:
#line 822 "../source/kmath.yacc"
	{
          kmath_val.sval = kmath_vsp[0].sval;
        }
break;
case 96:
#line 826 "../source/kmath.yacc"
	{nonum=1;}
break;
case 97:
#line 827 "../source/kmath.yacc"
	{
          kmath_val.sval = kmath_vsp[0].sval;
          nonum = 0;
        }
break;
case 98:
#line 832 "../source/kmath.yacc"
	{nonum=1;}
break;
case 99:
#line 833 "../source/kmath.yacc"
	{
          kmath_val.sval = kmath_vsp[0].sval;
          nonum = 0;
        }
break;
case 100:
#line 843 "../source/kmath.yacc"
	{nonum=1;}
break;
case 101:
#line 844 "../source/kmath.yacc"
	{
          kmath_val.sval = kmath_vsp[0].sval;
          nonum = 0;
        }
break;
case 102:
#line 849 "../source/kmath.yacc"
	{nonum=1;}
break;
case 103:
#line 850 "../source/kmath.yacc"
	{
          kmath_val.sval = kmath_vsp[0].sval;
          nonum = 0;
        }
break;
#line 2689 "y.tab.c"
    }
    kmath_ssp -= kmath_m;
    kmath_state = *kmath_ssp;
    kmath_vsp -= kmath_m;
    kmath_m = kmath_lhs[kmath_n];
    if (kmath_state == 0 && kmath_m == 0)
    {
#if YYDEBUG
        if (kmath_debug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        kmath_state = YYFINAL;
        *++kmath_ssp = YYFINAL;
        *++kmath_vsp = kmath_val;
        if (kmath_char < 0)
        {
            if ((kmath_char = kmathlex()) < 0) kmath_char = 0;
#if YYDEBUG
            if (kmath_debug)
            {
                kmath_s = 0;
                if (kmath_char <= YYMAXTOKEN) kmath_s = kmath_name[kmath_char];
                if (!kmath_s) kmath_s = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, kmath_char, kmath_s);
            }
#endif
        }
        if (kmath_char == 0) goto kmath_accept;
        goto kmath_loop;
    }
    if ((kmath_n = kmath_gindex[kmath_m]) && (kmath_n += kmath_state) >= 0 &&
            kmath_n <= YYTABLESIZE && kmath_check[kmath_n] == kmath_state)
        kmath_state = kmath_table[kmath_n];
    else
        kmath_state = kmath_dgoto[kmath_m];
#if YYDEBUG
    if (kmath_debug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *kmath_ssp, kmath_state);
#endif
    if (kmath_ssp >= kmath_sslim && kmath_growstack())
    {
        goto kmath_overflow;
    }
    *++kmath_ssp = (short) kmath_state;
    *++kmath_vsp = kmath_val;
    goto kmath_loop;

kmath_overflow:
    kmatherror("yacc stack overflow");

kmath_abort:
    return (1);

kmath_accept:
    return (0);
}
