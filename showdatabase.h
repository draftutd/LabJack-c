// MySqlTest.cpp : Defines the entry point for the console application.
//

#include <mysql.h>
#include <iostream>

using namespace std;
int qstate;

int main()
{
	MYSQL* conn;
	MYSQL_ROW row;
	MYSQL_RES* res;
	conn = mysql_init(0);

	conn = mysql_real_connect(conn, "localhost", "root", "0857351137", "data_all", 3306, NULL, 0);

	if (conn) {
		puts("Successful connection to database!");

		string query = "SELECT * FROM data_log";
		const char* q = query.c_str();
		qstate = mysql_query(conn, q);
		if (!qstate)
		{
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res))
			{
				printf("ID: %s, Date: %s, Time: %s, Data1: %s, Data2: %s, Data3: %s\n", row[0], row[1], row[2], row[3], row[4], row[5]);
			}
		}
		else
		{
			cout << "Query failed: " << mysql_error(conn) << endl;
		}
	}
	else {
		puts("Connection to database has failed!");
	}

	return 0;
}