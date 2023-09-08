/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:50 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/08 13:35:12 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if !defined(MALLOC_TESTER_H)
#define MALLOC_TESTER_H

#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/stat.h>
#if defined(__APPLE__)
#include <malloc/malloc.h>
#define malloc_usable_size malloc_size
#elif defined(__ELF__)
#include <malloc.h>
#endif

int save_call_nbr(const char *filename, int nbr);
int get_call_nbr(const char *filename);
void get_function_names(void *direcciones[], int numDirecciones);
void get_trace();

#endif