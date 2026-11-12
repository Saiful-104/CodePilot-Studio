
const express = require("express");
const { runCompiler } = require("../controllers/compilerController");

const router = express.Router();

// POST /api/compiler/run
router.post("/run", runCompiler);

module.exports = router;
