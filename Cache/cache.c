#include "cache.h"

bool Init_Cache(char *argv[], Cache *cache);
bool get_Address(FILE *fptr, unsigned long *ptr_address);
bool is_hit(unsigned long address, Cache *ptr_cache);
void get_address_subdivision(unsigned long address, Cache *ptr_cache, unsigned long *ptr_set_number, unsigned long *ptr_TAG);
bool is_set_TAG_equal(Cache *ptr_cache, unsigned long set_number, unsigned long TAG);
void set_TAG_not_equal(Cache *ptr_cache, unsigned long set_number, unsigned long TAG);

bool Init(int argc, char *argv[], Cache *cache, FILE **fptr)
{
	if (PARAMETER_COUNT != argc)
	{
		assert(PARAMETER_COUNT == argc);
		return false;
	}

	*fptr = fopen(argv[TRACE_FILE_NAME], "r");
	if (NULL == *fptr)
	{
		assert(NULL != *fptr);
		return false;
	}

	if (NULL == cache)
	{
		assert(NULL != cache);
		return false;
	}

	return Init_Cache(argv, cache);
}

bool Init_Cache(char *argv[], Cache *cache)
{
	assert(NULL != argv);
	assert(NULL != cache);

	cache->BlockSize = strtoul(argv[BLOCK_SIZE], NULL, 0);
	if (0 == cache->BlockSize)
	{
		assert(0 != cache->BlockSize);
		return false;
	}

	cache->set_degree = strtoul(argv[SET_DEGREE], NULL, 0);
	if (0 == cache->set_degree)
	{
		assert(0 != cache->set_degree);
		return false;
	}

	cache->set_amount = strtoul(argv[CACHE_SIZE], NULL, 0) / cache->BlockSize / cache->set_degree;
	if (0 == cache->set_amount)
	{
		assert(0 != cache->set_amount);
		return false;
	}

	cache->Set = (Set*)calloc(cache->set_amount, sizeof(Set));
	if (NULL == cache->Set)
	{
		assert(NULL != cache->Set);
		return false;
	}

	cache->CacheBlock_temp = (CacheBlock*)calloc(cache->set_amount*cache->set_degree, sizeof(CacheBlock));
	if (NULL == cache->CacheBlock_temp)
	{
		assert(NULL != cache->CacheBlock_temp);
		return false;
	}

	for (unsigned long i = 0; i < cache->set_amount; i++)
	{
		cache->Set[i].CacheBlock = &cache->CacheBlock_temp[i * cache->set_degree];
	}
	return true;
}

bool compute_miss_rate(FILE *fptr, Cache *ptr_cache, double *miss_rate)
{
	if (NULL == fptr)
	{
		assert(NULL != fptr);
		return false;
	}

	if (NULL == ptr_cache)
	{
		assert(NULL != ptr_cache);
		return false;
	}

	if (NULL == miss_rate)
	{
		assert(NULL != miss_rate);
		return false;
	}

	unsigned long address;
	unsigned int hit = 0, miss = 0;

	while (get_Address(fptr, &address))
	{
		if (is_hit(address, ptr_cache))
		{
			hit++;
		}
		else
		{
			miss++;
		}
	}
	*miss_rate = (double)miss / (miss + hit);
	return true;
}

bool get_Address(FILE *fptr, unsigned long *ptr_address)
{
	assert(NULL != fptr);
	assert(NULL != ptr_address);

	if (EOF == fscanf_s(fptr, "%li", ptr_address))
	{
		return false;
	}
	return true;
}

bool is_hit(unsigned long address, Cache *ptr_cache)
{
	assert(NULL != ptr_cache);

	unsigned long set_number, TAG;
	get_address_subdivision(address, ptr_cache, &set_number, &TAG);

	if (is_set_TAG_equal(ptr_cache, set_number, TAG))
	{
		return true;
	}
	else
	{
		set_TAG_not_equal(ptr_cache, set_number, TAG);
		return false;
	}
}

void get_address_subdivision(unsigned long address, Cache *ptr_cache, unsigned long *ptr_set_number, unsigned long *ptr_TAG)
{
	assert(NULL != ptr_cache);
	assert(NULL != ptr_set_number);
	assert(NULL != ptr_TAG);

	assert(0 != ptr_cache->BlockSize);
	assert(0 != ptr_cache->set_amount);

	unsigned long memory_block = address / ptr_cache->BlockSize;
	*ptr_set_number = memory_block % ptr_cache->set_amount;
	*ptr_TAG = memory_block / ptr_cache->set_amount;
}

bool is_set_TAG_equal(Cache *ptr_cache, unsigned long set_number, unsigned long TAG)
{
	assert(NULL != ptr_cache);

	for (size_t i = 0; i < ptr_cache->set_degree; i++)
	{
		if (ptr_cache->Set[set_number].CacheBlock[i].Valid&&ptr_cache->Set[set_number].CacheBlock[i].TAG == TAG)
		{
			ptr_cache->Set[set_number].CacheBlock[i].number = ++ptr_cache->Set->max;
			return true;
		}
	}
	return false;
}

void set_TAG_not_equal(Cache *ptr_cache, unsigned long set_number, unsigned long TAG)
{
	assert(NULL != ptr_cache);

	size_t j = 0;
	unsigned int number = UINT_MAX;
	for (size_t i = 0; i < ptr_cache->set_degree; i++)
	{
		if (ptr_cache->Set[set_number].CacheBlock[i].number < number)
		{
			number = ptr_cache->Set[set_number].CacheBlock[i].number;
			j = i;
		}
	}

	ptr_cache->Set[set_number].CacheBlock[j].Valid = true;
	ptr_cache->Set[set_number].CacheBlock[j].TAG = TAG;
	ptr_cache->Set[set_number].CacheBlock[j].number = ++ptr_cache->Set->max;
}

bool free_cache_file(FILE **fptr, Cache *ptr_cache)
{
	if (NULL == *fptr)
	{
		assert(NULL != *fptr);
		return false;
	}

	if (NULL == ptr_cache)
	{
		assert(NULL != ptr_cache);
		return false;
	}

	int value = fclose(*fptr);
	if (EOF == value)
	{
		assert(EOF != value);
		return false;
	}
	else
	{
		*fptr = NULL;
	}

	free((void*)ptr_cache->Set);
	free((void*)ptr_cache->CacheBlock_temp);
	ptr_cache->Set = NULL;
	ptr_cache->CacheBlock_temp = NULL;
	return true;
}

bool print_csv(char *file_name, char *argv[], double miss_rate)
{
	if (NULL == file_name)
	{
		assert(NULL != file_name);
		return false;
	}

	if (NULL == argv)
	{
		assert(NULL != argv);
		return false;
	}

	FILE *data_table = fopen(file_name, "a");
	if (NULL == data_table)
	{
		assert(NULL != data_table);
		return false;
	}

	fprintf(data_table, "%s,%s,%s,%.4lf\n", argv[CACHE_SIZE], argv[BLOCK_SIZE], argv[SET_DEGREE], miss_rate);

	int value = fclose(data_table);
	if (EOF == value)
	{
		assert(EOF != value);
		return false;
	}
	return true;
}