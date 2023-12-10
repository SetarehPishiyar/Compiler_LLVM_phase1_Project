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
    Final *left;
    operators op;
    Expr *right;

  public:
    Equation(Final *left, operators op, Expr *right):left(left),op(op),right(right) {}

  Expr *getLeft() { return *Left; }
  Expr *getRight() { return *Right; }
  Operators getOperator() { return Op; }
 
  virtual void accept(ASTVisitor &V) override
  {
      V.visit(&this);
  }

};


class IfState : public AST
{
  private:
    Conditions *C;
    llvm::SmallVector<Equation *> equations;
    llvm::SmallVector<ElifState *> elifs;
    ElseState *Else;

  public:
    IfState(Conditions *C, llvm::SmallVector<Equation *> equations, llvm::SmallVector<ElifState *> elifs, ElseState *Else) : C(C),equations(equations),elifs(elifs),Else(Else) {}
    Conditions *getCondition() {return *C;}
    llvm::SmallVector<Condition *> getEquations() {return equations;}
    llvm::SmallVector<ElifState *> getelifs() {return elifs;}
    ElseState *getElse() {return *Else;}

    virtual void accept(ASTVisitor &V) override
    {
      V.visit(&this);
    }
};


class ElifState : public IfState
{
  private:
    Conditions *C;
    llvm::SmallVector<Equation *> equations;

  public:
    ElifState( Conditions *C, llvm::SmallVector<Equation *> equations) : C(C),equations(equations) {}

    llvm::SmallVector<Equation *> getEquations() {return equations;}
    Conditions *getCondition() {return *C;}


    virtual void accept(ASTVisitor &V) override
    {
      V.visit(&this);
    }
};

class ElseState : IfState
{
  private:
    llvm::SmallVector<Equation *> equations;

  public:
    ElseState(llvm::SmallVector<Equation *> equations) : equations(equations) {}

    llvm::SmallVector<Equation *> getequations() {return equations;}

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
  Expr *getLeft() { return *Left; }
  Expr *getRight() { return *Right; }
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
  Condition(Expr left, CompOp op, Expr right) : left(left), op(op), right(right) {}

  Expr *getleft() {return *left;}
  CompOp getop()  {return op;}
  Expr *getright() {return *right;}
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
    Conditions *right;
  
  public:
    Conditions(Condition *left, and_or op, Conditions *right) : left(left), op(op), right(right) {}

    Condition *getleft() {return *left;}
    and_or op() {return op;}
    Conditions *getright() {return *right;}

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

class LoopcState : public AST
{
  private:
    Conditions *C;
    llvm::SmallVector<Equation *> equations;
  
  public:
    LoopcState(Conditions *C, llvm::SmallVector<Equation *> equations) : C(C), equations(equations) {}
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