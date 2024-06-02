#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <memory>
#include <atomic>
#include <functional>
#include <condition_variable>

#include "connection.h"

using namespace std;

class ConnPool {
public:
	// 删除拷贝构造和拷贝赋值运算符
	ConnPool(const ConnPool&) = delete;
	ConnPool& operator=(const ConnPool&) = delete;
	// 获取连接池
	static ConnPool* getConnPool();
	// 获取一个链接
	shared_ptr<Connection> getConnection();
	// 查看连接池中连接数量
	int getConnCountInPool() const;
private:
	// 默认构造
	ConnPool();
	// 加载配置文件
	bool loadConfigFile();
	// 运行在独立的线程中，专门用于生成新连接
	void produceConntionTask();
	// 扫描任务
	void scannerConnectionTask();
private:
	string _mysql_ip;				// IP地址
	unsigned short _mysql_port;		// 端口号
	string _mysql_user;				// 用户名
	string _mysql_password;			// 密码
	string _mysql_db_name;			// 数据库名

	int _init_size;					// 出事连接量
	int _max_size;					// 最大连接量
	int _max_idle_time;				// 最大空闲时间
	int _connection_timeout;		// 获取连接的超时时间

	queue<Connection*> _connQ;		// 连接队列
	mutex _mutex;					// 维护连接队列线程安全
	atomic_int _conn_count;			// 连接池中的连接数量
	condition_variable _not_full;   // 用于提示生产者线程可以继续生产
	condition_variable _not_empty;  // 用于提示消费者线程可以消费连接
};