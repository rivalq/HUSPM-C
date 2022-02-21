/*
 * Created on Sun Feb 20 2022
 *
 * Author: Jatin Garg, Lakshay Jindal, Ojus Bhutani
 */


QDatabase parse_data(string file_path) {
    ifstream cin(file_path);
    int num_items, num_sequences;
    cin >> num_items >> num_sequences;
    map <char, int> ptable;
    for (int i = 0; i < num_items; i++) {
        char id;cin >> id;
        int profit; cin >> profit;
        ptable[id] = profit;
    }
    vector<QSequence> database;
    for (int i = 0; i < num_sequences; i++) {
        int num_sets; cin >> num_sets;
        vector<set<QItem>> qsequence;
        for (int j = 0; j < num_sets; j++) {
            int k; cin >> k;
            set<QItem> st;
            while (k--) {
                char id; cin >> id;
                int quantity; cin >> quantity;
                st.insert(QItem(id, quantity, ptable[id]));
            }
            qsequence.push_back(st);
        }

        database.push_back(qsequence);
    }
    return database;
}