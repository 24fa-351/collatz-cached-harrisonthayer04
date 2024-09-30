#ifndef CACHE_H
#define CACHE_H

#include <stdlib.h>

typedef long long unsigned myInt;
typedef enum { NONE, LRU, FIFO, ARC } CachePolicy;

typedef struct cacheBin {
    myInt key;
    myInt data;

    struct cacheBin *previous;
    struct cacheBin *next;
    struct cacheBin *nextHashTable;

} cacheBin;

typedef struct Cache {
    myInt cacheHits;
    myInt cacheMisses;

    myInt capacity;
    myInt size;

    cacheBin *linkedListsHead;
    cacheBin *linkedListsTail;

    myInt hashTablesSize;
    cacheBin **hashTable;

    CachePolicy policy;
} Cache;

void initializeCache(Cache *cache, myInt capacity, CachePolicy policy);
void insertIntoCache(Cache *cache, myInt key, myInt data);
void deconstructCache(Cache *cache);
cacheBin *lookup(Cache *cache, myInt key);
myInt countOfEntries(Cache *cache);
void evict(Cache *cache, cacheBin *entry);
void accessBin(Cache *cache, cacheBin *entry);
myInt hashFunction(Cache *cache, myInt key);

#endif  // for CACHE_H