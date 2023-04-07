typedef struct charQueue
{
    pid_t processId;
    int index;
    char timeCreated[20];
    unsigned charValue;
} charQueue;


typedef struct controlStats
{
    unsigned long memoryUsed;
    int keyValue;
    int totalEmitters;
    int totalReceivers;
    int emittersAlive;
    int receiversAlive;
    int valuesReaded;
    int valuesInMemory;
} controlStats;



typedef struct semaphoresInfo
{
    sem_t semFilled;
    sem_t semEmpty;
    sem_t semStats;
} semaphoresInfo;