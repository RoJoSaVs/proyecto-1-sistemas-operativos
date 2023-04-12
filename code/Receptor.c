//Librerias
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
//Variables Globales

//Funtion aux
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

void read_info (int key){
    int clave = 10101001; //llave para xor

    int shm_text; //file descriptor of shared memory file
    void *sharedText;
    char *sharedTextName = "textMemory";

    shm_text = shm_open(sharedTextName, O_CREAT | O_RDWR, 0666); // Shared memory for data
    void *sharedText;
    sharedText = mmap(0, sizeof(char *), PROT_READ | PROT_WRITE, MAP_SHARED, shm_text, 0);
    
    int shm_stats;
    shm_stats = shm_open("shareStats", O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"
    


    //Semaforos
    char *semEmittersName = "filled";
    sem_t *semEmitters; // For emitters control
    semEmitters = sem_open(semEmittersName, O_CREAT, 0666, 10);

    char *semReceiversName = "empty";
    sem_t *semReceivers; // For receivers control
    semReceivers = sem_open(semReceiversName, O_CREAT, 0666, 0);

    char *semStatsName = "stats";
    sem_t *semStats; // Control to stats
    semStats = sem_open(semStatsName, O_CREAT, 0666, 1);    

    if(shm_text == -1)
    {
        red();
        printf("Buffer doesn't exist\n");
        reset();
    }
    // sharedText = mmap(0, sizeof(char[SizeOfTheStringInMemory]), PROT_READ | PROT_WRITE, MAP_SHARED, shm_text, 0);
    sharedText = mmap(0, sizeof(char[100]), PROT_READ | PROT_WRITE, MAP_SHARED, shm_text, 0);
    // printf((char *) sharedText);
    char *stringFromMemory = (char *) sharedText;

    char ch;

    for(int i = 0; i <= 101; i++) // 100 => SizeOfTheStringInMemory
    {
        ch = stringFromMemory[i];
        printf("%c", ch);
    }

    printf("\n");
    //long num = Int64.Parse(key);
    printf("llave: %d" ,key+4);
}

//main
int main(int argc, char *argv[])
{
    

    // Valida modo automatico o manual
    if(strcmp(argv[1], "auto") == 0 || strcmp(argv[1], "manual") == 0){
        char *a = argv[2];
        int num = atoi(a);
        read_info(getDecimal(num));
    }
    else{
        printf("Indicar modo: manual o auto");
        printf("\n");
    }

    

    return 0;
}