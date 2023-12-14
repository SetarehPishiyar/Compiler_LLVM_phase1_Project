
#ifndef AST_H
#define AST_H

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

class AST; // Abstract Syntax Tree
class Goal; // Top-level program
class Statement; // Top-level statement
class Expr; // Binary operation of numbers and identifiers
class Equation; // Assignment statement like a = 3;
class Define; // Declaration statement like int a;
class IfState;
class ElifState;
class ElseState;
class Condition;
class LoopcState;
class Conditions;
class Final;

class ASTVisitor
{
public:
    // Virtual visit functions for each AST node type
    virtual void visit(AST &) {}
    virtual void visit(Expr &) {}
    virtual void visit(Goal &) = 0;
    virtual void visit(Statement &) = 0;
    virtual void visit(Define &) = 0;
    virtual void visit(Equation &) = 0;
    virtual void visit(IfState &) = 0;
    virtual void visit(ElifState &) = 0;
    virtual void visit(ElseState &) = 0;
    virtual void visit(Condition &) = 0;
    virtual void visit(LoopcState &) = 0;
    virtual void visit(Conditions &) = 0;
    virtual void visit(Final &) = 0;
};

class AST
{
public:
    virtual ~AST() {}
    virtual void accept(ASTVisitor &V) = 0;
};

class Goal : public AST
{
private:
    llvm::SmallVector<Statement *> statements; // Stores the list of expressions

public:
    Goal(llvm::SmallVector<Statement *> Statements) : statements(Statements) {}
    llvm::SmallVector<Statement *> getStatements() { return statements; }

    llvm::SmallVector<Statement *>::const_iterator begin() { return statements.begin(); }

    llvm::SmallVector<Statement *>::const_iterator end() { return statements.end(); }
    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

class Statement : public AST
{
public:
    enum statementType
    {
        Define,
        Equation,
        IfState,
        LoopcState
    };

private:
    statementType Type;

public:
    statementType getKind()
    {
        return Type;
    }


    Statement(statementType type) : Type(type) {}
    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

class IfState : public Statement
{

private:
    Conditions *condition;
    llvm::SmallVector<Equation *> Equations;
    llvm::SmallVector<ElifState *> Elifs;
    ElseState *Else;

public:
    IfState(Conditions *condition, llvm::SmallVector<Equation *> Equations,llvm::SmallVector<ElifState *> Elifs,ElseState *Else) : 
    condition(condition), Equations(Equations), Statement(Statement::statementType::IfState), Elifs(Elifs), Else(Else) {}
    IfState(): Statement(Statement::statementType::IfState) {}

    Conditions *getCondition()
    {
        return condition;
    }

    llvm::SmallVector<Equation *> getEquation()
    {
        return Equations;
    }

    llvm::SmallVector<ElifState *> getElifs()
    {
        return Elifs;
    }

    ElseState *getElse()
    {
        return Else;
    }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

class ElifState : public IfState
{
    
private:
    Conditions *condition;
    llvm::SmallVector<Equation *> Equations;

public:
    ElifState(Conditions *condition, llvm::SmallVector<Equation *> Equations) :
     condition(condition), Equation(Equations), IfState() {}

    Conditions *getCondition()
    {
        return condition;
    }

    llvm::SmallVector<Equation *> getEquation()
    {
        return Equations;
    }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

class ElseState : public IfState
{

private:
    llvm::SmallVector<Equation *> Equations;

public:
    ElseState(llvm::SmallVector<Equation *> Equations) : Equations(Equations), IfState() {}

    llvm::SmallVector<Equation *> getEquation()
    {
        return Equations;
    }


    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

class LoopcState : public Statement
{

private:
    Conditions *condition;
    llvm::SmallVector<Equation *> Equations;

public:
    LoopcState(Conditions *condition, llvm::SmallVector<Equation * > Equations) : 
    condition(condition), Equations(Equations), Statement(Statement::statementType::IfState) {}

    Conditions *getCondition()
    {
        return condition;
    }

    llvm::SmallVector<Equation *> getEquation()
    {
        return Equations;
    }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

class Define : public Statement
{
private:
    llvm::SmallVector<llvm::StringRef, 8> vars;
    llvm::SmallVector<Expr *> exprs;

public:
    Define(llvm::SmallVector<llvm::StringRef, 8> vars, llvm::SmallVector<Expr *> exprs) :
        vars(vars), exprs(exprs), Statement(Statement::statementType::Define) {}

    llvm::SmallVector<llvm::StringRef, 8> getVars()
    {
        return vars;
    }

    llvm::SmallVector<Expr *> getExprs()
    {
        return exprs;
    }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

class Equation : public Statement
{
public:
    enum operators
    {
        plusequal,
        minusequal,
        multequal,
        divequal,
        modequal,
        equal
    };

private:
    Final *lvalue;
    operators AssignmentOp;
    Expr *rvalue;

public:
    Equation(Final *lvalue, operators AssignmentOp, Expr *rvalue) :
     lvalue(lvalue), AssignmentOp(AssignmentOp), rvalue(rvalue), Statement(statementType::Equation) {}
    Final *getLValue()
    {
        return lvalue;
    }

    Expr *getRValue()
    {
        return rvalue;
    }

    operators getAssignmentOP()
    {
        return AssignmentOp;
    }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

class Expr : public AST
{
public:
    enum Operator
    {
        plus,
        minus,
        mult,
        divide,
        modulus,
        power
    };

private:
    Expr *Left; // Left-hand side expression
    Expr *Right; // Right-hand side expression
    Operator Op;      // Operator of the binary operation

public:
    Expr(Operator Op, Expr *L, Expr *R) : 
    Op(Op), Left(L), Right(R) {}
    Expr() {}

    Expr *getLeft() { return Left; }

    Expr *getRight() { return Right; }

    Operator getOperator() { return Op; }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

class Conditions : public AST
{
public:
    enum AndOr
    {
        And,
        Or
    };
private:
    Conditions *Left;
    AndOr Sign;
    Conditions *Right;

public:
    Conditions(Conditions *left, AndOr sign, Conditions *right) : 
    Left(left), Sign(sign), Right(right) {}
    Conditions() {}

    Conditions *getLeft() { return Left; }

    AndOr getSign() { return Sign; }

    Conditions *getRight() { return Right; }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
};

class Condition : public Conditions
{
public:
    enum Operator
    {
        smallerequal,
        smaller,
        bigger,
        biggerequal,
        equalequal,
        notequal
    };

private:
    Expr *Left; // Left-hand side expression
    Expr *Right; // Right-hand side expression
    Operator Op;      // Operator of the boolean operation

public:
    Condition(Expr *left, Operator Op, Expr *right) : 
    Left(left), Op(Op), Right(right), Conditions() {}

    Expr *getLeft() { return Left; }

    Operator getSign() { return Op; }

    Expr *getRight() { return Right; }

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
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
  Final(ValueKind Kind, llvm::StringRef Val) : Kind(Kind), Val(Val), Expr() {}

  ValueKind getKind() { return Kind; }

  llvm::StringRef getVal() { return Val; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

#endif