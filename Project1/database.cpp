#include "database.h"


void Database::Add(const Date& date, const string& event) {

	auto p_it = storage[date].first.insert(event);
	if (p_it.second) {
		
		storage[date].second.push_back(event);


		/*auto it = find(storage[date].second.begin(), storage[date].second.end(), event);
		if (it != storage[date].second.end()) {

			stable_partition(it, storage[date].second.end(),
				[&event](string s) {
				return s != event;
			});
		}
		else {
			storage[date].second.push_back(event);
		}*/

	}

}


void Database::Print(ostream& os) const {
	for (const auto& item : storage) {
		for (const string& event : item.second.second) {
			if (item.second.first.count(event)) {
				os << item.first << " " << event << endl;
			}
		}
	}

}


pair<Date, string> Database::Last(const Date& date) const {

	auto it = storage.upper_bound(date);
	if (it != storage.begin()) {

		for (auto iter = prev(it)->second.second.rbegin();
			iter != prev(it)->second.second.rend();
			++iter)
		{
			if (prev(it)->second.first.count(*iter)) {
				return { prev(it)->first, *iter };
			}
		}




		/*for (int i = prev(it)->second.second.size() - 1; i >= 0; --i) {
			if (prev(it)->second.first.count(prev(it)->second.second[i])) {
				return { prev(it)->first, prev(it)->second.second[i] };
			}
		}*/
	}
	throw invalid_argument("");



}

void Database::DeleteEmptyDate() {

	auto cur = storage.begin();

	while (cur != storage.end())
	{
		if (cur->second.first.empty()) {
			cur = storage.erase(cur);
		}
		else
		{
			++cur;
		}
	}

	
}

int Database::DeleteEvent(const pair<Date, pair<set<string>, vector<string>>>& p,
	const vector<string>& v) {

	if (v.size() == storage[p.first].first.size()) {
		storage[p.first].first.clear();
		storage[p.first].second.clear();
		return v.size();
	}
	else {
		bool flag = true;
		for (auto it = v.rbegin(); it != v.rend(); ++it) {
			storage[p.first].first.erase(*it);

			if (flag) {
				if (storage[p.first].second.back() == *it) {
					storage[p.first].second.pop_back();
				}
				else {
					flag = false;
				}
			}

		}
		return v.size();

		
		
	}

}




