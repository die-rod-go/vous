#pragma once
#include <vector>
#include <chrono> 
#include "value.h"
#include "errors.h"

class VousCallable;
class Interpreter;

class ClockFunction : public VousCallable
{
public:
	Value call(const Interpreter& interpreter, std::vector<Value>& arguments) const override
	{
		auto now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		return Value(static_cast<double>(millis) / 1000.0);
	}

	int getArity() const override
	{
		return 0;
	}

	std::string toString() const override
	{
		return "<native fn>";
	}

};

class PrintFunction : public VousCallable
{
public:
	Value call(const Interpreter& interpreter, std::vector<Value>& arguments) const override
	{
		std::cout << arguments[0].toString();
		return arguments[0];
	}

	int getArity() const override
	{
		return 1;
	}

	std::string toString() const override
	{
		return "<native fn>";
	}

};

class PrintLineFunction : public VousCallable
{
public:
	Value call(const Interpreter& interpreter, std::vector<Value>& arguments) const override
	{
		std::cout << arguments[0].toString() << "\n";
		return arguments[0];
	}

	int getArity() const override
	{
		return 1;
	}

	std::string toString() const override
	{
		return "<native fn>";
	}

};

class InputFunction : public VousCallable
{
public:
	Value call(const Interpreter& interpreter, std::vector<Value>& arguments) const override
	{
		std::string input;
		std::getline(std::cin, input);
		return Value(input);
	}

	int getArity() const override
	{
		return 0;
	}

	std::string toString() const override
	{
		return "<native fn>";
	}

};

class ConvertToStringFunction : public VousCallable
{
public:
	Value call(const Interpreter& interpreter, std::vector<Value>& arguments) const override
	{
		return Value(arguments[0].toString());
	}

	int getArity() const override
	{
		return 1;
	}

	std::string toString() const override
	{
		return "<native fn>";
	}

};

class ConvertToNumberFunction : public VousCallable
{
public:
	Value call(const Interpreter& interpreter, std::vector<Value>& arguments) const override
	{
		return Value(std::stod(arguments[0].toString()));
	}

	int getArity() const override
	{
		return 1;
	}

	std::string toString() const override
	{
		return "<native fn>";
	}
};