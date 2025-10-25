// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

// "%code top" blocks.
#line 31 "frontend/parser/yacc.y"

    #include <iostream>

    #include <frontend/parser/parser.h>
    #include <frontend/parser/location.hh>
    #include <frontend/parser/scanner.h>
    #include <frontend/parser/yacc.h>

    using namespace FE;
    using namespace FE::AST;

    static YaccParser::symbol_type yylex(Scanner& scanner, Parser &parser)
    {
        (void)parser;
        return scanner.nextToken(); 
    }

    extern size_t errCnt;

#line 59 "frontend/parser/yacc.cpp"




#include "yacc.h"




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 4 "frontend/parser/yacc.y"
namespace  FE  {
#line 159 "frontend/parser/yacc.cpp"

  /// Build a parser object.
   YaccParser :: YaccParser  (FE::Scanner& scanner_yyarg, FE::Parser& parser_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      scanner (scanner_yyarg),
      parser (parser_yyarg)
  {}

   YaccParser ::~ YaccParser  ()
  {}

   YaccParser ::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
   YaccParser ::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

   YaccParser ::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
   YaccParser ::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
   YaccParser ::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

   YaccParser ::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

   YaccParser ::symbol_kind_type
   YaccParser ::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

   YaccParser ::stack_symbol_type::stack_symbol_type ()
  {}

   YaccParser ::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_ASSIGN_EXPR: // ASSIGN_EXPR
      case symbol_kind::S_EXPR: // EXPR
      case symbol_kind::S_NOCOMMA_EXPR: // NOCOMMA_EXPR
      case symbol_kind::S_LOGICAL_OR_EXPR: // LOGICAL_OR_EXPR
      case symbol_kind::S_LOGICAL_AND_EXPR: // LOGICAL_AND_EXPR
      case symbol_kind::S_EQUALITY_EXPR: // EQUALITY_EXPR
      case symbol_kind::S_RELATIONAL_EXPR: // RELATIONAL_EXPR
      case symbol_kind::S_ADDSUB_EXPR: // ADDSUB_EXPR
      case symbol_kind::S_MULDIV_EXPR: // MULDIV_EXPR
      case symbol_kind::S_UNARY_EXPR: // UNARY_EXPR
      case symbol_kind::S_BASIC_EXPR: // BASIC_EXPR
      case symbol_kind::S_FUNC_CALL_EXPR: // FUNC_CALL_EXPR
      case symbol_kind::S_LEFT_VAL_EXPR: // LEFT_VAL_EXPR
      case symbol_kind::S_LITERAL_EXPR: // LITERAL_EXPR
        value.YY_MOVE_OR_COPY< FE::AST::ExprNode* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INITIALIZER: // INITIALIZER
        value.YY_MOVE_OR_COPY< FE::AST::InitDecl* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_UNARY_OP: // UNARY_OP
        value.YY_MOVE_OR_COPY< FE::AST::Operator > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_PARAM_DECLARATOR: // PARAM_DECLARATOR
        value.YY_MOVE_OR_COPY< FE::AST::ParamDeclarator* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_PROGRAM: // PROGRAM
        value.YY_MOVE_OR_COPY< FE::AST::Root* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STMT: // STMT
      case symbol_kind::S_CONTINUE_STMT: // CONTINUE_STMT
      case symbol_kind::S_EXPR_STMT: // EXPR_STMT
      case symbol_kind::S_VAR_DECL_STMT: // VAR_DECL_STMT
      case symbol_kind::S_FUNC_BODY: // FUNC_BODY
      case symbol_kind::S_FUNC_DECL_STMT: // FUNC_DECL_STMT
      case symbol_kind::S_FOR_STMT: // FOR_STMT
      case symbol_kind::S_IF_STMT: // IF_STMT
      case symbol_kind::S_RETURN_STMT: // RETURN_STMT
      case symbol_kind::S_WHILE_STMT: // WHILE_STMT
      case symbol_kind::S_BLOCK_STMT: // BLOCK_STMT
        value.YY_MOVE_OR_COPY< FE::AST::StmtNode* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_TYPE: // TYPE
        value.YY_MOVE_OR_COPY< FE::AST::Type* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_VAR_DECLARATION: // VAR_DECLARATION
        value.YY_MOVE_OR_COPY< FE::AST::VarDeclaration* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_VAR_DECLARATOR: // VAR_DECLARATOR
        value.YY_MOVE_OR_COPY< FE::AST::VarDeclarator* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_FLOAT_CONST: // FLOAT_CONST
        value.YY_MOVE_OR_COPY< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INT_CONST: // INT_CONST
        value.YY_MOVE_OR_COPY< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_LL_CONST: // LL_CONST
        value.YY_MOVE_OR_COPY< long long > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STR_CONST: // STR_CONST
      case symbol_kind::S_ERR_TOKEN: // ERR_TOKEN
      case symbol_kind::S_SLASH_COMMENT: // SLASH_COMMENT
      case symbol_kind::S_MULTI_COMMENT: // MULTI_COMMENT
      case symbol_kind::S_IDENT: // IDENT
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_EXPR_LIST: // EXPR_LIST
      case symbol_kind::S_ARRAY_DIMENSION_EXPR_LIST: // ARRAY_DIMENSION_EXPR_LIST
        value.YY_MOVE_OR_COPY< std::vector<FE::AST::ExprNode*>* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INITIALIZER_LIST: // INITIALIZER_LIST
        value.YY_MOVE_OR_COPY< std::vector<FE::AST::InitDecl*>* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_PARAM_DECLARATOR_LIST: // PARAM_DECLARATOR_LIST
        value.YY_MOVE_OR_COPY< std::vector<FE::AST::ParamDeclarator*>* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STMT_LIST: // STMT_LIST
        value.YY_MOVE_OR_COPY< std::vector<FE::AST::StmtNode*>* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_VAR_DECLARATOR_LIST: // VAR_DECLARATOR_LIST
        value.YY_MOVE_OR_COPY< std::vector<FE::AST::VarDeclarator*>* > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

   YaccParser ::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_ASSIGN_EXPR: // ASSIGN_EXPR
      case symbol_kind::S_EXPR: // EXPR
      case symbol_kind::S_NOCOMMA_EXPR: // NOCOMMA_EXPR
      case symbol_kind::S_LOGICAL_OR_EXPR: // LOGICAL_OR_EXPR
      case symbol_kind::S_LOGICAL_AND_EXPR: // LOGICAL_AND_EXPR
      case symbol_kind::S_EQUALITY_EXPR: // EQUALITY_EXPR
      case symbol_kind::S_RELATIONAL_EXPR: // RELATIONAL_EXPR
      case symbol_kind::S_ADDSUB_EXPR: // ADDSUB_EXPR
      case symbol_kind::S_MULDIV_EXPR: // MULDIV_EXPR
      case symbol_kind::S_UNARY_EXPR: // UNARY_EXPR
      case symbol_kind::S_BASIC_EXPR: // BASIC_EXPR
      case symbol_kind::S_FUNC_CALL_EXPR: // FUNC_CALL_EXPR
      case symbol_kind::S_LEFT_VAL_EXPR: // LEFT_VAL_EXPR
      case symbol_kind::S_LITERAL_EXPR: // LITERAL_EXPR
        value.move< FE::AST::ExprNode* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INITIALIZER: // INITIALIZER
        value.move< FE::AST::InitDecl* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_UNARY_OP: // UNARY_OP
        value.move< FE::AST::Operator > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_PARAM_DECLARATOR: // PARAM_DECLARATOR
        value.move< FE::AST::ParamDeclarator* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_PROGRAM: // PROGRAM
        value.move< FE::AST::Root* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STMT: // STMT
      case symbol_kind::S_CONTINUE_STMT: // CONTINUE_STMT
      case symbol_kind::S_EXPR_STMT: // EXPR_STMT
      case symbol_kind::S_VAR_DECL_STMT: // VAR_DECL_STMT
      case symbol_kind::S_FUNC_BODY: // FUNC_BODY
      case symbol_kind::S_FUNC_DECL_STMT: // FUNC_DECL_STMT
      case symbol_kind::S_FOR_STMT: // FOR_STMT
      case symbol_kind::S_IF_STMT: // IF_STMT
      case symbol_kind::S_RETURN_STMT: // RETURN_STMT
      case symbol_kind::S_WHILE_STMT: // WHILE_STMT
      case symbol_kind::S_BLOCK_STMT: // BLOCK_STMT
        value.move< FE::AST::StmtNode* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_TYPE: // TYPE
        value.move< FE::AST::Type* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_VAR_DECLARATION: // VAR_DECLARATION
        value.move< FE::AST::VarDeclaration* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_VAR_DECLARATOR: // VAR_DECLARATOR
        value.move< FE::AST::VarDeclarator* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_FLOAT_CONST: // FLOAT_CONST
        value.move< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INT_CONST: // INT_CONST
        value.move< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_LL_CONST: // LL_CONST
        value.move< long long > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STR_CONST: // STR_CONST
      case symbol_kind::S_ERR_TOKEN: // ERR_TOKEN
      case symbol_kind::S_SLASH_COMMENT: // SLASH_COMMENT
      case symbol_kind::S_MULTI_COMMENT: // MULTI_COMMENT
      case symbol_kind::S_IDENT: // IDENT
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_EXPR_LIST: // EXPR_LIST
      case symbol_kind::S_ARRAY_DIMENSION_EXPR_LIST: // ARRAY_DIMENSION_EXPR_LIST
        value.move< std::vector<FE::AST::ExprNode*>* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INITIALIZER_LIST: // INITIALIZER_LIST
        value.move< std::vector<FE::AST::InitDecl*>* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_PARAM_DECLARATOR_LIST: // PARAM_DECLARATOR_LIST
        value.move< std::vector<FE::AST::ParamDeclarator*>* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STMT_LIST: // STMT_LIST
        value.move< std::vector<FE::AST::StmtNode*>* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_VAR_DECLARATOR_LIST: // VAR_DECLARATOR_LIST
        value.move< std::vector<FE::AST::VarDeclarator*>* > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
   YaccParser ::stack_symbol_type&
   YaccParser ::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_ASSIGN_EXPR: // ASSIGN_EXPR
      case symbol_kind::S_EXPR: // EXPR
      case symbol_kind::S_NOCOMMA_EXPR: // NOCOMMA_EXPR
      case symbol_kind::S_LOGICAL_OR_EXPR: // LOGICAL_OR_EXPR
      case symbol_kind::S_LOGICAL_AND_EXPR: // LOGICAL_AND_EXPR
      case symbol_kind::S_EQUALITY_EXPR: // EQUALITY_EXPR
      case symbol_kind::S_RELATIONAL_EXPR: // RELATIONAL_EXPR
      case symbol_kind::S_ADDSUB_EXPR: // ADDSUB_EXPR
      case symbol_kind::S_MULDIV_EXPR: // MULDIV_EXPR
      case symbol_kind::S_UNARY_EXPR: // UNARY_EXPR
      case symbol_kind::S_BASIC_EXPR: // BASIC_EXPR
      case symbol_kind::S_FUNC_CALL_EXPR: // FUNC_CALL_EXPR
      case symbol_kind::S_LEFT_VAL_EXPR: // LEFT_VAL_EXPR
      case symbol_kind::S_LITERAL_EXPR: // LITERAL_EXPR
        value.copy< FE::AST::ExprNode* > (that.value);
        break;

      case symbol_kind::S_INITIALIZER: // INITIALIZER
        value.copy< FE::AST::InitDecl* > (that.value);
        break;

      case symbol_kind::S_UNARY_OP: // UNARY_OP
        value.copy< FE::AST::Operator > (that.value);
        break;

      case symbol_kind::S_PARAM_DECLARATOR: // PARAM_DECLARATOR
        value.copy< FE::AST::ParamDeclarator* > (that.value);
        break;

      case symbol_kind::S_PROGRAM: // PROGRAM
        value.copy< FE::AST::Root* > (that.value);
        break;

      case symbol_kind::S_STMT: // STMT
      case symbol_kind::S_CONTINUE_STMT: // CONTINUE_STMT
      case symbol_kind::S_EXPR_STMT: // EXPR_STMT
      case symbol_kind::S_VAR_DECL_STMT: // VAR_DECL_STMT
      case symbol_kind::S_FUNC_BODY: // FUNC_BODY
      case symbol_kind::S_FUNC_DECL_STMT: // FUNC_DECL_STMT
      case symbol_kind::S_FOR_STMT: // FOR_STMT
      case symbol_kind::S_IF_STMT: // IF_STMT
      case symbol_kind::S_RETURN_STMT: // RETURN_STMT
      case symbol_kind::S_WHILE_STMT: // WHILE_STMT
      case symbol_kind::S_BLOCK_STMT: // BLOCK_STMT
        value.copy< FE::AST::StmtNode* > (that.value);
        break;

      case symbol_kind::S_TYPE: // TYPE
        value.copy< FE::AST::Type* > (that.value);
        break;

      case symbol_kind::S_VAR_DECLARATION: // VAR_DECLARATION
        value.copy< FE::AST::VarDeclaration* > (that.value);
        break;

      case symbol_kind::S_VAR_DECLARATOR: // VAR_DECLARATOR
        value.copy< FE::AST::VarDeclarator* > (that.value);
        break;

      case symbol_kind::S_FLOAT_CONST: // FLOAT_CONST
        value.copy< float > (that.value);
        break;

      case symbol_kind::S_INT_CONST: // INT_CONST
        value.copy< int > (that.value);
        break;

      case symbol_kind::S_LL_CONST: // LL_CONST
        value.copy< long long > (that.value);
        break;

      case symbol_kind::S_STR_CONST: // STR_CONST
      case symbol_kind::S_ERR_TOKEN: // ERR_TOKEN
      case symbol_kind::S_SLASH_COMMENT: // SLASH_COMMENT
      case symbol_kind::S_MULTI_COMMENT: // MULTI_COMMENT
      case symbol_kind::S_IDENT: // IDENT
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_EXPR_LIST: // EXPR_LIST
      case symbol_kind::S_ARRAY_DIMENSION_EXPR_LIST: // ARRAY_DIMENSION_EXPR_LIST
        value.copy< std::vector<FE::AST::ExprNode*>* > (that.value);
        break;

      case symbol_kind::S_INITIALIZER_LIST: // INITIALIZER_LIST
        value.copy< std::vector<FE::AST::InitDecl*>* > (that.value);
        break;

      case symbol_kind::S_PARAM_DECLARATOR_LIST: // PARAM_DECLARATOR_LIST
        value.copy< std::vector<FE::AST::ParamDeclarator*>* > (that.value);
        break;

      case symbol_kind::S_STMT_LIST: // STMT_LIST
        value.copy< std::vector<FE::AST::StmtNode*>* > (that.value);
        break;

      case symbol_kind::S_VAR_DECLARATOR_LIST: // VAR_DECLARATOR_LIST
        value.copy< std::vector<FE::AST::VarDeclarator*>* > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

   YaccParser ::stack_symbol_type&
   YaccParser ::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_ASSIGN_EXPR: // ASSIGN_EXPR
      case symbol_kind::S_EXPR: // EXPR
      case symbol_kind::S_NOCOMMA_EXPR: // NOCOMMA_EXPR
      case symbol_kind::S_LOGICAL_OR_EXPR: // LOGICAL_OR_EXPR
      case symbol_kind::S_LOGICAL_AND_EXPR: // LOGICAL_AND_EXPR
      case symbol_kind::S_EQUALITY_EXPR: // EQUALITY_EXPR
      case symbol_kind::S_RELATIONAL_EXPR: // RELATIONAL_EXPR
      case symbol_kind::S_ADDSUB_EXPR: // ADDSUB_EXPR
      case symbol_kind::S_MULDIV_EXPR: // MULDIV_EXPR
      case symbol_kind::S_UNARY_EXPR: // UNARY_EXPR
      case symbol_kind::S_BASIC_EXPR: // BASIC_EXPR
      case symbol_kind::S_FUNC_CALL_EXPR: // FUNC_CALL_EXPR
      case symbol_kind::S_LEFT_VAL_EXPR: // LEFT_VAL_EXPR
      case symbol_kind::S_LITERAL_EXPR: // LITERAL_EXPR
        value.move< FE::AST::ExprNode* > (that.value);
        break;

      case symbol_kind::S_INITIALIZER: // INITIALIZER
        value.move< FE::AST::InitDecl* > (that.value);
        break;

      case symbol_kind::S_UNARY_OP: // UNARY_OP
        value.move< FE::AST::Operator > (that.value);
        break;

      case symbol_kind::S_PARAM_DECLARATOR: // PARAM_DECLARATOR
        value.move< FE::AST::ParamDeclarator* > (that.value);
        break;

      case symbol_kind::S_PROGRAM: // PROGRAM
        value.move< FE::AST::Root* > (that.value);
        break;

      case symbol_kind::S_STMT: // STMT
      case symbol_kind::S_CONTINUE_STMT: // CONTINUE_STMT
      case symbol_kind::S_EXPR_STMT: // EXPR_STMT
      case symbol_kind::S_VAR_DECL_STMT: // VAR_DECL_STMT
      case symbol_kind::S_FUNC_BODY: // FUNC_BODY
      case symbol_kind::S_FUNC_DECL_STMT: // FUNC_DECL_STMT
      case symbol_kind::S_FOR_STMT: // FOR_STMT
      case symbol_kind::S_IF_STMT: // IF_STMT
      case symbol_kind::S_RETURN_STMT: // RETURN_STMT
      case symbol_kind::S_WHILE_STMT: // WHILE_STMT
      case symbol_kind::S_BLOCK_STMT: // BLOCK_STMT
        value.move< FE::AST::StmtNode* > (that.value);
        break;

      case symbol_kind::S_TYPE: // TYPE
        value.move< FE::AST::Type* > (that.value);
        break;

      case symbol_kind::S_VAR_DECLARATION: // VAR_DECLARATION
        value.move< FE::AST::VarDeclaration* > (that.value);
        break;

      case symbol_kind::S_VAR_DECLARATOR: // VAR_DECLARATOR
        value.move< FE::AST::VarDeclarator* > (that.value);
        break;

      case symbol_kind::S_FLOAT_CONST: // FLOAT_CONST
        value.move< float > (that.value);
        break;

      case symbol_kind::S_INT_CONST: // INT_CONST
        value.move< int > (that.value);
        break;

      case symbol_kind::S_LL_CONST: // LL_CONST
        value.move< long long > (that.value);
        break;

      case symbol_kind::S_STR_CONST: // STR_CONST
      case symbol_kind::S_ERR_TOKEN: // ERR_TOKEN
      case symbol_kind::S_SLASH_COMMENT: // SLASH_COMMENT
      case symbol_kind::S_MULTI_COMMENT: // MULTI_COMMENT
      case symbol_kind::S_IDENT: // IDENT
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_EXPR_LIST: // EXPR_LIST
      case symbol_kind::S_ARRAY_DIMENSION_EXPR_LIST: // ARRAY_DIMENSION_EXPR_LIST
        value.move< std::vector<FE::AST::ExprNode*>* > (that.value);
        break;

      case symbol_kind::S_INITIALIZER_LIST: // INITIALIZER_LIST
        value.move< std::vector<FE::AST::InitDecl*>* > (that.value);
        break;

      case symbol_kind::S_PARAM_DECLARATOR_LIST: // PARAM_DECLARATOR_LIST
        value.move< std::vector<FE::AST::ParamDeclarator*>* > (that.value);
        break;

      case symbol_kind::S_STMT_LIST: // STMT_LIST
        value.move< std::vector<FE::AST::StmtNode*>* > (that.value);
        break;

      case symbol_kind::S_VAR_DECLARATOR_LIST: // VAR_DECLARATOR_LIST
        value.move< std::vector<FE::AST::VarDeclarator*>* > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
   YaccParser ::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
   YaccParser ::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
   YaccParser ::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
   YaccParser ::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
   YaccParser ::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
   YaccParser ::debug_stream () const
  {
    return *yycdebug_;
  }

  void
   YaccParser ::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


   YaccParser ::debug_level_type
   YaccParser ::debug_level () const
  {
    return yydebug_;
  }

  void
   YaccParser ::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

   YaccParser ::state_type
   YaccParser ::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
   YaccParser ::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
   YaccParser ::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
   YaccParser ::operator() ()
  {
    return parse ();
  }

  int
   YaccParser ::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (scanner, parser));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_ASSIGN_EXPR: // ASSIGN_EXPR
      case symbol_kind::S_EXPR: // EXPR
      case symbol_kind::S_NOCOMMA_EXPR: // NOCOMMA_EXPR
      case symbol_kind::S_LOGICAL_OR_EXPR: // LOGICAL_OR_EXPR
      case symbol_kind::S_LOGICAL_AND_EXPR: // LOGICAL_AND_EXPR
      case symbol_kind::S_EQUALITY_EXPR: // EQUALITY_EXPR
      case symbol_kind::S_RELATIONAL_EXPR: // RELATIONAL_EXPR
      case symbol_kind::S_ADDSUB_EXPR: // ADDSUB_EXPR
      case symbol_kind::S_MULDIV_EXPR: // MULDIV_EXPR
      case symbol_kind::S_UNARY_EXPR: // UNARY_EXPR
      case symbol_kind::S_BASIC_EXPR: // BASIC_EXPR
      case symbol_kind::S_FUNC_CALL_EXPR: // FUNC_CALL_EXPR
      case symbol_kind::S_LEFT_VAL_EXPR: // LEFT_VAL_EXPR
      case symbol_kind::S_LITERAL_EXPR: // LITERAL_EXPR
        yylhs.value.emplace< FE::AST::ExprNode* > ();
        break;

