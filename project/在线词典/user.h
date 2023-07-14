#include <sqlite3.h>


typedef struct
{
    char id[20];
    char passwd[20];
} USERLOG;

void login(sqlite3* db);
void _register(sqlite3* db);