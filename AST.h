#ifndef AST_H
#define AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

class AST;
class Expr;
class Final;
class BinaryOp;
class typeDecl;

class ASTVisitor
{
public:
  virtual void visit(AST &){};
  virtual void visit(Expr &){};
  virtual void visit(Final &) = 0;
  virtual void visit(BinaryOp &) = 0;
  virtual void visit(typeDecl &) = 0;
};

class AST
{
public:
  virtual ~AST() {}
  virtual void accept(ASTVisitor &V) = 0;
};

class Expr : public AST
{
public:
  Expr() {}
};

class Final : public Expr
{
public:
  enum ValueKind
  {
    id,
    num
  };

private:
  ValueKind Kind;
  llvm::StringRef Val;

public:
  Final(ValueKind Kind, llvm::StringRef Val)
      : Kind(Kind), Val(Val) {}
  ValueKind getKind() { return Kind; }
  llvm::StringRef getVal() { return Val; }
  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class BinaryOp : public Expr
{
public:
  enum Operator
  {
    plus,
    minus,
    mult,
    divide,
    equal,
    plusequal,
    minusequal,
    multequal,
    divequal,
    power,
    modulus
  };

private:
  Expr *Left;
  Expr *Right;
  Operator Op;

public:
  BinaryOp(Operator Op, Expr *L, Expr *R)
      : Op(Op), Left(L), Right(R) {}
  Expr *getLeft() { return Left; }
  Expr *getRight() { return Right; }
  Operator getOperator() { return Op; }
  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class typeDecl : public AST
{
  using VarVector = llvm::SmallVector<llvm::StringRef, 8>;
  VarVector Vars;
  Expr *E;

public:
  typeDecl(llvm::SmallVector<llvm::StringRef, 8> Vars)
      : Vars(Vars) {}
  VarVector::const_iterator begin() { return Vars.begin(); }
  VarVector::const_iterator end() { return Vars.end(); }
  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};
#endif