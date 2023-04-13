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

// Access memory data
void displayStats(struct controlStats *pStats) {
    yellow();
    printf("---------------------------------------------------------------\n");
    bold_green();
    printf(" Memory used %ld\n", pStats->memoryUsed);
    printf(" Emitters Alive %d\n", pStats->emittersAlive);
    printf(" Receivers Alive %d\n", pStats->receiversAlive);
    printf(" Total Emmiters %d\n", pStats->totalEmitters);
    printf(" Total Receivers %d\n", pStats->totalReceivers);
    printf(" Amount of characters in shared memory %d\n", pStats->inputTextSize);
    printf(" Values in memory %d\n", pStats->valuesInMemory);
    yellow();
    printf("---------------------------------------------------------------\n");
    reset();
}

int main(int argc, char *argv[]) {

    // Open shared memory segment
    int shm_id = shm_open("shareStats", O_RDONLY, 0666);

    // Map shared memory into the process
    struct controlStats *stats = mmap(NULL, sizeof(struct controlStats), PROT_READ, MAP_SHARED, shm_id, 0);

    // Access memory data
    displayStats(stats);

    // Release the resources used by the shared memory segment and unmap it from the process address space
    close(shm_id);
    munmap(stats, sizeof(struct controlStats));

    // Access memory data
    displayStats(stats);

}

/*
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
                    displayStats(stats);
                    break;
            }
        }
    }

    // Close the game controller and quit SDL
    SDL_GameControllerClose(controller);
    SDL_Quit();
    return 0;
}
*/

