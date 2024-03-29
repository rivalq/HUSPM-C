/*
 * Created on Sun Feb 20 2022
 *
 * Author: Jatin Garg, Lakshay Jindal, Ojus Bhutani
 */
const int MAX_ITEMS = 1200;

template<typename T, typename U>
struct _QItem {
	T id;
	U cost;
	int quantity;
	_QItem(T id) : id(id) {}
	_QItem(T id, int quantity, U cost) : id(id), quantity(quantity), cost(cost) {}

	bool operator < (const _QItem& rhs) const {
		return id < rhs.id;
	}
	friend ostream& operator << (ostream& out, _QItem qt) {
		out << format("[ id = %d, quantity = %d, cost = %d ]", qt.id, qt.quantity, qt.cost) << endl;
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
		out << format("[ id = %d ]", qt.id) << endl;
		return out;
	}
};

using Item = _Item<int>;
using QItem = _QItem<int, int>;

template<typename T, typename U>
struct _UtilArray {
	int eid, next_pos, next_eid;
	T id;
	U u, ru;
	map<T, int> next_mp;
	_UtilArray() {}
	_UtilArray(int eid, T id, U u, U ru, int next_pos, int next_eid) : eid(eid), id(id), u(u), ru(ru), next_pos(next_pos), next_eid(next_eid) {}

	friend ostream& operator << (ostream& out, _UtilArray ut) {
		return out << format("[ id = %c, eid = %d, u = %d, ru = %d, next_pos = %d, next_eid = %d ]", ut.id, ut.eid, ut.u, ut.ru, ut.next_pos, ut.next_eid) << endl;
	}

};
using UtilArray = _UtilArray<int, int>;


/**
 * @ return utility of given itemset (st_1) in given q-itemset (st_2). If st_1 is not subset of st_2 return error of Not subset
 *
 */
template<typename T = int>
Result < T, ErrorCode> utility(set<Item> st_1, set<QItem> st_2) {
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
		}
	}
	return Result<T, ErrorCode>{.type = ResultType::Ok, .value = utility};
}


