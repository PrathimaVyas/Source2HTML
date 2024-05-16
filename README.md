# Source2HTML
**Abstract:**

The goal of this project is to display the source code in Browser. The Source to HTML program shall parse a source file and generate an equivalent HTML file. Browser will takes the HTML files as input and renders it in Browser window to display the source code. Reserved keywords, preprocessor directives, numerical constants, strings, header files etc must be displayed in predefined color.For system header files we should not print '<' and '>' because it will consider it as tag so we need to avoid that just print the header file name.

**Requirement:**
1. Application should accept C source file
2. The output file name can be passed through command line argument or we can take default file name
3. By default it should be in idle state 
4. Identify words, comments, preprocessor, constants etc and move to specific state
5. Store the characters in buffer while staying in the specific state
6. Identify end of words, comments, preprocessor, numeric constants and return an event
7. Leave the specific state and move back to the idle state

**Pre-requisite:**
1. Basic understanding of HTML
2. Usage of File pointers
3. File I/O
4. String tokenization
