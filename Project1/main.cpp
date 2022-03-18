#include "database.h"
//#include "date.h"
#include "condition_parser.h"
#include "node.h"
#include "test_runner.h"


#include <iostream>
#include <stdexcept>



string ParseEvent(istream& is) {
	is >> ws;
	string str{};
	getline(is, str);
	return str;
}


void TestEmptyNode() {
	{
		EmptyNode en;
		Assert(en.Evaluate(Date{ 0, 1, 1 }, "abc"), "EmptyNode 1");
		Assert(en.Evaluate(Date{ 2017, 11, 18 }, "def"), "EmptyNode 2");
		Assert(en.Evaluate(Date{ 9999, 12, 31 }, "ghi"), "EmptyNode 3");
	}
}

void TestDbAdd() {
	{
		Database db;
		db.Add({ 2017, 1, 1 }, "new year");
		db.Add({ 2017, 1, 7 }, "xmas");
		ostringstream out;
		db.Print(out);
		AssertEqual("2017-01-01 new year\n2017-01-07 xmas\n", out.str(), "straight ordering");
	}
	{
		Database db;
		db.Add({ 2017, 1, 1 }, "new year");
		db.Add({ 2017, 1, 1 }, "holiday");
		ostringstream out;
		db.Print(out);
		AssertEqual("2017-01-01 new year\n2017-01-01 holiday\n", out.str(), "several in one day");
	}
	{
		Database db;
		db.Add({ 2017, 1, 7 }, "xmas");
		db.Add({ 2017, 1, 1 }, "new year");
		ostringstream out;
		db.Print(out);
		AssertEqual("2017-01-01 new year\n2017-01-07 xmas\n", out.str(), "reverse ordering");
	}
	{
		Database db;
		db.Add({ 2017, 1, 1 }, "new year");
		db.Add({ 2017, 1, 1 }, "new year");
		db.Add({ 2017, 1, 1 }, "xmas");
		db.Add({ 2017, 1, 1 }, "new year");
		ostringstream out;
		db.Print(out);
		AssertEqual("2017-01-01 new year\n2017-01-01 xmas\n", out.str(), "uniq adding");
	}
}

void TestDbLast() {
	Database db;
	db.Add({ 2017, 1, 1 }, "new year");
	db.Add({ 2017, 1, 7 }, "xmas");
	{
		try {
			db.Last({ 2016, 12, 31 });
			Assert(false, "last, found no entries");
		}
		catch (...) {
			Assert(true, "last, found no entries");

		}
	}
	{
		ostringstream os;
		os << db.Last({ 2017, 1, 2 });
		AssertEqual("2017-01-01 new year", os.str(), "greater than date");
	}
	{
		ostringstream os;
		os << db.Last({ 2017, 1, 1 });
		AssertEqual("2017-01-01 new year", os.str(), "eq to date");
	}
	{
		ostringstream os;
		os << db.Last({ 2017, 1, 10 });
		AssertEqual("2017-01-07 xmas", os.str(), "greater than max date");
	}
}

void TestInsertionOrder()
{
	{
		Database db;
		db.Add({ 2017, 1, 1 }, "new year");
		db.Add({ 2017, 1, 7 }, "xmas");
		db.Add({ 2017, 1, 7 }, "party");
		db.Add({ 2017, 1, 7 }, "pie");
		ostringstream out;
		db.Print(out);
		AssertEqual("2017-01-01 new year\n2017-01-07 xmas\n2017-01-07 party\n2017-01-07 pie\n", out.str(), "Remove by date, left");
	}

}


