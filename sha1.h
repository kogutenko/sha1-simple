#include <inttypes.h>    // uint32_t, uint8_t
#include <limits.h>      // UCHAR_MAX


/* Constants */

const uint32_t K0 = 0x5A827999;    // (sqrt( 2) / 4) * 2^32
const uint32_t K1 = 0x6ED9EBA1;    // (sqrt( 3) / 4) * 2^32
const uint32_t K2 = 0x8F1BBCDC;    // (sqrt( 5) / 4) * 2^32
const uint32_t K3 = 0xCA62C1D6;    // (sqrt(10) / 4) * 2^32


/* Global variables */

uint32_t A = 0x67452301;
uint32_t B = 0xEFCDAB89;
uint32_t C = 0x98BADCFE;
uint32_t D = 0x10325476;
uint32_t E = 0xC3D2E1F0;


/* Arrays */

uint32_t W[80 * sizeof(uint32_t)]; // Extended hashing array


/* Functions descriptions */

void write_size(unsigned long long size);
void sha1(unsigned char *hash, FILE *input);
void sha1_cycle();


/* Circular shift */

#define ROTL(x, n) (((x) << (n)) | ((x) >> (32 - n)))


/* Non-linear functions */

#define F0(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define F1(x, y, z) ((x) ^ (y) ^ (z))
#define F2(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define F3(x, y, z) ((x) ^ (y) ^ (z))


void write_size(unsigned long long size) {
	
	size *= 8;
	
	W[14] = size >> 32;
	W[15] = size & 0xFFFFFFFF;
}


void fix_order() {
	
	/* Checking byte order */
	
	uint16_t byte = 1;
	
	if(*((uint8_t *) &byte) == 1) {
	
		/* Little-endian */
		
		uint8_t *W_bytes = (uint8_t *)W;
		
		unsigned char i = 0;
		unsigned char j = 0;
		
		while(i < 64) {
			
			uint32_t value;
			value  = W_bytes[i++]; value <<= 8;
			value |= W_bytes[i++]; value <<= 8;
			value |= W_bytes[i++]; value <<= 8;
			value |= W_bytes[i++];
			
			W[j++] = value;
		}
	}
}


void sha1(unsigned char *hash, FILE *input) {
	
	/* Initializing hash */
	
	A = 0x67452301;
	B = 0xEFCDAB89;
	C = 0x98BADCFE;
	D = 0x10325476;
	E = 0xC3D2E1F0;
	
	
	/* Reading from stream */
	
	unsigned long long size = 0; // Size in bytes
	
	while(!feof(input)) {
			
		uint8_t *W_bytes = (uint8_t *)W;
		
		/* Reading a 512-bit block */
		
		unsigned short length = fread(W_bytes, sizeof(uint8_t), 64, input);
		
		size += length;         // Increasing size
		
		if(length == 64) {
			
			/* Full 512-bit block */
			
			fix_order();        // Fix byte order
			
		} else if(length < 56) {
			
			/* Less than 448 bits */
			
			W_bytes[length++] = 0x80;                 // Putting unit
			
			while(length < 56) W_bytes[length++] = 0; // Putting zeros
			
			fix_order();        // Fix byte order
			
			write_size(size);   // Writing size
			
		} else {
			
			/* More or equal 448 bits */
			
			W_bytes[length++] = 0x80;
			
			while(length < 64) W_bytes[length++] = 0;
			
			fix_order();        // Fix byte order
			
			sha1_cycle();
			
			length = 0;
			
			while(length < 14) W[length++] = 0; // Putting zeros
			
			write_size(size);   // Writing size
		}
		
		sha1_cycle();
	}
	
	
	/* Writing hash */
	
	for(unsigned char i =  3; i != UCHAR_MAX; --i) {hash[i] = A & 0xFF; A >>= 8;}
	for(unsigned char i =  7; i >=  4; --i) {hash[i] = B & 0xFF; B >>= 8;}
	for(unsigned char i = 11; i >=  8; --i) {hash[i] = C & 0xFF; C >>= 8;}
	for(unsigned char i = 15; i >= 12; --i) {hash[i] = D & 0xFF; D >>= 8;}
	for(unsigned char i = 19; i >= 16; --i) {hash[i] = E & 0xFF; E >>= 8;}
}


void sha1_cycle() {
	
	/* Stage 1. Block conversion */
	
	for(unsigned char t = 16; t < 80; ++t)
		W[t] = ROTL(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16], 1);
	
	
	/* Stage 2. Initializing a, b, c, d, e */
	
	uint32_t a = A;
	uint32_t b = B;
	uint32_t c = C;
	uint32_t d = D;
	uint32_t e = E;
	
	
	/* Stage 3. Cycle of compression function */
	
	for(unsigned char t = 0; t < 80; ++t) {
		
		uint32_t TEMP = ROTL(a, 5) + e + W[t];
		
		switch(t / 20) {
			case 0: TEMP += F0(b, c, d) + K0; break;
			case 1: TEMP += F1(b, c, d) + K1; break;
			case 2: TEMP += F2(b, c, d) + K2; break;
			case 3: TEMP += F3(b, c, d) + K3; break;
		}
		
		e = d;
		d = c;
		c = ROTL(b, 30);
		b = a;
		a = TEMP;
	}
	
	
	/* Stage 4. Refreshing hash */
	
	A += a;
	B += b;
	C += c;
	D += d;
	E += e;
}
