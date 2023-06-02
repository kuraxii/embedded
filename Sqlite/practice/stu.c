#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <unistd.h>

#define  DATABASE   "student.db"

int do_insert(sqlite3 *db)
{
	char sql[128] = {};
	int id;
	char name[20];
	float score;
	char *errmsg;

	printf("Input id:");
	scanf("%d", &id);
	getchar();

	printf("Input name:");
	scanf("%s", name);
	getchar();

	printf("Input score:");
	scanf("%f", &score);
	getchar();

	sprintf(sql,"insert into stuinfo values(%d,'%s',%f);", id, name, score);

	if(sqlite3_exec(db, sql,NULL ,NULL ,&errmsg) != SQLITE_OK)
	{
		printf("%s,%d --> %s\n", __func__, __LINE__, errmsg);
		return -1;
	}
	else
	{
		printf("Insert done.\n");
	}

	return 0;
}

int callback(void* arg,int ncolumns ,char** f_value,char** f_name)
{
	int i = 0;
	for(i = 0; i < ncolumns; i++)
	{
		printf("%-13s ", f_value[i]);
	}
	putchar(10);

	puts("++++++++++++++++");

	return 0;
}

int do_query(sqlite3 *db)
{
	char sql[128] = {};
	int id;
	char *errmsg;

#if 0
	printf("Input id:");
	scanf("%d", &id);
	getchar();

	sprintf(sql,"select * from stuinfo where id = %d;", id);
#endif
#if 1
	sprintf(sql,"select * from stuinfo;");
#endif



	if(sqlite3_exec(db, sql,callback ,NULL ,&errmsg) != SQLITE_OK)
	{
		printf("%s,%d --> %s\n", __func__, __LINE__, errmsg);
		return -1;
	}
	else
	{
		printf("Query done.\n");
	}

	return 0;
}


// gcc stu.c -o stu -lsqlite3

int main(int argc, const char *argv[])
{
	sqlite3 * db;
	char *errmsg;
	int n = 0;
	
	if(sqlite3_open(DATABASE, &db) != SQLITE_OK)
	{
		printf("%s\n", sqlite3_errmsg(db));
		return -1;
	}
	else 
	{
		printf("Create or open success.\n");
	}

	if(sqlite3_exec(db, "create table if not exists stuinfo (id integer, name vchar(20) not null,score float);",\
				NULL, NULL,&errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	
	while(1)
	{
		printf("*******************************************************\n");
		printf("1,insert  2,delete  3,select  4,update 5,quit         \n");
		printf("*******************************************************\n");

		printf("Input:");

		scanf("%d",&n);
		switch(n)
		{
		case 1:
			do_insert(db);
			break;
		case 2:
		//	do_delete();
			break;
		case 3:
		//	do_query1(db);
			do_query(db);
			break;
		case 4:
		//	do_update();
			break;
		case 5:
			sqlite3_close(db);
			exit(0);
			break;
		default:
			printf("Invalid cmd.\n");

		}

	}
	
	return 0;
}
