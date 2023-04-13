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

// Control for string loaded from file
sem_t *semSharedText;


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

void setSharedItems(char *shareMemoryName){
    // ------------------------------------------------- //
    // ------------------ Semaphores ------------------- //

    // Semaphore for file string access
    semSharedText = sem_open("textFromFile", O_CREAT | O_RDWR, 0666, 1);

    // Semaphore for stats struct access
    char *semStatsName = "stats";
    semStats = sem_open(semStatsName, O_CREAT | O_RDWR, 0666, 1);

    // Semaphore for array in shared memory (Receiver)
    char *semReceiversName = "empty";
    semReceivers = sem_open(semReceiversName, O_CREAT, 0666, 0);


    // Semaphore for array in shared memory (Emitter)
    char *semEmittersName = "filled";
    semEmitters = sem_open(semEmittersName, O_CREAT | O_RDWR, 0666, stats->spacesToRead);


    // ------------------------------------------------- //
    // -------------------- Memory --------------------- //

    // Array from shared memory
    int shm_array; //file descriptor of shared memory file
    shm_array = shm_open(shareMemoryName, O_CREAT | O_RDWR, 0666); // Shared memory for data
    charArray = mmap(0, sizeof(struct charQueue[stats->spacesToRead]), PROT_READ | PROT_WRITE, MAP_SHARED, shm_array, 0);

}

void stringDataFromSharedMemory()
{
    // Semaphore for file string access
    sem_wait(semSharedText);

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
}

void getStatsStruct()
{
    // Stats struct shared memory
    int shm_stats;
    shm_stats = shm_open("shareStats", O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"
    stats = mmap(0, sizeof(struct controlStats), PROT_READ | PROT_WRITE, MAP_SHARED, shm_stats, 0);

    stats->emittersAlive++;
    stats->totalEmitters++;
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

void getDateTime(int index)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(charArray[index].timeCreated, sizeof(charArray[index].timeCreated)-1, "%d %m %Y %X", t);
}


void emitterLogic(int keyValue, int executionMode)
{
    int stringSize = 0;
    int stringIndex = 0;
    int emitterIndex = 0;
    int spacesToRead = 0;

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
        if(executionMode == 1){
            green();
            printf("Please press Enter: ");
            char ch = fgetc(stdin); //read a single character

            if(ch == 0x0A)
            {
                printf("ENTER KEY is pressed.\n");
            }
        }
        else // Auto mode delay
        {
            sleep(2);
        }


        sem_wait(semStats);
        if(stats->stringIndex == stats->inputTextSize)
        {
            sem_post(semStats);
            break;
        }

        emitterIndex = stats->emitterIndex;

        stringIndex = stats->stringIndex;
        stats->stringIndex++;

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
        charArray[emitterIndex].charValue = stringFromMemory[stringIndex] ^ key;
        getDateTime(emitterIndex);

        cyan();
        printf("Added %d    value in index %d   at date %s\n", charArray[emitterIndex].index, charArray[emitterIndex].charValue, charArray[emitterIndex].timeCreated);
        sem_post(semReceivers);

        __pid_t pid = getpid();
        stats->lastProcessInStats = pid;
        printf(" Process %d\n", stats->lastProcessInStats);
        if(stats->lastProcessInStats == stats->processToKill){
            exit(0);
        }
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
        int executionMode; // Total of spaces to write and read values (char)
        char *keyValueChar; // Key value to encription (char)
        
        keyValueChar = argv[2];
        shareMemoryName = argv[3];


        int keyValue; // Key value to encription
        keyValue = atoi(keyValueChar); // Cast from char to int


        if(strcmp(argv[1], "auto") == 0)
        {
            executionMode = 0;
        }

        else if(strcmp(argv[1], "manual") == 0)
        {
            executionMode = 1;
        }
        else
        {
            bold_red();
            printf("-------------------------------------------------------\n");
            printf("  Invalid execution mode for emitter (auto or manual)\n");
            printf("-------------------------------------------------------\n");
            return 0;
        }

        
        getStatsStruct();
        setSharedItems(shareMemoryName); // Creates shared items
        stringDataFromSharedMemory(); //Gets file string from shared memory
        emitterLogic(keyValue, executionMode);


        return 0;
    }
    return 0;

};