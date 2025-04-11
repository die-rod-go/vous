#include "environment.h"

/**
 * @brief Creates a named Variable-Value pair in the value map in the current environment.
 *
 * @param name The name of the variable the Value will be bound to.
 * @param value The Value to put in the environment map.
 */
void Environment::defineVariable(const std::string& name, Value value)
{
	values[name] = value;
}

/**
 * @brief Assigns a value to a variable in the environment chain.
 *
 * Searches for the variable in the current and enclosing environments,
 * and assigns the value if found. Throws a RuntimeError if the variable
 * is not found in any environment.
 *
 * @param name The token containing the name of the variable to assign the value to.
 * @param value The Value to assign to the variable.
 * @throws RuntimeError If the variable is not found in any environment.
 */
void Environment::assignVariable(Token name, Value value)
{
	// Check if variable exists in this environment
	if (values.find(name.lexeme) != values.end())
	{
		// Set the value and return
		values[name.lexeme] = value;
		return;
	}

	// if variable not found (above does not return)
	// Then check if enclosing environment exists
	if (enclosing != nullptr)
	{
		// Try to assign variable in enclosing environment and return
		enclosing->assignVariable(name, value);
		return;
	}

	// ERROR. Variable not found in any environment.
	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

/**
 * @brief Retrieves the value of a variable from the environment chain.
 *
 * Searches for the variable in the current and enclosing environments
 * until found. Throws a RuntimeError if the variable is not found.
 *
 * @param name The token representing the variable name.
 * @return The value associated with the variable.
 * @throws RuntimeError If the variable is not found in any environment.
 */
Value Environment::getVariable(Token name) const
{
	// Check if variable exists in this environment
	if (values.find(name.lexeme) != values.end())
	{
		// return it if it does
		return values.at(name.lexeme);
	}

	// if variable not found (above does not return)
	// Then check if enclosing environment exists
	if (enclosing.get() != nullptr)
		// Search for variable in enclosing environment
		return enclosing->getVariable(name);

	// ERROR. Variable not found in any environment.
	throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

/**
 * @brief Defines a new array in the current environment.
 *
 * Creates a new empty array and binds it to the given name.
 *
 * @param name The name of the array to define.
 */
void Environment::defineArray(const std::string& name)
{
	arrayMap[name] = std::vector<Value>();
}

/**
 * @brief Pushes a value to an array in the environment chain.
 *
 * Searches for the array in the current and enclosing environments
 * and pushes the value if found. Throws a RuntimeError if the array
 * is not found in any environment.
 *
 * @param name The token representing the name of the array.
 * @param value The value to push to the array.
 * @throws RuntimeError If the array is not found in any environment.
 */
void Environment::pushArray(Token name, Value value)
{
	// Check if array exists in this environment
	if (arrayMap.find(name.lexeme) != arrayMap.end())
	{
		// Push to it if it does
		arrayMap[name.lexeme].push_back(value);
		return;
	}

	// if array not found (above does not return)
	// then check if enclosing environment exists
	if (enclosing != nullptr)
	{
		// Try to push to array in enclosing environment and return
		enclosing->pushArray(name, value);
		return;
	}

	// ERROR. Array not found in any environment
	throw RuntimeError(name, "Undefined array '" + name.lexeme + "'.");
}

/**
 * @brief Assigns a value to a specific index in an array in the environment chain.
 *
 * Searches for the array in the current and enclosing environments and
 * assigns the value at the given index if found. Throws a RuntimeError
 * if the array is not found or if the index is out of bounds.
 *
 * @param name The token representing the name of the array.
 * @param index The index of the array element to assign.
 * @param value The value to assign to the array element.
 * @throws RuntimeError If the array is not found or the index is out of bounds.
 */
void Environment::setArrayElement(Token name, int index, Value value)
{
	// if array exists in this environment
	if (arrayMap.find(name.lexeme) != arrayMap.end())
	{
		// if index is in bounds
		if (index >= 0 && index < arrayMap[name.lexeme].size())
		{
			// assign value in array
			arrayMap[name.lexeme][index] = value;
			return;
		}
		else
		{
			// ERROR. out of bounds
			throw RuntimeError(name, "Index out of bounds for array '" + name.lexeme + "'.");
		}
	}

	// if array not found (above does not return)
	// then check if enclosing environment exists
	if (enclosing != nullptr)
	{
		// Attempt to assign variable in array in enclosing environment
		enclosing->setArrayElement(name, index, value);
		return;
	}

	// ERROR. Array not found in any environment
	throw RuntimeError(name, "Undefined array '" + name.lexeme + "'.");
}

/**
 * @brief Retrieves a value from a specific index in an array.
 *
 * Searches for the array in the current and enclosing environments
 * and retrieves the value at the given index if found. Throws a RuntimeError
 * if the array is not found or if the index is out of bounds.
 *
 * @param name The token representing the name of the array.
 * @param index The index of the element to retrieve.
 * @return The value at the specified index in the array.
 * @throws RuntimeError If the array is not found or the index is out of bounds.
 */
Value Environment::getArrayElement(Token name, int index)
{
	// check if array exists in this environment
	if (arrayMap.find(name.lexeme) != arrayMap.end())
	{
		// check if index is in bounds
		if (index >= 0 && index < arrayMap[name.lexeme].size())
		{
			// return the element
			return arrayMap[name.lexeme][index];
		}
		else
		{
			// error. out of bounds
			throw RuntimeError(name, "Index out of bounds for array '" + name.lexeme + "'.");
		}
	}

	// if array not found (above does not return)
	// then check if enclosing environment exists
	if (enclosing != nullptr)
		// attempt to find element in enclosing environment
		return enclosing->getArrayElement(name, index);

	// ERROR. Array not found in any environment
	throw RuntimeError(name, "Undefined array '" + name.lexeme + "'.");
}
