#include "environment.h"

//	defines a new variable in the current environment
void Environment::defineVariable(const std::string& name, Value value)
{
	values[name] = value;
}

//	assigns a variable in the current environments
//	recursivley checks enclosing environments until variable is assigned
//	or error is thrown
void Environment::assignVariable(Token name, Value value)
{
	//	check if variable exists in this environment
	if (values.find(name.lexeme) != values.end())
	{
		//	set the value and return
		values[name.lexeme] = value;
		return;
	}

	//	if variable not found (above does not return)
	//	then check if enclosing environment exists
	if (enclosing != nullptr)
	{
		//	try to assign variable in enclosing environment and return
		enclosing->assignVariable(name, value);
		return;
	}

	//	variable not found in any environment. error
	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

//	gets a variable from the current environments
//	recursivley checks enclosing environments until variable is found
//	or error is thrown
Value Environment::getVariable(Token name) const
{
	//	check if variable exists in this environment
	if (values.find(name.lexeme) != values.end())
	{
		//	return it if it does
		return values.at(name.lexeme);
	}

	//	if variable not found (above does not return)
	//	then check if enclosing environment exists
	if (enclosing.get() != nullptr)
		//	search for variable in enclosing environment
		return enclosing->getVariable(name);

	//	variable not found in any environment. error
	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
	return double();
}

//	defines a new array in current environment
void Environment::defineArray(const std::string& name)
{
	arrayMap[name] = std::vector<Value>();
}

//	pushes a variable to an array in the current environments
//	recursivley checks enclosing environments until value
//	is pushed or error is thrown
void Environment::pushArray(Token name, Value value)
{
	//	check if array exists in this environment
	if (arrayMap.find(name.lexeme) != arrayMap.end())
	{
		//	push to it if it does
		arrayMap[name.lexeme].push_back(value);
		return;
	}

	//	if array not found (above does not return)
	//	then check if enclosing environment exists
	if (enclosing != nullptr)
	{
		//	try to push to array in enclosing environment and return
		enclosing->pushArray(name, value);
		return;
	}

	//	array not found in any environment. error
	throw RuntimeError(name, "Undefined array '" + name.lexeme + "'.");
}

//	assigns a element in an array in the current environments
//	recursivley checks enclosing environments until element is assigned
//	or error is thrown
void Environment::setArrayElement(Token name, int index, Value value)
{
	//	check if array exists in this environment
	if (arrayMap.find(name.lexeme) != arrayMap.end())
	{
		//	check if index is in bounds
		if (index >= 0 && index < arrayMap[name.lexeme].size())
		{
			//	assign value in array
			arrayMap[name.lexeme][index] = value;
			return;
		}
		else
		{
			// error. out of bounds
			throw RuntimeError(name, "Index out of bounds for array '" + name.lexeme + "'.");
		}
	}

	//	if array not found (above does not return)
	//	then check if enclosing environment exists
	if (enclosing != nullptr)
	{
		//	attempt to assign variable in array in enclosing environment
		enclosing->setArrayElement(name, index, value);
		return;
	}

	//	array not found in any environment. error
	throw RuntimeError(name, "Undefined array '" + name.lexeme + "'.");
}

//	gets an element in an array in the current environments
//	recursivley checks enclosing environments until array element is found
//	or error is thrown
Value Environment::getArrayElement(Token name, int index)
{
	//	check if array exists in this environment
	if (arrayMap.find(name.lexeme) != arrayMap.end())
	{
		//	check if index is in bounds
		if (index >= 0 && index < arrayMap[name.lexeme].size())
		{
			//	return the element
			return arrayMap[name.lexeme][index];
		}
		else
		{
			//	error. out of bounds
			throw RuntimeError(name, "Index out of bounds for array '" + name.lexeme + "'.");
		}
	}

	//	if array not found (above does not return)
	//	then check if enclosing environment exists
	if (enclosing != nullptr)
		//	attempt to find element in enclosing environment
		return enclosing->getArrayElement(name, index);

	//	array not found in any environment. error
	throw RuntimeError(name, "Undefined array '" + name.lexeme + "'.");
}
