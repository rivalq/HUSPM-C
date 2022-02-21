import fs from "fs";
import { exit } from "process";
import generator from "./generator.js";
import runner from "./runner.js";

const argv = process.argv;

if (argv.length < 3) {
  console.log("Wrong format");
  exit(-1);
}
const type = argv[2];

if (type == "gen-tests") {
  generator();
} else if (type == "run") {
  runner();
}
