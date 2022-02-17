template<typename T, typename U>
struct _QItem {
    T id;
    U cost;
    int quantity;
    _QItem(T id, int quantity, U cost) : id(id), quantity(quantity), cost(cost) {}

    bool operator < (const _QItem& rhs) const {
        return id < rhs.id;
    }

};

template<typename T>
struct _Item {
    T id;
    _Item(T id) : id(id) {}
    bool operator < (const _Item& rhs) const {
        return id < rhs.id;
    }
};

using Item = _Item<int>;
using QItem = _QItem<int, int>;

template<typename T, typename U>
struct _QSequence {
    vector<set<QItem>> vals;

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
};
using QSequence = _QSequence<int, int>;

template<typename T, typename U>
struct QDatabase {
    vector < QSequence > database;
    U min_util;

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
        for (auto i : database) {
            i.remove_items(st);
        }
    }

    /*
        @Remove 1-Sequences which has SWU < min_util
    */
    void initial_prune() {
        auto mp = get_swu_values();
        set<Item> st;
        for (auto i : mp) {
            if (i.second < min_util) {
                st.insert(i.first);
            }
        }
        remove_items(st);
    }
};