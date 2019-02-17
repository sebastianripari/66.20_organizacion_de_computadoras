#include "memory_principal.h"

void memory_principal_init(memory_principal_t *self) {
	for (int i = 0 ; i < MEMORY_PRINCIPAL_SIZE ; ++i) {
		self->bytes[i] = 0;
	}
}

void memory_principal_get_block(memory_principal_t *self, unsigned int address,	char *bytes, int n_bytes) {
	int j = 0;
	for (int i = address ; i < (address + n_bytes) ; ++i) {
		bytes[j] = self->bytes[i];
		j++;
	}
}

void memory_principal_set_block(memory_principal_t *self, unsigned int address, unsigned char *bytes, int n_bytes) {
	int j = 0;
	for (int i = address ; i < (address + n_bytes) ; ++i) {
		self->bytes[i] = bytes[j];
		j++;
	}
}
