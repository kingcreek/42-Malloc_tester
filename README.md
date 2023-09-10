# 42-Malloc_tester

# INSTALL
`bash -c "$(curl -fsSL https://github.com/kingcreek/42-Malloc_tester/raw/main/install.sh)"`

# UNINSTALL
`bash -c "$(curl -fsSL https://github.com/kingcreek/42-Malloc_tester/raw/main/uninstall.sh)"`

# USAGE
-Make sure that your program and its libraries, if you use any (ft_printf, libft...) have been compiled with the `-g` flag

-Run `malloc_tester` in the terminal to start the program, type the name of your executable when prompted ex: `./a.out`, you can add arguments if necessary

# Compilation

Compile in Linux:
`make`

Load shared lib:
`LD_PRELOAD=./malloc_tester.so ./Your_Program`



Compile in MacOs:
`make`

Load shared DyLib:
`DYLD_INSERT_LIBRARIES=./malloc_tester.dylib ./Your_Program`


# TODO
~~-Bypass minilibx (self reminder: if i can't resolve the address, i skip the call ?) 🤬~~ DONE

~~-Give control to the terminal to interact with input commands (maybe finish, need some test)~~ DONE

~~-Change count system, because don't work if correctly protected malloc it's called dynamically inside a infinite loop (self reminder maybe a chance to work in (baseaddr + offset) taking advantage of the backtrace ?)~~ DONE