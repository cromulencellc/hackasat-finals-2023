#pragma once 
#include <memory>
uint64_t ticks( )
{
    uint32_t time_start = 0;
    __asm__ volatile ("rdtime %0" : "=r"(time_start));
    return (uint64_t) time_start;
}