# 42-Malloc_tester
 
Compile in Linux:
`gcc -shared -fPIC -o malloc_tester.so malloc_tester.c`

Load shared lib:
`LD_PRELOAD=./malloc_tester.so ./Your_Program`



Compile in MacOs:
`gcc -shared -o malloc_tester.dylib malloc_tester.c`

Load shared DyLib:
`DYLD_INSERT_LIBRARIES=./malloc_tester.dylib ./Your_Program`