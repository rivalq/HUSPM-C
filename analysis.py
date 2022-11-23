
import numpy as np

import matplotlib.pyplot as plt
from collections import OrderedDict

from scipy.interpolate import interp1d

nums = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
avg_util = []
# nums = [1]
avg_tran_size = []
avg_itemset_size = []
max_util = []
max_tran_size = []
max_item_size = []
for num in nums:
    s = "./tests/" + str(num) + "/output.txt"
    test_file = open(s)

    mp = dict()

    data = test_file.readlines()
    sum = 0
    cnt = 0
    cnt2 = 0
    transaction_size = 0
    item_set_size = 0
    mx = 0
    mx_tran = 0
    mx_item = 0
    for i in data:
        if(i[0] == '<'):
            val = int(i.split(' ')[-1])
            if val in mp.keys():
                mp[val] += 1
            else:
                mp[val] = 1
            """"     
            s = i[1:].split("[")
            tt = 0
            for _ in s:
                j = _.split("]")[0].split(",")
                tmp = []
                for k in j:
                    if k != " " and k != "":
                        tmp.append(k)
                j = tmp
                if(len(j)):
                    tt += 1
                    mx_item = max(mx_item, len(j))
                    transaction_size += 1
                    item_set_size += len(j)
                    cnt2 += 1
            mx_tran = max(mx_tran, tt)
            sum += val
            mx = max(mx, val)
            cnt += 1
            """

    def graph():
        od = OrderedDict(sorted(mp.items()))
        x = list(od.keys())
        y = list(od.values())

        s = len(y)
        for i in range(s - 2, -1, -1):
            y[i] += y[i + 1]

        x = np.array(x)
        y = np.array(y)
        cubic_interploation_model = interp1d(x, y, kind="cubic")
        X_ = np.linspace(x.min(), x.max())
        Y_ = cubic_interploation_model(X_)
        plt.plot(x, y)
        plt.title(
            "Number of HUSPs vs Minimum Utility (Test Data - " + str(num) + ")")
        plt.xlabel("Minimum Utility")
        plt.ylabel("Number of HUSPs")
        s = "./graph" + str(num) + ".png"
        plt.savefig(s)
        plt.close()
    graph()


print(avg_util)
print(max_util)
print(avg_tran_size)
print(max_tran_size)
print(avg_itemset_size)
print(max_item_size)
