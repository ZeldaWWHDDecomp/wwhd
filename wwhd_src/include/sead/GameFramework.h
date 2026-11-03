/* WWHD sead game framework: the framework the game runs on (system managers' heaps, the system
 * tasks it creates below the root task, the start-up wait for the root task and the draw-context
 * lock), the singleton one of its system tasks creates, and the per-TU copies of its translation
 * unit. Names from the binary's strings ("sead::SystemManagers", "sead::ResourceMgr",
 * "sead::FileDeviceMgr") and the vtables; layout from this game's constructor (0x50 bytes). */
#pragma once
#include "sead/TaskMgr.h"
#include "sead/Framework.h"

namespace sead {

/* ---- local copy of a class owned by another unit (sead graphics) ---- */
struct Graphics_l {
    void lockDrawContext();
    void unlockDrawContext();
};

struct SystemSingleton;

/* what the base framework's system-task hook gets (only the words it sets are known) */
struct SystemTaskArg {
    be<u32> mWords[4];
};
/* what the game framework's system tasks are created with */
struct SystemTaskSettings {
    be<u32> mWatchdogFlags;                 // handed to the watchdog-task hook
    be<u32> mReserved04;
    be<u32> mClockSettings;                 // address handed to the last hook
};

/* the game framework's vtable: the framework's, plus the system-task hooks */
struct GameFrameworkVtable {
    VtableSlot mHeader[14];
    VtableSlot mCreateControllerTask;       // 0x70 (root task)
    VtableSlot mCreateWatchdogTask;         // 0x78 (root task, flags)
    VtableSlot mCreateSystemSingleton;      // 0x80 (root task) -> singleton
    VtableSlot mCreateMenuTask;             // 0x88 (root task)
    VtableSlot mCreateClockTask;            // 0x90 (root task, clock settings)
};

/* the framework of a game: adds the system tasks and the start-up handshake */
struct GameFramework : Framework {
    be<u32> mDisplayRunning;
    SafeText mTexts[3];
    be<u32> mReserved44;
    be<u32> mDrawContextLock;               // function (lock): locks the graphics draw context
    be<u32> mReserved4C;

    static void initialize(FrameworkInitArgs* arg);
    static GameFramework* construct(GameFramework* framework);
    void startDisplay();
    void createSystemTasks(TaskBase* rootTask, SystemTaskSettings* settings);
    void createControllerTask(TaskBase* rootTask);
    void createWatchdogTask(TaskBase* rootTask, u32 flags);
    SystemSingleton* createSystemSingleton(TaskBase* rootTask);
    void createMenuTask(TaskBase* rootTask);
    void createClockTask(TaskBase* rootTask, void* clockSettings);
    void waitStartDisplayLoop();
    void lockFrameDrawContext();
    void unlockFrameDrawContext();
    void processFrameEntry();
    void prepareRun(void* heap);
    void runLoop();

    TaskMgr* taskMgr() const { return static_cast<TaskMgr*>(mTaskMgr.get()); }
    GameFrameworkVtable* vtable() const { return reinterpret_cast<GameFrameworkVtable*>(mVtable.get()); }
};

/* a singleton that is only its disposer, created on the root task's primary heap */
struct SystemSingleton : Disposer {
    static SystemSingleton* createInstance(RegistryHeap* heap);
    void destroy(u32 flags);
};

/* per-TU copies of the game framework translation unit */
namespace game_framework_instances {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
}

}

WWHD_SIZE(sead::SystemTaskArg, 0x10);
WWHD_OFFSET(sead::SystemTaskSettings, mClockSettings, 8);
WWHD_OFFSET(sead::GameFrameworkVtable, mCreateControllerTask, 0x70);
WWHD_OFFSET(sead::GameFrameworkVtable, mCreateClockTask, 0x90);
WWHD_OFFSET(sead::GameFramework, mDisplayRunning, 0x28);
WWHD_OFFSET(sead::GameFramework, mTexts, 0x2C);
WWHD_OFFSET(sead::GameFramework, mReserved44, 0x44);
WWHD_OFFSET(sead::GameFramework, mDrawContextLock, 0x48);
WWHD_SIZE(sead::GameFramework, 0x50);
WWHD_SIZE(sead::SystemSingleton, 0x10);
