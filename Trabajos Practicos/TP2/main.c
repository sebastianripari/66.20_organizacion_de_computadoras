#include <stdio.h>

#include "memory_cache.h"

#define ERROR -1
#define SUCCESS 0

#define HIT   0
#define MISS -1

int main(int argc, char* argv[]) {
	memory_cache_t memory_cache;

	memory_cache_init(&memory_cache);

	if(argc == 1) {
		printf("Error, no espefico ninguna ruta hacia un archivo.\n");
		return ERROR;
	} 

	FILE* input = fopen(argv[1], "r");

	if (input == NULL) {
		printf("Error, por algun motivo no se pudo abrir el archivo.\n");
		return ERROR;
	}

	unsigned short int address;
	
	char end_of_line;
	char blank_space;
	char coma;
	
	char opcode;
	while (fscanf(input, "%c", &opcode) == 1) {
		if (opcode == 'R') {
			fscanf(input, "%hu", &address);
			printf("R %i -> ", address);
			int value;
			if ((value = memory_cache_read_byte(&memory_cache, address)) == ERROR) {
				printf("MISS\n");
			} else {
				printf("HIT: el dato es %d\n", value);	
			};
		} else if(opcode == 'W') {
			unsigned short int value;
			fscanf(input, "%hu", &address);
			fscanf(input, "%c", &coma);
			fscanf(input, "%c", &blank_space);
			fscanf(input, "%hu", &value);
			printf("W %i, %i -> ", address, value);

			if (memory_cache_write_byte(&memory_cache, address, value) == HIT) printf("HIT\n");
			else 															   printf("MISS\n");			
			
		} else {
			printf("Miss rate: %f\n", memory_cache_get_miss_rate(&memory_cache));
			fscanf(input, "%c", &end_of_line);
		}
		fscanf(input, "%c", &end_of_line);
	}

	if (input != NULL) {
		fclose(input);
	}
	return SUCCESS;
}