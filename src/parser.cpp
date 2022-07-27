/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* "%code top" blocks.  */
#line 1 "src/parser.y"

    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    #include <cstring>
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

#line 95 "src/parser.cpp"




# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ID = 3,                         /* ID  */
  YYSYMBOL_STRING = 4,                     /* STRING  */
  YYSYMBOL_INTEGER = 5,                    /* INTEGER  */
  YYSYMBOL_FLOATING = 6,                   /* FLOATING  */
  YYSYMBOL_IF = 7,                         /* IF  */
  YYSYMBOL_ELSE = 8,                       /* ELSE  */
  YYSYMBOL_WHILE = 9,                      /* WHILE  */
  YYSYMBOL_INT = 10,                       /* INT  */
  YYSYMBOL_VOID = 11,                      /* VOID  */
  YYSYMBOL_FLOAT = 12,                     /* FLOAT  */
  YYSYMBOL_LPAREN = 13,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 14,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 15,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 16,                    /* RBRACE  */
  YYSYMBOL_SEMICOLON = 17,                 /* SEMICOLON  */
  YYSYMBOL_LBRACKET = 18,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 19,                  /* RBRACKET  */
  YYSYMBOL_COMMA = 20,                     /* COMMA  */
  YYSYMBOL_ADD = 21,                       /* ADD  */
  YYSYMBOL_SUB = 22,                       /* SUB  */
  YYSYMBOL_MUL = 23,                       /* MUL  */
  YYSYMBOL_DIV = 24,                       /* DIV  */
  YYSYMBOL_MOD = 25,                       /* MOD  */
  YYSYMBOL_OR = 26,                        /* OR  */
  YYSYMBOL_AND = 27,                       /* AND  */
  YYSYMBOL_LESS = 28,                      /* LESS  */
  YYSYMBOL_LESSEQUAL = 29,                 /* LESSEQUAL  */
  YYSYMBOL_GREATER = 30,                   /* GREATER  */
  YYSYMBOL_GREATEREQUAL = 31,              /* GREATEREQUAL  */
  YYSYMBOL_ASSIGN = 32,                    /* ASSIGN  */
  YYSYMBOL_EQUAL = 33,                     /* EQUAL  */
  YYSYMBOL_NOTEQUAL = 34,                  /* NOTEQUAL  */
  YYSYMBOL_NOT = 35,                       /* NOT  */
  YYSYMBOL_CONST = 36,                     /* CONST  */
  YYSYMBOL_RETURN = 37,                    /* RETURN  */
  YYSYMBOL_CONTINUE = 38,                  /* CONTINUE  */
  YYSYMBOL_BREAK = 39,                     /* BREAK  */
  YYSYMBOL_THEN = 40,                      /* THEN  */
  YYSYMBOL_YYACCEPT = 41,                  /* $accept  */
  YYSYMBOL_Program = 42,                   /* Program  */
  YYSYMBOL_Stmts = 43,                     /* Stmts  */
  YYSYMBOL_Stmt = 44,                      /* Stmt  */
  YYSYMBOL_LVal = 45,                      /* LVal  */
  YYSYMBOL_AssignStmt = 46,                /* AssignStmt  */
  YYSYMBOL_ExprStmt = 47,                  /* ExprStmt  */
  YYSYMBOL_BlankStmt = 48,                 /* BlankStmt  */
  YYSYMBOL_BlockStmt = 49,                 /* BlockStmt  */
  YYSYMBOL_50_1 = 50,                      /* $@1  */
  YYSYMBOL_IfStmt = 51,                    /* IfStmt  */
  YYSYMBOL_WhileStmt = 52,                 /* WhileStmt  */
  YYSYMBOL_53_2 = 53,                      /* @2  */
  YYSYMBOL_BreakStmt = 54,                 /* BreakStmt  */
  YYSYMBOL_ContinueStmt = 55,              /* ContinueStmt  */
  YYSYMBOL_ReturnStmt = 56,                /* ReturnStmt  */
  YYSYMBOL_Exp = 57,                       /* Exp  */
  YYSYMBOL_Cond = 58,                      /* Cond  */
  YYSYMBOL_PrimaryExp = 59,                /* PrimaryExp  */
  YYSYMBOL_UnaryExp = 60,                  /* UnaryExp  */
  YYSYMBOL_MulExp = 61,                    /* MulExp  */
  YYSYMBOL_AddExp = 62,                    /* AddExp  */
  YYSYMBOL_RelExp = 63,                    /* RelExp  */
  YYSYMBOL_EqExp = 64,                     /* EqExp  */
  YYSYMBOL_LAndExp = 65,                   /* LAndExp  */
  YYSYMBOL_LOrExp = 66,                    /* LOrExp  */
  YYSYMBOL_ConstExp = 67,                  /* ConstExp  */
  YYSYMBOL_FuncRParams = 68,               /* FuncRParams  */
  YYSYMBOL_Type = 69,                      /* Type  */
  YYSYMBOL_DeclStmt = 70,                  /* DeclStmt  */
  YYSYMBOL_VarDeclStmt = 71,               /* VarDeclStmt  */
  YYSYMBOL_ConstDeclStmt = 72,             /* ConstDeclStmt  */
  YYSYMBOL_VarDefList = 73,                /* VarDefList  */
  YYSYMBOL_ConstDefList = 74,              /* ConstDefList  */
  YYSYMBOL_VarDef = 75,                    /* VarDef  */
  YYSYMBOL_76_3 = 76,                      /* @3  */
  YYSYMBOL_ConstDef = 77,                  /* ConstDef  */
  YYSYMBOL_78_4 = 78,                      /* @4  */
  YYSYMBOL_ArrayIndices = 79,              /* ArrayIndices  */
  YYSYMBOL_InitVal = 80,                   /* InitVal  */
  YYSYMBOL_81_5 = 81,                      /* @5  */
  YYSYMBOL_ConstInitVal = 82,              /* ConstInitVal  */
  YYSYMBOL_83_6 = 83,                      /* @6  */
  YYSYMBOL_InitValList = 84,               /* InitValList  */
  YYSYMBOL_ConstInitValList = 85,          /* ConstInitValList  */
  YYSYMBOL_FuncDef = 86,                   /* FuncDef  */
  YYSYMBOL_87_7 = 87,                      /* $@7  */
  YYSYMBOL_88_8 = 88,                      /* @8  */
  YYSYMBOL_MaybeFuncFParams = 89,          /* MaybeFuncFParams  */
  YYSYMBOL_FuncFParams = 90,               /* FuncFParams  */
  YYSYMBOL_FuncFParam = 91,                /* FuncFParam  */
  YYSYMBOL_FuncArrayIndices = 92           /* FuncArrayIndices  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
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

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   295


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
static const yytype_int16 yyrline[] =
{
       0,    67,    67,    72,    73,    78,    79,    80,    81,    82,
      83,    84,    89,    94,    95,    96,    99,   107,   117,   122,
     127,   132,   132,   143,   149,   152,   157,   157,   172,   177,
     182,   185,   196,   200,   203,   206,   209,   222,   226,   232,
     233,   240,   251,   252,   258,   278,   279,   289,   299,   313,
     314,   324,   336,   339,   344,   349,   354,   361,   362,   367,
     374,   375,   382,   383,   390,   393,   394,   399,   403,   406,
     412,   413,   416,   419,   425,   429,   432,   436,   439,   447,
     474,   490,   490,   527,   553,   553,   600,   603,   609,   665,
     691,   691,   729,   788,   814,   814,   851,   854,   859,   862,
     868,   875,   868,   902,   903,   905,   909,   914,   927,   954,
     957
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ID", "STRING",
  "INTEGER", "FLOATING", "IF", "ELSE", "WHILE", "INT", "VOID", "FLOAT",
  "LPAREN", "RPAREN", "LBRACE", "RBRACE", "SEMICOLON", "LBRACKET",
  "RBRACKET", "COMMA", "ADD", "SUB", "MUL", "DIV", "MOD", "OR", "AND",
  "LESS", "LESSEQUAL", "GREATER", "GREATEREQUAL", "ASSIGN", "EQUAL",
  "NOTEQUAL", "NOT", "CONST", "RETURN", "CONTINUE", "BREAK", "THEN",
  "$accept", "Program", "Stmts", "Stmt", "LVal", "AssignStmt", "ExprStmt",
  "BlankStmt", "BlockStmt", "$@1", "IfStmt", "WhileStmt", "@2",
  "BreakStmt", "ContinueStmt", "ReturnStmt", "Exp", "Cond", "PrimaryExp",
  "UnaryExp", "MulExp", "AddExp", "RelExp", "EqExp", "LAndExp", "LOrExp",
  "ConstExp", "FuncRParams", "Type", "DeclStmt", "VarDeclStmt",
  "ConstDeclStmt", "VarDefList", "ConstDefList", "VarDef", "@3",
  "ConstDef", "@4", "ArrayIndices", "InitVal", "@5", "ConstInitVal", "@6",
  "InitValList", "ConstInitValList", "FuncDef", "$@7", "@8",
  "MaybeFuncFParams", "FuncFParams", "FuncFParam", "FuncArrayIndices", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-130)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-101)

#define yytable_value_is_error(Yyn) \
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
static const yytype_int8 yydefact[] =
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
static const yytype_uint8 yydefgoto[] =
{
       0,    20,    21,    22,    48,    24,    25,    26,    27,    51,
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

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
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

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
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

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
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


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
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
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
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
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
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
  case 2: /* Program: Stmts  */