      case symbol_kind::S_INITIALIZER: // INITIALIZER
        yylhs.value.emplace< FE::AST::InitDecl* > ();
        break;

      case symbol_kind::S_UNARY_OP: // UNARY_OP
        yylhs.value.emplace< FE::AST::Operator > ();
        break;

      case symbol_kind::S_PARAM_DECLARATOR: // PARAM_DECLARATOR
        yylhs.value.emplace< FE::AST::ParamDeclarator* > ();
        break;

      case symbol_kind::S_PROGRAM: // PROGRAM
        yylhs.value.emplace< FE::AST::Root* > ();
        break;

      case symbol_kind::S_STMT: // STMT
      case symbol_kind::S_CONTINUE_STMT: // CONTINUE_STMT
      case symbol_kind::S_EXPR_STMT: // EXPR_STMT
      case symbol_kind::S_VAR_DECL_STMT: // VAR_DECL_STMT
      case symbol_kind::S_FUNC_BODY: // FUNC_BODY
      case symbol_kind::S_FUNC_DECL_STMT: // FUNC_DECL_STMT
      case symbol_kind::S_FOR_STMT: // FOR_STMT
      case symbol_kind::S_IF_STMT: // IF_STMT
      case symbol_kind::S_RETURN_STMT: // RETURN_STMT
      case symbol_kind::S_WHILE_STMT: // WHILE_STMT
      case symbol_kind::S_BLOCK_STMT: // BLOCK_STMT
        yylhs.value.emplace< FE::AST::StmtNode* > ();
        break;

