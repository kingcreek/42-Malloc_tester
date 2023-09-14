/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:50 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/14 18:57:13 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if !defined(MALLOC_TESTER_H)
#define MALLOC_TESTER_H

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <err.h>
#include <execinfo.h>
#if defined(__APPLE__)
#include <malloc/malloc.h>
#define malloc_usable_size malloc_size
#elif defined(__ELF__)
#include <malloc.h>
#endif

#define PROGRAM_NAME_BUFFER_SIZE 512

/*FILE FUNCTIONS*/
void get_function_names(char *input);
void get_trace();
void program_finish();
void get_program_name(char *program_name, size_t size);

/*FILE FUNCTIONS*/
int write_in_file(const char *file, const char *str);
int find_in_file(const char *file, const char *str);
int write_in_file_simple(const char *file, const char *str);


#endif