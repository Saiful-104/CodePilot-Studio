
const { spawn } = require("child_process");
const fs = require("fs");
const os = require("os");
const path = require("path");

const TIMEOUT = 5000;
const MAX_OUTPUT_SIZE = 1024 * 1024;
const JAVA_HOME =
  process.env.JAVA_HOME ||
  "C:\\Program Files\\Eclipse Adoptium\\jdk-21.0.13.9-hotspot";
function executeCode(language, sourceCode, input = "") {
  return new Promise((resolve, reject) => {
    const configurations = {
      c: {
        source: "main.c",
        executable: "program.exe",
        compiler: "gcc",
        compileArgs: ["main.c", "-o", "program.exe"],
        run: "program.exe",
        runArgs: [],
      },

      cpp: {
        source: "main.cpp",
        executable: "program.exe",
        compiler: "g++",
        compileArgs: ["main.cpp", "-o", "program.exe"],
        run: "program.exe",
        runArgs: [],
      },

      java: {
        source: "Main.java",
        executable: "Main.class",
        compiler: JAVA_HOME
          ? path.join(JAVA_HOME, "bin", "javac.exe")
          : "javac",
        compileArgs: ["Main.java"],
        run: JAVA_HOME
          ? path.join(JAVA_HOME, "bin", "java.exe")
          : "java",
        runArgs: ["Main"],
      },
    };

    const config = configurations[language];

    if (!config) {
      return reject(new Error("Unsupported language."));
    }

    if (typeof sourceCode !== "string" || !sourceCode.trim()) {
      return reject(new Error("Source code cannot be empty."));
    }

    if (sourceCode.length > 100000) {
      return reject(new Error("Source code is too large."));
    }

    if (typeof input !== "string") {
      return reject(new Error("Input must be a string."));
    }

    const tempDir = fs.mkdtempSync(
      path.join(os.tmpdir(), "codepilot-")
    );

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

    const finish = (result) => {
      if (cleanup()) resolve(result);
    };

    const sourcePath = path.join(tempDir, config.source);

    try {
      fs.writeFileSync(sourcePath, sourceCode, "utf8");
    } catch (error) {
      cleanup();
      return reject(error);
    }

    const compile = spawn(
      config.compiler,
      config.compileArgs,
      {
        cwd: tempDir,
        windowsHide: true,
      }
    );

    let compileOutput = "";
    let compileError = "";

    timer = setTimeout(() => {
      compile.kill();
      finish({
        success: false,
        output: compileOutput,
        error: "Compilation timed out.",
      });
    }, TIMEOUT);

    compile.stdout.on("data", (data) => {
      compileOutput += data.toString();

      if (compileOutput.length > MAX_OUTPUT_SIZE) {
        compile.kill();
        finish({
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
        finish({
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
        return finish({
          success: false,
          output: compileOutput,
          error: compileError || "Compilation failed.",
          exitCode,
        });
      }

      const run = spawn(
        config.run,
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
        finish({
          success: false,
          output,
          error: "Program execution timed out.",
        });
      }, TIMEOUT);

      run.stdout.on("data", (data) => {
        output += data.toString();

        if (output.length > MAX_OUTPUT_SIZE) {
          run.kill();
          finish({
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
          finish({
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

        finish({
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
