#pragma once
#include "date.h"

//using namespace std;

class Database {
public:
	void Add(const Date& date, const string& event);

	void Print(ostream& os) const;

	// при вынесении определени€ в cpp не компилируетс€
	template <typename T>
	int RemoveIf(const T& predicate) {

		
		int counter{ 0 };
		bool flag = true;
		for ( auto& item : storage) {
			auto cur = item.second.first.begin();

			while (cur != item.second.first.end()) {
				if (predicate(item.first, *cur)) {
					
					//if (flag) {
					//	if (storage[item.first].second.back() == *cur) {
					//		storage[item.first].second.pop_back();
					//	}
					//	else {
					//		flag = false;
					//	}
					//}

					auto it = find(storage[item.first].second.begin()
						, storage[item.first].second.end(), *cur);
					if (it != storage[item.first].second.end()) {

						stable_partition(it, storage[item.first].second.end(),
							[&cur](string s) {
								return s != *cur;
							});

						storage[item.first].second.pop_back();
					}


					cur = (storage[item.first].first).erase(cur);

					

					++counter;


				}
				else {
					++cur;
				}
			}

			

			//flag = true;
			








			/*vector<string> v{};

			for (auto it = item.second.first.begin(); it != item.second.first.end(); ++it) {
				if (predicate(item.first, *it)) {
					v.push_back(*it);
				}
			}
			counter += DeleteEvent({item.first, item.second}, v);*/
		}

		
		DeleteEmptyDate();
		


		return counter;
	};

	template <typename T>
	vector<string> FindIf(const T& predicate) const{
		vector<string> v{};

		for (const auto& item : storage) {
			for (const auto& el : item.second.second) {
				if (item.second.first.count(el) && predicate(item.first, el) ) {
					v.push_back(ParseDareToString(item.first) + ' ' + el);
				}
			}
		}


		/*for (const auto& item : storage) {
			for (const auto& el : item.second) {
				if (predicate(item.first, el)) {
					
					v.push_back(ParseDareToString(item.first) + ' ' + el);
				}
			}
		}*/

		return v;
	}

	pair<Date, string> Last(const Date& date) const;

private:

	void DeleteEmptyDate();


	int DeleteEvent(const pair<Date, pair<set<string>, vector<string>>>& p,
		const vector<string>& v);

	//map<Date, vector<string>> storage;

	map<Date, pair<set<string>, vector<string>>> storage;

	


};