template<typename T, typename U>
struct _QSequence {
	deque<set<QItem>> vals;
	deque<bitset<MAX_ITEMS>> bitvec;
	vector<UtilArray> ut_arr;
	_QSequence(deque<set<QItem>> vals) :vals(vals) {
		bitvec.resize(vals.size());
		for (int i = 0; i < vals.size(); i++) {
			for (auto j : vals[i])bitvec[i][j.id] = 1;
		}
	}
	int match = 0;
	U prefix_util = 0;
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
		deque<set<QItem>> new_vals;
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
		bitvec.clear();
		bitvec.resize(vals.size());
		for (int i = 0; i < vals.size(); i++) {
			for (auto j : vals[i])bitvec[i][j.id] = 1;
		}
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
				ut_arr[id].next_mp = next_pos;
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
				auto res = utility(seq[j - 1], st);
				if (res.type == ResultType::Ok and dp[j - 1].type == ResultType::Ok) {
					if (dp[j].type == ResultType::Error or dp[j].value < res.value + dp[j - 1].value) {
						dp[j] = Result<U, ErrorCode>{ .type = ResultType::Ok, .value = dp[j - 1].value + res.value };
					}
				}
			}
		}
		return dp[m];
	}


	int get_utility(bitset<MAX_ITEMS>& bt, set<QItem>& st) {
		int res = 0;
		for (int i = bt._Find_first(); i < bt.size(); i = bt._Find_next(i)) {
			auto item = static_cast<QItem>(i);
			auto itr = st.lower_bound(item);
			assert(itr->id == i);
			auto p = *itr;
			res += p.absolute_utility();
		}
		return res;
	}

	int get_utility(vector<bitset<MAX_ITEMS>> bt) {
		int m = bt.size();
		int n = vals.size();
		vector<int> dp(m + 1, INT_MIN);
		dp[0] = 0;
		for (int i = 0; i < n; i++) {
			for (int j = m; j >= 1; j--) {
				if ((bt[j - 1] & bitvec[i]) == bt[j - 1]) {
					dp[j] = max(dp[j], dp[j - 1] + get_utility(bt[j - 1], vals[i]));
				}
			}
		}
		return dp[m];
	}

	void get_i_items(set<Item>& st) {
		int match_eid = -1;
		if (match > 0) {
			match_eid = ut_arr[match].eid;
		}
		for (int j = match + 1; j < ut_arr.size(); j++) {
			if (ut_arr[j].eid != match_eid)break;
			st.insert(ut_arr[j].id);
		}
	}
	void get_s_items(set<Item>& st) {
		int j = 1;
		if (match > 0) {
			j = ut_arr[j].next_eid;
		}
		if (j == -1)return;
		for (j;j < ut_arr.size(); j++) {
			st.insert(ut_arr[j].id);
		}
	}
	Result<U, ErrorCode> get_upper_bound(bitset<MAX_ITEMS> bt) {
		assert(bt.count() > 0);
		int next_eid = 1;
		if (match != 0)next_eid = ut_arr[match].next_eid;
		if (next_eid == -1) {
			return Result<U, ErrorCode>{.type = ResultType::Error, .error = ErrorCode::NOT_FOUND};
		}
		int id = next_eid;
		next_eid = ut_arr[next_eid].eid;
		for (int j = next_eid; j < bitvec.size(); j++) {
			if ((bt & bitvec[j]) == bt) {
				int bound = prefix_util;
				int next_next = ut_arr[id].next_eid;
				if (next_next != -1)bound += ut_arr[next_next].ru;
				int l = bt._Find_first();
				while (l != MAX_ITEMS and id < next_next) {
					if (ut_arr[id].id < l)id++;
					else if (ut_arr[id].id == l) {
						bound += ut_arr[id].u;
						id++;
						l = bt._Find_next(l);
					}
					else {
						l = bt._Find_next(l);
					}
				}
				return Result<U, ErrorCode>{.type = ResultType::Ok, .value = bound};
			}
			id += bitvec[j].count();
		}
		return Result<U, ErrorCode>{.type = ResultType::Error, .error = ErrorCode::NOT_FOUND};
	}

	Result<U, ErrorCode> get_upper_bound(Item it, bool i_item = 1) {
		if (match == 0 and i_item)return Result<U, ErrorCode>{.type = ResultType::Error, .error = ErrorCode::NOT_FOUND};
		if (i_item) {
			auto& mp = ut_arr[match].next_mp;
			if (mp.count(it.id) == 0) {
				return Result<U, ErrorCode>{.type = ResultType::Error, .error = ErrorCode::NOT_FOUND};
			}
			int j = mp[it.id];
			if (ut_arr[j].eid != ut_arr[match].eid) {
				return Result<U, ErrorCode>{.type = ResultType::Error, .error = ErrorCode::NOT_FOUND};
			}
			U val = prefix_util + ut_arr[j].ru + ut_arr[j].u;
			return Result<U, ErrorCode>{.type = ResultType::Ok, .value = val};
		}
		else {
			int next_eid = 1;
			if (match != 0)next_eid = ut_arr[match].next_eid;
			if (next_eid == -1) {
				return Result<U, ErrorCode>{.type = ResultType::Error, .error = ErrorCode::NOT_FOUND};
			}
			auto& mp = ut_arr[next_eid].next_mp;
			if (mp.count(it.id) == 0) {
				return Result<U, ErrorCode>{.type = ResultType::Error, .error = ErrorCode::NOT_FOUND};
			}
			int j = mp[it.id];
			U val = prefix_util + ut_arr[j].ru + ut_arr[j].u;
			return Result<U, ErrorCode>{.type = ResultType::Ok, .value = val};
		}
	}
	void increment_prefix(Item it, bool i_item = 1) {
		if (i_item) {
			assert(match != 0);
			assert(ut_arr[match].next_mp.count(it.id));
			int next_match = ut_arr[match].next_mp[it.id];
			assert(ut_arr[next_match].eid == ut_arr[match].eid);
			prefix_util += ut_arr[next_match].u;
			match = next_match;
		}
		else {
			int next_eid = 1;
			if (match != 0)next_eid = ut_arr[match].next_eid;
			assert(next_eid != -1);
			assert(ut_arr[next_eid].next_mp.count(it.id));
			int next_match = ut_arr[next_eid].next_mp[it.id];
			prefix_util += ut_arr[next_match].u;
			match = next_match;
		}
	}


	bool check_PBV(bitset<MAX_ITEMS> bt) {
		for (auto i : bitvec) {
			if ((i & bt) == bt)return true;
		}
		return false;
	}


};
using QSequence = _QSequence<int, int>;
using Sequence = vector<set<Item>>;


template<typename T, typename U>
struct _QDatabase {
	vector < QSequence > database;
	U min_util;
	int num_items, max_k;

	_QDatabase() {}
	_QDatabase(vector<QSequence> database, int num_items, int max_k) :database(database), num_items(num_items), max_k(max_k) {}

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
		}
	}

	vector<int> check_PBV(bitset<MAX_ITEMS> bt) {
		vector<int> res;
		for (int i = 0; i < database.size(); i++) {
			if (database[i].check_PBV(bt)) {
				res.push_back(i);
			}
		}
		return res;
	}
};

using QDatabase = _QDatabase<int, int>;