
#include<bits/stdc++.h>
using namespace std;

#define x first
#define y second


mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());





// weighted random selection

template<typename T>
class RandomSelection {

public:
	vector<T>w;
	RandomSelection() {}
	RandomSelection(vector<T>a) {
		w.resize(a.size());
		T sum = 0;
		sum = accumulate(a.begin(), a.end(), sum);
		w[0] = a[0] / sum;
		for (int i = 1; i < a.size(); i++) {
			w[i] = w[i - 1] + a[i] / sum;
		}
	}
	int select() {
		T L = w.front();
		T R = w.back();
		T randnum = uniform_real_distribution<T>(0, 1)(rng);
		int j = 0;
		for (auto i : w) {
			if (i >= randnum) {
				return j;
			}
			j++;
		}
		return j;
	}

};


template<typename T>
class AlogoHUIM_ABC {

public:
	const static int nectar_souces = 30;
	const static int iterations = 20000;
	const static int limit = 10;
	const static int times = 10;
	const static int MAX_ITEMS = 2000;
	const static int bucket_num = 20;
	const static int MAX_TRAN = 3200;
	const static int stuck = 10;

	T minutil;
	int max_k;
	int n;
	int m;
	vector<vector<pair<int, T>>> database;
	vector<T>w;
	map<int, int>key;
	map<int, int>rev_key;
	vector<double> scout_bucket;
	set<pair<int, vector<int>>>HUI;
	class Item {
	public:
		int item;
		bitset<MAX_TRAN> TIDS;
		vector<T> cost;
		Item() {}
		Item(int item) :item(item) {}
	};

	vector<Item>items;

	RandomSelection<T> percentage; // select which bit to on
	RandomSelection<T> select_onlooker;
	RandomSelection<T> select_scout;

	class BeeGroup {
	public:
		bitset<MAX_ITEMS> X;
		int fitness;
		int trail;
		BeeGroup() {
			fitness = 0;
			trail = 0;
		}
		void addtrail(int k) {
			trail += k;
		}
	};

	vector<BeeGroup> nectar, OnLooker, Employed, Scout;
	BeeGroup New_Group(int k) {
		int j = 0;
		BeeGroup new_grp;
		while (j != k) {
			int i = percentage.select();
			while (new_grp.X[i]) {
				i = percentage.select();
			}
			new_grp.X[i] = 1;
			j++;
		}
		return new_grp;
	}
	vector<int> check_PBV(BeeGroup& tempnode) {
		bitset<MAX_TRAN> X;
		for (int i = 0; i < m; i++) {
			X[i] = 1;
		}
		for (int i = 0; i < n; i++) {
			if (tempnode.X[i]) {
				X &= items[i].TIDS;
			}
		}
		vector<int> list;
		for (int i = 0; i < m; i++) {
			if (X[i]) {
				list.push_back(i);
			}
		}
		return list;
	}
	T fitness(BeeGroup& tempnode, vector<int>& list) {
		T fit = 0;
		vector<int>elements;
		for (int j = 0; j < n; j++) {
			if (tempnode.X[j]) {
				elements.push_back(j);
			}
		}
		for (auto i : elements) {
			for (auto j : list) {
				fit += items[i].cost[j];
			}
		}
		return fit;
	}
	void update_scout(int r) {
		int i = r / bucket_num;
		if (i < scout_bucket.size()) {
			scout_bucket[i]++;
		}
		else {
			scout_bucket[(int)scout_bucket.size() - 1]++;
		}
	}
	void change_kth_bit(BeeGroup& tempnode, int num) {
		for (int i = 0; i < num; i++) {
			int count = tempnode.X.count();

			if (count > 5) {
				// turn off
				//cout << "off" << endl;
				vector<int> temp;

				for (int i = 0; i < n; i++) {
					if (tempnode.X[i]) {
						temp.push_back(i);
					}
				}

				int k = rng() % temp.size();

				tempnode.X[temp[k]] = 0;
				continue;

			}
			int k = rng() % n;
			tempnode.X[k] = 1 - tempnode.X[k];
		}

	}

	void produce_new(BeeGroup& bestnode) {
		BeeGroup tempnode = bestnode;
		const int change = 1;

		int j = 0;

		do {
			vector<int>templist;
			int li = stuck;
			while (li--) {
				change_kth_bit(tempnode, 1);
				templist = check_PBV(tempnode);


				if (templist.size() > 0)break;
			}

			tempnode.fitness = fitness(tempnode, templist);

			if (tempnode.fitness > bestnode.fitness) {
				bestnode = tempnode;
			}
			else {
				tempnode = bestnode;
			}

			j++;

		} while (bestnode.fitness < minutil and j < times);

		if (bestnode.fitness >= minutil) {

			vector<int>elements;

			for (int i = 0; i < n; i++) {
				if (bestnode.X[i]) {
					elements.push_back(i);
				}
			}

			if (HUI.count({ bestnode.fitness, elements }) == 0) {
				update_scout(elements.size());
				HUI.insert({ bestnode.fitness, elements });
			}
		}
	}

