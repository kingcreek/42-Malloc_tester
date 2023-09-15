/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_helper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 13:31:58 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/15 20:43:02 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/malloc_tester.h"

int find_in_file(const char *file, const char *str)
{
	int fd = open(file, O_RDONLY);
	if (fd == -1)
	{
		perror("open");
		return -1;
	}

	char buffer[20000];
	ssize_t readSize;

	while ((readSize = read(fd, buffer, sizeof(buffer))) > 0)
	{
		if (strstr(buffer, str) != NULL)
		{
			close(fd);
			return 1;
		}
	}
	close(fd);
	return 0;
}

int write_in_file(const char *file, const char *str)
{
	if (find_in_file(file, str) == 0)
	{
		int fd = open(file, O_WRONLY | O_APPEND | O_CREAT, 0644);
		if (fd == -1)
		{
			return -1;
		}

		write(fd, str, strlen(str));
		write(fd, "\n", strlen("\n"));
		close(fd);
		return 1;
	}
	else
	{
		return 0;
	}
}

int write_in_file_simple(const char *file, const char *str)
{
	int fd = open(file, O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (fd == -1)
	{
		return -1;
	}

	write(fd, str, strlen(str));
	write(fd, "\n", strlen("\n"));
	close(fd);
	return 1;
}

int write_in_file_replace(const char *file, const char *str)
{
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        return -1;
    }

    write(fd, str, strlen(str));
    close(fd);
    return 1;
}

int read_int_from_file(const char *path) {
    int fd;            
    char buffer[1024]; 
    ssize_t bytes;
    int ret;

    fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el archivo");
    }
    bytes = read(fd, buffer, sizeof(buffer));
    if (bytes == -1) {
        perror("Error al leer el archivo");
        close(fd);
        return -1; 
    }
    ret = atoi(buffer);
    close(fd);

    return ret;
}

void incrementOrInsertInFile(const char* filename, const char* targetKey) {
    FILE* file = fopen(filename, "r+"); // Abrir el archivo en modo lectura y escritura

    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    char line[100];
    char key[50];
    int value;
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%s %d", key, &value) == 2) {
            if (strcmp(key, targetKey) == 0) {
                value++;
                fseek(file, -strlen(line), SEEK_CUR);
                fprintf(file, "%s %d\n", key, value);
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        // La clave no existe, agregar una nueva línea
        fprintf(file, "%s 1\n", targetKey);
    }

    fclose(file);
}

