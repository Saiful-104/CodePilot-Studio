# CodePilot Studio

**A Web-Based Multi-Language Code Execution Platform with a MiniLang Compiler**

## Project Overview

CodePilot Studio is a web-based coding platform designed to support C, C++, and Java code execution. It also includes a MiniLang compiler developed as part of the Compiler Construction Lab project.

The platform aims to provide a user-friendly interface for writing code, running programs, and viewing compilation errors and program output.

## Main Features

- Code editor and programming language selection
- C, C++, and Java code compilation and execution
- Compilation error and program output display
- MiniLang lexical analysis using Flex
- Syntax analysis using Bison
- Abstract Syntax Tree (AST) construction and readable output
- Symbol table management and nested scopes
- Semantic analysis and type checking
- Three-Address Code (TAC) generation
- Syntax error reporting and basic error recovery

## Technology Stack

### Compiler
- C
- Flex
- Bison
- GCC

### Web Application
- Frontend: React and Tailwind CSS
- Backend: Node.js and Express

### Code Execution
- C: GCC
- C++: G++
- Java: JDK

## Compiler Pipeline

1. Lexical Analysis
2. Syntax Analysis
3. Abstract Syntax Tree Construction
4. Symbol Table Management
5. Semantic Analysis
6. Three-Address Code Generation

## Project Structure

- `src/` — Compiler source code
- `include/` — Shared header files
- `tests/` — Test programs
- `examples/` — Sample programs
- `docs/` — Documentation and grammar specifications
- `Makefile` — Build automation

## Security

Code execution must be isolated and restricted using appropriate execution time and resource limits. User-submitted programs must not be executed with unrestricted server access.

## Project Goal

The goal is to combine compiler construction concepts with a web-based coding interface while maintaining the core requirements of the Compiler Construction Lab project.

## Project Status

Under development.
