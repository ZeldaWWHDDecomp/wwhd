// WWHD report listener registration and dispatch, reconstructed from local instructions.
#include "sead/ReportListeners.h"

static ReportList sReportListeners;
static ReportWriter* sDefaultReportWriter;
static u32 sReportListTable;
static u32 sReportListRegister;
void registerReportList(void*);

/* 0273ED58 */
static void registerReportListener(sead::EmbeddedObject* object) {
    if (object->mRegistered != 0) {
        sead::LinkList* owner = object->mOwner;
        if (owner != NULL) {
            object->mOwner = NULL;
            object->mRegistration.detach();
            owner->mCount = (u32)owner->mCount - 1;
        }
        object->mRegistered = 0;
    }
    sead::LinkList* owner = object->mOwner;
    if (owner != NULL) {
        object->mOwner = NULL;
        object->mRegistration.detach();
        owner->mCount = (u32)owner->mCount - 1;
    }
    object->mOwner = &sReportListeners;
    sReportListeners.insertBefore(&object->mRegistration);
    sReportListeners.mCount = (u32)sReportListeners.mCount + 1;
    object->mRegistered = 1;
}

/* 0273EE18 */
static void dispatchReport(const char* message) {
    sead::Link* node = sReportListeners.mNext;
    sead::Link* next = node->mNext;
    while (node != &sReportListeners) {
        sead::EmbeddedObject* object = reinterpret_cast<ReportLink*>(node)->mObject;
        ReportWriter* writer = static_cast<ReportWriter*>(object->mCallback.get());
        if (writer != NULL) gabi::call_ptr<void>(writer->mTable[3], writer, message);
        node = next;
        next = next->mNext;
    }
    ReportWriter* writer = sDefaultReportWriter;
    if (writer != NULL) gabi::call_ptr<void>(writer->mTable[3], writer, message);
}

/* 0273EEC0 */
static void initializeReportListeners() {
    sReportListeners.mTable = gabi::ea(&sReportListTable);
    sReportListeners.mCount = 0;
    sReportListeners.mNext = &sReportListeners;
    sReportListeners.mPrevious = &sReportListeners;
    registerReportList(&sReportListRegister);
}

/* WWHD glue
 data sReportListeners 1049FFB0
 data sDefaultReportWriter 101F8AF8
 data sReportListTable 10143620
 data sReportListRegister 101F8AEC
 ext 028F026C void registerReportList(void*)
*/
