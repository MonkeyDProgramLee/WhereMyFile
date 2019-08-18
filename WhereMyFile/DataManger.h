#pragma once
#include "Fuction.h"
#include <set>
#include ".\sqlite-amalgamation-3280000\sqlite3.h"

class SqliteManger
{
public:
	void Open(const string& path);
	void Close();
	int ExcuteSql(const string& sql);
	//查找
	void GetTable(const string& sql, int& row, int& col, char**& Ppret);
	sqlite3* Get()
	{
		return _db;
	}
	static SqliteManger* Getinstance()
	{
		if (inst == nullptr)
		{
			_mutex.lock();
			if (inst == nullptr)
			{
			 inst = new SqliteManger;
			}
		}
		return inst;
	}
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if(SqliteManger::inst)
			delete SqliteManger::inst;
		}
	};
	static CGarbo gc;
private:
	SqliteManger()
		:_db(nullptr)
	{}
	~SqliteManger() {
		Close();
	}
	SqliteManger(const SqliteManger&);
	SqliteManger& operator=(const SqliteManger&) = delete;
private:
	sqlite3* _db;
	static SqliteManger* inst;
	static std::mutex _mutex;
};
//RAII
class AutoTable
{
public:
	AutoTable(SqliteManger& sm, const string& sql, int& row, int& col, char**&Ppret) {
		sm.GetTable(sql, row, col, Ppret);
		_ppRet = Ppret;
	}
	~AutoTable() {
		sqlite3_free_table(_ppRet);
	}
	AutoTable(const AutoTable&) = delete;
	AutoTable operator=(const AutoTable&) = delete;
private:
	char** _ppRet;
};
//数据库管理模块
#define DB_NAME "doc.db"
#define TB_NAME "tb_doc"
class DataManager
{
public:
	void Init();//建表 连接 打开数据库
	void GetDoc(const string& path, std::set<string>& DBSet);//获取本地某个目录所有文件
	void InsertDoc(const string& path, const string& name);//插入数据
	void DeleDoc(const string& path, const string& name);//删除数据
	void Search(const string& key, std::vector<std::pair<string, string>>&  docinfos);
	void Clear();
	void Index();
private:
	int Insert(const string& path, const string& name);//插入数据
	int Delete(const string& path, const string& name);//删除数据
	void EventInsert();//事务
	void EventDel();//事务
private:
	SqliteManger* _dbmgr = SqliteManger::Getinstance();
	vector<std::pair<string, string>> InsertRecord;
	vector<std::pair<string, string>> DelRecord;
 };
