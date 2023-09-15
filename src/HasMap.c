#include "malloc_tester.h"

void initialize(struct HashMap *hashmap)
{
	hashmap->size = 0;
}

void incrementOrInsert(struct HashMap* hashmap, const char* key) {
    int i;
    for (i = 0; i < hashmap->size; i++) {
        if (strcmp(hashmap->data[i].key, key) == 0) {
            hashmap->data[i].value++;
            return;
        }
    }
    if (hashmap->size < MAX_SIZE) {
        strcpy(hashmap->data[hashmap->size].key, key);
        hashmap->data[hashmap->size].value = 1;
        hashmap->size++;
    }
}

void put(struct HashMap *hashmap, const char *key, int value)
{
	int i;
	for (i = 0; i < hashmap->size; i++)
	{
		if (strcmp(hashmap->data[i].key, key) == 0)
		{
			hashmap->data[i].value += value;
			return;
		}
	}

	if (hashmap->size < MAX_SIZE)
	{
		strcpy(hashmap->data[hashmap->size].key, key);
		hashmap->data[hashmap->size].value = value;
		hashmap->size++;
	}
}

int get(struct HashMap *hashmap, const char *key)
{
	int i;
	for (i = 0; i < hashmap->size; i++)
	{
		if (strcmp(hashmap->data[i].key, key) == 0)
		{
			return hashmap->data[i].value;
		}
	}
	return 0;
}
