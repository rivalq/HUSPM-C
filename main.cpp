#define DEV

#include <bits/stdc++.h>
#include <ranges>
using namespace std;
#include "format.h"
#include "structs.h"
#include "parse.h"



int main() {
    auto database = parse_data("test.txt");
    database.min_util = 200;
    cout << database.get_utility() << endl;
    auto st = database.initial_prune();
    database.construct_util_array();

}