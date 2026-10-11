/** Process management. */
#include "f_pc/f_pc_manager.h"
static HDRenderObject* l_renderObject;
static layer_class rootlayer;
static ManagerQueue queue;
static ManagerHeaderStatics l_headerStatics;
static const ManagerConstants l_constants;

/* 025DF904 */
BOOL fpcM_Draw(void* i_proc) {
    return fpcDw_Execute((base_process_class*)i_proc);
}

/* 025DF908 */
BOOL fpcM_DrawIterater(u32 i_drawIterFunc) {
    return fpcLyIt_OnlyHere(fpcLy_RootLayer(), i_drawIterFunc, NULL);
}

/* 025DF940 */
BOOL fpcM_Execute(void* i_proc) {
    return fpcEx_Execute((base_process_class*)i_proc);
}

/* 025DF944 */
BOOL fpcM_Delete(void* i_proc) {
    return fpcDt_Delete((base_process_class*)i_proc);
}

/* 025DF948 */
void fpcM_Management(u32 callBack1, u32 callBack2) {
    MtxInit();
    cAPIGph_Painter();
    // HD: no GameCube DVD-condition path. Restore the active window/camera before queues.
    // Keep every accessor call: the verified HD inlines fetch game info anew each time.
    if (fpcM_gameInfo()->mDlstWindowNum) {
        ManagerGameInfo* info = fpcM_gameInfo();
        s32 slot = info->mDlstWindow[0].mCameraID;
        ManagerWindow* current = &info->mDlstWindow[0];
        void* active = fpcM_cameraInfo(fpcM_gameInfo(), slot)->mpCamera;
        if (active) {
            fpcM_gameInfo()->mCurrentWindow = current;
            fpcM_gameInfo()->mCurrentView = active;
            fpcM_gameInfo()->mCurrentViewport = current;
        }
    }
    fpcDt_Handler();
    if (!fpcPi_Handler()) {
        JUTAssertion::showAssert("f_pc_manager.cpp", 0x256, "0");
    }
    if (!fpcCt_Handler()) {
        JUTAssertion::showAssert("f_pc_manager.cpp", 0x25A, "0");
    }
    if (callBack1) {
        fpcM_callManagement(callBack1);
    }
    fpcEx_Handler(gabi::guest_fn((const void*)fpcM_Execute));
    fpcDw_Handler(gabi::guest_fn((const void*)fpcM_DrawIterater), gabi::guest_fn((const void*)fpcM_Draw));
    if (callBack2) {
        fpcM_callManagement(callBack2);
    }
    // HD: draw the additional render object after the second callback.
    HDRenderObject* graphics = l_renderObject;
    if (graphics) {
        HDRenderObject_draw(graphics);
    }
}

/* 025DFA7C */
void fpcM_Init() {
    fpcLy_Create(&rootlayer, NULL, queue.lists, 10);
    fpcLn_Create();
}

/* 025DFAB8 */
base_process_class* fpcM_FastCreate(u32 i_procTypeID, u32 i_createReqFunc,
                                  void* i_createData, void* i_data) {
    // HD: retain the full-width profile argument (GameCube: s16).
    return fpcFCtRq_Request(fpcLy_CurrentLayer(), i_procTypeID, i_createReqFunc, i_createData, i_data);
}

/* 025DFB1C */
s32 fpcM_IsPause(void* i_proc, u32 i_flag) {
    // HD: full-width flags are passed through, with no GameCube u8 mask.
    return fpcPause_IsEnable((base_process_class*)i_proc, i_flag);
}

/* 025DFB20 */
void fpcM_PauseEnable(void* i_proc, u32 i_flag) {
    // HD: full-width flags are passed through.
    fpcPause_Enable((base_process_class*)i_proc, i_flag);
}

/* 025DFB24 */
void fpcM_PauseDisable(void* i_proc, u32 i_flag) {
    // HD: full-width flags are passed through.
    fpcPause_Disable((base_process_class*)i_proc, i_flag);
}

/* 025DFB28 */
void* fpcM_JudgeInLayer(u32 i_layerID, u32 i_judgeFunc, void* i_data) {
    layer_class* layer = fpcLy_Layer(i_layerID);
    if (!layer) {
        return NULL;
    }
    void* ret = fpcCtIt_JudgeInLayer(i_layerID, i_judgeFunc, i_data);
    if (!ret) {
        ret = fpcLyIt_Judge(layer, i_judgeFunc, i_data);
    }
    return ret;
}

/* 025DFBE8 */
static void __sinit_f_pc_manager_cpp() {
    // HD: header objects precede initialization of two additional constant pairs.
    for (int i = 0; i < 4; ++i) {
        l_headerStatics.zero[i] = 0;
    }
    __register_global_object(0x101F3C88);
    l_headerStatics.negativePi = l_constants.negativePi;
    l_headerStatics.positivePi = l_constants.positivePi;
    ManagerFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3C94);
    ManagerSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3CA0);
    f32 first = l_constants.first;
    f32 second = l_constants.second;
    l_headerStatics.firstPair[0] = first;
    l_headerStatics.secondPair[0] = second;
    l_headerStatics.firstPair[1] = first;
    l_headerStatics.secondPair[1] = second;
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 10058398 "f_pc_manager.cpp"
str 10058394 "0"
data l_renderObject 101F8344
data rootlayer 1048A9BC
data queue 1048A9E8
data l_headerStatics 1048A990
data l_constants 100583B4
ext 025DE2CC BOOL fpcDw_Execute(base_process_class*)
ext 025DEAA8 layer_class* fpcLy_RootLayer()
ext 025DED70 BOOL fpcLyIt_OnlyHere(layer_class*, u32, void*)
ext 025DE58C BOOL fpcEx_Execute(base_process_class*)
ext 025DE1B8 BOOL fpcDt_Delete(base_process_class*)
ext 0200FAC4 void MtxInit()
ext 025F2B08 void cAPIGph_Painter()
ext 025DE024 void fpcDt_Handler()
ext 025E0EE4 BOOL fpcPi_Handler()
ext 025DDCEC BOOL fpcCt_Handler()
ext 025DE788 void fpcEx_Handler(u32)
ext 025DE37C void fpcDw_Handler(u32, u32)
ext 02715310 void HDRenderObject_draw(HDRenderObject*)
ext 025DEBF0 void fpcLy_Create(layer_class*, void*, ManagerNodeList*, u32)
ext 025DF468 void fpcLn_Create()
ext 025DE884 base_process_class* fpcFCtRq_Request(layer_class*, u32, u32, void*, void*)
ext 025E0B20 s32 fpcPause_IsEnable(base_process_class*, u32)
ext 025E0B38 void fpcPause_Enable(base_process_class*, u32)
ext 025E0BA8 void fpcPause_Disable(base_process_class*, u32)
ext 025DEAC0 layer_class* fpcLy_Layer(u32)
ext 025DD780 void* fpcCtIt_JudgeInLayer(u32, u32, void*)
ext 025DEE20 void* fpcLyIt_Judge(layer_class*, u32, void*)
ext 028ED6F8 void ManagerFirstHeaderObject_ct(u8*)
ext 028EAB2C void ManagerSecondHeaderObject_ct(u8*)
*/
