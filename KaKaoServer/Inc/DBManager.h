#pragma once
#include "ServLibrary.h"
#include <mysql.h>

#include "Singleton.h"
class DBManager
{
	friend Singleton<DBManager>;
private:
	std::recursive_mutex m_rm;

private:
	bool isConnect() { return isconnect; }
	bool CheckQueryErr(const char* query);
public:
	int InitDBManager();
	void CloseDBManager();
	int ProcessQuery(const char* query, ...);
	MYSQL_ROW GetsqlRow()const { return sql_row; }
	MYSQL_RES* GetsqlRes()const;
private:
	MYSQL* connection;
	MYSQL conn;
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;
	bool isconnect;
public:

private:
};