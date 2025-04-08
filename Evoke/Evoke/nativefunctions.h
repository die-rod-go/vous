#pragma once
#include <vector>
#include <chrono> 
#include "value.h"

class VousCallable;
class Interpreter;

class ClockFunction : public VousCallable
{
public:
	Value call(const Interpreter& interpreter, std::vector<Value>& arguments) const override
	{
		// Get current time in seconds with millisecond precision
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