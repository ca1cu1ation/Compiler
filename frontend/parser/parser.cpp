#include <frontend/parser/parser.h>
#include <debug.h>

namespace FE
{
    using type = YaccParser::symbol_type;
    using kind = YaccParser::symbol_kind;

    void Parser::reportError(const location& loc, const std::string& message) { _parser.error(loc, message); }

    std::vector<Token> Parser::parseTokens_impl()
    {
        std::vector<Token> tokens;
        while (true)
        {
            type token = _scanner.nextToken();
            if (token.kind() == kind::S_END) break;

            Token result;
            result.token_name    = token.name();
            result.line_number   = token.location.begin.line;
            result.column_number = token.location.begin.column - 1;
            result.lexeme        = _scanner.YYText();

            switch (token.kind())
            {
                case kind::S_INT_CONST:
                    result.ival = token.value.as<int>();
                    result.type = Token::TokenType::T_INT;
                    break;
                case kind::S_FLOAT_CONST:
                    result.fval = token.value.as<float>();
                    result.type = Token::TokenType::T_FLOAT;
                    break;
                case kind::S_PLUS:
                    result.type = Token::TokenType::T_PLUS;
                    break;
                case kind::S_MINUS:
                    result.type = Token::TokenType::T_MINUS;
                    break;
                case kind::S_STAR:
                    result.type = Token::TokenType::T_STAR;
                    break;
                case kind::S_DIV:
                    result.type = Token::TokenType::T_DIV;
                    break;
                case kind::S_ASSIGN:
                    result.type = Token::TokenType::T_ASSIGN;
                    break;
                case kind::S_EQ:
                    result.type = Token::TokenType::T_EQ;
                    break;
                case kind::S_LE:
                    result.type = Token::TokenType::T_LE;
                    break;
                case kind::S_LT:
                    result.type = Token::TokenType::T_LT;
                    break;
                case kind::S_GE:
                    result.type = Token::TokenType::T_GE;
                    break;
                case kind::S_GT:
                    result.type = Token::TokenType::T_GT;
                    break;
                case kind::S_IDENT:
                case kind::S_SLASH_COMMENT:
                case kind::S_ERR_TOKEN:
                case kind::S_STR_CONST:
                    result.sval = token.value.as<std::string>();
                    result.type = Token::TokenType::T_STRING;
                    break;
                default: result.type = Token::TokenType::T_NONE; break;
            }

            tokens.push_back(result);
        }

        return tokens;
    }

    AST::Root* Parser::parseAST_impl()
    {
        _parser.parse();
        return ast;
    }
}  // namespace FE
