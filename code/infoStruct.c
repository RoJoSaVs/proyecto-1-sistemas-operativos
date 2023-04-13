typedef struct charQueue
{
    // pid_t processId;
    int index;
    char timeCreated[100];
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

    int stringIndex; // Control the current value readed of the string loaded from the file
    int emitterIndex; // Place where emitters are writing
    int receiverIndex; // Place where receivers are reading
} controlStats;