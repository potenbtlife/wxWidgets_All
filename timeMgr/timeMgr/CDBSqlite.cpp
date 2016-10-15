#include "CDBSqlite.h"

//缺省构造函数
CDBSqlite::CDBSqlite()
{
}

/**
*@Description: 构造函数，初始化dbname成员变量
*@param[IN] dbname：数据库名称
*/
CDBSqlite::CDBSqlite(string& dbname)
{
	m_dbname = dbname;
}


//构造函数
CDBSqlite::CDBSqlite(string& dbname, string& sql)
{
	m_dbname = dbname;
	m_sql = sql;
}

//析构函数
CDBSqlite::~CDBSqlite()
{
	sqlite3_finalize(m_pStmt);
	sqlite3_close(m_pdb);
}


//打开数据库连接
int CDBSqlite::open()
{
	if( m_dbname.empty() )
	{
		cerr<<"m_dbname isn't been initialised!"<<endl;
		return RET_FAIL;
	}

	int rt = sqlite3_open(m_dbname.c_str(), &m_pdb);
	if( rt != SQLITE_OK )
	{
		string tmp = sqlite3_errmsg(m_pdb);
		cout<<"Can't open database["<<m_dbname<<"],err_msg["<<sqlite3_errmsg(m_pdb)<<endl;
		sqlite3_close(m_pdb);
		return RET_FAIL;
	}
	
	return RET_OK;
}

//打开数据库连接
int CDBSqlite::open(string& dbname)
{
	int rt = sqlite3_open(dbname.c_str(), &m_pdb);
	if( rt != SQLITE_OK )
	{
		//cerr<<"Can't open database["<<dbname<<"],err_msg["<<sqlite3_errmsg(m_pdb)<<endl;
		errString=sqlite3_errstr(rt);
		sqlite3_close(m_pdb);
		return RET_FAIL;
	}

	return RET_OK;
}

//Description：语句准备，初始化（重置）陈述句柄成员变量；使用成员变量m_pdb、m_sql，需要检查是否已经初始化
int CDBSqlite::prepare()
{
	if( !m_pdb )
	{
		cerr<<"m_pdb isn't init!, call open frist!"<<endl;
		return RET_FAIL;
	}
	if( m_sql.empty() )
	{
		cerr<<"m_sql isn't initialise!"<<endl;
		return RET_FAIL;
	}

	const char *zLeftover;
	m_pStmt=0;
	retCode = sqlite3_prepare_v2(m_pdb, m_sql.c_str(), -1, &m_pStmt, &zLeftover);
	if( retCode != SQLITE_OK )
	{
		errString = sqlite3_errstr(retCode);
		return RET_FAIL;
	}

	return RET_OK;
}

//语句准备，初始化（重置）陈述句柄成员变量
int CDBSqlite::prepare(string& sql, int length)
{
	if( m_pdb )
	{
		cerr<<"m_pdb isn't init!, call open frist!"<<endl;
		return RET_FAIL;
	}
	if( sql=="" )
	{
		cerr<<"sql is null!"<<endl;
		return RET_FAIL;
	}

	retCode = sqlite3_prepare_v2(m_pdb, sql.c_str(), -1, &m_pStmt, NULL);
	if( retCode != SQLITE_OK )
	{
		errString=sqlite3_errstr(retCode);
		return RET_FAIL;
	}

	return RET_OK;
}

/**
*@Description: 绑定字符串变量
*@param[in] index : 需要绑定值的sql参数序号，从1开始
*@param[in] value : 绑定的值
*@param[in] length : value 占用的字节数，如果是负数，则取到结束符处
*@param[in] dtfunc : value的析构函数，可为 SQLITE_TRANSIENT 或 SQLITE_STATIC
*/
int CDBSqlite::bindString(int index, const char* value, int length, void dtfunc(void*) )
{
	int rt = sqlite3_bind_text(m_pStmt, index, value, length, dtfunc);
	if( rt != SQLITE_OK )
	{
		string errmsg = sqlite3_errstr(rt);
		cerr<<"bind failed! ["<<sqlite3_errstr(rt)<<"]"<<endl;
		return RET_FAIL;
	}
	
	return RET_OK;
}

