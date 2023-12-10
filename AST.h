#ifndef AST_H
#define AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

class AST;
class Expr;
class Final;
class BinaryOp;
class Define;
class Equation;
class IfState;
class LoopcState;
class ElifState;
class ElseState;
class Condition;
class Conditions;


class ASTVisitor
{
public:
  virtual void visit(AST &){};
  virtual void visit(Expr &){};
  virtual void visit(Final &) = 0;
  virtual void visit(BinaryOp &) = 0;
  virtual void visit(Define &) = 0;
  virtual void visit(Equation &) = 0;
  virtual void visit(IfState &) = 0;
  virtual void visit(ElifState &) = 0;
  virtual void visit(ElseState &) = 0;
  virtual void visit(LoopcState &) = 0;
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
  Final(ValueKind Kind, llvm::StringRef Val) : Kind(Kind), Val(Val) {}
  ValueKind getKind() { return Kind; }
  llvm::StringRef getVal() { return Val; }
  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class Equation : public AST 
{
  public:
    enum operators 
    {
      equal,
      plusequal,
      minusequal,
      multequal,
      divequal,
      modequal
    }

  private:
    Expr *left;
    operator op;
    Expr *right;

  public:
    Equation(Expr *left, operator op, Expr *right):left(left),op(op),right(right) {};
 
  virtual void accept(ASTVisitor &V) override
  {
      V.visit(&this);
  }

};

class IfState : public AST
{
  private:
    Conditions *left;
    Equation *right;

  public:
    IfState(Conditions *left, Equation *right) : left(left), right(right) {};

    Conditions *getleft() {return left;}
    Equation *getright() {return right;}

    virtual void accept(ASTVisitor &V) override
    {
      V.visit(&this);
    }
};

class ElifState : public IfState
{
  private:
    Conditions *left;
    Equation *right;

  public:
    ElifState(Conditions *left,Equation *right) : left(left), right(right) {};

    Conditions *getleft() {return left;}
    Equation *getright() {return right;}

    virtual void accept(ASTVisitor &V) override
    {
      V.visit(&this);
    }
};

class ElseState : IfState
{
  private:
    Conditions *left;
    Equation *right;

  public:
    ElseState(Conditions *left,Equation *right) : left(left), right(right) {};

    Conditions *getleft() {return *left;}
    Equation *getright() {return *right;}
    virtual void accept(ASTVisitor &V) override
    {
      V.visit(&this);
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
    power,
    modulus
  };

private:
  Expr *Left;
  Expr *Right;
  Operator Op;

public:
  BinaryOp(Operator Op, Expr *L, Expr *R) : Op(Op), Left(L), Right(R) {}
  Expr *getLeft() { return Left; }
  Expr *getRight() { return Right; }
  Operator getOperator() { return Op; }
  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class Condition : public Conditions
{
  public : 
      enum CompOp
      {
        smaller,
        bigger,
        smalleequal,
        biggerequal,
        equalequal,
        equal
      };

  private :
    Expr *left;
    CompOp op;
    Expr *right;

  public :
  Condition(Expr left, CompOp op, Expr right) : left(left), op(op), right(right) {};

  Expr *getleft() {return left;}
  CompOp getop()  {return op;}
  Expr *getright() {return right;}
  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class Conditions : public AST
{
  public:
    enum and_or
    {
      And,
      Or
    };

  private:
    Condition *left;
    and_or op;
    Condition *right;
  
  public:
    Conditions(Condition *left, and_or op, Condition *right) : left(left), op(op), right(right) {};

    Condition *getleft() {return *left;}
    and_or op() {return op;}
    Condition *getright() {return right;}

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

class LoopcState : public AST
{
  private:
    Conditions *left;
    Equation *right;
  
  public:
    LoopcState(Conditions *left, Equation *right) : left(left), right() {};
};


class Define : public AST
{
  using VarVector = llvm::SmallVector<llvm::StringRef, 8>;
  VarVector Vars;
  //Expr *E;
  using ExprVector = llvm::SmallVector<Expr *>;
  ExprVector Exprs;

public:
  Define(llvm::SmallVector<llvm::StringRef, 8> Vars, llvm::SmallVector<Expr *> Exprs): Vars(Vars), Exprs(Exprs) {}

  VarVector::const_iterator begin() { return Vars.begin(); }
  VarVector::const_iterator end() { return Vars.end(); }
  ExprVector::const_iterator begin() {return Exprs.begin();}
  ExprVector::const_iterator end() {return Exprs.end();}
  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

#endif