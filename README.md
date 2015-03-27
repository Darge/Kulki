# Kulki

Instruction:

1. Download and extract GTK+ 3.x bundle 3.6.4 (32-bit) or (for 64-bit systems) GTK+ 3.0 bundle 3.6.4 (64-bit) to some catalog (we will call it the GTK catalog).
You should install 64-bit version of GTK only, if your compiler is 64-bit (The standard mingw gcc is 32bit in codeblocks).

2. Add the GTK to the PATH environmental variable.

3. Run Codeblocks and in Menu -> Compiler, the "Compiler settings" tab, and then "Other options" tab, write:

`pkg-config gtk+-3.0 --cflags`

and type this in the "Other linker options":

`pkg-config gtk+-3.0 --libs`