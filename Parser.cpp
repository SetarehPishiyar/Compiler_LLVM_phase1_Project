#include "Parser.h"
#include "Lexer.h"
#include "AST.h"

AST *Parser::parse()
{
  AST *Res = parseGoal();
  expect(Token::eoi);
  return Res;
};

AST *Parser::parseGoal()
 {
   llvm::SmallVector<Statement *> stats;
   while (!Tok.is(Token::eoi))
   {
      switch (Tok.getKind())
      {

      case Token::KW_int:
        Define *left;
        left = parseDefine();
        if (left)
          stats.push_back(left);
        else
          goto _error2; 
        break;

      case Token::id:
        Equation *left;
        left = parseEquation();
        if(left)
          stats.push_back(left);
        else 
          goto _error2;
          break;

      case Token::KW_if:
        IfState *left;
        left = parseIf();
        if(left)
          stats.push_back(left);
        else    
          goto _error2;
          break;

        case Token::KW_loopc:
          LoopcState *left;
          left = parseLoop();
          if(left)
            stats.push_back(left);
          else
            goto _error2;
            break;

        default: 
          goto _error2;
          break;
      }
      advance();
   }
   return new Goal(stats);

   _error2:
      while (Tok.getKind() != Token::eoi)
        advance();
      return nullptr;
 };

 Statement *Parser::parseDefine()
 {

  //Expr *right;
  llvm::SmallVector<llvm::StringRef, 8> Vars;
  llvm::SmallVector<Expr * > Right;
  if(!Tok.is(Token::KW_int))
    goto _error1;
  advance();

  if(expect(Token::id))
    goto _error1;
  Vars.push_back(Tok.getText())
  advance();
  int count = 1;
  while (Tok.is(Token::comma))
  {
    advance();
    if(expect(Token::id))
      goto _error1;
    count++; 
    Vars.push_back(Tok.getText());
    advance();
  }
  if(Tok.is(Token::equal)){
      advance();
      Right.push_back(parseExpr());
      //right = parseExpr();      //badesh advance konim(toye function)
      count--;

      while (Tok.is(Token::comma)&&count>=0)
      {
        advance();
        count--;
        Right.push_back(parseExpr());
        //right = parseExpr();
      }
 }
  if(expect(Token::semicolon))
    goto _error1;

  return new Define(Vars,Right);

      _error1:
    while(Tok.getKind() != Token::eoi)
      advance();
    return nullptr;
  
 };


 Statement *Parser::parseEquation()
 {
    Expr *right;
    Final *left;
    left = (Final *)(parseFinal());

    if (!Tok.isOneOf(Token::equal, Token::plusequal, Token::minusequal,Token::multequal, Token::divequal, Token::modequal))
    {
      error();
      return nullptr;
    };

    Equation::operators op;

    switch (Tok.getKind())
    {
    case Token::equal:
      op = Equation::operators::equal;
      break;
    case Token::plusequal:
      op = Equation::operators::plusequal;
      break;
    case Token::minusequal:
      op = Equation::operators::minusequal;
      break;
    case Token::multequal:
      op = Equation::operators::multequal;
      break;
    case Token::divequal:
      op = Equation::operators::divequal;
      break;
    case Token::modequal:
      op = Equation::operators::modequal;
      break;
  
    default:
      goto _error3;
      break;
    }


    advance();
    right = parseExpr();

    return new Equation(left, op, right);

        _error3:
    while(Tok.getKind() != Token::eoi)
      advance();
    return nullptr;
 };


 Expr *Parser::parseExpr()
 {
  Expr *left = parseTerm();
  while (Tok.isOneOf(Token::plus, Token::minus))
  {
    Expr::Operator Op =  Tok.is(Token::plus) ? Expr::Operator::plus : Expr::Operator::minus;
    advance();
    Expr *right = parseTerm();
    left = new Expr(Op, left, right);
  }

  return left;
 };

Expr *Parser::parseTerm()
{
  Expr *left = parseFactor();
  Expr::Operator Op;
  while (Tok.isOneOf(Token::mult, Token::divide,Token::modulus))
  {
    if(Tok.is(Token::mult))
      Op = Expr::mult;
    else if(Tok.is(Token::divide))
      Op = Expr::divide;
    else if(Tok.is(Token::modulus))
      Op = Expr::modulus;
      advance();
      Expr *right = parseFactor();
      left = new Expr(Op, left, right);
  }
  return left;
};

Expr *Parser::parseFactor()
{
  Expr *left = parseFinal();
  while (Tok.is(Token::power))
  {
    Expr::Operator Op = Expr::Operator::power;
    advance();
    Expr *right = parseFinal();
    left = new Expr(Op, left, right);
  }
  return left;
};


