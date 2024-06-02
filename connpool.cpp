#include <thread>
#include <chrono>

#include "connpool.h"

// 线程安全的懒汉单例模式
ConnPool* ConnPool::getConnPool() {
	static ConnPool pool;
	return &pool;
}

ConnPool::ConnPool() {
	// 加载配置文件
	if (loadConfigFile()) {
		cout << "成功读取配置文件\n";
		cout << "ip: " << _mysql_ip << endl;
		cout << "port: " << _mysql_port << endl;
		cout << "username: " << _mysql_user << endl;
		cout << "password: " << _mysql_password << endl;
		cout << "dbname: " << _mysql_db_name << endl;
		cout << "initsize: " << _init_size << endl;
		cout << "maxsize: " << _max_size << endl;
		cout << "maxidletime: " << _max_idle_time << endl;
		cout << "connectiontimeout: " << _connection_timeout << endl;
	}
	else {
		cout << "读取配置文件失败\n";
	}
	// 创建初始数量的连接
	for (int i = 0; i < _init_size; ++i) {
		Connection* pconn = new  Connection;
		bool flag = pconn->connect(_mysql_ip, _mysql_port, _mysql_user, 
			_mysql_password, _mysql_db_name);
		pconn->m_timer.update();
		_connQ.push(pconn);
		++_conn_count;
	}
	// 启动一个线程，作为连接的生产者
	thread producer(std::bind(&ConnPool::produceConntionTask, this));
	producer.detach();
	// 启动一个新线程，用于定时扫描队列中连接的空闲时间，超时连接进行回收
	thread scanner(std::bind(&ConnPool::scannerConnectionTask, this));
	scanner.detach();
}

void ConnPool::produceConntionTask() {
	while (true) {
		unique_lock<mutex> locker(_mutex);
		while (!_connQ.empty()) {
			// 如果队列不空，生产线程等待
			_not_empty.wait(locker);
		}
		if (_conn_count < _max_size) {
			Connection* pc = new Connection;
			pc->connect(_mysql_ip, _mysql_port, _mysql_user, _mysql_password, 
				_mysql_db_name);
			pc->m_timer.update();
			_connQ.push(pc);
			++_conn_count;
		}
		// 通知消费者线程可以消费连接了
		_not_empty.notify_all();
	}
}

void ConnPool::scannerConnectionTask() {
	while (true) {
		// 先睡眠一段时间
		this_thread::sleep_for(seconds(_max_idle_time));
		// 扫描队列删除长时间空闲线程
		unique_lock<mutex> locker(_mutex);
		while (_conn_count > _init_size) {
			Connection* pc = _connQ.front();
			if (pc->m_timer.getElapsedSeconds() > _max_idle_time) {
				_connQ.pop();
				--_conn_count;
				delete pc;
			}
			else {
				break;
			}
		}
	}
}

shared_ptr<Connection> ConnPool::getConnection() {
	// 涉及队列的操作，需要上锁
	unique_lock<mutex> locker(_mutex);
	while (_connQ.empty()) {
		if (cv_status::timeout == _not_empty.wait_for(locker, 
			std::chrono::milliseconds(_connection_timeout))) {
			if (_connQ.empty()) {
				LOG("获取空闲链接超时了...获取连接失败");
				return nullptr;
			}
		}
	}
	// shared_ptr智能指针析构时，会把连接释放掉，但是我们只想让其归还连接
	shared_ptr<Connection> sp(_connQ.front(), [&](Connection* pcon) {
		unique_lock<mutex> locker(_mutex);
		pcon->m_timer.update();
		_connQ.push(pcon);
		_not_empty.notify_all();
	});
	_connQ.pop();
	_not_empty.notify_all();
	return sp;
}

bool ConnPool::loadConfigFile() {
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr) {
		LOG("mysql.ini not exists");
		return false;
	}
	// 如果文件存在
	while (!feof(pf)) {
		char line[1024] = { 0 };
		fgets(line, sizeof(line), pf);
		string str = line;
		auto it = str.find('=');
		string key = str.substr(0, it);
		string value = str.substr(it + 1, str.size() - it - 2);
		if (key == "ip") {
			_mysql_ip = value;
		}
		else if (key == "port") {
			_mysql_port = stoi(value);
		}
		else if (key == "username") {
			_mysql_user = value;
		}
		else if (key == "password") {
			_mysql_password = value;
		}
		else if (key == "initsize") {
			_init_size = stoi(value);
		}
		else if (key == "maxsize") {
			_max_size = stoi(value);
		}
		else if (key == "maxidletime") {
			_max_idle_time = stoi(value);
		}
		else if (key == "connectiontimeout") {
			_connection_timeout = stoi(value);
		}
		else if (key == "dbname") {
			_mysql_db_name = value;
		}
	}
	return true;
}

int ConnPool::getConnCountInPool() const {
	return _conn_count;
}