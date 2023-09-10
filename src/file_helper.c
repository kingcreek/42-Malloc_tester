/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_helper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 13:31:58 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/10 14:59:09 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/malloc_tester.h"

int find_in_file(const char *file, const char *str) {
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    char buffer[20000];
    ssize_t readSize;

    while ((readSize = read(fd, buffer, sizeof(buffer))) > 0) {
        if (strstr(buffer, str) != NULL) {
            close(fd);
            return 1;
        }
    }
    close(fd);
    return 0;
}

int write_in_file(const char *str) {
	const char *home_dir = getenv("HOME");
	char file_path[256];
	if (home_dir != NULL) {
		snprintf(file_path, sizeof(file_path), "%s/malloc_tester/address.0x00", home_dir);
	}
	
    if (find_in_file(file_path, str) == 0) {
        int fd = open(file_path, O_WRONLY | O_APPEND | O_CREAT, 0644);
        if (fd == -1) {
            perror("open");
            return -1;
        }

        write(fd, str, strlen(str));
        write(fd, "\n", strlen("\n"));
        close(fd);
        return 1;
    } else {
        return 0;
    }
}