Expr *Parser::parseFinal()
{
  Expr *Res = nullptr;
  switch (Tok.getKind())
    {
    case Token::num:
        Res = new Final(Final::num, Tok.getText());
        advance();
        break;
    case Token::id:
        Res = new Final(Final::id, Tok.getText());
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
        while (!Tok.isOneOf(Token::paranright, Token::mult, Token::plus, Token::minus, Token::divide, Token::eoi, Token::power, Token::modulus))
            advance();
        break;
    }
    return Res;

};

IfState *Parser::parseIf()
{
  llvm::SmallVector<Equation *> equs;
  llvm::SmallVector<ElifState *> elifs;
  ElseState *Else;

  if(expect(Token::KW_if))
    goto _error4;
  
  advance();

  Conditions cons = parseConditions();

  if(expect(Token::colon))
    goto _error4;
  advance();
  if(expect(Token::KW_begin))
    goto _error4;
  advance();

  while (Tok.is(Token::id))
  {
    Equation E = parseEquation();
    equs.push_back(E);
  }
  
  if(expect(Token::KW_end))
    goto _error4;

  advance();
  
  while (Tok.is(Token::KW_elif))
  {
    ElifState E;
    E = parseElif();
    elifs.push_back(E);
  }

  if(Tok.is(Token::KW_else)){
    Else = parseElse();
  }

  return new IfState(cons,equs,elifs,Else);

      _error4:
    while(Tok.getKind() != Token::eoi)
      advance();
    return nullptr;

};

ElifState *Parser::parseElif()
{

  llvm::SmallVector<Equation *> equations;

  if(expect(Token::KW_elif))
    goto _error5;

  advance();

  Conditions cons = parseConditions();

   if(expect(Token::colon))
    goto _error5;
  advance();
  if(expect(Token::KW_begin))
    goto _error5;
  advance();

  while (Tok.is(Token::id))
  {
    Equation E = parseEquation();
    equations.push_back(E);
  } 

  if(expect(Token::KW_end))
    goto _error5;
  
  advance();

  return new ElifState(cons,equations);

      _error5:
    while(Tok.getKind() != Token::eoi)
      advance();
    return nullptr;
  
};

ElseState *Parser::parseElse()
{
   llvm::SmallVector<Equation *> equations;

   if(expect(Token::KW_else))
    goto _error6;

  advance();
 
  if(expect(Token::colon))
    goto _error6;

  advance();

  if(expect(Token::KW_begin))
    goto _error6;

  advance();

  while (Tok.is(Token::id))
  {
    Equation E = parseEquation();
    equations.push_back(E);
  }
  
  if(expect(Token::KW_end))
      goto _error6;

  advance();

  return new ElseState(equations);
      _error6:
    while(Tok.getKind() != Token::eoi)
      advance();
    return nullptr;

  };
  
  LoopcState *Parser::parseLoopc()
  {
    llvm::SmallVector<Equation *> equations;

    if(expect(Token::KW_loopc))
      goto _error7;

    advance();

    Conditions cons = parseConditions();

    if(expect(Token::KW_colon))
      goto _error7;

    advance();

    if(expect(Token::KW_begin))
      goto _error7;

    advance();

    while(Tok.is(Token::id))
    {
      Equation E = parseEquation();
      equations.push_back(E);
    }

    if(expect(Token::KW_end))
      goto _error7;

    advance();
    
    return new LoopcState(cons, equations);

        _error7:
    while(Tok.getKind() != Token::eoi)
      advance();
    return nullptr;

  };

  Condition *Parser::parseCondition()
  {
    Expr *left;
    Expr *right;
    Condition::Operator op;

    left = parseExpr();
    switch (Tok.getKind())
    {
    case Token::smaller:
      op = Condition::Operator::smaller;
      break;
    case Token::bigger:
      op = Condition::Operator::bigger;
      break;
    case Token::smallerequal:
      op = Condition::Operator::smallerequal;
      break;
    case Token::biggerequal:
      op = Condition::Operator::biggerequal;
      break;
    case Token::equalequal:
      op = Condition::Operator::equalequal;
      break;
    default:
      goto _error8;
      break;
    }
    advance();

    right = parseExpr();

    return new Condition(left, op, right);

        _error8:
    while(Tok.getKind() != Token::eoi)
      advance();
    return nullptr;

  };

  Conditions *Parser::parseConditions()
  {
      Condition *left;
      Conditions::AndOr op;
      Conditions *right;

      left = parseCondition();
      switch (Tok.getKind())
      {
      case Token::KW_and:
          op = Conditions::AndOr::And;
        break;
      case Token::KW_or:
          op = Conditions::AndOr::Or;
        break;
      
      default:
          goto _error9;
        break;
      }
      advance();

      right = parseConditions();
      return new Conditions(left, op, right);


          _error9:
    while(Tok.getKind() != Token::eoi)
      advance();
    return nullptr;

  };

