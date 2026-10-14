/* HD title scene controller and its render delegate. Private reconstruction. */
#pragma once
#include "bindings.h"
#include "d/hd_state_task.h"
#include "d/hd_layout_mgr.h"
#include "d/hd_title_parts.h"
#include "hd/hd_render_resources.h"
struct hdTitleTask_l {
 u8 mTaskBase[0xC]; gptr<gfn> mVTable;
 SafeString mName; be<u32> mEnabled; be<u32> mReserved; HdStateDelegate mDelegate;
 u8 mTaskTail[0x10]; gptr<hdTitleLogo_c> mpScreen;
};
struct hdTitleScene_c {
 HdStateDisposer mDisposer;
 hdTitleTask_l mTask;
 void unloadLayout(); void createScreen(); void enter(); void calc(); void execute(); void draw(); void leave();
 void requestStart(); void requestFinish(); void render(void*);
 void* getPosition(); f32 getScaleX(); f32 getScaleY(); void* getCameraPosition(); f32 getCameraNear(); f32 getCameraFar();
 void* getLightPosition(); f32 getLightScaleX(); f32 getLightScaleY(); u32 getColor(); f32 getDepth(); u32 getMode(); u32 getFlags();
};
struct hdTitleStateObject_l { u8 mReserved[8]; gptr<gfn> mVTable; };
struct hdTitleState_l {
 be<u32> mId; gptr<void> mOwner; gptr<hdTitleStateObject_l> mpCurrent; gptr<void> mpNext;
};
struct hdTitleRenderContext_l { u8 mReserved[0xC]; be<u32> mLayerIndex; };
struct hdTitleGameState_l { u8 mReserved[0x1684]; be<u32> mSceneMode; };
WWHD_OFFSET(hdTitleScene_c,mTask,0x10);
WWHD_OFFSET(hdTitleTask_l,mDelegate,0x20);
WWHD_OFFSET(hdTitleTask_l,mpScreen,0x40);
WWHD_SIZE(hdTitleScene_c,0x54);
WWHD_OFFSET(hdTitleLogo_c,mAdvanceRequested,0x51);
WWHD_OFFSET(hdTitleGameState_l,mSceneMode,0x1684);

struct hdTitleSingleton_l { gptr<hdTitleScene_c> mpInstance; };
struct hdTitleDisposerSingleton_l { gptr<HdStateDisposer> mpInstance; };
struct hdTitleLayerSingleton_l { gptr<hdRenderLayerRegistry_l> mpInstance; };
struct hdTitleGameSingleton_l { gptr<hdTitleGameState_l> mpInstance; };
struct hdTitleManagerSingleton_l { gptr<void> mpInstance; };

struct hdSceneLayoutManagerSingleton_l { gptr<hdLayoutResMgr_c> mpInstance; };
