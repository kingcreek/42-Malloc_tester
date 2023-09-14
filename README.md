# 42-Malloc_tester

This project is for general purpose, useful to ensure that the mallocs that are implemented in the projects are correct and are handled well if the system returns NULL in any of them.

The program run the given executable (see the example in usage), it will execute normally, but the moment it encounters a malloc it will force it to be NULL, if the program is closed by some type of exit() or return, the program will be executed again whenever there has been some malloc on the way. When the program is opened again, the mallocs already checked will be skipped.

The idea behind this is to ensure that you don't try to manipulate a memory segment that hasn't been allocated.
It has another function such as showing the trace in case a poorly handled malloc causes a Segfault, (make sure you have compiled your program and all the libraries with -g to be able to trace the offsets!), showing the total memory allocated and freed for you at the end of the program

I hope it helps!!

# INSTALL
```
bash -c "$(curl -fsSL https://github.com/kingcreek/42-Malloc_tester/raw/main/install.sh)"
```

The `.malloc_tester` folder will be under your `$HOME` directory (`/Users/<your_username>/`)

# UNINSTALL
```
bash -c "$(curl -fsSL https://github.com/kingcreek/42-Malloc_tester/raw/main/uninstall.sh)"
```

# USAGE
-Make sure that your program and its libraries, if you use any (ft_printf, libft...) have been compiled with the `-g` flag

-Run `malloc_tester` in the terminal to start the program, type the name of your executable when prompted ex: `./a.out <optional args>`, you can add arguments if necessary

# RUN WITH DOCKER

You can build the Docker image using the Dockerfile from this repository. Follow these steps:

1. Make sure you have Docker installed on your system.

2. Open a terminal and run the following command to build the Docker image:

```bash
docker build -t nombre-de-la-imagen:etiqueta -f https://raw.github.com/kingcreek/42-Malloc_tester/main/Dockerfile .

Run:
```
docker run -it --rm malloc_t malloc_tester
```
# Compilation

Compile in Linux:
`make`

Load shared lib:
`LD_PRELOAD=./malloc_tester.so ./Your_Program`



Compile in MacOs:
`make`

Load shared DyLib:
`DYLD_INSERT_LIBRARIES=./malloc_tester.dylib ./Your_Program`

