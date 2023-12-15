/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:07 by imurugar          #+#    #+#             */
/*   Updated: 2023/12/14 21:06:10 by imurugar         ###   ########.fr       */
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
	char *location;
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

// check
//  useful libunwind, otool

#define _XOPEN_SOURCE 700
#include <ucontext.h>
void segfault_handler(int signal, siginfo_t *info, void *context)
{
	lock_mutex_malloc();
	if (signal == SIGABRT)
	{
		write(1, "Program: Abort\n", 15);
		exit(139);
	}
	if (signal == SIGBUS)
	{
		write(1, "Program: Bus error\n", 19);
		exit(139);
	}
	ucontext_t *ucontext = (ucontext_t *)context;
	void *program_counter = NULL;
#ifdef __APPLE__
	program_counter = (void *)ucontext->uc_mcontext->__ss.__rip - getSlide();
#endif
	write(1, "segmentation fault\n", 19);
	if (end_program != 1)
		get_trace(program_counter);
	exit(139);
}

/*
void segfault_handler(int sig) // I catch you!
{
	lock_mutex_malloc();
	write(1, "segmentation fault\n", 19);
	if (end_program != 1)
		get_trace();
	exit(139);
}*/

void program_finish()
{
	lock_mutex_malloc();
	if (end_program == 1)
	{
		if (calculate_leaks())
			fprintf(stdout, "Test completed correctly\n");
	}
	else
		exit(138);
	unlock_mutex_malloc();
}
/*
#ifdef __SANITIZE_ADDRESS__
#include <sanitizer/lsan_interface.h>
#endif

__attribute__((destructor)) static void destructor()
{
	fprintf(stdout, "Test  2\n");
	lock_mutex_malloc();
	if (end_program == 1)
	{
		if (calculate_leaks())
			fprintf(stdout, "Test completed correctly\n");
	}
	#ifdef __SANITIZE_ADDRESS__
		__lsan_do_leak_check();
	#endif
	unlock_mutex_malloc();
}
*/

__attribute__((constructor)) static void init()
{
	lock_mutex_malloc();
	pthread_mutex_init(&malloc_mutex, NULL);
	// TO TEST

	struct sigaction sa;
	sa.sa_sigaction = segfault_handler;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &sa, NULL);
	sigaction(SIGBUS, &sa, NULL);
	sigaction(SIGABRT, &sa, NULL);

	// signal(SIGSEGV, segfault_handler);
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
#ifdef __APPLE__
				allocations[malloc_counter].location = malloc_location(program_name, callstack[1] - getSlide());
#else
				allocations[malloc_counter].location = malloc_location(strings[1], callstack[1]);
#endif

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
			pthread_mutex_unlock(&malloc_mutex);
			return;
		}
		pthread_mutex_lock(&malloc_mutex);
		if (strstr(strings[1], program_name) != NULL)
		{
			int freed = 0;
			for (int i = 0; i < malloc_counter; i++)
			{
				if (allocations[i].ptr == p)
				{
					allocations[i].size = 0;
					allocations[i].ptr = (void *)0;
					freed = 1;
					break;
				}
			}
			/*
			if (freed == 0)
			{
				print_double_free(strings, callstack, size);
				exit(1);
			}
			*/
		}
		Real__free(strings);
		Real__free(p);
		unlock_mutex_malloc();
		pthread_mutex_unlock(&malloc_mutex);
		return;
	}

	Real__free(p);
}

int calculate_leaks()
{
	char file_path[256] = {0};
	const char *home_dir = getenv("HOME");
	if (home_dir != NULL)
		snprintf(file_path, sizeof(file_path), "%s/.malloc_tester/leaks", home_dir);
	size_t totalMem = 0;
	for (int i = 0; i < malloc_counter; i++)
	{
		totalMem += allocations[i].size;
		if (allocations[i].size > 0)
		{
			char info[256] = {0};
			snprintf(info, sizeof(info), "Total %ld bytes in %p", allocations[i].size, allocations[i].location);
			write_in_file_simple(file_path, info);
			write_in_file_simple(file_path, allocations[i].location);
		}
		free(allocations[i].location);
	}
	/*
	if (totalMem > 0)
		return (fprintf(stdout, " \n ===LEAKS=== \n Total memory not released by you: %lu bytes\n", totalMem), 0);
	*/
	return (1);
}

intptr_t getSlide()
{
#ifdef __APPLE__
	const struct mach_header *header;
	intptr_t slide = 0;

	header = _dyld_get_image_header(0); // 0 significa el binario principal

	if (header != NULL)
	{
		slide = _dyld_get_image_vmaddr_slide(0);
		// printf("Slide: %#lx\n", (unsigned long)slide);
	}
	return slide;
#else
	return 0;
#endif
}
