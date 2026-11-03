/* Port command dispatch, following CC0 tww JASCmdStack; HD uses JSU lists and a mutex. */
#include "JSystem/JAudio/JASCmdStack.h"
using namespace JASystem::Kernel;
namespace JASystem { namespace Kernel {
struct MutexView { u8 opaque; };
static MutexView commandMutex;
static JSUPtrList cmd_once;
static JSUPtrList cmd_stay;
static JSUListStaticBlock staticState;
static u8 staticRecord;
static u8 listOnceRecord;
static u8 listStayRecord;
static u8 mutexRecord;
/* 0280E8C0 */
void TPortCmd::destroy(s32 flags) {
    if(this) {
        portLinkDestroy(this,0);
        if(flags&1) portDelete(this);
    }
}
/* 0280E914 */
bool TPortCmd::setPortCmd(u32 func,TPortArgs* args) {
    if(mList) return false;
    portLock(&commandMutex);
    mFunc=func;mArgs=args;
    portUnlock(&commandMutex);
    return true;
}
/* 0280E9AC */
u8 TPortCmd::addPortCmdOnce() {
    portLock(&commandMutex);
    u8 result=portListAppend(&cmd_once,this);
    portUnlock(&commandMutex);
    return result;
}
/* 0280EA08 */
void portCmdProcOnce(JSUPtrList* head) {
    portLock(&commandMutex);
    JSUPtrLink* link=head->mHead;
    while(link) {
        TPortCmd* command=static_cast<TPortCmd*>(static_cast<void*>(link->mObject));
        JSUPtrLink* next=link->mNext;
        if(command) {
            u32 func=command->mFunc;
            if(func) gabi::call_ptr<void>(func,static_cast<TPortArgs*>(command->mArgs));
        }
        portListRemove(head,link);
        link=next;
    }
    portUnlock(&commandMutex);
}
/* 0280EAA4 */
void portCmdProcStay(JSUPtrList* head) {
    portLock(&commandMutex);
    JSUPtrLink* link=head->mHead;
    while(link) {
        TPortCmd* command=static_cast<TPortCmd*>(static_cast<void*>(link->mObject));
        if(command) {
            u32 func=command->mFunc;
            if(func) gabi::call_ptr<void>(func,static_cast<TPortArgs*>(command->mArgs));
        }
        link=link->mNext;
    }
    portUnlock(&commandMutex);
}
/* 0280EB24 */
void portCmdMain() { portCmdProcOnce(&cmd_once);portCmdProcStay(&cmd_stay); }
/* 0280EB58 */
static void __sinit_JASCmdStack_cpp() {
    staticState.mWords[2]=0;staticState.mWords[0]=0;
    staticState.mWords[3]=0;staticState.mWords[1]=0;
    portRegister(&staticRecord);
    portListInitiate(&cmd_once);portRegister(&listOnceRecord);
    portListInitiate(&cmd_stay);portRegister(&listStayRecord);
    portMutexConstruct(&commandMutex);portRegister(&mutexRecord);
}
}}
/* HD: compiler-generated list destructor wrapper. */
/* 0280EBE4 */
static void destroyPortCommandList(JSUPtrList* list,s32 flags) {
    if(list) {
        portListDestroy(list,0);
        if(flags&1) portDelete(list);
    }
}
/* WWHD glue
data commandMutex 104B5188
data cmd_once 104B517C
data cmd_stay 104B5170
data staticState 104B5160
data staticRecord 101F9D0C
data listOnceRecord 101F9D18
data listStayRecord 101F9D24
data mutexRecord 101F9D30
ext 027EC62C void portLinkDestroy(JSUPtrLink*,s32)
ext 0273AF40 void portDelete(void*)
ext 027604AC void portLock(void*)
ext 027604B4 void portUnlock(void*)
ext 027603F0 void portMutexConstruct(void*)
ext 028F026C void portRegister(void*)
ext 027EC6F4 u32 portListAppend(JSUPtrList*,JSUPtrLink*)
ext 027EC560 u32 portListRemove(JSUPtrList*,JSUPtrLink*)
ext 027EC678 void portListInitiate(JSUPtrList*)
ext 027EC68C void portListDestroy(JSUPtrList*,s32)
*/
