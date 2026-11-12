
const express = require("express");
const { runCode } = require("../controllers/executionController");

const router = express.Router();

// POST /api/execute/run
router.post("/run", runCode);

module.exports = router;
