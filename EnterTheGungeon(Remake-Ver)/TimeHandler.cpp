#include "TimeHandler.h"

void TimeHandler::init()
{
	_timer.init();

	initTextHandlers();
}

void TimeHandler::initTextHandlers()
{
	_textHandlers[0].init("����", "FPS: %I64u", { 10, 10 }, 20, WhiteRGB, true);
	_textHandlers[1].init("����", "WorldTime: %f", { 10, 30 }, 20, WhiteRGB, true);
	_textHandlers[2].init("����", "Stopwatch: %f", { 10, 50 }, 20, WhiteRGB, true);
}

void TimeHandler::update()
{
	_timer.update();
}

void TimeHandler::render(const HDC& hdc) const
{
	_textHandlers[0].render(hdc, getFrameRate());
	_textHandlers[1].render(hdc, getWorldTime());
	_textHandlers[2].render(hdc, getStopwatchTime());
}

bool TimeHandler::startAlarm(float fixedTime)
{
	stopStopwatch();
	startStopwatch();

	while (getStopwatchTime() <= fixedTime) {}
	
	stopStopwatch();

	return true;
}

