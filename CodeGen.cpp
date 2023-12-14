#include "CodeGen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;


namespace
{
  class ToIRVisitor : public ASTVisitor
  {
    Module *M;
    IRBuilder<> Builder;
    Type *VoidTy;
    Type *Int32Ty;
    Type *Int8PtrTy;
    Type *Int8PtrPtrTy;
    Constant *Int32Zero;

    Value *V;
    StringMap<AllocaInst *> nameMap;

    llvm::FunctionType* MainFty;
    llvm::FunctionType* MainFn;

  public:
    
    ToIRVisitor(Module *M) : M(M), Builder(M->getContext())
    {

      VoidTy = Type::getVoidTy(M->getContext());
      Int32Ty = Type::getInt32Ty(M->getContext());
      Int8PtrTy = Type::getInt8PtrTy(M->getContext());
      Int8PtrPtrTy = Int8PtrTy->getPointerTo();
      Int32Zero = ConstantInt::get(Int32Ty, 0, true);
    }

    
    void run(AST *Tree)
    {
      
      FunctionType *MainFty = FunctionType::get(Int32Ty, {Int32Ty, Int8PtrPtrTy}, false);
      Function *MainFn = Function::Create(MainFty, GlobalValue::ExternalLinkage, "main", M);

      
      BasicBlock *BB = BasicBlock::Create(M->getContext(), "entry", MainFn);
      Builder.SetInsertPoint(BB);

      
      Tree->accept(*this);

      
      Builder.CreateRet(Int32Zero);
    }

    
    virtual void visit(Goal &Node) override
    {
     
      for (auto I = Node.begin(), E = Node.end(); I != E; ++I)
      {
        (*I)->accept(*this);
      }
    };

    virtual void visit(Statement &Node)
    {
      if(Node.getKind() == Statement::statementType::Equation)
      {
        Equation* eq = (Equation*)&Node;
        eq->accept(*this);
      }
      else if(Node.getKind() == Statement::statementType::Define)
      {
        Define* define = (Define*)&Node;
        define->accept(*this);
      }
      else if(Node.getKind() == Statement::statementType::IfState)
      {
        IfState* ifs = (IfState*)&Node;
        ifs->accept(*this);
      }
      else if(Node.getKind() == Statement::statementType::LoopcState)
      {
        LoopcState* loopc = (LoopcState*)&Node;
        loopc->accept(*this);
      }
    };

    virtual void visit(Equation &Node) override
    {
     
      Node.getRight()->accept(*this);
      Value *val = V;

   
      auto varName = Node.getLeft()->getVal();

      
      Builder.CreateStore(val, nameMap[varName]);

      
      FunctionType *CalcWriteFnTy = FunctionType::get(VoidTy, {Int32Ty}, false);

      
      Function *CalcWriteFn = Function::Create(CalcWriteFnTy, GlobalValue::ExternalLinkage, "main_write", M);

      
      CallInst *Call = Builder.CreateCall(CalcWriteFnTy, CalcWriteFn, {val});
    };

    virtual void visit(Final &Node) override
    {
      if (Node.getKind() == Final::id)
      {
      
        V = Builder.CreateLoad(Int32Ty, nameMap[Node.getVal()]);
      }
      else
      {
        int intval;
        Node.getVal().getAsInteger(10, intval);
        V = ConstantInt::get(Int32Ty, intval, true);
      }
    };