#line 67 "src/parser.y"
            {
        ast.setRoot((yyvsp[0].stmttype));
    }
#line 1327 "src/parser.cpp"
    break;

  case 3: /* Stmts: Stmt  */
#line 72 "src/parser.y"
           {(yyval.stmttype)=(yyvsp[0].stmttype);}
#line 1333 "src/parser.cpp"
    break;

  case 4: /* Stmts: Stmts Stmt  */
#line 73 "src/parser.y"
                {
        (yyval.stmttype) = new SeqNode((yyvsp[-1].stmttype), (yyvsp[0].stmttype));
    }
#line 1341 "src/parser.cpp"
    break;

  case 5: /* Stmt: AssignStmt  */
#line 78 "src/parser.y"
                 { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1347 "src/parser.cpp"
    break;

  case 6: /* Stmt: ExprStmt  */
#line 79 "src/parser.y"
               { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1353 "src/parser.cpp"
    break;

  case 7: /* Stmt: BlockStmt  */
#line 80 "src/parser.y"
                {(yyval.stmttype) = (yyvsp[0].stmttype);}
#line 1359 "src/parser.cpp"
    break;

  case 8: /* Stmt: BlankStmt  */
#line 81 "src/parser.y"
                { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1365 "src/parser.cpp"
    break;

  case 9: /* Stmt: IfStmt  */
#line 82 "src/parser.y"
             { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1371 "src/parser.cpp"
    break;

  case 10: /* Stmt: WhileStmt  */
#line 83 "src/parser.y"
                { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1377 "src/parser.cpp"
    break;

  case 11: /* Stmt: BreakStmt  */
#line 84 "src/parser.y"
                {
        if (!whileCnt)
            fprintf(stderr, "\'break\' statement not in while statement\n");
        (yyval.stmttype) = (yyvsp[0].stmttype);
    }
#line 1387 "src/parser.cpp"
    break;

  case 12: /* Stmt: ContinueStmt  */
#line 89 "src/parser.y"
                   {
        if (!whileCnt)
            fprintf(stderr, "\'continue\' statement not in while statement\n");
        (yyval.stmttype) = (yyvsp[0].stmttype);
    }
#line 1397 "src/parser.cpp"
    break;

  case 13: /* Stmt: ReturnStmt  */
#line 94 "src/parser.y"
                 { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1403 "src/parser.cpp"
    break;

  case 14: /* Stmt: DeclStmt  */
#line 95 "src/parser.y"
               { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1409 "src/parser.cpp"
    break;

  case 15: /* Stmt: FuncDef  */
#line 96 "src/parser.y"
              { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1415 "src/parser.cpp"
    break;

  case 16: /* LVal: ID  */
#line 99 "src/parser.y"
         {
        SymbolEntry* se;
        se = identifiers->lookup((yyvsp[0].strtype));
        if (se == nullptr)
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)(yyvsp[0].strtype));
        (yyval.exprtype) = new Id(se);
        delete [](yyvsp[0].strtype);
    }
#line 1428 "src/parser.cpp"
    break;

  case 17: /* LVal: ID ArrayIndices  */
#line 107 "src/parser.y"
                     {
        SymbolEntry* se;
        se = identifiers->lookup((yyvsp[-1].strtype));
        if (se == nullptr)
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)(yyvsp[-1].strtype));
        (yyval.exprtype) = new Id(se, (yyvsp[0].exprtype));
        delete [](yyvsp[-1].strtype);
    }
#line 1441 "src/parser.cpp"
    break;

  case 18: /* AssignStmt: LVal ASSIGN Exp SEMICOLON  */
#line 117 "src/parser.y"
                                {
        (yyval.stmttype) = new AssignStmt((yyvsp[-3].exprtype), (yyvsp[-1].exprtype));
    }
#line 1449 "src/parser.cpp"
    break;

  case 19: /* ExprStmt: Exp SEMICOLON  */
#line 122 "src/parser.y"
                    {
        (yyval.stmttype) = new ExprStmt((yyvsp[-1].exprtype));
    }
#line 1457 "src/parser.cpp"
    break;

  case 20: /* BlankStmt: SEMICOLON  */
#line 127 "src/parser.y"
                {
        (yyval.stmttype) = new BlankStmt();
    }
#line 1465 "src/parser.cpp"
    break;

  case 21: /* $@1: %empty  */
#line 132 "src/parser.y"
             {
        identifiers = new SymbolTable(identifiers);
    }
#line 1473 "src/parser.cpp"
    break;

  case 22: /* BlockStmt: LBRACE $@1 Stmts RBRACE  */
#line 135 "src/parser.y"
                   {
        // midrule actions https://www.gnu.org/software/bison/manual/html_node/Using-Midrule-Actions.html
        (yyval.stmttype) = new CompoundStmt((yyvsp[-1].stmttype));

        SymbolTable* top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
#line 1486 "src/parser.cpp"
    break;

  case 23: /* BlockStmt: LBRACE RBRACE  */
#line 143 "src/parser.y"
                    {
        // 这里这个用加嘛 不确定
        (yyval.stmttype) = new CompoundStmt();
    }
#line 1495 "src/parser.cpp"
    break;

  case 24: /* IfStmt: IF LPAREN Cond RPAREN Stmt  */
#line 149 "src/parser.y"
                                            {
        (yyval.stmttype) = new IfStmt((yyvsp[-2].exprtype), (yyvsp[0].stmttype));
    }
#line 1503 "src/parser.cpp"
    break;

  case 25: /* IfStmt: IF LPAREN Cond RPAREN Stmt ELSE Stmt  */
#line 152 "src/parser.y"
                                           {
        (yyval.stmttype) = new IfElseStmt((yyvsp[-4].exprtype), (yyvsp[-2].stmttype), (yyvsp[0].stmttype));
    }
#line 1511 "src/parser.cpp"
    break;

  case 26: /* @2: %empty  */
#line 157 "src/parser.y"
                               {
        whileCnt++;
        WhileStmt *whileNode = new WhileStmt((yyvsp[-1].exprtype));
        (yyval.stmttype) = whileNode;
        whileStk.push(whileNode);
    }
#line 1522 "src/parser.cpp"
    break;

  case 27: /* WhileStmt: WHILE LPAREN Cond RPAREN @2 Stmt  */
#line 163 "src/parser.y"
         {
        StmtNode *whileNode = (yyvsp[-1].stmttype); 
        ((WhileStmt*)whileNode)->setStmt((yyvsp[0].stmttype));
        (yyval.stmttype) = whileNode;
        whileStk.pop();
        whileCnt--;
    }
#line 1534 "src/parser.cpp"
    break;

  case 28: /* BreakStmt: BREAK SEMICOLON  */
#line 172 "src/parser.y"
                      {
        (yyval.stmttype) = new BreakStmt(whileStk.top());
    }
#line 1542 "src/parser.cpp"
    break;

  case 29: /* ContinueStmt: CONTINUE SEMICOLON  */
#line 177 "src/parser.y"
                         {
        (yyval.stmttype) = new ContinueStmt(whileStk.top());
    }
#line 1550 "src/parser.cpp"
    break;

  case 30: /* ReturnStmt: RETURN SEMICOLON  */
#line 182 "src/parser.y"
                       {
        (yyval.stmttype) = new ReturnStmt();
    }
#line 1558 "src/parser.cpp"
    break;

  case 31: /* ReturnStmt: RETURN Exp SEMICOLON  */
#line 185 "src/parser.y"
                           {
        if (((yyvsp[-1].exprtype)->getType()->isFloat() && funcRetType->isInt()) ||
            ((yyvsp[-1].exprtype)->getType()->isInt() && funcRetType->isFloat())) {
            (yyval.stmttype) = new ReturnStmt(new ImplicitCastExpr((yyvsp[-1].exprtype), funcRetType));
        } else {
            (yyval.stmttype) = new ReturnStmt((yyvsp[-1].exprtype));            
        }
    }
#line 1571 "src/parser.cpp"
    break;

  case 32: /* Exp: AddExp  */
#line 196 "src/parser.y"
           { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1577 "src/parser.cpp"
    break;

  case 33: /* Cond: LOrExp  */
#line 200 "src/parser.y"
           { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1583 "src/parser.cpp"
    break;

  case 34: /* PrimaryExp: LPAREN Exp RPAREN  */
#line 203 "src/parser.y"
                        {
        (yyval.exprtype) = (yyvsp[-1].exprtype);
    }
#line 1591 "src/parser.cpp"
    break;

  case 35: /* PrimaryExp: LVal  */
#line 206 "src/parser.y"
           {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 1599 "src/parser.cpp"
    break;

  case 36: /* PrimaryExp: STRING  */
#line 209 "src/parser.y"
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
#line 1617 "src/parser.cpp"
    break;

  case 37: /* PrimaryExp: INTEGER  */
#line 222 "src/parser.y"
              {
        SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::intType, (yyvsp[0].numtype));
        (yyval.exprtype) = new Constant(se);
    }
#line 1626 "src/parser.cpp"
    break;

  case 38: /* PrimaryExp: FLOATING  */
#line 226 "src/parser.y"
               {
        SymbolEntry* se = new ConstantSymbolEntry(TypeSystem::floatType, (yyvsp[0].numtype));
        (yyval.exprtype) = new Constant(se);
    }
#line 1635 "src/parser.cpp"
    break;

  case 39: /* UnaryExp: PrimaryExp  */
#line 232 "src/parser.y"
                 { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1641 "src/parser.cpp"
    break;

  case 40: /* UnaryExp: ID LPAREN FuncRParams RPAREN  */
#line 233 "src/parser.y"
                                   {
        SymbolEntry* se;
        se = identifiers->lookup((yyvsp[-3].strtype));
        if (se == nullptr)
            fprintf(stderr, "function \"%s\" is undefined\n", (char*)(yyvsp[-3].strtype));
        (yyval.exprtype) = new CallExpr(se, (yyvsp[-1].exprtype));
    }
#line 1653 "src/parser.cpp"
    break;

  case 41: /* UnaryExp: ID LPAREN RPAREN  */
#line 240 "src/parser.y"
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
#line 1669 "src/parser.cpp"
    break;

  case 42: /* UnaryExp: ADD UnaryExp  */
#line 251 "src/parser.y"
                   { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1675 "src/parser.cpp"
    break;

  case 43: /* UnaryExp: SUB UnaryExp  */
#line 252 "src/parser.y"
                   {
        Type* exprType = (yyvsp[0].exprtype)->getType();
        SymbolEntry* se = new TemporarySymbolEntry(exprType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new UnaryExpr(se, UnaryExpr::SUB, (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1686 "src/parser.cpp"
    break;

  case 44: /* UnaryExp: NOT UnaryExp  */
#line 258 "src/parser.y"
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

            tmpExpr = new UnaryExpr(se, UnaryExpr::NOT, cmpZero);
        } else {
            tmpExpr = new UnaryExpr(se, UnaryExpr::NOT, (yyvsp[0].exprtype));
        }
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1709 "src/parser.cpp"
    break;

  case 45: /* MulExp: UnaryExp  */
#line 278 "src/parser.y"
               { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1715 "src/parser.cpp"
    break;

  case 46: /* MulExp: MulExp MUL UnaryExp  */
#line 279 "src/parser.y"
                          {
        SymbolEntry* se;
        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
        } else {
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        }
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::MUL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1730 "src/parser.cpp"
    break;

  case 47: /* MulExp: MulExp DIV UnaryExp  */
#line 289 "src/parser.y"
                          {
        SymbolEntry* se;
        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
        } else {
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        }
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::DIV, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1745 "src/parser.cpp"
    break;

  case 48: /* MulExp: MulExp MOD UnaryExp  */
#line 299 "src/parser.y"
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
#line 1762 "src/parser.cpp"
    break;

  case 49: /* AddExp: MulExp  */
#line 313 "src/parser.y"
             { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1768 "src/parser.cpp"
    break;

  case 50: /* AddExp: AddExp ADD MulExp  */
#line 314 "src/parser.y"
                        {
        SymbolEntry* se;
        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
        } else {
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        }
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::ADD, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1783 "src/parser.cpp"
    break;

  case 51: /* AddExp: AddExp SUB MulExp  */
#line 324 "src/parser.y"
                        {
        SymbolEntry* se;
        if ((yyvsp[-2].exprtype)->getType()->isFloat() || (yyvsp[0].exprtype)->getType()->isFloat()) {
            se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
        } else {
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        }
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::SUB, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1798 "src/parser.cpp"
    break;

  case 52: /* RelExp: AddExp  */
#line 336 "src/parser.y"
             {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 1806 "src/parser.cpp"
    break;

  case 53: /* RelExp: RelExp LESS AddExp  */
#line 339 "src/parser.y"
                         {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::LESS, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1816 "src/parser.cpp"
    break;

  case 54: /* RelExp: RelExp LESSEQUAL AddExp  */
#line 344 "src/parser.y"
                              {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::LESSEQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1826 "src/parser.cpp"
    break;

  case 55: /* RelExp: RelExp GREATER AddExp  */
#line 349 "src/parser.y"
                            {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::GREATER, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1836 "src/parser.cpp"
    break;

  case 56: /* RelExp: RelExp GREATEREQUAL AddExp  */
#line 354 "src/parser.y"
                                 {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::GREATEREQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1846 "src/parser.cpp"
    break;

  case 57: /* EqExp: RelExp  */
#line 361 "src/parser.y"
             { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1852 "src/parser.cpp"
    break;

  case 58: /* EqExp: EqExp EQUAL RelExp  */
#line 362 "src/parser.y"
                         {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::EQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1862 "src/parser.cpp"
    break;

  case 59: /* EqExp: EqExp NOTEQUAL RelExp  */
#line 367 "src/parser.y"
                            {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::NOTEQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1872 "src/parser.cpp"
    break;

  case 60: /* LAndExp: EqExp  */
#line 374 "src/parser.y"
            { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1878 "src/parser.cpp"
    break;

  case 61: /* LAndExp: LAndExp AND EqExp  */
#line 375 "src/parser.y"
                        {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::AND, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1888 "src/parser.cpp"
    break;

  case 62: /* LOrExp: LAndExp  */
#line 382 "src/parser.y"
              { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1894 "src/parser.cpp"
    break;

  case 63: /* LOrExp: LOrExp OR LAndExp  */
#line 383 "src/parser.y"
                        {
        SymbolEntry* se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        ExprNode* tmpExpr = new BinaryExpr(se, BinaryExpr::OR, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
        (yyval.exprtype) = tmpExpr->const_fold();
    }
#line 1904 "src/parser.cpp"
    break;

  case 64: /* ConstExp: AddExp  */
#line 390 "src/parser.y"
             { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1910 "src/parser.cpp"
    break;

  case 65: /* FuncRParams: Exp  */
#line 393 "src/parser.y"
          { (yyval.exprtype) = (yyvsp[0].exprtype); }
#line 1916 "src/parser.cpp"
    break;

  case 66: /* FuncRParams: FuncRParams COMMA Exp  */
#line 394 "src/parser.y"
                            {
        (yyval.exprtype) = (yyvsp[-2].exprtype);
        (yyval.exprtype)->setNext((yyvsp[0].exprtype));
    }
#line 1925 "src/parser.cpp"
    break;

  case 67: /* Type: INT  */
#line 399 "src/parser.y"
          {
        (yyval.type) = TypeSystem::intType;
        declType = TypeSystem::intType;
    }
#line 1934 "src/parser.cpp"
    break;

  case 68: /* Type: VOID  */
#line 403 "src/parser.y"
           {
        (yyval.type) = TypeSystem::voidType;
    }
#line 1942 "src/parser.cpp"
    break;

  case 69: /* Type: FLOAT  */
#line 406 "src/parser.y"
            {
        (yyval.type) = TypeSystem::floatType;
        declType = TypeSystem::floatType;
    }
#line 1951 "src/parser.cpp"
    break;

  case 70: /* DeclStmt: VarDeclStmt  */
#line 412 "src/parser.y"
                  { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1957 "src/parser.cpp"
    break;

  case 71: /* DeclStmt: ConstDeclStmt  */
#line 413 "src/parser.y"
                    { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1963 "src/parser.cpp"
    break;

  case 72: /* VarDeclStmt: Type VarDefList SEMICOLON  */
#line 416 "src/parser.y"
                                { (yyval.stmttype) = (yyvsp[-1].stmttype); }
#line 1969 "src/parser.cpp"
    break;

  case 73: /* ConstDeclStmt: CONST Type ConstDefList SEMICOLON  */
#line 419 "src/parser.y"
                                        {
        // 这里肯定还得区分一下 
        (yyval.stmttype) = (yyvsp[-1].stmttype);
    }
#line 1978 "src/parser.cpp"
    break;

  case 74: /* VarDefList: VarDefList COMMA VarDef  */
#line 425 "src/parser.y"
                              {
        (yyval.stmttype) = (yyvsp[-2].stmttype);
        (yyvsp[-2].stmttype)->setNext((yyvsp[0].stmttype));
    }
#line 1987 "src/parser.cpp"
    break;

  case 75: /* VarDefList: VarDef  */
#line 429 "src/parser.y"
             { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1993 "src/parser.cpp"
    break;

  case 76: /* ConstDefList: ConstDefList COMMA ConstDef  */
#line 432 "src/parser.y"
                                  {
        (yyval.stmttype) = (yyvsp[-2].stmttype);
        (yyvsp[-2].stmttype)->setNext((yyvsp[0].stmttype));
    }
#line 2002 "src/parser.cpp"
    break;

  case 77: /* ConstDefList: ConstDef  */
#line 436 "src/parser.y"
               { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 2008 "src/parser.cpp"
    break;

  case 78: /* VarDef: ID  */
#line 439 "src/parser.y"
         {
        SymbolEntry* se;
        se = new IdentifierSymbolEntry(declType, (yyvsp[0].strtype), identifiers->getLevel());
        if (!identifiers->install((yyvsp[0].strtype), se))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[0].strtype));
        (yyval.stmttype) = new DeclStmt(new Id(se));
        delete [](yyvsp[0].strtype);
    }
#line 2021 "src/parser.cpp"
    break;

  case 79: /* VarDef: ID ArrayIndices  */
#line 447 "src/parser.y"
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
#line 2053 "src/parser.cpp"
    break;

  case 80: /* VarDef: ID ASSIGN InitVal  */
#line 474 "src/parser.y"
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
#line 2074 "src/parser.cpp"
    break;

  case 81: /* @3: %empty  */
#line 490 "src/parser.y"
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
#line 2103 "src/parser.cpp"
    break;

  case 82: /* VarDef: ID ArrayIndices ASSIGN @3 InitVal  */
#line 514 "src/parser.y"
              {
        ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setArrayValue(arrayValue);
        ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setNotZeroNum(notZeroNum);
        if ((notZeroNum == 0) || ((InitValueListExpr*)(yyvsp[0].exprtype))->isEmpty()){
            ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setAllZero();
        }
        if (!identifiers->install((yyvsp[-4].strtype), (yyvsp[-1].se)))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[-4].strtype));
        (yyval.stmttype) = new DeclStmt(new Id((yyvsp[-1].se)), (yyvsp[0].exprtype));
        delete [](yyvsp[-4].strtype);
    }
#line 2119 "src/parser.cpp"
    break;

  case 83: /* ConstDef: ID ASSIGN ConstInitVal  */
#line 527 "src/parser.y"
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
#line 2150 "src/parser.cpp"
    break;

  case 84: /* @4: %empty  */
#line 553 "src/parser.y"
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
#line 2189 "src/parser.cpp"
    break;

  case 85: /* ConstDef: ID ArrayIndices ASSIGN @4 ConstInitVal  */
#line 587 "src/parser.y"
                   {
        ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setArrayValue(arrayValue);
        ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setNotZeroNum(notZeroNum);
        if ((notZeroNum == 0) || ((InitValueListExpr*)(yyvsp[0].exprtype))->isEmpty())
            ((IdentifierSymbolEntry*)(yyvsp[-1].se))->setAllZero();
        if (!identifiers->install((yyvsp[-4].strtype), (yyvsp[-1].se)))
            fprintf(stderr, "identifier \"%s\" is already defined\n", (char*)(yyvsp[-4].strtype));
        identifiers->install((yyvsp[-4].strtype), (yyvsp[-1].se));
        (yyval.stmttype) = new DeclStmt(new Id((yyvsp[-1].se)), (yyvsp[0].exprtype));
        delete [](yyvsp[-4].strtype);
    }
#line 2205 "src/parser.cpp"
    break;

  case 86: /* ArrayIndices: LBRACKET ConstExp RBRACKET  */
#line 600 "src/parser.y"
                                 {
        (yyval.exprtype) = (yyvsp[-1].exprtype);
    }
#line 2213 "src/parser.cpp"
    break;

  case 87: /* ArrayIndices: ArrayIndices LBRACKET ConstExp RBRACKET  */
#line 603 "src/parser.y"
                                              {
        (yyval.exprtype) = (yyvsp[-3].exprtype);
        (yyvsp[-3].exprtype)->setNext((yyvsp[-1].exprtype));
    }
#line 2222 "src/parser.cpp"
    break;

  case 88: /* InitVal: Exp  */
#line 609 "src/parser.y"
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
#line 2283 "src/parser.cpp"
    break;

  case 89: /* InitVal: LBRACE RBRACE  */
#line 665 "src/parser.y"
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
#line 2314 "src/parser.cpp"
    break;

  case 90: /* @5: %empty  */
#line 691 "src/parser.y"
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
#line 2336 "src/parser.cpp"
    break;

  case 91: /* InitVal: LBRACE @5 InitValList RBRACE  */
#line 708 "src/parser.y"
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
#line 2359 "src/parser.cpp"
    break;

  case 92: /* ConstInitVal: ConstExp  */
#line 729 "src/parser.y"
               {
        (yyval.exprtype) = (yyvsp[0].exprtype);
        if (!stk.empty()) {

            double val = (yyvsp[0].exprtype)->getValue();
            if (!val)
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
#line 2423 "src/parser.cpp"
    break;

  case 93: /* ConstInitVal: LBRACE RBRACE  */
#line 788 "src/parser.y"
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
#line 2454 "src/parser.cpp"
    break;

  case 94: /* @6: %empty  */
#line 814 "src/parser.y"
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
#line 2476 "src/parser.cpp"
    break;

  case 95: /* ConstInitVal: LBRACE @6 ConstInitValList RBRACE  */
#line 831 "src/parser.y"
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
#line 2499 "src/parser.cpp"
    break;

  case 96: /* InitValList: InitVal  */
#line 851 "src/parser.y"
              {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 2507 "src/parser.cpp"
    break;

  case 97: /* InitValList: InitValList COMMA InitVal  */
#line 854 "src/parser.y"
                                {
        (yyval.exprtype) = (yyvsp[-2].exprtype);
    }
#line 2515 "src/parser.cpp"
    break;

  case 98: /* ConstInitValList: ConstInitVal  */
#line 859 "src/parser.y"
                   {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 2523 "src/parser.cpp"
    break;

  case 99: /* ConstInitValList: ConstInitValList COMMA ConstInitVal  */
#line 862 "src/parser.y"
                                          {
        (yyval.exprtype) = (yyvsp[-2].exprtype);
    }
#line 2531 "src/parser.cpp"
    break;

  case 100: /* $@7: %empty  */
#line 868 "src/parser.y"
            {
        // SymbolTable::resetLabel();
        identifiers = new SymbolTable(identifiers);
        paramNo = 0;
        fpParamNo = 0;
        funcRetType = (yyvsp[-1].type);
    }
#line 2543 "src/parser.cpp"
    break;

  case 101: /* @8: %empty  */
#line 875 "src/parser.y"
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
#line 2566 "src/parser.cpp"
    break;

  case 102: /* FuncDef: Type ID $@7 LPAREN MaybeFuncFParams RPAREN @8 BlockStmt  */
#line 893 "src/parser.y"
                {
        (yyval.stmttype) = new FunctionDef((yyvsp[-1].se), (DeclStmt*)(yyvsp[-3].stmttype), (yyvsp[0].stmttype));
        SymbolTable* top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete [](yyvsp[-6].strtype);
    }
#line 2578 "src/parser.cpp"
    break;

  case 103: /* MaybeFuncFParams: FuncFParams  */
#line 902 "src/parser.y"
                  { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 2584 "src/parser.cpp"
    break;

  case 104: /* MaybeFuncFParams: %empty  */
#line 903 "src/parser.y"
             { (yyval.stmttype) = nullptr; }
#line 2590 "src/parser.cpp"
    break;

  case 105: /* FuncFParams: FuncFParams COMMA FuncFParam  */
#line 905 "src/parser.y"
                                   {
        (yyval.stmttype) = (yyvsp[-2].stmttype);
        (yyval.stmttype)->setNext((yyvsp[0].stmttype));
    }
#line 2599 "src/parser.cpp"
    break;

  case 106: /* FuncFParams: FuncFParam  */
#line 909 "src/parser.y"
                 {
        (yyval.stmttype) = (yyvsp[0].stmttype);
    }
#line 2607 "src/parser.cpp"
    break;

  case 107: /* FuncFParam: Type ID  */
#line 914 "src/parser.y"
              {
        SymbolEntry* se;
        if ((yyvsp[-1].type)->isFloat()) {
            se = new IdentifierSymbolEntry((yyvsp[-1].type), (yyvsp[0].strtype), identifiers->getLevel(), fpParamNo++);
        } else {
            se = new IdentifierSymbolEntry((yyvsp[-1].type), (yyvsp[0].strtype), identifiers->getLevel(), paramNo++);
        }
        identifiers->install((yyvsp[0].strtype), se);
        ((IdentifierSymbolEntry*)se)->setLabel();
        ((IdentifierSymbolEntry*)se)->setAddr(new Operand(se));
        (yyval.stmttype) = new DeclStmt(new Id(se));
        delete [](yyvsp[0].strtype);
    }
#line 2625 "src/parser.cpp"
    break;

  case 108: /* FuncFParam: Type ID FuncArrayIndices  */
#line 927 "src/parser.y"
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
        identifiers->install((yyvsp[-1].strtype), se);
        ((IdentifierSymbolEntry*)se)->setLabel();
        ((IdentifierSymbolEntry*)se)->setAddr(new Operand(se));
        (yyval.stmttype) = new DeclStmt(new Id(se));
        delete [](yyvsp[-1].strtype);
    }
#line 2655 "src/parser.cpp"
    break;

  case 109: /* FuncArrayIndices: LBRACKET RBRACKET  */
#line 954 "src/parser.y"
                        {
        (yyval.exprtype) = new ExprNode(nullptr);
    }
#line 2663 "src/parser.cpp"
    break;

  case 110: /* FuncArrayIndices: FuncArrayIndices LBRACKET Exp RBRACKET  */
#line 957 "src/parser.y"
                                             {
        (yyval.exprtype) = (yyvsp[-3].exprtype);
        (yyval.exprtype)->setNext((yyvsp[-1].exprtype));
    }
#line 2672 "src/parser.cpp"
    break;


#line 2676 "src/parser.cpp"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
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
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 961 "src/parser.y"


int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
