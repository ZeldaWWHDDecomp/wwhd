/* Calendar naming inferred from the twelve sequential month values and 1970/1/1 defaults. */
#include "agl/gx2/CalendarSupport.h"
using namespace calendar_support;
static CalendarMonth sJanuary;
static CalendarMonth sFebruary;
static CalendarMonth sMarch;
static CalendarMonth sApril;
static CalendarMonth sMay;
static CalendarMonth sJune;
static CalendarMonth sJuly;
static CalendarMonth sAugust;
static CalendarMonth sSeptember;
static CalendarMonth sOctober;
static CalendarMonth sNovember;
static CalendarMonth sDecember;
static CalendarYear sDefaultYear;
static CalendarMonth sDefaultMonth;
static CalendarDay sDefaultDay;
static CalendarHour sDefaultHour;
static CalendarMinute sDefaultMinute;
static CalendarSecond sDefaultSecond;
namespace calendar_support {
/* 027E9DE8 */
void CalendarYear::assign(u32 value) { mValue = value; }
/* 027E9DF0 */
void CalendarMonth::assign(u32 value) { mValue = value; }
/* 027E9E40 */
void CalendarDay::assign(u32 value) { mValue = value; }
/* 027E9E48 */
void CalendarHour::assign(u32 value) { mValue = value; }
/* 027E9E50 */
void CalendarMinute::assign(u32 value) { mValue = value; }
/* 027E9E58 */
void CalendarSecond::assign(u32 value) { mValue = value; }
}
/* 027E9DF8 */
CalendarMonth* createCalendarMonth(CalendarMonth* month,u32 value) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!month) month = static_cast<CalendarMonth*>(calendarAllocate(sizeof(CalendarMonth)));
    if (month) month->assign(value);
    return month;
}
/* 027E9E60 */
static void __sinit_CalendarSupport_cpp() {
    gabi::AutoFrame frame{gabi::current_function()};
    createCalendarMonth(&sJanuary,1);
    createCalendarMonth(&sFebruary,2);
    createCalendarMonth(&sMarch,3);
    createCalendarMonth(&sApril,4);
    createCalendarMonth(&sMay,5);
    createCalendarMonth(&sJune,6);
    createCalendarMonth(&sJuly,7);
    createCalendarMonth(&sAugust,8);
    createCalendarMonth(&sSeptember,9);
    createCalendarMonth(&sOctober,10);
    createCalendarMonth(&sNovember,11);
    createCalendarMonth(&sDecember,12);
    sDefaultYear.assign(1970);
    sDefaultMonth.mValue = sJanuary.mValue;
    sDefaultDay.assign(1);
    sDefaultHour.assign(0);
    sDefaultMinute.assign(0);
    sDefaultSecond.assign(0);
}
/* WWHD glue
data sJanuary 104A22AC
data sFebruary 104A22B0
data sMarch 104A22B4
data sApril 104A22B8
data sMay 104A22BC
data sJune 104A22C0
data sJuly 104A22C4
data sAugust 104A22C8
data sSeptember 104A22CC
data sOctober 104A22D0
data sNovember 104A22D4
data sDecember 104A22D8
data sDefaultYear 104A22DC
data sDefaultMonth 104A22E0
data sDefaultDay 104A22E4
data sDefaultHour 104A22E8
data sDefaultMinute 104A22EC
data sDefaultSecond 104A22F0
ext 0273AD10 void* calendarAllocate(u32)
*/
