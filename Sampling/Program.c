#include "Lazy_Sampling.h"
#include "correction.h"
#include "Random.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    double p = 0;
    unsigned int populationNumber = 0, sampleNumber = 0;
    int seed = 96661;
    LazySource source;
    unsigned int positives = 0;
    double correctedPercentage;
    unsigned int correctedPositives;
    
    const char *resultFileName = "sampleResult.txt";
    FILE *resultFile = NULL;

    if (argc < 4 || argc > 5
                 || sscanf(argv[1], "%lf", &p) != 1
                 || sscanf(argv[2], "%ud", &populationNumber) != 1
                 || sscanf(argv[3], "%ud", &sampleNumber) != 1
                 || (argc == 5 && sscanf(argv[4], "%d", &seed) != 1))
    {
        fprintf(stderr, "Usage: %s <p> <N> <n> [<s>]\n", argv[0]);
        return 1;
    }
    
    if (sampleNumber > populationNumber)
    {
        fprintf(stderr, "Samplesize can't be larger than Populationsize\n");
        return 1;
    }
    if (p < 0 || p > 1)
    {
        fprintf(stderr, "Probability can't be less than 0 or more than 1\n");
        return 1;
    }

    // Sampling_Init
    initRandom(seed);

    source = getLazySource(populationNumber, p);

    // Sampling_Ready -> Sampling_Progressing
    for (int i = 0; i < sampleNumber; i++)
    {
        if (takeElement(&source))
        {
            if (getWithProbability(SENSITIVITY))
            {
                positives++;
            }
        }
        else
        {
            if (getWithProbability(1 - SPECIFICITY))
            {
                positives++;
            }
        }
    }

    correctedPercentage = correction((double) positives / sampleNumber);
    correctedPositives = correctedPercentage * sampleNumber;

    // Sampling_Done
    printf("Sample result: %u\n", correctedPositives);
    resultFile = fopen(resultFileName, "w");
    if(resultFile == NULL)
    {
        fprintf(stderr, "Could not open file");
        return 1;
    }
        fprintf(resultFile, "%u\n", correctedPositives);
        fclose(resultFile);
    return 0;
}
