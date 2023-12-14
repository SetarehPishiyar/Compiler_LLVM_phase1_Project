#include "Lexer.h"

namespace charinfo
{
    LLVM_READNONE inline bool isWhitespace(char c){
         return c == ' ' || c == '\t' || c == '\f' || c == '\v' ||
           c == '\r' || c == '\n';
    }
    LLVM_READNONE inline bool isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }
    LLVM_READNONE inline bool isLetter(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
    LLVM_READNONE inline bool isDoubleOp(char c)
    {
        return c == '+' || c == '-' || c == '*' || c == '/' || c=='%';
    }
    LLVM_READNONE inline bool isComparison(char c)
    {
        return c == '!' || c == '=' || c == '<' || c == '>';
    }

}

void Lexer::next(Token &token)
{
  while (*BufferPtr && charinfo::isWhitespace(*BufferPtr))
  {
    ++BufferPtr;
  }
  if (!*BufferPtr)
  {
    token.Kind = Token::eoi;
    return;
  }
  if (charinfo::isLetter(*BufferPtr))
  {
    const char *end = BufferPtr + 1;
    while (charinfo::isLetter(*end))
      ++end;
    llvm::StringRef Name(BufferPtr, end - BufferPtr);
    Token::TokenKind kind;
        if (Name=="int")
          kind = Token::KW_int;
        else if (Name=="if")
          kind = Token::KW_if;
        else if (Name=="elif")
          kind = Token::KW_elif;
        else if (Name=="else")
          kind = Token::KW_else;
        else if (Name=="loopc")
          kind = Token::KW_loopc;
        else if (Name=="begin")
          kind = Token::KW_begin;
        else if (Name=="end")
          kind = Token::KW_end;
        else if (Name=="and")
          kind = Token::KW_and;
        else if (Name=="or")
          kind = Token::KW_or;
        else
          kind = Token::id;
         
    formToken(token, end, kind);
    return;
  }

  else if (charinfo::isDigit(*BufferPtr))
  {
    const char *end = BufferPtr + 1;
    while (charinfo::isDigit(*end))
      ++end;
    formToken(token, end, Token::num);
    return;
  }

  else if (charinfo::isComparison(*BufferPtr)) 
  {
    const char *end = BufferPtr + 2;
    // if (*end == '=')
    //   ++end;
    llvm::StringRef Name(BufferPtr, end - BufferPtr);
    Token::TokenKind kind;
        if(Name == "==")
            kind = Token::equalequal;
        else if(Name == ">=")
            kind = Token::biggerequal;
        else if(Name == "<=")
           kind =  Token::smallerequal;
        else if(Name == "!=")
            kind = Token::notequal;
    formToken(token, end, kind);
    return;
  }
  else if (charinfo::isDoubleOp(*BufferPtr)) 
  {
    const char *end = BufferPtr + 2;
    // if (*end == "=")
    //   ++end;
    llvm::StringRef Name(BufferPtr, end - BufferPtr);
    Token::TokenKind kind;
        if(Name == "+=")
            kind = Token::plusequal;
        else if(Name == "-=")
            kind = Token::minusequal;
        else if(Name == "*=")
            kind = Token::multequal;
        else if(Name == "/=")
            kind = Token::divequal;
         else if(Name == "%=")
            kind = Token::modequal;
    formToken(token, end, kind);
    return;
  }

  else { 
    const char *end = BufferPtr + 1;

    llvm::StringRef Name(BufferPtr, end - BufferPtr);
    Token::TokenKind kind;
        if(Name == ">")
            kind = Token::bigger;
        else if(Name == "<")
            kind = Token::smaller;
        else if(Name == ":")
            kind = Token::colon;
        else if(Name == "+")
            kind = Token::plus;
        else if(Name == "-")
            kind = Token::minus;
        else if(Name == "*")
            kind = Token::mult;
        else if(Name == "/")
            kind = Token::divide;
        else if(Name == "=")
            kind = Token::equal;
        else if(Name == "^")
            kind = Token::power;
        else if(Name == "%")
            kind = Token::modulus;
        else if(Name == "(")
            kind = Token::paranleft;
        else if(Name == ")")
            kind = Token::paranright;
        else if(Name == ";")
            kind = Token::semicolon;
        else if(Name == ",")
            kind = Token::comma;
        else
            kind = Token::unknown;

        formToken(token, end, kind);
        return;
  }
}

void Lexer::formToken(Token &Tok, const char *TokEnd, Token::TokenKind Kind)
{
  Tok.Kind = Kind;
  Tok.Text = llvm::StringRef(BufferPtr, TokEnd - BufferPtr);
  BufferPtr = TokEnd;
}