#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <readline/readline.h>


// gcc test_program.c -g -lreadline

void print(char *str)
{
	while(*str)
	{
		write(1, str, 1);
		str++;
	}
}

void a()
{
	char *str2 = malloc(1338);
	if(!str2)
		return;
	str2[50] = '\0';
	free(str2);
}
int main(int argc, char **argv, char **env)
{
	int		saved_stdin;
	int		saved_stdout;

	
	char *str = malloc(1337);
	if(!str)
		exit(0);
	if(str)
	{
		str[0] = 't';
		str[1] = 'e';
		str[2] = 's';
		str[3] = 't';
		str[4] = '\n';
		str[5] = '\0';
		print(str);
		free(str);
	}
	
	a();

	char **test = malloc(5 * sizeof(char*));
	if(!(test))
	{
		printf("test null\n");
		return 0;
	}
	for (int i = 0; i < 4; i++)
	{
		char *a = malloc(2);
		test[i] = a;
	}
	test[20][1] = '\0';

	print("Amonos\n");
	return 20;
}