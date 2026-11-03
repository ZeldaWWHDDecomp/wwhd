#pragma once
#include "wwhd.h"
namespace calendar_support {
struct CalendarYear { be<u32> mValue; void assign(u32); };
struct CalendarMonth { be<u32> mValue; void assign(u32); };
struct CalendarDay { be<u32> mValue; void assign(u32); };
struct CalendarHour { be<u32> mValue; void assign(u32); };
struct CalendarMinute { be<u32> mValue; void assign(u32); };
struct CalendarSecond { be<u32> mValue; void assign(u32); };
WWHD_SIZE(CalendarYear,4); WWHD_SIZE(CalendarMonth,4); WWHD_SIZE(CalendarDay,4);
WWHD_SIZE(CalendarHour,4); WWHD_SIZE(CalendarMinute,4); WWHD_SIZE(CalendarSecond,4);
}
calendar_support::CalendarMonth* createCalendarMonth(calendar_support::CalendarMonth*,u32);
void* calendarAllocate(u32);
