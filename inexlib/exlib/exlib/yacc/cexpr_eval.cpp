#ifndef lint
static const char cexpr_eval_sccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20110908

#define YYEMPTY        (-1)
#define cexpr_eval_clearin      (cexpr_eval_char = YYEMPTY)
#define cexpr_eval_errok        (cexpr_eval_errflag = 0)
#define YYRECOVERING() (cexpr_eval_errflag != 0)

#define YYPREFIX "cexpr_eval_"

#define YYPURE 0

#line 2 "../exlib/yacc/cexpr_eval.y"

#include "cexpr_eval.h"

#include "cexpr_lexer.h"

#include <inlib/vmanip>

extern int cexpr_lexer_lex(exlib::yacc::cexpr_lexer&);
static void cexpr_eval_error(exlib::yacc::cexpr_eval&,const char*); 

#define NotFound (-1)

/*#define DEBUG*/

#line 18 "../exlib/yacc/cexpr_eval.y"
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union {
  unsigned int m_index;
  unsigned int m_integer;
  double m_number;
  inlib::valop* m_expression;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 48 "y.tab.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() cexpr_eval_parse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() cexpr_eval_parse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() cexpr_eval_parse(exlib::yacc::cexpr_eval& aParser)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() cexpr_lexer_lex(void *YYLEX_PARAM)
# define YYLEX cexpr_lexer_lex(YYLEX_PARAM)
#else
# define YYLEX_DECL() cexpr_lexer_lex(void)
# define YYLEX cexpr_lexer_lex(aParser.lexer())
#endif

/* Parameters sent to cexpr_eval_error. */
#define YYERROR_DECL() cexpr_eval_error(const char *s)
#define YYERROR_CALL(msg) cexpr_eval_error(msg)

extern int YYPARSE_DECL();

#define STRG 257
#define T_NAME 258
#define NUMBER_INTEGER 259
#define NUMBER_REAL 260
#define T_PI 261
#define T_TRUE 262
#define T_FALSE 263
#define T_AND 264
#define T_OR 265
#define UMINUS 266
#define YYERRCODE 256
static const short cexpr_eval_lhs[] = {                           -1,
    0,    0,    0,    1,    1,    1,    1,    1,    1,    1,
    2,    2,    2,    2,    2,    2,    3,    3,    3,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    3,    3,    3,    3,
};
static const short cexpr_eval_len[] = {                            2,
    1,    1,    1,    2,    4,    3,    4,    3,    3,    1,
    3,    4,    3,    4,    4,    4,    3,    3,    3,    3,
    2,    3,    4,    6,    8,   10,   12,   14,    1,    1,
    1,    1,    1,    1,    1,
};
static const short cexpr_eval_defred[] = {                         0,
    1,   35,    0,   32,   33,   29,   30,   31,    0,    0,
    0,    0,    0,   10,    0,    0,    0,   21,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    9,   22,    0,    0,
    0,    0,    0,    0,   20,   19,    0,    0,    0,    0,
    0,    0,   23,    0,    0,    0,    0,    0,    0,    0,
    0,   24,    0,    0,   25,    0,    0,   26,    0,    0,
   27,    0,    0,   28,
};
static const short cexpr_eval_dgoto[] = {                         12,
   13,   14,   20,
};
static const short cexpr_eval_sindex[] = {                       -30,
    0,    0,  -34,    0,    0,    0,    0,    0,   -7,  -22,
  -22,    0,  -16,    0,  107,   -7,   -7,    0,  -16,  107,
  -21,   81,  -22,  -22,  -36, -117,   -7,   -7,   -7,   -7,
  -48,  -40,  -45,   37,   74,   92,    0,    0,  -16,  -16,
  -22,  -22,  -38,  -38,    0,    0,   -7,   -7,  163,   -7,
   -7,  163,    0,   -7,  -16,  -16,  163,  163,  163,  163,
  114,    0,   -7,  129,    0,   -7,  140,    0,   -7,  150,
    0,   -7,  157,    0,
};
static const short cexpr_eval_rindex[] = {                         0,
    0,    0,    1,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   19,    0,   24,    0,    0,    0,    8,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   26,   28,
    0,    0,   14,   27,    0,    0,    0,    0,   12,    0,
    0,   40,    0,    0,   32,   79,   54,   56,   64,   66,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,
};
static const short cexpr_eval_gindex[] = {                         0,
   89,    0,  306,
};
#define YYTABLESIZE 378
static const short cexpr_eval_table[] = {                         17,
   34,   41,   10,   30,    9,   16,   42,    4,   29,   11,
   10,   11,   47,   18,    9,   50,   25,   11,    2,   37,
   48,   25,    9,    3,    0,    6,   17,    8,    0,    0,
    0,    5,   17,   34,    0,    0,    0,    9,   34,   13,
    0,   34,   34,   34,   34,   34,   18,   34,    4,   11,
    0,   18,   11,   16,   18,   12,   18,   18,   18,   17,
   34,   34,   34,   15,   17,   14,    6,   17,    8,   17,
   17,   17,    5,   18,   18,   18,   17,   13,    7,    0,
   13,    9,    0,    0,    0,    0,   17,   17,   17,    0,
    0,   16,    0,   12,   16,    0,   12,   51,   19,   21,
    0,   15,   26,   14,   15,    0,   14,   26,    0,    0,
    0,   39,   40,   31,   53,   30,   28,   54,   27,    7,
   29,   38,   30,   28,   34,   27,    0,   29,    0,   55,
   56,    0,   38,   30,   28,   11,   27,   18,   29,   31,
   34,   33,   32,    0,    0,    0,    0,    0,   30,   28,
   17,   27,    0,   29,   62,   30,   28,   63,   27,    0,
   29,    0,    0,   13,    0,    0,   34,   33,   32,   65,
   30,   28,   66,   27,    0,   29,    0,   16,    0,   12,
   68,   30,   28,   69,   27,    0,   29,   15,    0,   14,
   71,   30,   28,   72,   27,    0,   29,   74,   30,   28,
    0,   27,    0,   29,   30,   28,    0,   27,    0,   29,
    0,    0,    0,    0,    0,    0,    2,    3,    4,    5,
    6,    7,    8,    0,    0,    1,    2,    3,    4,    5,
    6,    7,    8,    0,    2,    3,    4,    5,    6,    7,
    8,    0,   23,   24,    0,    0,    0,   23,   24,    2,
    3,    4,    5,    6,    7,    8,    0,    0,    0,    0,
    0,    0,    0,    0,   34,   34,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   11,   11,   18,   18,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   17,   17,    0,    2,    3,    4,    5,    6,    7,    8,
    0,    0,    0,   13,   13,   15,    0,    0,    0,    0,
    0,    0,    0,    0,   18,    0,   22,   16,   16,   12,
   12,   35,   36,    0,    0,    0,    0,   15,   15,   14,
   14,    0,   43,   44,   45,   46,    0,   49,    0,   52,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   57,   58,    0,   59,   60,    0,    0,   61,
    0,    0,    0,    0,    0,    0,    0,    0,   64,    0,
    0,   67,    0,    0,   70,    0,    0,   73,
};
static const short cexpr_eval_check[] = {                         40,
    0,   38,   33,   42,   45,   40,  124,    0,   47,   40,
   33,    0,   61,    0,   45,   61,   38,   40,    0,   41,
   61,   38,   45,    0,   -1,    0,    0,    0,   -1,   -1,
   -1,    0,   40,   33,   -1,   -1,   -1,   45,   38,    0,
   -1,   41,   42,   43,   44,   45,   33,   47,   41,   38,
   -1,   38,   41,    0,   41,    0,   43,   44,   45,   33,
   60,   61,   62,    0,   38,    0,   41,   41,   41,   43,
   44,   45,   41,   60,   61,   62,   40,   38,    0,   -1,
   41,   45,   -1,   -1,   -1,   -1,   60,   61,   62,   -1,
   -1,   38,   -1,   38,   41,   -1,   41,   61,   10,   11,
   -1,   38,  124,   38,   41,   -1,   41,  124,   -1,   -1,
   -1,   23,   24,   33,   41,   42,   43,   44,   45,   41,
   47,   41,   42,   43,  124,   45,   -1,   47,   -1,   41,
   42,   -1,   41,   42,   43,  124,   45,  124,   47,   33,
   60,   61,   62,   -1,   -1,   -1,   -1,   -1,   42,   43,
  124,   45,   -1,   47,   41,   42,   43,   44,   45,   -1,
   47,   -1,   -1,  124,   -1,   -1,   60,   61,   62,   41,
   42,   43,   44,   45,   -1,   47,   -1,  124,   -1,  124,
   41,   42,   43,   44,   45,   -1,   47,  124,   -1,  124,
   41,   42,   43,   44,   45,   -1,   47,   41,   42,   43,
   -1,   45,   -1,   47,   42,   43,   -1,   45,   -1,   47,
   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,  259,  260,
  261,  262,  263,   -1,   -1,  256,  257,  258,  259,  260,
  261,  262,  263,   -1,  257,  258,  259,  260,  261,  262,
  263,   -1,  264,  265,   -1,   -1,   -1,  264,  265,  257,
  258,  259,  260,  261,  262,  263,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  264,  265,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  264,  265,  264,  265,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  264,  265,   -1,  257,  258,  259,  260,  261,  262,  263,
   -1,   -1,   -1,  264,  265,    0,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,    9,   -1,   11,  264,  265,  264,
  265,   16,   17,   -1,   -1,   -1,   -1,  264,  265,  264,
  265,   -1,   27,   28,   29,   30,   -1,   32,   -1,   34,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   47,   48,   -1,   50,   51,   -1,   -1,   54,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   63,   -1,
   -1,   66,   -1,   -1,   69,   -1,   -1,   72,
};
#define YYFINAL 12
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 266
#if YYDEBUG
static const char *cexpr_eval_name[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'!'",0,0,0,0,"'&'",0,"'('","')'","'*'","'+'","','","'-'",0,"'/'",0,0,0,0,0,0,0,
0,0,0,0,0,"'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'|'",0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,"STRG","T_NAME","NUMBER_INTEGER","NUMBER_REAL","T_PI",
"T_TRUE","T_FALSE","T_AND","T_OR","UMINUS",
};
static const char *cexpr_eval_rule[] = {
"$accept : result",
"result : error",
"result : comparisons",
"result : expression",
"comparisons : '!' comparisons",
"comparisons : comparisons '&' '&' comparisons",
"comparisons : comparisons T_AND comparisons",
"comparisons : comparisons '|' '|' comparisons",
"comparisons : comparisons T_OR comparisons",
"comparisons : '(' comparisons ')'",
"comparisons : comparison",
"comparison : expression '>' expression",
"comparison : expression '>' '=' expression",
"comparison : expression '<' expression",
"comparison : expression '<' '=' expression",
"comparison : expression '=' '=' expression",
"comparison : expression '!' '=' expression",
"expression : expression '+' expression",
"expression : expression '-' expression",
"expression : expression '*' expression",
"expression : expression '/' expression",
"expression : '-' expression",
"expression : '(' expression ')'",
"expression : T_NAME '(' expression ')'",
"expression : T_NAME '(' expression ',' expression ')'",
"expression : T_NAME '(' expression ',' expression ',' expression ')'",
"expression : T_NAME '(' expression ',' expression ',' expression ',' expression ')'",
"expression : T_NAME '(' expression ',' expression ',' expression ',' expression ',' expression ')'",
"expression : T_NAME '(' expression ',' expression ',' expression ',' expression ',' expression ',' expression ')'",
"expression : T_PI",
"expression : T_TRUE",
"expression : T_FALSE",
"expression : NUMBER_INTEGER",
"expression : NUMBER_REAL",
"expression : T_NAME",
"expression : STRG",

};
#endif

int      cexpr_eval_debug;
int      cexpr_eval_nerrs;

int      cexpr_eval_errflag;
int      cexpr_eval_char;
YYSTYPE  cexpr_eval_val;
YYSTYPE  cexpr_eval_lval;

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

typedef struct {
    unsigned stacksize;
    short    *s_base;
    short    *s_mark;
    short    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
/* variables for the parser stack */
static YYSTACKDATA cexpr_eval_stack;
#line 577 "../exlib/yacc/cexpr_eval.y"

static void cexpr_eval_error(exlib::yacc::cexpr_eval& aParser,const char* aString) {
  aParser.out() << "Processor : " << aString << std::endl;
  aParser.set_status(false);
}
#line 315 "y.tab.c"

#if YYDEBUG
#include <stdio.h>		/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int cexpr_eval_growstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = data->s_mark - data->s_base;
    newss = (short *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == 0)
        return -1;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == 0)
        return -1;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void cexpr_eval_freestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define cexpr_eval_freestack(data) /* nothing */
#endif

#define YYABORT  goto cexpr_eval_abort
#define YYREJECT goto cexpr_eval_abort
#define YYACCEPT goto cexpr_eval_accept
#define YYERROR  goto cexpr_eval_errlab

int
YYPARSE_DECL()
{
    int cexpr_eval_m, cexpr_eval_n, cexpr_eval_state;
#if YYDEBUG
    const char *cexpr_eval_s;

    if ((cexpr_eval_s = getenv("YYDEBUG")) != 0)
    {
        cexpr_eval_n = *cexpr_eval_s;
        if (cexpr_eval_n >= '0' && cexpr_eval_n <= '9')
            cexpr_eval_debug = cexpr_eval_n - '0';
    }
#endif

    cexpr_eval_nerrs = 0;
    cexpr_eval_errflag = 0;
    cexpr_eval_char = YYEMPTY;
    cexpr_eval_state = 0;

#if YYPURE
    memset(&cexpr_eval_stack, 0, sizeof(cexpr_eval_stack));
#endif

    if (cexpr_eval_stack.s_base == NULL && cexpr_eval_growstack(&cexpr_eval_stack)) goto cexpr_eval_overflow;
    cexpr_eval_stack.s_mark = cexpr_eval_stack.s_base;
    cexpr_eval_stack.l_mark = cexpr_eval_stack.l_base;
    cexpr_eval_state = 0;
    *cexpr_eval_stack.s_mark = 0;

cexpr_eval_loop:
    if ((cexpr_eval_n = cexpr_eval_defred[cexpr_eval_state]) != 0) goto cexpr_eval_reduce;
    if (cexpr_eval_char < 0)
    {
        if ((cexpr_eval_char = YYLEX) < 0) cexpr_eval_char = 0;
#if YYDEBUG
        if (cexpr_eval_debug)
        {
            cexpr_eval_s = 0;
            if (cexpr_eval_char <= YYMAXTOKEN) cexpr_eval_s = cexpr_eval_name[cexpr_eval_char];
            if (!cexpr_eval_s) cexpr_eval_s = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, cexpr_eval_state, cexpr_eval_char, cexpr_eval_s);
        }
#endif
    }
    if ((cexpr_eval_n = cexpr_eval_sindex[cexpr_eval_state]) && (cexpr_eval_n += cexpr_eval_char) >= 0 &&
            cexpr_eval_n <= YYTABLESIZE && cexpr_eval_check[cexpr_eval_n] == cexpr_eval_char)
    {
#if YYDEBUG
        if (cexpr_eval_debug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, cexpr_eval_state, cexpr_eval_table[cexpr_eval_n]);
#endif
        if (cexpr_eval_stack.s_mark >= cexpr_eval_stack.s_last && cexpr_eval_growstack(&cexpr_eval_stack))
        {
            goto cexpr_eval_overflow;
        }
        cexpr_eval_state = cexpr_eval_table[cexpr_eval_n];
        *++cexpr_eval_stack.s_mark = cexpr_eval_table[cexpr_eval_n];
        *++cexpr_eval_stack.l_mark = cexpr_eval_lval;
        cexpr_eval_char = YYEMPTY;
        if (cexpr_eval_errflag > 0)  --cexpr_eval_errflag;
        goto cexpr_eval_loop;
    }
    if ((cexpr_eval_n = cexpr_eval_rindex[cexpr_eval_state]) && (cexpr_eval_n += cexpr_eval_char) >= 0 &&
            cexpr_eval_n <= YYTABLESIZE && cexpr_eval_check[cexpr_eval_n] == cexpr_eval_char)
    {
        cexpr_eval_n = cexpr_eval_table[cexpr_eval_n];
        goto cexpr_eval_reduce;
    }
    if (cexpr_eval_errflag) goto cexpr_eval_inrecovery;

    cexpr_eval_error(aParser,"syntax error");

    goto cexpr_eval_errlab;

cexpr_eval_errlab:
    ++cexpr_eval_nerrs;

cexpr_eval_inrecovery:
    if (cexpr_eval_errflag < 3)
    {
        cexpr_eval_errflag = 3;
        for (;;)
        {
            if ((cexpr_eval_n = cexpr_eval_sindex[*cexpr_eval_stack.s_mark]) && (cexpr_eval_n += YYERRCODE) >= 0 &&
                    cexpr_eval_n <= YYTABLESIZE && cexpr_eval_check[cexpr_eval_n] == YYERRCODE)
            {
#if YYDEBUG
                if (cexpr_eval_debug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *cexpr_eval_stack.s_mark, cexpr_eval_table[cexpr_eval_n]);
#endif
                if (cexpr_eval_stack.s_mark >= cexpr_eval_stack.s_last && cexpr_eval_growstack(&cexpr_eval_stack))
                {
                    goto cexpr_eval_overflow;
                }
                cexpr_eval_state = cexpr_eval_table[cexpr_eval_n];
                *++cexpr_eval_stack.s_mark = cexpr_eval_table[cexpr_eval_n];
                *++cexpr_eval_stack.l_mark = cexpr_eval_lval;
                goto cexpr_eval_loop;
            }
            else
            {
#if YYDEBUG
                if (cexpr_eval_debug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *cexpr_eval_stack.s_mark);
#endif
                if (cexpr_eval_stack.s_mark <= cexpr_eval_stack.s_base) goto cexpr_eval_abort;
                --cexpr_eval_stack.s_mark;
                --cexpr_eval_stack.l_mark;
            }
        }
    }
    else
    {
        if (cexpr_eval_char == 0) goto cexpr_eval_abort;
#if YYDEBUG
        if (cexpr_eval_debug)
        {
            cexpr_eval_s = 0;
            if (cexpr_eval_char <= YYMAXTOKEN) cexpr_eval_s = cexpr_eval_name[cexpr_eval_char];
            if (!cexpr_eval_s) cexpr_eval_s = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, cexpr_eval_state, cexpr_eval_char, cexpr_eval_s);
        }
#endif
        cexpr_eval_char = YYEMPTY;
        goto cexpr_eval_loop;
    }

