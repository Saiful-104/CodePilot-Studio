
const {
  executeCode,
} = require("../services/execution/executionService");

async function runCode(req, res) {
  try {
    const { language, sourceCode, input = "" } = req.body;

    const supportedLanguages = ["c", "cpp", "java"];

    if (!supportedLanguages.includes(language)) {
      return res.status(400).json({
        success: false,
        error: "Unsupported language. Use c, cpp or java.",
      });
    }

    if (typeof sourceCode !== "string" || !sourceCode.trim()) {
      return res.status(400).json({
        success: false,
        error: "Source code is required.",
      });
    }

    if (typeof input !== "string") {
      return res.status(400).json({
        success: false,
        error: "Input must be a string.",
      });
    }

    const result = await executeCode(
      language,
      sourceCode,
      input
    );

    return res.status(result.success ? 200 : 400).json(result);
  } catch (error) {
    console.error("Execution Controller Error:", error.message);

    return res.status(500).json({
      success: false,
      error: error.message || "Code execution failed.",
    });
  }
}

module.exports = { runCode };
