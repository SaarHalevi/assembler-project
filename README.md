
# Assembler Project

## Overview

The Assembler Project is a two-pass assembler designed for an imaginary computer model and assembly language.
This project demonstrates the process of translating assembly code into machine code, providing insights into the inner workings of assemblers and machine-level programming.

## Features

- **Two-Pass Assembly**: Implements a two-pass assembly process to accurately translate assembly code into machine code.
- **Custom Assembly Language**: Utilizes a unique assembly language tailored for the project's specific architecture.
- **Cross-Platform Support**: Developed in C, ensuring compatibility across various operating systems.

## Project Structure

- `src/`: Contains the source code files for the assembler.
- - `include/`: Includes header files necessary for the assembler's functionality.
- `test/`: Includes test cases and example assembly programs for validation.
- `makefile`: Build configuration file for compiling the project.

## Getting Started

To build and run the assembler:

1. **Clone the Repository**:

   ```bash
   git clone https://github.com/SaarHalevi/assembler-project.git
   cd assembler-project
   ```

2. **Build the Project**:

   ```bash
   make
   ```

3. **Run the Assembler**:

   ```bash
   ./assembler <input_file.asm> <output_file.obj>
   ```

   Replace `<input_file.asm>` with your assembly source file and `<output_file.obj>` with the desired output file name.

## Usage

- Write your assembly code in files with the `.asm` extension.
- Place these files in the `test/` directory or specify their path when running the assembler.
- The assembler will process the source file and generate an object file with the specified name.

  
