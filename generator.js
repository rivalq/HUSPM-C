import { createInterface as ci } from "readline";
import { exec } from "child_process";
import { readFile, readdir, writeFile, mkdirSync, fstat } from "fs";
export default function generator() {
  const readline = ci({
    input: process.stdin,
    output: process.stdout,
  });
  let transaction_count = 0;
  let num_items = 0;
  let profit_prob = [];
  let quantity_prob = [];

  function question(theQuestion) {
    return new Promise((resolve) =>
      readline.question(theQuestion, (answ) => resolve(answ))
    );
  }
  const main = async () => {
    transaction_count = await question(
      "Enter Number of sequences (in 1000's): "
    );
    num_items = await question("Enter Number of different items (in 1000s): ");
    /*l_profit = await question("Enter Lowest possible profit of an item: ");
    r_profit = await question("Enter Highest possible profit of an item: ");
    l_quantity = await question(
      "Enter Lowest possible quantity possible in itemset: "
    );
    r_quantity = await question(
      "Enter Highest possible quantity possible in itemset: "
    );*/
    console.log(transaction_count, num_items);

    let s = await question(
      "Enter probability distribution of profits [1-10]: "
    );
    s = s.split(" ");
    let sum = 0;
    for (let i = 0; i < 10; i++) {
      profit_prob.push(parseFloat(s[i], 10));
      sum += profit_prob[i];
    }
    for (let i = 0; i < 10; i++) {
      profit_prob[i] /= sum;
    }
    s = await question("Enter probability distribution of quantity [1-10]: ");
    s = s.split(" ");
    sum = 0;
    for (let i = 0; i < 10; i++) {
      quantity_prob.push(parseFloat(s[i], 10));
      sum += quantity_prob[i];
    }
    for (let i = 0; i < 10; i++) {
      quantity_prob[i] /= sum;
    }
    readline.close();
  };
  const get_quantity = () => {
    let p = Math.random();
    let s = 0;
    for (let i = 0; i < 10; i++) {
      s += quantity_prob[i];
      if (s >= p) {
        return i + 1;
      }
    }
    return 10;
  };
  const get_profit = () => {
    let p = Math.random();
    let s = 0;
    for (let i = 0; i < 10; i++) {
      s += profit_prob[i];
      if (s >= p) {
        return i + 1;
      }
    }
    return 10;
  };
  main().then(() => {
    exec(
      `./data-gen/lib/gen seq -ncust ${transaction_count} -nitems ${num_items} -slen 10 -tlen 4 -fname data -ascii`,
      function (err, stdout, stderr) {
        readFile("./data.data", "utf-8", function read(err, data) {
          if (err) {
            throw err;
          } else {
            data = data.split("\n");
            let mp = new Map();
            let profits = new Map();
            for (let i = 0; i < data.length; i++) {
              if (data[i] == "") continue;
              let arr = data[i].split(" ").map((x) => parseInt(x));
              let tid = arr[0];
              let sid = arr[1];
              let num_items = arr[2];
              let seq = [];
              for (let i = 3; i < arr.length; i++) {
                if (profits.has(arr[i]) == false) {
                  profits.set(arr[i], get_profit());
                }
                seq.push([arr[i], get_quantity()]);
              }
              if (mp.has(tid) == false) {
                mp.set(tid, []);
              }
              let temp = mp.get(tid);
              temp.push(seq);
              mp.set(tid, temp);
            }
            let test_num = 0;
            const write = () => {
              let str = `${profits.size} ${mp.size}\n`;
              for (const val of profits.entries()) {
                str += `${val[0]} ${val[1]}\n`;
              }
              for (const val of mp.entries()) {
                let arr = val[1];
                str += `${arr.length}\n`;
                for (let i = 0; i < arr.length; i++) {
                  let temp = arr[i];
                  str += `${temp.length}\n`;
                  for (let j = 0; j < temp.length; j++) {
                    str += `${temp[j][0]} ${temp[j][1]} `;
                  }
                  str += "\n";
                }
              }
              mkdirSync(`./tests/${test_num}`);
              writeFile(`./tests/${test_num}/input.txt`, str, (err) => {
                if (err) {
                  throw err;
                }
              });
            };
            readdir("./tests", (err, files) => {
              if (err) {
                throw err;
              }
              test_num = files.length + 1;
              write();
              readline.close();
            });
          }
        });
      }
    );
  });
}
