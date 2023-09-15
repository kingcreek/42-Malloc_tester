/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:07 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/15 20:43:06 by imurugar         ###   ########.fr       */
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
struct HashMap myMap;


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
	// fprintf(stdout, "Finished tester\n");

	if (end_program == 1)
		fprintf(stdout, "Finished tester\n");
		/*
		fprintf(stdout, "Finished tester\n\
		Total memory allocated by you:%zu\n\
		Total memory freed by you:%zu\n\
		Total unreleased memory and you pray for the system to release it:%zu\n",
				allocated_bytes, freed_bytes, allocated_bytes - freed_bytes);
		*/
}

void program_finish()
{
	lock_mutex_malloc();
	// if (malloc_counter == 0)
	//	fprintf(stdout, "Finished tester\n");

	if (end_program == 1)
		fprintf(stdout, "Finished tester\n");
		/*
		fprintf(stdout, "Finished tester\n\
		Total memory allocated by you:%zu\n\
		Total memory freed by you:%zu\n\
		Total unreleased memory and you pray for the system to release it:%zu\n",
				allocated_bytes, freed_bytes, allocated_bytes - freed_bytes);
		*/

	unlock_mutex_malloc();
}

//static uint8_t alternate_stack[SIGSTKSZ];
__attribute__((constructor)) static void init()
{
	lock_mutex_malloc();
	signal(SIGSEGV, segfault_handler);
	
	/* setup alternate stack */
    // {
    //   //stack_t ss = {};
    //   /* malloc is usually used here, I'm not 100% sure my static allocation
    //      is valid but it seems to work just fine. */
	// 	 /*
    //   ss.ss_sp = (void*)alternate_stack;
    //   ss.ss_size = SIGSTKSZ;
    //   ss.ss_flags = 0;
	//   */

    //   //if (sigaltstack(&ss, NULL) != 0) { err(1, "sigaltstack"); }
    // }

    // /* register our signal handlers */
    // {
    //   struct sigaction sig_action = {};
    //   sig_action.sa_sigaction = segfault_handler;
    //   sigemptyset(&sig_action.sa_mask);

    //   #ifdef __APPLE__
    //       /* for some reason we backtrace() doesn't work on osx
    //          when we use an alternate stack */
    //       sig_action.sa_flags = SA_SIGINFO;
    //   #else
    //       sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;
    //   #endif

    //   if (sigaction(SIGSEGV, &sig_action, NULL) != 0) { err(1, "sigaction"); }
    //   if (sigaction(SIGINT,  &sig_action, NULL) != 0) { err(1, "sigaction"); }
    // }
	
	/*
	struct sigaction sa;
	sa.sa_handler = segfault_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGSEGV, &sa, NULL);
	*/

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
	// if (malloc_counter == 0)
	//	fprintf(stdout, "Finished tester\n");

	if (end_program == 1)
		fprintf(stdout, "Finished tester\n");
		/*
		fprintf(stdout, "Finished tester\n\
		Total memory allocated by you:%zu\n\
		Total memory freed by you:%zu\n\
		Total unreleased memory and you pray for the system to release it:%zu\n",
				allocated_bytes, freed_bytes, allocated_bytes - freed_bytes);
		*/

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
			return Real__malloc(sz);;
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
					// if (write_in_file(file_path, address_str))
					// {
					// 	malloc_counter++;
					// 	free(strings);
					// 	unlock_mutex_malloc();
					// 	errno = ENOMEM;
					// 	return NULL;
					// }
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
	
	return Real__malloc(sz);
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