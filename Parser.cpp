#include "Parser.h"

AST *Parser::parse()
{
  AST *Res = parseCalc();
  expect(Token::eoi);
  return Res;
}

AST *Parser::parseClac()
{
  if(Tok.is(Token::KW_int)){
    llvm::SmallVector<llvm::StringRef, 8> Vars;
    advance();

    if (expect(Token::id))
      goto _error;
    Vars.push_back(Tok.getText());

    advance();
    int count=1;
    while (Tok.is(Token::comma))
    {
      advance();
      if (expect(Token::id))
      {
        goto _error;
      }
      count++;
      Vars.push_back(Tok.getText());
      advance();
    }

    if(Tok.is(Token::equal) || Tok.is(Token::plusequal) || Tok.is(Token::minusequal) || Tok.is(Token::multequal) || Tok.is(Token::divequal)){
      Vars.push_back(Tok.getText());
      advance();
      while(count>0){
        if(expect(Token::Expr))
        {
          goto here;
        }
        Vars.push_back(Tok.getText());
        advance();
        count--;
      }
    }
      
    }


    here:
    if(consume(Token::semicolon))
        goto _error;

    return new typeDecl(Vars);    
  }

    if(Tok.is(Token::id))
    {
       Expr *Left = new Final(Final::id, Tok.getText());

    advance();
    switch (Token.getKind())
    {
      case Token::equal:
          BinaryOp::Operator Op = BinaryOp::equal;
          Expr *E;
          advance();
          break;

      case Token::plusequal:
          BinaryOp::Operator Op = BinaryOp::plusequal;
          Expr *E;
          advance();
          break;
          
      case Token::minusequal:
          BinaryOp::Operator Op = BinaryOp::minusequal;
          Expr *E;
          advance();
          break;

      case Token::multequal:
          BinaryOp::Operator Op = BinaryOp::multequal;
          Expr *E;
          advance();
          break;

      case Token::divequal:
          BinaryOp::Operator Op = BinaryOp::divequal;
          Expr *E;
          advance();
          break;
      default : goto _error;
    }


    E = parseExpr();

    if (expect(Token::eoi))
      goto _error;

    Left = new BinaryOp(Op, Left, E);
    return Left;
  }
    
    Expr *Parser::parseExpr()
    {
        Expr *Left = parseTerm();
        while (Tok.isOneOf(Token::plus, Token::minus)){
          BinaryOp::Operator Op = Tok.is(Token::plus)? BinaryOp::plus: BinaryOp::minus;
          advance();
          Expr *Right = parseTerm();
          Left = new BinaryOp(Op, Left, Right);
        }
        return Left;
    }
    Expr *Parser::parseTerm()
   {
      Expr *Left = parseFactor();
      while (Tok.isOneOf(Token::mult, Token::divide, Token::modulus))
      {
          BinaryOp::Operator Op =
          Tok.is(Token::mult) ? BinaryOp::mult : Tok.is(Token::divide) ? BinaryOp::divide :BinaryOp::modulus;
          advance();
          Expr *Right = parseFactor();
          Left = new BinaryOp(Op, Left, Right);
      }
  return Left;
    }

 Expr *Parser::parseFactor()
   {
      Expr *Left = parseFinal();
      while (Tok.is(Token::power))
      {
          BinaryOp::Operator Op = BinaryOp::power;
          advance();
          Expr *Right = parseFinal();
          Left = new BinaryOp(Op, Left, Right);
      }
  return Left;
  }

  Expr *Parser::parseFinal()
   {
  Expr *Res = nullptr;
  switch (Tok.getKind())
  {
  case Token::num:
    Res = new Factor(Factor::num, Tok.getText());
    advance();
    break;
  case Token::id:
    Res = new Factor(Factor::id, Tok.getText());
    advance();
    break;
  case Token::paranleft:
    advance();
    Res = parseExpr();
    if (!consume(Token::paranright))
      break;
  default:
    if (!Res)
      error();
    while (!Tok.isOneOf(Token::paranright, Token::mult, Token::plus, Token::minus, Token::divide, Token::equal, Token::eoi, Token::plusequal, Token::minusequal, Token::multequal, Token::divequal, Token::modulus, Token::power))
      advance();
  }
  return Res;
  }

