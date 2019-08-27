#include "types.h"

#pragma once

void clear();

void printf();
void printf(const char * str);

void printb(uint64_t integer, uint8_t base);

void printb(uint64_t binary);
void printd(uint64_t decimal);
void printh(uint64_t hex);

void printp(void * ptr);