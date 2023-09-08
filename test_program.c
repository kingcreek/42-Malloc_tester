#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int print(char *str)
{
	while(*str)
	{
		write(1, str, 1);
		str++;
	}
}
int main()
{

	char *str;
	
	str = malloc(1337);
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
	
	str = malloc(1338);
	if(!str)
		exit (0);
	str[1] = '\0';
	free(str);

	char **test = malloc(50);
	if(!test)
		exit(0);
	for (int i = 0; i < 49; i++)
	{
		char *a = malloc(2);
		test[i] = a;
	}
	test[20][1] = '\0';
	
	
	return 0;
}