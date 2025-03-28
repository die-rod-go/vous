#pragma once
#include <string>
#include <variant>
#include "types.h"

class Value
{
public:
	Value() : value(double(0)) {};
	Value(double value) : value(value) {};
	Value(const std::string& value) : value(value) {};
	std::string toString()
	{
		if (getType() == Type::STRING)
			return getString();
		return std::to_string(getDouble());
	}

	double getDouble() const
	{
		if (std::holds_alternative<double>(value))
		{
			return std::get<double>(value);
		}
	}

	std::string getString() const
	{
		if (std::holds_alternative<std::string>(value))
		{
			return std::get<std::string>(value);
		}
	}

	void setValue(double value) { value = value; }
	void setValue(const std::string& v) { value = value; }

	Type getType() {
		if (std::holds_alternative<double>(value))
		{
			return Type::DOUBLE;
		}

		return Type::STRING;
	}

private:

	std::variant<double, std::string> value;
};