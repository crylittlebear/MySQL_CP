#pragma once
#include <chrono>

using namespace std;
using namespace std::chrono;

class Timer {
public:
	// 计时器构造
	Timer() {
		update();
	}
	// 更新时间
	void update() {
		m_timePoint = high_resolution_clock::now();
	}
	// 获取上次更新后经过的秒数
	double getElapsedSeconds() const {
		return getElapsedMicroseconds() * 0.000001;
	}
    // 获取上次更新后经过的毫秒数
	double getElapsedMiliseconds() const {
		return getElapsedMicroseconds() * 0.001;
	}
    // 获取上次更新后经过的微秒数
	long long getElapsedMicroseconds() const {
		return duration_cast<microseconds>(high_resolution_clock::now() - m_timePoint).count();
	}
private:
	time_point<high_resolution_clock> m_timePoint;
};