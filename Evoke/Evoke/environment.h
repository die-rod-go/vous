#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include "types.h"
#include "errors.h"
#include "token.h"
#include "stmt.h"

/**
 * @class Environment
 * @brief Represents an environment for managing variables and arrays.
 *
 * The Environment class handles the definition and assignment of variables
 * and arrays, including the ability to search and update variables and
 * arrays across multiple nested environments.
 */
class Environment
{
public:
	/** Constructs an Environment with no enclosing environment */
	Environment() : enclosing(nullptr) {}

	/** Constructs an Environment with an enclosing environment */
	Environment(std::unique_ptr<Environment> enclosing) : enclosing(std::move(enclosing)) {}

	/**
	 * @brief Custom copy constructor to handle the non-copyable unique_ptr<Environment>.
	 *
	 * The default copy constructor cannot copy unique_ptr members, so we explicitly
	 * create a new std::unique_ptr for the enclosing environment if it exists. The
	 * other members (values and arrayMap) are safely copied using the default behavior.
	*/
	Environment(const Environment& other)
		:	values(other.values),
			arrayMap(other.arrayMap),
			enclosing(other.enclosing ? std::make_unique<Environment>(*other.enclosing) : nullptr)
	{}

	/** The environment "holding" this one */
	mutable std::unique_ptr<Environment> enclosing;

	// ==========[ Variable handling ]===========
	
	/** Creates a named Variable-Value pair in the value map in the current environment. */
	void defineVariable(const std::string& name, Value value);

	/** Assigns a value to a variable in the environment chain. */
	void assignVariable(Token name, Value value);

	/** Get variable from existing environments(recursively searches) */
	Value getVariable(Token name) const;

	// ==========[ Array Handling ]===========
	
	/** Defines a new array in the current environment */
	void defineArray(const std::string& name);

	/** Pushes a variable to the array in the current environments(recursively searches) */
	void pushArray(Token name, Value value);

	/** Sets a value of an array element in the current environments(recursively searches) */
	void setArrayElement(Token name, int index, Value value);

	/** Gets the value of an array element in the current environments(recursively searches) */
	Value getArrayElement(Token name, int index);

private:
	/** Holds the values in the current environment */
	std::unordered_map<std::string, Value> values;

	/** Holds the current arrays in the current environment(array functionality will be reworked) */
	std::unordered_map<std::string, std::vector<Value>> arrayMap;

};

