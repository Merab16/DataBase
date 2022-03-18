#pragma once

#include "date.h"
#include "LogicalOperation.h"
#include "Comparison.h"
#include <vector>

class Node {
public:
	virtual bool Evaluate(const Date& date, const string& event) = 0;

private:


};


class EmptyNode : public Node {
public:
	EmptyNode() {};

	bool Evaluate(const Date& date, const string& event) override;

private:



};

class LogicalOperationNode : public Node {
public:

	LogicalOperationNode(LogicalOperation _oper, shared_ptr<Node> _left, shared_ptr<Node> _right)
		: left(_left)
		, right(_right)
		, oper(_oper)
	{};

	bool Evaluate(const Date& date, const string& event) override;

private:
	const shared_ptr<Node> left{};
	const shared_ptr<Node> right{};
	const LogicalOperation oper{};


};

class DateComparisonNode : public Node {
public:
	DateComparisonNode(Comparison cmp, const Date& date)
		: _cmp(cmp)
		, _date(date)
	{};

	bool Evaluate(const Date& date, const string& event) override;

private:

	const Comparison _cmp;
	const Date _date;

};

class EventComparisonNode : public Node {
public:
	EventComparisonNode(Comparison cmp, const string& value)
		: _cmp(cmp)
		, _value(value)
	{};

	bool Evaluate(const Date& date, const string& event) override;

private:

	const string _value;
	const Comparison _cmp;

};

template <typename T>
bool compare(const T& lhs, const T& rhs, const Comparison cmp) {
	switch (cmp)
	{
	case Comparison::Equal:
		return (lhs == rhs);
	case Comparison::Greater:
		return (lhs > rhs);
	case Comparison::GreaterOrEqual:
		return (lhs > rhs || lhs == rhs);
	case Comparison::Less:
		return (lhs < rhs);
	case Comparison::LessOrEqual:
		return (lhs < rhs || lhs == rhs);
	case Comparison::NotEqual:
		return !(lhs == rhs);
	}
}


