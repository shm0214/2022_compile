/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* "%code top" blocks.  */
#line 1 "src/parser.y" /* yacc.c:316  */

    #include <iostream>
    #include <cstring>
    #include <assert.h>
    #include "parser.h"
    #include <stack>
    extern Ast ast;

    int yylex();
    int yyerror(char const*);
    ArrayType* arrayType;
    Type* declType; // store type for variable declarations.
    Type* funcRetType; // store type for return value of funtion declarations.
    int idx;
    double* arrayValue; // store all number in float
    std::stack<InitValueListExpr*> stk;
    std::stack<StmtNode*> whileStk;
    InitValueListExpr* top;
    int leftCnt = 0;
    int whileCnt = 0;
    int paramNo = 0;
    int fpParamNo = 0;
    int notZeroNum = 0;
    extern int yylineno;
    #include <iostream>

#line 90 "src/parser.cpp" /* yacc.c:316  */



/* Copy the first part of user declarations.  */

#line 96 "src/parser.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "parser.h".  */
#ifndef YY_YY_INCLUDE_PARSER_H_INCLUDED
# define YY_YY_INCLUDE_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 28 "src/parser.y" /* yacc.c:355  */

    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"

#line 132 "src/parser.cpp" /* yacc.c:355  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ID = 258,
    STRING = 259,
    INTEGER = 260,
    FLOATING = 261,
    IF = 262,
    ELSE = 263,
    WHILE = 264,
    INT = 265,
    VOID = 266,
    FLOAT = 267,
    LPAREN = 268,
    RPAREN = 269,
    LBRACE = 270,
    RBRACE = 271,
    SEMICOLON = 272,
    LBRACKET = 273,
    RBRACKET = 274,
    COMMA = 275,
    ADD = 276,
    SUB = 277,
    MUL = 278,
    DIV = 279,
    MOD = 280,
    OR = 281,
    AND = 282,
    LESS = 283,
    LESSEQUAL = 284,
    GREATER = 285,
    GREATEREQUAL = 286,
    ASSIGN = 287,
    EQUAL = 288,
    NOTEQUAL = 289,
    NOT = 290,
    CONST = 291,
    RETURN = 292,
    CONTINUE = 293,
    BREAK = 294,
    THEN = 295
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 34 "src/parser.y" /* yacc.c:355  */

    double numtype; // store all number in float
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;
    SymbolEntry* se;

#line 194 "src/parser.cpp" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_INCLUDE_PARSER_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 211 "src/parser.cpp" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  60
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   247

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  41
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  52
/* YYNRULES -- Number of rules.  */
#define YYNRULES  110
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  183

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   295

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    67,    67,    72,    73,    78,    79,    80,    81,    82,
      83,    84,    89,    94,    95,    96,    99,   107,   117,   122,
     127,   132,   132,   143,   149,   152,   157,   157,   172,   177,
     182,   185,   196,   200,   203,   206,   209,   222,   226,   232,
     233,   240,   251,   252,   262,   282,   283,   294,   305,   319,
     320,   331,   344,   347,   352,   357,   362,   369,   370,   375,
     382,   383,   390,   391,   398,   401,   402,   407,   411,   414,
     420,   421,   424,   427,   433,   437,   440,   444,   447,   455,
     482,   498,   498,   536,   562,   562,   611,   614,   620,   676,
     702,   702,   740,   799,   825,   825,   862,   865,   870,   873,
     879,   886,   879,   913,   914,   916,   920,   925,   940,   968,
     971
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "STRING", "INTEGER", "FLOATING",
  "IF", "ELSE", "WHILE", "INT", "VOID", "FLOAT", "LPAREN", "RPAREN",
  "LBRACE", "RBRACE", "SEMICOLON", "LBRACKET", "RBRACKET", "COMMA", "ADD",
  "SUB", "MUL", "DIV", "MOD", "OR", "AND", "LESS", "LESSEQUAL", "GREATER",
  "GREATEREQUAL", "ASSIGN", "EQUAL", "NOTEQUAL", "NOT", "CONST", "RETURN",
  "CONTINUE", "BREAK", "THEN", "$accept", "Program", "Stmts", "Stmt",
  "LVal", "AssignStmt", "ExprStmt", "BlankStmt", "BlockStmt", "$@1",
  "IfStmt", "WhileStmt", "@2", "BreakStmt", "ContinueStmt", "ReturnStmt",
  "Exp", "Cond", "PrimaryExp", "UnaryExp", "MulExp", "AddExp", "RelExp",
  "EqExp", "LAndExp", "LOrExp", "ConstExp", "FuncRParams", "Type",
  "DeclStmt", "VarDeclStmt", "ConstDeclStmt", "VarDefList", "ConstDefList",
  "VarDef", "@3", "ConstDef", "@4", "ArrayIndices", "InitVal", "@5",
  "ConstInitVal", "@6", "InitValList", "ConstInitValList", "FuncDef",
  "$@7", "@8", "MaybeFuncFParams", "FuncFParams", "FuncFParam",
  "FuncArrayIndices", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295
};
# endif

#define YYPACT_NINF -130

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-130)))

