#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define  DATABASE  "student.db"
#define  N  128

int do_insert(sqlite3 *db)
{
	int id;
	char name[32] = {};
	char sex;
	int score;
	char sql[N] = {};
	char *errmsg;

	printf("Input id:");
	scanf("%d", &id);

	printf("Input name:");
	scanf("%s", name);
	getchar();

	printf("Input sex:");
	scanf("%c", &sex);

	printf("Input score:");
	scanf("%d", &score);

	sprintf(sql, "insert into stu values(%d, '%s', '%c', %d)", id, name, sex, score);

	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("Insert done.\n");
	}

	return 0;
}
int do_delete(sqlite3 *db)
{
	int id;
	char sql[N] = {};
	char *errmsg;

	printf("Input id:");
	scanf("%d", &id);

	sprintf(sql, "delete from stu where id = %d", id);

	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("Delete done.\n");
	}

	return 0;
}
int do_update(sqlite3 *db)
{
	int id;
	char sql[N] = {};
	char name[32] = "zhangsan";
	char *errmsg;

	printf("Input id:");
	scanf("%d", &id);

	sprintf(sql, "update stu set name='%s' where id=%d", name,id);

	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("update done.\n");
	}

	return 0;
}


int callback(void *arg, int f_num, char ** f_value, char ** f_name)
{
	int i = 0;

	for(i = 0; i < f_num; i++)
	{
	//	printf("%-8s %s", f_value[i], f_name[i]);
		printf("%-8s", f_value[i]);
	}

	printf("++++++++++++++++++++++");
	putchar(10);

	return 0;
}

int do_query(sqlite3 *db)
{
	char *errmsg;
	char sql[N] = "select count(*) from stu where name='zhangsan';";

	if(sqlite3_exec(db, sql, callback,NULL , &errmsg) != SQLITE_OK)
	{
		printf("%s", errmsg);
	}
	else
	{
		printf("select done.\n");
	}
}

int do_query1(sqlite3 *db)
{
	char *errmsg;
	char ** resultp;
	int nrow;
	int ncolumn;

	if(sqlite3_get_table(db, "select * from stu", &resultp, &nrow, &ncolumn, &errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
		return -1;
	}
	else
	{
		printf("query done.\n");
	}

	int i = 0;
	int j = 0;
	int index = ncolumn;

	for(j = 0; j < ncolumn; j++)
	{
		printf("%-10s ", resultp[j]);
	}
	putchar(10);

	for(i = 0; i < nrow; i++)
	{
		for(j = 0; j < ncolumn; j++)
		{
			printf("%-10s ", resultp[index++]);
		}
		putchar(10);
	}

return 0;
}

int main(int argc, const char *argv[])
{
	sqlite3 *db;
	char *errmsg;
	int n;
	
	if(sqlite3_open(DATABASE, &db) != SQLITE_OK)
	{
		printf("%s\n", sqlite3_errmsg(db));
		return -1;
	}
	else
	{
		printf("open DATABASE success.\n");
	}

	if(sqlite3_exec(db, "create table if not exists stu(id int, name char , sex char , score int);",
				NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("%s\n", errmsg);
	}
	else
	{
		printf("Create or open table success.\n");
	}

	while(1)
	{
		printf("********************************************\n");
		printf("1: insert  2:query  3:delete 4:update 5:quit\n");
		printf("********************************************\n");
		printf("Please select:");
		scanf("%d", &n);

		switch(n)
		{
			case 1:
				do_insert(db);
				break;
			case 2:
				do_query(db);
			//	do_query1(db);
				break;
			case 3:
				do_delete(db);
				break;
			case 4:
				do_update(db);
				break;
			case 5:
				printf("main exit.\n");
				sqlite3_close(db);
				exit(0);
				break;
			default :
				printf("Invalid data n.\n");
		}

	}




	return 0;
}
