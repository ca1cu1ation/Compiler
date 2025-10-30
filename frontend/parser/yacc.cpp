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
      case symbol_kind::S_BREAK_STMT: // BREAK_STMT
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
      case symbol_kind::S_BREAK_STMT: // BREAK_STMT
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
      case symbol_kind::S_BREAK_STMT: // BREAK_STMT
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
      case symbol_kind::S_BREAK_STMT: // BREAK_STMT
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
      case symbol_kind::S_BREAK_STMT: // BREAK_STMT
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
#line 139 "frontend/parser/yacc.y"
              {
        yylhs.value.as < FE::AST::Root* > () = new Root(yystack_[0].value.as < std::vector<FE::AST::StmtNode*>* > ());
        parser.ast = yylhs.value.as < FE::AST::Root* > ();
    }
#line 1062 "frontend/parser/yacc.cpp"
    break;

  case 3: // PROGRAM: PROGRAM END
#line 143 "frontend/parser/yacc.y"
                  {
        YYACCEPT;
    }
#line 1070 "frontend/parser/yacc.cpp"
    break;

  case 4: // STMT_LIST: STMT
#line 149 "frontend/parser/yacc.y"
         {
        yylhs.value.as < std::vector<FE::AST::StmtNode*>* > () = new std::vector<StmtNode*>();
        if (yystack_[0].value.as < FE::AST::StmtNode* > ()) yylhs.value.as < std::vector<FE::AST::StmtNode*>* > ()->push_back(yystack_[0].value.as < FE::AST::StmtNode* > ());
    }
#line 1079 "frontend/parser/yacc.cpp"
    break;

  case 5: // STMT_LIST: STMT_LIST STMT
#line 153 "frontend/parser/yacc.y"
                     {
        yylhs.value.as < std::vector<FE::AST::StmtNode*>* > () = yystack_[1].value.as < std::vector<FE::AST::StmtNode*>* > ();
        if (yystack_[0].value.as < FE::AST::StmtNode* > ()) yylhs.value.as < std::vector<FE::AST::StmtNode*>* > ()->push_back(yystack_[0].value.as < FE::AST::StmtNode* > ());
    }
#line 1088 "frontend/parser/yacc.cpp"
    break;

  case 6: // STMT: EXPR_STMT