#define YYTABLE_NINF -101

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      92,    32,  -130,  -130,  -130,    -7,     9,  -130,  -130,  -130,
     212,    31,  -130,   212,   212,   212,   133,    13,    21,    49,
      57,    92,  -130,    17,  -130,  -130,  -130,  -130,  -130,  -130,
    -130,  -130,  -130,    51,  -130,  -130,   126,    10,    83,  -130,
    -130,  -130,  -130,   112,   212,    72,   212,   212,  -130,    78,
    -130,    92,  -130,  -130,  -130,    91,  -130,    89,  -130,  -130,
    -130,  -130,   212,  -130,   212,   212,   212,   212,   212,    11,
      50,  -130,  -130,  -130,    -5,    10,    81,   212,    96,    10,
      54,    28,    85,    98,   118,  -130,   160,     7,   102,  -130,
    -130,   129,  -130,  -130,  -130,   126,   126,   187,    19,   122,
    -130,   149,  -130,   212,  -130,   134,    92,   212,   212,   212,
     212,   212,   212,   212,   212,  -130,  -130,   208,    22,  -130,
      91,  -130,   138,  -130,  -130,  -130,   133,    26,  -130,  -130,
    -130,   148,    10,    10,    10,    10,    54,    54,    28,    85,
      92,   141,  -130,  -130,  -130,  -130,  -130,   187,   187,   155,
     146,   142,  -130,    92,  -130,  -130,   208,   208,  -130,    39,
    -130,   143,  -130,   133,  -130,  -130,    40,  -130,  -130,   187,
     159,   156,   164,  -130,  -130,   208,  -130,  -130,   212,  -130,
    -130,   161,  -130
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    16,    36,    37,    38,     0,     0,    67,    68,    69,
       0,    21,    20,     0,     0,     0,     0,     0,     0,     0,
       0,     2,     3,    35,     5,     6,     8,     7,     9,    10,
      11,    12,    13,     0,    39,    45,    49,    32,     0,    14,
      70,    71,    15,     0,     0,    17,     0,     0,    35,     0,
      23,     0,    42,    43,    44,     0,    30,     0,    29,    28,
       1,     4,     0,    19,     0,     0,     0,     0,     0,    78,
       0,    75,    41,    65,     0,    64,     0,     0,     0,    52,
      57,    60,    62,    33,     0,    34,     0,     0,     0,    77,
      31,     0,    46,    47,    48,    50,    51,     0,    79,     0,
      72,     0,    40,     0,    86,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    26,    22,     0,     0,    73,
       0,    18,    90,    88,    80,    81,   104,    78,    74,    66,
      87,    24,    53,    54,    55,    56,    58,    59,    61,    63,
       0,    94,    92,    83,    84,    76,    89,     0,     0,     0,
       0,   103,   106,     0,    27,    93,     0,     0,    96,     0,
      82,   107,   101,     0,    25,    98,     0,    85,    91,     0,
       0,   108,     0,   105,    95,     0,    97,   109,     0,   102,
      99,     0,   110
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -130,  -130,   132,   -17,     2,  -130,  -130,  -130,    14,  -130,
    -130,  -130,  -130,  -130,  -130,  -130,   -10,   137,  -130,    -1,
      12,   -36,    29,    74,    71,  -130,   -41,  -130,   -15,  -130,
    -130,  -130,  -130,  -130,    87,  -130,    69,  -130,     4,  -127,
    -130,  -129,  -130,  -130,  -130,  -130,  -130,  -130,  -130,  -130,
      38,  -130
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    20,    21,    22,    48,    24,    25,    26,    27,    51,
      28,    29,   140,    30,    31,    32,    33,    78,    34,    35,
      36,    37,    80,    81,    82,    83,   142,    74,    38,    39,
      40,    41,    70,    88,    71,   148,    89,   157,    98,   124,
     147,   143,   156,   159,   166,    42,    99,   172,   150,   151,
     152,   171
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      49,    55,    23,    76,    61,    45,    46,    57,    75,   102,
      79,    79,    52,    53,    54,   103,     1,     2,     3,     4,
     158,   160,    47,    23,  -100,    44,    10,   165,   167,    44,
      56,    67,    68,    73,    13,    14,   105,    77,    58,   117,
      77,    75,   176,    97,    44,    43,   180,    50,    15,    62,
      44,   125,    91,    23,   144,   168,   174,    60,    97,   169,
     175,   111,   112,    92,    93,    94,    59,   100,    63,    61,
     101,   132,   133,   134,   135,    79,    79,    79,    79,    95,
      96,    75,   107,   108,   109,   110,    69,   123,    23,   131,
      77,   118,    85,   129,    87,     1,     2,     3,     4,     5,
     104,     6,     7,     8,     9,    10,    90,    11,    23,    12,
     106,   149,   113,    13,    14,     1,     2,     3,     4,   119,
      75,    75,   120,   154,   114,    10,    72,    15,    16,    17,
      18,    19,   115,    13,    14,   126,   164,   123,   123,    75,
     136,   137,    23,     7,     8,     9,   121,    15,   149,    64,
      65,    66,   127,   130,   146,    23,   153,   155,   161,   123,
     162,   170,   163,     1,     2,     3,     4,     5,   181,     6,
       7,     8,     9,    10,   178,    11,   116,    12,   177,    11,
     182,    13,    14,    86,    84,   139,   179,   138,   128,   145,
       1,     2,     3,     4,     0,    15,    16,    17,    18,    19,
      10,   173,   122,     0,     0,     0,     0,     0,    13,    14,
       0,     1,     2,     3,     4,     1,     2,     3,     4,     0,
       0,    10,    15,   141,     0,    10,     0,     0,     0,    13,
      14,     0,     0,    13,    14,     0,     0,     0,     0,     0,
       0,     0,     0,    15,     0,     0,     0,    15
};

