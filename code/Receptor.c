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

void read_info (int key, char* shareMemoryName, char* mode){

    //Inicializamos semaforo de stats
    int shm_stats;
    shm_stats = shm_open("shareStats", O_CREAT | O_RDWR, 0666); // Shared memory for stats with id "shareStats"
    
    //Inicializamos estructura de los stats
    struct controlStats *stats;
    stats = mmap(0, sizeof(struct controlStats), PROT_READ | PROT_WRITE, MAP_SHARED, shm_stats, 0);

    //Se llama a la memoria compartida de la memoria circular
    int shm_array; //file descriptor of shared memory file
    void *sharedMemoryArray;
    shm_array = shm_open(shareMemoryName, O_CREAT | O_RDWR, 0666); // Shared memory for data
        
    


    //Semaforos
    //Se crea semaforos de stats
    char *semStatsName = "stats";
    sem_t *semStats; // Control to stats
    semStats = sem_open(semStatsName, O_CREAT, 0666, 1);    

    sem_wait(semStats);
    //Estructura de la memoria circular
    struct charQueue *charArray; // Map the shared memory segment in process address space
    charArray = mmap(0, sizeof(struct charQueue[stats->spacesToRead]), PROT_READ | PROT_WRITE, MAP_SHARED, shm_array, 0);

    //Instancia semaforo emisor
    char *semEmittersName = "filled";
    sem_t *semEmitters; // For emitters control
    semEmitters = sem_open(semEmittersName, O_CREAT, 0666, stats->spacesToRead);
    sem_post(semStats);

    //Instancia semaforo receptor
    char *semReceiversName = "empty";
    sem_t *semReceivers; // For receivers control
    semReceivers = sem_open(semReceiversName, O_CREAT, 0666, 0);

   
    // sharedText = mmap(0, sizeof(char[SizeOfTheStringInMemory]), PROT_READ | PROT_WRITE, MAP_SHARED, shm_text, 0);
    //sharedText = mmap(0, sizeof(char[100]), PROT_READ | PROT_WRITE, MAP_SHARED, shm_text, 0);
    // printf((char *) sharedText);
    //char *stringFromMemory = (char *) sharedText;
    unsigned ch1;
    int ch;
    int pos;
    char time_data[20];
    int estado_leido=0;
    
    for(int i = 0; i <= stats->inputTextSize; i++) // 100 => SizeOfTheStringInMemory
    {
        sem_wait(semReceivers);
        //imprimir datos 
        ch1 = charArray[estado_leido].charValue^key;
        ch = ch1;
        pos = charArray[estado_leido].index;
        time_data[20]=charArray[estado_leido].timeCreated;
        cyan();
        printf("Valor leido: %d en la posicion: %d", ch,pos);
        printf("\n");
        yellow();
        printf("En el tiempo: %c ", time_data);
        printf("\n");

        if(estado_leido== (stats->spacesToRead-1)){
            estado_leido=0;}
        else
            estado_leido++;

        sem_post(semEmitters);

        if (strcmp(mode, "manual") == 0)
        {
            char received_char = getchar();
            if (received_char == 'q')
            {
                mode = "auto";
            }
            
        }
    }

    printf("\n");
    //long num = Int64.Parse(key);
    unsigned num1 = 'a';
    int num2= 12;
    int num3= num1+num2;
    printf("results: %d",num3);
    int num4= num3-num1;
    printf("results2: %d",num4);
    //printf("llave: %d" ,key+4);
    printf("\n");
}

//main
int main(int argc, char *argv[])
{
    
    if(argv[1]== NULL | argv[2]== NULL | argv[3]== NULL){
        printf("Ingresar todos los parametros necesarios: mode, key, Name");
        printf("\n");
    }
    else{
        
            // Valida modo automatico o manual
        if(strcmp(argv[1], "auto") == 0 || strcmp(argv[1], "manual") == 0){
            char *a = argv[2];
            int num = atoi(a);
            read_info(getDecimal(num),argv[3],argv[1]);
        }
        else{
            printf("Indicar modo: manual o auto");
            printf("\n");
        }
    }
    

    return 0;
}