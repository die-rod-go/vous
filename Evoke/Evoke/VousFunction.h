#pragma once
#include "vouscallable.h"
#include <memory>
#include "stmt.h"
#include "interpreter.h"


class VousFunction : public VousCallable
{
public:
	VousFunction(std::unique_ptr<FunctionStmt> declaration);
	virtual Value call(const Interpreter& interpreter, std::vector<Value>& arguments) const override;
	virtual int getArity() const override;
	virtual std::string toString() const override;
	std::unique_ptr<FunctionStmt> declaration;
private:
};

