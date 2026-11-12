
const { spawn } = require("child_process");
const fs = require("fs");
const os = require("os");
const path = require("path");

const TIMEOUT = 5000;
const MAX_OUTPUT_SIZE = 1024 * 1024;

function executeCode(language, sourceCode, input = "") {
  return new Promise((resolve, reject) => {
    const commands = {
      c: {
        source: "main.c",
        executable: "program.exe",
        compiler: "gcc",
        args: ["main.c", "-o", "program.exe"],
        run: "program.exe",
        runArgs: [],
      },
      cpp: {
        source: "main.cpp",
        executable: "program.exe",
        compiler: "g++",
        args: ["main.cpp", "-o", "program.exe"],
        run: "program.exe",
        runArgs: [],
      },
    };

    const config = commands[language];

    if (!config) {
      return reject(new Error("Unsupported language."));
    }

    if (typeof sourceCode !== "string" || !sourceCode.trim()) {
      return reject(new Error("Source code cannot be empty."));
    }

    if (sourceCode.length > 100000) {
      return reject(new Error("Source code is too large."));
    }

    const tempDir = fs.mkdtempSync(
      path.join(os.tmpdir(), "codepilot-")
    );

    const sourcePath = path.join(tempDir, config.source);
    const executablePath = path.join(tempDir, config.executable);

    let finished = false;
    let timer;

    const cleanup = () => {
      if (finished) return false;

      finished = true;
      clearTimeout(timer);

      try {
        fs.rmSync(tempDir, { recursive: true, force: true });
      } catch (error) {
        console.error("Temporary file cleanup failed:", error.message);
      }

      return true;
    };

    const result = (data) => {
      if (!cleanup()) return;
      resolve(data);
    };

    try {
      fs.writeFileSync(sourcePath, sourceCode, "utf8");
    } catch (error) {
      cleanup();
      return reject(error);
    }

    const compile = spawn(
      config.compiler,
      config.args,
      {
        cwd: tempDir,
        windowsHide: true,
      }
    );

    let compileError = "";
    let compileOutput = "";

    timer = setTimeout(() => {
      compile.kill();
      result({
        success: false,
        output: compileOutput,
        error: "Compilation timed out.",
      });
    }, TIMEOUT);

    compile.stdout.on("data", (data) => {
      compileOutput += data.toString();

      if (compileOutput.length > MAX_OUTPUT_SIZE) {
        compile.kill();
        result({
          success: false,
          output: "",
          error: "Compilation output limit exceeded.",
        });
      }
    });

    compile.stderr.on("data", (data) => {
      compileError += data.toString();

      if (compileError.length > MAX_OUTPUT_SIZE) {
        compile.kill();
        result({
          success: false,
          output: "",
          error: "Compilation error output limit exceeded.",
        });
      }
    });

    compile.on("error", (error) => {
      if (!finished) {
        cleanup();
        reject(new Error(`Compiler could not start: ${error.message}`));
      }
    });

    compile.on("close", (exitCode) => {
      if (finished) return;

      clearTimeout(timer);

      if (exitCode !== 0) {
        return result({
          success: false,
          output: compileOutput,
          error: compileError || "Compilation failed.",
          exitCode,
        });
      }

      const run = spawn(
        executablePath,
        config.runArgs,
        {
          cwd: tempDir,
          windowsHide: true,
        }
      );

      let output = "";
      let runtimeError = "";

      timer = setTimeout(() => {
        run.kill();
        result({
          success: false,
          output,
          error: "Program execution timed out.",
        });
      }, TIMEOUT);

      run.stdout.on("data", (data) => {
        output += data.toString();

        if (output.length > MAX_OUTPUT_SIZE) {
          run.kill();
          result({
            success: false,
            output: "",
            error: "Program output limit exceeded.",
          });
        }
      });

      run.stderr.on("data", (data) => {
        runtimeError += data.toString();

        if (runtimeError.length > MAX_OUTPUT_SIZE) {
          run.kill();
          result({
            success: false,
            output: "",
            error: "Runtime error output limit exceeded.",
          });
        }
      });

      run.on("error", (error) => {
        if (!finished) {
          cleanup();
          reject(new Error(`Program could not start: ${error.message}`));
        }
      });

      run.on("close", (runExitCode) => {
        if (finished) return;

        result({
          success: runExitCode === 0,
          output,
          error: runtimeError,
          exitCode: runExitCode,
        });
      });

      run.stdin.on("error", (error) => {
        if (error.code !== "EPIPE") {
          runtimeError += error.message;
        }
      });

      run.stdin.end(input);
    });
  });
}

module.exports = { executeCode };
