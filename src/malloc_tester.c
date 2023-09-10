/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:07 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/10 15:09:02 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE

// #include "../inc/stacktrace.h"
#include "../inc/malloc_tester.h"
#include "../inc/interpose.h"

#define MAX_CALLSTACK_SIZE 255
void *callstack[MAX_CALLSTACK_SIZE];

char file_path[256] = {0};

int ignore_malloc = 1;
size_t allocated_bytes;
size_t freed_bytes;
int malloc_counter;

static void lock_mutex_malloc() { ignore_malloc = 1; }
static void unlock_mutex_malloc() { ignore_malloc = 0; }

void segfault_handler(int signo)
{
	lock_mutex_malloc();
	if (malloc_counter != 0)
		get_trace();
	exit(139);
}

void close_handler(int signo)
{
	fprintf(stdout, "Finished tester\n");
}

__attribute__((constructor)) static void init()
{
	lock_mutex_malloc();
	signal(SIGSEGV, segfault_handler);
	signal(SIGINT, close_handler);
	atexit(program_finish);
	const char *home_dir = getenv("HOME");
	char file_path[256];
	if (home_dir != NULL)
		snprintf(file_path, sizeof(file_path), "%s/.malloc_tester/address.0x00", home_dir);
	ignore_malloc = 0;
	allocated_bytes = 0;
	freed_bytes = 0;
	malloc_counter = 0;
	unlock_mutex_malloc();
}

void program_finish()
{
	lock_mutex_malloc();
	if (malloc_counter == 0)
		fprintf(stdout, "Finished tester\n");
	unlock_mutex_malloc();
}

INTERPOSE_C_VOID(exit, (int status), (status))
{
	lock_mutex_malloc();
	if (malloc_counter == 0)
		fprintf(stdout, "Finished tester\n");
	unlock_mutex_malloc();
	Real__exit(status);
}

INTERPOSE_C(void *, malloc, (size_t sz), (sz))
{
	if (ignore_malloc == 0)
	{
		char **strings;

		lock_mutex_malloc();
		void *caller = NULL;
		int size = backtrace(callstack, sizeof(callstack) / sizeof(callstack[0]));
		strings = backtrace_symbols(callstack, size);
		if (size <= 0)
		{
			unlock_mutex_malloc();
			void *result = Real__malloc(sz);
			allocated_bytes += malloc_usable_size(result);
			return result;
		}
		caller = callstack[1];

		if (caller < (void *)0x7f0000000000)
		{
			char program_name[256] = {0};
			get_program_name(program_name);
			//  Skip previus malloc function calls
			// maybe can remove loop, i need test if it's always position 1
			for (int i = 0; i < size; i++)
			{
				if (strstr(strings[i], program_name) != NULL)
				{
					fprintf(stderr, "");
					char address_str[20];
					if (sscanf(strings[i], "%*[^+]+%[^)]", address_str) == 1)
					{
						if (write_in_file(file_path, address_str))
						{
							malloc_counter++;
							unlock_mutex_malloc();
							return NULL;
						}
					}
					void *result = Real__malloc(sz);
					allocated_bytes += malloc_usable_size(result);
					unlock_mutex_malloc();
					return result;
				}
			}
			unlock_mutex_malloc();
			return NULL;
		}
		unlock_mutex_malloc();
	}
	void *result = Real__malloc(sz);
	allocated_bytes += malloc_usable_size(result);
	return result;
}

INTERPOSE_C_VOID(free, (void *p), (p))
{
	if (ignore_malloc == 0)
	{
		freed_bytes += malloc_usable_size(p);
	}
	Real__free(p);
}
