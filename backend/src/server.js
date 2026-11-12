
const express = require("express");
const cors = require("cors");
require("dotenv").config();

const compilerRoutes = require("./routes/compilerRoutes");

const app = express();

// Middleware
app.use(cors());
app.use(express.json());

// Health check
app.get("/", (req, res) => {
  res.status(200).json({
    success: true,
    message: "Welcome to CodePilot Studio Backend!",
  });
});

// Compiler API routes
app.use("/api/compiler", compilerRoutes);

// Server configuration
const PORT = process.env.PORT || 5000;

// Start server
app.listen(PORT, () => {
  console.log(`CodePilot Studio Backend running on port ${PORT}`);
});
