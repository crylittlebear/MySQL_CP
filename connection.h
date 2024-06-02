#pragma once

#include <mysql/mysql.h>
#include <string>
#include <chrono>
#include <iostream>

#include "public.h"
#include "timer.h"

using namespace std;
using namespace std::chrono;

class Connection {
public:
	// 初始化数据库链接
	Connection();
	// 释放数据库链接资源
	~Connection();
	// 连接数据库
	bool connect(string ip, 
				 unsigned short port, 
				 string user,
				 string password, 
				 string dbName);
	// 更新数据库 insert, update, delete
	bool update(string sql);
	// 查询操作 select
	MYSQL_RES* query(string sql);
public:
	// 记录加入队列时刻到当前的时间
	Timer m_timer;
private:
	// 表示和mysql的一条链接
	MYSQL* m_conn;
};