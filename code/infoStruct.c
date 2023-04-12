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
    int spacesToRead;
    int inputTextSize;
    int totalEmitters;
    int totalReceivers;
    int emittersAlive;
    int receiversAlive;
    int valuesReaded;
    int valuesInMemory;
} controlStats;