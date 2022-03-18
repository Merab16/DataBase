#include "node.h"

bool EmptyNode::Evaluate(const Date& date, const string& event) {
    return true;
};


bool DateComparisonNode::Evaluate(const Date& date, const string& event) 
{
   
    return compare(date, _date, _cmp);
}

bool EventComparisonNode::Evaluate(const Date& date, const string& event) 
{
   
    return compare(event, _value, _cmp);
}

bool LogicalOperationNode::Evaluate(const Date& date, const string& event) 
{

    switch (oper)
    {
    case LogicalOperation::Or:
        return left->Evaluate(date, event) || right->Evaluate(date, event);

    case LogicalOperation::And:
        return left->Evaluate(date, event) && right->Evaluate(date, event);
    }

    throw invalid_argument("Invalid logical operation");
}