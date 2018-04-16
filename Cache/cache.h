#pragma once

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
	bool Valid;
	unsigned long TAG;
	unsigned int number;
}CacheBlock;

typedef struct
{
	CacheBlock *CacheBlock;
	unsigned int max;
}Set;

typedef struct
{
	unsigned long BlockSize;
	unsigned long set_degree;
	unsigned long set_amount;
	Set *Set;
	CacheBlock *CacheBlock_temp;
}Cache;

typedef enum
{
	Executable,
	TRACE_FILE_NAME,
	CACHE_SIZE,
	BLOCK_SIZE,
	SET_DEGREE,
	PARAMETER_COUNT
}Parameter;

#ifdef __cplusplus
extern "C" {
#endif 
	bool Init(int argc, char *argv[], Cache *cache, FILE **fptr);
	bool compute_miss_rate(FILE *fptr, Cache *ptr_cache, double *miss_rate);
	bool free_cache_file(FILE **fptr, Cache *ptr_cache);
	bool print_csv(char *file_name, char *argv[], double miss_rate);
#ifdef __cplusplus
}
#endif 