/**
*@Description: 绑定double型变量
*@param[in] index : 需要绑定值的sql参数序号，从1开始
*@param[in] value : 绑定的值
*@param[in] length : value 占用的字节数，如果是负数，则取到结束符处
*@param[in] dtfunc : value的析构函数，可为 SQLITE_TRANSIENT 或 SQLITE_STATIC
*/
int CDBSqlite::bindDouble(int index, double value)
{
	int rt = sqlite3_bind_double(m_pStmt, index, value);
	if( rt != SQLITE_OK )
	{
		string errmsg = sqlite3_errstr(rt);
		cerr<<"bind failed! ["<<sqlite3_errstr(rt)<<"]"<<endl;
		return RET_FAIL;
	}
	
	return RET_OK;
}

/**
*@Description: 绑定int型变量
*@param[in] index : 需要绑定值的sql参数序号，从1开始
*@param[in] value : 绑定的值
*/
int CDBSqlite::bindInt(int index, int value)
{
	int rt = sqlite3_bind_int(m_pStmt, index, value);
	if( rt != SQLITE_OK )
	{
		string errmsg = sqlite3_errstr(rt);
		cerr<<"bind failed! ["<<sqlite3_errstr(rt)<<"]"<<endl;
		return RET_FAIL;
	}
	
	return RET_OK;
}


/**
*@Description:执行陈述句柄，需要检查陈述句柄是否有效；如果是有返还结果集的查询，重复执行依次获取结果集
*@return 1:还有下一条记录，0：成功执行完毕， -1 ：执行错误
*/
int CDBSqlite::step()
{
	if( !m_pStmt )
	{
		errString="m_pStmt isn't init, call prepare first!";
		return RET_FAIL;
	}
	retCode = sqlite3_step(m_pStmt);

	if ( retCode == SQLITE_ROW ) return 1;
	else if( retCode == SQLITE_DONE ) return 0;
	else if( retCode == SQLITE_BUSY)
	{
		errString="database is busy! can't benn written!";
		return RET_FAIL;
	}else if( retCode == SQLITE_MISUSE )
	{
		errString="uses an interface that is undefined or unsupported!";
		return RET_FAIL;
	}else
	{
		errString=sqlite3_errstr(retCode);
		return RET_FAIL;
	}
}

/**
*@Description：获取某一列（char类型）的值，转换为string返回
*@param[IN] index: 列的索引号，从0开始
*@return: 获取到的值
*/
string CDBSqlite::getColumnString(int index)
{
	const unsigned char* pTmpBuf = sqlite3_column_text(m_pStmt, index);
	char buf[2048];
	sprintf(buf,"%s\n",pTmpBuf);
	string colStr=buf;

	return colStr;
}

/**
*@Description：获取某一列（int类型）的值
*@param[IN] index: 列的索引号，从0开始
*@return: 获取到的值
*/
int CDBSqlite::getColumnInt(int index)
{
	return sqlite3_column_int(m_pStmt, index);
}

/**
*@Description：获取某一列（double类型）的值
*@param[IN] index: 列的索引号，从0开始
*@return: 获取到的值
*/
double CDBSqlite::getColumnDouble(int index)
{
	return sqlite3_column_double(m_pStmt, index);
}


/**
*@Description: 获取结果集中某一列的类型
*@param[IN] index: 列的索引号，从0开始
*@return 获取到的类型：SQLITE_INTEGER : 1; SQLITE_FLOAT : 2;SQLITE_TEXT : 3; SQLITE3_TEXT : 3; SQLITE_BLOB : 4; SQLITE_NULL : 5；
*/
int getColumnType(int index)
{
	return 0;
}

/**
*@Description: 获取结果集中某一列的列名称
*@param[IN] index: 列的索引号，从0开始
*@return 列名称
*/
string CDBSqlite::getColumnName(int index)
{
	char buf[256];
	const char* pValue = sqlite3_column_name(m_pStmt, index);
	sprintf(buf,"%s\n",pValue);
	
	return string(buf);
}