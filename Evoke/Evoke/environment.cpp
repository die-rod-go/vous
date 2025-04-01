#include "environment.h"

void Environment::defineVariable(const std::string& name, Value value)
{
	values[name] = value;
}

void Environment::assignVariable(Token name, Value value)
{
	if (values.find(name.lexeme) != values.end())
	{
		values[name.lexeme] = value;
		return;
	}

	if (enclosing != nullptr)
	{
		enclosing->assignVariable(name, value);
		return;
	}

	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

Value Environment::getVariable(Token name) const
{
	if (values.find(name.lexeme) != values.end())
	{
		return values.at(name.lexeme);
	}

	if (enclosing.get() != nullptr)
		return enclosing->getVariable(name);

	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
	return double();
}

void Environment::defineArray(const std::string& name)
{
	arrayMap[name] = std::vector<Value>();
}

void Environment::pushArray(Token name, Value value)
{
	if (arrayMap.find(name.lexeme) != arrayMap.end())
	{
		arrayMap[name.lexeme].push_back(value);
		return;
	}

	if (enclosing != nullptr)
	{
		enclosing->pushArray(name, value);
		return;
	}

	throw RuntimeError(name, "Undefined array '" + name.lexeme + "'.");
}

void Environment::setArrayElement(Token name, int index, Value value)
{
	if (arrayMap.find(name.lexeme) != arrayMap.end())
	{
		if (index >= 0 && index < arrayMap[name.lexeme].size())
		{
			arrayMap[name.lexeme][index] = value;
			return;
		}
		else
		{
			throw RuntimeError(name, "Index out of bounds for array '" + name.lexeme + "'.");
		}
	}

	if (enclosing != nullptr)
	{
		enclosing->setArrayElement(name, index, value);
		return;
	}

	throw RuntimeError(name, "Undefined array '" + name.lexeme + "'.");
}

Value Environment::getArrayElement(Token name, int index)
{
	if (arrayMap.find(name.lexeme) != arrayMap.end())
	{
		if (index >= 0 && index < arrayMap[name.lexeme].size())
		{
			return arrayMap[name.lexeme][index];
		}
		else
		{
			throw RuntimeError(name, "Index out of bounds for array '" + name.lexeme + "'.");
		}
	}

	if (enclosing != nullptr)
		return enclosing->getArrayElement(name, index);

	throw RuntimeError(name, "Undefined array '" + name.lexeme + "'.");
}
