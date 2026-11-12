
const { compileCode } = require("../services/compiler/compilerService");

async function runCompiler(req, res) {
  try {
    const { sourceCode } = req.body;

    if (typeof sourceCode !== "string" || !sourceCode.trim()) {
      return res.status(400).json({
        success: false,
        error: "Source code is required.",
      });
    }

    const result = await compileCode(sourceCode);

    return res.status(result.success ? 200 : 400).json({
      success: result.success,
      output: result.output,
      error: result.error,
      exitCode: result.exitCode,
    });
  } catch (error) {
    console.error("Compiler Controller Error:", error.message);

    return res.status(500).json({
      success: false,
      error: "Unable to execute the MiniLang compiler.",
    });
  }
}

module.exports = { runCompiler };
