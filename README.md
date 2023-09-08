# 42-Malloc_tester
 
Compile in Linux:
`gcc -shared -fPIC malloc_tester.c -o malloc_tester.so`

Load shared lib:
`LD_PRELOAD=./malloc_tester.so ./Your_Program`



<<<<<<< HEAD
Compile in MacOs:
`gcc -dynamiclib malloc_tester.c -o malloc_tester.dylib`

Load shared DyLib:
`DYLD_INSERT_LIBRARIES=./malloc_tester.dylib ./Your_Program`


# BONUS
wtf? https://dogbolt.org/
=======
Compile in MacOs ¿¿¿???:
`gcc -shared -o malloc_tester.dylib malloc_tester.c`

Load shared DyLib ¿¿¿???:
`DYLD_INSERT_LIBRARIES=./malloc_tester.dylib ./Your_Program`
>>>>>>> 2d0b44b0e8bfdf611c07bd775677a81f8fdb1450