      case symbol_kind::S_TYPE: // TYPE
        yylhs.value.emplace< FE::AST::Type* > ();
        break;

      case symbol_kind::S_VAR_DECLARATION: // VAR_DECLARATION
        yylhs.value.emplace< FE::AST::VarDeclaration* > ();
        break;

      case symbol_kind::S_VAR_DECLARATOR: // VAR_DECLARATOR
        yylhs.value.emplace< FE::AST::VarDeclarator* > ();
        break;

      case symbol_kind::S_FLOAT_CONST: // FLOAT_CONST
        yylhs.value.emplace< float > ();
        break;

      case symbol_kind::S_INT_CONST: // INT_CONST
        yylhs.value.emplace< int > ();
        break;

      case symbol_kind::S_LL_CONST: // LL_CONST
        yylhs.value.emplace< long long > ();
        break;

      case symbol_kind::S_STR_CONST: // STR_CONST
      case symbol_kind::S_ERR_TOKEN: // ERR_TOKEN
      case symbol_kind::S_SLASH_COMMENT: // SLASH_COMMENT
      case symbol_kind::S_MULTI_COMMENT: // MULTI_COMMENT
      case symbol_kind::S_IDENT: // IDENT
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_EXPR_LIST: // EXPR_LIST
      case symbol_kind::S_ARRAY_DIMENSION_EXPR_LIST: // ARRAY_DIMENSION_EXPR_LIST
        yylhs.value.emplace< std::vector<FE::AST::ExprNode*>* > ();
        break;

      case symbol_kind::S_INITIALIZER_LIST: // INITIALIZER_LIST
        yylhs.value.emplace< std::vector<FE::AST::InitDecl*>* > ();
        break;

      case symbol_kind::S_PARAM_DECLARATOR_LIST: // PARAM_DECLARATOR_LIST
        yylhs.value.emplace< std::vector<FE::AST::ParamDeclarator*>* > ();
        break;

      case symbol_kind::S_STMT_LIST: // STMT_LIST
        yylhs.value.emplace< std::vector<FE::AST::StmtNode*>* > ();
        break;

      case symbol_kind::S_VAR_DECLARATOR_LIST: // VAR_DECLARATOR_LIST
        yylhs.value.emplace< std::vector<FE::AST::VarDeclarator*>* > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // PROGRAM: STMT_LIST
#line 137 "frontend/parser/yacc.y"
              {
        yylhs.value.as < FE::AST::Root* > () = new Root(yystack_[0].value.as < std::vector<FE::AST::StmtNode*>* > ());
        parser.ast = yylhs.value.as < FE::AST::Root* > ();
    }
#line 1057 "frontend/parser/yacc.cpp"
    break;

  case 3: // PROGRAM: PROGRAM END
#line 141 "frontend/parser/yacc.y"
                  {
        YYACCEPT;
    }
#line 1065 "frontend/parser/yacc.cpp"
    break;

  case 4: // STMT_LIST: STMT
#line 147 "frontend/parser/yacc.y"
         {
        yylhs.value.as < std::vector<FE::AST::StmtNode*>* > () = new std::vector<StmtNode*>();
        if (yystack_[0].value.as < FE::AST::StmtNode* > ()) yylhs.value.as < std::vector<FE::AST::StmtNode*>* > ()->push_back(yystack_[0].value.as < FE::AST::StmtNode* > ());
    }
#line 1074 "frontend/parser/yacc.cpp"
    break;

  case 5: // STMT_LIST: STMT_LIST STMT
#line 151 "frontend/parser/yacc.y"
                     {
        yylhs.value.as < std::vector<FE::AST::StmtNode*>* > () = yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > ();
        if (yystack_[0].value.as < FE::AST::StmtNode* > ()) yylhs.value.as < std::vector<FE::AST::StmtNode*>* > ()->push_back(yystack_[0].value.as < FE::AST::StmtNode* > ());
    }
#line 1083 "frontend/parser/yacc.cpp"
    break;

