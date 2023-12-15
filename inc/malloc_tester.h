/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:50 by imurugar          #+#    #+#             */
/*   Updated: 2023/12/14 21:07:29 by imurugar         ###   ########.fr       */
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
#include "sys/errno.h"
#include <execinfo.h>
#include <pthread.h>
#if defined(__APPLE__)
#include <malloc/malloc.h>
#include <mach-o/dyld.h>
#define malloc_usable_size malloc_size
#elif defined(__ELF__)
#include <malloc.h>
#endif

#define PROGRAM_NAME_BUFFER_SIZE 512

/*Functions that do things*/
void get_function_names(char *input);
void get_trace(void *program_counter);
void program_finish();
void get_program_name(char *program_name, size_t size);
void get_main_code_range_linux(uintptr_t* start, uintptr_t* end);

/*As you can see, file functions... Between you and me, i don't use half, but maybe tomorrow i will, right?*/
int write_in_file(const char *file, const char *str);
int find_in_file(const char *file, const char *str);
int write_in_file_simple(const char *file, const char *str);
int read_int_from_file(const char *path);
int write_in_file_replace(const char *file, const char *str);
int calculate_leaks();
char *malloc_location(const char *program_name, void const *const addr);
//void print_double_free(char **strings, void *stack_traces[], int size);
intptr_t getSlide();

#endif