void TestDatabase() {
	istringstream empty_is("");
	auto empty_condition = ParseCondition(empty_is);
	auto empty_predicate = [empty_condition](const Date& date, const string& event) {
		return empty_condition->Evaluate(date, event);
	};

	// Add 2 - Del 1 - Add deleted again
	{
		Database db;
		Date d(2019, 1, 1);
		db.Add(d, "e1");
		db.Add(d, "e2");
		istringstream is(R"(event == "e1")");
		auto condition = ParseCondition(is);
		auto predicate = [condition](const Date& date, const string& event) {
			return condition->Evaluate(date, event);
		};
		AssertEqual(db.RemoveIf(predicate), 1, "Db Add2-Del-Add 1");
		db.Add(d, "e1");
		AssertEqual(db.FindIf(empty_predicate).size(), 2, "Db Add2-Del-Add 2");
	}

	// Add
	{
		Database db;
		Date d(2019, 1, 1);
		db.Add(d, "e1");
		db.Add(d, "e1");
		istringstream is("date == 2019-01-01");
		auto condition = ParseCondition(is);
		auto predicate = [condition](const Date& date, const string& event) {
			return condition->Evaluate(date, event);
		};
		AssertEqual(db.FindIf(predicate).size(), 1, "Db Add Duplicates 1");
	}
	pair <Date, string> p{ {1, 1, 1}, " " };
	// Last
	{
		Database db;
		Date d(2019, 1, 1);
		Date d1(2019, 1, 2);
		Date d2(2018, 12, 22);
		db.Add(d1, "e1");
		db.Add(d2, "e2");
		p = { {2018, 12, 22} , "e2" };
		AssertEqual(db.Last(d), p, "Db Last 1");
		Date d3(2018, 12, 24);
		db.Add(d3, "e3");
		p = { {2018, 12, 24}, "e3" };
		AssertEqual(db.Last(d), p, "Db Last 2");

		// Get last event for date before first event 
		try {
			Date d4(2017, 2, 2);
			db.Last(d4);
			Assert(false, "Db Last 3");
		}
		catch (invalid_argument e) {
			// Pass
		}

		// Delete event and get last
		istringstream is("date == 2018-12-24");
		auto condition = ParseCondition(is);
		auto predicate = [condition](const Date& date, const string& event) {
			return condition->Evaluate(date, event);
		};
		db.RemoveIf(predicate);
		p = { {2018, 12, 22 }, "e2" };
		AssertEqual(db.Last(d), p, "Db Last 4");
		p = { { 2019, 1, 2 }, "e1" };
		AssertEqual(db.Last(d1), p, "Db Last 5");
		db.Add(d2, "e4");
		p = { { 2018, 12, 22 }, "e4" };
		AssertEqual(db.Last(d2), p, "Db Last 6");
	}

	// Del
	{
		Database db;
		db.Add({ 2019, 1, 1 }, "e1");
		db.Add({ 2019, 1, 1 }, "e2");
		db.Add({ 2018, 1, 7 }, "e3");
		db.Add({ 2018, 1, 7 }, "e4");
		istringstream is("date == 2018-01-07");
		auto condition = ParseCondition(is);
		auto predicate = [condition](const Date& date, const string& event) {
			return condition->Evaluate(date, event);
		};
		AssertEqual(db.RemoveIf(predicate), 2, "Db Del 1");
	}
	{
		Database db;
		db.Add({ 2019, 1, 1 }, "e1");
		db.Add({ 2019, 1, 1 }, "e2");
		db.Add({ 2018, 1, 7 }, "e3");
		db.Add({ 2018, 1, 7 }, "e4");
		istringstream is("date >= 2018-01-07 AND date <= 2020-01-01");
		auto condition = ParseCondition(is);
		auto predicate = [condition](const Date& date, const string& event) {
			return condition->Evaluate(date, event);
		};
		AssertEqual(db.RemoveIf(predicate), 4, "Db Del 2");
	}
	{
		Database db;
		db.Add({ 2019, 1, 1 }, "e1");
		db.Add({ 2019, 1, 1 }, "e2");
		db.Add({ 2018, 1, 7 }, "e3");
		db.Add({ 2018, 1, 7 }, "e4");
		AssertEqual(db.RemoveIf(empty_predicate), 4, "Db Del 3");
	}
	{
		Database db;
		db.Add({ 2019, 1, 1 }, "e1");
		db.Add({ 2019, 1, 1 }, "e2");
		db.Add({ 2018, 1, 7 }, "e3");
		db.Add({ 2018, 1, 7 }, "e4");
		istringstream is(R"(event == "e1")");
		auto condition = ParseCondition(is);
		auto predicate = [condition](const Date& date, const string& event) {
			return condition->Evaluate(date, event);
		};
		AssertEqual(db.RemoveIf(predicate), 1, "Db Del 4");
	}

	{
		Database db;
		db.Add({ 2019, 1, 1 }, "e1");
		db.Add({ 2019, 1, 1 }, "e2");
		db.Add({ 2018, 1, 7 }, "e3");
		db.Add({ 2018, 1, 7 }, "e4");
		istringstream is(R"(event == "e1" OR date == 2019-01-01)");
		auto condition = ParseCondition(is);
		auto predicate = [condition](const Date& date, const string& event) {
			return condition->Evaluate(date, event);
		};
		AssertEqual(db.RemoveIf(predicate), 2, "Db Del 5");
	}

	// Find
	{
		Database db;
		db.Add({ 2019, 1, 1 }, "e1");
		db.Add({ 2019, 1, 1 }, "e2");
		db.Add({ 2018, 1, 7 }, "e3");
		db.Add({ 2018, 1, 7 }, "e4");
		istringstream is("date == 2018-01-07");
		auto condition = ParseCondition(is);
		auto predicate = [condition](const Date& date, const string& event) {
			return condition->Evaluate(date, event);
		};
		AssertEqual(db.FindIf(predicate).size(), 2, "Db Find 1");
	}
	{
		Database db;
		db.Add({ 2019, 1, 1 }, "e1");
		db.Add({ 2019, 1, 1 }, "e2");
		db.Add({ 2018, 1, 7 }, "e3");
		db.Add({ 2018, 1, 7 }, "e4");
		istringstream is("date >= 2018-01-07 AND date <= 2020-01-01");
		auto condition = ParseCondition(is);
		auto predicate = [condition](const Date& date, const string& event) {
			return condition->Evaluate(date, event);
		};
		AssertEqual(db.FindIf(predicate).size(), 4, "Db Find 2");
	}
	{
		Database db;
		db.Add({ 2019, 1, 1 }, "e1");
		db.Add({ 2019, 1, 1 }, "e2");
		db.Add({ 2018, 1, 7 }, "e3");
		db.Add({ 2018, 1, 7 }, "e4");
		AssertEqual(db.FindIf(empty_predicate).size(), 4, "Db Find 3");
	}
	{
		Database db;
		db.Add({ 2019, 1, 1 }, "e1");
		db.Add({ 2019, 1, 1 }, "e2");
		db.Add({ 2018, 1, 7 }, "e3");
		db.Add({ 2018, 1, 7 }, "e4");
		istringstream is(R"(event == "e1")");
		auto condition = ParseCondition(is);
		auto predicate = [condition](const Date& date, const string& event) {
			return condition->Evaluate(date, event);
		};
		AssertEqual(db.FindIf(predicate).size(), 1, "Db Find 4");
	}

	{
		Database db;
		db.Add({ 2019, 1, 1 }, "e1");
		db.Add({ 2019, 1, 1 }, "e2");
		db.Add({ 2018, 1, 7 }, "e3");
		db.Add({ 2018, 1, 7 }, "e4");
		istringstream is(R"(event == "e1" OR date == 2019-01-01)");
		auto condition = ParseCondition(is);
		auto predicate = [condition](const Date& date, const string& event) {
			return condition->Evaluate(date, event);
		};
		AssertEqual(db.FindIf(predicate).size(), 2, "Db Find 5");
	}

	// Add - Del - Add - Del
	{
		Database db;
		db.Add({ 2019, 1, 1 }, "e1");
		db.Add({ 2019, 1, 1 }, "e2");
		AssertEqual(db.RemoveIf(empty_predicate), 2, "Db Add-Del-Add-Del 1");

		db.Add({ 2019, 1, 1 }, "e1");
		db.Add({ 2019, 1, 1 }, "e2");
		AssertEqual(db.RemoveIf(empty_predicate), 2, "Db Add-Del-Add-Del 1");
	}

	/*{
		Database db;
		db.Add({ 2017, 1, 1 }, "first");
		db.Add({ 2017, 1, 1 }, "second");
		db.Add({ 2017, 1, 1 }, "third");
		db.Add({ 2017, 1, 1 }, "fourth");
		db.Add({ 2017, 1, 1 }, "five");
		AssertEqual(2, DoRemove(db, R"(event == "second" OR event == "fourth")"), "Remove several");
		ostringstream os;
		db.Print(os);
		AssertEqual("2017-01-01 first\n2017-01-01 third\n2017-01-01 five\n", os.str(), "Check print after remove several- 3");
	}*/
}



