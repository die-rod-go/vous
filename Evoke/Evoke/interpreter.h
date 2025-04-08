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
	Interpreter();
	void interpret(std::vector<std::unique_ptr<Stmt>>& statements) const;

	//	exprs
	void visit(const CallExpr& expr) const override;
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
	void visit(const FunctionStmt& stmt) const override;
	void visit(const PrintStmt& stmt) const override;
	void visit(const ByteStmt& stmt) const override;
	void visit(const ArrayStmt& stmt) const override;
	void visit(const BlockStmt& stmt) const override;
	void visit(const IfStmt& stmt) const override;
	void visit(const WhileStmt& stmt) const override;

	mutable Environment globals;
	void executeBlock(const BlockStmt& stmt, std::unique_ptr<Environment> environment) const;

private:
	void execute(const Stmt& stmt) const;
	void evaluate(const Expr& expr) const;
	void checkNumberOperand(const Token& op, const Value& operand) const;
	void checkNumberOperands(const Token& op, const Value& left, const Value& right) const;
	void checkAddableOperands(const Token& op, const Value& left, const Value& right) const;
	void checkBoolOperands(const Token& op, const Value& left, const Value& right) const;
	bool areValuesEqual(const Value& left, const Value& right) const;
	Value addValues(const Token& op, const Value& left, const Value& right) const;
	bool isTruthy(const Value& value) const;

	mutable std::unique_ptr<Environment> environment;
	mutable Value currentResult;
};