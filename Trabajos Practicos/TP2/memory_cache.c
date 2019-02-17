#include "memory_cache.h"
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0

#define HIT    0
#define MISS  -1

#define NULL_C 1

void block_create(block_t *block, int tag) {
	block->tag = tag;
	block->metadata.valid = 1;
	block->metadata.dirty = 0;
}

static void memory_cache_take_tag_set_word(
	int n_bits_of_tag,
	int n_bits_of_set,
	int n_bits_of_word,
	int *tag,
	int *set,
	int *word,
	unsigned short int address
) {
	char *tag_str = malloc((n_bits_of_tag + NULL_C) * sizeof(char));
	char *set_str = malloc((n_bits_of_set + NULL_C) * sizeof(char));
	char *word_str = malloc((n_bits_of_word + NULL_C) * sizeof(char));

	tag_str[0] = '\0';
	set_str[0] = '\0';
	word_str[0] = '\0';

	for (int i = 0 ; i < n_bits_of_tag ; ++i) {
		if((address >> (15 - i)) & 1) tag_str[i] = '1';
		else 						  tag_str[i] = '0';
	}
	tag_str[n_bits_of_tag] = '\0';

	for (int i = 0 ; i < n_bits_of_set ; ++i) {
		if((address >> (15 - (n_bits_of_tag + i)) & 1)) set_str[i] = '1';
		else 						  			        set_str[i] = '0';
	}
	set_str[n_bits_of_set] = '\0';

	for (int i = 0 ; i < n_bits_of_word ; ++i) {
		if((address >> (14 - (n_bits_of_tag + n_bits_of_word + i)) & 1)) word_str[i] = '1';
		else 						  			                         word_str[i] = '0';
	}
	word_str[n_bits_of_word] = '\0';

	*tag = strtoul(tag_str, (char **) NULL, 2);
	*set = strtoul(set_str, (char **) NULL, 2);
	*word = strtoul(word_str, (char **) NULL, 2);

	free(tag_str);
	free(set_str);
	free(word_str);
}

void memory_cache_init(memory_cache_t *self) {
	memory_principal_init(&memory_principal);

	for (int i = 0 ; i < N_SETS ; ++i) {
		for (int j = 0 ; j < N_BLOCKS ; ++j) {
			self->sets[i].blocks[j].metadata.valid = 0;
			self->sets[i].blocks[j].metadata.dirty = 0;
		}
	}

	self->n_references = 0;
	self->n_misses = 0;
}

static void load_block_memory_principal(memory_principal_t *memory_principal, int address, block_t *block) {
	char *bytes = malloc(sizeof(char) * BLOCK_SIZE);
	memory_principal_get_block(memory_principal, address, bytes, BLOCK_SIZE);
	for (int i = 0 ; i < BLOCK_SIZE ; ++i) {
		block->words[i] = bytes[i];
	} 
	free(bytes);
}

static void store_block_memory_principal(memory_principal_t *memory_principal, int address, block_t *block) {
	memory_principal_set_block(memory_principal, address, block->words, BLOCK_SIZE);
}

