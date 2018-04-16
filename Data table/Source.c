#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

int main(void)
{
	unsigned int Cache_Size, Block_Size, n_way;
	char buffer[3][30];
	
	Cache_Size = 128 << 10;
	Block_Size = 16;
	for (size_t i = 0; i < 4; i++)
	{
		n_way = 1;
		for (size_t i = 0; i < 4; i++)
		{
			sprintf(buffer[0], "%u", Cache_Size);
			sprintf(buffer[1], "%u", Block_Size);
			sprintf(buffer[2], "%u", n_way);

			int value = spawnl(P_WAIT, "Cache.exe", "Cache.exe", " trace.txt", buffer[0], buffer[1], buffer[2], NULL);
			if (0 != value)
			{
				perror("error");
				assert(0 == value);
				return EXIT_FAILURE;
			}

			n_way <<= 1;
		}
		Cache_Size <<= 1;
	}

	Cache_Size = 512 << 10 ;
	for (size_t i = 0; i < 2; i++)
	{
		Block_Size = 8;
		for (size_t i = 0; i < 4; i++)
		{
			n_way = 1;
			for (size_t i = 0; i < 4; i++)
			{
				sprintf(buffer[0], "%u", Cache_Size);
				sprintf(buffer[1], "%u", Block_Size);
				sprintf(buffer[2], "%u", n_way);

				int value = spawnl(P_WAIT, "Cache.exe", "Cache.exe", " trace.txt", buffer[0], buffer[1], buffer[2], NULL);
				if (0 != value)
				{
					perror("error");
					assert(0 == value);
					return EXIT_FAILURE;
				}

				n_way <<= 1;
			}
			Block_Size <<= 1;
		}
		Cache_Size <<= 1;
	}
	return 0;
}