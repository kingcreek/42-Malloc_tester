/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:07 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/12 16:06:01 by imurugar         ###   ########.fr       */
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
char program_name[512] = {0};

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
	// fprintf(stdout, "Finished tester\n");

	if (malloc_counter == 0)
		fprintf(stdout, "Finished tester\n\
		Total memory allocated by you:%zu\n\
		Total memory freed by you:%zu\n\
		Total unreleased memory and you pray for the system to release it:%zu\n",
				allocated_bytes, freed_bytes, allocated_bytes - freed_bytes);
}

void program_finish()
{
	lock_mutex_malloc();
	// if (malloc_counter == 0)
	//	fprintf(stdout, "Finished tester\n");

	if (malloc_counter == 0)
		fprintf(stdout, "Finished tester\n\
		Total memory allocated by you:%zu\n\
		Total memory freed by you:%zu\n\
		Total unreleased memory and you pray for the system to release it:%zu\n",
				allocated_bytes, freed_bytes, allocated_bytes - freed_bytes);

	unlock_mutex_malloc();
}

__attribute__((constructor)) static void init()
{
	lock_mutex_malloc();
	// signal(SIGSEGV, segfault_handler);

	struct sigaction sa;
	sa.sa_handler = segfault_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGSEGV, &sa, NULL);

	signal(SIGINT, close_handler);
	atexit(program_finish);
	const char *home_dir = getenv("HOME");
	if (home_dir != NULL)
		snprintf(file_path, sizeof(file_path), "%s/.malloc_tester/address.0x00", home_dir);

	ignore_malloc = 0;
	allocated_bytes = 0;
	freed_bytes = 0;
	malloc_counter = 0;
	get_program_name(program_name);
	unlock_mutex_malloc();
}

INTERPOSE_C_VOID(exit, (int status), (status))
{
	lock_mutex_malloc();
	// if (malloc_counter == 0)
	//	fprintf(stdout, "Finished tester\n");

	if (malloc_counter == 0)
		fprintf(stdout, "Finished tester\n\
		Total memory allocated by you:%zu\n\
		Total memory freed by you:%zu\n\
		Total unreleased memory and you pray for the system to release it:%zu\n",
				allocated_bytes, freed_bytes, allocated_bytes - freed_bytes);

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
			// allocated_bytes += malloc_usable_size(result);
			return result;
		}
		caller = callstack[1];

		// Dl_info info;
		// if (dladdr(callstack[1], &info) && strstr(info.dli_fname, program_name) != NULL) {
		if (caller < (void *)0x7f0000000000)
		{
			//   Skip previus malloc function calls
			/*
			for (int i = 0; i < size; i++)
			{
				Dl_info info;
				if (dladdr(callstack[i], &info))
				{
					// dladdr(callstack[i], &info);
					fprintf(stderr, "Function number: %d\n", i);
					fprintf(stderr, "Function Name: %s\n", info.dli_sname);
					fprintf(stderr, "Library Path: %s\n", info.dli_fname);
					fprintf(stderr, "Library Base Address: %p\n", info.dli_fbase);
					fprintf(stderr, "Symbol Address: %p\n", info.dli_saddr);
					fprintf(stderr, "-----------------\n\n");
				}
			}
			*/
			if (strstr(strings[1], program_name) != NULL)
			{
				char address_str[20];
				if (sscanf(strings[1], "%*[^+]+%[^)]", address_str) == 1)
				{
					if (write_in_file(file_path, address_str))
					{
						malloc_counter++;
						free(strings);
						unlock_mutex_malloc();
						return NULL;
					}
				}
				void *result = Real__malloc(sz);
				allocated_bytes += malloc_usable_size(result);
				free(strings);
				unlock_mutex_malloc();
				return result;
			}

			unlock_mutex_malloc();
			free(strings);
			return NULL;
		}
		unlock_mutex_malloc();
	}
	void *result = Real__malloc(sz);
	// allocated_bytes += malloc_usable_size(result);
	return result;
}

INTERPOSE_C_VOID(free, (void *p), (p))
{

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
		/*
		for (int i = 0; i < size; i++)
		{
			Dl_info info;
			if (dladdr(callstack[i], &info))
			{
					// dladdr(callstack[i], &info);
					fprintf(stderr, "Function number: %d\n", i);
					fprintf(stderr, "Function Name: %s\n", info.dli_sname);
					fprintf(stderr, "Library Path: %s\n", info.dli_fname);
					fprintf(stderr, "Library Base Address: %p\n", info.dli_fbase);
					fprintf(stderr, "Symbol Address: %p\n", info.dli_saddr);
					fprintf(stderr, "-----------------\n\n");
			}
		}
		*/
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
	Real__free(p);
}
