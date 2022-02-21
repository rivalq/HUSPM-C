import { createInterface as ci } from "readline";
import { exec } from "child_process";
import { stderr, stdout } from "process";

export default function runner() {
  const readline = ci({
    input: process.stdin,
    output: process.stdout,
  });
  function question(theQuestion) {
    return new Promise((resolve) =>
      readline.question(theQuestion, (answ) => resolve(answ))
    );
  }
  const main = async () => {
    const test_case = await question("Enter Testcase which you wanna run: ");
    const output = "./tests/" + test_case + "/output.txt";
    const input = "./tests/" + test_case + "/input.txt";
    readline.close();
    console.log(input, output);
    exec(
      `g++ ./core/main.cpp --std=c++20 && ./a.out ${input} ${output}`,
      function (error, stdout, stderr) {}
    );
  };
  main().then(() => console.log("Succesfull"));
}
