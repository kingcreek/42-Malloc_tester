# 42-Malloc_tester

# USAGE

`bash -c "$(curl -fsSL https://github.com/kingcreek/42-Malloc_tester/raw/main/malloc_tester.sh)"`

# Compilation

Compile in Linux:
`gcc -shared -fPIC malloc_tester.c -o malloc_tester.so`

Load shared lib:
`LD_PRELOAD=./malloc_tester.so ./Your_Program`



Compile in MacOs:
`gcc -dynamiclib malloc_tester.c -o malloc_tester.dylib`

Load shared DyLib:
`DYLD_INSERT_LIBRARIES=./malloc_tester.dylib ./Your_Program`


# TODO
-Bypass minilibx (self reminder: if i can't resolve the address, i skip the call ?) 

-Give control to the terminal to interact with input commands (maybe finish, need some test)

-Change count system, because don't work if correctly protected malloc it's called dynamically inside a infinite loop (self reminder maybe a chance to work in (baseaddr + offset) taking advantage of the backtrace ?)