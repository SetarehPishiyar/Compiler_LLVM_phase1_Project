#include "Lexer.h"

namespace charinfo {
    LLVM_READONE inline bool isWhitespace(char c){
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
    LLVM_READNONE inline bool is isDoubleOp(char c)
    {
        return c == '+' || c == '-' || c == '*' || c == '/';
    }
    LLVM_READNONE inline bool is isComparison(char c)
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
    Token::TokenKind kind =
        if (Name=="int")
          Token::KW_int;
        else if (Name=="if")
          Token::KW_if;
        else if (Name=="elif")
          Token::KW_elif;
        else if (Name=="else")
          Token::KW_else;
        else if (Name=="loopc")
          Token::KW_loopc;
        else if (Name=="begin")
          Token::KW_begin;
        else if (Name=="end")
          Token::KW_end;
        else if (Name=="and")
          Token::KW_and;
        else if (Name=="or")
          Token::KW_or;
         
    formToken(token, end, kind);
    return;
  }

  else if (charinfo::isDigit(*BufferPtr))
  {
    const char *end = BufferPtr + 1;
    while (charinfo::isDigit(*end))
      ++end;
    formToken(token, end, Token::number);
    return;
  }

  else if (charinfo::isComparison(*BufferPtr)) 
  {
    const char *end = BufferPtr + 1;
    if (*end == '=')
      ++end;
    llvm::StringRef Name(BufferPtr, end - BufferPtr);
    Token::TokenKind kind =
        if(Name == '==')
            Token::equalequal;
        else if(Name == '>=')
            Token::biggerequal;
        else if(Name == '<=')
            Token::smallerequal;
        else if(Name == '!=')
            Token::notequal;
    formToken(token, end, kind);
    return;
  }
  else if (charinfo::isDoubleOp(*BufferPtr)) 
  {
    const char *end = BufferPtr + 1;
    if (*end == '=')
      ++end;
    llvm::StringRef Name(BufferPtr, end - BufferPtr);
    Token::TokenKind kind =
        if(Name == '+=')
            Token::plusequal;
        else if(Name == '-=')
            Token::minusequal;
        else if(Name == '*=')
            Token::multequal;
        else if(Name == '/=')
            Token::divequal;
    formToken(token, end, kind);
    return;
  }

  else { 
    switch (*BufferPtr) {
    #define CASE(ch, tok)                     
  case ch:                                
    formToken(token, BufferPtr + 1, tok); 
    break
      CASE('>', Token::bigger);
      CASE('<', Token::smaller);
      CASE('+', Token::plus);
      CASE('-', Token::minus);
      CASE('*', Token::mult);
      CASE('/', Token::divide);
      CASE('=', Token::equal);
      CASE('^', Token::power);
      CASE('%', Token::modulus);
      CASE('(', Token::Token::paranleft);
      CASE(')', Token::Token::paranright);
      CASE(';', Token::Token::semicolon);
      CASE(',', Token::Token::comma);
#undef CASE
    default:
      formToken(token, BufferPtr + 1, Token::unknown);
    }
    return;
  }
}

void Lexer::formToken(Token &Tok, const char *TokEnd, Token::TokenKind Kind)
{
  Tok.Kind = Kind;
  Tok.Text = llvm::StringRef(BufferPtr, TokEnd - BufferPtr);
  BufferPtr = TokEnd;
}