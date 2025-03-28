#pragma once
#include "expr.h"
#include "types.h"
#include "errors.h"
#include "stmt.h"
#include "environment.h"
#include "value.h"

class Interpreter : public ExprVisitor, public StmtVisitor
{
public:
	void interpret(std::vector<std::unique_ptr<Stmt>>& statements) const;

	//	exprs
	void visit(const UnaryExpr& expr) const override;
	void visit(const BinaryExpr& expr) const override;
	void visit(const GroupingExpr& expr) const override;
	void visit(const LiteralExpr& expr) const override;
	void visit(const VariableExpr& expr) const override;
	void visit(const AssignmentExpr& expr) const override;
	void visit(const ArrayPushExpr& expr) const override;
	void visit(const ArrayAccessExpr& expr) const override;
	void visit(const ArraySetExpr& expr) const override;
	void visit(const InputExpr& expr) const override;

	//	stmts
	void visit(const ExpressionStmt& stmt) const override;
	void visit(const PrintStmt& stmt) const override;
	void visit(const ByteStmt& stmt) const override;
	void visit(const ArrayStmt& stmt) const override;

private:
	void execute(const Stmt& stmt) const;
	void evaluate(const Expr& expr) const;
	void checkNumberOperand(Token op, Value operand) const;
	void checkNumberOperands(Token op, Value left, Value right) const;
	void checkAddableOperands(Token op, Value left, Value right) const;
	void checkBoolOperands(Token op, Value left, Value right) const;
	bool areValuesEqual(Value left, Value right) const;
	Value addValues(Value left, Value right) const;

	mutable Environment environment;
	mutable Value currentResult;
};