#include "VousFunction.h"

VousFunction::VousFunction(std::unique_ptr<FunctionStmt> declaration) : declaration(std::move(declaration))
{
}

Value VousFunction::call(const Interpreter& interpreter, std::vector<Value>& arguments) const
{
	auto environment = std::make_unique<Environment>();
	environment->enclosing = std::make_unique<Environment>(interpreter.globals);

	for (int i = 0; i < declaration->params.size(); i++)
	{
		environment->defineVariable(declaration->params[i].lexeme, arguments[i]);
	}

	interpreter.executeBlock(std::move(declaration->body), std::move(environment));
	return Value();
}

int VousFunction::getArity() const
{
	return declaration->params.size();
}

std::string VousFunction::toString() const
{
	return "<fn " + declaration->name.lexeme + ">";
}
