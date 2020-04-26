#include "DBManager.h"
#pragma comment (lib,"libmySQL.lib")

DBManager* DBManager::m_pInst = nullptr;

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
	// �ѱ��� ����ϱ� ���� ����
	mysql_query(connection, "set session character_set_connection=euckr;");
	mysql_query(connection, "set session character_set_result=euckr;");
	mysql_query(connection, "set session character_set_client=euckr;");
	isconnect = true;
	return 0;
}

// TODO : DB : Write�� ���� ��ȯ���� �ǹ̴� ����, read�� ��ȯ���� ���� ����ü�� ����������.
int DBManager::ProcessQuery(const char * query)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);
	bool retErr = CheckQueryErr(query);
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
		LOG("�ʱ�ȭ���� ���� sql_result ���!");
		return nullptr;
	}
}