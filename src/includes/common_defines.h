
#define GROUPSIZE 10
#define PORT 4006
#define MAX_ARRAY_ELEMENTS 100000
#define MAX_SIZE_OF_ONE_ENTRY 5
#define MAXMSG (MAX_ARRAY_ELEMENTS * MAX_SIZE_OF_ONE_ENTRY)

typedef enum _operation
{
    JOIN = 1,
    DELETE = 2,
    RESULT = 3
}OPERATION;


