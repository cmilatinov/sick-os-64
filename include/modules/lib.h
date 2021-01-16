#pragma once

#include "common/types.h"

void clear();

void erase(uint32_t numSpaces);

void printf();
void printf(const char * str);
void printc(const char c);

void printb(uint64_t integer, uint8_t base);

void printb(uint64_t binary);
void printd(uint64_t decimal);
void printh(uint64_t hex);

void printp(void * ptr);

void hexdump(void * loc, size_t size);