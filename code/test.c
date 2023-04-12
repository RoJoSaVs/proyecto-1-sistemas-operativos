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


int main()
{
    int shm_stats;
    void *sharedMemoryStats;
    shm_stats = shm_open("shareStats", O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"

    struct controlStats *stats;
    stats = mmap(0, sizeof(struct controlStats), PROT_READ | PROT_WRITE, MAP_SHARED, shm_stats, 0);


    // printf("%d\n", stats->inputTextSize);

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

    // printf((char *) sharedText);
    char *stringFromMemory = (char *) sharedText;

    char ch;

    for(int i = 0; i <= stats->inputTextSize; i++) // 100 => SizeOfTheStringInMemory
    {
        ch = stringFromMemory[i];
        printf("%c", ch);
    }

    printf("\n");

    return 0;
}