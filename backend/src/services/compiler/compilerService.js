
const { spawn } = require("child_process");

const COMPILER_PATH =
  "/mnt/c/Users/Saiful Islam/OneDrive/Desktop/CodePilot Studio/compiler/codepilot";

const WSL_DISTRIBUTION = "Ubuntu";

function compileCode(sourceCode) {
  return new Promise((resolve, reject) => {
    if (typeof sourceCode !== "string" || !sourceCode.trim()) {
      return reject(new Error("Source code cannot be empty."));
    }

    const compiler = spawn(
      "wsl.exe",
      [
        "-d",
        WSL_DISTRIBUTION,
        "--",
        COMPILER_PATH,
      ],
      {
        windowsHide: true,
      }
    );

    let stdout = "";
    let stderr = "";

    compiler.stdout.on("data", (data) => {
      stdout += data.toString();
    });

    compiler.stderr.on("data", (data) => {
      stderr += data.toString();
    });

    compiler.on("error", (error) => {
      reject(new Error(`Unable to start compiler: ${error.message}`));
    });

    compiler.on("close", (exitCode) => {
      resolve({
        success: exitCode === 0,
        output: stdout,
        error: stderr,
        exitCode,
      });
    });

    compiler.stdin.on("error", (error) => {
      if (error.code !== "EPIPE") {
        stderr += error.message;
      }
    });

    compiler.stdin.end(sourceCode);
  });
}

module.exports = { compileCode };
