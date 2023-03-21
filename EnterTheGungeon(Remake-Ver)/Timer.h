#pragma once

#include "pch.h"

class Timer
{
public:
	void			init();
	void			update();

	void			setFPS(float fps) { _lockFPS = fps; }
	void			setStopWatchUse(bool use) { _stopwatchUse = use; }
	int64			getFrameRate() const { return _frameRate; }
	float			getWorldTime() const { return _worldTime; }
	float			getDeltaTime() const { return _timeElapsed; }
	float			getTestTime() const { return _testTime; }
	uint64			getSeconds() const { return _seconds; }
	uint64			getMinutes() const { return _minutes; }
	uint64			getHours() const { return _hours; }

	void			initTestTime();
	
private:
	void			calculateCurrentTime();

private:
	uint64			_seconds;
	uint64			_minutes;
	uint64			_hours;
	int64			_currTime;			// ���� �ð� (���� Ÿ�̸� ��)
	int64			_lastTime;			// ���� �ð� (���� Ÿ�̸� ��)
	int64			_frequency;			// ���� Ÿ�̸��� ���ļ� (�ʴ� ���� ��), �������� ������ 0 ��ȯ
	int64			_frameRate;
	int64			_fpsFrameCount;
	float			_timeScale;			// ��� ���� ���� �ʴ� ���� ���� ����� ���� ���� �ð� ������ ��ȯ
	float			_timeElapsed;
	float			_fpsTimeElapsed;
	float			_worldTime;
	float			_testTime;
	float			_lockFPS;
	bool			_isHardware;		// ���� Ÿ�̸Ӹ� �� �� �ִ��� ����
	bool			_stopwatchUse;
};

