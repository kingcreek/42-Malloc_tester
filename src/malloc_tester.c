/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:07 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/09 13:25:15 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE

//#include "../inc/stacktrace.h"
#include "../inc/malloc_tester.h"
#include "../inc/interpose.h"

#define MAX_CALLSTACK_SIZE 255
void *callstack[MAX_CALLSTACK_SIZE];

const char *filename = "address.0x00";
int call_nbr;
int ignore_malloc = 1;
size_t allocated_bytes;
size_t freed_bytes;
int malloc_counter;
int currentFunctionCall = 0;

static void lock_mutex_malloc() { ignore_malloc = 1; }
static void unlock_mutex_malloc() { ignore_malloc = 0; }

void segfault_handler(int signo) {
	lock_mutex_malloc();
	if (malloc_counter != 0)
    	get_trace();
	exit(139);
}

__attribute__((constructor)) static void init()
{
	lock_mutex_malloc();
	signal(SIGSEGV, segfault_handler);
	atexit(program_finish);
	ignore_malloc = 0;
	allocated_bytes = 0;
	freed_bytes = 0;
	malloc_counter = 0;
	call_nbr = get_call_nbr(filename) == -1 ? 0 : get_call_nbr(filename);
	unlock_mutex_malloc();
}

void program_finish()
{
	lock_mutex_malloc();
	if (malloc_counter == 0)
		fprintf(stdout, "Finished tester\n");
}

INTERPOSE_C_VOID(exit, (int status), (status))
{
	lock_mutex_malloc();
	//get_trace();
	if (malloc_counter == 0)
		fprintf(stdout, "Finished tester\n");
	unlock_mutex_malloc();
	Real__exit(status);
	
}


INTERPOSE_C(void *, malloc, (size_t sz), (sz))
{

	if (ignore_malloc == 0)
	{

		lock_mutex_malloc();
		void *caller = NULL;
		int frames = backtrace(callstack, sizeof(callstack) / sizeof(callstack[0]));
		if (frames <= 0)
		{
			unlock_mutex_malloc();
			void *result = Real__malloc(sz);
			allocated_bytes += malloc_usable_size(result);
			return result;
		}
		caller = callstack[1];
		
		if (caller < (void *)0x7f0000000000)
		{
			//  Skip previus malloc function calls
			
			if (currentFunctionCall < call_nbr)
			{
				currentFunctionCall++;
				void *result = Real__malloc(sz);
				allocated_bytes += malloc_usable_size(result);
				unlock_mutex_malloc();
				return result;
			}
			//fprintf(stdout, "hooked %d frames, try alloc %zu\n", frames, sz);
			//fprintf(stderr, "Try crash in! %p\n", caller);
			//  save call number in file
			save_call_nbr(filename, ++call_nbr);
			currentFunctionCall++;
			malloc_counter++;
			/*
			//perform malloc only for storage memory requested
			void *result = Real__malloc(sz);
			allocated_bytes += malloc_usable_size(result);
			free(result);
			*/
			unlock_mutex_malloc();
			return NULL;
		}
		// fprintf(stderr, "Caught a call to malloc(%zu)\n", sz);
		unlock_mutex_malloc();
	}
	void *result = Real__malloc(sz);
	allocated_bytes += malloc_usable_size(result);
	return result;
}

INTERPOSE_C_VOID(free, (void *p), (p))
{
	if(ignore_malloc == 0)
	{
		freed_bytes += malloc_usable_size(p);
	}
	Real__free(p);
}

/*
INTERPOSE_C(void *, calloc, (size_t n, size_t sz), (n, sz))
{
	fprintf(stderr, "azsdsd");
	void *result = Real__calloc(n, sz);
	return result;
}
*/

/*
int my_printf(const char *format, ...) {
	int ret = printf("[+] Hello from interpose\n");
	return ret;
}

__attribute__((used)) static struct { const void *replacement; const void *replacee; } _interpose_printf
__attribute__ ((section ("__DATA,__interpose"))) = { (const void *)(unsigned long)&my_printf, (const void *)(unsigned long)&printf };

*/