#ifndef LEXER_H
#define LEXER_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"

class Lexer;

class Token {
    friend class Lexer;
    public:
        enum TokenKind : unsigned short {
            eoi,
            equal,
            plusequal,
            minusequal,
            multequal,
            divequal,
            modequal,
            plus,
            minus,
            mult,
            divide,
            modulus,
            power,
            paranleft,
            paranright,
            id,
            colon,
            num,
            semicolon,
            comma,
            KW_int,
            unknown,
            equalequal,
            biggerequal,
            smallerequal,
            smaller,
            bigger,
            notequal,
            KW_if,
            KW_elif,
            KW_else,
            KW_loopc,
            KW_begin,
            KW_end,
            KW_and,
            KW_or
    };
    private:
        TokenKind Kind;
        llvm::StringRef Text;

        public:
        TokenKind getKind() const { return Kind;}
        llvm::StringRef getText() const { return Text;}

        bool is(TokenKind K) const { return Kind == K; }
        bool isOneOf(TokenKind K1, TokenKind K2) const { return is(K1) || is(K2);}
        template <typename... Ts>
        bool isOneOf(TokenKind K1, TokenKind K2, Ts... Ks) const { return is(K1) || isOneOf(K2, Ks...);}    
};

class Lexer
{
  const char *BufferStart;
  const char *BufferPtr;

public:
  Lexer(const llvm::StringRef &Buffer)
  {
    BufferStart = Buffer.begin();
    BufferPtr = BufferStart;
  }

  void next(Token &token);

private:
  void formToken(Token &Result, const char *TokEnd,
                 Token::TokenKind Kind);
};
#endif
