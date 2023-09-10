/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_helper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 13:31:58 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/10 12:14:08 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/malloc_tester.h"

int save_call_nbr(const char *filename, int nbr)
{
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

	if (fd == -1)
		return (0);

	ssize_t bytes_written = write(fd, &nbr, sizeof(int));
	if (bytes_written != sizeof(int))
	{
		close(fd);
		return (0);
	}

	close(fd);
	return 1;
}

int get_call_nbr(const char *filename)
{
	int fd = open(filename, O_RDONLY);
	int nbr;

	if (fd == -1)
		return (-1);

	ssize_t bytes_read = read(fd, &nbr, sizeof(int));
	if (bytes_read != sizeof(int))
	{
		close(fd);
		return (-1);
	}
	close(fd);
	return nbr;
}

