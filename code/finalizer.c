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

/*
int main(int argc, char *argv[]) {

    // Open shared memory segment
    int shm_id = shm_open("shareStats", O_RDONLY, 0666);

    // Map shared memory into the process
    struct controlStats *stats = mmap(NULL, sizeof(struct controlStats), PROT_READ, MAP_SHARED, shm_id, 0);

    // Access memory data
    int memory_used = stats->memoryUsed;

    printf("Memory used: %d\n", memory_used);

    // Release the resources used by the shared memory segment and unmap it from the process address space
    close(shm_id);
    munmap(stats, sizeof(struct controlStats));

}
 */

#include <SDL.h>



int main(int argc, char* argv[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Check if there is at least one game controller available
    if (SDL_NumJoysticks() < 1) {
        SDL_Log("No game controller detected");
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
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_CONTROLLERBUTTONDOWN:
                    SDL_Log("Button %d down", event.cbutton.button);
                    break;
                case SDL_CONTROLLERBUTTONUP:
                    SDL_Log("Button %d up", event.cbutton.button);
                    break;
                case SDL_CONTROLLERAXISMOTION:
                    if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
                        SDL_Log("Left stick X-axis motion: %d", event.caxis.value);
                    }
                    else if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
                        SDL_Log("Left stick Y-axis motion: %d", event.caxis.value);
                    }
                    break;
            }
        }
    }

    // Close the game controller and quit SDL
    SDL_GameControllerClose(controller);
    SDL_Quit();
    return 0;
}
