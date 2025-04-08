#pragma once
#include <vector>
#include <string>

class Interpreter;
class Value;

class VousCallable
{
public:
	virtual Value call(const Interpreter& interpreter, std::vector<Value>& arguments) const = 0;
	virtual int getArity() const = 0;
	virtual std::string toString() const = 0;
};