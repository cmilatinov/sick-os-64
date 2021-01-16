#include "modules/lib.h"

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25

static uint16_t * const VIDEO_MEMORY = (uint16_t *) 0xb8000;
static const char * const symbols = "0123456789ABCDEF";
static uint32_t x = 0, y = 0;

void clear(){

	// Clear the screen.
	for(int i = 0; i < 2000; i++)
		VIDEO_MEMORY[i] = 0x0720;
	
	// Reset the cursor.
	x = y = 0;
}

void erase(uint32_t numSpaces) {
	for (uint32_t i = 0; i < numSpaces; i++) {
		if (x == 0 && y == 0)
			return;
		else if (x == 0) {
			x = SCREEN_WIDTH - 1;
			y--;
		} else 
			x--;
		VIDEO_MEMORY[(SCREEN_WIDTH * y) + x] = (VIDEO_MEMORY[(SCREEN_WIDTH * y) + x] & 0xFF00) | ' ';
	}
}

void printf(){
	return printf("\n");
}

void printf(const char * str){

	// Copy the string into video memory.
	for(int i = 0; str[i] != '\0'; i++){

		// If the character is a new line, simply increment y and set x to 0.
        if(str[i] == '\n'){
			x = 0;
			y++;
		// Otherwise, print the character and increment x.
		}else{
			VIDEO_MEMORY[(SCREEN_WIDTH * y) + x] = (VIDEO_MEMORY[(SCREEN_WIDTH * y) + x] & 0xFF00) | str[i];
			x++;
        }

		// If we've reached the end of the line, move to the next one.
        if(x >= SCREEN_WIDTH){
            x = 0;
            y++;
        }
		
		// If we've reached the end of the screen, clear it.
        if(y >= SCREEN_HEIGHT)
			clear();
    }

}

void printc(const char c) {
	char str[2] = { c, '\0' };
	printf(str);
}

void printb(uint64_t integer, uint8_t base){

	// Print zero if the integer is a zero.
	if(integer == 0) return printf("0");

	// 50 characters should be more than enough.
	char str[50];
	int i = 0;

	// Create char sequence.
	while(integer != 0){
		int digit = integer % base;
		integer /= base;
		str[i] = symbols[digit];
		i++;
	}
	
	// Reverse the sequence as the first character represents the zeroth power.
	for(int j = 0; j < i / 2; j++){
        int opposite = i - j - 1;
		str[j] ^= str[opposite];
		str[opposite] ^= str[j];
		str[j] ^= str[opposite];
	}

	// Terminating character.
	str[i] = '\0';

	// Print the string.
	printf(str);
}

void printb(uint64_t binary){

	// Prepend '0b' and print the integer in binary.
	printf("0b");
	printb(binary, 2);
}

void printd(uint64_t integer){

	// Print the integer in decimal.
	printb(integer, 10);
}

void printh(uint64_t hex){

	// Prepend '0x' and print the integer in hexadecimal.
	printf("0x");
	printb(hex, 16);
}

void printp(void * ptr){

	// Print the pointer in hex.
	printh(reinterpret_cast<uint64_t>(ptr));
}

void hexdump(void * loc, size_t size) {
	clear();

	uint8_t * ptr = reinterpret_cast<uint8_t*>(loc);

	for (size_t i = 0; i < size; i++) {
		if (ptr[i] < 0x10) {
			printf("0");
			printb(ptr[i], 16);
		} else
			printb(ptr[i], 16);
		
		if (i % 27 != 26)
			printf(" ");
	}
	
}