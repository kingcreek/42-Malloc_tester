#include <stdlib.h>

int main()
{
	char *str;
	str = malloc(20);
	if(!str)
		return 0;
	while(*str)
	{
		str++;
	}
	free(str);
	return 0;
}