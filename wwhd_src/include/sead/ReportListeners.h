// WWHD report listener list layouts from local instructions.
#pragma once
#include "sead/FoundationObjects.h"
struct ReportWriter { gptr<be<u32>> mTable; };
struct ReportLink : sead::Link { gptr<sead::EmbeddedObject> mObject; };
struct ReportList : sead::LinkList { be<u32> mTable; };
WWHD_OFFSET(ReportWriter,mTable,0);
WWHD_SIZE(ReportWriter,4);
WWHD_OFFSET(ReportLink,mObject,8);
WWHD_SIZE(ReportLink,12);
WWHD_OFFSET(ReportList,mTable,12);
WWHD_SIZE(ReportList,16);
