

# 🛠️ Assembler Project -  Assembly Language Compiler
This is a Final project in "Systems Programming Laboratory" course at the Open University. It's a comprehensive assembler program for a specific assembly language, written in C. 
The purpose is to simulate the operation of common system programs, and thus experience writing a large-scale software project.
Let’s dive in! 🚀

---

## 📋 Table of Contents
- [Project Overview](#-project-overview)
- [Workflow](#-workflow)
- [Project Structure](#-project-structure)
- [How to Test](#-how-to-test)
- [Dependencies](#-dependencies)
- [Contributing](#-contributing)

---

## 🌟 Project Overview
This project is a full-featured assembler. It processes `.as` files (assembly code) and generates:
- **Object files (`.ob`)**: Binary machine code.
- **Entry files (`.ent`)**: Labels marked as entry points.
- **External files (`.ext`)**: Labels marked as external references.

Key features:
- Macro expansion 🪄
- Symbol table management 🏷️
- Error detection and reporting ❌
- Support for directives (e.g., `.data`, `.string`, `.entry`).

---

## 🔄 Workflow

### 1. **Pre-Assembler (Macro Expansion)** �
- **Purpose**: Replace macros in the source code with their actual definitions.
- **How it works**:
  - Scans the input file for macro definitions (`mcro <name>` and `endmcro`).
  - Expands macros inline, creating an intermediate `.am` file.
  - Example: `LOOP_MACRO` becomes its repeated instructions.

### 2. **First Pass** 📖
- **Purpose**: Build the symbol table and calculate initial addresses.
- **How it works**:
  - Parses the `.am` file line by line.
  - Records labels (e.g., `LOOP:`) and their memory addresses.
  - Validates syntax and detects errors (e.g., invalid operands).

### 3. **Second Pass** 📝
- **Purpose**: Generate machine code and final outputs.
- **How it works**:
  - Converts instructions to binary using the symbol table.
  - Resolves label addresses (for jumps/calls).
  - Writes `.ob` (binary), `.ent` (entry points), and `.ext` (external references) files.

---

## 📂 Project Structure
```
assembler-project/
├── src/                  # Source code
│   ├── pre_assembler.c   # Macro expansion logic
│   ├── first_pass.c      # Symbol table creation
│   ├── second_pass.c     # Machine code generation
│   ├── utils/            # Helper functions (e.g., error handling)
│   └── main.c            # Driver code
├── include/              # Header files
├── test_cases/           # Sample .as files for testing
├── Makefile              # Compilation script
└── README.md             # You are here! 😄
```

---

## 🧪 How to Test
Test the assembler with a sample file:

1. **Compile the project**:
   ```bash
   make all
   ```

2. **Run the assembler on a test file**:
   ```bash
   ./assembler test_cases/example.as
   ```

3. **Check the outputs**:
   - `example.ob`: Binary machine code.
   - `example.ent`: Entry points (if any).
   - `example.ext`: External references (if any).

**Sample Output**:
```
✅ Success! Files generated:
- example.ob
- example.ent
```

---

## 📦 Dependencies
- **C Compiler** (e.g., `gcc`).
- **Make** (for building).
- Tested on Unix-based systems (Linux/macOS).


