#pragma once

// Debugging
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>

#define DEBUG 0
#if DEBUG
#define DBG printf
#else
#define DBG(...) \
    while (0) {  \
    }
#endif

#define LOG printf

#define LOG_ERR(...) fprintf(stderr, __VA_ARGS__)

#define CONTACT_ADMIN " Contact an admin if unexpected.\n"

void flushcin();

bool checkInput(bool exit_program=false);

void* safe_malloc(size_t length);

void* safe_calloc(size_t nmemb, size_t length);