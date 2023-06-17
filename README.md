# CSE232-System-Programming
CSE232 System Programming Course Spring 2023

Includes assignment answers and Group Project
---

# Assignments
8 Assignment about different course related topics

# Group Project

Implementation a macro-processor for M6800 assembly language in C programming language. I was responsible with implementation of `parse()` function and `DIV` macro. I also helped other members with their task. The codes and documentation is under `[Project]()` folder. [Contributers](https://github.com/yuuIind/CSE232-System-Programming/edit/main/README.md#contributers) and [Project details](https://github.com/yuuIind/CSE232-System-Programming/edit/main/README.md#project-details) are as follows:

### Contributers:
---
- Mert Korkut : `read()` function and `REM` macro
- Muhammet Hakan Taştan : `parse()` function and `DIV` macro
- Buse Karakaş : `is_macro()` function and `MULT` macro
- Ahmet Berke Kösretaş : `expand()` function and `SUB` macro
- Umutcan Ağgez : `createPT()` function and `ADD` macro

# Project details
- Full instructions are on '232proj23s.pdf' file
- Implementation a macro-processor for M6800 assembly language
- macro-processor is written in C
- gcc compiler is used to compile on Linux.
- macro-processor performs the following functions:
    - Read the macro definitions from a file and store them in a memory buffer
    - Check the condition for macro expansion
    - Create the parameter table
    - Macro expansion with parameters
- Macro definitions are found at the beginning of the file and they are in the following format:
    ``` 
        #MNAME: MACRO P1,P2
                instructions
                ...
                #ENDM
                where P1,P2,... are the dummy parameters. There can be maximum 3 parameters.
     ```
- Following is an example program with macro calls:
    ```
        instructions
        ...
        #MNAME 3,5 //3,5 are the actual parameters
        ...
        instructions
        ...
        #if ($1=’2’) MNAME 7,8 // if condition parameter $1 is ‘2’ then call macro with actual parameters 7,8
        ...
        instructions
        ...
    ```
- macro-processor must take the input file name and condition parameters as arguments, using argc, argv.
- The macro-processor uses a memory buffer to keep the macro definitions. Use the following data structures:
    ```
        struct mac {
                      char mname[8]; //macro name
                      char param[10][4]; //max. 10 parameters, each parameter max. 3 characters
                      char macro[256]; //macro body
        }
        struct mac buffer[10]; // memory buffer for 10 macro definitions
        int m_count; // counts the number of macro definitions
    ```
- Following functions are implemented for the macro-processor:
    - int read(char* filename)
        - Reads macro definitions from the file and stores them in the memory buffer.
        - It returns the number of macro definitions.
    - parse(char* filename)
        - The program with macro calls follows the macro definitions.
        - The program is read line by line. 
        - parse()function parses the current program line and stores its fields to the following data structure:
          ```
                char field[10][7]; //max 10 fields in a line, each field max 6 characters
          ```
        - Examples:
          ```
                L1: LDAA #20H Fields are: L1: LDAA #20H
                STAA 2000H Fields are: STAA 2000H
                #M1 // calls macro M1 Fields are: M1
                #M2 2,3 // calls macro M2 with parameters 2 and 3 Fields are: M2 2 3
                #if ($2=’ABC’) M3 200, 300 // calls M3 with parameter 200 if the condition is true
                                Fields are: #if $2 ABC M3 200 300
           ```
    - is_macro()
        - It checks if there is a macro call in the current line (Macro calls start with a macro name or #if).
        - If so, it calls expand() to expand the macro. Otherwise writes the line into the .asm file.
        - is_macro() function must also check if the macro call has a condition. Conditional calls have the following format:
          ```
                #if ($2=’4’) MNAME 3,5
          ```
        - $2, $3, $4, $5 are condition parameters that are given as arguments to the macro-processor such that $2 is argv[2], $3 is argv[3], etc.
        - is_macro()function must evaluate the condition and call expand() if it is true.
    - expand()
        - It first calls createPT()
        - Then it takes a line from the macro body and writes it to the expanded code. 
        - If there are any dummy parameters in that line, it also substitutes them by the actual parameters using the parameter table.
    - createPT()
        - It fills the parameter table for the current macro call.
        - Uses the following data structer:
          ```
              struct pt {
                            char mname[8]; //macro name
                            int nparams; //number of parameters
                            char dummy[10][4]; //max. 10 parameters, each parameter max. 3 characters
                            char actual[10][4];
                        }
              struct pt PT;
          ```
- Following macro definitions are written using M6800 instruction set:
    - ADD
    - SUB
    - MULT
    - DIV
    - REM

