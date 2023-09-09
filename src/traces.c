/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traces.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:02 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/09 13:17:54 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/malloc_tester.h"

// atos -o ./a.out -l 0x7fff5fc00000 - macOs ¿need relative or absolute addr? i dont know, u_u, try 
// addr2line -e ./a.out + 0x1224 - linux

void generate_addr2line_command(const char *input, char *output, size_t output_size)
{
    char program_name[256];
    unsigned long long address;

    int result = sscanf(input, "%[^+(]%*c%llx", program_name, &address);

    if (result != 2)
    {
        snprintf(output, output_size, "Error: Formato incorrecto.");
    }
    else
    {
        #ifdef __APPLE__
			/* apple does things differently... */
			//sprintf(addr2line_cmd,"atos -o %.256s %p", program_name, addr); 
			snprintf(output, output_size, "atos -o %s %#llx", program_name, address);
		#else
			snprintf(output, output_size, "addr2line -e %s + %#llx", program_name, address);
		#endif
        //snprintf(output, output_size, "addr2line -e %s + %#llx", program_name, address);
    }
}

void get_program_name(char *program_name)
{
	pid_t my_pid = getpid();
	char proc_path[1024];
	ssize_t len;

	snprintf(proc_path, sizeof(proc_path), "/proc/%d/cmdline", my_pid);

	int fd = open(proc_path, O_RDONLY);
	if (fd == -1)
	{
		return;
	}
	len = read(fd, program_name, sizeof(program_name) - 1);
	close(fd);

	if (len > 0)
	{
		program_name[len] = '\0';
	}
}

void get_trace()
{
	fprintf(stdout, "seg fault handle\n");
	void *callstack[40] = {0};
	char program_name[256] = {0};
	char cmd[256] = {0};
	size_t size;
	char **strings;
	size_t i;

	size = backtrace(callstack, sizeof(callstack) / sizeof(callstack[0]));
	strings = backtrace_symbols(callstack, size);

	if (strings == NULL)
	{
		return;
	}
	get_program_name(program_name);

	for (i = 0; i < size; i++)
	{
		if (strstr(strings[i], program_name) != NULL)
		{
			generate_addr2line_command(strings[i], cmd, sizeof(cmd));
			fprintf(stdout, "%s\n", cmd);
			break;
		}
	}

	free(strings);
}
