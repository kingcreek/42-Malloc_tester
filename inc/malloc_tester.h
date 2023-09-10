/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:50 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/10 13:11:58 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if !defined(MALLOC_TESTER_H)
#define MALLOC_TESTER_H

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/stat.h>
#if defined(__APPLE__)
#include <malloc/malloc.h>
#define malloc_usable_size malloc_size
#elif defined(__ELF__)
#include <malloc.h>
#endif


void get_function_names(char *input);
void get_trace();
void program_finish();
void get_program_name(char *program_name);
int write_in_file(const char *file, const char *str);
int find_in_file(const char *file, const char *str);
void *real_malloc_and_update(size_t sz);

#endif