/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traces.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:02 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/17 03:12:49 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/malloc_tester.h"

// mac atos -o a.out -l 0x100000000 0x1224
// atos -o ./a.out -l 0x7fff5fc00000 - macOs ¿need relative or absolute addr? i dont know, u_u, try
// addr2line -e ./a.out + 0x1224 - linux

void get_program_name(char *program_name, size_t size)
{

#ifdef __APPLE__
	const char *progname = getprogname();
	if (progname != NULL)
	{
		snprintf(program_name, size, "%s", progname);
		return;
	}
	if (size > 0 && program_name != NULL)
	{
		const char *argv0 = NULL;
		extern char **environ;
		if (environ != NULL)
		{
			for (char **env = environ; *env != NULL; ++env)
			{
				if (strncmp(*env, "_=", 2) == 0)
				{
					argv0 = *env + 2;
					break;
				}
			}
		}
		if (argv0 == NULL)
			argv0 = "unknown";
		snprintf(program_name, size, "%s", argv0);
	}
#else
	(void)size;
	pid_t my_pid = getpid();
	char proc_path[254];
	ssize_t len = 0;
	int fd = -1;

	snprintf(proc_path, sizeof(proc_path), "/proc/%d/cmdline", my_pid);
	fd = open(proc_path, O_RDONLY);
	if (fd == -1)
		return;
	size_t offset = 0;
	while ((len = read(fd, program_name + offset, PROGRAM_NAME_BUFFER_SIZE - offset - 1)) > 0 && offset < PROGRAM_NAME_BUFFER_SIZE)
		offset += len;
	close(fd);
	if (offset > 0)
		program_name[offset] = '\0';
#endif
}



void addr2line(const char* program_name, void const *const addr, char *file_path)
{
	char addr2line_cmd[512] = {0};

/* have addr2line map the address to the relent line in the code */
#ifdef __APPLE__
	/* apple does things differently... */
	sprintf(addr2line_cmd, "atos -o %.256s %p", program_name, addr);
	write_in_file_simple(file_path, addr2line_cmd);
#else
	char p_name[256];
	unsigned long long address;
	int result = sscanf(program_name, "%[^+(]%*c%llx", p_name, &address);
	if (result == 2)
	{
		sprintf(addr2line_cmd, "addr2line -e  %s + %#llx", p_name, address);
		write_in_file_simple(file_path, addr2line_cmd);
	}
#endif
}

#define MAX_STACK_FRAMES 10
static void *stack_traces[MAX_STACK_FRAMES];
void get_trace()
{
	char program_name[256] = {0};
	int i, trace_size = 0;
	char **messages = (char **)NULL;
	char file_path[256] = {0};

	const char *home_dir = getenv("HOME");
	if (home_dir != NULL)
		snprintf(file_path, sizeof(file_path), "%s/.malloc_tester/trace", home_dir);
	trace_size = backtrace(stack_traces, MAX_STACK_FRAMES);
	messages = backtrace_symbols(stack_traces, trace_size);

	get_program_name(program_name, sizeof(program_name));

	for (i = 0; i < trace_size; ++i)
	{
		#ifdef __APPLE__
		addr2line(program_name, stack_traces[i], file_path);
		#else
		addr2line(messages[i], stack_traces[i], file_path);
		#endif
	}
	//fprintf(stdout, "----UNRESOLVED TRACE----\n"), fflush(stdout);
	//backtrace_symbols_fd(stack_traces, sizeof(stack_traces) / sizeof(stack_traces[0]), 1);
	if (messages)
		free(messages);

	// #ifdef __APPLE__
	// 	void *callstack[8] = {0};
	// 	size_t size = backtrace(callstack, sizeof(callstack) / sizeof(callstack[0]));
	// 	backtrace_symbols_fd(callstack, sizeof(callstack) / sizeof(callstack[0]), 1);
	// #else

	// void *callstack[40] = {0};
	// char program_name[256] = {0};
	// char cmd[256] = {0};
	// char file_path[256] = {0};
	// size_t size;
	// char **strings;
	// size_t i;

	// // trace file output
	// const char *home_dir = getenv("HOME");
	// if (home_dir != NULL)
	// 	snprintf(file_path, sizeof(file_path), "%s/.malloc_tester/trace", home_dir);

	// size = backtrace(callstack, sizeof(callstack) / sizeof(callstack[0]));
	// strings = backtrace_symbols(callstack, size);

	// if (strings == NULL)
	// 	return;
	// get_program_name(program_name);

	// for (i = 0; i < size; i++)
	// {
	// 	if (strstr(strings[i], program_name) != NULL)
	// 	{
	// 		generate_addr2line_command(strings[i], cmd, sizeof(cmd));
	// 		write_in_file_simple(file_path, cmd);
	// 	}
	// }
	// free(strings);
	// #endif
}
