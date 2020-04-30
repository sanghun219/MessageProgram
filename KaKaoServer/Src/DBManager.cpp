#include "DBManager.h"
#pragma comment (lib,"libmySQL.lib")

bool DBManager::CheckQueryErr(const char * query)
{
	if (connection == nullptr && !isConnect())
	{
		LOG("ExecuteQuery ERROR!. errno : %d  error : %s", mysql_errno(&conn), mysql_error(&conn));
		return false;
	}

	if (mysql_query(connection, query) != 0)
	{
		return false;
	}
	return true;
}
int DBManager::InitDBManager()
{
	isconnect = false;
	connection = nullptr;
	sql_result = nullptr;

	const char* host = "localhost";
	const char* user = "root";
	const char* pw = "Yongil1792!";
	const char* db = "kakaoserver";

	if (mysql_init(&conn) == nullptr)
	{
		LOG("mysql_init ERROR!");
		return -1;
	}

	connection = mysql_real_connect(&conn, host, user, pw, db, 3306, (const char*)nullptr, 0);
	if (connection == nullptr)
	{
		LOG("%d ERROR : %s, %d\n", mysql_errno(&conn), mysql_errno(&conn));
		return -1;
	}
	else
	{
		LOG("DB Connected!");
		if (mysql_select_db(&conn, db))
		{
			LOG("%d ERROR : %s, %d\n", mysql_errno(&conn), mysql_errno(&conn));
			return -1;
		}
	}
	// 한글을 출력하기 위한 과정
	mysql_query(connection, "set session character_set_connection=euckr;");
	mysql_query(connection, "set session character_set_result=euckr;");
	mysql_query(connection, "set session character_set_client=euckr;");
	isconnect = true;
	return 0;
}

void DBManager::CloseDBManager()
{
	if (connection != nullptr)
	{
		mysql_close(connection);
		isconnect = false;
	}
}

// TODO : DB : Write인 경우는 반환값이 의미는 없음, read는 반환값을 토대로 구조체를 만들어줘야함.
int DBManager::ProcessQuery(const char* query, ...)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);

	static char copyQuery[1024];
	va_list ap;
	va_start(ap, query);
	vsprintf_s(copyQuery + strlen(copyQuery), sizeof(copyQuery), query, ap);
	va_end(ap);
	bool retErr = CheckQueryErr(copyQuery);
	copyQuery[0] = { 0, };

	if (!retErr)
	{
		LOG("CheckQueryErr!");
		return -1;
	}
	else
	{
		sql_result = mysql_store_result(connection);
		if (sql_result)
		{
			LOG("sql query execute");
			return 0;
		}
		else
		{
			LOG("%s", mysql_error(&conn));
			return -1;
		}
	}
}

MYSQL_RES * DBManager::GetsqlRes() const
{
	if (sql_result != nullptr)
	{
		return sql_result;
	}
	else
	{
		LOG("초기화되지 않은 sql_result 사용!");
		return nullptr;
	}
}