int memory_cache_build_address(int tag, int set, int word) {
	char word_str[N_BITS_WORD + NULL_C];
	char set_str[N_BITS_SET + NULL_C];
	char tag_str[N_BITS_TAG + NULL_C];

	for(int i = 0 ; i < N_BITS_WORD ; ++i) {
		if((word >> i) & 1) {
			word_str[N_BITS_WORD-NULL_C-i] = '1';
		} else {
			word_str[N_BITS_WORD-NULL_C-i] = '0';
		} 		   	    
	}
	word_str[N_BITS_WORD] = '\0';
	
	for(int i = 0 ; i <= N_BITS_SET ; ++i) {
		if((set >> i) & 1) {
			set_str[N_BITS_SET-NULL_C-i] = '1';
		} else {
			set_str[N_BITS_SET-NULL_C-i] = '0';
		} 		   	    
	}
	set_str[N_BITS_SET] = '\0';
	
	for(int i = 0 ; i <= N_BITS_TAG ; ++i) {
		if((tag >> i) & 1) {
			tag_str[N_BITS_TAG-NULL_C-i] = '1';
		} else {
			tag_str[N_BITS_TAG-NULL_C-i] = '0';
		} 		   	    
	}
	tag_str[N_BITS_TAG] = '\0';

	char address_str[N_BITS_MEMORY_ADDRESS + NULL_C];
	for(int i = 0 ; i < N_BITS_TAG  ; ++i) address_str[i]                           = tag_str[i]; 
	for(int i = 0 ; i < N_BITS_SET  ; ++i) address_str[i + N_BITS_TAG]              = set_str[i]; 
	for(int i = 0 ; i < N_BITS_WORD ; ++i) address_str[i + N_BITS_TAG + N_BITS_SET] = word_str[i]; 
	address_str[N_BITS_MEMORY_ADDRESS] = '\0';

	int address = strtoul(address_str, (char **) NULL, 2);

	return address;
}

int memory_cache_read_byte(memory_cache_t *self, unsigned short int address) {
	int match = MISS; 

	self->n_references++;

	int tag;
	int set;
	int word;

	memory_cache_take_tag_set_word(
		N_BITS_TAG,
		N_BITS_SET,
		N_BITS_WORD,
		&tag,
		&set,
		&word,
		address
	);

	printf("(set: %i, tag: %i, word: %i) ", set, tag, word);

	if (
		(self->sets[set].blocks[0].metadata.valid == 0) &&
		(self->sets[set].blocks[1].metadata.valid == 0)
	) {
		self->n_misses++;
		block_t block;
		block_create(&block, tag);
		load_block_memory_principal(&memory_principal, address, &block);
		self->sets[set].blocks[0] = block;
		self->sets[set].LRU_index = 1;
	} else if (
		(self->sets[set].blocks[0].metadata.valid == 1) &&
		(self->sets[set].blocks[1].metadata.valid == 0)
	) {
		if (self->sets[set].blocks[0].tag == tag) {
			match = (int) self->sets[set].blocks[0].words[word];
		} else {
			self->n_misses++;
			block_t block;
			block_create(&block, tag);
			load_block_memory_principal(&memory_principal, address, &block);
			self->sets[set].blocks[1] = block;
			self->sets[set].LRU_index = 0;
		}
	} else if (
		(self->sets[set].blocks[0].metadata.valid == 0) &&
		(self->sets[set].blocks[1].metadata.valid == 1)
	) {
		if (self->sets[set].blocks[1].tag == tag) {
			match = (int) self->sets[set].blocks[1].words[word];
		} else {
			self->n_misses++;
			block_t block;
			block_create(&block, tag);
			load_block_memory_principal(&memory_principal, address, &block);
			self->sets[set].blocks[0] = block;
			self->sets[set].LRU_index = 1;
		}
	} else if (
		(self->sets[set].blocks[0].metadata.valid == 1) &&
		(self->sets[set].blocks[1].metadata.valid == 1)
	) {
		if (self->sets[set].blocks[0].tag == tag) {
			match = (int) self->sets[set].blocks[0].words[word];
			self->sets[set].LRU_index = 1;
		} else if (self->sets[set].blocks[1].tag == tag) {
			match = (int) self->sets[set].blocks[1].words[word];
			self->sets[set].LRU_index = 0;
		} else {
			self->n_misses++;
			int address_to_store = memory_cache_build_address(self->sets[set].blocks[self->sets[set].LRU_index].tag, set, word);
			store_block_memory_principal(&memory_principal, address_to_store, &self->sets[set].blocks[self->sets[set].LRU_index]);
			block_t block;		
			block_create(&block, tag);
			load_block_memory_principal(&memory_principal, address, &block);
			self->sets[set].blocks[self->sets[set].LRU_index] = block;
			self->sets[set].LRU_index = self->sets[set].LRU_index == 1 ? 0 : 1;
		}
	}

	return match;
}

