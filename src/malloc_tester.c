/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:07 by imurugar          #+#    #+#             */
/*   Updated: 2023/12/09 13:44:29 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE

#include "../inc/malloc_tester.h"
#include "../inc/interpose.h"

#define MAX_CALLSTACK_SIZE 255
void *callstack[MAX_CALLSTACK_SIZE];

char file_path[256] = {0};

#define MAX_ALLOCATIONS 99999

typedef struct
{
	void *ptr;
	size_t size;
} AllocationRecord;

AllocationRecord allocations[MAX_ALLOCATIONS];

int ignore_malloc = 1;
int malloc_counter;
int end_program = 1;
char program_name[512] = {0};
int totalMallocs = 0;

pthread_mutex_t malloc_mutex;
static void lock_mutex_malloc() { ignore_malloc = 1; }	 // Personality disorder
static void unlock_mutex_malloc() { ignore_malloc = 0; } // His brother

void segfault_handler(int sig) // I catch you!
{
	write(1, "segmentation fault\n", 19);
	lock_mutex_malloc();
	if (end_program != 1)
		get_trace();
	exit(139);
}

/*
void close_handler(int signo)
{
	fprintf(stdout, " tester SIGINT\n");
	if (end_program == 1)
		fprintf(stdout, "Finished tester SIGINT\n");
}
*/

void program_finish()
{
	// fprintf(stdout, " tester atexit\n");
	lock_mutex_malloc();
	if (end_program == 1)
	{
		size_t totalMem = 0;
		for(int i = 0; i < malloc_counter; i++)
			totalMem += allocations[i].size;
		if (totalMem > 0)
			fprintf(stdout, " \n ===LEAKS=== \nMemory not released by you: %lu\n", totalMem);
		fprintf(stdout, "Finished tester atexit\n");
	}
	unlock_mutex_malloc();
}

__attribute__((constructor)) static void init()
{
	lock_mutex_malloc();
	pthread_mutex_init(&malloc_mutex, NULL);
	signal(SIGSEGV, segfault_handler);
	// signal(SIGINT, close_handler);
	atexit(program_finish);
	const char *home_dir = getenv("HOME");
	if (home_dir != NULL)
		snprintf(file_path, sizeof(file_path), "%s/.malloc_tester/address.0x00", home_dir);

	if (home_dir != NULL)
		totalMallocs = read_int_from_file(file_path);
	ignore_malloc = 0;
	malloc_counter = 0;
	get_program_name(program_name, sizeof(program_name));
	unlock_mutex_malloc();
}

/*
INTERPOSE_C_VOID(exit, (int status), (status)) //
{
	fprintf(stdout, " tester exit\n");
	lock_mutex_malloc();
	if (end_program == 1)
		fprintf(stdout, "Finished tester exit\n");
	unlock_mutex_malloc();
	Real__exit(status);
}
*/

INTERPOSE_C(void *, malloc, (size_t sz), (sz)) // Magic
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
			pthread_mutex_lock(&malloc_mutex);
			if (strstr(strings[1], program_name) != NULL)
			{
				// char address_str[40];
				// if (sscanf(strings[1], "%*[^+]+%[^)]", address_str) == 1)
				//{
				if (malloc_counter >= totalMallocs) // this malloc are not tested yet, let's break
				{
					// simple counters
					end_program = 0;
					malloc_counter++;
					totalMallocs++;
					// write in file to next iterations
					char n[100];
					snprintf(n, sizeof(n), "%d", totalMallocs);
					write_in_file_replace(file_path, n);
					// set errno
					errno = ENOMEM;
					// free & unlock
					free(strings);
					pthread_mutex_unlock(&malloc_mutex);
					unlock_mutex_malloc();
					return NULL;
				}
				
				void *result = Real__malloc(sz);
				allocations[malloc_counter].ptr = result;
				allocations[malloc_counter].size = sz;
				free(strings);
				pthread_mutex_unlock(&malloc_mutex);
				malloc_counter++;
				unlock_mutex_malloc();
				return result;
			}
			// This call does not come from the program, run!
			pthread_mutex_unlock(&malloc_mutex);
			unlock_mutex_malloc();
			free(strings);
			return Real__malloc(sz);
		}
		unlock_mutex_malloc();
	}
	// I have no fucking idea where this call is coming from, but I really don't care. System defeated!
	return Real__malloc(sz);
}

INTERPOSE_C_VOID(free, (void *p), (p)) // Maybe one day i will remember you and implement you, but for now ours cannot be
{

	if (ignore_malloc == 0)
	{
		lock_mutex_malloc();
		void *caller = NULL;
		int size = backtrace(callstack, sizeof(callstack) / sizeof(callstack[0]));
		char **strings = backtrace_symbols(callstack, size);
		if (size <= 0)
		{
			// freed_bytes += malloc_usable_size(p);
			Real__free(p);
			Real__free(strings);
			unlock_mutex_malloc();
			return;
		}

		/*
		for(int i = 0; i < size; i++)
		{
			fprintf(stderr, "in free: %d %s\n", i, strings[i]);
		}
		*/

		if (strstr(strings[1], program_name) != NULL)
		{
			for(int i = 0; i < malloc_counter; i++)
			{
				if (allocations[i].ptr == p)
				{
					allocations[i].size = 0;
					break;
				}
			}
			//freed_bytes += malloc_size(p);
			//fprintf(stderr, "total freed: %lu\n", freed_bytes);
		}
		/*
		caller = callstack[1];
		if (caller < (void *)0x7f0000000000)
		{
			if (strstr(strings[1], program_name) != NULL)
				freed_bytes += malloc_usable_size(p);
			// fprintf(stderr, "freed_bytes: %zu\n", freed_bytes);
		}
		*/

		Real__free(strings);
		Real__free(p);
		unlock_mutex_malloc();
		return;
	}

	// freed_bytes += malloc_usable_size(p);
	// unlock_mutex_malloc();

	Real__free(p);
}