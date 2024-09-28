#ifndef CACHE_H
#define CACHE_H

#include <stdlib.h>


typedef long long unsigned myInt;

typedef struct cacheBin {
    myInt key; // Here the key is the number being fed into the collatz algorithm.
    myInt data; // Here the data is the number of steps it will take for key to reach one if fed through the collatz algorithm.

    
    struct cacheBin *previous;
    struct cacheBin *next;
    struct cacheBin *nextHashTable;

} cacheBin;

typedef struct Cache {
    myInt capacity; // how many items the cache CAN hold vs...
    myInt size;     // how many items the cach IS holding.

    cacheBin *linkedListsHead;
    cacheBin *linkedListsTail;


    // Hash table should allow for O(1) Lookup times !!
    myInt hashTablesSize;
    cacheBin **hashTable;

} Cache;


// All of the below functions are pulled directly from the lecture slides
// Primary Methods

void initializeCacheBin(Cache *cache, myInt capacity);
void insertIntoCache(Cache *cache, myInt key, myInt data);
void deconstructCache(Cache *cache);
cacheBin* lookup(Cache *cache, myInt key);
myInt countOfEntries(Cache *cache);


// Support / Internal Methods

void evict(Cache *cache, cacheBin *entry);
void accessBin(Cache *cache, cacheBin *entry);
myInt hashFunction(Cache *cache, myInt key);

#endif // for CACHE_H