  case 6: // STMT: EXPR_STMT
#line 159 "frontend/parser/yacc.y"
              {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1091 "frontend/parser/yacc.cpp"
    break;

  case 7: // STMT: VAR_DECL_STMT
#line 162 "frontend/parser/yacc.y"
                    {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1099 "frontend/parser/yacc.cpp"
    break;

  case 8: // STMT: FUNC_DECL_STMT
#line 165 "frontend/parser/yacc.y"
                     {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1107 "frontend/parser/yacc.cpp"
    break;

  case 9: // STMT: FOR_STMT
#line 168 "frontend/parser/yacc.y"
               {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1115 "frontend/parser/yacc.cpp"
    break;

  case 10: // STMT: IF_STMT
#line 171 "frontend/parser/yacc.y"
              {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1123 "frontend/parser/yacc.cpp"
    break;

  case 11: // STMT: CONTINUE_STMT
#line 174 "frontend/parser/yacc.y"
                    {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1131 "frontend/parser/yacc.cpp"
    break;

  case 12: // STMT: SEMICOLON
#line 177 "frontend/parser/yacc.y"
                {
        yylhs.value.as < FE::AST::StmtNode* > () = nullptr;
    }
#line 1139 "frontend/parser/yacc.cpp"
    break;

  case 13: // STMT: RETURN_STMT
#line 180 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1147 "frontend/parser/yacc.cpp"
    break;

  case 14: // STMT: WHILE_STMT
#line 183 "frontend/parser/yacc.y"
                 {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1155 "frontend/parser/yacc.cpp"
    break;

  case 15: // STMT: BLOCK_STMT
#line 186 "frontend/parser/yacc.y"
                 {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1163 "frontend/parser/yacc.cpp"
    break;

  case 16: // STMT: SLASH_COMMENT
#line 189 "frontend/parser/yacc.y"
                    {
        // 单行注释，不生成AST节点，直接忽略
        yylhs.value.as < FE::AST::StmtNode* > () = nullptr;
    }
#line 1172 "frontend/parser/yacc.cpp"
    break;

  case 17: // STMT: MULTI_COMMENT
#line 193 "frontend/parser/yacc.y"
                    {
        // 多行注释，不生成AST节点，直接忽略
        yylhs.value.as < FE::AST::StmtNode* > () = nullptr;
    }
#line 1181 "frontend/parser/yacc.cpp"
    break;

  case 18: // CONTINUE_STMT: CONTINUE SEMICOLON
#line 200 "frontend/parser/yacc.y"
                       {
        yylhs.value.as < FE::AST::StmtNode* > () = new ContinueStmt(yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1189 "frontend/parser/yacc.cpp"
    break;

  case 19: // EXPR_STMT: EXPR SEMICOLON
#line 206 "frontend/parser/yacc.y"
                   {
        yylhs.value.as < FE::AST::StmtNode* > () = new ExprStmt(yystack_[1].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1197 "frontend/parser/yacc.cpp"
    break;

  case 20: // VAR_DECLARATION: TYPE VAR_DECLARATOR_LIST
#line 212 "frontend/parser/yacc.y"
                             {
        yylhs.value.as < FE::AST::VarDeclaration* > () = new VarDeclaration(yystack_[1].value.as < FE::AST::Type* > (), yystack_[0].value.as < std::vector<FE::AST::VarDeclarator*>* > (), false, yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1205 "frontend/parser/yacc.cpp"
    break;

  case 21: // VAR_DECLARATION: CONST TYPE VAR_DECLARATOR_LIST
#line 215 "frontend/parser/yacc.y"
                                     {
        yylhs.value.as < FE::AST::VarDeclaration* > () = new VarDeclaration(yystack_[1].value.as < FE::AST::Type* > (), yystack_[0].value.as < std::vector<FE::AST::VarDeclarator*>* > (), true, yystack_[2].location.begin.line, yystack_[2].location.begin.column);
    }
#line 1213 "frontend/parser/yacc.cpp"
    break;

  case 22: // VAR_DECL_STMT: VAR_DECLARATION SEMICOLON
#line 222 "frontend/parser/yacc.y"
                              {
        yylhs.value.as < FE::AST::StmtNode* > () = new VarDeclStmt(yystack_[1].value.as < FE::AST::VarDeclaration* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1221 "frontend/parser/yacc.cpp"
    break;

  case 23: // FUNC_BODY: LBRACE RBRACE
#line 228 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::StmtNode* > () = nullptr;
    }
#line 1229 "frontend/parser/yacc.cpp"
    break;

  case 24: // FUNC_BODY: LBRACE STMT_LIST RBRACE
#line 231 "frontend/parser/yacc.y"
                              {
        if (!yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > () || yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > ()->empty())
        {
            yylhs.value.as < FE::AST::StmtNode* > () = nullptr;
            delete yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > ();
        }
        else if (yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > ()->size() == 1)
        {
            yylhs.value.as < FE::AST::StmtNode* > () = (*yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > ())[0];
            delete yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > ();
        }
        else yylhs.value.as < FE::AST::StmtNode* > () = new BlockStmt(yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > (), yystack_[2].location.begin.line, yystack_[2].location.begin.column);
    }
#line 1247 "frontend/parser/yacc.cpp"
    break;

  case 25: // FUNC_DECL_STMT: TYPE IDENT LPAREN PARAM_DECLARATOR_LIST RPAREN FUNC_BODY
#line 247 "frontend/parser/yacc.y"
                                                             {
        Entry* entry = Entry::getEntry(yystack_[4].value.as < std::string > ());
        yylhs.value.as < FE::AST::StmtNode* > () = new FuncDeclStmt(yystack_[5].value.as < FE::AST::Type* > (), entry, yystack_[2].value.as < std::vector<FE::AST::ParamDeclarator*>* > (), yystack_[0].value.as < FE::AST::StmtNode* > (), yystack_[5].location.begin.line, yystack_[5].location.begin.column);
    }
#line 1256 "frontend/parser/yacc.cpp"
    break;

  case 26: // FOR_STMT: FOR LPAREN VAR_DECLARATION SEMICOLON EXPR SEMICOLON EXPR RPAREN STMT
#line 254 "frontend/parser/yacc.y"
                                                                         {
        VarDeclStmt* initStmt = new VarDeclStmt(yystack_[6].value.as < FE::AST::VarDeclaration* > (), yystack_[6].location.begin.line, yystack_[6].location.begin.column);
        yylhs.value.as < FE::AST::StmtNode* > () = new ForStmt(initStmt, yystack_[4].value.as < FE::AST::ExprNode* > (), yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::StmtNode* > (), yystack_[8].location.begin.line, yystack_[8].location.begin.column);
    }
#line 1265 "frontend/parser/yacc.cpp"
    break;

  case 27: // FOR_STMT: FOR LPAREN EXPR SEMICOLON EXPR SEMICOLON EXPR RPAREN STMT
#line 258 "frontend/parser/yacc.y"
                                                                {
        StmtNode* initStmt = new ExprStmt(yystack_[6].value.as < FE::AST::ExprNode* > (), yystack_[6].value.as < FE::AST::ExprNode* > ()->line_num, yystack_[6].value.as < FE::AST::ExprNode* > ()->col_num);
        yylhs.value.as < FE::AST::StmtNode* > () = new ForStmt(initStmt, yystack_[4].value.as < FE::AST::ExprNode* > (), yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::StmtNode* > (), yystack_[8].location.begin.line, yystack_[8].location.begin.column);
    }
#line 1274 "frontend/parser/yacc.cpp"
    break;

  case 28: // IF_STMT: IF LPAREN EXPR RPAREN STMT
#line 266 "frontend/parser/yacc.y"
                                {
        yylhs.value.as < FE::AST::StmtNode* > () = new IfStmt(yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::StmtNode* > (), nullptr, yystack_[4].location.begin.line, yystack_[4].location.begin.column);
    }
#line 1282 "frontend/parser/yacc.cpp"
    break;

  case 29: // IF_STMT: IF LPAREN EXPR RPAREN STMT ELSE STMT
#line 269 "frontend/parser/yacc.y"
                                           {
        yylhs.value.as < FE::AST::StmtNode* > () = new IfStmt(yystack_[4].value.as < FE::AST::ExprNode* > (), yystack_[2].value.as < FE::AST::StmtNode* > (), yystack_[0].value.as < FE::AST::StmtNode* > (), yystack_[6].location.begin.line, yystack_[6].location.begin.column);
    }
#line 1290 "frontend/parser/yacc.cpp"
    break;

  case 30: // RETURN_STMT: RETURN SEMICOLON
#line 277 "frontend/parser/yacc.y"
                     {
        yylhs.value.as < FE::AST::StmtNode* > () = new ReturnStmt(nullptr, yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1298 "frontend/parser/yacc.cpp"
    break;

  case 31: // RETURN_STMT: RETURN EXPR SEMICOLON
#line 280 "frontend/parser/yacc.y"
                            {
        yylhs.value.as < FE::AST::StmtNode* > () = new ReturnStmt(yystack_[1].value.as < FE::AST::ExprNode* > (), yystack_[2].location.begin.line, yystack_[2].location.begin.column);
    }
#line 1306 "frontend/parser/yacc.cpp"
    break;

  case 32: // WHILE_STMT: WHILE LPAREN EXPR RPAREN STMT
#line 286 "frontend/parser/yacc.y"
                                  {
        yylhs.value.as < FE::AST::StmtNode* > () = new WhileStmt(yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::StmtNode* > (), yystack_[4].location.begin.line, yystack_[4].location.begin.column);
    }
#line 1314 "frontend/parser/yacc.cpp"
    break;

  case 33: // BLOCK_STMT: LBRACE RBRACE
#line 292 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::StmtNode* > () = nullptr;
    }
#line 1322 "frontend/parser/yacc.cpp"
    break;

  case 34: // BLOCK_STMT: LBRACE STMT_LIST RBRACE
#line 295 "frontend/parser/yacc.y"
                              {
        if (!yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > () || yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > ()->empty()) {
            yylhs.value.as < FE::AST::StmtNode* > () = nullptr;
            delete yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > ();
        } else if (yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > ()->size() == 1) {
            yylhs.value.as < FE::AST::StmtNode* > () = (*yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > ())[0];
            delete yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > ();
        } else {
            yylhs.value.as < FE::AST::StmtNode* > () = new BlockStmt(yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > (), yystack_[2].location.begin.line, yystack_[2].location.begin.column);
        }
    }
#line 1338 "frontend/parser/yacc.cpp"
    break;

  case 35: // PARAM_DECLARATOR: TYPE IDENT
#line 310 "frontend/parser/yacc.y"
               {
        Entry* entry = Entry::getEntry(yystack_[0].value.as < std::string > ());
        yylhs.value.as < FE::AST::ParamDeclarator* > () = new ParamDeclarator(yystack_[1].value.as < FE::AST::Type* > (), entry, nullptr, yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1347 "frontend/parser/yacc.cpp"
    break;

  case 36: // PARAM_DECLARATOR: TYPE IDENT LBRACKET RBRACKET
#line 314 "frontend/parser/yacc.y"
                                   {
        std::vector<ExprNode*>* dim = new std::vector<ExprNode*>();
        dim->emplace_back(new LiteralExpr(-1, yystack_[1].location.begin.line, yystack_[1].location.begin.column));
        Entry* entry = Entry::getEntry(yystack_[2].value.as < std::string > ());
        yylhs.value.as < FE::AST::ParamDeclarator* > () = new ParamDeclarator(yystack_[3].value.as < FE::AST::Type* > (), entry, dim, yystack_[3].location.begin.line, yystack_[3].location.begin.column);
    }
#line 1358 "frontend/parser/yacc.cpp"
    break;

  case 37: // PARAM_DECLARATOR_LIST: %empty
#line 324 "frontend/parser/yacc.y"
                {
        yylhs.value.as < std::vector<FE::AST::ParamDeclarator*>* > () = new std::vector<ParamDeclarator*>();
    }
#line 1366 "frontend/parser/yacc.cpp"
    break;

  case 38: // PARAM_DECLARATOR_LIST: PARAM_DECLARATOR
#line 328 "frontend/parser/yacc.y"
                       {
        yylhs.value.as < std::vector<FE::AST::ParamDeclarator*>* > () = new std::vector<ParamDeclarator*>();
        yylhs.value.as < std::vector<FE::AST::ParamDeclarator*>* > ()->push_back(yystack_[0].value.as < FE::AST::ParamDeclarator* > ());
    }
#line 1375 "frontend/parser/yacc.cpp"
    break;

  case 39: // PARAM_DECLARATOR_LIST: PARAM_DECLARATOR_LIST COMMA PARAM_DECLARATOR
#line 332 "frontend/parser/yacc.y"
                                                   {
        yylhs.value.as < std::vector<FE::AST::ParamDeclarator*>* > () = yystack_[2].value.as < std::vector<FE::AST::ParamDeclarator*>* > ();
        yylhs.value.as < std::vector<FE::AST::ParamDeclarator*>* > ()->push_back(yystack_[0].value.as < FE::AST::ParamDeclarator* > ());
    }
#line 1384 "frontend/parser/yacc.cpp"
    break;

  case 40: // VAR_DECLARATOR: LEFT_VAL_EXPR
#line 340 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::VarDeclarator* > () = new VarDeclarator(yystack_[0].value.as < FE::AST::ExprNode* > (), nullptr, yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1392 "frontend/parser/yacc.cpp"
    break;

  case 41: // VAR_DECLARATOR: LEFT_VAL_EXPR ASSIGN INITIALIZER
#line 343 "frontend/parser/yacc.y"
                                       {
        yylhs.value.as < FE::AST::VarDeclarator* > () = new VarDeclarator(yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::InitDecl* > (), yystack_[2].location.begin.line, yystack_[2].location.begin.column);
    }
#line 1400 "frontend/parser/yacc.cpp"
    break;

  case 42: // VAR_DECLARATOR: IDENT LBRACKET RBRACKET
#line 346 "frontend/parser/yacc.y"
                              {
        ExprNode* lval = new LiteralExpr(0, yystack_[2].location.begin.line, yystack_[2].location.begin.column);
        std::vector<ExprNode*>* dim = new std::vector<ExprNode*>();
        dim->emplace_back(new LiteralExpr(-1, yystack_[1].location.begin.line, yystack_[1].location.begin.column));
        yylhs.value.as < FE::AST::VarDeclarator* > () = new VarDeclarator(lval, nullptr, yystack_[2].location.begin.line, yystack_[2].location.begin.column);
    }
#line 1411 "frontend/parser/yacc.cpp"
    break;

  case 43: // VAR_DECLARATOR: IDENT LBRACKET RBRACKET ARRAY_DIMENSION_EXPR_LIST
#line 352 "frontend/parser/yacc.y"
                                                        {
        ExprNode* lval = new LiteralExpr(0, yystack_[3].location.begin.line, yystack_[3].location.begin.column);
        std::vector<InitDecl*>* init_list = new std::vector<InitDecl*>();
        for (ExprNode* expr : *yystack_[0].value.as < std::vector<FE::AST::ExprNode*>* > ()) {
            init_list->push_back(new Initializer(expr, expr->line_num, expr->col_num));
        }
        InitDecl* init = new InitializerList(init_list, yystack_[0].location.begin.line, yystack_[0].location.begin.column);
        yylhs.value.as < FE::AST::VarDeclarator* > () = new VarDeclarator(lval, init, yystack_[3].location.begin.line, yystack_[3].location.begin.column);
    }
#line 1425 "frontend/parser/yacc.cpp"
    break;

  case 44: // VAR_DECLARATOR: IDENT LBRACKET RBRACKET ARRAY_DIMENSION_EXPR_LIST ASSIGN INITIALIZER
#line 361 "frontend/parser/yacc.y"
                                                                           {
        ExprNode* lval = new LiteralExpr(0, yystack_[5].location.begin.line, yystack_[5].location.begin.column);
        std::vector<InitDecl*>* init_list = new std::vector<InitDecl*>();
        for (ExprNode* expr : *yystack_[2].value.as < std::vector<FE::AST::ExprNode*>* > ()) {
            init_list->push_back(new Initializer(expr, expr->line_num, expr->col_num));
        }
        InitDecl* init = new InitializerList(init_list, yystack_[2].location.begin.line, yystack_[2].location.begin.column);
        yylhs.value.as < FE::AST::VarDeclarator* > () = new VarDeclarator(lval, init, yystack_[5].location.begin.line, yystack_[5].location.begin.column);
    }
#line 1439 "frontend/parser/yacc.cpp"
    break;

  case 45: // VAR_DECLARATOR: IDENT ARRAY_DIMENSION_EXPR_LIST
#line 370 "frontend/parser/yacc.y"
                                      {
        ExprNode* lval = new LiteralExpr(0, yystack_[1].location.begin.line, yystack_[1].location.begin.column);
        std::vector<InitDecl*>* init_list = new std::vector<InitDecl*>();
        for (ExprNode* expr : *yystack_[0].value.as < std::vector<FE::AST::ExprNode*>* > ()) {
            init_list->push_back(new Initializer(expr, expr->line_num, expr->col_num));
        }
        InitDecl* init = new InitializerList(init_list, yystack_[0].location.begin.line, yystack_[0].location.begin.column);
        yylhs.value.as < FE::AST::VarDeclarator* > () = new VarDeclarator(lval, init, yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1453 "frontend/parser/yacc.cpp"
    break;

  case 46: // VAR_DECLARATOR: IDENT ARRAY_DIMENSION_EXPR_LIST ASSIGN INITIALIZER
#line 379 "frontend/parser/yacc.y"
                                                         {
        ExprNode* lval = new LiteralExpr(0, yystack_[3].location.begin.line, yystack_[3].location.begin.column);
        std::vector<InitDecl*>* init_list = new std::vector<InitDecl*>();
        for (ExprNode* expr : *yystack_[2].value.as < std::vector<FE::AST::ExprNode*>* > ()) {
            init_list->push_back(new Initializer(expr, expr->line_num, expr->col_num));
        }
        InitDecl* init = new InitializerList(init_list, yystack_[2].location.begin.line, yystack_[2].location.begin.column);
        yylhs.value.as < FE::AST::VarDeclarator* > () = new VarDeclarator(lval, init, yystack_[3].location.begin.line, yystack_[3].location.begin.column);
    }
#line 1467 "frontend/parser/yacc.cpp"
    break;

  case 47: // VAR_DECLARATOR_LIST: VAR_DECLARATOR
#line 391 "frontend/parser/yacc.y"
                   {
        yylhs.value.as < std::vector<FE::AST::VarDeclarator*>* > () = new std::vector<VarDeclarator*>();
        yylhs.value.as < std::vector<FE::AST::VarDeclarator*>* > ()->push_back(yystack_[0].value.as < FE::AST::VarDeclarator* > ());
    }
#line 1476 "frontend/parser/yacc.cpp"
    break;

  case 48: // VAR_DECLARATOR_LIST: VAR_DECLARATOR_LIST COMMA VAR_DECLARATOR
#line 395 "frontend/parser/yacc.y"
                                               {
        yylhs.value.as < std::vector<FE::AST::VarDeclarator*>* > () = yystack_[2].value.as < std::vector<FE::AST::VarDeclarator*>* > ();
        yylhs.value.as < std::vector<FE::AST::VarDeclarator*>* > ()->push_back(yystack_[0].value.as < FE::AST::VarDeclarator* > ());
    }
#line 1485 "frontend/parser/yacc.cpp"
    break;

  case 49: // INITIALIZER: NOCOMMA_EXPR
#line 403 "frontend/parser/yacc.y"
                 {
        yylhs.value.as < FE::AST::InitDecl* > () = new Initializer(yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1493 "frontend/parser/yacc.cpp"
    break;

  case 50: // INITIALIZER: LBRACE INITIALIZER_LIST RBRACE
#line 406 "frontend/parser/yacc.y"
                                     {
        yylhs.value.as < FE::AST::InitDecl* > () = new InitializerList(yystack_[1].value.as < std::vector<FE::AST::InitDecl*>* > (), yystack_[2].location.begin.line, yystack_[2].location.begin.column);
    }
#line 1501 "frontend/parser/yacc.cpp"
    break;

  case 51: // INITIALIZER_LIST: INITIALIZER
#line 411 "frontend/parser/yacc.y"
                {
        yylhs.value.as < std::vector<FE::AST::InitDecl*>* > () = new std::vector<InitDecl*>();
        yylhs.value.as < std::vector<FE::AST::InitDecl*>* > ()->push_back(yystack_[0].value.as < FE::AST::InitDecl* > ());
    }
#line 1510 "frontend/parser/yacc.cpp"
    break;

  case 52: // INITIALIZER_LIST: INITIALIZER_LIST COMMA INITIALIZER
#line 415 "frontend/parser/yacc.y"
                                         {
        yylhs.value.as < std::vector<FE::AST::InitDecl*>* > () = yystack_[2].value.as < std::vector<FE::AST::InitDecl*>* > ();
        yylhs.value.as < std::vector<FE::AST::InitDecl*>* > ()->push_back(yystack_[0].value.as < FE::AST::InitDecl* > ());
    }
#line 1519 "frontend/parser/yacc.cpp"
    break;

  case 53: // ASSIGN_EXPR: LEFT_VAL_EXPR ASSIGN EXPR
#line 423 "frontend/parser/yacc.y"
                              {
        yylhs.value.as < FE::AST::ExprNode* > () = new AssignExpr(yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1527 "frontend/parser/yacc.cpp"
    break;

  case 54: // EXPR_LIST: NOCOMMA_EXPR
#line 429 "frontend/parser/yacc.y"
                 {
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > () = new std::vector<ExprNode*>();
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > ()->push_back(yystack_[0].value.as < FE::AST::ExprNode* > ());
    }
#line 1536 "frontend/parser/yacc.cpp"
    break;

  case 55: // EXPR_LIST: EXPR_LIST COMMA NOCOMMA_EXPR
#line 433 "frontend/parser/yacc.y"
                                   {
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > () = yystack_[2].value.as < std::vector<FE::AST::ExprNode*>* > ();
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > ()->push_back(yystack_[0].value.as < FE::AST::ExprNode* > ());
    }
#line 1545 "frontend/parser/yacc.cpp"
    break;

  case 56: // EXPR: NOCOMMA_EXPR
#line 440 "frontend/parser/yacc.y"
                 {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1553 "frontend/parser/yacc.cpp"
    break;

  case 57: // EXPR: EXPR COMMA NOCOMMA_EXPR
#line 443 "frontend/parser/yacc.y"
                              {
        if (yystack_[2].value.as < FE::AST::ExprNode* > ()->isCommaExpr()) {
            CommaExpr* ce = static_cast<CommaExpr*>(yystack_[2].value.as < FE::AST::ExprNode* > ());
            ce->exprs->push_back(yystack_[0].value.as < FE::AST::ExprNode* > ());
            yylhs.value.as < FE::AST::ExprNode* > () = ce;
        } else {
            auto vec = new std::vector<ExprNode*>();
            vec->push_back(yystack_[2].value.as < FE::AST::ExprNode* > ());
            vec->push_back(yystack_[0].value.as < FE::AST::ExprNode* > ());
            yylhs.value.as < FE::AST::ExprNode* > () = new CommaExpr(vec, yystack_[2].value.as < FE::AST::ExprNode* > ()->line_num, yystack_[2].value.as < FE::AST::ExprNode* > ()->col_num);
        }
    }
#line 1570 "frontend/parser/yacc.cpp"
    break;

  case 58: // NOCOMMA_EXPR: LOGICAL_OR_EXPR
#line 458 "frontend/parser/yacc.y"
                    {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1578 "frontend/parser/yacc.cpp"
    break;

  case 59: // NOCOMMA_EXPR: ASSIGN_EXPR
#line 461 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1586 "frontend/parser/yacc.cpp"
    break;

  case 60: // LOGICAL_OR_EXPR: LOGICAL_AND_EXPR
#line 468 "frontend/parser/yacc.y"
                     {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1594 "frontend/parser/yacc.cpp"
    break;

  case 61: // LOGICAL_OR_EXPR: LOGICAL_OR_EXPR OR LOGICAL_AND_EXPR
#line 471 "frontend/parser/yacc.y"
                                          {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::OR, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1602 "frontend/parser/yacc.cpp"
    break;

  case 62: // LOGICAL_AND_EXPR: EQUALITY_EXPR
#line 477 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1610 "frontend/parser/yacc.cpp"
    break;

  case 63: // LOGICAL_AND_EXPR: LOGICAL_AND_EXPR AND EQUALITY_EXPR
#line 480 "frontend/parser/yacc.y"
                                         {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::AND, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1618 "frontend/parser/yacc.cpp"
    break;

  case 64: // EQUALITY_EXPR: RELATIONAL_EXPR
#line 486 "frontend/parser/yacc.y"
                    {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1626 "frontend/parser/yacc.cpp"
    break;

  case 65: // EQUALITY_EXPR: EQUALITY_EXPR EQ RELATIONAL_EXPR
#line 489 "frontend/parser/yacc.y"
                                       {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::EQ, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1634 "frontend/parser/yacc.cpp"
    break;

  case 66: // EQUALITY_EXPR: EQUALITY_EXPR NE RELATIONAL_EXPR
#line 492 "frontend/parser/yacc.y"
                                       {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::NEQ, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1642 "frontend/parser/yacc.cpp"
    break;

  case 67: // RELATIONAL_EXPR: ADDSUB_EXPR
#line 498 "frontend/parser/yacc.y"
                {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1650 "frontend/parser/yacc.cpp"
    break;

  case 68: // RELATIONAL_EXPR: RELATIONAL_EXPR LT ADDSUB_EXPR
#line 501 "frontend/parser/yacc.y"
                                     {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::LT, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1658 "frontend/parser/yacc.cpp"
    break;

  case 69: // RELATIONAL_EXPR: RELATIONAL_EXPR LE ADDSUB_EXPR
#line 504 "frontend/parser/yacc.y"
                                     {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::LE, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1666 "frontend/parser/yacc.cpp"
    break;

  case 70: // RELATIONAL_EXPR: RELATIONAL_EXPR GT ADDSUB_EXPR
#line 507 "frontend/parser/yacc.y"
                                     {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::GT, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1674 "frontend/parser/yacc.cpp"
    break;

  case 71: // RELATIONAL_EXPR: RELATIONAL_EXPR GE ADDSUB_EXPR
#line 510 "frontend/parser/yacc.y"
                                     {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::GE, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1682 "frontend/parser/yacc.cpp"
    break;

  case 72: // ADDSUB_EXPR: MULDIV_EXPR
#line 516 "frontend/parser/yacc.y"
                {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1690 "frontend/parser/yacc.cpp"
    break;

  case 73: // ADDSUB_EXPR: ADDSUB_EXPR PLUS MULDIV_EXPR
#line 519 "frontend/parser/yacc.y"
                                   {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::ADD, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1698 "frontend/parser/yacc.cpp"
    break;

  case 74: // ADDSUB_EXPR: ADDSUB_EXPR MINUS MULDIV_EXPR
#line 522 "frontend/parser/yacc.y"
                                    {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::SUB, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1706 "frontend/parser/yacc.cpp"
    break;

  case 75: // MULDIV_EXPR: UNARY_EXPR
#line 528 "frontend/parser/yacc.y"
               {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1714 "frontend/parser/yacc.cpp"
    break;

  case 76: // MULDIV_EXPR: MULDIV_EXPR STAR UNARY_EXPR
#line 531 "frontend/parser/yacc.y"
                                  {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::MUL, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1722 "frontend/parser/yacc.cpp"
    break;

  case 77: // MULDIV_EXPR: MULDIV_EXPR SLASH UNARY_EXPR
#line 534 "frontend/parser/yacc.y"
                                   {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::DIV, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1730 "frontend/parser/yacc.cpp"
    break;

  case 78: // MULDIV_EXPR: MULDIV_EXPR MOD UNARY_EXPR
#line 537 "frontend/parser/yacc.y"
                                 {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::MOD, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1738 "frontend/parser/yacc.cpp"
    break;

  case 79: // UNARY_EXPR: BASIC_EXPR
#line 542 "frontend/parser/yacc.y"
               {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1746 "frontend/parser/yacc.cpp"
    break;

  case 80: // UNARY_EXPR: UNARY_OP UNARY_EXPR
#line 545 "frontend/parser/yacc.y"
                          {
        yylhs.value.as < FE::AST::ExprNode* > () = new UnaryExpr(yystack_[1].value.as < FE::AST::Operator > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > ()->line_num, yystack_[0].value.as < FE::AST::ExprNode* > ()->col_num);
    }
#line 1754 "frontend/parser/yacc.cpp"
    break;

  case 81: // BASIC_EXPR: LITERAL_EXPR
#line 551 "frontend/parser/yacc.y"
                 {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1762 "frontend/parser/yacc.cpp"
    break;

  case 82: // BASIC_EXPR: LEFT_VAL_EXPR
#line 554 "frontend/parser/yacc.y"
                    {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1770 "frontend/parser/yacc.cpp"
    break;

  case 83: // BASIC_EXPR: LPAREN EXPR RPAREN
#line 557 "frontend/parser/yacc.y"
                         {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[1].value.as < FE::AST::ExprNode* > ();
    }
#line 1778 "frontend/parser/yacc.cpp"
    break;

  case 84: // BASIC_EXPR: FUNC_CALL_EXPR
#line 560 "frontend/parser/yacc.y"
                     {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1786 "frontend/parser/yacc.cpp"
    break;

  case 85: // FUNC_CALL_EXPR: IDENT LPAREN RPAREN
#line 566 "frontend/parser/yacc.y"
                        {
        std::string funcName = yystack_[2].value.as < std::string > ();
        if (funcName != "starttime" && funcName != "stoptime")
        {
            Entry* entry = Entry::getEntry(funcName);
            yylhs.value.as < FE::AST::ExprNode* > () = new CallExpr(entry, nullptr, yystack_[2].location.begin.line, yystack_[2].location.begin.column);
        }
        else
        {    
            funcName = "_sysy_" + funcName;
            std::vector<ExprNode*>* args = new std::vector<ExprNode*>();
            args->emplace_back(new LiteralExpr(static_cast<int>(yystack_[2].location.begin.line), yystack_[2].location.begin.line, yystack_[2].location.begin.column));
            yylhs.value.as < FE::AST::ExprNode* > () = new CallExpr(Entry::getEntry(funcName), args, yystack_[2].location.begin.line, yystack_[2].location.begin.column);
        }
    }
#line 1806 "frontend/parser/yacc.cpp"
    break;

  case 86: // FUNC_CALL_EXPR: IDENT LPAREN EXPR_LIST RPAREN
#line 581 "frontend/parser/yacc.y"
                                    {
        Entry* entry = Entry::getEntry(yystack_[3].value.as < std::string > ());
        yylhs.value.as < FE::AST::ExprNode* > () = new CallExpr(entry, yystack_[1].value.as < std::vector<FE::AST::ExprNode*>* > (), yystack_[3].location.begin.line, yystack_[3].location.begin.column);
    }
#line 1815 "frontend/parser/yacc.cpp"
    break;

  case 87: // ARRAY_DIMENSION_EXPR_LIST: LBRACKET NOCOMMA_EXPR RBRACKET
#line 595 "frontend/parser/yacc.y"
                                   {
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > () = new std::vector<ExprNode*>();
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > ()->push_back(yystack_[1].value.as < FE::AST::ExprNode* > ());
    }
#line 1824 "frontend/parser/yacc.cpp"
    break;

  case 88: // ARRAY_DIMENSION_EXPR_LIST: ARRAY_DIMENSION_EXPR_LIST LBRACKET NOCOMMA_EXPR RBRACKET
#line 599 "frontend/parser/yacc.y"
                                                               {
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > () = yystack_[3].value.as < std::vector<FE::AST::ExprNode*>* > ();
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > ()->push_back(yystack_[1].value.as < FE::AST::ExprNode* > ());
    }
#line 1833 "frontend/parser/yacc.cpp"
    break;

  case 89: // LEFT_VAL_EXPR: IDENT
#line 606 "frontend/parser/yacc.y"
          {
        Entry* entry = Entry::getEntry(yystack_[0].value.as < std::string > ());
        yylhs.value.as < FE::AST::ExprNode* > () = new LeftValExpr(entry, nullptr, yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1842 "frontend/parser/yacc.cpp"
    break;

  case 90: // LEFT_VAL_EXPR: IDENT ARRAY_DIMENSION_EXPR_LIST
#line 610 "frontend/parser/yacc.y"
                                      {
        Entry* entry = Entry::getEntry(yystack_[1].value.as < std::string > ());
        yylhs.value.as < FE::AST::ExprNode* > () = new LeftValExpr(entry, yystack_[0].value.as < std::vector<FE::AST::ExprNode*>* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1851 "frontend/parser/yacc.cpp"
    break;

  case 91: // LITERAL_EXPR: INT_CONST
#line 617 "frontend/parser/yacc.y"
              {
        yylhs.value.as < FE::AST::ExprNode* > () = new LiteralExpr(yystack_[0].value.as < int > (), yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1859 "frontend/parser/yacc.cpp"
    break;

  case 92: // LITERAL_EXPR: FLOAT_CONST
#line 620 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::ExprNode* > () = new LiteralExpr(yystack_[0].value.as < float > (), yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1867 "frontend/parser/yacc.cpp"
    break;

  case 93: // LITERAL_EXPR: LL_CONST
#line 624 "frontend/parser/yacc.y"
               {
        yylhs.value.as < FE::AST::ExprNode* > () = new LiteralExpr(yystack_[0].value.as < long long > (), yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1875 "frontend/parser/yacc.cpp"
    break;

  case 94: // LITERAL_EXPR: STR_CONST
#line 627 "frontend/parser/yacc.y"
                {
        yylhs.value.as < FE::AST::ExprNode* > () = new LiteralExpr(yystack_[0].value.as < std::string > (), yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1883 "frontend/parser/yacc.cpp"
    break;

  case 95: // TYPE: INT
#line 635 "frontend/parser/yacc.y"
        {
        yylhs.value.as < FE::AST::Type* > () = TypeFactory::getBasicType(Type_t::INT);
    }
#line 1891 "frontend/parser/yacc.cpp"
    break;

  case 96: // TYPE: FLOAT
#line 638 "frontend/parser/yacc.y"
            {
        yylhs.value.as < FE::AST::Type* > () = TypeFactory::getBasicType(Type_t::FLOAT);
    }
#line 1899 "frontend/parser/yacc.cpp"
    break;

  case 97: // UNARY_OP: PLUS
#line 645 "frontend/parser/yacc.y"
         {
        yylhs.value.as < FE::AST::Operator > () = Operator::ADD;
    }
#line 1907 "frontend/parser/yacc.cpp"
    break;

  case 98: // UNARY_OP: MINUS
#line 648 "frontend/parser/yacc.y"
            {
        yylhs.value.as < FE::AST::Operator > () = Operator::SUB;
    }
#line 1915 "frontend/parser/yacc.cpp"
    break;

  case 99: // UNARY_OP: NOT
#line 651 "frontend/parser/yacc.y"
          {
        yylhs.value.as < FE::AST::Operator > () = Operator::NOT;
    }
#line 1923 "frontend/parser/yacc.cpp"
    break;


#line 1927 "frontend/parser/yacc.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
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


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
   YaccParser ::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
   YaccParser ::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
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
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
   YaccParser ::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  //  YaccParser ::context.
   YaccParser ::context::context (const  YaccParser & yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
   YaccParser ::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
   YaccParser ::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
   YaccParser ::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char  YaccParser ::yypact_ninf_ = -114;

  const signed char  YaccParser ::yytable_ninf_ = -1;

  const short
   YaccParser ::yypact_[] =
  {
     308,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,  -114,
    -114,  -114,    16,   -38,     4,    10,   -11,   388,    50,  -114,
     415,    60,     2,   308,  -114,  -114,  -114,    26,  -114,  -114,
    -114,  -114,  -114,  -114,  -114,  -114,    39,  -114,    21,    65,
      93,     6,    98,    36,  -114,  -114,  -114,    88,  -114,    62,
     415,   355,   415,    57,   415,   383,   415,  -114,  -114,    84,
      91,   -19,  -114,   170,  -114,  -114,  -114,  -114,  -114,   415,
     415,   415,   415,   415,   415,   415,   415,   415,   415,   415,
     415,   415,   415,   415,    17,  -114,    81,   123,  -114,  -114,
    -114,    29,  -114,    96,   415,    30,   106,    87,    91,    52,
    -114,   103,    81,  -114,  -114,  -114,    65,    93,     6,     6,
      98,    98,    98,    98,    36,    36,  -114,  -114,  -114,   107,
      50,   339,     1,    91,    11,   415,  -114,  -114,   108,   308,
     415,   415,   308,  -114,    69,   140,   124,    11,  -114,    11,
    -114,  -114,  -114,  -114,   141,    89,    95,  -114,    50,   125,
     132,     3,  -114,  -114,   -30,   308,   415,   415,  -114,   216,
    -114,   134,    11,    11,  -114,  -114,    70,    80,  -114,   262,
    -114,  -114,  -114,   308,   308,  -114,  -114,  -114
  };

  const signed char
   YaccParser ::yydefact_[] =
  {
       0,    91,    92,    93,    94,    16,    17,    97,    98,    99,
      95,    96,    89,     0,     0,     0,     0,     0,     0,    12,
       0,     0,     0,     2,     4,    11,     6,     0,     7,     8,
       9,    10,    13,    14,    15,    59,     0,    56,    58,    60,
      62,    64,    67,    72,    75,    79,    84,    82,    81,     0,
       0,     0,     0,    90,     0,     0,     0,    18,    30,     0,
       0,     0,    33,     0,     1,     3,     5,    22,    19,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    89,    47,    20,    40,    80,    82,
      85,     0,    54,     0,     0,     0,     0,     0,     0,     0,
      31,    89,    21,    83,    34,    57,    61,    63,    66,    65,
      69,    71,    68,    70,    73,    74,    76,    77,    78,    53,
      37,     0,    45,     0,     0,     0,    86,    87,     0,     0,
       0,     0,     0,    38,     0,     0,    42,     0,    48,     0,
      41,    49,    55,    88,    28,     0,     0,    32,     0,     0,
      35,    43,    46,    51,     0,     0,     0,     0,    39,     0,
      25,     0,     0,     0,    50,    29,     0,     0,    23,     0,
      36,    44,    52,     0,     0,    24,    26,    27
  };

  const signed char
   YaccParser ::yypgoto_[] =
  {
    -114,  -114,   -20,   -23,  -114,  -114,   115,  -114,  -114,  -114,
    -114,  -114,  -114,  -114,  -114,    33,  -114,    59,   126,  -113,
    -114,  -114,  -114,   -13,   -42,  -114,   114,   118,    68,   -43,
      74,   -44,  -114,  -114,    -9,    85,  -114,   -10,  -114
  };

  const unsigned char
   YaccParser ::yydefgoto_[] =
  {
       0,    22,    23,    24,    25,    26,    27,    28,   160,    29,
      30,    31,    32,    33,    34,   133,   134,    85,    86,   140,
     154,    35,    91,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,   122,    47,    48,    49,    50
  };

  const unsigned char
   YaccParser ::yytable_[] =
  {
      66,    63,    64,    53,    59,    54,    88,    61,    60,    92,
      93,   137,   163,   162,     1,     2,     3,     4,   164,    74,
      75,    76,    77,    69,   152,   103,   153,   105,     7,     8,
      57,   110,   111,   112,   113,     9,   116,   117,   118,    12,
      66,    95,    97,    99,    70,    98,    94,    55,    94,   171,
     172,    65,   128,    56,    20,    80,    81,    82,   139,    51,
     120,    52,   121,     1,     2,     3,     4,    67,     5,     6,
     119,   125,    69,   126,   129,    10,    11,     7,     8,    93,
      68,    69,   141,   142,     9,    10,    11,    71,    12,    13,
      84,    14,    15,    16,    69,   141,   132,   141,    83,    17,
      18,    19,    94,    20,    72,    73,   144,    21,    62,   147,
     135,   148,    69,   149,   173,    78,    79,   145,   146,   101,
     141,   141,    69,   123,   174,   100,    69,   151,   131,    69,
     156,    69,   165,   124,    87,    89,   157,    69,   135,   169,
     108,   109,   127,   166,   167,    87,    66,   130,   121,    69,
     176,   177,   114,   115,   143,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,   150,    52,
      96,   155,   159,     1,     2,     3,     4,   161,     5,     6,
     170,   158,   138,    87,   106,     0,   102,     7,     8,   107,
       0,     0,     0,     0,     9,    10,    11,     0,    12,    13,
       0,    14,    15,    16,     0,     0,     0,     0,    87,    17,
      18,    19,     0,    20,     0,     0,     0,    21,   104,     1,
       2,     3,     4,     0,     5,     6,     0,     0,     0,     0,
       0,     0,     0,     7,     8,     0,     0,     0,     0,     0,
       9,    10,    11,     0,    12,    13,     0,    14,    15,    16,
       0,     0,     0,     0,     0,    17,    18,    19,     0,    20,
       0,     0,     0,    21,   168,     1,     2,     3,     4,     0,
       5,     6,     0,     0,     0,     0,     0,     0,     0,     7,
       8,     0,     0,     0,     0,     0,     9,    10,    11,     0,
      12,    13,     0,    14,    15,    16,     0,     0,     0,     0,
       0,    17,    18,    19,     0,    20,     0,     0,     0,    21,
     175,     1,     2,     3,     4,     0,     5,     6,     0,     0,
       0,     0,     0,     0,     0,     7,     8,     0,     0,     0,
       0,     0,     9,    10,    11,     0,    12,    13,     0,    14,
      15,    16,     1,     2,     3,     4,     0,    17,    18,    19,
       0,    20,     0,     0,     0,    21,     7,     8,     1,     2,
       3,     4,     0,     9,     0,     0,     0,    12,     0,     0,
       0,     0,     7,     8,     0,     0,     0,     0,     0,     9,
       0,     0,    20,    12,     0,   136,     1,     2,     3,     4,
       0,     1,     2,     3,     4,     0,     0,     0,    20,    90,
       7,     8,     0,     0,     0,     7,     8,     9,    10,    11,
       0,    12,     9,     0,     0,     0,    12,     0,     1,     2,
       3,     4,     0,    18,     0,     0,    20,     0,     0,    58,
       0,    20,     7,     8,     0,     0,     0,     0,     0,     9,
       0,     0,     0,    12,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    20
  };

  const short
   YaccParser ::yycheck_[] =
  {
      23,    21,     0,    12,    17,    43,    50,    20,    18,    51,
      52,    10,    42,    10,     3,     4,     5,     6,    48,    13,
      14,    15,    16,    42,   137,    44,   139,    69,    17,    18,
      41,    74,    75,    76,    77,    24,    80,    81,    82,    28,
      63,    54,    55,    56,    23,    55,    45,    43,    45,   162,
     163,    49,    94,    43,    43,    19,    20,    21,    47,    43,
      43,    45,    45,     3,     4,     5,     6,    41,     8,     9,
      83,    42,    42,    44,    44,    25,    26,    17,    18,   121,
      41,    42,   124,   125,    24,    25,    26,    22,    28,    29,
      28,    31,    32,    33,    42,   137,    44,   139,    10,    39,
      40,    41,    45,    43,    11,    12,   129,    47,    48,   132,
     120,    42,    42,    44,    44,    17,    18,   130,   131,    28,
     162,   163,    42,    42,    44,    41,    42,   136,    41,    42,
      41,    42,   155,    10,    49,    50,    41,    42,   148,   159,
      72,    73,    46,   156,   157,    60,   169,    41,    45,    42,
     173,   174,    78,    79,    46,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    28,    45,
      55,    30,    47,     3,     4,     5,     6,    45,     8,     9,
      46,   148,   123,    98,    70,    -1,    60,    17,    18,    71,
      -1,    -1,    -1,    -1,    24,    25,    26,    -1,    28,    29,
      -1,    31,    32,    33,    -1,    -1,    -1,    -1,   123,    39,
      40,    41,    -1,    43,    -1,    -1,    -1,    47,    48,     3,
       4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    17,    18,    -1,    -1,    -1,    -1,    -1,
      24,    25,    26,    -1,    28,    29,    -1,    31,    32,    33,
      -1,    -1,    -1,    -1,    -1,    39,    40,    41,    -1,    43,
      -1,    -1,    -1,    47,    48,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,
      18,    -1,    -1,    -1,    -1,    -1,    24,    25,    26,    -1,
      28,    29,    -1,    31,    32,    33,    -1,    -1,    -1,    -1,
      -1,    39,    40,    41,    -1,    43,    -1,    -1,    -1,    47,
      48,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    17,    18,    -1,    -1,    -1,
      -1,    -1,    24,    25,    26,    -1,    28,    29,    -1,    31,
      32,    33,     3,     4,     5,     6,    -1,    39,    40,    41,
      -1,    43,    -1,    -1,    -1,    47,    17,    18,     3,     4,
       5,     6,    -1,    24,    -1,    -1,    -1,    28,    -1,    -1,
      -1,    -1,    17,    18,    -1,    -1,    -1,    -1,    -1,    24,
      -1,    -1,    43,    28,    -1,    46,     3,     4,     5,     6,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    43,    44,
      17,    18,    -1,    -1,    -1,    17,    18,    24,    25,    26,
      -1,    28,    24,    -1,    -1,    -1,    28,    -1,     3,     4,
       5,     6,    -1,    40,    -1,    -1,    43,    -1,    -1,    41,
      -1,    43,    17,    18,    -1,    -1,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    28,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43
  };

  const signed char
   YaccParser ::yystos_[] =
  {
       0,     3,     4,     5,     6,     8,     9,    17,    18,    24,
      25,    26,    28,    29,    31,    32,    33,    39,    40,    41,
      43,    47,    52,    53,    54,    55,    56,    57,    58,    60,
      61,    62,    63,    64,    65,    72,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    86,    87,    88,
      89,    43,    45,    85,    43,    43,    43,    41,    41,    74,
      88,    74,    48,    53,     0,    49,    54,    41,    41,    42,
      23,    22,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    10,    28,    68,    69,    86,    82,    86,
      44,    73,    75,    75,    45,    74,    57,    74,    88,    74,
      41,    28,    69,    44,    48,    75,    77,    78,    79,    79,
      80,    80,    80,    80,    81,    81,    82,    82,    82,    74,
      43,    45,    85,    42,    10,    42,    44,    46,    75,    44,
      41,    41,    44,    66,    67,    88,    46,    10,    68,    47,
      70,    75,    75,    46,    54,    74,    74,    54,    42,    44,
      28,    85,    70,    70,    71,    30,    41,    41,    66,    47,
      59,    45,    10,    42,    48,    54,    74,    74,    48,    53,
      46,    70,    70,    44,    44,    48,    54,    54
  };

  const signed char
   YaccParser ::yyr1_[] =
  {
       0,    51,    52,    52,    53,    53,    54,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    55,    56,
      57,    57,    58,    59,    59,    60,    61,    61,    62,    62,
      63,    63,    64,    65,    65,    66,    66,    67,    67,    67,
      68,    68,    68,    68,    68,    68,    68,    69,    69,    70,
      70,    71,    71,    72,    73,    73,    74,    74,    75,    75,
      76,    76,    77,    77,    78,    78,    78,    79,    79,    79,
      79,    79,    80,    80,    80,    81,    81,    81,    81,    82,
      82,    83,    83,    83,    83,    84,    84,    85,    85,    86,
      86,    87,    87,    87,    87,    88,    88,    89,    89,    89
  };

  const signed char
   YaccParser ::yyr2_[] =
  {
       0,     2,     1,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     3,     2,     2,     3,     6,     9,     9,     5,     7,
       2,     3,     5,     2,     3,     2,     4,     0,     1,     3,
       1,     3,     3,     4,     6,     2,     4,     1,     3,     1,
       3,     1,     3,     3,     1,     3,     1,     3,     1,     1,
       1,     3,     1,     3,     1,     3,     3,     1,     3,     3,
       3,     3,     1,     3,     3,     1,     3,     3,     3,     1,
       2,     1,     1,     3,     1,     3,     4,     3,     4,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const  YaccParser ::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "INT_CONST",
  "FLOAT_CONST", "LL_CONST", "STR_CONST", "ERR_TOKEN", "SLASH_COMMENT",
  "MULTI_COMMENT", "ASSIGN", "NE", "EQ", "LE", "GE", "LT", "GT", "PLUS",
  "MINUS", "STAR", "SLASH", "MOD", "AND", "OR", "NOT", "INT", "FLOAT",
  "STRING", "IDENT", "IF", "ELSE", "FOR", "WHILE", "CONTINUE", "BREAK",
  "SWITCH", "CASE", "GOTO", "DO", "RETURN", "CONST", "SEMICOLON", "COMMA",
  "LPAREN", "RPAREN", "LBRACKET", "RBRACKET", "LBRACE", "RBRACE", "END",
  "THEN", "$accept", "PROGRAM", "STMT_LIST", "STMT", "CONTINUE_STMT",
  "EXPR_STMT", "VAR_DECLARATION", "VAR_DECL_STMT", "FUNC_BODY",
  "FUNC_DECL_STMT", "FOR_STMT", "IF_STMT", "RETURN_STMT", "WHILE_STMT",
  "BLOCK_STMT", "PARAM_DECLARATOR", "PARAM_DECLARATOR_LIST",
  "VAR_DECLARATOR", "VAR_DECLARATOR_LIST", "INITIALIZER",
  "INITIALIZER_LIST", "ASSIGN_EXPR", "EXPR_LIST", "EXPR", "NOCOMMA_EXPR",
  "LOGICAL_OR_EXPR", "LOGICAL_AND_EXPR", "EQUALITY_EXPR",
  "RELATIONAL_EXPR", "ADDSUB_EXPR", "MULDIV_EXPR", "UNARY_EXPR",
  "BASIC_EXPR", "FUNC_CALL_EXPR", "ARRAY_DIMENSION_EXPR_LIST",
  "LEFT_VAL_EXPR", "LITERAL_EXPR", "TYPE", "UNARY_OP", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
   YaccParser ::yyrline_[] =
  {
       0,   137,   137,   141,   147,   151,   159,   162,   165,   168,
     171,   174,   177,   180,   183,   186,   189,   193,   200,   206,
     212,   215,   222,   228,   231,   247,   254,   258,   266,   269,
     277,   280,   286,   292,   295,   310,   314,   324,   328,   332,
     340,   343,   346,   352,   361,   370,   379,   391,   395,   403,
     406,   411,   415,   423,   429,   433,   440,   443,   458,   461,
     468,   471,   477,   480,   486,   489,   492,   498,   501,   504,
     507,   510,   516,   519,   522,   528,   531,   534,   537,   542,
     545,   551,   554,   557,   560,   566,   581,   595,   599,   606,
     610,   617,   620,   624,   627,   635,   638,   645,   648,   651
  };

  void
   YaccParser ::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
   YaccParser ::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


#line 4 "frontend/parser/yacc.y"
} //  FE 
#line 2576 "frontend/parser/yacc.cpp"

#line 656 "frontend/parser/yacc.y"


void FE::YaccParser::error(const FE::location& location, const std::string& message)
{
    std::cerr << "msg: " << message << ", error happened at: " << location << std::endl;
}