	AlogoHUIM_ABC(vector<vector<pair<int, T>>>raw_database, T minutil) :minutil(minutil) {

		map<int, T> ItemTwu;
		for (auto i : raw_database) {
			T util = 0;
			for (auto j : i) {
				util += j.y;
			}
			for (auto j : i) {
				ItemTwu[j.x] += util;
			}
		}
		// ItemTwu: key --> Item, value --> TWU of that item

		vector<int>NonTwu;
		// NonTwu: items whoose TWU < minutil
		for (auto i : ItemTwu) {
			if (i.y < minutil) {
				NonTwu.push_back(i.x);
			}
		}
		// erasing NonTwu items;
		for (auto i : NonTwu) {
			ItemTwu.erase(i);
		}
		int id = 0;

		// mapping the items
		// example if items are = {2,5,3,6} , they will be mapped as = {1,3,2,4}
		for (auto i : ItemTwu) {
			key[i.x] = id++;
			rev_key[id - 1] = i.x;
			w.push_back(i.y);
		}
		// pattern: Item (mapped_number) with TWU

		for (auto i : raw_database) {
			vector<pair<int, T>> revised_tran;
			for (auto j : i) {
				if (ItemTwu.count(j.x)) {
					revised_tran.push_back({ key[j.x],j.y });
				}
			}
			max_k = max(max_k, (int)revised_tran.size());
			if (revised_tran.size() > 0) {
				database.push_back(revised_tran);
			}
		}
		items.resize(ItemTwu.size());
		n = items.size();
		m = database.size();
		for (int i = 0; i < items.size(); i++) {
			items[i].item = i;
			items[i].cost.resize(m);
		}
		for (int i = 0; i < database.size(); i++) {
			for (int j = 0; j < database[i].size(); j++) {
				items[database[i][j].x].TIDS[j] = 1;
				items[database[i][j].x].cost[j] = database[i][j].y;
			}
		}
		scout_bucket.resize(items.size() / bucket_num);
		for (int i = 0; i < scout_bucket.size(); i++) {
			scout_bucket[i] = 1;
		}
		OnLooker.resize(nectar_souces);
		Employed.resize(nectar_souces);
	}



	void Initialization() {
		percentage = RandomSelection<T>(w);
		int s = 0;

		for (int i = 0; i < nectar_souces; i++) {
			BeeGroup tempnode, besttempnode;

			vector<int> templist;
			int j = 0;

			do {
				while (true) {
					int k = rng() % max_k + 1;
					tempnode = New_Group(k);
					templist = check_PBV(tempnode);

					if (templist.size() > 0)break;
				}

				tempnode.fitness = fitness(tempnode, templist);

				if (tempnode.fitness >= besttempnode.fitness) {
					besttempnode = tempnode;
				}

				j++;

			} while (besttempnode.fitness < minutil and j < times);

			besttempnode.trail = 0;

			vector<int> elements;
			nectar.push_back(besttempnode);

			Employed[i] = besttempnode;


			for (int i = 0; i < n; i++) {
				if (besttempnode.X[i]) {
					elements.push_back(i);
				}
			}

			if (besttempnode.fitness >= minutil) {
				if (HUI.count({ besttempnode.fitness, elements }) == 0) {
					update_scout(elements.size());
					HUI.insert({ besttempnode.fitness,elements });
				}
			}
		}
	}



	void EmployedBee() {
		Employed = nectar;
		BeeGroup temp;

		for (int i = 0; i < nectar_souces; i++) {

			//cout<<"#"<<i<<endl;

			produce_new(Employed[i]);

			//cout<<"#"<<"done"<<endl;

			if (Employed[i].fitness > nectar[i].fitness) {
				nectar[i] = Employed[i];
			}
			else {
				nectar[i].addtrail(1);
			}
		}
	}


	void OnLookerBee() {
		vector<T> fitness_select(nectar_souces);
		for (int i = 0; i < nectar_souces; i++) {
			fitness_select[i] = nectar[i].fitness;
		}

		select_onlooker = RandomSelection<T>(fitness_select);

		for (int i = 0; i < nectar_souces; i++) {
			int j = select_onlooker.select();

			OnLooker[i] = nectar[j];
			produce_new(OnLooker[i]);

			if (OnLooker[i].fitness > nectar[j].fitness) {
				nectar[j] = OnLooker[i];
			}
			else {
				nectar[j].addtrail(1);
			}

		}
	}

