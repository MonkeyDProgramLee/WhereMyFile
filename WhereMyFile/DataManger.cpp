#define _CRT_SECURE_NO_WARNINGS
#include "DataManger.h"
#include "Fuction.h"
std::shared_mutex mutex_;
SqliteManger::CGarbo gc;
SqliteManger* SqliteManger::inst = nullptr;
std::mutex SqliteManger::_mutex;

void SqliteManger::Open(const string& path) 
{
	int ret = sqlite3_open(path.c_str(),&_db);
	if (ret != SQLITE_OK)
	{
		ERROE_LOG("sqlite3_open");
	}
	else
	{
		TRACE_LOG("sqlite3_open succese");
	}
}
void SqliteManger::Close()
{
	int ret = sqlite3_close(_db);
	if (ret != SQLITE_OK)
	{
		ERROE_LOG("sqlite3_close");
	}
	else
	{
		TRACE_LOG("sqlite3_close succese");
	}
}
int SqliteManger::ExcuteSql(const string& sql) 
{
	char* errmsg;
	int ret = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errmsg);
	if (ret != SQLITE_OK)
	{
		ERROE_LOG("sqlite3_exec(%s)",sql.c_str());
		return ret;
	}
	else
	{
		TRACE_LOG("sqlite3_exec succese");
		return ret;
	}
	return ret;
}
void SqliteManger::GetTable(const string& sql, int& row, int& col, char**& Ppret)
{
	char* errmsg;
	int ret = sqlite3_get_table(_db, sql.c_str(),&Ppret , &row, &col, &errmsg);
	if (ret != SQLITE_OK)
	{
		ERROE_LOG("sqlite3_get_table(%s)", sql.c_str());
	}
	else
	{
		TRACE_LOG("sqlite3_exec succese");
	}
}
void DataManager::Init()
{
	std::unique_lock<std::shared_mutex> WriteLock(mutex_);
	_dbmgr->Open(DB_NAME);
	char sql[1024];
	sprintf(sql,"create table if not exists %s (path text, name text, name_pinyin text, name_initials text)", TB_NAME);
	
	_dbmgr->ExcuteSql(sql);
}
void DataManager::GetDoc(const string& path, std::set<string>& DBSet)
{
	char sql[1024];
	sprintf(sql, "select name from %s where path = '%s'", TB_NAME, path.c_str());
	int row, col;
	char** ppRet;
	std::shared_lock< std::shared_mutex> ReadLock(mutex_);
	AutoTable agt( *_dbmgr, sql, row, col, ppRet);
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			DBSet.insert(ppRet[i*col + j]);
		}
	}
}
int DataManager::Insert(const string& path, const string& name)
{
	//插入数据
	char sql_insert[1024];
	string AllPinYin = ChineseConvertPinYinAllSpell(name);
	string PinYinInitials = ChineseConvertPinYinInitials(name);
	sprintf(sql_insert,"insert into %s(path,name,name_pinyin,name_initials) values('%s', '%s', '%s', '%s');",TB_NAME,path.c_str(),name.c_str(), AllPinYin.c_str(), PinYinInitials.c_str());
	std::unique_lock<std::shared_mutex> WriteLock(mutex_);
	return _dbmgr->ExcuteSql(sql_insert);
}
int DataManager::Delete(const string& path, const string& name)
{
	char sql[1024];
	string path_ = path;
	sprintf(sql, "delete from %s where path like '%%%s%%' and name = '%%%s%%'", TB_NAME, path_.c_str(), name.c_str());
	std::unique_lock<std::shared_mutex> WriteLock(mutex_);
	return _dbmgr->ExcuteSql(sql);
}
void DataManager::InsertDoc(const string& path, const string& name)
{
	if (InsertRecord.size() < 1)
	{
		InsertRecord.push_back(std::make_pair(path, name));
	}
	else
	{
		EventInsert();
	}
}
void DataManager::DeleDoc(const string& path, const string& name)
{
	if (DelRecord.size()<100)
	{
		DelRecord.push_back(std::make_pair(path, name));
	}
	else
	{
		EventDel();
	}
}
void DataManager::Index()
{
	std::stringstream sql;
	sql << "create index key on "<< TB_NAME<< "(path);" ;
	std::unique_lock<std::shared_mutex> WriteLock(mutex_);
	_dbmgr->ExcuteSql(sql.str());
}
void DataManager::EventInsert()
{
	SqliteManger::Getinstance()->ExcuteSql("begin;");
	bool isExecSuccess = true;
	int rc;
	for (const auto&e : InsertRecord)
	{
		int rc = Insert(e.first, e.second);
		//判断是不是插入成功
		if (rc != SQLITE_OK)
		{
			isExecSuccess = false;
			ERROE_LOG("SQL ERROR \n");
		}
	}
	//插入完之后清空
	InsertRecord.clear();
	if (isExecSuccess)
	{
		SqliteManger::Getinstance()->ExcuteSql("commit;");
	}
	else
	{
		SqliteManger::Getinstance()->ExcuteSql("rollback;");
	}
}
void DataManager::Clear()
{
	if (!InsertRecord.empty())
	{
		EventInsert();
	}
	InsertRecord.clear();
	if (!DelRecord.empty())
	{
		EventDel();
	}
	InsertRecord.clear();
	DelRecord.clear();
}
void DataManager::EventDel() 
{
	SqliteManger::Getinstance()->ExcuteSql("begin;");
	bool isExecSuccess = true;
	int rc;
	for (const auto&e : DelRecord)
	{
		int rc = Delete(e.first, e.second);
		if (rc != SQLITE_OK)
		{
			isExecSuccess = false;
			ERROE_LOG("SQL ERROR \n");
		}
	}
	DelRecord.clear();
	if (isExecSuccess)
	{
		SqliteManger::Getinstance()->ExcuteSql("commit;");
	}
	else
	{
		SqliteManger::Getinstance()->ExcuteSql("rollback;");
		ERROE_LOG("Rollback");
	}
}
void DataManager::Search(const string& key, vector<std::pair<string, string>>&  docinfos)
{
	//Index();
	char sql[1024];
	sprintf(sql, "select name, path from %s where name like '%%%s%%' or name_pinyin like '%%%s%%' or name_initials like '%%%s%%' ", TB_NAME, key.c_str(), key.c_str(), key.c_str());
	//std::shared_lock< std::shared_mutex> ReadLock(mutex_);
	//_dbmgr.ExcuteSql(sql);
	int row, col;
	char** ppRet;
	std::shared_lock< std::shared_mutex> ReadLock(mutex_);
	AutoTable agt_name(*_dbmgr, sql, row, col, ppRet);
	ReadLock.unlock();
	int index = col;
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; j+=col)
		{
			docinfos.push_back(std::make_pair(ppRet[index], ppRet[index+1]));
			index += col;
		}
	}
	ShowOut(key, docinfos);
}