static const yytype_int16 yycheck[] =
{
      10,    16,     0,    44,    21,     1,    13,    17,    44,    14,
      46,    47,    13,    14,    15,    20,     3,     4,     5,     6,
     147,   148,    13,    21,    13,    18,    13,   156,   157,    18,
      17,    21,    22,    43,    21,    22,    77,    18,    17,    32,
      18,    77,   169,    32,    18,    13,   175,    16,    35,    32,
      18,    32,    62,    51,    32,    16,    16,     0,    32,    20,
      20,    33,    34,    64,    65,    66,    17,    17,    17,    86,
      20,   107,   108,   109,   110,   111,   112,   113,   114,    67,
      68,   117,    28,    29,    30,    31,     3,    97,    86,   106,
      18,    87,    14,   103,     3,     3,     4,     5,     6,     7,
      19,     9,    10,    11,    12,    13,    17,    15,   106,    17,
      14,   126,    27,    21,    22,     3,     4,     5,     6,    17,
     156,   157,    20,   140,    26,    13,    14,    35,    36,    37,
      38,    39,    14,    21,    22,    13,   153,   147,   148,   175,
     111,   112,   140,    10,    11,    12,    17,    35,   163,    23,
      24,    25,     3,    19,    16,   153,     8,    16,     3,   169,
      14,    18,    20,     3,     4,     5,     6,     7,   178,     9,
      10,    11,    12,    13,    18,    15,    16,    17,    19,    15,
      19,    21,    22,    51,    47,   114,   172,   113,   101,   120,
       3,     4,     5,     6,    -1,    35,    36,    37,    38,    39,
      13,   163,    15,    -1,    -1,    -1,    -1,    -1,    21,    22,
      -1,     3,     4,     5,     6,     3,     4,     5,     6,    -1,
      -1,    13,    35,    15,    -1,    13,    -1,    -1,    -1,    21,
      22,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    35,    -1,    -1,    -1,    35
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     9,    10,    11,    12,
      13,    15,    17,    21,    22,    35,    36,    37,    38,    39,
      42,    43,    44,    45,    46,    47,    48,    49,    51,    52,
      54,    55,    56,    57,    59,    60,    61,    62,    69,    70,
      71,    72,    86,    13,    18,    79,    13,    13,    45,    57,
      16,    50,    60,    60,    60,    69,    17,    57,    17,    17,
       0,    44,    32,    17,    23,    24,    25,    21,    22,     3,
      73,    75,    14,    57,    68,    62,    67,    18,    58,    62,
      63,    64,    65,    66,    58,    14,    43,     3,    74,    77,
      17,    57,    60,    60,    60,    61,    61,    32,    79,    87,
      17,    20,    14,    20,    19,    67,    14,    28,    29,    30,
      31,    33,    34,    27,    26,    14,    16,    32,    79,    17,
      20,    17,    15,    57,    80,    32,    13,     3,    75,    57,
      19,    44,    62,    62,    62,    62,    63,    63,    64,    65,
      53,    15,    67,    82,    32,    77,    16,    81,    76,    69,
      89,    90,    91,     8,    44,    16,    83,    78,    80,    84,
      80,     3,    14,    20,    44,    82,    85,    82,    16,    20,
      18,    92,    88,    91,    16,    20,    80,    19,    18,    49,
      82,    57,    19
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    41,    42,    43,    43,    44,    44,    44,    44,    44,
      44,    44,    44,    44,    44,    44,    45,    45,    46,    47,
      48,    50,    49,    49,    51,    51,    53,    52,    54,    55,
      56,    56,    57,    58,    59,    59,    59,    59,    59,    60,
      60,    60,    60,    60,    60,    61,    61,    61,    61,    62,
      62,    62,    63,    63,    63,    63,    63,    64,    64,    64,
      65,    65,    66,    66,    67,    68,    68,    69,    69,    69,
      70,    70,    71,    72,    73,    73,    74,    74,    75,    75,
      75,    76,    75,    77,    78,    77,    79,    79,    80,    80,
      81,    80,    82,    82,    83,    82,    84,    84,    85,    85,
      87,    88,    86,    89,    89,    90,    90,    91,    91,    92,
      92
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     4,     2,
       1,     0,     4,     2,     5,     7,     0,     6,     2,     2,
       2,     3,     1,     1,     3,     1,     1,     1,     1,     1,
       4,     3,     2,     2,     2,     1,     3,     3,     3,     1,
       3,     3,     1,     3,     3,     3,     3,     1,     3,     3,
       1,     3,     1,     3,     1,     1,     3,     1,     1,     1,
       1,     1,     3,     4,     3,     1,     3,     1,     1,     2,
       3,     0,     5,     3,     0,     5,     3,     4,     1,     2,
       0,     4,     1,     2,     0,     4,     1,     3,     1,     3,
       0,     0,     8,     1,     0,     3,     1,     2,     3,     2,
       4
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 67 "src/parser.y" /* yacc.c:1646  */
    {
        ast.setRoot((yyvsp[0].stmttype));
    }
#line 1444 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 3:
#line 72 "src/parser.y" /* yacc.c:1646  */
    {(yyval.stmttype)=(yyvsp[0].stmttype);}
#line 1450 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 73 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = new SeqNode((yyvsp[-1].stmttype), (yyvsp[0].stmttype));
    }
#line 1458 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 5:
#line 78 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1464 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 79 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1470 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 80 "src/parser.y" /* yacc.c:1646  */
    {(yyval.stmttype) = (yyvsp[0].stmttype);}
#line 1476 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 81 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1482 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 82 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1488 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 83 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1494 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 84 "src/parser.y" /* yacc.c:1646  */
    {
        if (!whileCnt)
            fprintf(stderr, "\'break\' statement not in while statement\n");
        (yyval.stmttype) = (yyvsp[0].stmttype);
    }
#line 1504 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 89 "src/parser.y" /* yacc.c:1646  */
    {
        if (!whileCnt)
            fprintf(stderr, "\'continue\' statement not in while statement\n");
        (yyval.stmttype) = (yyvsp[0].stmttype);
    }
#line 1514 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 94 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1520 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 95 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1526 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 96 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1532 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 99 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        se = identifiers->lookup((yyvsp[0].strtype));
        if (se == nullptr)
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)(yyvsp[0].strtype));
        (yyval.exprtype) = new Id(se);
        delete [](yyvsp[0].strtype);
    }
#line 1545 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 107 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        se = identifiers->lookup((yyvsp[-1].strtype));
        if (se == nullptr)
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)(yyvsp[-1].strtype));
        (yyval.exprtype) = new Id(se, (yyvsp[0].exprtype));
        delete [](yyvsp[-1].strtype);
    }
#line 1558 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 117 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = new AssignStmt((yyvsp[-3].exprtype), (yyvsp[-1].exprtype));
    }
#line 1566 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 122 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = new ExprStmt((yyvsp[-1].exprtype));
    }
#line 1574 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 127 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = new BlankStmt();
    }
#line 1582 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 132 "src/parser.y" /* yacc.c:1646  */
    {
        identifiers = new SymbolTable(identifiers);
    }
