#include <iostream>
#include "interpreter.h"
#include "vous.h"
#include "nativefunctions.h"
#include "VousFunction.h"
#include "vouscallable.h"


Interpreter::Interpreter() : environment(std::make_unique<Environment>())
{
	environment->defineVariable("clock", Value(std::make_shared<ClockFunction>()));
	environment->defineVariable("print", Value(std::make_shared<PrintFunction>()));
	environment->defineVariable("println", Value(std::make_shared<PrintLineFunction>()));
	environment->defineVariable("input", Value(std::make_shared<InputFunction>()));
	globals.defineVariable("clock", Value(std::make_shared<ClockFunction>()));

}

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>>& statements) const {
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

void Interpreter::visit(const CallExpr& expr) const
{
	evaluate(*expr.callee);
	Value callee = currentResult;

	std::vector<Value> arguments;
	for (const auto& argument : expr.arguments)
	{
		evaluate(*argument);
		arguments.push_back(currentResult);
	}

	if (callee.getType() != Type::FUNCTION)
		throw RuntimeError(expr.paren, "Can only call functions and classes.");

	std::shared_ptr<VousCallable> function = callee.getFunction();

	if (arguments.size() != function->getArity())
		throw RuntimeError(expr.paren, "Expected " + std::to_string(function->getArity()) + " arguments but got " + std::to_string(arguments.size()) + ".");

	currentResult = function->call(*this, arguments);
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
		currentResult = isTruthy(left) && isTruthy(right);
		break;
	case PIPE_PIPE:
		currentResult = isTruthy(left) || isTruthy(right);
		break;
	}
}

void Interpreter::visit(const LiteralExpr& expr) const
{
	currentResult = expr.literal.literal;
}

void Interpreter::visit(const VariableExpr& expr) const
{
	currentResult = environment->getVariable(expr.name);
}

void Interpreter::visit(const AssignmentExpr& expr) const
{
	evaluate(*expr.value);
	Value value = currentResult;
	environment->assignVariable(expr.name, value);
	currentResult = value;
}

void Interpreter::visit(const ArrayPushExpr& expr) const
{
	evaluate(*expr.value);
	environment->pushArray(expr.name, currentResult);
}

void Interpreter::visit(const ArrayAccessExpr& expr) const
{
	evaluate(*expr.index);
	checkNumberOperand(expr.name, currentResult);
	int index = static_cast<int>(currentResult.getDouble());
	currentResult = environment->getArrayElement(expr.name, index);
}

void Interpreter::visit(const ArraySetExpr& expr) const
{
	evaluate(*expr.index);
	checkNumberOperand(expr.name, currentResult);
	int index = static_cast<int>(currentResult.getDouble());
	evaluate(*expr.value);
	Value value = currentResult;
	environment->setArrayElement(expr.name, index, value);
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

void Interpreter::visit(const FunctionStmt& stmt) const
{
	//	clone returns unique_ptr<Stmt> and cast back to FunctionStmt
	std::unique_ptr<FunctionStmt> functionAst(
		static_cast<FunctionStmt*>(stmt.clone().release())
	);

	// Now wrap it in your runtime callable object (like LoxFunction in Java)
	std::shared_ptr<VousFunction> function = std::make_shared<VousFunction>(std::move(functionAst));

	environment->defineVariable(stmt.name.lexeme, Value(function));
}

void Interpreter::visit(const PrintStmt& stmt) const
{
	evaluate(*stmt.expr);
	Value value = currentResult;
	std::cout << value.toString() << "\n";
}

void Interpreter::visit(const ByteStmt& stmt) const
{
	Value value;
	if (stmt.initializer != nullptr)
	{
		evaluate(*stmt.initializer);
		value = currentResult;
	}

	environment->defineVariable(stmt.name.lexeme, value);
}

void Interpreter::visit(const ArrayStmt& stmt) const
{
	environment->defineArray(stmt.name.lexeme);
}

void Interpreter::visit(const BlockStmt& stmt) const
{
	executeBlock(stmt, std::move(std::make_unique<Environment>()));
}

void Interpreter::executeBlock(const BlockStmt& stmt, std::unique_ptr<Environment> environment) const
{
	std::unique_ptr<Environment> previous = std::move(this->environment);

	try
	{
		this->environment = std::move(environment);
		this->environment->enclosing = std::move(previous);

		for (const auto& statement : stmt.stmts)
		{
			execute(*statement);
		}

		//	restore the previous environment
		previous = std::move(this->environment->enclosing);
		this->environment = std::move(previous);
	}
	catch (...)
	{
		//	ensure environment is restored even if an exception occurs
		if (this->environment && this->environment->enclosing) {
			previous = std::move(this->environment->enclosing);
		}
		//	need to fix, potential unsafe double move of previous
		this->environment = std::move(previous);
		throw;
	}
}

void Interpreter::visit(const IfStmt& stmt) const
{
	evaluate(*stmt.condition);
	if (isTruthy(currentResult))
		execute(*stmt.thenBranch);
	else if (stmt.elseBranch != nullptr)
		execute(*stmt.elseBranch);
}

void Interpreter::visit(const WhileStmt& stmt) const
{
	evaluate(*stmt.condition);
	while (isTruthy(currentResult))
	{
		execute(*stmt.body);
		//	must reevaluate condition after each loop iteration
		evaluate(*stmt.condition);
	}
}

void Interpreter::evaluate(const Expr& expr) const
{
	expr.accept(*this);
}

void Interpreter::checkNumberOperand(const Token& op, const Value& operand) const
{
	if (operand.getType() == Type::DOUBLE)
		return;
	throw RuntimeError(op, "Operand must be a number");
}

void Interpreter::checkNumberOperands(const Token& op, const Value& left, const Value& right) const
{
	if (left.getType() == Type::DOUBLE && right.getType() == Type::DOUBLE)
		return;
	throw RuntimeError(op, "Operands must be numbers.");
}

void Interpreter::checkAddableOperands(const Token& op, const Value& left, const Value& right) const
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

void Interpreter::checkBoolOperands(const Token& op, const Value& left, const Value& right) const
{
	if (left.getType() == Type::BOOLEAN && right.getType() == Type::BOOLEAN)
		return;
	throw RuntimeError(op, "Operands must be booleans.");
}

bool Interpreter::areValuesEqual(const Value& left, const Value& right) const
{
	if (left.getType() != right.getType())
		return false;

	if (left.getType() == Type::DOUBLE)
		return left.getDouble() == right.getDouble();
	if (left.getType() == Type::BOOLEAN)
		return left.getBool() == right.getBool();
	return left.getString() == right.getString();
}

Value Interpreter::addValues(const Token& op, const Value& left, const Value& right) const
{
	if (left.getType() != right.getType())
		throw::RuntimeError(op, "Type mismatch. Types must match.");

	if (left.getType() != right.getType())
		throw::RuntimeError(Token(), "Type mismatch. Types must match.");

	if (left.getType() == Type::DOUBLE)
		return Value(left.getDouble() + right.getDouble());

	return Value(left.getString() + right.getString());
}

bool Interpreter::isTruthy(const Value& value) const
{
	switch (value.getType())
	{
	case Type::BOOLEAN:
		return value.getBool();
		break;
	case Type::DOUBLE:
		return value.getDouble() != 0;
		break;
	}
	return true;
}

