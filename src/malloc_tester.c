/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:07 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/08 13:51:34 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE

#include "../inc/malloc_tester.h"
#include "../inc/interpose.h"

#define MAX_CALLSTACK_SIZE 255
void *callstack[MAX_CALLSTACK_SIZE];

const char *filename = "address.0x00";
const char direccionString[20];
int call_nbr;
int clean_exit;
int ignore_malloc;
size_t allocated_bytes = 0;
size_t freed_bytes = 0;

void handler(int sig)
{
	fprintf(stderr, "Salida del programa");
	void *callstack[128];
	int frames = backtrace(callstack, 128);
	backtrace_symbols_fd(callstack, frames, STDOUT_FILENO);
	exit(0);
}

static void lock_mutex_malloc() { ignore_malloc = 1; }

static void unlock_mutex_malloc() { ignore_malloc = 0; }

void program_finish()
{
	/*
	lock_mutex_malloc();
	//Program finish without free all memory, posible exit(0) or leaks
	if (allocated_bytes >= freed_bytes)
	{
		fprintf(stdout, "program finish leaks?\n");
		clean_exit = 0;
		int file = open("repeater", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		close(file);
	}
	else
	{
		fprintf(stdout, "program finish!\n");
		//maybe a clean exit
		remove("repeater");
	}
	unlock_mutex_malloc();
	*/
	
	lock_mutex_malloc();
	fprintf(stdout, "program finish atexit\n");
	if (clean_exit)
		remove("repeater");
	unlock_mutex_malloc();
	
}

__attribute__((constructor)) static void init()
{
	atexit(program_finish);
	// signal(SIGINT, handler); //SIGSEGV
	fprintf(stderr, "malloc tester init.\n");
	clean_exit = 1;
	ignore_malloc = 0;
	lock_mutex_malloc();
	call_nbr = get_call_nbr(filename) == -1 ? 0 : get_call_nbr(filename);
	unlock_mutex_malloc();
}

INTERPOSE_C_VOID(exit, (int status), (status))
{
	/*
	lock_mutex_malloc();
	//Program finish without free all memory, posible exit(0) or leaks
	if (allocated_bytes >= freed_bytes)
	{
		clean_exit = 0;
		fprintf(stderr, "Program exit in exit(%d) function\n", status);
		int file = open("repeater", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		close(file);
	}
	else
	{
		fprintf(stdout, "program finish in exit!\n");
		//maybe a clean exit
		remove("repeater");
	}
	unlock_mutex_malloc();
	Real__exit(status);
	*/
	lock_mutex_malloc();
	clean_exit = 0;
	fprintf(stderr, "Program finish in exit(%d) function\n", status);
	int file = open("repeater", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	close(file);
	unlock_mutex_malloc();
	Real__exit(status);
}

#define PRINT_CALLER() fprintf(stderr, "Caller function: %s\n", __PRETTY_FUNCTION__)

INTERPOSE_C(void *, malloc, (size_t sz), (sz))
{

	if (ignore_malloc == 0)
	{
		static int currentFunctionCall = 0;
		// fprintf(stderr, "Malloc called\n");
		//  Interactive
		/*
		fflush(stdout);
		fprintf(stderr, "Malloc detected, try crash? y/n\n");

		char buffer[256];
		ssize_t bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
		if (bytesRead > 0) {
			buffer[bytesRead] = '\0'; // Agrega un terminador nulo al final del buffer
			if (strcmp(buffer, "y\n") == 0)
				return NULL;
			else
				return Real__malloc(sz);
		}
		return Real__malloc(sz);
		*/

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
			// get_trace();
			//  Skip previus malloc function calls
			if (currentFunctionCall < call_nbr)
			{
				currentFunctionCall++;
				void *result = Real__malloc(sz);
				allocated_bytes += malloc_usable_size(result);
				return result;
			}
			// fprintf(stderr, "hooked %d frames, try alloc %zu\n", frames, sz);
			// fprintf(stderr, "Try crash in! %p\n", caller);
			//  save call number in file
			save_call_nbr(filename, ++call_nbr);
			currentFunctionCall++;
			//perform malloc only for storage memory requested
			void *result = Real__malloc(sz);
			allocated_bytes += malloc_usable_size(result);
			free(result);
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
	freed_bytes += malloc_usable_size(p);
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
