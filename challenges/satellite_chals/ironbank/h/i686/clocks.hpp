#pragma once 
#include <x86intrin.h>
#include <stdint.h>
uint64_t ticks( )
{
    return  __rdtsc();
}