cexpr_eval_reduce:
#if YYDEBUG
    if (cexpr_eval_debug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, cexpr_eval_state, cexpr_eval_n, cexpr_eval_rule[cexpr_eval_n]);
#endif
    cexpr_eval_m = cexpr_eval_len[cexpr_eval_n];
    if (cexpr_eval_m)
        cexpr_eval_val = cexpr_eval_stack.l_mark[1-cexpr_eval_m];
    else
        memset(&cexpr_eval_val, 0, sizeof cexpr_eval_val);
    switch (cexpr_eval_n)
    {
case 1:
#line 45 "../exlib/yacc/cexpr_eval.y"
	{
  /* To reset lex and yacc in case of error.*/
}
break;
case 2:
#line 47 "../exlib/yacc/cexpr_eval.y"
	{
  if(aParser.find_names()||aParser.find_functions()) {
    aParser.set_result(0);
  } else {
    aParser.set_result(cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
  }
}
break;
case 3:
#line 54 "../exlib/yacc/cexpr_eval.y"
	{
  if(aParser.find_names()||aParser.find_functions()) {
    aParser.set_result(0);
  } else {
    aParser.set_result(cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
  }
}
break;
case 4:
#line 64 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : ! cps" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::NOT,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 5:
#line 75 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : cps && cps" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::CMP_AND,cexpr_eval_stack.l_mark[-3].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 6:
#line 87 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : cps && cps" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::CMP_AND,cexpr_eval_stack.l_mark[-2].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 7:
#line 99 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : cps || cps" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::CMP_OR,cexpr_eval_stack.l_mark[-3].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 8:
#line 111 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : cps || cps" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::CMP_OR,cexpr_eval_stack.l_mark[-2].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 9:
#line 123 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : (cps)" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::ASSIGN,cexpr_eval_stack.l_mark[-1].m_expression);
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 10:
#line 134 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : cp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::ASSIGN,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 11:
#line 148 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp > exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::CMP_GT,cexpr_eval_stack.l_mark[-2].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 12:
#line 160 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp >= exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::CMP_GE,cexpr_eval_stack.l_mark[-3].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 13:
#line 172 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp < exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::CMP_LT,cexpr_eval_stack.l_mark[-2].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 14:
#line 184 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp <= exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::CMP_LE,cexpr_eval_stack.l_mark[-3].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 15:
#line 196 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp == exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::CMP_EQ,cexpr_eval_stack.l_mark[-3].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 16:
#line 208 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp != exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::CMP_NE,cexpr_eval_stack.l_mark[-3].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
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
}
break;
case 17:
#line 297 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp + exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::ADD,cexpr_eval_stack.l_mark[-2].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 18:
#line 309 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp - exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::SUB,cexpr_eval_stack.l_mark[-2].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 19:
#line 321 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp * exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::MUL,cexpr_eval_stack.l_mark[-2].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
  /*
} | expression '*' '*' expression {
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp ** exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    $$ = 0;
  } else {
    $$ = new inlib::valop
      (inlib::valop::FUNC,aParser.find_function("pow"),$1,$4);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back($$);
  }
  */
}
break;
case 20:
#line 348 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : exp / exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::DIV,cexpr_eval_stack.l_mark[-2].m_expression,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 21:
#line 360 "../exlib/yacc/cexpr_eval.y"
	{ 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : - exp" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::MINUS,cexpr_eval_stack.l_mark[0].m_expression);
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 22:
#line 371 "../exlib/yacc/cexpr_eval.y"
	{ 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : (exp)" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::ASSIGN,cexpr_eval_stack.l_mark[-1].m_expression);
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }

}
break;
case 23:
#line 383 "../exlib/yacc/cexpr_eval.y"
	{ 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME(1)" << std::endl;
#endif
  if(aParser.find_names()) {
    cexpr_eval_val.m_expression = 0;
  } else if(aParser.find_functions()) {
    aParser.add_name(aParser.lexer().variable(cexpr_eval_stack.l_mark[-3].m_index));
    cexpr_eval_val.m_expression = 0;
  } else {
    std::string var = aParser.lexer().variable(cexpr_eval_stack.l_mark[-3].m_index);
    cexpr_eval_val.m_expression = new inlib::valop
      (inlib::valop::FUNC,aParser.find_function(var),
       cexpr_eval_stack.l_mark[-1].m_expression);
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 24:
#line 400 "../exlib/yacc/cexpr_eval.y"
	{ 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME(2)" << std::endl;
#endif
  if(aParser.find_names()) {
    cexpr_eval_val.m_expression = 0;
  } else if(aParser.find_functions()) {
    aParser.add_name(aParser.lexer().variable(cexpr_eval_stack.l_mark[-5].m_index));
    cexpr_eval_val.m_expression = 0;
  } else {
    std::string var = aParser.lexer().variable(cexpr_eval_stack.l_mark[-5].m_index);    
    cexpr_eval_val.m_expression = new inlib::valop
      (inlib::valop::FUNC,aParser.find_function(var),cexpr_eval_stack.l_mark[-3].m_expression,cexpr_eval_stack.l_mark[-1].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 25:
#line 417 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME(3)" << std::endl;
#endif
  if(aParser.find_names()) {
    cexpr_eval_val.m_expression = 0;
  } else if(aParser.find_functions()) {
    aParser.add_name(aParser.lexer().variable(cexpr_eval_stack.l_mark[-7].m_index));
    cexpr_eval_val.m_expression = 0;
  } else {
    std::string var = aParser.lexer().variable(cexpr_eval_stack.l_mark[-7].m_index);    
    cexpr_eval_val.m_expression = new inlib::valop
      (inlib::valop::FUNC,aParser.find_function(var),cexpr_eval_stack.l_mark[-5].m_expression,cexpr_eval_stack.l_mark[-3].m_expression,cexpr_eval_stack.l_mark[-1].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 26:
#line 435 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME(4)" << std::endl;
#endif
  if(aParser.find_names()) {
    cexpr_eval_val.m_expression = 0;
  } else if(aParser.find_functions()) {
    aParser.add_name(aParser.lexer().variable(cexpr_eval_stack.l_mark[-9].m_index));
    cexpr_eval_val.m_expression = 0;
  } else {
    std::string var = aParser.lexer().variable(cexpr_eval_stack.l_mark[-9].m_index);    
    cexpr_eval_val.m_expression = new inlib::valop
      (inlib::valop::FUNC,aParser.find_function(var),cexpr_eval_stack.l_mark[-7].m_expression,cexpr_eval_stack.l_mark[-5].m_expression,cexpr_eval_stack.l_mark[-3].m_expression,cexpr_eval_stack.l_mark[-1].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 27:
#line 454 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME(5)" << std::endl;
#endif
  if(aParser.find_names()) {
    cexpr_eval_val.m_expression = 0;
  } else if(aParser.find_functions()) {
    aParser.add_name(aParser.lexer().variable(cexpr_eval_stack.l_mark[-11].m_index));
    cexpr_eval_val.m_expression = 0;
  } else {
    std::string var = aParser.lexer().variable(cexpr_eval_stack.l_mark[-11].m_index);
    cexpr_eval_val.m_expression = new inlib::valop
      (inlib::valop::FUNC,aParser.find_function(var),cexpr_eval_stack.l_mark[-9].m_expression,cexpr_eval_stack.l_mark[-7].m_expression,cexpr_eval_stack.l_mark[-5].m_expression,cexpr_eval_stack.l_mark[-3].m_expression,cexpr_eval_stack.l_mark[-1].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 28:
#line 474 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME(6)" << std::endl;
#endif
  if(aParser.find_names()) {
    cexpr_eval_val.m_expression = 0;
  } else if(aParser.find_functions()) {
    aParser.add_name(aParser.lexer().variable(cexpr_eval_stack.l_mark[-13].m_index));
    cexpr_eval_val.m_expression = 0;
  } else {
    std::string var = aParser.lexer().variable(cexpr_eval_stack.l_mark[-13].m_index);    
    cexpr_eval_val.m_expression = new inlib::valop
      (inlib::valop::FUNC,aParser.find_function(var),cexpr_eval_stack.l_mark[-11].m_expression,cexpr_eval_stack.l_mark[-9].m_expression,cexpr_eval_stack.l_mark[-7].m_expression,cexpr_eval_stack.l_mark[-5].m_expression,cexpr_eval_stack.l_mark[-3].m_expression,cexpr_eval_stack.l_mark[-1].m_expression);
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().pop_back();
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }

}
break;
case 29:
#line 496 "../exlib/yacc/cexpr_eval.y"
	{ 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_PI" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::REAL,inlib::pi());
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 30:
#line 506 "../exlib/yacc/cexpr_eval.y"
	{ 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_TRUE" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::BOOL_TRUE,true);
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 31:
#line 516 "../exlib/yacc/cexpr_eval.y"
	{ 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_FALSE" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::BOOL_FALSE,false);
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 32:
#line 526 "../exlib/yacc/cexpr_eval.y"
	{ 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : NUMBER_INTEGER" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::UNSIGNED_INTEGER,(unsigned int)cexpr_eval_stack.l_mark[0].m_integer);
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 33:
#line 536 "../exlib/yacc/cexpr_eval.y"
	{ 
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : NUMBER_REAL" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::REAL,cexpr_eval_stack.l_mark[0].m_number);
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 34:
#line 546 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : T_NAME" << std::endl;
#endif
  if(aParser.find_names()) {
    aParser.add_name(aParser.lexer().variable(cexpr_eval_stack.l_mark[0].m_index));
    cexpr_eval_val.m_expression = 0;
  } else if(aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    std::string var = aParser.lexer().variable(cexpr_eval_stack.l_mark[0].m_index);
    unsigned int index;
    int ivar = NotFound;
    if(aParser.find_variable(var,index)) ivar = int(index);
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::NAME,var,ivar);
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
case 35:
#line 563 "../exlib/yacc/cexpr_eval.y"
	{
#ifdef DEBUG
  aParser.out() << "debug : cexpr_eval.y : STRG" << std::endl;
#endif
  if(aParser.find_names()||aParser.find_functions()) {
    cexpr_eval_val.m_expression = 0;
  } else {
    cexpr_eval_val.m_expression = new inlib::valop(inlib::valop::STRING,
                             aParser.lexer().variable(cexpr_eval_stack.l_mark[0].m_index));
    aParser.stack().push_back(cexpr_eval_val.m_expression);
  }
}
break;
#line 1181 "y.tab.c"
    }
    cexpr_eval_stack.s_mark -= cexpr_eval_m;
    cexpr_eval_state = *cexpr_eval_stack.s_mark;
    cexpr_eval_stack.l_mark -= cexpr_eval_m;
    cexpr_eval_m = cexpr_eval_lhs[cexpr_eval_n];
    if (cexpr_eval_state == 0 && cexpr_eval_m == 0)
    {
#if YYDEBUG
        if (cexpr_eval_debug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        cexpr_eval_state = YYFINAL;
        *++cexpr_eval_stack.s_mark = YYFINAL;
        *++cexpr_eval_stack.l_mark = cexpr_eval_val;
        if (cexpr_eval_char < 0)
        {
            if ((cexpr_eval_char = YYLEX) < 0) cexpr_eval_char = 0;
#if YYDEBUG
            if (cexpr_eval_debug)
            {
                cexpr_eval_s = 0;
                if (cexpr_eval_char <= YYMAXTOKEN) cexpr_eval_s = cexpr_eval_name[cexpr_eval_char];
                if (!cexpr_eval_s) cexpr_eval_s = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, cexpr_eval_char, cexpr_eval_s);
            }
#endif
        }
        if (cexpr_eval_char == 0) goto cexpr_eval_accept;
        goto cexpr_eval_loop;
    }
    if ((cexpr_eval_n = cexpr_eval_gindex[cexpr_eval_m]) && (cexpr_eval_n += cexpr_eval_state) >= 0 &&
            cexpr_eval_n <= YYTABLESIZE && cexpr_eval_check[cexpr_eval_n] == cexpr_eval_state)
        cexpr_eval_state = cexpr_eval_table[cexpr_eval_n];
    else
        cexpr_eval_state = cexpr_eval_dgoto[cexpr_eval_m];
#if YYDEBUG
    if (cexpr_eval_debug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *cexpr_eval_stack.s_mark, cexpr_eval_state);
#endif
    if (cexpr_eval_stack.s_mark >= cexpr_eval_stack.s_last && cexpr_eval_growstack(&cexpr_eval_stack))
    {
        goto cexpr_eval_overflow;
    }
    *++cexpr_eval_stack.s_mark = (short) cexpr_eval_state;
    *++cexpr_eval_stack.l_mark = cexpr_eval_val;
    goto cexpr_eval_loop;

cexpr_eval_overflow:
    cexpr_eval_error(aParser,"yacc stack overflow");

cexpr_eval_abort:
    cexpr_eval_freestack(&cexpr_eval_stack);
    return (1);

cexpr_eval_accept:
    cexpr_eval_freestack(&cexpr_eval_stack);
    return (0);
}