	void ScoutBee() {
		select_scout = RandomSelection<T>(scout_bucket);


		for (int i = 0; i < nectar_souces; i++) {
			if (nectar[i].trail > limit) {
				BeeGroup tempnode, bestnode;

				vector<int>templist;

				int j = 0;

				int bucket_length = scout_bucket.size();

				do {
					while (true) {
						int k = select_onlooker.select();
						k = k * bucket_num + rng() % bucket_num;
						if (k > max_k) {
							k = rng() % max_k + 1;
						}
						tempnode = New_Group(k);

						templist = check_PBV(tempnode);

						if (templist.size() > 0)break;

					}
					tempnode.fitness = fitness(tempnode, templist);

					if (tempnode.fitness > bestnode.fitness) {
						bestnode = tempnode;
					}
					j++;
				} while (bestnode.fitness < minutil and j < times);

				bestnode.trail = 0;
				nectar[i] = bestnode;

				vector<int>elements;

				for (int i = 0; i < n; i++) {
					if (bestnode.X[i]) {
						elements.push_back(i);
					}
				}
				if (bestnode.fitness >= minutil) {
					if (HUI.count({ bestnode.fitness, elements }) == 0) {
						update_scout(elements.size());
						HUI.insert({ bestnode.fitness, elements });
					}
				}

			}
		}
	}
	void print(double* total_utility, int* mn, int* mx) {
		int cnt = 0;
		for (auto i : HUI) {
			*total_utility += i.first;
			*mx = max(*mx, i.first);
			*mn = min(*mn, i.first);
			cnt++;
			cout << "HUI #:" << cnt << endl;
			cout << "Utility value: " << i.first << endl;
			cout << "Itemset: ";
			for (auto j : i.second) cout << rev_key[j] << " ";
			cout << endl;
		}
	}

};

int main() {
	freopen("3000_large/3000_large.txt", "r", stdin);
	int totalTransactions = 0;
	vector<vector<pair<int, double>>>database;
	while (!cin.eof()) {
		totalTransactions++;
		string s;
		getline(cin, s);
		vector<int>item;
		vector<double>cost;
		int cnt = 0;
		int num = 0;
		for (auto i : s) {
			if (i == ':') {
				if (cnt == 0) {
					item.push_back(num);
					num = 0;
				}
				cnt++;continue;
			}
			if (cnt == 1) {
				continue;
			}
			if (cnt == 0) {
				if (i == ' ')item.push_back(num), num = 0;
				else num = num * 10 + i - 48;
			}
			else {
				if (i == ' ' || i == 13)cost.push_back(num), num = 0;
				else num = num * 10 + i - 48;
			}
		}
		cost.push_back(num);
		vector<pair<int, double>>tran;
		for (int i = 0;i < item.size();i++) {
			tran.push_back({ item[i],cost[i] });
		}
		database.push_back(tran);

	}
	int minutil = 100;
	AlogoHUIM_ABC<double> abc(database, minutil);
	abc.Initialization();
	int iter = 20000;
	int cnt = 0;
	while (iter--) {
		abc.EmployedBee();
		abc.OnLookerBee();
		abc.ScoutBee();
		cnt++;
		cout << "AFTER " << cnt << " ITERATION: " << "HUIS found ";
		cout << abc.HUI.size() << endl;
	}

	freopen("3000_large/output_abc.txt", "w", stdout);
	double total_utility = 0;
	int minimum_util = INT_MAX, maximum_util = 0;
	abc.print(&total_utility, &minimum_util, &maximum_util);

	freopen("3000_large/stat_abc.txt", "w", stdout);

	double t = 1.0 * clock() / CLOCKS_PER_SEC;
	cout << "Total No of Transactions: " << totalTransactions << endl;
	cout << "TIME TAKEN : " << t << endl;
	cout << "efficiency: " << abc.HUI.size() / t << endl;
	cout << "No of Iterations: " << cnt << endl;
	cout << "Total HUIS found: " << abc.HUI.size() << endl;
	cout << "MinUtility required for HUIM: " << minutil << endl;
	cout << "Minimum utility among HUIs found: " << minimum_util << endl;
	cout << "Maximum Utility among HUIs found: " << maximum_util << endl;
	cout << "Average Utility: " << total_utility / abc.HUI.size() << endl;

}










// WE have removed the condition if hui already contains nector
// also gave last element leverage of 0.1
