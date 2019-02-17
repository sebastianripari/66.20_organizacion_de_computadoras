#ifndef __MEMORY_CACHE_H__
#define __MEMORY_CACHE_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "memory_principal.h"

#define SUCCESS 0
#define ERROR  -1

#define NULL_C 1

/* Cache size */
#define CACHE_SIZE 4096 // bytes 

/* Block size */
#define BLOCK_SIZE 32   // bytes

/* Number of Sets */
#define N_SETS CACHE_SIZE/(N_BLOCKS * BLOCK_SIZE)

/* Number of blocks */
#define N_BLOCKS 2 // 2-way

/* Words by Block */
#define N_WORDS 32 // 32 bytes / 1 bytes = 32 words

#define N_BITS_MEMORY_ADDRESS 16
#define N_BITS_TAG  5  // N_BITS_MEMORY_ADDRESS - N_BITS_TAG - N_BITS_SET 
#define N_BITS_SET  6  // log_2(N_SETS)
#define N_BITS_WORD 5  // log_2(N_WORDS)

/* Define a global memory principal */
memory_principal_t memory_principal;

typedef struct metadata_t {
	/* valid flag:
	 * valid = 1 -> Valid
	 * valid = 0 -> Invalid   
	 */ 
	int valid; 
	/* dirty flag
	 * dirty = 1 -> 
	 * dirty = 0 ->
	 */
	int dirty; 
} metadata_t;	

typedef struct block_t {
	/* Metadata */
	metadata_t metadata;
	/* Tag */
	int tag;
	/* Words */
	unsigned char words[N_WORDS];
} block_t;

typedef struct set_t {
	/* LRU index */
	int LRU_index;
	/* Blocks */
	block_t blocks[N_BLOCKS];
} set_t;

typedef struct memory_cache_t {
	/* Number of references */ 
	int n_references;
	/* Number of miss */
	int n_misses;
	/* Sets */
	set_t sets[N_SETS];
} memory_cache_t;

void memory_cache_init(memory_cache_t *self);

int memory_cache_read_byte(memory_cache_t *self, unsigned short int address);

int memory_cache_write_byte(memory_cache_t *self, unsigned short int address, unsigned char value);

float memory_cache_get_miss_rate(memory_cache_t *self);

#endif
