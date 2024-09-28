#include "cache.h"

void accessBin(Cache *cache, cacheBin *entry){
    if (entry == cache->linkedListsHead){
        return;
    }

    // if the bin is accessed, it will need 
    // to be moved to the begining of the 
    // linked list, so we will remove it from
    // where it is here:
    if(entry->previous != NULL){
        entry->previous->next = entry->next;
    }
    if(entry->next != NULL){
        entry->next->previous = entry->previous;
    }
    if(entry == cache->linkedListsTail){
        cache->linkedListsTail = entry->previous;
    }

    // So now we took care of the nodes previous
    // and next pointers properly, we can
    // focus on moving this bin to the begining
    // of the list
    entry->next = cache->linkedListsHead;
    entry->previous = NULL;
    if(cache->linkedListsHead != NULL){
        cache->linkedListsHead->previous = entry;
    }
    cache->linkedListsHead = entry;
    if(cache->linkedListsTail == NULL){
        cache->linkedListsTail = entry;
    }
}

cacheBin* lookup(Cache *cache, myInt key){
    myInt index = hashFunction(cache, key);
    cacheBin *temp = cache->hashTable[index];
    while(temp != NULL){
        if(temp->key == key){
            return temp;
        }
        temp = temp->nextHashTable;
    }
    return NULL;
}

myInt hashFunction(Cache *cache, myInt key){
    // this is a new hash function i am testing to
    // try to optimize my performance. 
    // https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = (key >> 16) ^ key;
    return key % cache->hashTablesSize;
    
}


void initializeCacheBin(Cache *cache, myInt capacity){
    cache->capacity = capacity;
    cache->size = 0;

    cache->linkedListsHead = NULL;
    cache->linkedListsTail = NULL;
    cache-> hashTablesSize = 131071; // This may need to be adjusted


    // Breakdown: Allocate the memory we will use for the hash table
    // the hashtable is going to store an array of pointers to our cacheBins
    // "cache->hashTablesSize" is the number of bins we need to store in the hash
    // table, then we will loop over the hash table indices and initialize each
    // spot in the hash table to NULL
    cache->hashTable = (cacheBin**)malloc(sizeof(cacheBin*) * cache->hashTablesSize);
    for (myInt index = 0 ; index < cache->hashTablesSize; index++){
        cache->hashTable[index] = NULL;
    }
}


void insertIntoCache(Cache *cache, myInt key, myInt data){
    // First, see if the cacheBin already exists by searching the 
    // Cache.
    cacheBin *entry = lookup(cache, key);
    // If it does, we can just make the data to the
    // head of the linked list
    if (entry != NULL){
        entry->data = data;
        accessBin(cache, entry);
        return;
    }

    // if the cache is full we need to dump the 
    // least recently used bin so we can add our
    // new bin
    if (cache->size == cache->capacity){
        cacheBin *evictBin = cache-> linkedListsTail;
        evict(cache, evictBin);
    }

    // create a bin to put into the cache, init
    // all of the data because we can't use a constructor
    // like we would in c++ sigh
    entry = (cacheBin*)malloc(sizeof(cacheBin));
    entry->key = key;
    entry->data = data;
    entry->previous = NULL;
    entry->next = NULL;
    entry->nextHashTable = NULL;
    
    // add your new entry into the hash table
    myInt index = hashFunction(cache, key);
    entry->nextHashTable = cache->hashTable[index];
    cache->hashTable[index] = entry;

    // add the new entry to the front / head of linked list
    entry->next = cache->linkedListsHead;
    if(cache->linkedListsHead != NULL){
        cache->linkedListsHead->previous = entry;
    }
    cache->linkedListsHead = entry;
    if(cache->linkedListsTail == NULL){
        cache->linkedListsTail = entry;
    }
    cache->size++;

}



// Support / Internal Methods

void evict(Cache *cache, cacheBin *entry){
    // remove the bin from the hash table
    myInt index = hashFunction(cache, entry->key);
    cacheBin *current = cache->hashTable[index];
    cacheBin *prev = NULL;
    while(current != NULL){
        if(current == entry){
            if(prev == NULL){
                cache->hashTable[index] = current->nextHashTable;
            }else{
                prev->nextHashTable = current->nextHashTable;
            }
            break;

        }
        prev = current;
        current = current->nextHashTable;
    }
    // now that its out of the hash table we will 
    // remove the bin from the linked list
    // Remove entry from linked list
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
    free(entry);
    cache->size--;
}

myInt countOfEntries(Cache *cache){
    return cache->size;
}


void deconstructCache(Cache *cache){
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
