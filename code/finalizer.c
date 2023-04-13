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

#include <SDL.h>
#include <signal.h>

struct controlStats *stats;
struct charQueue *charArray; // Map the shared memory segment in process address space

void terminate(struct controlStats *pStats, int pid){
    pStats->processToKill = pid;


}

// Access memory data
void displayStats(struct controlStats *pStats) {
    yellow();
    printf("---------------------------------------------------------------\n");
    bold_green();
    printf("|%-50s|%-10lu|\n", "Memory used", pStats->memoryUsed);
    printf("|%-50s|%-10d|\n", "Emitters Alive", pStats->emittersAlive);
    printf("|%-50s|%-10d|\n", "Receivers Alive", pStats->receiversAlive);
    printf("|%-50s|%-10d|\n", "Total Emmiters", pStats->totalEmitters);
    printf("|%-50s|%-10d|\n", "Total Receivers", pStats->totalReceivers);
    printf("|%-50s|%-10d|\n", "Amount of characters in shared memory", pStats->inputTextSize);
    printf("|%-50s|%-10d|\n", "Values in memory", pStats->valuesInMemory);
    yellow();
    printf("---------------------------------------------------------------\n");
    reset();
}

int finalizerLogic() {
    char *shareMemoryName;
    shareMemoryName = "CE";

    int shm_stats;
    shm_stats = shm_open("shareStats", O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"
    stats = mmap(0, sizeof(struct controlStats), PROT_READ | PROT_WRITE, MAP_SHARED, shm_stats, 0);


    // Find and terminate emitters
    char command[256];
    sprintf(command, "pgrep emitter");

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen() failed");
        exit(1);
    }

    char pid_str[256];
    while (fgets(pid_str, 256, fp) != NULL) {
        int pid = atoi(pid_str);

        printf("|%-50s|%-10d|\n", "Matando proceso emisor", pid);
        while(stats->killDone==0){

            terminate(stats, pid);
        }
        stats->killDone=0;
   }

    pclose(fp);

    // Find and terminate receivers
    char command2[256];
    sprintf(command2, "pgrep receiver");

    FILE *fp2 = popen(command2, "r");
    if (fp2 == NULL) {
        perror("popen() failed");
        exit(1);
    }

    char pid_str2[256];
    while (fgets(pid_str2, 256, fp) != NULL) {
        int pid2 = atoi(pid_str2);
        printf("|%-50s|%-10d|\n", "Matando proceso receptor", pid2);
        while (stats->killDone == 0) {

            terminate(stats, pid2);
        }
        stats->killDone = 0;
    }
    pclose(fp2);

    // Access memory data
    displayStats(stats);


    //Release the shared info from the .txt
    int shm_text; //file descriptor of shared memory file
    void *sharedText;
    char *sharedTextName = "textMemory";
    shm_text = shm_open(sharedTextName, O_CREAT | O_RDWR, 0666); // Shared memory for data
    sharedText = mmap(0, sizeof(char *), PROT_READ | PROT_WRITE, MAP_SHARED, shm_text, 0);
    munmap(sharedText, sizeof(char *));
    shm_unlink(sharedTextName);


    // Release semaphores for critical region
    char *semEmittersName = "filled";
    sem_t *semEmitters; // For emitters control
    semEmitters = sem_open(semEmittersName, O_CREAT, 0666, 0);
    sem_close(semEmitters);
    sem_unlink(semEmittersName);


    char *semReceiversName = "empty";
    sem_t *semReceivers; // For receivers control
    semReceivers = sem_open(semReceiversName, O_CREAT, 0666, 0);
    sem_close(semReceivers);
    sem_unlink(semReceiversName);

    char *semStatsName = "stats";
    sem_t *semStats; // Control to stats
    semStats = sem_open(semStatsName, O_CREAT, 0666, 1);
    sem_close(semStats);
    sem_unlink(semStatsName);

    // Release shared memory for array
    int shm_array; //file descriptor of shared memory file

    shm_array = shm_open(shareMemoryName, O_CREAT | O_RDWR, 0666); // Shared memory for data

    charArray = mmap(0, stats->spacesToRead, PROT_READ | PROT_WRITE, MAP_SHARED, shm_array, 0);
    munmap(charArray, sizeof(char *));

    shm_unlink(shareMemoryName);

    // Release the resources used by the shared memory segment and unmap it from the process address space
    munmap(stats, sizeof(struct controlStats));
    shm_unlink("shareStats");
}


int main(int argc, char* argv[])
{
    // Open shared memory segment
    int shm_id = shm_open("shareStats", O_RDONLY, 0666);
    // Map shared memory into the process
    struct controlStats *stats = mmap(NULL, sizeof(struct controlStats), PROT_READ, MAP_SHARED, shm_id, 0);

    // Initialize SDL
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Check if there is at least one game controller available
    if (SDL_NumJoysticks() < 1) {
        SDL_Log("No game controller detected, please connect a controller");
        return 1;
    }

    // Open the first available game controller
    SDL_GameController* controller = SDL_GameControllerOpen(0);
    if (controller == NULL) {
        SDL_Log("Unable to open game controller: %s", SDL_GetError());
        return 1;
    }

    // Keep reading controller input until the user quits the program
    SDL_Event event;
    int quit = 0;
    while (!quit) {
        // Poll for events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                default:
                    break;
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_CONTROLLERBUTTONUP:
                    SDL_Log("Button %d up", event.cbutton.button);
                    finalizerLogic();
                    quit = 1;
                    break;
            }
        }
    }

    // Close the game controller and quit SDL
    SDL_GameControllerClose(controller);
    SDL_Quit();
    return 0;
}


