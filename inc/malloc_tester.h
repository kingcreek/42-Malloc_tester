/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:50 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/15 20:43:21 by imurugar         ###   ########.fr       */
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
#if defined(__APPLE__)
#include <malloc/malloc.h>
#define malloc_usable_size malloc_size
#elif defined(__ELF__)
#include <malloc.h>
#endif

#define PROGRAM_NAME_BUFFER_SIZE 512
#define MAX_SIZE 5000

/*FILE FUNCTIONS*/
void get_function_names(char *input);
void get_trace();
void program_finish();
void get_program_name(char *program_name, size_t size);

/*FILE FUNCTIONS*/
int write_in_file(const char *file, const char *str);
int find_in_file(const char *file, const char *str);
int write_in_file_simple(const char *file, const char *str);
int read_int_from_file(const char *path);
int write_in_file_replace(const char *file, const char *str);
void incrementOrInsertInFile(const char* filename, const char* targetKey);

/*HasMap*/
struct KeyValue {
    char key[20];
    int value;
};

struct HashMap {
    struct KeyValue data[MAX_SIZE];
    int size;
};

void initialize(struct HashMap *hashmap);
int get(struct HashMap* hashmap, const char* key);
void put(struct HashMap* hashmap, const char* key, int value);
void incrementOrInsert(struct HashMap* hashmap, const char* key);

#endif