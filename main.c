#include <stdio.h>       // Basic IO
#include <stdlib.h>      // malloc(), free()

#include "interface.h"   // Interface text
#include "rcodes.h"      // Return codes
#include "sha1.h"        // SHA-1 implementation


char to_hex(unsigned char value) {
	
	if(value >= 10) {
		
		return value - 10 + 'a';
		
	} else {
		
		return value + '0';
	}
}

int main(int argc, char **argv) {
	
	unsigned char R_CODE = 0; // Return code
	
	if(argc < 2) {
		
		/* If no arguments */
		
		printf(TITLE);
		printf(USAGE, argv[0]);
		
		R_CODE = RCODE_DESCRIPTION;
		
	} else if(argc > 2) {
		
		/* If too many arguments */
		
		printf(ERR_TOO_MANY_ARGS);
		printf(USAGE, argv[0]);
		
		R_CODE = RCODE_ERR_TOO_MANY_ARGS;
		
	} else {
		
		/* Trying to open file */
		
		FILE *input = fopen(argv[1], "rb");
		
		if(input == NULL) {
			
			/* Cannot open file */
			
			fprintf(stderr, ERR_OPEN_FILE, argv[1]);
			
			R_CODE = RCODE_ERR_OPEN_FILE;
			
		} else {
			
			/* Allocating memory for hash */
			
			unsigned char *hash = malloc(20 * sizeof(*hash));
			
			if(hash == NULL) {
				
				fprintf(stderr, ERR_MEMORY_ALLOC, 20 * sizeof(*hash));
				
				R_CODE = RCODE_ERR_MEMORY_ALLOC;
				
			} else {
				
				/* Handling file */
				
				sha1(hash, input);
				
				/* Printing hash */
				
				for(unsigned char i = 0; i < 20; ++i) {
					
					putchar(to_hex(hash[i] >> 4));
					putchar(to_hex(hash[i] & 0x0F));
				}
				
				putchar('\n');
				
				free(hash);
			}
			
			fclose(input);
		}
	}
	
	return R_CODE;
}
