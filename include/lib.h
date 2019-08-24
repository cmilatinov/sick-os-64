#include "types.h"

#pragma once

void clear();

void printf(const char * str);
void printb(uint64_t integer, uint8_t base);
void printi(uint64_t integer);
void printh(uint64_t hex);
void printp(void * ptr);