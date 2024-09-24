#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signals.h>

#define USAGE_ERR "Usage: ./Deamon FilePath/Filename\n"

int main ( int ac, char **av)
{
	char *basePath = NULL;
	char *token = NULL;

	if (ac != 2) return (write(2, USAGE_ERR, strlen(USAGE_ERR) + 1));
	
	basePath = malloc(sizeof(char *) + strlen(av[1]) + 1);
	if (!basePath) return 1;
	strcpy(basePath, av[1]);
	
	token = strtok(basePath, "/");
	while (token != NULL)
	{
		basePath = token;
		token = strtok(NULL, "/");
	}
	
	printf("token (strtok) : %s\n", token);
	printf("BasePath = %s\n", basePath);

	while (1337)
	{
		
	}
}
