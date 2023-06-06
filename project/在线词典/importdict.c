#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#define DATABASE "database.db"

int insert(sqlite3 *db, char *buf, int len)
{
    char word[18] = {'\0'};
    char explain[200] = {'\0'};
    char sql[300] = {'\0'};
    char *errmsg;
    int word_t = 0;
    while (word_t < 17)
    {
        if ((*buf >= 'a' && *buf <= 'z') || (*buf >= 'A' && *buf <= 'Z') || *buf == '-')
        {
            word[word_t] = *buf;
        }

        word_t++;
        buf++;
    }

    int i = 0;
    sprintf(explain, "%s", buf);
    explain[strlen(explain)-1] = '\0';


    sprintf(sql, "insert into dict values(\"%s\",\"%s\");", word, explain);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        printf("=====%s\n==%s\n========%s\n", word, explain, errmsg);
        return -1;
    }
  
}

int main()
{
    sqlite3* db;

    if(sqlite3_open(DATABASE, &db) != SQLITE_OK)
	{
		printf("%s\n", sqlite3_errmsg(db));
		return -1;
	}
	else 
	{
		printf("Create or open success.\n");
	}



    char buf[1024] = {'\0'};
    FILE *p = fopen("./dict.txt", "r");

    while (!feof(p))
    {
        fgets(buf, sizeof(buf), p);
        insert(db, buf, strlen(buf));
    }
}