#include "Timer.h"
// use QueryPerformanceCounter & QueryPerformanceFrequency

void Timer::Reset() {
	//Set the current time:
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	startTime = t.QuadPart;
	currentCallToUpdate = t.QuadPart;
	lastCallToUpdate = t.QuadPart;
	//t4 = t3 = t2 = GetTickCount64();
}

void Timer::Update() {
	lastCallToUpdate = currentCallToUpdate;
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	currentCallToUpdate = t.QuadPart;
	//t3 = t2;
	//t2 = GetTickCount64();
}

float Timer::GetTimeTotal() {
	return (float)(currentCallToUpdate - startTime) / frequency;
}

float Timer::GetTimeDelta() {
	return (float)(currentCallToUpdate - lastCallToUpdate) / frequency;
}

/*
float Timer::GetTimeTotal() {
	return (t2 - t4) / 1000;
}

float Timer::GetTimeDelta() {
	return (t2 - t3) / 1000;
}
*/
long long int Timer::GetTickDelta() {
	//return t2 - t3;
	return currentCallToUpdate - lastCallToUpdate;
}