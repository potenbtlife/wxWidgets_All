#ifndef ASTMANAGER_CDBSQLITE_H
#define ASTMANAGER_CDBSQLITE_H

#include "sqlite3.h"
#include<string>
#include<iostream>

using namespace std;

const int RET_OK = 0; //执行成功返回值
const int RET_FAIL = -1; //执行失败返回值

class CDBSqlite
{
public:
	//缺省构造函数
	CDBSqlite();

	/**
	*@Description: 构造函数，初始化dbname成员变量
	*@param[IN] dbname：数据库名称
	*/
	CDBSqlite(string& dbname);

	/**
	*@Description: 构造函数，初始化两个成员变量
	*@param[IN] dbname：数据库名称
	*@parmm[IN] sql: 执行sql
	*/
	CDBSqlite(string& dbname, string& sql);

	//析构函数
	~CDBSqlite();

	/**
	*@Description: 打开sqlite3 连接，使用内部成员变量m_dbname，需要是否已经初始化
	*@return SQLITE_OK ：打开成功； 非0：错误代码
	*/
	int open();

	/**
	*@Description: 打开sqlite3 连接，使用参数传递数据库名称
	*@return SQLITE_OK ：打开成功； 非0：错误代码
	*/
	int open(string& dbname);

	/**
	*@Description：语句准备，初始化（重置）陈述句柄成员变量；使用成员变量m_pdb、m_sql，需要检查是否已经初始化
	*@return SQLITE_OK : 成功； 非0：错误代码
	*/
	int prepare();

	/**
	*@Descripton：语句准备，初始化（重置）陈述句柄成员变量
	*@param[IN] sql: 语句准备的sql
	*@param[IN] length: sql的长度
	*@return SQLITE_OK : 成功； 非0：错误代码
	*/
	int prepare(string& sql, int length);

	/**
	*@Description: 绑定字符串变量
	*@param[in] index : 需要绑定值的sql参数序号，从1开始
	*@param[in] value : 绑定的值
	*@param[in] length : value 占用的字节数，如果是负数，则取到结束符处
	*@param[in] dtfunc : value的析构函数，可为 SQLITE_TRANSIENT 或 SQLITE_STATIC
	*/
	int bindString(int index, const char* value, int length, void dtfunc(void*) );

	/**
	*@Description: 绑定double型变量
	*@param[in] index : 需要绑定值的sql参数序号，从1开始
	*@param[in] value : 绑定的值
	*/
	int bindDouble(int index, double value);

	/**
	*@Description: 绑定int型变量
	*@param[in] index : 需要绑定值的sql参数序号，从1开始
	*@param[in] value : 绑定的值
	*/
	int CDBSqlite::bindInt(int index, int value);

	/**
	*@Description:执行陈述句柄，需要检查陈述句柄是否有效；如果是有返还结果集的查询，重复执行依次获取结果集
	*@return 1:还有下一条记录，0：成功执行完毕， -1 ：执行错误
	*/
	int step();

	/**
	*@Description: 获取结果集中某一列的类型
	*@param[IN] index: 列的索引号，从0开始
	*@return 获取到的类型：SQLITE_INTEGER : 1; SQLITE_FLOAT : 2;SQLITE_TEXT : 3; SQLITE3_TEXT : 3; SQLITE_BLOB : 4; SQLITE_NULL : 5；
	*/
	int getColumnType(int index);

	/**
	*@Description: 获取结果集中某一列的列名称
	*@param[IN] index: 列的索引号，从0开始
	*@return 列名称
	*/
	string getColumnName(int index);

	/**
	*@Description：获取某一列（int类型）的值
	*@param[IN] index: 列的索引号，从0开始
	*@return: 获取到的值
	*/
	int getColumnInt(int index);

	/**
	*@Description：获取某一列（double类型）的值
	*@param[IN] index: 列的索引号，从0开始
	*@return: 获取到的值
	*/
	double getColumnDouble(int index);

	/**
	*@Description：获取某一列（char类型）的值，转换为string返回
	*@param[IN] index: 列的索引号，从0开始
	*@return: 获取到的值
	*/
	string getColumnString(int index);

	/**
	*@Description: 设置成员变量m_sql的值
	*@param[in] sql : sql语句
	*/
	void setSql(string sql)
	{
		m_sql = sql;
	}
	
	/**
	*@Description: 获取成员变量m_sql的值
	*@return 返回m_sql的值
	*/
	string getSql()
	{
		return m_sql;
	}
	
	//释放sqlite3_stmt* 指向的对象
	int finalize(){
		
		return sqlite3_finalize(m_pStmt);
	}

	int close(){
		return sqlite3_close(m_pdb);
	}

	int retCode; //保存上一次执行sqlite函数的返回值，为了让调用者知道错误码
	string errString; //保存错误描述字符串

protected:
	sqlite3* m_pdb; //保存sqlite3 的连接
	sqlite3_stmt* m_pStmt; //保存陈述句柄
	string m_dbname; //保存sqlite3数据库名称
	string m_sql; //保存执行sql
};





#endif // ASTMANAGER_CDBSQLITE_H