/* Process management, WWHD fields used by this unit. */
#pragma once
#include "bindings.h"
#include "gc_api/f_pc/f_pc.h"
struct ManagerWindow {
    u8 mViewport[0x28];
    be<s8> mCameraID;
    be<s8> mMode;
    u8 padding[2];
};
struct ManagerCameraInfo {
    gptr<void> mpCamera;
    u8 remaining[0x30];
};
/* HD: unit-local view of fields still opaque in the shared play layout.
 * Offsets are from dComIfGp_get(), including the save-info prefix. */
struct ManagerGameInfo {
    u8 _00[0x5AC9];
    be<u8> mDlstWindowNum;
    u8 _5ACA[2];
    ManagerWindow mDlstWindow[1];
    ManagerCameraInfo mCameraInfo[1];
    u8 _5B2C[0x5D04 - 0x5B2C];
    gptr<ManagerWindow> mCurrentWindow;
    gptr<void> mCurrentView;
    gptr<void> mCurrentViewport;
};
WWHD_SIZE(ManagerWindow, 0x2C);
WWHD_SIZE(ManagerCameraInfo, 0x34);
WWHD_OFFSET(ManagerGameInfo, mDlstWindow, 0x5ACC);
WWHD_OFFSET(ManagerGameInfo, mCameraInfo, 0x5AF8);
WWHD_OFFSET(ManagerGameInfo, mCurrentWindow, 0x5D04);
inline ManagerGameInfo* fpcM_gameInfo() {
    return reinterpret_cast<ManagerGameInfo*>(dComIfGp_get());
}
inline ManagerCameraInfo* fpcM_cameraInfo(ManagerGameInfo* info, s32 slot) {
    // HD: signed slot multiplication wraps in the original 32-bit guest address arithmetic.
    return gabi::at<ManagerCameraInfo>(gabi::ea(info->mCameraInfo) + u32(slot) * sizeof(ManagerCameraInfo));
}
struct layer_class { u8 storage[0x2C]; };
struct ManagerNodeList { gptr<void> mpHead; gptr<void> mpTail; be<s32> mSize; };
struct ManagerQueue { ManagerNodeList lists[10]; };
struct HDRenderObject;
struct ManagerHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    be<f32> firstPair[2];
    be<f32> secondPair[2];
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct ManagerConstants { be<f32> negativePi; be<f32> positivePi; be<f32> first; be<f32> second; };
WWHD_SIZE(ManagerHeaderStatics, 0x2C);
WWHD_OFFSET(ManagerHeaderStatics, firstObject, 0x18);
BOOL fpcM_Draw(void*);
BOOL fpcM_DrawIterater(u32);
BOOL fpcM_Execute(void*);
BOOL fpcM_Delete(void*);
void fpcM_Management(u32, u32);
void fpcM_Init();
base_process_class* fpcM_FastCreate(u32, u32, void*, void*);
s32 fpcM_IsPause(void*, u32);
void fpcM_PauseEnable(void*, u32);
void fpcM_PauseDisable(void*, u32);
void* fpcM_JudgeInLayer(u32, u32, void*);
BOOL fpcDw_Execute(base_process_class*);
layer_class* fpcLy_RootLayer();
BOOL fpcLyIt_OnlyHere(layer_class*, u32, void*);
BOOL fpcEx_Execute(base_process_class*);
BOOL fpcDt_Delete(base_process_class*);
void MtxInit();
void cAPIGph_Painter();
void fpcDt_Handler();
BOOL fpcPi_Handler();
BOOL fpcCt_Handler();
void fpcEx_Handler(u32);
void fpcDw_Handler(u32, u32);
void HDRenderObject_draw(HDRenderObject*);
void fpcLy_Create(layer_class*, void*, ManagerNodeList*, u32);
void fpcLn_Create();
base_process_class* fpcFCtRq_Request(layer_class*, u32, u32, void*, void*);
s32 fpcPause_IsEnable(base_process_class*, u32);
void fpcPause_Enable(base_process_class*, u32);
void fpcPause_Disable(base_process_class*, u32);
layer_class* fpcLy_Layer(u32);
void* fpcCtIt_JudgeInLayer(u32, u32, void*);
void* fpcLyIt_Judge(layer_class*, u32, void*);
void ManagerFirstHeaderObject_ct(u8*);
void ManagerSecondHeaderObject_ct(u8*);
inline void fpcM_callManagement(u32 callback) { gabi::call_ptr<void>(callback); }
