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



void errorMsg(){
    yellow();
    printf("--------------------------------------------------------\n");
    cyan();
    printf("Please insert the requested data\n");
    printf(" 1 - Buffer name | Example: CE\n");
    printf(" 2 - Letters to store at the same time | Example: 10\n");
    printf(" 3 - Key value to encript data | Example: 5\n");
    printf(" Example Command for execution: ./output/initializer CE 10 5 \n");
    yellow();
    printf("--------------------------------------------------------\n");
    reset();
}

// Set the initial values of the struct containing the stats
struct controlStats defaultValues(int keyValue)
{
    struct controlStats initialControlStats;

    initialControlStats.memoryUsed = 0;
    initialControlStats.keyValue = keyValue;
    initialControlStats.totalEmitters = 0;
    initialControlStats.totalReceivers = 0;
    initialControlStats.emittersAlive = 0;
    initialControlStats.receiversAlive = 0;
    initialControlStats.valuesReaded = 0;
    initialControlStats.valuesInMemory = 0;

    return initialControlStats;
}

int fileDataToSharedMemory()
{
    // =================================================================================== //
    // Reading content from file: https://www.tutorialkart.com/c-programming/c-read-text-file/
    FILE    *textfile;
    char    *text;
    long    numbytes;
    
    textfile = fopen("code/file.txt", "r");
    if(textfile == NULL)
    {
        return 1;
    }
    
    fseek(textfile, 0L, SEEK_END);
    numbytes = ftell(textfile);
    fseek(textfile, 0L, SEEK_SET);  

    text = (char*)calloc(numbytes, sizeof(char));   
    if(text == NULL)
    {
        return 1;
    }

    fread(text, sizeof(char), numbytes, textfile);
    fclose(textfile);

    // =================================================================================== //
    // Data from file to shared memory

    int shm_text; //file descriptor of shared memory file
    void *sharedText;
    char *sharedTextName = "textMemory";

    shm_text = shm_open(sharedTextName, O_CREAT | O_RDWR, 0666); // Shared memory for data

    ftruncate(shm_text, sizeof(char[numbytes])); // Configure the size of the shared memory block
    
    
    sharedText = mmap(0, sizeof(char[numbytes]), PROT_READ | PROT_WRITE, MAP_SHARED, shm_text, 0);
    strcpy(sharedText, text);

    sem_t *semSharedText;
    semSharedText = sem_open("textFromFile", O_CREAT, 0666, 1);
    
    return 0;
}


int main(int argc, char *argv[])
{
	if(argc != 4)
    {
        errorMsg();
	}
    
    else
    {
        int keepProcess = fileDataToSharedMemory();
        if(keepProcess != 0)
        {
            return 0;
        }
        // =================================================================================== //
        // Shared memory code section for processing
        char *shareMemoryName; // Name given to the shared memory space 
        char *spacesToReadChar; // Total of spaces to write and read values (char)
        char *keyValueChar; // Key value to encription (char)
        
        shareMemoryName = argv[1];
        spacesToReadChar = argv[2];
        keyValueChar = argv[3];
        
        int spacesToRead; // Total of spaces to write and read values
        int keyValue; // Key value to encription

        spacesToRead = atoi(spacesToReadChar);
        keyValue = atoi(keyValueChar);


        // =================================================================================== //
        // Info of shared memory for array
        int shm_array; //file descriptor of shared memory file
        void *sharedMemoryArray;
        shm_array = shm_open(shareMemoryName, O_CREAT | O_RDWR, 0666); // Shared memory for data

        ftruncate(shm_array, sizeof(struct charQueue[spacesToRead])); // Configure the size of the shared memory block
        
        struct charQueue *charArray; // Map the shared memory segment in process address space
        charArray = mmap(0, sizeof(struct charQueue[spacesToRead]), PROT_READ | PROT_WRITE, MAP_SHARED, shm_array, 0);


        // =================================================================================== //
        // Shared memory for stats
        int shm_stats;
        void *sharedMemoryStats;
        shm_stats = shm_open("shareStats", O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"

        ftruncate(shm_stats, sizeof(struct controlStats));

        struct controlStats *stats;
        stats = mmap(0, sizeof(struct controlStats), PROT_READ | PROT_WRITE, MAP_SHARED, shm_stats, 0);
        *stats = defaultValues(keyValue);

        // =================================================================================== //
        // Semaphores for critical region
        char *semEmittersName = "filled";
        sem_t *semEmitters; // For emitters control
        semEmitters = sem_open(semEmittersName, O_CREAT, 0666, spacesToRead);

        char *semReceiversName = "empty";
        sem_t *semReceivers; // For receivers control
        semReceivers = sem_open(semReceiversName, O_CREAT, 0666, 0);

        char *semStatsName = "stats";
        sem_t *semStats; // Control to stats
        semStats = sem_open(semStatsName, O_CREAT, 0666, 1);

        yellow();
        printf("---------------------------------------------------------------\n");
        bold_green();
        printf(" Buffer has been created with %d spaces and the key value is %d\n", spacesToRead, keyValue);
        yellow();
        printf("---------------------------------------------------------------\n");
        reset();
        
    }

	return 0;

};