#line 1590 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 135 "src/parser.y" /* yacc.c:1646  */
    {
        // midrule actions https://www.gnu.org/software/bison/manual/html_node/Using-Midrule-Actions.html
        (yyval.stmttype) = new CompoundStmt((yyvsp[-1].stmttype));

        SymbolTable* top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
#line 1603 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 143 "src/parser.y" /* yacc.c:1646  */
    {
        // 这里这个用加嘛 不确定
        (yyval.stmttype) = new CompoundStmt();
    }
#line 1612 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 149 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = new IfStmt((yyvsp[-2].exprtype), (yyvsp[0].stmttype));
    }
#line 1620 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 152 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = new IfElseStmt((yyvsp[-4].exprtype), (yyvsp[-2].stmttype), (yyvsp[0].stmttype));
    }
#line 1628 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 157 "src/parser.y" /* yacc.c:1646  */
    {
        whileCnt++;
        WhileStmt *whileNode = new WhileStmt((yyvsp[-1].exprtype));
        (yyval.stmttype) = whileNode;
        whileStk.push(whileNode);
    }
#line 1639 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 163 "src/parser.y" /* yacc.c:1646  */
    {
        StmtNode *whileNode = (yyvsp[-1].stmttype); 
        ((WhileStmt*)whileNode)->setStmt((yyvsp[0].stmttype));
        (yyval.stmttype) = whileNode;
        whileStk.pop();
        whileCnt--;
    }
#line 1651 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 172 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = new BreakStmt(whileStk.top());
    }
#line 1659 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 177 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = new ContinueStmt(whileStk.top());
    }
#line 1667 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 182 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = new ReturnStmt();
    }
#line 1675 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 185 "src/parser.y" /* yacc.c:1646  */
    {
        if (((yyvsp[-1].exprtype)->getType()->isFloat() && funcRetType->isInt()) ||
            ((yyvsp[-1].exprtype)->getType()->isInt() && funcRetType->isFloat())) {
            (yyval.stmttype) = new ReturnStmt(new ImplicitCastExpr((yyvsp[-1].exprtype), funcRetType));
        } else {
            (yyval.stmttype) = new ReturnStmt((yyvsp[-1].exprtype));            
        }
    }
#line 1688 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 196 "src/parser.y" /* yacc.c:1646  */
    { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1694 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 200 "src/parser.y" /* yacc.c:1646  */
    { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1700 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 203 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.exprtype) = (yyvsp[-1].exprtype);
    }
#line 1708 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 206 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 1716 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 209 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        se = globals->lookup(std::string((yyvsp[0].strtype)));
        // 这里如果str内容和变量名相同 怎么处理
        if (se == nullptr) {
            Type* type = new StringType(strlen((yyvsp[0].strtype)));
            se = new ConstantSymbolEntry(type, std::string((yyvsp[0].strtype)));
            globals->install(std::string((yyvsp[0].strtype)), se);
        }
        ExprNode* expr = new ExprNode(se);

        (yyval.exprtype) = expr;
    }
#line 1734 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 222 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::intType, (yyvsp[0].numtype));
        (yyval.exprtype) = new Constant(se);
    }
#line 1743 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 226 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::floatType, (yyvsp[0].numtype));
        (yyval.exprtype) = new Constant(se);
    }
#line 1752 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 232 "src/parser.y" /* yacc.c:1646  */
    { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1758 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 233 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        se = identifiers->lookup((yyvsp[-3].strtype));
        if (se == nullptr)
            fprintf(stderr, "function \"%s\" is undefined\n", (char*)(yyvsp[-3].strtype));
        (yyval.exprtype) = new CallExpr(se, (yyvsp[-1].exprtype));
    }
#line 1770 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 240 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        se = identifiers->lookup((yyvsp[-2].strtype));
        if (se == nullptr)
            fprintf(stderr, "function \"%s\" is undefined\n", (char*)(yyvsp[-2].strtype));
        if (strcmp((yyvsp[-2].strtype), "_sysy_starttime") == 0 || strcmp((yyvsp[-2].strtype), "_sysy_stoptime") == 0) {
            ExprNode* param = new Constant(new ConstantSymbolEntry(TypeSystem::intType, yylineno));
            (yyval.exprtype) = new CallExpr(se, param);
        } else
            (yyval.exprtype) = new CallExpr(se);
    }
#line 1786 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 251 "src/parser.y" /* yacc.c:1646  */
    { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1792 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 252 "src/parser.y" /* yacc.c:1646  */
    {
        Type* exprType = (yyvsp[0].exprtype)->getType();
        SymbolEntry* se = new TemporarySymbolEntry(exprType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new UnaryExpr(se, UnaryExpr::SUB, (yyvsp[0].exprtype));
        if (exprType->isFloat()) {
            (yyval.exprtype) = tmpExpr;
        } else {
            (yyval.exprtype) = tmpExpr->const_fold();
        }
    }
#line 1807 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 262 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr;
        if ((yyvsp[0].exprtype)->getType()->isFloat()) {

            SymbolEntry* zero =
                new ConstantSymbolEntry(TypeSystem::constFloatType, 0);
            SymbolEntry* temp = new TemporarySymbolEntry(
                TypeSystem::boolType, SymbolTable::getLabel());
            BinaryExpr* cmpZero = new BinaryExpr(temp, BinaryExpr::NOTEQUAL,
                                                    (yyvsp[0].exprtype), new Constant(zero));

            (yyval.exprtype) = new UnaryExpr(se, UnaryExpr::NOT, cmpZero);
        } else {
            tmpExpr = new UnaryExpr(se, UnaryExpr::NOT, (yyvsp[0].exprtype));
            (yyval.exprtype) = tmpExpr->const_fold();
        }
    }
#line 1830 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 282 "src/parser.y" /* yacc.c:1646  */
    { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1836 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 283 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::MUL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        } else {
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::MUL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
            (yyval.exprtype) = tmpExpr->const_fold();
        }
    }
#line 1852 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 294 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::DIV, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        } else {
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::DIV, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
            (yyval.exprtype) = tmpExpr->const_fold();
        }
    }
