# 42-Malloc_tester

This project is for general purpose, useful to ensure that the mallocs that are implemented in the projects are correct and are handled well if the system returns NULL in any of them.

The program run the given executable (see the example in usage), it will execute normally, but the moment it encounters a malloc it will force it to be NULL, if the program is closed by some type of exit() or return, the program will be executed again whenever there has been some malloc on the way. When the program is opened again, the mallocs already checked will be skipped.

The idea behind this is to ensure that you don't try to manipulate a memory segment that hasn't been allocated.
It has another function such as showing the trace in case a poorly handled malloc causes a Segfault, (make sure you have compiled your program and all the libraries with -g to be able to trace the offsets!), ~~showing the total memory allocated and freed for you at the end of the program~~

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

# RUN WITH DOCKER

You can build the Docker image using the Dockerfile from this repository. Follow these steps:

1. Make sure you have Docker installed on your system.

2. Open a terminal and run the following command to build the Docker image:

```
curl -L https://raw.githubusercontent.com/kingcreek/42-Malloc_tester/main/Dockerfile | docker build -t malloc_t -f - .
```
Run:
```
docker run -it --rm -v /path/to/your/program:/program_name malloc_t
```

To make everything a little easier, from the docker command you can copy the project to the machine that runs Docker.
`/path/to/your/program` refers to your local project
`/program_name` refers to the project path within the machine in Docker

# USAGE
-Make sure that your program and its libraries, if you use any (ft_printf, libft...) have been compiled with the `-g` flag

-Run `malloc_tester` in the terminal to start the program, type the name of your executable when prompted, you can add arguments if necessary: `./a.out <optional args>`

# Compilation

Compile in Linux:
`make`

Load shared lib:
`LD_PRELOAD=./malloc_tester.so ./Your_Program <optional args>`

Compile in MacOs:
`make`

Load shared DyLib:
`DYLD_INSERT_LIBRARIES=./malloc_tester.dylib ./Your_Program <optional args>`

