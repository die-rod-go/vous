#pragma once
#include <string>
#include <variant>
#include "types.h"
#include "vouscallable.h"
#include <memory>

class Value
{
public:
	Value() : value(double(0)) {};
	Value(double value) : value(value) {};
	Value(const std::string& value) : value(value) {};
	Value(bool value) : value(value) {};
	Value(std::shared_ptr<VousCallable> value) : value(value) {};

	Type getType() const {
		if (std::holds_alternative<double>(value)) return Type::DOUBLE;
		if (std::holds_alternative<bool>(value)) return Type::BOOLEAN;
		if (std::holds_alternative<std::string>(value)) return Type::STRING;
		if (std::holds_alternative<std::shared_ptr<VousCallable>>(value)) return Type::FUNCTION;
		return Type::NOV;
	}

	std::string toString()
	{
		switch (getType())
		{
		case Type::STRING:
			return getString();
			break;
		case Type::BOOLEAN:
			return getBool() ? "true" : "false";
			break;
		case Type::DOUBLE:
			return std::to_string(getDouble());
			break;
		case Type::FUNCTION:
			return getFunction()->toString();
			break;
		}

		return "NOV";
	}

	double getDouble() const
	{
		if (std::holds_alternative<double>(value))
			return std::get<double>(value);
	}

	std::string getString() const
	{
		if (std::holds_alternative<std::string>(value))
			return std::get<std::string>(value);
	}

	bool getBool() const
	{
		if (std::holds_alternative<bool>(value))
			return std::get<bool>(value);
	}

	std::shared_ptr<VousCallable> getFunction() const
	{
		if (std::holds_alternative<std::shared_ptr<VousCallable>>(value))
			return std::get<std::shared_ptr<VousCallable>>(value);
	}

private:

	std::variant<double, std::string, bool, std::shared_ptr<VousCallable>> value;
};