#line 161 "frontend/parser/yacc.y"
              {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1096 "frontend/parser/yacc.cpp"
    break;

  case 7: // STMT: VAR_DECL_STMT
#line 164 "frontend/parser/yacc.y"
                    {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1104 "frontend/parser/yacc.cpp"
    break;

  case 8: // STMT: FUNC_DECL_STMT
#line 167 "frontend/parser/yacc.y"
                     {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1112 "frontend/parser/yacc.cpp"
    break;

  case 9: // STMT: FOR_STMT
#line 170 "frontend/parser/yacc.y"
               {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1120 "frontend/parser/yacc.cpp"
    break;

  case 10: // STMT: IF_STMT
#line 173 "frontend/parser/yacc.y"
              {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1128 "frontend/parser/yacc.cpp"
    break;

  case 11: // STMT: CONTINUE_STMT
#line 176 "frontend/parser/yacc.y"
                    {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1136 "frontend/parser/yacc.cpp"
    break;

  case 12: // STMT: SEMICOLON
#line 179 "frontend/parser/yacc.y"
                {
        yylhs.value.as < FE::AST::StmtNode* > () = nullptr;
    }
#line 1144 "frontend/parser/yacc.cpp"
    break;

  case 13: // STMT: RETURN_STMT
#line 182 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1152 "frontend/parser/yacc.cpp"
    break;

  case 14: // STMT: WHILE_STMT
#line 185 "frontend/parser/yacc.y"
                 {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1160 "frontend/parser/yacc.cpp"
    break;

  case 15: // STMT: BLOCK_STMT
#line 188 "frontend/parser/yacc.y"
                 {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1168 "frontend/parser/yacc.cpp"
    break;

  case 16: // STMT: BREAK_STMT
#line 191 "frontend/parser/yacc.y"
                 {
        yylhs.value.as < FE::AST::StmtNode* > () = yystack_[0].value.as < FE::AST::StmtNode* > ();
    }
#line 1176 "frontend/parser/yacc.cpp"
    break;

  case 17: // STMT: SLASH_COMMENT
#line 194 "frontend/parser/yacc.y"
                    {
        // 单行注释，不生成AST节点，直接忽略
        yylhs.value.as < FE::AST::StmtNode* > () = nullptr;
    }
#line 1185 "frontend/parser/yacc.cpp"
    break;

  case 18: // STMT: MULTI_COMMENT
#line 198 "frontend/parser/yacc.y"
                    {
        // 多行注释，不生成AST节点，直接忽略
        yylhs.value.as < FE::AST::StmtNode* > () = nullptr;
    }
#line 1194 "frontend/parser/yacc.cpp"
    break;

  case 19: // CONTINUE_STMT: CONTINUE SEMICOLON
#line 205 "frontend/parser/yacc.y"
                       {
        yylhs.value.as < FE::AST::StmtNode* > () = new ContinueStmt(yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1202 "frontend/parser/yacc.cpp"
    break;

  case 20: // EXPR_STMT: EXPR SEMICOLON
#line 211 "frontend/parser/yacc.y"
                   {
        yylhs.value.as < FE::AST::StmtNode* > () = new ExprStmt(yystack_[1].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1210 "frontend/parser/yacc.cpp"
    break;

  case 21: // VAR_DECLARATION: TYPE VAR_DECLARATOR_LIST
#line 217 "frontend/parser/yacc.y"
                             {
        yylhs.value.as < FE::AST::VarDeclaration* > () = new VarDeclaration(yystack_[1].value.as < FE::AST::Type* > (), yystack_[0].value.as < std::vector<FE::AST::VarDeclarator*>* > (), false, yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1218 "frontend/parser/yacc.cpp"
    break;

  case 22: // VAR_DECLARATION: CONST TYPE VAR_DECLARATOR_LIST
#line 220 "frontend/parser/yacc.y"
                                     {
        yylhs.value.as < FE::AST::VarDeclaration* > () = new VarDeclaration(yystack_[1].value.as < FE::AST::Type* > (), yystack_[0].value.as < std::vector<FE::AST::VarDeclarator*>* > (), true, yystack_[2].location.begin.line, yystack_[2].location.begin.column);
    }
#line 1226 "frontend/parser/yacc.cpp"
    break;

  case 23: // VAR_DECL_STMT: VAR_DECLARATION SEMICOLON
#line 227 "frontend/parser/yacc.y"
                              {
        yylhs.value.as < FE::AST::StmtNode* > () = new VarDeclStmt(yystack_[1].value.as < FE::AST::VarDeclaration* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1234 "frontend/parser/yacc.cpp"
    break;

  case 24: // FUNC_BODY: LBRACE RBRACE
#line 233 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::StmtNode* > () = nullptr;
    }
#line 1242 "frontend/parser/yacc.cpp"
    break;

  case 25: // FUNC_BODY: LBRACE STMT_LIST RBRACE
#line 236 "frontend/parser/yacc.y"
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
#line 1260 "frontend/parser/yacc.cpp"
    break;

  case 26: // FUNC_DECL_STMT: TYPE IDENT LPAREN PARAM_DECLARATOR_LIST RPAREN FUNC_BODY
#line 252 "frontend/parser/yacc.y"
                                                             {
        Entry* entry = Entry::getEntry(yystack_[4].value.as < std::string > ());
        yylhs.value.as < FE::AST::StmtNode* > () = new FuncDeclStmt(yystack_[5].value.as < FE::AST::Type* > (), entry, yystack_[2].value.as < std::vector<FE::AST::ParamDeclarator*>* > (), yystack_[0].value.as < FE::AST::StmtNode* > (), yystack_[5].location.begin.line, yystack_[5].location.begin.column);
    }
#line 1269 "frontend/parser/yacc.cpp"
    break;

  case 27: // FOR_STMT: FOR LPAREN VAR_DECLARATION SEMICOLON EXPR SEMICOLON EXPR RPAREN STMT
#line 259 "frontend/parser/yacc.y"
                                                                         {
        VarDeclStmt* initStmt = new VarDeclStmt(yystack_[6].value.as < FE::AST::VarDeclaration* > (), yystack_[6].location.begin.line, yystack_[6].location.begin.column);
        yylhs.value.as < FE::AST::StmtNode* > () = new ForStmt(initStmt, yystack_[4].value.as < FE::AST::ExprNode* > (), yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::StmtNode* > (), yystack_[8].location.begin.line, yystack_[8].location.begin.column);
    }
#line 1278 "frontend/parser/yacc.cpp"
    break;

  case 28: // FOR_STMT: FOR LPAREN EXPR SEMICOLON EXPR SEMICOLON EXPR RPAREN STMT
#line 263 "frontend/parser/yacc.y"
                                                                {
        StmtNode* initStmt = new ExprStmt(yystack_[6].value.as < FE::AST::ExprNode* > (), yystack_[6].value.as < FE::AST::ExprNode* > ()->line_num, yystack_[6].value.as < FE::AST::ExprNode* > ()->col_num);
        yylhs.value.as < FE::AST::StmtNode* > () = new ForStmt(initStmt, yystack_[4].value.as < FE::AST::ExprNode* > (), yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::StmtNode* > (), yystack_[8].location.begin.line, yystack_[8].location.begin.column);
    }
#line 1287 "frontend/parser/yacc.cpp"
    break;

  case 29: // IF_STMT: IF LPAREN EXPR RPAREN STMT
#line 271 "frontend/parser/yacc.y"
                                           {
        yylhs.value.as < FE::AST::StmtNode* > () = new IfStmt(yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::StmtNode* > (), nullptr, yystack_[4].location.begin.line, yystack_[4].location.begin.column);
    }
#line 1295 "frontend/parser/yacc.cpp"
    break;

  case 30: // IF_STMT: IF LPAREN EXPR RPAREN STMT ELSE STMT
#line 274 "frontend/parser/yacc.y"
                                           {
        yylhs.value.as < FE::AST::StmtNode* > () = new IfStmt(yystack_[4].value.as < FE::AST::ExprNode* > (), yystack_[2].value.as < FE::AST::StmtNode* > (), yystack_[0].value.as < FE::AST::StmtNode* > (), yystack_[6].location.begin.line, yystack_[6].location.begin.column);
    }
#line 1303 "frontend/parser/yacc.cpp"
    break;

  case 31: // RETURN_STMT: RETURN SEMICOLON
#line 282 "frontend/parser/yacc.y"
                     {
        yylhs.value.as < FE::AST::StmtNode* > () = new ReturnStmt(nullptr, yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1311 "frontend/parser/yacc.cpp"
    break;

  case 32: // RETURN_STMT: RETURN EXPR SEMICOLON
#line 285 "frontend/parser/yacc.y"
                            {
        yylhs.value.as < FE::AST::StmtNode* > () = new ReturnStmt(yystack_[1].value.as < FE::AST::ExprNode* > (), yystack_[2].location.begin.line, yystack_[2].location.begin.column);
    }
#line 1319 "frontend/parser/yacc.cpp"
    break;

  case 33: // WHILE_STMT: WHILE LPAREN EXPR RPAREN STMT
#line 291 "frontend/parser/yacc.y"
                                  {
        yylhs.value.as < FE::AST::StmtNode* > () = new WhileStmt(yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::StmtNode* > (), yystack_[4].location.begin.line, yystack_[4].location.begin.column);
    }
#line 1327 "frontend/parser/yacc.cpp"
    break;

  case 34: // BLOCK_STMT: LBRACE RBRACE
#line 297 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::StmtNode* > () = nullptr;
    }
#line 1335 "frontend/parser/yacc.cpp"
    break;

  case 35: // BLOCK_STMT: LBRACE STMT_LIST RBRACE
#line 300 "frontend/parser/yacc.y"
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
#line 1351 "frontend/parser/yacc.cpp"
    break;

  case 36: // PARAM_DECLARATOR: TYPE IDENT
#line 315 "frontend/parser/yacc.y"
               {
        Entry* entry = Entry::getEntry(yystack_[0].value.as < std::string > ());
        yylhs.value.as < FE::AST::ParamDeclarator* > () = new ParamDeclarator(yystack_[1].value.as < FE::AST::Type* > (), entry, nullptr, yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1360 "frontend/parser/yacc.cpp"
    break;

  case 37: // PARAM_DECLARATOR: TYPE IDENT LBRACKET RBRACKET
#line 319 "frontend/parser/yacc.y"
                                   {
        std::vector<ExprNode*>* dim = new std::vector<ExprNode*>();
        dim->emplace_back(new LiteralExpr(-1, yystack_[1].location.begin.line, yystack_[1].location.begin.column));
        Entry* entry = Entry::getEntry(yystack_[2].value.as < std::string > ());
        yylhs.value.as < FE::AST::ParamDeclarator* > () = new ParamDeclarator(yystack_[3].value.as < FE::AST::Type* > (), entry, dim, yystack_[3].location.begin.line, yystack_[3].location.begin.column);
    }
#line 1371 "frontend/parser/yacc.cpp"
    break;

  case 38: // PARAM_DECLARATOR: TYPE IDENT ARRAY_DIMENSION_EXPR_LIST
#line 326 "frontend/parser/yacc.y"
                                          {
        std::vector<ExprNode*>* dim = yystack_[0].value.as < std::vector<FE::AST::ExprNode*>* > ();
        Entry* entry = Entry::getEntry(yystack_[1].value.as < std::string > ());
        yylhs.value.as < FE::AST::ParamDeclarator* > () = new ParamDeclarator(yystack_[2].value.as < FE::AST::Type* > (), entry, dim, yystack_[2].location.begin.line, yystack_[2].location.begin.column);
    }
#line 1381 "frontend/parser/yacc.cpp"
    break;

  case 39: // PARAM_DECLARATOR: TYPE IDENT LBRACKET RBRACKET ARRAY_DIMENSION_EXPR_LIST
#line 331 "frontend/parser/yacc.y"
                                                            {
        std::vector<ExprNode*>* dim = yystack_[0].value.as < std::vector<FE::AST::ExprNode*>* > ();
        dim->insert(dim->begin(), new LiteralExpr(-1, yystack_[2].location.begin.line, yystack_[2].location.begin.column));
        Entry* entry = Entry::getEntry(yystack_[3].value.as < std::string > ());
        yylhs.value.as < FE::AST::ParamDeclarator* > () = new ParamDeclarator(yystack_[4].value.as < FE::AST::Type* > (), entry, dim, yystack_[4].location.begin.line, yystack_[4].location.begin.column);
    }
#line 1392 "frontend/parser/yacc.cpp"
    break;

  case 40: // PARAM_DECLARATOR_LIST: %empty
#line 340 "frontend/parser/yacc.y"
                {
        yylhs.value.as < std::vector<FE::AST::ParamDeclarator*>* > () = new std::vector<ParamDeclarator*>();
    }
#line 1400 "frontend/parser/yacc.cpp"
    break;

  case 41: // PARAM_DECLARATOR_LIST: PARAM_DECLARATOR
#line 344 "frontend/parser/yacc.y"
                       {
        yylhs.value.as < std::vector<FE::AST::ParamDeclarator*>* > () = new std::vector<ParamDeclarator*>();
        yylhs.value.as < std::vector<FE::AST::ParamDeclarator*>* > ()->push_back(yystack_[0].value.as < FE::AST::ParamDeclarator* > ());
    }
#line 1409 "frontend/parser/yacc.cpp"
    break;

  case 42: // PARAM_DECLARATOR_LIST: PARAM_DECLARATOR_LIST COMMA PARAM_DECLARATOR
#line 348 "frontend/parser/yacc.y"
                                                   {
        yylhs.value.as < std::vector<FE::AST::ParamDeclarator*>* > () = yystack_[2].value.as < std::vector<FE::AST::ParamDeclarator*>* > ();
        yylhs.value.as < std::vector<FE::AST::ParamDeclarator*>* > ()->push_back(yystack_[0].value.as < FE::AST::ParamDeclarator* > ());
    }
#line 1418 "frontend/parser/yacc.cpp"
    break;

  case 43: // VAR_DECLARATOR: LEFT_VAL_EXPR
#line 356 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::VarDeclarator* > () = new VarDeclarator(yystack_[0].value.as < FE::AST::ExprNode* > (), nullptr, yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1426 "frontend/parser/yacc.cpp"
    break;

  case 44: // VAR_DECLARATOR: LEFT_VAL_EXPR ASSIGN INITIALIZER
#line 359 "frontend/parser/yacc.y"
                                       {
        yylhs.value.as < FE::AST::VarDeclarator* > () = new VarDeclarator(yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::InitDecl* > (), yystack_[2].location.begin.line, yystack_[2].location.begin.column);
    }
#line 1434 "frontend/parser/yacc.cpp"
    break;

  case 45: // VAR_DECLARATOR: IDENT LBRACKET RBRACKET
#line 362 "frontend/parser/yacc.y"
                              {
        ExprNode* lval = new LiteralExpr(yystack_[2].value.as < std::string > (), yystack_[2].location.begin.line, yystack_[2].location.begin.column);
        std::vector<ExprNode*>* dim = new std::vector<ExprNode*>();
        dim->emplace_back(new LiteralExpr(-1, yystack_[1].location.begin.line, yystack_[1].location.begin.column));
        yylhs.value.as < FE::AST::VarDeclarator* > () = new VarDeclarator(lval, nullptr, yystack_[2].location.begin.line, yystack_[2].location.begin.column);
    }
#line 1445 "frontend/parser/yacc.cpp"
    break;

  case 46: // VAR_DECLARATOR: IDENT LBRACKET RBRACKET ARRAY_DIMENSION_EXPR_LIST
#line 368 "frontend/parser/yacc.y"
                                                        {
        ExprNode* lval = new LiteralExpr(yystack_[3].value.as < std::string > (), yystack_[3].location.begin.line, yystack_[3].location.begin.column);
        std::vector<InitDecl*>* init_list = new std::vector<InitDecl*>();
        for (ExprNode* expr : *yystack_[0].value.as < std::vector<FE::AST::ExprNode*>* > ()) {
            init_list->push_back(new Initializer(expr, expr->line_num, expr->col_num));
        }
        InitDecl* init = new InitializerList(init_list, yystack_[0].location.begin.line, yystack_[0].location.begin.column);
        yylhs.value.as < FE::AST::VarDeclarator* > () = new VarDeclarator(lval, init, yystack_[3].location.begin.line, yystack_[3].location.begin.column);
    }
#line 1459 "frontend/parser/yacc.cpp"
    break;

  case 47: // VAR_DECLARATOR: IDENT LBRACKET RBRACKET ARRAY_DIMENSION_EXPR_LIST ASSIGN INITIALIZER
#line 377 "frontend/parser/yacc.y"
                                                                           {
        ExprNode* lval = new LiteralExpr(yystack_[5].value.as < std::string > (), yystack_[5].location.begin.line, yystack_[5].location.begin.column);
        std::vector<InitDecl*>* init_list = new std::vector<InitDecl*>();
        for (ExprNode* expr : *yystack_[2].value.as < std::vector<FE::AST::ExprNode*>* > ()) {
            init_list->push_back(new Initializer(expr, expr->line_num, expr->col_num));
        }
        InitDecl* init = new InitializerList(init_list, yystack_[2].location.begin.line, yystack_[2].location.begin.column);
        yylhs.value.as < FE::AST::VarDeclarator* > () = new VarDeclarator(lval, init, yystack_[5].location.begin.line, yystack_[5].location.begin.column);
    }
#line 1473 "frontend/parser/yacc.cpp"
    break;

  case 48: // VAR_DECLARATOR_LIST: VAR_DECLARATOR
#line 389 "frontend/parser/yacc.y"
                   {
        yylhs.value.as < std::vector<FE::AST::VarDeclarator*>* > () = new std::vector<VarDeclarator*>();
        yylhs.value.as < std::vector<FE::AST::VarDeclarator*>* > ()->push_back(yystack_[0].value.as < FE::AST::VarDeclarator* > ());
    }
#line 1482 "frontend/parser/yacc.cpp"
    break;

  case 49: // VAR_DECLARATOR_LIST: VAR_DECLARATOR_LIST COMMA VAR_DECLARATOR
#line 393 "frontend/parser/yacc.y"
                                               {
        yylhs.value.as < std::vector<FE::AST::VarDeclarator*>* > () = yystack_[2].value.as < std::vector<FE::AST::VarDeclarator*>* > ();
        yylhs.value.as < std::vector<FE::AST::VarDeclarator*>* > ()->push_back(yystack_[0].value.as < FE::AST::VarDeclarator* > ());
    }
#line 1491 "frontend/parser/yacc.cpp"
    break;

  case 50: // INITIALIZER: NOCOMMA_EXPR
#line 401 "frontend/parser/yacc.y"
                 {
        yylhs.value.as < FE::AST::InitDecl* > () = new Initializer(yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1499 "frontend/parser/yacc.cpp"
    break;

  case 51: // INITIALIZER: LBRACE INITIALIZER_LIST RBRACE
#line 404 "frontend/parser/yacc.y"
                                     {
        yylhs.value.as < FE::AST::InitDecl* > () = new InitializerList(yystack_[1].value.as < std::vector<FE::AST::InitDecl*>* > (), yystack_[2].location.begin.line, yystack_[2].location.begin.column);
    }
#line 1507 "frontend/parser/yacc.cpp"
    break;

  case 52: // INITIALIZER_LIST: INITIALIZER
#line 409 "frontend/parser/yacc.y"
                {
        yylhs.value.as < std::vector<FE::AST::InitDecl*>* > () = new std::vector<InitDecl*>();
        yylhs.value.as < std::vector<FE::AST::InitDecl*>* > ()->push_back(yystack_[0].value.as < FE::AST::InitDecl* > ());
    }
#line 1516 "frontend/parser/yacc.cpp"
    break;

  case 53: // INITIALIZER_LIST: INITIALIZER_LIST COMMA INITIALIZER
#line 413 "frontend/parser/yacc.y"
                                         {
        yylhs.value.as < std::vector<FE::AST::InitDecl*>* > () = yystack_[2].value.as < std::vector<FE::AST::InitDecl*>* > ();
        yylhs.value.as < std::vector<FE::AST::InitDecl*>* > ()->push_back(yystack_[0].value.as < FE::AST::InitDecl* > ());
    }
#line 1525 "frontend/parser/yacc.cpp"
    break;

  case 54: // ASSIGN_EXPR: LEFT_VAL_EXPR ASSIGN EXPR
#line 421 "frontend/parser/yacc.y"
                              {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::ASSIGN, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1533 "frontend/parser/yacc.cpp"
    break;

  case 55: // EXPR_LIST: NOCOMMA_EXPR
#line 427 "frontend/parser/yacc.y"
                 {
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > () = new std::vector<ExprNode*>();
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > ()->push_back(yystack_[0].value.as < FE::AST::ExprNode* > ());
    }
#line 1542 "frontend/parser/yacc.cpp"
    break;

  case 56: // EXPR_LIST: EXPR_LIST COMMA NOCOMMA_EXPR
#line 431 "frontend/parser/yacc.y"
                                   {
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > () = yystack_[2].value.as < std::vector<FE::AST::ExprNode*>* > ();
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > ()->push_back(yystack_[0].value.as < FE::AST::ExprNode* > ());
    }
#line 1551 "frontend/parser/yacc.cpp"
    break;

  case 57: // EXPR: NOCOMMA_EXPR
#line 438 "frontend/parser/yacc.y"
                 {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1559 "frontend/parser/yacc.cpp"
    break;

  case 58: // EXPR: EXPR COMMA NOCOMMA_EXPR
#line 441 "frontend/parser/yacc.y"
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
#line 1576 "frontend/parser/yacc.cpp"
    break;

  case 59: // NOCOMMA_EXPR: LOGICAL_OR_EXPR
#line 456 "frontend/parser/yacc.y"
                    {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1584 "frontend/parser/yacc.cpp"
    break;

  case 60: // NOCOMMA_EXPR: ASSIGN_EXPR
#line 459 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1592 "frontend/parser/yacc.cpp"
    break;

  case 61: // LOGICAL_OR_EXPR: LOGICAL_AND_EXPR
#line 466 "frontend/parser/yacc.y"
                     {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1600 "frontend/parser/yacc.cpp"
    break;

  case 62: // LOGICAL_OR_EXPR: LOGICAL_OR_EXPR OR LOGICAL_AND_EXPR
#line 469 "frontend/parser/yacc.y"
                                          {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::OR, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1608 "frontend/parser/yacc.cpp"
    break;

  case 63: // LOGICAL_AND_EXPR: EQUALITY_EXPR
#line 475 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1616 "frontend/parser/yacc.cpp"
    break;

  case 64: // LOGICAL_AND_EXPR: LOGICAL_AND_EXPR AND EQUALITY_EXPR
#line 478 "frontend/parser/yacc.y"
                                         {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::AND, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1624 "frontend/parser/yacc.cpp"
    break;

  case 65: // EQUALITY_EXPR: RELATIONAL_EXPR
#line 484 "frontend/parser/yacc.y"
                    {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1632 "frontend/parser/yacc.cpp"
    break;

  case 66: // EQUALITY_EXPR: EQUALITY_EXPR EQ RELATIONAL_EXPR
#line 487 "frontend/parser/yacc.y"
                                       {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::EQ, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1640 "frontend/parser/yacc.cpp"
    break;

  case 67: // EQUALITY_EXPR: EQUALITY_EXPR NE RELATIONAL_EXPR
#line 490 "frontend/parser/yacc.y"
                                       {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::NEQ, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1648 "frontend/parser/yacc.cpp"
    break;

  case 68: // RELATIONAL_EXPR: ADDSUB_EXPR
#line 496 "frontend/parser/yacc.y"
                {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1656 "frontend/parser/yacc.cpp"
    break;

  case 69: // RELATIONAL_EXPR: RELATIONAL_EXPR LT ADDSUB_EXPR
#line 499 "frontend/parser/yacc.y"
                                     {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::LT, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1664 "frontend/parser/yacc.cpp"
    break;

  case 70: // RELATIONAL_EXPR: RELATIONAL_EXPR LE ADDSUB_EXPR
#line 502 "frontend/parser/yacc.y"
                                     {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::LE, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1672 "frontend/parser/yacc.cpp"
    break;

  case 71: // RELATIONAL_EXPR: RELATIONAL_EXPR GT ADDSUB_EXPR
#line 505 "frontend/parser/yacc.y"
                                     {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::GT, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1680 "frontend/parser/yacc.cpp"
    break;

  case 72: // RELATIONAL_EXPR: RELATIONAL_EXPR GE ADDSUB_EXPR
#line 508 "frontend/parser/yacc.y"
                                     {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::GE, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1688 "frontend/parser/yacc.cpp"
    break;

  case 73: // ADDSUB_EXPR: MULDIV_EXPR
#line 514 "frontend/parser/yacc.y"
                {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1696 "frontend/parser/yacc.cpp"
    break;

  case 74: // ADDSUB_EXPR: ADDSUB_EXPR PLUS MULDIV_EXPR
#line 517 "frontend/parser/yacc.y"
                                   {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::ADD, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1704 "frontend/parser/yacc.cpp"
    break;

  case 75: // ADDSUB_EXPR: ADDSUB_EXPR MINUS MULDIV_EXPR
#line 520 "frontend/parser/yacc.y"
                                    {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::SUB, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1712 "frontend/parser/yacc.cpp"
    break;

  case 76: // MULDIV_EXPR: UNARY_EXPR
#line 526 "frontend/parser/yacc.y"
               {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1720 "frontend/parser/yacc.cpp"
    break;

  case 77: // MULDIV_EXPR: MULDIV_EXPR STAR UNARY_EXPR
#line 529 "frontend/parser/yacc.y"
                                  {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::MUL, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1728 "frontend/parser/yacc.cpp"
    break;

  case 78: // MULDIV_EXPR: MULDIV_EXPR SLASH UNARY_EXPR
#line 532 "frontend/parser/yacc.y"
                                   {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::DIV, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1736 "frontend/parser/yacc.cpp"
    break;

  case 79: // MULDIV_EXPR: MULDIV_EXPR MOD UNARY_EXPR
#line 535 "frontend/parser/yacc.y"
                                 {
        yylhs.value.as < FE::AST::ExprNode* > () = new BinaryExpr(Operator::MOD, yystack_[2].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1744 "frontend/parser/yacc.cpp"
    break;

  case 80: // UNARY_EXPR: BASIC_EXPR
#line 540 "frontend/parser/yacc.y"
               {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1752 "frontend/parser/yacc.cpp"
    break;

  case 81: // UNARY_EXPR: UNARY_OP UNARY_EXPR
#line 543 "frontend/parser/yacc.y"
                          {
        yylhs.value.as < FE::AST::ExprNode* > () = new UnaryExpr(yystack_[1].value.as < FE::AST::Operator > (), yystack_[0].value.as < FE::AST::ExprNode* > (), yystack_[0].value.as < FE::AST::ExprNode* > ()->line_num, yystack_[0].value.as < FE::AST::ExprNode* > ()->col_num);
    }
#line 1760 "frontend/parser/yacc.cpp"
    break;

  case 82: // BASIC_EXPR: LITERAL_EXPR
#line 549 "frontend/parser/yacc.y"
                 {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1768 "frontend/parser/yacc.cpp"
    break;

  case 83: // BASIC_EXPR: LEFT_VAL_EXPR
#line 552 "frontend/parser/yacc.y"
                    {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1776 "frontend/parser/yacc.cpp"
    break;

  case 84: // BASIC_EXPR: LPAREN EXPR RPAREN
#line 555 "frontend/parser/yacc.y"
                         {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[1].value.as < FE::AST::ExprNode* > ();
    }
#line 1784 "frontend/parser/yacc.cpp"
    break;

  case 85: // BASIC_EXPR: FUNC_CALL_EXPR
#line 558 "frontend/parser/yacc.y"
                     {
        yylhs.value.as < FE::AST::ExprNode* > () = yystack_[0].value.as < FE::AST::ExprNode* > ();
    }
#line 1792 "frontend/parser/yacc.cpp"
    break;

  case 86: // FUNC_CALL_EXPR: IDENT LPAREN RPAREN
#line 564 "frontend/parser/yacc.y"
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
#line 1812 "frontend/parser/yacc.cpp"
    break;

  case 87: // FUNC_CALL_EXPR: IDENT LPAREN EXPR_LIST RPAREN
#line 579 "frontend/parser/yacc.y"
                                    {
        Entry* entry = Entry::getEntry(yystack_[3].value.as < std::string > ());
        yylhs.value.as < FE::AST::ExprNode* > () = new CallExpr(entry, yystack_[1].value.as < std::vector<FE::AST::ExprNode*>* > (), yystack_[3].location.begin.line, yystack_[3].location.begin.column);
    }
#line 1821 "frontend/parser/yacc.cpp"
    break;

  case 88: // ARRAY_DIMENSION_EXPR_LIST: LBRACKET NOCOMMA_EXPR RBRACKET
#line 587 "frontend/parser/yacc.y"
                                   {
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > () = new std::vector<ExprNode*>();
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > ()->push_back(yystack_[1].value.as < FE::AST::ExprNode* > ());
    }
#line 1830 "frontend/parser/yacc.cpp"
    break;

  case 89: // ARRAY_DIMENSION_EXPR_LIST: ARRAY_DIMENSION_EXPR_LIST LBRACKET NOCOMMA_EXPR RBRACKET
#line 591 "frontend/parser/yacc.y"
                                                               {
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > () = yystack_[3].value.as < std::vector<FE::AST::ExprNode*>* > ();
        yylhs.value.as < std::vector<FE::AST::ExprNode*>* > ()->push_back(yystack_[1].value.as < FE::AST::ExprNode* > ());
    }
#line 1839 "frontend/parser/yacc.cpp"
    break;

  case 90: // LEFT_VAL_EXPR: IDENT
#line 598 "frontend/parser/yacc.y"
          {
        Entry* entry = Entry::getEntry(yystack_[0].value.as < std::string > ());
        yylhs.value.as < FE::AST::ExprNode* > () = new LeftValExpr(entry, nullptr, yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1848 "frontend/parser/yacc.cpp"
    break;

  case 91: // LEFT_VAL_EXPR: IDENT ARRAY_DIMENSION_EXPR_LIST
#line 602 "frontend/parser/yacc.y"
                                      {
        Entry* entry = Entry::getEntry(yystack_[1].value.as < std::string > ());
        yylhs.value.as < FE::AST::ExprNode* > () = new LeftValExpr(entry, yystack_[0].value.as < std::vector<FE::AST::ExprNode*>* > (), yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1857 "frontend/parser/yacc.cpp"
    break;

  case 92: // LITERAL_EXPR: INT_CONST
#line 609 "frontend/parser/yacc.y"
              {
        yylhs.value.as < FE::AST::ExprNode* > () = new LiteralExpr(yystack_[0].value.as < int > (), yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1865 "frontend/parser/yacc.cpp"
    break;

  case 93: // LITERAL_EXPR: FLOAT_CONST
#line 612 "frontend/parser/yacc.y"
                  {
        yylhs.value.as < FE::AST::ExprNode* > () = new LiteralExpr(yystack_[0].value.as < float > (), yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1873 "frontend/parser/yacc.cpp"
    break;

  case 94: // LITERAL_EXPR: LL_CONST
#line 616 "frontend/parser/yacc.y"
               {
        yylhs.value.as < FE::AST::ExprNode* > () = new LiteralExpr(yystack_[0].value.as < long long > (), yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1881 "frontend/parser/yacc.cpp"
    break;

  case 95: // LITERAL_EXPR: STR_CONST
#line 619 "frontend/parser/yacc.y"
                {
        yylhs.value.as < FE::AST::ExprNode* > () = new LiteralExpr(yystack_[0].value.as < std::string > (), yystack_[0].location.begin.line, yystack_[0].location.begin.column);
    }
#line 1889 "frontend/parser/yacc.cpp"
    break;

  case 96: // TYPE: INT
#line 627 "frontend/parser/yacc.y"
        {
        yylhs.value.as < FE::AST::Type* > () = TypeFactory::getBasicType(Type_t::INT);
    }
#line 1897 "frontend/parser/yacc.cpp"
    break;

  case 97: // TYPE: FLOAT
#line 630 "frontend/parser/yacc.y"
            {
        yylhs.value.as < FE::AST::Type* > () = TypeFactory::getBasicType(Type_t::FLOAT);
    }
#line 1905 "frontend/parser/yacc.cpp"
    break;

  case 98: // UNARY_OP: PLUS
#line 637 "frontend/parser/yacc.y"
         {
        yylhs.value.as < FE::AST::Operator > () = Operator::ADD;
    }
#line 1913 "frontend/parser/yacc.cpp"
    break;

  case 99: // UNARY_OP: MINUS
#line 640 "frontend/parser/yacc.y"
            {
        yylhs.value.as < FE::AST::Operator > () = Operator::SUB;
    }
#line 1921 "frontend/parser/yacc.cpp"
    break;

  case 100: // UNARY_OP: NOT
#line 643 "frontend/parser/yacc.y"
          {
        yylhs.value.as < FE::AST::Operator > () = Operator::NOT;
    }
#line 1929 "frontend/parser/yacc.cpp"
    break;

  case 101: // BREAK_STMT: BREAK SEMICOLON
#line 649 "frontend/parser/yacc.y"
                    {
        yylhs.value.as < FE::AST::StmtNode* > () = new BreakStmt(yystack_[1].location.begin.line, yystack_[1].location.begin.column);
    }
#line 1937 "frontend/parser/yacc.cpp"
    break;


#line 1941 "frontend/parser/yacc.cpp"

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


  const short  YaccParser ::yypact_ninf_ = -134;

  const signed char  YaccParser ::yytable_ninf_ = -1;

  const short
   YaccParser ::yypact_[] =
  {
     350,  -134,  -134,  -134,  -134,  -134,  -134,  -134,  -134,  -134,
    -134,  -134,   -20,   -35,   -11,    11,    17,    23,   415,    24,
    -134,   431,   166,     2,   350,  -134,  -134,  -134,    27,  -134,
    -134,  -134,  -134,  -134,  -134,  -134,  -134,    53,  -134,    -2,
      49,   110,     4,   106,    15,  -134,  -134,  -134,    71,  -134,
      78,   431,  -134,    69,   431,    39,   431,   398,   431,  -134,
    -134,  -134,    84,    80,    13,  -134,   212,  -134,  -134,  -134,
    -134,  -134,   431,   431,   431,   431,   431,   431,   431,   431,
     431,   431,   431,   431,   431,   431,   431,    22,  -134,    76,
     101,  -134,  -134,  -134,    47,  -134,    68,   431,    48,    93,
      86,    80,    56,  -134,    96,    76,  -134,  -134,  -134,    49,
     110,     4,     4,   106,   106,   106,   106,    15,    15,  -134,
    -134,  -134,  -134,    24,    42,    80,     9,   431,  -134,  -134,
      92,   350,   431,   431,   350,  -134,    57,   121,   120,  -134,
       9,  -134,  -134,  -134,  -134,   136,    91,    95,  -134,    24,
     126,   122,    -7,  -134,   -26,   350,   431,   431,  -134,   258,
    -134,   382,    39,     9,     9,  -134,  -134,    60,    61,  -134,
     304,   120,  -134,  -134,   350,   350,  -134,    39,  -134,  -134
  };

  const signed char
   YaccParser ::yydefact_[] =
  {
       0,    92,    93,    94,    95,    17,    18,    98,    99,   100,
      96,    97,    90,     0,     0,     0,     0,     0,     0,     0,
      12,     0,     0,     0,     2,     4,    11,     6,     0,     7,
       8,     9,    10,    13,    14,    15,    60,     0,    57,    59,
      61,    63,    65,    68,    73,    76,    80,    85,    83,    82,
       0,     0,    16,     0,     0,    91,     0,     0,     0,    19,
     101,    31,     0,     0,     0,    34,     0,     1,     3,     5,
      23,    20,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    90,    48,    21,
      43,    81,    83,    86,     0,    55,     0,     0,     0,     0,
       0,     0,     0,    32,    90,    22,    84,    35,    58,    62,
      64,    67,    66,    70,    72,    69,    71,    74,    75,    77,
      78,    79,    54,    40,     0,     0,     0,     0,    87,    88,
       0,     0,     0,     0,     0,    41,     0,     0,    45,    49,
       0,    44,    50,    56,    89,    29,     0,     0,    33,     0,
       0,    36,    46,    52,     0,     0,     0,     0,    42,     0,
      26,     0,    38,     0,     0,    51,    30,     0,     0,    24,
       0,    37,    47,    53,     0,     0,    25,    39,    27,    28
  };

  const short
   YaccParser ::yypgoto_[] =
  {
    -134,  -134,   -16,   -24,  -134,  -134,   111,  -134,  -134,  -134,
    -134,  -134,  -134,  -134,  -134,    28,  -134,    51,   115,  -133,
    -134,  -134,  -134,   -17,   -44,  -134,   108,   105,    72,    -1,
      63,   -22,  -134,  -134,  -127,    79,  -134,   -14,  -134,  -134
  };

  const unsigned char
   YaccParser ::yydefgoto_[] =
  {
       0,    23,    24,    25,    26,    27,    28,    29,   160,    30,
      31,    32,    33,    34,    35,   135,   136,    88,    89,   141,
     154,    36,    94,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    55,    48,    49,    50,    51,    52
  };

  const unsigned char
   YaccParser ::yytable_[] =
  {
      69,    62,    67,   163,    64,    63,    66,   153,    56,    95,
      96,   152,     1,     2,     3,     4,   164,    77,    78,    79,
      80,    73,   165,    53,   162,    54,     7,     8,   108,    91,
     172,   173,    57,     9,    83,    84,    85,    12,    97,    98,
     100,   102,    69,   101,   177,     1,     2,     3,     4,    10,
      11,    68,    21,   130,    58,    72,   140,   106,    59,     7,
       8,   119,   120,   121,    60,   123,     9,   124,    70,   122,
      12,    74,     1,     2,     3,     4,   113,   114,   115,   116,
      96,    86,   142,   143,    97,    21,     7,     8,   138,   127,
      72,   128,   131,     9,    71,    72,   142,    12,    72,   149,
     134,   150,    72,    72,   174,   175,    87,   145,   104,   137,
     148,   126,    21,    93,   129,   146,   147,    96,   125,   142,
     142,    75,    76,    81,    82,   103,    72,   133,    72,    90,
      92,   166,   156,    72,   132,   137,   157,    72,   144,   167,
     168,   124,    90,   170,   117,   118,    69,   111,   112,   151,
     178,   179,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    54,   155,   161,    99,     1,
       2,     3,     4,   159,     5,     6,   139,   158,   105,   110,
      90,   109,     0,     7,     8,     0,     0,     0,     0,     0,
       9,    10,    11,     0,    12,    13,     0,    14,    15,    16,
      17,     0,     0,     0,    90,    18,    19,    20,     0,    21,
       0,     0,     0,    22,    65,     1,     2,     3,     4,     0,
       5,     6,     0,     0,     0,     0,     0,     0,     0,     7,
       8,     0,     0,     0,     0,     0,     9,    10,    11,     0,
      12,    13,     0,    14,    15,    16,    17,     0,     0,     0,
       0,    18,    19,    20,     0,    21,     0,     0,     0,    22,
     107,     1,     2,     3,     4,     0,     5,     6,     0,     0,
       0,     0,     0,     0,     0,     7,     8,     0,     0,     0,
       0,     0,     9,    10,    11,     0,    12,    13,     0,    14,
      15,    16,    17,     0,     0,     0,     0,    18,    19,    20,
       0,    21,     0,     0,     0,    22,   169,     1,     2,     3,
       4,     0,     5,     6,     0,     0,     0,     0,     0,     0,
       0,     7,     8,     0,     0,     0,     0,     0,     9,    10,
      11,     0,    12,    13,     0,    14,    15,    16,    17,     0,
       0,     0,     0,    18,    19,    20,     0,    21,     0,     0,
       0,    22,   176,     1,     2,     3,     4,     0,     5,     6,
       0,     0,     0,     0,     0,     0,     0,     7,     8,     0,
       0,     0,     0,     0,     9,    10,    11,     0,    12,    13,
       0,    14,    15,    16,    17,     1,     2,     3,     4,    18,
      19,    20,     0,    21,     0,     0,     0,    22,     0,     7,
       8,     1,     2,     3,     4,     0,     9,     0,     0,     0,
      12,     0,     0,     0,     0,     7,     8,     0,     1,     2,
       3,     4,     9,    10,    11,    21,    12,     0,   171,     0,
       0,     0,     7,     8,     1,     2,     3,     4,    19,     9,
       0,    21,     0,    12,     0,     0,     0,     0,     7,     8,
       0,     0,     0,     0,     0,     9,    61,     0,    21,    12,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    21
  };

  const short
   YaccParser ::yycheck_[] =
  {
      24,    18,     0,    10,    21,    19,    22,   140,    43,    53,
      54,   138,     3,     4,     5,     6,    42,    13,    14,    15,
      16,    23,    48,    43,   151,    45,    17,    18,    72,    51,
     163,   164,    43,    24,    19,    20,    21,    28,    45,    56,
      57,    58,    66,    57,   171,     3,     4,     5,     6,    25,
      26,    49,    43,    97,    43,    42,    47,    44,    41,    17,
      18,    83,    84,    85,    41,    43,    24,    45,    41,    86,
      28,    22,     3,     4,     5,     6,    77,    78,    79,    80,
     124,    10,   126,   127,    45,    43,    17,    18,    46,    42,
      42,    44,    44,    24,    41,    42,   140,    28,    42,    42,
      44,    44,    42,    42,    44,    44,    28,   131,    28,   123,
     134,    10,    43,    44,    46,   132,   133,   161,    42,   163,
     164,    11,    12,    17,    18,    41,    42,    41,    42,    50,
      51,   155,    41,    42,    41,   149,    41,    42,    46,   156,
     157,    45,    63,   159,    81,    82,   170,    75,    76,    28,
     174,   175,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    45,    30,    45,    57,     3,
       4,     5,     6,    47,     8,     9,   125,   149,    63,    74,
     101,    73,    -1,    17,    18,    -1,    -1,    -1,    -1,    -1,
      24,    25,    26,    -1,    28,    29,    -1,    31,    32,    33,
      34,    -1,    -1,    -1,   125,    39,    40,    41,    -1,    43,
      -1,    -1,    -1,    47,    48,     3,     4,     5,     6,    -1,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,
      18,    -1,    -1,    -1,    -1,    -1,    24,    25,    26,    -1,
      28,    29,    -1,    31,    32,    33,    34,    -1,    -1,    -1,
      -1,    39,    40,    41,    -1,    43,    -1,    -1,    -1,    47,
      48,     3,     4,     5,     6,    -1,     8,     9,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    17,    18,    -1,    -1,    -1,
      -1,    -1,    24,    25,    26,    -1,    28,    29,    -1,    31,
      32,    33,    34,    -1,    -1,    -1,    -1,    39,    40,    41,
      -1,    43,    -1,    -1,    -1,    47,    48,     3,     4,     5,
       6,    -1,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    17,    18,    -1,    -1,    -1,    -1,    -1,    24,    25,
      26,    -1,    28,    29,    -1,    31,    32,    33,    34,    -1,
      -1,    -1,    -1,    39,    40,    41,    -1,    43,    -1,    -1,
      -1,    47,    48,     3,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,    18,    -1,
      -1,    -1,    -1,    -1,    24,    25,    26,    -1,    28,    29,
      -1,    31,    32,    33,    34,     3,     4,     5,     6,    39,
      40,    41,    -1,    43,    -1,    -1,    -1,    47,    -1,    17,
      18,     3,     4,     5,     6,    -1,    24,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    17,    18,    -1,     3,     4,
       5,     6,    24,    25,    26,    43,    28,    -1,    46,    -1,
      -1,    -1,    17,    18,     3,     4,     5,     6,    40,    24,
      -1,    43,    -1,    28,    -1,    -1,    -1,    -1,    17,    18,
      -1,    -1,    -1,    -1,    -1,    24,    41,    -1,    43,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43
  };

  const signed char
   YaccParser ::yystos_[] =
  {
       0,     3,     4,     5,     6,     8,     9,    17,    18,    24,
      25,    26,    28,    29,    31,    32,    33,    34,    39,    40,
      41,    43,    47,    52,    53,    54,    55,    56,    57,    58,
      60,    61,    62,    63,    64,    65,    72,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    86,    87,
      88,    89,    90,    43,    45,    85,    43,    43,    43,    41,
      41,    41,    74,    88,    74,    48,    53,     0,    49,    54,
      41,    41,    42,    23,    22,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    10,    28,    68,    69,
      86,    82,    86,    44,    73,    75,    75,    45,    74,    57,
      74,    88,    74,    41,    28,    69,    44,    48,    75,    77,
      78,    79,    79,    80,    80,    80,    80,    81,    81,    82,
      82,    82,    74,    43,    45,    42,    10,    42,    44,    46,
      75,    44,    41,    41,    44,    66,    67,    88,    46,    68,
      47,    70,    75,    75,    46,    54,    74,    74,    54,    42,
      44,    28,    85,    70,    71,    30,    41,    41,    66,    47,
      59,    45,    85,    10,    42,    48,    54,    74,    74,    48,
      53,    46,    70,    70,    44,    44,    48,    85,    54,    54
  };

  const signed char
   YaccParser ::yyr1_[] =
  {
       0,    51,    52,    52,    53,    53,    54,    54,    54,    54,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    55,
      56,    57,    57,    58,    59,    59,    60,    61,    61,    62,
      62,    63,    63,    64,    65,    65,    66,    66,    66,    66,
      67,    67,    67,    68,    68,    68,    68,    68,    69,    69,
      70,    70,    71,    71,    72,    73,    73,    74,    74,    75,
      75,    76,    76,    77,    77,    78,    78,    78,    79,    79,
      79,    79,    79,    80,    80,    80,    81,    81,    81,    81,
      82,    82,    83,    83,    83,    83,    84,    84,    85,    85,
      86,    86,    87,    87,    87,    87,    88,    88,    89,    89,
      89,    90
  };

  const signed char
   YaccParser ::yyr2_[] =
  {
       0,     2,     1,     2,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     2,     3,     2,     2,     3,     6,     9,     9,     5,
       7,     2,     3,     5,     2,     3,     2,     4,     3,     5,
       0,     1,     3,     1,     3,     3,     4,     6,     1,     3,
       1,     3,     1,     3,     3,     1,     3,     1,     3,     1,
       1,     1,     3,     1,     3,     1,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     3,     1,     3,     3,     3,
       1,     2,     1,     1,     3,     1,     3,     4,     3,     4,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2
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
  "LEFT_VAL_EXPR", "LITERAL_EXPR", "TYPE", "UNARY_OP", "BREAK_STMT", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
   YaccParser ::yyrline_[] =
  {
       0,   139,   139,   143,   149,   153,   161,   164,   167,   170,
     173,   176,   179,   182,   185,   188,   191,   194,   198,   205,
     211,   217,   220,   227,   233,   236,   252,   259,   263,   271,
     274,   282,   285,   291,   297,   300,   315,   319,   326,   331,
     340,   344,   348,   356,   359,   362,   368,   377,   389,   393,
     401,   404,   409,   413,   421,   427,   431,   438,   441,   456,
     459,   466,   469,   475,   478,   484,   487,   490,   496,   499,
     502,   505,   508,   514,   517,   520,   526,   529,   532,   535,
     540,   543,   549,   552,   555,   558,   564,   579,   587,   591,
     598,   602,   609,   612,   616,   619,   627,   630,   637,   640,
     643,   649
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
#line 2597 "frontend/parser/yacc.cpp"

#line 654 "frontend/parser/yacc.y"


void FE::YaccParser::error(const FE::location& location, const std::string& message)
{
    std::cerr << "msg: " << message << ", error happened at: " << location << std::endl;
}
