#include "xEzTime.h"
#include "xEzDebug.h"


bool EzTime::getDateToday(int& year, int& month, int& day)
{
	time_t nowTime = 0;
	nowTime = time(NULL);

	return getDateFromTime(nowTime, year, month, day);
}

bool EzTime::getDateFromTime(time_t tm, int& year, int& month, int& day)
{
	struct tm local;
	errno_t err = localtime_s(&local, &tm);
	if (!EzVerify(0 == err))
		return false;

	year = local.tm_year + 1900;
	month = local.tm_mon + 1;
	day = local.tm_mday;

	return true;
}

bool EzTime::getTimeCurrent(int& hour, int& min, int& sec)
{
	time_t nowTime = time(NULL);
	struct tm local;
	errno_t err = localtime_s(&local, &nowTime);
	if (!EzVerify(0 == err))
		return false;

	hour = local.tm_hour;
	min = local.tm_min;
	sec = local.tm_sec;

	return true;
}
