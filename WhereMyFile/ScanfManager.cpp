#include "Fuction.h"
#include "ScanfManager.h"
#include <threadpoolapiset.h>
void ScanfManager::Scan(const string& path) {
	vector<string> HomeDirs;
	vector<string> HomeFiles;
	std::set<string> HomeSet;
	std::set<string> DBSet;
	DirectoryList(path, HomeDirs, HomeFiles);	
	HomeSet.insert(HomeDirs.begin(), HomeDirs.end());
	HomeSet.insert(HomeFiles.begin(), HomeFiles.end());
	//数据库中的数据	
	DataManager _dmger;
	_dmger.GetDoc(path,DBSet);
	auto HomeIt = HomeSet.begin();
	auto DBit = DBSet.begin();
	while (HomeIt != HomeSet.end() && DBit != DBSet.end())
	{
		
	   if (*HomeIt < *DBit)
		{
			_datamgr.InsertDoc(path,*HomeIt);
			HomeIt++;
		}
		else if (*HomeIt > *DBit)
		{
			//删除数据库数据
			_datamgr.DeleDoc(path, *DBit);
			DBit++;
		}
		else 
	   {
		   ++HomeIt;
		   ++DBit;
	   }
	}
	while (HomeIt != HomeSet.end())
	{
		_datamgr.InsertDoc(path, *HomeIt);
		HomeIt++;
	}
	while (DBit != DBSet.end())
	{
		_datamgr.DeleDoc(path, *DBit);
		DBit++;
	}
	//递归比对子目录
	for (const auto& AllFile : HomeDirs)
	{
		string _path = path + "\\" + AllFile;
		Scan(_path);	
	}

	
}