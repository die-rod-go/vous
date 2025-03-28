#include "ASTPrinter.h"

std::string ASTPrinter::print(const Expr& expr) {
	result.str("");
	result.clear();
	expr.accept(*this);
	return result.str();
}

void ASTPrinter::visit(const UnaryExpr& expr) const {
	result << "(" << expr.op.lexeme << " ";
	expr.operand->accept(*this);
	result << ")";
}

void ASTPrinter::visit(const BinaryExpr& expr) const {
	result << "( " << expr.op.lexeme << " ";
	expr.left->accept(*this);
	result << " ";
	expr.right->accept(*this);
	result << ")";
}

void ASTPrinter::visit(const GroupingExpr& expr) const {
	result << "(group ";
	expr.expr->accept(*this);
	result << ")";
}

void ASTPrinter::visit(const LiteralExpr& expr) const {
	result << expr.literal.lexeme;
}

void ASTPrinter::visit(const VariableExpr& expr) const {
	result << expr.name.lexeme;
}

void ASTPrinter::visit(const AssignmentExpr& expr) const {
	result << "(set " << expr.name.lexeme << " ";
	expr.value->accept(*this);
	result << ")";
}
