/*************************************************************
*name: ÂíÖÂÔ¶
*loginID:ics517021910070
...
*/
#include "cachelab.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <errno.h>

int hit_count=0;
int miss_count=0;
int eviction_count=0;


typedef struct {
	char valid_bit;
	unsigned long tag;
	int LRU_count;
} Cache_line;

typedef struct {
	Cache_line* lines;
} Cache_set;

typedef struct {
	int S;
	int E;
	Cache_set* sets;
} Cache;

/*
	s -- the number of sets
	E -- the number of cache lines in one set
	b -- the size of one block in one cache line
*/


void init_cache(int s, int E, int b, Cache* cache) 
{
	cache->S = 2 << s;
	cache->E = E;
	cache->sets = (Cache_set*)malloc(cache->S * sizeof(Cache_set));

	int i, j;
	for (i = 0; i < cache->S; i++) {//init every cache line
		cache->sets[i].lines = (Cache_line*)malloc(E * sizeof(Cache_line));
		for (j = 0; j < cache->E; j++) 
		{//init every cache line
			cache->sets[i].lines[j].valid_bit = 0;
			cache->sets[i].lines[j].LRU_count = 0;
		}
	}

	return;
}

int get_hitIndex(Cache *cache, int set_index, int tag)//whether there is a hit
{
    int hitIndex = -1;

    for (int i = 0; i < cache->E; i++) 
	{
        if (cache->sets[set_index].lines[i].valid_bit == 1 && cache->sets[set_index].lines[i].tag == tag) // valid and the tag matches
		{
            hitIndex = i;
            break;
        }
    }

    return hitIndex;
}

int get_emptyIndex(Cache *cache, int set_index, int tag) //find whether there is an empty line in the given set
{
    int i;
    int emptyIndex = -1;
    for (i = 0; i < cache->E; ++i) 
	{
        if (cache->sets[set_index].lines[i].valid_bit == 0) 
		{
            emptyIndex = i;
            break;
        }
    }

    return emptyIndex;
}

 
 
 void load(Cache *cache, int set_index, int tag, int isVerbose) 
 {
    int hitIndex = get_hitIndex(cache, set_index, tag);//whether there is a hit
    if (hitIndex == -1) 
	{               //one miss
        miss_count++;
        if (isVerbose) 
		{
            printf("miss ");
        } 
        int emptyIndex = get_emptyIndex(cache, set_index, tag);     
        if (emptyIndex == -1) //need eviction
		{         
            eviction_count++;
            if (isVerbose)
			{
                printf("eviction ");
            }
			int flag = 1;
            for (int i = 0; i < cache->E; i++)
			{
                if (cache->sets[set_index].lines[i].LRU_count == cache->E - 1 && flag==1) //find the least recent used line, and other line LRU_count++
				{
                    cache->sets[set_index].lines[i].valid_bit = 1;
                    cache->sets[set_index].lines[i].LRU_count = 0;
                    cache->sets[set_index].lines[i].tag = tag;
                    flag = 0;
                } 
				else
				{
                    cache->sets[set_index].lines[i].LRU_count++;//it is not used this time
                }
            }
        }
		else    // don't need eviction
		{           
            
            for (int i = 0; i < cache->E; i++) 
			{
                if (i != emptyIndex)
			    {
                    cache->sets[set_index].lines[i].LRU_count++;//it is not used this time
                } 
				else 
				{
                    cache->sets[set_index].lines[i].valid_bit = 1;
                    cache->sets[set_index].lines[i].tag = tag;
                    cache->sets[set_index].lines[i].LRU_count = 0;
                }
            }
        }
    } 
	else //one hit
	{                          
        hit_count++;
        if (isVerbose)
		{
            printf("hit ");
        }

        int tempLRU_count = cache->sets[set_index].lines[hitIndex].LRU_count;
        for (int i = 0; i < cache->E; i++) 
		{
            if (i != hitIndex) 
			{
                if (cache->sets[set_index].lines[i].LRU_count <  tempLRU_count)//less than the hit one's LRU
				{
                    cache->sets[set_index].lines[i].LRU_count++;
                }
            } 
			else 
			{
                cache->sets[set_index].lines[i].LRU_count = 0;// the hit one's LRU is set to zero
            }
        }
    }
}

void store(Cache *cache, int set_index, int tag, int isVerbose)//store is just like a load
{
    load(cache, set_index, tag, isVerbose);
}



void modify(Cache *cache, int set_index, int tag, int isVerbose) // a write is just like one load then one store
{
    load(cache, set_index, tag, isVerbose);
    store(cache, set_index, tag, isVerbose);
}

void virtualcache(int s, int E, int b, char* pfile, int isVerbose, Cache* cache)
{
	FILE *file;                        // pointer to FILE object 
	file = fopen(pfile, "r");
	char type;                          // L-load S-store M-modify 
	unsigned long address;              // 64-bit  memory address 
	int size;                           //number of bytes accessed by operation 

	int tag_move_bits = b + s;//

	while (fscanf(file, " %c %lx,%d", &type, &address, &size) > 0) //for every line in the file
	{
		if (type == 'I')
		{
			continue;//if it is an instruction, do nothing
		}
		else 
		{
			
			int tag = address >> tag_move_bits;//get the tag
			int set_index = (address >> b) & ((1 << s) - 1);//get the index
			if (isVerbose == 1) //print detailed info
			{
				printf("%c %lx,%d ", type, address, size);
			}
			if (type == 'S') {
				store(cache, set_index, tag, isVerbose);
			}
			if (type == 'M') {
				modify(cache, set_index, tag, isVerbose);
			}
			if (type== 'L') {
				load(cache, set_index, tag, isVerbose);
			}
			if (isVerbose == 1) {
				printf("\n");
			}
		}
	}

	fclose(file);
	return;
}


int main(int argc, char *argv[])
{
	int s, E, b;
	char pfile[100];                                             //store the name of the file
	int isVerbose = 0;
	Cache cache;
	char ch;
	while ((ch = getopt(argc, argv, "vs:E:b:t:")) != -1) {
		switch (ch) {
		case 'v':
			isVerbose = 1;
			break;
		case 's':
			s = atoi(optarg);
			break;
		case 'E':
			E = atoi(optarg);
			break;
		case 'b':
			b = atoi(optarg);
			break;
		case 't':
			strcpy(pfile, optarg);//copy the address of trace to file
			break;
		default:
			break;
		}
	}

	init_cache(s, E, b, &cache);
	virtualcache(s, E, b, pfile, isVerbose, &cache);
	printSummary(hit_count, miss_count, eviction_count);
	return 0;
}






