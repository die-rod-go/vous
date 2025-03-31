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
	const std::unique_ptr<Environment> enclosing;
	std::unordered_map<std::string, Value> values;
	std::unordered_map<std::string, std::vector<Value>> arrayMap;

public:
	Environment() : enclosing(nullptr) {}
	Environment(std::unique_ptr<Environment> enclosing) : enclosing(std::move(enclosing)) {}
	std::unique_ptr<Environment> clone();

	// Variable handling
	void defineVariable(const std::string& name, Value value);
	void assignVariable(Token name, Value value);
	Value getVariable(Token name) const;

	//	Array handling
	void defineArray(const std::string& name);
	void pushArray(Token name, Value value);
	void setArrayElement(Token name, int index, Value value);
	Value getArrayElement(Token name, int index);
};

