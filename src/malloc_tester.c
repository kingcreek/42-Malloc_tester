/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:07 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/16 15:32:52 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE

#include "../inc/malloc_tester.h"
#include "../inc/interpose.h"

#define MAX_CALLSTACK_SIZE 255
void *callstack[MAX_CALLSTACK_SIZE];

char file_path[256] = {0};

int ignore_malloc = 1;
size_t allocated_bytes;
size_t freed_bytes;
int malloc_counter;
int end_program = 1;
char program_name[512] = {0};
int totalMallocs = 0;

static void lock_mutex_malloc() { ignore_malloc = 1; }
static void unlock_mutex_malloc() { ignore_malloc = 0; }

void segfault_handler(int sig)
{
		write(1, "segmentation fault\n", 19);
		lock_mutex_malloc();
		if (end_program != 1)
			get_trace();
		exit(139);
}

void close_handler(int signo)
{
	if (end_program == 1)
		fprintf(stdout, "Finished tester SIGINT\n");
	exit(0);
}

void program_finish()
{
	lock_mutex_malloc();
	if (end_program == 1)
		fprintf(stdout, "Finished tester atexit\n");
	exit(-198156);
	unlock_mutex_malloc();
}

__attribute__((constructor)) static void init()
{
	lock_mutex_malloc();
	fprintf(stdout, "init tester\n");
	signal(SIGSEGV, segfault_handler);
	signal(SIGINT, close_handler);
	atexit(program_finish);
	const char *home_dir = getenv("HOME");
	if (home_dir != NULL)
		snprintf(file_path, sizeof(file_path), "%s/.malloc_tester/address.0x00", home_dir);
	
	totalMallocs = 0;
	if (home_dir != NULL)
		totalMallocs = read_int_from_file(file_path);
	ignore_malloc = 0;
	allocated_bytes = 0;
	freed_bytes = 0;
	malloc_counter = 0;
	get_program_name(program_name, sizeof(program_name));
	unlock_mutex_malloc();
}

INTERPOSE_C_VOID(exit, (int status), (status))
{
	lock_mutex_malloc();
	if (end_program == 1 && status != -198156)
		fprintf(stdout, "Finished tester exit\n");
	unlock_mutex_malloc();
	Real__exit(status);
}

INTERPOSE_C(void *, malloc, (size_t sz), (sz))
{
	if (ignore_malloc == 0)
	{
		lock_mutex_malloc();
		void *caller = NULL;
		int size = backtrace(callstack, sizeof(callstack) / sizeof(callstack[0]));
		char **strings = backtrace_symbols(callstack, size);
		if (size <= 0)
		{
			unlock_mutex_malloc();
			void *result = Real__malloc(sz);
			return result;
		}
		caller = callstack[1];

		if (caller < (void *)0x7f0000000000)
		{
			if (strstr(strings[1], program_name) != NULL)
			{
				char address_str[20];
				if (sscanf(strings[1], "%*[^+]+%[^)]", address_str) == 1)
				{
					if(malloc_counter >= totalMallocs)
					{
						malloc_counter++;
						end_program = 0;
						free(strings);
						totalMallocs++;
						char n[20];
						snprintf(n, sizeof(n), "%d", totalMallocs);
						write_in_file_replace(file_path, n);
						unlock_mutex_malloc();
						errno = ENOMEM;
						return NULL;
					}
					malloc_counter++;
				}
				void *result = Real__malloc(sz);
				allocated_bytes += malloc_usable_size(result);
				free(strings);
				unlock_mutex_malloc();
				return result;
			}

			unlock_mutex_malloc();
			free(strings);
			return Real__malloc(sz);
			//return NULL;
		}
		unlock_mutex_malloc();
	}
	void *result = Real__malloc(sz);
	// allocated_bytes += malloc_usable_size(result);
	return result;
}

INTERPOSE_C_VOID(free, (void *p), (p))
{
	/*
	if (ignore_malloc == 0)
	{
		// lock_mutex_malloc();
		void *caller = NULL;
		int size = backtrace(callstack, sizeof(callstack) / sizeof(callstack[0]));
		char **strings = backtrace_symbols(callstack, size);
		if (size <= 0)
		{
			unlock_mutex_malloc();
			// freed_bytes += malloc_usable_size(p);
			Real__free(p);
			Real__free(strings);
			unlock_mutex_malloc();
			return;
		}
		caller = callstack[3];
		if (caller < (void *)0x7f0000000000)
		{
			if (strstr(strings[1], program_name) != NULL)
				freed_bytes += malloc_usable_size(p);
			// fprintf(stderr, "freed_bytes: %zu\n", freed_bytes);
		}
		Real__free(strings);
	}

	// freed_bytes += malloc_usable_size(p);
	// unlock_mutex_malloc();
	*/
	Real__free(p);
}