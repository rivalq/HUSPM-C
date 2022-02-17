
template<typename T, typename U>
struct _QItem {
	T id;
	U cost;
	int quantity;
	_QItem(T id) :id(id) {}
	_QItem(T id, int quantity, U cost) : id(id), quantity(quantity), cost(cost) {}

	bool operator < (const _QItem& rhs) const {
		return id < rhs.id;
	}
	friend ostream& operator << (ostream& out, _QItem qt) {
		out << format("[ id = %c, quantity = %d, cost = %d ]", qt.id, qt.quantity, qt.cost) << endl;
		return out;
	}

	_QItem operator-> () {
		return _QItem(this->id, this->quantity, this->cost);
	}

	_QItem operator*() {
		return *this;
	}

	U absolute_utility() {
		return cost * quantity;
	}

};



template<typename T>
struct _Item {
	T id;
	_Item(T id) : id(id) {}
	bool operator < (const _Item& rhs) const {
		return id < rhs.id;
	}
	friend ostream& operator << (ostream& out, _Item qt) {
		out << format("[ id = %c ]", qt.id) << endl;
		return out;
	}
};

using Item = _Item<char>;
using QItem = _QItem<char, int>;

template<typename T, typename U>
struct _UtilArray {
	int eid, next_pos, next_eid;
	T id;
	U u, ru;
	_UtilArray() {}
	_UtilArray(int eid, T id, U u, U ru, int next_pos, int next_eid) : eid(eid), id(id), u(u), ru(ru), next_pos(next_pos), next_eid(next_eid) {}

	friend ostream& operator << (ostream& out, _UtilArray ut) {
		return out << format("[ id = %c, eid = %d, u = %d, ru = %d, next_pos = %d, next_eid = %d ]", ut.id, ut.eid, ut.u, ut.ru, ut.next_pos, ut.next_eid) << endl;
	}

};
using UtilArray = _UtilArray<char, int>;


/**
 * @ return utility of given itemset (st_1) in given q-itemset (st_2). If st_1 is not subset of st_2 return error of Not subset
 *
 */
template<typename T>
Result < T, ErrorCode> get_utility(set<Item> st_1, set<QItem> st_2) {
	if (st_1.size() > st_2.size())return Result<T, ErrorCode> {.type = ResultType::Error, .error = ErrorCode::NOT_SUBSET};
	T utility = 0;
	for (auto i : st_1) {
		auto item = static_cast<QItem>(i.id);
		auto itr = st_2.lower_bound(item);
		if (itr == st_2.end() or itr->id != i.id) {
			return Result<T, ErrorCode> {.type = ResultType::Error, .error = ErrorCode::NOT_SUBSET};
		}
		else {
			auto p = *itr;
			utility += p.absolute_utility();
			//utility += itr->absolute_utility();
		}
	}
	return Result<T, ErrorCode>{.type = ResultType::Ok, .value = utility};
}


template<typename T, typename U>
struct _QSequence {
	vector<set<QItem>> vals;
	vector<UtilArray> ut_arr;
	_QSequence(vector<set<QItem>> vals) :vals(vals) {}

	U get_utility() {
		U res = 0;
		for (auto i : vals) {
			for (auto j : i) {
				res += j.cost * j.quantity;
			}
		}
		return res;
	}
	set<Item> get_items() {
		set<Item> res;
		for (auto i : vals) {
			for (auto j : i) {
				res.insert(Item(j.id));
			}
		}
		return res;
	}

	size_t length() {
		size_t l = 0;
		for (auto i : vals) l += i.size();
		return l;
	}

	void remove_items(const set<Item>& st) {
		vector<set<QItem>> new_vals;
		for (auto i : vals) {
			set<QItem> new_itemset;
			for (auto j : i) {
				if (st.count(Item(j.id)) == 0) {
					new_itemset.insert(j);
				}
			}
			if (new_itemset.size() > 0) {
				new_vals.push_back(new_itemset);
			}
		}
		swap(vals, new_vals);
	}
	//we will have 1-based indexing
	vector<UtilArray> construct_util_array() {
		int id = length();
		int eid = vals.size();
		int next_eid = -1;
		U ru = static_cast<U>(0);
		ut_arr.resize(id + 1);
		map<T, int> next_pos;
		for (auto st : vals | views::reverse) {
			for (auto it : st | views::reverse) {
				ut_arr[id].id = it.id;
				ut_arr[id].eid = eid;
				ut_arr[id].u = it.absolute_utility();
				ut_arr[id].ru = ru;
				ut_arr[id].next_eid = next_eid;
				ru += ut_arr[id].u;
				if (next_pos.count(it.id)) {
					ut_arr[id].next_pos = next_pos[it.id];
				}
				else {
					ut_arr[id].next_pos = -1;
				}
				next_pos[it.id] = id;
				id--;
			}
			next_eid = id + 1;
			eid--;
		}
		return ut_arr;
	}


	/*
	* @ return max utility of seq in current qsequence
	*/
	Result<U, ErrorCode> get_utility(vector<set<Item>> seq) {
		int n = vals.size();
		int m = seq.size();
		vector<Result<U, ErrorCode>> dp(m + 1);
		dp[0] = Result <U, ErrorCode>{ .type = ResultType::Ok, .value = 0 };
		for (int i = 1; i <= m; i++) {
			dp[i] = Result<U, ErrorCode>{ .type = ResultType::Error, .error = ErrorCode::INVALID };
		}
		for (auto st : vals) {
			for (int j = m; j >= 1; j--) {
				auto res = get_utility(seq[j - 1], st);
				if (res.type == ResultType::Ok and dp[j - 1].type == ResultType::Ok) {
					if (dp[j].type == ResultType::Error or dp[j].value < res.value + dp[j - 1].value) {
						dp[j] = Result<U, ErrorCode>{ .type = ResultType::Ok, .value = dp[j - 1].value + res.value };
					}
				}
			}
		}
		return dp[m];
	}

};
using QSequence = _QSequence<char, int>;

template<typename T, typename U>
struct QDatabase {
	vector < QSequence > database;
	U min_util;

	QDatabase(vector<QSequence> database) :database(database) {}

	U get_utility() {
		U res = 0;
		for (auto i : database) {
			res += i.get_utility();
		}
		return res;
	}
	map <Item, U> get_swu_values() {
		map<Item, U> res;
		for (auto i : database) {
			auto sequence_util = i.get_utility();
			auto items = i.get_items();
			for (auto i : items) {
				res[i] += sequence_util;
			}
		}
		return res;
	}

	/*
		@Remove Items from database which are present in set st
	*/
	void remove_items(const set<Item>& st) {
		for (auto& i : database) {
			i.remove_items(st);
		}
	}

	/*
		@Remove 1-Sequences which has SWU < min_util
	*/
	set<Item> initial_prune() {
		auto mp = get_swu_values();
		set<Item> st;
		for (auto i : mp) {
			if (i.second < min_util) {
				st.insert(i.first);
			}
		}
		remove_items(st);
		return st;
	}
	void construct_util_array() {
		for (auto& i : database) {
			auto arr = i.construct_util_array();
#ifdef DEV
			for (auto j : arr | views::drop(1)) {
				cout << j << endl;
			}
			cout << "--------\n";
#endif
		}
	}
};