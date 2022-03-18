#pragma once

#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>

using namespace std;



class Date {
public:
    // конструктор выбрасывает исключение, если его аргументы некорректны
    Date(int new_year, int new_month, int new_day);

    int GetYear() const;
    int GetMonth() const;
    int GetDay() const;

private:
    int year;
    int month;
    int day;
};

// определить сравнение для дат необходимо для использования их в качестве ключей словаря
bool operator<(const Date& lhs, const Date& rhs);

// даты будут по умолчанию выводиться в нужном формате
ostream& operator<<(ostream& stream, const Date& date);

template <typename K, typename V>
ostream& operator << (ostream& os, const pair<K, V>& p) {

    os << p.first << ' ' << p.second;
    return os;
};

bool operator==(const Date& lhs, const Date& rhs);

bool operator>(const Date& lhs, const Date& rhs);

Date ParseDate(istream& is);

string ParseDareToString(const Date& date);