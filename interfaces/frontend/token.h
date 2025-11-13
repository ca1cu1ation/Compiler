#ifndef __INTERFACES_FRONTEND_TOKEN_H__
#define __INTERFACES_FRONTEND_TOKEN_H__

#include <string>

namespace FE
{
    struct Token
    {
        std::string token_name;     ///< 词法分析中使用的 token 名称
        std::string lexeme;         ///< 该 token 的原始文本内容
        int         line_number;    ///< 该 token 所在的行号
        int         column_number;  ///< 该 token 所在的列号

        enum class TokenType
        {
            T_INT,
            T_LL,
            T_FLOAT,
            T_DOUBLE,
            T_STRING,
            T_PLUS,
            T_MINUS,
            T_STAR,
            T_SLASH,
            T_MOD,
            T_ASSIGN,
            T_NE,
            T_EQ,
            T_LE,
            T_LT,
            T_GE,
            T_GT,
            T_AND,
            T_OR,
            T_BITAND,
            T_BITOR,
            T_NOT,
            T_IDENT,
            T_SLASH_COMMENT,    // 单行注释
            T_MULTI_COMMENT,    // 多行注释
            T_ERR,              // 错误 token
            T_NONE
        } type;
        union
        {
            int       ival;
            long long lval;
            float     fval;
            double    dval;
        };
        std::string sval;
    };
}  // namespace FE

#endif  // __INTERFACES_FRONTEND_TOKEN_H__
