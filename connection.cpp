#include "connection.h"

Connection::Connection() {
	m_conn = mysql_init(nullptr);
	m_timer.update();
}

Connection::~Connection() {
	if (m_conn != nullptr)
		mysql_close(m_conn);
}

bool Connection::connect(string ip, 
						 unsigned short port,  
						 string user, 
						 string password, 
						 string dbName) {
	MYSQL* p = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), 
		password.c_str(),dbName.c_str(), port, nullptr, 0);
	return p != nullptr;
}

bool Connection::update(std::string sql) {
	if (mysql_query(m_conn, sql.c_str())) {
		LOG("更新失败: " + sql);
		return false;
	}
	return true;
}

MYSQL_RES* Connection::query(std::string sql) {
	if (mysql_query(m_conn, sql.c_str())) {
		LOG("查询失败: " + sql);
		return nullptr;
	}
	return mysql_use_result(m_conn);
}