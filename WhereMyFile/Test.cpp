#include "Fuction.h"
#include "ScanfManager.h"
#include "DataManger.h"
#include <iostream>
#include <thread>

//void SqliteTest()
//{
//	SqliteManger sql;
//	sql.Open("test.db");
//	string create_tb = "create table tb_doc(id int primary key, doc_path text, doc_name text )";
//	sql.ExcuteSql(create_tb);
//	sql.Close();
//}
//void DirectoryListTest()
//{
//	vector<string> docs;
//	vector<string> file;
//	string path = "C:\\Users\\VSAS\\source\\repos\\WhereMyFile\\WhereMyFile";
//	DirectoryList(path, docs,file);
//	for (auto &e : docs) {
//		cout << e << endl;
//	}
//	for (auto &e : file) {
//		cout << e << endl;
//	}
//	if (file.empty())
//	{
//		cout << "2333333333333333333333" << endl;
//	}
//}
void DataManagerTest()
{
	DataManager dbm;
	dbm.Init(); 
	vector<std::pair<string, string>>  docinfos;
	dbm.Search("sql", docinfos);
}
void test()
{
	ScanfManager scanm, scanm1;
	//while (1)
	//{
		scanm.Scan("C:\\Users\\VSAS\\source\\repos\\WhereMyFile");
		//std::this_thread::sleep_for(std::chrono::seconds(5));
	//}
}
void ScanfManagerTest()
{
	

	
		std::thread thread_(test);
		thread_.join();

	//scanm.Scan("C:\\Users\\VSAS\\source\\repos\\WhereMyFile");
}
void Test()
{
	cout << "hahhahahaha" << endl;
}
void Testthread()
{
	std::thread T(Test);
	T.join();
}
//void TeststdChineseConvertPinYinAllSpell()
//{
//	cout << ChineseConvertPinYinAllSpell("fasdfad") << endl;
//	 ColourPrintf("fsadfasdf");
//	 cout << ChineseConvertPinYinInitials("fasdfasdf") << endl;
//
//}
void showcolourTest()
{
	string str("新建文本文档");
	showcolour(str, "wenben");
}
int main() {
	//SqliteTest();
	//DirectoryListTest();
	ScanfManagerTest();
	//DataManagerTest();
	//TeststdChineseConvertPinYinAllSpell();
	//showcolourTest();

	//Testthread();
	system("pause");
	return 0;

}