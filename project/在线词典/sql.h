#include <sqlite3.h>




int callback(void *data, int argc, char **argv, char **azColName);
sqlite3 *opendb();