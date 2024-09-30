#include "cache.h"

myInt hashFunction(Cache *cache, myInt key) {
    // https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = (key >> 16) ^ key;
    return key % cache->hashTablesSize;
}

void accessBin(Cache *cache, cacheBin *entry) {
    if (cache->policy == LRU) {
        if (entry == cache->linkedListsHead) {
            return;
        }
        if (entry->previous != NULL) {
            entry->previous->next = entry->next;
        }
        if (entry->next != NULL) {
            entry->next->previous = entry->previous;
        }
        if (entry == cache->linkedListsTail) {
            cache->linkedListsTail = entry->previous;
        }
        entry->next = cache->linkedListsHead;
        entry->previous = NULL;
        if (cache->linkedListsHead != NULL) {
            cache->linkedListsHead->previous = entry;
        }
        cache->linkedListsHead = entry;
        if (cache->linkedListsTail == NULL) {
            cache->linkedListsTail = entry;
        }
    } else if (cache->policy == FIFO) {
        return;
    }
}

cacheBin *lookup(Cache *cache, myInt key) {
    myInt index = hashFunction(cache, key);
    cacheBin *temp = cache->hashTable[index];
    while (temp != NULL) {
        if (temp->key == key) {
            return temp;
        }
        temp = temp->nextHashTable;
    }
    return NULL;
}

void initializeCache(Cache *cache, myInt capacity, CachePolicy policy) {
    cache->cacheHits = 0;
    cache->cacheMisses = 0;
    cache->capacity = capacity;
    cache->size = 0;
    cache->policy = policy;
    cache->linkedListsHead = NULL;
    cache->linkedListsTail = NULL;
    cache->hashTablesSize = 65537;  // may need to be adjusted
    cache->hashTable =
        (cacheBin **)malloc(sizeof(cacheBin *) * cache->hashTablesSize);

    for (myInt index = 0; index < cache->hashTablesSize; index++) {
        cache->hashTable[index] = NULL;
    }
}

void insertIntoCache(Cache *cache, myInt key, myInt data) {
    cacheBin *entry = lookup(cache, key);
    if (entry != NULL) {
        entry->data = data;
        accessBin(cache, entry);
        return;
    }
    if (cache->size == cache->capacity) {
        cacheBin *binToBeDumped;
        if (cache->policy == LRU || cache->policy == FIFO) {
            binToBeDumped = cache->linkedListsTail;
        }
        evict(cache, binToBeDumped);
    }
    entry = (cacheBin *)malloc(sizeof(cacheBin));
    entry->key = key;
    entry->data = data;
    entry->previous = NULL;
    entry->next = NULL;
    entry->nextHashTable = NULL;

    myInt index = hashFunction(cache, key);
    entry->nextHashTable = cache->hashTable[index];
    cache->hashTable[index] = entry;

    if (cache->policy == LRU || cache->policy == FIFO) {
        entry->next = cache->linkedListsHead;
        if (cache->linkedListsHead != NULL) {
            cache->linkedListsHead->previous = entry;
        }
        cache->linkedListsHead = entry;
        if (cache->linkedListsTail == NULL) {
            cache->linkedListsTail = entry;
        }
    }
    cache->size++;
}

void evict(Cache *cache, cacheBin *entry) {
    myInt index = hashFunction(cache, entry->key);
    cacheBin *current = cache->hashTable[index];
    cacheBin *prev = NULL;

    while (current != NULL) {
        if (current == entry) {
            if (prev == NULL) {
                cache->hashTable[index] = current->nextHashTable;
            } else {
                prev->nextHashTable = current->nextHashTable;
            }
            break;
        }
        prev = current;
        current = current->nextHashTable;
    }

    if (cache->policy == LRU || cache->policy == FIFO) {
        if (entry->previous != NULL) {
            entry->previous->next = entry->next;
        }
        if (entry->next != NULL) {
            entry->next->previous = entry->previous;
        }
        if (entry == cache->linkedListsHead) {
            cache->linkedListsHead = entry->next;
        }
        if (entry == cache->linkedListsTail) {
            cache->linkedListsTail = entry->previous;
        }
    }
    free(entry);
    cache->size--;
}

myInt countOfEntries(Cache *cache) { return cache->size; }

void deconstructCache(Cache *cache) {
    for (myInt index = 0; index < cache->hashTablesSize; index++) {
        cacheBin *entry = cache->hashTable[index];
        while (entry != NULL) {
            cacheBin *nextEntry = entry->nextHashTable;
            free(entry);
            entry = nextEntry;
        }
    }
    free(cache->hashTable);
    cache->hashTable = NULL;
    cache->linkedListsHead = NULL;
    cache->linkedListsTail = NULL;
    cache->size = 0;
}
