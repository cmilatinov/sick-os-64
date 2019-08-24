#include "lib.h"

static uint16_t * const VIDEO_MEMORY = (uint16_t *) 0xb8000;
static const char * symbols = "0123456789abcdef";
static uint32_t x = 0, y = 0;

void clear(){

	// Clear the screen.
	for(int i = 0; i < 2000; i++)
		VIDEO_MEMORY[i] = 0x0720;
	
	// Reset the cursor.
	x = y = 0;
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
			VIDEO_MEMORY[(80 * y) + x] = (VIDEO_MEMORY[(80 * y) + x] & 0xFF00) | str[i];
			x++;
        }

		// If we've reached the end of the line, move to the next one.
        if(x >= 80){
            x = 0;
            y++;
        }
		
		// If we've reached the end of the screen, clear it.
        if(y >= 25)
			clear();
    }

}

void printb(uint64_t integer, uint8_t base){

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

void printi(uint64_t integer){

	// Print the integer in base 10.
	printb(integer, 10);
}

void printh(uint64_t hex){

	// Prepend '0x' and print the integer in base 16.
	printf("0x");
	printb(hex, 16);
}

void printp(void * ptr){

	// Print the pointer in hex.
	printh(reinterpret_cast<uint64_t>(ptr));
}