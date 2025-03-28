#pragma once
#include <string>
#include <variant>
#include "types.h"

class Value
{
public:
	Value() : value(byte(0)) {};
	Value(byte value) : value(value) {};
	Value(const std::string& value) : value(value) {};
	std::string toString()
	{
		if (getType() == Type::STRING)
			return getString();
		return std::to_string((int)getByte());
	}

	byte getByte() const
	{
		if (std::holds_alternative<byte>(value))
		{
			return std::get<byte>(value);
		}
		
		//Pulse::error(Token(), "Value error. Value is not a byte");
	}

	std::string getString() const
	{
		if (std::holds_alternative<std::string>(value))
		{
			return std::get<std::string>(value);
		}
		//Pulse::error(Token(), "Value error. Value is not a string");
	}

	void setValue(byte value) { value = value; }
	void setValue(const std::string& v) { value = value; }

	Type getType() {
		if (std::holds_alternative<byte>(value))
		{
			return Type::BYTE;
		}

		return Type::STRING;
	}

private:

	std::variant<byte, std::string> value;
};