/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_tester.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imurugar <imurugar@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 10:12:50 by imurugar          #+#    #+#             */
/*   Updated: 2023/09/08 10:26:17 by imurugar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_TESTER_H
#define MALLOC_TESTER_H


#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

int save_call_nbr(const char *filename, int nbr);
int get_call_nbr(const char *filename);
void get_function_names(void *direcciones[], int numDirecciones);
void get_trace();

#endif