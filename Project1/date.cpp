#include "date.h"

bool operator<(const Date& lhs, const Date& rhs) {
	// воспользуемся тем фактом, что векторы уже можно сравнивать на <:
	// создадим вектор из года, месяца и дня для каждой даты и сравним их
	return vector<int>{lhs.GetYear(), lhs.GetMonth(), lhs.GetDay()} <
		vector<int>{rhs.GetYear(), rhs.GetMonth(), rhs.GetDay()};
}

bool operator>(const Date& lhs, const Date& rhs) {
	// воспользуемся тем фактом, что векторы уже можно сравнивать на <:
	// создадим вектор из года, месяца и дня для каждой даты и сравним их
	return vector<int>{lhs.GetYear(), lhs.GetMonth(), lhs.GetDay()} >
		vector<int>{rhs.GetYear(), rhs.GetMonth(), rhs.GetDay()};
}

bool operator==(const Date& lhs, const Date& rhs) {
	return vector<int>{lhs.GetYear(), lhs.GetMonth(), lhs.GetDay()} ==
		vector<int>{rhs.GetYear(), rhs.GetMonth(), rhs.GetDay()};
}


ostream& operator<<(ostream& stream, const Date& date) {
	stream << setw(4) << setfill('0') << date.GetYear() <<
		"-" << setw(2) << setfill('0') << date.GetMonth() <<
		"-" << setw(2) << setfill('0') << date.GetDay();
	return stream;
}

Date::Date(int new_year, int new_month, int new_day) {
	year = new_year;
	if (new_month > 12 || new_month < 1) {
		throw logic_error("Month value is invalid: " + to_string(new_month));
	}
	month = new_month;
	if (new_day > 31 || new_day < 1) {
		throw logic_error("Day value is invalid: " + to_string(new_day));
	}
	day = new_day;
}


int Date::GetYear() const {
	return year;
}
int Date::GetMonth() const {
	return month;
}
int Date::GetDay() const {
	return day;
}


Date ParseDate(istream& is) {
	bool ok = true;

	int year;
	ok = ok && (is >> year);
	ok = ok && (is.peek() == '-');
	is.ignore(1);

	int month;
	ok = ok && (is >> month);
	ok = ok && (is.peek() == '-');
	is.ignore(1);

	int day;
	ok = ok && (is >> day);
	ok = ok && is.eof();

	/* if (!ok) {
		 throw logic_error("Wrong date format: " + date);
	 }*/
	return Date(year, month, day);
}

string ParseDareToString(const Date& date) {

	string year{ "0000" }, month{ "00" }, day{ "00" };
	string _year{}, _month{}, _day{};
	_year = to_string(date.GetYear());
	_month = to_string(date.GetMonth());
	_day = to_string(date.GetDay());

	for (int i = 0; i < _year.size(); ++i) {
		year[year.size() - 1 - i] = _year[_year.size() - 1 - i];
	}

	for (int i = 0; i < _month.size(); ++i) {
		month[month.size() - 1 - i] = _month[_month.size() - 1 - i];
	}

	for (int i = 0; i < _day.size(); ++i) {
		day[day.size() - 1 - i] = _day[_day.size() - 1 - i];
	}

	return year + '-' + month + '-' + day;

};






