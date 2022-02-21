/*
 * Created on Sun Feb 20 2022
 *
 * Author: Jatin Garg, Lakshay Jindal, Ojus Bhutani
 */


 //#define DEV


#include <bits/stdc++.h>
#include <ranges>
using namespace std;
#include "utils.h"
#include "format.h"
#include "core.h"
#include "parse.h"


map<Sequence, int> HUSPs;

void LQS_Dfs(Sequence pattern, QDatabase& database, vector<int> ids) {
    set<Item> i_items, s_items;
    cout << "In" << endl;
    auto get = [&](int i) {return database.database[i];};

    for (auto q_seq : ids | views::transform(get)) {
        q_seq.get_i_items(i_items);
        q_seq.get_s_items(s_items);
    }
    cout << i_items.size() << " " << s_items.size() << endl;
    for (auto item : i_items) {
        int upper_bound = 0;
        vector<int> new_ids;
        int ptr = 0;
        for (auto q_seq : ids | views::transform(get)) {
            auto res = q_seq.get_upper_bound(item, 1);
            if (res.type == ResultType::Ok) {
                new_ids.push_back(ids[ptr]);
                upper_bound += res.value;
            }
            ptr++;
        }
        if (upper_bound < database.min_util)continue;
        assert(pattern.size() > 0);
        auto new_pattern = pattern;
        pattern.back().insert(item);
        int util = 0;
        for (auto q_seq : new_ids | views::transform(get)) {
            auto res = q_seq.get_utility(pattern);
            assert(res.type == ResultType::Ok);
            util += res.value;
        }
        if (util >= database.min_util) {
            HUSPs[pattern] = util;
        }
        vector<int> old_match, old_prefix_util;
        for (auto j : new_ids) {
            auto& q_seq = database.database[j];
            old_match.push_back(q_seq.match);
            old_prefix_util.push_back(q_seq.prefix_util);
            // Increment Prefix
            q_seq.increment_prefix(item, 1);
        }
        LQS_Dfs(pattern, database, new_ids);
        // reverse
        assert(pattern.size() > 0);
        pattern.back().erase(item);
        ptr = 0;
        for (auto j : new_ids) {
            auto& q_seq = database.database[j];
            q_seq.match = old_match[ptr];
            q_seq.prefix_util = old_prefix_util[ptr];
            ptr++;
        }
    }
    for (auto item : s_items) {
        int upper_bound = 0;
        vector<int> new_ids;
        int ptr = 0;
        for (auto q_seq : ids | views::transform(get)) {
            auto res = q_seq.get_upper_bound(item, 0);
            if (res.type == ResultType::Ok) {
                new_ids.push_back(ids[ptr]);
                upper_bound += res.value;
            }
            ptr++;
        }
        if (upper_bound < database.min_util)continue;
        auto new_pattern = pattern;
        pattern.push_back(set<Item>{item});
        int util = 0;
        for (auto q_seq : new_ids | views::transform(get)) {
            auto res = q_seq.get_utility(pattern);
            assert(res.type == ResultType::Ok);
            util += res.value;
        }
        if (util >= database.min_util) {
            HUSPs[pattern] = util;
        }
        vector<int> old_match, old_prefix_util;
        for (auto j : new_ids) {
            auto& q_seq = database.database[j];
            old_match.push_back(q_seq.match);
            old_prefix_util.push_back(q_seq.prefix_util);
            // Increment Prefix
            q_seq.increment_prefix(item, 0);
        }
        LQS_Dfs(pattern, database, new_ids);
        // reverse
        pattern.pop_back();
        ptr = 0;
        for (auto j : new_ids) {
            auto& q_seq = database.database[j];
            q_seq.match = old_match[ptr];
            q_seq.prefix_util = old_prefix_util[ptr];
            ptr++;
        }
    }
}

ostream& operator << (ostream& out, Sequence seq) {
    string str = "<";
    for (auto i : seq) {
        str += " [";
        for (auto j : i) {

            str.push_back(j.id);
            str.push_back(',');
        }
        if (str.back() == ',')str.pop_back();
        str += "],";
    }
    if (str.back() == ',')str.pop_back();
    str += " >";
    return out << str;
}

int main(int argc, char const* argv[]) {
    if (argc < 3) {
        cout << "Specify input and output files" << endl;
        exit(-1);
    }
    string input = argv[1];
    string output = argv[2];
    auto database = parse_data(input);
    database.min_util = 5;
    database.construct_util_array();
    Sequence pattern;
    vector<int> ids(database.database.size());
    iota(ids.begin(), ids.end(), 0);
    LQS_Dfs(pattern, database, ids);
    {
        ofstream cout(output);
        cout << HUSPs.size() << endl;
        for (auto i : HUSPs) {
            auto [seq, util] = i;
            cout << seq << " " << util << endl;
        }
    }
}