#line 1868 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 305 "src/parser.y" /* yacc.c:1646  */
    {

        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            // error
            // already handled in `ast.cpp`
            fprintf(stderr, "Operands of `mod` must be both integers");
        }

        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::MOD, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1885 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 319 "src/parser.y" /* yacc.c:1646  */
    { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1891 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 320 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::ADD, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        } else {
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::ADD, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
            (yyval.exprtype) = tmpExpr->const_fold();
        }
    }
#line 1907 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 331 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::SUB, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        } else {
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::SUB, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
            (yyval.exprtype) = tmpExpr->const_fold();
        }
    }
#line 1923 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 344 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 1931 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 347 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::LESS, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1941 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 352 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::LESSEQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1951 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 357 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::GREATER, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1961 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 362 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::GREATEREQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1971 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 369 "src/parser.y" /* yacc.c:1646  */
    { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1977 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 370 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::EQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1987 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 59:
#line 375 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::NOTEQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1997 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 382 "src/parser.y" /* yacc.c:1646  */
    { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 2003 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 383 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::AND, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 2013 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 390 "src/parser.y" /* yacc.c:1646  */
    { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 2019 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 391 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::OR, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 2029 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 398 "src/parser.y" /* yacc.c:1646  */
    { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 2035 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 401 "src/parser.y" /* yacc.c:1646  */
    { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 2041 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 402 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.exprtype) = (yyvsp[-2].exprtype);
        (yyval.exprtype)->setNext((yyvsp[0].exprtype));
    }
#line 2050 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 407 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.type) = TypeSystem::intType;
        declType = TypeSystem::intType;
    }
#line 2059 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 411 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.type) = TypeSystem::voidType;
    }
#line 2067 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 414 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.type) = TypeSystem::floatType;
        declType = TypeSystem::floatType;
    }
#line 2076 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 420 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 2082 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 421 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 2088 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 424 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[-1].stmttype); }
#line 2094 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 427 "src/parser.y" /* yacc.c:1646  */
    {
        // 这里肯定还得区分一下 
        (yyval.stmttype) = (yyvsp[-1].stmttype);
    }
#line 2103 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 433 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = (yyvsp[-2].stmttype);
        (yyvsp[-2].stmttype)->setNext((yyvsp[0].stmttype));
    }
#line 2112 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 437 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 2118 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 440 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = (yyvsp[-2].stmttype);
        (yyvsp[-2].stmttype)->setNext((yyvsp[0].stmttype));
    }
#line 2127 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 444 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 2133 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 447 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry(declType, (yyvsp[0].strtype), identifiers->getLevel());
        if (!identifiers->install((yyvsp[0].strtype), se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[0].strtype));
        (yyval.stmttype) = new DeclStmt(new Id(se));
        delete [](yyvsp[0].strtype);
    }
#line 2146 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 455 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        std::vector<int> vec;
        ExprNode* temp = (yyvsp[0].exprtype);
        while (temp) {
            vec.push_back(temp->getValue());
            temp = (ExprNode*)(temp->getNext());
        }
        Type* type = declType;
        Type* temp1;
        while (!vec.empty()) {
            temp1 = new ArrayType(type, vec.back());
            if (type->isArray())
                ((ArrayType*)type)->setArrayType(temp1);
            type = temp1;
            vec.pop_back();
        }
        arrayType = (ArrayType*)type;
        se = new IdentifierSymbolEntry(type, (yyvsp[-1].strtype), identifiers->getLevel());
        ((IdentifierSymbolEntry*)se)->setAllZero();
        // int *p = new int[type->getSize()];
        // ((IdentifierSymbolEntry*)se)->setArrayValue(p);
        if (!identifiers->install((yyvsp[-1].strtype), se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[-1].strtype));
        (yyval.stmttype) = new DeclStmt(new Id(se));
        delete [](yyvsp[-1].strtype);
    }
#line 2178 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 482 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry(declType, (yyvsp[-2].strtype), identifiers->getLevel());
        if (!identifiers->install((yyvsp[-2].strtype), se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[-2].strtype));
        // 这里要不要存值不确定
        double val = (yyvsp[0].exprtype)->getValue();
        if (declType->isInt() && (yyvsp[0].exprtype)->getType()->isFloat()) {
            float temp = (float)val;
            int temp1 = (int)temp;
            val = (double)temp1;
        }
        ((IdentifierSymbolEntry*)se)->setValue(val);
        (yyval.stmttype) = new DeclStmt(new Id(se), (yyvsp[0].exprtype));
        delete [](yyvsp[-2].strtype);
    }
#line 2199 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 498 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        std::vector<int> vec;
        ExprNode* temp = (yyvsp[-1].exprtype);
        while (temp) {
            vec.push_back(temp->getValue());
            temp = (ExprNode*)(temp->getNext());
        }
        Type* type = declType;
        Type* temp1;
        for(auto it = vec.rbegin(); it != vec.rend(); it++) {
            temp1 = new ArrayType(type, *it);
            if (type->isArray())
                ((ArrayType*)type)->setArrayType(temp1);
            type = temp1;
        }
        arrayType = (ArrayType*)type;
        idx = 0;
        std::stack<InitValueListExpr*>().swap(stk);
        se = new IdentifierSymbolEntry(type, (yyvsp[-2].strtype), identifiers->getLevel());
        (yyval.se) = se;
        arrayValue = new double[arrayType->getSize()];
        notZeroNum = 0;
    }
#line 2228 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 522 "src/parser.y" /* yacc.c:1646  */
    {
        ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setArrayValue(arrayValue);
        ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setNotZeroNum(notZeroNum);
        if ((notZeroNum == 0) || ((InitValueListExpr*)(yyvsp[0].exprtype))->isEmpty()){
            ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setAllZero();
            ((InitValueListExpr*)(yyvsp[0].exprtype))->setAllZero();
        }
        if (!identifiers->install((yyvsp[-4].strtype), (yyvsp[-1].se)))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[-4].strtype));
        (yyval.stmttype) = new DeclStmt(new Id((yyvsp[-1].se)), (yyvsp[0].exprtype));
        delete [](yyvsp[-4].strtype);
    }
