#include "dbManager.h"
#include"sqlite3.h"
#include <iostream>
#include<string>
#include"RDConstant.h"

using namespace std;

const string cstr_file_db = "./app/"+Dice_name+"/dice.db";

dbManager * dbManager::instance = nullptr;

sqlite3 * dbManager::getDatabase()
{
	return (dbManager::instance)->database;
}

dbManager * dbManager::getInstance()
{
	return dbManager::instance;
}

dbManager::dbManager()
{
	isDatabaseReady = false;
	int i_ret_code = sqlite3_open(cstr_file_db.c_str(), &database);
	if (i_ret_code == SQLITE_OK) 
	{
		if (dbManager::instance != nullptr) 
		{
			free(dbManager::instance);
			dbManager::instance = this;
		}
		dbManager::instance = this;
		isDatabaseReady = true;
	}
}

dbManager::~dbManager()
{
	if (dbManager::instance == this) dbManager::instance = nullptr;
	sqlite3_close(this->database);
}

int dbManager::registerTable(std::string str_table_name, std::string str_table_sql)
{
	bool isExist;
	int i_ret_code = isTableExist(str_table_name, isExist);
	if (i_ret_code == SQLITE_OK && !isExist) {
		const char * sql_command = str_table_sql.c_str();
		char * pchar_err_message = nullptr;
		int i_ret_code_2 = sqlite3_exec(this->database, sql_command, &sqlite3_callback, (void*)&i_data_database_create, &pchar_err_message);
		if (i_ret_code_2 != SQLITE_OK)
		{
#ifdef _DEBUG
			std::cout << pchar_err_message;
#endif // _DEBUG
			isDatabaseReady = true;
			return i_ret_code_2;
		}
		else {
			isDatabaseReady = false;
			return i_ret_code_2;
		}
	}
	else return i_ret_code;
}

inline int dbManager::isTableExist(const std::string & table_name, bool & isExist)
{
	std::string sql_command = "select count(*) from sqlite_master where type ='table' and name ='" + table_name + "'";
	char * pchar_err_message = nullptr;
	int i_count_of_table = 0;
	int ret_code = sqlite3_exec(this->database, sql_command.c_str(), &sqlite3_callback_isTableExist, (void*)&i_count_of_table, &pchar_err_message);
	if (ret_code == SQLITE_OK) {
		isExist = i_count_of_table > 0;
		return ret_code;
	}
#ifdef _DEBUG
	else {
		std::cout << sqlite3_errmsg(database);
	}
#endif
	isExist = false;
	return ret_code;
}

int dbManager::sqlite3_callback(void * data, int argc, char ** argv, char ** azColName)
{
	int i_database_op = *((int*)data);
	switch (i_database_op)
	{
	case i_data_database_create:
		return SQLITE_OK;
	case i_data_database_update:
		return SQLITE_OK;
	default:
		return SQLITE_ABORT;
	}
}

int dbManager::sqlite3_callback_isTableExist(void * data, int argc, char ** argv, char ** azColName)
{
	int * i_handle = (int*)data;
	if (argc == 1) {
		*i_handle = atoi(argv[0]);
	}
	return SQLITE_OK;
}

