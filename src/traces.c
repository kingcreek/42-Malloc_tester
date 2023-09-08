/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traces.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:02 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/08 10:30:52 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/malloc_tester.h"

void get_function_names(void *address[], int addrNumb) {
    int i;
    char addr2line_cmd[1024];

    for (i = 0; i < addrNumb; i++) {
        sprintf(addr2line_cmd, "addr2line -e ./fractol %p", address[i]);
        fflush(stdout);
        fprintf(stderr, "Dirección %d: %p\n", i, address[i]);

        if (fork() == 0) {
            execlp("bash", "bash", "-c", addr2line_cmd, NULL);
            exit(0);
        } else {
            wait(NULL);
        }
    }
}

void get_trace() {
    void *callstack[20];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(callstack, sizeof(callstack) / sizeof(callstack[0]));
    strings = backtrace_symbols(callstack, size);

    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < size; i++) {
        fprintf(stderr, "%s\n", strings[i]);
    }

    get_function_names(callstack, size);
    free(strings);
}