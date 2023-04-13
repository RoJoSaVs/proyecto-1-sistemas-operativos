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



void setSemaphores(int spacesToRead)
{
    // Semaphore for stats struct access
    char *semStatsName = "stats";
    semStats = sem_open(semStatsName, O_CREAT | O_RDWR, 0666, 1);
    

    // Semaphore for array in shared memory (Receiver)
    char *semReceiversName = "empty";
    semReceivers = sem_open(semReceiversName, O_CREAT, 0666, 0);


    // Semaphore for array in shared memory (Emitter)
    char *semEmittersName = "filled";
    semEmitters = sem_open(semEmittersName, O_CREAT | O_RDWR, 0666, spacesToRead);

}

void setSharedMemory(int spacesToRead)
{
    // Stats struct shared memory
    int shm_stats;
    shm_stats = shm_open("shareStats", O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"
    stats = mmap(0, sizeof(struct controlStats), PROT_READ | PROT_WRITE, MAP_SHARED, shm_stats, 0);


    int shm_array; //file descriptor of shared memory file
    shm_array = shm_open("CE", O_CREAT | O_RDWR, 0666); // Shared memory for data
    charArray = mmap(0, sizeof(struct charQueue[spacesToRead]), PROT_READ | PROT_WRITE, MAP_SHARED, shm_array, 0);
}

int main()
{
    int spacesToRead = 10;
    int key = 5;
    setSemaphores(spacesToRead);
    setSharedMemory(spacesToRead);

    for(int i = 0; i < spacesToRead; i++){
        printf("%d -- %d", i, charArray[i].index);
        printf(" ");
        char ch = charArray[i].charValue ^ key;
        printf("%c", ch);
        printf("\n");
        sem_post(semEmitters);
    }




}