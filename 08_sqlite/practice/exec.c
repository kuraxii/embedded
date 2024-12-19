#include <sqlite3.h>
#include <stdio.h>

#define DB "test.db"

int callback(void *args, int f_num, char **f_value, char **f_name)
{
    printf("f_num: %d\n", f_num);
    for(int i = 0; i < f_num; i++)
    {
        printf("%s:%s\n", f_name[i], f_value[i]);
    
    }
    puts("");
    return 0;
}

int main()
{
    sqlite3 *db = NULL;
    char* errmsg = NULL;
    int ret = sqlite3_open(DB, &db);
    if(ret != SQLITE_OK)
    {
        printf("%s\n", sqlite3_errmsg(db));
        return -1;
    }
    printf("open success\n");

    ret = sqlite3_exec(db, "select * from tab;", callback, NULL, &errmsg);
    if(ret != SQLITE_OK)
    {
        printf("%s\n", errmsg);
        return -1;
    }

    char **result = NULL;
    int row, col;

    ret = sqlite3_get_table(db, "select * from tab where id='321';", &result, &row, &col, &errmsg);
    if(ret != SQLITE_OK)
    {
        printf("%s\n", errmsg);
        return -1;
    }
    printf("%d %d\n", row, col);


    for(int i = 0; i <= row; i++)
    {
        for(int j = 0; j < col; j++)
        {
            printf("%s ", result[j+i*col]);
        }
        puts("");
    }



    sqlite3_close(db);



}