#line 2245 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 536 "src/parser.y" /* yacc.c:1646  */
    {

        if (declType->isFloat()) {
            declType = TypeSystem::constFloatType;
        } else if (declType->isInt()) {
            declType = TypeSystem::constIntType;
        } else {
            // error
        }

        SymbolEntry* se;
        se = new IdentifierSymbolEntry(declType, (yyvsp[-2].strtype), identifiers->getLevel());
        ((IdentifierSymbolEntry*)se)->setConst();
        if (!identifiers->install((yyvsp[-2].strtype), se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[-2].strtype));
        identifiers->install((yyvsp[-2].strtype), se);
        double val = (yyvsp[0].exprtype)->getValue();
        if (declType->isInt() && (yyvsp[0].exprtype)->getType()->isFloat()) {
            float temp = (float)val;
            int temp1 = (int)temp;
            val = (double)temp1;
        }
        ((IdentifierSymbolEntry*)se)->setValue(val);
        (yyval.stmttype) = new DeclStmt(new Id(se), (yyvsp[0].exprtype));
        delete [](yyvsp[-2].strtype);
    }
#line 2276 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 562 "src/parser.y" /* yacc.c:1646  */
    {

        if (declType->isFloat()) {
            declType = TypeSystem::constFloatType;
        } else if (declType->isInt()) {
            declType = TypeSystem::constIntType;
        } else {
            // error
        }

        SymbolEntry* se;
        std::vector<int> vec;
        ExprNode* temp = (yyvsp[-1].exprtype);
        while (temp) {
            vec.push_back(temp->getValue());
            temp = (ExprNode*)(temp->getNext());
        }
        Type* type = declType;
        Type* temp1;
        for(auto it = vec.rbegin(); it != vec.rend(); it++) {
            temp1 = new ArrayType(type, *it, true);
            if (type->isArray())
                ((ArrayType*)type)->setArrayType(temp1);
            type = temp1;
        }
        arrayType = (ArrayType*)type;
        idx = 0;
        std::stack<InitValueListExpr*>().swap(stk);
        se = new IdentifierSymbolEntry(type, (yyvsp[-2].strtype), identifiers->getLevel());
        ((IdentifierSymbolEntry*)se)->setConst();
        (yyval.se) = se;
        arrayValue = new double[arrayType->getSize()];
        notZeroNum = 0;
    }
#line 2315 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 596 "src/parser.y" /* yacc.c:1646  */
    {
        ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setArrayValue(arrayValue);
        ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setNotZeroNum(notZeroNum);
        if ((notZeroNum == 0) || ((InitValueListExpr*)(yyvsp[0].exprtype))->isEmpty()){
            ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setAllZero();
            ((InitValueListExpr*)(yyvsp[0].exprtype))->setAllZero();
        }
        if (!identifiers->install((yyvsp[-4].strtype), (yyvsp[-1].se)))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[-4].strtype));
        identifiers->install((yyvsp[-4].strtype), (yyvsp[-1].se));
        (yyval.stmttype) = new DeclStmt(new Id((yyvsp[-1].se)), (yyvsp[0].exprtype));
        delete [](yyvsp[-4].strtype);
    }
#line 2333 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 611 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.exprtype) = (yyvsp[-1].exprtype);
    }
#line 2341 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 614 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.exprtype) = (yyvsp[-3].exprtype);
        (yyvsp[-3].exprtype)->setNext((yyvsp[-1].exprtype));
    }
#line 2350 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 620 "src/parser.y" /* yacc.c:1646  */
    {
        if (!(yyvsp[0].exprtype)->getType()->isInt() && !(yyvsp[0].exprtype)->getType()->isFloat()) {
            // error
            fprintf(stderr, "rval is invalid.\n");
        }
        (yyval.exprtype) = (yyvsp[0].exprtype);
        if (!stk.empty()) {
            
            double val = (yyvsp[0].exprtype)->getValue();
            if (val)
                notZeroNum++;
            if (declType->isInt() && (yyvsp[0].exprtype)->getType()->isFloat()) {
                float temp = (float)val;
                int temp1 = (int)temp;
                arrayValue[idx++] = (double)temp1;
            } else {
                arrayValue[idx++] = val;
            }

            Type* arrTy = stk.top()->getSymbolEntry()->getType();
            if (arrTy == TypeSystem::intType || arrTy == TypeSystem::floatType) {
                if ((arrTy->isInt() && (yyvsp[0].exprtype)->getType()->isFloat()) ||
                    (arrTy->isFloat() && (yyvsp[0].exprtype)->getType()->isInt())) {
                    ImplicitCastExpr* temp = new ImplicitCastExpr((yyvsp[0].exprtype), declType);
                    stk.top()->addExpr(temp);
                } else {
                    stk.top()->addExpr((yyvsp[0].exprtype));
                }
            } else {
                while (arrTy) {
                    if (((ArrayType*)arrTy)->getElementType() != TypeSystem::intType &&
                        ((ArrayType*)arrTy)->getElementType() != TypeSystem::floatType) {
                        arrTy = ((ArrayType*)arrTy)->getElementType();
                        SymbolEntry* se = new ConstantSymbolEntry(arrTy);
                        InitValueListExpr* list = new InitValueListExpr(se);
                        stk.top()->addExpr(list);
                        stk.push(list);
                    } else {
                        Type* elemType = ((ArrayType*)arrTy)->getElementType();
                        if ((elemType->isInt() && (yyvsp[0].exprtype)->getType()->isFloat()) ||
                            (elemType->isFloat() && (yyvsp[0].exprtype)->getType()->isInt())) {
                            ImplicitCastExpr* temp = new ImplicitCastExpr((yyvsp[0].exprtype), elemType);
                            stk.top()->addExpr(temp);
                        } else {
                            stk.top()->addExpr((yyvsp[0].exprtype));
                        }
                        while (stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt) {
                            arrTy = ((ArrayType*)arrTy)->getArrayType();
                            stk.pop();
                        }
                        break;
                    }
                }
            }
        }         
    }