    virtual void visit(Expr &Node) override
    {
      Node.getLeft()->accept(*this);
      Value *Left = V;

      Node.getRight()->accept(*this);
      Value *Right = V;

      switch (Node.getOperator())
      {
      case Expr::Operator::plus:
        V = Builder.CreateNSWAdd(Left, Right);
        break;
      case Expr::Operator::minus:
        V = Builder.CreateNSWSub(Left, Right);
        break;
        case Expr::Operator::mult:
        V = Builder.CreateNSWMul(Left, Right);
        break;
        case Expr::Operator::divide:
        V = Builder.CreateSDiv(Left, Right);
        break;
        case Expr::Operator::modulus:
        {
          Value* div = Builder.CreateSDiv(Left, Right);
          Value* mult = Builder.CreateNSWMul(div, Right);
          V = Builder.CreateNSWSub(Left, mult);
        break;
        }

        case Expr::Operator::power:
        {
          Final* ff = (Final *) Right;
          int RightVal;
          RightVal = ff->getVal().getAsInteger(10, RightVal);
          if(RightVal == 0)
          {
            V = ConstantInt::get(Int32Ty, 1, true);
            break;
          }
          else
          {
            for(int i = 1; i < RightVal; i++)
            {
              Left = Builder.CreateNSWMul(Left, Left);
            }
            V = Left;
            break;
          }
        }
      
      }
    };

    virtual void visit(Condition &Node) override
    {
        Node.getLeft()->accept(*this);
        Value* Left = V;
        Node.getRight()->accept(*this);
        Value* Right = V;
        switch (Node.getSign())
        {
        case Condition::Operator::equalequal:
          V = Builder.CreateICmpEQ(Left, Right);
          break;
        case Condition::Operator::smaller:
          V = Builder.CreateICmpSLT(Left, Right);
          break;
        case Condition::Operator::smallerequal:
          V = Builder.CreateICmpSLE(Left, Right);
          break;
        case Condition::Operator::biggerequal:
          V = Builder.CreateICmpSGE(Left, Right);
          break;
        case Condition::Operator::bigger:
          V = Builder.CreateICmpSGT(Left, Right);
        case Condition::Operator::notequal:
          V = Builder.CreateICmpNE(Left, Right);
          break;
          break;
        default:
          break;
        }

    };

    virtual void visit(BinaryOp &Node) override
    {
     
      Node.getLeft()->accept(*this);
      Value *Left = V;

      
      Node.getRight()->accept(*this);
      Value *Right = V;

     
      switch (Node.getOperator())
      {
      case BinaryOp::Plus:
        V = Builder.CreateNSWAdd(Left, Right);
        break;
      case BinaryOp::Minus:
        V = Builder.CreateNSWSub(Left, Right);
        break;
      case BinaryOp::Mul:
        V = Builder.CreateNSWMul(Left, Right);
        break;
      case BinaryOp::Div:
        V = Builder.CreateSDiv(Left, Right);
        break;
      }
    };

    virtual void visit(Define &Node) override
    {
      llvm::SmallVector<llvm::StringRef, 8> vars = Node.getVars();
      llvm::SmallVector<Expr *> Exprs = Node.getExpr();
      auto expr = Exprs.begin();
      Value *val = nullptr;
      for(auto var = vars.begin(), var_end = vars.end(); var != var_end; ++var)
      {
        llvm::StringRef Var = *var;
        Expr* Expr = *expr;

        if(Expr)
        {
          Expr->accept(*this);
          val = V;
          ++expr;

          nameMap[Var] = Builder.CreateAlloca(Int32Ty);
          if(val != nullptr)
          {
            Builder.CreateStore(val, nameMap[Var]);
          }
        }
        else
        {
          nameMap[Var] = Builder.CreateAlloca(Int32Ty);

          if(val != nullptr)
          {
            Value* zero = ConstantInt::get(Type::getInt32Ty(M->getContext()), 0)
            Builder.CreateStore(zero, nameMap[Var]);
          }
        }
      }
    };

    virtual void visit(Conditions &Node) override
    {
      Node.getLeft()->accept(*this);
      Value* Left = V;
      Node.getRight()->accept(*this);
      Value* Right = V;
        switch (Node.getSign())
      {
      case Conditions::AndOr::And:
          V = Builder.CreateAnd(Left, Right);
          break;
      case Conditions::AndOr::Or:
          V = Builder.CreateOr(Left, Right);
          break;
      }
    };
  };
}; 

void CodeGen::compile(AST *Tree)
{
  
  LLVMContext Ctx;
  Module *M = new Module("calc.expr", Ctx);

  ToIRVisitor ToIR(M);
  ToIR.run(Tree);

  M->print(outs(), nullptr);
}