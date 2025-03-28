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

	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");

}

void Environment::subscribe(const std::string& eventName, std::unique_ptr<Stmt> stmt)
{
	eventMap[eventName].push_back(std::move(stmt));
}

const std::vector<std::unique_ptr<Stmt>>& Environment::getSubscribedStatements(const std::string& eventName) const
{
	std::vector<std::unique_ptr<Stmt>> emptyList;
	auto iterator = eventMap.find(eventName);
	if (iterator != eventMap.end())
	{
		return iterator->second;
	}
	return emptyList;
}

Value Environment::getVariable(Token name) const
{
	if (values.find(name.lexeme) != values.end())
	{
		return values.at(name.lexeme);
	}
	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
	return byte();
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

	throw RuntimeError(name, "Undefined array '" + name.lexeme + "'.");
}

Value Environment::getArrayElement(Token name, int index)
{
	if (arrayMap.find(name.lexeme) != arrayMap.end())
	{
		if (index >= 0 && index < arrayMap[name.lexeme].size())
		{
			return arrayMap[name.lexeme][index];;
		}
		else
		{
			throw RuntimeError(name, "Index out of bounds for array '" + name.lexeme + "'.");
		}
	}

	throw RuntimeError(name, "Undefined array '" + name.lexeme + "'.");
}
