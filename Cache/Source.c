#include "cache.h"

int main(int argc, char *argv[])
{
	Cache cache;
	FILE *fptr = NULL;
	double miss_rate;

	if (false == Init(argc, argv, &cache, &fptr))
	{
		return EXIT_FAILURE;
	}

	if (false == compute_miss_rate(fptr, &cache, &miss_rate))
	{
		return EXIT_FAILURE;
	}

	if (false == free_cache_file(&fptr, &cache))
	{
		return EXIT_FAILURE;
	}

	if (false == print_csv("Miss Rate.csv", argv, miss_rate))
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}