int main() {


	Database db;
	TestRunner tr;

	//tr.RunTest(TestEmptyNode, "Тест 2 из Coursera");
	 //tr.RunTest(TestDbAdd, "Тест 3(1) из Coursera");
	//tr.RunTest(TestDbFind, "Тест 3(2) из Coursera");
	 //tr.RunTest(TestDbLast, "Тест 3(3) из Coursera");
	//tr.RunTest(TestDbRemoveIf, "Тест 3(4) из Coursera");
	 //tr.RunTest(TestInsertionOrder, "Тест на порядок вывода");
	//tr.RunTest(TestsMyCustom, "Мои тесты");
	// tr.RunTest(TestDatabase, "Тест базы данных с GitHub");




	for (string line; getline(cin, line); ) {
		istringstream is(line);

		string command;
		is >> command;
		if (command == "Add") {
			const auto date = ParseDate(is);
			const auto event = ParseEvent(is);
			db.Add(date, event);
		}
		else if (command == "Print") {
			db.Print(cout);
		}

		else if (command == "Del") {
			auto condition = ParseCondition(is);

			auto predicate = [condition](const Date& date, const string& event) {
				return condition->Evaluate(date, event);
			};

			int count = db.RemoveIf(predicate);
			cout << "Removed " << count << " entries" << endl;
		}

		else if (command == "Find") {
			auto condition = ParseCondition(is);
			auto predicate = [condition](const Date& date, const string& event) {
				return condition->Evaluate(date, event);
			};
			const auto entries = db.FindIf(predicate);
			for (const auto& entry : entries) {
				cout << entry << endl;
			}
			cout << "Found " << entries.size() << " entries" << endl;
		}

		else if (command == "Last") {
			try {
				cout << db.Last(ParseDate(is)) << endl;
			}
			catch (invalid_argument&) {
				cout << "No entries" << endl;
			}
		}
		else if (command.empty()) {
			continue;
		}
		else {
			throw logic_error("Unknown command: " + command);
		}


	}

	return 0;
}