int memory_cache_write_byte(memory_cache_t *self, unsigned short int address, unsigned char value) {
	int tag;
	int set;
	int word;

	self->n_references++;

	memory_cache_take_tag_set_word(
		N_BITS_TAG,
		N_BITS_SET,
		N_BITS_WORD,
		&tag,
		&set,
		&word,
		address
	);

	printf("(set: %i, tag: %i, word: %i) ", set, tag, word);


	if (
		(self->sets[set].blocks[0].metadata.valid == 0) &&
		(self->sets[set].blocks[1].metadata.valid == 0)
	) {
		self->n_misses++;
		block_t block;
		block_create(&block, tag);
		load_block_memory_principal(&memory_principal, address, &block);
		self->sets[set].blocks[0] = block;
		self->sets[set].blocks[0].words[word] = value;
		self->sets[set].blocks[0].metadata.dirty = 1;
	} else if (
		(self->sets[set].blocks[0].metadata.valid == 1) &&
		(self->sets[set].blocks[1].metadata.valid == 0)
	) {
		if (self->sets[set].blocks[0].tag == tag) {
			self->sets[set].blocks[0].words[word] = value;
			self->sets[set].blocks[0].metadata.dirty = 1;

			return HIT;
		} else {
			self->n_misses++;
			store_block_memory_principal(&memory_principal, address, &self->sets[set].blocks[0]);
			block_t block;
			block_create(&block, tag);
			load_block_memory_principal(&memory_principal, address, &block);
			block.words[word] = value;
			block.metadata.dirty = 1;
			self->sets[set].blocks[0] = block;

			return MISS;
		}
	} else if (
		(self->sets[set].blocks[0].metadata.valid == 0) &&
		(self->sets[set].blocks[1].metadata.valid == 1)
	) {
		if (self->sets[set].blocks[1].tag == tag) {
			self->sets[set].blocks[1].words[word] = value;
			self->sets[set].blocks[1].metadata.dirty = 1;

			return HIT;
		} else {
			self->n_misses++;
			store_block_memory_principal(&memory_principal, address, &self->sets[set].blocks[1]);
			block_t block;
			block_create(&block, tag);
			load_block_memory_principal(&memory_principal, address, &block);
			block.words[word] = value;
			block.metadata.dirty = 1;
			self->sets[set].blocks[1] = block;

			return MISS;
		}
	} else if (
		(self->sets[set].blocks[0].metadata.valid == 1) &&
		(self->sets[set].blocks[1].metadata.valid == 1)
	) {
		if (self->sets[set].blocks[0].tag == tag) {
			self->sets[set].blocks[0].words[word] = value;
			self->sets[set].blocks[0].metadata.dirty = 1;

			return HIT;
		} else if (self->sets[set].blocks[1].tag == tag) {
			self->sets[set].blocks[1].words[word] = value;
			self->sets[set].blocks[1].metadata.dirty = 1;

			return HIT;
		} else {
			self->n_misses++;
			int LRU_index = self->sets[set].LRU_index;
			store_block_memory_principal(&memory_principal, address, &self->sets[set].blocks[LRU_index]);
			block_t block;
			block_create(&block, tag);
			load_block_memory_principal(&memory_principal, address, &block);
			block.words[word] = value;
			block.metadata.dirty = 1;
			self->sets[set].blocks[LRU_index] = block;
			self->sets[set].LRU_index = self->sets[set].LRU_index == 1 ? 0 : 1;

			return MISS;
		}
	}	

	return MISS;									
}

float memory_cache_get_miss_rate(memory_cache_t *self) {
	printf("Number of misses: %d\n", self->n_misses);
	printf("Number of references: %d\n", self->n_references);

	return ( ( (float) self->n_misses ) / ( (float) (self->n_references ) ) );
}
