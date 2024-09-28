#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "cache.h"

#define IS_EVEN(x) (((x) & 1) == 0)
typedef long long unsigned myInt;

long long unsigned int generateRandomNumber(
    long long unsigned int nSmallestPossibleValue,
    long long unsigned int nLargestPossibleValue) {
    // https://mathbits.com/MathBits/CompSci/LibraryFunc/rand.htm <- Inspiration
    return nSmallestPossibleValue +
           rand() % (nLargestPossibleValue - nSmallestPossibleValue + 1);
}

long long unsigned int *collatzConjecture(
    long long unsigned int nTestValues, long long unsigned int nSmallestValue,
    long long unsigned int nLargestValue, Cache *cache) {
    FILE *csvFile;
    csvFile = fopen("collatzResults.csv", "w+");
    fprintf(csvFile, "Number,Steps\n");

    myInt *results = (myInt *)malloc(nTestValues * sizeof(myInt));


    for (long long unsigned int index = 0; index < nTestValues; index++) {
        long long unsigned int numberOfStepsUntilOne = 0;
        long long unsigned int randomNumber =
            generateRandomNumber(nSmallestValue, nLargestValue);
        long long unsigned int currentNumber = randomNumber;

        myInt stackCapacity = 4096;
        myInt *stack = (myInt *)malloc(sizeof(myInt) * stackCapacity);
        myInt stackIndex = 0;

while (currentNumber != 1) {
            // Check if currentNumber is in cache.
            cacheBin *entry = lookup(cache, currentNumber);
            if (entry != NULL) {
                // Use cached value.
                numberOfStepsUntilOne += entry->data;
                // Update LRU.
                accessBin(cache, entry);
                break;
            }

            // Store currentNumber in stack.
            if (stackIndex >= stackCapacity) {
                // Need to reallocate stack.
                stackCapacity *= 2;
                stack = (myInt *)realloc(stack, sizeof(myInt) * stackCapacity);
            }
            stack[stackIndex++] = currentNumber;

            // Perform Collatz operation.
            if (currentNumber % 2 == 1) {
                currentNumber = 3 * currentNumber + 1;
            } else {
                currentNumber = currentNumber / 2;
            }
            numberOfStepsUntilOne += 1;
        }
        results[index] = numberOfStepsUntilOne;

        // Update the cache with numbers from the stack.
        myInt steps = numberOfStepsUntilOne;
        for (myInt i = stackIndex; i > 0; i--) {
            myInt num = stack[i-1];
            insertIntoCache(cache, num, steps);
            steps -= 1;
        }
        free(stack);

        // Write to CSV file.
        fprintf(csvFile, "%llu,%llu\n", randomNumber, numberOfStepsUntilOne);
    }

    fclose(csvFile);
    return results;
}

int main(int argc, char *argv[]) {
    long long unsigned int userNumberOfValuesToTest = atoi(argv[1]);
    long long unsigned int userSmallestValueToTest = atoi(argv[2]);
    long long unsigned int userLargestValueToTest = atoi(argv[3]);

    srand(time(NULL) ^ getpid());


    Cache cache;
    initializeCacheBin(&cache, 100000000);
    long long unsigned int *output =
        collatzConjecture(userNumberOfValuesToTest, userSmallestValueToTest,
                          userLargestValueToTest, &cache);
    for (long long unsigned int index = 0; index < userNumberOfValuesToTest;
         index++) {
        //printf("%llu: ", index + 1);
        //printf("%llu\n", output[index]);
    }
    free(output);
    deconstructCache(&cache);
    return 0;
}