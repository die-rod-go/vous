#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include "types.h"
#include "errors.h"
#include "token.h"
#include "stmt.h"

class Environment
{
private:
	//	stores values in the current environment
	std::unordered_map<std::string, Value> values;

	//	stores current arrays (array functionality will be reworked)
	std::unordered_map<std::string, std::vector<Value>> arrayMap;

public:
	Environment() : enclosing(nullptr) {}
	Environment(std::unique_ptr<Environment> enclosing) : enclosing(std::move(enclosing)) {}

	//	takes in a const reference and makes a "copy"
	Environment(const Environment& other)
		:	values(other.values),
			arrayMap(other.arrayMap),
			enclosing(other.enclosing ? std::make_unique<Environment>(*other.enclosing) : nullptr)
	{}

	//	the environment "holding" this one
	mutable std::unique_ptr<Environment> enclosing;

	// Variable handling
	void defineVariable(const std::string& name, Value value);	//	defines a new variable in current environment
	void assignVariable(Token name, Value value);				//	assigns a variable in the existing environments
	Value getVariable(Token name) const;						//	get variable from existing environments

	//	Array handling
	void defineArray(const std::string& name);					//	defines a new array in the current environments
	void pushArray(Token name, Value value);					//	pushes a variable to the array in current environments
	void setArrayElement(Token name, int index, Value value);	//	sets a value of an array element in the current environments
	Value getArrayElement(Token name, int index);				//	gets the value of an array element in the current environments
};

