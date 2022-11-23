/*
 * Created on Sun Feb 20 2022
 *
 * Author: Jatin Garg, Lakshay Jindal, Ojus Bhutani
 */


QDatabase parse_data(string file_path) {
    ifstream cin(file_path);
    int num_items, num_sequences, max_item_num = 0, max_k = 0;
    cin >> num_items >> num_sequences;
    map <int, int> ptable;
    for (int i = 0; i < num_items; i++) {
        int id;cin >> id;
        int profit; cin >> profit;
        max_item_num = max(max_item_num, id);
        ptable[id] = profit;
    }
    vector<QSequence> database;
    for (int i = 0; i < num_sequences; i++) {
        int num_sets; cin >> num_sets;
        deque<set<QItem>> qsequence;
        for (int j = 0; j < num_sets; j++) {
            int k; cin >> k;
            set<QItem> st;
            max_k = max(max_k, k);
            while (k--) {
                int id; cin >> id;
                int quantity; cin >> quantity;
                st.insert(QItem(id, quantity, ptable[id]));
            }
            qsequence.push_back(st);
        }

        database.push_back(qsequence);
    }
    return QDatabase(database, max_item_num + 1, max_k);
}

ostream& operator << (ostream& out, Sequence seq) {
    string str = "<";
    for (auto i : seq) {
        str += " [";
        for (auto j : i) {

            str += (to_string(j.id));
            str.push_back(',');
        }
        if (str.back() == ',')str.pop_back();
        str += "],";
    }
    if (str.back() == ',')str.pop_back();
    str += " >";
    return out << str;
}
