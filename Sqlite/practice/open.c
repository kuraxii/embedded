#include <sqlite3.h>
#include <stdio.h>

#define DB "database.db"

int main()
{
    sqlite3 *db;

    int ret = sqlite3_open(DB, &db);
    if(ret != SQLITE_OK)
    {
        printf("%s\n", sqlite3_errmsg(db));
        return -1;
    }
    printf("open success\n");
    sqlite3_close(db);

}