#ifndef __MEMORY_PRINCIPAL_H__
#define __MEMORY_PRINCIPAL_H__

#define MEMORY_PRINCIPAL_SIZE 64000 // bytes

typedef struct memory_principal_t {
	char bytes[MEMORY_PRINCIPAL_SIZE];
} memory_principal_t;

void memory_principal_init(memory_principal_t *self);
void memory_principal_get_block(memory_principal_t *self, unsigned int address,char *bytes, int n_bytes);
void memory_principal_set_block(memory_principal_t *self, unsigned int address, unsigned char *bytes, int n_bytes);

#endif