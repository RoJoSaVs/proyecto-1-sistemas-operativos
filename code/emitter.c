#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdbool.h>
#include <math.h>

// Local imports
#include "colorConf.c"
#include "infoStruct.c"

// Constant values for the whole code Semaphores and Shared Memory pointer
// -----------------------------------------------------------------------//
// Control for stats
sem_t *semStats;
struct controlStats *stats;

// Control for array
sem_t *semReceivers; // For receivers control
sem_t *semEmitters; // For emitters control
struct charQueue *charArray; // Map the shared memory segment in process address space

// String readed from shared memory
char *stringFromMemory;

void errorMsg(){
    yellow();
    printf("--------------------------------------------------------\n");
    cyan();
    printf("Please insert the requested data\n");
    printf(" 1 - Buffer name | Example: CE\n");
    printf(" 2 - Execution mode | Example: (auto or manual)\n");
    printf(" 3 - Key value to encript data | Example: 5\n");
    printf(" Example Command for execution: ./output/emiter CE auto 5 \n");
    yellow();
    printf("--------------------------------------------------------\n");
    reset();
}


void stringDataFromSharedMemory()
{
    // Semaphore for file string access
    sem_t *semSharedText;
    semSharedText = sem_open("textFromFile", O_CREAT | O_RDWR, 0666, 1);

    sem_wait(semSharedText);
    // sleep(10);


    int shm_text; //file descriptor of shared memory file
    void *sharedText;
    char *sharedTextName = "textMemory";

    shm_text = shm_open(sharedTextName, O_CREAT | O_RDWR, 0666); // Shared memory for data

    if(shm_text == -1)
    {
        red();
        printf("Buffer doesn't exist\n");
        reset();
    }
    
    sharedText = mmap(0, sizeof(char *), PROT_READ | PROT_WRITE, MAP_SHARED, shm_text, 0);

    stringFromMemory = (char *) sharedText;
    
    sem_post(semSharedText);

    // return stringFromMemory;
}

void getStatsStruct()
{
    // Semaphore for stats struct access
    char *semStatsName = "stats";
    semStats = sem_open(semStatsName, O_CREAT | O_RDWR, 0666, 1);
    
    sem_wait(semStats);
    // sleep(10);

    // Stats struct shared memory
    int shm_stats;
    shm_stats = shm_open("shareStats", O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"
    stats = mmap(0, sizeof(struct controlStats), PROT_READ | PROT_WRITE, MAP_SHARED, shm_stats, 0);


    // stats->totalEmitters = 0;
    // stats->emittersAlive = 0;
    // printf("Total emiters %d \n", stats->totalEmitters);

    sem_post(semStats);
}


int getDecimal(int clave)
{
    int num = clave;
    int dec_value = 0;
 
    // Initializing base value to 1, i.e 2^0
    int base = 1;
 
    int temp = num;
    while (temp) {
        int last_digit = temp % 10;
        temp = temp / 10;
 
        dec_value += last_digit * base;
 
        base = base * 2;
    }
 
    return dec_value;
}

// void emitterLogic(int spacesToRead, char *shareMemoryName)
void emitterLogic(int spacesToRead, int keyValue)
{
    char *shareMemoryName = "CE";

    // Semaphore for array in shared memory (Receiver)
    char *semReceiversName = "empty";
    semReceivers = sem_open(semReceiversName, O_CREAT, 0666, 0);


    // Semaphore for array in shared memory (Emitter)
    char *semEmittersName = "filled";
    semEmitters = sem_open(semEmittersName, O_CREAT | O_RDWR, 0666, spacesToRead);


    // Array from shared memory
    int shm_array; //file descriptor of shared memory file
    shm_array = shm_open(shareMemoryName, O_CREAT | O_RDWR, 0666); // Shared memory for data
    charArray = mmap(0, sizeof(struct charQueue[spacesToRead]), PROT_READ | PROT_WRITE, MAP_SHARED, shm_array, 0);

    int stringSize = 0;
    int stringIndex = 0;
    int emitterIndex = 0;

    // Manual execution


    // Initial values for the first reading
    sem_wait(semStats);
    stringSize = stats->inputTextSize;
    stringIndex = stats->stringIndex;
    emitterIndex = stats->emitterIndex;
    spacesToRead = stats->spacesToRead;
    sem_post(semStats);

    int key = getDecimal(keyValue);

    for(emitterIndex = 0; emitterIndex <= stringSize; emitterIndex++)
    {
        // Manual execution code

        
        sem_wait(semStats);
        if(stats->stringIndex == stats->inputTextSize)
        {
            break;
        }

        emitterIndex = stats->emitterIndex;
        if(stats->emitterIndex == (stats->spacesToRead - 1))
        {
            stats->emitterIndex = 0;
        }
        else
        {
            stats->emitterIndex++;
        }
        stats->valuesInMemory++;
        sem_post(semStats);

        sem_wait(semEmitters);
        charArray[emitterIndex].index = emitterIndex;
        charArray[emitterIndex].charValue = stringFromMemory[emitterIndex] ^ key;
        // charArray[emitterIndex].timeCreated = string;

        printf("Added %d value in index %d at date \n", charArray[emitterIndex].index, charArray[emitterIndex].charValue);
        sleep(5);
        sem_post(semReceivers);

    }
}



int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        errorMsg();
        return 0;
    }
    else
    {
        // =================================================================================== //
        // Shared memory code section for processing
        char *shareMemoryName; // Name given to the shared memory space 
        char *executionMode; // Total of spaces to write and read values (char)
        char *keyValueChar; // Key value to encription (char)
        
        shareMemoryName = argv[1];
        executionMode = argv[2];
        keyValueChar = argv[3];


        int keyValue; // Key value to encription
        keyValue = atoi(keyValueChar); // Cast from char to int

        // Shared memory access

        // char *stringFromMemory = stringDataFromSharedMemory(); //Gets file string from shared memory
        stringDataFromSharedMemory(); //Gets file string from shared memory
        getStatsStruct();
        emitterLogic(10, keyValue);
        
        


        return 0;
    }

};