import { createInterface as ci } from "readline";
import { exec } from "child_process";
export default function generator() {
  const readline = ci({
    input: process.stdin,
    output: process.stdout,
  });
  let transaction_count = 0;
  let num_items = 0;
  let l_profit, r_profit, l_quantity, r_quantity;

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
    l_profit = await question("Enter Lowest possible profit of an item: ");
    r_profit = await question("Enter Highest possible profit of an item: ");
    l_quantity = await question(
      "Enter Lowest possible quantity possible in itemset: "
    );
    r_quantity = await question(
      "Enter Highest possible quantity possible in itemset: "
    );
    readline.close();
  };
  main().then(() => {
    exec(
      `./data-gen/lib/gen seq -ncust ${transaction_count} -nitems ${num_items} -fname data -ascii`,
      function (err, stdout, stderr) {}
    );
  });
}
