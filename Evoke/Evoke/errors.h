#pragma once
#include "token.h"

class RuntimeError : public std::exception {
public:
	Token token;
	std::string message;

	RuntimeError(Token token, std::string message) : token(token), message(message)
	{}
	const char* what() const noexcept override {
		return "An error occurred while parsing!";
	}
};