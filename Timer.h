#pragma once
// use QueryPerformanceCounter & QueryPerformanceFrequency

#include <Windows.h>
#include <sysinfoapi.h>

class Timer
{
public:
	long long startTime;
	long long lastCallToUpdate;
	long long currentCallToUpdate;
	long long frequency;
	//long long int t2, t3, t4;
	Timer() {
		//t4 = t2 = t3 = GetTickCount64();
		LARGE_INTEGER t;
		QueryPerformanceFrequency(&t);
		frequency = t.QuadPart;
		Reset();
	}
	void Reset();
	void Update();
	float GetTimeTotal();
	float GetTimeDelta();
	long long GetTickDelta();
};