#line 2411 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 676 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        ExprNode* list;
        if (stk.empty()) {
            // 如果只用一个{}初始化数组，那么栈一定为空
            // 此时也没必要再加入栈了
            memset(arrayValue, 0, arrayType->getSize());
            idx += arrayType->getSize() / declType->getSize();
            se = new ConstantSymbolEntry(arrayType);
            list = new InitValueListExpr(se);
        } else {
            // 栈不空说明肯定不是只有{}
            // 此时需要确定{}到底占了几个元素
            Type* type = ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType();
            int len = type->getSize() / declType->getSize();
            memset(arrayValue + idx, 0, type->getSize());
            idx += len;
            se = new ConstantSymbolEntry(type);
            list = new InitValueListExpr(se);
            stk.top()->addExpr(list);
            while (stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt) {
                stk.pop();
            }
        }
        (yyval.exprtype) = list;
    }
#line 2442 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 702 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        if (!stk.empty())
            arrayType = (ArrayType*)(
                ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType());
        se = new ConstantSymbolEntry(arrayType);
        if (arrayType->getElementType() != TypeSystem::intType &&
            arrayType->getElementType() != TypeSystem::floatType) {
            arrayType = (ArrayType*)(arrayType->getElementType());
        }
        InitValueListExpr* expr = new InitValueListExpr(se);
        if (!stk.empty())
            stk.top()->addExpr(expr);
        stk.push(expr);
        (yyval.exprtype) = expr;
        leftCnt++;
    }
#line 2464 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 719 "src/parser.y" /* yacc.c:1646  */
    {
        leftCnt--;
        while (stk.top() != (yyvsp[-2].exprtype) && stk.size() > (long unsigned int)(leftCnt + 1))
            stk.pop();
        if (stk.top() == (yyvsp[-2].exprtype))
            stk.pop();
        (yyval.exprtype) = (yyvsp[-2].exprtype);
        if (!stk.empty())
            while (stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt) {
                stk.pop();
            }
        int size = ((ArrayType*)((yyval.exprtype)->getSymbolEntry()->getType()))->getSize() / declType->getSize();
        while (idx % size != 0)
            arrayValue[idx++] = 0;
        if (!stk.empty())
            arrayType = (ArrayType*)(
                ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType());
    }
#line 2487 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 740 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.exprtype) = (yyvsp[0].exprtype);
        if (!stk.empty()) {

            double val = (yyvsp[0].exprtype)->getValue();
            if (val)
                notZeroNum++;
            if (declType->isInt() && (yyvsp[0].exprtype)->getType()->isFloat()) {
                float temp = (float)val;
                int temp1 = (int)temp;
                arrayValue[idx++] = (double)temp1;
            } else {
                arrayValue[idx++] = val;
            }

            Type* arrTy = stk.top()->getSymbolEntry()->getType();
            if (arrTy == TypeSystem::constIntType || arrTy == TypeSystem::constFloatType) {
                if ((arrTy->isFloat() && (yyvsp[0].exprtype)->getType()->isInt()) || 
                    (arrTy->isInt() && (yyvsp[0].exprtype)->getType()->isFloat())) {

                    ImplicitCastExpr* temp = new ImplicitCastExpr((yyvsp[0].exprtype), arrTy); 
                    // arrTy is const here
                    stk.top()->addExpr(temp);

                } else {
                    stk.top()->addExpr((yyvsp[0].exprtype));
                }
            } else {
                while (arrTy) {
                    if (((ArrayType*)arrTy)->getElementType() != TypeSystem::constIntType &&
                        ((ArrayType*)arrTy)->getElementType() != TypeSystem::constFloatType) {
                        arrTy = ((ArrayType*)arrTy)->getElementType();
                        SymbolEntry* se = new ConstantSymbolEntry(arrTy);
                        InitValueListExpr* list = new InitValueListExpr(se);
                        stk.top()->addExpr(list);
                        stk.push(list);
                    } else {
                        Type* elemType = ((ArrayType*)arrTy)->getElementType();
                        if ((elemType->isFloat() && (yyvsp[0].exprtype)->getType()->isInt()) || 
                            (elemType->isInt() && (yyvsp[0].exprtype)->getType()->isFloat())) {

                            ImplicitCastExpr* temp = new ImplicitCastExpr((yyvsp[0].exprtype), elemType); 
                            // `elemType` is const here
                            stk.top()->addExpr(temp);

                        } else {
                            stk.top()->addExpr((yyvsp[0].exprtype));
                        }

                        while (stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt) {
                            arrTy = ((ArrayType*)arrTy)->getArrayType();
                            stk.pop();
                        }
                        break;
                    }
                }
            }
        }
    }
#line 2551 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 799 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        ExprNode* list;
        if (stk.empty()) {
            // 如果只用一个{}初始化数组，那么栈一定为空
            // 此时也没必要再加入栈了
            memset(arrayValue, 0, arrayType->getSize());
            idx += arrayType->getSize() / declType->getSize();
            se = new ConstantSymbolEntry(arrayType);
            list = new InitValueListExpr(se);
        } else {
            // 栈不空说明肯定不是只有{}
            // 此时需要确定{}到底占了几个元素
            Type* type = ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType();
            int len = type->getSize() / declType->getSize();
            memset(arrayValue + idx, 0, type->getSize());
            idx += len;
            se = new ConstantSymbolEntry(type);
            list = new InitValueListExpr(se);
            stk.top()->addExpr(list);
            while (stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt) {
                stk.pop();
            }
        }
        (yyval.exprtype) = list;
    }
#line 2582 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 825 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        if (!stk.empty())
            arrayType = (ArrayType*)(
                ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType());
        se = new ConstantSymbolEntry(arrayType);
        if (arrayType->getElementType() != TypeSystem::intType &&
            arrayType->getElementType() != TypeSystem::floatType) {
            arrayType = (ArrayType*)(arrayType->getElementType());
        }
        InitValueListExpr* expr = new InitValueListExpr(se);
        if (!stk.empty())
            stk.top()->addExpr(expr);
        stk.push(expr);
        (yyval.exprtype) = expr;
        leftCnt++;
    }
