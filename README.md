# 🚀 Assembler Project

## 📖 Overview
The **Assembler Project** is a two-pass assembler developed in C for an imaginary computer architecture. It translates a custom assembly language into machine code through a series of well-defined steps. This project serves as an educational tool, revealing the inner workings of assemblers, macro expansion, and machine-level programming.

## ✨ Features
- **Two-Pass Assembly Process:**
  - **First Pass:** Scans the pre-processed `.am` file to build the symbol table, identify entry symbols, and detect syntax errors.
  - **Second Pass:** Re-examines the `.am` file to catch logical errors and encodes the assembly code into binary machine code.
- **Macro Pre-Processing:**
  - Automatically expands macros and generates an intermediate file with a `.am` extension.
  - Ensures no line exceeds 80 characters and that macros are correctly defined.
- **Robust Error Handling:**
  - Detects and handles errors at each phase (pre-assembling, first pass, second pass).
  - Gracefully manages memory allocation errors by freeing resources and continuing with subsequent files.
- **Cross-Platform Compatibility:** 
  - Developed in C, making it easily portable across various operating systems.
- **Memory Management:**
  - Designed with the assumption that input files require no more than 3996 memory cells for encoding.

## 🗂️ Project Structure
assembler-project/ ├── src/ # Source code files │ ├── include/ # Header files │ ├── assembler.c # Main program orchestrating the assembly process │ ├── front_end.c # Front-end processing (if applicable) │ ├── pre_assembler.c # Pre-assembly: macro expansion and validation │ ├── first_pass.c # First pass: syntax analysis and symbol table creation │ ├── second_pass.c # Second pass: logical analysis and code encoding │ └── back_end.c # Back-end: object file and additional file generation ├── test/ # Test cases and example assembly programs └── makefile # Build configuration file

bash
Copy
Edit

## 🔧 Getting Started

### Prerequisites
- A C compiler (e.g., GCC)
- Make utility

### Installation
1. **Clone the Repository:**
   ```bash
   git clone https://github.com/SaarHalevi/assembler-project.git
   cd assembler-project
Build the Project:
bash
Copy
Edit
make
💻 Usage
Prepare Your Assembly Source:

Write your assembly code in files with the .am suffix.
Place your source files in the test/ directory or specify their full path when executing the assembler.
Run the Assembler:

bash
Copy
Edit
./assembler <input_file.am> <output_file.obj>
Replace <input_file.am> with your assembly source file and <output_file.obj> with the desired name for the generated object file.

🔄 Assembly Process Workflow
Pre-Assembling:

Macro Expansion: Checks for macro definition errors and ensures line lengths do not exceed 80 characters.
Intermediate File Generation: Creates a .am file with expanded macros if no errors are found.
Error Handling: Aborts processing for the file if errors are detected.
First Pass:

Syntax Analysis: Scans the .am file to detect syntax errors.
Symbol Table Creation: Builds a symbol table and collects entry symbols.
Memory Management: Terminates processing for the file if memory allocation errors occur.
Second Pass:

Logical Analysis: Re-examines the .am file for logical errors.
Binary Encoding: Encodes the assembly instructions into binary machine code.
Error Handling: Halts further processing for the file upon errors or memory issues.
Back-End:

Object File Creation: Generates the final object file based on the encoded binary data.
Additional Files: Creates extra files for entries and externals when needed.
Resource Cleanup: Frees allocated memory after processing each file.
⚠️ Error Handling & Memory Management
The assembler handles errors at each stage of the assembly process.
Memory allocation issues trigger a cleanup routine that frees all allocated resources before moving on to the next file.
Detailed error messages help identify and correct issues in your assembly code.
📝 Notes
Input Files: Ensure your assembly source files use the .am suffix.
Memory Limit: The assembler assumes that source files require no more than 3996 memory cells.
This project serves as both a functional assembler and a learning tool for understanding low-level programming and compiler design.
🤝 Contributing
Contributions, bug reports, and feature requests are welcome! Feel free to fork the repository and submit pull requests.

📜 License
Include license information here if applicable.

