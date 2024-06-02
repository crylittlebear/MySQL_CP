#include <iostream>
#include <string>
#include <sstream>
#include "connection.h"
#include "connpool.h"
#include "timer.h"

int main() {
	//Connection conn;
	//if (conn.connect("127.0.0.1", 3306, "root", "root", "heng")) {
	//	std::cout << "连接MySQL成功\n";
	//}
	//else {
	//	std::cout << "连接MySQL失败\n";
	//}
	//char sql[1024] = { 0 };
	//sprintf(sql, "insert into user values(%d, '%s', %d, '%s');", 9, "张三", 21, "Male");
	//std::string sql(R"(update user set name = 'zhang san' where id = 1;)");
	//std::string sql(R"(delete from user where id != 1;)");
	//std::vector<std::string> strVec;
	//strVec.push_back(R"(insert into user values(2, 'li si', 18, 'Male'))");
	//strVec.push_back(R"(insert into user values(3, 'wang wu', 21, 'Male'))");
	//strVec.push_back(R"(insert into user values(4, 'zhao liu', 16, 'Male'))");
	//strVec.push_back(R"(insert into user values(5, 'xiao hong', 24, 'Female'))");
	//strVec.push_back(R"(insert into user values(6, 'xiao lan', 13, 'Female'))");
	//for (auto& str : strVec) {
	//	if (conn.update(str)) {
	//		std::cout << "更新数据库成功\n";
	//	}
	//	else {
	//		std::cout << "更新数据库失败\n";
	//	}
	//}

	Connection condel1;
	stringstream ss1;
	ss1 << "delete from user";
	condel1.connect("127.0.0.1", 3306, "heng", "root", "chat");
	if (condel1.update(ss1.str())) {
		cout << "清空user表成功\n";
	}

	Timer time1;
	for (int i = 0; i < 500; ++i) {
		Connection conn;
		stringstream ss;
		ss << "insert into user values(" << i << ", 'Royal', 30, 'Male')";
		//cout << ss.str() << endl;
		conn.connect("127.0.0.1", 3306, "heng", "root", "chat");
		conn.update(ss.str());
		//if (conn.update(ss.str())) {
		//	cout << "更新成功\n";
		//}
	}
	cout << "without conntionPool, 1000 inserts cost: " 
			<< time1.getElapsedMiliseconds() << "ms\n";
	Connection condel2;
	stringstream ss2;
	ss2 << "delete from user";
	condel2.connect("127.0.0.1", 3306, "heng", "root", "chat");
	if (condel2.update(ss2.str())) {
		cout << "清空user表成功\n";
	}

	// 使用连接池
	ConnPool* pool = ConnPool::getConnPool();
	Timer time2;
	for (int i = 0; i < 500; ++i) {
		shared_ptr<Connection> pcon = pool->getConnection();
		stringstream ss;
		ss << "insert into user values(" << i << ", 'Royal', 30, 'Male')";
		pcon->update(ss.str());
		//cout << ss.str() << endl;
		//if (pcon->update(ss.str())) {
		//	cout << "数据库更新成功\n";
		//}
		//else {
		//	cout << "数据库更新失败\n";
		//}
	}
	cout << "With connectionPool, 1000 inserts cost: " 
		<< time2.getElapsedMiliseconds() << "ms\n";
	Connection condel3;
	stringstream ss3;
	ss3 << "delete from user";
	condel3.connect("127.0.0.1", 3306, "heng", "root", "chat");
	if (condel3.update(ss3.str())) {
		cout << "清空user表成功\n";
	}
	//shared_ptr<Connection> conn1 = pool->getConnection();
	//bool res = conn1->update(std::string(R"(update user set name = 'lucy' where age = 24)"));
	//if (res) {
	//	cout << "更新数据成功\n";
	//}
	//else {
	//	cout << "更新数据失败\n";
	//}
}