#line 2604 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 842 "src/parser.y" /* yacc.c:1646  */
    {
        leftCnt--;
        while (stk.top() != (yyvsp[-2].exprtype) && stk.size() > (long unsigned int)(leftCnt + 1))
            stk.pop();
        if (stk.top() == (yyvsp[-2].exprtype))
            stk.pop();
        (yyval.exprtype) = (yyvsp[-2].exprtype);
        if (!stk.empty())
            while (stk.top()->isFull() && stk.size() != (long unsigned int)leftCnt) {
                stk.pop();
            }
        while (
            idx % (((ArrayType*)((yyval.exprtype)->getSymbolEntry()->getType()))->getSize() / sizeof(int)) != 0)
            arrayValue[idx++] = 0;
        if (!stk.empty())
            arrayType = (ArrayType*)(
                ((ArrayType*)(stk.top()->getSymbolEntry()->getType()))->getElementType());
    }
#line 2627 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 862 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 2635 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 865 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.exprtype) = (yyvsp[-2].exprtype);
    }
#line 2643 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 870 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 2651 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 873 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.exprtype) = (yyvsp[-2].exprtype);
    }
#line 2659 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 879 "src/parser.y" /* yacc.c:1646  */
    {
        // SymbolTable::resetLabel();
        identifiers = new SymbolTable(identifiers);
        paramNo = 0;
        fpParamNo = 0;
        funcRetType = (yyvsp[-1].type);
    }
#line 2671 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 101:
#line 886 "src/parser.y" /* yacc.c:1646  */
    {
        Type* funcType;
        std::vector<Type*> vec;
        std::vector<SymbolEntry*> vec1;
        DeclStmt* temp = (DeclStmt*)(yyvsp[-1].stmttype);
        while (temp) {
            vec.push_back(temp->getId()->getSymbolEntry()->getType());
            vec1.push_back(temp->getId()->getSymbolEntry());
            temp = (DeclStmt*)(temp->getNext());
        }
        funcType = new FunctionType((yyvsp[-5].type), vec, vec1);
        SymbolEntry* se = new IdentifierSymbolEntry(
            funcType, (yyvsp[-4].strtype), identifiers->getPrev()->getLevel());
        if (!identifiers->getPrev()->install((yyvsp[-4].strtype), se)) {
            fprintf(stderr, "redefinition of \'%s %s\'\n", (yyvsp[-4].strtype), se->getType()->toStr().c_str());
        }
        (yyval.se) = se; 
    }
#line 2694 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 102:
#line 904 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = new FunctionDef((yyvsp[-1].se), (DeclStmt*)(yyvsp[-3].stmttype), (yyvsp[0].stmttype));
        SymbolTable* top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete [](yyvsp[-6].strtype);
    }
#line 2706 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 103:
#line 913 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 2712 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 104:
#line 914 "src/parser.y" /* yacc.c:1646  */
    { (yyval.stmttype) = nullptr; }
#line 2718 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 105:
#line 916 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = (yyvsp[-2].stmttype);
        (yyval.stmttype)->setNext((yyvsp[0].stmttype));
    }
#line 2727 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 106:
#line 920 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.stmttype) = (yyvsp[0].stmttype);
    }
#line 2735 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 107:
#line 925 "src/parser.y" /* yacc.c:1646  */
    {
        SymbolEntry* se;
        if ((yyvsp[-1].type)->isFloat()) {
            se = new IdentifierSymbolEntry((yyvsp[-1].type), (yyvsp[0].strtype), identifiers->getLevel(), fpParamNo++);
            ((IdentifierSymbolEntry*)se)->setAllParamNo(fpParamNo + paramNo - 1);
        } else {
            se = new IdentifierSymbolEntry((yyvsp[-1].type), (yyvsp[0].strtype), identifiers->getLevel(), paramNo++);
            ((IdentifierSymbolEntry*)se)->setAllParamNo(fpParamNo + paramNo - 1);
        }
        identifiers->install((yyvsp[0].strtype), se);
        ((IdentifierSymbolEntry*)se)->setLabel();
        ((IdentifierSymbolEntry*)se)->setAddr(new Operand(se));
        (yyval.stmttype) = new DeclStmt(new Id(se));
        delete [](yyvsp[0].strtype);
    }
#line 2755 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 108:
#line 940 "src/parser.y" /* yacc.c:1646  */
    {
        // 这里也需要求值
        SymbolEntry* se;
        ExprNode* temp = (yyvsp[0].exprtype);
        Type* arr = (yyvsp[-2].type);
        Type* arr1;
        std::stack<ExprNode*> stk;
        while (temp) {
            stk.push(temp);
            temp = (ExprNode*)(temp->getNext());
        }
        while (!stk.empty()) {
            arr1 = new ArrayType(arr, stk.top()->getValue());
            if (arr->isArray())
                ((ArrayType*)arr)->setArrayType(arr1);
            arr = arr1;
            stk.pop();
        }
        se = new IdentifierSymbolEntry(arr, (yyvsp[-1].strtype), identifiers->getLevel(), paramNo++);
        ((IdentifierSymbolEntry*)se)->setAllParamNo(fpParamNo + paramNo - 1);
        identifiers->install((yyvsp[-1].strtype), se);
        ((IdentifierSymbolEntry*)se)->setLabel();
        ((IdentifierSymbolEntry*)se)->setAddr(new Operand(se));
        (yyval.stmttype) = new DeclStmt(new Id(se));
        delete [](yyvsp[-1].strtype);
    }
#line 2786 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 109:
#line 968 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.exprtype) = new ExprNode(nullptr);
    }
#line 2794 "src/parser.cpp" /* yacc.c:1646  */
    break;

  case 110:
#line 971 "src/parser.y" /* yacc.c:1646  */
    {
        (yyval.exprtype) = (yyvsp[-3].exprtype);
        (yyval.exprtype)->setNext((yyvsp[-1].exprtype));
    }
#line 2803 "src/parser.cpp" /* yacc.c:1646  */
    break;


#line 2807 "src/parser.cpp" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 975 "src/parser.y" /* yacc.c:1906  */


int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
