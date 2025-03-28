#include "interpreter.h"
#include "pulse.h"
#include <iostream>

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>>& statements) const
{
	try {
		for (const auto& statement : statements)
		{
			execute(*statement, false);
		}
	}
	catch (RuntimeError& error)
	{
		Pulse::runtimeError(error.token, error.message);
	}
}

void Interpreter::execute(const Stmt& stmt, bool evoked) const
{
	stmt.accept(*this, evoked);
}

void Interpreter::visit(const UnaryExpr& expr) const
{
	evaluate(*expr.operand);
	Value right = currentResult;

	switch (expr.op.type)
	{
	case (BANG):
		checkNumberOperand(expr.op, right);
		currentResult = Value(~right.getByte());
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
		currentResult = Value(left.getByte() - right.getByte());
		break;
	case SLASH:
		checkNumberOperands(expr.op, left, right);
		if (right.getByte() == 0)
			throw RuntimeError(expr.op, "Division by zero.");
		currentResult = Value(left.getByte() / right.getByte());
		break;
	case STAR:
		checkNumberOperands(expr.op, left, right);
		currentResult = Value(left.getByte() * right.getByte());
		break;
	case PERCENT:
		checkNumberOperands(expr.op, left, right);
		if (right.getByte() == 0)
			throw RuntimeError(expr.op, "Division by zero.");
		currentResult = Value(left.getByte() % right.getByte());
		break;
	case PLUS:
		currentResult = addValues(left, right);
		break;
	case GREATER:
		checkNumberOperands(expr.op, left, right);
		currentResult = Value(left.getByte() > right.getByte());
		break;
	case GREATER_EQUAL:
		checkNumberOperands(expr.op, left, right);
		currentResult = Value(left.getByte() >= right.getByte());
		break;
	case LESS:
		checkNumberOperands(expr.op, left, right);
		currentResult = Value(left.getByte() < right.getByte());
		break;
	case LESS_EQUAL:
		checkNumberOperands(expr.op, left, right);
		currentResult = Value(left.getByte() <= right.getByte());
		break;
	case BANG_EQUAL:
		checkNumberOperands(expr.op, left, right);
		currentResult = Value(left.getByte() != right.getByte());
		break;
	case EQUAL_EQUAL:
		currentResult = areValuesEqual(left, right);
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
	int index = static_cast<int>(currentResult.getByte());
	currentResult = environment.getArrayElement(expr.name, index);
}

void Interpreter::visit(const ArraySetExpr& expr) const
{
	evaluate(*expr.index);
	checkNumberOperand(expr.name, currentResult);
	int index = static_cast<int>(currentResult.getByte());
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

void Interpreter::visit(const ExpressionStmt& stmt, bool evoked) const
{
	if (evoked)
		evaluate(*stmt.expr);
	else
	{
		environment.subscribe(stmt.subscribedEvent.lexeme, stmt.clone());
	}
}

void Interpreter::visit(const PrintStmt& stmt, bool evoked) const
{
	if (evoked)
	{
		evaluate(*stmt.expr);
		Value value = currentResult;
		std::cout << value.toString() << std::endl;
	}
	else
	{
		environment.subscribe(stmt.subscribedEvent.lexeme, stmt.clone());
	}
}

void Interpreter::visit(const ByteStmt& stmt, bool evoked) const
{
	if (evoked)
	{
		Value value;
		if (stmt.initializer != nullptr)
		{
			evaluate(*stmt.initializer);
			value = currentResult;
		}

		environment.defineVariable(stmt.name.lexeme, value);
	}
	else
	{
		environment.subscribe(stmt.subscribedEvent.lexeme, stmt.clone());
	}
}

void Interpreter::visit(const ArrayStmt& stmt, bool evoked) const
{
	if (evoked)
	{
		environment.defineArray(stmt.name.lexeme);
	}
	else
	{
		environment.subscribe(stmt.subscribedEvent.lexeme, stmt.clone());
	}
}

void Interpreter::visit(const EmitStmt& stmt, bool evoked) const
{
	//
	if (stmt.subscribedEvent.type == NONE)
	{
		emit(stmt);
	}
	else if (evoked)
	{
		emit(stmt);
	}
	else
	{
		environment.subscribe(stmt.subscribedEvent.lexeme, stmt.clone());
	}
}

void Interpreter::emit(const EmitStmt& stmt) const
{
	if (stmt.condition != nullptr)
	{
		evaluate(*stmt.condition);
		Value is_true(1);
		bool shouldTrigger = areValuesEqual(currentResult, is_true);
		if (shouldTrigger)
		{
			if (stmt.op.type == QUESTION)
			{
				triggerEvent(stmt.eventName.lexeme);
			}
			else if (stmt.op.type == QUESTION_QUESTION)
			{
				while (shouldTrigger)
				{
					triggerEvent(stmt.eventName.lexeme);
					evaluate(*stmt.condition);
					shouldTrigger = areValuesEqual(currentResult, is_true);
				}
			}
		}
	}
	else
	{
		triggerEvent(stmt.eventName.lexeme);
	}
}

void Interpreter::evaluate(const Expr& expr) const
{
	expr.accept(*this);
}

void Interpreter::checkNumberOperand(Token op, Value operand) const
{
	if (operand.getType() == Type::BYTE)
		return;
	throw RuntimeError(op, "Operand must be a number");
}

void Interpreter::checkNumberOperands(Token op, Value left, Value right) const
{
	if (left.getType() == Type::BYTE && right.getType() == Type::BYTE)
		return;
	throw RuntimeError(op, "Operands must be numbers.");
}

bool Interpreter::areValuesEqual(Value left, Value right) const
{
	if (left.getType() != right.getType())
		return false;

	if (left.getType() == Type::BYTE)
		return left.getByte() == right.getByte();
	return left.getString() == right.getString();
}

Value Interpreter::addValues(Value left, Value right) const
{
	if (left.getType() != right.getType())
		throw::RuntimeError(Token(), "Type mismatch. Types must match.");
	if (left.getType() == Type::BYTE)
		return Value(left.getByte() + right.getByte());
	return Value(left.getString() + right.getString());
}

void Interpreter::triggerEvent(const std::string& eventName) const
{
	const auto& statements = environment.getSubscribedStatements(eventName);
	for (const auto& stmt : statements)
	{
		execute(*stmt, true);
	}
}
