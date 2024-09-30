#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "cache.h"

#define IS_EVEN(x) (((x)&1) == 0)
typedef long long unsigned myInt;

myInt generateRandomNumber(myInt nSmallestPossibleValue,
                           myInt nLargestPossibleValue) {
    // https://mathbits.com/MathBits/CompSci/LibraryFunc/rand.htm <- Inspiration
    return nSmallestPossibleValue +
           rand() % (nLargestPossibleValue - nSmallestPossibleValue + 1);
}

myInt collatzConjecture(myInt collatzNumber) {
    myInt steps = 0;
    while (collatzNumber != 1) {
        if (IS_EVEN(collatzNumber))
            collatzNumber = collatzNumber / 2;
        else
            collatzNumber = 3 * collatzNumber + 1;
        steps++;
    }
    return steps;
}
myInt collatzConjectureCached(myInt collatzNumber, Cache *cache) {
    cacheBin *entry = lookup(cache, collatzNumber);
    if (entry != NULL) {
        accessBin(cache, entry);
        cache->cacheHits++;
        return entry->data;
    }
    cache->cacheMisses++;
    myInt steps = collatzConjecture(collatzNumber);
    insertIntoCache(cache, collatzNumber, steps);
    return steps;
}

int main(int argc, char *argv[]) {
    myInt userNumberOfValuesToTest = atoll(argv[1]);
    myInt userSmallestValueToTest = atoll(argv[2]);
    myInt userLargestValueToTest = atoll(argv[3]);
    char *cachePolicy = argv[4];

    srand(time(NULL) ^ getpid());

    CachePolicy cachePolicyUsed;
    if (strcmp(cachePolicy, "none") == 0)
        cachePolicyUsed = NONE;
    else if (strcmp(cachePolicy, "LRU") == 0)
        cachePolicyUsed = LRU;
    else if (strcmp(cachePolicy, "FIFO") == 0)
        cachePolicyUsed = FIFO;
    else {
        printf(
            "Error: Options for cache policy are none, LRU, and FIFO\n");
        return 1;
    }
    Cache cache;
    if (cachePolicyUsed != NONE) {
        myInt cacheSize = atoll(argv[5]);
        initializeCache(&cache, cacheSize, cachePolicyUsed);
    }
    FILE *csvFile = fopen("collatzResults.csv", "w+");
    fprintf(csvFile, "Number,Steps\n");
    for (myInt index = 0; index < userNumberOfValuesToTest; index++) {
        myInt randomNumber = generateRandomNumber(userSmallestValueToTest,
                                                  userLargestValueToTest);
        myInt stepsUntilOne;

        if (cachePolicyUsed == NONE)
            stepsUntilOne = collatzConjecture(randomNumber);
        else
            stepsUntilOne = collatzConjectureCached(randomNumber, &cache);

        fprintf(csvFile, "%llu,%llu\n", randomNumber, stepsUntilOne);
    }
    fclose(csvFile);
    if (cachePolicyUsed != NONE) {
        myInt hitsPlusMisses = cache.cacheHits + cache.cacheMisses;
        double cacheHitRate = 0.0;
        if (hitsPlusMisses > 0) {
            cacheHitRate =
                ((double)cache.cacheHits / (double)hitsPlusMisses) * 100.0;
        }
        printf("Cache Hit Percentage: %.2f%%\n", cacheHitRate);

        deconstructCache(&cache);
    }
}