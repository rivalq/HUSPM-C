#include <bits/stdc++.h>
#include <ranges>

using namespace std;

#include "utils.h"
#include "format.h"
#include "core.h"
#include "parse.h"

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
string input, output;

namespace HUSPM_SOFT {
    map < set<Item>, pair<int, int>> HUI;

    template<typename T>
    class RandomSelection {
    public:
        vector<T>w;
        RandomSelection() {}
        RandomSelection(vector<T>a) {
            w.resize(a.size());
            T sum = 0;
            sum = accumulate(a.begin(), a.end(), sum);
            assert(sum > 0);
            w[0] = a[0] / sum;
            for (int i = 1; i < a.size(); i++) {
                w[i] = w[i - 1] + a[i] / sum;
            }
        }
        int select() {
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

    template <class D, class W, class URBG>
    void weighted_shuffle
    (D first, D last
        , W first_weight, W last_weight
        , URBG&& g)
    {
        while (first != last and first_weight != last_weight)
        {
            std::discrete_distribution dd(first_weight, last_weight);
            auto i = dd(g);
            if (i)
            {
                std::iter_swap(first, std::next(first, i));
                std::iter_swap(first_weight, std::next(first_weight, i));
            }
            ++first;
            ++first_weight;
        }
    }


    class ABC_HUIM {
        const static int nectar_souces = 100;
        const static int limit = 10;
        const static int times = 10;
        const static int bucket_num = 20;
        const static int MAX_TRAN = 3200;
        const static int stuck = 10;
    public:
        const static int iterations = 20000;

        QDatabase db;
        int num_items;
        int max_k = 1;
        map<Item, int> Item_utils, rank;
        vector<double> w;
        RandomSelection<double> percentage;
        vector<double> scout_bucket;
        ABC_HUIM(QDatabase db) {
            this->db = db;
        }

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
        BeeGroup New_Group(int k) {
            vector<int> items(num_items);
            iota(items.begin(), items.end(), 0);
            weighted_shuffle(items.begin(), items.end(), w.begin(), w.end(), rng);
            BeeGroup new_grp;
            for (int j = 0; j < k; j++)new_grp.X[j] = 1;
            return new_grp;
        }
        vector<BeeGroup> nectar, OnLooker, Employed, Scout;

        vector<int> check_PBV(BeeGroup& tempnode) {
            return db.check_PBV(tempnode.X);
        }

        int fitness_relaxed(BeeGroup& node) {
            auto ids = check_PBV(node);
            int res = 0;
            for (auto id : ids) {
                res += db.database[id].get_utility(vector<bitset<MAX_ITEMS>>{node.X});
            }
            return res;
        }
        int fitness(BeeGroup& node, vector<int> ids) {
            int res = 0;
            for (auto id : ids) {
                res += db.database[id].get_utility();
            }
            return res;
        }
        void update_scout(int r) {
            r /= bucket_num;
            assert(r < scout_bucket.size());
            scout_bucket[r]++;
        }

        void change_kth_bit(BeeGroup& tempnode, int num) {
            for (int i = 0; i < num; i++) {

                int count = 0;
                for (int j = 0; j < num_items; j++)count += tempnode.X[j];

                if (count == 0) {
                    int k = percentage.select();
                    tempnode.X[k] = 1;
                    continue;
                }
                if (count == max_k) {
                    vector<int> temp;
                    for (int i = tempnode.X._Find_first(); i < tempnode.X.size(); i = tempnode.X._Find_next(i)) {
                        temp.push_back(i);
                    }
                    assert(temp.size() == count);
                    int k = rng() % temp.size();
                    assert(k < num_items);
                    tempnode.X[temp[k]] = 0;
                    continue;
                }
                int k = rng() % num_items;
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
                    if (templist.size() > 0 and tempnode.X.count())break;
                }

                tempnode.fitness = (tempnode.X.count() == 0) ? 0 : fitness(tempnode, templist);

                if (tempnode.fitness > bestnode.fitness) {
                    bestnode = tempnode;
                }
                else {
                    tempnode = bestnode;
                }
                j++;

            } while (bestnode.fitness < db.min_util and j < times);

            if (bestnode.fitness >= db.min_util) {

                set<Item>elements;

                for (int i = 0; i < num_items; i++) {
                    if (bestnode.X[i]) {
                        elements.insert(i);
                    }
                }
                if (HUI.count({ elements }) == 0) {
                    update_scout(elements.size());
                    HUI[elements] = { bestnode.fitness ,fitness_relaxed(bestnode) };
                }
            }
        }

        void init() {
            Item_utils = db.get_swu_values();
            num_items = db.num_items;
            max_k = db.max_k;
            vector<double> w(num_items);
            for (auto [item, util] : Item_utils) {
                assert(item.id < num_items);
                w[item.id] = util;
            }
            scout_bucket.resize(num_items / bucket_num + 1);
            for (int i = 0; i < scout_bucket.size(); i++) {
                scout_bucket[i] = 1;
            }
            OnLooker.resize(nectar_souces);
            Employed.resize(nectar_souces);
            percentage = RandomSelection<double>(w);
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

                } while (besttempnode.fitness < db.min_util and j < times);

                besttempnode.trail = 0;

                set<Item> elements;
                nectar.push_back(besttempnode);

                Employed[i] = besttempnode;

                for (int i = 0; i < num_items; i++) {
                    if (besttempnode.X[i]) {
                        elements.insert(i);
                    }
                }

                if (besttempnode.fitness >= db.min_util) {
                    if (HUI.count({ elements }) == 0) {
                        update_scout(elements.size());
                        HUI[elements] = { besttempnode.fitness ,fitness_relaxed(besttempnode) };
                    }
                }
            }
        }

