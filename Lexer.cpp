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
        Name == "type" ? Token::KW_type : (Name == "int" ? Token::KW_int : Token::ident);
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