#pragma once
#include "Fuction.h"
#include "DataManger.h"
#include <set>

class ScanfManager
{
public:
	ScanfManager()
	{
		_datamgr.Init();
	}
	void Scan(const string& path);
	void clear()
	{
		_datamgr.Clear();
	}

private:
	DataManager _datamgr;
};


