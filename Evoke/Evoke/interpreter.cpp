#include "interpreter.h"
#include "vous.h"
#include <iostream>

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>>& statements) const
{
	try {
		for (const auto& statement : statements)
		{
			execute(*statement);
		}
	}
	catch (RuntimeError& error)
	{
		Vous::runtimeError(error.token, error.message);
	}
}

void Interpreter::execute(const Stmt& stmt) const
{
	stmt.accept(*this);
}

void Interpreter::visit(const UnaryExpr& expr) const
{
	evaluate(*expr.operand);
	Value right = currentResult;

	switch (expr.op.type)
	{
	case (MINUS):
		checkNumberOperand(expr.op, right);
		currentResult = Value(-right.getDouble());
		break;
	case (BANG):
		checkBoolOperands(expr.op, right, right);
		currentResult = Value(!right.getBool());
		break;
	}
}

void Interpreter::visit(const BinaryExpr& expr) const
{
	evaluate(*expr.left);
	Value left = currentResult;

	evaluate(*expr.right);
	Value right = currentResult;

	switch (expr.op.type)
	{
	case MINUS:
		checkNumberOperands(expr.op, left, right);
		currentResult = Value(left.getDouble() - right.getDouble());
		break;
	case SLASH:
		checkNumberOperands(expr.op, left, right);
		if (right.getDouble() == 0)
			throw RuntimeError(expr.op, "Division by zero.");
		currentResult = Value(left.getDouble() / right.getDouble());
		break;
	case STAR:
		checkNumberOperands(expr.op, left, right);
		currentResult = Value(left.getDouble() * right.getDouble());
		break;
	case PERCENT:
		checkNumberOperands(expr.op, left, right);
		if (right.getDouble() == 0)
			throw RuntimeError(expr.op, "Division by zero.");
		currentResult = Value(double(int(left.getDouble()) % int(right.getDouble())));
		break;
	case PLUS:
		checkAddableOperands(expr.op, left, right);
		currentResult = addValues(expr.op, left, right);
		break;
	case GREATER:
		checkNumberOperands(expr.op, left, right);
		currentResult = Value(left.getDouble() > right.getDouble());
		break;
	case GREATER_EQUAL:
		checkNumberOperands(expr.op, left, right);
		currentResult = Value(left.getDouble() >= right.getDouble());
		break;
	case LESS:
		checkNumberOperands(expr.op, left, right);
		currentResult = Value(left.getDouble() < right.getDouble());
		break;
	case LESS_EQUAL:
		checkNumberOperands(expr.op, left, right);
		currentResult = Value(left.getDouble() <= right.getDouble());
		break;
	case BANG_EQUAL:
		currentResult = !areValuesEqual(left, right);
		break;
	case EQUAL_EQUAL:
		currentResult = areValuesEqual(left, right);
		break;
	case AND_AND:
		checkBoolOperands(expr.op, left, right);
		currentResult = left.getBool() && right.getBool();
		break;
	case PIPE_PIPE:
		checkBoolOperands(expr.op, left, right);
		currentResult = left.getBool() || right.getBool();
		break;
	}
}

void Interpreter::visit(const LiteralExpr& expr) const
{
	currentResult = expr.literal.literal;
}

void Interpreter::visit(const VariableExpr& expr) const
{
	currentResult = environment.getVariable(expr.name);
}

void Interpreter::visit(const AssignmentExpr& expr) const
{
	evaluate(*expr.value);
	Value value = currentResult;
	environment.assignVariable(expr.name, value);
	currentResult = value;
}

void Interpreter::visit(const ArrayPushExpr& expr) const
{
	evaluate(*expr.value);
	environment.pushArray(expr.name, currentResult);
}

void Interpreter::visit(const ArrayAccessExpr& expr) const
{
	evaluate(*expr.index);
	checkNumberOperand(expr.name, currentResult);
	int index = static_cast<int>(currentResult.getDouble());
	currentResult = environment.getArrayElement(expr.name, index);
}

void Interpreter::visit(const ArraySetExpr& expr) const
{
	evaluate(*expr.index);
	checkNumberOperand(expr.name, currentResult);
	int index = static_cast<int>(currentResult.getDouble());
	evaluate(*expr.value);
	Value value = currentResult;
	environment.setArrayElement(expr.name, index, value);
}

void Interpreter::visit(const InputExpr& expr) const
{
	std::string input;
	std::getline(std::cin, input);
	currentResult = Value(input);
}

void Interpreter::visit(const GroupingExpr& expr) const
{
	evaluate(*expr.expr);
	currentResult = currentResult;
}

void Interpreter::visit(const ExpressionStmt& stmt) const
{
	evaluate(*stmt.expr);
}

void Interpreter::visit(const PrintStmt& stmt) const
{
	evaluate(*stmt.expr);
	Value value = currentResult;
	std::cout << value.toString() << std::endl;
}

void Interpreter::visit(const ByteStmt& stmt) const
{
	Value value;
	if (stmt.initializer != nullptr)
	{
		evaluate(*stmt.initializer);
		value = currentResult;
	}

	environment.defineVariable(stmt.name.lexeme, value);
}

void Interpreter::visit(const ArrayStmt& stmt) const
{
	environment.defineArray(stmt.name.lexeme);
}

void Interpreter::evaluate(const Expr& expr) const
{
	expr.accept(*this);
}

void Interpreter::checkNumberOperand(Token op, Value operand) const
{
	if (operand.getType() == Type::DOUBLE)
		return;
	throw RuntimeError(op, "Operand must be a number");
}

void Interpreter::checkNumberOperands(Token op, Value left, Value right) const
{
	if (left.getType() == Type::DOUBLE && right.getType() == Type::DOUBLE)
		return;
	throw RuntimeError(op, "Operands must be numbers.");
}

void Interpreter::checkAddableOperands(Token op, Value left, Value right) const
{
	switch (left.getType())
	{
	case Type::DOUBLE:
		return;
		break;
	case Type::STRING:
		return;
		break;
	}

	throw::RuntimeError(op, "Operands must be numbers or strings.");
}

void Interpreter::checkBoolOperands(Token op, Value left, Value right) const
{
	if (left.getType() == Type::BOOLEAN && right.getType() == Type::BOOLEAN)
		return;
	throw RuntimeError(op, "Operands must be booleans.");
}

void Interpreter::checkBangableOperands(Token op, Value left, Value right) const
{

}

bool Interpreter::areValuesEqual(Value left, Value right) const
{
	if (left.getType() != right.getType())
		return false;

	if (left.getType() == Type::DOUBLE)
		return left.getDouble() == right.getDouble();
	if (left.getType() == Type::BOOLEAN)
		return left.getBool() == right.getBool();
	return left.getString() == right.getString();
}

Value Interpreter::addValues(Token op, Value left, Value right) const
{
	if (left.getType() != right.getType())
		throw::RuntimeError(op, "Type mismatch. Types must match.");

	if (left.getType() != right.getType())
		throw::RuntimeError(Token(), "Type mismatch. Types must match.");

	if (left.getType() == Type::DOUBLE)
		return Value(left.getDouble() + right.getDouble());

	return Value(left.getString() + right.getString());
}

