void solve_soft(Sequence pattern, QDatabase& database, vector<int> ids) {
    set<Item> pos_items;
    auto get = [&](int i) {return database.database[i];};

    for (auto q_seq : ids | views::transform(get)) {
        q_seq.get_s_items(pos_items);
    }
    // Now we will consider subsets of  pos_items and check
    // If some subsets upper_bound value >= min_util then recursively continues

}