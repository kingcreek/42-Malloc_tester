#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <stdbool.h>

static void* (*real_malloc)(size_t size);
static void  (*real_free)(void *ptr);

static bool simulate_malloc_failure = true;

__attribute__((constructor))
static void init()
{
    real_malloc = dlsym(RTLD_NEXT, "malloc");
    real_free   = dlsym(RTLD_NEXT, "free");
    fprintf(stderr, "init\n");
}

void *malloc(size_t size)
{
	return NULL;
	/*
    if (simulate_malloc_failure) {
        fprintf(stderr, "Simulate malloc failure? (y/n): ");
        char response;
        scanf(" %c", &response);
        getchar();  // Consume el carácter de salto de línea adicional

        if (response == 'y' || response == 'Y') {
            fprintf(stderr, "Simulated malloc failure\n");
            return NULL;
        }
    }
	*/

    void *ptr = real_malloc(size);
    //fprintf(stderr, "malloc(%zd) = %p\n", size, ptr);
    return ptr;
}

void free(void *ptr)
{
    real_free(ptr);
    //fprintf(stderr, "free(%p)\n", ptr);
}