# Meta-Learning-Project-Visual-Memory-Management

A small IDE interface where users can input a short snippet of simple C or Python code (with a restricted subset of syntax). Your program does not directly execute the code; instead, it simulates an interpreter, recording the changes in variable memory state at each step. The interface includes a timeline slider that allows users to scroll back to the memory state after any previous line of code execution.

It involves AST (Abstract Syntax Tree) parsing and requires maintaining a stack of state snapshots. This is a practical application of compiler principles and data structures (stack / hash table).

Interface implementation: code highlighting on the left (using Qt's QScintilla or Python's tkinter + Pygments), and a large table on the right displaying variable names, values, and schematic memory addresses.
