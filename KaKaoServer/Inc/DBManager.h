#pragma once
#include "ServLibrary.h"
#include "DBInfo.h"

class DBManager
{
private:
	static DBManager* m_pInst;
	std::recursive_mutex m_rm;
public:
	static DBManager* GetInst()
	{
		if (m_pInst == nullptr)
			m_pInst = new DBManager();
		return m_pInst;
	}
private:
	bool isConnect() { return isconnect; }
	bool CheckQueryErr(const char* query);
public:
	int InitDBManager();
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
	DBManager()
	{
		isconnect = false;
		connection = nullptr;
		sql_result = nullptr;
	}
private:
	~DBManager()
	{
		if (connection != nullptr)
		{
			mysql_close(connection);
			isconnect = false;
		}
	}
};