        void EmployedBee() {
            Employed = nectar;
            BeeGroup temp;

            for (int i = 0; i < nectar_souces; i++) {
                produce_new(Employed[i]);
                if (Employed[i].fitness > nectar[i].fitness) {
                    nectar[i] = Employed[i];
                }
                else {
                    nectar[i].addtrail(1);
                }
            }
        }

        void OnLookerBee() {
            vector<double> fitness_select(nectar_souces);
            for (int i = 0; i < nectar_souces; i++) {
                fitness_select[i] = nectar[i].fitness;
            }

            auto select_onlooker = RandomSelection<double>(fitness_select);

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
            auto select_scout = RandomSelection<double>(scout_bucket);


            for (int i = 0; i < nectar_souces; i++) {
                if (nectar[i].trail > limit) {
                    BeeGroup tempnode, bestnode;

                    vector<int>templist;

                    int j = 0;

                    int bucket_length = scout_bucket.size();

                    do {
                        while (true) {
                            int k = select_scout.select();
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
                    } while (bestnode.fitness < db.min_util and j < times);

                    bestnode.trail = 0;
                    nectar[i] = bestnode;

                    set<Item>elements;

                    for (int i = 0; i < num_items; i++) {
                        if (bestnode.X[i]) {
                            elements.insert(i);
                        }
                    }
                    if (bestnode.fitness >= db.min_util) {
                        if (HUI.count({ elements }) == 0) {
                            update_scout(elements.size());
                            HUI[elements] = { bestnode.fitness ,fitness_relaxed(bestnode) };
                        }
                    }
                }
            }
        }



    };



    int choice(vector<double> w) {
        for (int i = 1; i < w.size(); i++) {
            w[i] += w[i - 1];
        }
        assert(w.back() > 0);
        double p = uniform_real_distribution<double>(0, w.back())(rng);
        for (int i = 0; i < w.size(); i++) {
            if (w[i] >= p) {
                return i;
            }
        }
        return (int)w.size() - 1;
    }



    void getHUIMs(QDatabase db) {
        ABC_HUIM abc(db);
        abc.init();
        for (int k = 0; k < 100; k++) {
            abc.EmployedBee();
            abc.OnLookerBee();
            abc.ScoutBee();
            cout << HUI.size() << endl;
        }
        for (auto i : HUI) {
            cout << vector<set<Item>>{i.first} << " " << i.second.first << endl;
        }
    }

}




int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Specify input and output files" << endl;
        exit(-1);
    }
    if (argc < 4) {
        cout << "Specify Min utility" << endl;
        exit(-1);
    }
    input = argv[1];
    output = argv[2];
    output += "_soft.txt";
    //freopen(output.c_str(), "w", stdout);
    auto database = parse_data(input);
    database.min_util = stoi(argv[3]);
    database.construct_util_array();
    HUSPM_SOFT::getHUIMs(database);
}